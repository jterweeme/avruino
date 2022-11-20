/*
ATTiny85

1/PB5/PCINT5/RESET/ADC0/dW
2/PB3/PCINT3/XTAL1/CLKI/OC1B/ADC3
3/PB4/PCINT4/XTAL2/CLKO/OC1B/ADC2
4/GND
5/PB0/MOSI/DI/SDA/AIN0/OC0A/OC1A/AREF/PCINT0
6/PB1/MISO/D0/AIN1/OC0B/OC1A/PCINT1
7/PB2/SCK/USCK/SCL/ADC1/T0/INT0/PCINT2
8/VCC

*/

#ifndef _ATTINY85_
#define _ATTINY85_
#include "mcu_tiny13.h"
#include "pinport.h"

static constexpr uint8_t
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


