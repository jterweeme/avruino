/*
PS2 Keyboard

DATA: D8
CLK: D3
*/

#include "usbkb.h"
#include "misc.h"
#include "keyboard.h"
#include <string.h>
#include <avr/interrupt.h>
//#include "stream.h"


PS2Keyboard *g_kb;

ISR(TIMER0_OVF_vect)
{
    g_kb->timeTick();
}

ISR(INT0_vect)
{
    g_kb->isr();
}

static inline char nibble(uint8_t n)
{
    return n <= 9 ? '0' + n : 'A' + n - 10;
}

static uint8_t lut(uint8_t x)
{
    switch (x)
    {
    case PS2_A:
        return HID_KEYBOARD_SC_A;
    case PS2_B:
        return HID_KEYBOARD_SC_B;
    case PS2_C:
        return HID_KEYBOARD_SC_C;
    case PS2_D:
        return HID_KEYBOARD_SC_D;
    case PS2_E:
        return HID_KEYBOARD_SC_E;
    case PS2_F:
        return HID_KEYBOARD_SC_F;
    case PS2_G:
        return HID_KEYBOARD_SC_G;
    case PS2_H:
        return HID_KEYBOARD_SC_H;
    case PS2_I:
        return HID_KEYBOARD_SC_I;
    case PS2_J:
        return HID_KEYBOARD_SC_J;
    case PS2_K:
        return HID_KEYBOARD_SC_K;
    case PS2_L:
        return HID_KEYBOARD_SC_L;
    case PS2_M:
        return HID_KEYBOARD_SC_M;
    case PS2_N:
        return HID_KEYBOARD_SC_N;
    case PS2_O:
        return HID_KEYBOARD_SC_O;
    case PS2_P:
        return HID_KEYBOARD_SC_P;
    case PS2_Q:
        return HID_KEYBOARD_SC_Q;
    case PS2_R:
        return HID_KEYBOARD_SC_R;
    case PS2_S:
        return HID_KEYBOARD_SC_S;
    case PS2_T:
        return HID_KEYBOARD_SC_T;
    case PS2_U:
        return HID_KEYBOARD_SC_U;
    case PS2_V:
        return HID_KEYBOARD_SC_V;
    case PS2_W:
        return HID_KEYBOARD_SC_W;
    case PS2_X:
        return HID_KEYBOARD_SC_X;
    case PS2_Y:
        return HID_KEYBOARD_SC_Y;
    case PS2_Z:
        return HID_KEYBOARD_SC_Z;
    case PS2_BACKSPACE:
        return HID_KEYBOARD_SC_BACKSPACE;
    case PS2_SPACE:
        return HID_KEYBOARD_SC_SPACE;
    case PS2_ESC:
        return HID_KEYBOARD_SC_ESCAPE;
    case PS2_ENTER:
        return HID_KEYBOARD_SC_ENTER;
    }

    return 0;
}

static bool isPS2Modifier(uint8_t ps2scan)
{
    switch (ps2scan)
    {
    case PS2_LSHIFT:
    case PS2_LCTRL:
        return true;
    }

    return false;
}

static uint8_t modLUT(uint8_t ps2scan)
{
    switch (ps2scan)
    {
    case PS2_LSHIFT:
        return 1<<1;
    case PS2_LCTRL:
        return 1<<0;
    }

    return 0;
}

int main()
{
    sei();
    TCCR0A = 0;
    TCCR0B = 1<<CS01 | 1<<CS00;
    TCNT0 = 0;
    TIMSK0 = 1<<TOIE0;
    EICRA |= 1<<ISC00 | 1<<ISC01;
    EIMSK |= 1<<INT0;
    USBKB usbkb;
    Board b;
    PS2Keyboard ps2kb(&b.pin8);
    g_kb = &ps2kb;
    KBReport report;
   
    while (true)
    {
        volatile uint8_t x = ps2kb.get_scan_code();
        //memset((void *)&report, 0, sizeof(report));

        if (x > 0)
        {
            if (x == 0xf0)
            {
                x = ps2kb.get_scan_code();

                if (isPS2Modifier(x))
                    report.modifier &= ~(modLUT(x));
                else
                    report.keyCode[0] = 0;
            }
            else
            {
                if (isPS2Modifier(x))
                    report.modifier |= modLUT(x);
                else
                    report.keyCode[0] = lut(x);
            }

            //usbkb.sendReport(report);
        }

        usbkb.sendReport(report);
    }

    return 0;
}




