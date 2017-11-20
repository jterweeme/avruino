#include "calc.h"
#include <stdio.h>

uint32_t MyStack::_pow10(uint8_t e) const
{
    switch (e)
    {
    case 0:
        return 1;
    case 1:
        return 10;
    case 2:
        return 100;
    case 3:
        return 1000;
    case 4:
        return 10000;
    case 5:
        return 100000;
    default:
        return 1;
    }
}

uint32_t MyStack::get()
{
    uint32_t ret = 0;

    for (uint8_t i = 0; i < _pos; i++)
        ret += _stack[i] * _pow10(_pos - i - 1);

    return ret;
}

void Calculator::equals()
{
    _register = _stack.get();
    _sum = _op->perform(_sum, _register);
    _op = &_dum;
    _ol->clear();
    char buf[16] = {0};
    snprintf(buf, 16, "%u", _sum);
    _ol->push(buf);
    _stack.clear();
}

void Calculator::reset()
{
    _op = &_add;
    _sum = _register = 0;
    _stack.clear();
    _ol->clear();
    _ol->push('0');
}

void Calculator::push(uint8_t x)
{
    if (_stack.pos() == 0)
        _ol->clear();

    _stack.push(x);
    _ol->push(x + '0');
}




