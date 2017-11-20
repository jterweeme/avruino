/*
Loop on default UART using interrupt method
*/

#include "board.h"
#include <avr/sleep.h>

class MyUart : public DefaultUart
{
    void onReceive();
};

class App
{
    MyUart uart;
public:
    App();
    int run();
};

void MyUart::onReceive()
{
    myPutc(readByte());
}

App::App()
{
    uart.enableRead();
    uart.enableReadInterrupt();
    uart.puts("Testbericht\r\n");
}

int App::run()
{
    while (true)
        sleep_mode();

    return 0;
}

int main()
{
    App app;
    return app.run();
}


