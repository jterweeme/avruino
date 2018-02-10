/*
werkt niet
*/

#include "board.h"
#include "uart.h"
#include "timer.h"

class MyTimer0 : public Timer0
{
    Port &port;
public:
    MyTimer0(Port &port) : port(port) { }
    void onOverflow();
};

class MyTimer1 : public Timer1
{
    Pin &pin;
public:
    MyTimer1(Pin &pin) : pin(pin) { }
    void onOverflow();
};

class App : public Board
{
    DefaultUart uart;
    MyTimer0 timer0;
    MyTimer1 timer1;
public:
    App();
    int run();
};

int App::run()
{
    while (true)
    {
        pin13.toggle();
        Utility::delay(7500);
    }

    return 0;
}

void MyTimer0::onOverflow()
{
    *counter = 0;
}

void MyTimer1::onOverflow()
{
    DefaultUart::getInstance()->puts("Hello\r\n");
    pin.toggle();
    *counter = 0;
}

App::App() : timer0(portC), timer1(pin13)
{
    uart.puts("Starting Timer1 app...\r\n");
    pin13.direction(OUTPUT);
}

int main()
{
    App app;

    while (true) {
    }

    return 0;
}


