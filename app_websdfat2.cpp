/*
Mega eth CS: D53
Mega SD CS: D9

Webserver op w5100, gebruikt index.html op FAT geformatteerd SD kaart
*/

#include <avr/interrupt.h>
#include "util.h"
#include "w5100dhcp.h"
#include "webserver.h"
#include "w5100server.h"
#include "w5100client.h"
#include "fatty.h"

static W5100Class w5100;
static EthernetClass eth(&w5100);
W5100Class *g_w5100 = &w5100;
ostream *gout;

int main()
{
    // 16,000,000/16,000 = 1000
    // 16,000 / 256 = 62

    Board b;
    Sd2Card sd(&b.pin4);
    Fatty zd(&sd);
    TCCR0B = CS02; // | CS00;
    TIMSK0 |= 1<<TOIE0;
    zei();
    DefaultUart s;
    *p_ucsr9a |= 1<<u2x9;
    *p_ubrr9 = 16;
    UartStream cout(&s);
    gout = &cout;

    cout << "Initialize Ethernet...\r\n";
    cout.flush();
    w5100.init();
    uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05};
    w5100.setMACAddress(mac);
    w5100.setIPAddress(0);

    cout << "Starting DHCP...\r\n";
    DhcpClass dhcp(&eth);
    dhcp.beginWithDHCP(mac);
    w5100.setIPAddress(dhcp.localIp());
    w5100.setGatewayIp(dhcp.gateway());
    w5100.setSubnetMask(dhcp.subnetMask2());
    eth.addresses(cout);
    cout << "DHCP:    ";
    hex32(dhcp.dhcpServerIp(), cout);
    cout << "\r\n";

    EthernetServer server = EthernetServer(&eth, 80);
    server.begin();
    cout << "Initializing SD card...\r\n";
    bool ret = zd.begin();

    if (!ret)
    {
        cout << "init failed!\r\n";
        return 0;
    }

    cout << "SD Card initialized successful\r\n";
    Webserver web(&zd, &cout);

    while (true)
    {
        EthernetClient client = server.available();
        web.dispatch(client);
    }

    return 0;
}

#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 * 256))

#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)

#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)

volatile unsigned long timer0_overflow_count = 0;
volatile unsigned long timer0_millis = 0;
static unsigned char timer0_fract = 0;

ISR(TIMER0_OVF_vect)
{
    unsigned long m = timer0_millis;
    unsigned char f = timer0_fract;

    m += MILLIS_INC;
    f += FRACT_INC;
    if (f >= FRACT_MAX) {
        f -= FRACT_MAX;
        m += 1;
    }

    timer0_fract = f;
    timer0_millis = m;
    timer0_overflow_count++;
}

unsigned long millis()
{
    unsigned long m;
    uint8_t oldSREG = SREG;
    cli();
    m = timer0_millis;
    SREG = oldSREG;

    return m;
}




