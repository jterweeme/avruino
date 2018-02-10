#include "analog.h"
#include "misc.h"
#include <stdio.h>
#include "uart.h"

int main()
{
    DefaultUart s;
    Analog2 a;
    a.init();
    TouchScreen ts(&a);

    while (true)
    {
        TSPoint p = ts.getPoint();
        char buf[50];
        snprintf(buf, 50, "%u %u %u\r\n", p.x, p.y, p.z);
        s.write(buf);
    }

    return 0;
}


