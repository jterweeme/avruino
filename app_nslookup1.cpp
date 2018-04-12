#include "uip_client.h"
#include "stream.h"

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

static UIPEthernetClass eth;

int main()
{
    DefaultUart s;
    *p_ucsr0a |= 1<<u2x0;
    *p_ubrr0 = 16;
    UartStream cout(&s);
    cout << "Startup\r\n";
    cout.flush();
    *p_tccr0b = cs02; // | CS00;
    *p_timsk0 |= 1<<toie0;
    zei();
    uint8_t mac[6] = {0,1,2,3,4,5};
    uint32_t myIP = (uint32_t)192 | (uint32_t)168<<8 | (uint32_t)200<<16 | (uint32_t)101<<24;
    uint32_t dns = 0x08080808;
    uint32_t subnet = 0x00ffffff;
    uint32_t gw = (uint32_t)192 | (uint32_t)168<<8 | (uint32_t)200<<16 | (uint32_t)1<<24;
    eth.begin(mac, myIP, dns, gw, subnet);  //, 0x20b2abc0, 0x08080808, 0x00ffffff);
    cout << "eth begin\r\n";
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



