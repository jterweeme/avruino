#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdarg.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <avr/pgmspace.h>
#include <ctype.h>
#ifndef String_class_h
#define String_class_h
#ifdef __cplusplus
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <avr/pgmspace.h>

class __FlashStringHelper;
#define F(string_literal) (reinterpret_cast<const __FlashStringHelper *>(PSTR(string_literal)))

class StringSumHelper;

class String
{
    typedef void (String::*StringIfHelperType)() const;
    void StringIfHelper() const {}

public:
    String(const char *cstr = "");
    String(const String &str);
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    String(String &&rval);
    String(StringSumHelper &&rval);
#endif
    explicit String(char c);
    explicit String(unsigned char, unsigned char base=10);
    explicit String(int, unsigned char base=10);
    explicit String(unsigned int, unsigned char base=10);
    explicit String(long, unsigned char base=10);
    explicit String(unsigned long, unsigned char base=10);
    ~String(void);
    unsigned char reserve(unsigned int size);
    inline unsigned int length(void) const {return len;}
    String & operator = (const String &rhs);
    String & operator = (const char *cstr);
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    String & operator = (String &&rval);
    String & operator = (StringSumHelper &&rval);
#endif
    unsigned char concat(const String &str);
    unsigned char concat(const char *cstr);
    unsigned char concat(char c);
    unsigned char concat(unsigned char c);
    unsigned char concat(int num);
    unsigned char concat(unsigned int num);
    unsigned char concat(long num);
    unsigned char concat(unsigned long num);
    String & operator += (const String &rhs)    {concat(rhs); return (*this);}
    String & operator += (const char *cstr)     {concat(cstr); return (*this);}
    String & operator += (char c)           {concat(c); return (*this);}
    String & operator += (unsigned char num)        {concat(num); return (*this);}
    String & operator += (int num)          {concat(num); return (*this);}
    String & operator += (unsigned int num)     {concat(num); return (*this);}
    String & operator += (long num)         {concat(num); return (*this);}
    String & operator += (unsigned long num)    {concat(num); return (*this);}
    friend StringSumHelper & operator + (const StringSumHelper &lhs, const String &rhs);
    friend StringSumHelper & operator + (const StringSumHelper &lhs, const char *cstr);
    friend StringSumHelper & operator + (const StringSumHelper &lhs, char c);
    friend StringSumHelper & operator + (const StringSumHelper &lhs, unsigned char num);
    friend StringSumHelper & operator + (const StringSumHelper &lhs, int num);
    friend StringSumHelper & operator + (const StringSumHelper &lhs, unsigned int num);
    friend StringSumHelper & operator + (const StringSumHelper &lhs, long num);
    friend StringSumHelper & operator + (const StringSumHelper &lhs, unsigned long num);
    operator StringIfHelperType() const { return buffer ? &String::StringIfHelper : 0; }
    int compareTo(const String &s) const;
    unsigned char equals(const String &s) const;
    unsigned char equals(const char *cstr) const;
    unsigned char operator == (const String &rhs) const {return equals(rhs);}
    unsigned char operator == (const char *cstr) const {return equals(cstr);}
    unsigned char operator != (const String &rhs) const {return !equals(rhs);}
    unsigned char operator != (const char *cstr) const {return !equals(cstr);}
    unsigned char operator <  (const String &rhs) const;
    unsigned char operator >  (const String &rhs) const;
    unsigned char operator <= (const String &rhs) const;
    unsigned char operator >= (const String &rhs) const;
    unsigned char equalsIgnoreCase(const String &s) const;
    unsigned char startsWith( const String &prefix) const;
    unsigned char startsWith(const String &prefix, unsigned int offset) const;
    unsigned char endsWith(const String &suffix) const;
    char charAt(unsigned int index) const;
    void setCharAt(unsigned int index, char c);
    char operator [] (unsigned int index) const;
    char& operator [] (unsigned int index);
    void getBytes(unsigned char *buf, unsigned int bufsize, unsigned int index=0) const;
    void toCharArray(char *buf, unsigned int bufsize, unsigned int index=0) const
        {getBytes((unsigned char *)buf, bufsize, index);}
    const char * c_str() const { return buffer; }

    int indexOf( char ch ) const;
    int indexOf( char ch, unsigned int fromIndex ) const;
    int indexOf( const String &str ) const;
    int indexOf( const String &str, unsigned int fromIndex ) const;
    int lastIndexOf( char ch ) const;
    int lastIndexOf( char ch, unsigned int fromIndex ) const;
    int lastIndexOf( const String &str ) const;
    int lastIndexOf( const String &str, unsigned int fromIndex ) const;
    String substring( unsigned int beginIndex ) const;
    String substring( unsigned int beginIndex, unsigned int endIndex ) const;
    void replace(char find, char replace);
    void replace(const String& find, const String& replace);
    void toLowerCase(void);
    void toUpperCase(void);
    void trim(void);
    long toInt(void) const;
protected:
    char *buffer;
    unsigned int capacity;
    unsigned int len;
    unsigned char flags;
protected:
    void init(void);
    void invalidate(void);
    unsigned char changeBuffer(unsigned int maxStrLen);
    unsigned char concat(const char *cstr, unsigned int length);
    String & copy(const char *cstr, unsigned int length);
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    void move(String &rhs);
#endif
};

class StringSumHelper : public String
{
public:
    StringSumHelper(const String &s) : String(s) {}
    StringSumHelper(const char *p) : String(p) {}
    StringSumHelper(char c) : String(c) {}
    StringSumHelper(unsigned char num) : String(num) {}
    StringSumHelper(int num) : String(num) {}
    StringSumHelper(unsigned int num) : String(num) {}
    StringSumHelper(long num) : String(num) {}
    StringSumHelper(unsigned long num) : String(num) {}
};

#endif
#endif

#include <stddef.h>

void * operator new(size_t size);
void * operator new[](size_t size);
void operator delete(void * ptr);
void operator delete[](void * ptr);

__extension__ typedef int __guard __attribute__((mode (__DI__)));

extern "C" int __cxa_guard_acquire(__guard *);
extern "C" void __cxa_guard_release (__guard *);
extern "C" void __cxa_guard_abort (__guard *);
extern "C" void __cxa_pure_virtual(void);


class Print;

class Printable
{
  public:
    virtual size_t printTo(Print& p) const = 0;
};

static const uint8_t DEC = 10;
static const uint8_t HEX = 16;
static const uint8_t OCT = 8;
static const uint8_t BIN = 2;

class Print
{
private:
    int write_error;
    size_t printNumber(unsigned long, uint8_t);
    size_t printFloat(double, uint8_t);
protected:
    void setWriteError(int err = 1) { write_error = err; }
public:
    Print() : write_error(0) {}
    int getWriteError() { return write_error; }
    void clearWriteError() { setWriteError(0); }
    virtual size_t write(uint8_t) = 0;

    size_t write(const char *str)
    {
      if (str == NULL) return 0;
      return write((const uint8_t *)str, strlen(str));
    }

    virtual size_t write(const uint8_t *buffer, size_t size);
    size_t print(const __FlashStringHelper *);
    size_t print(const String &);
    size_t print(const char[]);
    size_t print(char);
    size_t print(unsigned char, int = DEC);
    size_t print(int, int = DEC);
    size_t print(unsigned int, int = DEC);
    size_t print(long, int = DEC);
    size_t print(unsigned long, int = DEC);
    size_t print(double, int = 2);
    size_t print(const Printable&);
    size_t println(const __FlashStringHelper *);
    size_t println(const String &s);
    size_t println(const char[]);
    size_t println(char);
    size_t println(unsigned char, int = DEC);
    size_t println(int, int = DEC);
    size_t println(unsigned int, int = DEC);
    size_t println(long, int = DEC);
    size_t println(unsigned long, int = DEC);
    size_t println(double, int = 2);
    size_t println(const Printable&);
    size_t println(void);
};


extern "C"{

static const uint8_t HIGH = 0x1;
static const uint8_t LOW = 0x0;

static const uint8_t INPUT = 0x0;
static const uint8_t OUTPUT = 0x1;
static const uint8_t INPUT_PULLUP = 0x2;

#define true 0x1
#define false 0x0

#define SERIAL  0x0
#define DISPLAY 0x1

#define LSBFIRST 0
#define MSBFIRST 1

#define CHANGE 1
#define FALLING 2
#define RISING 3

#if defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__) || defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
#define DEFAULT 0
#define EXTERNAL 1
#define INTERNAL 2
#else
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega644__) || defined(__AVR_ATmega644A__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644PA__)
#define INTERNAL1V1 2
#define INTERNAL2V56 3
#else
#define INTERNAL 3
#endif
#define DEFAULT 1
#define EXTERNAL 0
#endif

#ifdef abs
#undef abs
#endif

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#define abs(x) ((x)>0?(x):-(x))
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#define radians(deg) ((deg)*DEG_TO_RAD)
#define degrees(rad) ((rad)*RAD_TO_DEG)
#define sq(x) ((x)*(x))

#define interrupts() sei()
#define noInterrupts() cli()

#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )
#define microsecondsToClockCycles(a) ( (a) * clockCyclesPerMicrosecond() )

#define lowByte(w) ((uint8_t) ((w) & 0xff))
#define highByte(w) ((uint8_t) ((w) >> 8))

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))


typedef unsigned int word;

#define bit(b) (1UL << (b))

typedef uint8_t boolean;
typedef uint8_t byte;

void init(void);

void pinMode(uint8_t, uint8_t);
void digitalWrite(uint8_t, uint8_t);
int digitalRead(uint8_t);
int analogRead(uint8_t);
void analogReference(uint8_t mode);
void analogWrite(uint8_t, int);

unsigned long millis(void);
unsigned long micros(void);
void delay(unsigned long);
void delayMicroseconds(unsigned int us);
unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout);

void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val);
uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder);

void attachInterrupt(uint8_t, void (*)(void), int mode);
void detachInterrupt(uint8_t);

void setup(void);
void loop(void);

#define analogInPinToBit(P) (P)

extern const uint16_t PROGMEM port_to_mode_PGM[];
extern const uint16_t PROGMEM port_to_input_PGM[];
extern const uint16_t PROGMEM port_to_output_PGM[];

extern const uint8_t PROGMEM digital_pin_to_port_PGM[];
extern const uint8_t PROGMEM digital_pin_to_bit_mask_PGM[];
extern const uint8_t PROGMEM digital_pin_to_timer_PGM[];

#define digitalPinToPort(P) ( pgm_read_byte( digital_pin_to_port_PGM + (P) ) )
#define digitalPinToBitMask(P) ( pgm_read_byte( digital_pin_to_bit_mask_PGM + (P) ) )
#define digitalPinToTimer(P) ( pgm_read_byte( digital_pin_to_timer_PGM + (P) ) )
#define analogInPinToBit(P) (P)
#define portOutputRegister(P) ( (volatile uint8_t *)( pgm_read_word( port_to_output_PGM + (P))) )
#define portInputRegister(P) ( (volatile uint8_t *)( pgm_read_word( port_to_input_PGM + (P))) )
#define portModeRegister(P) ( (volatile uint8_t *)( pgm_read_word( port_to_mode_PGM + (P))) )

#define NOT_A_PIN 0
#define NOT_A_PORT 0

#define NOT_ON_TIMER 0
#define TIMER0A 1
#define TIMER0B 2
#define TIMER1A 3
#define TIMER1B 4
#define TIMER2  5
#define TIMER2A 6
#define TIMER2B 7

#define TIMER3A 8
#define TIMER3B 9
#define TIMER3C 10
#define TIMER4A 11
#define TIMER4B 12
#define TIMER4C 13
#define TIMER4D 14  
#define TIMER5A 15
#define TIMER5B 16
#define TIMER5C 17

}

inline boolean isAlphaNumeric(int c) __attribute__((always_inline));
inline boolean isAlpha(int c) __attribute__((always_inline));
inline boolean isAscii(int c) __attribute__((always_inline));
inline boolean isWhitespace(int c) __attribute__((always_inline));
inline boolean isControl(int c) __attribute__((always_inline));
inline boolean isDigit(int c) __attribute__((always_inline));
inline boolean isGraph(int c) __attribute__((always_inline));
inline boolean isLowerCase(int c) __attribute__((always_inline));
inline boolean isPrintable(int c) __attribute__((always_inline));
inline boolean isPunct(int c) __attribute__((always_inline));
inline boolean isSpace(int c) __attribute__((always_inline));
inline boolean isUpperCase(int c) __attribute__((always_inline));
inline boolean isHexadecimalDigit(int c) __attribute__((always_inline));
inline int toAscii(int c) __attribute__((always_inline));
inline int toLowerCase(int c) __attribute__((always_inline));
inline int toUpperCase(int c)__attribute__((always_inline));

inline boolean isAlphaNumeric(int c)
{
  return ( isalnum(c) == 0 ? false : true);
}

inline boolean isAlpha(int c)
{
  return ( isalpha(c) == 0 ? false : true);
}

inline boolean isAscii(int c)
{
  return ( isascii (c) == 0 ? false : true);
}

inline boolean isWhitespace(int c)
{
  return ( isblank (c) == 0 ? false : true);
}

inline boolean isControl(int c)
{
  return ( iscntrl (c) == 0 ? false : true);
}

inline boolean isDigit(int c)
{
  return ( isdigit (c) == 0 ? false : true);
}

inline boolean isGraph(int c)
{
  return ( isgraph (c) == 0 ? false : true);
}

inline boolean isLowerCase(int c)
{
  return (islower (c) == 0 ? false : true);
}

inline boolean isPrintable(int c)
{
  return ( isprint (c) == 0 ? false : true);
}

inline boolean isPunct(int c)
{
    return ( ispunct (c) == 0 ? false : true);
}

inline boolean isSpace(int c)
{
    return ( isspace (c) == 0 ? false : true);
}

inline boolean isUpperCase(int c)
{
    return ( isupper (c) == 0 ? false : true);
}

inline boolean isHexadecimalDigit(int c)
{
    return ( isxdigit (c) == 0 ? false : true);
}

inline int toAscii(int c)
{
    return toascii (c);
}

inline int toLowerCase(int c)
{
    return tolower (c);
}

inline int toUpperCase(int c)
{
    return toupper (c);
}

class Stream : public Print
{
protected:
    unsigned long _timeout;
    unsigned long _startMillis;
    int timedRead();
    int timedPeek();
    int peekNextDigit();
public:
    virtual int available() = 0;
    virtual int read() = 0;
    virtual int peek() = 0;
    virtual void flush() = 0;
    Stream() {_timeout=1000;}
    void setTimeout(unsigned long timeout);
    bool find(char *target);
    bool find(char *target, size_t length);
    bool findUntil(char *target, char *terminator);
    bool findUntil(char *target, size_t targetLen, char *terminate, size_t termLen);
    long parseInt();
    float parseFloat();
    size_t readBytes( char *buffer, size_t length);
    size_t readBytesUntil( char terminator, char *buffer, size_t length);
    String readString();
    String readStringUntil(char terminator);
protected:
    long parseInt(char skipChar);
    float parseFloat(char skipChar);
};

struct ring_buffer;

class HardwareSerial : public Stream
{
private:
    ring_buffer *_rx_buffer;
    ring_buffer *_tx_buffer;
    volatile uint8_t *_ubrrh;
    volatile uint8_t *_ubrrl;
    volatile uint8_t *_ucsra;
    volatile uint8_t *_ucsrb;
    volatile uint8_t *_ucsrc;
    volatile uint8_t *_udr;
    uint8_t _rxen;
    uint8_t _txen;
    uint8_t _rxcie;
    uint8_t _udrie;
    uint8_t _u2x;
    bool transmitting;
public:
    HardwareSerial(ring_buffer *rx_buffer, ring_buffer *tx_buffer,
      volatile uint8_t *ubrrh, volatile uint8_t *ubrrl,
      volatile uint8_t *ucsra, volatile uint8_t *ucsrb,
      volatile uint8_t *ucsrc, volatile uint8_t *udr,
      uint8_t rxen, uint8_t txen, uint8_t rxcie, uint8_t udrie, uint8_t u2x);
    void begin(unsigned long);
    void begin(unsigned long, uint8_t);
    void end();
    virtual int available(void);
    virtual int peek(void);
    virtual int read(void);
    virtual void flush(void);
    virtual size_t write(uint8_t);
    inline size_t write(unsigned long n) { return write((uint8_t)n); }
    inline size_t write(long n) { return write((uint8_t)n); }
    inline size_t write(unsigned int n) { return write((uint8_t)n); }
    inline size_t write(int n) { return write((uint8_t)n); }
    using Print::write;
    operator bool();
};

static const uint8_t SERIAL5_N1 = 0x00;
#define SERIAL_6N1 0x02
#define SERIAL_7N1 0x04
#define SERIAL_8N1 0x06
#define SERIAL_5N2 0x08
#define SERIAL_6N2 0x0A
#define SERIAL_7N2 0x0C
#define SERIAL_8N2 0x0E
#define SERIAL_5E1 0x20
#define SERIAL_6E1 0x22
#define SERIAL_7E1 0x24
#define SERIAL_8E1 0x26
#define SERIAL_5E2 0x28
#define SERIAL_6E2 0x2A
#define SERIAL_7E2 0x2C
#define SERIAL_8E2 0x2E
#define SERIAL_5O1 0x30
#define SERIAL_6O1 0x32
#define SERIAL_7O1 0x34
#define SERIAL_8O1 0x36
#define SERIAL_5O2 0x38
#define SERIAL_6O2 0x3A
#define SERIAL_7O2 0x3C
#define SERIAL_8O2 0x3E

extern HardwareSerial Serial;
extern void serialEventRun(void) __attribute__((weak));

uint16_t makeWord(uint16_t w);
uint16_t makeWord(byte h, byte l);

#define word(...) makeWord(__VA_ARGS__)

unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout = 1000000L);

static const uint8_t NUM_DIGITAL_PINS = 20;
static const uint8_t NUM_ANALOG_PINS = 6;
#define analogInputToDigitalPin(p)  ((p < 6) ? (p) + 14 : -1)

#define digitalPinHasPWM(p) ((p) == 3 || (p) == 5 || (p) == 6 || (p) == 9 || (p) == 10 || (p) == 11)

static const uint8_t SS   = 10;
static const uint8_t MOSI = 11;
static const uint8_t MISO = 12;
static const uint8_t SCK  = 13;

static const uint8_t SDA = 18;
static const uint8_t SCL = 19;
static const uint8_t LED_BUILTIN = 13;

static const uint8_t A0 = 14;
static const uint8_t A1 = 15;
static const uint8_t A2 = 16;
static const uint8_t A3 = 17;
static const uint8_t A4 = 18;
static const uint8_t A5 = 19;
static const uint8_t A6 = 20;
static const uint8_t A7 = 21;

#define digitalPinToPCICR(p)    (((p) >= 0 && (p) <= 21) ? (&PCICR) : ((uint8_t *)0))
#define digitalPinToPCICRbit(p) (((p) <= 7) ? 2 : (((p) <= 13) ? 0 : 1))
#define digitalPinToPCMSK(p)    (((p) <= 7) ? (&PCMSK2) : (((p) <= 13) ? (&PCMSK0) : (((p) <= 21) ? (&PCMSK1) : ((uint8_t *)0))))
#define digitalPinToPCMSKbit(p) (((p) <= 7) ? (p) : (((p) <= 13) ? ((p) - 8) : ((p) - 14)))



#ifndef SdFat_h
#define SdFat_h
#include <avr/pgmspace.h>

#include <stdint.h>
#include <avr/io.h>

struct pin_map_t {
  volatile uint8_t* ddr;
  volatile uint8_t* pin;
  volatile uint8_t* port;
  uint8_t bit;
};

uint8_t const SDA_PIN = 18;
uint8_t const SCL_PIN = 19;
uint8_t const SS_PIN = 10;
uint8_t const MOSI_PIN = 11;
uint8_t const MISO_PIN = 12;
uint8_t const SCK_PIN = 13;

static const pin_map_t digitalPinMap[] = {
  {&DDRD, &PIND, &PORTD, 0},  // D0  0
  {&DDRD, &PIND, &PORTD, 1},  // D1  1
  {&DDRD, &PIND, &PORTD, 2},  // D2  2
  {&DDRD, &PIND, &PORTD, 3},  // D3  3
  {&DDRD, &PIND, &PORTD, 4},  // D4  4
  {&DDRD, &PIND, &PORTD, 5},  // D5  5
  {&DDRD, &PIND, &PORTD, 6},  // D6  6
  {&DDRD, &PIND, &PORTD, 7},  // D7  7
  {&DDRB, &PINB, &PORTB, 0},  // B0  8
  {&DDRB, &PINB, &PORTB, 1},  // B1  9
  {&DDRB, &PINB, &PORTB, 2},  // B2 10
  {&DDRB, &PINB, &PORTB, 3},  // B3 11
  {&DDRB, &PINB, &PORTB, 4},  // B4 12
  {&DDRB, &PINB, &PORTB, 5},  // B5 13
  {&DDRC, &PINC, &PORTC, 0},  // C0 14
  {&DDRC, &PINC, &PORTC, 1},  // C1 15
  {&DDRC, &PINC, &PORTC, 2},  // C2 16
  {&DDRC, &PINC, &PORTC, 3},  // C3 17
  {&DDRC, &PINC, &PORTC, 4},  // C4 18
  {&DDRC, &PINC, &PORTC, 5}   // C5 19
};
static const uint8_t digitalPinCount = sizeof(digitalPinMap)/sizeof(pin_map_t);

uint8_t badPinNumber(void)
  __attribute__((error("Pin number is too large or not a constant")));

static inline __attribute__((always_inline))
  uint8_t getPinMode(uint8_t pin) {
  if (__builtin_constant_p(pin) && pin < digitalPinCount) {
    return (*digitalPinMap[pin].ddr >> digitalPinMap[pin].bit) & 1;
  } else {
    return badPinNumber();
  }
}
static inline __attribute__((always_inline))
  void setPinMode(uint8_t pin, uint8_t mode) {
  if (__builtin_constant_p(pin) && pin < digitalPinCount) {
    if (mode) {
      *digitalPinMap[pin].ddr |= 1 << digitalPinMap[pin].bit;
    } else {
      *digitalPinMap[pin].ddr &= ~(1 << digitalPinMap[pin].bit);
    }
  } else {
    badPinNumber();
  }
}
static inline __attribute__((always_inline))
  uint8_t fastDigitalRead(uint8_t pin) {
  if (__builtin_constant_p(pin) && pin < digitalPinCount) {
    return (*digitalPinMap[pin].pin >> digitalPinMap[pin].bit) & 1;
  } else {
    return badPinNumber();
  }
}
static inline __attribute__((always_inline))
  void fastDigitalWrite(uint8_t pin, uint8_t value) {
  if (__builtin_constant_p(pin) && pin < digitalPinCount) {
    if (value) {
      *digitalPinMap[pin].port |= 1 << digitalPinMap[pin].bit;
    } else {
      *digitalPinMap[pin].port &= ~(1 << digitalPinMap[pin].bit);
    }
  } else {
    badPinNumber();
  }
}

uint8_t const CMD0 = 0X00;
uint8_t const CMD8 = 0X08;
uint8_t const CMD9 = 0X09;
uint8_t const CMD10 = 0X0A;
uint8_t const CMD13 = 0X0D;
uint8_t const CMD17 = 0X11;
uint8_t const CMD24 = 0X18;
uint8_t const CMD25 = 0X19;
uint8_t const CMD32 = 0X20;
uint8_t const CMD33 = 0X21;
uint8_t const CMD38 = 0X26;
uint8_t const CMD55 = 0X37;
uint8_t const CMD58 = 0X3A;
uint8_t const ACMD23 = 0X17;
uint8_t const ACMD41 = 0X29;
uint8_t const R1_READY_STATE = 0X00;
uint8_t const R1_IDLE_STATE = 0X01;
uint8_t const R1_ILLEGAL_COMMAND = 0X04;
uint8_t const DATA_START_BLOCK = 0XFE;
uint8_t const STOP_TRAN_TOKEN = 0XFD;
uint8_t const WRITE_MULTIPLE_TOKEN = 0XFC;
uint8_t const DATA_RES_MASK = 0X1F;
uint8_t const DATA_RES_ACCEPTED = 0X05;
typedef struct CID {
  uint8_t mid;  // Manufacturer ID
  char oid[2];  // OEM/Application ID
  char pnm[5];  // Product name
  unsigned prv_m : 4;  // Product revision n.m
  unsigned prv_n : 4;
  // byte 9-12
  uint32_t psn;  // Product serial number
  // byte 13
  unsigned mdt_year_high : 4;  // Manufacturing date
  unsigned reserved : 4;
  // byte 14
  unsigned mdt_month : 4;
  unsigned mdt_year_low :4;
  // byte 15
  unsigned always1 : 1;
  unsigned crc : 7;
}cid_t;

typedef struct CSDV1 {
  // byte 0
  unsigned reserved1 : 6;
  unsigned csd_ver : 2;
  // byte 1
  uint8_t taac;
  // byte 2
  uint8_t nsac;
  // byte 3
  uint8_t tran_speed;
  // byte 4
  uint8_t ccc_high;
  // byte 5
  unsigned read_bl_len : 4;
  unsigned ccc_low : 4;
  // byte 6
  unsigned c_size_high : 2;
  unsigned reserved2 : 2;
  unsigned dsr_imp : 1;
  unsigned read_blk_misalign :1;
  unsigned write_blk_misalign : 1;
  unsigned read_bl_partial : 1;
  // byte 7
  uint8_t c_size_mid;
  // byte 8
  unsigned vdd_r_curr_max : 3;
  unsigned vdd_r_curr_min : 3;
  unsigned c_size_low :2;
  // byte 9
  unsigned c_size_mult_high : 2;
  unsigned vdd_w_cur_max : 3;
  unsigned vdd_w_curr_min : 3;
  // byte 10
  unsigned sector_size_high : 6;
  unsigned erase_blk_en : 1;
  unsigned c_size_mult_low : 1;
  // byte 11
  unsigned wp_grp_size : 7;
  unsigned sector_size_low : 1;
  // byte 12
  unsigned write_bl_len_high : 2;
  unsigned r2w_factor : 3;
  unsigned reserved3 : 2;
  unsigned wp_grp_enable : 1;
  unsigned write_partial : 1;
  unsigned write_bl_len_low : 2;
  // byte 14
  unsigned reserved5: 2;
  unsigned file_format : 2;
  unsigned tmp_write_protect : 1;
  unsigned perm_write_protect : 1;
  unsigned copy : 1;
  unsigned file_format_grp : 1;
  // byte 15
  unsigned always1 : 1;
  unsigned crc : 7;
}csd1_t;

typedef struct CSDV2 {
  unsigned reserved1 : 6;
  unsigned csd_ver : 2;
  uint8_t taac;
  uint8_t nsac;
  uint8_t tran_speed;
  uint8_t ccc_high;
  unsigned read_bl_len : 4;
  unsigned ccc_low : 4;
  unsigned reserved2 : 4;
  unsigned dsr_imp : 1;
  unsigned read_blk_misalign :1;
  unsigned write_blk_misalign : 1;
  unsigned read_bl_partial : 1;
  unsigned reserved3 : 2;
  unsigned c_size_high : 6;
  uint8_t c_size_mid;
  uint8_t c_size_low;
  unsigned sector_size_high : 6;
  unsigned erase_blk_en : 1;
  unsigned reserved4 : 1;
  unsigned wp_grp_size : 7;
  unsigned sector_size_low : 1;
  unsigned write_bl_len_high : 2;
  unsigned r2w_factor : 3;
  unsigned reserved5 : 2;
  unsigned wp_grp_enable : 1;
  unsigned reserved6 : 5;
  unsigned write_partial : 1;
  unsigned write_bl_len_low : 2;
  unsigned reserved7: 2;
  unsigned file_format : 2;
  unsigned tmp_write_protect : 1;
  unsigned perm_write_protect : 1;
  unsigned copy : 1;
  unsigned file_format_grp : 1;
  unsigned always1 : 1;
  unsigned crc : 7;
}csd2_t;

union csd_t {
  csd1_t v1;
  csd2_t v2;
};

uint8_t const SPI_FULL_SPEED = 0;
uint8_t const SPI_HALF_SPEED = 1;
uint8_t const SPI_QUARTER_SPEED = 2;

#define MEGA_SOFT_SPI 0
#if MEGA_SOFT_SPI && (defined(__AVR_ATmega1280__)||defined(__AVR_ATmega2560__))
#define SOFTWARE_SPI
#endif
#ifndef SOFTWARE_SPI

uint8_t const  SD_CHIP_SELECT_PIN = SS_PIN;
uint8_t const  SPI_MOSI_PIN = MOSI_PIN;
uint8_t const  SPI_MISO_PIN = MISO_PIN;
uint8_t const  SPI_SCK_PIN = SCK_PIN;
#define OPTIMIZE_HARDWARE_SPI

#else
uint8_t const SD_CHIP_SELECT_PIN = 10;
uint8_t const SPI_MOSI_PIN = 11;
uint8_t const SPI_MISO_PIN = 12;
uint8_t const SPI_SCK_PIN = 13;
#endif
#define SD_PROTECT_BLOCK_ZERO 1
uint16_t const SD_INIT_TIMEOUT = 2000;
uint16_t const SD_ERASE_TIMEOUT = 10000;
uint16_t const SD_READ_TIMEOUT = 300;
uint16_t const SD_WRITE_TIMEOUT = 600;
uint8_t const SD_CARD_ERROR_CMD0 = 0X1;
uint8_t const SD_CARD_ERROR_CMD8 = 0X2;
uint8_t const SD_CARD_ERROR_CMD17 = 0X3;
uint8_t const SD_CARD_ERROR_CMD24 = 0X4;
uint8_t const SD_CARD_ERROR_CMD25 = 0X05;
uint8_t const SD_CARD_ERROR_CMD58 = 0X06;
uint8_t const SD_CARD_ERROR_ACMD23 = 0X07;
uint8_t const SD_CARD_ERROR_ACMD41 = 0X08;
uint8_t const SD_CARD_ERROR_BAD_CSD = 0X09;
uint8_t const SD_CARD_ERROR_ERASE = 0X0A;
uint8_t const SD_CARD_ERROR_ERASE_SINGLE_BLOCK = 0X0B;
uint8_t const SD_CARD_ERROR_ERASE_TIMEOUT = 0X0C;
uint8_t const SD_CARD_ERROR_READ = 0X0D;
uint8_t const SD_CARD_ERROR_READ_REG = 0X0E;
uint8_t const SD_CARD_ERROR_READ_TIMEOUT = 0X0F;
uint8_t const SD_CARD_ERROR_STOP_TRAN = 0X10;
uint8_t const SD_CARD_ERROR_WRITE = 0X11;
uint8_t const SD_CARD_ERROR_WRITE_BLOCK_ZERO = 0X12;
uint8_t const SD_CARD_ERROR_WRITE_MULTIPLE = 0X13;
uint8_t const SD_CARD_ERROR_WRITE_PROGRAMMING = 0X14;
uint8_t const SD_CARD_ERROR_WRITE_TIMEOUT = 0X15;
uint8_t const SD_CARD_ERROR_SCK_RATE = 0X16;
uint8_t const SD_CARD_TYPE_SD1 = 1;
uint8_t const SD_CARD_TYPE_SD2 = 2;
uint8_t const SD_CARD_TYPE_SDHC = 3;

class Sd2Card {
public:
    Sd2Card(void) : errorCode_(0), inBlock_(0), partialBlockRead_(0), type_(0) {}
    uint32_t cardSize(void);
    uint8_t erase(uint32_t firstBlock, uint32_t lastBlock);
    uint8_t eraseSingleBlockEnable(void);
    uint8_t errorCode(void) const {return errorCode_;}
    uint8_t errorData(void) const {return status_;}
    uint8_t init(void) { return init(SPI_FULL_SPEED, SD_CHIP_SELECT_PIN); }
    uint8_t init(uint8_t sckRateID) {    return init(sckRateID, SD_CHIP_SELECT_PIN);  }
    uint8_t init(uint8_t sckRateID, uint8_t chipSelectPin);
    void partialBlockRead(uint8_t value);
    uint8_t partialBlockRead(void) const {return partialBlockRead_;}
    uint8_t readBlock(uint32_t block, uint8_t* dst);
    uint8_t readData(uint32_t block, uint16_t offset, uint16_t count, uint8_t* dst);
    uint8_t readCID(cid_t* cid) { return readRegister(CMD10, cid); }
    uint8_t readCSD(csd_t* csd) { return readRegister(CMD9, csd); }
    void readEnd(void);
    uint8_t setSckRate(uint8_t sckRateID);
    uint8_t type(void) const {return type_;}
    uint8_t writeBlock(uint32_t blockNumber, const uint8_t* src);
    uint8_t writeData(const uint8_t* src);
    uint8_t writeStart(uint32_t blockNumber, uint32_t eraseCount);
    uint8_t writeStop(void);
private:
    uint32_t block_;
    uint8_t chipSelectPin_;
    uint8_t errorCode_;
    uint8_t inBlock_;
    uint16_t offset_;
    uint8_t partialBlockRead_;
    uint8_t status_;
    uint8_t type_;

    uint8_t cardAcmd(uint8_t cmd, uint32_t arg)
    {
        cardCommand(CMD55, 0);return cardCommand(cmd, arg);
    }
   uint8_t cardCommand(uint8_t cmd, uint32_t arg);
    void error(uint8_t code) {errorCode_ = code;}
    uint8_t readRegister(uint8_t cmd, void* buf);
    uint8_t sendWriteCommand(uint32_t blockNumber, uint32_t eraseCount);
    void chipSelectHigh(void);
    void chipSelectLow(void);
    void type(uint8_t value) {type_ = value;}
    uint8_t waitNotBusy(uint16_t timeoutMillis);
    uint8_t writeData(uint8_t token, const uint8_t* src);
    uint8_t waitStartBlock(void);
};


uint8_t const BOOTSIG0 = 0X55;
uint8_t const BOOTSIG1 = 0XAA;

struct partitionTable {

  uint8_t  boot;
  uint8_t  beginHead;
  unsigned beginSector : 6;
  unsigned beginCylinderHigh : 2;
  uint8_t  beginCylinderLow;
  uint8_t  type;
  uint8_t  endHead;
  unsigned endSector : 6;
  unsigned endCylinderHigh : 2;
  uint8_t  endCylinderLow;
  uint32_t firstSector;
  uint32_t totalSectors;
};
typedef struct partitionTable part_t;

struct masterBootRecord {
  uint8_t  codeArea[440];
  uint32_t diskSignature;
  uint16_t usuallyZero;
  part_t   part[4];
  uint8_t  mbrSig0;
  uint8_t  mbrSig1;
};

typedef struct masterBootRecord mbr_t;
struct biosParmBlock {
  uint16_t bytesPerSector;
  uint8_t  sectorsPerCluster;
  uint16_t reservedSectorCount;
  uint8_t  fatCount;
  uint16_t rootDirEntryCount;
  uint16_t totalSectors16;
  uint8_t  mediaType;
  uint16_t sectorsPerFat16;
  uint16_t sectorsPerTrtack;
  uint16_t headCount;
  uint32_t hidddenSectors;
  uint32_t totalSectors32;
  uint32_t sectorsPerFat32;
  uint16_t fat32Flags;
  uint16_t fat32Version;
  uint32_t fat32RootCluster;
  uint16_t fat32FSInfo;
  uint16_t fat32BackBootBlock;
  uint8_t  fat32Reserved[12];
};
typedef struct biosParmBlock bpb_t;
struct fat32BootSector {
  uint8_t  jmpToBootCode[3];
  char     oemName[8];
  bpb_t    bpb;
  uint8_t  driveNumber;
  uint8_t  reserved1;
  uint8_t  bootSignature;
  uint32_t volumeSerialNumber;
  char     volumeLabel[11];
  char     fileSystemType[8];
  uint8_t  bootCode[420];
  uint8_t  bootSectorSig0;
  uint8_t  bootSectorSig1;
};
uint16_t const FAT16EOC = 0XFFFF;
uint16_t const FAT16EOC_MIN = 0XFFF8;
uint32_t const FAT32EOC = 0X0FFFFFFF;
uint32_t const FAT32EOC_MIN = 0X0FFFFFF8;
uint32_t const FAT32MASK = 0X0FFFFFFF;
typedef struct fat32BootSector fbs_t;

struct directoryEntry {

  uint8_t  name[11];

  uint8_t  attributes;

  uint8_t  reservedNT;

  uint8_t  creationTimeTenths;
  uint16_t creationTime;
  uint16_t creationDate;
  uint16_t lastAccessDate;
  uint16_t firstClusterHigh;
  uint16_t lastWriteTime;
  uint16_t lastWriteDate;
  uint16_t firstClusterLow;
  uint32_t fileSize;
};
typedef struct directoryEntry dir_t;
uint8_t const DIR_NAME_0XE5 = 0X05;
uint8_t const DIR_NAME_DELETED = 0XE5;
uint8_t const DIR_NAME_FREE = 0X00;
uint8_t const DIR_ATT_READ_ONLY = 0X01;
uint8_t const DIR_ATT_HIDDEN = 0X02;
uint8_t const DIR_ATT_SYSTEM = 0X04;
uint8_t const DIR_ATT_VOLUME_ID = 0X08;
uint8_t const DIR_ATT_DIRECTORY = 0X10;
uint8_t const DIR_ATT_ARCHIVE = 0X20;
uint8_t const DIR_ATT_LONG_NAME = 0X0F;
uint8_t const DIR_ATT_LONG_NAME_MASK = 0X3F;
uint8_t const DIR_ATT_DEFINED_BITS = 0X3F;
static inline uint8_t DIR_IS_LONG_NAME(const dir_t* dir) {
  return (dir->attributes & DIR_ATT_LONG_NAME_MASK) == DIR_ATT_LONG_NAME;
}
uint8_t const DIR_ATT_FILE_TYPE_MASK = (DIR_ATT_VOLUME_ID | DIR_ATT_DIRECTORY);
static inline uint8_t DIR_IS_FILE(const dir_t* dir) {
  return (dir->attributes & DIR_ATT_FILE_TYPE_MASK) == 0;
}
static inline uint8_t DIR_IS_SUBDIR(const dir_t* dir) {
  return (dir->attributes & DIR_ATT_FILE_TYPE_MASK) == DIR_ATT_DIRECTORY;
}

static inline uint8_t DIR_IS_FILE_OR_SUBDIR(const dir_t* dir) {
  return (dir->attributes & DIR_ATT_VOLUME_ID) == 0;
}


#define ALLOW_DEPRECATED_FUNCTIONS 1
class SdVolume;

uint8_t const LS_DATE = 1;
uint8_t const LS_SIZE = 2;
uint8_t const LS_R = 4;
uint8_t const O_READ = 0X01;
uint8_t const O_RDONLY = O_READ;
uint8_t const O_WRITE = 0X02;
uint8_t const O_WRONLY = O_WRITE;
uint8_t const O_RDWR = (O_READ | O_WRITE);
uint8_t const O_ACCMODE = (O_READ | O_WRITE);
uint8_t const O_APPEND = 0X04;
uint8_t const O_SYNC = 0X08;
uint8_t const O_CREAT = 0X10;
uint8_t const O_EXCL = 0X20;
uint8_t const O_TRUNC = 0X40;
uint8_t const T_ACCESS = 1;
uint8_t const T_CREATE = 2;
uint8_t const T_WRITE = 4;
uint8_t const FAT_FILE_TYPE_CLOSED = 0;
uint8_t const FAT_FILE_TYPE_NORMAL = 1;
uint8_t const FAT_FILE_TYPE_ROOT16 = 2;
uint8_t const FAT_FILE_TYPE_ROOT32 = 3;
uint8_t const FAT_FILE_TYPE_SUBDIR = 4;
uint8_t const FAT_FILE_TYPE_MIN_DIR = FAT_FILE_TYPE_ROOT16;

static inline uint16_t FAT_DATE(uint16_t year, uint8_t month, uint8_t day) {
  return (year - 1980) << 9 | month << 5 | day;
}
static inline uint16_t FAT_YEAR(uint16_t fatDate) {
  return 1980 + (fatDate >> 9);
}
static inline uint8_t FAT_MONTH(uint16_t fatDate) {
  return (fatDate >> 5) & 0XF;
}
static inline uint8_t FAT_DAY(uint16_t fatDate) {
  return fatDate & 0X1F;
}
static inline uint16_t FAT_TIME(uint8_t hour, uint8_t minute, uint8_t second) {
  return hour << 11 | minute << 5 | second >> 1;
}
static inline uint8_t FAT_HOUR(uint16_t fatTime) {
  return fatTime >> 11;
}
static inline uint8_t FAT_MINUTE(uint16_t fatTime) {
  return(fatTime >> 5) & 0X3F;
}
static inline uint8_t FAT_SECOND(uint16_t fatTime) {
  return 2*(fatTime & 0X1F);
}
uint16_t const FAT_DEFAULT_DATE = ((2000 - 1980) << 9) | (1 << 5) | 1;
uint16_t const FAT_DEFAULT_TIME = (1 << 11);
class SdFile : public Print {
public:
  SdFile(void) : type_(FAT_FILE_TYPE_CLOSED) {}
  void clearUnbufferedRead(void) {
    flags_ &= ~F_FILE_UNBUFFERED_READ;
  }
  uint8_t close(void);
  uint8_t contiguousRange(uint32_t* bgnBlock, uint32_t* endBlock);
  uint8_t createContiguous(SdFile* dirFile,
          const char* fileName, uint32_t size);
  uint32_t curCluster(void) const {return curCluster_;}
  uint32_t curPosition(void) const {return curPosition_;}

  static void dateTimeCallback(
    void (*dateTime)(uint16_t* date, uint16_t* time)) {
    dateTime_ = dateTime;
  }
  static void dateTimeCallbackCancel(void) {
    dateTime_ = 0;
  }
  uint32_t dirBlock(void) const {return dirBlock_;}
  uint8_t dirEntry(dir_t* dir);
  uint8_t dirIndex(void) const {return dirIndex_;}
  static void dirName(const dir_t& dir, char* name);
  uint32_t fileSize(void) const {return fileSize_;}
  uint32_t firstCluster(void) const {return firstCluster_;}
  uint8_t isDir(void) const {return type_ >= FAT_FILE_TYPE_MIN_DIR;}
  uint8_t isFile(void) const {return type_ == FAT_FILE_TYPE_NORMAL;}
  uint8_t isOpen(void) const {return type_ != FAT_FILE_TYPE_CLOSED;}
  uint8_t isSubDir(void) const {return type_ == FAT_FILE_TYPE_SUBDIR;}
  uint8_t isRoot(void) const {
    return type_ == FAT_FILE_TYPE_ROOT16 || type_ == FAT_FILE_TYPE_ROOT32;
  }
  void ls(uint8_t flags = 0, uint8_t indent = 0);
  uint8_t makeDir(SdFile* dir, const char* dirName);
  uint8_t open(SdFile* dirFile, uint16_t index, uint8_t oflag);
  uint8_t open(SdFile* dirFile, const char* fileName, uint8_t oflag);

  uint8_t openRoot(SdVolume* vol);
  static void printDirName(const dir_t& dir, uint8_t width);
  static void printFatDate(uint16_t fatDate);
  static void printFatTime(uint16_t fatTime);
  static void printTwoDigits(uint8_t v);
  int16_t read(void) {
    uint8_t b;
    return read(&b, 1) == 1 ? b : -1;
  }
  int16_t read(void* buf, uint16_t nbyte);
  int8_t readDir(dir_t* dir);
  static uint8_t remove(SdFile* dirFile, const char* fileName);
  uint8_t remove(void);
  void rewind(void) {
    curPosition_ = curCluster_ = 0;
  }
  uint8_t rmDir(void);
  uint8_t rmRfStar(void);
  uint8_t seekCur(uint32_t pos) {
    return seekSet(curPosition_ + pos);
  }
  uint8_t seekEnd(void) {return seekSet(fileSize_);}
  uint8_t seekSet(uint32_t pos);
  void setUnbufferedRead(void) {
    if (isFile()) flags_ |= F_FILE_UNBUFFERED_READ;
  }
  uint8_t timestamp(uint8_t flag, uint16_t year, uint8_t month, uint8_t day,
          uint8_t hour, uint8_t minute, uint8_t second);
  uint8_t sync(void);
  uint8_t type(void) const {return type_;}
  uint8_t truncate(uint32_t size);
  uint8_t unbufferedRead(void) const {
    return flags_ & F_FILE_UNBUFFERED_READ;
  }
  SdVolume* volume(void) const {return vol_;}
  size_t write(uint8_t b);
  size_t write(const void* buf, uint16_t nbyte);
  size_t write(const char* str);
  void write_P(PGM_P str);
  void writeln_P(PGM_P str);
#if ALLOW_DEPRECATED_FUNCTIONS
  uint8_t contiguousRange(uint32_t& bgnBlock, uint32_t& endBlock) {
    return contiguousRange(&bgnBlock, &endBlock);
  }
  uint8_t createContiguous(SdFile& dirFile,
    const char* fileName, uint32_t size) {
    return createContiguous(&dirFile, fileName, size);
  }

  static void dateTimeCallback(
    void (*dateTime)(uint16_t& date, uint16_t& time)) {
    oldDateTime_ = dateTime;
    dateTime_ = dateTime ? oldToNew : 0;
  }
  uint8_t dirEntry(dir_t& dir) {return dirEntry(&dir);}
  uint8_t makeDir(SdFile& dir, const char* dirName) {
    return makeDir(&dir, dirName);
  }

  uint8_t open(SdFile& dirFile,  const char* fileName, uint8_t oflag) {
    return open(&dirFile, fileName, oflag);
  }

  uint8_t open(SdFile& dirFile, const char* fileName) { return open(dirFile, fileName, O_RDWR); }

  uint8_t open(SdFile& dirFile, uint16_t index, uint8_t oflag) {
    return open(&dirFile, index, oflag);
  }

  uint8_t openRoot(SdVolume& vol) {return openRoot(&vol);}
  int8_t readDir(dir_t& dir) {return readDir(&dir);}
  static uint8_t remove(SdFile& dirFile, const char* fileName) {
    return remove(&dirFile, fileName);
  }
 private:
  static void (*oldDateTime_)(uint16_t& date, uint16_t& time);
  static void oldToNew(uint16_t* date, uint16_t* time) {
    uint16_t d;
    uint16_t t;
    oldDateTime_(d, t);
    *date = d;
    *time = t;
  }
#endif
private:
    static uint8_t const F_OFLAG = (O_ACCMODE | O_APPEND | O_SYNC);
    static uint8_t const F_UNUSED = 0X30;
    static uint8_t const F_FILE_UNBUFFERED_READ = 0X40;
    static uint8_t const F_FILE_DIR_DIRTY = 0X80;

#if ((F_UNUSED | F_FILE_UNBUFFERED_READ | F_FILE_DIR_DIRTY) & F_OFLAG)
#error flags_ bits conflict
#endif
  uint8_t   flags_;
  uint8_t   type_;
  uint32_t  curCluster_;
  uint32_t  curPosition_;
  uint32_t  dirBlock_;
  uint8_t   dirIndex_;
  uint32_t  fileSize_;
  uint32_t  firstCluster_;
  SdVolume* vol_;
  uint8_t addCluster(void);
  uint8_t addDirCluster(void);
  dir_t* cacheDirEntry(uint8_t action);
  static void (*dateTime_)(uint16_t* date, uint16_t* time);
  static uint8_t make83Name(const char* str, uint8_t* name);
  uint8_t openCachedEntry(uint8_t cacheIndex, uint8_t oflags);
  dir_t* readDirCache(void);
};

union cache_t {
  uint8_t  data[512];
  uint16_t fat16[256];
  uint32_t fat32[128];
  dir_t    dir[16];
  mbr_t    mbr;
  fbs_t    fbs;
};
class SdVolume {
 public:
  SdVolume(void) :allocSearchStart_(2), fatType_(0) {}
  static uint8_t* cacheClear(void) {
    cacheFlush();
    cacheBlockNumber_ = 0XFFFFFFFF;
    return cacheBuffer_.data;
  }
  
    uint8_t init(Sd2Card* dev) { return init(dev, 1) ? true : init(dev, 0);}
    uint8_t init(Sd2Card* dev, uint8_t part);
    uint8_t blocksPerCluster(void) const {return blocksPerCluster_;}
    uint32_t blocksPerFat(void)  const {return blocksPerFat_;}
    uint32_t clusterCount(void) const {return clusterCount_;}
    uint8_t clusterSizeShift(void) const {return clusterSizeShift_;}
    uint32_t dataStartBlock(void) const {return dataStartBlock_;}
    uint8_t fatCount(void) const {return fatCount_;}
    uint32_t fatStartBlock(void) const {return fatStartBlock_;}
    uint8_t fatType(void) const {return fatType_;}
    uint32_t rootDirEntryCount(void) const {return rootDirEntryCount_;}
    uint32_t rootDirStart(void) const {return rootDirStart_;}
    static Sd2Card* sdCard(void) {return sdCard_;}
#if ALLOW_DEPRECATED_FUNCTIONS
    uint8_t init(Sd2Card& dev) {return init(&dev);}
    uint8_t init(Sd2Card& dev, uint8_t part) {   return init(&dev, part); }
#endif
private:
    friend class SdFile;
    static uint8_t const CACHE_FOR_READ = 0;
    static uint8_t const CACHE_FOR_WRITE = 1;
    static cache_t cacheBuffer_;
    static uint32_t cacheBlockNumber_;
    static Sd2Card* sdCard_;
    static uint8_t cacheDirty_;
    static uint32_t cacheMirrorBlock_;
    uint32_t allocSearchStart_;
    uint8_t blocksPerCluster_;
    uint32_t blocksPerFat_;
    uint32_t clusterCount_;
    uint8_t clusterSizeShift_;
    uint32_t dataStartBlock_;
    uint8_t fatCount_;
    uint32_t fatStartBlock_;
    uint8_t fatType_;
    uint16_t rootDirEntryCount_;
    uint32_t rootDirStart_;
    uint8_t allocContiguous(uint32_t count, uint32_t* curCluster);

    uint8_t blockOfCluster(uint32_t position) const {
          return (position >> 9) & (blocksPerCluster_ - 1);}

    uint32_t clusterStartBlock(uint32_t cluster) const {
           return dataStartBlock_ + ((cluster - 2) << clusterSizeShift_);}

    uint32_t blockNumber(uint32_t cluster, uint32_t position) const {
           return clusterStartBlock(cluster) + blockOfCluster(position);}

    static uint8_t cacheFlush(void);
    static uint8_t cacheRawBlock(uint32_t blockNumber, uint8_t action);
    static void cacheSetDirty(void) {cacheDirty_ |= CACHE_FOR_WRITE;}
    static uint8_t cacheZeroBlock(uint32_t blockNumber);
    uint8_t chainSize(uint32_t beginCluster, uint32_t* size) const;
    uint8_t fatGet(uint32_t cluster, uint32_t* value) const;
    uint8_t fatPut(uint32_t cluster, uint32_t value);

    uint8_t fatPutEOC(uint32_t cluster) {
        return fatPut(cluster, 0x0FFFFFFF);
    }

    uint8_t freeChain(uint32_t cluster);

    uint8_t isEOC(uint32_t cluster) const {
        return  cluster >= (fatType_ == 16 ? FAT16EOC_MIN : FAT32EOC_MIN);
    }

    uint8_t readBlock(uint32_t block, uint8_t* dst) {
        return sdCard_->readBlock(block, dst);}

    uint8_t readData(uint32_t block, uint16_t offset,
    uint16_t count, uint8_t* dst) {
      return sdCard_->readData(block, offset, count, dst);
    }

    uint8_t writeBlock(uint32_t block, const uint8_t* dst) {
        return sdCard_->writeBlock(block, dst);
    }
};
#endif





#include <avr/pgmspace.h>

#define PgmPrint(x) SerialPrint_P(PSTR(x))
#define PgmPrintln(x) SerialPrintln_P(PSTR(x))
#define NOINLINE __attribute__((noinline,unused))
#define UNUSEDOK __attribute__((unused))

static UNUSEDOK int FreeRam(void) {
  extern int  __bss_end;
  extern int* __brkval;
  int free_memory;
  if (reinterpret_cast<int>(__brkval) == 0) {
    free_memory = reinterpret_cast<int>(&free_memory)
                  - reinterpret_cast<int>(&__bss_end);
  } else {
    free_memory = reinterpret_cast<int>(&free_memory)
                  - reinterpret_cast<int>(__brkval);
  }
  return free_memory;
}

static NOINLINE void SerialPrint_P(PGM_P str) {
  for (uint8_t c; (c = pgm_read_byte(str)); str++) Serial.write(c);
}

static NOINLINE void SerialPrintln_P(PGM_P str) {
  SerialPrint_P(str);
  Serial.println();
}

static const uint8_t FILE_READ = O_READ;
static const uint8_t FILE_WRITE = O_READ | O_WRITE | O_CREAT;

class File : public Stream
{
    char _name[13];
    SdFile *_file;
public:
    File(SdFile f, const char *name);
    File(void);
    ~File(void);
    virtual size_t write(uint8_t);
    virtual size_t write(const uint8_t *buf, size_t size);
    virtual int read();
    virtual int peek();
    virtual int available();
    virtual void flush();
    int read(void *buf, uint16_t nbyte);
    boolean seek(uint32_t pos);
    uint32_t position();
    uint32_t size();
    void close();
    operator bool();
    char * name();
    boolean isDirectory(void);
    File openNextFile(uint8_t mode = O_RDONLY);
    void rewindDirectory(void);
    using Print::write;
};

class SDClass {

private:
    Sd2Card card;
    SdVolume volume;
    SdFile root;
    SdFile getParentDir(const char *filepath, int *indx);
public:
    boolean begin(uint8_t csPin = SD_CHIP_SELECT_PIN);
    File open(const char *filename, uint8_t mode = FILE_READ);
    boolean exists(char *filepath);
    boolean mkdir(char *filepath);
    boolean remove(char *filepath);
    boolean rmdir(char *filepath);
private:
    int fileOpenMode;
    friend class File;
    friend boolean callback_openPath(SdFile&, char *, boolean, void *);
};

extern SDClass SD;



extern "C"{

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define EXTERNAL_INT_0 0
#define EXTERNAL_INT_1 1
#define EXTERNAL_INT_2 2
#define EXTERNAL_INT_3 3
#define EXTERNAL_INT_4 4
#define EXTERNAL_INT_5 5
#define EXTERNAL_INT_6 6
#define EXTERNAL_INT_7 7

#define EXTERNAL_NUM_INTERRUPTS 2

typedef void (*voidFuncPtr)(void);

}


#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 * 256))
#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)
#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
static const uint8_t FRACT_MAX = 1000 >> 3;
volatile unsigned long timer0_overflow_count = 0;
volatile unsigned long timer0_millis = 0;
static unsigned char timer0_fract = 0;

ISR(TIMER0_OVF_vect)
{
    unsigned long m = timer0_millis;
    unsigned char f = timer0_fract;
    m += MILLIS_INC;
    f += FRACT_INC;

    if (f >= FRACT_MAX)
    {
        f -= FRACT_MAX;
        m += 1;
    }

    timer0_fract = f;
    timer0_millis = m;
    timer0_overflow_count++;
}

unsigned long millis()
{
    unsigned long m;
    uint8_t oldSREG = SREG;
    cli();
    m = timer0_millis;
    SREG = oldSREG;
    return m;
}

unsigned long micros()
{
    unsigned long m;
    uint8_t oldSREG = SREG, t;
    cli();
    m = timer0_overflow_count;
    t = TCNT0;

    if ((TIFR0 & _BV(TOV0)) && (t < 255))
        m++;

    SREG = oldSREG;
    return ((m << 8) + t) * (64 / clockCyclesPerMicrosecond());
}

void delay(unsigned long ms)
{
    uint16_t start = (uint16_t)micros();

    while (ms > 0) {
        if (((uint16_t)micros() - start) >= 1000) {
            ms--;
            start += 1000;
        }
    }
}

void delayMicroseconds(unsigned int us)
{
#if F_CPU >= 20000000L
#elif F_CPU >= 16000000L
    if (--us == 0)
        return;

    us <<= 2;
    us -= 2;
#else
#endif

    __asm__ __volatile__ (
        "1: sbiw %0,1" "\n\t"
        "brne 1b" : "=w" (us) : "0" (us)
    );
}

void init()
{
    sei();
#if defined(TCCR0A) && defined(WGM01)
    sbi(TCCR0A, WGM01);
    sbi(TCCR0A, WGM00);
#endif

#if defined(__AVR_ATmega128__)
    sbi(TCCR0, CS02);
#elif defined(TCCR0) && defined(CS01) && defined(CS00)
    sbi(TCCR0, CS01);
    sbi(TCCR0, CS00);
#elif defined(TCCR0B) && defined(CS01) && defined(CS00)
    sbi(TCCR0B, CS01);
    sbi(TCCR0B, CS00);
#elif defined(TCCR0A) && defined(CS01) && defined(CS00)
    sbi(TCCR0A, CS01);
    sbi(TCCR0A, CS00);
#else
    #error Timer 0 prescale factor 64 not set correctly
#endif

#if defined(TIMSK) && defined(TOIE0)
    sbi(TIMSK, TOIE0);
#elif defined(TIMSK0) && defined(TOIE0)
    sbi(TIMSK0, TOIE0);
#else
    #error  Timer 0 overflow interrupt not set correctly
#endif

#if defined(TCCR1B) && defined(CS11) && defined(CS10)
    TCCR1B = 0;
    sbi(TCCR1B, CS11);
#if F_CPU >= 8000000L
    sbi(TCCR1B, CS10);
#endif
#elif defined(TCCR1) && defined(CS11) && defined(CS10)
    sbi(TCCR1, CS11);
#if F_CPU >= 8000000L
    sbi(TCCR1, CS10);
#endif
#endif

#if defined(TCCR1A) && defined(WGM10)
    sbi(TCCR1A, WGM10);
#elif defined(TCCR1)
    #warning this needs to be finished
#endif

#if defined(TCCR2) && defined(CS22)
    sbi(TCCR2, CS22);
#elif defined(TCCR2B) && defined(CS22)
    sbi(TCCR2B, CS22);
#else
    #warning Timer 2 not finished (may not be present on this CPU)
#endif

#if defined(TCCR2) && defined(WGM20)
    sbi(TCCR2, WGM20);
#elif defined(TCCR2A) && defined(WGM20)
    sbi(TCCR2A, WGM20);
#else
    #warning Timer 2 not finished (may not be present on this CPU)
#endif

#if defined(TCCR3B) && defined(CS31) && defined(WGM30)
    sbi(TCCR3B, CS31);
    sbi(TCCR3B, CS30);
    sbi(TCCR3A, WGM30);
#endif

#if defined(ADCSRA)
    sbi(ADCSRA, ADPS2);
    sbi(ADCSRA, ADPS1);
    sbi(ADCSRA, ADPS0);
    sbi(ADCSRA, ADEN);
#endif

#if defined(UCSRB)
    UCSRB = 0;
#elif defined(UCSR0B)
    UCSR0B = 0;
#endif
}



Sd2Card card;
SdVolume volume;
SdFile root;
const int chipSelect = 4;


int main(void)
{
    init();
    Serial.begin(9600);
    while (!Serial) { }
    Serial.print("\nInitializing SD card...");
    pinMode(10, OUTPUT);

    if (!card.init(SPI_HALF_SPEED, chipSelect)) {
        Serial.println("initialization failed. Things to check:");
        Serial.println("* is a card is inserted?");
        Serial.println("* Is your wiring correct?");
        Serial.println("* did you change the chipSelect pin to match your shield or module?");
        return 0;
    } else {
        Serial.println("Wiring is correct and a card is present.");
    }

    Serial.print("\nCard type: ");
    switch(card.type()) {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
    }

    if (!volume.init(card)) {
Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
        return 0;
    }

    uint32_t volumesize;
    Serial.print("\nVolume type is FAT");
    Serial.println(volume.fatType(), DEC);
    Serial.println();
    volumesize = volume.blocksPerCluster();
    volumesize *= volume.clusterCount();
    volumesize *= 512;
    Serial.print("Volume size (bytes): ");
    Serial.println(volumesize);
    Serial.print("Volume size (Kbytes): ");
    volumesize /= 1024;
    Serial.println(volumesize);
    Serial.print("Volume size (Mbytes): ");
    volumesize /= 1024;
    Serial.println(volumesize);
    Serial.println("\nFiles found on the card (name, date and size in bytes): ");
    root.openRoot(volume);
    root.ls(LS_R | LS_DATE | LS_SIZE);
    while (true) { }
	return 0;
}

#define __AVR_LIBC_DEPRECATED_ENABLE__



static const uint8_t SERIAL_BUFFER_SIZE = 64;

struct ring_buffer
{
    unsigned char buffer[SERIAL_BUFFER_SIZE];
    volatile unsigned int head;
    volatile unsigned int tail;
};

ring_buffer rx_buffer  =  { { 0 }, 0, 0 };
ring_buffer tx_buffer  =  { { 0 }, 0, 0 };

inline void store_char(unsigned char c, ring_buffer *buffer)
{
    unsigned i = (unsigned int)(buffer->head + 1) % SERIAL_BUFFER_SIZE;

    if (i != buffer->tail)
    {
        buffer->buffer[buffer->head] = c;
        buffer->head = i;
    }
}

void serialEvent() __attribute__((weak));
void serialEvent() {}
#define serialEvent_implemented
ISR(USART_RX_vect)
{
    if (bit_is_clear(UCSR0A, UPE0))
    {
        unsigned char c = UDR0;
        store_char(c, &rx_buffer);
    }
}

void serialEventRun(void)
{
    if (Serial.available())
        serialEvent();
}

ISR(USART_UDRE_vect)
{
    if (tx_buffer.head == tx_buffer.tail)
    {
        cbi(UCSR0B, UDRIE0);
    }
    else
    {
        unsigned char c = tx_buffer.buffer[tx_buffer.tail];
        tx_buffer.tail = (tx_buffer.tail + 1) % SERIAL_BUFFER_SIZE;
        UDR0 = c;
    }
}

HardwareSerial::HardwareSerial(ring_buffer *rx_buffer, ring_buffer *tx_buffer,
    volatile uint8_t *ubrrh, volatile uint8_t *ubrrl,
    volatile uint8_t *ucsra, volatile uint8_t *ucsrb,
    volatile uint8_t *ucsrc, volatile uint8_t *udr,
    uint8_t rxen, uint8_t txen, uint8_t rxcie, uint8_t udrie, uint8_t u2x)
{
    _rx_buffer = rx_buffer;
    _tx_buffer = tx_buffer;
    _ubrrh = ubrrh;
    _ubrrl = ubrrl;
    _ucsra = ucsra;
    _ucsrb = ucsrb;
    _ucsrc = ucsrc;
    _udr = udr;
    _rxen = rxen;
    _txen = txen;
    _rxcie = rxcie;
    _udrie = udrie;
    _u2x = u2x;
}

void HardwareSerial::begin(unsigned long baud)
{
  uint16_t baud_setting;
  bool use_u2x = true;

  if (baud == 57600) {
    use_u2x = false;
  }

try_again:

  if (use_u2x) {
    *_ucsra = 1 << _u2x;
    baud_setting = (F_CPU / 4 / baud - 1) / 2;
  } else {
    *_ucsra = 0;
    baud_setting = (F_CPU / 8 / baud - 1) / 2;
  }

  if ((baud_setting > 4095) && use_u2x)
  {
    use_u2x = false;
    goto try_again;
  }

  *_ubrrh = baud_setting >> 8;
  *_ubrrl = baud_setting;
  transmitting = false;
  sbi(*_ucsrb, _rxen);
  sbi(*_ucsrb, _txen);
  sbi(*_ucsrb, _rxcie);
  cbi(*_ucsrb, _udrie);
}

void HardwareSerial::begin(unsigned long baud, byte config)
{
    uint16_t baud_setting;
    bool use_u2x = true;

    if (baud == 57600)
        use_u2x = false;

try_again:

    if (use_u2x) {
        *_ucsra = 1 << _u2x;
        baud_setting = (F_CPU / 4 / baud - 1) / 2;
    } else {
        *_ucsra = 0;
        baud_setting = (F_CPU / 8 / baud - 1) / 2;
    }

    if ((baud_setting > 4095) && use_u2x)
    {
        use_u2x = false;
        goto try_again;
    }

    *_ubrrh = baud_setting >> 8;
    *_ubrrl = baud_setting;
    *_ucsrc = config;
    sbi(*_ucsrb, _rxen);
    sbi(*_ucsrb, _txen);
    sbi(*_ucsrb, _rxcie);
    cbi(*_ucsrb, _udrie);
}

void HardwareSerial::end()
{
    while (_tx_buffer->head != _tx_buffer->tail) { }
    cbi(*_ucsrb, _rxen);
    cbi(*_ucsrb, _txen);
    cbi(*_ucsrb, _rxcie);
    cbi(*_ucsrb, _udrie);
    _rx_buffer->head = _rx_buffer->tail;
}

int HardwareSerial::available(void)
{
  return (unsigned int)(SERIAL_BUFFER_SIZE + _rx_buffer->head - _rx_buffer->tail) % SERIAL_BUFFER_SIZE;
}

int HardwareSerial::peek(void)
{
  if (_rx_buffer->head == _rx_buffer->tail) {
    return -1;
  } else {
    return _rx_buffer->buffer[_rx_buffer->tail];
  }
}

int HardwareSerial::read(void)
{
  if (_rx_buffer->head == _rx_buffer->tail) {
    return -1;
  } else {
    unsigned char c = _rx_buffer->buffer[_rx_buffer->tail];
    _rx_buffer->tail = (unsigned int)(_rx_buffer->tail + 1) % SERIAL_BUFFER_SIZE;
    return c;
  }
}

void HardwareSerial::flush()
{
  while (transmitting && ! (*_ucsra & _BV(TXC0)));
  transmitting = false;
}

size_t HardwareSerial::write(uint8_t c)
{
  unsigned i = (_tx_buffer->head + 1) % SERIAL_BUFFER_SIZE;

  while (i == _tx_buffer->tail)
    ;

  _tx_buffer->buffer[_tx_buffer->head] = c;
  _tx_buffer->head = i;

  sbi(*_ucsrb, _udrie);
  transmitting = true;
  sbi(*_ucsra, TXC0);

  return 1;
}

HardwareSerial::operator bool() {
    return true;
}

HardwareSerial Serial(&rx_buffer, &tx_buffer, &UBRR0H, &UBRR0L, &UCSR0A,
            &UCSR0B, &UCSR0C, &UDR0, RXEN0, TXEN0, RXCIE0, UDRIE0, U2X0);

static const uint8_t NO_SKIP_CHAR = 1;

int Stream::timedRead()
{
  int c;
  _startMillis = millis();
  do {
    c = read();
    if (c >= 0) return c;
  } while(millis() - _startMillis < _timeout);
  return -1;
}

int Stream::timedPeek()
{
  int c;
  _startMillis = millis();
  do {
    c = peek();
    if (c >= 0) return c;
  } while(millis() - _startMillis < _timeout);
  return -1;
}

int Stream::peekNextDigit()
{
  int c;
  while (1) {
    c = timedPeek();
    if (c < 0) return c;
    if (c == '-') return c;
    if (c >= '0' && c <= '9') return c;
    read();
  }
}

void Stream::setTimeout(unsigned long timeout)
{
  _timeout = timeout;
}

bool  Stream::find(char *target)
{
  return findUntil(target, NULL);
}

bool Stream::find(char *target, size_t length)
{
  return findUntil(target, length, NULL, 0);
}

bool  Stream::findUntil(char *target, char *terminator)
{
  return findUntil(target, strlen(target), terminator, strlen(terminator));
}

bool Stream::findUntil(char *target, size_t targetLen, char *terminator, size_t termLen)
{
  size_t index = 0;
  size_t termIndex = 0;
  int c;

  if( *target == 0)
        return true;
  while( (c = timedRead()) > 0){

    if(c != target[index])
      index = 0;

    if( c == target[index]){
      if(++index >= targetLen){
        return true;
      }
    }

    if(termLen > 0 && c == terminator[termIndex]){
      if(++termIndex >= termLen)
        return false;
    }
    else
      termIndex = 0;
  }
  return false;
}

long Stream::parseInt()
{
  return parseInt(NO_SKIP_CHAR);
}

long Stream::parseInt(char skipChar)
{
  boolean isNegative = false;
  long value = 0;
  int c;

  c = peekNextDigit();
  if(c < 0)
    return 0;

  do{
    if(c == skipChar)
      ;
    else if(c == '-')
      isNegative = true;
    else if(c >= '0' && c <= '9')
      value = value * 10 + c - '0';
    read();
    c = timedPeek();
  }
  while( (c >= '0' && c <= '9') || c == skipChar );

  if(isNegative)
    value = -value;
  return value;
}

float Stream::parseFloat()
{
  return parseFloat(NO_SKIP_CHAR);
}

float Stream::parseFloat(char skipChar){
  boolean isNegative = false;
  boolean isFraction = false;
  long value = 0;
  char c;
  float fraction = 1.0;

  c = peekNextDigit();
  if(c < 0)
    return 0;

  do{
    if(c == skipChar)
      ;
    else if(c == '-')
      isNegative = true;
    else if (c == '.')
      isFraction = true;
    else if(c >= '0' && c <= '9')  {
      value = value * 10 + c - '0';
      if(isFraction)
         fraction *= 0.1;
    }
    read();
    c = timedPeek();
  }
  while( (c >= '0' && c <= '9')  || c == '.' || c == skipChar );

  if(isNegative)
    value = -value;
  if(isFraction)
    return value * fraction;
  else
    return value;
}

size_t Stream::readBytes(char *buffer, size_t length)
{
  size_t count = 0;
  while (count < length) {
    int c = timedRead();
    if (c < 0) break;
    *buffer++ = (char)c;
    count++;
  }
  return count;
}

size_t Stream::readBytesUntil(char terminator, char *buffer, size_t length)
{
  if (length < 1) return 0;
  size_t index = 0;
  while (index < length) {
    int c = timedRead();
    if (c < 0 || c == terminator) break;
    *buffer++ = (char)c;
    index++;
  }
  return index;
}

String Stream::readString()
{
  String ret;
  int c = timedRead();
  while (c >= 0)
  {
    ret += (char)c;
    c = timedRead();
  }
  return ret;
}

String Stream::readStringUntil(char terminator)
{
  String ret;
  int c = timedRead();
  while (c >= 0 && c != terminator)
  {
    ret += (char)c;
    c = timedRead();
  }
  return ret;
}

void * operator new(size_t size)
{
  return malloc(size);
}

void * operator new[](size_t size)
{
  return malloc(size);
}

void operator delete(void * ptr)
{
  free(ptr);
}

void operator delete[](void * ptr)
{
  free(ptr);
}

int __cxa_guard_acquire(__guard *g) {return !*(char *)(g);};
void __cxa_guard_release (__guard *g) {*(char *)g = 1;};
void __cxa_guard_abort (__guard *) {};

void __cxa_pure_virtual(void) {};


size_t Print::write(const uint8_t *buffer, size_t size)
{
  size_t n = 0;
  while (size--) {
    n += write(*buffer++);
  }
  return n;
}

size_t Print::print(const __FlashStringHelper *ifsh)
{
  const char * __attribute__((progmem)) p = (const char * ) ifsh;
  size_t n = 0;
  while (1) {
    unsigned char c = pgm_read_byte(p++);
    if (c == 0) break;
    n += write(c);
  }
  return n;
}

size_t Print::print(const String &s)
{
  size_t n = 0;
  for (uint16_t i = 0; i < s.length(); i++) {
    n += write(s[i]);
  }
  return n;
}

size_t Print::print(const char str[])
{
  return write(str);
}

size_t Print::print(char c)
{
  return write(c);
}

size_t Print::print(unsigned char b, int base)
{
  return print((unsigned long) b, base);
}

size_t Print::print(int n, int base)
{
  return print((long) n, base);
}

size_t Print::print(unsigned int n, int base)
{
  return print((unsigned long) n, base);
}

size_t Print::print(long n, int base)
{
  if (base == 0) {
    return write(n);
  } else if (base == 10) {
    if (n < 0) {
      int t = print('-');
      n = -n;
      return printNumber(n, 10) + t;
    }
    return printNumber(n, 10);
  } else {
    return printNumber(n, base);
  }
}

size_t Print::print(unsigned long n, int base)
{
  if (base == 0) return write(n);
  else return printNumber(n, base);
}

size_t Print::print(double n, int digits)
{
  return printFloat(n, digits);
}

size_t Print::println(const __FlashStringHelper *ifsh)
{
  size_t n = print(ifsh);
  n += println();
  return n;
}


size_t Print::print(const Printable& x)
{
  return x.printTo(*this);
}

size_t Print::println(void)
{
  size_t n = print('\r');
  n += print('\n');
  return n;
}

size_t Print::println(const String &s)
{
  size_t n = print(s);
  n += println();
  return n;
}

size_t Print::println(const char c[])
{
  size_t n = print(c);
  n += println();
  return n;
}

size_t Print::println(char c)
{
  size_t n = print(c);
  n += println();
  return n;
}

size_t Print::println(unsigned char b, int base)
{
  size_t n = print(b, base);
  n += println();
  return n;
}

size_t Print::println(int num, int base)
{
  size_t n = print(num, base);
  n += println();
  return n;
}

size_t Print::println(unsigned int num, int base)
{
  size_t n = print(num, base);
  n += println();
  return n;
}

size_t Print::println(long num, int base)
{
  size_t n = print(num, base);
  n += println();
  return n;
}

size_t Print::println(unsigned long num, int base)
{
  size_t n = print(num, base);
  n += println();
  return n;
}

size_t Print::println(double num, int digits)
{
  size_t n = print(num, digits);
  n += println();
  return n;
}

size_t Print::println(const Printable& x)
{
  size_t n = print(x);
  n += println();
  return n;
}

size_t Print::printNumber(unsigned long n, uint8_t base) {
  char buf[8 * sizeof(long) + 1];
  char *str = &buf[sizeof(buf) - 1];
  *str = '\0';

  if (base < 2) base = 10;

  do {
    unsigned long m = n;
    n /= base;
    char c = m - base * n;
    *--str = c < 10 ? c + '0' : c + 'A' - 10;
  } while(n);

  return write(str);
}

size_t Print::printFloat(double number, uint8_t digits)
{
  size_t n = 0;

  if (isnan(number)) return print("nan");
  if (isinf(number)) return print("inf");
  if (number > 4294967040.0) return print ("ovf");
  if (number <-4294967040.0) return print ("ovf");

  if (number < 0.0)
  {
     n += print('-');
     number = -number;
  }

  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.0;

  number += rounding;

  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  n += print(int_part);

  if (digits > 0) {
    n += print(".");
  }

  while (digits-- > 0)
  {
    remainder *= 10.0;
    int toPrint = int(remainder);
    n += print(toPrint);
    remainder -= toPrint;
  }

  return n;
}

static const uint8_t PA = 1;
static const uint8_t PB = 2;
static const uint8_t PC = 3;
static const uint8_t PD = 4;
static const uint8_t PE = 5;
static const uint8_t PF = 6;
static const uint8_t PG = 7;
static const uint8_t PH = 8;
static const uint8_t PJ = 10;
static const uint8_t PK = 11;
static const uint8_t PL = 12;

const uint16_t PROGMEM port_to_mode_PGM[] = {
    NOT_A_PORT,
    NOT_A_PORT,
    (uint16_t) &DDRB,
    (uint16_t) &DDRC,
    (uint16_t) &DDRD,
};

const uint16_t PROGMEM port_to_output_PGM[] = {
    NOT_A_PORT,
    NOT_A_PORT,
    (uint16_t) &PORTB,
    (uint16_t) &PORTC,
    (uint16_t) &PORTD,
};

const uint16_t PROGMEM port_to_input_PGM[] = {
    NOT_A_PORT,
    NOT_A_PORT,
    (uint16_t) &PINB,
    (uint16_t) &PINC,
    (uint16_t) &PIND,
};

const uint8_t PROGMEM digital_pin_to_port_PGM[] = {
    PD,
    PD,
    PD,
    PD,
    PD,
    PD,
    PD,
    PD,
    PB,
    PB,
    PB,
    PB,
    PB,
    PB,
    PC,
    PC,
    PC,
    PC,
    PC,
    PC,
};

const uint8_t PROGMEM digital_pin_to_bit_mask_PGM[] = {
    _BV(0),
    _BV(1),
    _BV(2),
    _BV(3),
    _BV(4),
    _BV(5),
    _BV(6),
    _BV(7),
    _BV(0),
    _BV(1),
    _BV(2),
    _BV(3),
    _BV(4),
    _BV(5),
    _BV(0),
    _BV(1),
    _BV(2),
    _BV(3),
    _BV(4),
    _BV(5),
};

const uint8_t PROGMEM digital_pin_to_timer_PGM[] = {
    NOT_ON_TIMER,
    NOT_ON_TIMER,
    NOT_ON_TIMER,
    TIMER2B,
    NOT_ON_TIMER,
    TIMER0B,
    TIMER0A,
    NOT_ON_TIMER,
    NOT_ON_TIMER,
    TIMER1A,
    TIMER1B,
    TIMER2A,
    NOT_ON_TIMER,
    NOT_ON_TIMER,
    NOT_ON_TIMER,
    NOT_ON_TIMER,
    NOT_ON_TIMER,
    NOT_ON_TIMER,
    NOT_ON_TIMER,
    NOT_ON_TIMER,
};

void pinMode(uint8_t pin, uint8_t mode)
{
    uint8_t bit = digitalPinToBitMask(pin);
    uint8_t port = digitalPinToPort(pin);
    volatile uint8_t *reg, *out;

    if (port == NOT_A_PIN) return;

    reg = portModeRegister(port);
    out = portOutputRegister(port);

    if (mode == INPUT) {
        uint8_t oldSREG = SREG;
                cli();
        *reg &= ~bit;
        *out &= ~bit;
        SREG = oldSREG;
    } else if (mode == INPUT_PULLUP) {
        uint8_t oldSREG = SREG;
                cli();
        *reg &= ~bit;
        *out |= bit;
        SREG = oldSREG;
    } else {
        uint8_t oldSREG = SREG;
                cli();
        *reg |= bit;
        SREG = oldSREG;
    }
}

static void turnOffPWM(uint8_t timer)
{
    switch (timer)
    {
#if defined(TCCR1A) && defined(COM1A1)
        case TIMER1A:   cbi(TCCR1A, COM1A1);    break;
#endif
#if defined(TCCR1A) && defined(COM1B1)
        case TIMER1B:   cbi(TCCR1A, COM1B1);    break;
#endif

#if defined(TCCR2) && defined(COM21)
        case  TIMER2:   cbi(TCCR2, COM21);      break;
#endif

#if defined(TCCR0A) && defined(COM0A1)
        case  TIMER0A:  cbi(TCCR0A, COM0A1);    break;
#endif

#if defined(TIMER0B) && defined(COM0B1)
        case  TIMER0B:  cbi(TCCR0A, COM0B1);    break;
#endif
#if defined(TCCR2A) && defined(COM2A1)
        case  TIMER2A:  cbi(TCCR2A, COM2A1);    break;
#endif
#if defined(TCCR2A) && defined(COM2B1)
        case  TIMER2B:  cbi(TCCR2A, COM2B1);    break;
#endif
    }
}

void digitalWrite(uint8_t pin, uint8_t val)
{
    uint8_t timer = digitalPinToTimer(pin);
    uint8_t bit = digitalPinToBitMask(pin);
    uint8_t port = digitalPinToPort(pin);
    volatile uint8_t *out;

    if (port == NOT_A_PIN) return;

    if (timer != NOT_ON_TIMER) turnOffPWM(timer);

    out = portOutputRegister(port);

    uint8_t oldSREG = SREG;
    cli();

    if (val == LOW) {
        *out &= ~bit;
    } else {
        *out |= bit;
    }

    SREG = oldSREG;
}

int digitalRead(uint8_t pin)
{
    uint8_t timer = digitalPinToTimer(pin);
    uint8_t bit = digitalPinToBitMask(pin);
    uint8_t port = digitalPinToPort(pin);

    if (port == NOT_A_PIN) return LOW;

    if (timer != NOT_ON_TIMER) turnOffPWM(timer);

    if (*portInputRegister(port) & bit) return HIGH;
    return LOW;
}

File::File(SdFile f, const char *n)
{
    _file = (SdFile *)malloc(sizeof(SdFile));

    if (_file)
    {
        memcpy(_file, &f, sizeof(SdFile));
        strncpy(_name, n, 12);
        _name[12] = 0;
    }
}

File::File(void) {
  _file = 0;
  _name[0] = 0;
}

File::~File(void) {
}

char *File::name(void) {
  return _name;
}

boolean File::isDirectory(void) {
  return (_file && _file->isDir());
}

size_t File::write(uint8_t val) {
  return write(&val, 1);
}

size_t File::write(const uint8_t *buf, size_t size) {
  size_t t;
  if (!_file) {
    setWriteError();
    return 0;
  }
  _file->clearWriteError();
  t = _file->write(buf, size);
  if (_file->getWriteError()) {
    setWriteError();
    return 0;
  }
  return t;
}

int File::peek() {
  if (! _file)
    return 0;

  int c = _file->read();
  if (c != -1) _file->seekCur(-1);
  return c;
}

int File::read() {
  if (_file)
    return _file->read();
  return -1;
}

int File::read(void *buf, uint16_t nbyte) {
  if (_file)
    return _file->read(buf, nbyte);
  return 0;
}

int File::available() {
  if (! _file) return 0;

  uint32_t n = size() - position();

  return n > 0X7FFF ? 0X7FFF : n;
}

void File::flush() {
  if (_file)
    _file->sync();
}

boolean File::seek(uint32_t pos) {
  if (! _file) return false;

  return _file->seekSet(pos);
}

uint32_t File::position() {
  if (! _file) return -1;
  return _file->curPosition();
}

uint32_t File::size() {
  if (! _file) return 0;
  return _file->fileSize();
}

void File::close() {
  if (_file) {
    _file->close();
    free(_file);
    _file = 0;
  }
}

File::operator bool() {
  if (_file)
    return  _file->isOpen();
  return false;
}

static const uint8_t MAX_COMPONENT_LEN = 12;
static const uint8_t PATH_COMPONENT_BUFFER_LEN = MAX_COMPONENT_LEN + 1;

bool getNextPathComponent(char *path, unsigned int *p_offset, char *buffer)
{
    int bufferOffset = 0;
    int offset = *p_offset;

    if (path[offset] == '/')
        offset++;

    while (bufferOffset < MAX_COMPONENT_LEN && (path[offset] != '/')  && (path[offset] != '\0'))
        buffer[bufferOffset++] = path[offset++];

    buffer[bufferOffset] = '\0';

    if (path[offset] == '/')
        offset++;

    *p_offset = offset;
    return (path[offset] != '\0');
}

boolean walkPath(char *filepath, SdFile& parentDir, boolean (*callback)(SdFile& parentDir,
        char *filePathComponent, boolean isLastComponent, void *object), void *object = NULL)
{
    SdFile subfile1;
    SdFile subfile2;
    char buffer[PATH_COMPONENT_BUFFER_LEN];
    unsigned int offset = 0;
    SdFile *p_parent;
    SdFile *p_child;
    SdFile *p_tmp_sdfile;
    p_child = &subfile1;
    p_parent = &parentDir;

    while (true)
    {

        boolean moreComponents = getNextPathComponent(filepath, &offset, buffer);
        boolean shouldContinue = callback((*p_parent), buffer, !moreComponents, object);

    if (!shouldContinue) {
      if (p_parent != &parentDir) {
        (*p_parent).close();
      }
      return false;
    }

    if (!moreComponents) {
      break;
    }

    boolean exists = (*p_child).open(*p_parent, buffer, O_RDONLY);

    if (p_parent != &parentDir) {
      (*p_parent).close();
    }
    if (exists) {
      if (p_parent == &parentDir) {
        p_parent = &subfile2;
      }

      p_tmp_sdfile = p_parent;
      p_parent = p_child;
      p_child = p_tmp_sdfile;
    } else {
      return false;
    }
  }

    if (p_parent != &parentDir)
        (*p_parent).close();

    return true;
}

boolean callback_pathExists(SdFile& parentDir, char *filePathComponent,
                boolean isLastComponent, void *object)
{
    SdFile child;
    boolean exists = child.open(parentDir, filePathComponent, O_RDONLY);

    if (exists)
        child.close();

    return exists;
}

boolean callback_makeDirPath(SdFile& parentDir, char *filePathComponent,
                 boolean isLastComponent, void *object)
{
    boolean result = false;
    SdFile child;
    result = callback_pathExists(parentDir, filePathComponent, isLastComponent, object);

    if (!result)
        result = child.makeDir(parentDir, filePathComponent);

    return result;
}

boolean callback_remove(SdFile& parentDir, char *filePathComponent,
            boolean isLastComponent, void *object) {
  if (isLastComponent) {
    return SdFile::remove(parentDir, filePathComponent);
  }
  return true;
}

boolean callback_rmdir(SdFile& parentDir, char *filePathComponent,
            boolean isLastComponent, void *object) {
  if (isLastComponent) {
    SdFile f;
    if (!f.open(parentDir, filePathComponent, O_READ)) return false;
    return f.rmDir();
  }
  return true;
}

boolean SDClass::begin(uint8_t csPin)
{
    return card.init(SPI_HALF_SPEED, csPin) &&
         volume.init(card) && root.openRoot(volume);
}

SdFile SDClass::getParentDir(const char *filepath, int *index)
{
    SdFile d1 = root;
    SdFile d2;
    SdFile *parent = &d1;
    SdFile *subdir = &d2;
    const char *origpath = filepath;

  while (strchr(filepath, '/')) {

    if (filepath[0] == '/') {
      filepath++;
      continue;
    }

    if (! strchr(filepath, '/'))
      break;

    uint8_t idx = strchr(filepath, '/') - filepath;

    if (idx > 12)
      idx = 12;

    char subdirname[13];
    strncpy(subdirname, filepath, idx);
    subdirname[idx] = 0;

    subdir->close();
    if (! subdir->open(parent, subdirname, O_READ)) {
      return SdFile();
    }
    filepath += idx;
    parent->close();
    SdFile *t = parent;
    parent = subdir;
    subdir = t;
  }

  *index = (int)(filepath - origpath);
  return *parent;
}

File SDClass::open(const char *filepath, uint8_t mode)
{
    int pathidx;
    SdFile parentdir = getParentDir(filepath, &pathidx);
    filepath += pathidx;

    if (! filepath[0])
        return File(parentdir, "/");

    SdFile file;

    if (!parentdir.isOpen())
        return File();

  if (parentdir.isRoot()) {
    if ( ! file.open(SD.root, filepath, mode)) {
      return File();
    }
  } else {
    if ( ! file.open(parentdir, filepath, mode)) {
      return File();
    }
    parentdir.close();
  }

  if (mode & (O_APPEND | O_WRITE))
    file.seekSet(file.fileSize());
  return File(file, filepath);
}

boolean SDClass::exists(char *filepath)
{
    return walkPath(filepath, root, callback_pathExists);
}

boolean SDClass::mkdir(char *filepath)
{
    return walkPath(filepath, root, callback_makeDirPath);
}

boolean SDClass::rmdir(char *filepath) {
  return walkPath(filepath, root, callback_rmdir);
}

boolean SDClass::remove(char *filepath) {
  return walkPath(filepath, root, callback_remove);
}

File File::openNextFile(uint8_t mode) {
  dir_t p;

  while (_file->readDir(&p) > 0) {

    if (p.name[0] == DIR_NAME_FREE) {
      return File();
    }

    if (p.name[0] == DIR_NAME_DELETED || p.name[0] == '.') {
      continue;
    }

    if (!DIR_IS_FILE_OR_SUBDIR(&p))
        continue;

    SdFile f;
    char name[13];
    _file->dirName(p, name);

    if (f.open(_file, name, mode)) {
      return File(f, name);
    } else {
      return File();
    }
  }

  return File();
}

void File::rewindDirectory(void) {
  if (isDirectory())
    _file->rewind();
}

SDClass SD;

uint32_t SdVolume::cacheBlockNumber_ = 0XFFFFFFFF;
cache_t SdVolume::cacheBuffer_;
Sd2Card* SdVolume::sdCard_;
uint8_t SdVolume::cacheDirty_ = 0;
uint32_t SdVolume::cacheMirrorBlock_ = 0;
uint8_t SdVolume::allocContiguous(uint32_t count, uint32_t* curCluster) {
uint32_t bgnCluster;
uint8_t setStart;

  if (*curCluster) {
    bgnCluster = *curCluster + 1;
    setStart = false;
  } else {
    bgnCluster = allocSearchStart_;
    setStart = 1 == count;
  }
  uint32_t endCluster = bgnCluster;
  uint32_t fatEnd = clusterCount_ + 1;
  for (uint32_t n = 0;; n++, endCluster++) {
    if (n >= clusterCount_) return false;
    if (endCluster > fatEnd) {
      bgnCluster = endCluster = 2;
    }
    uint32_t f;
    if (!fatGet(endCluster, &f)) return false;

    if (f != 0) {
      bgnCluster = endCluster + 1;
    } else if ((endCluster - bgnCluster + 1) == count) {
      break;
    }
  }
  if (!fatPutEOC(endCluster)) return false;

  while (endCluster > bgnCluster) {
    if (!fatPut(endCluster - 1, endCluster)) return false;
    endCluster--;
  }
  if (*curCluster != 0) {
    if (!fatPut(*curCluster, bgnCluster)) return false;
  }
  *curCluster = bgnCluster;

  if (setStart) allocSearchStart_ = bgnCluster + 1;

  return true;
}
uint8_t SdVolume::cacheFlush(void) {
  if (cacheDirty_) {
    if (!sdCard_->writeBlock(cacheBlockNumber_, cacheBuffer_.data)) {
      return false;
    }
    if (cacheMirrorBlock_) {
      if (!sdCard_->writeBlock(cacheMirrorBlock_, cacheBuffer_.data)) {
        return false;
      }
      cacheMirrorBlock_ = 0;
    }
    cacheDirty_ = 0;
  }
  return true;
}
uint8_t SdVolume::cacheRawBlock(uint32_t blockNumber, uint8_t action) {
  if (cacheBlockNumber_ != blockNumber) {
    if (!cacheFlush()) return false;
    if (!sdCard_->readBlock(blockNumber, cacheBuffer_.data)) return false;
    cacheBlockNumber_ = blockNumber;
  }
  cacheDirty_ |= action;
  return true;
}

uint8_t SdVolume::cacheZeroBlock(uint32_t blockNumber) {
  if (!cacheFlush()) return false;

  for (uint16_t i = 0; i < 512; i++) {
    cacheBuffer_.data[i] = 0;
  }
  cacheBlockNumber_ = blockNumber;
  cacheSetDirty();
  return true;
}
uint8_t SdVolume::chainSize(uint32_t cluster, uint32_t* size) const {
  uint32_t s = 0;
  do {
    if (!fatGet(cluster, &cluster)) return false;
    s += 512UL << clusterSizeShift_;
  } while (!isEOC(cluster));
  *size = s;
  return true;
}
uint8_t SdVolume::fatGet(uint32_t cluster, uint32_t* value) const {
  if (cluster > (clusterCount_ + 1)) return false;
  uint32_t lba = fatStartBlock_;
  lba += fatType_ == 16 ? cluster >> 8 : cluster >> 7;
  if (lba != cacheBlockNumber_) {
    if (!cacheRawBlock(lba, CACHE_FOR_READ)) return false;
  }
  if (fatType_ == 16) {
    *value = cacheBuffer_.fat16[cluster & 0XFF];
  } else {
    *value = cacheBuffer_.fat32[cluster & 0X7F] & FAT32MASK;
  }
  return true;
}
uint8_t SdVolume::fatPut(uint32_t cluster, uint32_t value) {
  if (cluster < 2) return false;

  if (cluster > (clusterCount_ + 1)) return false;

  uint32_t lba = fatStartBlock_;
  lba += fatType_ == 16 ? cluster >> 8 : cluster >> 7;

  if (lba != cacheBlockNumber_) {
    if (!cacheRawBlock(lba, CACHE_FOR_READ)) return false;
  }
  if (fatType_ == 16) {
    cacheBuffer_.fat16[cluster & 0XFF] = value;
  } else {
    cacheBuffer_.fat32[cluster & 0X7F] = value;
  }
  cacheSetDirty();

  if (fatCount_ > 1) cacheMirrorBlock_ = lba + blocksPerFat_;
  return true;
}
uint8_t SdVolume::freeChain(uint32_t cluster) {
  allocSearchStart_ = 2;

  do {
    uint32_t next;
    if (!fatGet(cluster, &next)) return false;
    if (!fatPut(cluster, 0)) return false;
    cluster = next;
  } while (!isEOC(cluster));

  return true;
}

uint8_t SdVolume::init(Sd2Card* dev, uint8_t part) {
  uint32_t volumeStartBlock = 0;
  sdCard_ = dev;
  if (part) {
    if (part > 4)return false;
    if (!cacheRawBlock(volumeStartBlock, CACHE_FOR_READ)) return false;
    part_t* p = &cacheBuffer_.mbr.part[part-1];
    if ((p->boot & 0X7F) !=0  ||
      p->totalSectors < 100 ||
      p->firstSector == 0) {
      return false;
    }
    volumeStartBlock = p->firstSector;
  }
  if (!cacheRawBlock(volumeStartBlock, CACHE_FOR_READ)) return false;
  bpb_t* bpb = &cacheBuffer_.fbs.bpb;
  if (bpb->bytesPerSector != 512 ||
    bpb->fatCount == 0 ||
    bpb->reservedSectorCount == 0 ||
    bpb->sectorsPerCluster == 0) {
      return false;
  }
  fatCount_ = bpb->fatCount;
  blocksPerCluster_ = bpb->sectorsPerCluster;

  clusterSizeShift_ = 0;
  while (blocksPerCluster_ != (1 << clusterSizeShift_)) {
    if (clusterSizeShift_++ > 7) return false;
  }
  blocksPerFat_ = bpb->sectorsPerFat16 ?
                    bpb->sectorsPerFat16 : bpb->sectorsPerFat32;

  fatStartBlock_ = volumeStartBlock + bpb->reservedSectorCount;

  rootDirEntryCount_ = bpb->rootDirEntryCount;

  rootDirStart_ = fatStartBlock_ + bpb->fatCount * blocksPerFat_;

  dataStartBlock_ = rootDirStart_ + ((32 * bpb->rootDirEntryCount + 511)/512);

  uint32_t totalBlocks = bpb->totalSectors16 ?
                           bpb->totalSectors16 : bpb->totalSectors32;
  clusterCount_ = totalBlocks - (dataStartBlock_ - volumeStartBlock);
  clusterCount_ >>= clusterSizeShift_;

  if (clusterCount_ < 4085) {
    fatType_ = 12;
  } else if (clusterCount_ < 65525) {
    fatType_ = 16;
  } else {
    rootDirStart_ = bpb->fat32RootCluster;
    fatType_ = 32;
  }
  return true;
}

String::String(const char *cstr)
{
    init();
    if (cstr) copy(cstr, strlen(cstr));
}

String::String(const String &value)
{
    init();
    *this = value;
}

#ifdef __GXX_EXPERIMENTAL_CXX0X__
String::String(String &&rval)
{
    init();
    move(rval);
}
String::String(StringSumHelper &&rval)
{
    init();
    move(rval);
}
#endif

String::String(char c)
{
    init();
    char buf[2];
    buf[0] = c;
    buf[1] = 0;
    *this = buf;
}

String::String(unsigned char value, unsigned char base)
{
    init();
    char buf[9];
    utoa(value, buf, base);
    *this = buf;
}

String::String(int value, unsigned char base)
{
    init();
    char buf[18];
    itoa(value, buf, base);
    *this = buf;
}

String::String(unsigned int value, unsigned char base)
{
    init();
    char buf[17];
    utoa(value, buf, base);
    *this = buf;
}

String::String(long value, unsigned char base)
{
    init();
    char buf[34];
    ltoa(value, buf, base);
    *this = buf;
}

String::String(unsigned long value, unsigned char base)
{
    init();
    char buf[33];
    ultoa(value, buf, base);
    *this = buf;
}

String::~String()
{
    free(buffer);
}

inline void String::init(void)
{
    buffer = NULL;
    capacity = 0;
    len = 0;
    flags = 0;
}

void String::invalidate(void)
{
    if (buffer) free(buffer);
    buffer = NULL;
    capacity = len = 0;
}

unsigned char String::reserve(unsigned int size)
{
    if (buffer && capacity >= size) return 1;
    if (changeBuffer(size)) {
        if (len == 0) buffer[0] = 0;
        return 1;
    }
    return 0;
}

unsigned char String::changeBuffer(unsigned int maxStrLen)
{
    char *newbuffer = (char *)realloc(buffer, maxStrLen + 1);
    if (newbuffer) {
        buffer = newbuffer;
        capacity = maxStrLen;
        return 1;
    }
    return 0;
}

String & String::copy(const char *cstr, unsigned int length)
{
    if (!reserve(length)) {
        invalidate();
        return *this;
    }
    len = length;
    strcpy(buffer, cstr);
    return *this;
}

#ifdef __GXX_EXPERIMENTAL_CXX0X__
void String::move(String &rhs)
{
    if (buffer) {
        if (capacity >= rhs.len) {
            strcpy(buffer, rhs.buffer);
            len = rhs.len;
            rhs.len = 0;
            return;
        } else {
            free(buffer);
        }
    }
    buffer = rhs.buffer;
    capacity = rhs.capacity;
    len = rhs.len;
    rhs.buffer = NULL;
    rhs.capacity = 0;
    rhs.len = 0;
}
#endif

String & String::operator = (const String &rhs)
{
    if (this == &rhs) return *this;

    if (rhs.buffer) copy(rhs.buffer, rhs.len);
    else invalidate();

    return *this;
}

#ifdef __GXX_EXPERIMENTAL_CXX0X__
String & String::operator = (String &&rval)
{
    if (this != &rval) move(rval);
    return *this;
}

String & String::operator = (StringSumHelper &&rval)
{
    if (this != &rval) move(rval);
    return *this;
}
#endif

String & String::operator = (const char *cstr)
{
    if (cstr) copy(cstr, strlen(cstr));
    else invalidate();

    return *this;
}

unsigned char String::concat(const String &s)
{
    return concat(s.buffer, s.len);
}

unsigned char String::concat(const char *cstr, unsigned int length)
{
    unsigned int newlen = len + length;
    if (!cstr) return 0;
    if (length == 0) return 1;
    if (!reserve(newlen)) return 0;
    strcpy(buffer + len, cstr);
    len = newlen;
    return 1;
}

unsigned char String::concat(const char *cstr)
{
    if (!cstr) return 0;
    return concat(cstr, strlen(cstr));
}

unsigned char String::concat(char c)
{
    char buf[2];
    buf[0] = c;
    buf[1] = 0;
    return concat(buf, 1);
}

unsigned char String::concat(unsigned char num)
{
    char buf[4];
    itoa(num, buf, 10);
    return concat(buf, strlen(buf));
}

unsigned char String::concat(int num)
{
    char buf[7];
    itoa(num, buf, 10);
    return concat(buf, strlen(buf));
}

unsigned char String::concat(unsigned int num)
{
    char buf[6];
    utoa(num, buf, 10);
    return concat(buf, strlen(buf));
}

unsigned char String::concat(long num)
{
    char buf[12];
    ltoa(num, buf, 10);
    return concat(buf, strlen(buf));
}

unsigned char String::concat(unsigned long num)
{
    char buf[11];
    ultoa(num, buf, 10);
    return concat(buf, strlen(buf));
}

StringSumHelper & operator + (const StringSumHelper &lhs, const String &rhs)
{
    StringSumHelper &a = const_cast<StringSumHelper&>(lhs);
    if (!a.concat(rhs.buffer, rhs.len)) a.invalidate();
    return a;
}

StringSumHelper & operator + (const StringSumHelper &lhs, const char *cstr)
{
    StringSumHelper &a = const_cast<StringSumHelper&>(lhs);
    if (!cstr || !a.concat(cstr, strlen(cstr))) a.invalidate();
    return a;
}

StringSumHelper & operator + (const StringSumHelper &lhs, char c)
{
    StringSumHelper &a = const_cast<StringSumHelper&>(lhs);
    if (!a.concat(c)) a.invalidate();
    return a;
}

StringSumHelper & operator + (const StringSumHelper &lhs, unsigned char num)
{
    StringSumHelper &a = const_cast<StringSumHelper&>(lhs);
    if (!a.concat(num)) a.invalidate();
    return a;
}

StringSumHelper & operator + (const StringSumHelper &lhs, int num)
{
    StringSumHelper &a = const_cast<StringSumHelper&>(lhs);
    if (!a.concat(num)) a.invalidate();
    return a;
}

StringSumHelper & operator + (const StringSumHelper &lhs, unsigned int num)
{
    StringSumHelper &a = const_cast<StringSumHelper&>(lhs);
    if (!a.concat(num)) a.invalidate();
    return a;
}

StringSumHelper & operator + (const StringSumHelper &lhs, long num)
{
    StringSumHelper &a = const_cast<StringSumHelper&>(lhs);
    if (!a.concat(num)) a.invalidate();
    return a;
}

StringSumHelper & operator + (const StringSumHelper &lhs, unsigned long num)
{
    StringSumHelper &a = const_cast<StringSumHelper&>(lhs);
    if (!a.concat(num)) a.invalidate();
    return a;
}

int String::compareTo(const String &s) const
{
    if (!buffer || !s.buffer) {
        if (s.buffer && s.len > 0) return 0 - *(unsigned char *)s.buffer;
        if (buffer && len > 0) return *(unsigned char *)buffer;
        return 0;
    }
    return strcmp(buffer, s.buffer);
}

unsigned char String::equals(const String &s2) const
{
    return (len == s2.len && compareTo(s2) == 0);
}

unsigned char String::equals(const char *cstr) const
{
    if (len == 0) return (cstr == NULL || *cstr == 0);
    if (cstr == NULL) return buffer[0] == 0;
    return strcmp(buffer, cstr) == 0;
}

unsigned char String::operator<(const String &rhs) const
{
    return compareTo(rhs) < 0;
}

unsigned char String::operator>(const String &rhs) const
{
    return compareTo(rhs) > 0;
}

unsigned char String::operator<=(const String &rhs) const
{
    return compareTo(rhs) <= 0;
}

unsigned char String::operator>=(const String &rhs) const
{
    return compareTo(rhs) >= 0;
}

unsigned char String::equalsIgnoreCase( const String &s2 ) const
{
    if (this == &s2) return 1;
    if (len != s2.len) return 0;
    if (len == 0) return 1;
    const char *p1 = buffer;
    const char *p2 = s2.buffer;
    while (*p1) {
        if (tolower(*p1++) != tolower(*p2++)) return 0;
    }
    return 1;
}

unsigned char String::startsWith( const String &s2 ) const
{
    if (len < s2.len) return 0;
    return startsWith(s2, 0);
}

unsigned char String::startsWith( const String &s2, unsigned int offset ) const
{
    if (offset > len - s2.len || !buffer || !s2.buffer) return 0;
    return strncmp( &buffer[offset], s2.buffer, s2.len ) == 0;
}

unsigned char String::endsWith( const String &s2 ) const
{
    if ( len < s2.len || !buffer || !s2.buffer) return 0;
    return strcmp(&buffer[len - s2.len], s2.buffer) == 0;
}

char String::charAt(unsigned int loc) const
{
    return operator[](loc);
}

void String::setCharAt(unsigned int loc, char c)
{
    if (loc < len) buffer[loc] = c;
}

char & String::operator[](unsigned int index)
{
    static char dummy_writable_char;
    if (index >= len || !buffer) {
        dummy_writable_char = 0;
        return dummy_writable_char;
    }
    return buffer[index];
}

char String::operator[]( unsigned int index ) const
{
    if (index >= len || !buffer) return 0;
    return buffer[index];
}

void String::getBytes(unsigned char *buf, unsigned int bufsize, unsigned int index) const
{
    if (!bufsize || !buf) return;
    if (index >= len) {
        buf[0] = 0;
        return;
    }
    unsigned int n = bufsize - 1;
    if (n > len - index) n = len - index;
    strncpy((char *)buf, buffer + index, n);
    buf[n] = 0;
}

int String::indexOf(char c) const
{
    return indexOf(c, 0);
}

int String::indexOf( char ch, unsigned int fromIndex ) const
{
    if (fromIndex >= len) return -1;
    const char* temp = strchr(buffer + fromIndex, ch);
    if (temp == NULL) return -1;
    return temp - buffer;
}

int String::indexOf(const String &s2) const
{
    return indexOf(s2, 0);
}

int String::indexOf(const String &s2, unsigned int fromIndex) const
{
    if (fromIndex >= len) return -1;
    const char *found = strstr(buffer + fromIndex, s2.buffer);
    if (found == NULL) return -1;
    return found - buffer;
}

int String::lastIndexOf( char theChar ) const
{
    return lastIndexOf(theChar, len - 1);
}

int String::lastIndexOf(char ch, unsigned int fromIndex) const
{
    if (fromIndex >= len) return -1;
    char tempchar = buffer[fromIndex + 1];
    buffer[fromIndex + 1] = '\0';
    char* temp = strrchr( buffer, ch );
    buffer[fromIndex + 1] = tempchar;
    if (temp == NULL) return -1;
    return temp - buffer;
}

int String::lastIndexOf(const String &s2) const
{
    return lastIndexOf(s2, len - s2.len);
}

int String::lastIndexOf(const String &s2, unsigned int fromIndex) const
{
    if (s2.len == 0 || len == 0 || s2.len > len) return -1;
    if (fromIndex >= len) fromIndex = len - 1;
    int found = -1;
    for (char *p = buffer; p <= buffer + fromIndex; p++) {
        p = strstr(p, s2.buffer);
        if (!p) break;
        if ((unsigned int)(p - buffer) <= fromIndex) found = p - buffer;
    }
    return found;
}

String String::substring( unsigned int left ) const
{
    return substring(left, len);
}

String String::substring(unsigned int left, unsigned int right) const
{
    if (left > right)
    {
        unsigned int temp = right;
        right = left;
        left = temp;
    }

    String out;
    if (left > len) return out;
    if (right > len) right = len;
    char temp = buffer[right];
    buffer[right] = '\0';
    out = buffer + left;
    buffer[right] = temp;
    return out;
}

void String::replace(char find, char replace)
{
    if (!buffer) return;
    for (char *p = buffer; *p; p++) {
        if (*p == find) *p = replace;
    }
}

void String::replace(const String& find, const String& replace)
{
    if (len == 0 || find.len == 0) return;
    int diff = replace.len - find.len;
    char *readFrom = buffer;
    char *foundAt;
    if (diff == 0) {
        while ((foundAt = strstr(readFrom, find.buffer)) != NULL) {
            memcpy(foundAt, replace.buffer, replace.len);
            readFrom = foundAt + replace.len;
        }
    } else if (diff < 0) {
        char *writeTo = buffer;
        while ((foundAt = strstr(readFrom, find.buffer)) != NULL) {
            unsigned int n = foundAt - readFrom;
            memcpy(writeTo, readFrom, n);
            writeTo += n;
            memcpy(writeTo, replace.buffer, replace.len);
            writeTo += replace.len;
            readFrom = foundAt + find.len;
            len += diff;
        }
        strcpy(writeTo, readFrom);
    } else {
        unsigned int size = len;
        while ((foundAt = strstr(readFrom, find.buffer)) != NULL) {
            readFrom = foundAt + find.len;
            size += diff;
        }
        if (size == len) return;
        if (size > capacity && !changeBuffer(size)) return;
        int index = len - 1;
        while (index >= 0 && (index = lastIndexOf(find, index)) >= 0) {
            readFrom = buffer + index + find.len;
            memmove(readFrom + diff, readFrom, len - (readFrom - buffer));
            len += diff;
            buffer[len] = 0;
            memcpy(buffer + index, replace.buffer, replace.len);
            index--;
        }
    }
}

void String::toLowerCase(void)
{
    if (!buffer) return;
    for (char *p = buffer; *p; p++) {
        *p = tolower(*p);
    }
}

void String::toUpperCase(void)
{
    if (!buffer) return;
    for (char *p = buffer; *p; p++) {
        *p = toupper(*p);
    }
}

void String::trim(void)
{
    if (!buffer || len == 0) return;
    char *begin = buffer;
    while (isspace(*begin)) begin++;
    char *end = buffer + len - 1;
    while (isspace(*end) && end >= begin) end--;
    len = end + 1 - begin;
    if (begin > buffer) memcpy(buffer, begin, len);
    buffer[len] = 0;
}

long String::toInt(void) const
{
    if (buffer) return atol(buffer);
    return 0;
}

static void spiSend(uint8_t b) {
  SPDR = b;
  while (!(SPSR & (1 << SPIF)));
}
static  uint8_t spiRec(void) {
  spiSend(0XFF);
  return SPDR;
}

uint8_t Sd2Card::cardCommand(uint8_t cmd, uint32_t arg) {
  readEnd();
  chipSelectLow();
  waitNotBusy(300);
  spiSend(cmd | 0x40);
  for (int8_t s = 24; s >= 0; s -= 8) spiSend(arg >> s);
  uint8_t crc = 0XFF;
  if (cmd == CMD0) crc = 0X95;
  if (cmd == CMD8) crc = 0X87;
  spiSend(crc);
  for (uint8_t i = 0; ((status_ = spiRec()) & 0X80) && i != 0XFF; i++);
  return status_;
}
uint32_t Sd2Card::cardSize(void) {
  csd_t csd;
  if (!readCSD(&csd)) return 0;
  if (csd.v1.csd_ver == 0) {
    uint8_t read_bl_len = csd.v1.read_bl_len;
    uint16_t c_size = (csd.v1.c_size_high << 10)
                      | (csd.v1.c_size_mid << 2) | csd.v1.c_size_low;
    uint8_t c_size_mult = (csd.v1.c_size_mult_high << 1)
                          | csd.v1.c_size_mult_low;
    return (uint32_t)(c_size + 1) << (c_size_mult + read_bl_len - 7);
  } else if (csd.v2.csd_ver == 1) {
    uint32_t c_size = ((uint32_t)csd.v2.c_size_high << 16)
                      | (csd.v2.c_size_mid << 8) | csd.v2.c_size_low;
    return (c_size + 1) << 10;
  } else {
    error(SD_CARD_ERROR_BAD_CSD);
    return 0;
  }
}
void Sd2Card::chipSelectHigh(void) {
  digitalWrite(chipSelectPin_, HIGH);
}

void Sd2Card::chipSelectLow(void) {
  digitalWrite(chipSelectPin_, LOW);
}

uint8_t Sd2Card::erase(uint32_t firstBlock, uint32_t lastBlock) {
  if (!eraseSingleBlockEnable()) {
    error(SD_CARD_ERROR_ERASE_SINGLE_BLOCK);
    goto fail;
  }
  if (type_ != SD_CARD_TYPE_SDHC) {
    firstBlock <<= 9;
    lastBlock <<= 9;
  }
  if (cardCommand(CMD32, firstBlock)
    || cardCommand(CMD33, lastBlock)
    || cardCommand(CMD38, 0)) {
      error(SD_CARD_ERROR_ERASE);
      goto fail;
  }
  if (!waitNotBusy(SD_ERASE_TIMEOUT)) {
    error(SD_CARD_ERROR_ERASE_TIMEOUT);
    goto fail;
  }
  chipSelectHigh();
  return true;

 fail:
  chipSelectHigh();
  return false;
}

uint8_t Sd2Card::eraseSingleBlockEnable(void) {
  csd_t csd;
  return readCSD(&csd) ? csd.v1.erase_blk_en : 0;
}

uint8_t Sd2Card::init(uint8_t sckRateID, uint8_t chipSelectPin) {
  errorCode_ = inBlock_ = partialBlockRead_ = type_ = 0;
  chipSelectPin_ = chipSelectPin;
  uint16_t t0 = (uint16_t)millis();
  uint32_t arg;

  pinMode(chipSelectPin_, OUTPUT);
  chipSelectHigh();
  pinMode(SPI_MISO_PIN, INPUT);
  pinMode(SPI_MOSI_PIN, OUTPUT);
  pinMode(SPI_SCK_PIN, OUTPUT);

  pinMode(SS_PIN, OUTPUT);
  digitalWrite(SS_PIN, HIGH);
  SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0);
  SPSR &= ~(1 << SPI2X);

  for (uint8_t i = 0; i < 10; i++) spiSend(0XFF);

  chipSelectLow();

  while ((status_ = cardCommand(CMD0, 0)) != R1_IDLE_STATE) {
    if (((uint16_t)millis() - t0) > SD_INIT_TIMEOUT) {
      error(SD_CARD_ERROR_CMD0);
      goto fail;
    }
  }
  if ((cardCommand(CMD8, 0x1AA) & R1_ILLEGAL_COMMAND)) {
    type(SD_CARD_TYPE_SD1);
  } else {
    for (uint8_t i = 0; i < 4; i++) status_ = spiRec();
    if (status_ != 0XAA) {
      error(SD_CARD_ERROR_CMD8);
      goto fail;
    }
    type(SD_CARD_TYPE_SD2);
  }
  arg = type() == SD_CARD_TYPE_SD2 ? 0X40000000 : 0;

  while ((status_ = cardAcmd(ACMD41, arg)) != R1_READY_STATE) {
    if (((uint16_t)millis() - t0) > SD_INIT_TIMEOUT) {
      error(SD_CARD_ERROR_ACMD41);
      goto fail;
    }
  }

  if (type() == SD_CARD_TYPE_SD2) {
    if (cardCommand(CMD58, 0)) {
      error(SD_CARD_ERROR_CMD58);
      goto fail;
    }
    if ((spiRec() & 0XC0) == 0XC0) type(SD_CARD_TYPE_SDHC);
    for (uint8_t i = 0; i < 3; i++) spiRec();
  }
  chipSelectHigh();

#ifndef SOFTWARE_SPI
  return setSckRate(sckRateID);
#else
  return true;
#endif

 fail:
  chipSelectHigh();
  return false;
}
void Sd2Card::partialBlockRead(uint8_t value) {
  readEnd();
  partialBlockRead_ = value;
}

uint8_t Sd2Card::readBlock(uint32_t block, uint8_t* dst) {
  return readData(block, 0, 512, dst);
}
uint8_t Sd2Card::readData(uint32_t block,
        uint16_t offset, uint16_t count, uint8_t* dst) {
  uint16_t n;
  if (count == 0) return true;
  if ((count + offset) > 512) {
    goto fail;
  }
  if (!inBlock_ || block != block_ || offset < offset_) {
    block_ = block;
    if (type()!= SD_CARD_TYPE_SDHC) block <<= 9;
    if (cardCommand(CMD17, block)) {
      error(SD_CARD_ERROR_CMD17);
      goto fail;
    }
    if (!waitStartBlock()) {
      goto fail;
    }
    offset_ = 0;
    inBlock_ = 1;
  }

#ifdef OPTIMIZE_HARDWARE_SPI
  SPDR = 0XFF;

  for (;offset_ < offset; offset_++) {
    while (!(SPSR & (1 << SPIF)));
    SPDR = 0XFF;
  }
  n = count - 1;
  for (uint16_t i = 0; i < n; i++) {
    while (!(SPSR & (1 << SPIF)));
    dst[i] = SPDR;
    SPDR = 0XFF;
  }
  while (!(SPSR & (1 << SPIF)));
  dst[n] = SPDR;
#else
  for (;offset_ < offset; offset_++) {
    spiRec();
  }
  for (uint16_t i = 0; i < count; i++) {
    dst[i] = spiRec();
  }
#endif

  offset_ += count;
  if (!partialBlockRead_ || offset_ >= 512) {
    readEnd();
  }
  return true;

 fail:
  chipSelectHigh();
  return false;
}
void Sd2Card::readEnd(void) {
  if (inBlock_) {
#ifdef OPTIMIZE_HARDWARE_SPI
    SPDR = 0XFF;
    while (offset_++ < 513) {
      while (!(SPSR & (1 << SPIF)));
      SPDR = 0XFF;
    }
    while (!(SPSR & (1 << SPIF)));
#else
    while (offset_++ < 514) spiRec();
#endif
    chipSelectHigh();
    inBlock_ = 0;
  }
}
uint8_t Sd2Card::readRegister(uint8_t cmd, void* buf) {
  uint8_t* dst = reinterpret_cast<uint8_t*>(buf);
  if (cardCommand(cmd, 0)) {
    error(SD_CARD_ERROR_READ_REG);
    goto fail;
  }
  if (!waitStartBlock()) goto fail;
  for (uint16_t i = 0; i < 16; i++) dst[i] = spiRec();
  spiRec();
  spiRec();
  chipSelectHigh();
  return true;

 fail:
  chipSelectHigh();
  return false;
}

uint8_t Sd2Card::setSckRate(uint8_t sckRateID) {
  if (sckRateID > 6) {
    error(SD_CARD_ERROR_SCK_RATE);
    return false;
  }
  if ((sckRateID & 1) || sckRateID == 6) {
    SPSR &= ~(1 << SPI2X);
  } else {
    SPSR |= (1 << SPI2X);
  }
  SPCR &= ~((1 <<SPR1) | (1 << SPR0));
  SPCR |= (sckRateID & 4 ? (1 << SPR1) : 0)
    | (sckRateID & 2 ? (1 << SPR0) : 0);
  return true;
}
uint8_t Sd2Card::waitNotBusy(uint16_t timeoutMillis) {
  uint16_t t0 = millis();
  do {
    if (spiRec() == 0XFF) return true;
  }
  while (((uint16_t)millis() - t0) < timeoutMillis);
  return false;
}
uint8_t Sd2Card::waitStartBlock(void) {
  uint16_t t0 = millis();
  while ((status_ = spiRec()) == 0XFF) {
    if (((uint16_t)millis() - t0) > SD_READ_TIMEOUT) {
      error(SD_CARD_ERROR_READ_TIMEOUT);
      goto fail;
    }
  }
  if (status_ != DATA_START_BLOCK) {
    error(SD_CARD_ERROR_READ);
    goto fail;
  }
  return true;

 fail:
  chipSelectHigh();
  return false;
}

uint8_t Sd2Card::writeBlock(uint32_t blockNumber, const uint8_t* src) {
#if SD_PROTECT_BLOCK_ZERO
  if (blockNumber == 0) {
    error(SD_CARD_ERROR_WRITE_BLOCK_ZERO);
    goto fail;
  }
#endif

  if (type() != SD_CARD_TYPE_SDHC) blockNumber <<= 9;
  if (cardCommand(CMD24, blockNumber)) {
    error(SD_CARD_ERROR_CMD24);
    goto fail;
  }
  if (!writeData(DATA_START_BLOCK, src)) goto fail;

  if (!waitNotBusy(SD_WRITE_TIMEOUT)) {
    error(SD_CARD_ERROR_WRITE_TIMEOUT);
    goto fail;
  }
  if (cardCommand(CMD13, 0) || spiRec()) {
    error(SD_CARD_ERROR_WRITE_PROGRAMMING);
    goto fail;
  }
  chipSelectHigh();
  return true;

 fail:
  chipSelectHigh();
  return false;
}
uint8_t Sd2Card::writeData(const uint8_t* src) {
  if (!waitNotBusy(SD_WRITE_TIMEOUT)) {
    error(SD_CARD_ERROR_WRITE_MULTIPLE);
    chipSelectHigh();
    return false;
  }
  return writeData(WRITE_MULTIPLE_TOKEN, src);
}
uint8_t Sd2Card::writeData(uint8_t token, const uint8_t* src) {
#ifdef OPTIMIZE_HARDWARE_SPI

  SPDR = token;

  for (uint16_t i = 0; i < 512; i += 2) {
    while (!(SPSR & (1 << SPIF)));
    SPDR = src[i];
    while (!(SPSR & (1 << SPIF)));
    SPDR = src[i+1];
  }

  while (!(SPSR & (1 << SPIF)));

#else
  spiSend(token);
  for (uint16_t i = 0; i < 512; i++) {
    spiSend(src[i]);
  }
#endif
  spiSend(0xff);
  spiSend(0xff);
  status_ = spiRec();
  if ((status_ & DATA_RES_MASK) != DATA_RES_ACCEPTED) {
    error(SD_CARD_ERROR_WRITE);
    chipSelectHigh();
    return false;
  }
  return true;
}

uint8_t Sd2Card::writeStart(uint32_t blockNumber, uint32_t eraseCount) {
#if SD_PROTECT_BLOCK_ZERO
  if (blockNumber == 0) {
    error(SD_CARD_ERROR_WRITE_BLOCK_ZERO);
    goto fail;
  }
#endif
  if (cardAcmd(ACMD23, eraseCount)) {
    error(SD_CARD_ERROR_ACMD23);
    goto fail;
  }
  if (type() != SD_CARD_TYPE_SDHC) blockNumber <<= 9;
  if (cardCommand(CMD25, blockNumber)) {
    error(SD_CARD_ERROR_CMD25);
    goto fail;
  }
  return true;

 fail:
  chipSelectHigh();
  return false;
}

uint8_t Sd2Card::writeStop(void) {
  if (!waitNotBusy(SD_WRITE_TIMEOUT)) goto fail;
  spiSend(STOP_TRAN_TOKEN);
  if (!waitNotBusy(SD_WRITE_TIMEOUT)) goto fail;
  chipSelectHigh();
  return true;
 fail:
  error(SD_CARD_ERROR_STOP_TRAN);
  chipSelectHigh();
  return false;
}

#include <avr/pgmspace.h>
//#include <Arduino.h>
void (*SdFile::dateTime_)(uint16_t* date, uint16_t* time) = NULL;

#if ALLOW_DEPRECATED_FUNCTIONS
void (*SdFile::oldDateTime_)(uint16_t& date, uint16_t& time) = NULL;
#endif
uint8_t SdFile::addCluster() {
  if (!vol_->allocContiguous(1, &curCluster_)) return false;

  if (firstCluster_ == 0) {
    firstCluster_ = curCluster_;
    flags_ |= F_FILE_DIR_DIRTY;
  }
  return true;
}
uint8_t SdFile::addDirCluster(void) {
  if (!addCluster()) return false;

  uint32_t block = vol_->clusterStartBlock(curCluster_);
  for (uint8_t i = vol_->blocksPerCluster_; i != 0; i--) {
    if (!SdVolume::cacheZeroBlock(block + i - 1)) return false;
  }
  fileSize_ += 512UL << vol_->clusterSizeShift_;
  return true;
}
dir_t* SdFile::cacheDirEntry(uint8_t action) {
  if (!SdVolume::cacheRawBlock(dirBlock_, action)) return NULL;
  return SdVolume::cacheBuffer_.dir + dirIndex_;
}

uint8_t SdFile::close(void) {
  if (!sync())return false;
  type_ = FAT_FILE_TYPE_CLOSED;
  return true;
}

uint8_t SdFile::contiguousRange(uint32_t* bgnBlock, uint32_t* endBlock) {
  if (firstCluster_ == 0) return false;

  for (uint32_t c = firstCluster_; ; c++) {
    uint32_t next;
    if (!vol_->fatGet(c, &next)) return false;

    if (next != (c + 1)) {
      if (!vol_->isEOC(next)) return false;
      *bgnBlock = vol_->clusterStartBlock(firstCluster_);
      *endBlock = vol_->clusterStartBlock(c)
                  + vol_->blocksPerCluster_ - 1;
      return true;
    }
  }
}

uint8_t SdFile::createContiguous(SdFile* dirFile,
        const char* fileName, uint32_t size) {
  if (size == 0) return false;
  if (!open(dirFile, fileName, O_CREAT | O_EXCL | O_RDWR)) return false;

  uint32_t count = ((size - 1) >> (vol_->clusterSizeShift_ + 9)) + 1;

  if (!vol_->allocContiguous(count, &firstCluster_)) {
    remove();
    return false;
  }
  fileSize_ = size;

  flags_ |= F_FILE_DIR_DIRTY;
  return sync();
}

uint8_t SdFile::dirEntry(dir_t* dir) {
  if (!sync()) return false;

  dir_t* p = cacheDirEntry(SdVolume::CACHE_FOR_READ);
  if (!p) return false;

  memcpy(dir, p, sizeof(dir_t));
  return true;
}

void SdFile::dirName(const dir_t& dir, char* name) {
  uint8_t j = 0;
  for (uint8_t i = 0; i < 11; i++) {
    if (dir.name[i] == ' ')continue;
    if (i == 8) name[j++] = '.';
    name[j++] = dir.name[i];
  }
  name[j] = 0;
}

void SdFile::ls(uint8_t flags, uint8_t indent) {
  dir_t* p;

  rewind();
  while ((p = readDirCache())) {
    if (p->name[0] == DIR_NAME_FREE) break;

    if (p->name[0] == DIR_NAME_DELETED || p->name[0] == '.') continue;

    if (!DIR_IS_FILE_OR_SUBDIR(p)) continue;

    for (int8_t i = 0; i < indent; i++) Serial.print(' ');

    printDirName(*p, flags & (LS_DATE | LS_SIZE) ? 14 : 0);

    if (flags & LS_DATE) {
       printFatDate(p->lastWriteDate);
       Serial.print(' ');
       printFatTime(p->lastWriteTime);
    }
    if (!DIR_IS_SUBDIR(p) && (flags & LS_SIZE)) {
      Serial.print(' ');
      Serial.print(p->fileSize);
    }
    Serial.println();

    if ((flags & LS_R) && DIR_IS_SUBDIR(p)) {
      uint16_t index = curPosition()/32 - 1;
      SdFile s;
      if (s.open(this, index, O_READ)) s.ls(flags, indent + 2);
      seekSet(32 * (index + 1));
    }
  }
}
uint8_t SdFile::make83Name(const char* str, uint8_t* name) {
  uint8_t c;
  uint8_t n = 7;
  uint8_t i = 0;
  while (i < 11) name[i++] = ' ';
  i = 0;
  while ((c = *str++) != '\0') {
    if (c == '.') {
      if (n == 10) return false;  // only one dot allowed
      n = 10;  // max index for full 8.3 name
      i = 8;   // place for extension
    } else {
      // illegal FAT characters
      PGM_P p = PSTR("|<>^+=?/[];,*\"\\");
      uint8_t b;
      while ((b = pgm_read_byte(p++))) if (b == c) return false;
      // check size and only allow ASCII printable characters
      if (i > n || c < 0X21 || c > 0X7E)return false;
      // only upper case allowed in 8.3 names - convert lower to upper
      name[i++] = c < 'a' || c > 'z' ?  c : c + ('A' - 'a');
    }
  }
  // must have a file name, extension is optional
  return name[0] != ' ';
}

uint8_t SdFile::makeDir(SdFile* dir, const char* dirName) {
  dir_t d;

  if (!open(dir, dirName, O_CREAT | O_EXCL | O_RDWR)) return false;

  flags_ = O_READ;
  type_ = FAT_FILE_TYPE_SUBDIR;

  if (!addDirCluster())return false;

  if (!sync()) return false;

  dir_t* p = cacheDirEntry(SdVolume::CACHE_FOR_WRITE);
  if (!p) return false;

  p->attributes = DIR_ATT_DIRECTORY;

  memcpy(&d, p, sizeof(d));
  for (uint8_t i = 1; i < 11; i++) d.name[i] = ' ';
  d.name[0] = '.';

  uint32_t block = vol_->clusterStartBlock(firstCluster_);
  if (!SdVolume::cacheRawBlock(block, SdVolume::CACHE_FOR_WRITE)) return false;

  memcpy(&SdVolume::cacheBuffer_.dir[0], &d, sizeof(d));

  d.name[1] = '.';
  if (dir->isRoot()) {
    d.firstClusterLow = 0;
    d.firstClusterHigh = 0;
  } else {
    d.firstClusterLow = dir->firstCluster_ & 0XFFFF;
    d.firstClusterHigh = dir->firstCluster_ >> 16;
  }
  memcpy(&SdVolume::cacheBuffer_.dir[1], &d, sizeof(d));

  curPosition_ = 2 * sizeof(d);

  return SdVolume::cacheFlush();
}

uint8_t SdFile::open(SdFile* dirFile, const char* fileName, uint8_t oflag) {
  uint8_t dname[11];
  dir_t* p;
  if (isOpen())return false;

  if (!make83Name(fileName, dname)) return false;
  vol_ = dirFile->vol_;
  dirFile->rewind();

  uint8_t emptyFound = false;

  while (dirFile->curPosition_ < dirFile->fileSize_) {
    uint8_t index = 0XF & (dirFile->curPosition_ >> 5);
    p = dirFile->readDirCache();
    if (p == NULL) return false;

    if (p->name[0] == DIR_NAME_FREE || p->name[0] == DIR_NAME_DELETED) {
      if (!emptyFound) {
        emptyFound = true;
        dirIndex_ = index;
        dirBlock_ = SdVolume::cacheBlockNumber_;
      }
      if (p->name[0] == DIR_NAME_FREE) break;
    } else if (!memcmp(dname, p->name, 11)) {
      if ((oflag & (O_CREAT | O_EXCL)) == (O_CREAT | O_EXCL)) return false;

      return openCachedEntry(0XF & index, oflag);
    }
  }
  if ((oflag & (O_CREAT | O_WRITE)) != (O_CREAT | O_WRITE)) return false;

  if (emptyFound) {
    p = cacheDirEntry(SdVolume::CACHE_FOR_WRITE);
    if (!p) return false;
  } else {
    if (dirFile->type_ == FAT_FILE_TYPE_ROOT16) return false;

    if (!dirFile->addDirCluster()) return false;

    // use first entry in cluster
    dirIndex_ = 0;
    p = SdVolume::cacheBuffer_.dir;
  }
  // initialize as empty file
  memset(p, 0, sizeof(dir_t));
  memcpy(p->name, dname, 11);

  // set timestamps
  if (dateTime_) {
    // call user function
    dateTime_(&p->creationDate, &p->creationTime);
  } else {
    // use default date/time
    p->creationDate = FAT_DEFAULT_DATE;
    p->creationTime = FAT_DEFAULT_TIME;
  }
  p->lastAccessDate = p->creationDate;
  p->lastWriteDate = p->creationDate;
  p->lastWriteTime = p->creationTime;

  // force write of entry to SD
  if (!SdVolume::cacheFlush()) return false;

  // open entry in cache
  return openCachedEntry(dirIndex_, oflag);
}

uint8_t SdFile::open(SdFile* dirFile, uint16_t index, uint8_t oflag) {
  if (isOpen())return false;

  if ((oflag & (O_CREAT | O_EXCL)) == (O_CREAT | O_EXCL)) return false;

  vol_ = dirFile->vol_;

  if (!dirFile->seekSet(32 * index)) return false;

  dir_t* p = dirFile->readDirCache();
  if (p == NULL) return false;

  if (p->name[0] == DIR_NAME_FREE ||
      p->name[0] == DIR_NAME_DELETED || p->name[0] == '.') {
    return false;
  }
  return openCachedEntry(index & 0XF, oflag);
}
uint8_t SdFile::openCachedEntry(uint8_t dirIndex, uint8_t oflag) {
  dir_t* p = SdVolume::cacheBuffer_.dir + dirIndex;

  if (p->attributes & (DIR_ATT_READ_ONLY | DIR_ATT_DIRECTORY)) {
    if (oflag & (O_WRITE | O_TRUNC)) return false;
  }
  dirIndex_ = dirIndex;
  dirBlock_ = SdVolume::cacheBlockNumber_;

  firstCluster_ = (uint32_t)p->firstClusterHigh << 16;
  firstCluster_ |= p->firstClusterLow;

  if (DIR_IS_FILE(p)) {
    fileSize_ = p->fileSize;
    type_ = FAT_FILE_TYPE_NORMAL;
  } else if (DIR_IS_SUBDIR(p)) {
    if (!vol_->chainSize(firstCluster_, &fileSize_)) return false;
    type_ = FAT_FILE_TYPE_SUBDIR;
  } else {
    return false;
  }
  flags_ = oflag & (O_ACCMODE | O_SYNC | O_APPEND);

  curCluster_ = 0;
  curPosition_ = 0;

  if (oflag & O_TRUNC) return truncate(0);
  return true;
}

uint8_t SdFile::openRoot(SdVolume* vol) {
  if (isOpen()) return false;

  if (vol->fatType() == 16) {
    type_ = FAT_FILE_TYPE_ROOT16;
    firstCluster_ = 0;
    fileSize_ = 32 * vol->rootDirEntryCount();
  } else if (vol->fatType() == 32) {
    type_ = FAT_FILE_TYPE_ROOT32;
    firstCluster_ = vol->rootDirStart();
    if (!vol->chainSize(firstCluster_, &fileSize_)) return false;
  } else {
    return false;
  }
  vol_ = vol;
  flags_ = O_READ;

  curCluster_ = 0;
  curPosition_ = 0;
  dirBlock_ = 0;
  dirIndex_ = 0;
  return true;
}

void SdFile::printDirName(const dir_t& dir, uint8_t width) {
  uint8_t w = 0;
  for (uint8_t i = 0; i < 11; i++) {
    if (dir.name[i] == ' ')continue;
    if (i == 8) {
      Serial.print('.');
      w++;
    }
    Serial.write(dir.name[i]);
    w++;
  }
  if (DIR_IS_SUBDIR(&dir)) {
    Serial.print('/');
    w++;
  }
  while (w < width) {
    Serial.print(' ');
    w++;
  }
}

void SdFile::printFatDate(uint16_t fatDate) {
  Serial.print(FAT_YEAR(fatDate));
  Serial.print('-');
  printTwoDigits(FAT_MONTH(fatDate));
  Serial.print('-');
  printTwoDigits(FAT_DAY(fatDate));
}

void SdFile::printFatTime(uint16_t fatTime) {
  printTwoDigits(FAT_HOUR(fatTime));
  Serial.print(':');
  printTwoDigits(FAT_MINUTE(fatTime));
  Serial.print(':');
  printTwoDigits(FAT_SECOND(fatTime));
}

void SdFile::printTwoDigits(uint8_t v) {
  char str[3];
  str[0] = '0' + v/10;
  str[1] = '0' + v % 10;
  str[2] = 0;
  Serial.print(str);
}

int16_t SdFile::read(void* buf, uint16_t nbyte) {
  uint8_t* dst = reinterpret_cast<uint8_t*>(buf);

  if (!isOpen() || !(flags_ & O_READ)) return -1;

  if (nbyte > (fileSize_ - curPosition_)) nbyte = fileSize_ - curPosition_;

  uint16_t toRead = nbyte;
  while (toRead > 0) {
    uint32_t block;
    uint16_t offset = curPosition_ & 0X1FF;
    if (type_ == FAT_FILE_TYPE_ROOT16) {
      block = vol_->rootDirStart() + (curPosition_ >> 9);
    } else {
      uint8_t blockOfCluster = vol_->blockOfCluster(curPosition_);
      if (offset == 0 && blockOfCluster == 0) {
        if (curPosition_ == 0) {
          curCluster_ = firstCluster_;
        } else {
          if (!vol_->fatGet(curCluster_, &curCluster_)) return -1;
        }
      }
      block = vol_->clusterStartBlock(curCluster_) + blockOfCluster;
    }
    uint16_t n = toRead;

    if (n > (512 - offset)) n = 512 - offset;

    if ((unbufferedRead() || n == 512) &&
      block != SdVolume::cacheBlockNumber_) {
      if (!vol_->readData(block, offset, n, dst)) return -1;
      dst += n;
    } else {
      if (!SdVolume::cacheRawBlock(block, SdVolume::CACHE_FOR_READ)) return -1;
      uint8_t* src = SdVolume::cacheBuffer_.data + offset;
      uint8_t* end = src + n;
      while (src != end) *dst++ = *src++;
    }
    curPosition_ += n;
    toRead -= n;
  }
  return nbyte;
}

int8_t SdFile::readDir(dir_t* dir) {
  int8_t n;
  if (!isDir() || (0X1F & curPosition_)) return -1;

  while ((n = read(dir, sizeof(dir_t))) == sizeof(dir_t)) {
    if (dir->name[0] == DIR_NAME_FREE) break;
    if (dir->name[0] == DIR_NAME_DELETED || dir->name[0] == '.') continue;
    if (DIR_IS_FILE_OR_SUBDIR(dir)) return n;
  }
  return n < 0 ? -1 : 0;
}
dir_t* SdFile::readDirCache(void) {
  if (!isDir()) return NULL;
  uint8_t i = (curPosition_ >> 5) & 0XF;
  if (read() < 0) return NULL;
  curPosition_ += 31;
  return (SdVolume::cacheBuffer_.dir + i);
}

uint8_t SdFile::remove(void) {
  if (!truncate(0)) return false;
  dir_t* d = cacheDirEntry(SdVolume::CACHE_FOR_WRITE);
  if (!d) return false;
  d->name[0] = DIR_NAME_DELETED;
  type_ = FAT_FILE_TYPE_CLOSED;
  return SdVolume::cacheFlush();
}

uint8_t SdFile::remove(SdFile* dirFile, const char* fileName) {
  SdFile file;
  if (!file.open(dirFile, fileName, O_WRITE)) return false;
  return file.remove();
}

uint8_t SdFile::rmDir(void) {
  if (!isSubDir()) return false;

  rewind();

  while (curPosition_ < fileSize_) {
    dir_t* p = readDirCache();
    if (p == NULL) return false;
    if (p->name[0] == DIR_NAME_FREE) break;
    if (p->name[0] == DIR_NAME_DELETED || p->name[0] == '.') continue;
    if (DIR_IS_FILE_OR_SUBDIR(p)) return false;
  }
  type_ = FAT_FILE_TYPE_NORMAL;
  flags_ |= O_WRITE;
  return remove();
}

uint8_t SdFile::rmRfStar(void) {
  rewind();
  while (curPosition_ < fileSize_) {
    SdFile f;

    uint16_t index = curPosition_/32;

    dir_t* p = readDirCache();
    if (!p) return false;

    if (p->name[0] == DIR_NAME_FREE) break;

    if (p->name[0] == DIR_NAME_DELETED || p->name[0] == '.') continue;

    if (!DIR_IS_FILE_OR_SUBDIR(p)) continue;

    if (!f.open(this, index, O_READ)) return false;
    if (f.isSubDir()) {
      if (!f.rmRfStar()) return false;
    } else {
      f.flags_ |= O_WRITE;
      if (!f.remove()) return false;
    }
    if (curPosition_ != (32*(index + 1))) {
      if (!seekSet(32*(index + 1))) return false;
    }
  }
  // don't try to delete root
  if (isRoot()) return true;
  return rmDir();
}

uint8_t SdFile::seekSet(uint32_t pos) {
  if (!isOpen() || pos > fileSize_) return false;

  if (type_ == FAT_FILE_TYPE_ROOT16) {
    curPosition_ = pos;
    return true;
  }
  if (pos == 0) {
    curCluster_ = 0;
    curPosition_ = 0;
    return true;
  }
  uint32_t nCur = (curPosition_ - 1) >> (vol_->clusterSizeShift_ + 9);
  uint32_t nNew = (pos - 1) >> (vol_->clusterSizeShift_ + 9);

  if (nNew < nCur || curPosition_ == 0) {
    curCluster_ = firstCluster_;
  } else {
    nNew -= nCur;
  }
  while (nNew--) {
    if (!vol_->fatGet(curCluster_, &curCluster_)) return false;
  }
  curPosition_ = pos;
  return true;
}

uint8_t SdFile::sync(void) {
  if (!isOpen()) return false;

  if (flags_ & F_FILE_DIR_DIRTY) {
    dir_t* d = cacheDirEntry(SdVolume::CACHE_FOR_WRITE);
    if (!d) return false;

    if (!isDir()) d->fileSize = fileSize_;

    d->firstClusterLow = firstCluster_ & 0XFFFF;
    d->firstClusterHigh = firstCluster_ >> 16;

    if (dateTime_) {
      dateTime_(&d->lastWriteDate, &d->lastWriteTime);
      d->lastAccessDate = d->lastWriteDate;
    }
    flags_ &= ~F_FILE_DIR_DIRTY;
  }
  return SdVolume::cacheFlush();
}

uint8_t SdFile::timestamp(uint8_t flags, uint16_t year, uint8_t month,
         uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) {
  if (!isOpen()
    || year < 1980
    || year > 2107
    || month < 1
    || month > 12
    || day < 1
    || day > 31
    || hour > 23
    || minute > 59
    || second > 59) {
      return false;
  }
  dir_t* d = cacheDirEntry(SdVolume::CACHE_FOR_WRITE);
  if (!d) return false;

  uint16_t dirDate = FAT_DATE(year, month, day);
  uint16_t dirTime = FAT_TIME(hour, minute, second);
  if (flags & T_ACCESS) {
    d->lastAccessDate = dirDate;
  }
  if (flags & T_CREATE) {
    d->creationDate = dirDate;
    d->creationTime = dirTime;
    d->creationTimeTenths = second & 1 ? 100 : 0;
  }
  if (flags & T_WRITE) {
    d->lastWriteDate = dirDate;
    d->lastWriteTime = dirTime;
  }
  SdVolume::cacheSetDirty();
  return sync();
}

uint8_t SdFile::truncate(uint32_t length) {
  if (!isFile() || !(flags_ & O_WRITE)) return false;

  if (length > fileSize_) return false;

  if (fileSize_ == 0) return true;

  uint32_t newPos = curPosition_ > length ? length : curPosition_;

  if (!seekSet(length)) return false;

  if (length == 0) {
    if (!vol_->freeChain(firstCluster_)) return false;
    firstCluster_ = 0;
  } else {
    uint32_t toFree;
    if (!vol_->fatGet(curCluster_, &toFree)) return false;

    if (!vol_->isEOC(toFree)) {
      if (!vol_->freeChain(toFree)) return false;

      if (!vol_->fatPutEOC(curCluster_)) return false;
    }
  }
  fileSize_ = length;
  flags_ |= F_FILE_DIR_DIRTY;
  if (!sync()) return false;
  return seekSet(newPos);
}

size_t SdFile::write(const void* buf, uint16_t nbyte) {
  const uint8_t* src = reinterpret_cast<const uint8_t*>(buf);
  uint16_t nToWrite = nbyte;
  if (!isFile() || !(flags_ & O_WRITE)) goto writeErrorReturn;
  if ((flags_ & O_APPEND) && curPosition_ != fileSize_) {
    if (!seekEnd()) goto writeErrorReturn;
  }

  while (nToWrite > 0) {
    uint8_t blockOfCluster = vol_->blockOfCluster(curPosition_);
    uint16_t blockOffset = curPosition_ & 0X1FF;
    if (blockOfCluster == 0 && blockOffset == 0) {
      if (curCluster_ == 0) {
        if (firstCluster_ == 0) {
          if (!addCluster()) goto writeErrorReturn;
        } else {
          curCluster_ = firstCluster_;
        }
      } else {
        uint32_t next;
        if (!vol_->fatGet(curCluster_, &next)) return false;
        if (vol_->isEOC(next)) {
          if (!addCluster()) goto writeErrorReturn;
        } else {
          curCluster_ = next;
        }
      }
    }
    uint16_t n = 512 - blockOffset;

    if (n > nToWrite) n = nToWrite;

    uint32_t block = vol_->clusterStartBlock(curCluster_) + blockOfCluster;
    if (n == 512) {
      if (SdVolume::cacheBlockNumber_ == block) {
        SdVolume::cacheBlockNumber_ = 0XFFFFFFFF;
      }
      if (!vol_->writeBlock(block, src)) goto writeErrorReturn;
      src += 512;
    } else {
      if (blockOffset == 0 && curPosition_ >= fileSize_) {
        if (!SdVolume::cacheFlush()) goto writeErrorReturn;
        SdVolume::cacheBlockNumber_ = block;
        SdVolume::cacheSetDirty();
      } else {
        if (!SdVolume::cacheRawBlock(block, SdVolume::CACHE_FOR_WRITE)) {
          goto writeErrorReturn;
        }
      }
      uint8_t* dst = SdVolume::cacheBuffer_.data + blockOffset;
      uint8_t* end = dst + n;
      while (dst != end) *dst++ = *src++;
    }
    nToWrite -= n;
    curPosition_ += n;
  }
  if (curPosition_ > fileSize_) {
    fileSize_ = curPosition_;
    flags_ |= F_FILE_DIR_DIRTY;
  } else if (dateTime_ && nbyte) {
    flags_ |= F_FILE_DIR_DIRTY;
  }

  if (flags_ & O_SYNC) {
    if (!sync()) goto writeErrorReturn;
  }
  return nbyte;

 writeErrorReturn:
  setWriteError();
  return 0;
}

size_t SdFile::write(uint8_t b) {
  return write(&b, 1);
}

size_t SdFile::write(const char* str) {
  return write(str, strlen(str));
}

void SdFile::write_P(PGM_P str) {
  for (uint8_t c; (c = pgm_read_byte(str)); str++) write(c);
}

void SdFile::writeln_P(PGM_P str) {
  write_P(str);
  println();
}





