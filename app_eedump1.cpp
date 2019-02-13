/*
hexdump van EEProm naar UART @9600 baud
*/

#include <avr/eeprom.h>
#include <avr/sleep.h>
#include <ctype.h>
#include "misc.h"
#include "uart.h"
#include "stream.h"

class EEBlock
{
    uint8_t ebuf[128];
public:
    void read(const void *eeprom_addr);
    void hexDump(ostream &os) const;
};

void EEBlock::read(const void *eeprom_addr)
{
    eeprom_read_block(ebuf, eeprom_addr, 128);
}

void EEBlock::hexDump(ostream &os) const
{
    for (uint8_t i = 0; i < 128; i += 16)
    {
        for (uint8_t j = 0; j < 16; j++)
        {
            os.put(nibble(ebuf[i + j] >> 4 & 0xf));
            os.put(nibble(ebuf[i + j] & 0xf));
            os.put(' ');
        }

        os << " >";

        for (uint8_t j = 0; j < 16; j++)
        {
            if (isprint(ebuf[i + j]))
                os.put(ebuf[i + j]);
            else
                os.put('.');
        }

        os << "<\r\n";
    }   
}

int main()
{
    DefaultUart s;
    UartStream cout(&s);
    EEBlock eeblock;

    for (uint16_t i = 0; i < 1024; i += 128)
    {
        eeblock.read((const void *)i);
        eeblock.hexDump(cout);
    }
    cout.flush();

    while (true)
        sleep_mode();

    return 0;
}


