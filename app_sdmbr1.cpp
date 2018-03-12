/*
makes a hexdump of the MBR of an SD card, so doesn't use the
FAT code

works

ChipSelect = D9
*/

#include "zd2card.h"
#include "board.h"
#include "stream.h"

static Sd2Card *g_sd;

static inline bool izprint(uint8_t c)
{
    return c >= 0x20 && c <= 0x7e ? true : false;
}

static void hexDump(uint8_t *point, ostream &os)
{
    for (uint16_t i = 0; i < 512; i += 16)
    {
        for (uint8_t j = 0; j < 16; j++)
        {
            os.put(nibble(point[i + j] >> 4));
            os.put(nibble(point[i + j] & 0xf));
            os.put(' ');
        }

        for (uint8_t j = 0; j < 16; j++)
        {
            if (izprint(point[i + j]))
                os.put(point[i + j]);
            else
                os.put('.');
        }

        os << "\r\n";
    }   
}

int main()
{
    *p_tccr0b = 1<<cs02;
    *p_timsk0 = 1<<toie0;
    zei();
    Board board;
    Sd2Card sd(&board.pin9);
    g_sd = &sd;
    sd.init(SPI_FULL_SPEED);
    uint8_t buf[512];
    sd.readBlock(0, buf);
#if defined (__AVR_ATmega32U4__)
    CDC cdc;
    USBStream cout(&cdc);
#else
    DefaultUart s;
    UartStream cout(&s);
#endif
    cout << "Startup\r\n";

    while (true)
    {
        hexDump(buf, cout);
        cout << "\r\n";
        cout.flush();

        for (volatile uint32_t i = 0; i < 0xfffff; i++)
            ;
    }

    return 0;
}

extern "C" void TIMER0_OVF __attribute__ ((signal, used, externally_visible));
void TIMER0_OVF
{
    g_sd->tick();
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



