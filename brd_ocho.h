/*
ATMega8 in Uno board

1/PC6/RESET
2/PD0/RXD           D0
3/PD1/TXD           D1
4/PD2/INT0          D2
5/PD3/INT1          D3
6/PD4/XCK/T0        D4
7/VCC
8/GND
9/PB6/XTAL1/TOSC1
10/PB7/XTAL2/TOSC2
11/PD5/T1           D5
12/PD6/AIN0         D6
13/PD7/AIN1         D7
14/PB0/ICP1         D8
15/PB1/OC1A         D9
16/PB2/SS/OC1B      D10
17/PB3/MOSI/OC2     D11
18/PB4/MISO         D12
19/PB5/SCK          D13
20/AVCC
21/AREF
22/GND
23/PC0/ADC0         A0
24/PC1/ADC1         A1
25/PC2/ADC2         A2
26/PC3/ADC3         A3
27/PC4/ADC4/SDA     A4
28/PC5/ADC5/SCL     A5

*/

#ifndef _OCHO_H_
#define _OCHO_H_

#include "mcu_m8.h"
#include "pinport.h"

static constexpr uint8_t
    ucsr9a = ucsra,
        mpcm9 = 0, u2x9 = 1, upe9 = 2, dor9 = 3, fe9 = 4, udre9 = 5, txc9 = 6, rxc9 = 7,
    ucsr9b = ucsrb,
        txb89 = txb8, rxb89 = rxb8, ucxz92 = ucsz2, txen9 = txen,
        rxen9 = rxen, udrie9 = udre, txcie9 = txcie, rxcie9 = rxcie,
    ubrr9 = ubrrl,
    ubrr9l = ubrrl,
    ubrr9h = ubrrl,
    udr9 = udr,

    ocr0a_base = portd_base,
    ocr0a_bit = pd6,
    ocr0b_base = portd_base,
    ocr0b_ddr = ocr0b_base + 1,
    ocr0b_bit = pd5,
    ocr1a_base = portb_base,
    ocr1a_ddr = ocr1a_base + 1,
    ocr1a_port = ocr1a_base + 2,
    ocr1a_bit = pb1,
    ocr1b_base = portb_base,
    ocr1b_ddr = ocr1b_base + 1,
    ocr1b_port = ocr1b_base + 2,
    ocr1b_bit = pb2,
    ocr2b_base = portd_base,
    ocr2b_ddr = ocr2b_base + 1,
    ocr2b_port = ocr2b_base + 2,
    ocr2b_bit = pd3,

    int0_port_base = portd_base,
    int0_ddr = int0_port_base + 1,
    int0_port = int0_port_base + 2,
    int0_bit = pd2,

    ss_port_base = portb_base,
    ss_ddr = ss_port_base + 1,
    ss_port = ss_port_base + 2,
    ss_bit = pb2,
    sck_port_base = portb_base,
    sck_ddr = sck_port_base + 1,
    sck_port = sck_port_base + 2,
    sck_bit = pb5,
    mosi_port_base = portb_base,
    mosi_ddr = mosi_port_base + 1,
    mosi_port = mosi_port_base + 2,
    mosi_bit = pb3,
    miso_port_base = portb_base,
    miso_ddr = miso_port_base + 1,
    miso_port = miso_port_base + 2,
    miso_bit = pb4,

    pin0_base = portd_base,
    pin0_in = pin0_base + 0,
    pin0_ddr = pin0_base + 1,
    pin0_port = pin0_base + 2,
    pin0_bit = pd0,
    pin1_base = portd_base,
    pin1_in = pin1_base + 0,
    pin1_ddr = pin1_base + 1,
    pin1_port = pin1_base + 2,
    pin1_bit = pd1,
    pin2_base = portd_base,
    pin2_in = pin2_base + 0,
    pin2_ddr = pin2_base + 1,
    pin2_port = pin2_base + 2,
    pin2_bit = pd2,
    pin3_base = portd_base,
    pin3_in = pin3_base + 0,
    pin3_ddr = pin3_base + 1,
    pin3_port = pin3_base + 2,
    pin3_bit = pd3,
    pin4_base = portd_base,
    pin4_in = pin4_base + 0,
    pin4_ddr = pin4_base + 1,
    pin4_port = pin4_base + 2,
    pin4_bit = pd4,
    pin5_base = portd_base,
    pin5_in = pin5_base + 0,
    pin5_ddr = pin5_base + 1,
    pin5_port = pin5_base + 2,
    pin5_bit = pd5,
    pin6_base = portd_base,
    pin6_in = pin6_base + 0,
    pin6_ddr = pin6_base + 1,
    pin6_port = pin6_base + 2,
    pin6_bit = pd6,
    pin7_base = portd_base,
    pin7_in = pin7_base + 0,
    pin7_ddr = pin7_base + 1,
    pin7_port = pin7_base + 2,
    pin7_bit = pd7,
    pin8_base = portb_base,
    pin8_in = pin8_base + 0,
    pin8_ddr = pin8_base + 1,
    pin8_port = pin8_base + 2,
    pin8_bit = pb0,
    pin9_base = portb_base,
    pin9_in = pin9_base + 0,
    pin9_ddr = pin9_base + 1,
    pin9_port = pin9_base + 2,
    pin9_bit = pb1,
    pin10_base = portb_base,
    pin10_in = pin10_base + 0,
    pin10_ddr = pin10_base + 1,
    pin10_port = pin10_base + 2,
    pin10_bit = pb2,
    pin11_base = portb_base,
    pin11_in = pin11_base + 0,
    pin11_ddr = pin11_base + 1,
    pin11_port = pin11_base + 2,
    pin11_bit = pb3,
    pin12_base = portb_base,
    pin12_in = pin12_base + 0,
    pin12_ddr = pin12_base + 1,
    pin12_port = pin12_base + 2,
    pin12_bit = pb4,
    pin13_base = portb_base,
    pin13_in = pin13_base + 0,
    pin13_ddr = pin13_base + 1,
    pin13_port = pin13_base + 2,
    pin13_bit = pb5,
    pinA0_base = portc_base,
    pinA0_in = pinA0_base + 0,
    pinA0_ddr = pinA0_base + 1,
    pinA0_port = pinA0_base + 2,
    pinA0_bit = pc0,
    pinA1_base = portc_base,
    pinA1_in = pinA1_base + 0,
    pinA1_ddr = pinA1_base + 1,
    pinA1_port = pinA1_base + 2,
    pinA1_bit = pc1,
    pinA2_base = portc_base,
    pinA2_in = pinA2_base + 0,
    pinA2_ddr = pinA2_base + 1,
    pinA2_port = pinA2_base + 2,
    pinA2_bit = pc2,
    pinA3_base = portc_base,
    pinA3_in = pinA3_base + 0,
    pinA3_ddr = pinA3_base + 1,
    pinA3_port = pinA3_base + 2,
    pinA3_bit = pc3,
    pinA4_base = portc_base,
    pinA4_in = pinA4_base + 0,
    pinA4_ddr = pinA4_base + 1,
    pinA4_port = pinA4_base + 2,
    pinA4_bit = pc4,
    pinA5_base = portc_base,
    pinA5_bit = pc5;

static volatile uint8_t
    * const p_portb_base = (volatile uint8_t * const)portb_base,
    * const p_pinb = (volatile uint8_t * const)pinb,
    * const p_ddrb = (volatile uint8_t * const)ddrb,
    * const p_portb = (volatile uint8_t * const)portb,
    * const p_portc_base = (volatile uint8_t * const)portc_base,
    * const p_pinc = (volatile uint8_t * const)pinc,
    * const p_ddrc = (volatile uint8_t * const)ddrc,
    * const p_portc = (volatile uint8_t * const)portc,
    * const p_portd_base = (volatile uint8_t * const)portd_base,
    * const p_pind = (volatile uint8_t * const)pind,
    * const p_ddrd = (volatile uint8_t * const)ddrd,
    * const p_portd = (volatile uint8_t * const)portd,
    * const p_tifr = (volatile uint8_t * const)tifr,
    //* const p_eimsk = (volatile uint8_t * const)eimsk,
    * const p_tcnt0 = (volatile uint8_t * const)tcnt0,
    * const p_ocr0a = (volatile uint8_t * const)ocr0a,
    * const p_ocr0b = (volatile uint8_t * const)ocr0b,
    * const p_tccr0a = (volatile uint8_t * const)tccr0a,
    * const p_tccr0b = (volatile uint8_t * const)tccr0b,
    * const p_spcr = (volatile uint8_t * const)spcr,
    * const p_spsr = (volatile uint8_t * const)spsr,
    * const p_spdr = (volatile uint8_t * const)spdr,
    //* const p_eicra = (volatile uint8_t * const)eicra,
    * const p_timsk = (volatile uint8_t * const)timsk,
    * const p_adcl = (volatile uint8_t * const)adcl,
    * const p_adch = (volatile uint8_t * const)adch,
    * const p_adcsra = (volatile uint8_t * const)adcsra,
    //* const p_adcsrb = (volatile uint8_t * const)adcsrb,
    * const p_admux = (volatile uint8_t * const)admux,
    * const p_tccr1a = (volatile uint8_t * const)tccr1a,
    * const p_tccr1b = (volatile uint8_t * const)tccr1b,
    * const p_ocr1al = (volatile uint8_t * const)ocr1al,
    * const p_ocr1ah = (volatile uint8_t * const)ocr1ah,
    * const p_tccr2a = (volatile uint8_t * const)tccr2a,
    * const p_tccr2b = (volatile uint8_t * const)tccr2b,
    * const p_ocr2a = (volatile uint8_t * const)ocr2a,
    * const p_ocr2b = (volatile uint8_t * const)ocr2b,
#if 0
    * const p_ucsr0a = (volatile uint8_t * const)ucsr0a,
    * const p_ucsr0b = (volatile uint8_t * const)ucsr0b,
    * const p_ucsr0c = (volatile uint8_t * const)ucsr0c,
    * const p_ubrr0l = (volatile uint8_t * const)ubrr0l,
    * const p_ubrr0h = (volatile uint8_t * const)ubrr0h,
    * const p_udr0 = (volatile uint8_t * const)udr0,
#endif
    * const p_ucsr9a = (volatile uint8_t * const)ucsr9a,
    * const p_ucsr9b = (volatile uint8_t * const)ucsr9b,
    * const p_ubrr9l = (volatile uint8_t * const)ubrr9l,
    * const p_ubrr9h = (volatile uint8_t * const)ubrr9h,
    * const p_udr9 = (volatile uint8_t * const)udr9,

    * const p_ocr0b_ddr = (volatile uint8_t * const)ocr0b_ddr,
    * const p_ocr1a_ddr = (volatile uint8_t * const)ocr1a_ddr,
    * const p_ocr1b_ddr = (volatile uint8_t * const)ocr1b_ddr,
    * const p_ocr2b_ddr = (volatile uint8_t * const)ocr2b_ddr,

    * const p_int0_ddr = (volatile uint8_t * const)int0_ddr,
    * const p_int0_port = (volatile uint8_t * const)int0_port,

    * const p_ss_ddr = (volatile uint8_t * const)ss_ddr,
    * const p_ss_port = (volatile uint8_t * const)ss_port,
    * const p_sck_ddr = (volatile uint8_t * const)sck_ddr,
    * const p_sck_port = (volatile uint8_t * const)sck_port,
    * const p_port_sck = (volatile uint8_t * const)sck_port,
    * const p_mosi_ddr = (volatile uint8_t * const)mosi_ddr,
    * const p_mosi_port = (volatile uint8_t * const)mosi_port,
    * const p_miso_ddr = (volatile uint8_t * const)miso_ddr,
    * const p_miso_port = (volatile uint8_t * const)miso_port,

    * const p_pin0_base = (volatile uint8_t * const)pin0_base,
    * const p_pin0_ddr = (volatile uint8_t * const)pin0_ddr,
    * const p_pin0_port = (volatile uint8_t * const)pin0_port,
    * const p_pin1_base = (volatile uint8_t * const)pin1_base,
    * const p_pin1_ddr = (volatile uint8_t * const)pin1_ddr,
    * const p_pin1_port = (volatile uint8_t * const)pin1_port,
    * const p_pin2_base = (volatile uint8_t * const)pin2_base,
    * const p_pin2_ddr = (volatile uint8_t * const)pin2_ddr,
    * const p_pin2_port = (volatile uint8_t * const)pin2_port,
    * const p_pin3_base = (volatile uint8_t * const)pin3_base,
    * const p_pin3_ddr = (volatile uint8_t * const)pin3_ddr,
    * const p_pin3_port = (volatile uint8_t * const)pin3_port,
    * const p_pin4_base = (volatile uint8_t * const)pin4_base,
    * const p_pin4_ddr = (volatile uint8_t * const)pin4_ddr,
    * const p_pin5_base = (volatile uint8_t * const)pin5_base,
    * const p_pin5_ddr = (volatile uint8_t * const)pin5_ddr,
    * const p_pin6_base = (volatile uint8_t * const)pin6_base,
    * const p_pin6_in = (volatile uint8_t * const)p_pin6_in,
    * const p_pin6_ddr = (volatile uint8_t * const)pin6_ddr,
    * const p_pin6_port = (volatile uint8_t * const)pin6_port,
    * const p_pin7_base = (volatile uint8_t * const)pin7_base,
    * const p_pin7_ddr = (volatile uint8_t * const)pin7_ddr,
    * const p_pin8_base = (volatile uint8_t * const)pin8_base,
    * const p_pin8_ddr = (volatile uint8_t * const)pin8_ddr,
    * const p_pin9_base = (volatile uint8_t * const)pin9_base,
    * const p_pin9_ddr = (volatile uint8_t * const)pin9_ddr,
    * const p_pin10_base = (volatile uint8_t * const)pin10_base,
    * const p_pin10_ddr = (volatile uint8_t * const)pin10_ddr,
    * const p_pin11_base = (volatile uint8_t * const)pin11_base,
    * const p_pin11_ddr = (volatile uint8_t * const)pin11_ddr,
    * const p_pin12_base = (volatile uint8_t * const)pin12_base,
    * const p_pin12_ddr = (volatile uint8_t * const)pin12_ddr,
    * const p_pin13_base = (volatile uint8_t * const)pin13_base,
    * const p_pin13_ddr = (volatile uint8_t * const)pin13_ddr,
    * const p_pin13_port = (volatile uint8_t * const)pin13_port,
    * const p_pinA0_base = (volatile uint8_t * const)pinA0_base,
    * const p_pinA0_ddr = (volatile uint8_t * const)pinA0_ddr,
    * const p_pinA0_port = (volatile uint8_t * const)pinA0_port,
    * const p_pinA1_base = (volatile uint8_t * const)pinA1_base,
    * const p_pinA1_ddr = (volatile uint8_t * const)pinA1_ddr,
    * const p_pinA1_port = (volatile uint8_t * const)pinA1_port,
    * const p_pinA2_base = (volatile uint8_t * const)pinA2_base,
    * const p_pinA2_ddr = (volatile uint8_t * const)pinA2_ddr,
    * const p_pinA2_port = (volatile uint8_t * const)pinA2_port,
    * const p_pinA3_base = (volatile uint8_t * const)pinA3_base,
    * const p_pinA3_ddr = (volatile uint8_t * const)pinA3_ddr,
    * const p_pinA3_port = (volatile uint8_t * const)pinA3_port,
    * const p_pinA4_base = (volatile uint8_t * const)pinA4_base,
    * const p_pinA4_ddr = (volatile uint8_t * const)pinA4_ddr,
    * const p_pinA4_port = (volatile uint8_t * const)pinA4_port,
    * const p_pinA5_base = (volatile uint8_t * const)pinA5_base;

static volatile uint16_t
    * const p_tcnt1 = (volatile uint16_t * const)tcnt1,
    * const p_ocr1a = (volatile uint16_t * const)ocr1a,
    * const p_ocr1b = (volatile uint16_t * const)ocr1b,
    * const p_icr1 = (volatile uint16_t * const)icr1,
    //* const p_ubrr0 = (volatile uint16_t * const)ubrr0,
    * const p_ubrr9 = (volatile uint16_t * const)ubrr9;

struct Board
{
    Port
        portB { p_portb_base },
        portC { p_portc_base },
        portD { p_portd_base };

    Pin
        pin0 {portD, pd0},
        pin1 {portD, pd1},
        pin2 {portD, pd2},
        pin3 {portD, pd3},
        pin4 {portD, pd4},
        pin5 {portD, pd5},
        pin6 {portD, pd6},
        pin7 {portD, pd7},
        pin8 {portB, pb0},
        pin9 {portB, pb1},
        pin10 {portB, pb2},
        pin11 {portB, pb3},
        pin12 {portB, pb4},
        pin13 {portB, pb5},
        pinA0 {portC, pc0},
        pinA1 {portC, pc1},
        pinA2 {portC, pc2},
        pinA3 {portC, pc3},
        pinA4 {portC, pc4},
        pinA5 {portC, pc5};
};

#define INTR0 __vector_1()
//#define INT1 __vector_2()
//#define PCINT0 vector_3()
#define TIMER2_COMPA __vector_7()
#define TIMER2_COMPB __vector_8()
#define TIMER2_OVF __vector_9()
#define TIMER1_CAPT __vector_10()
#define TIMER1_OVF __vector_13()
#define TIMER0_COMPA __vector_14()
#define TIMER0_COMPB __vector_15()
#define TIMER0_OVF __vector_16()
#define USART_RX __vector_18()
#endif


