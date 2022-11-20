/*
Jasper ter Weeme
*/

#ifndef _BOARD_H_
#define _BOARD_H_

//deze code kiest het bord aan de hand van opgegeven MCU

#if defined (__AVR_ATmega328P__)
#include "brd_uno.h"
#elif defined (__AVR_ATmega8__)
#include "brd_ocho.h"
#elif defined (__AVR_ATmega32U4__)
#include "brd_leonardo.h"
#elif defined (__AVR_ATmega2560__)
#include "mega.h"
#elif defined (__AVR_AT90USB1286__)
#include "brd_teensy20pp.h"
#elif defined (__AVR_ATmega16U2__)
#include "zestien.h"
#elif defined (__AVR_ATtiny85__)
#include "attiny85.h"
#elif defined (__AVR_ATtiny13__)
#include "brd_attiny13.h"
#endif

#endif


