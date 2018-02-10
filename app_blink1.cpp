/*
LED: D13
*/

#include "board.h"

int main()
{
    *p_pin13_ddr |= 1<<pin13_bit;

    while (true)
    {
        *p_pin13_port ^= 1<<pin13_bit;

        for (volatile uint32_t i = 0; i < 0x1ffff; i++) // delay
            ;
    }
        
    return 0;
}


