/*
credits to Tomasz Wisniewski
pcarduino.blogspot.com
github.com/dagon666

PCSpeaker, UNO: OC1A/D9/PB1
PCSpeaker, MEGA: OC1A/D11/PB5
PCSpeaker, LEONARDO: OC1A/D9/PB5
*/

#include "types.h"
#include "board.h"
#include "misc.h"

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

static constexpr uint8_t
    SLIP_END = 0xc0,
    SLIP_ESC = 0xdb,
    SLIP_ESC_END = 0xdc,
    SLIP_ESC_ESC = 0xdd,
    PWM_SAMPLES_BUFFER = 128,
    PWM_BUFFERS = 4;

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

static uint16_t crc16_update(uint16_t crc, uint8_t a)
{
    crc ^= a;

    for (uint8_t i = 0; i < 8; i++)
        crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;

    return crc;
}

static uint8_t slip_verify_crc16(uint8_t *a_buff, uint8_t a_buflen, uint8_t a_crcpos)
{
    uint16_t crc_recv = 0x00, crc_calcd = 0x00;

    if ((a_crcpos + 2) > a_buflen || !a_buflen)
        return 0;

    crc_recv = (*(uint16_t *)&a_buff[a_crcpos]);
    (*(uint16_t *)&a_buff[a_crcpos]) = 0x0000;

    while (a_buflen)
    {
        crc_calcd = crc16_update(crc_calcd, *a_buff);
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
    while ((*p_ucsr9a & 1<<udre9) == 0);
    *p_udr9 = c;
}

static void serial_flush()
{
    unsigned char dummy __attribute__((unused)) = 0x00;

    while (*p_ucsr9a & 1<<rxc9)
        dummy = *p_udr9;
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

    g_head = (g_head + a_data->num) % max_samples;  // used to be atomic...
}



int main(void)
{
    *p_ucsr9a = 0x00;
    *p_ucsr9a |= 1<<u2x9;
    *p_ubrr9l = 8;
    *p_ucsr9c |= 1<<ucsz91 | 1<<ucsz90;
    *p_ucsr9b |= 1<<rxen9 | 1<<txen9;
    serial_flush();
    *p_ucsr9b &= ~(1<<txcie9);
    *p_ucsr9b |= 1<<rxcie9;
    zei();
	serial_flush();
	max_samples = PWM_SAMPLES_BUFFER * PWM_BUFFERS;
    *p_tccr1a = 0;
    *p_tccr1b = 1<<wgm13;
    *p_ocr1a = 0;
    *p_tccr1a |= 1<<com1a1;
    *p_ocr1a_ddr |= 1<<ocr1a_bit;
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

extern "C" void TIMER0_COMPA __attribute__ ((signal, used, externally_visible));
void TIMER0_COMPA
{
	if (g_tail != g_head)
    {
		*p_ocr1a = p[g_tail >> 7].samples[g_tail & 0x7f];
		g_tail = (g_tail + 1) % max_samples;
	}
}

extern "C" void USART_RX __attribute__ ((signal, used, externally_visible));
void USART_RX
{
    if (*p_ucsr9a & 1<<fe9)    // framing error
    {
        volatile uint8_t data __attribute__ ((unused)) = *p_udr9;
        return;
    }

    volatile uint8_t data = *p_udr9;
    g_rx_buf.push(data);
}


