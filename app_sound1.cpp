/*
credits to Tomasz Wisniewski
pcarduino.blogspot.com
github.com/dagon666

PCSpeaker: D9

*/

#include "types.h"
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <util/crc16.h>
#include "board.h"

#ifndef F_CPU
#define F_CPU 16000000
#endif

static constexpr uint8_t PWM_SAMPLES_BUFFER = 128, PWM_BUFFERS = 4;
static RingBuf<64> g_rx_buf;
static volatile uint16_t g_head = 0x00, g_tail = 0x00, max_samples = 0x00;

struct packet
{
	uint16_t crc16;
	uint8_t num;
	uint8_t samples[PWM_SAMPLES_BUFFER];
};

static volatile struct packet p[PWM_BUFFERS];

static uint8_t zerial_getc(uint8_t &a_data)
{
    if (g_rx_buf.empty())
        return 0;

    a_data = g_rx_buf.get();
    return 1;
}

#define SLIP_END  0300
#define SLIP_ESC  0333
#define SLIP_ESC_END 0334
#define SLIP_ESC_ESC 0335

static uint8_t slip_verify_crc16(uint8_t *a_buff, uint8_t a_buflen, uint8_t a_crcpos)
{
    uint16_t crc_recv = 0x00, crc_calcd = 0x00;

    if ((a_crcpos + 2) > a_buflen || !a_buflen)
        return 0;

    crc_recv = (*(uint16_t *)&a_buff[a_crcpos]);
    (*(uint16_t *)&a_buff[a_crcpos]) = 0x0000;

    while (a_buflen)
    {
        crc_calcd = _crc16_update(crc_calcd, *a_buff);
        a_buff++;
        a_buflen--;
    }

    return crc_calcd == crc_recv ? crc_calcd : 0;
}


static uint8_t zlip_recv(uint8_t *a_buff, uint8_t a_buflen)
{
    uint8_t c = 0;
    uint16_t recv = 0;
    uint8_t escape = 0xff;

    while (1)
    {
        if (zerial_getc(c) == 0)
            continue;

        switch (c)
        {
        case 0300:
            if (recv)
                return recv;

            break;
        case 0333:
            escape = 0;
            break;
        default:
            if (!escape)
            {
                c = (c == SLIP_ESC_END ? SLIP_END : (c == SLIP_ESC_ESC ? SLIP_ESC : c));
                escape = 0xff;
            }

            if (recv < a_buflen)
                a_buff[recv++] = c;

            break;
        }
    }
}

static void serial_send(uint8_t c)
{
    while ((*p_ucsr0a & 1<<udre0) == 0)
        ;

    *p_udr0 = c;
}

static void serial_flush()
{
    unsigned char dummy __attribute__((unused)) = 0x00;

    while (*p_ucsr0a & 1<<rxc0)
        dummy = *p_udr0;
}

ISR(USART_RX_vect, ISR_BLOCK)
{
    if (UCSR0A & 1<<FE0)    // framing error
    {
        volatile uint8_t data __attribute__ ((unused)) = UDR0;
        return;
    }

    volatile uint8_t data = UDR0;
    g_rx_buf.push(data);
}

static inline void serial_collect_samples(volatile struct packet *a_data)
{
    uint16_t avail = (max_samples + g_head - g_tail) % max_samples;

    if (avail >= 384)
        serial_send('W');

    uint8_t size = zlip_recv((uint8_t *)a_data, sizeof(struct packet));

    if (size < 4)
        return;

    if (!slip_verify_crc16((uint8_t *)a_data, size, 0))
        return;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        g_head = (g_head + a_data->num) % max_samples;
    }
}

ISR(TIMER0_COMPA_vect, ISR_NOBLOCK)
{
	if (g_tail != g_head)
    {
		*p_ocr1a = p[g_tail >> 7].samples[g_tail & 0x7f];
		g_tail = (g_tail + 1) % max_samples;
	}
}

int main(void)
{
    *p_ucsr0a = 0x00;
    *p_ucsr0a |= 1<<u2x0;
    *p_ubrr0l = 8;
    *p_ucsr0c |= 1<<ucsz01 | 1<<ucsz00;
    *p_ucsr0b |= 1<<rxen0 | 1<<txen0;
    serial_flush();
    *p_ucsr0b &= ~(1<<txcie0);
    *p_ucsr0b |= 1<<rxcie0;
    sei();
	serial_flush();
	max_samples = PWM_SAMPLES_BUFFER * PWM_BUFFERS;
    *p_tccr1a = 0;
    *p_tccr1b = 1<<wgm13;
    *p_ocr1a = 0;
    *p_tccr1a |= 1<<com1a1;
    *p_ddr_ocr1a |= 1<<ocr1a_bit;
    *p_tccr1b |= 1<<cs10;
    *p_icr1 = (1<<8) - 1;
    *p_tccr0a = 1<<wgm01;
    *p_tccr0b = 0;
    *p_tcnt0 = 0;
    *p_ocr0a = 0;
    *p_tccr0b &= 0xf8;
    *p_tccr0b |= 1<<cs01;
    *p_ocr0a = 0x7c;
    *p_tcnt0 = 0;
    *p_timsk0 |= 1<<ocie0a;

	while (1)
    {
        serial_collect_samples(&p[g_head>>7]);
    }
	return 0;
}



