#include "analog.h"
#include "misc.h"
#include <stdio.h>

static constexpr uint8_t SAMPLES = 2;

static void getPoint(Analog2 &analog, DefaultUart &serial)
{
    int samples[SAMPLES];
    uint8_t valid = 1;
    DDRC &= ~(1<<3);    // A3
    DDRB &= ~(1<<1);    // D9
    DDRB |= 1<<0;       // D8
    DDRC |= 1<<2;       // A2
    PORTB |= 1<<0;      // D8
    PORTC &= ~(1<<2);   // A2

    for (uint8_t i = 0; i < SAMPLES; i++)
        samples[i] = analog.read(Analog2::ADC3); //A3

    if (samples[0] - samples[1] < -4 || samples[0] - samples[1] > 4)
        valid = 0;
    else
        samples[1] = (samples[0] + samples[1]) >> 1; // average 2 samples

    //uint16_t x = 1023 - samples[2/2];
    uint16_t x = samples[2/2] % 1024;
    x -= 200;
    x /= 2.9;
    DDRB &= ~(1<<0);    // D8
    DDRC &= ~(1<<2);    // A2
    DDRC |= 1<<3;       // A3
    DDRB |= 1<<1;       // D9
    PORTB &= ~(1<<1);   // D9
    PORTC |= 1<<3;      // A3

    for (uint8_t i = 0; i < SAMPLES; i++)
        samples[i] = analog.read(Analog2::ADC2); //A2

    if (samples[0] - samples[1] < -4 || samples[0] - samples[1] > 4)
        valid = 0;
    else
        samples[1] = (samples[0] + samples[1]) >> 1;

    uint16_t y = 1023 - samples[SAMPLES/2];
    y -= 90;
    y /= 2.5;
    DDRB |= 1<<0;       //xp    D8
    DDRC &= ~(1<<3);    //yp    A3
    PORTB &= ~(1<<0);   //xp    D8
    PORTB |= 1<<1;      //ym    D9
    uint16_t z1 = analog.read(Analog2::ADC2);
    uint16_t z2 = analog.read(Analog2::ADC3);
    uint16_t z = 1023 - (z2 - z1);

    if (!valid)
        z = 0;

    char buf[100];
    snprintf(buf, 100, "%u %u %u\r\n", x, y, z);
    serial.write(buf);
}

int main()
{
    DefaultUart s;
    //s.init();
    Analog2 a;
    a.init();

    while (true)
        getPoint(a, s);

    return 0;
}



