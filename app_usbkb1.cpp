/*
Buttons geven USB keyboard output

buttons worden gepolled op PORTF, de A0 tot A5 op de Leonardo
*/

#include "usbkb.h"
#include "misc.h"
#include <string.h>

KBReport report;

void hidTask(USBKB &kb)
{
    uint8_t i = 0;
    memset((void *)&report, 0, sizeof(report));

    if ((*p_pinf & 1<<0) == 0)
        report.keyCode[i++] = HID_KEYBOARD_SC_A;

    if ((*p_pinf & 1<<1) == 0)
        report.keyCode[i++] = HID_KEYBOARD_SC_B;

    if ((*p_pinf & 1<<4) == 0)
        report.keyCode[i++] = HID_KEYBOARD_SC_C;

    if ((*p_pinf & 1<<5) == 0)
        report.keyCode[i++] = HID_KEYBOARD_SC_D;

    if ((*p_pinf & 1<<6) == 0)
        report.keyCode[i++] = HID_KEYBOARD_SC_E;

    if ((*p_pinf & 1<<7) == 0)
        report.keyCode[i++] = HID_KEYBOARD_SC_F;

    kb.sendReport(report);
}

int main()
{
    *p_ddrf &= ~(1<<0 | 1<<1 | 1<<4 | 1<<5 | 1<<6 | 1<<7);
    *p_portf |= 1<<0 | 1<<1 | 1<<4 | 1<<5 | 1<<6 | 1<<7;
    USBKB kb;
    
    while (true)
        hidTask(kb);

    return 0;
}


