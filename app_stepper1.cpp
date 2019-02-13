/*
Stepper motor experiment
*/

#include "board.h"

#define F_CPU 1000000UL
#include <util/delay.h>

class Stepper
{
private:
    volatile uint8_t *_coil1_port, *_coil2_port, *_coil3_port, *_coil4_port;
    volatile uint8_t _coil1_pin, _coil2_pin, _coil3_pin, _coil4_pin;
    uint8_t _step = 0;
public:
    Stepper(volatile uint8_t *coil1_port, uint8_t coil1_pin,
            volatile uint8_t *coil2_port, uint8_t coil2_pin,
            volatile uint8_t *coil3_port, uint8_t coil3_pin,
            volatile uint8_t *coil4_port, uint8_t coil4_pin);
    void xstep(uint8_t n);
    void step();
    void step(uint16_t n);
};

Stepper::Stepper(volatile uint8_t *coil1_port, uint8_t coil1_pin,
        volatile uint8_t *coil2_port, uint8_t coil2_pin,
        volatile uint8_t *coil3_port, uint8_t coil3_pin,
        volatile uint8_t *coil4_port, uint8_t coil4_pin)
{
    _coil1_port = coil1_port, _coil1_pin = coil1_pin;
    _coil2_port = coil2_port, _coil2_pin = coil2_pin;
    _coil3_port = coil3_port, _coil3_pin = coil3_pin;
    _coil4_port = coil4_port, _coil4_pin = coil4_pin;
}

void Stepper::step(uint16_t n)
{
    while (n--)
    {
        step();
        _delay_ms(100);
    }
}

void Stepper::xstep(uint8_t n)
{
    switch (n)
    {
    case 0:
        *_coil1_port |= 1<<_coil1_pin;
        *_coil2_port &= ~(1<<_coil2_pin);
        *_coil3_port |= 1<<_coil3_pin;
        *_coil4_port &= ~(1<<_coil4_pin);
        break;
    case 1:
        *_coil1_port &= ~(1<<_coil1_pin);
        *_coil2_port |= 1<<_coil2_pin;
        *_coil3_port |= 1<<_coil3_pin;
        *_coil4_port &= ~(1<<_coil4_pin);
        break;
    case 2:
        *_coil1_port &= ~(1<<_coil1_pin);
        *_coil2_port |= 1<<_coil2_pin;
        *_coil3_port &= ~(1<<_coil3_pin);
        *_coil4_port |= 1<<_coil4_pin;
        break;
    case 3:
        *_coil1_port |= 1<<_coil1_pin;
        *_coil2_port &= ~(1<<_coil2_pin);
        *_coil3_port &= ~(1<<_coil3_pin);
        *_coil4_port |= 1<<_coil4_pin;
        break;
    }
}

void Stepper::step()
{
    xstep(_step);
    _step = (_step + 1) % 4; // rotate _step
}

int main()
{
    *p_pin0_ddr |= 1<<pin0_bit;
    *p_pin1_ddr |= 1<<pin1_bit;
    *p_pin2_ddr |= 1<<pin2_bit;
    *p_pin3_ddr |= 1<<pin3_bit;

    Stepper stepper(p_pin0_port, pin0_bit, p_pin1_port, pin1_bit, p_pin2_port, pin2_bit,
                p_pin3_port, pin3_bit);

    stepper.step(1000);
    
    while (true)
    {
    }
    return 0;
}


