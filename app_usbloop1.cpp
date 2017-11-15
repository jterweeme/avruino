#include "board.h"
#include "busby.h"
#include <avr/io.h>

class App
{
    USB uart;
public:
    App();
    int run();
};

App::App()
{
    uart.puts("Testbericht\r\n");
}

int App::run()
{

    while (true)
    {
        uint8_t byte = uart.readByte();
        
        if (byte != 255)
        {
            uart.myPutc(byte);
            uart.flush();
        }
    }

    return 0;
}

int main()
{
    App app;
    return app.run();
}


