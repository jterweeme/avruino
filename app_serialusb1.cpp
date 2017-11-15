#include "helios.h"
#include "busby.h"
#include <avr/io.h>

class MyUart : public DefaultUart
{
    USB * const usb;
public:
    MyUart(USB *usb) : usb(usb)
    {
        enableRead();
        enableReadInterrupt();
    }

    void onReceive();
};

class App
{
    USB usb;
    MyUart uart;
public:
    App() : uart(&usb) { DDRC = 1 << 7; }
    int run();
};

void MyUart::onReceive()
{
    PORTC ^= 1 << 7;
    usb->myPutc(readByte());
}

int App::run()
{
    while (true) { }
    return 0;
}

int main()
{
    App app;
    return app.run();
}


