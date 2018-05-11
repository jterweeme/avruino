#include "board.h"

#if defined (__AVR_ATmega32U4__)
#include "cdc.h"
#endif

#include "uart.h"
#include "stream.h"

static const char PI[] = "3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328230664709384460955058223172535940812848111745";

int main()
{
#if defined (__AVR_ATmega32U4__)
    CDC cdc;
    USBStream cout(&cdc);
    USBIStream cin(&cdc);
#else
    DefaultUart s;
    s.enableTransmit();
    s.enableRead();
    UartStream cout(&s);
    UartIStream cin(&s);
#endif
    uint16_t i = 0;

    while (true)
    {
        while (true)
        {
            uint8_t c = cin.get();

            if (c == 255)
                continue;

            cout.put(c);
            cout.flush();
        
            if (c != PI[i++])
                break;
        }

        cout << "\r\nHelaas!\r\n";
        hex16(i, cout);
        cout << "\r\n";
        cout.flush();
        i = 0;
    }

    return 0;
}

#if defined (__AVR_ATmega32U4__)
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
#endif


