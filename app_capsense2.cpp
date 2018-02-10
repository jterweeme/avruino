#include "board.h"
#include "misc.h"
#include <stdio.h>
#include "stream.h"
#include "capsense.h"

int main()
{
    Board board;
    CapSense toets08(board.pin13, board.pin5);
    CapSense sense6(board.pin13, board.pin6);
    CapSense sense7(board.pin13, board.pin7);
    CapSense sense8(board.pin13, board.pin8);
    CapSense sense9(board.pin13, board.pin9);
    CapSense sense10(board.pin13, board.pin10);
    CapSense sense11(board.pin13, board.pin11);
    CapSense sense12(board.pin13, board.pin12);
    CapSense senseA2(board.pin13, board.pinA2);
    CapSense senseA3(board.pin13, board.pinA3);
    CapSense senseA4(board.pin13, board.pinA4);
    CapSense senseA5(board.pin13, board.pinA5);
#if defined (__AVR_ATmega32U4__)
    CDC cdc;
    USBStream cout(&cdc);
#else
    DefaultUart uart;
    uart.enableTransmit();
    UartStream cout(&uart);
#endif
    while (true)
    {
        uint32_t total08 = toets08.senseRaw(30);
        uint32_t total6 = sense6.senseRaw(30);
        uint32_t total7 = sense7.senseRaw(30);
        uint32_t total8 = sense8.senseRaw(30);
        uint32_t total9 = sense9.senseRaw(30);
        uint32_t total10 = sense10.senseRaw(30);
        uint32_t total11 = sense11.senseRaw(30);
        uint32_t total12 = sense12.senseRaw(30);
        uint32_t totalA5 = senseA5.senseRaw(30);
        uint32_t totalA4 = senseA4.senseRaw(30);
        uint32_t totalA3 = senseA3.senseRaw(30);
        uint32_t totalA2 = senseA2.senseRaw(30);
        char buf[50];
        snprintf(buf, 50, "%lu\t", total12);
        cout.writeString(buf);
        snprintf(buf, 50, "%lu\t", total11);
        cout.writeString(buf);
        snprintf(buf, 50, "%lu\t", total10);
        cout.writeString(buf);
        snprintf(buf, 50, "%lu\t", total9);
        cout.writeString(buf);
        snprintf(buf, 50, "%lu\t", total8);
        cout.writeString(buf);
        snprintf(buf, 50, "%lu\t", total7);
        cout.writeString(buf);
        snprintf(buf, 50, "%lu\t", total6);
        cout.writeString(buf);
        snprintf(buf, 50, "%lu\t", total08);
        cout.writeString(buf);
        snprintf(buf, 50, "%lu\t", totalA2);
        cout.writeString(buf);
        snprintf(buf, 50, "%lu\t", totalA3);
        cout.writeString(buf);
        snprintf(buf, 50, "%lu\t", totalA4);
        cout.writeString(buf);
        snprintf(buf, 50, "%lu\t", totalA5);
        cout.writeString(buf);
        cout.writeString("\r\n");
        cout.flush();

        for (volatile uint32_t i = 0; i < 0x3fff; i++)
            ;
    }
    return 0;
}



