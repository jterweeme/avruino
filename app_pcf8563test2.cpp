#include "board.h"

#if BOARD == HELIOS
#include "busby.h"
#endif

class App : public Board
{
    I2CBus bus;
#if BOARD == HELIOS
    USB uart;
#else
    DefaultUart uart;
#endif
    PCF8563 klok;
public:
    App();
    void test(Terminal *terminal) { terminal->puts("test\r\n"); }
    inline void delay(const uint32_t x) { Utility::delay(x); }
    int run();
};

App::App() : klok(&bus)
{
    uart.puts("Starting\r\n");
    pin13.direction(OUTPUT);
}

int App::run()
{
    while (true)
    {
        pin13.toggle();
        klok.gettime(&uart);
        delay(0x1ffff);
    }

    return 0;
}

int main()
{
    App app;
    return app.run();
}



