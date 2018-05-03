//#include "uno.h"
#include "board.h"
#include "analog.h"
#include "stream.h"
#include "lcd.h"

class App : public Board
{
public:
    int run();
};

int App::run()
{
    LCD lcd(&pin8, &pin9, &pin4, &pin5, &pin6, &pin7);
    DefaultUart uart;
    UartStream cout(&uart);
    Analog analog;
    DFKeyPad dfk(analog);
    cout << "Hello";

    while (true)
    {
        switch (dfk.poll())
        {
        case DFKeyPad::SELECT:
            lcd.clear();
            lcd.puts("\rSelect");
            cout << "Select\r\n";
            break;
        case DFKeyPad::UP:
            lcd.clear();
            lcd.puts("\rUp");
            cout << "Up\r\n";
            break;
        case DFKeyPad::LEFT:
            lcd.clear();
            lcd.puts("\rLeft");
            cout << "Left\r\n";
            break;
        case DFKeyPad::RIGHT:
            lcd.clear();
            lcd.puts("\rRight");
            cout << "Right\r\n";
            break;
        case DFKeyPad::DOWN:
            lcd.clear();
            lcd.puts("\rDown");
            cout << "Down\r\n";
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

