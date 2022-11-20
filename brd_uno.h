/*
Arduino Uno bord met atmega328p

1/PC6/PCINT14/RESET         RESET
2/PD0/PCINT16/RXD           D0
3/PD1/PCINT17/TXD           D1
4/PD2/PCINT18/INT0          D2
5/PD3/PCINT19/OC2B/INT1     D3
6/PD4/PCINT20/XCK/T0        D4
7/VCC
8/GND
9/PB6/PCINT6/XTAL1/TOSC1
10/PB7/PCINT7/XTAL2/TOSC2
11/PD5/PCINT21/OC0B/T1      D5
12/PD6/AIN0/OC0A/PCINT22    D6
13/PD7/AIN1/PCINT23         D7
14/PB0/ICP1/CLK0/PCINT0     D8
15/PB1/OC1A/PCINT1          D9
16/PB2/SS/OC1B/PCINT2       D10
17/PB3/MOSI/OC2A/PCINT3     D11
18/PB4/MISO/PCINT4          D12
19/PB5/SCK/PCINT5           D13
20/AVCC
21/AREF
22/GND
23/PC0/ADC0/PCINT8          A0
24/PC1/ADC1/PCINT9          A1
25/PC2/ADC2/PCINT10         A2
26/PC3/ADC3/PCINT11         A3
27/PC4/ADC4/SDA/PCINT12     A4
28/PC5/ADC5/SCL/PCINT13     A5


*/

#ifndef _BRD_UNO_H_
#define _BRD_UNO_H_
#include "mcu_m328.h"
#include "pinport.h"

static constexpr uint8_t
    ucsr9a = ucsr0a,
        mpcm9 = 0, u2x9 = 1, upe9 = 2, dor9 = 3, fe9 = 4, udre9 = 5, txc9 = 6, rxc9 = 7,
    ucsr9b = ucsr0b,
        txb89 = 0, rxb89 = 1, ucxz92 = 2, txen9 = txen0,
        rxen9 = 4, udrie9 = 5, txcie9 = 6, rxcie9 = rxcie0,
    ucsr9c = ucsr0c,
        ucpol9 = 0, ucsz90 = 1, ucpha9 = 1, ucsz91 = 2, udord9 = 2, usbs9 = 3,
        upm90 = 4, upm91 = 5, umsel90 = 6, umsel91 = 7,
    ubrr9 = ubrr0,
    ubrr9l = ubrr0l,
    ubrr9h = ubrr0h,
    udr9 = udr0,

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
    * const p_ucsr9a = (volatile uint8_t * const)ucsr9a,
    * const p_ucsr9b = (volatile uint8_t * const)ucsr9b,
    * const p_ucsr9c = (volatile uint8_t * const)ucsr9c,
    * const p_ubrr9l = (volatile uint8_t * const)ubrr9l,
    * const p_ubrr9h = (volatile uint8_t * const)ubrr9h,
    * const p_udr9 = (volatile uint8_t * const)udr9,

    * const p_pin0_base = (volatile uint8_t * const)pin0_base,
    * const p_pin0_in = (volatile uint8_t * const)pin0_in,
    * const p_pin0_ddr = (volatile uint8_t * const)pin0_ddr,
    * const p_pin0_port = (volatile uint8_t * const)pin0_port,
    * const p_pin1_base = (volatile uint8_t * const)pin1_base,
    * const p_pin1_in = (volatile uint8_t * const)pin1_in,
    * const p_pin1_ddr = (volatile uint8_t * const)pin1_ddr,
    * const p_pin1_port = (volatile uint8_t * const)pin1_port,
    * const p_pin2_base = (volatile uint8_t * const)pin2_base,
    * const p_pin2_in = (volatile uint8_t * const)pin2_in,
    * const p_pin2_ddr = (volatile uint8_t * const)pin2_ddr,
    * const p_pin2_port = (volatile uint8_t * const)pin2_port,
    * const p_pin3_base = (volatile uint8_t * const)pin3_base,
    * const p_pin3_in = (volatile uint8_t * const)pin3_in,
    * const p_pin3_ddr = (volatile uint8_t * const)pin3_ddr,
    * const p_pin3_port = (volatile uint8_t * const)pin3_port,
    * const p_pin4_base = (volatile uint8_t * const)pin4_base,
    * const p_pin4_in = (volatile uint8_t * const)pin4_in,
    * const p_pin4_ddr = (volatile uint8_t * const)pin4_ddr,
    * const p_pin5_base = (volatile uint8_t * const)pin5_base,
    * const p_pin5_in = (volatile uint8_t * const)pin5_in,
    * const p_pin5_ddr = (volatile uint8_t * const)pin5_ddr,
    * const p_pin6_base = (volatile uint8_t * const)pin6_base,
    * const p_pin6_in = (volatile uint8_t * const)pin6_in,
    * const p_pin6_ddr = (volatile uint8_t * const)pin6_ddr,
    * const p_pin6_port = (volatile uint8_t * const)pin6_port,
    * const p_pin7_base = (volatile uint8_t * const)pin7_base,
    * const p_pin7_in = (volatile uint8_t * const)pin7_in,
    * const p_pin7_ddr = (volatile uint8_t * const)pin7_ddr,
    * const p_pin8_base = (volatile uint8_t * const)pin8_base,
    * const p_pin8_in = (volatile uint8_t * const)pin8_in,
    * const p_pin8_ddr = (volatile uint8_t * const)pin8_ddr,
    * const p_pin8_port = (volatile uint8_t * const)pin8_port,
    * const p_pin9_base = (volatile uint8_t * const)pin9_base,
    * const p_pin9_in = (volatile uint8_t * const)pin9_in,
    * const p_pin9_ddr = (volatile uint8_t * const)pin9_ddr,
    * const p_pin9_port = (volatile uint8_t * const)pin9_port,
    * const p_pin10_base = (volatile uint8_t * const)pin10_base,
    * const p_pin10_in = (volatile uint8_t * const)pin10_in,
    * const p_pin10_ddr = (volatile uint8_t * const)pin10_ddr,
    * const p_pin10_port = (volatile uint8_t * const)pin10_port,
    * const p_pin11_base = (volatile uint8_t * const)pin11_base,
    * const p_pin11_ddr = (volatile uint8_t * const)pin11_ddr,
    * const p_pin11_port = (volatile uint8_t * const)pin11_port,
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
    * const p_ubrr0 = (volatile uint16_t * const)ubrr0,
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


