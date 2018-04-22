/*
 DHCP Chat  Server
 
 A simple server that distributes any incoming messages to all
 connected clients.  To use telnet to  your device's IP address and type.
 You can see the client's input in the serial monitor as well.
 Using an Arduino Wiznet Ethernet shield. 
 
 THis version attempts to get an IP address using DHCP
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 
 created 21 May 2011
 modified 9 Apr 2012
 by Tom Igoe
 Based on ChatServer example by David A. Mellis
 
 */

#include "w5100client.h"
#include "w5100server.h"
#include <avr/interrupt.h>
#include "util.h"
#include "w5100dhcp.h"
#include "board.h"
#include "stream.h"

W5100Class *g_w5100;
ostream *gout;

int main()
{
    TCCR0A |= 1<<WGM01 | 1<<WGM00;
    TCCR0B |= 1<<CS01 | 1<<CS00;
    TIMSK0 |= 1<<TOIE0;
    sei();
    DefaultUart s;
    *p_ucsr9a |= 1<<u2x9;
    *p_ubrr9 = 16;
    UartStream cout(&s);
    gout = &cout;

    W5100Class w5100;
    g_w5100 = &w5100;
    EthernetClass eth(&w5100);

    cout << "Initialize Ethernet...\r\n";
    cout.flush();
    w5100.init();
    uint8_t mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };
    w5100.setMACAddress(mac);
    w5100.setIPAddress(0);

    cout << "Starting DHCP...\r\n";
    DhcpClass dhcp(&eth);
    dhcp.beginWithDHCP(mac);
    w5100.setIPAddress(dhcp.localIp());
    w5100.setGatewayIp(dhcp.gateway());
    w5100.setSubnetMask(dhcp.subnetMask2());
    eth.addresses(cout);
    cout << "\r\n";

    EthernetServer server(&eth, 23);
    server.begin();
    cout << "Server started\r\n";

    bool gotAMessage = false; // whether or not you got a message from the client yet

    while (true)
    {
        EthernetClient client = server.available();

        if (client)
        {
            if (!gotAMessage) {
                client.print("Hello, client!\r\n"); 
                gotAMessage = true;
            }

            char thisChar = client.read();
            server.write(thisChar);
        }
    }
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




