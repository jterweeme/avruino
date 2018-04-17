/*
Mega eth CS: D53/SS
Uno eth CS: D10/SS

werkt met mega
*/

#include "uip_server.h"
#include "misc.h"
#include "board.h"
#include "stream.h"
#include "dhcp.h"

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <util/delay.h>

static UIPEthernetClass eth;
static uint8_t g_count = 0;

static void addresses(UIPEthernetClass &eth, ostream &os)
{
    uint32_t ip = eth.localIP();
    uint32_t subnet = eth.subnetMask();
    uint32_t gw = eth.gatewayIP();
    uint32_t dns = eth.dnsServerIP();
    os << "IP:      ";
    hex32(ip, os);
    os << "\r\nSubnet:  ";
    hex32(subnet, os);
    os << "\r\nGateway: ";
    hex32(gw, os);
    os << "\r\nDNS:     ";
    hex32(dns, os);
    os << "\r\n";
}

int main()
{
    // 16,000,000/16,000 = 1000
    // 16,000 / 256 = 62

    DefaultUart s;
    *p_ucsr9a |= 1<<u2x9;
    *p_ubrr9 = 16;
    UartStream cout(&s);
    UIPServer server = UIPServer(&eth, 80);
    *p_tccr0b = 1<<cs02; // | CS00;
    *p_timsk0 |= 1<<toie0;
    zei();

    cout << "Initialize Ethernet...\r\n";
    cout.flush();
    uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05};
    eth.init(mac);

    cout << "Starting DHCP...\r\n";
    cout.flush();
    DhcpClass dhcp(&eth);
    dhcp.beginWithDHCP(mac);
    eth.configure(dhcp.getLocalIp(), dhcp.getDnsServerIp(), dhcp.getGw(), dhcp.getSubnetMask());
    addresses(eth, cout);
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



