#include "uip_server.h"
#include "stream.h"

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

static UIPEthernetClass eth;

static inline char convertCase(char c)
{
    if (my_isupper(c))
        return c + 32;

    if (my_islower(c))
        return c - 32;

    return c;
}

int main()
{
    UIPServer server = UIPServer(&eth, 23);
    *p_tccr0b = cs02; // | CS00;
    *p_timsk0 |= 1<<toie0;
    zei();
    uint8_t mac[6] = {0,1,2,3,4,5};
    DhcpClass dhcp(&eth);
    eth.begin(mac, &dhcp);
    server.begin();
#if defined (__AVR_ATmega2560__)
    UartBase uart1(p_ubrr1, p_udr1, p_ucsr1a, p_ucsr1b);
    uart1.enableTransmit();
    *p_ucsr1a |= 1<<u2x1;
    *p_ubrr1 = 16;
    UartStream cout(&uart1);
    cout << "Startup\r\n";
#endif
    while (true)
    {
        UIPClient client = server.available();

        if (client)
        {
#if defined (__AVR_ATmega2560__)
            cout << "Client connected\r\n";
#endif
            while (client.connected())
            {
                if (client.available())
                {
                    char c = client.read();
#if defined (__AVR_ATmega2560__)
                    cout.put(convertCase(c));
#endif
                    server.write(convertCase(c));
                }
            }

            for (volatile uint16_t i = 0; i < 0x4fff; i++); // delay
            client.stop();
#if defined (__AVR_ATmega2560__)
            cout << "Client disconnected\r\n";
#endif
        }
    }
    return 0;
}

extern "C" void TIMER0_OVF __attribute__ ((signal, used, externally_visible));
void TIMER0_OVF
{
    eth.tick2();
}



