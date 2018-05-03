/*
Jasper ter Weeme
*/

#ifndef _BOARD_H_
#define _BOARD_H_

#if defined (__AVR_ATmega328P__)
#include "uno.h"
#elif defined (__AVR_ATmega8__)
#include "ocho.h"
#elif defined (__AVR_ATmega32U4__)
#include "leonardo.h"
#elif defined (__AVR_ATmega2560__)
#include "mega.h"
#elif defined (__AVR_AT90USB1286__)
#include "teensy20pp.h"
#elif defined (__AVR_ATmega16U2__)
#include "zestien.h"
#endif

#define zei()  __asm__ __volatile__ ("sei" ::: "memory")
#define zli()  __asm__ __volatile__ ("cli" ::: "memory")

#endif


