#define UNO 1
#define HELIOS 2

#ifndef BOARD
#define BOARD UNO
#endif

#if BOARD == UNO
#include "uno.h"
#elif BOARD == HELIOS
#include "helios.h"
#endif

#include "misc.h"
#include <avr/io.h>

class MyTimer0 : public Timer0
{
public:
    void onOverflow();
};

class MyTimer1 : public Timer1
{
public:
    void onOverflow();
};

class App
{
    DefaultUart uart;
    MyTimer0 timer0;
    MyTimer1 timer1;
public:
    App();
};

void MyTimer0::onOverflow()
{
    PORTB ^= (1<<5);
    *counter = 0;
}

void MyTimer1::onOverflow()
{
    DefaultUart::getInstance()->puts("Hello\r\n");
    *counter = 0;
}

App::App()
{
    uart.puts("Starting Timer1 app...\r\n");
    DDRB |= (1<<5);
}

int main()
{
    App app;

    while (true) {
    }

    return 0;
}


