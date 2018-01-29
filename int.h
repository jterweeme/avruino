#ifndef _INT_H_
#define _INT_H_

#define FECTOR(N) __vector_ ## N

#define IZR(vector, ...)            \
    extern "C" void vector (void) __attribute__ ((signal,__INTR_ATTRS)) __VA_ARGS__; \
    void vector (void)

#endif



