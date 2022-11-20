/*
Teensy20++, AT90USB1286

1/PE6
2/PE7
3/
4/
5/
6/
7/
8/
9/PE3
10/PB0
11/PB1/PCINT1/SCLK
12/PB2/PDI/PCINT2/MOSI
13/PB3/PDO/PCINT3/MISO
14/PB4/PCINT4/OC2A
15/PB5/PCINT5/OC1A
16/PB6/PCINT6/OC1B
17/PB7/PCINT7/OC0A/OC1C
18/PE4/INT4/TOSC1
19/PE5/INT5/TOSC2
20/RESET
21/VCC
22/GND
23/XTAL2
24/XTAL1
25/PD0/OC0B/SCL/INT0
26/PD1/OC2B/SDA/INT1
27/PD2/RXD1/INT2
28/PD3/TXD1/INT3
29/PD4/ICP1
30/PD5
31/PD6
32/PD7
33/PE0
34/PE1
35/PC0
36/PC1
37/PC2
38/PC3
39/PC4
40/PC5
41/PC6
42/
43/
44/
45/
46/
47/
48/
49/
50/
51/
52/VCC
53/GND
54/
55/
56/
57/
58/
59/
60/PF1/ADC1
61/PF0/ADC0
62/AREF
63/GND
64/AVCC
*/

#ifndef _TEENSY20PP_H_
#define _TEENSY20PP_H_
#include "mcu_usb1286.h"
#include "pinport.h"

static constexpr uint8_t
    //UCSR9 maps to a UCSR with different number, often UCSR0 or UCSR1
    //Dit maakt het mogelijk om software te schrijven die op verschillende boards
    //werken
    ucsr9a = ucsr1a,
        mpcm9 = 0, u2x9 = 1, upe9 = 2, dor9 = 3, fe9 = 4, udre9 = 5, txc9 = 6, rxc9 = 7,
    ucsr9b = ucsr1b,
        txb89 = 0, rxb89 = 1, ucsz92 = 2, txen9 = 3,
        rxen9 = 4, udrie9 = 5, txcie9 = 6, rxcie9 = 7,
    ucsr9c = ucsr1c,
        ucpol9 = ucpol1, ucsz90 = ucsz10, ucsz91 = ucsz11, usbs9 = usbs1,
        upm90 = upm10, upm91 = upm11, umsel90 = umsel10, umsel91 = umsel11,
    ubrr9 = ubrr1,
    ubrr9l = ubrr1l,
    ubrr9h = ubrr1h,
    udr9 = udr1,

    pin0_base = porte_base,
    pin0_in = pin0_base + 0,
    pin0_ddr = pin0_base + 1,
    pin0_port = pin0_base + 2,
    pin0_bit = pe0,
    pin1_base = portd_base,
    pin1_in = pin1_base + 0,
    pin1_ddr = pin1_base + 1,
    pin1_port = pin1_base + 2,
    pin1_bit = pd2,
    pin2_base = portd_base,
    pin2_in = pin2_base + 0,
    pin2_ddr = pin2_base + 1,
    pin2_port = pin2_base + 2,
    pin2_bit = pd1,
    pin3_base = portd_base,
    pin3_in = pin3_base + 0,
    pin3_ddr = pin3_base + 1,
    pin3_port = pin3_base + 2,
    pin3_bit = pd0,
    pin4_base = portb_base,
    pin4_in = pin4_base + 0,
    pin4_ddr = pin4_base + 1,
    pin4_port = pin4_base + 2,
    pin4_bit = pb0,
    pin5_base = portb_base,
    pin5_in = pin5_base + 0,
    pin5_ddr = pin5_base + 1,
    pin5_port = pin5_base + 2,
    pin5_bit = pb2,
    pin6_base = portb_base,
    pin6_in = pin6_base + 0,
    pin6_ddr = pin6_base + 1,
    pin6_port = pin6_base + 2,
    pin6_bit = pb3,
    pin7_base = portb_base,
    pin7_in = pin7_base + 0,
    pin7_ddr = pin7_base + 1,
    pin7_port = pin7_base + 2,
    pin7_bit = pb4,
    pin8_base = portb_base,
    pin8_in = pin8_base + 0,
    pin8_ddr = pin8_base + 1,
    pin8_port = pin8_base + 2,
    pin8_bit = pb5,
    pin9_base = portb_base,
    pin9_in = pin9_base + 0,
    pin9_ddr = pin9_base + 1,
    pin9_port = pin9_base + 2,
    pin9_bit = pb6,
    pin10_base = portd_base,
    pin10_in = pin10_base + 0,
    pin10_ddr = pin10_base + 1,
    pin10_port = pin10_base + 2,
    pin10_bit = pd3,
    pin11_base = portd_base,
    pin11_ddr = pin11_base + 1,
    pin11_bit = pd4,
    pin12_base = portd_base,
    pin12_ddr = pin12_base + 1,
    pin12_bit = pd5,
    pin13_base = portd_base,
    pin13_in = pin13_base + 0,
    pin13_ddr = pin13_base + 1,
    pin13_port = pin13_base + 2,
    pin13_bit = pb6,
    pinA0_base = portb_base,
    pinA0_in = pinA0_base + 0,
    pinA0_ddr = pinA0_base + 1,
    pinA0_port = pinA0_base + 2,
    pinA0_bit = pb7,
    pinA1_base = portc_base,
    pinA1_in = pinA1_base + 0,
    pinA1_ddr = pinA1_base + 1,
    pinA1_port = pinA1_base + 2,
    pinA1_bit = pc0,
    pinA2_base = portc_base,
    pinA2_in = pinA2_base + 0,
    pinA2_ddr = pinA2_base + 1,
    pinA2_port = pinA2_base + 2,
    pinA2_bit = pc1,
    pinA3_base = portc_base,
    pinA3_in = pinA3_base + 0,
    pinA3_ddr = pinA3_base + 1,
    pinA3_port = pinA3_base + 2,
    pinA3_bit = pc2,
    pinA4_base = portc_base,
    pinA4_in = pinA4_base + 0,
    pinA4_ddr = pinA4_base + 1,
    pinA4_port = pinA4_base + 2,
    pinA4_bit = pc3,
    pinA5_base = portc_base,
    pinA5_in = pinA5_base + 0,
    pinA5_ddr = pinA5_base + 1,
    pinA5_port = pinA5_base + 2,
    pinA5_bit = pc4;

static volatile uint8_t
    * const p_ucsr9a = (volatile uint8_t * const)ucsr9a,
    * const p_ucsr9b = (volatile uint8_t * const)ucsr9b,
    * const p_ucsr9c = (volatile uint8_t * const)ucsr9c,
    * const p_ubrr9l = (volatile uint8_t * const)ubrr9l,
    * const p_ubrr9h = (volatile uint8_t * const)ubrr9h,
    * const p_udr9 = (volatile uint8_t * const)udr9,
    * const p_pin0_in = (volatile uint8_t * const)pin0_in,
    * const p_pin0_ddr = (volatile uint8_t * const)pin0_ddr,
    * const p_pin0_port = (volatile uint8_t * const)pin0_port,
    * const p_pin1_in = (volatile uint8_t * const)pin1_in,
    * const p_pin1_ddr = (volatile uint8_t * const)pin1_ddr,
    * const p_pin1_port = (volatile uint8_t * const)pin1_port,
    * const p_pin2_in = (volatile uint8_t * const)pin2_in,
    * const p_pin2_ddr = (volatile uint8_t * const)pin2_ddr,
    * const p_pin2_port = (volatile uint8_t * const)pin2_port,
    * const p_pin3_in = (volatile uint8_t * const)pin3_in,
    * const p_pin3_ddr = (volatile uint8_t * const)pin3_ddr,
    * const p_pin3_port = (volatile uint8_t * const)pin3_port,
    * const p_pin4_in = (volatile uint8_t * const)pin4_in,
    * const p_pin4_ddr = (volatile uint8_t * const)pin4_ddr,
    * const p_pin5_in = (volatile uint8_t * const)pin5_in,
    * const p_pin5_ddr = (volatile uint8_t * const)pin5_ddr,
    * const p_pin6_in = (volatile uint8_t * const)pin6_in,
    * const p_pin6_ddr = (volatile uint8_t * const)pin6_ddr,
    * const p_pin6_port = (volatile uint8_t * const)pin6_port,
    * const p_pin7_in = (volatile uint8_t * const)pin7_in,
    * const p_pin7_ddr = (volatile uint8_t * const)pin7_ddr,
    * const p_pin8_in = (volatile uint8_t * const)pin8_in,
    * const p_pin8_ddr = (volatile uint8_t * const)pin8_ddr,
    * const p_pin8_port = (volatile uint8_t * const)pin8_port,
    * const p_pin9_in = (volatile uint8_t * const)pin9_in,
    * const p_pin9_ddr = (volatile uint8_t * const)pin9_ddr,
    * const p_pin9_port = (volatile uint8_t * const)pin9_port,
    * const p_pin10_in = (volatile uint8_t * const)pin10_in,
    * const p_pin10_ddr = (volatile uint8_t * const)pin10_ddr,
    * const p_pin10_port = (volatile uint8_t * const)pin10_port,
    * const p_pin11_ddr = (volatile uint8_t * const)pin11_ddr,
    * const p_pin12_ddr = (volatile uint8_t * const)pin12_ddr,
    * const p_pin13_ddr = (volatile uint8_t * const)pin13_ddr,
    * const p_pin13_port = (volatile uint8_t * const)pin13_port,
    * const p_pinA0_ddr = (volatile uint8_t * const)pinA0_ddr,
    * const p_pinA0_port = (volatile uint8_t * const)pinA0_port,
    * const p_pinA1_ddr = (volatile uint8_t * const)pinA1_ddr,
    * const p_pinA1_port = (volatile uint8_t * const)pinA1_port,
    * const p_pinA2_ddr = (volatile uint8_t * const)pinA2_ddr,
    * const p_pinA2_port = (volatile uint8_t * const)pinA2_port,
    * const p_pinA3_ddr = (volatile uint8_t * const)pinA3_ddr,
    * const p_pinA3_port = (volatile uint8_t * const)pinA3_port,
    * const p_pinA4_ddr = (volatile uint8_t * const)pinA4_ddr,
    * const p_pinA4_port = (volatile uint8_t * const)pinA4_port;

static volatile uint16_t
    * const p_ubrr9 = (volatile uint16_t * const)ubrr9;

struct Board
{
    Port
        portB { p_portb_base },
        portC { p_portc_base },
        portD { p_portd_base };

    Pin
        pin1 {portD, pd2},
        pin2 {portD, pd1},
        pin3 {portD, pd0},
        pin4 {portB, pb0},
        pin5 {portB, pb2},
        pin6 {portB, pb3},
        pin7 {portB, pb4},
        pin8 {portB, pb5},
        pin9 {portB, pb6},
        pin10 {portD, pd3},
        pin11 {portD, pd4},
        pin12 {portD, pd5},
        pin13 {portB, pb7},
        pinA0 {portD, pd6},
        pinA1 {portC, pc0},
        pinA2 {portC, pc1},
        pinA3 {portC, pc2},
        pinA4 {portC, pc3},
        pinA5 {portC, pc4};
};
#endif




