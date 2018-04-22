#include "w5100client.h"
#include "stream.h"
#include "w5100dhcp.h"
#include "w5100eth.h"
#include "w5100dns.h"
#include "board.h"
#include "util.h"
#include <avr/interrupt.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

static W5100Class w5100;
static EthernetClass eth(&w5100);
W5100Class *g_w5100 = &w5100;
ostream *gout;

int main()
{
    DefaultUart s;
    *p_ucsr9a |= 1<<u2x9;
    *p_ubrr9 = 16;
    UartStream cout(&s);
    gout = &cout;
    *p_tccr0b = cs02; // | CS00;
    *p_timsk0 |= 1<<toie0;
    zei();

    cout << "Initialize Ethernet\r\n";
    cout.flush();
    w5100.init();
    uint8_t mac[6] = {0,1,2,3,4,5};
    w5100.setMACAddress(mac);
    w5100.setIPAddress(0);

    cout << "Starting DHCP...\r\n";
    cout.flush();
    DhcpClass dhcp(&eth);
    dhcp.beginWithDHCP(mac);
    w5100.setIPAddress(dhcp.localIp());
    w5100.setGatewayIp(dhcp.gateway());
    w5100.setSubnetMask(dhcp.subnetMask2());
    eth.addresses(cout);
    cout << "\r\n";
#if 1
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
#endif

    EthernetClient client(&eth);
    
    if (client.connect("www.astron.nl", 80))
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

#if 0
extern "C" void TIMER0_OVF __attribute__ ((signal, used, externally_visible));
void TIMER0_OVF
{
    //eth.tick2();
}
#endif



