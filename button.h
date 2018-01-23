#ifndef _BUTTON_H_
#define _BUTTON_H_
#include "analog.h"
#include <string.h>
#include "tft.h"

class Frame
{
protected:
    TFT * const _tft;
public:
    virtual void draw() const { }
    virtual uint16_t dispatch(TSPoint p) { return 0; }
    virtual uint16_t id(TSPoint p) const { return 0; }
    Frame(TFT *tft) : _tft(tft) { }
};

class Widget
{
protected:
    uint16_t _id = 0;
    uint16_t _x = 0, _y = 0;
public:
    inline uint16_t x() const { return _x; }
    inline uint16_t y() const { return _y; }
    void id(uint16_t i) { _id = i; }
    uint16_t id() const { return _id; }
    void pos(uint16_t x, uint16_t y) { _x = x, _y = y; }
    virtual void draw(TFT *tft) const { }
};

class Label : public Widget
{
private:
    char _label[50] = {0};
public:
    void draw(TFT *tft) const;
    void label(const char *label) { strncpy(_label, label, 50); }
};

class Button : public Widget
{
private:
    uint16_t _w = 0, _h = 0, _fill = 0;
    char _label[10] = {0};
public:
    void size(uint16_t w, uint16_t h) { _w = w, _h = h; }
    void fill(uint16_t color) { _fill = color; }
    uint16_t fill() const { return _fill; }
    inline uint16_t w() const { return _w; }
    inline uint16_t h() const { return _h; }
    void draw(TFT *tft) const;
    void label(const char *label) { strncpy(_label, label, 10); }
    bool contains(TSPoint p) const;
};

#if 0
class Button
{
private:
    uint8_t _id = 0;
    uint16_t _x = 0, _y = 0, _w = 0, _h = 0, _fill = 0;
    char _label[10] = {0};
public:
    void id(uint8_t i) { _id = i; }
    uint8_t id() const { return _id; }
    void pos(uint16_t x, uint16_t y) { _x = x, _y = y; }
    void size(uint16_t w, uint16_t h) { _w = w, _h = h; }
    void fill(uint16_t color) { _fill = color; }
    uint16_t fill() const { return _fill; }
    inline uint16_t x() const { return _x; }
    inline uint16_t y() const { return _y; }
    inline uint16_t w() const { return _w; }
    inline uint16_t h() const { return _h; }
    void draw(TFT *tft) const;
    void label(const char *label) { strncpy(_label, label, 10); }
    bool contains(TSPoint p) const;
};
#endif
#endif

