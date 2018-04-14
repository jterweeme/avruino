/*
Loop on standard UART using polling method
*/

#include "board.h"
#include "uart.h"

int main()
{
    DefaultUart uart;
    *p_ucsr9a |= 1<<u2x9;
    *p_ubrr9 = 16;
    uart.enableRead();
    uart.puts("Testbericht\r\n");

    while (true)
        uart.myPutc(uart.readByte());

    return 0;
}



