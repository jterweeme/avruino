/*
TFT calculator

LCD data 0 = D8
LCD data 1 = D9
LCD data 2 = D2
LCD data 3 = D3
LCD data 4 = D4
*/

#include "board.h"
#include "st7783.h"

#include <util/delay.h>
#include "pgmspees.h"
#include "misc.h"
#include "analog.h"
#include "button.h"
#include "calc.h"

class OutputLine : public Output
{
private:
    char _buffer[20] = {0};
    uint8_t _pos = 0;
    TFT * const _tft;
public:
    OutputLine(TFT *tft) : _tft(tft) { }
    void clear() { _buffer[_pos = 0] = 0; redraw(); }
    void push(char c);
    void push(const char *s) { while (*s) push(*s++); }
    void redraw();
};

class GUI
{
public:
    static const uint8_t
        ID_BTN1 = 101,
        ID_BTN2 = 102,
        ID_BTN3 = 103,
        ID_BTN4 = 104,
        ID_BTN5 = 105,
        ID_BTN6 = 106,
        ID_BTN7 = 107,
        ID_BTN8 = 108,
        ID_BTN9 = 109,
        ID_BTN0 = 100,
        ID_BTNC = 111,
        ID_BTNADD = 112,
        ID_BTNMIN = 113,
        ID_BTNMUL = 114,
        ID_BTNDIV = 115,
        ID_BTNEQ = 110,
        ID_EMPTY1 = 116,
        ID_EMPTY2 = 117;
private:
    MyArray<Button, 18> _buttons;
    TFT * const _tft;
    OutputLine _ol;
    uint16_t _prevClr = 0xf;
    Button *_prevBtn = 0;
public:
    Button *find(TSPoint p) const;
    GUI(TFT *tft) : _tft(tft), _ol(tft) { }
    void init();
    void lightUp(Button *button);
    void draw();
};

void GUI::lightUp(Button *button)
{
    if (_prevBtn)
    {
        _prevBtn->fill(_prevClr);
        _prevBtn->draw(_tft);
    }

    if (button)
    {
        _prevBtn = button;
        _prevClr = _prevBtn->fill();
        button->fill(0xfff);
        button->draw(_tft);
    }
}

void OutputLine::redraw()
{
    _tft->fillRect(0, 0, 240, 10, 0);
    _tft->drawString(240 - strlen(_buffer) * 8, 0, _buffer);
}

void GUI::draw()
{
    for (size_t i = 0; i < _buttons.size(); i++)
        _buttons.at(i).draw(_tft);
}

Button *GUI::find(TSPoint p) const
{
    for (size_t i = 0; i < _buttons.size(); i++)
        if (_buttons.at(i).contains(p))
            return &_buttons.at(i);

    return 0;
}

void OutputLine::push(char c)
{
    if (_pos >= 19)
        return;

    _buffer[_pos++] = c;
    _buffer[_pos] = 0;
    redraw();
}

struct buttons_t
{
    uint8_t x;
    uint16_t y;
    uint8_t id;
    char label[10];
    uint16_t color;
};

void GUI::init()
{
    static const buttons_t buttonmap[] = {
        {5, 25, ID_BTN1, "1", 0xf},
        {90, 25, ID_BTN2, "2", 0xf},
        {175, 25, ID_BTN3, "3", 0xf},
        {5, 75, ID_BTN4, "4", 0xf},
        {90, 75, ID_BTN5, "5", 0xf},
        {175, 75, ID_BTN6, "6", 0xf},
        {5, 125, ID_BTN7, "7", 0xf},
        {90, 125, ID_BTN8, "8", 0xf},
        {175, 125, ID_BTN9, "9", 0xf},
        {5, 175, ID_BTNADD, "+", 0xf},
        {90, 175, ID_BTN0, "0", 0xf},
        {175, 175, ID_BTNMIN, "-", 0xf},
        {5, 225, ID_BTNMUL, "*", 0xf},
        {90, 225, ID_BTNDIV, "/", 0xf},
        {175, 225, ID_EMPTY1, 0, 0xf},
        {5, 275, ID_BTNC, "C", 0xf000},
        {90, 275, ID_EMPTY2, 0, 0xf},
        {175, 275, ID_BTNEQ, "=", 0x4f00}
    };

    for (int i = 0; i < 18; i++)
    {
        _buttons.at(i).pos(buttonmap[i].x, buttonmap[i].y);
        _buttons.at(i).size(60, 40);
        _buttons.at(i).id(buttonmap[i].id);
        _buttons.at(i).label(buttonmap[i].label);
        _buttons.at(i).fill(buttonmap[i].color);
    }
}

int main()
{
    ST7783 tft;
    Analog2 analog;
    analog.init();
    tft.reset();
    tft.begin();
    tft.fillScreen(0);
    GUI gui(&tft);
    gui.init();
    gui.draw();
    TouchScreen ts(&analog);
    OutputLine ol(&tft);
    Calculator calc(&ol);
    Sub sub;
    Mul mul;
    Div div;
    calc.reset();
    uint8_t id;

    while (true)
    {
        TSPoint p = ts.getPoint();

        if (p.z > 200)
        {
            Button *button = gui.find(p);
            gui.lightUp(button);
            id = button ? button->id() : 0;
            _delay_ms(250);

            switch (id)
            {
                case GUI::ID_BTN1:
                    calc.push(1);
                    break;
                case GUI::ID_BTN2:
                    calc.push(2);
                    break;
                case GUI::ID_BTN3:
                    calc.push(3);
                    break;
                case GUI::ID_BTN4:
                    calc.push(4);
                    break;
                case GUI::ID_BTN5:
                    calc.push(5);
                    break;
                case GUI::ID_BTN6:
                    calc.push(6);
                    break;
                case GUI::ID_BTN7:
                    calc.push(7);
                    break;
                case GUI::ID_BTN8:
                    calc.push(8);
                    break;
                case GUI::ID_BTN9:
                    calc.push(9);
                    break;
                case GUI::ID_BTN0:
                    calc.push(0);
                    break;
                case GUI::ID_BTNADD:
                    calc.add();
                    break;
                case GUI::ID_BTNMIN:
                    calc.op(&sub);
                    break;
                case GUI::ID_BTNMUL:
                    calc.op(&mul);
                    break;
                case GUI::ID_BTNDIV:
                    calc.op(&div);
                    break;
                case GUI::ID_BTNC:
                    calc.reset();
                    break;
                case GUI::ID_BTNEQ:
                    calc.equals();
                    break;
            }
        }
    }

    return 0;
}


