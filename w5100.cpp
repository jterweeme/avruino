/*
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@bug.st>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>
#include "w5100.h"

#ifndef F_CPU
#define F_CPU 16000000
#endif

#include <util/delay.h>
#include "board.h"
#include <avr/pgmspace.h>

#define SPI_CLOCK_DIV4 0x00
#define SPI_CLOCK_DIV16 0x01
#define SPI_CLOCK_DIV64 0x02
#define SPI_CLOCK_DIV128 0x03
#define SPI_CLOCK_DIV2 0x04
#define SPI_CLOCK_DIV8 0x05
#define SPI_CLOCK_DIV32 0x06

#define SPI_MODE0 0x00
#define SPI_MODE1 0x04
#define SPI_MODE2 0x08
#define SPI_MODE3 0x0C

#define SPI_MODE_MASK 0x0C  // CPOL = bit 3, CPHA = bit 2 on SPCR
#define SPI_CLOCK_MASK 0x03  // SPR1 = bit 1, SPR0 = bit 0 on SPCR
#define SPI_2XCLOCK_MASK 0x01  // SPI2X = bit 0 on SPSR

class SPIClass {
public:
    inline static uint8_t transfer(uint8_t _data);
    static void begin(); // Default
    static void end() { *p_spcr &= ~(1<<spe); }
    static void setBitOrder(uint8_t);
    static void setDataMode(uint8_t);
    static void setClockDivider(uint8_t);
};

//extern SPIClass SPI;

uint8_t SPIClass::transfer(uint8_t _data)
{
    *p_spdr = _data;
    while (!(SPSR & _BV(SPIF)));
    return *p_spdr;
}

static constexpr uint8_t
    cs_port_base = pin10_base,
    cs_bit = pin10_bit,
    cs_ddr = cs_port_base + 1,
    cs_port = cs_port_base + 2;

static volatile uint8_t * const p_cs_ddr = (volatile uint8_t * const)cs_ddr;
static volatile uint8_t * const p_cs_port = (volatile uint8_t * const)cs_port;

SPIClass SPI;

void SPIClass::begin()
{   
    *p_cs_port |= 1<<cs_bit;
    *p_cs_ddr |= 1<<cs_bit;
    *p_ss_port |= 1<<ss_bit;
    *p_ss_ddr |= 1<<ss_bit;
    *p_spcr |= 1<<mstr | 1<<spe;
    *p_sck_ddr |= 1<<sck_bit;
    *p_mosi_ddr |= 1<<mosi_bit;
}

void SPIClass::setBitOrder(uint8_t bitOrder)
{
    if(bitOrder == 0)
        *p_spcr |= 1<<dord;
    else
        *p_spcr &= ~(1<<dord);
}

void SPIClass::setDataMode(uint8_t mode)
{
    *p_spcr = (SPCR & ~SPI_MODE_MASK) | mode;
}

void SPIClass::setClockDivider(uint8_t rate)
{
    *p_spcr = (SPCR & ~SPI_CLOCK_MASK) | (rate & SPI_CLOCK_MASK);
    *p_spsr = (SPSR & ~SPI_2XCLOCK_MASK) | ((rate >> 2) & SPI_2XCLOCK_MASK);
}

#define TX_RX_MAX_BUF_SIZE 2048
#define TX_BUF 0x1100
#define RX_BUF (TX_BUF + TX_RX_MAX_BUF_SIZE)

#define TXBUF_BASE 0x4000
#define RXBUF_BASE 0x6000

void W5100Class::init(void)
{
    _delay_ms(300);
    SPI.begin();
    *p_cs_ddr |= 1<<cs_bit;
    *p_ss_ddr |= 1<<ss_bit;
    writeMR(1<<RST);
    writeTMSR(0x55);
    writeRMSR(0x55);

    for (int i=0; i<MAX_SOCK_NUM; i++) {
        SBASE[i] = TXBUF_BASE + SSIZE * i;
        RBASE[i] = RXBUF_BASE + RSIZE * i;
    }
}

uint16_t W5100Class::getTXFreeSize(SOCKET s)
{
  uint16_t val=0, val1=0;
  do {
    val1 = readSnTX_FSR(s);
    if (val1 != 0)
      val = readSnTX_FSR(s);
  } 
  while (val != val1);
  return val;
}

uint16_t W5100Class::getRXReceivedSize(SOCKET s)
{
    uint16_t val=0,val1=0;
    do {
        val1 = readSnRX_RSR(s);
        if (val1 != 0)
            val = readSnRX_RSR(s);
    } 
    while (val != val1);
    return val;
}


void W5100Class::send_data_processing(SOCKET s, const uint8_t *data, uint16_t len)
{
    // This is same as having no offset in a call to send_data_processing_offset
    send_data_processing_offset(s, 0, data, len);
}

void W5100Class::send_data_processing_offset(SOCKET s, uint16_t data_offset,
    const uint8_t *data, uint16_t len)
{
  uint16_t ptr = readSnTX_WR(s);
  ptr += data_offset;
  uint16_t offset = ptr & SMASK;
  uint16_t dstAddr = offset + SBASE[s];

  if (offset + len > SSIZE) 
  {
    // Wrap around circular buffer
    uint16_t size = SSIZE - offset;
    write(dstAddr, data, size);
    write(SBASE[s], data + size, len - size);
  } 
  else {
    write(dstAddr, data, len);
  }

  ptr += len;
  writeSnTX_WR(s, ptr);
}


void W5100Class::recv_data_processing(SOCKET s, uint8_t *data, uint16_t len, uint8_t peek)
{
    uint16_t ptr = readSnRX_RD(s);
    read_data(s, (uint8_t *)ptr, data, len);

    if (!peek)
    {
        ptr += len;
        writeSnRX_RD(s, ptr);
    }
}

void W5100Class::read_data(SOCKET s, volatile uint8_t *src, volatile uint8_t *dst, uint16_t len)
{
    uint16_t size;
    uint16_t src_mask = (uint16_t)src & RMASK;
    uint16_t src_ptr = RBASE[s] + src_mask;

    if( (src_mask + len) > RSIZE ) 
    {
        size = RSIZE - src_mask;
        read(src_ptr, (uint8_t *)dst, size);
        dst += size;
        read(RBASE[s], (uint8_t *) dst, len - size);
    } 
    else
    {
        read(src_ptr, (uint8_t *) dst, len);
    }
}


uint8_t W5100Class::write(uint16_t _addr, uint8_t _data)
{
    *p_cs_port &= ~(1<<cs_bit);
    *p_ss_port &= ~(1<<ss_bit);
    SPI.transfer(0xF0);
    SPI.transfer(_addr >> 8);
    SPI.transfer(_addr & 0xFF);
    SPI.transfer(_data);
    *p_cs_port |= 1<<cs_bit;
    *p_ss_port |= 1<<ss_bit;
    return 1;
}

uint16_t W5100Class::write(uint16_t _addr, const uint8_t *_buf, uint16_t _len)
{
    for (uint16_t i=0; i<_len; i++)
    {
        *p_cs_port &= ~(1<<cs_bit);
        *p_ss_port &= ~(1<<ss_bit);
        SPI.transfer(0xF0);
        SPI.transfer(_addr >> 8);
        SPI.transfer(_addr & 0xFF);
        _addr++;
        SPI.transfer(_buf[i]);
        *p_cs_port |= 1<<cs_bit;
        *p_ss_port |= 1<<ss_bit;
    }
    return _len;
}

uint8_t W5100Class::read(uint16_t _addr)
{
    *p_cs_port &= ~(1<<cs_bit);
    *p_ss_port &= ~(1<<ss_bit);
    SPI.transfer(0x0F);
    SPI.transfer(_addr >> 8);
    SPI.transfer(_addr & 0xFF);
    uint8_t _data = SPI.transfer(0);
    *p_cs_port |= 1<<cs_bit;
    *p_ss_port |= 1<<ss_bit;
    return _data;
}

uint16_t W5100Class::read(uint16_t _addr, uint8_t *_buf, uint16_t _len)
{
    for (uint16_t i = 0; i < _len; i++)
    {
        *p_cs_port &= ~(1<<cs_bit);
        *p_ss_port &= ~(1<<ss_bit);
        SPI.transfer(0x0F);
        SPI.transfer(_addr >> 8);
        SPI.transfer(_addr & 0xFF);
        _addr++;
        _buf[i] = SPI.transfer(0);
        *p_cs_port |= 1<<cs_bit;
        *p_ss_port |= 1<<ss_bit;
    }
    return _len;
}

void W5100Class::execCmdSn(SOCKET s, SockCMD _cmd)
{
    writeSnCR(s, _cmd);     // Send command to socket
    while (readSnCR(s));    // Wait for command to complete
}



