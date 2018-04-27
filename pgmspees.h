#ifndef __PGMSPACE_H_
#define __PGMSPACE_H_ 1

#ifndef __DOXYGEN__
#define __need_size_t
#endif

#include "types.h"
#ifndef __ATTR_CONST__
#define __ATTR_CONST__ __attribute__((__const__))
#endif

#ifndef __ATTR_PROGMEM__
#define __ATTR_PROGMEM__ __attribute__((__progmem__))
#endif

#ifndef __ATTR_PURE__
#define __ATTR_PURE__ __attribute__((__pure__))
#endif

#define PROGMEM __ATTR_PROGMEM__

#define PGM_P const char *
#define PGM_VOID_P const void *




# define PSTR(s) (__extension__({static const char __c[] PROGMEM = (s); &__c[0];}))

#ifndef __DOXYGEN__ /* Internal macros, not documented. */
#define __LPM_classic__(addr)   \
(__extension__({                \
    uint16_t __addr16 = (uint16_t)(addr); \
    uint8_t __result;           \
    __asm__ __volatile__        \
    (                           \
        "lpm" "\n\t"            \
        "mov %0, r0" "\n\t"     \
        : "=r" (__result)       \
        : "z" (__addr16)        \
        : "r0"                  \
    );                          \
    __result;                   \
}))

#define __LPM_tiny__(addr)      \
(__extension__({                \
    uint16_t __addr16 = (uint16_t)(addr) + __AVR_TINY_PM_BASE_ADDRESS__; \
    uint8_t __result;           \
    __asm__                     \
    (                           \
        "ld %0, z" "\n\t"       \
        : "=r" (__result)       \
        : "z" (__addr16)        \
    );                          \
    __result;                   \
}))

#define __LPM_enhanced__(addr)  \
(__extension__({                \
    uint16_t __addr16 = (uint16_t)(addr); \
    uint8_t __result;           \
    __asm__ __volatile__        \
    (                           \
        "lpm %0, Z" "\n\t"      \
        : "=r" (__result)       \
        : "z" (__addr16)        \
    );                          \
    __result;                   \
}))

#define __LPM_word_classic__(addr)          \
(__extension__({                            \
    uint16_t __addr16 = (uint16_t)(addr);   \
    uint16_t __result;                      \
    __asm__ __volatile__                    \
    (                                       \
        "lpm"           "\n\t"              \
        "mov %A0, r0"   "\n\t"              \
        "adiw r30, 1"   "\n\t"              \
        "lpm"           "\n\t"              \
        "mov %B0, r0"   "\n\t"              \
        : "=r" (__result), "=z" (__addr16)  \
        : "1" (__addr16)                    \
        : "r0"                              \
    );                                      \
    __result;                               \
}))

#define __LPM_word_tiny__(addr)             \
(__extension__({                            \
    uint16_t __addr16 = (uint16_t)(addr) + __AVR_TINY_PM_BASE_ADDRESS__; \
    uint16_t __result;                      \
    __asm__                                 \
    (                                       \
        "ld %A0, z+"     "\n\t"             \
        "ld %B0, z"      "\n\t"             \
        : "=r" (__result), "=z" (__addr16)  \
        : "1" (__addr16)                    \
    );                                      \
    __result;                               \
}))

#define __LPM_word_enhanced__(addr)         \
(__extension__({                            \
    uint16_t __addr16 = (uint16_t)(addr);   \
    uint16_t __result;                      \
    __asm__ __volatile__                    \
    (                                       \
        "lpm %A0, Z+"   "\n\t"              \
        "lpm %B0, Z"    "\n\t"              \
        : "=r" (__result), "=z" (__addr16)  \
        : "1" (__addr16)                    \
    );                                      \
    __result;                               \
}))

#define __LPM_dword_classic__(addr)         \
(__extension__({                            \
    uint16_t __addr16 = (uint16_t)(addr);   \
    uint32_t __result;                      \
    __asm__ __volatile__                    \
    (                                       \
        "lpm"           "\n\t"              \
        "mov %A0, r0"   "\n\t"              \
        "adiw r30, 1"   "\n\t"              \
        "lpm"           "\n\t"              \
        "mov %B0, r0"   "\n\t"              \
        "adiw r30, 1"   "\n\t"              \
        "lpm"           "\n\t"              \
        "mov %C0, r0"   "\n\t"              \
        "adiw r30, 1"   "\n\t"              \
        "lpm"           "\n\t"              \
        "mov %D0, r0"   "\n\t"              \
        : "=r" (__result), "=z" (__addr16)  \
        : "1" (__addr16)                    \
        : "r0"                              \
    );                                      \
    __result;                               \
}))

#define __LPM_dword_tiny__(addr)            \
(__extension__({                            \
    uint16_t __addr16 = (uint16_t)(addr) + __AVR_TINY_PM_BASE_ADDRESS__; \
    uint32_t __result;                      \
    __asm__                                 \
    (                                       \
        "ld %A0, z+"    "\n\t"              \
        "ld %B0, z+"    "\n\t"              \
        "ld %C0, z+"    "\n\t"              \
        "ld %D0, z"     "\n\t"              \
        : "=r" (__result), "=z" (__addr16)  \
        : "1" (__addr16)                    \
    );                                      \
    __result;                               \
}))

#define __LPM_dword_enhanced__(addr)        \
(__extension__({                            \
    uint16_t __addr16 = (uint16_t)(addr);   \
    uint32_t __result;                      \
    __asm__ __volatile__                    \
    (                                       \
        "lpm %A0, Z+"   "\n\t"              \
        "lpm %B0, Z+"   "\n\t"              \
        "lpm %C0, Z+"   "\n\t"              \
        "lpm %D0, Z"    "\n\t"              \
        : "=r" (__result), "=z" (__addr16)  \
        : "1" (__addr16)                    \
    );                                      \
    __result;                               \
}))

#define __LPM_float_classic__(addr)         \
(__extension__({                            \
    uint16_t __addr16 = (uint16_t)(addr);   \
    float __result;                         \
    __asm__ __volatile__                    \
    (                                       \
        "lpm"           "\n\t"              \
        "mov %A0, r0"   "\n\t"              \
        "adiw r30, 1"   "\n\t"              \
        "lpm"           "\n\t"              \
        "mov %B0, r0"   "\n\t"              \
        "adiw r30, 1"   "\n\t"              \
        "lpm"           "\n\t"              \
        "mov %C0, r0"   "\n\t"              \
        "adiw r30, 1"   "\n\t"              \
        "lpm"           "\n\t"              \
        "mov %D0, r0"   "\n\t"              \
        : "=r" (__result), "=z" (__addr16)  \
        : "1" (__addr16)                    \
        : "r0"                              \
    );                                      \
    __result;                               \
}))

#define __LPM_float_tiny__(addr)            \
(__extension__({                            \
    uint16_t __addr16 = (uint16_t)(addr) + __AVR_TINY_PM_BASE_ADDRESS__; \
    float __result;                         \
    __asm__                                 \
    (                                       \
        "ld %A0, z+"   "\n\t"               \
        "ld %B0, z+"   "\n\t"               \
        "ld %C0, z+"   "\n\t"               \
        "ld %D0, z"    "\n\t"               \
        : "=r" (__result), "=z" (__addr16)  \
        : "1" (__addr16)                    \
    );                                      \
    __result;                               \
}))

#define __LPM_float_enhanced__(addr)        \
(__extension__({                            \
    uint16_t __addr16 = (uint16_t)(addr);   \
    float __result;                         \
    __asm__ __volatile__                    \
    (                                       \
        "lpm %A0, Z+"   "\n\t"              \
        "lpm %B0, Z+"   "\n\t"              \
        "lpm %C0, Z+"   "\n\t"              \
        "lpm %D0, Z"    "\n\t"              \
        : "=r" (__result), "=z" (__addr16)  \
        : "1" (__addr16)                    \
    );                                      \
    __result;                               \
}))

#include <avr/io.h>



#if defined (__AVR_HAVE_LPMX__)
#define __LPM(addr)         __LPM_enhanced__(addr)
#define __LPM_word(addr)    __LPM_word_enhanced__(addr)
#define __LPM_dword(addr)   __LPM_dword_enhanced__(addr)
#define __LPM_float(addr)   __LPM_float_enhanced__(addr)
/*
Macro to read data from program memory for avr tiny parts(tiny 4/5/9/10/20/40).
why:
- LPM instruction is not available in AVR_TINY instruction set.
- Programs are executed starting from address 0x0000 in program memory.
But it must be addressed starting from 0x4000 when accessed via data memory.
Reference: TINY device (ATTiny 4,5,9,10,20 and 40) datasheets
Bug: avrtc-536
*/
#elif defined (__AVR_TINY__)
#define __LPM(addr)         __LPM_tiny__(addr)
#define __LPM_word(addr)    __LPM_word_tiny__(addr)
#define __LPM_dword(addr)   __LPM_dword_tiny__(addr)
#define __LPM_float(addr)   __LPM_float_tiny__(addr)
#else
#define __LPM(addr)         __LPM_classic__(addr)
#define __LPM_word(addr)    __LPM_word_classic__(addr)
#define __LPM_dword(addr)   __LPM_dword_classic__(addr)
#define __LPM_float(addr)   __LPM_float_classic__(addr)
#endif

#endif  /* !__DOXYGEN__ */

/** \ingroup avr_pgmspace
    \def pgm_read_byte_near(address_short)
    Read a byte from the program space with a 16-bit (near) address. 
    \note The address is a byte address.
    The address is in the program space. */

#define pgm_read_byte_near(address_short) __LPM((uint16_t)(address_short))

/** \ingroup avr_pgmspace
    \def pgm_read_word_near(address_short)
    Read a word from the program space with a 16-bit (near) address. 
    \note The address is a byte address. 
    The address is in the program space. */

#define pgm_read_word_near(address_short) __LPM_word((uint16_t)(address_short))

/** \ingroup avr_pgmspace
    \def pgm_read_dword_near(address_short)
    Read a double word from the program space with a 16-bit (near) address. 
    \note The address is a byte address. 
    The address is in the program space. */

#define pgm_read_dword_near(address_short) \
    __LPM_dword((uint16_t)(address_short))

/** \ingroup avr_pgmspace
    \def pgm_read_float_near(address_short)
    Read a float from the program space with a 16-bit (near) address. 
    \note The address is a byte address. 
    The address is in the program space. */

#define pgm_read_float_near(address_short) \
    __LPM_float((uint16_t)(address_short))

/** \ingroup avr_pgmspace
    \def pgm_read_ptr_near(address_short)
    Read a pointer from the program space with a 16-bit (near) address. 
    \note The address is a byte address. 
    The address is in the program space. */

#define pgm_read_ptr_near(address_short) \
    (void*)__LPM_word((uint16_t)(address_short))

#if defined(RAMPZ) || defined(__DOXYGEN__)

/* Only for devices with more than 64K of program memory.
   RAMPZ must be defined (see iom103.h, iom128.h).
*/

/* The classic functions are needed for ATmega103. */
#ifndef __DOXYGEN__		/* These are internal macros, avoid "is
				   not documented" warnings. */
#define __ELPM_classic__(addr)      \
(__extension__({                    \
    uint32_t __addr32 = (uint32_t)(addr); \
    uint8_t __result;               \
    __asm__ __volatile__            \
    (                               \
        "out %2, %C1" "\n\t"        \
        "mov r31, %B1" "\n\t"       \
        "mov r30, %A1" "\n\t"       \
        "elpm" "\n\t"               \
        "mov %0, r0" "\n\t"         \
        : "=r" (__result)           \
        : "r" (__addr32),           \
          "I" (_SFR_IO_ADDR(RAMPZ)) \
        : "r0", "r30", "r31"        \
    );                              \
    __result;                       \
}))

#define __ELPM_enhanced__(addr)     \
(__extension__({                    \
    uint32_t __addr32 = (uint32_t)(addr); \
    uint8_t __result;               \
    __asm__ __volatile__            \
    (                               \
        "out %2, %C1" "\n\t"        \
        "movw r30, %1" "\n\t"       \
        "elpm %0, Z+" "\n\t"        \
        : "=r" (__result)           \
        : "r" (__addr32),           \
          "I" (_SFR_IO_ADDR(RAMPZ)) \
        : "r30", "r31"              \
    );                              \
    __result;                       \
}))

#define __ELPM_xmega__(addr)        \
(__extension__({                    \
    uint32_t __addr32 = (uint32_t)(addr); \
    uint8_t __result;               \
    __asm__ __volatile__            \
    (                               \
        "in __tmp_reg__, %2" "\n\t" \
        "out %2, %C1" "\n\t"        \
        "movw r30, %1" "\n\t"       \
        "elpm %0, Z+" "\n\t"        \
        "out %2, __tmp_reg__"       \
        : "=r" (__result)           \
        : "r" (__addr32),           \
          "I" (_SFR_IO_ADDR(RAMPZ)) \
        : "r30", "r31"              \
    );                              \
    __result;                       \
}))

#define __ELPM_word_classic__(addr)     \
(__extension__({                        \
    uint32_t __addr32 = (uint32_t)(addr); \
    uint16_t __result;                  \
    __asm__ __volatile__                \
    (                                   \
        "out %2, %C1"   "\n\t"          \
        "mov r31, %B1"  "\n\t"          \
        "mov r30, %A1"  "\n\t"          \
        "elpm"          "\n\t"          \
        "mov %A0, r0"   "\n\t"          \
        "in r0, %2"     "\n\t"          \
        "adiw r30, 1"   "\n\t"          \
        "adc r0, __zero_reg__" "\n\t"   \
        "out %2, r0"    "\n\t"          \
        "elpm"          "\n\t"          \
        "mov %B0, r0"   "\n\t"          \
        : "=r" (__result)               \
        : "r" (__addr32),               \
          "I" (_SFR_IO_ADDR(RAMPZ))     \
        : "r0", "r30", "r31"            \
    );                                  \
    __result;                           \
}))

#define __ELPM_word_enhanced__(addr)    \
(__extension__({                        \
    uint32_t __addr32 = (uint32_t)(addr); \
    uint16_t __result;                  \
    __asm__ __volatile__                \
    (                                   \
        "out %2, %C1"   "\n\t"          \
        "movw r30, %1"  "\n\t"          \
        "elpm %A0, Z+"  "\n\t"          \
        "elpm %B0, Z"   "\n\t"          \
        : "=r" (__result)               \
        : "r" (__addr32),               \
          "I" (_SFR_IO_ADDR(RAMPZ))     \
        : "r30", "r31"                  \
    );                                  \
    __result;                           \
}))

#define __ELPM_word_xmega__(addr)       \
(__extension__({                        \
    uint32_t __addr32 = (uint32_t)(addr); \
    uint16_t __result;                  \
    __asm__ __volatile__                \
    (                                   \
        "in __tmp_reg__, %2" "\n\t"     \
        "out %2, %C1"   "\n\t"          \
        "movw r30, %1"  "\n\t"          \
        "elpm %A0, Z+"  "\n\t"          \
        "elpm %B0, Z"   "\n\t"          \
        "out %2, __tmp_reg__"           \
        : "=r" (__result)               \
        : "r" (__addr32),               \
          "I" (_SFR_IO_ADDR(RAMPZ))     \
        : "r30", "r31"                  \
    );                                  \
    __result;                           \
}))

#define __ELPM_dword_classic__(addr)      \
(__extension__({                          \
    uint32_t __addr32 = (uint32_t)(addr); \
    uint32_t __result;                    \
    __asm__ __volatile__                  \
    (                                     \
        "out %2, %C1"          "\n\t"     \
        "mov r31, %B1"         "\n\t"     \
        "mov r30, %A1"         "\n\t"     \
        "elpm"                 "\n\t"     \
        "mov %A0, r0"          "\n\t"     \
        "in r0, %2"            "\n\t"     \
        "adiw r30, 1"          "\n\t"     \
        "adc r0, __zero_reg__" "\n\t"     \
        "out %2, r0"           "\n\t"     \
        "elpm"                 "\n\t"     \
        "mov %B0, r0"          "\n\t"     \
        "in r0, %2"            "\n\t"     \
        "adiw r30, 1"          "\n\t"     \
        "adc r0, __zero_reg__" "\n\t"     \
        "out %2, r0"           "\n\t"     \
        "elpm"                 "\n\t"     \
        "mov %C0, r0"          "\n\t"     \
        "in r0, %2"            "\n\t"     \
        "adiw r30, 1"          "\n\t"     \
        "adc r0, __zero_reg__" "\n\t"     \
        "out %2, r0"           "\n\t"     \
        "elpm"                 "\n\t"     \
        "mov %D0, r0"          "\n\t"     \
        : "=r" (__result)                 \
        : "r" (__addr32),                 \
          "I" (_SFR_IO_ADDR(RAMPZ))       \
        : "r0", "r30", "r31"              \
    );                                    \
    __result;                             \
}))

#define __ELPM_dword_enhanced__(addr)     \
(__extension__({                          \
    uint32_t __addr32 = (uint32_t)(addr); \
    uint32_t __result;                    \
    __asm__ __volatile__                  \
    (                                     \
        "out %2, %C1"   "\n\t"            \
        "movw r30, %1"  "\n\t"            \
        "elpm %A0, Z+"  "\n\t"            \
        "elpm %B0, Z+"  "\n\t"            \
        "elpm %C0, Z+"  "\n\t"            \
        "elpm %D0, Z"   "\n\t"            \
        : "=r" (__result)                 \
        : "r" (__addr32),                 \
          "I" (_SFR_IO_ADDR(RAMPZ))       \
        : "r30", "r31"                    \
    );                                    \
    __result;                             \
}))

#define __ELPM_dword_xmega__(addr)        \
(__extension__({                          \
    uint32_t __addr32 = (uint32_t)(addr); \
    uint32_t __result;                    \
    __asm__ __volatile__                  \
    (                                     \
        "in __tmp_reg__, %2" "\n\t"       \
        "out %2, %C1"   "\n\t"            \
        "movw r30, %1"  "\n\t"            \
        "elpm %A0, Z+"  "\n\t"            \
        "elpm %B0, Z+"  "\n\t"            \
        "elpm %C0, Z+"  "\n\t"            \
        "elpm %D0, Z"   "\n\t"            \
        "out %2, __tmp_reg__"             \
        : "=r" (__result)                 \
        : "r" (__addr32),                 \
          "I" (_SFR_IO_ADDR(RAMPZ))       \
        : "r30", "r31"                    \
    );                                    \
    __result;                             \
}))

#define __ELPM_float_classic__(addr)      \
(__extension__({                          \
    uint32_t __addr32 = (uint32_t)(addr); \
    float __result;                       \
    __asm__ __volatile__                  \
    (                                     \
        "out %2, %C1"          "\n\t"     \
        "mov r31, %B1"         "\n\t"     \
        "mov r30, %A1"         "\n\t"     \
        "elpm"                 "\n\t"     \
        "mov %A0, r0"          "\n\t"     \
        "in r0, %2"            "\n\t"     \
        "adiw r30, 1"          "\n\t"     \
        "adc r0, __zero_reg__" "\n\t"     \
        "out %2, r0"           "\n\t"     \
        "elpm"                 "\n\t"     \
        "mov %B0, r0"          "\n\t"     \
        "in r0, %2"            "\n\t"     \
        "adiw r30, 1"          "\n\t"     \
        "adc r0, __zero_reg__" "\n\t"     \
        "out %2, r0"           "\n\t"     \
        "elpm"                 "\n\t"     \
        "mov %C0, r0"          "\n\t"     \
        "in r0, %2"            "\n\t"     \
        "adiw r30, 1"          "\n\t"     \
        "adc r0, __zero_reg__" "\n\t"     \
        "out %2, r0"           "\n\t"     \
        "elpm"                 "\n\t"     \
        "mov %D0, r0"          "\n\t"     \
        : "=r" (__result)                 \
        : "r" (__addr32),                 \
          "I" (_SFR_IO_ADDR(RAMPZ))       \
        : "r0", "r30", "r31"              \
    );                                    \
    __result;                             \
}))

#define __ELPM_float_enhanced__(addr)     \
(__extension__({                          \
    uint32_t __addr32 = (uint32_t)(addr); \
    float __result;                       \
    __asm__ __volatile__                  \
    (                                     \
        "out %2, %C1"   "\n\t"            \
        "movw r30, %1"  "\n\t"            \
        "elpm %A0, Z+"  "\n\t"            \
        "elpm %B0, Z+"  "\n\t"            \
        "elpm %C0, Z+"  "\n\t"            \
        "elpm %D0, Z"   "\n\t"            \
        : "=r" (__result)                 \
        : "r" (__addr32),                 \
          "I" (_SFR_IO_ADDR(RAMPZ))       \
        : "r30", "r31"                    \
    );                                    \
    __result;                             \
}))

#define __ELPM_float_xmega__(addr)        \
(__extension__({                          \
    uint32_t __addr32 = (uint32_t)(addr); \
    float __result;                       \
    __asm__ __volatile__                  \
    (                                     \
        "in __tmp_reg__, %2" "\n\t"       \
        "out %2, %C1"   "\n\t"            \
        "movw r30, %1"  "\n\t"            \
        "elpm %A0, Z+"  "\n\t"            \
        "elpm %B0, Z+"  "\n\t"            \
        "elpm %C0, Z+"  "\n\t"            \
        "elpm %D0, Z"   "\n\t"            \
        "out %2, __tmp_reg__"             \
        : "=r" (__result)                 \
        : "r" (__addr32),                 \
          "I" (_SFR_IO_ADDR(RAMPZ))       \
        : "r30", "r31"                    \
    );                                    \
    __result;                             \
}))

/*
Check for architectures that implement RAMPD (avrxmega3, avrxmega5,
avrxmega7) as they need to save/restore RAMPZ for ELPM macros so it does
not interfere with data accesses.
*/
#if defined (__AVR_HAVE_RAMPD__)

#define __ELPM(addr)        __ELPM_xmega__(addr)
#define __ELPM_word(addr)   __ELPM_word_xmega__(addr)
#define __ELPM_dword(addr)  __ELPM_dword_xmega__(addr)
#define __ELPM_float(addr)  __ELPM_float_xmega__(addr)

#else

#if defined (__AVR_HAVE_LPMX__)

#define __ELPM(addr)        __ELPM_enhanced__(addr)
#define __ELPM_word(addr)   __ELPM_word_enhanced__(addr)
#define __ELPM_dword(addr)  __ELPM_dword_enhanced__(addr)
#define __ELPM_float(addr)  __ELPM_float_enhanced__(addr)

#else

#define __ELPM(addr)        __ELPM_classic__(addr)
#define __ELPM_word(addr)   __ELPM_word_classic__(addr)
#define __ELPM_dword(addr)  __ELPM_dword_classic__(addr)
#define __ELPM_float(addr)  __ELPM_float_classic__(addr)

#endif  /* __AVR_HAVE_LPMX__ */

#endif  /* __AVR_HAVE_RAMPD__ */

#endif	/* !__DOXYGEN__ */

/** \ingroup avr_pgmspace
    \def pgm_read_byte_far(address_long)
    Read a byte from the program space with a 32-bit (far) address. 

    \note The address is a byte address. 
    The address is in the program space. */

#define pgm_read_byte_far(address_long)  __ELPM((uint32_t)(address_long))

/** \ingroup avr_pgmspace
    \def pgm_read_word_far(address_long)
    Read a word from the program space with a 32-bit (far) address. 

    \note The address is a byte address.
    The address is in the program space. */

#define pgm_read_word_far(address_long)  __ELPM_word((uint32_t)(address_long))

/** \ingroup avr_pgmspace
    \def pgm_read_dword_far(address_long)
    Read a double word from the program space with a 32-bit (far) address. 

    \note The address is a byte address.
    The address is in the program space. */

#define pgm_read_dword_far(address_long) __ELPM_dword((uint32_t)(address_long))

/** \ingroup avr_pgmspace
    \def pgm_read_float_far(address_long)
    Read a float from the program space with a 32-bit (far) address. 

    \note The address is a byte address.
    The address is in the program space. */

#define pgm_read_float_far(address_long) __ELPM_float((uint32_t)(address_long))

/** \ingroup avr_pgmspace
    \def pgm_read_ptr_far(address_long)
    Read a pointer from the program space with a 32-bit (far) address. 

    \note The address is a byte address.
    The address is in the program space. */

#define pgm_read_ptr_far(address_long) (void*)__ELPM_word((uint32_t)(address_long))

#endif /* RAMPZ or __DOXYGEN__ */

/** \ingroup avr_pgmspace
    \def pgm_read_byte(address_short)
    Read a byte from the program space with a 16-bit (near) address. 

    \note The address is a byte address. 
    The address is in the program space. */

#define pgm_read_byte(address_short)    pgm_read_byte_near(address_short)

/** \ingroup avr_pgmspace
    \def pgm_read_word(address_short)
    Read a word from the program space with a 16-bit (near) address. 

    \note The address is a byte address. 
    The address is in the program space. */

#define pgm_read_word(address_short)    pgm_read_word_near(address_short)

/** \ingroup avr_pgmspace
    \def pgm_read_dword(address_short)
    Read a double word from the program space with a 16-bit (near) address. 

    \note The address is a byte address. 
    The address is in the program space. */

#define pgm_read_dword(address_short)   pgm_read_dword_near(address_short)

/** \ingroup avr_pgmspace
    \def pgm_read_float(address_short)
    Read a float from the program space with a 16-bit (near) address. 

    \note The address is a byte address. 
    The address is in the program space. */

#define pgm_read_float(address_short)   pgm_read_float_near(address_short)

/** \ingroup avr_pgmspace
    \def pgm_read_ptr(address_short)
    Read a pointer from the program space with a 16-bit (near) address. 

    \note The address is a byte address. 
    The address is in the program space. */

#define pgm_read_ptr(address_short)     pgm_read_ptr_near(address_short)

#define pgm_get_far_address(var)                          \
({                                                    \
	uint_farptr_t tmp;                                \
                                                      \
	__asm__ __volatile__(                             \
                                                      \
			"ldi	%A0, lo8(%1)"           "\n\t"    \
			"ldi	%B0, hi8(%1)"           "\n\t"    \
			"ldi	%C0, hh8(%1)"           "\n\t"    \
			"clr	%D0"                    "\n\t"    \
		:                                             \
			"=d" (tmp)                                \
		:                                             \
			"p"  (&(var))                             \
	);                                                \
	tmp;                                              \
})

extern const void * memchr_P(const void *, int __val, size_t __len) __ATTR_CONST__;
extern int memcmp_P(const void *, const void *, size_t) __ATTR_PURE__;
extern void *memccpy_P(void *, const void *, int __val, size_t);
extern void *memcpy_P(void *, const void *, size_t);
extern void *memmem_P(const void *, size_t, const void *, size_t) __ATTR_PURE__;
extern const void * memrchr_P(const void *, int __val, size_t __len) __ATTR_CONST__;
extern char *strcat_P(char *, const char *);

/** \ingroup avr_pgmspace
    \fn const char *strchr_P(const char *s, int val)
    \brief Locate character in program space string.

    The strchr_P() function locates the first occurrence of \p val
    (converted to a char) in the string pointed to by \p s in program
    space. The terminating null character is considered to be part of
    the string.

    The strchr_P() function is similar to strchr() except that \p s is
    pointer to a string in program space.

    \returns The strchr_P() function returns a pointer to the matched
    character or \c NULL if the character is not found. */
extern const char * strchr_P(const char *, int __val) __ATTR_CONST__;

/** \ingroup avr_pgmspace
    \fn const char *strchrnul_P(const char *s, int c)

    The strchrnul_P() function is like strchr_P() except that if \p c is
    not found in \p s, then it returns a pointer to the null byte at the
    end of \p s, rather than \c NULL. (Glibc, GNU extension.)

    \return The strchrnul_P() function returns a pointer to the matched
    character, or a pointer to the null byte at the end of \p s (i.e.,
    \c s+strlen(s)) if the character is not found.	*/
extern const char * strchrnul_P(const char *, int __val) __ATTR_CONST__;

/** \ingroup avr_pgmspace
    \fn int strcmp_P(const char *s1, const char *s2)

    The strcmp_P() function is similar to strcmp() except that \p s2 is
    pointer to a string in program space.

    \returns The strcmp_P() function returns an integer less than, equal
    to, or greater than zero if \p s1 is found, respectively, to be less
    than, to match, or be greater than \p s2. A consequence of the
    ordering used by strcmp_P() is that if \p s1 is an initial substring
    of \p s2, then \p s1 is considered to be "less than" \p s2. */
extern int strcmp_P(const char *, const char *) __ATTR_PURE__;

/** \ingroup avr_pgmspace
    \fn char *strcpy_P(char *dest, const char *src)

    The strcpy_P() function is similar to strcpy() except that src is a
    pointer to a string in program space.

    \returns The strcpy_P() function returns a pointer to the destination
    string dest. */
extern char *strcpy_P(char *, const char *);

/** \ingroup avr_pgmspace
    \fn int strcasecmp_P(const char *s1, const char *s2)
    \brief Compare two strings ignoring case.

    The strcasecmp_P() function compares the two strings \p s1 and \p s2,
    ignoring the case of the characters.

    \param s1 A pointer to a string in the devices SRAM.
    \param s2 A pointer to a string in the devices Flash.

    \returns The strcasecmp_P() function returns an integer less than,
    equal to, or greater than zero if \p s1 is found, respectively, to
    be less than, to match, or be greater than \p s2. A consequence of
    the ordering used by strcasecmp_P() is that if \p s1 is an initial
    substring of \p s2, then \p s1 is considered to be "less than" \p s2. */
extern int strcasecmp_P(const char *, const char *) __ATTR_PURE__;
extern char *strcasestr_P(const char *, const char *) __ATTR_PURE__;
extern size_t strcspn_P(const char *__s, const char * __reject) __ATTR_PURE__;

/** \ingroup avr_pgmspace
    \fn size_t strlcat_P(char *dst, const char *src, size_t siz)
    \brief Concatenate two strings.

    The strlcat_P() function is similar to strlcat(), except that the \p src
    string must be located in program space (flash).

    Appends \p src to string \p dst of size \p siz (unlike strncat(),
    \p siz is the full size of \p dst, not space left).  At most \p siz-1
    characters will be copied.  Always NULL terminates (unless \p siz <=
    \p strlen(dst)).

    \returns The strlcat_P() function returns strlen(src) + MIN(siz,
    strlen(initial dst)).  If retval >= siz, truncation occurred.	*/
extern size_t strlcat_P (char *, const char *, size_t );

/** \ingroup avr_pgmspace
    \fn size_t strlcpy_P(char *dst, const char *src, size_t siz)
    \brief Copy a string from progmem to RAM.

    Copy \p src to string \p dst of size \p siz.  At most \p siz-1
    characters will be copied.  Always NULL terminates (unless \p siz == 0).
    The strlcpy_P() function is similar to strlcpy() except that the
    \p src is pointer to a string in memory space.

    \returns The strlcpy_P() function returns strlen(src). If
    retval >= siz, truncation occurred.  */
extern size_t strlcpy_P (char *, const char *, size_t );

/** \ingroup avr_pgmspace
    \fn size_t strnlen_P(const char *src, size_t len)
    \brief Determine the length of a fixed-size string.

    The strnlen_P() function is similar to strnlen(), except that \c src is a
    pointer to a string in program space.

    \returns The strnlen_P function returns strlen_P(src), if that is less than
    \c len, or \c len if there is no '\\0' character among the first \c len
    characters pointed to by \c src. */
extern size_t strnlen_P(const char *, size_t) __ATTR_CONST__; /* program memory can't change */

/** \ingroup avr_pgmspace
    \fn int strncmp_P(const char *s1, const char *s2, size_t n)

    The strncmp_P() function is similar to strcmp_P() except it only compares
    the first (at most) n characters of s1 and s2.

    \returns The strncmp_P() function returns an integer less than, equal to,
    or greater than zero if s1 (or the first n bytes thereof) is found,
    respectively, to be less than, to match, or be greater than s2.  */
extern int strncmp_P(const char *, const char *, size_t) __ATTR_PURE__;

/** \ingroup avr_pgmspace
    \fn int strncasecmp_P(const char *s1, const char *s2, size_t n)
    \brief Compare two strings ignoring case.

    The strncasecmp_P() function is similar to strcasecmp_P(), except it
    only compares the first \p n characters of \p s1.

    \param s1 A pointer to a string in the devices SRAM.
    \param s2 A pointer to a string in the devices Flash.
    \param n The maximum number of bytes to compare.

    \returns The strncasecmp_P() function returns an integer less than,
    equal to, or greater than zero if \p s1 (or the first \p n bytes
    thereof) is found, respectively, to be less than, to match, or be
    greater than \p s2. A consequence of the ordering used by
    strncasecmp_P() is that if \p s1 is an initial substring of \p s2,
    then \p s1 is considered to be "less than" \p s2.  */
extern int strncasecmp_P(const char *, const char *, size_t) __ATTR_PURE__;

/** \ingroup avr_pgmspace
    \fn char *strncat_P(char *dest, const char *src, size_t len)
    \brief Concatenate two strings.

    The strncat_P() function is similar to strncat(), except that the \e src
    string must be located in program space (flash).

    \returns The strncat_P() function returns a pointer to the resulting string
    dest.  */
extern char *strncat_P(char *, const char *, size_t);

/** \ingroup avr_pgmspace
    \fn char *strncpy_P(char *dest, const char *src, size_t n)

    The strncpy_P() function is similar to strcpy_P() except that not more
    than n bytes of src are copied.  Thus, if there is no null byte among the
    first n bytes of src, the result will not be null-terminated.

    In the case where the length of src is less than that of n, the remainder
    of dest will be padded with nulls.

    \returns The strncpy_P() function returns a pointer to the destination
    string dest.  */
extern char *strncpy_P(char *, const char *, size_t);

/** \ingroup avr_pgmspace
    \fn char *strpbrk_P(const char *s, const char *accept)

    The strpbrk_P() function locates the first occurrence in the string
    \p s of any of the characters in the flash string \p accept. This
    function is similar to strpbrk() except that \p accept is a pointer
    to a string in program space.

    \return  The strpbrk_P() function returns a pointer to the character
    in \p s that matches one of the characters in \p accept, or \c NULL
    if no such character is found. The terminating zero is not considered
    as a part of string: if one or both args are empty, the result will
    \c NULL. */
extern char *strpbrk_P(const char *__s, const char * __accept) __ATTR_PURE__;

/** \ingroup avr_pgmspace
    \fn const char *strrchr_P(const char *s, int val)
    \brief Locate character in string.

    The strrchr_P() function returns a pointer to the last occurrence of
    the character \p val in the flash string \p s.

    \return The strrchr_P() function returns a pointer to the matched
    character or \c NULL if the character is not found. */
extern const char * strrchr_P(const char *, int __val) __ATTR_CONST__;

/** \ingroup avr_pgmspace
    \fn char *strsep_P(char **sp, const char *delim)
    \brief Parse a string into tokens.

    The strsep_P() function locates, in the string referenced by \p *sp,
    the first occurrence of any character in the string \p delim (or the
    terminating '\\0' character) and replaces it with a '\\0'.  The
    location of the next character after the delimiter character (or \c
    NULL, if the end of the string was reached) is stored in \p *sp. An
    ``empty'' field, i.e. one caused by two adjacent delimiter
    characters, can be detected by comparing the location referenced by
    the pointer returned in \p *sp to '\\0'. This function is similar to
    strsep() except that \p delim is a pointer to a string in program
    space.

    \return The strsep_P() function returns a pointer to the original
    value of \p *sp. If \p *sp is initially \c NULL, strsep_P() returns
    \c NULL. */
extern char *strsep_P(char **__sp, const char * __delim);

/** \ingroup avr_pgmspace
    \fn size_t strspn_P(const char *s, const char *accept)

    The strspn_P() function calculates the length of the initial segment
    of \p s which consists entirely of characters in \p accept. This
    function is similar to strspn() except that \p accept is a pointer
    to a string in program space.

    \return  The strspn_P() function returns the number of characters in
    the initial segment of \p s which consist only of characters from \p
    accept. The terminating zero is not considered as a part of string.	*/
extern size_t strspn_P(const char *__s, const char * __accept) __ATTR_PURE__;

/** \ingroup avr_pgmspace
    \fn char *strstr_P(const char *s1, const char *s2)
    \brief Locate a substring.

    The strstr_P() function finds the first occurrence of the substring
    \p s2 in the string \p s1.  The terminating '\\0' characters are not
    compared. The strstr_P() function is similar to strstr() except that
    \p s2 is pointer to a string in program space.

    \returns The strstr_P() function returns a pointer to the beginning
    of the substring, or NULL if the substring is not found. If \p s2
    points to a string of zero length, the function returns \p s1. */
extern char *strstr_P(const char *, const char *) __ATTR_PURE__;

/** \ingroup avr_pgmspace
    \fn char *strtok_P(char *s, const char * delim)
    \brief Parses the string into tokens.

    strtok_P() parses the string \p s into tokens. The first call to
    strtok_P() should have \p s as its first argument. Subsequent calls
    should have the first argument set to NULL. If a token ends with a
    delimiter, this delimiting character is overwritten with a '\\0' and a
    pointer to the next character is saved for the next call to strtok_P().
    The delimiter string \p delim may be different for each call.

    The strtok_P() function is similar to strtok() except that \p delim
    is pointer to a string in program space.

    \returns The strtok_P() function returns a pointer to the next token or
    NULL when no more tokens are found.

    \note strtok_P() is NOT reentrant. For a reentrant version of this
    function see strtok_rP().
 */
extern char *strtok_P(char *__s, const char * __delim);

/** \ingroup avr_pgmspace
    \fn char *strtok_rP (char *string, const char *delim, char **last)
    \brief Parses string into tokens.

    The strtok_rP() function parses \p string into tokens. The first call to
    strtok_rP() should have string as its first argument. Subsequent calls
    should have the first argument set to NULL. If a token ends with a
    delimiter, this delimiting character is overwritten with a '\\0' and a
    pointer to the next character is saved for the next call to strtok_rP().
    The delimiter string \p delim may be different for each call. \p last is
    a user allocated char* pointer. It must be the same while parsing the
    same string. strtok_rP() is a reentrant version of strtok_P().

    The strtok_rP() function is similar to strtok_r() except that \p delim
    is pointer to a string in program space.

    \returns The strtok_rP() function returns a pointer to the next token or
    NULL when no more tokens are found. */
extern char *strtok_rP(char *__s, const char * __delim, char **__last);

/** \ingroup avr_pgmspace
    \fn size_t strlen_PF(uint_farptr_t s)
    \brief Obtain the length of a string

    The strlen_PF() function is similar to strlen(), except that \e s is a
    far pointer to a string in program space.

    \param s A far pointer to the string in flash

    \returns The strlen_PF() function returns the number of characters in
    \e s. The contents of RAMPZ SFR are undefined when the function returns. */
extern size_t strlen_PF(uint_farptr_t src) __ATTR_CONST__; /* program memory can't change */

/** \ingroup avr_pgmspace
    \fn size_t strnlen_PF(uint_farptr_t s, size_t len)
    \brief Determine the length of a fixed-size string

    The strnlen_PF() function is similar to strnlen(), except that \e s is a
    far pointer to a string in program space.

    \param s A far pointer to the string in Flash
    \param len The maximum number of length to return

    \returns The strnlen_PF function returns strlen_P(\e s), if that is less
    than \e len, or \e len if there is no '\\0' character among the first \e
    len characters pointed to by \e s. The contents of RAMPZ SFR are
    undefined when the function returns. */
extern size_t strnlen_PF(uint_farptr_t src, size_t len) __ATTR_CONST__;

/** \ingroup avr_pgmspace
    \fn void *memcpy_PF(void *dest, uint_farptr_t src, size_t n)
    \brief Copy a memory block from flash to SRAM

    The memcpy_PF() function is similar to memcpy(), except the data
    is copied from the program space and is addressed using a far pointer.

    \param dest A pointer to the destination buffer
    \param src A far pointer to the origin of data in flash memory
    \param n The number of bytes to be copied

    \returns The memcpy_PF() function returns a pointer to \e dst. The contents
    of RAMPZ SFR are undefined when the function returns. */
extern void *memcpy_PF(void *dest, uint_farptr_t src, size_t len);

/** \ingroup avr_pgmspace
    \fn char *strcpy_PF(char *dst, uint_farptr_t src)
    \brief Duplicate a string

    The strcpy_PF() function is similar to strcpy() except that \e src is a far
    pointer to a string in program space.

    \param dst A pointer to the destination string in SRAM
    \param src A far pointer to the source string in Flash

    \returns The strcpy_PF() function returns a pointer to the destination
    string \e dst. The contents of RAMPZ SFR are undefined when the funcion
    returns. */
extern char *strcpy_PF(char *dest, uint_farptr_t src);

/** \ingroup avr_pgmspace
    \fn char *strncpy_PF(char *dst, uint_farptr_t src, size_t n)
    \brief Duplicate a string until a limited length

    The strncpy_PF() function is similar to strcpy_PF() except that not more
    than \e n bytes of \e src are copied.  Thus, if there is no null byte among
    the first \e n bytes of \e src, the result will not be null-terminated.

    In the case where the length of \e src is less than that of \e n, the
    remainder of \e dst will be padded with nulls.

    \param dst A pointer to the destination string in SRAM
    \param src A far pointer to the source string in Flash
    \param n The maximum number of bytes to copy

    \returns The strncpy_PF() function returns a pointer to the destination
    string \e dst. The contents of RAMPZ SFR are undefined when the function
    returns. */
extern char *strncpy_PF(char *dest, uint_farptr_t src, size_t len);

/** \ingroup avr_pgmspace
    \fn char *strcat_PF(char *dst, uint_farptr_t src)
    \brief Concatenates two strings

    The strcat_PF() function is similar to strcat() except that the \e src
    string must be located in program space (flash) and is addressed using
    a far pointer

    \param dst A pointer to the destination string in SRAM
    \param src A far pointer to the string to be appended in Flash

    \returns The strcat_PF() function returns a pointer to the resulting
    string \e dst. The contents of RAMPZ SFR are undefined when the function
    returns */
extern char *strcat_PF(char *dest, uint_farptr_t src);

/** \ingroup avr_pgmspace
    \fn size_t strlcat_PF(char *dst, uint_farptr_t src, size_t n)
    \brief Concatenate two strings

    The strlcat_PF() function is similar to strlcat(), except that the \e src
    string must be located in program space (flash) and is addressed using
    a far pointer.

    Appends src to string dst of size \e n (unlike strncat(), \e n is the
    full size of \e dst, not space left).  At most \e n-1 characters
    will be copied.  Always NULL terminates (unless \e n <= strlen(\e dst)).

    \param dst A pointer to the destination string in SRAM
    \param src A far pointer to the source string in Flash
    \param n The total number of bytes allocated to the destination string

    \returns The strlcat_PF() function returns strlen(\e src) + MIN(\e n,
    strlen(initial \e dst)).  If retval >= \e n, truncation occurred. The
    contents of RAMPZ SFR are undefined when the funcion returns. */
extern size_t strlcat_PF(char *dst, uint_farptr_t src, size_t siz);

/** \ingroup avr_pgmspace
    \fn char *strncat_PF(char *dst, uint_farptr_t src, size_t n)
    \brief Concatenate two strings

    The strncat_PF() function is similar to strncat(), except that the \e src
    string must be located in program space (flash) and is addressed using a
    far pointer.

    \param dst A pointer to the destination string in SRAM
    \param src A far pointer to the source string in Flash
    \param n The maximum number of bytes to append

    \returns The strncat_PF() function returns a pointer to the resulting
    string \e dst. The contents of RAMPZ SFR are undefined when the function
    returns. */
extern char *strncat_PF(char *dest, uint_farptr_t src, size_t len);

/** \ingroup avr_pgmspace
    \fn int strcmp_PF(const char *s1, uint_farptr_t s2)
    \brief Compares two strings

    The strcmp_PF() function is similar to strcmp() except that \e s2 is a far
    pointer to a string in program space.

    \param s1 A pointer to the first string in SRAM
    \param s2 A far pointer to the second string in Flash

    \returns The strcmp_PF() function returns an integer less than, equal to,
    or greater than zero if \e s1 is found, respectively, to be less than, to
    match, or be greater than \e s2. The contents of RAMPZ SFR are undefined
    when the function returns. */
extern int strcmp_PF(const char *s1, uint_farptr_t s2) __ATTR_PURE__;

/** \ingroup avr_pgmspace
    \fn int strncmp_PF(const char *s1, uint_farptr_t s2, size_t n)
    \brief Compare two strings with limited length

    The strncmp_PF() function is similar to strcmp_PF() except it only
    compares the first (at most) \e n characters of \e s1 and \e s2.

    \param s1 A pointer to the first string in SRAM
    \param s2 A far pointer to the second string in Flash
    \param n The maximum number of bytes to compare

    \returns The strncmp_PF() function returns an integer less than, equal
    to, or greater than zero if \e s1 (or the first \e n bytes thereof) is found,
    respectively, to be less than, to match, or be greater than \e s2. The
    contents of RAMPZ SFR are undefined when the function returns. */
extern int strncmp_PF(const char *s1, uint_farptr_t s2, size_t n) __ATTR_PURE__;

/** \ingroup avr_pgmspace
    \fn int strcasecmp_PF(const char *s1, uint_farptr_t s2)
    \brief Compare two strings ignoring case

    The strcasecmp_PF() function compares the two strings \e s1 and \e s2, ignoring
    the case of the characters.

    \param s1 A pointer to the first string in SRAM
    \param s2 A far pointer to the second string in Flash

    \returns The strcasecmp_PF() function returns an integer less than, equal
    to, or greater than zero if \e s1 is found, respectively, to be less than, to
    match, or be greater than \e s2. The contents of RAMPZ SFR are undefined
    when the function returns. */
extern int strcasecmp_PF(const char *s1, uint_farptr_t s2) __ATTR_PURE__;

/** \ingroup avr_pgmspace
    \fn int strncasecmp_PF(const char *s1, uint_farptr_t s2, size_t n)
    \brief Compare two strings ignoring case

    The strncasecmp_PF() function is similar to strcasecmp_PF(), except it
    only compares the first \e n characters of \e s1 and the string in flash is
    addressed using a far pointer.

    \param s1 A pointer to a string in SRAM
    \param s2 A far pointer to a string in Flash
    \param n The maximum number of bytes to compare

    \returns The strncasecmp_PF() function returns an integer less than, equal
    to, or greater than zero if \e s1 (or the first \e n bytes thereof) is found,
    respectively, to be less than, to match, or be greater than \e s2. The
    contents of RAMPZ SFR are undefined when the function returns.  */
extern int strncasecmp_PF(const char *s1, uint_farptr_t s2, size_t n) __ATTR_PURE__;

/** \ingroup avr_pgmspace
    \fn char *strstr_PF(const char *s1, uint_farptr_t s2)
    \brief Locate a substring.

    The strstr_PF() function finds the first occurrence of the substring \c s2
    in the string \c s1.  The terminating '\\0' characters are not
    compared.
    The strstr_PF() function is similar to strstr() except that \c s2 is a
    far pointer to a string in program space.

    \returns The strstr_PF() function returns a pointer to the beginning of the
    substring, or NULL if the substring is not found.
    If \c s2 points to a string of zero length, the function returns \c s1. The
    contents of RAMPZ SFR are undefined when the function returns. */
extern char *strstr_PF(const char *s1, uint_farptr_t s2);
extern size_t strlcpy_PF(char *dst, uint_farptr_t src, size_t siz);
extern int memcmp_PF(const void *, uint_farptr_t, size_t) __ATTR_PURE__;

extern size_t __strlen_P(const char *) __ATTR_CONST__;  /* internal helper function */
__attribute__((__always_inline__)) static __inline__ size_t strlen_P(const char * s);
static __inline__ size_t strlen_P(const char *s) {
  return __builtin_constant_p(__builtin_strlen(s))
     ? __builtin_strlen(s) : __strlen_P(s);
}
#endif



