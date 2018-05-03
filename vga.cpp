#include "vga.h"
#include <avr/interrupt.h>
#include "sleepy.h"
#include <stdio.h>
#include "board.h"

VGA::VGA()
{
    instance = this;
    for (int i = 0; i < 30; i++)
        ::sprintf(charBuffer[i], "Line %03i - hello!", i);

    zei();
    *p_timsk0 = 0;
    *p_ocr0a = 0;
    *p_ocr0b = 0;
    *p_ddrb |= (1<<2);
    *p_tccr1a = 0;
    *p_tccr1b = 0;
    *p_tccr1a |= 1<<wgm10 | 1<<wgm11 | 1<<com1b1;
    *p_tccr1b |= 1<<wgm12 | 1<<wgm13 | 5;
    *p_ocr1a = 259;
    *p_ocr1b = 0;
    *p_tifr1 = 1<<tov1;
    *p_timsk1 = 1<<toie1;
    *p_ddrd |= 1<<3;
    *p_tccr2a = 0;
    *p_tccr2b = 0;
    *p_tccr2a |= 1<<wgm20 | 1<<wgm21 | 1<<com2b1;
    *p_tccr2b |= 1<<wgm22 | 2;
    *p_ocr2a = 63;
    *p_ocr2b = 7;
    *p_tifr2 = 1<<tov2;
    *p_timsk2 = 1<<toie2;
    *p_ubrr9 = 0;
    *p_ddrd |= (1<<4);
    *p_ucsr9b = 0;
    *p_ucsr9c = 1<<umsel90 | 1<<umsel91 | 1<<ucpol9;   // 1<<UCPHA0
    set_sleep_mode(SLEEP_MODE_IDLE);
}

void VGA::interrupt()
{
    vLine = 0;
    messageLine = 0;
    backPorchLinesToGo = 35;
}

VGA *VGA::instance;

void VGA::scanLine()
{
    if (backPorchLinesToGo)
    {
        backPorchLinesToGo--;
        return;
    }

    if (vLine >= 480)
        return;

    const register uint8_t *linePtr = &screen_font[(vLine>>1) & 0x07][0];
    char *messagePtr = &(charBuffer[messageLine][0]);
    uint8_t i = 20;
    *p_ucsr9b = 1<<txen9;

    while (i--)
        *p_udr9 = pgm_read_byte(linePtr + (*messagePtr++));

    while ((*p_ucsr9a & 1<<txc9) == 0) {
    }

    *p_ucsr9b = 0;
    vLine++;

    if ((vLine & 0xF) == 0)
        messageLine++;
}


