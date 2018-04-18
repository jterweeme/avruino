
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <util/delay.h>
#include "EthernetClient.h"
#include "EthernetServer.h"
#include "misc.h"
#include "board.h"
#include "stream.h"
#include <avr/interrupt.h>
#include "util.h"
#include "dhcp2.h"

static W5100Class w5100;
static EthernetClass eth(&w5100);
static uint8_t g_count = 0;
W5100Class *g_w5100 = &w5100;

int main()
{
    // 16,000,000/16,000 = 1000
    // 16,000 / 256 = 62

    EthernetServer server = EthernetServer(&eth, 80);
    *p_tccr0b = 1<<cs02; // | CS00;
    *p_timsk0 |= 1<<toie0;
    zei();
    DefaultUart s;
    *p_ucsr9a |= 1<<u2x9;
    *p_ubrr9 = 16;
    UartStream cout(&s);

    cout << "Initialize Ethernet...\r\n";
    cout.flush();
#if 1
    w5100.init();
    uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05};
    w5100.setMACAddress(mac);
    w5100.setIPAddress(IPAddress(0,0,0,0).raw_address());
    
    cout << "Starting DHCP...\r\n";
    DhcpClass dhcp(&eth);
    dhcp.beginWithDHCP(mac);
    w5100.setIPAddress(dhcp.getLocalIp().raw_address());
    w5100.setGatewayIp(dhcp.getGatewayIp().raw_address());
    w5100.setSubnetMask(dhcp.getSubnetMask().raw_address());
    eth.addresses(cout);
    cout << "\r\n";

    server.begin();
    cout << "Server started\r\n";

    while (true)
    {
        EthernetClient client = server.available();

        if (client)
        {
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
#endif
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
    eth.tick2();
}
#endif


