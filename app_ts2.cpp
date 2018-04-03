#include "analog.h"
#include "misc.h"
#include "uart.h"
#include "stream.h"

void print16(uint16_t n, ostream &os)
{
    os.put(nibble(n >> 12 & 0xf));
    os.put(nibble(n >> 8 & 0xf));
    os.put(nibble(n >> 4 & 0xf));
    os.put(nibble(n & 0xf));
}

int main()
{
    DefaultUart s;
    UartStream cout(&s);
    Analog2 a;
    a.init();
    TouchScreen ts(&a);

    while (true)
    {
        TSPoint p = ts.getPoint();
        print16(p.x, cout);
        cout.put(' ');
        print16(p.y, cout);
        cout.put(' ');
        print16(p.z, cout);
        cout << "\r\n";
    }

    return 0;
}


