/*
Uno:
RED: D6/OC0A/PD6
GREEN: D7/PD7
HSYNC: D3/OC2B/PD3
VSYNC: D9/OC1A/PB1
*/

#include "vgax.h"
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <math.h>
#include <stdlib.h>
#include "board.h"

//typedef unsigned char byte;

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
    //*p_ddrb |= 1<<1;
    *p_ddr_ocr1a |= 1<<ocr1a_bit;
    *p_tccr1a = 1<<wgm11 | 1<<com1a1;
    *p_tccr1b = 1<<wgm12 | 1<<wgm13 | 1<<cs12 | 1<<cs10; //1024 prescaler
    *p_icr1=259; //16666 / 64 uS=260 (less one)
    *p_ocr1a=0; //64 / 64 uS=1 (less one)
    *p_tifr1 = 1<<tov1; //clear overflow flag
    *p_timsk1 = 1<<toie1; //interrupt on overflow on TIMER1
    //*p_ddrd |= 1<<3;
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

#define FNT_NANOFONT_HEIGHT 6
#define FNT_NANOFONT_SYMBOLS_COUNT 95
#define PADDLE_HEIGHT 8
#define PADDLE_WIDTH 2
#define RIGHT_PADDLE_X (VGAX_WIDTH-4)
#define LEFT_PADDLE_X 2
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


