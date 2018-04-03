/*
PS2 scancodes naar UART

Uno:
PS2 DATA: D8
PS2 CLK: D2
*/

#include "board.h"
#include "stream.h"
#include "keyboard.h"

PS2Keyboard *g_kb;

int main()
{
    zei();
    *p_tccr0a = 0;
    *p_tccr0b = 1<<cs01 | 1<<cs00;     // clk/64
    *p_tcnt0 = 0;
    *p_timsk0 = 1<<toie0;              // timer0 overflow generates interrupt
    *p_eicra |= 1<<isc00 | 1<<isc01;   // ext int 0 falling edge?
    *p_eimsk |= 1<<int0;               // enable int 0
#if defined (__AVR_ATmega32U4__)
    CDC cdc;
    USBStream os(&cdc);
#else
    DefaultUart s;
    UartStream os(&s);
#endif
    Board b;
    PS2Keyboard keyboard(&b.pin8);
    g_kb = &keyboard;
    os << "Keyboard Test:\r\n";

    while (true)
    {
        uint8_t x = keyboard.get_scan_code();

        if (x > 0)
        {
            os.put(nibble(x >> 4));
            os.put(nibble(x & 0xf));
            os << "\r\n";
            os.flush();
        }
    }

    return 0;
}

extern "C" void TIMER0_OVF __attribute__ ((signal, used, externally_visible));
void TIMER0_OVF
{
    g_kb->timeTick();
}

extern "C" void INTR0 __attribute__ ((signal, used, externally_visible));
void INTR0
{
    g_kb->isr();
}


