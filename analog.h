#ifndef _ANALOG_H_
#define _ANALOG_H_
#include <stdint.h>
#include <avr/io.h>

class AnalogBase
{
protected:
    volatile uint8_t * const base;
    volatile uint8_t * const adcl;
    volatile uint8_t * const adch;
    volatile uint8_t * const adcsra;
    volatile uint8_t * const adcsrb;
    volatile uint8_t * const admux;
public:
    static const uint8_t BREFS0 = 6;
    static const uint8_t BREFS1 = 7;
    static const uint8_t BADPS0 = 0;
    static const uint8_t BADPS1 = 1;
    static const uint8_t BADPS2 = 2;
    static const uint8_t BADEN = 7;
    static const uint8_t BADATE = 5;
    static const uint8_t BADTS0 = 0;
    static const uint8_t BADTS1 = 1;
    static const uint8_t BADTS2 = 2;
    static const uint8_t BADSC = 6;
    static const uint8_t RREFS0 = 1 << BREFS0;
    AnalogBase(uint8_t *base);
    unsigned read() { return *adcl | ((unsigned)*adch << 8); }
};

class Analog : public AnalogBase
{
public:
    Analog();
};

class Analog2
{
public:
    static constexpr uint8_t ADC0 = 0,
        ADC1 = 1<<MUX0,
        ADC2 = 1<<MUX1,
        ADC3 = 1<<MUX0 | 1<<MUX1,
        ADC4 = 1<<MUX2,
        ADC5 = 1<<MUX0 | 1<<MUX2;
    void init();
    uint16_t read(uint8_t input);
};


struct TSPoint
{
    uint16_t x = 0, y = 0, z = 0;
    TSPoint() { }
    TSPoint(uint16_t x1, uint16_t y1, uint16_t z1) : x(x1), y(y1), z(z1) { }
};

class TouchScreen
{
private:
    Analog2 * const _adc;
    uint16_t _threshold = 200;
public:
    TouchScreen(Analog2 *adc) : _adc(adc) { }
    static const uint8_t SAMPLES = 2;
    TSPoint getPoint();
};

class DFKeyPad
{
    AnalogBase &adc;
public:
    static const uint8_t SELECT = 1, UP = 2, LEFT = 3, RIGHT = 4, DOWN = 5;
    DFKeyPad(AnalogBase &adc) : adc(adc) { }
    uint8_t poll();
};
#endif



