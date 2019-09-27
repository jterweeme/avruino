/*
Meet voltage op ADC0 en print uit in binair op seriele terminal
*/

#include "board.h"
#include "uart.h"
#include "stream.h"

int main()
{
    DefaultUart uart;
    UartStream cout(&uart);
    //cout << "Starting application...\r\n";
    *p_admux = 0x00 | 1<<refs0 |1<<adlar;   // avcc, 8 bit
#if defined (__AVR_ATmega8__)
    *p_adcsra = 3 | 1<<aden | 1<<adfr;
#else
    *p_adcsra = 1<<adps1 | 1<<adps0 | 1<<aden | 1<<adate;
    *p_adcsrb &= ~(1<<adts2 | 1<<adts1 | 1<<adts0);
#endif
    *p_adcsra |= 1<<adsc;
    //cout << "Start Conversion...\r\n";

    while (true)
    {
        uint8_t x = *p_adch;
        cout.put(x);
        for (volatile uint32_t i = 0; i < 0x1ffff; i++);    // delay
    }

    return 0;
}


