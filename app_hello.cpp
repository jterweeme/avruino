/*
Prints hello on the default UART
*/

#include "board.h"

class App
{
    DefaultUart uart;
public:
    App();
};

App::App()
{
    uart.puts("Hello whatever\r\n");
    uart.printf("PrintF%u %s\r\n", 123, "test");
}

int main()
{
    App app;
    return 0;
}


