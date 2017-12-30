#ifndef _MEGA_H_
#define _MEGA_H_
#include "misc.h"

#define F_CPU 16000000UL

class SPIBus : public SPIBase
{
public:
    SPIBus();
};

static constexpr uint8_t
    porta_base = 0x20,
        pina = porta_base,
        ddra = porta_base + 1,
        porta = porta_base + 2,
    portb_base = 0x23,
        pinb = portb_base,
        pinb0 = 0, pinb1 = 1, pinb2 = 2, pinb3 = 3, pinb4 = 4, pinb5 = 5, pinb6 = 6, pinb7 = 7,
        ddrb = portb_base + 1,
        ddb0 = 0, ddb1 = 1, ddb2 = 2, ddb3 = 3, ddb4 = 4, ddb5 = 5, ddb6 = 6, ddb7 = 7,
        portb = portb_base + 2,
        pb0 = 0, pb1 = 1, pb2 = 2, pb3 = 3, pb4 = 4, pb5 = 5, pb6 = 6, pb7 = 7,
    portc_base = 0x26,
        pinc = portc_base,
        pinc0 = 0, pinc1 = 1, pinc2 = 2, pinc3 = 3, pinc4 = 4, pinc5 = 5, pinc6 = 6, pinc7 = 7,
        ddrc = portc_base + 1,
    portd_base = 0x29,
        pind = portd_base,
        ddrd = portd_base + 1,
        portd = portd_base + 2,
    porte_base = 0x2c,
        pine = porte_base,
        ddre = porte_base + 1,
        porte = porte_base + 2,
    portf_base = 0x2f,
    portg_base = 0x32,
    tcnt0 = 0x26,
    ocr0a = 0x27,
    ocr0b = 0x28,
    timsk0 = 0x6e,
    timsk1 = 0x6f,
    timsk2 = 0x70,
    adcl = 0x78,
    adch = 0x79,
    adcsra = 0x7a,
        adps0 = 0, adps1 = 1, adps2 = 2, adie = 3, adif = 4, adate = 5, adsc = 6, aden = 7,
    adcsrb = 0x7b,
        adts0 = 0, adts1 = 1, adts2 = 2, acme = 6,
    admux = 0x7c, mux0 = 0, mux1 = 1, mux2 = 2, mux3 = 3, adlar = 5, refs0 = 6, refs1 = 7,
    tccr1a = 0x80, wgm10 = 0, wgm11 = 1, com1b0 = 4, com1b1 = 5, com1a0 = 6, com1a1 = 7,
    tccr1b = 0x81, cs10 = 0, cs11 = 1, cs12 = 2, wgm12 = 3, wgm13 = 4, ices1 = 6, icn1 = 7,
    tccr1c = 0x82, foc1b = 6, foc1a = 7,
    tcnt1 = 0x84,
    ocr1a = 0x88,
    ocr1al = 0x88,
    ocr1ah = 0x89,
    ocr1b = 0x8a,
    ocr1bl = 0x8a,
    ocr1bh = 0x8b,
    ucsr0a = 0xc0,
    ucsr0b = 0xc1,
        txb80 = 0, rxb80 = 1, ucxz02 = 2, txen0 = 3,
        rxen0 = 4, udrie0 = 5, txcie0 = 6, rxcie0 = 7,
    ucsr0c = 0xc2,
    ubrr0 = 0xc4,
    ubrr0l = 0xc4,
    ubrr0h = 0xc5,
    udr0 = 0xc6,
    ss_port_base = portb_base,
    ss_ddr = ss_port_base + 1,
    ss_port = ss_port_base + 2,
    pss = pb0,
    sck_port_base = portb_base,
    sck_ddr = sck_port_base + 1,
    sck_port = sck_port_base + 2,
    psck = pb1,
    mosi_port_base = portb_base,
    mosi_ddr = mosi_port_base + 1,
    mosi_port = mosi_port_base + 2,
    pmosi = pb2,
    miso_port_base = portb_base,
    miso_ddr = miso_port_base + 1,
    miso_port = miso_port_base + 2,
    pmiso = pb3;

static constexpr uint16_t
    porth_base = 0x100,
    portj_base = 0x103,
    portk_base = 0x106,
    portl_base = 0x109;

static volatile uint8_t
    *p_pinb = (volatile uint8_t * const)pinb,
    *p_ddrb = (volatile uint8_t * const)ddrb,
    *p_portb = (volatile uint8_t * const)portb,
    *p_pinc = (volatile uint8_t * const)pinc,
    *p_ddrc = (volatile uint8_t * const)ddrc,
    *p_tcnt0 = (volatile uint8_t * const)tcnt0,
    *p_ocr0b = (volatile uint8_t * const)ocr0b,
    *p_timsk0 = (volatile uint8_t * const)timsk0,
    *p_timsk1 = (volatile uint8_t * const)timsk1,
    *p_adcl = (volatile uint8_t * const)adcl,
    *p_adch = (volatile uint8_t * const)adch,
    *p_adcsra = (volatile uint8_t * const)adcsra,
    *p_adcsrb = (volatile uint8_t * const)adcsrb,
    *p_admux = (volatile uint8_t * const)admux,
    *p_tccr1a = (volatile uint8_t * const)tccr1a,
    *p_tccr1b = (volatile uint8_t * const)tccr1b,
    *p_tccr1c = (volatile uint8_t * const)tccr1c,
    *p_ocr1al = (volatile uint8_t * const)ocr1al,
    *p_ocr1ah = (volatile uint8_t * const)ocr1ah,
    *p_ucsr0a = (volatile uint8_t * const)ucsr0a,
    *p_ucsr0b = (volatile uint8_t * const)ucsr0b,
    *p_ucsr0c = (volatile uint8_t * const)ucsr0c,
    *p_ubrr0l = (volatile uint8_t * const)ubrr0l,
    *p_ubrr0h = (volatile uint8_t * const)ubrr0h,
    *p_udr0 = (volatile uint8_t * const)udr0,
    *p_ddr_ss = (volatile uint8_t * const)ss_ddr,
    *p_port_ss = (volatile uint8_t * const)ss_port,
    *p_ddr_sck = (volatile uint8_t * const)sck_ddr,
    *p_port_sck = (volatile uint8_t * const)sck_port,
    *p_ddr_mosi = (volatile uint8_t * const)mosi_ddr,
    *p_port_mosi = (volatile uint8_t * const)mosi_port,
    *p_ddr_miso = (volatile uint8_t * const)miso_ddr,
    *p_port_miso = (volatile uint8_t * const)miso_port;

struct Board
{
    Port portA { (uint8_t *)porta_base },
        portB { (uint8_t *)portb_base },
        portC { (uint8_t *)portc_base },
        portD { (uint8_t *)portd_base },
        portE { (uint8_t *)porte_base },
        portF { (uint8_t *)portf_base },
        portG { (uint8_t *)portg_base },
        portH { (uint8_t *)porth_base },
        portJ { (uint8_t *)portj_base },
        portK { (uint8_t *)portk_base },
        portL { (uint8_t *)portl_base };

    Pin pin0 {portE, BIT0},
        pin1 {portE, BIT1},
        pin2 {portE, BIT4},
        pin3 {portE, BIT5},
        pin4 {portG, BIT5},
        pin5 {portE, BIT3},
        pin6 {portH, BIT3},
        pin7 {portH, BIT4},
        pin8 {portH, BIT5},
        pin9 {portH, BIT6};
    Pin pin10 {portB, BIT4};
    Pin pin11 {portB, BIT5};
    Pin pin12 {portB, BIT6};
    Pin pin13 {portB, BIT7};
    Pin pin14 {portJ, BIT1};
    Pin pin15 {portJ, BIT0};
    Pin pin16 {portH, BIT1};
    Pin pin17 {portH, BIT0};
    Pin pin18 {portD, BIT3};
    Pin pin19 {portD, BIT2};
    Pin pin20 {portD, BIT1};
    Pin pin21 {portD, BIT0};
    Pin pin22 {portA, BIT0};
    Pin pin23 {portA, BIT1};
    Pin pin24 {portA, BIT2};
    Pin pin25 {portA, BIT3};
    Pin pin26 {portA, BIT4};
    Pin pin27 {portA, BIT5};
    Pin pin28 {portA, BIT6};
    Pin pin29 {portA, BIT7};
    Pin pin30 {portC, BIT7};
    Pin pin31 {portC, BIT6};
    Pin pin32 {portC, BIT5};
    Pin pin33 {portC, BIT4};
    Pin pin34 {portC, BIT3},
        pin35 {portC, BIT2},
        pin36 {portC, BIT1},
        pin37 {portC, BIT0},
        pin38 {portD, BIT7},
        pin39 {portG, BIT2},
        pin40 {portG, BIT1},
        pin41 {portG, BIT0},
        pin42 {portL, BIT7},
        pin43 {portL, BIT6},
        pin44 {portL, BIT5},
        pin45 {portL, BIT4},
        pin46 {portL, BIT3},
        pin47 {portL, BIT2},
        pin48 {portL, BIT1},
        pin49 {portL, BIT0},
        pin50 {portB, BIT3},
        pin51 {portB, BIT2},
        pin52 {portB, BIT1},
        pin53 {portB, BIT0},
        pinA0 {portF, BIT0},
        pinA1 {portF, BIT1},
        pinA2 {portF, BIT2},
        pinA3 {portF, BIT3},
        pinA4 {portF, BIT4},
        pinA5 {portF, BIT5},
        pinA6 {portF, BIT6},
        pinA7 {portF, BIT7},
        pinA8 {portK, BIT0},
        pinA9 {portK, BIT1},
        pinA10 {portK, BIT2},
        pinA11 {portK, BIT3},
        pinA12 {portK, BIT4},
        pinA13 {portK, BIT5},
        pinA14 {portK, BIT6},
        pinA15 {portK, BIT7},
        pinSS {portB, BIT0},
        pinSCK {portB, BIT1},
        pinMOSI {portB, BIT2},
        pinMISO {portB, BIT3};
};
#endif


