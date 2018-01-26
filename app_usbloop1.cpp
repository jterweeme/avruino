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



