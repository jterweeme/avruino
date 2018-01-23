#include "analog.h"
#include "board.h"

TSPoint TouchScreen::getPoint()
{
    Board b;
    int samples[SAMPLES];
    uint8_t valid = 1;
#if defined (__AVR_ATmega328P__)
    DDRC &= ~(1<<3);    // A3
    DDRB &= ~(1<<1);    // D9
    DDRB |= 1<<0;       // D8
    DDRC |= 1<<2;       // A2
    PORTB |= 1<<0;      // D8
    PORTC &= ~(1<<2);   // A2
#elif defined (__AVR_ATmega2560__)
    DDRF &= ~(1<<3);    // A3
    DDRH &= ~(1<<6);    // D9
    DDRH |= 1<<5;       // D8
    DDRF |= 1<<2;       // A2
    PORTH |= 1<<5;      // D8
    PORTF &= ~(1<<2);   // A2
#else
    b.pin11.direction(INPUT);
    b.pin9.direction(INPUT);
    b.pin8.direction(OUTPUT);
    b.pinA2.direction(OUTPUT);
    b.pin8.set();
    b.pinA2.clear();
#endif
    for (uint8_t i = 0; i < SAMPLES; i++)
    {
#if defined (__AVR_ATmega328P__)
        samples[i] = _adc->read(Analog2::ADC3);
#elif defined (__AVR_ATmega2560__)
        samples[i] = _adc->read(Analog2::ADC3);
#else
        samples[i] = _adc->read(Analog2::ADC4);
#endif
    }

    if (samples[0] - samples[1] < -4 || samples[0] - samples[1] > 4)
        valid = 0;
    else
        samples[1] = (samples[0] + samples[1]) >> 1; // average 2 samples

    uint16_t x = samples[2/2] % 1024;
    x -= 200;
    x /= 2.9;
#if defined (__AVR_ATmega328P__)
    DDRB &= ~(1<<0);    //D8
    DDRC &= ~(1<<2);    //A2
    DDRC |= 1<<3;       //A3
    DDRB |= 1<<1;       //D9
    PORTB &= ~(1<<1);   //D9
    PORTC |= 1<<3;      //A3
#elif defined (__AVR_ATmega2560P__)
    DDRH &= ~(1<<5);    //D8
    DDRF &= ~(1<<2);    //A2
    DDRF |= 1<<3;       //A3
    DDRH |= 1<<6;       //D9
    PORTH &= ~(1<<6);   //D9
    PORTF |= 1<<3;      //A3
#else
    b.pin8.direction(INPUT);
    b.pinA2.direction(INPUT);
    b.pinA3.direction(OUTPUT);
    b.pin9.direction(OUTPUT);
    b.pin9.clear();
    b.pinA3.set();
#endif
    for (uint8_t i = 0; i < SAMPLES; i++)
    {
#if defined (__AVR_ATmega328P__)
        samples[i] = _adc->read(Analog2::ADC2);
#elif defined (__AVR_ATmega2560__)
        samples[i] = _adc->read(Analog2::ADC2);
#else
        samples[i] = _adc->read(Analog2::ADC5);
#endif
    }

    if (samples[0] - samples[1] < -4 || samples[0] - samples[1] > 4)
        valid = 0;
    else
        samples[1] = (samples[0] + samples[1]) >> 1;

    uint16_t y = 1023 - samples[SAMPLES/2];
    y -= 90;
    y /= 2.5;
#if defined (__AVR_ATmega328P__)
    DDRB |= 1<<0;       //xp    D8
    DDRC &= ~(1<<3);    //yp    A3
    PORTB &= ~(1<<0);   //xp    D8
    PORTB |= 1<<1;      //ym    D9
    uint16_t z1 = _adc->read(Analog2::ADC2);
    uint16_t z2 = _adc->read(Analog2::ADC3);
#elif defined (__AVR_ATmega2560__)
    DDRH |= 1<<5;       //xp    D8
    DDRF &= ~(1<<3);    //yp    A3
    PORTH &= ~(1<<5);   //xp    D8
    PORTH |= 1<<6;      //ym    D9
    uint16_t z1 = _adc->read(Analog2::ADC2);
    uint16_t z2 = _adc->read(Analog2::ADC3);
#else
    b.pin8.direction(OUTPUT);   //xp
    b.pinA3.direction(INPUT);   //yp
    b.pin8.clear();    //xp
    b.pin9.set();   //ym
    uint16_t z1 = _adc->read(Analog2::ADC5);
    uint16_t z2 = _adc->read(Analog2::ADC4);
#endif
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


