/*
directory listing sd card FAT formatted

UNO: CS <-> D9

werkt met UNO
*/

#include "fatty.h"
#include "misc.h"
#include "stream.h"

static ZD *g_zd;

static inline char nibble(uint8_t n)
{
    return n <= 9 ? '0' + n : 'A' + n - 10;
}

void printDirectory(Fyle dir, int numTabs, ostream &os)
{
    while (true)
    {
        Fyle entry = dir.openNextFile();

        if (!entry)
            break;

        for (uint8_t i = 0; i < numTabs; i++)
            os.writeString("\t");

        os.writeString(entry.name());
        
        if (entry.isDirectory())
        {
            os.writeString("/");
            printDirectory(entry, numTabs + 1, os);
        }
        else
        {
            os.writeString("\t\t");
            uint32_t size = entry.size();
            os.write(nibble(size >> 28 & 0xf));
            os.write(nibble(size >> 24 & 0xf));
            os.write(nibble(size >> 20 & 0xf));
            os.write(nibble(size >> 16 & 0xf));
            os.write(nibble(size >> 12 & 0xf));
            os.write(nibble(size >> 8 & 0xf));
            os.write(nibble(size >> 4 & 0xf));
            os.write(nibble(size & 0xf));
            os.writeString("\r\n");
        }
        entry.close();
    }
}

int main()
{
    *p_tccr0b = 1<<cs02;
    *p_timsk0 |= 1<<toie0;
    zei();
    Fyle myFile;
#if defined (__AVR_ATmega32U4__)
    CDC cdc;
    USBStream cout(&cdc);
#else
    DefaultUart s;
    UartStream cout(&s);
#endif
    ZD zd;
    g_zd = &zd;

#if 0
    for (uint32_t i = 0; i < 0xff; i++)
    {
        for (uint32_t j = 0; j < 50; j++)
            cout.write('.');
        
        cout.writeString("\r\n");
    }
#endif

    cout.writeString("\r\nAttempt to open SD card\r\n");
    cout.flush();
    bool ret = zd.begin();

    if (!ret)
    {
        cout.writeString("initialization failed!\r\n");
        return 0;
    }

    cout.writeString("start\r\n");
    myFile = zd.open("/");
    printDirectory(myFile, 0, cout);

    while (true)
        ;

    return 0;
}

extern "C" void TIMER0_OVF __attribute__ ((signal, used, externally_visible));
void TIMER0_OVF
{
    g_zd->tick();
}


