#include "board.h"
#include <avr/io.h>

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
    {
        uart.myPutc(uart.readByte());
    }

    return 0;
}

int main()
{
    App app;
    return app.run();
}


