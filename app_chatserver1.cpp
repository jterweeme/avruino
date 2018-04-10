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

#include "EthernetClient.h"
#include "EthernetServer.h"
#include <avr/interrupt.h>
#include "util.h"

int main()
{
    EthernetClass eth;
    uint8_t mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };
    IPAddress ip(192,168,200, 100);
    IPAddress gateway(192,168,1, 1);
    IPAddress subnet(255, 255, 0, 0);
    EthernetServer server(&eth, 23);
    bool gotAMessage = false; // whether or not you got a message from the client yet
    sei();
    TCCR0A |= 1<<WGM01 | 1<<WGM00;
    TCCR0B |= 1<<CS01 | 1<<CS00;
    TIMSK0 |= 1<<TOIE0;
    eth.begin(mac, ip);
    //ip = Ethernet.localIP();
    server.begin();

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




