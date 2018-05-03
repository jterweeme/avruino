/*
App_pong1.cpp

RGB: TX0
HSYNC: OCR2B
VSYNC: OCR1B

Uno:
RGB: TX0/#1
HSync: #3
VSync: #10

Mega:
RGB: #1
HSync: #9
VSync: #12
*/

#include <avr/interrupt.h>
#include "pgmspees.h"
#include "sleepy.h"
#include <stdio.h>
#include "board.h"

static const uint8_t screen_font[8][256] PROGMEM = 
{
  { 
  0xFF, 0xC0, 0xC0, 0xC9, 0xF7, 0xE3, 0xF7, 0xFF, 0x80, 0xFF, 0x80, 0xF8, 0xE1, 0xE0, 0xC0, 0xB3, 
  0xBF, 0xFE, 0xF3, 0xCC, 0xC0, 0xE0, 0xFF, 0xF3, 0xF3, 0xF3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xE7, 0xC9, 0xC9, 0xE7, 0xFF, 0xE3, 0xCF, 0xF3, 0xCF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 
  0xC1, 0xE7, 0xC3, 0xC3, 0xF1, 0x81, 0xE3, 0x81, 0xC3, 0xC3, 0xFF, 0xFF, 0xF3, 0xFF, 0xCF, 0xC3, 
  0xC1, 0xE7, 0x81, 0xE1, 0x83, 0x80, 0x80, 0xE1, 0x99, 0xC3, 0xF0, 0x8C, 0x87, 0x9C, 0x9C, 0xE3, 
  0x81, 0xC3, 0x81, 0xC3, 0x81, 0x99, 0x99, 0x9C, 0x9C, 0x99, 0x80, 0xC3, 0x9F, 0xC3, 0xF7, 0xFF, 
  0xE7, 0xFF, 0x8F, 0xFF, 0xF1, 0xFF, 0xE3, 0xFF, 0x8F, 0xE7, 0xF9, 0x8F, 0xC7, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xF7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF1, 0xF3, 0x8F, 0xC4, 0xFF, 
  0xC3, 0xFF, 0xF1, 0xC0, 0x99, 0x8F, 0xE7, 0xFF, 0xC0, 0x99, 0x8F, 0x99, 0xC1, 0x8F, 0x9C, 0xE7, 
  0xF1, 0xFF, 0xE0, 0xC3, 0xFF, 0xFF, 0xC3, 0xFF, 0xFF, 0x9E, 0x99, 0xF3, 0xE3, 0x99, 0x83, 0xF8, 
  0xF1, 0xE3, 0xFF, 0xFF, 0xFF, 0x81, 0xE1, 0xE3, 0xE7, 0xFF, 0xFF, 0x9E, 0x9E, 0xF3, 0xFF, 0xFF, 
  0xEE, 0xD5, 0x92, 0xF3, 0xF3, 0xF3, 0xE4, 0xFF, 0xFF, 0xE4, 0xE4, 0xFF, 0xE4, 0xE4, 0xF3, 0xFF, 
  0xF3, 0xF3, 0xFF, 0xF3, 0xFF, 0xF3, 0xF3, 0xE4, 0xE4, 0xFF, 0xE4, 0xFF, 0xE4, 0xFF, 0xE4, 0xF3, 
  0xE4, 0xFF, 0xFF, 0xE4, 0xF3, 0xFF, 0xFF, 0xE4, 0xF3, 0xF3, 0xFF, 0x80, 0xFF, 0x87, 0xF8, 0x80, 
  0xFF, 0xFF, 0xFF, 0xFF, 0x81, 0xFF, 0xFF, 0xFF, 0x81, 0xE3, 0xE3, 0xF1, 0xFF, 0xFC, 0xE3, 0xC3, 
  0xFF, 0xE7, 0xCF, 0xF3, 0xF8, 0xF3, 0xE7, 0xFF, 0xE3, 0xFF, 0xFF, 0xF8, 0xC3, 0xC7, 0xFF, 0xFF, 
  }, 
  { 
  0xFF, 0xBF, 0x80, 0x80, 0xE3, 0xC1, 0xF7, 0xFF, 0x80, 0xE1, 0x9E, 0xFC, 0xCC, 0xE6, 0xCE, 0xD2, 
  0x8F, 0xF8, 0xE1, 0xCC, 0x92, 0xCE, 0xFF, 0xE1, 0xE1, 0xF3, 0xF3, 0xE7, 0xFF, 0xED, 0xF3, 0x80, 
  0xFF, 0xC3, 0xC9, 0xC9, 0xC1, 0x9C, 0xC9, 0xCF, 0xE7, 0xE7, 0xCC, 0xE7, 0xFF, 0xFF, 0xFF, 0xF9, 
  0x9C, 0xC7, 0x99, 0x99, 0xE1, 0x9F, 0xCF, 0x99, 0x99, 0x99, 0xE7, 0xE7, 0xE7, 0xFF, 0xE7, 0x99, 
  0x9C, 0xC3, 0xCC, 0xCC, 0xC9, 0xCE, 0xCE, 0xCC, 0x99, 0xE7, 0xF9, 0xCC, 0xCF, 0x88, 0x8C, 0xC9, 
  0xCC, 0x99, 0xCC, 0x99, 0xA5, 0x99, 0x99, 0x9C, 0x9C, 0x99, 0x9C, 0xCF, 0xCF, 0xF3, 0xE3, 0xFF, 
  0xE7, 0xFF, 0xCF, 0xFF, 0xF9, 0xFF, 0xC9, 0xFF, 0xCF, 0xFF, 0xFF, 0xCF, 0xE7, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xE7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE7, 0xF3, 0xE7, 0x91, 0xF7, 
  0x99, 0x99, 0xFF, 0x9E, 0xFF, 0xFF, 0xE7, 0xFF, 0x9E, 0xFF, 0xFF, 0xFF, 0x9C, 0xFF, 0xE3, 0xE7, 
  0xFF, 0xFF, 0xC9, 0x99, 0x99, 0x8F, 0x99, 0x8F, 0x99, 0xF3, 0xFF, 0xF3, 0xC9, 0x99, 0x99, 0xF2, 
  0xFF, 0xFF, 0xF1, 0xF1, 0x83, 0xFF, 0xC9, 0xC9, 0xFF, 0xFF, 0xFF, 0x9C, 0x9C, 0xF3, 0xE6, 0x99, 
  0xBB, 0xAA, 0xC4, 0xF3, 0xF3, 0xF3, 0xE4, 0xFF, 0xFF, 0xE4, 0xE4, 0xFF, 0xE4, 0xE4, 0xF3, 0xFF, 
  0xF3, 0xF3, 0xFF, 0xF3, 0xFF, 0xF3, 0xF3, 0xE4, 0xE4, 0xFF, 0xE4, 0xFF, 0xE4, 0xFF, 0xE4, 0xF3, 
  0xE4, 0xFF, 0xFF, 0xE4, 0xF3, 0xFF, 0xFF, 0xE4, 0xF3, 0xF3, 0xFF, 0x80, 0xFF, 0x87, 0xF8, 0x80, 
  0xFF, 0xC3, 0x81, 0x80, 0x99, 0xFF, 0xCC, 0xC4, 0xE7, 0xC9, 0xC9, 0xE7, 0xFF, 0xF9, 0xCF, 0x99, 
  0x81, 0xE7, 0xE7, 0xE7, 0xF2, 0xF3, 0xE7, 0xC4, 0xC9, 0xFF, 0xFF, 0xF9, 0xC9, 0xF3, 0xFF, 0xFF, 
  }, 
  { 
  0xFF, 0xAD, 0x92, 0x80, 0xC1, 0xE3, 0xE3, 0xF3, 0x8C, 0xCC, 0xB3, 0xF8, 0xCC, 0xE0, 0xC0, 0xE1, 
  0x83, 0xE0, 0xC0, 0xCC, 0x92, 0xE3, 0xFF, 0xC0, 0xC0, 0xF3, 0xF9, 0xCF, 0x9F, 0xCC, 0xE1, 0x80, 
  0xFF, 0xC3, 0xC9, 0x80, 0x9F, 0x99, 0xE3, 0x9F, 0xCF, 0xF3, 0xE1, 0xE7, 0xFF, 0xFF, 0xFF, 0xF3, 
  0x98, 0xE7, 0xF9, 0xF9, 0xC9, 0x83, 0x9F, 0xF9, 0x99, 0x99, 0xE7, 0xE7, 0xCF, 0x81, 0xF3, 0xF9, 
  0x90, 0x99, 0xCC, 0x9F, 0xCC, 0xCB, 0xCB, 0x9F, 0x99, 0xE7, 0xF9, 0xC9, 0xCF, 0x80, 0x84, 0x9C, 
  0xCC, 0x99, 0xCC, 0x8F, 0xE7, 0x99, 0x99, 0x9C, 0xC9, 0x99, 0xB9, 0xCF, 0xE7, 0xF3, 0xC9, 0xFF, 
  0xF3, 0xC3, 0xCF, 0xC3, 0xF9, 0xC3, 0xCF, 0xC4, 0xC9, 0xC7, 0xF9, 0xCC, 0xE7, 0x99, 0x83, 0xC3, 
  0x91, 0xC4, 0x91, 0xC1, 0xC1, 0x99, 0x99, 0x9C, 0x9C, 0x99, 0x81, 0xE7, 0xF3, 0xE7, 0xFF, 0xE3, 
  0x9F, 0xFF, 0xC3, 0xE1, 0xC3, 0xC3, 0xC3, 0xC3, 0xE1, 0xC3, 0xC3, 0xC7, 0xE3, 0xC7, 0xC9, 0xFF, 
  0x81, 0xC0, 0x99, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE1, 0x99, 0xC0, 0xCD, 0xC3, 0x99, 0xF3, 
  0xC3, 0xC7, 0xFF, 0xFF, 0xFF, 0x99, 0xC9, 0xC9, 0xE7, 0xFF, 0xFF, 0x99, 0x99, 0xFF, 0xCC, 0xCC, 
  0xEE, 0xD5, 0x92, 0xF3, 0xF3, 0x83, 0xE4, 0xFF, 0x83, 0x84, 0xE4, 0x80, 0x84, 0xE4, 0x83, 0xFF, 
  0xF3, 0xF3, 0xFF, 0xF3, 0xFF, 0xF3, 0xF0, 0xE4, 0xE4, 0xE0, 0x84, 0x80, 0xE4, 0x80, 0x84, 0x80, 
  0xE4, 0x80, 0xFF, 0xE4, 0xF0, 0xF0, 0xFF, 0xE4, 0x80, 0xF3, 0xFF, 0x80, 0xFF, 0x87, 0xF8, 0x80, 
  0xC4, 0x99, 0x99, 0xC9, 0xCF, 0xC0, 0xCC, 0x91, 0xC3, 0x9C, 0x9C, 0xF3, 0xC0, 0xC0, 0x9F, 0x99, 
  0xFF, 0x81, 0xF3, 0xCF, 0xF2, 0xF3, 0xFF, 0x91, 0xC9, 0xFF, 0xFF, 0xF9, 0xC9, 0xE7, 0xE1, 0xFF, 
  }, 
  { 
  0xFF, 0xBF, 0x80, 0x80, 0x80, 0x80, 0xC1, 0xE1, 0x9E, 0xDE, 0xA1, 0xC1, 0xCC, 0xE7, 0xCE, 0x8C, 
  0x80, 0x80, 0xF3, 0xCC, 0xC2, 0xC9, 0xFF, 0xF3, 0xF3, 0xF3, 0x80, 0x80, 0x9F, 0x80, 0xC0, 0xC0, 
  0xFF, 0xE7, 0xFF, 0xC9, 0xC3, 0xF3, 0xC4, 0xFF, 0xCF, 0xF3, 0x80, 0x81, 0xFF, 0x81, 0xFF, 0xE7, 
  0x90, 0xE7, 0xE3, 0xE3, 0x99, 0xF9, 0x83, 0xF3, 0xC3, 0xC1, 0xFF, 0xFF, 0x9F, 0xFF, 0xF9, 0xF3, 
  0x90, 0x99, 0xC1, 0x9F, 0xCC, 0xC3, 0xC3, 0x9F, 0x81, 0xE7, 0xF9, 0xC3, 0xCF, 0x80, 0x90, 0x9C, 
  0xC1, 0x99, 0xC1, 0xC7, 0xE7, 0x99, 0x99, 0x94, 0xE3, 0xC3, 0xF3, 0xCF, 0xF3, 0xF3, 0x9C, 0xFF, 
  0xFF, 0xF9, 0xC1, 0x99, 0xC1, 0x99, 0x87, 0x99, 0xC4, 0xE7, 0xF9, 0xC9, 0xE7, 0x80, 0x99, 0x99, 
  0xCC, 0x99, 0xC4, 0x9F, 0xE7, 0x99, 0x99, 0x94, 0xC9, 0x99, 0xB3, 0x8F, 0xFF, 0xF1, 0xFF, 0xC9, 
  0x99, 0x99, 0x99, 0xFC, 0xF9, 0xF9, 0xF9, 0x9F, 0xCC, 0x99, 0x99, 0xE7, 0xF3, 0xE7, 0x9C, 0xC3, 
  0xCF, 0xF9, 0x80, 0xC3, 0xC3, 0xC3, 0x99, 0x99, 0x99, 0xCC, 0x99, 0x9F, 0x87, 0x81, 0x82, 0xE1, 
  0xF9, 0xE7, 0xC3, 0x99, 0x83, 0x89, 0xE0, 0xE3, 0xCF, 0x81, 0x81, 0x90, 0x92, 0xF3, 0x99, 0xE6, 
  0xBB, 0xAA, 0x88, 0xF3, 0xF3, 0xF3, 0xE4, 0xFF, 0xF3, 0xFC, 0xE4, 0xFC, 0xFC, 0xE4, 0xF3, 0xFF, 
  0xF3, 0xF3, 0xFF, 0xF3, 0xFF, 0xF3, 0xF3, 0xE4, 0xE7, 0xE7, 0xFF, 0xFF, 0xE7, 0xFF, 0xFF, 0xFF, 
  0xE4, 0xFF, 0xFF, 0xE4, 0xF3, 0xF3, 0xFF, 0xE4, 0xF3, 0xF3, 0xFF, 0x80, 0xFF, 0x87, 0xF8, 0x80, 
  0x91, 0x83, 0x9F, 0xC9, 0xE7, 0x93, 0xCC, 0xF3, 0x99, 0x80, 0x9C, 0xC1, 0x92, 0x92, 0x83, 0x99, 
  0x81, 0xE7, 0xE7, 0xE7, 0xF3, 0xF3, 0x81, 0xFF, 0xE3, 0xF3, 0xFF, 0xF9, 0xC9, 0xCF, 0xE1, 0xFF, 
  }, 
  { 
  0xFF, 0xA1, 0x9E, 0xC1, 0xC1, 0x80, 0x80, 0xE1, 0x9E, 0xDE, 0xA1, 0x99, 0xE1, 0xE7, 0xCE, 0x8C, 
  0x83, 0xE0, 0xF3, 0xCC, 0xF2, 0xC9, 0xC0, 0xC0, 0xF3, 0xC0, 0xF9, 0xCF, 0x9F, 0xCC, 0x80, 0xE1, 
  0xFF, 0xE7, 0xFF, 0x80, 0xF9, 0xE7, 0x91, 0xFF, 0xCF, 0xF3, 0xE1, 0xE7, 0xFF, 0xFF, 0xFF, 0xCF, 
  0x84, 0xE7, 0xCF, 0xF9, 0x80, 0xF9, 0x99, 0xE7, 0x99, 0xF9, 0xFF, 0xFF, 0xCF, 0xFF, 0xF3, 0xE7, 
  0x90, 0x81, 0xCC, 0x9F, 0xCC, 0xCB, 0xCB, 0x98, 0x99, 0xE7, 0x99, 0xC9, 0xCE, 0x94, 0x98, 0x9C, 
  0xCF, 0x91, 0xC9, 0xF1, 0xE7, 0x99, 0x99, 0x80, 0xE3, 0xE7, 0xE6, 0xCF, 0xF9, 0xF3, 0xFF, 0xFF, 
  0xFF, 0xC1, 0xCC, 0x9F, 0x99, 0x81, 0xCF, 0x99, 0xCC, 0xE7, 0xF9, 0xC3, 0xE7, 0x80, 0x99, 0x99, 
  0xCC, 0x99, 0xCC, 0xC3, 0xE7, 0x99, 0x99, 0x80, 0xE3, 0x99, 0xE7, 0xE7, 0xF3, 0xE7, 0xFF, 0x9C, 
  0xC3, 0x99, 0x81, 0xE0, 0xC1, 0xC1, 0xC1, 0x9F, 0xC0, 0x81, 0x81, 0xE7, 0xF3, 0xE7, 0x80, 0x99, 
  0xC3, 0xC0, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0xCC, 0x99, 0x9F, 0xCF, 0xE7, 0x9C, 0xF3, 
  0xC1, 0xE7, 0x99, 0x99, 0x99, 0x81, 0xFF, 0xFF, 0x9F, 0x9F, 0xF9, 0xE6, 0xE4, 0xF3, 0xCC, 0xCC, 
  0xEE, 0xD5, 0x92, 0xF3, 0x83, 0x83, 0x84, 0x80, 0x83, 0x84, 0xE4, 0x84, 0x80, 0x80, 0x83, 0x83, 
  0xF0, 0x80, 0x80, 0xF0, 0x80, 0x80, 0xF0, 0xE4, 0xE0, 0xE4, 0x80, 0x84, 0xE4, 0x80, 0x84, 0x80, 
  0x80, 0x80, 0x80, 0xE0, 0xF0, 0xF0, 0xE0, 0x80, 0x80, 0x83, 0xF0, 0x80, 0x80, 0x87, 0xF8, 0xFF, 
  0x9B, 0x99, 0x9F, 0xC9, 0xCF, 0x93, 0xCC, 0xF3, 0x99, 0x9C, 0xC9, 0x99, 0x92, 0x92, 0x9F, 0x99, 
  0xFF, 0xE7, 0xCF, 0xF3, 0xF3, 0xF3, 0xFF, 0xC4, 0xFF, 0xF3, 0xF3, 0x89, 0xC9, 0xC3, 0xE1, 0xFF, 
  }, 
  { 
  0xFF, 0xB3, 0x8C, 0xE3, 0xE3, 0xC1, 0xC1, 0xF3, 0x8C, 0xCC, 0xB3, 0x99, 0xF3, 0xC7, 0xCC, 0xE1, 
  0x8F, 0xF8, 0xC0, 0xFF, 0xF2, 0xE3, 0xC0, 0xE1, 0xF3, 0xE1, 0xF3, 0xE7, 0x80, 0xED, 0x80, 0xF3, 
  0xFF, 0xFF, 0xFF, 0xC9, 0x83, 0xCC, 0x99, 0xFF, 0xE7, 0xE7, 0xCC, 0xE7, 0xE7, 0xFF, 0xE7, 0x9F, 
  0x8C, 0xE7, 0x99, 0x99, 0xF9, 0x99, 0x99, 0xE7, 0x99, 0xF3, 0xE7, 0xE7, 0xE7, 0x81, 0xE7, 0xFF, 
  0x9F, 0x99, 0xCC, 0xCC, 0xC9, 0xCE, 0xCF, 0xCC, 0x99, 0xE7, 0x99, 0xCC, 0xCC, 0x9C, 0x9C, 0xC9, 
  0xCF, 0xC3, 0xCC, 0x99, 0xE7, 0x99, 0xC3, 0x88, 0xC9, 0xE7, 0xCC, 0xCF, 0xFC, 0xF3, 0xFF, 0xFF, 
  0xFF, 0x99, 0xCC, 0x99, 0x99, 0x9F, 0xCF, 0xC1, 0xCC, 0xE7, 0x99, 0xC9, 0xE7, 0x94, 0x99, 0x99, 
  0xC1, 0xC1, 0xCF, 0xF9, 0xE5, 0x99, 0xC3, 0x80, 0xC9, 0xC1, 0xCD, 0xE7, 0xF3, 0xE7, 0xFF, 0x9C, 
  0xF3, 0x99, 0x9F, 0xCC, 0x99, 0x99, 0x99, 0xC3, 0xCF, 0x9F, 0x9F, 0xE7, 0xF3, 0xE7, 0x9C, 0x81, 
  0xCF, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0xC1, 0xE1, 0x99, 0xC0, 0x8C, 0x81, 0x98, 0xF3, 
  0x99, 0xE7, 0x99, 0x99, 0x99, 0x91, 0xC0, 0xC1, 0x99, 0x9F, 0xF9, 0xCC, 0xC8, 0xF3, 0xE6, 0x99, 
  0xBB, 0xAA, 0xC4, 0xF3, 0xF3, 0xF3, 0xE4, 0xE4, 0xF3, 0xE4, 0xE4, 0xE4, 0xFF, 0xFF, 0xFF, 0xF3, 
  0xFF, 0xFF, 0xF3, 0xF3, 0xFF, 0xF3, 0xF3, 0xE4, 0xFF, 0xE4, 0xFF, 0xE4, 0xE4, 0xFF, 0xE4, 0xFF, 
  0xFF, 0xF3, 0xE4, 0xFF, 0xFF, 0xF3, 0xE4, 0xE4, 0xF3, 0xFF, 0xF3, 0x80, 0x80, 0x87, 0xF8, 0xFF, 
  0x91, 0x83, 0x9F, 0xC9, 0x99, 0x93, 0xC1, 0xF3, 0xC3, 0xC9, 0xC9, 0x99, 0xC0, 0xC0, 0xCF, 0x99, 
  0x81, 0xFF, 0xFF, 0xFF, 0xF3, 0x93, 0xE7, 0x91, 0xFF, 0xFF, 0xFF, 0xC9, 0xFF, 0xFF, 0xE1, 0xFF, 
  }, 
  { 
  0xFF, 0xBF, 0x80, 0xF7, 0xF7, 0xE3, 0xE3, 0xFF, 0x80, 0xE1, 0x9E, 0x99, 0xC0, 0x87, 0x8C, 0xD2, 
  0xBF, 0xFE, 0xE1, 0xCC, 0xF2, 0x99, 0xC0, 0xF3, 0xF3, 0xF3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xE7, 0xFF, 0xC9, 0xE7, 0x9C, 0xC4, 0xFF, 0xF3, 0xCF, 0xFF, 0xFF, 0xE7, 0xFF, 0xE7, 0xBF, 
  0xC1, 0x81, 0x81, 0xC3, 0xF0, 0xC3, 0xC3, 0xE7, 0xC3, 0xC7, 0xE7, 0xE7, 0xF3, 0xFF, 0xCF, 0xE7, 
  0xC3, 0x99, 0x81, 0xE1, 0x83, 0x80, 0x87, 0xE0, 0x99, 0xC3, 0xC3, 0x8C, 0x80, 0x9C, 0x9C, 0xE3, 
  0x87, 0xF1, 0x8C, 0xC3, 0xC3, 0x81, 0xE7, 0x9C, 0x9C, 0xC3, 0x80, 0xC3, 0xFE, 0xC3, 0xFF, 0xFF, 
  0xFF, 0xC4, 0x91, 0xC3, 0xC4, 0xC3, 0x87, 0xF9, 0x8C, 0xC3, 0x99, 0x8C, 0xC3, 0x9C, 0x99, 0xC3, 
  0xCF, 0xF9, 0x87, 0x83, 0xF3, 0xC4, 0xE7, 0xC9, 0x9C, 0xF9, 0x81, 0xF1, 0xF3, 0x8F, 0xFF, 0x80, 
  0xF9, 0xC0, 0xC3, 0xE0, 0xC0, 0xC0, 0xC0, 0xF9, 0xE1, 0xC3, 0xC3, 0xC3, 0xE1, 0xC3, 0x9C, 0x99, 
  0x81, 0xC0, 0x98, 0xC3, 0xC3, 0xC3, 0xC0, 0xC0, 0xF9, 0xF3, 0xC3, 0xF3, 0x81, 0xE7, 0x9C, 0x93, 
  0xC0, 0xC3, 0xC3, 0xC0, 0x99, 0x99, 0xFF, 0xFF, 0xC3, 0xFF, 0xFF, 0x99, 0x98, 0xF3, 0xFF, 0xFF, 
  0xEE, 0xD5, 0x92, 0xF3, 0xF3, 0xF3, 0xE4, 0xE4, 0xF3, 0xE4, 0xE4, 0xE4, 0xFF, 0xFF, 0xFF, 0xF3, 
  0xFF, 0xFF, 0xF3, 0xF3, 0xFF, 0xF3, 0xF3, 0xE4, 0xFF, 0xE4, 0xFF, 0xE4, 0xE4, 0xFF, 0xE4, 0xFF, 
  0xFF, 0xF3, 0xE4, 0xFF, 0xFF, 0xF3, 0xE4, 0xE4, 0xF3, 0xFF, 0xF3, 0x80, 0x80, 0x87, 0xF8, 0xFF, 
  0xC4, 0x9F, 0x9F, 0xC9, 0x81, 0xC7, 0xCF, 0xF3, 0xE7, 0xE3, 0x88, 0xC3, 0xFF, 0xCF, 0xE3, 0x99, 
  0xFF, 0x81, 0x81, 0x81, 0xF3, 0x93, 0xE7, 0xFF, 0xFF, 0xFF, 0xFF, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 
  }, 
  { 
  0xFF, 0xC0, 0xC0, 0xFF, 0xFF, 0xC1, 0xC1, 0xFF, 0x80, 0xFF, 0x80, 0xC3, 0xF3, 0x8F, 0x9F, 0xB3, 
  0xFF, 0xFF, 0xF3, 0xFF, 0xFF, 0xC3, 0xFF, 0x80, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xCF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xCF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x83, 0xFF, 0xFF, 0xC3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0x87, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x83, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xC3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x83, 0xFF, 0xFF, 0xF3, 0xFF, 0xE7, 0x9C, 0xC7, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0xFE, 0xFF, 0xFF, 0xFF, 
  0xBB, 0xAA, 0x88, 0xF3, 0xF3, 0xF3, 0xE4, 0xE4, 0xF3, 0xE4, 0xE4, 0xE4, 0xFF, 0xFF, 0xFF, 0xF3, 
  0xFF, 0xFF, 0xF3, 0xF3, 0xFF, 0xF3, 0xF3, 0xE4, 0xFF, 0xE4, 0xFF, 0xE4, 0xE4, 0xFF, 0xE4, 0xFF, 
  0xFF, 0xF3, 0xE4, 0xFF, 0xFF, 0xF3, 0xE4, 0xE4, 0xF3, 0xFF, 0xF3, 0x80, 0x80, 0x87, 0xF8, 0xFF, 
  0xFF, 0x9F, 0xFF, 0xFF, 0xFF, 0xFF, 0x9F, 0xFF, 0x81, 0xFF, 0xFF, 0xFF, 0xFF, 0x9F, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xF3, 0xC7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 
  }, 
};

class Display
{
public:
    Display();
    void scanLine();
    void interrupt();
    char message[30][20];
private:
    volatile int messageLine;
    volatile int vLine;
    volatile uint8_t backPorchLinesToGo;
};

static Display display;

static volatile uint16_t joy1;

ISR(ADC_vect)
{
    joy1 = ADCL | (uint16_t)ADCH << 8;
    ADCSRA |= 1<<ADSC;  // restart conversion
}

ISR(TIMER1_OVF_vect)
{
    display.interrupt();
}

Display::Display()
{
    for (int i = 0; i < 30; i++)
        ::sprintf(message[i], "Line %03i - hello!", i);

    zei();
    *p_timsk0 = 0;
    *p_ocr0a = 0;
    *p_ocr0b = 0;
    *p_ocr1b_ddr |= 1<<ocr1b_bit;
    *p_tccr1a = 1<<wgm10 | 1<<wgm11 | 1<<com1b1;  
    *p_tccr1b = 1<<wgm12 | 1<<wgm13 | 5;
    *p_ocr1a = 259;
    *p_ocr1b = 0;
    *p_tifr1 = 1<<tov1;
    *p_timsk1 = 1<<toie1;
    *p_ocr2b_ddr |= 1<<ocr2b_bit;
    *p_tccr2a = 1<<wgm20 | 1<<wgm21 | 1<<com2b1;
    *p_tccr2b = 1<<wgm22 | 2;
    *p_ocr2a = 63;
    *p_ocr2b = 7;
    *p_tifr2 = 1<<tov2;
    *p_timsk2 = 1<<toie2;
    *p_ubrr9 = 0;
    //*p_ddrd |= 1<<4;
    *p_ucsr9b = 0; 
    *p_ucsr9c = 1<<umsel90 | 1<<umsel91 | 1<<ucpol9; // 1<<ucpha0?
    set_sleep_mode(SLEEP_MODE_IDLE);
}

void Display::interrupt()
{
    vLine = 0;
    messageLine = 0;
    backPorchLinesToGo = 35;
}
  
ISR(TIMER2_OVF_vect)
{
}

void Display::scanLine()
{    
    if (backPorchLinesToGo)
    {
        backPorchLinesToGo--;
        return;   
    }
    
    if (vLine >= 480)
        return;
    
    const register uint8_t *linePtr = &screen_font[vLine>>1 & 0x07][0];
    char *messagePtr = &(message[messageLine][0]);
    uint8_t i = 20;
    *p_ucsr9b = 1<<txen9;

    while (i--)
        *p_udr9 = pgm_read_byte(linePtr + (*messagePtr++));

    while ((*p_ucsr9a & 1<<txc9) == 0)
        ;
  
    *p_ucsr9b = 0;
    vLine++;  
  
    if ((vLine & 0xF) == 0)
        messageLine++;
}

int main()
{
    sei();
    ADMUX = 1<<REFS0;
    ADCSRA = 1<<ADPS2 | 1<<ADPS1 | 1<<ADPS0 | 1<<ADIE | 1<<ADEN;

    while (true)
    {
        sleep_mode();
        display.scanLine();
    }

    return 0;
}



