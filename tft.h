#ifndef _TFT_H_
#define _TFT_H_
#include "board.h"

static const uint16_t AUTO_READINC  = 1<<1;
static const uint16_t READ_BGR      = 1<<2;
static const uint16_t READ_LOWHIGH  = 1<<3;
static const uint16_t READ_NODUMMY  = 1<<6;
static const uint16_t INVERT_GS     = 1<<8;
static const uint16_t INVERT_SS     = 1<<9;
static const uint16_t MV_AXIS       = 1<<10;
static const uint16_t INVERT_RGB    = 1<<11;
static const uint16_t REV_SCREEN    = 1<<12;

class TFT
{
public:
    void inline write8(uint8_t x);
    void write16(uint16_t x);
    void writeCmd(uint16_t x);
    void writeCmdData(uint16_t cmd, uint16_t data);
    void writeData(uint16_t x);
    uint8_t read8();
    uint16_t read16();
    uint16_t readReg(uint16_t reg, int8_t index = 0);
    uint16_t readID();
    void init_table16(const void *table, int16_t size);
    void reset();
    void begin();
    void setAddrWindow(int16_t x, int16_t y, int16_t x1, int16_t y1);
    void setRotation(uint8_t r);
    int16_t width();
    int16_t height();
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void drawVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
    void drawHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
    void fillScreen(uint16_t color);
    void drawChar(int16_t x, int16_t y, char c);
    void drawString(int16_t x, int16_t y, const char *s);
protected:
    Board b;
    uint16_t _lcd_ID, _lcd_rev, _lcd_madctl, _MC, _MP, _MW, _SC, _EC, _SP, _EP;
    uint16_t _lcd_capable;
    const int16_t WIDTH = 240, HEIGHT = 320;
    int16_t _width, _height, cursor_x, cursor_y;
    uint16_t textcolor, textbgcolor;
    uint8_t textsize, rotation;
};
#endif



