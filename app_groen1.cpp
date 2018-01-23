#include <avr/io.h>
class SPI
{
public:
    void init();
    void transfer(uint8_t);
};

void SPI::init()
{
    PORTB |= (1<<PB0);
    DDRB |= (1<<PB0) | (1<<PB1) | (1<<PB2);
    SPCR = (1<<SPE) | (1<<MSTR);
}

void SPI::transfer(uint8_t foo)
{
    SPDR = foo;

    while (!(SPSR & ((1<<SPIF)))) {
    }
}

int main()
{
    DDRF |= 1<<4 | 1<<5;
    PORTF &= ~(1<<4);
    PORTF &= ~(1<<5);
    SPI spi;
    spi.init();
    spi.transfer(1<<5);
    spi.transfer(20);
    PORTF |= 1<<4;
    PORTF &= ~(1<<4);

    while (true)
        ;

    return 0;
}


