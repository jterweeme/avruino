#include "board.h"
#include "misc.h"
#include <stdio.h>
#include "stream.h"
#include "capsense.h"

int main()
{
    CapSense sense2(p_pin2_base, pin2_bit);
    CapSense sense6(p_pin6_base, pin6_bit);
    CapSense sense7(p_pin7_base, pin7_bit);
    CapSense sense8(p_pin8_base, pin8_bit);
    CapSense sense9(p_pin9_base, pin9_bit);
    CapSense sense10(p_pin10_base, pin10_bit);
    CapSense sense11(p_pin11_base, pin11_bit);
    CapSense sense12(p_pin12_base, pin12_bit);
    CapSense senseA5(p_pinA5_base, pinA5_bit);
    CapSense senseA4(p_pinA4_base, pinA4_bit);
    CapSense senseA3(p_pinA3_base, pinA3_bit);
    CapSense senseA2(p_pinA2_base, pinA2_bit);
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
        uint32_t total2 = sense2.senseRaw(30);
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
        snprintf(buf, 50, "%lu\t", total2);
        cout.writeString(buf);
        snprintf(buf, 50, "%lu\t", total6);
        cout.writeString(buf);
        snprintf(buf, 50, "%lu\t", total7);
        cout.writeString(buf);
        snprintf(buf, 50, "%lu\t", total8);
        cout.writeString(buf);
        snprintf(buf, 50, "%lu\t", total9);
        cout.writeString(buf);
        snprintf(buf, 50, "%lu\t", total10);
        cout.writeString(buf);
        snprintf(buf, 50, "%lu\t", total11);
        cout.writeString(buf);
        snprintf(buf, 50, "%lu\t", total12);
        cout.writeString(buf);
        snprintf(buf, 50, "%lu\t", totalA5);
        cout.writeString(buf);
        snprintf(buf, 50, "%lu\t", totalA4);
        cout.writeString(buf);
        snprintf(buf, 50, "%lu\t", totalA3);
        cout.writeString(buf);
        snprintf(buf, 50, "%lu\t", totalA2);
        cout.writeString(buf);
        cout.writeString("\r\n");
        cout.flush();

        for (volatile uint32_t i = 0; i < 0xfff; i++)
            ;
    }
    return 0;
}



