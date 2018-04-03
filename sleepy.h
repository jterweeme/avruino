#ifndef _AVR_SLEEP_H_
#define _AVR_SLEEP_H_ 1

#include <avr/io.h>
#include "types.h"

#if defined(SLEEP_CTRL)

#define _SLEEP_CONTROL_REG  SLEEP_CTRL
#define _SLEEP_ENABLE_MASK  SLEEP_SEN_bm
#define _SLEEP_SMODE_GROUP_MASK  SLEEP_SMODE_gm

#elif defined(SLPCTRL)

#define _SLEEP_CONTROL_REG  SLPCTRL_CTRLA
#define _SLEEP_ENABLE_MASK  SLPCTRL_SEN_bm
#define _SLEEP_SMODE_GROUP_MASK  SLPCTRL_SMODE_gm

#elif defined(SMCR)
    #define _SLEEP_CONTROL_REG  SMCR
    #define _SLEEP_ENABLE_MASK  _BV(SE)
#elif defined(__AVR_AT94K__)
#define _SLEEP_CONTROL_REG  MCUR
#define _SLEEP_ENABLE_MASK  _BV(SE)
#elif !defined(__DOXYGEN__)
#define _SLEEP_CONTROL_REG  MCUCR
#define _SLEEP_ENABLE_MASK  _BV(SE)
#endif


/* Special casing these three devices - they are the
   only ones that need to write to more than one register. */
#if defined(__AVR_ATmega161__)

    #define set_sleep_mode(mode) \
    do { \
        MCUCR = ((MCUCR & ~_BV(SM1)) | ((mode) == SLEEP_MODE_PWR_DOWN || (mode) == SLEEP_MODE_PWR_SAVE ? _BV(SM1) : 0)); \
        EMCUCR = ((EMCUCR & ~_BV(SM0)) | ((mode) == SLEEP_MODE_PWR_SAVE ? _BV(SM0) : 0)); \
    } while(0)


#elif defined(__AVR_ATmega162__) \
|| defined(__AVR_ATmega8515__)

    #define set_sleep_mode(mode) \
    do { \
        MCUCR = ((MCUCR & ~_BV(SM1)) | ((mode) == SLEEP_MODE_IDLE ? 0 : _BV(SM1))); \
        MCUCSR = ((MCUCSR & ~_BV(SM2)) | ((mode) == SLEEP_MODE_STANDBY  || (mode) == SLEEP_MODE_EXT_STANDBY ? _BV(SM2) : 0)); \
        EMCUCR = ((EMCUCR & ~_BV(SM0)) | ((mode) == SLEEP_MODE_PWR_SAVE || (mode) == SLEEP_MODE_EXT_STANDBY ? _BV(SM0) : 0)); \
    } while(0)

/* For xmegas, check presence of SLEEP_SMODE<n>_bm and define set_sleep_mode accordingly. */
#elif defined(__AVR_XMEGA__)

#define set_sleep_mode(mode) \
  do { \
    _SLEEP_CONTROL_REG = ((_SLEEP_CONTROL_REG & ~(_SLEEP_SMODE_GROUP_MASK)) | (mode)); \
  } while(0)

/* For everything else, check for presence of SM<n> and define set_sleep_mode accordingly. */
#else
#if defined(SM2)

    #define set_sleep_mode(mode) \
    do { \
        _SLEEP_CONTROL_REG = ((_SLEEP_CONTROL_REG & ~(_BV(SM0) | _BV(SM1) | _BV(SM2))) | (mode)); \
    } while(0)

#elif defined(SM1)

    #define set_sleep_mode(mode) \
    do { \
        _SLEEP_CONTROL_REG = ((_SLEEP_CONTROL_REG & ~(_BV(SM0) | _BV(SM1))) | (mode)); \
    } while(0)

#elif defined(SM)

    #define set_sleep_mode(mode) \
    do { \
        _SLEEP_CONTROL_REG = ((_SLEEP_CONTROL_REG & ~_BV(SM)) | (mode)); \
    } while(0)

#else

    #error "No SLEEP mode defined for this device."

#endif /* if defined(SM2) */
#endif /* #if defined(__AVR_ATmega161__) */



/** \ingroup avr_sleep

    Put the device in sleep mode. How the device is brought out of sleep mode
    depends on the specific mode selected with the set_sleep_mode() function.
    See the data sheet for your device for more details. */


#if defined(__DOXYGEN__)

/** \ingroup avr_sleep

    Set the SE (sleep enable) bit.
*/
extern void sleep_enable (void);

#else

#define sleep_enable()             \
do {                               \
  _SLEEP_CONTROL_REG |= (uint8_t)_SLEEP_ENABLE_MASK;   \
} while(0)

#endif


#if defined(__DOXYGEN__)

/** \ingroup avr_sleep

    Clear the SE (sleep enable) bit.
*/
extern void sleep_disable (void);

#else

#define sleep_disable()            \
do {                               \
  _SLEEP_CONTROL_REG &= (uint8_t)(~_SLEEP_ENABLE_MASK);  \
} while(0)

#endif


/** \ingroup avr_sleep

    Put the device into sleep mode.  The SE bit must be set
    beforehand, and it is recommended to clear it afterwards.
*/
#if defined(__DOXYGEN__)

extern void sleep_cpu (void);

#else

#define sleep_cpu()                              \
do {                                             \
  __asm__ __volatile__ ( "sleep" "\n\t" :: );    \
} while(0)

#endif


#if defined(__DOXYGEN__)

/** \ingroup avr_sleep

    Put the device into sleep mode, taking care of setting
    the SE bit before, and clearing it afterwards. */
extern void sleep_mode (void);

#else

#define sleep_mode() \
do {                 \
    sleep_enable();  \
    sleep_cpu();     \
    sleep_disable(); \
} while (0)

#endif


#if defined(__DOXYGEN__)

/** \ingroup avr_sleep

    Disable BOD before going to sleep.
    Not available on all devices.
*/
extern void sleep_bod_disable (void);

#else

#if defined(BODS) && defined(BODSE)

#ifdef BODCR

#define BOD_CONTROL_REG BODCR

#else

#define BOD_CONTROL_REG MCUCR

#endif

#define sleep_bod_disable() \
do { \
  uint8_t tempreg; \
  __asm__ __volatile__("in %[tempreg], %[mcucr]" "\n\t" \
                       "ori %[tempreg], %[bods_bodse]" "\n\t" \
                       "out %[mcucr], %[tempreg]" "\n\t" \
                       "andi %[tempreg], %[not_bodse]" "\n\t" \
                       "out %[mcucr], %[tempreg]" \
                       : [tempreg] "=&d" (tempreg) \
                       : [mcucr] "I" _SFR_IO_ADDR(BOD_CONTROL_REG), \
                         [bods_bodse] "i" (_BV(BODS) | _BV(BODSE)), \
                         [not_bodse] "i" (~_BV(BODSE))); \
} while (0)

#endif

#endif

#endif



