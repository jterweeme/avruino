#ifndef _SD_H_
#define _SD_H_

#include "board.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdarg.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <avr/pgmspace.h>
#include <ctype.h>
#include <stddef.h>
#include <stdint.h>

#define __AVR_LIBC_DEPRECATED_ENABLE__
static const uint8_t SERIAL_BUFFER_SIZE = 64;

struct ring_buffer
{
    unsigned char buffer[SERIAL_BUFFER_SIZE];
    volatile unsigned int head;
    volatile unsigned int tail;
};

inline void store_char(unsigned char c, ring_buffer *buffer)
{
    unsigned i = (unsigned int)(buffer->head + 1) % SERIAL_BUFFER_SIZE;

    if (i != buffer->tail)
    {
        buffer->buffer[buffer->head] = c;
        buffer->head = i;
    }
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
static const uint8_t NO_SKIP_CHAR = 1;
static const uint8_t MAX_COMPONENT_LEN = 12;
static const uint8_t PATH_COMPONENT_BUFFER_LEN = MAX_COMPONENT_LEN + 1;
class __FlashStringHelper;
class StringSumHelper;
__extension__ typedef int __guard __attribute__((mode (__DI__)));
extern "C" int __cxa_guard_acquire(__guard *);
extern "C" void __cxa_guard_release (__guard *);
extern "C" void __cxa_guard_abort (__guard *);
extern "C" void __cxa_pure_virtual(void);
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
    size_t print(const char str[]) { return write(str); }
    size_t print(char c) { return write(c); }
    size_t print(unsigned char b, int base = DEC) { return print((unsigned long)b, base); }
    size_t print(int n, int base = DEC) { return print((long)n, base); }
    size_t print(unsigned int n, int base = DEC) { return print((unsigned long)n, base); }
    size_t print(long, int = DEC);
    size_t print(unsigned long, int = DEC);
    size_t print(double n, int digits = 2) { return printFloat(n, digits); }
    size_t println(const char[]);
    size_t println(char);
    size_t println(unsigned char, int = DEC);
    size_t println(int, int = DEC);
    size_t println(unsigned int, int = DEC);
    size_t println(long, int = DEC);
    size_t println(unsigned long, int = DEC);
    size_t println(double, int = 2);
    size_t println(void);
};

static const uint8_t HIGH = 0x1;
static const uint8_t LOW = 0x0;
static const uint8_t INPUT_PULLUP = 0x2;
#define microsecondsToClockCycles(a) ( (a) * clockCyclesPerMicrosecond() )
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))
typedef uint8_t boolean;
typedef uint8_t byte;
void pinMode(uint8_t, uint8_t);
void digitalWrite(uint8_t, uint8_t);
int digitalRead(uint8_t);
int analogRead(uint8_t);
void analogReference(uint8_t mode);
void analogWrite(uint8_t, int);
unsigned long micros(void);
unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout);
void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val);
uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder);
extern const uint16_t PROGMEM port_to_mode_PGM[];
extern const uint16_t PROGMEM port_to_input_PGM[];
extern const uint16_t PROGMEM port_to_output_PGM[];
extern const uint8_t PROGMEM digital_pin_to_port_PGM[];
extern const uint8_t PROGMEM digital_pin_to_bit_mask_PGM[];
extern const uint8_t PROGMEM digital_pin_to_timer_PGM[];
#define digitalPinToPort(P) ( pgm_read_byte( digital_pin_to_port_PGM + (P) ) )
#define digitalPinToBitMask(P) ( pgm_read_byte( digital_pin_to_bit_mask_PGM + (P) ) )
#define digitalPinToTimer(P) ( pgm_read_byte( digital_pin_to_timer_PGM + (P) ) )
#define portOutputRegister(P) ( (volatile uint8_t *)( pgm_read_word( port_to_output_PGM + (P))) )
#define portModeRegister(P) ( (volatile uint8_t *)( pgm_read_word( port_to_mode_PGM + (P))) )
static const uint8_t NOT_A_PIN = 0;
static const uint8_t NOT_A_PORT = 0;
static const uint8_t NOT_ON_TIMER = 0;
static const uint8_t TIMER0A = 1;
static const uint8_t TIMER0B = 2;
static const uint8_t TIMER1A = 3;
static const uint8_t TIMER1B = 4;
static const uint8_t TIMER2A = 6;
static const uint8_t TIMER2B = 7;

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
    void setTimeout(unsigned long timeout) { _timeout = timeout; }
    bool findUntil(char *target, char *terminator);
    bool findUntil(char *target, size_t targetLen, char *terminate, size_t termLen);
    bool find(char *target) { return findUntil(target, NULL); }
    bool find(char *target, size_t length) { return findUntil(target, length, NULL, 0); }
    long parseInt() { return parseInt(NO_SKIP_CHAR); }
    size_t readBytes(char *buffer, size_t length);
    size_t readBytesUntil( char terminator, char *buffer, size_t length);
protected:
    long parseInt(char skipChar);
};

class SerialWrapper : public DefaultUart
{
public:
    void print(const char *s) { puts(s); }
    void print(char c) { myPutc(c); }
    void println() { puts("\r\n"); }
    void println(uint32_t u) { printf("%u\r\n", u); }
    void println(const char *s) { printf("%s\r\n", s); }
};

extern void serialEventRun(void) __attribute__((weak));
uint16_t makeWord(uint16_t w);
uint16_t makeWord(byte h, byte l);
unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout = 1000000L);
static const uint8_t NUM_DIGITAL_PINS = 20;
static const uint8_t NUM_ANALOG_PINS = 6;
static const uint8_t SS = 10;
static const uint8_t MOSI = 11;
static const uint8_t MISO = 12;
static const uint8_t SCK = 13;
static const uint8_t SDA = 18;
static const uint8_t SCL = 19;
static const uint8_t LED_BUILTIN = 13;

struct pin_map_t
{
    volatile uint8_t* ddr;
    volatile uint8_t* pin;
    volatile uint8_t* port;
    uint8_t bit;
};

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
uint8_t const SDA_PIN = 18;
uint8_t const SCL_PIN = 19;
uint8_t const SS_PIN = 10;
uint8_t const MOSI_PIN = 11;
uint8_t const MISO_PIN = 12;
uint8_t const SCK_PIN = 13;

static const pin_map_t digitalPinMap[] = {
  {&DDRD, &PIND, &PORTD, 0},
  {&DDRD, &PIND, &PORTD, 1},
  {&DDRD, &PIND, &PORTD, 2},
  {&DDRD, &PIND, &PORTD, 3},
  {&DDRD, &PIND, &PORTD, 4},
  {&DDRD, &PIND, &PORTD, 5},
  {&DDRD, &PIND, &PORTD, 6},
  {&DDRD, &PIND, &PORTD, 7},
  {&DDRB, &PINB, &PORTB, 0},
  {&DDRB, &PINB, &PORTB, 1},
  {&DDRB, &PINB, &PORTB, 2},
  {&DDRB, &PINB, &PORTB, 3},
  {&DDRB, &PINB, &PORTB, 4},
  {&DDRB, &PINB, &PORTB, 5},
  {&DDRC, &PINC, &PORTC, 0},
  {&DDRC, &PINC, &PORTC, 1},
  {&DDRC, &PINC, &PORTC, 2},
  {&DDRC, &PINC, &PORTC, 3},
  {&DDRC, &PINC, &PORTC, 4},
  {&DDRC, &PINC, &PORTC, 5}
};

struct cid_t
{
    uint8_t mid;
    char old[2];
    char pnm[5];
    unsigned prv_m:4;
    unsigned prv_n:4;
    uint32_t psn;
    unsigned mdt_year_high:4;
    unsigned reserved:4;
    unsigned mdt_month:4;
    unsigned mdt_year_low:4;
    unsigned always1:1;
    unsigned crc:7;
} __attribute__ ((packed));

typedef struct CSDV1
{
    unsigned reserved1 : 6;
    unsigned csd_ver : 2;
    uint8_t taac;
    uint8_t nsac;
    uint8_t tran_speed;
    uint8_t ccc_high;
    unsigned read_bl_len : 4;
    unsigned ccc_low : 4;
    unsigned c_size_high : 2;
    unsigned reserved2 : 2;
    unsigned dsr_imp : 1;
    unsigned read_blk_misalign :1;
    unsigned write_blk_misalign : 1;
    unsigned read_bl_partial : 1;
    uint8_t c_size_mid;
    unsigned vdd_r_curr_max : 3;
    unsigned vdd_r_curr_min : 3;
    unsigned c_size_low :2;
    unsigned c_size_mult_high : 2;
    unsigned vdd_w_cur_max : 3;
    unsigned vdd_w_curr_min : 3;
    unsigned sector_size_high : 6;
    unsigned erase_blk_en : 1;
    unsigned c_size_mult_low : 1;
    unsigned wp_grp_size : 7;
    unsigned sector_size_low : 1;
    unsigned write_bl_len_high : 2;
    unsigned r2w_factor : 3;
    unsigned reserved3 : 2;
    unsigned wp_grp_enable : 1;
    unsigned write_partial : 1;
    unsigned write_bl_len_low : 2;
    unsigned reserved5: 2;
    unsigned file_format : 2;
    unsigned tmp_write_protect : 1;
    unsigned perm_write_protect : 1;
    unsigned copy : 1;
    unsigned file_format_grp : 1;
    unsigned always1 : 1;
    unsigned crc : 7;
} csd1_t;


typedef struct CSDV2
{
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

union csd_t
{
    csd1_t v1;
    csd2_t v2;
};


static uint8_t const SD_CHIP_SELECT_PIN = SS_PIN;
static uint8_t const SPI_FULL_SPEED = 0;
static uint8_t const SPI_HALF_SPEED = 1;
static uint8_t const SPI_QUARTER_SPEED = 2;
static uint8_t const SPI_MOSI_PIN = MOSI_PIN;
static uint8_t const SPI_MISO_PIN = MISO_PIN;
static uint8_t const SPI_SCK_PIN = SCK_PIN;
static uint8_t const SD_CARD_TYPE_SD1 = 1;
static uint8_t const SD_CARD_TYPE_SD2 = 2;
static uint8_t const SD_CARD_TYPE_SDHC = 3;

class Sd2Card
{
    static uint16_t const SD_INIT_TIMEOUT = 2000;
    static uint16_t const SD_ERASE_TIMEOUT = 10000;
    static uint16_t const SD_READ_TIMEOUT = 300;
    static uint16_t const SD_WRITE_TIMEOUT = 600;
    static uint8_t const SD_CARD_ERROR_CMD0 = 0X1;
    static uint8_t const SD_CARD_ERROR_CMD8 = 0X2;
    static uint8_t const SD_CARD_ERROR_CMD17 = 0X3;
    static uint8_t const SD_CARD_ERROR_CMD24 = 0X4;
    static uint8_t const SD_CARD_ERROR_CMD25 = 0X05;
    static uint8_t const SD_CARD_ERROR_CMD58 = 0X06;
    static uint8_t const SD_CARD_ERROR_ACMD23 = 0X07;
    static uint8_t const SD_CARD_ERROR_ACMD41 = 0X08;
    static uint8_t const SD_CARD_ERROR_BAD_CSD = 0X09;
    static uint8_t const SD_CARD_ERROR_ERASE = 0X0A;
    static uint8_t const SD_CARD_ERROR_ERASE_SINGLE_BLOCK = 0X0B;
    static uint8_t const SD_CARD_ERROR_ERASE_TIMEOUT = 0X0C;
    static uint8_t const SD_CARD_ERROR_READ = 0X0D;
    static uint8_t const SD_CARD_ERROR_READ_REG = 0X0E;
    static uint8_t const SD_CARD_ERROR_READ_TIMEOUT = 0X0F;
    static uint8_t const SD_CARD_ERROR_STOP_TRAN = 0X10;
    static uint8_t const SD_CARD_ERROR_WRITE = 0X11;
    static uint8_t const SD_CARD_ERROR_WRITE_BLOCK_ZERO = 0X12;
    static uint8_t const SD_CARD_ERROR_WRITE_MULTIPLE = 0X13;
    static uint8_t const SD_CARD_ERROR_WRITE_PROGRAMMING = 0X14;
    static uint8_t const SD_CARD_ERROR_WRITE_TIMEOUT = 0X15;
    static uint8_t const SD_CARD_ERROR_SCK_RATE = 0X16;
    uint32_t block_;
    uint8_t chipSelectPin_;
    uint8_t errorCode_;
    uint8_t inBlock_;
    uint16_t offset_;
    uint8_t partialBlockRead_;
    uint8_t status_;
    uint8_t type_;
    PortB portB;
    PortD portD;
    Pin cs;
public:
    Sd2Card() : errorCode_(0), inBlock_(0), partialBlockRead_(0), type_(0), cs(portD, BIT4) {}
    uint32_t cardSize();
    uint8_t erase(uint32_t firstBlock, uint32_t lastBlock);
    uint8_t eraseSingleBlockEnable(void);
    uint8_t errorCode() const {return errorCode_;}
    uint8_t errorData() const {return status_;}
    uint8_t init() { return init(SPI_FULL_SPEED, SD_CHIP_SELECT_PIN); }
    uint8_t init(uint8_t sckRateID) {    return init(sckRateID, SD_CHIP_SELECT_PIN);  }
    uint8_t init(uint8_t sckRateID, uint8_t chipSelectPin);
    void partialBlockRead(uint8_t value);
    uint8_t partialBlockRead() const {return partialBlockRead_;}
    uint8_t readBlock(uint32_t block, uint8_t* dst) { return readData(block, 0, 512, dst); }
    uint8_t readData(uint32_t block, uint16_t offset, uint16_t count, uint8_t* dst);
    uint8_t readCID(cid_t* cid) { return readRegister(CMD10, cid); }
    uint8_t readCSD(csd_t* csd) { return readRegister(CMD9, csd); }
    void readEnd();
    uint8_t setSckRate(uint8_t sckRateID);
    uint8_t type(void) const {return type_;}
    uint8_t writeBlock(uint32_t blockNumber, const uint8_t* src);
    uint8_t writeData(const uint8_t* src);
    uint8_t writeStart(uint32_t blockNumber, uint32_t eraseCount);
    uint8_t writeStop(void);
private:
    uint8_t cardAcmd(uint8_t cmd, uint32_t arg)
    {   cardCommand(CMD55, 0);return cardCommand(cmd, arg);
    }

    uint8_t cardCommand(uint8_t cmd, uint32_t arg);
    void error(uint8_t code) {errorCode_ = code;}
    uint8_t readRegister(uint8_t cmd, void* buf);
    uint8_t sendWriteCommand(uint32_t blockNumber, uint32_t eraseCount);
    void inline chipSelectHigh() { cs.set(); }
    void inline chipSelectLow() { cs.clear(); }
    void type(uint8_t value) {type_ = value;}
    uint8_t waitNotBusy(uint16_t timeoutMillis);
    uint8_t writeData(uint8_t token, const uint8_t* src);
    uint8_t waitStartBlock();
};

uint8_t const BOOTSIG0 = 0X55;
uint8_t const BOOTSIG1 = 0XAA;

struct partitionTable
{
    uint8_t boot;
    uint8_t beginHead;
    unsigned beginSector : 6;
    unsigned beginCylinderHigh : 2;
    uint8_t beginCylinderLow;
    uint8_t type;
    uint8_t endHead;
    unsigned endSector : 6;
    unsigned endCylinderHigh : 2;
    uint8_t endCylinderLow;
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
    uint8_t jmpToBootCode[3];
    char oemName[8];
    bpb_t bpb;
    uint8_t driveNumber;
    uint8_t reserved1;
    uint8_t bootSignature;
    uint32_t volumeSerialNumber;
    char volumeLabel[11];
    char fileSystemType[8];
    uint8_t bootCode[420];
    uint8_t bootSectorSig0;
    uint8_t bootSectorSig1;
};
uint16_t const FAT16EOC = 0XFFFF;
uint16_t const FAT16EOC_MIN = 0XFFF8;
uint32_t const FAT32EOC = 0X0FFFFFFF;
uint32_t const FAT32EOC_MIN = 0X0FFFFFF8;
uint32_t const FAT32MASK = 0X0FFFFFFF;
typedef struct fat32BootSector fbs_t;

struct directoryEntry
{
    uint8_t name[11];
    uint8_t attributes;
    uint8_t reservedNT;
    uint8_t creationTimeTenths;
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

union cache_t
{
    uint8_t data[512];
    uint16_t fat16[256];
    uint32_t fat32[128];
    dir_t dir[16];
    mbr_t mbr;
    fbs_t fbs;
};

class SdVolume;

class SdFile : public Print
{
    static void oldToNew(uint16_t* date, uint16_t* time);
    static uint8_t const F_OFLAG = (O_ACCMODE | O_APPEND | O_SYNC);
    static uint8_t const F_UNUSED = 0X30;
    static uint8_t const F_FILE_UNBUFFERED_READ = 0X40;
    static uint8_t const F_FILE_DIR_DIRTY = 0X80;
    uint8_t flags_;
    uint8_t type_;
    uint32_t curCluster_;
    uint32_t curPosition_;
    uint32_t dirBlock_;
    uint8_t dirIndex_;
    uint32_t fileSize_;
    uint32_t firstCluster_;
    SdVolume* vol_;
    uint8_t addCluster(void);
    uint8_t addDirCluster(void);
    dir_t* cacheDirEntry(uint8_t action);
    static void (*dateTime_)(uint16_t* date, uint16_t* time);
    static uint8_t make83Name(const char* str, uint8_t* name);
    uint8_t openCachedEntry(uint8_t cacheIndex, uint8_t oflags);
    dir_t* readDirCache(void);
    static void (*oldDateTime_)(uint16_t& date, uint16_t& time);
public:
    static inline uint16_t FAT_DATE(uint16_t year, uint8_t month, uint8_t day);
    static inline uint16_t FAT_YEAR(uint16_t fatDate) { return 1980 + (fatDate >> 9); }
    static inline uint8_t FAT_MONTH(uint16_t fatDate) { return (fatDate >> 5) & 0XF; }
    static inline uint8_t FAT_DAY(uint16_t fatDate) {  return fatDate & 0X1F; }

    static inline uint16_t FAT_TIME(uint8_t hour, uint8_t minute, uint8_t second) {
        return hour << 11 | minute << 5 | second >> 1;
    }

    static inline uint8_t FAT_HOUR(uint16_t fatTime) { return fatTime >> 11; }
    static inline uint8_t FAT_MINUTE(uint16_t fatTime) { return(fatTime >> 5) & 0X3F; }
    static inline uint8_t FAT_SECOND(uint16_t fatTime) { return 2*(fatTime & 0X1F); }
    uint16_t const FAT_DEFAULT_DATE = ((2000 - 1980) << 9) | (1 << 5) | 1;
    uint16_t const FAT_DEFAULT_TIME = (1 << 11);
    SdFile(void) : type_(FAT_FILE_TYPE_CLOSED) {}
    void clearUnbufferedRead(void) { flags_ &= ~F_FILE_UNBUFFERED_READ; }
    uint8_t close(void);
    uint8_t contiguousRange(uint32_t* bgnBlock, uint32_t* endBlock);
    uint8_t createContiguous(SdFile* dirFile, const char* fileName, uint32_t size);
    uint32_t curCluster(void) const {return curCluster_;}
    uint32_t curPosition(void) const {return curPosition_;}
    static void dateTimeCallback(void (*dateTime)(uint16_t* date, uint16_t* time));
    static void dateTimeCallbackCancel(void) {    dateTime_ = 0; }
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

    void ls(uint8_t flags, uint8_t indent, SerialWrapper &Serial);
    uint8_t makeDir(SdFile* dir, const char* dirName);
    uint8_t open(SdFile* dirFile, uint16_t index, uint8_t oflag);
    uint8_t open(SdFile* dirFile, const char* fileName, uint8_t oflag);
    uint8_t openRoot(SdVolume* vol);
    static void printDirName(const dir_t& dir, uint8_t width, SerialWrapper &Serial);
    static void printFatDate(uint16_t fatDate, SerialWrapper &Serial);
    static void printFatTime(uint16_t fatTime, SerialWrapper &Serial);
    static void printTwoDigits(uint8_t v, SerialWrapper &Serial);
    int16_t read(void) { uint8_t b;    return read(&b, 1) == 1 ? b : -1;  }
    int16_t read(void* buf, uint16_t nbyte);
    int8_t readDir(dir_t* dir);
    static uint8_t remove(SdFile* dirFile, const char* fileName);
    uint8_t remove(void);
    void rewind(void) {  curPosition_ = curCluster_ = 0;  }
    uint8_t rmDir(void);
    uint8_t rmRfStar(void);
    uint8_t seekCur(uint32_t pos) { return seekSet(curPosition_ + pos);  }
    uint8_t seekEnd(void) {return seekSet(fileSize_);}
    uint8_t seekSet(uint32_t pos);
    void setUnbufferedRead(void) { if (isFile()) flags_ |= F_FILE_UNBUFFERED_READ;  }

    uint8_t timestamp(uint8_t flag, uint16_t year, uint8_t month, uint8_t day,
          uint8_t hour, uint8_t minute, uint8_t second);

    uint8_t sync(void);
    uint8_t type(void) const {return type_;}
    uint8_t truncate(uint32_t size);
    uint8_t unbufferedRead(void) const { return flags_ & F_FILE_UNBUFFERED_READ;  }
    SdVolume* volume(void) const {return vol_;}
    size_t write(uint8_t b) { return write(&b, 1); }
    size_t write(const void* buf, uint16_t nbyte);
    size_t write(const char* str) { return write(str, strlen(str)); }
    void write_P(PGM_P str) { for (uint8_t c; (c = pgm_read_byte(str)); str++) write(c); }
    void writeln_P(PGM_P str) { write_P(str); println(); }

    uint8_t contiguousRange(uint32_t& bgnBlock, uint32_t& endBlock) {
        return contiguousRange(&bgnBlock, &endBlock);
    }

    uint8_t createContiguous(SdFile& dirFile, const char* fileName, uint32_t size) {
        return createContiguous(&dirFile, fileName, size);
    }

    static void dateTimeCallback(void (*dateTime)(uint16_t& date, uint16_t& time))
    {
        oldDateTime_ = dateTime;
        dateTime_ = dateTime ? oldToNew : 0;
    }

    uint8_t dirEntry(dir_t& dir) {return dirEntry(&dir);}
    uint8_t makeDir(SdFile& dir, const char* dirName) { return makeDir(&dir, dirName);  }

    uint8_t open(SdFile& dirFile,  const char* fileName, uint8_t oflag)
    {
        return open(&dirFile, fileName, oflag);
    }

    uint8_t open(SdFile& dirFile, const char* fileName) {
        return open(dirFile, fileName, O_RDWR);
    }

    uint8_t open(SdFile& dirFile, uint16_t index, uint8_t oflag) {
        return open(&dirFile, index, oflag);
    }

    uint8_t openRoot(SdVolume& vol) { return openRoot(&vol); }
    int8_t readDir(dir_t& dir) { return readDir(&dir); }
    static uint8_t remove(SdFile& dirFile, const char* fileName);
};

class SdVolume
{
public:
    SdVolume(void) :allocSearchStart_(2), fatType_(0) {}

    static uint8_t* cacheClear(void)
    {
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
    uint8_t init(Sd2Card& dev) {return init(&dev);}
    uint8_t init(Sd2Card& dev, uint8_t part) {   return init(&dev, part); }
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
    uint8_t fatPutEOC(uint32_t cluster) { return fatPut(cluster, 0x0FFFFFFF); }
    uint8_t freeChain(uint32_t cluster);
    uint8_t isEOC(uint32_t cluster) const {
        return  cluster >= (fatType_ == 16 ? FAT16EOC_MIN : FAT32EOC_MIN);
    }
    uint8_t readBlock(uint32_t block, uint8_t* dst) { return sdCard_->readBlock(block, dst);}
    uint8_t readData(uint32_t block, uint16_t offset,
    uint16_t count, uint8_t* dst) {    return sdCard_->readData(block, offset, count, dst); }

    uint8_t writeBlock(uint32_t block, const uint8_t* dst) {
        return sdCard_->writeBlock(block, dst);
    }
};


static const uint8_t FILE_WRITE = O_READ | O_WRITE | O_CREAT;

class File : public Stream
{
    char _name[13];
    SdFile *_file;
public:
    File(SdFile f, const char *name);
    File() { _file = 0; _name[0] = 0; }
    ~File() { }
    virtual size_t write(uint8_t val) { return write(&val, 1); }
    virtual size_t write(const uint8_t *buf, size_t size);
    virtual int read();
    virtual int peek();
    virtual int available();
    virtual void flush() { if (_file) _file->sync(); }
    int read(void *buf, uint16_t nbyte);
    uint8_t seek(uint32_t pos);
    uint32_t position();
    uint32_t size();
    void close();
    operator bool();
    char *name() { return _name; }
    uint8_t isDirectory(void);
    File openNextFile(uint8_t mode = O_RDONLY);
    void rewindDirectory() { if (isDirectory()) _file->rewind(); }
    using Print::write;
};



class SDClass
{
    Sd2Card card;
    SdVolume volume;
    SdFile root;
    SdFile getParentDir(const char *filepath, int *indx);
    static const uint8_t FILE_READ = O_READ;
    static bool getNextPathComponent(char *path, unsigned int *p_offset, char *buffer);
public:
    static uint8_t walkPath(char *filepath, SdFile& parentDir,
            boolean (*callback)(SdFile& parentDir,
            char *filePathComponent, boolean isLastComponent, void *object), void *object = NULL);

    static uint8_t callback_remove(SdFile& parentDir, char *filePathComponent,
            boolean isLastComponent, void *object)
    {
        if (isLastComponent)
            return SdFile::remove(parentDir, filePathComponent);
        
        return true;
    }

    static uint8_t callback_rmdir(SdFile& parentDir, char *filePathComponent,
        uint8_t isLastComponent, void *object)
    {
        if (isLastComponent)
        {
        SdFile f;

        if (!f.open(parentDir, filePathComponent, O_READ))
            return false;

        return f.rmDir();
        }
        return true;
    }


    static uint8_t callback_pathExists(SdFile& parentDir, char *filePathComponent,
                boolean isLastComponent, void *object)
    {
    SdFile child;
    boolean exists = child.open(parentDir, filePathComponent, O_RDONLY);

    if (exists)
        child.close();

    return exists;
    }

    static uint8_t callback_makeDirPath(SdFile& parentDir, char *filePathComponent,
                 boolean isLastComponent, void *object)
    {
    boolean result = false;
    SdFile child;
    result = callback_pathExists(parentDir, filePathComponent, isLastComponent, object);

    if (!result)
        result = child.makeDir(parentDir, filePathComponent);

    return result;
    }
    uint8_t begin(uint8_t csPin = SD_CHIP_SELECT_PIN);
    File open(const char *filename, uint8_t mode = FILE_READ);
    uint8_t exists(char *filepath) { return walkPath(filepath, root, callback_pathExists); }
    uint8_t mkdir(char *filepath) { return walkPath(filepath, root, callback_makeDirPath); }
    uint8_t remove(char *filepath) { return walkPath(filepath, root, callback_remove); }
    uint8_t rmdir(char *filepath) { return walkPath(filepath, root, callback_rmdir); }
private:
    int fileOpenMode;
    friend class File;
    friend uint8_t callback_openPath(SdFile&, char *, uint8_t, void *);
};

typedef void (*voidFuncPtr)(void);

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


static const uint8_t digitalPinCount = sizeof(digitalPinMap)/sizeof(pin_map_t);

uint8_t badPinNumber(void) __attribute__((error("Pin number is too large or not a constant")));

static inline __attribute__((always_inline)) void setPinMode(uint8_t pin, uint8_t mode) {
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


#endif


