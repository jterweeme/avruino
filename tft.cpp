// code from Adafruit TFT library

#ifndef F_CPU
#define F_CPU 16000000L
#endif

#include <util/delay.h>
#include "glcdfont.h"
#include "tft.h"
#include "board.h"
#include "misc.h"

static const uint16_t TFTLCD_DELAY = 0xffff;

#if 0
template <class T> static void zwap(T &a, T &b)
{
    T c(a);
    a = b;
    b = c;
}
#else
void zwap(int16_t &a, int16_t &b)
{
    int16_t c = a;
    a = b;
    b = c;
}
#endif

int zabs(int n)
{
    return n < 0 ? -n : n;
}

void TFT::init_table16(const void *table, int16_t size)
{
    uint16_t *p = (uint16_t *)table;

    while (size > 0)
    {
        uint16_t cmd = pgm_read_word(p++);
        uint16_t d = pgm_read_word(p++);

        if (cmd == TFTLCD_DELAY)
        {
            //_delay_ms(0xff);
        }
        else
        {
            *p_pinA3_port &= ~(1<<pinA3_bit);
            writeCmd(cmd);
            writeData(d);
            *p_pinA3_port |= 1<<pinA3_bit;
        }
        size -= 2 * sizeof(int16_t);
    }
}

void TFT::fillScreen(uint16_t color)
{
    fillRect(0, 0, _width, _height, color);
}

void TFT::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    drawHLine(x, y, w, color);
    drawHLine(x, y+h-1, w, color);
    drawVLine(x, y, h, color);
    drawVLine(x+w-1, y, h, color);
}

void TFT::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    if (x < 0 || y < 0 || x >= width() || y >= height())
        return;

    *p_pinA2_ddr |= 1<<pinA2_bit;
    *p_pinA3_ddr |= 1<<pinA3_bit;
    writeCmdData(_MC, x);
    writeCmdData(_MP, y);
    writeCmdData(_MW, color);
}

void TFT::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
    int16_t steep = zabs(y1 - y0) > zabs(x1 - x0);

    if (steep)
    {
        zwap(x0, y0);
        zwap(x1, y1);
    }

    if (x0 > x1)
    {
        zwap(x0, x1);
        zwap(y0, y1);
    }

    int16_t dx = x1 - x0;
    int16_t dy = zabs(y1 - y0);
    int16_t err = dx / 2;
    int16_t ystep = y0 < y1 ? 1 : -1;

    for (; x0<=x1; x0++)
    {
        if (steep)
            drawPixel(y0, x0, color);
        else
            drawPixel(x0, y0, color);

        err -= dy;

        if (err < 0)
        {
            y0 += ystep;
            err += dx;
        }
    }
}

void TFT::drawHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
    drawLine(x, y, x + w - 1, y, color);
}

void TFT::drawVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
    drawLine(x, y, x, y + h - 1, color);
}

void TFT::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    for (int16_t i = x; i < x + w; i ++)
        drawVLine(i, y, h, color);
}

void TFT::drawChar(int16_t x, int16_t y, char c)
{
    for (int8_t i = 0; i < 5; i++)
    {
        uint8_t line = pgm_read_byte(&font[c * 5 + i]);

        for (int8_t j = 0; j < 8; j++, line >>= 1)
            if (line & 1)
                drawPixel(x + i, y + j, 0xffff);
    }
}

void TFT::drawString(int16_t x, int16_t y, const char *s)
{
    size_t n = my_strlen(s);

    for (size_t i = 0; i < n; i++)
        drawChar(x + i * 8, y, s[i]);
}

void TFT::setAddrWindow(int16_t x, int16_t y, int16_t x1, int16_t y1)
{
    writeCmdData(_MC, x);
    writeCmdData(_MP, y);
    writeCmdData(_SC, x);
    writeCmdData(_SP, y);
    writeCmdData(_EC, x1);
    writeCmdData(_EP, y1);
}

void TFT::begin()
{
    reset();
    _lcd_capable = AUTO_READINC | REV_SCREEN | INVERT_GS;

    static const uint16_t ST7781_regValues[] PROGMEM =
    {
        0x00FF, 0x0001,     //can we do 0xFF
        0x00F3, 0x0008,
        0x00, 0x0001,
        0x0001, 0x0100,     // Driver Output Control Register (R01h)
        0x0002, 0x0700,     // LCD Driving Waveform Control (R02h)
        0x0003, 0x1030,     // Entry Mode (R03h)
        0x0008, 0x0302,
        0x0009, 0x0000,
        0x0010, 0x0000,     // Power Control 1 (R10h)
        0x0011, 0x0007,     // Power Control 2 (R11h)
        0x0012, 0x0000,     // Power Control 3 (R12h)
        0x0013, 0x0000,     // Power Control 4 (R13h)
        TFTLCD_DELAY, 50,
        0x0010, 0x14B0,     // Power Control 1 SAP=1, BT=4, APE=1, AP=3
        TFTLCD_DELAY, 10,
        0x0011, 0x0007,     // Power Control 2 VC=7
        TFTLCD_DELAY, 10,
        0x0012, 0x008E,     // Power Control 3 VCIRE=1, VRH=14
        0x0013, 0x0C00,     // Power Control 4 VDV=12
        0x0029, 0x0015,     // NVM read data 2 VCM=21
        TFTLCD_DELAY, 10,
        0x0030, 0x0000,     // Gamma Control 1
        0x0031, 0x0107,     // Gamma Control 2
        0x0032, 0x0000,     // Gamma Control 3
        0x0035, 0x0203,     // Gamma Control 6
        0x0036, 0x0402,     // Gamma Control 7
        0x0037, 0x0000,     // Gamma Control 8
        0x0038, 0x0207,     // Gamma Control 9
        0x0039, 0x0000,     // Gamma Control 10
        0x003C, 0x0203,     // Gamma Control 13
        0x003D, 0x0403,     // Gamma Control 14
        0x0060, 0xA700,     // Driver Output Control (R60h) .kbv was 0xa700
        0x0061, 0x0001,     // Driver Output Control (R61h)
        0x0090, 0X0029,     // Panel Interface Control 1 (R90h)
        0x0007, 0x0133,     // Display Control (R07h)
        TFTLCD_DELAY, 50,
    };

    init_table16(ST7781_regValues, sizeof(ST7781_regValues));
    setRotation(0);             //PORTRAIT
}

uint8_t TFT::read8()
{
    uint8_t dst = 0;
#if defined (__AVR_ATmega328P__)
    *p_portc |= 1<<pc0;
    *p_portc &= ~(1<<pc0);
    *p_portc &= ~(1<<pc0);
    *p_portc &= ~(1<<pc0);
    dst = (*p_pinb & 0x03) | (*p_pind & 0xfc);
    *p_portc |= 1<<pc0;
#else
    *p_pinA0_port |= 1<<pinA0_bit;
    *p_pinA0_port &= ~(1<<pinA0_bit);
    *p_pinA0_port &= ~(1<<pinA0_bit);
    *p_pinA0_port &= ~(1<<pinA0_bit);
    dst |= (*p_pin8_in & 1<<pin8_bit) << 0;
    dst |= (*p_pin9_in & 1<<pin9_bit) << 1;
    dst |= (*p_pin2_in & 1<<pin2_bit) << 2;
    dst |= (*p_pin3_in & 1<<pin3_bit) << 3;
    dst |= (*p_pin4_in & 1<<pin4_bit) << 4;
    dst |= (*p_pin5_in & 1<<pin5_bit) << 5;
    dst |= (*p_pin6_in & 1<<pin6_bit) << 4;
    dst |= (*p_pin7_in & 1<<pin7_bit) << 5;
    *p_pinA0_port |= 1<<pinA0_bit;
#endif
    return dst;
}

void TFT::writeData(uint16_t x)
{
    *p_pinA2_port |= 1<<pinA2_bit;
    write16(x);
}

void TFT::writeCmdData(uint16_t cmd, uint16_t data)
{
    *p_pinA3_port &= ~(1<<pinA3_bit);
    writeCmd(cmd);
    writeData(data);
    *p_pinA3_port |= 1<<pinA3_bit;
}

void TFT::reset()
{
    *p_pin8_ddr |= 1<<pin8_bit;
    *p_pin9_ddr |= 1<<pin9_bit;
    *p_pin2_ddr |= 1<<pin2_bit;
    *p_pin3_ddr |= 1<<pin3_bit;
    *p_pin4_ddr |= 1<<pin4_bit;
    *p_pin5_ddr |= 1<<pin5_bit;
    *p_pin6_ddr |= 1<<pin6_bit;
    *p_pin7_ddr |= 1<<pin7_bit;
    *p_pinA0_ddr |= 1<<pinA0_bit;
    *p_pinA1_ddr |= 1<<pinA1_bit;
    *p_pinA2_ddr |= 1<<pinA2_bit;
    *p_pinA3_ddr |= 1<<pinA3_bit;
    *p_pinA4_ddr |= 1<<pinA4_bit;
    *p_pinA3_port |= 1<<pinA3_bit;
    *p_pinA0_port |= 1<<pinA0_bit;
    *p_pinA1_port |= 1<<pinA1_bit;
    *p_pinA4_port |= 1<<pinA4_bit;
    _delay_ms(50);
    *p_pinA4_port &= ~(1<<pinA4_bit);
    _delay_ms(100);
    *p_pinA4_port |= 1<<pinA4_bit;
    _delay_ms(100);
    writeCmdData(0xB0, 0x0000);
}

uint16_t TFT::read16()
{
    uint16_t ret = read8();
    uint8_t lo = read8();
    return ret << 8 | lo;
}

uint16_t TFT::readReg(uint16_t reg, int8_t index)
{
    *p_pinA3_port &= ~(1<<pinA3_bit);
    writeCmd(reg);
    *p_pin2_ddr &= ~(1<<pin2_bit);
    *p_pin3_ddr &= ~(1<<pin3_bit);
    *p_pin4_ddr &= ~(1<<pin4_bit);
    *p_pin5_ddr &= ~(1<<pin5_bit);
    *p_pin6_ddr &= ~(1<<pin6_bit);
    *p_pin7_ddr &= ~(1<<pin7_bit);
    *p_pin8_ddr &= ~(1<<pin8_bit);
    *p_pin9_ddr &= ~(1<<pin9_bit);
    *p_pinA2_port |= 1<<pinA2_bit;
    _delay_ms(1);
    uint16_t ret;

    do
    {
        ret = read16();
    }
    while (--index >= 0);

    *p_pinA0_port |= 1<<pinA0_bit;
    *p_pinA3_port |= 1<<pinA3_bit;
    *p_pin2_ddr |= 1<<pin2_bit;
    *p_pin3_ddr |= 1<<pin3_bit;
    *p_pin4_ddr |= 1<<pin4_bit;
    *p_pin5_ddr |= 1<<pin5_bit;
    *p_pin6_ddr |= 1<<pin6_bit;
    *p_pin7_ddr |= 1<<pin7_bit;
    *p_pin8_ddr |= 1<<pin8_bit;
    *p_pin9_ddr |= 1<<pin9_bit;
    return ret;
}

inline void TFT::write8(uint8_t x)
{

#if defined (__AVR_ATmega328P__)
    PORTB = (PORTB & ~0x03) | (x & 0x03);
    PORTD = (PORTD & ~0xfc) | (x & 0xfc);
    PORTC &= ~(1<<1);
    PORTC |= 1<<1;
#elif defined (__AVR_ATmega2560__)
    *p_porth = *p_porth & 0x87;
    *p_porth |= (x << 5) & (1<<ph5 | 1<<ph6);
    *p_porth |= (x >> 3) & (1<<ph4 | 1<<ph3);
    *p_porte = (*p_porte & 0xc7);
    *p_porte |= (x << 2) & (1<<pe4 | 1<<pe5);
    *p_porte |= (x >> 2) & (1<<pe3);
    *p_portg = (*p_portg & 0xdf);
    *p_portg |= (x << 1) & (1<<pg5);
    *p_pinA1_port &= ~(1<<pinA1_bit);
    *p_pinA1_port |= 1<<pinA1_bit;
#else
    Board b;
    b.pin8.set(x & 1<<0 ? true : false);
    b.pin9.set(x & 1<<1 ? true : false);
    b.pin2.set(x & 1<<2 ? true : false);
    b.pin3.set(x & 1<<3 ? true : false);
    b.pin4.set(x & 1<<4 ? true : false);
    b.pin5.set(x & 1<<5 ? true : false);
    b.pin6.set(x & 1<<6 ? true : false);
    b.pin7.set(x & 1<<7 ? true : false);
    *p_pinA1_port &= ~(1<<pinA1_bit);
    *p_pinA1_port |= 1<<pinA1_bit;
#endif
}

void TFT::write16(uint16_t x)
{
    uint8_t h = x >> 8, l = (uint8_t)x;
    write8(h);
    write8(l);
}

void TFT::writeCmd(uint16_t x)
{
    *p_pinA2_port &= ~(1<<pinA2_bit);
    write16(x);
}

uint16_t TFT::readID()
{
    return readReg(0);
}

void TFT::setRotation(uint8_t r)
{
    uint16_t GS, SS, ORG;
    uint8_t val, rotation = r & 3;
    _width = rotation & 1 ? HEIGHT : WIDTH;
    _height = rotation & 1 ? WIDTH : HEIGHT;

    switch (rotation)
    {
    case 0:                    //PORTRAIT:
        val = 0x48;             //MY=0, MX=1, MV=0, ML=0, BGR=1
        break;
    case 1:                    //LANDSCAPE: 90 degrees
        val = 0x28;             //MY=0, MX=0, MV=1, ML=0, BGR=1
        break;
    case 2:                    //PORTRAIT_REV: 180 degrees
        val = 0x98;             //MY=1, MX=0, MV=0, ML=1, BGR=1
        break;
    case 3:                    //LANDSCAPE_REV: 270 degrees
        val = 0xF8;             //MY=1, MX=1, MV=1, ML=1, BGR=1
        break;
    }

    if (_lcd_capable & INVERT_GS)
        val ^= 0x80;

    if (_lcd_capable & INVERT_SS)
        val ^= 0x40;

    if (_lcd_capable & INVERT_RGB)
        val ^= 0x08;

    _MC = 0x20;
    _MP = 0x21;
    _MW = 0x22;
    _SC = 0x50;
    _EC = 0x51;
    _SP = 0x52;
    _EP = 0x53;
    GS = val & 0x80 ? 1 << 15 : 0;
    writeCmdData(0x60, GS | 0x2700);
    SS = (val & 0x40) ? (1 << 8) : 0;
    writeCmdData(0x01, SS);     // set Driver Output Control
    ORG = (val & 0x20) ? (1 << 3) : 0;

    if (val & 0x08)
        ORG |= 0x1000;  //BGR

    writeCmdData(0x03, ORG | 0x0030);    // set GRAM write direction and BGR=1.

    if ((rotation & 1) && ((_lcd_capable & MV_AXIS) == 0))
    {
        uint16_t x;
        x = _MC, _MC = _MP, _MP = x;
        x = _SC, _SC = _SP, _SP = x;    //.kbv check 0139
        x = _EC, _EC = _EP, _EP = x;    //.kbv check 0139
    }

    setAddrWindow(0, 0, width() - 1, height() - 1);
}





