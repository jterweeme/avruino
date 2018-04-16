#include "uip_server.h"
#include "stream.h"
#include "dhcp.h"

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

static UIPEthernetClass eth;

static inline char convertCase(char c)
{
    if (my_isupper(c))
        return c + 32;

    if (my_islower(c))
        return c - 32;

    return c;
}

int main()
{
    UIPServer server = UIPServer(&eth, 23);
    *p_tccr0b = cs02; // | CS00;
    *p_timsk0 |= 1<<toie0;
    zei();
    DefaultUart s;
    *p_ucsr9a |= 1<<u2x9;
    *p_ubrr9 = 16;
    UartStream cout(&s);

    cout << "Initialize Ethernet\r\n";
    cout.flush();
    uint8_t mac[6] = {0,1,2,3,4,5};
    eth.init(mac);

    cout << "Starting DHCP\r\n";
    cout.flush();
    DhcpClass dhcp(&eth);
    dhcp.beginWithDHCP(mac);
    eth.configure(dhcp.getLocalIp(), dhcp.getDnsServerIp(), dhcp.getGw(), dhcp.getSubnetMask());

    server.begin();

    while (true)
    {
        UIPClient client = server.available();

        if (client)
        {
            cout << "Client connected\r\n";

            if (client.connected() && client.available())
            {
                client.write("\e[1;32m");

                for (uint8_t i = 0; i < 80; i++)
                    client.write('A');
            }

            while (client.connected())
            {
                if (client.available())
                {

                    char c = client.read();
                    cout.put(convertCase(c));
                    server.write(convertCase(c));
                }
            }

            for (volatile uint16_t i = 0; i < 0x4fff; i++); // delay
            client.stop();
            cout << "Client disconnected\r\n";
        }
    }
    return 0;
}

extern "C" void TIMER0_OVF __attribute__ ((signal, used, externally_visible));
void TIMER0_OVF
{
    eth.tick2();
}



