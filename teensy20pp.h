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
#include "pinport.h"

static constexpr uint8_t
    porta_base = 0x20,
        pina = porta_base + 0,
        ddra = porta_base + 1,
        porta = porta_base + 2,
            pa0 = 0, pa1 = 1, pa2 = 2, pa3 = 3, pa4 = 4, pa5 = 5, pa6 = 6, pa7 = 7,
    portb_base = 0x23,
        pinb = portb_base + 0,
        ddrb = portb_base + 1,
        portb = portb_base + 2,
            pb0 = 0, pb1 = 1, pb2 = 2, pb3 = 3, pb4 = 4, pb5 = 5, pb6 = 6, pb7 = 7,
    portc_base = 0x26,
        pinc = portc_base + 0,
        ddrc = portc_base + 1,
        portc = portc_base + 2,
            pc0 = 0, pc1 = 1, pc2 = 2, pc3 = 3, pc4 = 4, pc5 = 5, pc6 = 6, pc7 = 7,
    portd_base = 0x29,
        pind = portd_base + 0,
        ddrd = portd_base + 1,
        portd = portd_base + 2,
            pd0 = 0, pd1 = 1, pd2 = 2, pd3 = 3, pd4 = 4, pd5 = 5, pd6 = 6, pd7 = 7,
    porte_base = 0x2c,
        pine = porte_base + 0,
        ddre = porte_base + 1,
        porte = porte_base + 2,
            pe0 = 0, pe1 = 1, pe2 = 2, pe3 = 3, pe4 = 4, pe5 = 5, pe6 = 6, pe7 = 7,
    tifr0 = 0x35, tov0 = 0, ocf0a = 1, ocf0b = 2,
    tifr1 = 0x36, tov1 = 0, ocf1a = 1, ocf1b = 2, ocf1c = 3, icf1 = 5,
    tifr2 = 0x37, tov2 = 0, ocf2a = 1, ocf2b = 2,
    tifr3 = 0x38, tov3 = 0, ocf3a = 1, ocf3b = 2, ocf3c = 3, icf3 = 5,
    pcifr = 0x3b,
    eifr = 0x3c,
    eimsk = 0x3d, int0 = 0, int1 = 1, int2 = 2, int3 = 3, int4 = 4, int5 = 5, int6 = 6, int7 = 7,
    gpior0 = 0x3e,
    eecr = 0x3f, eere = 0, eepe = 1, eempe = 2, eerie = 3, eepm0 = 4, eepm1 = 5,
    eedr = 0x40,
    eear = 0x41, eearl = 0x41, eearh = 0x42,
    gtccr = 0x43, psrsync = 0, psrasy = 1, tsm = 7,
    tccr0a = 0x44, wgm00 = 0, wgm01 = 1, com0b0 = 4, com0b1 = 5, com0a0 = 6, com0a1 = 7,
    tccr0b = 0x45, cs00 = 0, cs01 = 1, cs02 = 2, wgm02 = 3, foc0b = 6, foc0a = 7,
    tcnt0 = 0x46,
    ocr0a = 0x47,
    ocr0b = 0x48,
    pllcsr = 0x49, plock = 0, plee = 1, pllp0 = 2, pllp1 = 3, pllp2 = 4,
    gpior1 = 0x4a,
    gpior2 = 0x4b,
    spcr = 0x4c, spr0 = 0, spr1 = 1, cpha = 2, cpol = 3, mstr = 4, dord = 5, spe = 6, spie = 7,
    spsr = 0x4d, spi2x = 0, wcol = 6, spif = 7,
    spdr = 0x4e,
    acsr = 0x50, acis0 = 0, acis1 = 1, acic = 2, acie = 3, aci = 4,
    eicra = 0x69,
        isc00 = 0, isc01 = 1, isc10 = 2, isc11 = 3, isc20 = 4, isc21 = 5, isc30 = 6, isc31 = 7,
    timsk0 = 0x6e, toie0 = 0, ocie0a = 1, ocie0b = 2,
    timsk1 = 0x6f, toie1 = 0, ocie1a = 1, ocie1b = 2, ocie1c = 3, icie1 = 5,
    timsk2 = 0x70, toie2 = 0, ocie2a = 1, ocie2b = 2,
    timsk3 = 0x71, toie3 = 0, ocie3a = 1, ocie3b = 2, ocie3c = 3, icie3 = 5,
    adc = 0x78, adcw = 0x78, adcl = 0x78, adch = 0x79,
    adcsra = 0x7a,
        adps0 = 0, adps1 = 1, adps2 = 2, adie = 3, adif = 4, adate = 5, adsc = 6, aden = 7,
    adcsrb = 0x7b, adts0 = 0, adts1 = 1, adts2 = 2, acme = 6, adhsm = 7,
    admux = 0x7c,
        mux0 = 0, mux1 = 1, mux2 = 2, mux3 = 3, mux4 = 4, adlar = 5, refs0 = 6, refs1 = 7,
    tccr1a = 0x80,
        wgm10 = 0, wgm11 = 1, com1c0 = 2, com1c1 = 3,
        com1b0 = 4, com1b1 = 5, com1a0 = 6, com1a1 = 7,
    tccr1b = 0x81, cs10 = 0, cs11 = 1, cs12 = 2, wgm12 = 3, wgm13 = 4, ices1 = 6, icnc1 = 7,
    tccr1c = 0x82, foc1c = 5, foc1b = 6, foc1a = 7,
    tcnt1 = 0x84, tcnt1l = 0x84, tcnt1h = 0x85,
    icr1 = 0x86, icr1l = 0x86, icr1h = 0x87,
    ocr1a = 0x88, ocr1al = 0x88, ocr1ah = 0x89,
    ocr1b = 0x8a,
    ocr1c = 0x8c, ocr1cl = 0x8c, ocr1ch = 0x8d,
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
    tccr2a = 0xb0, wgm20 = 0, wgm21 = 1, com2b0 = 4, com2b1 = 5, com2a0 = 6, com2a1 = 7,
    tccr2b = 0xb1, cs20 = 0, cs21 = 1, cs22 = 2, wgm22 = 3, foc2b = 6, foc2a = 7,
    tcnt2 = 0xb2,
    ocr2a = 0xb3,
    ocr2b = 0xb4,
    assr = 0xb6,
    twbr = 0xb8,
    twar = 0xba,
    twdr = 0xbb,
    ucsr1a = 0xc8,
        mpcm1 = 0, u2x1 = 1, upe1 = 2, dor1 = 3, fe1 = 4, udre1 = 5, txc1 = 6, rxc1 = 7,
    ucsr1b = 0xc9,
        txb81 = 0, rxb81 = 1, ucsz12 = 2, txen1 = 3,
        rxen1 = 4, udrie1 = 5, txcie1 = 6, rxcie1 = 7,
    ucsr1c = 0xca,
        ucpol1 = 0, ucsz10 = 1, ucsz11 = 2, usbs1 = 3,
        upm10 = 4, upm11 = 5, umsel10 = 6, umsel11 = 7,
    ubrr1 = 0xcc,
    ubrr1l = 0xcc,
    ubrr1h = 0xcd,
    udr1 = 0xce,
    uhwcon = 0xd7,
    udint = 0xe1,
    udien = 0xe2,
    udaddr = 0xe3,
    ueintx = 0xe8,
    uenum = 0xe9,
    uerst = 0xea,
    uedatx = 0xf1,

    /*
UCSR9 maps to a UCSR with different number, often UCSR0 or UCSR1
    */
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

    ocr0a_port_base = portb_base,
    ocr0a_ddr = ocr0a_port_base + 1,
    ocr0a_bit = pb7,
    ocr0b_port_base = portd_base,
    ocr0b_ddr = ocr0b_port_base + 1,
    ocr0b_bit = pd0,
    ocr1a_port_base = portb_base,
    ocr1a_in = ocr1a_port_base + 0,
    ocr1a_ddr = ocr1a_port_base + 1,
    ocr1a_port = ocr1a_port_base + 2,
    ocr1a_bit = pb5,
    ocr1b_port_base = portb_base,
    ocr1b_in = ocr1b_port_base + 0,
    ocr1b_ddr = ocr1b_port_base + 1,
    ocr1b_port = ocr1b_port_base + 2,
    ocr1b_bit = pb6,
    ocr2a_port_base = portb_base,
    ocr2a_ddr = ocr2a_port_base + 1,
    ocr2a_port = ocr2a_port_base + 2,
    ocr2a_bit = pb4,
    ocr2b_port_base = portd_base,
    ocr2b_ddr = ocr2b_port_base + 1,
    ocr2b_port = ocr2b_port_base + 2,
    ocr2b_bit = pd1,

    int0_port_base = portd_base,
    int0_ddr = int0_port_base + 1,
    int0_port = int0_port_base + 2,
    int0_bit = pd0,

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
    miso_in = miso_port_base + 0,
    miso_ddr = miso_port_base + 1,
    miso_port = miso_port_base + 2,
    miso_bit = pb3,

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
    * const p_uedatx = (volatile uint8_t * const)uedatx,

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




