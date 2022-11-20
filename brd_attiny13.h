/*
ATTiny13

pinout:

1/PB5/PCINT5/RESET/ADC0/dW
2/PB3/PCINT3/CLKI/ADC3
3/PB4/PCINT4/ADC2
4/GND
5/PB0/MOSI/AIN0/OC0A/PCINT0         D13
6/PB1/MISO/AIN1/OC0B/INT0/PCINT1
7/PB2/SCK/ADC1/T0/PCINT2
8/VCC

*/

#ifndef _ATTINY13_
#define _ATTINY13_
#include "pinport.h"

static constexpr uint8_t
    portb_base = 0x36,
        pinb = portb_base,
            pinb0 = 0, pinb1 = 1, pinb2 = 2, pinb3 = 3,
            pinb4 = 4, pinb5 = 5,
        ddrb = portb_base + 1,
            ddb0 = 0, ddb1 = 1, ddb2 = 2, ddb3 = 3, ddb4 = 4, ddb5 = 5, ddb6 = 6, ddb7 = 7,
        portb = portb_base + 2,

    ocr0b = 0x49,
    tccr0a = 0x4f, wgm00 = 0, wgm01 = 1, com0b0 = 4, com0b1 = 5, com0a0 = 6, com0a1 = 7,
    tcnt0 = 0x52,
    tccr0b = 0x53, cs00 = 0, cs01 = 1, cs02 = 2, wgm02 = 3, foc0b = 6, foc0a = 7,
    mcusr = 0x54, porf = 0, extrf = 1, borf = 2, wdrf = 3,
    mcucr = 0x55, isc00 = 0, isc01 = 1, sm0 = 3, sm1 = 4, se = 5, pud = 6,
    ocr0a = 0x56,
    tifr0 = 0x58, tov0 = 1, ocf0a = 2, ocf0b = 3,
    timsk0 = 0x59, toie0 = 1, ocie0a = 2, ocie0b = 3,
    gifr = 0x5a,
    gimsk = 0x5b,

    pin0_base = portb_base,
    pin0_in = pin0_base + 0,
    pin0_ddr = pin0_base + 1,
    pin0_port = pin0_base + 2,
    pin0_bit = 0,
    pin1_base = portb_base,
    pin1_in = pin1_base + 0,
    pin1_ddr = pin1_base + 1,
    pin1_port = pin1_base + 2,
    pin1_bit = 1,
    pin2_base = portb_base,
    pin2_in = pin2_base + 0,
    pin2_ddr = pin2_base + 1,
    pin2_port = pin2_base + 2,
    pin2_bit = 2,
    pin3_base = portb_base,
    pin3_in = pin3_base + 0,
    pin3_ddr = pin3_base + 1,
    pin3_port = pin3_base + 2,
    pin3_bit = 3,
    pin13_base = portb_base,
    pin13_in = pin13_base + 0,
    pin13_ddr = pin13_base + 1,
    pin13_port = pin13_base + 2,
    pin13_bit = 0;
    
static volatile uint8_t
    * const p_portb_base = (volatile uint8_t * const)portb_base,
    * const p_pinb = (volatile uint8_t * const)pinb,
    * const p_ddrb = (volatile uint8_t * const)ddrb,

    * const p_ocr0b = (volatile uint8_t * const)ocr0b,
    * const p_tccr0a = (volatile uint8_t * const)tccr0a,
    * const p_tcnt0 = (volatile uint8_t * const)tcnt0,
    * const p_tccr0b = (volatile uint8_t * const)tccr0b,
    * const p_ocr0a = (volatile uint8_t * const)ocr0a,
    * const p_tifr0 = (volatile uint8_t * const)tifr0,
    * const p_timsk0 = (volatile uint8_t * const)timsk0,

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
    * const p_pin13_base = (volatile uint8_t * const)pin13_base,
    * const p_pin13_ddr = (volatile uint8_t * const)pin13_ddr,
    * const p_pin13_port = (volatile uint8_t * const)pin13_port;


#endif


