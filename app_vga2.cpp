#include "vga.h"
#include <avr/sleep.h>

class App
{
    VGA vga;
public:
    App() { }
    int run();
};

int App::run()
{
    while (true)
    {
        sleep_mode();
        vga.scanLine();
    }
    return 0;
}

int main()
{
    App app;
    return app.run();
}


