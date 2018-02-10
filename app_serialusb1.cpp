#include "board.h"
#include "stream.h"
#include <avr/io.h>
#include "uart.h"

class MyUart : public DefaultUart
{
    ostream * const _os;
public:
    MyUart(ostream *os) : _os(os) { enableRead(); enableReadInterrupt(); }
    void onReceive();
};

class App
{
    CDC _cdc;
    USBStream cout;
    MyUart _uart;
public:
    App() : cout(&_cdc), _uart(&cout) { DDRC = 1 << 7; }
    int run();
};

void MyUart::onReceive()
{
    PORTC ^= 1 << 7;
    _os->write(readByte());
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


