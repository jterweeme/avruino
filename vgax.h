#ifndef _VGAX_H_
#define _VGAX_H_
#include "types.h"

#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

#define VGAX_HEIGHT 60 //number of lines
#define VGAX_BWIDTH 30 //number of bytes in a row
#define VGAX_WIDTH (VGAX_BWIDTH*4) //number of pixels in a row
#define VGAX_BSIZE (VGAX_BWIDTH*VGAX_HEIGHT) //size of framebuffer in bytes
#define VGAX_SIZE (VGAX_WIDTH*VGAX_HEIGHT) //size of framebuffer in pixels

//framebuffer. if you want you can write directly to this array. its safe
extern uint8_t vgaxfb[VGAX_HEIGHT*VGAX_BWIDTH];

//clock replacement. this is increment in the VSYNC interrupt, so run at 60Hz
extern uint16_t vtimer;


class VGAX
{
public:
    static void begin(bool enableTone=true);
    static void end();

    static inline void putpixel(uint8_t x, uint8_t y, uint8_t color)
    {
        uint8_t *p=vgaxfb + y*VGAX_BWIDTH + (x>>2);
        uint8_t bitpos = 6 - (x & 3) * 2;
        *p = (*p & ~(3 <<bitpos)) | color <<bitpos;
    }

    static inline uint8_t getpixel(uint8_t x, uint8_t y) {
        uint8_t p = vgaxfb[y*VGAX_BWIDTH + (x>>2)], bitpos=6-(x & 3)*2;
        return (p >> bitpos) & 3;
    }

    static inline void putpixel4(uint8_t bx, uint8_t y, uint8_t fourpixels)
    { vgaxfb[y*VGAX_BWIDTH + bx]=fourpixels; }

    static inline uint8_t getpixel4(uint8_t bx, uint8_t y)
    { return vgaxfb[y*VGAX_BWIDTH + bx]; }

    static void clear(uint8_t color);
    static void copy(uint8_t *src);
    static void bitblit(uint8_t *src, uint8_t swidth, uint8_t sheight, char dx, char dy,
                uint8_t color);

    static void blit(uint8_t *src, uint8_t swidth, uint8_t sheight, char dx, char dy);
    static void blit4(uint8_t *src, uint8_t sheight, char dx, char dy);
    static void blit8(uint8_t *src, uint8_t sheight, char dx, char dy);

    static void blitwmask(uint8_t *src, uint8_t *mask, uint8_t swidth, uint8_t sheight,
                char dx, char dy);

    static void blit8wmask(uint8_t *src, uint8_t *mask, uint8_t sheight, char dx, char dy);
    static void blit4aligned(uint8_t *src, uint8_t sheight, uint8_t dbx, uint8_t dy);
    static void blit8aligned(uint8_t *src, uint8_t sheight, uint8_t dbx, uint8_t dy);
    static void fillrect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color);

    static void printPROGMEM(uint8_t *fnt, uint8_t glyphscount, uint8_t fntheight,
                uint8_t hspace, uint8_t vspace, const char *str, char dx, char dy,
                uint8_t color);

    static void printSRAM(uint8_t *fnt, uint8_t glyphscount, uint8_t fntheight,
                uint8_t hspace, uint8_t vspace, const char *str, char dx, char dy,
                uint8_t color);

    static void delay(int msec);
    static inline unsigned millis() { return vtimer*16; }
    static inline unsigned long micros() { return vtimer*16000; }
    static void tone(unsigned int frequency);
    static void noTone();
};


#endif



