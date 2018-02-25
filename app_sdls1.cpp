/*
directory listing sd card FAT formatted

works
*/

#include "fatty.h"
#include "misc.h"
#include <avr/interrupt.h>
#include <stdio.h>
#include "stream.h"
#include "int.h"

static ZD *g_zd;

IZR(TIMER0_OVF_vect)
{
    g_zd->tick();
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
            char buf[50];
            ::snprintf(buf, 50, "%lu\r\n", entry.size());
            os.writeString(buf);
        }
        entry.close();
    }
}

int main()
{
    *p_tccr0b = 1<<cs02;
    *p_timsk0 |= 1<<toie0;
    sei();
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



