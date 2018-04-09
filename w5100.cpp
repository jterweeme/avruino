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
#include "uno.h"
#include <avr/pgmspace.h>

#define SPI_CLOCK_DIV4 0x00
#define SPI_CLOCK_DIV16 0x01
#define SPI_CLOCK_DIV64 0x02
#define SPI_CLOCK_DIV128 0x03
#define SPI_CLOCK_DIV2 0x04
#define SPI_CLOCK_DIV8 0x05
#define SPI_CLOCK_DIV32 0x06
//#define SPI_CLOCK_DIV64 0x07

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

  // SPI Configuration methods

  inline static void attachInterrupt();
  inline static void detachInterrupt(); // Default

  static void begin(); // Default
  static void end();

  static void setBitOrder(uint8_t);
  static void setDataMode(uint8_t);
  static void setClockDivider(uint8_t);
};

extern SPIClass SPI;

uint8_t SPIClass::transfer(uint8_t _data)
{
    SPDR = _data;
    while (!(SPSR & _BV(SPIF)));
    return SPDR;
}

void SPIClass::attachInterrupt() {
    SPCR |= _BV(SPIE);
}

void SPIClass::detachInterrupt() {
    SPCR &= ~_BV(SPIE);
}


SPIClass SPI;

void SPIClass::begin()
{   
    *p_port_ss |= 1<<pss;
    *p_ddr_ss |= 1<<pss;
    SPCR |= _BV(MSTR);
    SPCR |= _BV(SPE);

    *p_ddr_sck |= 1<<psck;
    *p_ddr_mosi |= 1<<pmosi;
}

void SPIClass::end() {
    SPCR &= ~_BV(SPE);
}

void SPIClass::setBitOrder(uint8_t bitOrder)
{
    if(bitOrder == 0) {
        SPCR |= 1<<DORD;
    } else {
        SPCR &= ~(_BV(DORD));
    }
}

void SPIClass::setDataMode(uint8_t mode)
{
  SPCR = (SPCR & ~SPI_MODE_MASK) | mode;
}

void SPIClass::setClockDivider(uint8_t rate)
{
  SPCR = (SPCR & ~SPI_CLOCK_MASK) | (rate & SPI_CLOCK_MASK);
  SPSR = (SPSR & ~SPI_2XCLOCK_MASK) | ((rate >> 2) & SPI_2XCLOCK_MASK);
}


W5100Class W5100;

#define TX_RX_MAX_BUF_SIZE 2048
#define TX_BUF 0x1100
#define RX_BUF (TX_BUF + TX_RX_MAX_BUF_SIZE)

#define TXBUF_BASE 0x4000
#define RXBUF_BASE 0x6000

void W5100Class::init(void)
{
    _delay_ms(300);

    SPI.begin();
    initSS();
  
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

void W5100Class::send_data_processing_offset(SOCKET s, uint16_t data_offset, const uint8_t *data, uint16_t len)
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
  uint16_t ptr;
  ptr = readSnRX_RD(s);
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
  uint16_t src_mask;
  uint16_t src_ptr;

  src_mask = (uint16_t)src & RMASK;
  src_ptr = RBASE[s] + src_mask;

  if( (src_mask + len) > RSIZE ) 
  {
    size = RSIZE - src_mask;
    read(src_ptr, (uint8_t *)dst, size);
    dst += size;
    read(RBASE[s], (uint8_t *) dst, len - size);
  } 
  else
    read(src_ptr, (uint8_t *) dst, len);
}


uint8_t W5100Class::write(uint16_t _addr, uint8_t _data)
{
  setSS();  
  SPI.transfer(0xF0);
  SPI.transfer(_addr >> 8);
  SPI.transfer(_addr & 0xFF);
  SPI.transfer(_data);
  resetSS();
  return 1;
}

uint16_t W5100Class::write(uint16_t _addr, const uint8_t *_buf, uint16_t _len)
{
  for (uint16_t i=0; i<_len; i++)
  {
    setSS();    
    SPI.transfer(0xF0);
    SPI.transfer(_addr >> 8);
    SPI.transfer(_addr & 0xFF);
    _addr++;
    SPI.transfer(_buf[i]);
    resetSS();
  }
  return _len;
}

uint8_t W5100Class::read(uint16_t _addr)
{
  setSS();  
  SPI.transfer(0x0F);
  SPI.transfer(_addr >> 8);
  SPI.transfer(_addr & 0xFF);
  uint8_t _data = SPI.transfer(0);
  resetSS();
  return _data;
}

uint16_t W5100Class::read(uint16_t _addr, uint8_t *_buf, uint16_t _len)
{
  for (uint16_t i=0; i<_len; i++)
  {
    setSS();
    SPI.transfer(0x0F);
    SPI.transfer(_addr >> 8);
    SPI.transfer(_addr & 0xFF);
    _addr++;
    _buf[i] = SPI.transfer(0);
    resetSS();
  }
  return _len;
}

void W5100Class::execCmdSn(SOCKET s, SockCMD _cmd) {
  // Send command to socket
  writeSnCR(s, _cmd);
  // Wait for command to complete
  while (readSnCR(s))
    ;
}
