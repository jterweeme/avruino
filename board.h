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
#include "helios.h"
#elif defined (__AVR_ATmega2560__)
#include "mega.h"
#endif


#endif


