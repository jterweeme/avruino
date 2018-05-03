/*
Uno: OC1A/#9
Ocho: OC1A/#9
*/

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/pgmspace.h>
#include "board.h"
#include <util/delay.h>
#include "misc.h"

static const uint16_t notes[] PROGMEM =
{
    0, 262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, 523, 554,
    587, 622, 659, 698, 740, 784, 831, 880, 932, 988, 1047, 1109, 1175, 1245,
    1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976, 2093, 2217, 2349, 2489,
    2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951, 2*2093, 2*2217, 2*2349,
    2*2489, 2*2637, 2*2794, 2*2960, 2*3136, 2*3322, 2*3520, 2*3729, 2*3951, 0
};

static void delay(uint16_t n)
{
    for (uint16_t i = 0; i < n; i++)
        _delay_ms(1);
}

class Rtttl
{
private:
    inline char read_byte(const char *p, bool pgm) { return pgm ? pgm_read_byte(p) : *p; }
    void _play(const char *p, uint8_t octave_offset, bool pgm);
    void _tone(uint16_t freq);
    void _noTone() const { *p_ocr1a = 0; }
    uint8_t _noten(char c) const;
public:
    void play_P(const char *p, uint8_t octave_offset = 0) { _play(p, octave_offset, true); }
    void play(const char *p, uint8_t octave_offset = 0) { _play(p, octave_offset, false); }
};

static constexpr uint8_t CLK1 = 1, CLK8 = 2, CLK64 = 3, CLK256 = 4, CLK1024 = 5;

void Rtttl::_tone(uint16_t frequency)
{
    uint8_t prescalarbits = 0b001;
    *p_ocr1a_ddr |= 1<<ocr1a_bit;
    *p_tccr1a = 1<<com1a0;
    *p_tccr1b = 1<<wgm12 | CLK1;
    uint32_t ocr = F_CPU / (frequency<<1) - 1;
    prescalarbits = CLK1;

    if (ocr > 0xffff)
    {
        ocr = F_CPU / (frequency<<8) - 1;
        prescalarbits = CLK64;
    }

    *p_tccr1b = (*p_tccr1b & 0xf8) | prescalarbits;
    *p_ocr1a = ocr;
}

uint8_t Rtttl::_noten(char c) const
{
    switch (c)
    {
    case 'c':
        return 1;
    case 'd':
        return 3;
    case 'e':
        return 5;
    case 'f':
        return 6;
    case 'g':
        return 8;
    case 'a':
        return 10;
    case 'b':
        return 12;
    case 'p':
    default:
        return 0;
    }
}

void Rtttl::_play(const char *p, uint8_t octave_offset, bool pgm)
{
    uint8_t default_dur = 4, default_oct = 6;
    int bpm = 63, num;
    long wholenote, duration;
    uint8_t note, scale;

    while (read_byte(p, pgm) != ':')
        p++; // ignore name

    if (read_byte(++p, pgm) == 'd')
    {
        p += 2; // skip "d="
        num = 0;

        while (my_isdigit(read_byte(p, pgm)))
            num = num * 10 + (read_byte(p++, pgm) - '0');

        if (num > 0)
            default_dur = num;

        p++; // skip comma
    }

    if (read_byte(p, pgm) == 'o')
    {
        p+=2; // skip "o="
        num = read_byte(p++, pgm) - '0';

        if (num >= 3 && num <= 7)
            default_oct = num;

        p++; // skip comma
    }

    if (read_byte(p, pgm) == 'b')
    {
        p+=2; // skip "b="
        num = 0;

        while (my_isdigit(read_byte(p, pgm)))
            num = num * 10 + (read_byte(p++, pgm) - '0');

        bpm = num;
        p++; // skip colon
    }

    wholenote = (60 * 1000L / bpm) * 4; // this is the time for whole note (in milliseconds)

    while (read_byte(p, pgm))
    {
        num = 0;

        while (my_isdigit(read_byte(p, pgm)))
            num = (num * 10) + (read_byte(p++, pgm) - '0');

        duration = num ? wholenote / num : wholenote / default_dur;
        note = _noten(read_byte(p++, pgm));

        if (read_byte(p, pgm) == '#')
        {
            note++;
            p++;
        }

        if (read_byte(p, pgm) == '.')
        {
            duration += duration / 2;
            p++;
        }

        scale = my_isdigit(read_byte(p, pgm)) ? read_byte(p++, pgm) - '0' : default_oct;
        scale += octave_offset;

        if (read_byte(p, pgm) == ',')
            p++; // skip comma for next note (or we may be at the end)

        if (note)
        {
            _tone(pgm_read_word(&notes[(scale - 4) * 12 + note]));
            delay(duration);
            _noTone();
        }
        else
        {
            delay(duration);
        }
    }
}

#if 0
const char indiana[] PROGMEM =
    "Indiana:d=4,o=5,b=250:e,8p,8f,8g,8p,1c6,8p.,d,8p,8e,1f,p.,g,8p,8a,8b,8p,"
    "1f6,p,a,8p,8b,2c6,2d6,2e6,e,8p,8f,8g,8p,1c6,p,d6,8p,8e6,1f.6,g,8p,8g,e.6,"
    "8p,d6,8p,8g,e.6,8p,d6,8p,8g,f.6,8p,e6,8p,8d6,2c6";

const char AuldLangSyne[] PROGMEM =
    "AuldLS:d=4,o=6,b=101:g5,c,8c,c,e,d,8c,d,8e,8d,c,8c,"
    "e,g,2a,a,g,8e,e,c,d,8c,d,8e,8d,c,8a5,a5,g5,2c";

const char walklife[] PROGMEM =
    "walklife:d=4,o=6,b=80:8d.6,4d.6,32c.6,32d.6,16f.6,"
    "32d.6,8c6,8a#.5,2a#5,32c.6,32d.6,8f.6,4f.6,32c.6,32d.6,16f.6,32d.6,8c6,8a#.5,"
    "4a#.5,32c.6,32d.6,16f.6,32d.6,32c.6,32a#.5,8f.6,4f.6,32c.6,32d.6,16f.6,32d.6,"
    "8c6,8a#.5,2a#5,32c.6,32d.6,8f.6,4f.6,32c.6,32d.6,16f.6,32d.6,8c6,8a#.5,4a#.5,"
    "32c.6,32d.6,16f.6,32d.6,32c.6,32a#.5,8f6,8f6,";

const char wilhelmus[] PROGMEM =
    "Wilhelmus:d=4,o=5,b=90:d,g,g,8a,8b,8c6,8a,b,8a,8b,c6,b,8a,8g,a,2g,p,d,g,"
    "g,8a,8b,8c6,8a,b,8a,8b,c6,b,8a,8g,a,2g,p,8b,8c6,2d6,e6,2d6,c6,b,8a,8b,c6,b,"
    "a,g,2a,p,d,8g,8f#,8g,8a,b,2a,g,f#,d,8e,8f#,g,g,f#,2g";

const char good_bad_ugly[] PROGMEM =
    "Good Bad:d=4,o=6,b=40:32b5,32e6,32b5,32e6,"
    "8b.5,16g.5,16a.5,4e5,32b5,32e6,32b5,32e6,8b.5,16g.5,16a.5,4d6,32b5,32e6,"
    "32b5,32e6,8b.5,16g.5,32f#.5,32e.5,4d5,32b5,32e6,32b5,32e6,8b.5,16a.5,4e5,";

const char jamesbond[] PROGMEM =
    "James Bond:d=4,o=6,b=112:16c.5,32d.5,32d.5,16d.5,8d.5,16c.5,16c.5,16c.5,"
    "16c.5,32d#.5,32d#.5,16d#.5,8d#.5,16d.5,16d.5,16d.5,16c.5,32d.5,32d.5,16d.5,"
    "8d.5,16c.5,16c.5,16c.5,16c.5,32d#.5,32d#.5,16d#.5,8d#.5,16d.5,16d.5,16d.5,"
    "16c.5,32d.5,32d.5,16d.5,8d.5,16c.5,16c.5,16c.5,16c.5,32d#.5,32d#.5,16d#.5,"
    "8d#.5,16d.5,16d.5,16c.5,16b.5,2a#5,16g.5,16f.5,2g.5";

const char bobbouwer[] PROGMEM =
    "KunnenWi:d=4,o=5,b=140:g6,16g,16p,8e6,d6,b,16c,16p,8p,16c,16p,8p,16d,16p,8p,"
    "16d,16p,8p,d6,16g,16p,8c6,b,g,16c,16p,8p,16d,16p,8p,16g,16p,8p,16g,16p,8p,"
    "g6,16g,16p,8e6,d6,b,16c,16p,8p,16c,16p,8p,16d,16p,8p,16d,16p,8p,d6,16g,16p,"
    "8c6,b,g,16c,16p,8p,16d,16p,8p,16g";
#endif

const char godfather[] PROGMEM =
    "Godfather:d=4,o=5,b=160:8g,8c6,8d#6,8d6,8c6,8d#6,8c6,8d6,c6,8g#,8a#,2g,8p,"
    "8g,8c6,8d#6,8d6,8c6,8d#6,8c6,8d6,c6,8g,8f#,2f,8p,8f,8g#,8b,2d6,8p,8f,8g#,"
    "8b,2c6,8p,8c,8d#,8a#,8g#,g,8a#,8g#,8g#,8g,8g,8b4,2c";

#if 0
const char final_countdown[] PROGMEM =
    "Final Countdown:d=16,o=5,b=125:b,a,4b,4e,4p,8p,c6,b,8c6,8b,4a,4p,8p,c6,b,"
    "4c6,4e,4p,8p,a,g,8a,8g,8f#,8a,4g.,f#,g,4a.,g,a,8b,8a,8g,8f#,4e,4c6,2b.,b,c6,b,a,1b";

const char barbie[] PROGMEM =
    "Barbie Girl:d=8,o=5,b=125:g#,e,g#,c#6,4a,4p,f#,d#,f#,b,"
    "4g#,f#,e,4p,e,c#,4f#,4c#,4p,f#,e,4g#,4f#";

const char back_future[] PROGMEM =
    "Back to the Future:d=16,o=5,b=200:4g.,p,4c.,p,2f#.,p,g.,"
    "p,a.,p,8g,p,8e,p,8c,p,4f#,p,g.,p,a.,p,8g.,p,8d.,p,8g.,p,"
    "8d.6,p,4d.6,p,4c#6,p,b.,p,c#.6,p,2d.6";

const char macarena[] PROGMEM =
    "Macarena:d=8,o=5,b=180:f,f,f,4f,f,f,f,f,f,f,f,a,c,c,4f,f,f,4f,f,f,f,"
    "f,f,f,d,c,4p,4f,f,f,4f,f,f,f,f,f,f,f,a,4p,2c.6,4a,c6,a,f,4p,2p";

const char rule_brittania[] PROGMEM =
    "Rule Britania:d=8,o=5,b=100: e.,e,f,4f,e,f.,16e,d.,16c,2b4,4g,4f,"
    "16e,16c,16f,16d,g,f,4e,d.,16c,4c";

const char anthem[] PROGMEM =
    "USA National Anthem:d=8,o=5,b=120:e.,d,4c,4e,4g,4c6.,p,e6.,d6,4c6,"
    "4e,4f#,4g.,p,4g,4e6.,d6,4c6,2b,a,4b,c6.,16p,4c6,4g,4e,32p,4c";
#endif

int main()
{
    zei();
    Rtttl player;
    player.play_P(godfather, 0);
    return 0;
}





