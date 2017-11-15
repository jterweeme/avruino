#include "uno.h"

class App : public Board
{
    DefaultUart terminal;
    LCD lcd;
    DS1302 klok;
public:
    App();
    int run();
};

App::App() : lcd(&pin8, &pin9, &pin4, &pin5, &pin6, &pin7), klok(&pinA1, &pinA2, &pinA3)
{
    terminal.puts("Startup DS1302 test\r\n");
}

int App::run()
{
    while (true)
    {
        klok.update();
        DS1302_Regs regs = klok.get();
        terminal.printf("%u\r\n", regs.seconds);

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


