#include "i2c.h"
#include "stream.h"
#include "board.h"
#include "misc.h"
#include <stdio.h>

int main()
{
    I2CBus i2c;
    PCF8563 klok(&i2c);
#if defined (__AVR_ATmega32U4__)
    CDC cdc;
    USBStream cout(&cdc);
#else
    DefaultUart uart;
    UartStream cout(&uart);
#endif

    while (true)
    {
        klok.readTime();
        char buf[50];
        ::snprintf(buf, 50, "%u:%u:%u\r\n", klok.hour(), klok.min(), klok.sec());
        cout << buf;
        cout.flush();
        Utility::delay(0x4ffff);
    }

    return 0;
}



