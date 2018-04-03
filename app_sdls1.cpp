/*
directory listing sd card FAT formatted

UNO: CS <-> D9

werkt met UNO
*/

#include "fatty.h"
#include "misc.h"
#include "stream.h"

static Fatty *g_zd;

static void printDirectory(Fyle dir, uint8_t numTabs, ostream &os)
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
            os.put(nibble(size >> 28 & 0xf));
            os.put(nibble(size >> 24 & 0xf));
            os.put(nibble(size >> 20 & 0xf));
            os.put(nibble(size >> 16 & 0xf));
            os.put(nibble(size >> 12 & 0xf));
            os.put(nibble(size >> 8 & 0xf));
            os.put(nibble(size >> 4 & 0xf));
            os.put(nibble(size & 0xf));
            os.writeString("\r\n");
        }
        entry.close();
    }
}

int main()
{
    Board b;
    Sd2Card sd(&b.pin9);
    Fatty zd(&sd);
    g_zd = &zd;
    *p_tccr0b = 1<<cs02;
    *p_timsk0 |= 1<<toie0;
    zei();
#if defined (__AVR_ATmega32U4__)
    CDC cdc;
    USBStream cout(&cdc);
#else
    DefaultUart s;
    UartStream cout(&s);
#endif
    cout << "\r\nAttempt to open SD card\r\n";
    cout.flush();
    bool ret = zd.begin();

    if (!ret)
    {
        cout << "initialization failed!\r\n";
        return 0;
    }

    cout << "start\r\n";
    Fyle myFile = zd.open("/");
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


