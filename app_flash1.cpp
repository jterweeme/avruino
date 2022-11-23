//dump flash chip met Teensy++ 2.0 bord
#include "cdc.h"
#include "stream.h"

int main()
{
    CDC cdc;
    USBStream cout(&cdc);

    //8 data bits input
    *p_ddrf = 0;
    //*p_ddra = 0;

    //16 address bits output
    *p_ddrc = 0xff;
    *p_ddrd = 0xff;

    //OE bit
    *p_ddrb |= 1<<4;
    *p_portb &= ~(1<<4);

    *p_portc = 10;
    *p_portd = 10;


    while (true)
    {
        uint8_t byte = cdc.receive();

        if (byte == 255)
            continue;

        byte = *p_pinf;
        cout.put(nibble(byte >> 4 & 0xf));
        cout.put(nibble(byte >> 0 & 0xf));
        cout.flush();
    }



    return 0;
}

extern "C" void USB_COM __attribute__ ((signal, used, externally_visible));
void USB_COM
{
    USB::instance->com();
}

extern "C" void USB_GEN __attribute__ ((signal, used, externally_visible));
void USB_GEN
{
    USB::instance->gen();
}

