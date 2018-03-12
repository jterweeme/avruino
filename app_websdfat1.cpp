/*
Mega eth CS: D53
*/

#include "uip_server.h"
#include "misc.h"
#include "fatty.h"
#include <stdio.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <util/delay.h>

class Buffer
{
    char _buf[512] = {0};
    uint16_t _pos = 0;
public:
    void push(char c) { _buf[_pos++] = c; }
    char *get() { return _buf; }
    void reset();
    bool end() const;
};

bool Buffer::end() const
{
    return _buf[_pos - 4] == 0x0d && _buf[_pos - 3] == 0x0a && _buf[_pos - 2] == 0x0d &&
        _buf[_pos - 1] == 0x0a ? true : false;
}

void Buffer::reset()
{
    _pos = 0;

    for (uint16_t i = 0; i < 512; i++)
        _buf[i] = 0;
}

static UIPEthernetClass eth;
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
    // 16,000,000/16,000 = 1000
    // 16,000 / 256 = 62

    UIPServer server = UIPServer(&eth, 80);
    //Fyle myFile;
    Board b;
    Sd2Card sd(&b.pin9);
    Fatty zd(&sd);
    g_zd = &zd;
    TCCR0B = CS02; // | CS00;
    TIMSK0 |= 1<<TOIE0;
    zei();
    uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05};
    IPAddrezz myIP(192,168,178,32);
    eth.begin(mac, myIP);
    server.begin();
    Buffer buffer;
    DefaultUart s;
    UartStream cout(&s);
    cout << "Startup\r\n";
    cout.flush();

    bool ret = zd.begin();

    if (!ret)
    {
        cout.writeString("init failed!\r\n");
        return 0;
    }

    while (true)
    {
        UIPClient client = server.available();

        if (client)
        {
            cout << "Client\r\n";
            cout.flush();

            while (client.connected())
            {
                if (client.available())
                {
                    char c = client.read();
                    buffer.push(c);

                    if (buffer.end())
                    {
                        if (strncmp("GET ", buffer.get(), 4) == 0)
                        {
                            char fn[100] = {0};
                            char ext[10] = {0};
                            
                            for (uint16_t i = 5; i < 512; i++)
                            {
                                if (buffer.get()[i] != ' ')
                                    fn[i - 5] = buffer.get()[i];
                                else
                                    break;
                            }
                            
                            if (fn[0] == 0)
                                strncpy(fn, "index.htm", 100);

                            size_t dot = 0;

                            for (uint8_t i = 0; i < strlen(fn); i++)
                                if (fn[i] == '.')
                                    dot = i;

                            for (char *p = fn + dot + 1, *d = ext; *p != 0; p++)
                                *d++ = *p;
                            
                            cout << fn;
                            cout << "\r\n";
                            cout.flush();
                            FyleIfstream ifs;
                            ifs.open(fn);
                            client.write("HTTP/1.1 200 OK\r\n");
                            cout << "HTTP/1.1 200 OK\r\n";
                            cout.flush();

                            if (strncmp(ext, "svg", 3) == 0)
                                client.write("Content-Type: image/svg+xml\r\n");
                            else
                                client.write("Content-Type: text/html\r\n");

                            client.write("Connection: close\r\n\r\n");  // let op de dubbel nl
                        
                            int c2;

                            while ((c2 = ifs.get()) != -1)
                                client.write(c2);

                            ifs.close();
                        }
                        else if (strncmp("PUT ", buffer.get(), 4) == 0)
                        {
#if 0
                            serial.write(buffer.get());
#endif
                        }

                        buffer.reset();
                        break;
                    }
                }
            }
            _delay_ms(1);
            client.stop();
        }
    }

    return 0;
}

extern "C" void TIMER0_OVF __attribute__ ((signal, used, externally_visible));
void TIMER0_OVF
{
    g_zd->tick();
    eth.tick2();
}




