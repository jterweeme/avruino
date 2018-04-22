#include "uip_client.h"
#include "stream.h"
#include "dhcp.h"
#include "dns.h"

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

static UIPEthernetClass eth;

int main()
{
    DefaultUart s;
    *p_ucsr9a |= 1<<u2x9;
    *p_ubrr9 = 16;
    UartStream cout(&s);
    *p_tccr0b = cs02; // | CS00;
    *p_timsk0 |= 1<<toie0;
    zei();

    cout << "Initialize Ethernet\r\n";
    cout.flush();
    uint8_t mac[6] = {0,1,2,3,4,5};
    eth.init(mac);

    cout << "Starting DHCP\r\n";
    cout.flush();
    DhcpClass dhcp(&eth);
    dhcp.beginWithDHCP(mac);
    eth.configure(dhcp.getLocalIp(), dhcp.getDnsServerIp(), dhcp.getGw(), dhcp.getSubnetMask());
    eth.addresses(cout);
    cout << "\r\n";

    DNSClient dns(&eth);
    dns.begin(0x08080808);
    uint32_t remote_addr;
    int ret = dns.getHostByName("www.astron.nl", remote_addr);

    if (ret == 1)
    {
        hex32(remote_addr, cout);
        cout << "\r\n";
    }
    else
    {
        cout << "Failed\r\n";
    }

    while (true)
    {
    }
#if 0
    UIPClient client(&eth);

    while (true)
    {
        if (client.connect("astron.nl", 80))
        {
            cout << "connected\r\n";
            client.write("GET / HTTP/1.1\r\n");
            client.write("Host: astron.nl\r\n");
            client.write("Connection: close\r\n\r\n");
            break;
        }
        else
        {
            cout << "Connection failed\r\n";

            for (volatile uint32_t i = 0; i < 0x1ffff; i++);    // delay
            continue;
        }
    }

    while (true)
    {
        if (client.available())
        {
            char c = client.read();
            cout.put(c);
        }
    }
#endif
    return 0;
}

extern "C" void TIMER0_OVF __attribute__ ((signal, used, externally_visible));
void TIMER0_OVF
{
    eth.tick2();
}



