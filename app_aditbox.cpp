#include "st7783.h"

#ifndef F_CPU
#define F_CPU 16000000L
#endif

#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <string.h>
#include "misc.h"
#include "analog.h"
#include "button.h"

static constexpr uint16_t
    ID_FRAME_PINCODE = 6000,
    ID_FRAME_MENU = 6001,
    ID_FRAME_EDIT = 6002,
    ID_GMAIL = 1,
    ID_GITHUB = 2,
    ID_OUTLOOK = 3,
    ID_BTN1 = 4,
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
    ID_BTNEDIT = 5000,
    ID_EMPTY1 = 116,
    ID_EMPTY2 = 117;

//static Serial *g_serial;

class Pincode : public Frame
{
private:
    Label _title;
    Label _pincode;
    MyArray<Button, 12> _buttons;
    Button *find(TSPoint p);
public:
    Pincode(TFT *tft);
    void draw() const;
    uint16_t dispatch(TSPoint p);
};

class Menu : public Frame
{
private:
    MyArray<Button, 12> _buttons;
    Button *find(TSPoint p);
public:
    Menu(TFT *tft);
    void draw() const;
    uint16_t dispatch(TSPoint p);
};

class Editor : public Frame
{
private:
    MyArray<Button, 12> _buttons;
public:
    Editor(TFT *tft);
    void draw() const;
    uint16_t dispatch(TSPoint p);
};

uint16_t Pincode::dispatch(TSPoint p)
{
    Button *button = find(p);

    if (button == 0)
        return 0;

    button->fill(0xfff);
    button->draw(_tft);

    if (button->id() == ID_BTNEQ)
        return ID_FRAME_MENU;

    return 0;
}

Button *Menu::find(TSPoint p)
{
    for (size_t i = 0; i < _buttons.size(); i++)
        if (_buttons.at(i).contains(p))
            return &_buttons.at(i);

    return 0;   
}

Button *Pincode::find(TSPoint p)
{
    for (size_t i = 0; i < _buttons.size(); i++)
        if (_buttons.at(i).contains(p))
            return &_buttons.at(i);

    return 0;
}

uint16_t Menu::dispatch(TSPoint p)
{
    Button *button = find(p);

    if (button == 0)
        return 0;

    button->fill(0xfff);
    button->draw(_tft);

    if (button->id() == ID_BTNEDIT)
        return ID_FRAME_EDIT;

    return 0;
}

struct buttons_t
{
    uint8_t x;
    uint16_t y;
    uint8_t id;
    char label[40];
    uint16_t color;
};

Editor::Editor(TFT *tft) : Frame(tft)
{
    static constexpr buttons_t buttonmap[] = {
        {5, 105, ID_GMAIL, "Gmail", 0xf},
        {90, 105, ID_GITHUB, "GitHub", 0xf},
        {175, 105, ID_OUTLOOK, "Edit", 0xf},
        {5, 155, 0, "", 0xf},
        {90, 155, 0, "", 0xf},
        {175, 155, 0, "", 0xf},
        {5, 205, 0, "", 0xf},
        {90, 205, 0, "", 0xf},
        {175, 205, 0, "", 0xf},
        {5, 255, 0, "", 0xf},
        {90, 255, 0, "", 0xf},
        {175, 255, 0, "Edit", 0xf000},
    };

    for (uint8_t i = 0; i < 12; i++)
    {
        _buttons.at(i).pos(buttonmap[i].x, buttonmap[i].y);
        _buttons.at(i).id(buttonmap[i].id);
        _buttons.at(i).size(60, 40);
        _buttons.at(i).label(buttonmap[i].label);
        _buttons.at(i).fill(buttonmap[i].color);
    }
}

uint16_t Editor::dispatch(TSPoint p)
{
    return 0;
}

void Editor::draw() const
{
    _tft->fillScreen(0);

    for (size_t i = 0; i < _buttons.size(); i++)
        _buttons.at(i).draw(_tft);
}

Menu::Menu(TFT *tft) : Frame(tft)
{
    static constexpr buttons_t buttonmap[] = {
        {5, 105, ID_GMAIL, "Gmail", 0xf},
        {90, 105, ID_GITHUB, "GitHub", 0xf},
        {175, 105, ID_OUTLOOK, "Outlook", 0xf},
        {5, 155, 0, "", 0xf},
        {90, 155, 0, "", 0xf},
        {175, 155, 0, "", 0xf},
        {5, 205, 0, "", 0xf},
        {90, 205, 0, "", 0xf},
        {175, 205, 0, "", 0xf},
        {5, 255, 0, "", 0xf},
        {90, 255, 0, "", 0xf},
        {175, 255, 0, "Edit", 0xf000},
    };

    for (uint8_t i = 0; i < 12; i++)
    {
        _buttons.at(i).pos(buttonmap[i].x, buttonmap[i].y);
        _buttons.at(i).id(buttonmap[i].id);
        _buttons.at(i).size(60, 40);
        _buttons.at(i).label(buttonmap[i].label);
        _buttons.at(i).fill(buttonmap[i].color);
    }
}

void Menu::draw() const
{
    _tft->fillScreen(0);

    for (size_t i = 0; i < _buttons.size(); i++)
        _buttons.at(i).draw(_tft);
}

void Pincode::draw() const
{
    _tft->fillScreen(0);

    for (size_t i = 0; i < _buttons.size(); i++)
        _buttons.at(i).draw(_tft);

    _title.draw(_tft);
    _pincode.draw(_tft);
}

Pincode::Pincode(TFT *tft) : Frame(tft)
{
    _title.label("Enter pincode:");
    _title.pos(5, 5);
    _pincode.label("....");
    _pincode.pos(5, 35);

    static constexpr buttons_t buttonmap[] = {
        {5, 105, ID_BTN1, "1", 0xf},
        {90, 105, ID_BTN2, "2", 0xf},
        {175, 105, ID_BTN3, "3", 0xf},
        {5, 155, ID_BTN4, "4", 0xf},
        {90, 155, ID_BTN5, "5", 0xf},
        {175, 155, ID_BTN6, "6", 0xf},
        {5, 205, ID_BTN7, "7", 0xf},
        {90, 205, ID_BTN8, "8", 0xf},
        {175, 205, ID_BTN9, "9", 0xf},
        {5, 255, ID_BTNC, "C", 0xf000},
        {90, 255, ID_BTN0, "0", 0xf},
        {175, 255, ID_BTNEQ, "OK", 0x4f00},
    };

    for (uint8_t i = 0; i < 12; i++)
    {
        _buttons.at(i).pos(buttonmap[i].x, buttonmap[i].y);
        _buttons.at(i).id(buttonmap[i].id);
        _buttons.at(i).size(60, 40);
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
    Pincode pincode(&tft);
    Menu menu(&tft);
    Editor editor(&tft);
    TouchScreen ts(&analog);
    //Serial s;
    //s.init();
    //g_serial = &s;
    Frame *frame = &pincode;
    frame->draw();

    while (true)
    {
        TSPoint p = ts.getPoint();
   
        if (p.z > 200)
        {
            uint16_t ret = frame->dispatch(p);

            switch (ret)
            {
            case ID_FRAME_MENU:
                frame = &menu;
                frame->draw();
                break;
            case ID_FRAME_EDIT:
                frame = &editor;
                frame->draw();
                break;
            }
        }
    }


    return 0;
}


