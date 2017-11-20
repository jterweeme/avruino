#include <avr/io.h>
#include "uno.h"
#include "analog.h"

class App : public Board
{
    LCD lcd;
    DefaultUart uart;
    Analog analog;
    DFKeyPad dfk;
public:
    App() : lcd(&pin8, &pin9, &pin4, &pin5, &pin6, &pin7), dfk(analog)  { }
    int run();
};

int App::run()
{
    uart.puts("Hello");

    while (true)
    {
        switch (dfk.poll())
        {
        case DFKeyPad::SELECT:
            lcd.clear();
            lcd.puts("\rSelect");
            uart.puts("Select\r\n");
            break;
        case DFKeyPad::UP:
            lcd.clear();
            lcd.puts("\rUp");
            uart.puts("Up\r\n");
            break;
        case DFKeyPad::LEFT:
            lcd.clear();
            lcd.puts("\rLeft");
            uart.puts("Left\r\n");
            break;
        case DFKeyPad::RIGHT:
            lcd.clear();
            lcd.puts("\rRight");
            uart.puts("Right\r\n");
            break;
        case DFKeyPad::DOWN:
            lcd.clear();
            lcd.puts("\rDown");
            uart.puts("Down\r\n");
            break;
        }
    }

    return 0;
}

int main()
{
    App app;
    return app.run();
}

