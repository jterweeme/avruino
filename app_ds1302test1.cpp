#include "uno.h"
#include "stream.h"

class App : public Board
{
public:
    int run();
};

int App::run()
{
    DefaultUart terminal;
    UartStream cout(&terminal);
    LCD lcd(&pin8, &pin9, &pin4, &pin5, &pin6, &pin7);
    DS1302 klok(&pinA1, &pinA2, &pinA3);
    cout << "Startup DS1302 test\r\n";

    while (true)
    {
        klok.update();
        DS1302_Regs regs = klok.get();
        //terminal.printf("%u\r\n", regs.seconds);

        lcd.printf("\r%u%u:%u%u:%u%u",
                regs.h24.hour10,
                regs.h24.hour,
                regs.minutes10,
                regs.minutes,
                regs.seconds10,
                regs.seconds);
    }

    return 0;
}

int main()
{
    App app;
    return app.run();
}


