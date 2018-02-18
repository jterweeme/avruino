/*
Uno:
RED: D6/OC0A/PD6
GREEN: D7/PD7
HSYNC: D3/OC2B/PD3
VSYNC: D9/OC1A/PB1
JOY1: A1/ADC1
JOY2: A3/ADC3
*/

#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <math.h>
#include <stdlib.h>
#include "analog.h"
#include "board.h"

Analog2 analog;

int32_t random() { return analog.read(Analog2::ADC5); }
int32_t random(int32_t max) { return random() % max; }
int32_t random(int32_t min, int32_t max) { return random(max - min) + min; }

int32_t map(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

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

//VGAX class. This is a static class. Multiple instances will not work
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
  /*
   * getpixel(x, y)
   *    x: horizontal pixel coordinate. Must be less than VGAX_WIDTH
   *    y: vertical pixel coordinate. Must be less than VGAX_HEIGHT
   *    return: 2bit color at <x,y> coordinate
   */
  static inline uint8_t getpixel(uint8_t x, uint8_t y) {
    uint8_t p = vgaxfb[y*VGAX_BWIDTH + (x>>2)], bitpos=6-(x & 3)*2;
    return (p >> bitpos) & 3;
  }
  /*
   * putpixel4(B, y, color)
   *    bx: horizontal BYTE coordinate. Must be less than VGAX_BWIDTH
   *    y: vertical pixel coordinate. Must be less than VGAX_HEIGHT
   *    fourpixels: four 2bit pixels in a byte.
   *      The four pixels must be packed in this order: highest 2bit of the byte
   *      are the first, left-most, pixel:
   *                      +---+---+---+---+---+---+---+---+
   *      highest bit --> | 1 | 1 | 2 | 2 | 3 | 3 | 4 | 4 | <-- lowest bit
   *                      +---+---+---+---+---+---+---+---+
   *      For example, if you want to set the first pixel (the left-most pixel)
   *      to 3 and the fourth pixel (right-most) to 1:
   *                      +---+---+---+---+---+---+---+---+
   *      highest bit --> | 1 | 1 | 0 | 0 | 0 | 0 | 0 | 1 | <-- lowest bit
   *                      +---+---+---+---+---+---+---+---+
   *      This byte is 193 in decimal and 0xC1 in hexadecimal
   */
    static inline void putpixel4(uint8_t bx, uint8_t y, uint8_t fourpixels)
    { vgaxfb[y*VGAX_BWIDTH + bx]=fourpixels; }
  /*
   * getpixel4(bx, y)
   *    bx: horizontal BYTE coordinate. Must be less than VGAX_BWIDTH
   *    y: vertical pixel coordinate. Must be less than VGAX_HEIGHT
   *    return: four 2bit pixels packed in a byte
   */
  static inline uint8_t getpixel4(uint8_t bx, uint8_t y) {
    return vgaxfb[y*VGAX_BWIDTH + bx]; }
  /*
   * clear(color)
   *    color: 2bit color to clear the framebuffer
   */
  static void clear(uint8_t color);
  /*
   * copy(src)
   *    src: source data. src size must be equal to framebuffer
   */
  static void copy(uint8_t *src);
  /*
   * bitblit(src, swidth, sheight, dx, dy, color)
   *    src: source data. each byte hold 8 pixels. higher bit (0b10000000) will
   *      be blitted first. Bits set as 1 are opaque, 0 are transparent
   *    swidth: source width in pixels. This is the number of horizontal bits
   *      to be blitted and can be not a multiple of 8
   *    sheight: source height in pixels
   *    dx: destination x coordinate in pixels. supported range: -127,+127
   *    dy: destination y coordinate in pixels. supported range: -127,+127
   *    color: 2bit color use for opaque pixels (with bits set to 1)
   */
  static void bitblit(uint8_t *src, uint8_t swidth, uint8_t sheight, char dx, char dy,
                uint8_t color);
  /*
   * blit(src, sx, sy, swidth, sheight, dx, dy)
   *    src: source data
   *    swidth: source width in pixels
   *    sheight: source height in pixels
   *    dx: destination x coordinate in pixels. supported range: -127,+127
   *    dy: destination y coordinate in pixels. supported range: -127,+127
   *
   * blit[N] versions are optimized for swidth=[N] cases
   */
  static void blit(uint8_t *src, uint8_t swidth, uint8_t sheight, char dx, char dy);
  static void blit4(uint8_t *src, uint8_t sheight, char dx, char dy);
  static void blit8(uint8_t *src, uint8_t sheight, char dx, char dy);
  /*
   * blitwmask(src, mask, sx, sy, swidth, sheight, dx, dy)
   *    src: source data. transparent pixels must be set to 0
   *    mask: source mask. pixel format is the same as src (2bit per pixel).
   *      value of 3 mean transparent pixel, value 0 mean opaque
   *    swidth: source width in pixels
   *    sheight: source height in pixels
   *    dx: destination x coordinate in pixels. supported range: -127,+127
   *    dy: destination y coordinate in pixels. supported range: -127,+127
   *
   * Masked blit is implemented using AND+OR raster operation, like described
   * here: http://en.wikipedia.org/wiki/Bit_blit
   *
   * blit[N]wmask versions are optimized for swidth=[N] cases
   */
  static void blitwmask(uint8_t *src, uint8_t *mask, uint8_t swidth, uint8_t sheight,
                char dx, char dy);
  static void blit8wmask(uint8_t *src, uint8_t *mask, uint8_t sheight, char dx, char dy);
  /*
   * blit[N]aligned(src, sheight, dbx, dy)
   *    src: source data
   *    sheight: source height in pixels
   *    dbx: destination x coordinate in BYTES. supported range: 0,VGAX_BWIDTH
   *    dy: destination y coordinate in pixels. supported range: 0,VGAX_HEIGHT
   *
   * These methods does not perform any clipping. coordinates must point inside
   * framebuffer
   */
  static void blit4aligned(uint8_t *src, uint8_t sheight, uint8_t dbx, uint8_t dy);
  static void blit8aligned(uint8_t *src, uint8_t sheight, uint8_t dbx, uint8_t dy);
  /*
   * fillrect(x, y, width, height, color)
   *    x: destination x coordinate in pixels
   *    y: destination y coordinate in pixels
   *    width: width of the rect in pixels
   *    height: height of the rect in pixels
   *    color: color of the rect
   */
  static void fillrect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color);
  /*
   * print(fnt, glyphscount, fntheight, hspace, vspace, str, dx, dy, color)
   *    fnt: font definition, generated from 2BITFONT tool
   *    glyphscount: number of symbols inside font definition (generated from
   *      2bitfont tool)
   *    fntheight: font height (generated from 2bitfont tool)
   *    hspace: horizontal space between each printed symbol
   *    vspace: vertical space between each printed symbol
   *    str: string to be printed. The string is readed from PROGMEM (FLASH)
   *    dx: destination x coordinate in pixels
   *    dy: destination y coordinate in pixels
   *    color: color of the text
   */
  static void printPROGMEM(uint8_t *fnt, uint8_t glyphscount, uint8_t fntheight, 
                uint8_t hspace, uint8_t vspace, const char *str, char dx, char dy, 
                uint8_t color);
  /*
   * printSRAM(...)
   *    same as printPROGMEM but read from SRAM
   */
  static void printSRAM(uint8_t *fnt, uint8_t glyphscount, uint8_t fntheight, 
                uint8_t hspace, uint8_t vspace, const char *str, char dx, char dy, 
                uint8_t color);
  /*
   * delay(msec)
   *    msec: number of milliseconds to wait
   */
  static void delay(int msec);
  /*
   * millis()
   *    return the number of milliseconds ellapsed
   */
  static inline unsigned millis() {
    return vtimer*16;
  }
  /*
   * micros()
   *    return the number of microseconds ellapsed  
   */
  static inline unsigned long micros() {
    return vtimer*16000;
  }
  /*
   * tone(frequency)
   *    frequency: tone frequency
   *
   * Audio generation will start in the next horizontal interrupt. To mute
   * audio you need to call noTone after some time.
   */
  static void tone(unsigned int frequency);
  /*
   * noTone()
   *     stop the tone generation
   */
  static void noTone();
};

#define PRINT_LOOP_CODE \
  if (c=='\n') { \
    dx=dx0; \
    dy0+=fntheight+vspace; \
  } else if (c==' ') { \
    dx+=3+hspace; \
  } else { \
    c-=33; \
    if (c>=0 && c<glyphscount) { \
      uint8_t *fntg=fnt+c*(1+fntheight); \
      uint8_t fntw=pgm_read_byte(fntg); \
      /*(inline of bitblit(fntg+1, fntw, fntheight, dx, dy, color & 3))\
        to save from stack overflow*/ \
      uint8_t *sptr=fntg+1; \
      uint8_t sheight=fntheight; \
      uint8_t dy=dy0; \
      while (sheight--) { \
          uint8_t m=pgm_read_byte(sptr++); \
          /*first nibble (higher 4 bit of src)*/ \
          if (dx+0>=VGAX_WIDTH) \
            goto next; \
          if (dx+0>-1 && m&(1<<7)) \
            putpixel(dx+0, dy, color); \
          if (fntw==0) \
            goto next; \
          if (dx+1>=VGAX_WIDTH) \
            goto next; \
          if (dx+1>-1 && m&(1<<6)) \
            putpixel(dx+1, dy, color); \
          if (fntw==1) \
            goto next; \
          if (dx+2>=VGAX_WIDTH) \
            goto next; \
          if (dx+2>-1 && m&(1<<5)) \
            putpixel(dx+2, dy, color); \
          if (fntw==2) \
            goto next; \
          if (dx+3>=VGAX_WIDTH) \
            goto next; \
          if (dx+3>-1 && m&(1<<4)) \
            putpixel(dx+3, dy, color); \
          if (fntw==3) \
            goto next; \
          /*second nibble (lower 4 bit of src)*/ \
          if (dx+4>=VGAX_WIDTH) \
            goto next; \
          if (dx+4>-1 && m&(1<<3)) \
            putpixel(dx+4, dy, color); \
          if (fntw==4) \
            goto next; \
          if (dx+5>=VGAX_WIDTH) \
            goto next; \
          if (dx+5>-1 && m&(1<<2)) \
            putpixel(dx+5, dy, color); \
          if (fntw==5) \
            goto next; \
          if (dx+6>=VGAX_WIDTH) \
            goto next; \
          if (dx+6>-1 && m&(1<<1)) \
            putpixel(dx+6, dy, color); \
          if (fntw==6) \
            goto next; \
          if (dx+7>=VGAX_WIDTH) \
            goto next; \
          if (dx+7>-1 && m&(1<<0)) \
            putpixel(dx+7, dy, color); \
      next: \
        dy++; \
      } \
      dx+=fntw+1; \
    } else { \
      dx+=hspace; \
    } \
  }

void VGAX::printPROGMEM(uint8_t *fnt, uint8_t glyphscount, uint8_t fntheight, 
                        uint8_t hspace, uint8_t vspace, const char *str, 
                        char dx, char dy0, uint8_t color)
{
    char dx0 = dx;
    char *pstr= (char*)str;
    char c;

    while (c=pgm_read_byte(pstr++))
    {
        PRINT_LOOP_CODE  
    }
}

void VGAX::printSRAM(uint8_t *fnt, uint8_t glyphscount, uint8_t fntheight, uint8_t hspace, 
                     uint8_t vspace, const char *str, char dx, char dy0, 
                     uint8_t color)
{
    char dx0 = dx;
    char *pstr=(char*)str;
    char c;

    while ((c = *pstr++))
    {
        PRINT_LOOP_CODE  
    }
}

//HSYNC pin used by TIMER2
#define HSYNCPIN 3

//VSYNC pin used by TIMER1. Can be 9 or 10
#define VSYNCPIN 9

//Number of VGA lines to be skipped (black lines)
/*These lines includes the vertical sync pulse and back porch.
Minimum value must be 35 (calculate from Nick Gammon)
You can modify this value to center the framebuffer vertically, or not*/
#define SKIPLINES 90

//number of visible lines, after SKIPLINES
#define RENDERLCOUNT 359

static uint8_t afreq, afreq0;
unsigned vtimer;
static uint8_t aline, rlinecnt;
static uint8_t vskip;
uint8_t vgaxfb[VGAX_HEIGHT*VGAX_BWIDTH];

//VSYNC interrupt
ISR(TIMER1_OVF_vect) {
  aline=-1;
  vskip=SKIPLINES;
  vtimer++;
  rlinecnt=0;
}
//HSYNC interrupt
ISR(TIMER2_OVF_vect)
{
#if 1
  asm volatile(                                   //4c to load Z and Y
    "      ld r16, Z                        \n\t" //c1 r16=afreq
    "      cpi %[freq0], 0                  \n\t" //c1 afreq0==0 ?
    "      breq no_audio                    \n\t" //c1/2 *0
    "play_audio:                            \n\t" 
    "      cpi r16, 0                       \n\t" //c1 afreq==0 ?
    "      brne dont_flip_audio_pin         \n\t" //c1/2 *1
    "flip_audio_pin:                        \n\t" 
    "      ldi r18, 1                       \n\t" //c1
    "      out %[audiopin], r18             \n\t" //c1
    "      st Z, %[freq0]                   \n\t" //c1 afreq=afreq0
    "      rjmp end                         \n\t" //c2
    //"    mov r16, %[freq0]\n\r"
    //"    dec r16\n\r"
    "no_audio:                              \n\t" 
    "      nop                              \n\t" //c1
    "      nop                              \n\t" //c1
    "      nop                              \n\t" //c1
    //"    nop                              \n\t" //c1
    "      nop                              \n\t" //c1
    "      nop                              \n\t" //c1
    "      nop                              \n\t" //c1
    "      rjmp end                         \n\t" //c2
    "dont_flip_audio_pin:                   \n\t" 
    "      dec r16                          \n\t" //c1
    "      st Z, r16                        \n\t" //c1
    //"    nop                              \n\t" //c1
    "end:                                   \n\t"
  :
  : "z" (&afreq),
    [freq0] "r" (afreq0),
    [audiopin] "i" _SFR_IO_ADDR(PINC)
  : "r16", "r18");

  //check vertical porch
  if (vskip) {
      vskip--;
      return;
  }
#endif
  if (rlinecnt<60) {   
    #define DEJITTER_OFFSET 1
    #define DEJITTER_SYNC -3
#if 1
    asm volatile(
      "     lds r16, 0x46\n\t"
      "     subi r16, %[tsync]    \n\t" 
      "     andi r16, 7           \n\t"
      "     call TL               \n\t"
      "TL:                        \n\t" 
      "     pop r31               \n\t" 
      "     pop r30               \n\t" 
      "     adiw r30, (LW-TL-5)   \n\t" 
      "     add r30, r16          \n\t" 
      "     ijmp                  \n\t" 
      "LW:                        \n\t" 
      "     nop\n\t"
      "     nop\n\t" 
      "     nop\n\t" 
      "     nop\n\t" 
      "     nop\n\t" 
      "     nop\n\t" 
      "     nop\n\t"
    :
    : [tsync] "i" ((uint8_t)DEJITTER_SYNC)
    : "r30", "r31", "r16", "r17"
);
#endif
#if 1
    asm volatile (
      "    ldi r20, 4       \n\t" //const for <<2bit
#ifdef VGAX_DEV_DEPRECATED
      ".rept 14             \n\t" //center line
      "    nop              \n\t" //
      ".endr                \n\t" //
#endif
      ".rept 30             \n\t" //output 4 pixels for each iteration
      "    ld r16, Z+       \n\t" //
      "    out %[port], r16 \n\t" //write pixel 1
      "    mul r16, r20     \n\t" //<<2
      "    out %[port], r0  \n\t" //write pixel 2
      "    mul r0, r20      \n\t" //<<4
      "    out %[port], r0  \n\t" //write pixel 3
      "    mul r0, r20      \n\t" //<<6
      "    out %[port], r0  \n\t" //write pixel 4
      ".endr                \n\t" //
      "    nop              \n\t" //expand last pixel
      "    ldi r16, 0       \n\t" //
      "    out %[port], r16 \n\t" //write black for next pixels
    :
    : [port] "I" (_SFR_IO_ADDR(PORTD)),
      "z" "I" (/*rline*/(uint8_t *)vgaxfb + rlinecnt*VGAX_BWIDTH)
    : "r16", "r17", "r20", "r21", "memory");

    if (++aline==5) { 
      aline=-1;
      rlinecnt++;
    } else {
#ifdef VGAX_DEV_DEPRECATED
      asm volatile(
        ".rept 17 \n\t" //
        "    nop  \n\t" //
        ".endr    \n\t" //
      :::);
#endif
    }
#endif
  } 
}
void VGAX::begin(bool enableTone)
{
    //Timers setup code, modified version of the Nick Gammon's VGA sketch
    cli();
    //setup audio pin
    if (enableTone)
        DDRC |= 1<<0;
    
    *p_timsk0=0;
    *p_tccr0a=0;
    *p_tccr0b=1; //enable 16MHz counter (used to fix the HSYNC interrupt jitter)
    *p_ocr0a=0;
    *p_ocr0b=0;
    *p_tcnt0=0;
    *p_ddr_ocr1a |= 1<<ocr1a_bit;
    *p_tccr1a = 1<<wgm11 | 1<<com1a1;
    *p_tccr1b = 1<<wgm12 | 1<<wgm13 | 1<<cs12 | 1<<cs10; //1024 prescaler
    *p_icr1=259; //16666 / 64 uS=260 (less one)
    *p_ocr1a=0; //64 / 64 uS=1 (less one)
    *p_tifr1 = 1<<tov1; //clear overflow flag
    *p_timsk1 = 1<<toie1; //interrupt on overflow on TIMER1
    *p_ddr_ocr2b |= 1<<ocr2b_bit;
    *p_tccr2a = 1<<wgm20 | 1<<wgm21 | 1<<com2b1; //pin3=COM2B1
    *p_tccr2b = 1<<wgm22 | 1<<cs21; //8 prescaler
    *p_ocr2a = 63; //32 / 0.5 uS=64 (less one)
    *p_ocr2b = 7; //4 / 0.5 uS=8 (less one)
    *p_tifr2 = 1<<tov2; //clear overflow flag
    *p_timsk2 = 1<<toie2; //interrupt on overflow on TIMER2
    *p_ddrd |= 1<<6 | 1<<7;
    sei();
}
void VGAX::end()
{
    TCCR0A=0;
    TCCR0B=0;
    TCCR1A=0;
    TCCR1B=0;
    TCCR2A=0;
    TCCR2B=0;
}
void VGAX::clear(uint8_t color) {
    register uint8_t c=color;
    c&=3;
    register uint8_t c0=(c*4) | c;
    c0|=c0*16;
    uint16_t cnt = VGAX_BSIZE;
    uint8_t *o=(uint8_t *)vgaxfb;

    while (cnt--)
        *o++=c0;
}
void VGAX::copy(uint8_t *src)
{
    uint8_t *o = (uint8_t *)vgaxfb;
    uint16_t cnt = VGAX_BSIZE;

    while (cnt--)
        *o++=pgm_read_byte(src++);
}
void VGAX::fillrect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color)
{
    uint8_t rh = height;

    while (rh--)
    {
        uint8_t rw=width, rx=x;

        while (rw--)
        {
            if (rx<VGAX_WIDTH && y<VGAX_HEIGHT)
                putpixel(rx, y, color);

            rx++;
        }
        y++;
    }
}
void VGAX::tone(unsigned int frequency)
{
    //HSYNC=32usec
    afreq=1000000 / frequency / 2 / 32;
    afreq0=afreq;
}

void VGAX::noTone()
{
    afreq0=0;
}

void VGAX::delay(int msec)
{
    while (msec--)
    {
        unsigned cnt=16000/32; //TODO: use a more precise way to calculate cnt

        while (cnt--)
            asm volatile("nop\nnop\nnop\nnop\n");
    }
}

class VGAXUtils {
public:
  //porting of TVOut::draw_line method, writen by Myles Metzer
  static inline void draw_line(uint8_t x0, uint8_t y0, 
                               uint8_t x1, uint8_t y1, uint8_t c) {
    if (x0>VGAX_WIDTH || y0>VGAX_HEIGHT || x1>VGAX_WIDTH || y1>VGAX_HEIGHT)
      return;
    if (x0==x1)
      VGAXUtils::draw_column(x0, y0, y1, c);
    else if (y0==y1)
      VGAXUtils::draw_row(y0, x0, x1, c);
    else {
      int e;
      signed int dx, dy, j, temp;
      signed char s1, s2, xchange;
      signed int x, y;
      x=x0;
      y=y0;
      //take absolute value
      if (x1<x0) {
        dx=x0-x1;
        s1=-1;
      } else if (x1==x0) {
        dx=0;
        s1=0;
      } else {
        dx=x1-x0;
        s1=1;
      }
      if (y1<y0) {
        dy=y0-y1;
        s2=-1;
      } else if (y1==y0) {
        dy=0;
        s2=0;
      } else {
        dy=y1-y0;
        s2=1;
      }
      xchange=0;   
      if (dy>dx) {
        temp=dx;
        dx=dy;
        dy=temp;
        xchange=1;
      } 
      e=((int)dy<<1)-dx;  
      for (j=0; j<=dx; j++) {
        VGAX::putpixel(x, y, c);
        if (e>=0) {
          if (xchange==1) 
            x=x+s1;
          else 
            y=y+s2;
          e=e-((int)dx<<1);
        }
        if (xchange==1)
          y=y+s2;
        else
          x=x+s1;
        e=e+((int)dy<<1);
      }
    }
  }
  //porting of TVOut::draw_circle method, writen by Myles Metzer
  static inline void draw_circle(uint8_t x0, uint8_t y0, 
                                 uint8_t radius, uint8_t c, char fc) {
    int f=1-radius;
    int ddF_x=1;
    int ddF_y=-2 * radius;
    int x=0;
    int y=radius;
    uint8_t pyy=y, pyx=x;
    //there is a fill color

    if (fc!=-1)
      VGAXUtils::draw_row(y0, x0-radius, x0+radius, fc);

    VGAX::putpixel(x0, y0+radius, c);
    VGAX::putpixel(x0, y0-radius, c);
    VGAX::putpixel(x0+radius, y0, c);
    VGAX::putpixel(x0-radius, y0, c);
    
    while (x < y) {
      if (f >= 0) {
        y--;
        ddF_y += 2;
        f += ddF_y;
      }
      x++;
      ddF_x += 2;
      f += ddF_x;
      //there is a fill color
      if (fc != -1) {
        //prevent double draws on the same rows
        if (pyy != y) {
          VGAXUtils::draw_row(y0+y, x0-x, x0+x, fc);
          VGAXUtils::draw_row(y0-y, x0-x, x0+x, fc);
        }
        if (pyx != x && x != y) {
          VGAXUtils::draw_row(y0+x, x0-y, x0+y, fc);
          VGAXUtils::draw_row(y0-x, x0-y, x0+y, fc);
        }
        pyy=y;
        pyx=x;
      }
      VGAX::putpixel(x0+x, y0+y, c);
      VGAX::putpixel(x0-x, y0+y, c);
      VGAX::putpixel(x0+x, y0-y, c);
      VGAX::putpixel(x0-x, y0-y, c);
      VGAX::putpixel(x0+y, y0+x, c);
      VGAX::putpixel(x0-y, y0+x, c);
      VGAX::putpixel(x0+y, y0-x, c);
      VGAX::putpixel(x0-y, y0-x, c);
    }
  }
  //porting of TVOut::draw_rect method, writen by Myles Metzer
  static inline void draw_rect(uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, 
                               uint8_t c, char fc) {
    if (fc != -1) {
      for (unsigned char i=y0; i < y0+h; i++) {
        VGAXUtils::draw_row(i, x0, x0+w, fc);
      }
    }
    VGAXUtils::draw_line(x0,   y0,   x0+w, y0,   c);
    VGAXUtils::draw_line(x0,   y0,   x0,   y0+h, c);
    VGAXUtils::draw_line(x0+w, y0,   x0+w, y0+h, c);
    VGAXUtils::draw_line(x0,   y0+h, x0+w, y0+h, c);
  }
  //porting of TVOut::draw_row method, writen by Myles Metzer
  static inline void draw_row(uint8_t line, uint8_t x0, uint8_t x1, uint8_t c) {
    if (x0 == x1) {
      VGAX::putpixel(x0, line, c);
    } else {
      if (x0 > x1) {
        uint8_t tmp=x0;
        x0=x1;
        x1=tmp;
      }
      while (x0 < x1) {
        VGAX::putpixel(x0++, line, c);
      }
    }
  }
  //porting of TVOut::draw_column method, writen by Myles Metzer
  static inline void draw_column(uint8_t row, uint8_t y0, uint8_t y1, uint8_t c) {
    if (y0 == y1) {
      VGAX::putpixel(row, y0, c);
    } else {
      if (y1 < y0) {
        uint8_t tmp=y0;
        y0=y1;
        y1=tmp;
      }
      while (y0 < y1) {
        VGAX::putpixel(row, y0++, c);
      }
    }
  }
  //porting of TVOut::draw_line method, writen by Myles Metzer
  //Safe version with screen clipping
  static inline void draw_line_safe(uint8_t x0, uint8_t y0, 
                                    uint8_t x1, uint8_t y1, uint8_t c) {
    if (x0>VGAX_WIDTH || y0>VGAX_HEIGHT || x1>VGAX_WIDTH || y1>VGAX_HEIGHT)
      return;
    if (x0==x1)
      VGAXUtils::draw_column_safe(x0, y0, y1, c);
    else if (y0==y1)
      VGAXUtils::draw_row_safe(y0, x0, x1, c);
    else {
      int e;
      signed int dx, dy, j, temp;
      signed char s1, s2, xchange;
      signed int x, y;
      x=x0;
      y=y0;
      //take absolute value
      if (x1<x0) {
        dx=x0-x1;
        s1=-1;
      } else if (x1==x0) {
        dx=0;
        s1=0;
      } else {
        dx=x1-x0;
        s1=1;
      }
      if (y1<y0) {
        dy=y0-y1;
        s2=-1;
      } else if (y1==y0) {
        dy=0;
        s2=0;
      } else {
        dy=y1-y0;
        s2=1;
      }
      xchange=0;   
      if (dy>dx) {
        temp=dx;
        dx=dy;
        dy=temp;
        xchange=1;
      } 
      e=((int)dy<<1)-dx;  
      for (j=0; j<=dx; j++) {
        putpixel_safe(x, y, c);
        if (e>=0) {
          if (xchange==1) 
            x=x+s1;
          else 
            y=y+s2;
          e=e-((int)dx<<1);
        }
        if (xchange==1)
          y=y+s2;
        else
          x=x+s1;
        e=e+((int)dy<<1);
      }
    }
  }
  //porting of TVOut::draw_circle method, writen by Myles Metzer
  //Safe version with screen clipping
  static inline void draw_circle_safe(uint8_t x0, uint8_t y0, 
                                      uint8_t radius, uint8_t c, char fc) {
    int f=1-radius;
    int ddF_x=1;
    int ddF_y=-2 * radius;
    int x=0;
    int y=radius;
    uint8_t pyy=y, pyx=x;
    //there is a fill color
    if (fc!=-1)
      VGAXUtils::draw_row_safe(y0, x0-radius, x0+radius, fc);
    VGAX::putpixel(x0, y0+radius, c);
    VGAX::putpixel(x0, y0-radius, c);
    VGAX::putpixel(x0+radius, y0, c);
    VGAX::putpixel(x0-radius, y0, c);
    
    while (x < y) {
      if (f >= 0) {
        y--;
        ddF_y += 2;
        f += ddF_y;
      }
      x++;
      ddF_x += 2;
      f += ddF_x;
      //there is a fill color
      if (fc != -1) {
        //prevent double draws on the same rows
        if (pyy != y) {
          VGAXUtils::draw_row_safe(y0+y, x0-x, x0+x, fc);
          VGAXUtils::draw_row_safe(y0-y, x0-x, x0+x, fc);
        }
        if (pyx != x && x != y) {
          VGAXUtils::draw_row_safe(y0+x, x0-y, x0+y, fc);
          VGAXUtils::draw_row_safe(y0-x, x0-y, x0+y, fc);
        }
        pyy=y;
        pyx=x;
      }
      putpixel_safe(x0+x, y0+y, c);
      putpixel_safe(x0-x, y0+y, c);
      putpixel_safe(x0+x, y0-y, c);
      putpixel_safe(x0-x, y0-y, c);
      putpixel_safe(x0+y, y0+x, c);
      putpixel_safe(x0-y, y0+x, c);
      putpixel_safe(x0+y, y0-x, c);
      putpixel_safe(x0-y, y0-x, c);
    }
  }
  //porting of TVOut::draw_rect method, writen by Myles Metzer
  //Safe version with screen clipping
  static inline void draw_rect_safe(uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, 
                                    uint8_t c, char fc) {
    if (fc != -1) {
      for (unsigned char i=y0; i < y0+h; i++) {
        VGAXUtils::draw_row_safe(i, x0, x0+w, fc);
      }
    }
    VGAXUtils::draw_line_safe(x0,   y0,   x0+w, y0,   c);
    VGAXUtils::draw_line_safe(x0,   y0,   x0,   y0+h, c);
    VGAXUtils::draw_line_safe(x0+w, y0,   x0+w, y0+h, c);
    VGAXUtils::draw_line_safe(x0,   y0+h, x0+w, y0+h, c);
  }
  //porting of TVOut::draw_row method, writen by Myles Metzer
  //Safe version with screen clipping
  static inline void draw_row_safe(uint8_t line, uint8_t x0, uint8_t x1, uint8_t c) {
    if (x0 == x1) {
      putpixel_safe(x0, line, c);
    } else {
      if (x0 > x1) {
        uint8_t tmp=x0;
        x0=x1;
        x1=tmp;
      }
      while (x0 < x1) {
        putpixel_safe(x0++, line, c);
      }
    }
  }
  //porting of TVOut::draw_column method, writen by Myles Metzer
  //Safe version with screen clipping
  static inline void draw_column_safe(uint8_t row, uint8_t y0, uint8_t y1, uint8_t c) {
    if (y0 == y1) {
      putpixel_safe(row, y0, c);
    } else {
      if (y1 < y0) {
        uint8_t tmp=y0;
        y0=y1;
        y1=tmp;
      }
      while (y0 < y1) {
        putpixel_safe(row, y0++, c);
      }
    }
  }
  static inline void putpixel_safe(char x, char y, uint8_t color) {
    if (x>=VGAX_WIDTH || y>=VGAX_HEIGHT || x<0 || y<0)
      return;
    VGAX::putpixel(x, y, color);
  }
};

static constexpr uint8_t FNT_NANOFONT_HEIGHT = 6;
static constexpr uint8_t FNT_NANOFONT_SYMBOLS_COUNT = 95;
static constexpr uint8_t PADDLE_HEIGHT = 8;
static constexpr uint8_t PADDLE_WIDTH = 2;
static constexpr uint8_t RIGHT_PADDLE_X = VGAX_WIDTH - 4;
static constexpr uint8_t LEFT_PADDLE_X = 2;
#define MAX_Y_VELOCITY 0.1

static VGAX vga;
static VGAXUtils vgaU;

//data size=570 bytes
static const uint8_t
fnt_nanofont_data[FNT_NANOFONT_SYMBOLS_COUNT][1+FNT_NANOFONT_HEIGHT] PROGMEM=
{
    { 1, 128, 128, 128, 0, 128, 0, }, //glyph '!' code=0
    { 3, 160, 160, 0, 0, 0, 0, }, //glyph '"' code=1
    //{ 5, 80, 248, 80, 248, 80, 0, },  //glyph '#' code=2
    //{ 5, 248, 248, 248, 248, 248, 0, },  //glyph '#' code=2 - full rectangle
    { 5, 240, 240, 240, 240, 240, 240, },  //glyph '#' = 'full rectangle' code=2
    //{ 5, 32, 160, 240, 120, 32, 32, },  //glyph '#' = 'planeR' code=2
    { 3, 80, 32, 112, 112, 32, 0, },  //glyph '$' = 'bomb' code=3
    { 5, 32, 40, 120, 240, 32, 32, },  //glyph '%' = 'planeL' code=4
    { 5, 96, 144, 104, 144, 104, 0, },  //glyph '&' code=5
    { 5, 248, 248, 248, 248, 248, 248, },  //glyph ''' = 'rectangle 5 x 6' code=6
    //{ 5, 120, 160, 112, 40, 240, 0, },  //glyph '$' code=3
    //{ 5, 136, 16, 32, 64, 136, 0, },  //glyph '%' code=4
    //{ 5, 96, 144, 104, 144, 104, 0, },  //glyph '&' code=5
    //{ 2, 128, 64, 0, 0, 0, 0, },  //glyph ''' code=6
    { 2, 64, 128, 128, 128, 64, 0, }, //glyph '(' code=7
{ 2, 128, 64, 64, 64, 128, 0, },  //glyph ')' code=8
{ 3, 0, 160, 64, 160, 0, 0, },  //glyph '*' code=9
{ 3, 0, 64, 224, 64, 0, 0, }, //glyph '+' code=10
{ 2, 0, 0, 0, 0, 128, 64, },  //glyph ',' code=11
{ 3, 0, 0, 224, 0, 0, 0, }, //glyph '-' code=12
{ 1, 0, 0, 0, 0, 128, 0, }, //glyph '.' code=13
{ 5, 8, 16, 32, 64, 128, 0, },  //glyph '/' code=14
{ 4, 96, 144, 144, 144, 96, 0, }, //glyph '0' code=15
{ 3, 64, 192, 64, 64, 224, 0, },  //glyph '1' code=16
{ 4, 224, 16, 96, 128, 240, 0, }, //glyph '2' code=17
{ 4, 224, 16, 96, 16, 224, 0, },  //glyph '3' code=18
{ 4, 144, 144, 240, 16, 16, 0, }, //glyph '4' code=19
{ 4, 240, 128, 224, 16, 224, 0, },  //glyph '5' code=20
{ 4, 96, 128, 224, 144, 96, 0, }, //glyph '6' code=21
{ 4, 240, 16, 32, 64, 64, 0, }, //glyph '7' code=22
{ 4, 96, 144, 96, 144, 96, 0, },  //glyph '8' code=23
{ 4, 96, 144, 112, 16, 96, 0, },  //glyph '9' code=24
{ 1, 0, 128, 0, 128, 0, 0, }, //glyph ':' code=25
{ 2, 0, 128, 0, 0, 128, 64, },  //glyph ';' code=26
{ 3, 32, 64, 128, 64, 32, 0, }, //glyph '<' code=27
{ 3, 0, 224, 0, 224, 0, 0, }, //glyph '=' code=28
{ 3, 128, 64, 32, 64, 128, 0, },  //glyph '>' code=29
{ 4, 224, 16, 96, 0, 64, 0, },  //glyph '?' code=30
{ 4, 96, 144, 176, 128, 112, 0, },  //glyph '@' code=31
{ 4, 96, 144, 240, 144, 144, 0, },  //glyph 'A' code=32
{ 4, 224, 144, 224, 144, 224, 0, }, //glyph 'B' code=33
{ 4, 112, 128, 128, 128, 112, 0, }, //glyph 'C' code=34
{ 4, 224, 144, 144, 144, 224, 0, }, //glyph 'D' code=35
{ 4, 240, 128, 224, 128, 240, 0, }, //glyph 'E' code=36
{ 4, 240, 128, 224, 128, 128, 0, }, //glyph 'F' code=37
{ 4, 112, 128, 176, 144, 112, 0, }, //glyph 'G' code=38
{ 4, 144, 144, 240, 144, 144, 0, }, //glyph 'H' code=39
{ 3, 224, 64, 64, 64, 224, 0, },  //glyph 'I' code=40
{ 4, 240, 16, 16, 144, 96, 0, },  //glyph 'J' code=41
{ 4, 144, 160, 192, 160, 144, 0, }, //glyph 'K' code=42
{ 4, 128, 128, 128, 128, 240, 0, }, //glyph 'L' code=43
{ 5, 136, 216, 168, 136, 136, 0, }, //glyph 'M' code=44
{ 4, 144, 208, 176, 144, 144, 0, }, //glyph 'N' code=45
{ 4, 96, 144, 144, 144, 96, 0, }, //glyph 'O' code=46
{ 4, 224, 144, 224, 128, 128, 0, }, //glyph 'P' code=47
{ 4, 96, 144, 144, 144, 96, 16, },  //glyph 'Q' code=48
{ 4, 224, 144, 224, 160, 144, 0, }, //glyph 'R' code=49
{ 4, 112, 128, 96, 16, 224, 0, }, //glyph 'S' code=50
{ 3, 224, 64, 64, 64, 64, 0, }, //glyph 'T' code=51
{ 4, 144, 144, 144, 144, 96, 0, },  //glyph 'U' code=52
{ 3, 160, 160, 160, 160, 64, 0, },  //glyph 'V' code=53
{ 5, 136, 168, 168, 168, 80, 0, },  //glyph 'W' code=54
{ 4, 144, 144, 96, 144, 144, 0, },  //glyph 'X' code=55
{ 3, 160, 160, 64, 64, 64, 0, },  //glyph 'Y' code=56
{ 4, 240, 16, 96, 128, 240, 0, }, //glyph 'Z' code=57
{ 2, 192, 128, 128, 128, 192, 0, }, //glyph '[' code=58
{ 5, 128, 64, 32, 16, 8, 0, },  //glyph '\' code=59
{ 2, 192, 64, 64, 64, 192, 0, },  //glyph ']' code=60
{ 5, 32, 80, 136, 0, 0, 0, }, //glyph '^' code=61
{ 4, 0, 0, 0, 0, 240, 0, }, //glyph '_' code=62
{ 2, 128, 64, 0, 0, 0, 0, },  //glyph '`' code=63
{ 3, 0, 224, 32, 224, 224, 0, },  //glyph 'a' code=64
{ 3, 128, 224, 160, 160, 224, 0, }, //glyph 'b' code=65
{ 3, 0, 224, 128, 128, 224, 0, }, //glyph 'c' code=66
{ 3, 32, 224, 160, 160, 224, 0, },  //glyph 'd' code=67
{ 3, 0, 224, 224, 128, 224, 0, }, //glyph 'e' code=68
{ 2, 64, 128, 192, 128, 128, 0, },  //glyph 'f' code=69
{ 3, 0, 224, 160, 224, 32, 224, },  //glyph 'g' code=70
{ 3, 128, 224, 160, 160, 160, 0, }, //glyph 'h' code=71
{ 1, 128, 0, 128, 128, 128, 0, }, //glyph 'i' code=72
{ 2, 0, 192, 64, 64, 64, 128, },  //glyph 'j' code=73
{ 3, 128, 160, 192, 160, 160, 0, }, //glyph 'k' code=74
{ 1, 128, 128, 128, 128, 128, 0, }, //glyph 'l' code=75
{ 5, 0, 248, 168, 168, 168, 0, }, //glyph 'm' code=76
{ 3, 0, 224, 160, 160, 160, 0, }, //glyph 'n' code=77
{ 3, 0, 224, 160, 160, 224, 0, }, //glyph 'o' code=78
{ 3, 0, 224, 160, 160, 224, 128, }, //glyph 'p' code=79
{ 3, 0, 224, 160, 160, 224, 32, },  //glyph 'q' code=80
{ 3, 0, 224, 128, 128, 128, 0, }, //glyph 'r' code=81
{ 2, 0, 192, 128, 64, 192, 0, },  //glyph 's' code=82
{ 3, 64, 224, 64, 64, 64, 0, }, //glyph 't' code=83
{ 3, 0, 160, 160, 160, 224, 0, }, //glyph 'u' code=84
{ 3, 0, 160, 160, 160, 64, 0, },  //glyph 'v' code=85
{ 5, 0, 168, 168, 168, 80, 0, },  //glyph 'w' code=86
{ 3, 0, 160, 64, 160, 160, 0, },  //glyph 'x' code=87
{ 3, 0, 160, 160, 224, 32, 224, },  //glyph 'y' code=88
{ 2, 0, 192, 64, 128, 192, 0, },  //glyph 'z' code=89
{ 3, 96, 64, 192, 64, 96, 0, }, //glyph '{' code=90
{ 1, 128, 128, 128, 128, 128, 0, }, //glyph '|' code=91
{ 3, 192, 64, 96, 64, 192, 0, },  //glyph '}' code=92
//{ 3, 96, 192, 0, 0, 0, 0, },  //glyph '~' code=93
{ 5, 32, 160, 240, 120, 32, 32, }, //= 'planeR' code=93
{ 4, 48, 64, 224, 64, 240, 0, },  //glyph '£' code=94
};

static const char str0[] PROGMEM="0"; 
static const char str1[] PROGMEM="1"; 
static const char str2[] PROGMEM="2"; 
static const char str3[] PROGMEM="3"; 
static const char str4[] PROGMEM="4"; 
static const char str5[] PROGMEM="5"; 
static const char str6[] PROGMEM="6"; 
static const char str7[] PROGMEM="7"; 
static const char str8[] PROGMEM="8"; 
static const char str9[] PROGMEM="9"; 
static const char str10[] PROGMEM="#"; 
static const char str15[] PROGMEM="Red wins!"; 
static const char str16[] PROGMEM="Green wins!"; 
static const char str18[] PROGMEM="Game Over!!!"; 
static const char str21[] PROGMEM="########";
static const char str30[] PROGMEM="Choose Arduino Game"; 
static const char str31[] PROGMEM="Pong"; 
static const char str32[] PROGMEM="Breakout"; 
static const char str33[] PROGMEM="Bomber"; 
static const char str34[] PROGMEM="Etch-a-Sketch"; 
static const char str35[] PROGMEM="Tetris"; 
static const char str40[] PROGMEM="X"; 
static const char str51[] PROGMEM="~"; //planeR
static const char str52[] PROGMEM="%"; //planeL
static const char str53[] PROGMEM="$"; //bomb
static const char str54[] PROGMEM="'"; //rectangle 5 x 6
static const char str59[] PROGMEM="'''''''"; //clear 'Hit' banner 
static const char str510[] PROGMEM="Hit!!!"; //'Hit' banner
static const char str511[] PROGMEM="Arduino VGA Bomber"; 
static const char str512[] PROGMEM="by Roberto Melzi"; 
static const char str515[] PROGMEM="Game Over!!!"; 



// ************* Variable definition Pong ***************************************
uint8_t x,y;
bool buttonStatus = 0;
bool buttonOneStatus = 0;
bool buttonTwoStatus = 0;
uint16_t wheelOnePosition;
uint16_t wheelTwoPosition;
uint8_t state = 1;
int rightPaddleY;
int leftPaddleY;
int rightPaddleY0 = 30;
int leftPaddleY0 = 30;
uint8_t ballX = 4;
uint8_t ballY = 20;
uint8_t ballX0 = 21;
uint8_t ballY0 = 21; 
float ballPX = 20;
float ballPY = 20; 
float ballVx;
float ballVy;
uint8_t scoreR = 0;
uint8_t scoreL = 0; 
int PaddleHit = 0; 
uint8_t ticPosition = 12; 
int padPosition; 
int padPositionOld; 
uint8_t ballXold;
uint8_t ballYold; 
float ballFX = 70.;
float ballFY = 30.;
float speedX; 
float speedY; 
float speedT; 
float angle; 
float angleDeg = 45.; 
uint8_t gameStep = 0; 
bool cancelSafe = 0;
uint8_t hitScore = 0; 
uint8_t lives = 9; 
int distanceX;
int distanceY; 
int iDel;
int jDel; 
int ballXhit; 
int ballYhit; 
static const float pi PROGMEM = 3.1415296;
int nBricks = 0;
int color = 1; 
static const float speedIncrement PROGMEM = 1.25992105; 
int beginning = 0;
int shot = 12;
int bombX;
int bombX0;
int score = 40;
int frame = 0;
int bombX_0;
uint8_t planeX;
uint8_t planeY;
uint8_t planeX0;
uint8_t planeY0;

void parameterPongIni()
{
    rightPaddleY0 = rightPaddleY < 30 ? rightPaddleY + 1 : rightPaddleY - 1;
    leftPaddleY0 = leftPaddleY < 30 ? leftPaddleY + 1 : leftPaddleY - 1;
    ballVx = random(50., 80.)/500. + (scoreL + scoreR)/35;
    ballVy = random(40.)/1000. - 0.02;
}

void parameterIniBomber()
{ 
    speedX = 10 + random(VGAX_WIDTH - 20);
    speedY = 10 + random(VGAX_HEIGHT/4);
    bombX0 = speedX;
    ballVx = speedY + 4;
    speedT = 0.1 + random(20)/100.;
    angle = speedT - random(-10,10)/100.;
    angleDeg = random(10)/200.;
    score = 40; 
    lives = 0;
    buttonOneStatus = 0; 
}

void parameterUpdate()
{
    angle = angleDeg/180* pi;
    speedX = speedX > 0 ? sin(angle) * speedT : -sin(angle) * speedT;
    speedY = speedY > 0 ? cos(angle) * speedT : -cos(angle) * speedT;
}

void vgaPrint(const char* str, uint8_t x, uint8_t y, uint8_t color)
{
    vga.printPROGMEM((uint8_t *)fnt_nanofont_data, FNT_NANOFONT_SYMBOLS_COUNT,
        FNT_NANOFONT_HEIGHT, 3, 1, str, x, y, color);
}

void vgaTone(int freq, int time) {
   vga.tone(freq);
   vga.delay(time); 
   vga.noTone(); 
}

void drawBall(){ // Pong
   if ((ballX != ballX0) || (ballY != ballY0)){
      vga.putpixel(ballX0, ballY0, 0);
      vga.putpixel(ballX0, ballY0 + 1, 0);
      vga.putpixel(ballX0 + 1, ballY0, 0);
      vga.putpixel(ballX0 + 1, ballY0 + 1, 0);
      vga.putpixel(ballX, ballY, 3);
      vga.putpixel(ballX, ballY + 1, 3);
      vga.putpixel(ballX + 1, ballY, 3);
      vga.putpixel(ballX + 1, ballY + 1, 3);
   }
}

void drawBallBreakout(int x, int y, int col) { // Breakout
   vga.putpixel(x, y, col); 
   vga.putpixel(x + 1, y, col); 
   vga.putpixel(x, y + 1, col); 
   vga.putpixel(x + 1, y + 1, col);
}

void ballCoordinatesBreakout() { // Breakout
  ballXold = ballX; 
  ballYold = ballY; 
  ballFX += speedX;
  ballFY += speedY;
  ballX = int(ballFX); 
  ballY = int(ballFY); 
}

void cancelPad() { // Breakout
   vgaU.draw_line(padPositionOld - 6, 58, padPositionOld + 6, 58, 0);
   vgaU.draw_line(padPositionOld - 6, 57, padPositionOld + 6, 57, 0);
}

void drawPadBreakout() { // Breakout
   vgaU.draw_line(padPosition - 6, 58, padPosition + 6, 58, 1);
   vgaU.draw_line(padPosition - 6, 57, padPosition + 6, 57, 1);
}

void drawBorder() { // Pong
    vgaU.draw_line(0,0,VGAX_WIDTH-1,0,3);
    vgaU.draw_line(0,VGAX_HEIGHT-1,VGAX_WIDTH-1,VGAX_HEIGHT-1,3);
    vgaU.draw_line(0,0,0,VGAX_HEIGHT-1,3);
    vgaU.draw_line(VGAX_WIDTH-1,0,VGAX_WIDTH-1,VGAX_HEIGHT,3);
}

void drawPaddles()
{
    rightPaddleY = ((wheelOnePosition / 8) * (VGAX_HEIGHT-PADDLE_HEIGHT-1))/ 128 + 1;
    x = RIGHT_PADDLE_X;
 
    if (rightPaddleY != rightPaddleY0 || PaddleHit > 0){
      vgaU.draw_column(x,rightPaddleY0,rightPaddleY0+PADDLE_HEIGHT,0);
      vgaU.draw_column(x+1,rightPaddleY0,rightPaddleY0+PADDLE_HEIGHT,0);
      vgaU.draw_column(x,rightPaddleY,rightPaddleY+PADDLE_HEIGHT,2);
      vgaU.draw_column(x+1,rightPaddleY,rightPaddleY+PADDLE_HEIGHT,2);
    }
    // ----------------- draw left paddle -------------------------------------------------
    leftPaddleY = ((wheelTwoPosition / 8) * (VGAX_HEIGHT-PADDLE_HEIGHT-1))/ 128 + 1;
    x = LEFT_PADDLE_X;
    if (leftPaddleY != leftPaddleY0 || PaddleHit > 0){
       vgaU.draw_column(x,leftPaddleY0,leftPaddleY0+PADDLE_HEIGHT,0);
       vgaU.draw_column(x+1,leftPaddleY0,leftPaddleY0+PADDLE_HEIGHT,0);
       vgaU.draw_column(x,leftPaddleY,leftPaddleY+PADDLE_HEIGHT,1);
       vgaU.draw_column(x+1,leftPaddleY,leftPaddleY+PADDLE_HEIGHT,1);
    }
}

void drawNet()
{
    for (int i=1; i<VGAX_HEIGHT - 4; i += 6) {
        vgaU.draw_column(VGAX_WIDTH/2, i, i + 3, 3);
    }
}

void drawGameScreen() { // Pong 
    drawBall(); 
    drawPaddles(); 
    drawNet(); 
    ballX0 = ballX;
    ballY0 = ballY; 
    leftPaddleY0 = leftPaddleY;
    rightPaddleY0 = rightPaddleY; 
}

void drawPongStartScreen() { // Pong 
   vgaPrint(str10, 52, 4, 0);
   vgaPrint(str10, 64, 4, 0);
   vgaPrint(str0, 52, 4, 1);
   vgaPrint(str0, 64, 4, 2);
   vgaPrint(str16, 66, 24, 0);
   vgaPrint(str15, 12, 24, 0);
   vgaPrint(str21, 11, 24, 0);
   vgaPrint(str21, 65, 24, 0);
   drawBorder(); 
   drawGameScreen(); 
   buttonStatus = 0;
   vga.delay(200);
}

void drawBorderBreakout() { // Breakout 
   vgaU.draw_line(0, 0, 0, 59, 3);
   vgaU.draw_line(0, 0, 99, 0, 3);
   vgaU.draw_line(0, 59, 99, 59, 3);
   vgaU.draw_line(99, 0, 99, 60, 3);
}

void brick(int x, int y, int col) { // Breakout
   vgaU.draw_line(x - 5, y + 1, x + 5, y + 1 , col);
   vgaU.draw_line(x - 5, y - 1, x + 5, y - 1 , col);
   vga.putpixel(x - 5, y, col); 
   vga.putpixel(x + 4, y, col); 
}

static void drawBricksGrid(int n)
{
    nBricks = 0;

    if (n % 3 == 0)
    {
        for (int i = 1; i <= int(100/13 - 1); i++) {
            for (int j = 1; j < 5; j++) {
                color = (i + j)%3 + 1; 
                brick(8 + i*12, 3 + j*4, color); 
                if (color |= 0) {nBricks++;}
            }
        }
    }

  if (n%3 == 1)
  {   
    for (int i = 0; i <= int(100/13); i++) {
      for (int j = 0; j < 7; j++) {
         color = (i + j)%4 + 0; 
         brick(8 + i*12, 3 + j*4, color); 
         if (color |= 0) {nBricks++;}
      }
    }
  }
  if (n%3 == 2)
  {   
    for (int i = 0; i <= int(100/13); i++) {
      for (int j = 0; j < 7; j++) {
         color = (i + j)%3 + 1; 
         brick(8 + i*12, 3 + j*4, color); 
         if (color |= 0) {nBricks++;}
      }
    }
  }
}

void drawLives();



void gameIniBreakout()
{
    lives = 9; 
    speedT = .06; 
    gameStep = 0; 
    beginning = 0; 
    angleDeg = 180/4; 
    parameterUpdate(); 
    drawLives(); 
    drawBorderBreakout();
    cancelPad(); 
    drawPadBreakout(); 
    drawBricksGrid(gameStep); 
}

static void processInputs()
{
    buttonOneStatus = PINC & 1<<4 ? false : true;
    buttonTwoStatus = PINC & 1<<2 ? false : true;
    wheelOnePosition = analog.read(Analog2::ADC1);
    wheelTwoPosition = analog.read(Analog2::ADC3);
    buttonStatus = buttonOneStatus || buttonTwoStatus;
}

void drawStartMenu(){
   if (state == 0) {
      state = 1; 
      ballX = 4;
      ballY = 20;
      ballX0 = 21;
      ballY0 = 21; 
      vga.clear(0);
   }
   vgaPrint(str30, 20, 2, 3); 
   vgaPrint(str31, 38, 12, 2); 
   vgaPrint(str32, 38, 20, 2); 
   vgaPrint(str33, 38, 28, 2); 
   vgaPrint(str34, 38, 36, 2); 
   //vgaPrint(str10, 70, 52, 3); 
   //vgaPrint(str35, 38, 44, 2); 
   vgaPrint(str40, 26, ticPosition, 1); 
   vga.delay(50);
   processInputs(); 
   if (buttonTwoStatus == 1){
      buttonOneStatus = 0;
      vgaPrint(str40, 26, ticPosition, 0); 
      ticPosition += 8; 
      if (ticPosition > 36) {ticPosition = 12;}
      vgaPrint(str40, 26, ticPosition, 1);
      vga.delay(200);
   }
   if (buttonOneStatus == 1){ 
      buttonOneStatus = 0;
      buttonTwoStatus = 0;
      buttonStatus = 0; 
      vga.clear(0);
      if (ticPosition == 12) { // Pong
         state = 2;
         ballY = ((wheelTwoPosition / 8) * (VGAX_HEIGHT-PADDLE_HEIGHT-1))/ 128 + 4;
         drawPongStartScreen(); 
      } 
      if (ticPosition == 20) { // Breakout 
         state = 4;
         buttonTwoStatus = 0; 
         buttonOneStatus = 0; 
         buttonStatus = 0; 
         gameIniBreakout(); 
         vga.delay(200); 
      } 
      if (ticPosition == 28) { // Bomber
         parameterIniBomber(); 
         state = 6;
      } 
      if (ticPosition == 36) {state = 7;} // drawingToy
      //if (ticPosition == 44) {state = 5;} // Tetris
   }
}

void ballStart()
{
    // Breakout
    ballFX = padPosition; 
    ballFY = 55; 
    ballCoordinatesBreakout(); 
    ballY = 55; 
    drawBallBreakout(ballX, ballY, 2); 
    //while (buttonStatus == 0 && state != 1){

    while (buttonStatus == 0 && state == 4)
    {
      padPositionOld = padPosition; 
      processInputs();
      padPosition = map(wheelOnePosition, 1023, 0, 7, 93);

      if (padPosition != padPositionOld){
         cancelPad(); 
         drawPadBreakout();
         drawBallBreakout(ballX, ballY, 0); 
         ballFX = padPosition; 
         ballFY = 55; 
         ballCoordinatesBreakout(); 
         ballY = 55; 
         drawBallBreakout(ballX, ballY, 2); 
      }
      if (buttonTwoStatus == 1){
         state = 0;
         buttonTwoStatus = 0; 
         buttonOneStatus = 0; 
         buttonStatus = 0; 
         vga.clear(0);
         vga.delay(300);
         drawStartMenu();
      }
   }
}

void gameOverBreakout() { // Breakout
  vgaTone(660, 200); 
  vgaTone(330, 200);
  vgaTone(165, 200); 
  vgaTone(82, 200);
  vgaPrint(str18, 25, 40, 1);
  ballStart(); 
  //if (state != 1){ 
  if (state == 4){ 
     vga.clear(0); 
     gameIniBreakout(); 
  }
  vga.delay(300);
  buttonStatus = 0;
  buttonOneStatus = 0; 
  buttonTwoStatus = 0; 
}

void drawLives() { // Breakout
    vgaPrint(str10, 110, 10, 0);
    if(lives == 9) {vgaPrint(str9, 110, 10, 3);}
    if(lives == 8) {vgaPrint(str8, 110, 10, 3);}
    if(lives == 7) {vgaPrint(str7, 110, 10, 3);}
    if(lives == 6) {vgaPrint(str6, 110, 10, 3);}
    if(lives == 5) {vgaPrint(str5, 110, 10, 3);}
    if(lives == 4) {vgaPrint(str4, 110, 10, 3);}
    if(lives == 3) {vgaPrint(str3, 110, 10, 3);}
    if(lives == 2) {vgaPrint(str2, 110, 10, 3);}
    if(lives == 1) {vgaPrint(str1, 110, 10, 3);}
    if(lives == 0) {gameOverBreakout();} 
}

void drawScore()
{ // Pong
    vgaPrint(str10, 52, 4, 0);
    vgaPrint(str10, 64, 4, 0);
    if(scoreL == 0) {vgaPrint(str0, 52, 4, 1);} 
    if(scoreR == 0) {vgaPrint(str0, 64, 4, 2);}
    if(scoreL == 1) {vgaPrint(str1, 52, 4, 1);} 
    if(scoreR == 1) {vgaPrint(str1, 64, 4, 2);}
    if(scoreL == 2) {vgaPrint(str2, 52, 4, 1);} 
    if(scoreR == 2) {vgaPrint(str2, 64, 4, 2);}
    if(scoreL == 3) {vgaPrint(str3, 52, 4, 1);} 
    if(scoreR == 3) {vgaPrint(str3, 64, 4, 2);}
    if(scoreL == 4) {vgaPrint(str4, 52, 4, 1);} 
    if(scoreR == 4) {vgaPrint(str4, 64, 4, 2);}
    if(scoreL == 5) {vgaPrint(str5, 52, 4, 1);} 
    if(scoreR == 5) {vgaPrint(str5, 64, 4, 2);}
    if(scoreL == 6) {vgaPrint(str6, 52, 4, 1);} 
    if(scoreR == 6) {vgaPrint(str6, 64, 4, 2);}
    if(scoreL == 7) {vgaPrint(str7, 52, 4, 1);} 
    if(scoreR == 7) {vgaPrint(str7, 64, 4, 2);}

    if(scoreL == 8)
    {
        vgaPrint(str8, 52, 4, 1);
        vgaPrint(str15, 12, 24, 1);
        ballX = VGAX_WIDTH - 6; 
        buttonStatus = 0;

        while(buttonStatus == 0)
        {
            processInputs(); 
            ballY = ((wheelOnePosition / 8) * (VGAX_HEIGHT-PADDLE_HEIGHT-1))/ 128 + 4;
            drawGameScreen(); 
        }
    }

    if(scoreR == 8) {
       vgaPrint(str8, 64, 4, 2);
       vgaPrint(str16, 66, 24, 2);
       ballX = 4; 
       buttonStatus = 0; 
       while(buttonStatus == 0){
          processInputs(); 
          ballY = ((wheelTwoPosition / 8) * (VGAX_HEIGHT-PADDLE_HEIGHT-1))/ 128 + 4;
          drawGameScreen(); 
       }
    }
    if (scoreL == 8 || scoreR == 8)
    {
        buttonStatus = 0;

        do
        { 
            processInputs(); 
        }
        while (buttonStatus == 0);

        if (buttonOneStatus == 1)
        {
          state = 0;
          vga.delay(300);
       }
       else{
          vga.delay(300);
          parameterPongIni();
          drawPongStartScreen(); 
          state = 2; 
       }
    }
}





void waitForStart()
{
    // Pong
    buttonStatus = 0;

    if (scoreR == 8 || scoreR == 0)
    {
        processInputs(); 
        ballY = ((wheelTwoPosition / 8) * (VGAX_HEIGHT-PADDLE_HEIGHT-1))/ 128 + 4;
        drawGameScreen(); 
    }

    if (scoreL == 8)
    {
        processInputs(); 
        ballY = ((wheelOnePosition / 8) * (VGAX_HEIGHT-PADDLE_HEIGHT-1))/ 128 + 4;
        drawGameScreen(); 
    }

    if (buttonStatus == 1)
    {
       ballPX = ballX; 
       ballPY = ballY;
       parameterPongIni(); 
       scoreL = 0; 
       scoreR = 0; 
       drawScore(); 
       buttonStatus = 0;
       state = 3; 
    }
}

void drawPlaneBomber()
{
    if (speedT > 0 && planeX != planeX0)
    {
        vgaPrint(str54, planeX0, planeY0, 0); 
        vgaPrint(str51, planeX, planeY, 1);
    }

    if (speedT < 0 && planeX != planeX0)
    {
        vgaPrint(str54, planeX0, planeY0, 0); 
        vgaPrint(str52, planeX, planeY, 1);
    }
}

static void drawBombBomber()
{
   vgaPrint(str53, bombX_0, ballVy, 0);
   vgaPrint(str53, bombX, ballFY, 2);
}

static void drawBombBomberard()
{
    if (padPosition != padPositionOld)
    {
        vgaU.draw_column(padPositionOld, VGAX_HEIGHT-4, VGAX_HEIGHT-7, 0);
        vgaU.draw_row(VGAX_HEIGHT-4, padPositionOld-2, padPositionOld+3, 0);
        vgaU.draw_row(VGAX_HEIGHT-3, padPositionOld-5, padPositionOld+6, 0);
        vgaU.draw_row(VGAX_HEIGHT-2, padPositionOld-5, padPositionOld+6, 0);
        vgaU.draw_column(padPosition, VGAX_HEIGHT-4, VGAX_HEIGHT-7, 3);
        vgaU.draw_row(VGAX_HEIGHT-4, padPosition-2, padPosition+3, 3);
        vgaU.draw_row(VGAX_HEIGHT-3, padPosition-5, padPosition+6, 3);
        vgaU.draw_row(VGAX_HEIGHT-2, padPosition-5, padPosition+6, 3);
    }
}

static void drawShotBomber()
{
// --------------------- draw shot ----------------------------------------------
    if (beginning == shot - 1)
    {
        vgaU.draw_column(padPositionOld, 2, VGAX_HEIGHT - 7, 0);
    }

    if (beginning == shot)
    {
        vgaU.draw_column(padPosition, 2, VGAX_HEIGHT - 7, 1);
        vga.delay(5); 
    }
}

static void drawScoreBomber()
{ // Bomber
    vgaU.draw_line(VGAX_WIDTH - (116 - 1), 1, VGAX_WIDTH-score-1, 1, 0);  
    vga.putpixel(VGAX_WIDTH - 116, 1, (lives + 2)%3 + 1);
    vgaU.draw_line(VGAX_WIDTH - 1 , 1, VGAX_WIDTH-score, 1, 3);  
}

static void drawGameScreenBomber()
{
    drawPlaneBomber();
    drawBombBomber();
    drawScoreBomber(); 
    vgaU.draw_row(VGAX_HEIGHT-1, VGAX_WIDTH, 1, 2); 
    drawBombBomberard(); 
    drawShotBomber(); 
}

static void nextlivesBreakout()
{
    gameStep++; 
    speedT = speedIncrement*speedT; 
    drawBricksGrid(gameStep); 
}

void searchHitBrick(int bX, int bY) { // Breakout
   distanceX = 120;
   distanceY = 60;
   for (int i = 0; i <= int(100/(13)); i++) {
      for (int j = 0; j < 7; j++) {
         if (abs(8 + i*12 - bX) < distanceX){
            distanceX = abs(8 + i*12 - bX); 
            iDel = i;
         }
         if (abs(1 + 2 + j*4 - bY) < distanceY){
            distanceY = abs(1 + 2 + j*4 - bY); 
            jDel = j;
         }
      }
   }
   brick(8 + iDel*12, 3 + jDel*4, 0); 
   nBricks--; 
   if (nBricks == 0){nextlivesBreakout();}
}

void gameOverSoundBomber() { // Bomber
   vga.delay(300);
   vgaTone(660, 200); 
   vgaTone(330, 200);
   vgaTone(165, 200); 
   vgaTone(82, 200);
}

void bomberHitSoundBomber() { //Bomber
   vgaTone(880, 200); 
   vgaTone(440, 200); 
   vgaTone(220, 200);
   vgaTone(110, 200); 
}

void nextlivesSoundBomber() { //Bomber
   vgaTone(440, 100);
   vgaTone(880, 100);
}

void drawingToy()
{ 
  processInputs(); 
  ballX = map(wheelOnePosition, 0, 1024, VGAX_WIDTH - 1, 1);
  ballY = map(wheelTwoPosition, 0, 1024, VGAX_HEIGHT - 1, 1);
  vga.putpixel(ballX, ballY, color);
  if (buttonOneStatus == 1) {
     vga.clear(0);
     vgaTone(262, 80);
     vgaTone(330, 80);
     vgaTone(392, 80);
     vgaTone(524, 80);
     //buttonOneStatus = 0; 
  }
  if (buttonTwoStatus == 1) {
    if (color == 3) 
       {color = 0;} 
    else 
       {if (color == 2) 
           {color = 3;}
       else 
           {if (color == 1) 
               {color = 2;}
            else
               {if (color == 0) {color = 1;}}
           }
       }
    //buttonTwoStatus = 0;
    vgaTone(440, 60);  
    vga.delay(200); 
  }
  if(buttonOneStatus == 1 && buttonTwoStatus == 1) {
     state = 0;
     //ballX = 4;
     //ballY = 20;
  }
}

void tetris(){
}

void pong()
{
     ballPX += ballVx;
     ballPY += ballVy;
     ballX = int(ballPX);
     ballY = int(ballPY);
    
     if(ballPY <= 1 || ballPY >= VGAX_HEIGHT - 2 ) {
        ballVy = -ballVy;
        ballPY += ballVy;
        ballY = int(ballPY);
        vgaTone(880,30); 
     }; 
     
     // ---------------------------------------- left paddle hit -------------------------------------------------
     if(ballVx < 0 && ballX == LEFT_PADDLE_X+PADDLE_WIDTH -1 && ballY >= leftPaddleY - 1 && ballY <= leftPaddleY + PADDLE_HEIGHT) { 
        ballVx = -ballVx;
        ballVy += 0.2 * ((ballY - leftPaddleY + 1) - (PADDLE_HEIGHT / 2)) / (PADDLE_HEIGHT / 2);
        vgaTone(660, 30);
        drawScore();
        PaddleHit = 2; 
        drawPaddles(); 
      }
     //----------------------------------------- right paddle hit --------------------------------------------------
     if(ballVx > 0 && ballX == RIGHT_PADDLE_X - 1 && ballY >= rightPaddleY - 1 && ballY <= rightPaddleY + PADDLE_HEIGHT) { 
        ballVx = -ballVx;
        ballVy += 0.2 * ((ballY - rightPaddleY + 1) - (PADDLE_HEIGHT / 2)) / (PADDLE_HEIGHT / 2);
        vgaTone(660, 30);
        drawScore();
        PaddleHit = 2; 
        drawPaddles(); 
     }
     
     //limit vertical speed
     if(ballVy > MAX_Y_VELOCITY)
        ballVy = MAX_Y_VELOCITY;

     if(ballVy < -MAX_Y_VELOCITY)
        ballVy = -MAX_Y_VELOCITY;
     
     if(ballX <= 0) { // ------------------------- ball out from left ------------------------------------------
        vgaTone(220, 200);
        scoreR++; 
        drawScore();  
        ballX = 4;   
        PaddleHit = 1;  
        while(buttonTwoStatus == 0){
           processInputs(); 
           ballY = ((wheelTwoPosition / 8) * (VGAX_HEIGHT-PADDLE_HEIGHT-1))/ 128 + 4;
           drawGameScreen(); 
           PaddleHit = 0; 
        }
        ballPX = ballX; 
        ballPY = ballY;
        parameterPongIni(); 
        drawBorder();
     }
     if(ballX >= VGAX_WIDTH - 2) { // ------------ ball out from right ------------------------------------------
        vgaTone(220, 200); 
        scoreL++; 
        drawScore();      
        ballX = VGAX_WIDTH - 6;   
        PaddleHit = 1; 
        while(buttonOneStatus == 0){
           processInputs(); 
           ballY = ((wheelOnePosition / 8) * (VGAX_HEIGHT-PADDLE_HEIGHT-1))/ 128 + 4;
           drawGameScreen(); 
           PaddleHit = 0; 
        }
        ballPX = ballX; 
        ballPY = ballY;
        parameterPongIni(); 
        drawBorder(); 
        ballVx = -ballVx;
     }
     //if (state == 1) {
     //if (state != 2) {
     if (state == 1 || state == 0) {
        vga.clear(0); 
        vga.delay(300);
     }
     else {drawGameScreen();} // ************************************* Pong end check ******************************
     PaddleHit -= 1; 
     PaddleHit = constrain(PaddleHit, 0, 10); 
  }
// ---------------------------------------- void pong() end ---------------------------------------------



// ----------------------------------------------- breakout -----------------------------------
void breakout()
{
    if (beginning == 0)
    { 
        padPositionOld = padPosition; 
        processInputs();
        padPosition = map(wheelOnePosition, 1023, 0, 7, 93); 
        gameIniBreakout(); 
        ballStart(); 
        beginning = 1; 
    }

    padPositionOld = padPosition; 
    processInputs();
    padPosition = map(wheelOnePosition, 1023, 0, 7, 93); 

    if (padPosition != padPositionOld)
    {
        cancelPad();
        drawPadBreakout();
    }
  
    ballCoordinatesBreakout(); 
  
    if ((ballX != ballXold) | (ballY != ballYold))
    { 
        if (cancelSafe == 0)
        {
            drawBallBreakout(ballXold, ballYold, 0); 
            cancelSafe = 1; 
        }

     if (vga.getpixel(ballX, ballY) != 0) {hitScore += 1;}
     if (vga.getpixel(ballX + 1, ballY) != 0) {hitScore += 2;}
     if (vga.getpixel(ballX, ballY + 1) != 0) {hitScore += 4;}
     if (vga.getpixel(ballX + 1, ballY + 1) != 0) {hitScore += 8;}
     if (hitScore == 0) // *************************************** ball did not hit anything and can go ahead ************
     {
        drawBallBreakout(ballX, ballY, 2); 
        cancelSafe = 0; 
     }
     else // ******************************* ball has hit something  ***********************
     {
        if ((hitScore == 3) | (hitScore == 12)) 
        {
           speedY = -speedY;
        }
        else
        {
           if ((hitScore == 5) | (hitScore == 10)) 
           {
              speedX = -speedX;
           }
           else 
           {
              speedX = -speedX;
              speedY = -speedY;
           }
        }
        if (ballY > 55) 
        {
            if ((vga.getpixel(ballX, ballY + 1) == 1) | (vga.getpixel(ballX + 1, ballY + 1) == 1))
            // ******* ball hits the paddle *************
           {
              angleDeg = angleDeg - 2*speedX / abs(speedX) * (padPosition - ballX); 
              if (angleDeg < 20) {angleDeg = 20;}
              if (angleDeg > 70) {angleDeg = 70;}
              parameterUpdate(); 
           } 
           else // *************** ball hits the bottom *************************
           {
              
              lives = lives - 1; 
              if (lives |= 0){vgaTone(110,100);}
              drawLives(); 
              //if (state != 1 && state != 0) {ballStart();}
              if (state == 4 ) {ballStart();} 
              cancelSafe = 0; 
              speedY = - abs(speedY); 
           }
        }
        else if (ballX > 1 && ballX + 1 < 99 && ballY > 1) // ball hits a brick
        {
           vgaTone(440,30);
            if ((hitScore == 1) | (hitScore == 3))
            {ballXhit = ballX; ballYhit = ballY;}

           if ((hitScore == 2) | (hitScore == 10)) {ballXhit = ballX + 1; ballYhit = ballY;}
           if ((hitScore == 8) | (hitScore == 12)) {ballXhit = ballX + 1; ballYhit = ballY + 1;}
           if ((hitScore == 4) | (hitScore == 5)) {ballXhit = ballX; ballYhit = ballY + 1;}
           searchHitBrick(ballXhit, ballYhit); 
        }
        hitScore = 0; 
      }
   }
}

static void bomber()
{ 
    if (beginning == shot)
    {  
        if (padPosition > speedX - 2 - lives && padPosition < speedX + 2 + lives)
        { // plane is hit ------------------
            speedX = -10; 
            score += 8;
            vga.tone(110);
            vga.delay(60); 
            vga.noTone();
        }

        if (padPosition > (bombX - 2 ) && padPosition < (bombX + 2))
        {
            ballVx = speedY + 4;
            bombX0 = speedX;
            angle = speedT - 0.1*lives + random(-100,100)/1000.;
            angleDeg = 0.1*lives + random(100)/1000.;
            iDel = 0;
            score += 8; 
            vga.tone(165);
            vga.delay(60); 
            vga.noTone(); 
        }
    }
            
    if(speedX > VGAX_WIDTH || speedX < 0)
    {
        score += -3 - lives; 
        speedT = random(200)/1000.0 + 0.2 + 0.2*lives;
        speedY = random(VGAX_HEIGHT*2/3*100)/100.0 + 2;

        if (random(1000) > 500)
        {
            speedX = 10;
        } 
        else
        {
           speedX = VGAX_WIDTH-10; 
           speedT = -speedT; 
        }
     }
     
     speedX += speedT;
        
        bombX = bombX0 + angle*iDel;            
        ballFY = ballVx + 0.6*angleDeg*iDel + 0.08*iDel*iDel/(60 - 20*lives);
        if(ballFY > VGAX_HEIGHT - 5 || bombX < 0 || bombX > VGAX_WIDTH){
          if(bombX >= padPosition - 6 && bombX < padPosition + 6){ // Bomber has been hit
             vgaPrint(str510, 50, 10, 1); 
             bomberHitSoundBomber(); 
             vgaPrint(str59, 50, 10, 0); 
             vgaPrint(str59, 51, 10, 0); 
             score += -30; 
          }
          ballVx = speedY + 4;
          bombX0 = speedX;
          angle = speedT - 0.1*lives + random(-100,100)/1000.; 
          angleDeg = 0.1*lives + random(100)/1000.; 
          iDel = 0;
       }
       else
       {iDel = iDel + 1;}
     
       if(beginning != 0 ) {
          beginning += -1; 
       }
       if ( buttonOneStatus == 1 && beginning == 0) {
          score += -.5; 
          beginning = shot; 
          padPositionOld = padPosition;
          buttonOneStatus = 0; 
        }
         
        if(score < 0) { // --------------- Game Over !!! ------------------------ 
           buttonOneStatus = 0; 
           gameOverSoundBomber(); 
           vgaPrint(str515, 40, 20, 1); 
           parameterIniBomber(); 
           padPositionOld = padPosition + 1;   
           do
           {
              processInputs(); 
           } while (buttonStatus == 0);  
           vga.clear(0); 
           if (buttonOneStatus == 0){state = 0;}
           vga.delay(300);
        }
        if (score > 116) {
          score = 58;
          nextlivesSoundBomber(); 
          lives++; 
    }

    planeX = uint8_t(speedX);
    planeY = uint8_t(speedY); 
    padPosition = int((((1024 - wheelOnePosition) / 8) * (VGAX_WIDTH - 12))/ 128 + 6);

    if (state == 1)
        vga.clear(0); 
    else
        drawGameScreenBomber(); 

    bombX_0 = bombX;
    ballVy = ballFY; 
    planeX0 = planeX;
    planeY0 = planeY;
    padPositionOld = padPosition; 
}

void loop()
{
    processInputs();

    switch (state)
    {
    case 0:
    case 1:
        drawStartMenu();
        break;
    case 2:
        waitForStart();
        break;
    case 3:
        pong();
        break;
    case 4:
        breakout();
        break;
    case 5:
        tetris();
        break;
    case 6:
        bomber();
        break;
    case 7:
        drawingToy();
        break;
    }
}

int main()
{
    DDRC &= ~(1<<2 | 1<<4);
    PORTC |= 1<<2 | 1<<4;
    analog.init();
    vga.begin();

    while (true)
        loop();  
}



