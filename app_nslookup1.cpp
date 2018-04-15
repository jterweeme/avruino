#include "uip_client.h"
#include "stream.h"
#include "dhcp.h"

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

static UIPEthernetClass eth;

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
    addresses(eth, cout);
    cout << "\r\n";

    UIPClient client(&eth);
    
    if (client.connect("astron.nl", 80))
    {
        cout << "connected\r\n";
        client.write("GET / HTTP/1.1\r\n");
        client.write("Host: astron.nl\r\n");
        client.write("Connection: close\r\n\r\n");
    }
    else
    {
        cout << "Connection failed\r\n";
    }

    while (true)
    {
        if (client.available())
        {
            char c = client.read();
            cout.put(c);
        }
    }

    return 0;
}

extern "C" void TIMER0_OVF __attribute__ ((signal, used, externally_visible));
void TIMER0_OVF
{
    eth.tick2();
}



