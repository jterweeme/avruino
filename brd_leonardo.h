/*
Arduino Leonardo, ATmega32U4

1/PE6
2/UVcc
3/D-
4/D+
5/UGnd
6/UCap
7/VBus
8/PB0/SS/PCINT0
9/PB1
10/PB2
11/PB3
12/PB7/PCINT7/OC0A/OC1C/RTS
13/RESET
14/VCC
15/GND
16/XTAL2
17/XTAL1
18/PD0/OC0B/SCL/INT0
19/PD1/SDA/INT1
20/PD2
21/PD3
22/PD5/XCK1/CTS
23/GND
24/AVCC
25/PD4
26/PD6
27/PD7
28/PB4
29/PB5/PCINT5/OC1A/OC4B
30/PB6/PCINT6/OC1B/OC4B
31/PC6/OC3A/OC4A
32/PC7/ICP3/CLK0/OC4A
33/PE2/HWB
34/VCC
35/GND
36/PF7/ADC7/TDI
37/PF6/ADC6/TDO
38/PF5/ADC5/TMS
39/PF4/ADC4/TCK
40/PF1/ADC1
41/PF0/ADC0
42/AREF
43/GND
44/AVCC


*/

#ifndef _LEONARDO_H_
#define _LEONARDO_H_
#include "mcu_32u4.h"
#include "pinport.h"

static constexpr uint8_t
    //UCSR9 maps to a UCSR with different number, often UCSR0 or UCSR1
    ucsr9a = ucsr1a,
        mpcm9 = 0, u2x9 = 1, upe9 = 2, dor9 = 3, fe9 = 4, udre9 = 5, txc9 = 6, rxc9 = 7,
    ucsr9b = ucsr1b,
        txb89 = 0, rxb89 = 1, ucsz92 = 2, txen9 = 3,
        rxen9 = 4, udrie9 = 5, txcie9 = 6, rxcie9 = 7,
    ucsr9c = ucsr1c,
        ucpol9 = 0, ucsz90 = 1, ucsz91 = 2, usbs9 = 3,
        upm90 = 4, upm91 = 5, umsel90 = 6, umsel91 = 7,
    ubrr9 = ubrr1, ubrr9l = ubrr1l, ubrr9h = ubrr1h,
    udr9 = udr1,

    pin0_base = portd_base,
    pin0_in = pin0_base + 0,
    pin0_ddr = pin0_base + 1,
    pin0_port = pin0_base + 2,
    pin0_bit = pd2,
    pin1_base = portd_base,
    pin1_in = pin1_base + 0,
    pin1_ddr = pin1_base + 1,
    pin1_port = pin1_base + 2,
    pin1_bit = pd3,
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
    pin4_base = portd_base,
    pin4_in = pin4_base + 0,
    pin4_ddr = pin4_base + 1,
    pin4_port = pin4_base + 2,
    pin4_bit = pd4,
    pin5_base = portc_base,
    pin5_in = pin5_base + 0,
    pin5_ddr = pin5_base + 1,
    pin5_port = pin5_base + 2,
    pin5_bit = pc6,
    pin6_base = portd_base,
    pin6_in = pin6_base + 0,
    pin6_ddr = pin6_base + 1,
    pin6_port = pin6_base + 2,
    pin6_bit = pd7,
    pin7_base = porte_base,
    pin7_in = pin7_base + 0,
    pin7_ddr = pin7_base + 1,
    pin7_port = pin7_base + 2,
    pin7_bit = pe6,
    pin8_base = portb_base,
    pin8_in = pin8_base + 0,
    pin8_ddr = pin8_base + 1,
    pin8_port = pin8_base + 2,
    pin8_bit = pb4,
    pin9_base = portb_base,
    pin9_in = pin9_base + 0,
    pin9_ddr = pin9_base + 1,
    pin9_port = pin9_base + 2,
    pin9_bit = pb5,
    pin10_base = portb_base,
    pin10_in = pin10_base + 0,
    pin10_ddr = pin10_base + 1,
    pin10_port = pin10_base + 2,
    pin10_bit = pb6,
    pin11_base = portb_base,
    pin11_in = pin11_base + 0,
    pin11_ddr = pin11_base + 1,
    pin11_port = pin11_base + 2,
    pin11_bit = pb7,
    pin12_base = portd_base,
    pin12_in = pin12_base + 0,
    pin12_ddr = pin12_base + 1,
    pin12_port = pin12_base + 2,
    pin12_bit = pd6,
    pin13_base = portc_base,
    pin13_in = pin13_base + 0,
    pin13_ddr = pin13_base + 1,
    pin13_port = pin13_base + 2,
    pin13_bit = pc7,
    pinA0_base = portf_base,
    pinA0_in = pinA0_base + 0,
    pinA0_ddr = pinA0_base + 1,
    pinA0_port = pinA0_base + 2,
    pinA0_bit = pf7,
    pinA1_base = portf_base,
    pinA1_in = pinA1_base + 0,
    pinA1_ddr = pinA1_base + 1,
    pinA1_port = pinA1_base + 2,
    pinA1_bit = pf6,
    pinA2_base = portf_base,
    pinA2_in = pinA2_base + 0,
    pinA2_ddr = pinA2_base + 1,
    pinA2_port = pinA2_base + 2,
    pinA2_bit = pf5,
    pinA3_base = portf_base,
    pinA3_in = pinA3_base + 0,
    pinA3_ddr = pinA3_base + 1,
    pinA3_port = pinA3_base + 2,
    pinA3_bit = pf4,
    pinA4_base = portf_base,
    pinA4_in = pinA4_base + 0,
    pinA4_ddr = pinA4_base + 1,
    pinA4_port = pinA4_base + 2,
    pinA4_bit = pf1,
    pinA5_base = portf_base,
    pinA5_in = pinA5_base + 0,
    pinA5_ddr = pinA5_base + 1,
    pinA5_port = pinA5_base + 2,
    pinA5_bit = pf0;

static volatile uint8_t
    * const p_ucsr9a = (volatile uint8_t * const)ucsr9a,
    * const p_ucsr9b = (volatile uint8_t * const)ucsr9b,
    * const p_ucsr9c = (volatile uint8_t * const)ucsr9c,
    * const p_ubrr9l = (volatile uint8_t * const)ubrr9l,
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
    * const p_pinA0_in = (volatile uint8_t * const)pinA0_in,
    * const p_pinA0_ddr = (volatile uint8_t * const)pinA0_ddr,
    * const p_pinA0_port = (volatile uint8_t * const)pinA0_port,
    * const p_pinA1_base = (volatile uint8_t * const)pinA1_base,
    * const p_pinA1_in = (volatile uint8_t * const)pinA1_in,
    * const p_pinA1_ddr = (volatile uint8_t * const)pinA1_ddr,
    * const p_pinA1_port = (volatile uint8_t * const)pinA1_port,
    * const p_pinA2_base = (volatile uint8_t * const)pinA2_base,
    * const p_pinA2_in = (volatile uint8_t * const)pinA2_in,
    * const p_pinA2_ddr = (volatile uint8_t * const)pinA2_ddr,
    * const p_pinA2_port = (volatile uint8_t * const)pinA2_port,
    * const p_pinA3_base = (volatile uint8_t * const)pinA3_base,
    * const p_pinA3_in = (volatile uint8_t * const)pinA3_in,
    * const p_pinA3_ddr = (volatile uint8_t * const)pinA3_ddr,
    * const p_pinA3_port = (volatile uint8_t * const)pinA3_port,
    * const p_pinA4_base = (volatile uint8_t * const)pinA4_base,
    * const p_pinA4_in = (volatile uint8_t * const)pinA4_in,
    * const p_pinA4_ddr = (volatile uint8_t * const)pinA4_ddr,
    * const p_pinA4_port = (volatile uint8_t * const)pinA4_port,
    * const p_pinA5_base = (volatile uint8_t * const)pinA5_base,
    * const p_pinA5_in = (volatile uint8_t * const)pinA5_in,
    * const p_pinA5_ddr = (volatile uint8_t * const)pinA5_ddr,
    * const p_pinA5_port = (volatile uint8_t * const)pinA5_port;

static volatile uint16_t
    * const p_ubrr9 = (volatile uint16_t * const)ubrr9;

#if 0
class Timer3 : public Timer<uint16_t>
{
protected:
    static Timer3 *instance;
public:
    Timer3();
    static Timer3 *getInstance() { return instance; }
};

class Timer4 : public Timer<uint16_t>
{
protected:
    static Timer4 *instance;
public:
    Timer4();
    static Timer4 *getInstance() { return instance; }
};
#endif

struct Board
{
    Port
        portB { (uint8_t *)portb_base },
        portC { (uint8_t *)portc_base },
        portD { (uint8_t *)portd_base },
        portE { (uint8_t *)porte_base },
        portF { (uint8_t *)portf_base };

    Pin
        pin0 { portD, pd2 },
        pin1 { portD, pd3 },
        pin2 { portD, pd1 },
        pin3 { portD, pd0 },
        pin4 { portD, pd4 },
        pin5 { portC, pc6 },
        pin6 { portD, BIT7 },
        pin7 { portE, BIT6 },
        pin8 { portB, BIT4 },
        pin9 { portB, BIT5 },
        pin10 { portB, BIT6 },
        pin11 { portB, BIT7 },
        pin12 { portD, BIT6 },
        pin13 { portC, BIT7 },
        pinA0 { portF, BIT7 },
        pinA1 { portF, BIT6 },
        pinA2 { portF, BIT5 },
        pinA3 { portF, BIT4 },
        pinA4 { portF, BIT1 },
        pinA5 { portF, BIT0 };
};

#define INTR0 __vector_1()
#define USB_GEN __vector_10()
#define USB_COM __vector_11()
#define TIMER1_CAPT __vector_16()
#define TIMER1_COMPA __vector_17()
#define TIMER1_OVF __vector_20()
#define TIMER0_COMPA __vector_21()
#define TIMER0_COMPB __vector_22()
#define TIMER0_OVF __vector_23()
#define USART1_RX __vector_25()
#define USART_RX USART1_RX
#endif


