/*
Loop on standard UART using polling method
*/

#include "board.h"

class App
{
    DefaultUart uart;
public:
    App();
    int run();
};

App::App()
{
    uart.enableRead();
    uart.puts("Testbericht\r\n");
}

int App::run()
{
    while (true)
        uart.myPutc(uart.readByte());

    return 0;       // never reach this
}

int main()
{
    App app;
    return app.run();
}


