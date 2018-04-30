/*
this is a comment
I love comments
*/

#include "w5100client.h"
#include "w5100udp.h"
#include "dhcp.h"
#include "board.h"
#include "stream.h"
#include "webtest1.h"

static W5100Class w5100;
static EthernetClass eth(&w5100);
W5100Class *g_w5100 = &w5100;
ostream *gout;

int main()
{
    zei();
#if defined (__AVR_ATmega32U4__)
    CDC cdc;
    USBStream cout(&cdc);
#else
    DefaultUart s;
    *p_ucsr9a |= 1<<u2x9;
    *p_ubrr9 = 16;
    UartStream cout(&s);
#endif
    gout = &cout;

    cout << "Initialize Ethernet...\r\n";
    cout.flush();
    w5100.init();
    uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05};
    w5100.setMACAddress(mac);
    w5100.setIPAddress(0);
    
    cout << "Starting DHCP...\r\n";
    cout.flush();
    W5100UDP udp(&eth);
    DhcpClass dhcp(&udp);
    dhcp.beginWithDHCP(mac);
    w5100.setIPAddress(dhcp.localIp());
    w5100.setGatewayIp(dhcp.gateway());
    w5100.setSubnetMask(dhcp.subnetMask2());
    eth.addresses(cout);
    cout << "\r\n";
    cout.flush();

    EthernetServer server = EthernetServer(&eth, 80);
    server.begin();
    cout << "Server started\r\n";
    cout.flush();
    WebTest1 web(&cout);

    while (true)
    {
        EthernetClient client = server.available();
        web.dispatch(client);
    }

    return 0;
}

#if defined (__AVR_ATmega32U4__)
extern "C" void USB_COM __attribute__ ((signal, used, externally_visible));
void USB_COM
{
    USB::instance->com();
}

extern "C" void USB_GEN __attribute__ ((signal, used, externally_visible));
void USB_GEN
{
    USB::instance->gen();
}
#endif


