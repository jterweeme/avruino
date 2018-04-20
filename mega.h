#ifndef _MEGA_H_
#define _MEGA_H_
#include "misc.h"

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

static constexpr uint8_t
    porta_base = 0x20,
        pina = porta_base,
            pina0 = 0, pina1 = 1, pina2 = 2, pina3 = 3,
            pina4 = 4, pina5 = 5, pina6 = 6, pina7 = 7,
        ddra = porta_base + 1,
        porta = porta_base + 2,
    portb_base = 0x23,
        pinb = portb_base,
            pinb0 = 0, pinb1 = 1, pinb2 = 2, pinb3 = 3,
            pinb4 = 4, pinb5 = 5, pinb6 = 6, pinb7 = 7,
        ddrb = portb_base + 1,
            ddb0 = 0, ddb1 = 1, ddb2 = 2, ddb3 = 3, ddb4 = 4, ddb5 = 5, ddb6 = 6, ddb7 = 7,
        portb = portb_base + 2,
            portb0 = 0, portb1 = 1, portb2 = 2, portb3 = 3,
            portb4 = 4, portb5 = 5, portb6 = 6, portb7 = 7,
        pb0 = 0, pb1 = 1, pb2 = 2, pb3 = 3, pb4 = 4, pb5 = 5, pb6 = 6, pb7 = 7,
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
        pd0 = 0, pd1 = 1, pd2 = 2, pd3 = 3, pd4 = 4, pd5 = 5, pd6 = 6, pd7 = 7,
    porte_base = 0x2c,
        pine = porte_base,
        ddre = porte_base + 1,
        porte = porte_base + 2,
            porte0 = 0,
        pe0 = 0, pe1 = 1, pe3 = 3, pe4 = 4, pe5 = 5,
    portf_base = 0x2f,
        pinf = portf_base,
            pinf0 = 0, pinf1 = 1, pinf2 = 2, pinf3 = 3,
            pinf4 = 4, pinf5 = 5, pinf6 = 6, pinf7 = 7,
        ddrf = portf_base + 1,
            ddf0 = 0, ddf1 = 1, ddf2 = 2, ddf3 = 3, ddf4 = 4, ddf5 = 5, ddf6 = 6, ddf7 = 7,
        portf = portf_base + 2,
            portf0 = 0, portf1 = 1, portf2 = 2, portf3 = 3,
            portf4 = 4, portf5 = 5, portf6 = 6, portf7 = 7,
        pf0 = 0, pf1 = 1, pf2 = 2, pf3 = 3, pf4 = 4, pf5 = 5, pf6 = 6, pf7 = 7,
    portg_base = 0x32,
        ping = portg_base,
            ping0 = 0, ping1 = 1, ping2 = 2, ping3 = 3, ping4 = 4, ping5 = 5,
        ddrg = portg_base + 1,
            ddg0 = 0, ddg1 = 1, ddg2 = 2, ddg3 = 3, ddg4 = 4, ddg5 = 5,
        portg = portg_base + 2,
            portg0 = 0, portg1 = 1, portg2 = 2, portg3 = 3, portg4 = 4, portg5 = 5,
        pg0 = 0, pg1 = 1, pg2 = 2, pg3 = 3, pg4 = 4, pg5 = 5,
    tifr0 = 0x35, tov0 = 0, ocf0a = 1, ocf0b = 2,
    tifr1 = 0x36, tov1 = 0, ocf1a = 1, ocf1b = 2, ocf1c = 3, icf1 = 5,
    tifr2 = 0x37, tov2 = 0, ocf2a = 1, ocf2b = 2,
    tifr3 = 0x38, tov3 = 0, ocf3a = 1, ocf3b = 2, ocf3c = 3, icf3 = 5,
    tifr4 = 0x39,
    tifr5 = 0x3a,
    pcifr = 0x3b, pcif1 = 1, pcif0 = 0,
    eifr = 0x3c,
    eimsk = 0x3d, int7 = 7, int6 = 6, int5 = 5, int4 = 4, int3 = 3, int2 = 2, int1 = 1, int0 = 0,
    gpior0 = 0x3e,
    eecr = 0x3f, eepm1 = 5, eepm0 = 4, eerie = 3, eempe = 2, eepe = 1, eere = 0,
    eedr = 0x40,
    eear = 0x41,
    eearl = 0x41,
    eearh = 0x42,
    tccr0a = 0x44, wgm00 = 0, wgm01 = 1, com0b0 = 4, com0a1 = 7, com0a0 = 6, com0b1 = 5,
    tccr0b = 0x45, cs00 = 0, cs01 = 1, cs02 = 2, wgm02 = 3, foc0b = 6, foc0a = 7,
    tcnt0 = 0x46,
    ocr0a = 0x47,
    ocr0b = 0x48,
    gpior1 = 0x4a,
    gpior2 = 0x4b,
    spcr = 0x4c, spie = 7, spe = 6, dord = 5, mstr = 4, cpol = 3, cpha = 2, spr1 = 1, spr0 = 0,
    spsr = 0x4d, spi2x = 0, wcol = 6, spif = 7,
    spdr = 0x4e,
    acsr = 0x50,
    pcicr = 0x68,
    eicra = 0x69,
        isc31 = 7, isc30 = 6, isc21 = 5, isc20 = 4, isc11 = 3, isc10 = 2, isc01 = 1, isc00 = 0,
    eicrb = 0x6a,
    pcmsk0 = 0x6b,
    pcmsk1 = 0x6c,
    pcmsk2 = 0x6d,
    timsk0 = 0x6e, toie0 = 0, ocie0a = 1, ocie0b = 2,
    timsk1 = 0x6f, toie1 = 0, ocie1a = 1, ocie1b = 2, ocie1c = 3, icie1 = 5,
    timsk2 = 0x70, toie2 = 0, ocie2a = 1, ocie2b = 2,
    timsk3 = 0x71,
    timsk4 = 0x72,
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
    tccr2a = 0xb0, com2a1 = 7, com2a0 = 6, com2b1 = 5, com2b0 = 4, wgm21 = 1, wgm20 = 0,
    tccr2b = 0xb1, foc2a = 7, foc2b = 6, wgm22 = 3, cs22 = 2, cs21 = 1, cs20 = 0,
    tcnt2 = 0xb2,
    ocr2a = 0xb3,
    ocr2b = 0xb4,
    assr = 0xb6,
    twbr = 0xb8,
    twsr = 0xb9,
    twar = 0xba,
    ucsr0a = 0xc0,
        rxc0 = 7, txc0 = 6, udre0 = 5, fe0 = 4, dor0 = 3, upe0 = 2, mpcm0 = 0, u2x0 = 1,
    ucsr0b = 0xc1,
        txb80 = 0, rxb80 = 1, ucxz02 = 2, txen0 = 3,
        rxen0 = 4, udrie0 = 5, txcie0 = 6, rxcie0 = 7,
    ucsr0c = 0xc2,
        umsel01 = 7, umsel00 = 6, upm01 = 5, upm00 = 4,
        usbs0 = 3, ucsz01 = 2, ucsz00 = 1, ucpol0 = 0,
    ubrr0 = 0xc4,
    ubrr0l = 0xc4,
    ubrr0h = 0xc5,
    udr0 = 0xc6,
    ucsr1a = 0xc8,
        rxc1 = 7, txc1 = 6, udre1 = 5, fe1 = 4, dor1 = 3, upe1 = 2, u2x1 = 1, mpcm1 = 0,
    ucsr1b = 0xc9,
        rxcie1 = 7, txcie1 = 6, udrie1 = 5, rxen1 = 4,
        txen1 = 3, ucsz12 = 2, rxb81 = 1, txb81 = 0,
    ucsr1c = 0xca,
        umsel11 = 7, umsel10 = 6, upm11 = 5, upm10 = 4,
        usbs1 = 3, ucsz11 = 2, ucsz10 = 1, ucpol1 = 0,
    ubrr1 = 0xcc,
    ubrr1l = 0xcc,
    ubrr1h = 0xcd,
    udr1 = 0xce,

    ucsr9a = ucsr0a,
        rxc9 = rxc0, txc9 = txc0, udre9 = udre0, fe9 = fe0,
        dor9 = dor0, upe9 = upe0, mpcm9 = mpcm0, u2x9 = u2x0,
    ucsr9b = ucsr0b,
        txb89 = txb80, rxb89 = rxb80, ucxz92 = ucxz02, txen9 = txen0,
        rxen9 = rxen0, udrie9 = udrie0, txcie9 = txcie0, rxcie9 = rxcie0,
    ucsr9c = ucsr0c,
        umsel91 = 7, umsel90 = 6, upm91 = 5, upm90 = 4,
        usbs9 = 3, ucsz91 = 2, ucsz90 = 1, ucpol9 = 0,
    ubrr9 = ubrr0,
    ubrr9l = ubrr0l,
    ubrr9h = ubrr0h,
    udr9 = udr0,

    ocr1a_port_base = portb_base,
    ocr1a_ddr = ocr1a_port_base + 1,
    ocr1a_port = ocr1a_port_base + 2,
    ocr1a_bit = pb5,
    ocr1b_port_base = portb_base,
    ocr1b_ddr = ocr1b_port_base + 1,
    ocr1b_port = ocr1b_port_base + 2,
    ocr1b_bit = pb6,
    ph6 = 6,
    ocr2b_bit = ph6,

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
    miso_ddr = miso_port_base + 1,
    miso_port = miso_port_base + 2,
    miso_bit = pb3,

    pin0_base = porte_base,
    pin0_bit = pe0,
    pin1_bit = pe1,
    pin2_bit = pe4,
    pin3_bit = pe5,
    pin4_bit = pg5,
    pin5_bit = pe3,
    ph3 = 3,
    ph4 = 4,
    ph5 = 5,
    pin6_bit = ph3,
    pin7_bit = ph4,
    pin8_bit = ph5,
    pin9_bit = ph6,
    pin10_base = portb_base,
    pin10_bit = pb4,
    pin11_bit = pb5,
    pin12_bit = pb6,
    pin13_base = portb_base,
    pin13_ddr = pin13_base + 1,
    pin13_port = pin13_base + 2,
    pin13_bit = pb7,
    pinA0_base = portf_base,
    pinA0_ddr = pinA0_base + 1,
    pinA0_port = pinA0_base + 2,
    pinA0_bit = pf0;

static constexpr uint16_t
    porth_base = 0x100,
    portj_base = 0x103,
    portk_base = 0x106,
    portl_base = 0x109,

    ocr2b_port_base = porth_base,
    ocr2b_ddr = ocr2b_port_base + 1,
    ocr2b_port = ocr2b_port_base + 2;

static volatile uint8_t
    * const p_porta_base = (volatile uint8_t * const)porta_base,
    * const p_pina = (volatile uint8_t * const)pina,
    * const p_portb_base = (volatile uint8_t * const)portb_base,
    * const p_pinb = (volatile uint8_t * const)pinb,
    * const p_ddrb = (volatile uint8_t * const)ddrb,
    * const p_portb = (volatile uint8_t * const)portb,
    * const p_portc_base = (volatile uint8_t * const)portc_base,
    * const p_pinc = (volatile uint8_t * const)pinc,
    * const p_ddrc = (volatile uint8_t * const)ddrc,
    * const p_ddrd = (volatile uint8_t * const)ddrd,
    * const p_ddrf = (volatile uint8_t * const)ddrf,
    * const p_portf = (volatile uint8_t * const)portf,
    * const p_portg = (volatile uint8_t * const)portg,
    * const p_tifr1 = (volatile uint8_t * const)tifr1,
    * const p_tifr2 = (volatile uint8_t * const)tifr2,
    * const p_tifr3 = (volatile uint8_t * const)tifr3,
    * const p_tifr4 = (volatile uint8_t * const)tifr4,
    * const p_tifr5 = (volatile uint8_t * const)tifr5,
    * const p_eimsk = (volatile uint8_t * const)eimsk,
    * const p_tccr0a = (volatile uint8_t * const)tccr0a,
    * const p_tccr0b = (volatile uint8_t * const)tccr0b,
    * const p_tcnt0 = (volatile uint8_t * const)tcnt0,
    * const p_ocr0a = (volatile uint8_t * const)ocr0a,
    * const p_ocr0b = (volatile uint8_t * const)ocr0b,
    * const p_gpior1 = (volatile uint8_t * const)gpior1,
    * const p_gpior2 = (volatile uint8_t * const)gpior2,
    * const p_spcr = (volatile uint8_t * const)spcr,
    * const p_spsr = (volatile uint8_t * const)spsr,
    * const p_spdr = (volatile uint8_t * const)spdr,
    * const p_eicra = (volatile uint8_t * const)eicra,
    * const p_eicrb = (volatile uint8_t * const)eicrb,
    * const p_pcmsk0 = (volatile uint8_t * const)pcmsk0,
    * const p_pcmsk1 = (volatile uint8_t * const)pcmsk1,
    * const p_pcmsk2 = (volatile uint8_t * const)pcmsk2,
    * const p_timsk0 = (volatile uint8_t * const)timsk0,
    * const p_timsk1 = (volatile uint8_t * const)timsk1,
    * const p_timsk2 = (volatile uint8_t * const)timsk2,
    * const p_timsk3 = (volatile uint8_t * const)timsk3,
    * const p_timsk4 = (volatile uint8_t * const)timsk4,
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
    * const p_tccr2a = (volatile uint8_t * const)tccr2a,
    * const p_tccr2b = (volatile uint8_t * const)tccr2b,
    * const p_ocr2a = (volatile uint8_t * const)ocr2a,
    * const p_ocr2b = (volatile uint8_t * const)ocr2b,
    * const p_assr = (volatile uint8_t * const)assr,
    * const p_twar = (volatile uint8_t * const)twar,
    * const p_ucsr0a = (volatile uint8_t * const)ucsr0a,
    * const p_ucsr0b = (volatile uint8_t * const)ucsr0b,
    * const p_ucsr0c = (volatile uint8_t * const)ucsr0c,
    * const p_ubrr0l = (volatile uint8_t * const)ubrr0l,
    * const p_ubrr0h = (volatile uint8_t * const)ubrr0h,
    * const p_udr0 = (volatile uint8_t * const)udr0,
    * const p_ucsr1a = (volatile uint8_t * const)ucsr1a,
    * const p_ucsr1b = (volatile uint8_t * const)ucsr1b,
    * const p_ucsr1c = (volatile uint8_t * const)ucsr1c,
    * const p_ubrr1l = (volatile uint8_t * const)ubrr1l,
    * const p_ubrr1h = (volatile uint8_t * const)ubrr1h,
    * const p_udr1 = (volatile uint8_t * const)udr1,

    * const p_ucsr9a = (volatile uint8_t * const)ucsr9a,
    * const p_ucsr9b = (volatile uint8_t * const)ucsr9b,
    * const p_ucsr9c = (volatile uint8_t * const)ucsr9c,
    * const p_ubrr9l = (volatile uint8_t * const)ubrr9l,
    * const p_ubrr9h = (volatile uint8_t * const)ubrr9h,
    * const p_udr9 = (volatile uint8_t * const)udr9,

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

    * const p_pin0_base = (volatile uint8_t * const)pin0_base,
    * const p_pin13_base = (volatile uint8_t * const)pin13_base,
    * const p_pin13_ddr = (volatile uint8_t * const)pin13_ddr,
    * const p_pin13_port = (volatile uint8_t * const)pin13_port,
    * const p_pinA0_base = (volatile uint8_t * const)pinA0_base,
    * const p_pinA0_ddr = (volatile uint8_t * const)pinA0_ddr,
    * const p_pinA0_port = (volatile uint8_t * const)pinA0_port;

static volatile uint16_t
    * const p_tcnt1 = (volatile uint16_t * const)tcnt1,
    * const p_ocr1a = (volatile uint16_t * const)ocr1a,
    * const p_ocr1b = (volatile uint16_t * const)ocr1b,
    * const p_icr1 = (volatile uint16_t * const)icr1,
    * const p_ubrr0 = (volatile uint16_t * const)ubrr0,
    * const p_ubrr1 = (volatile uint16_t * const)ubrr1,
    * const p_ubrr9 = (volatile uint16_t * const)ubrr9;

struct Board
{
    Port
        portA { p_porta_base },
        portB { p_portb_base },
        portC { p_portc_base },
        portD { (uint8_t *)portd_base },
        portE { (uint8_t *)porte_base },
        portF { (uint8_t *)portf_base },
        portG { (uint8_t *)portg_base },
        portH { (uint8_t *)porth_base },
        portJ { (uint8_t *)portj_base },
        portK { (uint8_t *)portk_base },
        portL { (uint8_t *)portl_base };

    Pin
        pin0 {portE, BIT0},
        pin1 {portE, BIT1},
        pin2 {portE, BIT4},
        pin3 {portE, BIT5},
        pin4 {portG, BIT5},
        pin5 {portE, pe3},
        pin6 {portH, ph3},
        pin7 {portH, ph4},
        pin8 {portH, ph5},
        pin9 {portH, ph6},
        pin10 {portB, pb4},
        pin11 {portB, pb5},
        pin12 {portB, pb6},
        pin13 {portB, pb7},
        pin14 {portJ, BIT1},
        pin15 {portJ, BIT0},
        pin16 {portH, BIT1},
        pin17 {portH, BIT0},
        pin18 {portD, BIT3},
        pin19 {portD, BIT2},
        pin20 {portD, BIT1},
        pin21 {portD, BIT0},
        pin22 {portA, BIT0},
        pin23 {portA, BIT1},
        pin24 {portA, BIT2},
        pin25 {portA, BIT3},
        pin26 {portA, BIT4},
        pin27 {portA, BIT5},
        pin28 {portA, BIT6},
        pin29 {portA, BIT7},
        pin30 {portC, BIT7},
        pin31 {portC, BIT6},
        pin32 {portC, BIT5},
        pin33 {portC, BIT4},
        pin34 {portC, BIT3},
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
        pin50 {portB, pb3},
        pin51 {portB, pb2},
        pin52 {portB, pb1},
        pin53 {portB, pb0},
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

#define INTR0 __vector_1()
#define TIMER0_COMPA __vector_21()
#define TIMER0_COMPB __vector_22()
#define TIMER0_OVF __vector_23()
#define USART_RX __vector_25()
#define TIMER1_OVF __vector_20()
#define TIMER2_OVF __vector_15()

#endif


