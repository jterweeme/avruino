#ifndef _UNO_H_
#define _UNO_H_
#include "misc.h"

#define F_CPU 16000000UL

class SPIBus : public SPIBase
{
public:
    SPIBus();
};

static constexpr uint8_t
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
        mpcm0 = 0, u2x0 = 1, upe0 = 2, dor0 = 3, fe0 = 4, udre0 = 5, txc0 = 6, rxc0 = 7,
    ucsr0b = 0xc1,
        txb80 = 0, rxb80 = 1, ucxz02 = 2, txen0 = 3,
        rxen0 = 4, udrie0 = 5, txcie0 = 6, rxcie0 = 7,
    ucsr0c = 0xc2,
    ubrr0 = 0xc4,
    ubrr0l = 0xc4,
    ubrr0h = 0xc5,
    udr0 = 0xc6;

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
    *p_udr0 = (volatile uint8_t * const)udr0;

static volatile uint16_t
    *pocr0a = (volatile uint16_t * const)ocr0a,
    *ptcnt1 = (volatile uint16_t * const)tcnt1,
    *pocr1a = (volatile uint16_t * const)ocr1a,
    *pubrr0 = (volatile uint16_t * const)ubrr0;

struct Board
{
    Port
        portB { (uint8_t *)portb_base },
        portC { (uint8_t *)portc_base },
        portD { (uint8_t *)portd_base };

    Pin
        pin0 {portD, BIT0},
        pin1 {portD, BIT1},
        pin2 {portD, BIT2},
        pin3 {portD, BIT3},
        pin4 {portD, BIT4},
        pin5 {portD, BIT5},
        pin6 {portD, BIT6},
        pin7 {portD, BIT7},
        pin8 {portB, BIT0},
        pin9 {portB, BIT1},
        pin10 {portB, BIT2},
        pin11 {portB, BIT3},
        pin12 {portB, BIT4},
        pin13 {portB, BIT5},
        pinA0 {portC, BIT0},
        pinA1 {portC, BIT1},
        pinA2 {portC, BIT2},
        pinA3 {portC, BIT3},
        pinA4 {portC, BIT4},
        pinA5 {portC, BIT5};
};

#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )
#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 * 256))
#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)
#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
static const uint8_t FRACT_MAX = 1000 >> 3;
static volatile unsigned long timer0_overflow_count = 0;

unsigned long millis();

#endif


