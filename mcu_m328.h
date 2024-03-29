#ifndef _MCU_M328_H_
#define _MCU_M328_H_

#include "types.h"

#define zei()  __asm__ __volatile__ ("sei" ::: "memory")
#define zli()  __asm__ __volatile__ ("cli" ::: "memory")

static constexpr uint8_t
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
            portc0 = 0, portc1 = 1, portc2 = 2, portc3 = 3,
            portc4 = 4, portc5 = 5, portc6 = 6, portc7 = 7,
            pc0 = 0, pc1 = 1, pc2 = 2, pc3 = 3, pc4 = 4, pc5 = 5, pc6 = 6, pc7 = 7, 
    portd_base = 0x29,
        pind = portd_base, 
            pind0 = 0, pind1 = 1, pind2 = 2, pind3 = 3,
            pind4 = 4, pind5 = 5, pind6 = 6, pind7 = 7,
        ddrd = portd_base + 1,
            ddd0 = 0, ddd1 = 1, ddd2 = 2, ddd3 = 3,
            ddd4 = 4, ddd5 = 5, ddd6 = 6, ddd7 = 7,
        portd = portd_base + 2,
            portd0 = 0, portd1 = 1, portd2 = 2, portd3 = 3,
            portd4 = 4, portd5 = 5, portd6 = 6, portd7 = 7,
            pd0 = 0, pd1 = 1, pd2 = 2, pd3 = 3, pd4 = 4, pd5 = 5, pd6 = 6, pd7 = 7,
    tifr0 = 0x35, tov0 = 0, ocf0a = 1, ocf0b = 2,
    tifr1 = 0x36, tov1 = 0, ocf1a = 1, ocf1b = 2, icf1 = 5,
    tifr2 = 0x37, tov2 = 0, ocf2a = 1, ocf2b = 2,
    pcifr = 0x3b, pcif0 = 0, pcif1 = 1, pcif2 = 2,
    eifr = 0x3c, intf0 = 0, intf1 = 1,
    eimsk = 0x3d, int0 = 0, int1 = 1,
    gpior0 = 0x3e,
    eecr = 0x3f,
    eedr = 0x40,
    tccr0a = 0x44, wgm00 = 0, wgm01 = 1, com0b0 = 4, com0b1 = 5, com0a0 = 6, com0a1 = 7,
    tccr0b = 0x45, cs00 = 0, cs01 = 1, cs02 = 2, wgm02 = 3, foc0b = 6, foc0a = 7,
    tcnt0 = 0x46,
    ocr0a = 0x47,
    ocr0b = 0x48,
    spcr = 0x4c, spr0 = 0, spr1 = 1, cpha = 2, cpol = 3, mstr = 4, dord = 5, spe = 6, spie = 7,
    spsr = 0x4d, spi2x = 0, wcol = 6, spif = 7,
    spdr = 0x4e,
    acsr = 0x50,
    smcr = 0x53,
    mcusr = 0x54,
    spmcsr = 0x57,
    wdtcsr = 0x60,
    clkpr = 0x61,
    pcicr = 0x68,
    eicra = 0x69, isc00 = 0, isc01 = 1, isc10 = 2, isc11 = 3,
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
    ocr1a = 0x88, ocr1al = 0x88, ocr1ah = 0x89,
    ocr1b = 0x8a, ocr1bl = 0x8a, ocr1bh = 0x8b,
    tccr2a = 0xb0, wgm20 = 0, wgm21 = 1, com2b0 = 4, com2b1 = 5, com2a0 = 6, com2a1 = 7,
    tccr2b = 0xb1, cs20 = 0, cs21 = 1, wgm22 = 3, foc2b = 6, foc2a = 7,
    tcnt2 = 0xb2,
    ocr2a = 0xb3,
    ocr2b = 0xb4,
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
    miso_bit = pb4;

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
    * const p_tifr0 = (volatile uint8_t * const)tifr0,
    * const p_tifr1 = (volatile uint8_t * const)tifr1,
    * const p_tifr2 = (volatile uint8_t * const)tifr2,
    * const p_eimsk = (volatile uint8_t * const)eimsk,
    * const p_tcnt0 = (volatile uint8_t * const)tcnt0,
    * const p_ocr0a = (volatile uint8_t * const)ocr0a,
    * const p_ocr0b = (volatile uint8_t * const)ocr0b,
    * const p_tccr0a = (volatile uint8_t * const)tccr0a,
    * const p_tccr0b = (volatile uint8_t * const)tccr0b,
    * const p_spcr = (volatile uint8_t * const)spcr,
    * const p_spsr = (volatile uint8_t * const)spsr,
    * const p_spdr = (volatile uint8_t * const)spdr,
    * const p_eicra = (volatile uint8_t * const)eicra,
    * const p_timsk0 = (volatile uint8_t * const)timsk0,
    * const p_timsk1 = (volatile uint8_t * const)timsk1,
    * const p_timsk2 = (volatile uint8_t * const)timsk2,
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
    * const p_ucsr0a = (volatile uint8_t * const)ucsr0a,
    * const p_ucsr0b = (volatile uint8_t * const)ucsr0b,
    * const p_ucsr0c = (volatile uint8_t * const)ucsr0c,
    * const p_ubrr0l = (volatile uint8_t * const)ubrr0l,
    * const p_ubrr0h = (volatile uint8_t * const)ubrr0h,
    * const p_udr0 = (volatile uint8_t * const)udr0,

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
    * const p_miso_port = (volatile uint8_t * const)miso_port;

#endif


