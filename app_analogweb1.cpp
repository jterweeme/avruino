/*
Mega eth CS: D53

werkt met mega
*/

#include "uip_ethernet.h"
#include "uip_server.h"
#include "uip_client.h"
#include "misc.h"
#include "board.h"
#include <stdio.h>

#define F_CPU 16000000UL
#include <util/delay.h>

static uint32_t g_millis = 0;

extern "C" void TIMER0_OVF __attribute__ ((signal, used, externally_visible));
void TIMER0_OVF
{
    g_millis++;
}

uint32_t millis()
{
    return g_millis;
}

static inline char nibble(uint8_t n)
{
    return n <= 9 ? '0' + n : 'A' + n - 10;
}

static uint8_t g_count = 0;

int main()
{
    // 16,000,000/16,000 = 1000
    // 16,000 / 256 = 62
    UIPServer server = UIPServer(80);
    *p_tccr0b = 1<<cs02; // | CS00;
    *p_timsk0 |= 1<<toie0;
    zei();

    uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05};
    IPAddrezz myIP(192,168,178,32);
    UIPEthernet.begin(mac, myIP);
    uint32_t ip = UIPEthernet.localIP();
    server.begin();

    while (true)
    {
        UIPClient client = server.available();

        if (client)
        {
            bool currentLineIsBlank = true;
        
            while (client.connected())
            {
                if (client.available())
                {
                    char c = client.read();

                    if (c == '\n' && currentLineIsBlank)
                    {
                        client.write("HTTP/1.1 200 OK\r\n");
                        client.write("Content-Type: text/html\r\n");
                        client.write("Connection: close\r\n");
                        client.write("Refresh: 5\r\n\r\n");
                        client.write("<!DOCTYPE HTML>\r\n");
                        client.write("<html>\r\n");

                        for (int analogChannel = 0; analogChannel < 6; analogChannel++)
                        {
                            client.write("analog input ");
                            char buf[100];
                            snprintf(buf, 100, "%u", analogChannel);
                            client.write(buf);
                            client.write(" is ");
                            snprintf(buf, 100, "%u", g_count++);
                            client.write(buf);
                            client.write("<br />\r\n");
                        }

                        client.write("</html>\r\n");
                        break;
                    }

                    if (c == '\n')
                        currentLineIsBlank = true;
                    else if (c != '\r')
                        currentLineIsBlank = false;
                }
            }

            _delay_ms(1);
            client.stop();
        }
    }

    return 0;
}





