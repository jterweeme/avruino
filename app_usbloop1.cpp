/*
reads from USB
prints to USB in opposite case
*/

#include "board.h"
#include "cdc.h"
#include "misc.h"

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
    CDC usb;

    while (true)
    {
        uint8_t byte = usb.receive();

        if (byte != 255)
        {
            usb.sendByte(convertCase(byte));
            usb.flush();
        }
    }

    return 0;
}

extern "C" void USB_COM __attribute__ ((signal, used, externally_visible));
void USB_COM
{
    USB::instance->com();
}

extern "C" void USB_GEN __attribute__ ((signal, used, externally_visible));
void USB_GEN
{
    USB::instance->gen();
}


