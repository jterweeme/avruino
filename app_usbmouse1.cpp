/*
usb muis bestuurd met analog knob

Analog knob x axis: A5/ADC0
Analog knob y axis: A4/ADC1
Button 0: A0/PF7
Button 1: A1/PF6
*/

#include "usbmouse.h"
#include "analog.h"

inline int8_t vec(uint16_t reading)
{
    if (reading < 256)
        return -1;

    if (reading > 768)
        return 1;

    return 0;
}

int main()
{
    *p_ddrf &= ~(1<<7 | 1<<6);
    *p_portf |= 1<<7 | 1<<6;
    USBMouse usbmouse;
    zei();
    Analog analog;
    analog.init();

    while (true)
    {
        MouseReportData mrd;
        mrd.button = 0;
        uint16_t reading = analog.read(Analog::ADC0);
        mrd.x = vec(reading);
        reading = analog.read(Analog::ADC1);
        mrd.y = vec(reading);

        if ((*p_pinf & 1<<7) == 0)
            mrd.button |= 1<<0;     // left button

        if ((*p_pinf & 1<<6) == 0)
            mrd.button |= 1<<1;     // right button

        usbmouse.sendReport(mrd);
    }

    return 0;
}


