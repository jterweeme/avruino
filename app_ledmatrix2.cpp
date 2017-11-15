#include "board.h"

uint8_t tekens[][5] =
{
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
    {0x06, 0x49, 0x49, 0x29, 0x1e}, // 9
    {0x7e, 0x11, 0x11, 0x11, 0x7e}, // A
    {0x7f, 0x49, 0x49, 0x49, 0x36}, // B
    {0x3e, 0x41, 0x41, 0x41, 0x22}  // C
};

class App : public Board
{
    SPIBus spi;
public:
    void init();
    void dinges(int);
    int run();
    App();
};

App::App()
{
    pinA2.direction(OUTPUT);
    pinA3.direction(OUTPUT);
    pinA2.clear();
    pinA3.clear();
}

int App::run()
{
    while (true)
        dinges(3);

    return 0;
}

void App::dinges(int i)
{
    spi.transfer(1<<0);
    spi.transfer(tekens[i][0]);
    pinA3.set();
    pinA3.clear();
    spi.transfer(1<<1);
    spi.transfer(tekens[i][1]);
    pinA3.set();
    pinA3.clear();
    spi.transfer(1<<2);
    spi.transfer(tekens[i][2]);
    pinA3.set();
    pinA3.clear();
    spi.transfer(1<<3);
    spi.transfer(tekens[i][3]);
    pinA3.set();
    pinA3.clear();
    spi.transfer(1<<4);
    spi.transfer(tekens[i][4]);
    pinA3.set();
    pinA3.clear();
}

int main()
{
    App app;
    return app.run();
}



