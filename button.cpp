#include "button.h"

void Button::draw(TFT &tft) const
{
    tft.drawRect(_x, _y, _w, _h, 0xffff);
    tft.fillRect(_x + 1, _y + 1, _w - 2, _h - 2, _fill);
    tft.drawString(_x + 3, _y + 3, _label);
}

bool Button::contains(TSPoint p) const
{
    return p.x >= _x && p.x <= _x + _w && p.y >= _y && p.y <= _y + _h;
}



