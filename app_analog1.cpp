#include <avr/io.h>
#include "uno.h"

class App : public Board
{
    LCD lcd;
    DefaultUart uart;
public:
    App() : lcd(&pin8, &pin9, &pin4, &pin5, &pin6, &pin7)  { }
    int run();
};

int App::run()
{
    ADMUX = 0x00 | (1<<REFS0);
    ADCSRA = (1<<ADPS1) | (1<<ADPS0) | (1<<ADEN) | (1<<ADATE);
    ADCSRB &= ~(1<<ADTS2) | (1<<ADTS1) | (1<<ADTS0);
    ADCSRA |= (1<<ADSC);
    uart.puts("Hello");

    while (true)
    {
        unsigned x = ADCL | ((unsigned)ADCH << 8);
        uart.printf("%u\r\n", x);
        Utility::delay(0x1ffff);
        //lcd.printf("Hello");
        //uart.printf("Doet ie het?\r\n");
    }

    return 0;
}

int main()
{
    App app;
    return app.run();
}

