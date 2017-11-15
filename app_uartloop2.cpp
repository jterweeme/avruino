#include "board.h"
#include <avr/io.h>

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
    {
    }

    return 0;
}

int main()
{
    App app;
    return app.run();
}


