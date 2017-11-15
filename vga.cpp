#include "vga.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdio.h>

VGA::VGA()
{
    instance = this;
    for (int i = 0; i < 30; i++)
        ::sprintf(charBuffer[i], "Line %03i - hello!", i);

    sei();
    TIMSK0 = 0;
    OCR0A = 0;
    OCR0B = 0;
    DDRB |= (1<<2);
    TCCR1A = 0;
    TCCR1B = 0;
    TCCR1A |= (1<<WGM10) | (1<<WGM11) | (1<<COM1B1);
    TCCR1B |= (1<<WGM12) | (1<<WGM13) | 5;
    OCR1A = 259;
    OCR1B = 0;
    TIFR1 = (1<<TOV1);
    TIMSK1 = (1<<TOIE1);
    DDRD |= (1<<3);
    TCCR2A = 0;
    TCCR2B = 0;
    TCCR2A |= (1<<WGM20) | (1<<WGM21) | (1<<COM2B1);
    TCCR2B |= (1<<WGM22) | 2;
    OCR2A = 63;
    OCR2B = 7;
    TIFR2 = (1<<TOV2);
    TIMSK2 = (1<<TOIE2);
    UBRR0 = 0;
    DDRD |= (1<<4);
    UCSR0B = 0;
    UCSR0C = (1<<UMSEL00) | (1<<UMSEL01) | (1<<UCPHA0) | (1<<UCPOL0);
    set_sleep_mode (SLEEP_MODE_IDLE);
}

void VGA::interrupt()
{
    vLine = 0;
    messageLine = 0;
    backPorchLinesToGo = 35;
}

ISR(TIMER1_OVF_vect)
{
    VGA::getInstance()->interrupt();
}


ISR(TIMER2_OVF_vect)
{
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
    UCSR0B = _BV (TXEN0);

    while (i--)
        UDR0 = pgm_read_byte(linePtr + (*messagePtr++));

    while (!(UCSR0A & _BV(TXC0))) {
    }

    UCSR0B = 0;
    vLine++;

    if ((vLine & 0xF) == 0)
        messageLine++;
}


