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
            pinb0 = 0, pinb1 = 1, pinb2 = 2, pinb3 = 3,
            pinb4 = 4, pinb5 = 5, pinb6 = 6, pinb7 = 7,
        ddrb = portb_base + 1,
            ddb0 = 0, ddb1 = 1, ddb2 = 2, ddb3 = 3, ddb4 = 4, ddb5 = 5, ddb6 = 6, ddb7 = 7,
        portb = portb_base + 2,
            portb0 = 0, portb1 = 1, pb2 = 2, pb3 = 3,
            portb4 = 4, pb5 = 5, pb6 = 6, pb7 = 7,
    portc_base = 0x26,
        pinc = portc_base,
            pinc0 = 0, pinc1 = 1, pinc2 = 2, pinc3 = 3,
            pinc4 = 4, pinc5 = 5, pinc6 = 6, pinc7 = 7,
        ddrc = portc_base + 1,
        portc = portc_base + 2,
    portd_base = 0x29,
        pind = portd_base,
        ddrd = portd_base + 1,
        portd = portd_base + 2,
    tifr0 = 0x35,
    tccr0a = 0x44, wgm00 = 0, wgm01 = 1, com0b0 = 4, com0b1 = 5, com0a0 = 6, com0a1 = 7,
    tccr0b = 0x45, cs00 = 0, cs01 = 1, cs02 = 2, wgm02 = 3, foc0b = 6, foc0a = 7,
    tcnt0 = 0x46,
    ocr0a = 0x47,
    ocr0b = 0x48,
    timsk0 = 0x6e, toie0 = 0, ocie0a = 1, ocie0b = 2,
    timsk1 = 0x6f, toie1 = 0, ocie1a = 1, ocie1b = 2, icie1 = 5,
    timsk2 = 0x70, toie2 = 0, ocie2a = 1, ocie2b = 2,
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
    icr1 = 0x86,
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
        ucpol0 = 0, ucsz00 = 1, ucpha0 = 1, ucsz01 = 2, udord0 = 2, usbs0 = 3,
        upm00 = 4, upm01 = 5, umsel00 = 6, umsel01 = 7,
    ubrr0 = 0xc4,
    ubrr0l = 0xc4,
    ubrr0h = 0xc5,
    udr0 = 0xc6,

    ocr1a_port_base = portb_base,
    ocr1a_ddr = ocr1a_port_base + 1,
    ocr1a_port = ocr1a_port_base + 2,
    pocr1a = portb1,
    ss_port_base = portb_base,
    ss_ddr = ss_port_base + 1,
    ss_port = ss_port_base + 2,
    pss = pb2,  //pin ss
    sck_port_base = portb_base,
    sck_ddr = sck_port_base + 1,
    sck_port = sck_port_base + 2,
    psck = pb5, //pin sck
    mosi_port_base = portb_base,
    mosi_ddr = mosi_port_base + 1,
    mosi_port = mosi_port_base + 2,
    pmosi = pb3,
    miso_port_base = portb_base,
    miso_ddr = miso_port_base + 1,
    miso_port = miso_port_base + 2,
    pmiso = portb4;

static volatile uint8_t
    * const p_pinb = (volatile uint8_t * const)pinb,
    * const p_ddrb = (volatile uint8_t * const)ddrb,
    * const p_portb = (volatile uint8_t * const)portb,
    * const p_pinc = (volatile uint8_t * const)pinc,
    * const p_ddrc = (volatile uint8_t * const)ddrc,
    * const p_tcnt0 = (volatile uint8_t * const)tcnt0,
    * const p_ocr0a = (volatile uint8_t * const)ocr0a,
    * const p_ocr0b = (volatile uint8_t * const)ocr0b,
    * const p_tccr0a = (volatile uint8_t * const)tccr0a,
    * const p_tccr0b = (volatile uint8_t * const)tccr0b,
    * const p_timsk0 = (volatile uint8_t * const)timsk0,
    * const p_timsk1 = (volatile uint8_t * const)timsk1,
    * const p_adcl = (volatile uint8_t * const)adcl,
    * const p_adch = (volatile uint8_t * const)adch,
    * const p_adcsra = (volatile uint8_t * const)adcsra,
    * const p_adcsrb = (volatile uint8_t * const)adcsrb,
    * const p_admux = (volatile uint8_t * const)admux,
    * const p_tccr1a = (volatile uint8_t * const)tccr1a,
    * const p_tccr1b = (volatile uint8_t * const)tccr1b,
    * const p_tccr1c = (volatile uint8_t * const)tccr1c,
    * const p_ocr1al = (volatile uint8_t * const)ocr1al,
    * const p_ocr1ah = (volatile uint8_t * const)ocr1ah,
    * const p_ucsr0a = (volatile uint8_t * const)ucsr0a,
    * const p_ucsr0b = (volatile uint8_t * const)ucsr0b,
    * const p_ucsr0c = (volatile uint8_t * const)ucsr0c,
    * const p_ubrr0l = (volatile uint8_t * const)ubrr0l,
    * const p_ubrr0h = (volatile uint8_t * const)ubrr0h,
    * const p_udr0 = (volatile uint8_t * const)udr0,

    * const p_ddr_ocr1a = (volatile uint8_t * const)ocr1a_ddr,
    * const p_ddr_ss = (volatile uint8_t * const)ss_ddr,
    * const p_port_ss = (volatile uint8_t * const)ss_port,
    * const p_ddr_sck = (volatile uint8_t * const)sck_ddr,
    * const p_port_sck = (volatile uint8_t * const)sck_port,
    * const p_ddr_mosi = (volatile uint8_t * const)mosi_ddr,
    * const p_port_mosi = (volatile uint8_t * const)mosi_port,
    * const p_ddr_miso = (volatile uint8_t * const)miso_ddr,
    * const p_port_miso = (volatile uint8_t * const)miso_port;

static volatile uint16_t
    * const p_tcnt1 = (volatile uint16_t * const)tcnt1,
    * const p_ocr1a = (volatile uint16_t * const)ocr1a,
    * const p_icr1 = (volatile uint16_t * const)icr1,
    * const p_ubrr0 = (volatile uint16_t * const)ubrr0;

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


