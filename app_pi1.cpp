#include "board.h"

#if defined (__AVR_ATmega32U4__)
#include "cdc.h"
#endif

#include "uart.h"
#include "stream.h"

static const char PI[] = "3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328230664709384460955058223172535940812848111745";

static inline char nibble(uint8_t n)
{
    return n <= 9 ? '0' + n : 'A' + n - 10;
}

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
            uint8_t byte = cin.get();

            if (byte == 255)
                continue;

            cout.write(byte);
            cout.flush();
        
            if (byte != PI[i++])
                break;
        }

        cout.writeString("\r\nHelaas!\r\n");
        cout.write(nibble(i >> 12 & 0xf));
        cout.write(nibble(i >> 8 & 0xf));
        cout.write(nibble(i >> 4 & 0xf));
        cout.write(nibble(i & 0xf));
        cout.writeString("\r\n");
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


