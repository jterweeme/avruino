/*
Jasper ter Weeme
*/

#ifndef _BOARD_H_
#define _BOARD_H_

#define UNO 1
#define HELIOS 2
#define MEGA 3

// Default Arduino Uno
#ifndef BOARD
#define BOARD UNO
#endif

#if defined (__AVR_ATmega328P__)
#include "uno.h"
#elif defined (__AVR_ATmega8__)
#include "uno.h"
#elif defined (__AVR_ATmega32U4__)
#include "leonardo.h"               // kan ook helios zijn!
#elif defined (__AVR_ATmega2560__)
#include "mega.h"
#elif defined (__AVR_AT90USB1286__)
#include "twaalf.h"
#endif

#define zei()  __asm__ __volatile__ ("sei" ::: "memory")
#define zli()  __asm__ __volatile__ ("cli" ::: "memory")

#endif


