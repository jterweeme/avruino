//#include "board.h"
#include "misc.h"
#include "busby.h"

class App
{
    USB busby;
    DefaultUart uart;
    Wifi wifi;
public:
    App();
    int run();
};

App::App() : wifi(&uart)
{
    uart.enableRead();
    busby.puts("Wifi Test 1");
}

int App::run()
{
    while (true)
    {
        busby.puts("Dinges\r\n");
        Utility::delay(0x4ffff);
    }

    return 0;
}


int main()
{
    App app;
    return app.run();
}


