#ifndef _CALC_H_
#define _CALC_H_
#include <stdint.h>

class Output
{
public:
    virtual void clear() { }
    virtual void push(char c) { }
    void push(const char *s) { while (*s) push(*s++); }
};

class MyStack
{
private:
    uint8_t _stack[50] = {0};
    uint8_t _pos = 0;
    uint32_t _pow10(uint8_t e) const;
public:
    void push(uint8_t n) { _stack[_pos++] = n; }
    uint8_t pos() const { return _pos; }
    uint32_t get();
    void clear() { _pos = 0; }
};

class Operator
{
public:
    virtual uint32_t perform(uint32_t x, uint32_t y) const { return x; }
};

class Add : public Operator
{
public:
    uint32_t perform(uint32_t x, uint32_t y) const { return x + y; }
};

class Calculator
{
private:
    int32_t _sum = 0;
    int32_t _register = 0;
    Output *_ol;
    MyStack _stack;
    Operator *_op;
    Operator _dum;
    Add _add;
public:
    Calculator(Output *ol) : _ol(ol), _op(&_add) { }
    void push(uint8_t n);
    void op(Operator *op) { equals(); _op = op; }
    void add() { equals(); op(&_add); }
    void equals();
    void reset();
};

class Sub : public Operator
{
public:
    uint32_t perform(uint32_t x, uint32_t y) const { return x - y; }
};

class Mul : public Operator
{
public:
    uint32_t perform(uint32_t x, uint32_t y) const { return x * y; }
};

class Div : public Operator
{
public:
    uint32_t perform(uint32_t x, uint32_t y) const { return x / y; }
};

#endif


