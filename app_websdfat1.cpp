/*
Mega eth CS: D53
Mega SD CS: D9

Webserver op enc28j60, gebruikt index.html op FAT geformatteerd SD kaart
*/

#include "enc28j60tcp.h"
#include "enc28j60udp.h"
#include "dhcp.h"
#include "fatty.h"
#include "webserver.h"
#include "board.h"
#include <avr/io.h>

static UIPEthernetClass eth;
static Fatty *g_zd;
ostream *gout;

int main()
{
    // 16,000,000/16,000 = 1000
    // 16,000 / 256 = 62

    UIPServer server = UIPServer(&eth, 80);
    Board b;
    Sd2Card sd(&b.pin9);
    Fatty zd(&sd);
    g_zd = &zd;
    TCCR0B = CS02; // | CS00;
    TIMSK0 |= 1<<TOIE0;
    zei();
    DefaultUart s;
    *p_ucsr9a |= 1<<u2x9;
    *p_ubrr9 = 16;
    UartStream cout(&s);
    gout = &cout;

    cout << "Initialize SD Card...\r\n";
    bool ret = zd.begin();

    if (!ret)
    {
        cout << "init failed!\r\n";
        return 0;
    }

    cout << "SD Card initialized successful\r\n";

    cout << "Initialize Ethernet...\r\n";
    cout.flush();
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

    server.begin();
    Webserver web(&zd, &cout);

    while (true)
    {
        UIPClient client = server.available();
        web.dispatch(client);
    }

    return 0;
}

extern "C" void TIMER0_OVF __attribute__ ((signal, used, externally_visible));
void TIMER0_OVF
{
    eth.tick2();
}




