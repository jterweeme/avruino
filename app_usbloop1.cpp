/*
reads from USB
prints to USB in opposite case
*/

#include "board.h"
#include "cdc.h"
#include "misc.h"

static inline bool isUpper(char c)
{
    return c >= 'A' && c <= 'Z';
}

static inline bool isLower(char c)
{
    return c >= 'a' && c <= 'z';
}

static inline char convertCase(char c)
{
    if (isUpper(c))
        return c + 32;

    if (isLower(c))
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

#ifndef BUSBY_INT
extern "C" void __vector_11() __attribute__ ((signal, used, externally_visible));
void __vector_11()
{
    USB::instance->com();
}

extern "C" void __vector_10() __attribute__ ((signal, used, externally_visible));
void __vector_10()
{
    USB::instance->gen();
}
#endif


