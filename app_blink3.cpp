#include "board.h"

int main()
{
    Board b;
    b.pin13.direction(OUTPUT);

    while (true)
    {
        b.pin13.toggle();
        
        for (volatile uint32_t i = 0; i < 0x1ffff; i++)
            ;
    }
    return 0;;
}


