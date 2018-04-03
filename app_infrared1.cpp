#include <avr/interrupt.h>
#include "misc.h"

#ifndef F_CPU
#define F_CPU 16000000
#endif

#include <util/delay.h>
#include "infrared.h"
#include "stream.h"

int main()
{
    int RECV_PIN = 10;
    IRrecv irrecv(RECV_PIN);
    decode_results results;
    DefaultUart serial;
    UartStream cout(&serial);
    serial.enableTransmit();
    cout << "Enabling IRin\r\n";
    irrecv.enableIRIn();
    cout << "Enabled IRin\r\n";

    while (true)
    {
        if (irrecv.decode(&results))
        {
            for (int8_t i = 7; i >= 0; i--)
                serial.myPutc(nibble(results.value >> (i << 2) & 0xf));

            cout << "\r\n";
            irrecv.resume();
        }

        _delay_ms(100);
    }

    return 0;
}



