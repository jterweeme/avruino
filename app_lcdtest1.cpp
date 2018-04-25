/*
prints Lunar Lander 1976 on 1602LCD (from DFK Keyboard)
*/

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include "board.h"
#include "uart.h"
#include <util/delay.h>

class App : public Board
{
    LCD lcd;
public:
    App() : lcd(&pin8, &pin9, &pin4, &pin5, &pin6, &pin7)  { }
    int run();
};

int App::run()
{
    lcd << "Lunar Lander";
    lcd.lcd_write_instruction_4d(LCD::SETCURSOR | LCD::LINETWO);
    _delay_us(80);
    //lcd.puts("1989");
    lcd.printf("%u", 1976);
    while (true) { }
    return 0;
}

int main()
{
    App app;
    return app.run();
}

