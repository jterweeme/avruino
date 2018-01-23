#include "board.h"
#include "cdc.h"

inline bool isUpper(char c) { return c >= 'A' && c <= 'Z'; }
inline bool isLower(char c) { return c >= 'a' && c <= 'z'; }

inline char convert(char c)
{
    if (isUpper(c)) return c + 32;
    if (isLower(c)) return c - 32;
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
            usb.sendByte(convert(byte));
            usb.flush();
        }
    }

    return 0;
}


