#include "board.h"

int main()
{
    *p_pin13_ddr |= 1<<pin13_bit;
    *p_ocr0a_ddr |= 1<<ocr0a_bit;
    *p_tccr0a = 1<<com0a0 | 1<<com0a0;
    *p_tccr0b = 1<<foc0a | 1<<cs00 | 1<<cs01;
    *p_ocr0a = 200;

    while (true)
    {
        *p_tccr0b ^= 1<<wgm01;
        *p_pin13_port ^= 1<<pin13_bit;

        //delay
        for (volatile uint32_t i = 0; i < 0x8ffff; i++)
            ;
    }

    return 0;
}


