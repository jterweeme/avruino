#include "uno.h"
#include "uart.h"

class App
{
    DefaultUart uart;
public:
    App() { }
    int run();
};

int App::run()
{
    const char s1[] = "plankton";
    //const char s2[] = "plankton";
    uart.printf("Hello world %u\r\n", Utility::strlen(s1));
    while (true) { }
    return 0;
}

int main()
{
    App app;
    return app.run();
}



