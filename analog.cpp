#include "analog.h"
#include "board.h"

TSPoint TouchScreen::getPoint(Analog2 &analog)
{
    Board b;
    int samples[SAMPLES];
    uint8_t valid = 1;
    b.pin11.direction(INPUT);
    b.pin9.direction(INPUT);
    b.pin8.direction(OUTPUT);
    b.pinA2.direction(OUTPUT);
    b.pin8.set();
    b.pinA2.clear();

    for (uint8_t i = 0; i < SAMPLES; i++)
        samples[i] = analog.read(Analog2::ADC3);

    if (samples[0] - samples[1] < -4 || samples[0] - samples[1] > 4)
        valid = 0;
    else
        samples[1] = (samples[0] + samples[1]) >> 1; // average 2 samples

    uint16_t x = samples[2/2] % 1024;
    x -= 200;
    x /= 2.9;
    b.pin8.direction(INPUT);
    b.pinA2.direction(INPUT);
    b.pinA3.direction(OUTPUT);
    b.pin9.direction(OUTPUT);
    b.pin9.clear();
    b.pinA3.set();

    for (uint8_t i = 0; i < SAMPLES; i++)
        samples[i] = analog.read(Analog2::ADC2);

    if (samples[0] - samples[1] < -4 || samples[0] - samples[1] > 4)
        valid = 0;
    else
        samples[1] = (samples[0] + samples[1]) >> 1;

    uint16_t y = 1023 - samples[SAMPLES/2];
    y -= 90;
    y /= 2.5;
    b.pin8.direction(OUTPUT);   //xp
    b.pinA3.direction(INPUT);   //yp
    b.pin8.clear();    //xp
    b.pin9.set();   //ym
    uint16_t z1 = analog.read(Analog2::ADC2);
    uint16_t z2 = analog.read(Analog2::ADC3);
    uint16_t z = 1023 - (z2 - z1);

    if (!valid)
        z = 0;

    return TSPoint(x, y, z);
}

void Analog2::init()
{
    *p_admux = 1<<refs0;
    *p_adcsra = 1<<adps2 | 1<<adps1 | 1<<adps0 | 1<<aden;
}

uint16_t Analog2::read(uint8_t input)
{
    *p_admux &= ~(1<<mux0 | 1<<mux1 | 1<<mux2 | 1<<mux3); // clear mux bits
    *p_admux |= input; // select input port
    *p_adcsra |= 1<<adsc;

    while (*p_adcsra & 1<<adsc)
        ;

    return *p_adcl | (uint16_t)*p_adch << 8;
}


