#include "board.h"

#if BOARD == HELIOS
#include "busby.h"
#endif

class MyTimer1 : public Timer0
{
    PCF8563 *klok;
    Terminal *terminal;
    Pin &pin;
public:
    MyTimer1(PCF8563 *klok, Terminal *terminal, Pin &pin)
      :
        klok(klok), terminal(terminal), pin(pin)
    { }

    void onOverflow();
};

class App : public Board
{
    I2CBus bus;
#if BOARD == HELIOS
    USB uart;
#else
    DefaultUart uart;
#endif
    PCF8563 klok;
    MyTimer1 timer1;
public:
    App();
    void test(Terminal *terminal) { terminal->puts("test\r\n"); }
    int run();
};

void MyTimer1::onOverflow()
{
    terminal->puts("timer\r\n");
    pin.toggle();
}

App::App()
  :
    klok(&bus),
    timer1(&klok, &uart, pin13)
{
    //uart.puts("Starting PCF8563 test...\r\n");
    uart.puts("Starting\r\n");
    pin13.direction(OUTPUT);
}

int App::run()
{
    while (true)
    {
        //uart.puts("Onzin\r\n");
        test(&uart);
    }
    return 0;
}

int main()
{
    App app;
    return app.run();
}

