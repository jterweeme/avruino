#include <avr/io.h>

uint8_t tekens[][5] =
{
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
    {0x06, 0x49, 0x49, 0x29, 0x1e}, // 9
    {0x7e, 0x11, 0x11, 0x11, 0x7e}, // A
    {0x7f, 0x49, 0x49, 0x49, 0x36}, // B
    {0x3e, 0x41, 0x41, 0x41, 0x22}  // C
};

class SPI
{
public:
    SPI() { }
    void init();
    void transfer(uint8_t);
};

class App
{
    SPI spi;
public:
    void init();
    void dinges(int);
    int run();
    App() { init(); }
};

void App::init()
{
    DDRF |= (1<<4) | (1<<5);
    PORTF &= ~(1<<4);
    PORTF &= ~(1<<5);
    spi.init();
}

int App::run()
{
    while (true)
        dinges(3);

    return 0;
}

void App::dinges(int i)
{
    spi.transfer(1<<0);
    spi.transfer(tekens[i][0]);
    PORTF |= (1<<4);
    PORTF &= ~(1<<4);
    spi.transfer(1<<1);
    spi.transfer(tekens[i][1]);
    PORTF |= (1<<4);
    PORTF &= ~(1<<4);
    spi.transfer(1<<2);
    spi.transfer(tekens[i][2]);
    PORTF |= (1<<4);
    PORTF &= ~(1<<4);
    spi.transfer(1<<3);
    spi.transfer(tekens[i][3]);
    PORTF |= (1<<4);
    PORTF &= ~(1<<4);
    spi.transfer(1<<4);
    spi.transfer(tekens[i][4]);
    PORTF |= (1<<4);
    PORTF &= ~(1<<4);
}

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
    App app;
    return app.run();
}



