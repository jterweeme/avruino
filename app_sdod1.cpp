/*
makes a hexdump of the MBR of an SD card, so doesn't use the
FAT code

works

ChipSelect = D9
*/

#include "zd2card.h"
#include <ctype.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "board.h"
#include "stream.h"
#include "int.h"

static Sd2Card *g_sd;

IZR(TIMER0_OVF_vect)
{
    g_sd->tick();
}

static void hexDump(uint8_t *point, ostream &os)
{
    for (uint16_t i = 0; i < 512; i += 16)
    {
        for (uint8_t j = 0; j < 16; j++)
        {
            char wbuf[10];  // write buffer
            snprintf(wbuf, 10, "%02x ", point[i + j]);
            os.write(wbuf);
        }

        for (uint8_t j = 0; j < 16; j++)
        {
            if (isprint(point[i + j]))
                os.write(point[i + j]);
            else
                os.write('.');
        }

        os.write("\r\n");
    }   
}

int main()
{
    *p_tccr0b = 1<<cs02;
    *p_timsk0 = 1<<toie0;
    sei();
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
    while (true)
    {
        hexDump(buf, cout);
        cout.writeString("\r\n");
        cout.flush();

        for (volatile uint32_t i = 0; i < 0xfffff; i++)
            ;
    }

    return 0;
}


