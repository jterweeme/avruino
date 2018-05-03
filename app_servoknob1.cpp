#include "board.h"

static constexpr uint8_t CLK1 = 1, CLK8 = 2, CLK64 = 3, CLK256 = 4, CLK1024 = 5;

int main()
{
    *p_ocr1a_ddr |= 1<<ocr1a_bit;
    *p_tccr1a = 1<<com1a0;
    *p_tccr1b = 1<<wgm12 | CLK1;
    *p_ocr1a = 0x5555;

    while (true)
    {
        
    }
    return 0;
}



