#include <avr/interrupt.h>
#include "misc.h"

#define F_CPU 16000000
#include <util/delay.h>
#include "infrared.h"

inline char nibble(uint8_t n)
{
    return n <= 9 ? '0' + n : 'A' + n - 10;
}

int main()
{
    int RECV_PIN = 10;
    IRrecv irrecv(RECV_PIN);
    decode_results results;
    DefaultUart serial;
    serial.enableTransmit();
    serial.puts("Enabling IRin\r\n");
    irrecv.enableIRIn();
    serial.puts("Enabled IRin\r\n");

    while (true)
    {
        if (irrecv.decode(&results))
        {
            for (int8_t i = 7; i >= 0; i--)
                serial.myPutc(nibble(results.value >> (i << 2) & 0xf));

            serial.puts("\r\n");
            irrecv.resume();
        }

        _delay_ms(100);
    }

    return 0;
}



