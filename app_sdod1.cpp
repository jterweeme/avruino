/*
makes a hexdump of an SD card, so doesn't use the
FAT code
*/

#include "zd2card.h"
#include <ctype.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "board.h"
#include "stream.h"

static Sd2Card *g_sd;

uint32_t g_millis = 0;

ISR(TIMER0_OVF_vect)
{
    g_sd->tick();
    g_millis++;
}

uint32_t millis2()
{
    return g_millis;
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
    TCCR0B = 1<<CS02;
    TIMSK0 = 1<<TOIE0;
    sei();
    Board board;
    Sd2Card sd(&board.pin9);
    g_sd = &sd;
    sd.init(SPI_FULL_SPEED);
    uint8_t buf[512];
    sd.readBlock(0, buf);
    DefaultUart serial;
    UartStream cout(&serial);

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


