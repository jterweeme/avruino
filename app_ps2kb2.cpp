/*
PS2 scancodes naar UART

PS2 DATA: D8
PS2 CLK: D2
*/

#include "board.h"
#include "stream.h"
#include <avr/interrupt.h>
#include "keyboard.h"

PS2Keyboard *g_kb;

ISR(TIMER0_OVF_vect)
{
    g_kb->timeTick();
}

ISR(INT0_vect)
{
    g_kb->isr();
}

int main()
{
    sei();
    TCCR0A = 0;
    TCCR0B = 1<<CS01 | 1<<CS00;     // clk/64
    TCNT0 = 0;
    TIMSK0 = 1<<TOIE0;              // timer0 overflow generates interrupt
    EICRA |= 1<<ISC00 | 1<<ISC01;   // ext int 0 falling edge?
    EIMSK |= 1<<INT0;               // enable int 0
#if defined (__AVR_ATmega32U4__)
    CDC cdc;
    USBStream os(&cdc);
#else
    DefaultUart s;
    UartStream os(&s);
#endif
    //s.init();
    Board b;
    PS2Keyboard keyboard(&b.pin8);
    g_kb = &keyboard;
    os.writeString("Keyboard Test:\r\n");

    while (true)
    {
        uint8_t x = keyboard.get_scan_code();

        if (x > 0)
        {
            os.put(nibble(x >> 4));
            os.put(nibble(x & 0xf));
            os.writeString("\r\n");
            os.flush();
        }
    }

    return 0;
}


