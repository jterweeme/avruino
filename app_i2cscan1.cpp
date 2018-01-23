/*
I2C bus scanner
*/

#include "i2c.h"
#include "board.h"
#include "stream.h"
#include <stdio.h>

int main()
{
#if defined (__AVR_ATmega32U4__)
    CDC cdc;
    USBStream os(&cdc);
#else
    DefaultUart uart;
    UartStream os(&uart);
#endif
    I2CBus bus;

    while (true)
    {
        bus.scan();

        for (vector<uint8_t>::iterator it = bus.slaves.begin(); it < bus.slaves.end(); it++)
        {
            char buf[50];
            ::snprintf(buf, 50, "0x%x\r\n", *it);
            os.writeString(buf);
            os.flush();
        }

        Utility::delay(0xfffff);
    }

    return 0;
}


