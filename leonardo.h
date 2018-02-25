#ifndef _LEONARDO_H_
#define _LEONARDO_H_

#ifndef F_CPU
#define F_CPU 16000000
#endif

#include "misc.h"
#include "timer.h"

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
            pinc6 = 6, pinc7 = 7,
        ddrc = portc_base + 1,
            ddc6 = 6, ddc7 = 7,
        portc = portc_base + 2,
            portc6 = 6, portc7 = 7,
            pc6 = 6, pc7 = 7,
    portd_base = 0x29,
        pind = portd_base,
        ddrd = portd_base + 1,
        portd = portd_base + 2,
            portd0 = 0, portd1 = 1, portd2 = 2, portd3 = 3,
            portd4 = 4, portd5 = 5, portd6 = 6, portd7 = 7,
            pd0 = 0, pd1 = 1, pd2 = 2, pd3 = 3, pd4 = 4, pd5 = 5, pd6 = 6, pd7 = 7,
    porte_base = 0x2c,
        pine = porte_base,
        ddre = porte_base + 1,
        porte = porte_base + 2,
            porte2 = 2, porte6 = 6,
            pe2 = 2, pe6 = 6,
    portf_base = 0x2f,
        pinf = portf_base,
        ddrf = portf_base + 1,
        portf = portf_base + 2,
            portf0 = 0, portf1 = 1, portf4 = 4, portf5 = 5, portf6 = 6, portf7 = 7,
            pf0 = 0, pf1 = 1, pf4 = 4, pf5 = 5, pf6 = 6, pf7 = 7,
    tifr0 = 0x35, tov0 = 0, ocf0a = 1, ocf0b = 2,
    tifr1 = 0x36, tov1 = 0, ocf1a = 1, ocf1b = 2, ocf1c = 3, icf1 = 5,
    tifr3 = 0x38,
    tifr4 = 0x39,
    tifr5 = 0x3a,
    tccr0a = 0x44, wgm00 = 0, wgm01 = 1, com0b0 = 4, com0b1 = 5, com0a0 = 6, com0a1 = 7,
    tccr0b = 0x45, cs00 = 0, cs01 = 1, cs02 = 2, wgm02 = 3, foc0b = 6, foc0a = 7,
    tcnt0 = 0x46,
    ocr0a = 0x47,
    ocr0b = 0x48,
    pllcsr = 0x49, plock = 0, plle = 1, pindiv = 4,
    gpior1 = 0x4a,
    gpior2 = 0x4b,
    spcr = 0x4c, spr0 = 0, spr1 = 1, cpha = 2, cpol = 3, mstr = 4, dord = 5, spe = 6, spie = 7,
    spsr = 0x4d, spi2x = 0, wcol = 6, spif = 7,
    spdr = 0x4e,
    pllfrq = 0x52,
        pdiv0 = 0, pdiv1 = 1, pdiv2 = 2, pdiv3 = 3,
        plltm0 = 4, plltm1 = 5, pllusb = 6, pinmux = 7,
    mcusr = 0x54,
    mcucr = 0x55,
    timsk0 = 0x6e, toie0 = 0, ocie0a = 1, ocie0b = 2,
    timsk1 = 0x6f, toie1 = 0, ocie1a = 1, ocie1b = 2, ocie1c = 3, icie1 = 5,
    timsk3 = 0x71, toie3 = 0, ocie3a = 1, ocie3b = 2, ocie3c = 3, icie3 = 5,
    timsk4 = 0x72, toie4 = 2, ocie4b = 5, ocie4a = 6, ocie4d = 7,
    adcl = 0x78,
    adch = 0x79,
    adcsra = 0x7a,
        adps0 = 0, adps1 = 1, adps2 = 2, adie = 3, adif = 4, adate = 5, adsc = 6, aden = 7,
    adcsrb = 0x7b,
    admux = 0x7c,
        mux0 = 0, mux1 = 1, mux2 = 2, mux3 = 3, mux4 = 4, adlar = 5, refs0 = 6, refs1 = 7,
    tccr1a = 0x80,
        wgm10 = 0, wgm11 = 1, com1c0 = 2, com1c1 = 3,
        com1b0 = 4, com1b1 = 5, com1a0 = 6, com1a1 = 7,
    tccr1b = 0x81, cs10 = 0, cs11 = 1, cs12 = 2, wgm12 = 3, wgm13 = 4, ines1 = 6, icnc1 = 7,
    tccr1c = 0x82, foc1c = 5, foc1b = 6, foc1a = 7,
    tcnt1 = 0x84,
    tcnt1l = 0x84,
    tcnt1h = 0x85,
    icr1 = 0x86,
    icr1l = 0x86,
    icr1h = 0x87,
    ocr1a = 0x88,
    ocr1al = 0x88,
    ocr1ah = 0x89,
    ocr1b = 0x8a,
    ocr1bl = 0x8a,
    ocr1bh = 0x8b,
    ocr1c = 0x8c,
    ocr1cl = 0x8c,
    ocr1ch = 0x8d,
    tccr3a = 0x90,
        wgm30 = 0, wgm31 = 1, com3c0 = 2, com3c1 = 3,
        com3b0 = 4, com3b1 = 5, com3a0 = 6, com3a1 = 7,
    tccr3b = 0x91, cs30 = 0, cs31 = 1, cs32 = 2, wgm32 = 3, wgm33 = 4, ices3 = 6, icnc3 = 7,
    tccr3c = 0x92,
    tcnt3 = 0x94,
    tcnt3l = 0x94,
    tcnt3h = 0x95,
    icr3l = 0x96,
    icr3h = 0x97,
    ocr3a = 0x98,
    ocr3al = 0x98,
    ocr3ah = 0x99,
    ocr3b = 0x9a,
    ocr3bl = 0x9a,
    ocr3bh = 0x9b,
    twbr = 0xb8,
    twsr = 0xb9,
    twar = 0xba,
    ucsr1a = 0xc8,
        mpcm1 = 0, u2x1 = 1, upe1 = 2, dor1 = 3, fe1 = 4, udre1 = 5, txc1 = 6, rxc1 = 7,
    ucsr1b = 0xc9,
        txb81 = 0, rxb81 = 1, ucsz12 = 2, txen1 = 3,
        rxen1 = 4, udrie1 = 5, txcie1 = 6, rxcie1 = 7,
    ucsr1c = 0xca,
    ubrr1 = 0xcc,
    ubrr1l = 0xcc,
    ubrr1h = 0xcd,
    udr1 = 0xce,
    uhwcon = 0xd7, uvrege = 0,
    usbcon = 0xd8, vbuste = 0, otgpade = 4, frzclk = 5, usbe = 7,
    usbsta = 0xd9, vbus = 0, speed = 3,
    usbint = 0xda, vbusti = 0,
    udcon = 0xe0, detach = 0, rmwkup = 1, lsm = 2, rstcpu = 3,
    udint = 0xe1, suspi = 0, sofi = 2, eorsti = 3, wakeupi = 4, eorsmi = 5, uprsmi = 6,
    udien = 0xe2, suspe = 0, sofe = 2, eorste = 3, wakeupe = 4, eorshe = 5, uprsme = 6,
    udaddr = 0xe3,
        adden = 7,
    udfnum = 0xe4,
    udfnuml = 0xe4,
    udfnumh = 0xe5,
    udmfn = 0xe6,
    ueintx = 0xe8,
        txini = 0, stalledi = 1, rxouti = 2, rxstpi = 3,
        nakouti = 4, rwal = 5, nakini = 6, fifocon = 7,
    uenum = 0xe9,
    uerst = 0xea,
    ueconx = 0xeb, epen = 0, rstdt = 3, stallrqc = 4, stallrq = 5,
    uecfg0x = 0xec, epdir = 0, eptype0 = 6, eptype1 = 7,
    uecfg1x = 0xed, alloc = 1, epbk0 = 2, epbk1 = 3, epsize0 = 4, epsize1 = 5, epsize2 = 6,
    ueienx = 0xf0,
        txine = 0, stallede = 1, rxoute = 2, rxstpe = 3, nakoute = 4, nakine = 6, flerre = 7,
    uedatx = 0xf1,
    ueint = 0xf4,

    ocr1a_port_base = portb_base,
    ocr1a_ddr = ocr1a_port_base + 1,
    ocr1a_port = ocr1a_port_base + 2,
    ocr1a_bit = pb5,
    ocr1b_port_base = portb_base,
    ocr1b_ddr = ocr1b_port_base + 1,
    ocr1b_port = ocr1b_port_base + 2,
    ocr1b_bit = pb6,
    ss_port_base = portb_base,
    ss_ddr = ss_port_base + 1,
    ss_port = ss_port_base + 2,
    pss = pb0,
    sck_port_base = portb_base,
    sck_ddr = sck_port_base + 1,
    sck_port = sck_port_base + 2,
    psck = pb1,
    mosi_port_base = portb_base,
    mosi_ddr = mosi_port_base + 1,
    mosi_port = mosi_port_base + 2,
    pmosi = pb2,
    miso_port_base = portb_base,
    miso_ddr = miso_port_base + 1,
    miso_port = miso_port_base + 2,
    pmiso = pb3,

    pin0_base = portd_base,
    pin0_bit = pd2,
    pin1_base = portd_base,
    pin1_bit = pd3,
    pin2_base = portd_base,
    pin2_bit = pd1,
    pin3_base = portd_base,
    pin3_bit = pd0,
    pin4_base = portd_base,
    pin4_bit = pd4,
    pin5_base = portc_base,
    pin5_bit = pc6,
    pin6_base = portd_base,
    pin6_bit = pd7,
    pin7_base = porte_base,
    pin7_bit = pe6,
    pin8_base = portb_base,
    pin8_bit = pb4,
    pin9_base = portb_base,
    pin9_bit = pb5,
    pin10_base = portb_base,
    pin10_bit = pb6,
    pin11_base = portb_base,
    pin11_bit = pb7,
    pin12_base = portd_base,
    pin12_bit = pd6,
    pin13_base = portc_base,
    pin13_ddr = pin13_base + 1,
    pin13_port = pin13_base + 2,
    pin13_bit = pc7,
    pinA0_base = portf_base,
    pinA0_bit = pf7,
    pinA1_base = portf_base,
    pinA1_bit = pf6,
    pinA2_base = portf_base,
    pinA2_bit = pf5,
    pinA3_base = portf_base,
    pinA3_bit = pf4,
    pinA4_base = portf_base,
    pinA4_bit = pf1,
    pinA5_base = portf_base,
    pinA5_bit = pf0;

static volatile uint8_t
    * const p_pinb = (volatile uint8_t * const)pinb,
    * const p_ddrb = (volatile uint8_t * const)ddrb,
    * const p_portb = (volatile uint8_t * const)portb,
    * const p_pinc = (volatile uint8_t * const)pinc,
    * const p_ddrc = (volatile uint8_t * const)ddrc,
    * const p_portc = (volatile uint8_t * const)portc,
    * const p_ddrd = (volatile uint8_t * const)ddrd,
    * const p_portd = (volatile uint8_t * const)portd,
    * const p_pinf = (volatile uint8_t * const)pinf,
    * const p_ddrf = (volatile uint8_t * const)ddrf,
    * const p_portf = (volatile uint8_t * const)portf,
    * const p_tifr0 = (volatile uint8_t * const)tifr0,
    * const p_tifr1 = (volatile uint8_t * const)tifr1,
    * const p_tifr3 = (volatile uint8_t * const)tifr3,
    * const p_tifr4 = (volatile uint8_t * const)tifr4,
    * const p_tifr5 = (volatile uint8_t * const)tifr5,
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
    * const p_pllfrq = (volatile uint8_t * const)pllfrq,
    * const p_timsk0 = (volatile uint8_t * const)timsk0,
    * const p_timsk1 = (volatile uint8_t * const)timsk1,
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
    * const p_tccr3a = (volatile uint8_t * const)tccr3a,
    * const p_tccr3b = (volatile uint8_t * const)tccr3b,
    * const p_tccr3c = (volatile uint8_t * const)tccr3c,
    * const p_ocr3a = (volatile uint8_t * const)ocr3a,
    * const p_ocr3b = (volatile uint8_t * const)ocr3b,
    * const p_twbr = (volatile uint8_t * const)twbr,
    * const p_twsr = (volatile uint8_t * const)twsr,
    * const p_twar = (volatile uint8_t * const)twar,
    * const p_ucsr1a = (volatile uint8_t * const)ucsr1a,
    * const p_ucsr1b = (volatile uint8_t * const)ucsr1b,
    * const p_udr1 = (volatile uint8_t * const)udr1,
    * const p_uhwcon = (volatile uint8_t * const)uhwcon,
    * const p_usbcon = (volatile uint8_t * const)usbcon,
    * const p_usbsta = (volatile uint8_t * const)usbsta,
    * const p_usbint = (volatile uint8_t * const)usbint,
    * const p_udcon = (volatile uint8_t * const)udcon,
    * const p_udint = (volatile uint8_t * const)udint,
    * const p_udien = (volatile uint8_t * const)udien,
    * const p_udaddr = (volatile uint8_t * const)udaddr,
    * const p_udfnuml = (volatile uint8_t * const)udfnuml,
    * const p_ueintx = (volatile uint8_t * const)ueintx,
    * const p_uenum = (volatile uint8_t * const)uenum,
    * const p_uerst = (volatile uint8_t * const)uerst,
    * const p_ueconx = (volatile uint8_t * const)ueconx,
    * const p_uecfg0x = (volatile uint8_t * const)uecfg0x,
    * const p_uecfg1x = (volatile uint8_t * const)uecfg1x,
    * const p_ueienx = (volatile uint8_t * const)ueienx,
    * const p_uedatx = (volatile uint8_t * const)uedatx,
    * const p_ueint = (volatile uint8_t * const)ueint,

    * const p_ddr_ss = (volatile uint8_t * const)ss_ddr,
    * const p_port_ss = (volatile uint8_t * const)ss_port,
    * const p_ddr_sck = (volatile uint8_t * const)sck_ddr,
    * const p_port_sck = (volatile uint8_t * const)sck_port,
    * const p_ddr_mosi = (volatile uint8_t * const)mosi_ddr,
    * const p_port_mosi = (volatile uint8_t * const)mosi_port,
    * const p_ddr_miso = (volatile uint8_t * const)miso_ddr,
    * const p_port_miso = (volatile uint8_t * const)miso_port,

    * const p_pin0_base = (volatile uint8_t * const)pin0_base,
    * const p_pin1_base = (volatile uint8_t * const)pin1_base,
    * const p_pin2_base = (volatile uint8_t * const)pin2_base,
    * const p_pin3_base = (volatile uint8_t * const)pin3_base,
    * const p_pin4_base = (volatile uint8_t * const)pin4_base,
    * const p_pin5_base = (volatile uint8_t * const)pin5_base,
    * const p_pin6_base = (volatile uint8_t * const)pin6_base,
    * const p_pin7_base = (volatile uint8_t * const)pin7_base,
    * const p_pin8_base = (volatile uint8_t * const)pin8_base,
    * const p_pin9_base = (volatile uint8_t * const)pin9_base,
    * const p_pin10_base = (volatile uint8_t * const)pin10_base,
    * const p_pin11_base = (volatile uint8_t * const)pin11_base,
    * const p_pin12_base = (volatile uint8_t * const)pin12_base,
    * const p_pin13_base = (volatile uint8_t * const)pin13_base,
    * const p_pin13_ddr = (volatile uint8_t * const)pin13_ddr,
    * const p_pin13_port = (volatile uint8_t * const)pin13_port,
    * const p_pinA0_base = (volatile uint8_t * const)pinA0_base,
    * const p_pinA1_base = (volatile uint8_t * const)pinA1_base,
    * const p_pinA2_base = (volatile uint8_t * const)pinA2_base,
    * const p_pinA3_base = (volatile uint8_t * const)pinA3_base,
    * const p_pinA4_base = (volatile uint8_t * const)pinA4_base,
    * const p_pinA5_base = (volatile uint8_t * const)pinA5_base;

static volatile uint16_t
    * const p_ubrr1 = (volatile uint16_t * const)ubrr1,
    * const p_udfnum = (volatile uint16_t * const)udfnum;

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

class SPIBus : public SPIBase
{
public:
    SPIBus();
};

struct Board
{
    Port
        portB { (uint8_t *)portb_base },
        portC { (uint8_t *)portc_base },
        portD { (uint8_t *)portd_base },
        portE { (uint8_t *)porte_base },
        portF { (uint8_t *)portf_base };

    Pin
        pin0 { portD, BIT2 },
        pin1 { portD, BIT3 },
        pin2 { portD, BIT1 },
        pin3 { portD, BIT0 },
        pin4 { portD, BIT4 },
        pin5 { portC, BIT6 },
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

#define USB_GEN __vector_10()
#define USB_COM __vector_11()
#define TIMER1_CAPT __vector_16()
#define TIMER1_COMPA __vector_17()
#define TIMER0_COMPA __vector_21()
#define TIMER0_COMPB __vector_22()
#define TIMER0_OVF __vector_23()
#endif


