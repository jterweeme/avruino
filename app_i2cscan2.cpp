#include "misc.h"
#include "busby.h"

class App
{
    USB busby;
    I2CBus bus;
    void delay() { for (volatile uint32_t i = 0; i < 0x4ffff; i++) { } }
public:
    App();
    int run();
};

App::App()
{
    busby.puts("Start I2C Scanner\r\n");
    bus.scan();
}

int App::run()
{
    while (true)
    {
        busby.puts("Start I2C Scanner\r\n");

        for (vector<uint8_t>::iterator it = bus.slaves.begin(); it < bus.slaves.end(); it++)
            busby.printf("0x%x\r\n", *it);

        delay();
    }

    return 0;
}

int main()
{
    App app;
    return app.run();
}


