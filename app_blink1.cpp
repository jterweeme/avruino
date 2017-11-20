#include <avr/io.h>

void delay()
{
    for (volatile unsigned i = 0; i < 0xffff; i++) { }
}

int main()
{
    DDRC |= 1 << 6;

    while (true)
    {
        PORTC ^= 1 << 6;
        ::delay();
    }
        
    return 0;
}


