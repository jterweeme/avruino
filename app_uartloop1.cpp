/*
Loop on standard UART using polling method
*/

#include "board.h"
#include "uart.h"

int main()
{
    DefaultUart uart;
    uart.enableRead();
    uart.puts("Testbericht\r\n");

    while (true)
        uart.myPutc(uart.readByte());

    return 0;
}


