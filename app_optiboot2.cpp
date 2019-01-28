#define FUNC_READ 1
#define FUNC_WRITE 1
#define OPTIBOOT_MAJVER 6
#define OPTIBOOT_MINVER 2

#if !defined(OPTIBOOT_CUSTOMVER)
#define OPTIBOOT_CUSTOMVER 0
#endif

unsigned const int __attribute__((section(".version"))) 
optiboot_version = 256*(OPTIBOOT_MAJVER + OPTIBOOT_CUSTOMVER) + OPTIBOOT_MINVER;


#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>

#ifndef _AVR_BOOT_H_
#define _AVR_BOOT_H_    1

#include <limits.h>

#if defined (SPMCSR)
#  define __SPM_REG    SPMCSR
#elif defined (SPMCR)
#  define __SPM_REG    SPMCR
#else
#  error AVR processor does not provide bootloader support!
#endif

#if defined(SPMEN)
#  define __SPM_ENABLE  SPMEN
#elif defined(SELFPRGEN)
#  define __SPM_ENABLE  SELFPRGEN
#else
#  error Cannot find SPM Enable bit definition!
#endif

#define BOOTLOADER_SECTION    __attribute__ ((section (".bootloader")))

/* Create common bit definitions. */
#ifdef ASB
#define __COMMON_ASB    ASB
#else
#define __COMMON_ASB    RWWSB
#endif

#ifdef ASRE
#define __COMMON_ASRE   ASRE
#else
#define __COMMON_ASRE   RWWSRE
#endif

/* Define the bit positions of the Boot Lock Bits. */

#define BLB12           5
#define BLB11           4
#define BLB02           3
#define BLB01           2

/** \ingroup avr_boot
    \def boot_spm_interrupt_enable()
    Enable the SPM interrupt. */

#define boot_spm_interrupt_enable()   (__SPM_REG |= (uint8_t)_BV(SPMIE))

/** \ingroup avr_boot
    \def boot_spm_interrupt_disable()
    Disable the SPM interrupt. */

#define boot_spm_interrupt_disable()  (__SPM_REG &= (uint8_t)~_BV(SPMIE))
#define boot_is_spm_interrupt()       (__SPM_REG & (uint8_t)_BV(SPMIE))
#define boot_rww_busy()          (__SPM_REG & (uint8_t)_BV(__COMMON_ASB))
#define boot_spm_busy()               (__SPM_REG & (uint8_t)_BV(__SPM_ENABLE))

/** \ingroup avr_boot
    \def boot_spm_busy_wait()
    Wait while the SPM instruction is busy. */

#define boot_spm_busy_wait()          do{}while(boot_spm_busy())

#define __BOOT_PAGE_ERASE         (_BV(__SPM_ENABLE) | _BV(PGERS))
#define __BOOT_PAGE_WRITE         (_BV(__SPM_ENABLE) | _BV(PGWRT))
#define __BOOT_PAGE_FILL          _BV(__SPM_ENABLE)
#define __BOOT_RWW_ENABLE         (_BV(__SPM_ENABLE) | _BV(__COMMON_ASRE))
#define __BOOT_LOCK_BITS_SET      (_BV(__SPM_ENABLE) | _BV(BLBSET))

#define __boot_page_fill_short(address, data)   \
(__extension__({                                 \
    __asm__ __volatile__                         \
    (                                            \
        "movw  r0, %3\n\t"                       \
        "out %0, %1\n\t"                         \
        "spm\n\t"                                \
        "clr  r1\n\t"                            \
        :                                        \
        : "i" (_SFR_IO_ADDR(__SPM_REG)),        \
          "r" ((uint8_t)__BOOT_PAGE_FILL),       \
          "z" ((uint16_t)address),               \
          "r" ((uint16_t)data)                   \
        : "r0"                                   \
    );                                           \
}))

#define __boot_page_erase_short(address)        \
(__extension__({                                 \
    __asm__ __volatile__                         \
    (                                            \
        "out %0, %1\n\t"                         \
        "spm\n\t"                                \
        :                                        \
        : "i" (_SFR_IO_ADDR(__SPM_REG)),        \
          "r" ((uint8_t)__BOOT_PAGE_ERASE),      \
          "z" ((uint16_t)address)                \
    );                                           \
}))

#define __boot_page_write_short(address)        \
(__extension__({                                 \
    __asm__ __volatile__                         \
    (                                            \
        "out %0, %1\n\t"                         \
        "spm\n\t"                                \
        :                                        \
        : "i" (_SFR_IO_ADDR(__SPM_REG)),        \
          "r" ((uint8_t)__BOOT_PAGE_WRITE),      \
          "z" ((uint16_t)address)                \
    );                                           \
}))

#define __boot_rww_enable_short()                      \
(__extension__({                                 \
    __asm__ __volatile__                         \
    (                                            \
        "out %0, %1\n\t"                         \
        "spm\n\t"                                \
        :                                        \
        : "i" (_SFR_IO_ADDR(__SPM_REG)),        \
          "r" ((uint8_t)__BOOT_RWW_ENABLE)       \
    );                                           \
}))

#define __boot_lock_bits_set_short(lock_bits)                    \
(__extension__({                                           \
    uint8_t value = (uint8_t)(~(lock_bits));               \
    __asm__ __volatile__                                   \
    (                                                      \
        "ldi r30, 1\n\t"                                   \
        "ldi r31, 0\n\t"                                   \
        "mov r0, %2\n\t"                                   \
        "out %0, %1\n\t"                                   \
        "spm\n\t"                                          \
        :                                                  \
        : "i" (_SFR_IO_ADDR(__SPM_REG)),                  \
          "r" ((uint8_t)__BOOT_LOCK_BITS_SET),             \
          "r" (value)                                      \
        : "r0", "r30", "r31"                               \
    );                                                     \
}))

#define GET_LOW_FUSE_BITS           (0x0000)
#define GET_LOCK_BITS               (0x0001)
#define GET_EXTENDED_FUSE_BITS      (0x0002)

#define GET_HIGH_FUSE_BITS          (0x0003)

#define boot_lock_fuse_bits_get_short(address)                   \
(__extension__({                                           \
    uint8_t __result;                                      \
    __asm__ __volatile__                                   \
    (                                                      \
        "ldi r30, %3\n\t"                                  \
        "ldi r31, 0\n\t"                                   \
        "out %1, %2\n\t"                                   \
        "lpm %0, Z\n\t"                                    \
        : "=r" (__result)                                  \
        : "i" (_SFR_IO_ADDR(__SPM_REG)),                  \
          "r" ((uint8_t)__BOOT_LOCK_BITS_SET),             \
          "M" (address)                                    \
        : "r0", "r30", "r31"                               \
    );                                                     \
    __result;                                              \
}))

#define __boot_rww_enable_short()                      \
(__extension__({                                 \
    __asm__ __volatile__                         \
    (                                            \
        "out %0, %1\n\t"                         \
        "spm\n\t"                                \
        :                                        \
        : "i" (_SFR_IO_ADDR(__SPM_REG)),        \
          "r" ((uint8_t)__BOOT_RWW_ENABLE)       \
    );                                           \
}))

#define boot_rww_enable() __boot_rww_enable_short()
#define __BOOT_SIGROW_READ (_BV(__SPM_ENABLE) | _BV(SIGRD))
#endif

#ifndef LED_START_FLASHES
#define LED_START_FLASHES 0
#endif

#define WATCHDOG_OFF    (0)
#define WATCHDOG_16MS   (_BV(WDE))
#define WATCHDOG_32MS   (_BV(WDP0) | _BV(WDE))
#define WATCHDOG_64MS   (_BV(WDP1) | _BV(WDE))
#define WATCHDOG_125MS  (_BV(WDP1) | _BV(WDP0) | _BV(WDE))
#define WATCHDOG_250MS  (_BV(WDP2) | _BV(WDE))
#define WATCHDOG_500MS  (_BV(WDP2) | _BV(WDP0) | _BV(WDE))
#define WATCHDOG_1S     (_BV(WDP2) | _BV(WDP1) | _BV(WDE))
#define WATCHDOG_2S     (_BV(WDP2) | _BV(WDP1) | _BV(WDP0) | _BV(WDE))
#ifndef __AVR_ATmega8__
#define WATCHDOG_4S     (_BV(WDP3) | _BV(WDE))
#define WATCHDOG_8S     (_BV(WDP3) | _BV(WDP0) | _BV(WDE))
#endif


#if SPM_PAGESIZE > 255
typedef uint16_t pagelen_t ;
#define GETLENGTH(len) len = getch()<<8; len |= getch()
#else
typedef uint8_t pagelen_t;
#define GETLENGTH(len) (void) getch() /* skip high byte */; len = getch()
#endif

int main(void) __attribute__ ((OS_main)) __attribute__ ((section (".init9")));
uint8_t __attribute__((noinline)) getch(void);
void __attribute__((noinline)) verifySpace();

static void watchdogConfig(uint8_t x)
{
    WDTCSR = 1<<WDCE | 1<<WDE;
    WDTCSR = x;
}

#if LED_START_FLASHES > 0
static inline void flash_led(uint8_t);
#endif
static inline void watchdogReset();
static inline void writebuffer(int8_t memtype, uint8_t *mybuff, uint16_t address, pagelen_t len);
static inline void read_mem(uint8_t memtype, uint16_t address, pagelen_t len);
void appStart(uint8_t rstFlags) __attribute__ ((naked));

#if !defined(RAMSTART)  // newer versions of gcc avr-libc define RAMSTART
#define RAMSTART 0x100
#if defined (__AVR_ATmega644P__)
#undef SIGNATURE_2
#define SIGNATURE_2 0x0A
#elif defined(__AVR_ATmega1280__)
#undef RAMSTART
#define RAMSTART (0x200)
#endif
#endif

#define buff    ((uint8_t*)(RAMSTART))

#define STK_OK 0x10
#define STK_INSYNC 0x14
#define STK_GET_PARAMETER 0x41
#define STK_SW_MINOR 0x82
#define STK_SW_MAJOR 0x81
#define STK_SET_DEVICE 0x42
#define STK_SET_DEVICE_EXT 0x45
#define STK_LOAD_ADDRESS 0x55
#define STK_UNIVERSAL 0x56
#define STK_PROG_PAGE 0x64
#define STK_READ_PAGE 0x74
#define STK_READ_SIGN 0x75
#define STK_LEAVE_PROGMODE 0x51
#define CRC_EOP 0x20

static void putch(char ch)
{
    while ((UCSR0A & 1<<UDRE0) == 0);
    UDR0 = ch;
}

static void getNch(uint8_t count)
{
    do getch(); while (--count);
    verifySpace();
}

int main()
{
    uint8_t ch;
    register uint16_t address = 0;
    register pagelen_t  length;
    asm volatile ("clr __zero_reg__");
#if defined(__AVR_ATmega8__) || defined (__AVR_ATmega32__) || defined (__AVR_ATmega16__)
    SP=RAMEND;  // This is done by hardware reset
#endif
    ch = MCUSR;
    MCUSR = 0;

    if (ch & (_BV(WDRF) | _BV(BORF) | _BV(PORF)))
        appStart(ch);
#if LED_START_FLASHES > 0
    TCCR1B = 1<<CS12 | 1<<CS10; // div 1024
#endif
    UCSR0A = 1<<U2X0; //Double speed mode USART0
    UCSR0B = 1<<RXEN0 | 1<<TXEN0;
    UCSR0C = 1<<UCSZ00 | 1<<UCSZ01;
    UBRR0 = 16;
    watchdogConfig(WATCHDOG_1S);
#if (LED_START_FLASHES > 0) || defined(LED_DATA_FLASH)
    LED_DDR |= _BV(LED);
#endif

#if LED_START_FLASHES > 0
    flash_led(LED_START_FLASHES * 2);
#endif

    for (;;)
    {
        ch = getch();

        if(ch == STK_GET_PARAMETER)
        {
            unsigned char which = getch();
            verifySpace();

            if (which == STK_SW_MINOR) {
	            putch(optiboot_version & 0xFF);
            } else if (which == STK_SW_MAJOR) {
	            putch(optiboot_version >> 8);
            } else {
	            putch(0x03);
            }
        }
        else if(ch == STK_SET_DEVICE) {
            getNch(20);
        }
    else if(ch == STK_SET_DEVICE_EXT) {
      getNch(5);
    }
    else if(ch == STK_LOAD_ADDRESS) {
      uint16_t newAddress;
      newAddress = getch();
      newAddress = (newAddress & 0xff) | (getch() << 8);
#ifdef RAMPZ
      RAMPZ = (newAddress & 0x8000) ? 1 : 0;
#endif
      newAddress += newAddress; // Convert from word address to byte address
      address = newAddress;
      verifySpace();
    }
    else if(ch == STK_UNIVERSAL)
    {
        getNch(4);
        putch(0x00);
    }
    else if(ch == STK_PROG_PAGE)
    {
        uint8_t desttype;
        uint8_t *bufPtr;
        pagelen_t savelength;
        GETLENGTH(length);
        savelength = length;
        desttype = getch();
        bufPtr = buff;
        do *bufPtr++ = getch();
        while (--length);
        verifySpace();
        writebuffer(desttype, buff, address, savelength);
    }
    else if(ch == STK_READ_PAGE)
    {
        uint8_t desttype;
        GETLENGTH(length);
        desttype = getch();
        verifySpace();
        read_mem(desttype, address, length);
    }
    else if(ch == STK_READ_SIGN)
    {
        verifySpace();
        putch(SIGNATURE_0);
        putch(SIGNATURE_1);
        putch(SIGNATURE_2);
    }
    else if (ch == STK_LEAVE_PROGMODE)
    { /* 'Q' */
        watchdogConfig(WATCHDOG_16MS);
        verifySpace();
    }
    else
    {
        verifySpace();
    }
    putch(STK_OK);
    }
}

uint8_t getch(void)
{
    uint8_t ch;

#ifdef LED_DATA_FLASH
#if defined(__AVR_ATmega8__) || defined (__AVR_ATmega32__) || defined (__AVR_ATmega16__)
    LED_PORT ^= _BV(LED);
#else
    LED_PIN |= _BV(LED);
#endif
#endif
    while ((UCSR0A & 1<<RXC0) == 0);

    if ((UCSR0A & 1<<FE0) == 0)
        watchdogReset();

    ch = UDR0;

#ifdef LED_DATA_FLASH
#if defined(__AVR_ATmega8__) || defined (__AVR_ATmega32__) || defined (__AVR_ATmega16__)
  LED_PORT ^= _BV(LED);
#else
  LED_PIN |= _BV(LED);
#endif
#endif

  return ch;
}

void verifySpace()
{
    if (getch() != CRC_EOP) {
        watchdogConfig(WATCHDOG_16MS);    // shorten WD timeout
        while (1)			      // and busy-loop so that WD causes
            ; 				      //  a reset and app start.
    }
    putch(STK_INSYNC);
}

#if LED_START_FLASHES > 0
void flash_led(uint8_t count) {
  do {
    TCNT1 = -(F_CPU/(1024*16));
    TIFR1 = _BV(TOV1);
    while(!(TIFR1 & _BV(TOV1)));
#if defined(__AVR_ATmega8__) || defined (__AVR_ATmega32__) || defined (__AVR_ATmega16__)
    LED_PORT ^= _BV(LED);
#else
    LED_PIN |= _BV(LED);
#endif
    watchdogReset();
  } while (--count);
}
#endif

void watchdogReset() {
  __asm__ __volatile__ (
    "wdr\n"
  );
}

void appStart(uint8_t rstFlags) {
    __asm__ __volatile__ ("mov r2, %0\n" :: "r" (rstFlags));
    watchdogConfig(WATCHDOG_OFF);
    __asm__ __volatile__ (
    "ldi r30,%[rstvec]\n"
    "clr r31\n"
    "ijmp\n"::[rstvec] "M"(appstart_vec)
  );
}

static inline void writebuffer(int8_t memtype, uint8_t *mybuff,
			       uint16_t address, pagelen_t len)
{
    switch (memtype) {
    case 'E': // EEPROM
#if defined(SUPPORT_EEPROM) || defined(BIGBOOT)
        while(len--) {
	    eeprom_write_byte((uint8_t *)(address++), *mybuff++);
        }
#else
	while (1)
	    ; // Error: wait for WDT
#endif
	break;
    default:  // FLASH
	{
	    uint8_t *bufPtr = mybuff;
	    uint16_t addrPtr = (uint16_t)(void*)address;
	    __boot_page_erase_short((uint16_t)(void*)address);
	    boot_spm_busy_wait();

	    do
        {
		    uint16_t a;
		    a = *bufPtr++;
		    a |= (*bufPtr++) << 8;
		    __boot_page_fill_short((uint16_t)(void*)addrPtr,a);
		    addrPtr += 2;
	    } while (len -= 2);

	    __boot_page_write_short((uint16_t)(void*)address);
	    boot_spm_busy_wait();
#if defined(RWWSRE)
	    boot_rww_enable();
#endif
	} // default block
	break;
    } // switch
}

static inline void read_mem(uint8_t memtype, uint16_t address, pagelen_t length)
{
    uint8_t ch;

    switch (memtype) {

#if defined(SUPPORT_EEPROM) || defined(BIGBOOT)
    case 'E': // EEPROM
	do {
	    putch(eeprom_read_byte((uint8_t *)(address++)));
	} while (--length);
	break;
#endif
    default:
	do {
	    __asm__ ("lpm %0,Z+\n" : "=r" (ch), "=z" (address): "1" (address));
	    putch(ch);
	} while (--length);
	break;
    } // switch
}


