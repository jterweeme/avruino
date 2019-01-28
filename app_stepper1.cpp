/*
Stepper motor experiment
*/

#include "board.h"

class Stepper
{
private:
    uint8_t *_coil1_port, *_coil2_port, *_coil3_port, *_coil4_port;
    uint8_t _coil1_pin, _coil2_pin, _coil3_pin, _coil4_pin;
public:
    Stepper(uint8_t *coil1_port, uint8_t coil1_pin,
            uint8_t *coil2_port, uint8_t coil2_pin,
            uint8_t *coil3_port, uint8_t coil3_pin,
            uint8_t *coil4_port, uint8_t coil4_pin);
    void step(uint16_t n);
};

Stepper::Stepper(uint8_t *coil1_port, uint8_t coil1_pin,
        uint8_t *coil2_port, uint8_t coil2_pin,
        uint8_t *coil3_port, uint8_t coil3_pin,
        uint8_t *coil4_port, uint8_t coil4_pin)
{
    _coil1_port = coil1_port, _coil1_pin = coil1_pin;
    _coil2_port = coil2_port, _coil2_pin = coil2_pin;
    _coil3_port = coil3_port, _coil3_pin = coil3_pin;
    _coil4_port = coil4_port, _coil4_pin = coil4_pin;
}

void Stepper::step(uint16_t n)
{
    
}

int main()
{
    *p_pin8_ddr |= 1<<pin8_bit;
    *p_pin9_ddr |= 1<<pin9_bit;
    *p_pin10_ddr |= 1<<pin10_bit;
    *p_pin11_ddr |= 1<<pin11_bit;

    Stepper stepper(p_pin8_port, pin8_bit, p_pin9_port, pin9_bit, p_pin10_port, pin10_bit,
                p_pin11_port, pin11_bit);
    
    while (true)
    {
    }
    return 0;
}


