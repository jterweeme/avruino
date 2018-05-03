/*
Loop and convert case on standard UART using polling method
*/

#include "board.h"
#include "uart.h"
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
    DefaultUart uart;
    uart.enableRead();

    while (true)
        uart.myPutc(convertCase(uart.readByte()));

    return 0;
}



