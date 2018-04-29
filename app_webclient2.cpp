#include "w5100client.h"
#include "w5100udp.h"
#include "dhcp.h"
#include "stream.h"
#include "dns.h"
#include "board.h"

W5100Class *g_w5100;
ostream *gout;

int main()
{
    DefaultUart s;
    *p_ucsr9a |= 1<<u2x9;
    *p_ubrr9 = 16;
    UartStream cout(&s);
    gout = &cout;
    zei();

    cout << "Initialize Ethernet\r\n";
    cout.flush();
    W5100Class w5100;
    g_w5100 = &w5100;
    w5100.init();
    uint8_t mac[6] = {0,1,2,3,4,5};
    w5100.setMACAddress(mac);
    w5100.setIPAddress(0);

    cout << "Starting DHCP...\r\n";
    cout.flush();
    EthernetClass eth(&w5100);
    W5100UDP udp(&eth);
    DhcpClass dhcp(&udp);
    dhcp.beginWithDHCP(mac);
    w5100.setIPAddress(dhcp.localIp());
    w5100.setGatewayIp(dhcp.gateway());
    w5100.setSubnetMask(dhcp.subnetMask2());
    eth.addresses(cout);
    cout << "\r\n";

    W5100UDP udp2(&eth);
    DNSClient dns(&udp2);
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

    EthernetClient client(&eth);
    
    if (client.connect(remote_addr, 80))
    {
        cout << "connected\r\n";
        client << "GET / HTTP/1.1\r\n";
        client << "Host: astron.nl\r\n";
        client << "Connection: close\r\n\r\n";
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



