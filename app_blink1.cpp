#include <avr/io.h>

void delay()
{
    for (volatile unsigned i = 0; i < 0xffff; i++) { }
}

int main()
{
    DDRB |= 1 << 7;

    while (true)
    {
        PORTB ^= 1 << 7;
        ::delay();
    }
        
    return 0;
}


