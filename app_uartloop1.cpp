/*
Loop on standard UART using polling method
*/

#include "board.h"
#include "uart.h"

int main()
{
    DefaultUart uart;
    //*p_ucsr1a |= 1<<u2x1;
    *p_ubrr1 = 103;
    uart.enableRead();
    uart.puts("Testbericht\r\n");

    while (true)
        uart.myPutc(uart.readByte());

    return 0;
}



