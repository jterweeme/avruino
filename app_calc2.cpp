#include "calc.h"
#include <avr/interrupt.h>
#include "misc.h"
#include <avr/sleep.h>
#include "uart.h"

Calculator *g_calc;
Sub sub;
Div div2;
Mul mul;

#if 0
ISR(USART_RX_vect)
{
    switch (UDR0)
    {
        case '1':
            g_calc->push(1);
            break;
        case '2':
            g_calc->push(2);
            break;
        case '3':
            g_calc->push(3);
            break;
        case '4':
            g_calc->push(4);
            break;
        case '5':
            g_calc->push(5);
            break;
        case '6':
            g_calc->push(6);
            break;
        case '7':
            g_calc->push(7);
            break;
        case '8':
            g_calc->push(8);
            break;
        case '9':
            g_calc->push(9);
            break;
        case '0':
            g_calc->push(0);
            break;
        case '-':
            g_calc->op(&sub);
            break;
        case '*':
            g_calc->op(&mul);
            break;
        case '/':
            g_calc->op(&div2);
            break;
        case '=':
            g_calc->equals();
            break;
        case '+':
            g_calc->add();
            break;
        case 'c':
            g_calc->reset();
            break;
    }
}
#endif

class OutputTerminal : public Output
{
    Uart *_serial;

    char _buf[20] = {0};
    uint8_t _pos = 0;
public:
    OutputTerminal(Uart *s) : _serial(s) { }
    void redraw();
    void clear();
    void push(char c);
};

void OutputTerminal::redraw()
{
    _serial->write('\r');

    for (uint8_t i = 0; i <= 20 - _pos; i++)
        _serial->write(' ');

    for (uint8_t i = 0; i <= _pos; i++)
        _serial->write(_buf[i]);
}

void OutputTerminal::push(char c)
{
    if (_pos >= 19)
        return;

    _buf[_pos++] = c;
    _buf[_pos] = 0;     // null terminate
    redraw();
}

void OutputTerminal::clear()
{
    _buf[_pos = 0] = 0;
    redraw();
}

int main()
{
    Uart s;
    OutputTerminal ot(&s);
    //s.init();
    s.enableRead();
    s.enableReadInterrupt();
    sei();
    Calculator calc(&ot);
    g_calc = &calc;
    calc.reset();

    while (true)
        sleep_mode();

    return 0;
}


