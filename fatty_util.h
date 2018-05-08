#ifndef _FATTY_UTIL_H_
#define _FATTY_UTIL_H_
#include "fatty.h"

static void printDirectory(Fyle dir, int numTabs, ostream &os)
{
    while (true)
    {
        Fyle entry = dir.openNextFile();

        if (!entry)
            break;

        for (uint8_t i = 0; i < numTabs; i++)
            os << "\t";

        if (entry.isDirectory())
            os.put('[');

        os << entry.name();

        if (entry.isDirectory())
            os << "]\r\n";

        if (entry.isDirectory())
        {
#if 0
            os.put('/');
            printDirectory(entry, numTabs + 1, os);
#endif
        }
        else
        {
            for (uint8_t i = my_strlen(entry.name()); i < 15; i++)
                os.put(' ');

            //os << "\t\t";
            uint32_t size = entry.size();
            hex32(size, os);
            os << "\r\n";
        }
        entry.close();
    }
}

static void cat(istream &is, ostream &os)
{
    for (int c; (c = is.get()) != -1;)
    {
        if (c == 0x0a)
            os.put(0x0d);

        os.put(c);
    }
}

static void more(istream &ctrl, istream &is, ostream &os)
{
    uint16_t line = 0, col = 0;

    for (int c; (c = is.get()) != -1;)
    {
        if (c == '<' || c == '&')
            os << "\e[1;32m";

        if (c == 0x0a)
        {
            os << "\r\n";
            line++;
            col = 0;

            if (line > 50)
            {
                line = 0;
                os << "-- More --\r\n";
                char c2 = ctrl.get();

                while (c2 == 0xff)
                    c2 = ctrl.get();
#if 1
                os.put(nibble(c2 >> 4 & 0xf));
                os.put(nibble(c2 & 0xf));
#endif
                if (c2 == 'q')
                    return;
            }

            continue;
        }

        if (col > 95)
        {
            col = 0;
            line++;
            os << "\r\n";
        }

        os.put(c);
        col++;

        if (c == '>' || c == ';')
            os << "\e[0m";
    }
}

static void od(istream &is, ostream &os)
{
    bool available = true;
    uint8_t buf[16];
    uint8_t cnt = 0;

    while (available)
    {
        for (uint8_t i = 0; i < 16; i++)
        {
            int c = is.get();

            if (c == -1)
            {
                available = false;
                cnt = i;
                break;
            }

            buf[i] = c;
            cnt = 16;
        }

        for (uint8_t i = 0; i < cnt; i++)
        {
            os.put(nibble(buf[i] >> 4 & 0xf));
            os.put(nibble(buf[i] & 0xf));
            os.put(' ');
        }

        os << " >";

        for (uint8_t i = 0; i < cnt; i++)
        {
            if (my_isprint(buf[i]))
                os.put(buf[i]);
            else
                os.put('.');
        }

        os << "<\r\n";
    }
}
#endif



