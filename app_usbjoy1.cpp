/*
USB Joystick using analog knob and buttons

Analog knob x axis: A5/ADC0
Analog knob y axis: A4/ADC1
Button 0: A0/PF7
Button 1: A1/PF6
*/

#include "usbjoy.h"
#include "analog.h"
#include "board.h"

int main()
{
    USBJoy joy;
    zei();
    *p_ddrf &= ~(1<<7 | 1<<6);
    *p_portf |= 1<<7 | 1<<6;
    JoyReportData jrd;
    Analog2 analog;
    analog.init();

    while (true)
    {
        jrd.x = jrd.y = jrd.z = 0;
        jrd.button = 0;

        uint16_t reading = analog.read(Analog2::ADC0);
        jrd.x = (reading >> 2) - 128;
        reading = analog.read(Analog2::ADC1);
        jrd.y = (reading >> 2) - 128;

        if ((*p_pinf & 1<<7) == 0)
            jrd.button |= 1<<0;
        
        if ((*p_pinf & 1<<6) == 0)
            jrd.button |= 1<<1;

        joy.sendReport(jrd);
    }

    return 0;
}



