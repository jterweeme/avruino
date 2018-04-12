/*
Loop on standard UART using polling method
*/

#include "board.h"
#include "uart.h"

int main()
{
    DefaultUart uart;
#if defined (__AVR_AT90USB1286__)
    *p_ubrr1 = 103;
#endif
    uart.enableRead();
    uart.puts("Testbericht\r\n");

    while (true)
        uart.myPutc(uart.readByte());

    return 0;
}



