#ifndef _TWAALF_H_
#define _TWAALF_H_
#include "misc.h"

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

static constexpr uint8_t
    porta_base = 0x20,
        pina = porta_base,
        ddra = porta_base + 1,
    portb_base = 0x23,
        pinb = portb_base,
        ddrb = portb_base + 1,
        portb = portb_base + 2,
            pb0 = 0, pb1 = 1, pb2 = 2, pb3 = 3, pb4 = 4, pb5 = 5, pb6 = 6, pb7 = 7,
    portc_base = 0x26,
        pinc = portc_base,
        ddrc = portc_base + 1,
    portd_base = 0x29,
        pind = portd_base,
    porte_base = 0x2c,
        pine = porte_base,
    tifr0 = 0x35, tov0 = 0, ocf0a = 1, ocf0b = 2,
    tifr1 = 0x36, tov1 = 0, ocf1a = 1, ocf1b = 2, ocf1c = 3, icf1 = 5,
    tifr2 = 0x37, tov2 = 0, ocf2a = 1, ocf2b = 2,
    tifr3 = 0x38, tov3 = 0, ocf3a = 1, ocf3b = 2, ocf3c = 3, icf3 = 5,
    pcifr = 0x3b,
    eifr = 0x3c,
    eimsk = 0x3d,
    gpior0 = 0x3e,
    tccr0a = 0x44, wgm00 = 0, wgm01 = 1, com0b0 = 4, com0b1 = 5, com0a0 = 6, com0a1 = 7,
    tcnt0 = 0x46,
    pllcsr = 0x49,
    gpior1 = 0x4a,
    gpior2 = 0x4b,
    spcr = 0x2c, spr0 = 0, spr1 = 1, cpha = 2, cpol = 3, mstr = 4, dord = 5, spe = 6, spie = 7,
    spsr = 0x2d, spi2x = 0, wcol = 6, spif = 7,
    spdr = 0x2e,
    timsk0 = 0x6e,
    timsk1 = 0x6f,
    timsk2 = 0x70, toie2 = 0, ocie2a = 1, ocie2b = 2,
    timsk3 = 0x71, toie3 = 0, ocie3a = 1, ocie3b = 2, ocie3c = 3, icie3 = 5,
    tccr3a = 0x90,
    tccr3b = 0x91,
    tccr3c = 0x92,
    uhcon = 0x9e,
    uhint = 0x9f,
    uhien = 0xa0,
    uhaddr = 0xa1,
    uhfnuml = 0xa2,
    uhfnumh = 0xa3,
    upstax = 0xac,
    upienx = 0xae,
    tccr2a = 0xb0,
    tccr2b = 0xb1,
    tcnt2 = 0xb2,
    ocr2a = 0xb3,
    ocr2b = 0xb4,
    assr = 0xb6,
    twbr = 0xb8,
    twar = 0xba,
    twdr = 0xbb,
    ucsr1a = 0xc8, mpcm1 = 0, u2x1 = 1, upe1 = 2, dor1 = 3, fe1 = 4, udre1 = 5, txc1 = 6, rxc1 = 7,
    ucsr1b = 0xc9,
        txb81 = 0, rxb81 = 1, ucsz12 = 2, txen1 = 3, rxen1 = 4, udrie1 = 5, txcie1 = 6, rxcie1 = 7,
    ucsr1c = 0xca,
    ubrr1 = 0xcc,
    ubrr1l = 0xcc,
    ubrr1h = 0xcd,
    udr1 = 0xce,
    uhwcon = 0xd7,
    udint = 0xe1,
    udien = 0xe2,
    udaddr = 0xe3,

    ss_port_base = portb_base,
    ss_ddr = ss_port_base + 1,
    ss_port = ss_port_base + 2,
    ss_bit = pb0,
    sck_port_base = portb_base,
    sck_ddr = sck_port_base + 1,
    sck_port = sck_port_base + 2,
    sck_bit = pb1,
    mosi_port_base = portb_base,
    mosi_ddr = mosi_port_base + 1,
    mosi_port = mosi_port_base + 2,
    mosi_bit = pb2,
    miso_port_base = portb_base,
    miso_ddr = miso_port_base + 1,
    miso_port = miso_port_base + 2,
    miso_bit = pb4,

    pin1_bit = pd2,
    pin2_bit = pd1,
    pin3_bit = pd0,
    pin4_bit = pb0
    pin5_bit = pb1,
    pin5_bit = pb2,
    pin6_bit = pb3,
    pin7_bit = pb4,
    pin8_bit = pb5,
    pin9_bit = pb6,
    pin10_bit = pd3,
    pin11_bit = pd4,
    pin12_bit = pd5,
    pin13_base = portb_base,
    pin13_ddr = pin13_base + 1,
    pin13_port = pin13_base + 2,
    pin13_bit = pb7,
    pinA0 = pd6,
    pinA1 = pc0,
    pinA2 = pc1,
    pinA3 = pc2,
    pinA4 = pc3;

static volatile uint8_t
    * const p_porta_base = (volatile uint8_t * const)porta_base,
    * const p_portb_base = (volatile uint8_t * const)portb_base,
    * const p_pinb = (volatile uint8_t * const)pinb,
    * const p_ddrb = (volatile uint8_t * const)ddrb,
    * const p_portc_base = (volatile uint8_t * const)portc_base,
    * const p_portd_base = (volatile uint8_t * const)portd_base,
    * const p_porte_base = (volatile uint8_t * const)porte_base,
    * const p_tifr0 = (volatile uint8_t * const)tifr0,
    * const p_tifr1 = (volatile uint8_t * const)tifr1,
    * const p_tifr2 = (volatile uint8_t * const)tifr2,
    * const p_tifr3 = (volatile uint8_t * const)tifr3,
    * const p_tcnt0 = (volatile uint8_t * const)tcnt0,
    * const p_gpior1 = (volatile uint8_t * const)gpior1,
    * const p_gpior2 = (volatile uint8_t * const)gpior2,
    * const p_spcr = (volatile uint8_t * const)spcr,
    * const p_spsr = (volatile uint8_t * const)spsr,
    * const p_spdr = (volatile uint8_t * const)spdr,
    * const p_timsk0 = (volatile uint8_t * const)timsk0,
    * const p_timsk1 = (volatile uint8_t * const)timsk1,
    * const p_timsk2 = (volatile uint8_t * const)timsk2,
    * const p_timsk3 = (volatile uint8_t * const)timsk3,
    * const p_twbr = (volatile uint8_t * const)twbr,
    * const p_twar = (volatile uint8_t * const)twar,
    * const p_twdr = (volatile uint8_t * const)twdr,
    * const p_ucsr1a = (volatile uint8_t * const)ucsr1a,
    * const p_ucsr1b = (volatile uint8_t * const)ucsr1b,
    * const p_ucsr1c = (volatile uint8_t * const)ucsr1c,
    * const p_udr1 = (volatile uint8_t * const)udr1,

    * const p_ss_ddr = (volatile uint8_t * const)ss_ddr,
    * const p_ss_port = (volatile uint8_t * const)ss_port,
    * const p_sck_ddr = (volatile uint8_t * const)sck_ddr,
    * const p_mosi_ddr = (volatile uint8_t * const)mosi_ddr,
    * const p_mosi_port = (volatile uint8_t * const)mosi_port,
    * const p_miso_ddr = (volatile uint8_t * const)miso_ddr,
    * const p_miso_port = (volatile uint8_t * const)miso_port,

    * const p_pin13_ddr = (volatile uint8_t * const)pin13_ddr,
    * const p_pin13_port = (volatile uint8_t * const)pin13_port;

static volatile uint16_t
    * const p_ubrr1 = (volatile uint16_t * const)ubrr1;

struct Board
{
    Port
        portB { p_portb_base },
        portC { p_portc_base },
        portD { p_portd_base };

    Pin
        pin9 {portB, BIT6};
        pin11 {portD, BIT4};
};

#endif


