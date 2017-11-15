#include "board.h"

class App
{
    DefaultUart uart;
    I2CBus bus;
public:
    App();
    int run();
};

App::App()
{
    uart.puts("Start I2C Scanner\r\n");
    bus.scan();
    
    for (vector<uint8_t>::iterator it = bus.slaves.begin(); it < bus.slaves.end(); it++)
        uart.printf("0x%x\r\n", *it);
}

int App::run()
{
    return 0;
}

int main()
{
    App app;
    return app.run();
}


