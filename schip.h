#ifndef _AVR_BOOT_H_
#define _AVR_BOOT_H_
#include <avr/io.h>

#if defined (SPMCSR)
#  define __SPM_REG    SPMCSR
#else
#  if defined (SPMCR)
#    define __SPM_REG    SPMCR
#  else
#    error AVR processor does not provide bootloader support!
#  endif
#endif


#if defined(SPMEN)
#  define __SPM_ENABLE  SPMEN
#elif defined(SELFPRGEN)
#  define __SPM_ENABLE  SELFPRGEN
#else
#  error Cannot find SPM Enable bit definition!
#endif

#define BOOTLOADER_SECTION    __attribute__ ((section (".bootloader")))

#ifndef __DOXYGEN__
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

#define BLB12           5
#define BLB11           4
#define BLB02           3
#define BLB01           2
#endif

#define boot_spm_interrupt_enable()   (__SPM_REG |= (uint8_t)_BV(SPMIE))
#define boot_spm_interrupt_disable()  (__SPM_REG &= (uint8_t)~_BV(SPMIE))
#define boot_is_spm_interrupt()       (__SPM_REG & (uint8_t)_BV(SPMIE))
#define boot_rww_busy()          (__SPM_REG & (uint8_t)_BV(__COMMON_ASB))
#define boot_spm_busy()               (__SPM_REG & (uint8_t)_BV(__SPM_ENABLE))
#define boot_spm_busy_wait()          do{}while(boot_spm_busy())

#ifndef __DOXYGEN__
#define __BOOT_PAGE_ERASE         (_BV(__SPM_ENABLE) | _BV(PGERS))
#define __BOOT_PAGE_WRITE         (_BV(__SPM_ENABLE) | _BV(PGWRT))
#define __BOOT_PAGE_FILL          _BV(__SPM_ENABLE)
#define __BOOT_RWW_ENABLE         (_BV(__SPM_ENABLE) | _BV(__COMMON_ASRE))
#if defined(BLBSET)
#define __BOOT_LOCK_BITS_SET      (_BV(__SPM_ENABLE) | _BV(BLBSET))
#elif defined(RFLB)  /* Some devices have RFLB defined instead of BLBSET. */
#define __BOOT_LOCK_BITS_SET      (_BV(__SPM_ENABLE) | _BV(RFLB))
#endif

#define __boot_page_fill_normal(address, data)   \
(__extension__({                                 \
    __asm__ __volatile__                         \
    (                                            \
        "movw  r0, %3\n\t"                       \
        "sts %0, %1\n\t"                         \
        "spm\n\t"                                \
        "clr  r1\n\t"                            \
        :                                        \
        : "i" (_SFR_MEM_ADDR(__SPM_REG)),        \
          "r" ((uint8_t)(__BOOT_PAGE_FILL)),     \
          "z" ((uint16_t)(address)),             \
          "r" ((uint16_t)(data))                 \
        : "r0"                                   \
    );                                           \
}))

#define __boot_page_fill_alternate(address, data)\
(__extension__({                                 \
    __asm__ __volatile__                         \
    (                                            \
        "movw  r0, %3\n\t"                       \
        "sts %0, %1\n\t"                         \
        "spm\n\t"                                \
        ".word 0xffff\n\t"                       \
        "nop\n\t"                                \
        "clr  r1\n\t"                            \
        :                                        \
        : "i" (_SFR_MEM_ADDR(__SPM_REG)),        \
          "r" ((uint8_t)(__BOOT_PAGE_FILL)),     \
          "z" ((uint16_t)(address)),             \
          "r" ((uint16_t)(data))                 \
        : "r0"                                   \
    );                                           \
}))

#define __boot_page_fill_extended(address, data) \
(__extension__({                                 \
    __asm__ __volatile__                         \
    (                                            \
        "movw  r0, %4\n\t"                       \
        "movw r30, %A3\n\t"                      \
        "sts %1, %C3\n\t"                        \
        "sts %0, %2\n\t"                         \
        "spm\n\t"                                \
        "clr  r1\n\t"                            \
        :                                        \
        : "i" (_SFR_MEM_ADDR(__SPM_REG)),        \
          "i" (_SFR_MEM_ADDR(RAMPZ)),            \
          "r" ((uint8_t)(__BOOT_PAGE_FILL)),     \
          "r" ((uint32_t)(address)),             \
          "r" ((uint16_t)(data))                 \
        : "r0", "r30", "r31"                     \
    );                                           \
}))

#define __boot_page_erase_normal(address)        \
(__extension__({                                 \
    __asm__ __volatile__                         \
    (                                            \
        "sts %0, %1\n\t"                         \
        "spm\n\t"                                \
        :                                        \
        : "i" (_SFR_MEM_ADDR(__SPM_REG)),        \
          "r" ((uint8_t)(__BOOT_PAGE_ERASE)),    \
          "z" ((uint16_t)(address))              \
    );                                           \
}))

#define __boot_page_erase_alternate(address)     \
(__extension__({                                 \
    __asm__ __volatile__                         \
    (                                            \
        "sts %0, %1\n\t"                         \
        "spm\n\t"                                \
        ".word 0xffff\n\t"                       \
        "nop\n\t"                                \
        :                                        \
        : "i" (_SFR_MEM_ADDR(__SPM_REG)),        \
          "r" ((uint8_t)(__BOOT_PAGE_ERASE)),    \
          "z" ((uint16_t)(address))              \
    );                                           \
}))

#define __boot_page_erase_extended(address)      \
(__extension__({                                 \
    __asm__ __volatile__                         \
    (                                            \
        "movw r30, %A3\n\t"                      \
        "sts  %1, %C3\n\t"                       \
        "sts %0, %2\n\t"                         \
        "spm\n\t"                                \
        :                                        \
        : "i" (_SFR_MEM_ADDR(__SPM_REG)),        \
          "i" (_SFR_MEM_ADDR(RAMPZ)),            \
          "r" ((uint8_t)(__BOOT_PAGE_ERASE)),    \
          "r" ((uint32_t)(address))              \
        : "r30", "r31"                           \
    );                                           \
}))

#define __boot_page_write_normal(address)        \
(__extension__({                                 \
    __asm__ __volatile__                         \
    (                                            \
        "sts %0, %1\n\t"                         \
        "spm\n\t"                                \
        :                                        \
        : "i" (_SFR_MEM_ADDR(__SPM_REG)),        \
          "r" ((uint8_t)(__BOOT_PAGE_WRITE)),    \
          "z" ((uint16_t)(address))              \
    );                                           \
}))

#define __boot_page_write_alternate(address)     \
(__extension__({                                 \
    __asm__ __volatile__                         \
    (                                            \
        "sts %0, %1\n\t"                         \
        "spm\n\t"                                \
        ".word 0xffff\n\t"                       \
        "nop\n\t"                                \
        :                                        \
        : "i" (_SFR_MEM_ADDR(__SPM_REG)),        \
          "r" ((uint8_t)(__BOOT_PAGE_WRITE)),    \
          "z" ((uint16_t)(address))              \
    );                                           \
}))

#define __boot_page_write_extended(address)      \
(__extension__({                                 \
    __asm__ __volatile__                         \
    (                                            \
        "movw r30, %A3\n\t"                      \
        "sts %1, %C3\n\t"                        \
        "sts %0, %2\n\t"                         \
        "spm\n\t"                                \
        :                                        \
        : "i" (_SFR_MEM_ADDR(__SPM_REG)),        \
          "i" (_SFR_MEM_ADDR(RAMPZ)),            \
          "r" ((uint8_t)(__BOOT_PAGE_WRITE)),    \
          "r" ((uint32_t)(address))              \
        : "r30", "r31"                           \
    );                                           \
}))

#define __boot_rww_enable()                      \
(__extension__({                                 \
    __asm__ __volatile__                         \
    (                                            \
        "sts %0, %1\n\t"                         \
        "spm\n\t"                                \
        :                                        \
        : "i" (_SFR_MEM_ADDR(__SPM_REG)),        \
          "r" ((uint8_t)(__BOOT_RWW_ENABLE))     \
    );                                           \
}))

#define __boot_rww_enable_alternate()            \
(__extension__({                                 \
    __asm__ __volatile__                         \
    (                                            \
        "sts %0, %1\n\t"                         \
        "spm\n\t"                                \
        ".word 0xffff\n\t"                       \
        "nop\n\t"                                \
        :                                        \
        : "i" (_SFR_MEM_ADDR(__SPM_REG)),        \
          "r" ((uint8_t)(__BOOT_RWW_ENABLE))     \
    );                                           \
}))

#define __boot_lock_bits_set(lock_bits)                    \
(__extension__({                                           \
    uint8_t value = (uint8_t)(~(lock_bits));               \
    __asm__ __volatile__                                   \
    (                                                      \
        "ldi r30, 1\n\t"                                   \
        "ldi r31, 0\n\t"                                   \
        "mov r0, %2\n\t"                                   \
        "sts %0, %1\n\t"                                   \
        "spm\n\t"                                          \
        :                                                  \
        : "i" (_SFR_MEM_ADDR(__SPM_REG)),                  \
          "r" ((uint8_t)(__BOOT_LOCK_BITS_SET)),           \
          "r" (value)                                      \
        : "r0", "r30", "r31"                               \
    );                                                     \
}))

#define __boot_lock_bits_set_alternate(lock_bits)          \
(__extension__({                                           \
    uint8_t value = (uint8_t)(~(lock_bits));               \
    __asm__ __volatile__                                   \
    (                                                      \
        "ldi r30, 1\n\t"                                   \
        "ldi r31, 0\n\t"                                   \
        "mov r0, %2\n\t"                                   \
        "sts %0, %1\n\t"                                   \
        "spm\n\t"                                          \
        ".word 0xffff\n\t"                                 \
        "nop\n\t"                                          \
        :                                                  \
        : "i" (_SFR_MEM_ADDR(__SPM_REG)),                  \
          "r" ((uint8_t)(__BOOT_LOCK_BITS_SET)),           \
          "r" (value)                                      \
        : "r0", "r30", "r31"                               \
    );                                                     \
}))
#endif	/* __DOXYGEN__ */

#define GET_LOW_FUSE_BITS           (0x0000)
#define GET_LOCK_BITS               (0x0001)
#define GET_EXTENDED_FUSE_BITS      (0x0002)
#define GET_HIGH_FUSE_BITS          (0x0003)

#define boot_lock_fuse_bits_get(address)                   \
(__extension__({                                           \
    uint8_t __result;                                      \
    __asm__ __volatile__                                   \
    (                                                      \
        "sts %1, %2\n\t"                                   \
        "lpm %0, Z\n\t"                                    \
        : "=r" (__result)                                  \
        : "i" (_SFR_MEM_ADDR(__SPM_REG)),                  \
          "r" ((uint8_t)(__BOOT_LOCK_BITS_SET)),           \
          "z" ((uint16_t)(address))                        \
    );                                                     \
    __result;                                              \
}))

#ifndef __DOXYGEN__
#define __BOOT_SIGROW_READ (_BV(__SPM_ENABLE) | _BV(SIGRD))
#endif

#define boot_signature_byte_get(addr) \
(__extension__({                      \
      uint8_t __result;                         \
      __asm__ __volatile__                      \
      (                                         \
        "sts %1, %2\n\t"                        \
        "lpm %0, Z" "\n\t"                      \
        : "=r" (__result)                       \
        : "i" (_SFR_MEM_ADDR(__SPM_REG)),       \
          "r" ((uint8_t)(__BOOT_SIGROW_READ)),  \
          "z" ((uint16_t)(addr))                \
      );                                        \
      __result;                                 \
}))

#if defined(__AVR_ATmega161__) || defined(__AVR_ATmega163__) \
    || defined(__AVR_ATmega323__)

#define boot_page_fill(address, data) __boot_page_fill_alternate(address, data)
#define boot_page_erase(address)      __boot_page_erase_alternate(address)
#define boot_page_write(address)      __boot_page_write_alternate(address)
#define boot_rww_enable()             __boot_rww_enable_alternate()
#define boot_lock_bits_set(lock_bits) __boot_lock_bits_set_alternate(lock_bits)

#elif (FLASHEND > USHRT_MAX)

#define boot_page_fill(address, data) __boot_page_fill_extended(address, data)
#define boot_page_erase(address)      __boot_page_erase_extended(address)
#define boot_page_write(address)      __boot_page_write_extended(address)
#define boot_rww_enable()             __boot_rww_enable()
#define boot_lock_bits_set(lock_bits) __boot_lock_bits_set(lock_bits)
#else
#define boot_page_fill(address, data) __boot_page_fill_normal(address, data)
#define boot_page_erase(address)      __boot_page_erase_normal(address)
#define boot_page_write(address)      __boot_page_write_normal(address)
#define boot_rww_enable()             __boot_rww_enable()
#define boot_lock_bits_set(lock_bits) __boot_lock_bits_set(lock_bits)
#endif

#define boot_page_fill_safe(address, data) \
do { \
    boot_spm_busy_wait();                       \
    eeprom_busy_wait();                         \
    boot_page_fill(address, data);              \
} while (0)

/** \ingroup avr_boot

    Same as boot_page_erase() except it waits for eeprom and spm operations to
    complete before erasing the page. */

#define boot_page_erase_safe(address) \
do { \
    boot_spm_busy_wait();                       \
    eeprom_busy_wait();                         \
    boot_page_erase (address);                  \
} while (0)

/** \ingroup avr_boot

    Same as boot_page_write() except it waits for eeprom and spm operations to
    complete before writing the page. */

#define boot_page_write_safe(address) \
do { \
    boot_spm_busy_wait();                       \
    eeprom_busy_wait();                         \
    boot_page_write (address);                  \
} while (0)

/** \ingroup avr_boot

    Same as boot_rww_enable() except waits for eeprom and spm operations to
    complete before enabling the RWW mameory. */

#define boot_rww_enable_safe() \
do { \
    boot_spm_busy_wait();                       \
    eeprom_busy_wait();                         \
    boot_rww_enable();                          \
} while (0)

/** \ingroup avr_boot

    Same as boot_lock_bits_set() except waits for eeprom and spm operations to
    complete before setting the lock bits. */

#define boot_lock_bits_set_safe(lock_bits) \
do { \
    boot_spm_busy_wait();                       \
    eeprom_busy_wait();                         \
    boot_lock_bits_set (lock_bits);             \
} while (0)

#endif



