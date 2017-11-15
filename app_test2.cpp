#include <avr/interrupt.h>
class Observer
{
public:
    virtual void update() {
    PORTB ^= (1<<5);
    TCNT0 = 0;
    }
};

class Timer
{
};

class Timer0 : public Timer
{
};

class TimerTick : public Observer
{
public:
    void update();
};

class Timer1 //: public Timer
{
    volatile uint16_t * const counter;
    volatile uint8_t * const tifr;
    volatile uint8_t * const timsk;
    volatile uint8_t * const tccra;
    volatile uint8_t * const tccrb;
    static Timer1 *instance;
    Observer *observer;
public:
    void isr() { observer->update(); }
    Timer1(Observer *observer);
    static Timer1 *getInstance() { return instance; }
};

class App
{
    TimerTick tt;
    Timer1 timer1;
public:
    App();
};

App::App() : timer1(&tt)
{
    DDRB |= (1<<5);
}

Timer1::Timer1(Observer *observer)
  :
    counter((uint16_t *)0x84),
    tifr((uint8_t *)0x36),
    timsk((uint8_t *)0x6f),
    tccra((uint8_t *)0x80),
    tccrb((uint8_t *)0x81),
    observer(observer)
{
    *tifr = 0;
    *tccrb |= (1<<CS12) | (1<<CS10);
    *timsk |= (1<<TOIE1);
    sei();
}

void TimerTick::update()
{
    PORTB ^= (1<<5);
    TCNT0 = 0;
}

extern "C" void __vector_13() __attribute__ ((signal, used, externally_visible));

void __vector_13()
{
    Timer1::getInstance()->isr();
}

Timer1 *Timer1::instance;

int main()
{
    App app;

    while (true) {
    }

    return 0;
}



