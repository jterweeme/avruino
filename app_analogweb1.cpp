/*
Mega eth CS: D53
*/

#include "uip_ethernet.h"
#include "uip_server.h"
#include "uip_client.h"
#include <avr/interrupt.h>
#include "misc.h"
#include <stdio.h>

#define F_CPU 16000000UL
#include <util/delay.h>

uint32_t g_millis = 0;

ISR(TIMER0_OVF_vect)
{
    g_millis++;
}

#if 0
uint32_t millis()
{
    return g_millis;
}
#endif

inline char nibble(uint8_t n)
{
    return n <= 9 ? '0' + n : 'A' + n - 10;
}

static uint8_t g_count = 0;

int main()
{
    // 16,000,000/16,000 = 1000
    // 16,000 / 256 = 62
    UIPServer server = UIPServer(80);
#if 0
    Serial serial;
    serial.init();
#endif
    TCCR0B = CS02; // | CS00;
    TIMSK0 |= 1<<TOIE0;
    sei();

    uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05};
#if 0
    serial.write("begin ethernet\r\n");
#endif
    IPAddrezz myIP(192,168,200,56);
    UIPEthernet.begin(mac, myIP);
#if 0
    serial.write("My IP address: \r\n");
#endif
    uint32_t ip = UIPEthernet.localIP();
    //ip = millis();
#if 0
    for (int8_t i = 7; i >= 0; i--)
        serial.write(nibble(ip >> (i << 2) & 0xf));

    serial.write("\r\n");
#endif
    server.begin();

    while (true)
    {
        UIPClient client = server.available();

        if (client)
        {
#if 0
            serial.write("new client");
#endif
            bool currentLineIsBlank = true;
        
            while (client.connected())
            {
                if (client.available())
                {
                    char c = client.read();
#if 0
                    serial.write(c);
#endif
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
                            int sensorReading = 100;
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





