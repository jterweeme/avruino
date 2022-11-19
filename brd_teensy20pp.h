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
    * const p_porta_base = (volatile uint8_t * const)porta_base,
    * const p_portb_base = (volatile uint8_t * const)portb_base,
    * const p_pinb = (volatile uint8_t * const)pinb,
    * const p_ddrb = (volatile uint8_t * const)ddrb,
    * const p_portc_base = (volatile uint8_t * const)portc_base,
    * const p_ddrc = (volatile uint8_t * const)ddrc,
    * const p_portd_base = (volatile uint8_t * const)portd_base,
    * const p_ddrd = (volatile uint8_t * const)ddrd,
    * const p_porte_base = (volatile uint8_t * const)porte_base,
    * const p_ddre = (volatile uint8_t * const)ddre,
    * const p_tifr0 = (volatile uint8_t * const)tifr0,
    * const p_tifr1 = (volatile uint8_t * const)tifr1,
    * const p_tifr2 = (volatile uint8_t * const)tifr2,
    * const p_tifr3 = (volatile uint8_t * const)tifr3,
    * const p_eimsk = (volatile uint8_t * const)eimsk,
    * const p_tccr0a = (volatile uint8_t * const)tccr0a,
    * const p_tccr0b = (volatile uint8_t * const)tccr0b,
    * const p_tcnt0 = (volatile uint8_t * const)tcnt0,
    * const p_ocr0a = (volatile uint8_t * const)ocr0a,
    * const p_ocr0b = (volatile uint8_t * const)ocr0b,
    * const p_pllcsr = (volatile uint8_t * const)pllcsr,
    * const p_gpior1 = (volatile uint8_t * const)gpior1,
    * const p_gpior2 = (volatile uint8_t * const)gpior2,
    * const p_spcr = (volatile uint8_t * const)spcr,
    * const p_spsr = (volatile uint8_t * const)spsr,
    * const p_spdr = (volatile uint8_t * const)spdr,
    * const p_eicra = (volatile uint8_t * const)eicra,
    * const p_timsk0 = (volatile uint8_t * const)timsk0,
    * const p_timsk1 = (volatile uint8_t * const)timsk1,
    * const p_timsk2 = (volatile uint8_t * const)timsk2,
    * const p_timsk3 = (volatile uint8_t * const)timsk3,
    * const p_adcl = (volatile uint8_t * const)adcl,
    * const p_adch = (volatile uint8_t * const)adch,
    * const p_adcsra = (volatile uint8_t * const)adcsra,
    * const p_adcsrb = (volatile uint8_t * const)adcsrb,
    * const p_admux = (volatile uint8_t * const)admux,
    * const p_tccr1a = (volatile uint8_t * const)tccr1a,
    * const p_tccr1b = (volatile uint8_t * const)tccr1b,
    * const p_tccr1c = (volatile uint8_t * const)tccr1c,
    * const p_tccr3a = (volatile uint8_t * const)tccr3a,
    * const p_tccr3b = (volatile uint8_t * const)tccr3b,
    * const p_tccr3c = (volatile uint8_t * const)tccr3c,
    * const p_tccr2a = (volatile uint8_t * const)tccr2a,
    * const p_tccr2b = (volatile uint8_t * const)tccr2b,
    * const p_tcnt2 = (volatile uint8_t * const)tcnt2,
    * const p_ocr2a = (volatile uint8_t * const)ocr2a,
    * const p_ocr2b = (volatile uint8_t * const)ocr2b,
    * const p_assr = (volatile uint8_t * const)assr,
    * const p_twbr = (volatile uint8_t * const)twbr,
    * const p_twar = (volatile uint8_t * const)twar,
    * const p_twdr = (volatile uint8_t * const)twdr,
    * const p_ucsr1a = (volatile uint8_t * const)ucsr1a,
    * const p_ucsr1b = (volatile uint8_t * const)ucsr1b,
    * const p_ucsr1c = (volatile uint8_t * const)ucsr1c,
    * const p_udr1 = (volatile uint8_t * const)udr1,
    * const p_uhwcon = (volatile uint8_t * const)uhwcon,
    * const p_udaddr = (volatile uint8_t * const)udaddr,
    * const p_uenum = (volatile uint8_t * const)uenum,
    * const p_uerst = (volatile uint8_t * const)uerst,
    * const p_ueconx = (volatile uint8_t * const)ueconx,
    * const p_uecfg0x = (volatile uint8_t * const)uecfg0x,
    * const p_uecfg1x = (volatile uint8_t * const)uecfg1x,
    * const p_uedatx = (volatile uint8_t * const)uedatx,
    * const p_uebclx = (volatile uint8_t * const)uebclx,
    * const p_uebchx = (volatile uint8_t * const)uebchx,

    * const p_ucsr9a = (volatile uint8_t * const)ucsr9a,
    * const p_ucsr9b = (volatile uint8_t * const)ucsr9b,
    * const p_ucsr9c = (volatile uint8_t * const)ucsr9c,
    * const p_ubrr9l = (volatile uint8_t * const)ubrr9l,
    * const p_ubrr9h = (volatile uint8_t * const)ubrr9h,
    * const p_udr9 = (volatile uint8_t * const)udr9,

    * const p_ocr0a_ddr = (volatile uint8_t * const)ocr0a_ddr,
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
    * const p_mosi_ddr = (volatile uint8_t * const)mosi_ddr,
    * const p_mosi_port = (volatile uint8_t * const)mosi_port,
    * const p_miso_ddr = (volatile uint8_t * const)miso_ddr,
    * const p_miso_port = (volatile uint8_t * const)miso_port,

/*
for compatibility with Arduino, the Teensy20++ uses
different numbering

???
*/
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
    * const p_tcnt1 = (volatile uint16_t * const)tcnt1,
    * const p_icr1 = (volatile uint16_t * const)icr1,
    * const p_ocr1a = (volatile uint16_t * const)ocr1a,
    * const p_ocr1b = (volatile uint16_t * const)ocr1b,
    * const p_ocr1c = (volatile uint16_t * const)ocr1c,
    * const p_ubrr1 = (volatile uint16_t * const)ubrr1,
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

#define INTR0 __vector_1()
#define TIMER2_OVF __vector_15()
#define TIMER1_OVF __vector_20()
#define TIMER0_COMPA __vector_21()
#define TIMER0_COMPB __vector_22()
#define TIMER0_OVF __vector_23()
#define USART_RX __vector_25()


#endif




