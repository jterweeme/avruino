/*
LED: D13
*/

#include "board.h"

int main()
{
    //pin 13 output
    *p_pin13_ddr |= 1<<pin13_bit;

    while (true)
    {
        //toggle pin 13
        *p_pin13_port ^= 1<<pin13_bit;

        //delay
        for (volatile uint32_t i = 0; i < 0x1ffff; i++)
            continue;
    }
        
    return 0;
}


