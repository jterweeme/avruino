/*
Mega eth CS: D53/SS
Uno eth CS: D10/SS

werkt met mega
*/

#include "uip_server.h"
#include "misc.h"
#include "board.h"
#include "stream.h"

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <util/delay.h>

static UIPEthernetClass eth;
static uint8_t g_count = 0;

int main()
{
    // 16,000,000/16,000 = 1000
    // 16,000 / 256 = 62

    DefaultUart s;
    *p_ucsr0a |= 1<<u2x0;
    *p_ubrr0 = 16;
    UartStream cout(&s);
    UIPServer server = UIPServer(&eth, 80);
    *p_tccr0b = 1<<cs02; // | CS00;
    *p_timsk0 |= 1<<toie0;
    zei();
    uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05};
    cout << "Contacting DHCP\r\n";
    IPAddrezz myIP(192,168,178,32);
    eth.begin(mac, myIP); // init via DHCP
    uint32_t ip = eth.localIP();
    cout << "IP: ";
    hex32(ip, cout);
    cout << "\r\n";
    server.begin();
    cout << "Server started\r\n";

    while (true)
    {
        UIPClient client = server.available();

        if (client)
        {
            cout << "Client\r\n";
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

                        for (uint8_t analogChannel = 0; analogChannel < 6; analogChannel++)
                        {
                            client.write("analog input ");
                            client.write(nibble(analogChannel & 0xf));
                            client.write(" is ");
                            uint8_t count = g_count++;
                            client.write(nibble(count >> 4 & 0xf));
                            client.write(nibble(count & 0xf));
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

extern "C" void TIMER0_OVF __attribute__ ((signal, used, externally_visible));
void TIMER0_OVF
{
    eth.tick2();
}



