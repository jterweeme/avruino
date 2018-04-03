/*
I2C bus scanner
*/

#include "i2c.h"
#include "board.h"
#include "stream.h"

int main()
{
#if defined (__AVR_ATmega32U4__)
    CDC cdc;
    USBStream cout(&cdc);
#else
    DefaultUart uart;
    UartStream cout(&uart);
#endif
    I2CBus bus;

    while (true)
    {
        bus.scan();

        for (vector<uint8_t>::iterator it = bus.slaves.begin(); it < bus.slaves.end(); it++)
        {
            cout << "0x";
            cout.put(nibble(*it >> 4 & 0xf));
            cout.put(nibble(*it & 0xf));
            cout << "\r\n";
            cout.flush();
        }

        for (volatile uint32_t i = 0; i < 0xfffff; i++);
    }

    return 0;
}


