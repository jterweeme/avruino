/*
Mega eth CS: #53/SS
Uno eth CS: #10/SS
*/

#include "enc28j60tcp.h"
#include "enc28j60udp.h"
#include "board.h"
#include "dhcp.h"
#include "webtest1.h"
#include "stream.h"

static Enc28J60IP *g_eth;
ostream *gout;

int main()
{
    // 16,000,000/16,000 = 1000
    // 16,000 / 256 = 62

    zei();
    DefaultUart s;
    UartStream cout(&s);
    gout = &cout;

    cout << "Initialize Ethernet...\r\n";
    cout.flush();
    Enc28J60Network nw;
    Enc28J60IP eth(&nw);
    g_eth = &eth;
    *p_tccr0b = 1<<cs02; // | CS00;
    *p_timsk0 |= 1<<toie0;
    uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05};
    eth.init(mac);

    cout << "Starting DHCP...\r\n";
    cout.flush();
    UIPUDP udp(&eth);
    DhcpClass dhcp(&udp);
    dhcp.beginWithDHCP(mac);
    eth.configure(dhcp.localIp(), dhcp.dnsServer(), dhcp.gateway(), dhcp.subnetMask2());
    eth.addresses(cout);
    cout << "\r\n";

    UIPServer server = UIPServer(&eth, 80);
    server.begin();
    cout << "Server started\r\n";
    WebTest1 webtest1(&cout);

    while (true)
    {
        UIPClient client = server.available();
        webtest1.dispatch(client);
    }

    return 0;
}

extern "C" void TIMER0_OVF __attribute__ ((signal, used, externally_visible));
void TIMER0_OVF
{
    g_eth->tick2();
}



