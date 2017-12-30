/*
makes a hexdump of an SD card, so doesn't use the
FAT code
*/

#include "zd2card.h"
#include <ctype.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "board.h"

uint32_t g_millis = 0;

ISR(TIMER0_OVF_vect)
{
    g_millis++;
}

uint32_t millis2()
{
    return g_millis;
}

void hexDump(uint8_t *point, DefaultUart &serial)
{
    for (uint16_t i = 0; i < 512; i += 16)
    {
        for (uint8_t j = 0; j < 16; j++)
        {
            char wbuf[10];  // write buffer
            snprintf(wbuf, 10, "%02x ", point[i + j]);
            serial.write(wbuf);
        }

        for (uint8_t j = 0; j < 16; j++)
        {
            if (isprint(point[i + j]))
                serial.write(point[i + j]);
            else
                serial.write('.');
        }

        serial.write("\r\n");
    }   
}

int main()
{
    TCCR0B = 1<<CS02;
    TIMSK0 = 1<<TOIE0;
    sei();
    Board board;
    Sd2Card sd(&board.pin9);
    sd.init(SPI_HALF_SPEED);
    uint8_t buf[512];
    sd.readBlock(0, buf);
    DefaultUart serial;
    hexDump(buf, serial);

    while (true)
        ;

    return 0;
}


