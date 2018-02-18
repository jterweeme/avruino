#include <string.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <avr/boot.h>
#include <math.h>

#ifndef BOARD
#define BOARD BOARD_LEONARDO
#endif

#ifndef ARCH
#define ARCH ARCH_AVR8
#endif

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#ifndef F_USB
#define F_USB 16000000UL
#endif

#include <util/delay.h>

#define USE_STATIC_OPTIONS (USB_DEVICE_OPT_FULLSPEED | USB_OPT_REG_ENABLED | USB_OPT_AUTO_PLL)
#define USB_DEVICE_ONLY
#define USE_FLASH_DESCRIPTORS
#define FIXED_CONTROL_ENDPOINT_SIZE      8
#define FIXED_NUM_CONFIGURATIONS         1

#define __INCLUDE_FROM_USB_DRIVER
#define __INCLUDE_FROM_RNDIS_DRIVER
#define __INCLUDE_FROM_CDC_DRIVER
#define __INCLUDE_FROM_USBTASK_C
#define __INCLUDE_FROM_USB_CONTROLLER_C
#define __INCLUDE_FROM_DEVICESTDREQ_C
#define __INCLUDE_FROM_COMMON_H


#define ENDPOINT_DIR_MASK                  0x80
#define ENDPOINT_DIR_OUT                   0x00
#define ENDPOINT_DIR_IN                    0x80
#define PIPE_DIR_MASK                      0x80
#define PIPE_DIR_OUT                       0x00
#define PIPE_DIR_IN                        0x80
#define EP_TYPE_MASK                       0x03
#define EP_TYPE_CONTROL                    0x00
#define EP_TYPE_ISOCHRONOUS                0x01
#define EP_TYPE_BULK                       0x02
#define EP_TYPE_INTERRUPT                  0x03

enum USB_Modes_t
{   
    USB_MODE_None   = 0,
    USB_MODE_Device = 1,
    USB_MODE_Host   = 2,
    USB_MODE_UID    = 3,
};  

#define ARCH_AVR8           0
#define ARCH_UC3            1
#define ARCH_XMEGA          2

#define ARCH_           ARCH_AVR8
#define ARCH        ARCH_AVR8

#define BOARD_USER                 0
#define BOARD_NONE                 1
#define BOARD_USBKEY               2
#define BOARD_STK525               3
#define BOARD_STK526               4
#define BOARD_RZUSBSTICK           5
#define BOARD_ATAVRUSBRF01         6
#define BOARD_BUMBLEB              7
#define BOARD_XPLAIN               8
#define BOARD_XPLAIN_REV1          9
#define BOARD_EVK527               10
#define BOARD_TEENSY               11
#define BOARD_USBTINYMKII          12
#define BOARD_BENITO               13
#define BOARD_JMDBU2               14
#define BOARD_OLIMEX162            15
#define BOARD_UDIP                 16
#define BOARD_BUI                  17
#define BOARD_UNO                  18
#define BOARD_CULV3                19
#define BOARD_BLACKCAT             20
#define BOARD_MAXIMUS              21
#define BOARD_MINIMUS              22
#define BOARD_ADAFRUITU4           23
#define BOARD_MICROSIN162          24
#define BOARD_USBFOO               25
#define BOARD_SPARKFUN8U2          26
#define BOARD_EVK1101              27
#define BOARD_TUL                  28
#define BOARD_EVK1100              29
#define BOARD_EVK1104              30
#define BOARD_A3BU_XPLAINED        31
#define BOARD_TEENSY2              32
#define BOARD_USB2AX               33
#define BOARD_USB2AX_V3            34
#define BOARD_MICROPENDOUS_32U2    35
#define BOARD_MICROPENDOUS_A       36
#define BOARD_MICROPENDOUS_1       37
#define BOARD_MICROPENDOUS_2       38
#define BOARD_MICROPENDOUS_3       39
#define BOARD_MICROPENDOUS_4       40
#define BOARD_MICROPENDOUS_DIP     41
#define BOARD_MICROPENDOUS_REV1    42
#define BOARD_MICROPENDOUS_REV2    43
#define BOARD_B1_XPLAINED          44
#define BOARD_MULTIO               45
#define BOARD_BIGMULTIO            46
#define BOARD_DUCE                 47
#define BOARD_OLIMEX32U4           48
#define BOARD_OLIMEXT32U4          49
#define BOARD_OLIMEXISPMK2         50
#define BOARD_LEONARDO             51
#define BOARD_UC3A3_XPLAINED       52
#define BOARD_USB2AX_V31           53
#define BOARD_STANGE_ISP           54
#define BOARD_C3_XPLAINED          55
#define BOARD_U2S                  56
#define BOARD_YUN                  57
#define BOARD_MICRO                58
#define BOARD_POLOLUMICRO          59
#define BOARD_XPLAINED_MINI        60

#ifndef __USBCONTROLLER_AVR8_H__
#define __USBCONTROLLER_AVR8_H__

#ifndef __USBTASK_H__
#define __USBTASK_H__

#ifndef __USBMODE_H__
#define __USBMODE_H__

#ifndef __LUFA_COMMON_H__
#define __LUFA_COMMON_H__




#ifndef __LUFA_BOARDTYPES_H__
#define __LUFA_BOARDTYPES_H__

#if !defined(__DOXYGEN__)
#define BOARD_                 BOARD_NONE

#if !defined(BOARD)
#define BOARD              BOARD_NONE
#endif
#endif

#endif


#ifndef __LUFA_COMPILERSPEC_H__
#define __LUFA_COMPILERSPEC_H__

#if defined(__GNUC__) || defined(__DOXYGEN__)
#define GCC_FORCE_POINTER_ACCESS(StructPtr)   \
__asm__ __volatile__("" : "=b" (StructPtr) : "0" (StructPtr))


#define GCC_MEMORY_BARRIER()                  __asm__ __volatile__("" ::: "memory");
#define GCC_IS_COMPILE_CONST(x)               __builtin_constant_p(x)
#else
#define GCC_FORCE_POINTER_ACCESS(StructPtr)
#define GCC_MEMORY_BARRIER()
#define GCC_IS_COMPILE_CONST(x)               0
#endif

#endif

#ifndef __LUFA_ATTR_H__
#define __LUFA_ATTR_H__

#if (__GNUC__ >= 3) || defined(__DOXYGEN__)
#define ATTR_NO_RETURN               __attribute__ ((noreturn))
#define ATTR_WARN_UNUSED_RESULT      __attribute__ ((warn_unused_result))
#define ATTR_NON_NULL_PTR_ARG(...)   __attribute__ ((nonnull (__VA_ARGS__)))
#define ATTR_NAKED                   __attribute__ ((naked))
#define ATTR_NO_INLINE               __attribute__ ((noinline))
#define ATTR_ALWAYS_INLINE           __attribute__ ((always_inline))
#define ATTR_PURE                    __attribute__ ((pure))
#define ATTR_CONST                   __attribute__ ((const))
#define ATTR_DEPRECATED              __attribute__ ((deprecated))
#define ATTR_WEAK                    __attribute__ ((weak))
#endif

#define ATTR_NO_INIT                     __attribute__ ((section (".noinit")))

#define ATTR_INIT_SECTION(SectionIndex)  \
__attribute__ ((used, naked, section (".init" #SectionIndex )))

#define ATTR_ALIAS(Func)                 __attribute__ ((alias( #Func )))
#define ATTR_PACKED                      __attribute__ ((packed))
#define ATTR_ALIGNED(Bytes)              __attribute__ ((aligned(Bytes)))
#endif


typedef uint8_t uint_reg_t;

#define ARCH_HAS_EEPROM_ADDRESS_SPACE
#define ARCH_HAS_FLASH_ADDRESS_SPACE
#define ARCH_HAS_MULTI_ADDRESS_SPACE
#define ARCH_LITTLE_ENDIAN

#ifndef __LUFA_ENDIANNESS_H__
#define __LUFA_ENDIANNESS_H__

#if !(defined(ARCH_BIG_ENDIAN) || defined(ARCH_LITTLE_ENDIAN))
#error ARCH_BIG_ENDIAN or ARCH_LITTLE_ENDIAN not set for the specified architecture.
#endif

#define SWAPENDIAN_16(x)   (uint16_t)((((x) & 0xFF00) >> 8) | (((x) & 0x00FF) << 8))

#define SWAPENDIAN_32(x) \
  (uint32_t)((((x) & 0xFF000000UL) >> 24UL) | (((x) & 0x00FF0000UL) >> 8UL) | \
                           (((x) & 0x0000FF00UL) << 8UL)  | (((x) & 0x000000FFUL) << 24UL))

#if defined(ARCH_BIG_ENDIAN) && !defined(le16_to_cpu)
#define le16_to_cpu(x)           SwapEndian_16(x)
#define le32_to_cpu(x)           SwapEndian_32(x)
#define be16_to_cpu(x)           (x)
#define be32_to_cpu(x)           (x)
#define cpu_to_le16(x)           SwapEndian_16(x)
#define cpu_to_le32(x)           SwapEndian_32(x)
#define cpu_to_be16(x)           (x)
#define cpu_to_be32(x)           (x)
#define LE16_TO_CPU(x)           SWAPENDIAN_16(x)
#define LE32_TO_CPU(x)           SWAPENDIAN_32(x)
#define BE16_TO_CPU(x)           (x)
#define BE32_TO_CPU(x)           (x)
#define CPU_TO_LE16(x)           SWAPENDIAN_16(x)
#define CPU_TO_LE32(x)           SWAPENDIAN_32(x)
#define CPU_TO_BE16(x)           (x)
#define CPU_TO_BE32(x)           (x)
#elif !defined(le16_to_cpu)
#define le16_to_cpu(x)           (x)
#define le32_to_cpu(x)           (x)
#define be16_to_cpu(x)           SwapEndian_16(x)
#define be32_to_cpu(x)           SwapEndian_32(x)
#define cpu_to_le16(x)           (x)
#define cpu_to_le32(x)           (x)
#define cpu_to_be16(x)           SwapEndian_16(x)
#define cpu_to_be32(x)           SwapEndian_32(x)
#define LE16_TO_CPU(x)           (x)
#define LE32_TO_CPU(x)           (x)
#define BE16_TO_CPU(x)           SWAPENDIAN_16(x)
#define BE32_TO_CPU(x)           SWAPENDIAN_32(x)
#define CPU_TO_LE16(x)           (x)
#define CPU_TO_LE32(x)           (x)
#define CPU_TO_BE16(x)           SWAPENDIAN_16(x)
#define CPU_TO_BE32(x)           SWAPENDIAN_32(x)
#endif

static inline uint16_t
SwapEndian_16(const uint16_t Word) ATTR_WARN_UNUSED_RESULT ATTR_CONST ATTR_ALWAYS_INLINE;

static inline uint16_t SwapEndian_16(const uint16_t Word)
{
                if (GCC_IS_COMPILE_CONST(Word))
                  return SWAPENDIAN_16(Word);

                uint8_t Temp;

                union
                {
                    uint16_t Word;
                    uint8_t  Bytes[2];
                } Data;

                Data.Word = Word;

                Temp = Data.Bytes[0];
                Data.Bytes[0] = Data.Bytes[1];
                Data.Bytes[1] = Temp;

                return Data.Word;
}

static inline uint32_t
SwapEndian_32(const uint32_t DWord) ATTR_WARN_UNUSED_RESULT ATTR_CONST ATTR_ALWAYS_INLINE;

static inline uint32_t SwapEndian_32(const uint32_t DWord)
{
                if (GCC_IS_COMPILE_CONST(DWord))
                  return SWAPENDIAN_32(DWord);

                uint8_t Temp;

                union
                {
                    uint32_t DWord;
                    uint8_t  Bytes[4];
                } Data;

                Data.DWord = DWord;

                Temp = Data.Bytes[0];
                Data.Bytes[0] = Data.Bytes[3];
                Data.Bytes[3] = Temp;

                Temp = Data.Bytes[1];
                Data.Bytes[1] = Data.Bytes[2];
                Data.Bytes[2] = Temp;

                return Data.DWord;
}

static inline void SwapEndian_n(void* const Data, uint8_t Length) ATTR_NON_NULL_PTR_ARG(1);
static inline void SwapEndian_n(void* const Data, uint8_t Length)
{
                uint8_t* CurrDataPos = (uint8_t*)Data;

                while (Length > 1)
                {
                    uint8_t Temp = *CurrDataPos;
                    *CurrDataPos = *(CurrDataPos + Length - 1);
                    *(CurrDataPos + Length - 1) = Temp;

                    CurrDataPos++;
                    Length -= 2;
                }
}

#endif

#define MACROS do
#define MACROE while (0)

#if !defined(MAX) || defined(__DOXYGEN__)
#define MAX(x, y)               (((x) > (y)) ? (x) : (y))
#endif

#if !defined(MIN) || defined(__DOXYGEN__)
#define MIN(x, y)               (((x) < (y)) ? (x) : (y))
#endif

#if !defined(STRINGIFY) || defined(__DOXYGEN__)
#define STRINGIFY(x)            #x
#define STRINGIFY_EXPANDED(x)   STRINGIFY(x)
#endif

#if !defined(CONCAT) || defined(__DOXYGEN__)
#define CONCAT(x, y)            x ## y
#define CONCAT_EXPANDED(x, y)   CONCAT(x, y)
#endif

#if !defined(ISR) || defined(__DOXYGEN__)
#define ISR(Name, ...) \
    void Name (void) __attribute__((__interrupt__)) __VA_ARGS__; void Name (void)
#endif

static inline uint8_t BitReverse(uint8_t Byte) ATTR_WARN_UNUSED_RESULT ATTR_CONST;
static inline uint8_t BitReverse(uint8_t Byte)
{
    Byte = (((Byte & 0xF0) >> 4) | ((Byte & 0x0F) << 4));
    Byte = (((Byte & 0xCC) >> 2) | ((Byte & 0x33) << 2));
    Byte = (((Byte & 0xAA) >> 1) | ((Byte & 0x55) << 1));
    return Byte;
}

static inline void Delay_MS(uint16_t Milliseconds) ATTR_ALWAYS_INLINE;
static inline void Delay_MS(uint16_t Milliseconds)
{
    if (GCC_IS_COMPILE_CONST(Milliseconds))
    {
        _delay_ms(Milliseconds);
    }
    else
    {
        while (Milliseconds--)
            _delay_ms(1);
    }
}


static inline uint_reg_t GetGlobalInterruptMask(void) ATTR_ALWAYS_INLINE ATTR_WARN_UNUSED_RESULT;
static inline uint_reg_t GetGlobalInterruptMask(void)
{
    GCC_MEMORY_BARRIER();
    return SREG;
}

static inline void SetGlobalInterruptMask(const uint_reg_t GlobalIntState) ATTR_ALWAYS_INLINE;
static inline void SetGlobalInterruptMask(const uint_reg_t GlobalIntState)
{
    GCC_MEMORY_BARRIER();
    SREG = GlobalIntState;
    GCC_MEMORY_BARRIER();
}

static inline void GlobalInterruptEnable(void) ATTR_ALWAYS_INLINE;
static inline void GlobalInterruptEnable(void)
{
    GCC_MEMORY_BARRIER();
    sei();
    GCC_MEMORY_BARRIER();
}

static inline void GlobalInterruptDisable(void) ATTR_ALWAYS_INLINE;
static inline void GlobalInterruptDisable(void)
{
    GCC_MEMORY_BARRIER();
    cli();
    GCC_MEMORY_BARRIER();
}
#endif

#if !defined(__DOXYGEN__)
#define USB_SERIES_4_AVR
#define USB_CAN_BE_DEVICE

#if (defined(USB_HOST_ONLY) && defined(USB_DEVICE_ONLY))
#error USB_HOST_ONLY and USB_DEVICE_ONLY are mutually exclusive.
#elif defined(USB_HOST_ONLY)
#if !defined(USB_CAN_BE_HOST)
#error USB_HOST_ONLY is not available for the currently selected microcontroller model.
#else
#undef USB_CAN_BE_DEVICE
#endif
#elif defined(USB_DEVICE_ONLY)
#undef USB_CAN_BE_HOST
#endif
#endif
#endif


#define CONTROL_REQTYPE_DIRECTION  0x80
#define CONTROL_REQTYPE_TYPE       0x60
#define CONTROL_REQTYPE_RECIPIENT  0x1F
#define REQDIR_HOSTTODEVICE        (0 << 7)
#define REQDIR_DEVICETOHOST        (1 << 7)
#define REQTYPE_STANDARD           (0 << 5)
#define REQTYPE_CLASS              (1 << 5)
#define REQTYPE_VENDOR             (2 << 5)
#define REQREC_DEVICE              (0 << 0)
#define REQREC_INTERFACE           (1 << 0)
#define REQREC_ENDPOINT            (2 << 0)
#define REQREC_OTHER               (3 << 0)

typedef struct
{
    uint8_t  bmRequestType; /**< Type of the request. */
    uint8_t  bRequest; /**< Request command code. */
    uint16_t wValue; /**< wValue parameter of the request. */
    uint16_t wIndex; /**< wIndex parameter of the request. */
    uint16_t wLength; /**< Length of the data to transfer in bytes. */
} ATTR_PACKED USB_Request_Header_t;

enum USB_Control_Request_t
{   
    REQ_GetStatus           = 0,
    REQ_ClearFeature        = 1,
    REQ_SetFeature          = 3,
    REQ_SetAddress          = 5,
    REQ_GetDescriptor       = 6,
    REQ_SetDescriptor       = 7,
    REQ_GetConfiguration    = 8,
    REQ_SetConfiguration    = 9,
    REQ_GetInterface        = 10,
    REQ_SetInterface        = 11,
    REQ_SynchFrame          = 12
};  

enum USB_Feature_Selectors_t
{
    FEATURE_SEL_EndpointHalt       = 0x00,
    FEATURE_SEL_DeviceRemoteWakeup = 0x01,
    FEATURE_SEL_TestMode           = 0x02,
};




#define FEATURE_SELFPOWERED_ENABLED     (1 << 0)
#define FEATURE_REMOTE_WAKEUP_ENABLED   (1 << 1)


extern volatile bool USB_IsInitialized;
extern USB_Request_Header_t USB_ControlRequest;

#if !defined(DEVICE_STATE_AS_GPIOR) || defined(__DOXYGEN__)
extern volatile uint8_t USB_DeviceState;
#else
#define USB_DeviceState            CONCAT_EXPANDED(GPIOR, DEVICE_STATE_AS_GPIOR)
#endif

void USB_USBTask(void);

static void USB_DeviceTask(void);

#define HOST_TASK_NONBLOCK_WAIT(Duration, NextState) do { USB_HostState = \
    HOST_STATE_WaitForDevice; \
    WaitMSRemaining = (Duration);               \
    PostWaitState   = (NextState);              } while (0)


#endif






enum USB_Interrupts_t
{
    USB_INT_VBUSTI  = 0,
    USB_INT_WAKEUPI = 2,
    USB_INT_SUSPI   = 3,
    USB_INT_EORSTI  = 4,
    USB_INT_SOFI    = 5,
    USB_INT_RXSTPI  = 6,
};

static inline void USB_INT_Enable(const uint8_t Interrupt) ATTR_ALWAYS_INLINE;
static inline void USB_INT_Enable(const uint8_t Interrupt)
{
    switch (Interrupt)
    {
    case USB_INT_VBUSTI:
        USBCON |= (1 << VBUSTE);
        break;
    case USB_INT_WAKEUPI:
        UDIEN  |= (1 << WAKEUPE);
        break;
    case USB_INT_SUSPI:
        UDIEN  |= (1 << SUSPE);
        break;
    case USB_INT_EORSTI:
        UDIEN  |= (1 << EORSTE);
        break;
    case USB_INT_SOFI:
        UDIEN  |= (1 << SOFE);
        break;
    case USB_INT_RXSTPI:
        UEIENX |= (1 << RXSTPE);
        break;
    default:
        break;
    }
}

static inline void USB_INT_Disable(const uint8_t Interrupt) ATTR_ALWAYS_INLINE;
static inline void USB_INT_Disable(const uint8_t Interrupt)
{
    switch (Interrupt)
    {
                    case USB_INT_VBUSTI:
                        USBCON &= ~(1 << VBUSTE);
                        break;
                    case USB_INT_WAKEUPI:
                        UDIEN  &= ~(1 << WAKEUPE);
                        break;
                    case USB_INT_SUSPI:
                        UDIEN  &= ~(1 << SUSPE);
                        break;
                    case USB_INT_EORSTI:
                        UDIEN  &= ~(1 << EORSTE);
                        break;
                    case USB_INT_SOFI:
                        UDIEN  &= ~(1 << SOFE);
                        break;
                    case USB_INT_RXSTPI:
                        UEIENX &= ~(1 << RXSTPE);
                        break;
                    default:
                        break;
                }
            }

static inline void USB_INT_Clear(const uint8_t Interrupt) ATTR_ALWAYS_INLINE;
static inline void USB_INT_Clear(const uint8_t Interrupt)
{
    switch (Interrupt)
    {
                    case USB_INT_VBUSTI:
                        USBINT &= ~(1 << VBUSTI);
                        break;
                    case USB_INT_WAKEUPI:
                        UDINT  &= ~(1 << WAKEUPI);
                        break;
                    case USB_INT_SUSPI:
                        UDINT  &= ~(1 << SUSPI);
                        break;
                    case USB_INT_EORSTI:
                        UDINT  &= ~(1 << EORSTI);
                        break;
                    case USB_INT_SOFI:
                        UDINT  &= ~(1 << SOFI);
                        break;
                    case USB_INT_RXSTPI:
                        UEINTX &= ~(1 << RXSTPI);
                        break;
                    default:
                        break;
    }
}

static inline bool
USB_INT_IsEnabled(const uint8_t Interrupt) ATTR_ALWAYS_INLINE ATTR_WARN_UNUSED_RESULT;

static inline bool USB_INT_IsEnabled(const uint8_t Interrupt)
{
    switch (Interrupt)
    {
                    case USB_INT_VBUSTI:
                        return (USBCON & (1 << VBUSTE));
                    case USB_INT_WAKEUPI:
                        return (UDIEN  & (1 << WAKEUPE));
                    case USB_INT_SUSPI:
                        return (UDIEN  & (1 << SUSPE));
                    case USB_INT_EORSTI:
                        return (UDIEN  & (1 << EORSTE));
                    case USB_INT_SOFI:
                        return (UDIEN  & (1 << SOFE));
                    case USB_INT_RXSTPI:
                        return (UEIENX & (1 << RXSTPE));
                    default:
                        return false;
    }
}

static inline bool
USB_INT_HasOccurred(const uint8_t Interrupt) ATTR_ALWAYS_INLINE ATTR_WARN_UNUSED_RESULT;

static inline bool USB_INT_HasOccurred(const uint8_t Interrupt)
{
    switch (Interrupt)
    {
    case USB_INT_VBUSTI:
        return (USBINT & (1 << VBUSTI));
    case USB_INT_WAKEUPI:
        return (UDINT  & (1 << WAKEUPI));
    case USB_INT_SUSPI:
        return (UDINT  & (1 << SUSPI));
    case USB_INT_EORSTI:
        return (UDINT  & (1 << EORSTI));
    case USB_INT_SOFI:
        return (UDINT  & (1 << SOFI));
    case USB_INT_RXSTPI:
        return (UEINTX & (1 << RXSTPI));
    default:
        return false;
    }
}

void EVENT_USB_UIDChange(void);
void EVENT_USB_Host_HostError(const uint8_t ErrorCode);
void EVENT_USB_Host_DeviceAttached(void);
void EVENT_USB_Host_DeviceUnattached(void);

void EVENT_USB_Host_DeviceEnumerationFailed(const uint8_t ErrorCode,
                                                        const uint8_t SubErrorCode);

enum USB_Device_States_t
{
    DEVICE_STATE_Unattached = 0,
    DEVICE_STATE_Powered = 1,
    DEVICE_STATE_Default = 2,
    DEVICE_STATE_Addressed = 3,
    DEVICE_STATE_Configured = 4,
    DEVICE_STATE_Suspended = 5,
};

uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
           const uint16_t wIndex,
       const void** const DescriptorAddress) ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);


#define NO_DESCRIPTOR                     0
#define USB_CONFIG_POWER_MA(mA)           ((mA) >> 1)

#define USB_STRING_LEN(UnicodeChars)   (sizeof(USB_Descriptor_Header_t) + ((UnicodeChars) << 1))

#define USB_STRING_DESCRIPTOR(String)     \
{ .Header = {.Size = sizeof(USB_Descriptor_Header_t) + (sizeof(String) - 2), \
.Type = DTYPE_String}, .UnicodeString = String }


#define USB_STRING_DESCRIPTOR_ARRAY(...) \
 { .Header = {.Size = sizeof(USB_Descriptor_Header_t) + \
sizeof((uint16_t){__VA_ARGS__}), .Type = DTYPE_String}, .UnicodeString = {__VA_ARGS__} }


#define VERSION_BCD(Major, Minor, Revision) \
                                        CPU_TO_LE16( ((Major & 0xFF) << 8) | \
                                                 ((Minor & 0x0F) << 4) | \
                                               (Revision & 0x0F) )

#define LANGUAGE_ID_ENG                   0x0409


#define USB_CONFIG_ATTR_RESERVED          0x80
#define USB_CONFIG_ATTR_SELFPOWERED       0x40
#define USB_CONFIG_ATTR_REMOTEWAKEUP      0x20
#define ENDPOINT_ATTR_NO_SYNC             (0 << 2)

#define ENDPOINT_ATTR_ASYNC               (1 << 2)
#define ENDPOINT_ATTR_ADAPTIVE            (2 << 2)




#define ENDPOINT_ATTR_SYNC                (3 << 2)


#define ENDPOINT_USAGE_DATA               (0 << 4)

#define ENDPOINT_USAGE_FEEDBACK           (1 << 4)

#define ENDPOINT_USAGE_IMPLICIT_FEEDBACK  (2 << 4)

enum USB_DescriptorTypes_t
{
    DTYPE_Device                    = 0x01,
    DTYPE_Configuration             = 0x02,
    DTYPE_String                    = 0x03,
    DTYPE_Interface                 = 0x04,
    DTYPE_Endpoint                  = 0x05,
    DTYPE_DeviceQualifier           = 0x06,
    DTYPE_Other                     = 0x07,
    DTYPE_InterfacePower            = 0x08,
    DTYPE_InterfaceAssociation      = 0x0B,
    DTYPE_CSInterface               = 0x24,
    DTYPE_CSEndpoint                = 0x25,
};

enum USB_Descriptor_ClassSubclassProtocol_t
{
    USB_CSCP_NoDeviceClass          = 0x00,
    USB_CSCP_NoDeviceSubclass       = 0x00,
    USB_CSCP_NoDeviceProtocol       = 0x00,
    USB_CSCP_VendorSpecificClass    = 0xFF,
    USB_CSCP_VendorSpecificSubclass = 0xFF,
    USB_CSCP_VendorSpecificProtocol = 0xFF,
    USB_CSCP_IADDeviceClass         = 0xEF,
    USB_CSCP_IADDeviceSubclass      = 0x02,
    USB_CSCP_IADDeviceProtocol      = 0x01,
};

typedef struct
{
    uint8_t Size; /**< Size of the descriptor, in bytes. */
    uint8_t Type;
} ATTR_PACKED USB_Descriptor_Header_t;

typedef struct
{
    uint8_t bLength; /**< Size of the descriptor, in bytes. */
    uint8_t bDescriptorType;
} ATTR_PACKED USB_StdDescriptor_Header_t;

typedef struct
{
    USB_Descriptor_Header_t Header; /**< Descriptor header, including type and size. */
    uint16_t USBSpecification;
    uint8_t  Class; /**< USB device class. */
    uint8_t  SubClass; /**< USB device subclass. */
    uint8_t  Protocol; /**< USB device protocol. */
    uint8_t  Endpoint0Size; /**< Size of the control (address 0) endpoint's bank in bytes. */
    uint16_t VendorID; /**< Vendor ID for the USB product. */
    uint16_t ProductID; /**< Unique product ID for the USB product. */
    uint16_t ReleaseNumber;
    uint8_t  ManufacturerStrIndex;
    uint8_t  ProductStrIndex;
    uint8_t  SerialNumStrIndex;
    uint8_t  NumberOfConfigurations;
} ATTR_PACKED USB_Descriptor_Device_t;

typedef struct
{
    uint8_t  bLength; /**< Size of the descriptor, in bytes. */
    uint8_t  bDescriptorType;
    uint16_t bcdUSB;
    uint8_t  bDeviceClass; /**< USB device class. */
    uint8_t  bDeviceSubClass; /**< USB device subclass. */
    uint8_t  bDeviceProtocol; /**< USB device protocol. */
    uint8_t  bMaxPacketSize0; /**< Size of the control (address 0) endpoint's bank in bytes. */
    uint16_t idVendor; /**< Vendor ID for the USB product. */
    uint16_t idProduct; /**< Unique product ID for the USB product. */
    uint16_t bcdDevice;
    uint8_t  iManufacturer;
    uint8_t  iProduct;
    uint8_t iSerialNumber;
    uint8_t  bNumConfigurations;
} ATTR_PACKED USB_StdDescriptor_Device_t;

typedef struct
{
    USB_Descriptor_Header_t Header; /**< Descriptor header, including type and size. */
    uint16_t USBSpecification;
    uint8_t  Class; /**< USB device class. */
    uint8_t  SubClass; /**< USB device subclass. */
    uint8_t  Protocol; /**< USB device protocol. */
    uint8_t  Endpoint0Size; /**< Size of the control (address 0) endpoint's bank in bytes. */
    uint8_t  NumberOfConfigurations;
    uint8_t  Reserved; /**< Reserved for future use, must be 0. */
} ATTR_PACKED USB_Descriptor_DeviceQualifier_t;

typedef struct
{
    uint8_t  bLength; /**< Size of the descriptor, in bytes. */
    uint8_t  bDescriptorType;
    uint16_t bcdUSB;
    uint8_t  bDeviceClass; /**< USB device class. */
    uint8_t  bDeviceSubClass; /**< USB device subclass. */
    uint8_t  bDeviceProtocol; /**< USB device protocol. */
    uint8_t  bMaxPacketSize0;
    uint8_t  bNumConfigurations;
    uint8_t  bReserved; /**< Reserved for future use, must be 0. */
} ATTR_PACKED USB_StdDescriptor_DeviceQualifier_t;

typedef struct
{
    USB_Descriptor_Header_t Header; /**< Descriptor header, including type and size. */
    uint16_t TotalConfigurationSize;
    uint8_t  TotalInterfaces; /**< Total number of interfaces in the configuration. */
    uint8_t  ConfigurationNumber; /**< Configuration index of the current configuration. */
    uint8_t  ConfigurationStrIndex; /**< Index criptor describing the configuration. */
    uint8_t  ConfigAttributes;
    uint8_t  MaxPowerConsumption;
} ATTR_PACKED USB_Descriptor_Configuration_Header_t;

typedef struct
{
    uint8_t  bLength; /**< Size of the descriptor, in bytes. */
    uint8_t  bDescriptorType;
    uint16_t wTotalLength;
    uint8_t  bNumInterfaces; /**< Total number of interfaces in the configuration. */
    uint8_t  bConfigurationValue; /**< Configuration index of the current configuration. */
    uint8_t  iConfiguration; /**< Index of a string descriptor describing the configuration. */
    uint8_t  bmAttributes;
    uint8_t  bMaxPower;
} ATTR_PACKED USB_StdDescriptor_Configuration_Header_t;

typedef struct
{
    USB_Descriptor_Header_t Header; /**< Descriptor header, including type and size. */
    uint8_t InterfaceNumber; /**< he interface in the current configuration. */
    uint8_t AlternateSetting;
    uint8_t TotalEndpoints; /**< Total number of endpoints in the interface. */
    uint8_t Class; /**< Interface class ID. */
    uint8_t SubClass; /**< Interface subclass ID. */
    uint8_t Protocol; /**< Interface protocol ID. */
    uint8_t InterfaceStrIndex; /**< ing descriptor describing the interface. */
} ATTR_PACKED USB_Descriptor_Interface_t;

typedef struct
{
    uint8_t bLength; /**< Size of the descriptor, in bytes. */
    uint8_t bDescriptorType;
    uint8_t bInterfaceNumber; /**< Index of the interface in the current configuration. */
    uint8_t bAlternateSetting;
    uint8_t bNumEndpoints; /**< Total number of endpoints in the interface. */
    uint8_t bInterfaceClass; /**< Interface class ID. */
    uint8_t bInterfaceSubClass; /**< Interface subclass ID. */
    uint8_t bInterfaceProtocol; /**< Interface protocol ID. */
    uint8_t iInterface;
} ATTR_PACKED USB_StdDescriptor_Interface_t;

typedef struct
{
    USB_Descriptor_Header_t Header; /**< Descriptor header, including type and size. */

    uint8_t FirstInterfaceIndex; /**< Index of the first associated interface. */
    uint8_t TotalInterfaces; /**< Total number of associated interfaces. */

    uint8_t Class; /**< Interface class ID. */
    uint8_t SubClass; /**< Interface subclass ID. */
    uint8_t Protocol; /**< Interface protocol ID. */

    uint8_t IADStrIndex;
} ATTR_PACKED USB_Descriptor_Interface_Association_t;


typedef struct
{
    uint8_t bLength; /**< Size of the descriptor, in bytes. */
    uint8_t bDescriptorType;
    uint8_t bFirstInterface; /**< Index of the first associated interface. */
    uint8_t bInterfaceCount; /**< Total number of associated interfaces. */
    uint8_t bFunctionClass; /**< Interface class ID. */
    uint8_t bFunctionSubClass; /**< Interface subclass ID. */
    uint8_t bFunctionProtocol; /**< Interface protocol ID. */
    uint8_t iFunction;
} ATTR_PACKED USB_StdDescriptor_Interface_Association_t;

typedef struct
{
    USB_Descriptor_Header_t Header; /**< Descriptor header, including type and size. */

    uint8_t  EndpointAddress;
    uint8_t  Attributes;
    uint16_t EndpointSize;
    uint8_t  PollingIntervalMS;
} ATTR_PACKED USB_Descriptor_Endpoint_t;

typedef struct
{
    uint8_t  bLength; /**< Size of the descriptor, in bytes. */
    uint8_t  bDescriptorType;
    uint8_t  bEndpointAddress;
    uint8_t  bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t  bInterval;
} ATTR_PACKED USB_StdDescriptor_Endpoint_t;

typedef struct
{
    USB_Descriptor_Header_t Header;
    wchar_t  UnicodeString[];
} ATTR_PACKED USB_Descriptor_String_t;

typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bString[];
} ATTR_PACKED USB_StdDescriptor_String_t;

typedef struct
{
    uint8_t  Address;
    uint16_t Size;
    uint8_t  Type;
    uint8_t  Banks;
} USB_Endpoint_Table_t;

#define ENDPOINT_EPNUM_MASK 0x0F
#define ENDPOINT_CONTROLEP 0

static inline uint8_t
Endpoint_BytesToEPSizeMask(const uint16_t Bytes) ATTR_WARN_UNUSED_RESULT ATTR_CONST
                                                                       ATTR_ALWAYS_INLINE;

#ifndef __USBDEVICE_AVR8_H__
#define __USBDEVICE_AVR8_H__



static inline uint8_t Endpoint_BytesToEPSizeMask(const uint16_t Bytes)
{
                uint8_t  MaskVal    = 0;
                uint16_t CheckBytes = 8;

                while (CheckBytes < Bytes)
                {
                    MaskVal++;
                    CheckBytes <<= 1;
                }

                return (MaskVal << EPSIZE0);
}

void Endpoint_ClearEndpoints(void);

bool Endpoint_ConfigureEndpoint_Prv(const uint8_t Number,
                                                const uint8_t UECFG0XData,
                                                const uint8_t UECFG1XData);

#if (!defined(FIXED_CONTROL_ENDPOINT_SIZE) || defined(__DOXYGEN__))
#define ENDPOINT_CONTROLEP_DEFAULT_SIZE     8
#endif

#define ENDPOINT_TOTAL_ENDPOINTS        7

enum Endpoint_WaitUntilReady_ErrorCodes_t
{
    ENDPOINT_READYWAIT_NoError = 0,
    ENDPOINT_READYWAIT_EndpointStalled = 1,
    ENDPOINT_READYWAIT_DeviceDisconnected      = 2,
    ENDPOINT_READYWAIT_BusSuspended            = 3,
    ENDPOINT_READYWAIT_Timeout                 = 4,
};

static inline bool Endpoint_ConfigureEndpoint(const uint8_t Address,
                                              const uint8_t Type,
                                              const uint16_t Size,
                                              const uint8_t Banks) ATTR_ALWAYS_INLINE;

static inline bool Endpoint_ConfigureEndpoint(const uint8_t Address,
                                             const uint8_t Type,
                                             const uint16_t Size,
                                             const uint8_t Banks)
{
    uint8_t Number = (Address & ENDPOINT_EPNUM_MASK);

    if (Number >= ENDPOINT_TOTAL_ENDPOINTS)
        return false;

    return Endpoint_ConfigureEndpoint_Prv(Number,
                        ((Type << EPTYPE0) | ((Address & ENDPOINT_DIR_IN) ? (1 << EPDIR) : 0)),
          ((1 << ALLOC) | ((Banks > 1) ? (1 << EPBK0) : 0) | Endpoint_BytesToEPSizeMask(Size)));
}

static inline uint16_t Endpoint_BytesInEndpoint(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
static inline uint16_t Endpoint_BytesInEndpoint(void)
{
    return (((uint16_t)UEBCHX << 8) | UEBCLX);
}



static inline uint8_t
Endpoint_GetEndpointDirection(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;

static inline uint8_t Endpoint_GetEndpointDirection(void)
{
    return (UECFG0X & (1 << EPDIR)) ? ENDPOINT_DIR_IN : ENDPOINT_DIR_OUT;
}

static inline uint8_t
Endpoint_GetCurrentEndpoint(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;

static inline uint8_t Endpoint_GetCurrentEndpoint(void)
{
#if !defined(CONTROL_ONLY_DEVICE)
    return ((UENUM & ENDPOINT_EPNUM_MASK) | Endpoint_GetEndpointDirection());
#else
    return ENDPOINT_CONTROLEP;
#endif
}

static inline void Endpoint_SelectEndpoint(const uint8_t Address) ATTR_ALWAYS_INLINE;
static inline void Endpoint_SelectEndpoint(const uint8_t Address)
{
#if !defined(CONTROL_ONLY_DEVICE)
    UENUM = (Address & ENDPOINT_EPNUM_MASK);
#endif
}



static inline void Endpoint_ResetEndpoint(const uint8_t Address) ATTR_ALWAYS_INLINE;
static inline void Endpoint_ResetEndpoint(const uint8_t Address)
{
    UERST = (1 << (Address & ENDPOINT_EPNUM_MASK));
    UERST = 0;
}

static inline void Endpoint_EnableEndpoint(void) ATTR_ALWAYS_INLINE;
static inline void Endpoint_EnableEndpoint(void)
{
    UECONX |= (1 << EPEN);
}

static inline void Endpoint_DisableEndpoint(void) ATTR_ALWAYS_INLINE;
static inline void Endpoint_DisableEndpoint(void)
{
    UECONX &= ~(1 << EPEN);
}

static inline bool Endpoint_IsEnabled(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
static inline bool Endpoint_IsEnabled(void)
{
    return ((UECONX & (1 << EPEN)) ? true : false);
}

static inline uint8_t Endpoint_GetBusyBanks(void) ATTR_ALWAYS_INLINE ATTR_WARN_UNUSED_RESULT;
static inline uint8_t Endpoint_GetBusyBanks(void)
{
    return (UESTA0X & (0x03 << NBUSYBK0));
}

static inline void Endpoint_AbortPendingIN(void)
{
    while (Endpoint_GetBusyBanks() != 0)
    {
        UEINTX |= (1 << RXOUTI);
        while (UEINTX & (1 << RXOUTI));
    }
}

static inline bool Endpoint_IsReadWriteAllowed(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
static inline bool Endpoint_IsReadWriteAllowed(void)
{
    return ((UEINTX & (1 << RWAL)) ? true : false);
}

static inline bool Endpoint_IsConfigured(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
static inline bool Endpoint_IsConfigured(void)
{
    return ((UESTA0X & (1 << CFGOK)) ? true : false);
}

static inline uint8_t
Endpoint_GetEndpointInterrupts(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;

static inline uint8_t Endpoint_GetEndpointInterrupts(void)
{
    return UEINT;
}

static inline bool
Endpoint_HasEndpointInterrupted(const uint8_t Address) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;

static inline bool Endpoint_HasEndpointInterrupted(const uint8_t Address)
{
    return ((Endpoint_GetEndpointInterrupts() &
        (1 << (Address & ENDPOINT_EPNUM_MASK))) ? true : false);
}

static inline bool Endpoint_IsINReady(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
static inline bool Endpoint_IsINReady(void)
{
    return ((UEINTX & (1 << TXINI)) ? true : false);
}

static inline bool Endpoint_IsOUTReceived(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
static inline bool Endpoint_IsOUTReceived(void)
{
    return ((UEINTX & (1 << RXOUTI)) ? true : false);
}

static inline bool Endpoint_IsSETUPReceived(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
static inline bool Endpoint_IsSETUPReceived(void)
{
    return ((UEINTX & (1 << RXSTPI)) ? true : false);
}

static inline void Endpoint_ClearSETUP(void) ATTR_ALWAYS_INLINE;
static inline void Endpoint_ClearSETUP(void)
{
    UEINTX &= ~(1 << RXSTPI);
}

static inline void Endpoint_ClearIN(void) ATTR_ALWAYS_INLINE;
static inline void Endpoint_ClearIN(void)
{
#if !defined(CONTROL_ONLY_DEVICE)
    UEINTX &= ~((1 << TXINI) | (1 << FIFOCON));
#else
    UEINTX &= ~(1 << TXINI);
#endif
}

static inline void Endpoint_ClearOUT(void) ATTR_ALWAYS_INLINE;
static inline void Endpoint_ClearOUT(void)
{
#if !defined(CONTROL_ONLY_DEVICE)
    UEINTX &= ~((1 << RXOUTI) | (1 << FIFOCON));
#else
    UEINTX &= ~(1 << RXOUTI);
#endif
}

static inline void Endpoint_StallTransaction(void) ATTR_ALWAYS_INLINE;
static inline void Endpoint_StallTransaction(void)
{
    UECONX |= (1 << STALLRQ);
}

static inline void Endpoint_ClearStall(void) ATTR_ALWAYS_INLINE;
static inline void Endpoint_ClearStall(void)
{
    UECONX |= (1 << STALLRQC);
}

static inline bool Endpoint_IsStalled(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
static inline bool Endpoint_IsStalled(void)
{
    return ((UECONX & (1 << STALLRQ)) ? true : false);
}

static inline void Endpoint_ResetDataToggle(void) ATTR_ALWAYS_INLINE;
static inline void Endpoint_ResetDataToggle(void)
{
    UECONX |= (1 << RSTDT);
}

static inline void Endpoint_SetEndpointDirection(const uint8_t DirectionMask) ATTR_ALWAYS_INLINE;
static inline void Endpoint_SetEndpointDirection(const uint8_t DirectionMask)
{
    UECFG0X = ((UECFG0X & ~(1 << EPDIR)) | (DirectionMask ? (1 << EPDIR) : 0));
}

static inline uint8_t Endpoint_Read_8(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
static inline uint8_t Endpoint_Read_8(void)
{
    return UEDATX;
}

static inline void Endpoint_Write_8(const uint8_t Data) ATTR_ALWAYS_INLINE;
static inline void Endpoint_Write_8(const uint8_t Data)
{
    UEDATX = Data;
}

static inline void Endpoint_Discard_8(void) ATTR_ALWAYS_INLINE;
static inline void Endpoint_Discard_8(void)
{
    uint8_t Dummy;
    Dummy = UEDATX;
    (void)Dummy;
}

static inline uint16_t Endpoint_Read_16_LE(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
static inline uint16_t Endpoint_Read_16_LE(void)
{
    union
    {
        uint16_t Value;
        uint8_t  Bytes[2];
    } Data;

    Data.Bytes[0] = UEDATX;
    Data.Bytes[1] = UEDATX;
    return Data.Value;
}

#ifndef __ENDPOINT_AVR8_H__
#define __ENDPOINT_AVR8_H__



static inline uint16_t Endpoint_Read_16_BE(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
static inline uint16_t Endpoint_Read_16_BE(void)
{
    union
    {
                    uint16_t Value;
                    uint8_t  Bytes[2];
                } Data;

                Data.Bytes[1] = UEDATX;
                Data.Bytes[0] = UEDATX;

                return Data.Value;
            }

static inline void Endpoint_Write_16_LE(const uint16_t Data) ATTR_ALWAYS_INLINE;
static inline void Endpoint_Write_16_LE(const uint16_t Data)
{
    UEDATX = (Data & 0xFF);
    UEDATX = (Data >> 8);
}

static inline void Endpoint_Write_16_BE(const uint16_t Data) ATTR_ALWAYS_INLINE;
static inline void Endpoint_Write_16_BE(const uint16_t Data)
{
    UEDATX = (Data >> 8);
    UEDATX = (Data & 0xFF);
}

static inline void Endpoint_Discard_16(void) ATTR_ALWAYS_INLINE;
static inline void Endpoint_Discard_16(void)
{
    uint8_t Dummy;
    Dummy = UEDATX;
    Dummy = UEDATX;
    (void)Dummy;
}

static inline uint32_t Endpoint_Read_32_LE(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
static inline uint32_t Endpoint_Read_32_LE(void)
{
    union
    {
        uint32_t Value;
        uint8_t  Bytes[4];
    } Data;

    Data.Bytes[0] = UEDATX;
    Data.Bytes[1] = UEDATX;
    Data.Bytes[2] = UEDATX;
    Data.Bytes[3] = UEDATX;
    return Data.Value;
}

static inline uint32_t Endpoint_Read_32_BE(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
static inline uint32_t Endpoint_Read_32_BE(void)
{
    union
    {
        uint32_t Value;
        uint8_t  Bytes[4];
    } Data;

    Data.Bytes[3] = UEDATX;
    Data.Bytes[2] = UEDATX;
    Data.Bytes[1] = UEDATX;
    Data.Bytes[0] = UEDATX;
    return Data.Value;
}

static inline void Endpoint_Write_32_LE(const uint32_t Data) ATTR_ALWAYS_INLINE;
static inline void Endpoint_Write_32_LE(const uint32_t Data)
{
    UEDATX = (Data &  0xFF);
    UEDATX = (Data >> 8);
    UEDATX = (Data >> 16);
    UEDATX = (Data >> 24);
}

static inline void Endpoint_Write_32_BE(const uint32_t Data) ATTR_ALWAYS_INLINE;
static inline void Endpoint_Write_32_BE(const uint32_t Data)
{
    UEDATX = (Data >> 24);
    UEDATX = (Data >> 16);
    UEDATX = (Data >> 8);
    UEDATX = (Data &  0xFF);
}

static inline void Endpoint_Discard_32(void) ATTR_ALWAYS_INLINE;
static inline void Endpoint_Discard_32(void)
{
    uint8_t Dummy;
    Dummy = UEDATX;
    Dummy = UEDATX;
    Dummy = UEDATX;
    Dummy = UEDATX;
    (void)Dummy;
}

#if (!defined(FIXED_CONTROL_ENDPOINT_SIZE) || defined(__DOXYGEN__))
extern uint8_t USB_Device_ControlEndpointSize;
#else
#define USB_Device_ControlEndpointSize FIXED_CONTROL_ENDPOINT_SIZE
#endif

bool Endpoint_ConfigureEndpointTable(const USB_Endpoint_Table_t* const Table,
                                const uint8_t Entries);

void Endpoint_ClearStatusStage(void);
uint8_t Endpoint_WaitUntilReady(void);


#endif

#define USB_DEVICE_OPT_LOWSPEED            (1 << 0)
#define USB_DEVICE_OPT_FULLSPEED               (0 << 0)

#if (!defined(NO_INTERNAL_SERIAL) && (defined(USB_SERIES_7_AVR) || defined(USB_SERIES_6_AVR) || \
                  defined(USB_SERIES_4_AVR) || defined(USB_SERIES_2_AVR) || \
                  defined(__DOXYGEN__)))

#define USE_INTERNAL_SERIAL            0xDC

#define INTERNAL_SERIAL_LENGTH_BITS    80
#define INTERNAL_SERIAL_START_ADDRESS  0x0E
#else
#define USE_INTERNAL_SERIAL            NO_DESCRIPTOR
#define INTERNAL_SERIAL_LENGTH_BITS    0
#define INTERNAL_SERIAL_START_ADDRESS  0
#endif


void USB_Device_SendRemoteWakeup(void);


static inline uint16_t USB_Device_GetFrameNumber(void) ATTR_ALWAYS_INLINE ATTR_WARN_UNUSED_RESULT;
static inline uint16_t USB_Device_GetFrameNumber(void)
{
    return UDFNUM;
}

#if !defined(NO_SOF_EVENTS)
static inline void USB_Device_EnableSOFEvents(void) ATTR_ALWAYS_INLINE;
static inline void USB_Device_EnableSOFEvents(void)
{
    USB_INT_Enable(USB_INT_SOFI);
}

static inline void USB_Device_DisableSOFEvents(void) ATTR_ALWAYS_INLINE;
static inline void USB_Device_DisableSOFEvents(void)
{
    USB_INT_Disable(USB_INT_SOFI);
}
#endif

#if !defined(__DOXYGEN__)
#if defined(USB_DEVICE_OPT_LOWSPEED)
static inline void USB_Device_SetLowSpeed(void) ATTR_ALWAYS_INLINE;
static inline void USB_Device_SetLowSpeed(void)
{
    UDCON |= 1<<LSM;
}

static inline void USB_Device_SetFullSpeed(void) ATTR_ALWAYS_INLINE;
static inline void USB_Device_SetFullSpeed(void)
{
    UDCON &= ~(1 << LSM);
}
#endif

static inline void USB_Device_SetDeviceAddress(const uint8_t Address) ATTR_ALWAYS_INLINE;
static inline void USB_Device_SetDeviceAddress(const uint8_t Address)
{
    UDADDR = (UDADDR & (1 << ADDEN)) | (Address & 0x7F);
}

static inline void USB_Device_EnableDeviceAddress(const uint8_t Address) ATTR_ALWAYS_INLINE;
static inline void USB_Device_EnableDeviceAddress(const uint8_t Address)
{
    (void)Address;
    UDADDR |= (1 << ADDEN);
}

static inline bool USB_Device_IsAddressSet(void) ATTR_ALWAYS_INLINE ATTR_WARN_UNUSED_RESULT;
static inline bool USB_Device_IsAddressSet(void)
{
    return (UDADDR & (1 << ADDEN));
}

#if (USE_INTERNAL_SERIAL != NO_DESCRIPTOR)
static inline void
USB_Device_GetSerialString(uint16_t* const UnicodeString) ATTR_NON_NULL_PTR_ARG(1);

static inline void USB_Device_GetSerialString(uint16_t* const UnicodeString)
{
    uint_reg_t CurrentGlobalInt = GetGlobalInterruptMask();
    GlobalInterruptDisable();
    uint8_t SigReadAddress = INTERNAL_SERIAL_START_ADDRESS;

    for (uint8_t SerialCharNum = 0; SerialCharNum < (INTERNAL_SERIAL_LENGTH_BITS / 4);
        SerialCharNum++)
    {
        uint8_t SerialByte = boot_signature_byte_get(SigReadAddress);

        if (SerialCharNum & 0x01)
        {
            SerialByte >>= 4;
            SigReadAddress++;
        }

        SerialByte &= 0x0F;

        UnicodeString[SerialCharNum] = cpu_to_le16((SerialByte >= 10) ?
                (('A' - 10) + SerialByte) : ('0' + SerialByte));
    }

    SetGlobalInterruptMask(CurrentGlobalInt);
}
#endif

#endif

#endif

#ifndef __DEVICESTDREQ_H__
#define __DEVICESTDREQ_H__

#if defined(ARCH_HAS_MULTI_ADDRESS_SPACE) || defined(__DOXYGEN__)
enum USB_DescriptorMemorySpaces_t
{
#if defined(ARCH_HAS_FLASH_ADDRESS_SPACE) || defined(__DOXYGEN__)
MEMSPACE_FLASH = 0, /**< Indicates the requested descriptor is located in FLASH memory. */
#endif
#if defined(ARCH_HAS_EEPROM_ADDRESS_SPACE) || defined(__DOXYGEN__)
MEMSPACE_EEPROM = 1, /**< Indicates the requested descriptor is located in EEPROM memory. */
#endif
MEMSPACE_RAM = 2, /**< Indicates the requested descriptor is located in RAM memory. */
};
#endif

extern uint8_t USB_Device_ConfigurationNumber;

#if !defined(NO_DEVICE_REMOTE_WAKEUP)
extern bool USB_Device_RemoteWakeupEnabled;
#endif

#if !defined(NO_DEVICE_SELF_POWER)
extern bool USB_Device_CurrentlySelfPowered;
#endif

#if !defined(__DOXYGEN__)
#if defined(USE_RAM_DESCRIPTORS) && defined(USE_EEPROM_DESCRIPTORS)
#error USE_RAM_DESCRIPTORS and USE_EEPROM_DESCRIPTORS are mutually exclusive.
#elif defined(USE_RAM_DESCRIPTORS) && defined(USE_FLASH_DESCRIPTORS)
#error USE_RAM_DESCRIPTORS and USE_FLASH_DESCRIPTORS are mutually exclusive.
#elif defined(USE_FLASH_DESCRIPTORS) && defined(USE_EEPROM_DESCRIPTORS)
#error USE_FLASH_DESCRIPTORS and USE_EEPROM_DESCRIPTORS are mutually exclusive.
#elif defined(USE_FLASH_DESCRIPTORS) && defined(USE_EEPROM_DESCRIPTORS) && defined(USE_RAM_DESCRIPTORS)
#error Only one of the USE_*_DESCRIPTORS modes should be selected.
#endif

void USB_Device_ProcessControlRequest(void);

#if defined(__INCLUDE_FROM_DEVICESTDREQ_C)
                static void USB_Device_SetAddress(void);
                static void USB_Device_SetConfiguration(void);
                static void USB_Device_GetConfiguration(void);
                static void USB_Device_GetDescriptor(void);
                static void USB_Device_GetStatus(void);
                static void USB_Device_ClearSetFeature(void);

#if !defined(NO_INTERNAL_SERIAL) && (USE_INTERNAL_SERIAL != NO_DESCRIPTOR)
static void USB_Device_GetInternalSerialDescriptor(void);
#endif
#endif
#endif


#endif

#if (F_USB == 8000000)
#elif (F_USB == 16000000)
#if (defined(__AVR_AT90USB82__) || defined(__AVR_AT90USB162__) || \
     defined(__AVR_ATmega8U2__) || defined(__AVR_ATmega16U2__) || \
     defined(__AVR_ATmega32U2__))
#define USB_PLL_PSC                (1 << PLLP0)
#elif (defined(__AVR_ATmega16U4__) || defined(__AVR_ATmega32U4__))
#define USB_PLL_PSC                (1 << PINDIV)
#elif (defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB647__))
#define USB_PLL_PSC                ((1 << PLLP2) | (1 << PLLP1))
#elif (defined(__AVR_AT90USB1286__) || defined(__AVR_AT90USB1287__))
#define USB_PLL_PSC                ((1 << PLLP2) | (1 << PLLP0))
#endif
#endif

#if !defined(USB_PLL_PSC)
#error No PLL prescale value available for chosen F_USB value and AVR model.
#endif


#define USB_OPT_REG_DISABLED               (1 << 1)
#define USB_OPT_REG_ENABLED                (0 << 1)
#define USB_OPT_REG_KEEP_ENABLED           (1 << 3)
#define USB_OPT_MANUAL_PLL                 (1 << 2)
#define USB_OPT_AUTO_PLL                   (0 << 2)

#if !defined(USB_STREAM_TIMEOUT_MS) || defined(__DOXYGEN__)
#define USB_STREAM_TIMEOUT_MS       100
#endif

static inline bool USB_VBUS_GetStatus(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
static inline bool USB_VBUS_GetStatus(void)
{
    return ((USBSTA & (1 << VBUS)) ? true : false);
}

static inline void USB_Detach(void) ATTR_ALWAYS_INLINE;
static inline void USB_Detach(void)
{
    UDCON  |=  (1 << DETACH);
}

static inline void USB_Attach(void) ATTR_ALWAYS_INLINE;
static inline void USB_Attach(void)
{
    UDCON  &= ~(1 << DETACH);
}

void USB_Init(void);

void USB_Disable(void);
void USB_ResetInterface(void);

#define USB_CurrentMode USB_MODE_Device

#if !defined(USE_STATIC_OPTIONS) || defined(__DOXYGEN__)
extern volatile uint8_t USB_Options;
#elif defined(USE_STATIC_OPTIONS)
#define USB_Options USE_STATIC_OPTIONS
#endif

#if !defined(__DOXYGEN__)
#if defined(__INCLUDE_FROM_USB_CONTROLLER_C)
static void USB_Init_Device(void);
#endif

static inline void USB_PLL_On(void) ATTR_ALWAYS_INLINE;
static inline void USB_PLL_On(void)
{
    PLLCSR = USB_PLL_PSC;
    PLLCSR = (USB_PLL_PSC | (1 << PLLE));
}

static inline void USB_PLL_Off(void) ATTR_ALWAYS_INLINE;
static inline void USB_PLL_Off(void)
{
    PLLCSR = 0;
}

static inline bool USB_PLL_IsReady(void) ATTR_WARN_UNUSED_RESULT ATTR_ALWAYS_INLINE;
static inline bool USB_PLL_IsReady(void)
{
    return ((PLLCSR & (1 << PLOCK)) ? true : false);
}

static inline void USB_REG_On(void) ATTR_ALWAYS_INLINE;
static inline void USB_REG_On(void)
{
    UHWCON |=  (1 << UVREGE);
}

static inline void USB_REG_Off(void) ATTR_ALWAYS_INLINE;
static inline void USB_REG_Off(void)
{
    UHWCON &= ~(1 << UVREGE);
}

static inline void USB_OTGPAD_On(void) ATTR_ALWAYS_INLINE;
static inline void USB_OTGPAD_On(void)
{
    USBCON |=  (1 << OTGPADE);
}

static inline void USB_OTGPAD_Off(void) ATTR_ALWAYS_INLINE;
static inline void USB_OTGPAD_Off(void)
{
    USBCON &= ~(1 << OTGPADE);
}

static inline void USB_CLK_Freeze(void) ATTR_ALWAYS_INLINE;
static inline void USB_CLK_Freeze(void)
{
    USBCON |=  (1 << FRZCLK);
}

static inline void USB_CLK_Unfreeze(void) ATTR_ALWAYS_INLINE;
static inline void USB_CLK_Unfreeze(void)
{
    USBCON &= ~(1 << FRZCLK);
}

static inline void USB_Controller_Enable(void) ATTR_ALWAYS_INLINE;
static inline void USB_Controller_Enable(void)
{
    USBCON |=  (1 << USBE);
}

static inline void USB_Controller_Disable(void) ATTR_ALWAYS_INLINE;
static inline void USB_Controller_Disable(void)
{
    USBCON &= ~(1 << USBE);
}

static inline void USB_Controller_Reset(void) ATTR_ALWAYS_INLINE;
static inline void USB_Controller_Reset(void)
{
    USBCON &= ~(1 << USBE);
    USBCON |=  (1 << USBE);
}

#endif

#endif

#if !defined(USB_HOST_TIMEOUT_MS) || defined(__DOXYGEN__)
#define USB_HOST_TIMEOUT_MS                1000
#endif

enum USB_Host_SendControlErrorCodes_t
{
    HOST_SENDCONTROL_Successful         = 0,
    HOST_SENDCONTROL_DeviceDisconnected = 1,
    HOST_SENDCONTROL_PipeError          = 2,
    HOST_SENDCONTROL_SetupStalled       = 3,
    HOST_SENDCONTROL_SoftwareTimeOut    = 4,
};

extern uint8_t USB_Host_ConfigurationNumber;
uint8_t USB_Host_SendControlRequest(void* const BufferPtr);
uint8_t USB_Host_SetDeviceConfiguration(const uint8_t ConfigNumber);
uint8_t USB_Host_GetDeviceConfiguration(uint8_t* const ConfigNumber) ATTR_NON_NULL_PTR_ARG(1);

uint8_t USB_Host_GetDescriptor(const uint8_t Type,
                              const uint8_t Index,
                              void* const Buffer,
                              const uint8_t BufferLength) ATTR_NON_NULL_PTR_ARG(3);

uint8_t USB_Host_GetDeviceStatus(uint8_t* const FeatureStatus) ATTR_NON_NULL_PTR_ARG(1);
uint8_t USB_Host_ClearEndpointStall(const uint8_t EndpointAddress);
uint8_t USB_Host_SetInterfaceAltSetting(const uint8_t InterfaceIndex, const uint8_t AltSetting);


uint8_t USB_Host_GetInterfaceAltSetting(const uint8_t InterfaceIndex,
                                uint8_t* const AltSetting) ATTR_NON_NULL_PTR_ARG(2);

static inline uint8_t
USB_Host_GetDeviceDescriptor(USB_Descriptor_Device_t* const
    DeviceDescriptorPtr) ATTR_NON_NULL_PTR_ARG(1);

static inline uint8_t
USB_Host_GetDeviceDescriptor(USB_Descriptor_Device_t* const DeviceDescriptorPtr)
{
    return USB_Host_GetDescriptor(DTYPE_Device, 0, DeviceDescriptorPtr,
        sizeof(USB_Descriptor_Device_t));
}

static inline uint8_t USB_Host_GetDeviceStringDescriptor(const uint8_t Index,
                                                    void* const Buffer,
                                             const uint8_t BufferLength) ATTR_NON_NULL_PTR_ARG(2);

static inline uint8_t USB_Host_GetDeviceStringDescriptor(const uint8_t Index,
                                                   void* const Buffer,
                                                 const uint8_t BufferLength)
{
    return USB_Host_GetDescriptor(DTYPE_String, Index,  Buffer, BufferLength);
}

enum USB_WaitForTypes_t
{
    USB_HOST_WAITFOR_SetupSent,
    USB_HOST_WAITFOR_InReceived,
    USB_HOST_WAITFOR_OutReady,
};

#define DESCRIPTOR_PCAST(DescriptorPtr, Type) ((Type*)(DescriptorPtr))
#define DESCRIPTOR_CAST(DescriptorPtr, Type)  (*DESCRIPTOR_PCAST(DescriptorPtr, Type))

#define DESCRIPTOR_TYPE(DescriptorPtr)    DESCRIPTOR_PCAST(DescriptorPtr, USB_Descriptor_Header_t)->Type

#define DESCRIPTOR_SIZE(DescriptorPtr)    DESCRIPTOR_PCAST(DescriptorPtr, USB_Descriptor_Header_t)->Size

typedef uint8_t (* ConfigComparatorPtr_t)(void*);

enum USB_Host_GetConfigDescriptor_ErrorCodes_t
{
    HOST_GETCONFIG_Successful       = 0,
    HOST_GETCONFIG_DeviceDisconnect = 1,
    HOST_GETCONFIG_PipeError        = 2,
    HOST_GETCONFIG_SetupStalled     = 3,
    HOST_GETCONFIG_SoftwareTimeOut  = 4,
    HOST_GETCONFIG_BuffOverflow     = 5,
    HOST_GETCONFIG_InvalidData      = 6,
};

enum DSearch_Return_ErrorCodes_t
{
    DESCRIPTOR_SEARCH_Found = 0,
    DESCRIPTOR_SEARCH_Fail = 1,
    DESCRIPTOR_SEARCH_NotFound = 2,
};

enum DSearch_Comp_Return_ErrorCodes_t
{
    DESCRIPTOR_SEARCH_COMP_Found           = 0,
    DESCRIPTOR_SEARCH_COMP_Fail            = 1,
    DESCRIPTOR_SEARCH_COMP_EndOfDescriptor = 2,
};

uint8_t USB_Host_GetDeviceConfigDescriptor(const uint8_t ConfigNumber,
                                     uint16_t* const ConfigSizePtr,
                                    void* const BufferPtr,
               const uint16_t BufferSize) ATTR_NON_NULL_PTR_ARG(2) ATTR_NON_NULL_PTR_ARG(3);

void USB_GetNextDescriptorOfType(uint16_t* const BytesRem,
                         void** const CurrConfigLoc,
                          const uint8_t Type)
                         ATTR_NON_NULL_PTR_ARG(1) ATTR_NON_NULL_PTR_ARG(2);

void USB_GetNextDescriptorOfTypeBefore(uint16_t* const BytesRem,
                                 void** const CurrConfigLoc,
                                const uint8_t Type,
                               const uint8_t BeforeType)
                         ATTR_NON_NULL_PTR_ARG(1) ATTR_NON_NULL_PTR_ARG(2);

void USB_GetNextDescriptorOfTypeAfter(uint16_t* const BytesRem,
                                 void** const CurrConfigLoc,
                                 const uint8_t Type,
                                 const uint8_t AfterType)
                                 ATTR_NON_NULL_PTR_ARG(1) ATTR_NON_NULL_PTR_ARG(2);

uint8_t USB_GetNextDescriptorComp(uint16_t* const BytesRem,
                              void** const CurrConfigLoc,
                       ConfigComparatorPtr_t const ComparatorRoutine) ATTR_NON_NULL_PTR_ARG(1)
                                          ATTR_NON_NULL_PTR_ARG(2) ATTR_NON_NULL_PTR_ARG(3);

static inline void USB_GetNextDescriptor(uint16_t* const BytesRem,
                    void** CurrConfigLoc) ATTR_NON_NULL_PTR_ARG(1) ATTR_NON_NULL_PTR_ARG(2);

static inline void USB_GetNextDescriptor(uint16_t* const BytesRem, void** CurrConfigLoc)
{
    uint16_t CurrDescriptorSize = DESCRIPTOR_CAST(*CurrConfigLoc, USB_Descriptor_Header_t).Size;

    if (*BytesRem < CurrDescriptorSize)
        CurrDescriptorSize = *BytesRem;

    *CurrConfigLoc  = (void*)((uintptr_t)*CurrConfigLoc + CurrDescriptorSize);
    *BytesRem      -= CurrDescriptorSize;
}


volatile bool        USB_IsInitialized;
USB_Request_Header_t USB_ControlRequest;
volatile uint8_t     USB_DeviceState;

void USB_USBTask(void)
{
    USB_DeviceTask();
}

static void USB_DeviceTask(void)
{
    if (USB_DeviceState == DEVICE_STATE_Unattached)
        return;

    uint8_t PrevEndpoint = Endpoint_GetCurrentEndpoint();
    Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);

    if (Endpoint_IsSETUPReceived())
        USB_Device_ProcessControlRequest();

    Endpoint_SelectEndpoint(PrevEndpoint);
}




enum Endpoint_Stream_RW_ErrorCodes_t
{
    ENDPOINT_RWSTREAM_NoError            = 0,
    ENDPOINT_RWSTREAM_EndpointStalled    = 1,
    ENDPOINT_RWSTREAM_DeviceDisconnected = 2,
    ENDPOINT_RWSTREAM_BusSuspended       = 3,
    ENDPOINT_RWSTREAM_Timeout            = 4,
    ENDPOINT_RWSTREAM_IncompleteTransfer = 5,
};  

enum Endpoint_ControlStream_RW_ErrorCodes_t
{   
    ENDPOINT_RWCSTREAM_NoError            = 0,
    ENDPOINT_RWCSTREAM_HostAborted        = 1,
    ENDPOINT_RWCSTREAM_DeviceDisconnected = 2,
    ENDPOINT_RWCSTREAM_BusSuspended       = 3,
};  




#define CDC_CONTROL_LINE_OUT_DTR         (1 << 0)
#define CDC_CONTROL_LINE_OUT_RTS         (1 << 1)
#define CDC_CONTROL_LINE_IN_DCD          (1 << 0)
#define CDC_CONTROL_LINE_IN_DSR          (1 << 1)
#define CDC_CONTROL_LINE_IN_BREAK        (1 << 2)
#define CDC_CONTROL_LINE_IN_RING         (1 << 3)
#define CDC_CONTROL_LINE_IN_FRAMEERROR   (1 << 4)
#define CDC_CONTROL_LINE_IN_PARITYERROR  (1 << 5)
#define CDC_CONTROL_LINE_IN_OVERRUNERROR (1 << 6)

#define CDC_FUNCTIONAL_DESCRIPTOR(DataSize)        \
             struct                                        \
             {                                             \
                  USB_Descriptor_Header_t Header;          \
                  uint8_t                 SubType;         \
                  uint8_t                 Data[DataSize];  \
             }

enum CDC_Descriptor_ClassSubclassProtocol_t
{
    CDC_CSCP_CDCClass = 0x02,
    CDC_CSCP_NoSpecificSubclass     = 0x00,
    CDC_CSCP_ACMSubclass            = 0x02,
    CDC_CSCP_ATCommandProtocol      = 0x01,
    CDC_CSCP_NoSpecificProtocol     = 0x00,
    CDC_CSCP_VendorSpecificProtocol = 0xFF,
    CDC_CSCP_CDCDataClass           = 0x0A,
    CDC_CSCP_NoDataSubclass         = 0x00,
    CDC_CSCP_NoDataProtocol         = 0x00,
};

enum CDC_ClassRequests_t
{
    CDC_REQ_SendEncapsulatedCommand = 0x00,
    CDC_REQ_GetEncapsulatedResponse = 0x01,
    CDC_REQ_SetLineEncoding         = 0x20,
    CDC_REQ_GetLineEncoding         = 0x21,
    CDC_REQ_SetControlLineState     = 0x22,
    CDC_REQ_SendBreak               = 0x23,
};

enum CDC_ClassNotifications_t
{
    CDC_NOTIF_SerialState = 0x20,
};

enum CDC_DescriptorSubtypes_t
{
    CDC_DSUBTYPE_CSInterface_Header = 0x00,
    CDC_DSUBTYPE_CSInterface_CallManagement   = 0x01,
    CDC_DSUBTYPE_CSInterface_ACM              = 0x02,
    CDC_DSUBTYPE_CSInterface_DirectLine       = 0x03,
    CDC_DSUBTYPE_CSInterface_TelephoneRinger  = 0x04,
    CDC_DSUBTYPE_CSInterface_TelephoneCall    = 0x05,
    CDC_DSUBTYPE_CSInterface_Union            = 0x06,
    CDC_DSUBTYPE_CSInterface_CountrySelection = 0x07,
    CDC_DSUBTYPE_CSInterface_TelephoneOpModes = 0x08,
    CDC_DSUBTYPE_CSInterface_USBTerminal      = 0x09,
    CDC_DSUBTYPE_CSInterface_NetworkChannel   = 0x0A,
    CDC_DSUBTYPE_CSInterface_ProtocolUnit     = 0x0B,
    CDC_DSUBTYPE_CSInterface_ExtensionUnit    = 0x0C,
    CDC_DSUBTYPE_CSInterface_MultiChannel     = 0x0D,
    CDC_DSUBTYPE_CSInterface_CAPI             = 0x0E,
    CDC_DSUBTYPE_CSInterface_Ethernet         = 0x0F,
    CDC_DSUBTYPE_CSInterface_ATM              = 0x10,
};

enum CDC_LineEncodingFormats_t
{
    CDC_LINEENCODING_OneStopBit          = 0,
    CDC_LINEENCODING_OneAndAHalfStopBits = 1,
    CDC_LINEENCODING_TwoStopBits         = 2,
};

enum CDC_LineEncodingParity_t
{
    CDC_PARITY_None  = 0,
    CDC_PARITY_Odd   = 1,
    CDC_PARITY_Even  = 2,
    CDC_PARITY_Mark  = 3,
    CDC_PARITY_Space = 4,
};

typedef struct
{
    USB_Descriptor_Header_t Header;
    uint8_t                 Subtype;
    uint16_t                CDCSpecification;
} ATTR_PACKED USB_CDC_Descriptor_FunctionalHeader_t;



typedef struct
{
    uint8_t  bFunctionLength; /**< Size of the descriptor, in bytes. */
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubType;
    uint16_t bcdCDC;
} ATTR_PACKED USB_CDC_StdDescriptor_FunctionalHeader_t;

typedef struct
{
    USB_Descriptor_Header_t Header;
    uint8_t                 Subtype;
    uint8_t                 Capabilities;
} ATTR_PACKED USB_CDC_Descriptor_FunctionalACM_t;

typedef struct
{
            uint8_t bFunctionLength; /**< Size of the descriptor, in bytes. */
            uint8_t bDescriptorType;
            uint8_t bDescriptorSubType;
            uint8_t bmCapabilities;
} ATTR_PACKED USB_CDC_StdDescriptor_FunctionalACM_t;

typedef struct
{
    USB_Descriptor_Header_t Header;
    uint8_t                 Subtype;
    uint8_t                 MasterInterfaceNumber;
    uint8_t                 SlaveInterfaceNumber;
} ATTR_PACKED USB_CDC_Descriptor_FunctionalUnion_t;

typedef struct
{
    uint8_t bFunctionLength; /**< Size of the descriptor, in bytes. */
    uint8_t bDescriptorType; /**< Type alue in \ref USB_DescriptorTypes_t or a value
                              *   given by the specific class.
                              */
    uint8_t bDescriptorSubType; /**< Suuish between CDC class-specific descriptors,
                                 *   must be \ref CDC_DSUBTYPE_CSInterface_Union.
                                 */
    uint8_t bMasterInterface; /**< Interface number of the CDC Control interface. */
    uint8_t bSlaveInterface0; /**< Interface number of the CDC Data interface. */
} ATTR_PACKED USB_CDC_StdDescriptor_FunctionalUnion_t;

typedef struct
{
    uint32_t BaudRateBPS; /**< Baud rate of the virtual serial port, in bits per second. */
    uint8_t  CharFormat; /**< Character format of the virtual serial port, a value from the
                          *   \ref CDC_LineEncodingFormats_t enum.
                          */
    uint8_t  ParityType; /**< Parity setting of the virtual serial port, a value from the
                          *   \ref CDC_LineEncodingParity_t enum.
                          */
    uint8_t  DataBits; /**< Bits of data per character of the virtual serial port. */
} ATTR_PACKED CDC_LineEncoding_t;




#define RNDIS_ERROR_LOGICAL_CMD_FAILED        0x80
#define REMOTE_NDIS_VERSION_MAJOR             0x01
#define REMOTE_NDIS_VERSION_MINOR             0x00

#define REMOTE_NDIS_PACKET_MSG                0x00000001UL
#define REMOTE_NDIS_INITIALIZE_MSG            0x00000002UL
static constexpr uint32_t REMOTE_NDIS_HALT_MSG = 0x00000003;
#define REMOTE_NDIS_QUERY_MSG                 0x00000004UL
#define REMOTE_NDIS_SET_MSG                   0x00000005UL
#define REMOTE_NDIS_RESET_MSG                 0x00000006UL
#define REMOTE_NDIS_INDICATE_STATUS_MSG       0x00000007UL
static constexpr uint32_t REMOTE_NDIS_KEEPALIVE_MSG = 0x00000008;

#define REMOTE_NDIS_INITIALIZE_CMPLT          0x80000002UL
#define REMOTE_NDIS_QUERY_CMPLT               0x80000004UL
#define REMOTE_NDIS_SET_CMPLT                 0x80000005UL
#define REMOTE_NDIS_RESET_CMPLT               0x80000006UL
#define REMOTE_NDIS_KEEPALIVE_CMPLT           0x80000008UL

#define REMOTE_NDIS_STATUS_SUCCESS            0x00000000UL
#define REMOTE_NDIS_STATUS_FAILURE            0xC0000001UL
#define REMOTE_NDIS_STATUS_INVALID_DATA       0xC0010015UL
#define REMOTE_NDIS_STATUS_NOT_SUPPORTED      0xC00000BBUL
#define REMOTE_NDIS_STATUS_MEDIA_CONNECT      0x4001000BUL
#define REMOTE_NDIS_STATUS_MEDIA_DISCONNECT   0x4001000CUL

#define REMOTE_NDIS_MEDIA_STATE_CONNECTED     0x00000000UL
#define REMOTE_NDIS_MEDIA_STATE_DISCONNECTED  0x00000001UL

#define REMOTE_NDIS_MEDIUM_802_3              0x00000000UL

#define REMOTE_NDIS_DF_CONNECTIONLESS         0x00000001UL
#define REMOTE_NDIS_DF_CONNECTION_ORIENTED    0x00000002UL

#define REMOTE_NDIS_PACKET_DIRECTED           0x00000001UL
#define REMOTE_NDIS_PACKET_MULTICAST          0x00000002UL
#define REMOTE_NDIS_PACKET_ALL_MULTICAST      0x00000004UL
#define REMOTE_NDIS_PACKET_BROADCAST          0x00000008UL
#define REMOTE_NDIS_PACKET_SOURCE_ROUTING     0x00000010UL
#define REMOTE_NDIS_PACKET_PROMISCUOUS        0x00000020UL
#define REMOTE_NDIS_PACKET_SMT                0x00000040UL
#define REMOTE_NDIS_PACKET_ALL_LOCAL          0x00000080UL
#define REMOTE_NDIS_PACKET_GROUP              0x00001000UL
#define REMOTE_NDIS_PACKET_ALL_FUNCTIONAL     0x00002000UL
#define REMOTE_NDIS_PACKET_FUNCTIONAL         0x00004000UL
#define REMOTE_NDIS_PACKET_MAC_FRAME          0x00008000UL

#define OID_GEN_SUPPORTED_LIST                0x00010101UL
#define OID_GEN_HARDWARE_STATUS               0x00010102UL
#define OID_GEN_MEDIA_SUPPORTED               0x00010103UL
#define OID_GEN_MEDIA_IN_USE                  0x00010104UL
#define OID_GEN_MAXIMUM_FRAME_SIZE            0x00010106UL
#define OID_GEN_MAXIMUM_TOTAL_SIZE            0x00010111UL
#define OID_GEN_LINK_SPEED                    0x00010107UL
#define OID_GEN_TRANSMIT_BLOCK_SIZE           0x0001010AUL
#define OID_GEN_RECEIVE_BLOCK_SIZE            0x0001010BUL
#define OID_GEN_VENDOR_ID                     0x0001010CUL
#define OID_GEN_VENDOR_DESCRIPTION            0x0001010DUL
#define OID_GEN_CURRENT_PACKET_FILTER         0x0001010EUL
#define OID_GEN_MAXIMUM_TOTAL_SIZE            0x00010111UL
#define OID_GEN_MEDIA_CONNECT_STATUS          0x00010114UL
#define OID_GEN_PHYSICAL_MEDIUM               0x00010202UL
#define OID_GEN_XMIT_OK                       0x00020101UL
#define OID_GEN_RCV_OK                        0x00020102UL
#define OID_GEN_XMIT_ERROR                    0x00020103UL
#define OID_GEN_RCV_ERROR                     0x00020104UL
#define OID_GEN_RCV_NO_BUFFER                 0x00020105UL
#define OID_802_3_PERMANENT_ADDRESS           0x01010101UL
#define OID_802_3_CURRENT_ADDRESS             0x01010102UL
#define OID_802_3_MULTICAST_LIST              0x01010103UL
#define OID_802_3_MAXIMUM_LIST_SIZE           0x01010104UL
#define OID_802_3_RCV_ERROR_ALIGNMENT         0x01020101UL
#define OID_802_3_XMIT_ONE_COLLISION          0x01020102UL
#define OID_802_3_XMIT_MORE_COLLISIONS        0x01020103UL

#define ETHERNET_FRAME_SIZE_MAX               200

enum RNDIS_ClassRequests_t
{
    RNDIS_REQ_SendEncapsulatedCommand = 0x00,
    RNDIS_REQ_GetEncapsulatedResponse = 0x01,
};

enum RNDIS_States_t
{
    RNDIS_Uninitialized    = 0,
    RNDIS_Initialized      = 1,
    RNDIS_Data_Initialized = 2, /**< Adapter alized and ready for data transfers. */
};

enum RNDIS_ClassNotifications_t
{
    RNDIS_NOTIF_ResponseAvailable = 0x01,
};

enum NDIS_Hardware_Status_t
{
    NDIS_HardwareStatus_Ready, /**< Hardware Ready to accept commands from the host. */
    NDIS_HardwareStatus_Initializing, /**< Hardware busy initializing. */
    NDIS_HardwareStatus_Reset, /**< Hardware reset. */
    NDIS_HardwareStatus_Closing, /**< Hardware currently closing. */
    NDIS_HardwareStatus_NotReady /**< Hardware  accept commands from the host. */
};

typedef struct
{
    uint8_t Octets[6]; /**< Individual bytes of a MAC address */
} ATTR_PACKED MAC_Address_t;

typedef struct
{
    uint32_t MessageType; /**< RNDIS message type, a \c REMOTE_NDIS_*_MSG constant */
    uint32_t MessageLength; /**< Total length of the RNDIS message, in bytes */
} ATTR_PACKED RNDIS_Message_Header_t;

typedef struct
{
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t DataOffset;
    uint32_t DataLength;
    uint32_t OOBDataOffset;
    uint32_t OOBDataLength;
    uint32_t NumOOBDataElements;
    uint32_t PerPacketInfoOffset;
    uint32_t PerPacketInfoLength;
    uint32_t VcHandle;
    uint32_t Reserved;
} ATTR_PACKED RNDIS_Packet_Message_t;

typedef struct
{
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t RequestId;

    uint32_t MajorVersion;
    uint32_t MinorVersion;
    uint32_t MaxTransferSize;
} ATTR_PACKED RNDIS_Initialize_Message_t;

typedef struct
{
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t RequestId;
    uint32_t Status;

    uint32_t MajorVersion;
    uint32_t MinorVersion;
    uint32_t DeviceFlags;
    uint32_t Medium;
    uint32_t MaxPacketsPerTransfer;
    uint32_t MaxTransferSize;
    uint32_t PacketAlignmentFactor;
    uint32_t AFListOffset;
    uint32_t AFListSize;
} ATTR_PACKED RNDIS_Initialize_Complete_t;

typedef struct
{
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t RequestId;
} ATTR_PACKED RNDIS_KeepAlive_Message_t;

typedef struct
{
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t RequestId;
    uint32_t Status;
} ATTR_PACKED RNDIS_KeepAlive_Complete_t;

typedef struct
{
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t Status;

    uint32_t AddressingReset;
} ATTR_PACKED RNDIS_Reset_Complete_t;

typedef struct
{
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t RequestId;

    uint32_t Oid;
    uint32_t InformationBufferLength;
    uint32_t InformationBufferOffset;
    uint32_t DeviceVcHandle;
} ATTR_PACKED RNDIS_Set_Message_t;

typedef struct
{
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t RequestId;
    uint32_t Status;
} ATTR_PACKED RNDIS_Set_Complete_t;

typedef struct
{
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t RequestId;

    uint32_t Oid;
    uint32_t InformationBufferLength;
    uint32_t InformationBufferOffset;
    uint32_t DeviceVcHandle;
} ATTR_PACKED RNDIS_Query_Message_t;

typedef struct
{
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t RequestId;
    uint32_t Status;

    uint32_t InformationBufferLength;
    uint32_t InformationBufferOffset;
} ATTR_PACKED RNDIS_Query_Complete_t;

#define LUFA_VERSION_RELEASE_TYPE_DEVELOPMENT   0
#define LUFA_VERSION_RELEASE_TYPE_BETA          1
#define LUFA_VERSION_RELEASE_TYPE_FULL          2
#define LUFA_VERSION_INTEGER              0x170418
#define LUFA_VERSION_STRING               "170418"
#define LUFA_VERSION_RELEASE_TYPE         LUFA_VERSION_RELEASE_TYPE_FULL

#define CLIENT_IP_ADDRESS                { 10,     0,    0,    1}
#define SERVER_IP_ADDRESS                { 10,     0,    0,    2}

#define ADAPTER_MAC_ADDRESS              {0x02, 0x00, 0x02, 0x00, 0x02, 0x00}
#define SERVER_MAC_ADDRESS               {0x00, 0x01, 0x00, 0x01, 0x00, 0x01}

#define NO_DECODE_ETHERNET
#define NO_DECODE_ARP
#define NO_DECODE_IP
#define NO_DECODE_ICMP
#define NO_DECODE_TCP
#define NO_DECODE_UDP
#define NO_DECODE_DHCP

#define ETHERTYPE_IPV4                   0x0800
#define ETHERTYPE_ARP                    0x0806
#define ETHERTYPE_RARP                   0x8035
#define ETHERTYPE_APPLETALK              0x809b
#define ETHERTYPE_APPLETALKARP           0x80f3
#define ETHERTYPE_IEEE8021Q              0x8100
#define ETHERTYPE_NOVELLIPX              0x8137
#define ETHERTYPE_NOVELL                 0x8138
#define ETHERTYPE_IPV6                   0x86DD
#define ETHERTYPE_COBRANET               0x8819
#define ETHERTYPE_PROVIDERBRIDGING       0x88a8
#define ETHERTYPE_MPLSUNICAST            0x8847
#define ETHERTYPE_MPLSMULTICAST          0x8848
#define ETHERTYPE_PPPoEDISCOVERY         0x8863
#define ETHERTYPE_PPPoESESSION           0x8864
#define ETHERTYPE_EAPOVERLAN             0x888E
#define ETHERTYPE_HYPERSCSI              0x889A
#define ETHERTYPE_ATAOVERETHERNET        0x88A2
#define ETHERTYPE_ETHERCAT               0x88A4
#define ETHERTYPE_SERCOSIII              0x88CD
#define ETHERTYPE_CESoE                  0x88D8
#define ETHERTYPE_MACSECURITY            0x88E5
#define ETHERTYPE_FIBRECHANNEL           0x8906
#define ETHERTYPE_QINQ                   0x9100
#define ETHERTYPE_VLLT                   0xCAFE

#define PROTOCOL_ICMP                    1
#define PROTOCOL_IGMP                    2
#define PROTOCOL_TCP                     6
#define PROTOCOL_UDP                     17
#define PROTOCOL_OSPF                    89
#define PROTOCOL_SCTP                    132



static const MAC_Address_t PROGMEM AdapterMACAddress     = {ADAPTER_MAC_ADDRESS};
static const char PROGMEM AdapterVendorDescription[]     = "LUFA RNDIS Adapter";

uint8_t                 CurrRNDISState              = RNDIS_Uninitialized;
uint32_t                CurrPacketFilter            = 0;



typedef struct
{
    struct
    {
        uint8_t  ControlInterfaceNumber;
        USB_Endpoint_Table_t DataINEndpoint;
        USB_Endpoint_Table_t DataOUTEndpoint;
        USB_Endpoint_Table_t NotificationEndpoint;
        char*         AdapterVendorDescription;
        MAC_Address_t AdapterMACAddress;
        uint8_t*      MessageBuffer;
        uint16_t      MessageBufferLength;
    } Config;

    struct
    {
        bool     ResponseReady;
        uint8_t  CurrRNDISState;
        uint32_t CurrPacketFilter;
    } State;
} USB_ClassInfo_RNDIS_Device_t;

bool RNDIS_Device_ConfigureEndpoints(USB_ClassInfo_RNDIS_Device_t* const RNDISInterfaceInfo) ATTR_NON_NULL_PTR_ARG(1);

void RNDIS_Device_ProcessControlRequest(USB_ClassInfo_RNDIS_Device_t* const RNDISInterfaceInfo) ATTR_NON_NULL_PTR_ARG(1);

void RNDIS_Device_USBTask(USB_ClassInfo_RNDIS_Device_t* const RNDISInterfaceInfo) ATTR_NON_NULL_PTR_ARG(1);

bool RNDIS_Device_IsPacketReceived(USB_ClassInfo_RNDIS_Device_t* const RNDISInterfaceInfo) ATTR_NON_NULL_PTR_ARG(1);

uint8_t RNDIS_Device_ReadPacket(USB_ClassInfo_RNDIS_Device_t* const RNDISInterfaceInfo,
                                            void* Buffer,
                                            uint16_t* const PacketLength) ATTR_NON_NULL_PTR_ARG(1);

uint8_t RNDIS_Device_SendPacket(USB_ClassInfo_RNDIS_Device_t* const RNDISInterfaceInfo,
                                            void* Buffer,
                                            const uint16_t PacketLength) ATTR_NON_NULL_PTR_ARG(1);

#define RNDIS_DEVICE_MIN_MESSAGE_BUFFER_LENGTH  sizeof(AdapterSupportedOIDList) + sizeof(RNDIS_Query_Complete_t)



#define TEMPLATE_FUNC_NAME                        Endpoint_Write_Stream_LE
#define TEMPLATE_BUFFER_TYPE                      const void*
#define TEMPLATE_CLEAR_ENDPOINT()                 Endpoint_ClearIN()
#define TEMPLATE_BUFFER_OFFSET(Length)            0
#define TEMPLATE_BUFFER_MOVE(BufferPtr, Amount)   BufferPtr += Amount
#define TEMPLATE_TRANSFER_BYTE(BufferPtr)         Endpoint_Write_8(*BufferPtr)

#if defined(TEMPLATE_FUNC_NAME)

uint8_t TEMPLATE_FUNC_NAME (TEMPLATE_BUFFER_TYPE const Buffer,
                            uint16_t Length,
                            uint16_t* const BytesProcessed)
{
    uint8_t* DataStream      = ((uint8_t*)Buffer + TEMPLATE_BUFFER_OFFSET(Length));
    uint16_t BytesInTransfer = 0;
    uint8_t  ErrorCode;

    if ((ErrorCode = Endpoint_WaitUntilReady()))
      return ErrorCode;

    if (BytesProcessed != NULL)
    {
        Length -= *BytesProcessed;
        TEMPLATE_BUFFER_MOVE(DataStream, *BytesProcessed);
    }

    while (Length)
    {
        if (!(Endpoint_IsReadWriteAllowed()))
        {
            TEMPLATE_CLEAR_ENDPOINT();

#if !defined(INTERRUPT_CONTROL_ENDPOINT)
            USB_USBTask();
#endif

            if (BytesProcessed != NULL)
            {
                *BytesProcessed += BytesInTransfer;
                return ENDPOINT_RWSTREAM_IncompleteTransfer;
            }

            if ((ErrorCode = Endpoint_WaitUntilReady()))
              return ErrorCode;
        }
        else
        {
            TEMPLATE_TRANSFER_BYTE(DataStream);
            TEMPLATE_BUFFER_MOVE(DataStream, 1);
            Length--;
            BytesInTransfer++;
        }
    }

    return ENDPOINT_RWSTREAM_NoError;
}

#undef TEMPLATE_FUNC_NAME
#undef TEMPLATE_BUFFER_TYPE
#undef TEMPLATE_TRANSFER_BYTE
#undef TEMPLATE_CLEAR_ENDPOINT
#undef TEMPLATE_BUFFER_OFFSET
#undef TEMPLATE_BUFFER_MOVE

#endif

#define TEMPLATE_FUNC_NAME                        Endpoint_Write_Stream_BE
#define TEMPLATE_BUFFER_TYPE                      const void*
#define TEMPLATE_CLEAR_ENDPOINT()                 Endpoint_ClearIN()
#define TEMPLATE_BUFFER_OFFSET(Length)            (Length - 1)
#define TEMPLATE_BUFFER_MOVE(BufferPtr, Amount)   BufferPtr -= Amount
#define TEMPLATE_TRANSFER_BYTE(BufferPtr)         Endpoint_Write_8(*BufferPtr)

#if defined(TEMPLATE_FUNC_NAME)

uint8_t Endpoint_Write_Stream_BE(const void * const Buffer,
                            uint16_t Length,
                            uint16_t* const BytesProcessed)
{
    uint8_t* DataStream      = ((uint8_t*)Buffer + TEMPLATE_BUFFER_OFFSET(Length));
    uint16_t BytesInTransfer = 0;
    uint8_t  ErrorCode;

    if ((ErrorCode = Endpoint_WaitUntilReady()))
      return ErrorCode;

    if (BytesProcessed != NULL)
    {
        Length -= *BytesProcessed;
        TEMPLATE_BUFFER_MOVE(DataStream, *BytesProcessed);
    }

    while (Length)
    {
        if (!(Endpoint_IsReadWriteAllowed()))
        {
            TEMPLATE_CLEAR_ENDPOINT();
            USB_USBTask();

            if (BytesProcessed != NULL)
            {
                *BytesProcessed += BytesInTransfer;
                return ENDPOINT_RWSTREAM_IncompleteTransfer;
            }

            if ((ErrorCode = Endpoint_WaitUntilReady()))
              return ErrorCode;
        }
        else
        {
            TEMPLATE_TRANSFER_BYTE(DataStream);
            TEMPLATE_BUFFER_MOVE(DataStream, 1);
            Length--;
            BytesInTransfer++;
        }
    }

    return ENDPOINT_RWSTREAM_NoError;
}

#undef TEMPLATE_FUNC_NAME
#undef TEMPLATE_BUFFER_TYPE
#undef TEMPLATE_TRANSFER_BYTE
#undef TEMPLATE_CLEAR_ENDPOINT
#undef TEMPLATE_BUFFER_OFFSET
#undef TEMPLATE_BUFFER_MOVE

#endif

#define TEMPLATE_FUNC_NAME                        Endpoint_Read_Stream_LE
#define TEMPLATE_BUFFER_TYPE                      void*
#define TEMPLATE_CLEAR_ENDPOINT()                 Endpoint_ClearOUT()
#define TEMPLATE_BUFFER_OFFSET(Length)            0
#define TEMPLATE_BUFFER_MOVE(BufferPtr, Amount)   BufferPtr += Amount
#define TEMPLATE_TRANSFER_BYTE(BufferPtr)         *BufferPtr = Endpoint_Read_8()

#if defined(TEMPLATE_FUNC_NAME)

uint8_t TEMPLATE_FUNC_NAME (TEMPLATE_BUFFER_TYPE const Buffer,
                            uint16_t Length,
                            uint16_t* const BytesProcessed)
{
    uint8_t* DataStream      = ((uint8_t*)Buffer + TEMPLATE_BUFFER_OFFSET(Length));
    uint16_t BytesInTransfer = 0;
    uint8_t  ErrorCode;

    if ((ErrorCode = Endpoint_WaitUntilReady()))
      return ErrorCode;

    if (BytesProcessed != NULL)
    {
        Length -= *BytesProcessed;
        TEMPLATE_BUFFER_MOVE(DataStream, *BytesProcessed);
    }

    while (Length)
    {
        if (!(Endpoint_IsReadWriteAllowed()))
        {
            TEMPLATE_CLEAR_ENDPOINT();

#if !defined(INTERRUPT_CONTROL_ENDPOINT)
            USB_USBTask();
#endif

            if (BytesProcessed != NULL)
            {
                *BytesProcessed += BytesInTransfer;
                return ENDPOINT_RWSTREAM_IncompleteTransfer;
            }

            if ((ErrorCode = Endpoint_WaitUntilReady()))
              return ErrorCode;
        }
        else
        {
            TEMPLATE_TRANSFER_BYTE(DataStream);
            TEMPLATE_BUFFER_MOVE(DataStream, 1);
            Length--;
            BytesInTransfer++;
        }
    }

    return ENDPOINT_RWSTREAM_NoError;
}

#undef TEMPLATE_FUNC_NAME
#undef TEMPLATE_BUFFER_TYPE
#undef TEMPLATE_TRANSFER_BYTE
#undef TEMPLATE_CLEAR_ENDPOINT
#undef TEMPLATE_BUFFER_OFFSET
#undef TEMPLATE_BUFFER_MOVE

#endif

#define TEMPLATE_FUNC_NAME                        Endpoint_Read_Stream_BE
#define TEMPLATE_BUFFER_TYPE                      void*
#define TEMPLATE_CLEAR_ENDPOINT()                 Endpoint_ClearOUT()
#define TEMPLATE_BUFFER_OFFSET(Length)            (Length - 1)
#define TEMPLATE_BUFFER_MOVE(BufferPtr, Amount)   BufferPtr -= Amount
#define TEMPLATE_TRANSFER_BYTE(BufferPtr)         *BufferPtr = Endpoint_Read_8()

#if defined(TEMPLATE_FUNC_NAME)

uint8_t TEMPLATE_FUNC_NAME (TEMPLATE_BUFFER_TYPE const Buffer,
                            uint16_t Length,
                            uint16_t* const BytesProcessed)
{
    uint8_t* DataStream      = ((uint8_t*)Buffer + TEMPLATE_BUFFER_OFFSET(Length));
    uint16_t BytesInTransfer = 0;
    uint8_t  ErrorCode;

    if ((ErrorCode = Endpoint_WaitUntilReady()))
      return ErrorCode;

    if (BytesProcessed != NULL)
    {
        Length -= *BytesProcessed;
        TEMPLATE_BUFFER_MOVE(DataStream, *BytesProcessed);
    }

    while (Length)
    {
        if (!(Endpoint_IsReadWriteAllowed()))
        {
            TEMPLATE_CLEAR_ENDPOINT();

#if !defined(INTERRUPT_CONTROL_ENDPOINT)
            USB_USBTask();
#endif

            if (BytesProcessed != NULL)
            {
                *BytesProcessed += BytesInTransfer;
                return ENDPOINT_RWSTREAM_IncompleteTransfer;
            }

            if ((ErrorCode = Endpoint_WaitUntilReady()))
              return ErrorCode;
        }
        else
        {
            TEMPLATE_TRANSFER_BYTE(DataStream);
            TEMPLATE_BUFFER_MOVE(DataStream, 1);
            Length--;
            BytesInTransfer++;
        }
    }

    return ENDPOINT_RWSTREAM_NoError;
}

#undef TEMPLATE_FUNC_NAME
#undef TEMPLATE_BUFFER_TYPE
#undef TEMPLATE_TRANSFER_BYTE
#undef TEMPLATE_CLEAR_ENDPOINT
#undef TEMPLATE_BUFFER_OFFSET
#undef TEMPLATE_BUFFER_MOVE

#endif

#if defined(ARCH_HAS_FLASH_ADDRESS_SPACE)
#define TEMPLATE_FUNC_NAME                        Endpoint_Write_PStream_LE
#define TEMPLATE_BUFFER_TYPE                      const void*
#define TEMPLATE_CLEAR_ENDPOINT()                 Endpoint_ClearIN()
#define TEMPLATE_BUFFER_OFFSET(Length)            0
#define TEMPLATE_BUFFER_MOVE(BufferPtr, Amount)   BufferPtr += Amount
#define TEMPLATE_TRANSFER_BYTE(BufferPtr)         Endpoint_Write_8(pgm_read_byte(BufferPtr))

#if defined(TEMPLATE_FUNC_NAME)

uint8_t TEMPLATE_FUNC_NAME (TEMPLATE_BUFFER_TYPE const Buffer,
                            uint16_t Length,
                            uint16_t* const BytesProcessed)
{
    uint8_t* DataStream      = ((uint8_t*)Buffer + TEMPLATE_BUFFER_OFFSET(Length));
    uint16_t BytesInTransfer = 0;
    uint8_t  ErrorCode;

    if ((ErrorCode = Endpoint_WaitUntilReady()))
      return ErrorCode;

    if (BytesProcessed != NULL)
    {
        Length -= *BytesProcessed;
        TEMPLATE_BUFFER_MOVE(DataStream, *BytesProcessed);
    }

    while (Length)
    {
        if (!(Endpoint_IsReadWriteAllowed()))
        {
            TEMPLATE_CLEAR_ENDPOINT();

#if !defined(INTERRUPT_CONTROL_ENDPOINT)
            USB_USBTask();
#endif

            if (BytesProcessed != NULL)
            {
                *BytesProcessed += BytesInTransfer;
                return ENDPOINT_RWSTREAM_IncompleteTransfer;
            }

            if ((ErrorCode = Endpoint_WaitUntilReady()))
              return ErrorCode;
        }
        else
        {
            TEMPLATE_TRANSFER_BYTE(DataStream);
            TEMPLATE_BUFFER_MOVE(DataStream, 1);
            Length--;
            BytesInTransfer++;
        }
    }

    return ENDPOINT_RWSTREAM_NoError;
}

#undef TEMPLATE_FUNC_NAME
#undef TEMPLATE_BUFFER_TYPE
#undef TEMPLATE_TRANSFER_BYTE
#undef TEMPLATE_CLEAR_ENDPOINT
#undef TEMPLATE_BUFFER_OFFSET
#undef TEMPLATE_BUFFER_MOVE

#endif

#define TEMPLATE_FUNC_NAME                        Endpoint_Write_PStream_BE
#define TEMPLATE_BUFFER_TYPE                      const void*
#define TEMPLATE_CLEAR_ENDPOINT()                 Endpoint_ClearIN()
#define TEMPLATE_BUFFER_OFFSET(Length)            (Length - 1)
#define TEMPLATE_BUFFER_MOVE(BufferPtr, Amount)   BufferPtr -= Amount
#define TEMPLATE_TRANSFER_BYTE(BufferPtr)         Endpoint_Write_8(pgm_read_byte(BufferPtr))

#if defined(TEMPLATE_FUNC_NAME)

uint8_t TEMPLATE_FUNC_NAME (TEMPLATE_BUFFER_TYPE const Buffer,
                            uint16_t Length,
                            uint16_t* const BytesProcessed)
{
    uint8_t* DataStream      = ((uint8_t*)Buffer + TEMPLATE_BUFFER_OFFSET(Length));
    uint16_t BytesInTransfer = 0;
    uint8_t  ErrorCode;

    if ((ErrorCode = Endpoint_WaitUntilReady()))
      return ErrorCode;

    if (BytesProcessed != NULL)
    {
        Length -= *BytesProcessed;
        TEMPLATE_BUFFER_MOVE(DataStream, *BytesProcessed);
    }

    while (Length)
    {
        if (!(Endpoint_IsReadWriteAllowed()))
        {
            TEMPLATE_CLEAR_ENDPOINT();

#if !defined(INTERRUPT_CONTROL_ENDPOINT)
            USB_USBTask();
#endif

            if (BytesProcessed != NULL)
            {
                *BytesProcessed += BytesInTransfer;
                return ENDPOINT_RWSTREAM_IncompleteTransfer;
            }

            if ((ErrorCode = Endpoint_WaitUntilReady()))
              return ErrorCode;
        }
        else
        {
            TEMPLATE_TRANSFER_BYTE(DataStream);
            TEMPLATE_BUFFER_MOVE(DataStream, 1);
            Length--;
            BytesInTransfer++;
        }
    }

    return ENDPOINT_RWSTREAM_NoError;
}

#undef TEMPLATE_FUNC_NAME
#undef TEMPLATE_BUFFER_TYPE
#undef TEMPLATE_TRANSFER_BYTE
#undef TEMPLATE_CLEAR_ENDPOINT
#undef TEMPLATE_BUFFER_OFFSET
#undef TEMPLATE_BUFFER_MOVE

#endif


#endif

#if defined(ARCH_HAS_EEPROM_ADDRESS_SPACE)
#define TEMPLATE_FUNC_NAME                        Endpoint_Write_EStream_LE
#define TEMPLATE_BUFFER_TYPE                      const void*
#define TEMPLATE_CLEAR_ENDPOINT()                 Endpoint_ClearIN()
#define TEMPLATE_BUFFER_OFFSET(Length)            0
#define TEMPLATE_BUFFER_MOVE(BufferPtr, Amount)   BufferPtr += Amount
#define TEMPLATE_TRANSFER_BYTE(BufferPtr)         Endpoint_Write_8(eeprom_read_byte(BufferPtr))

#if defined(TEMPLATE_FUNC_NAME)

uint8_t TEMPLATE_FUNC_NAME (TEMPLATE_BUFFER_TYPE const Buffer,
                            uint16_t Length,
                            uint16_t* const BytesProcessed)
{
    uint8_t* DataStream      = ((uint8_t*)Buffer + TEMPLATE_BUFFER_OFFSET(Length));
    uint16_t BytesInTransfer = 0;
    uint8_t  ErrorCode;

    if ((ErrorCode = Endpoint_WaitUntilReady()))
      return ErrorCode;

    if (BytesProcessed != NULL)
    {
        Length -= *BytesProcessed;
        TEMPLATE_BUFFER_MOVE(DataStream, *BytesProcessed);
    }

    while (Length)
    {
        if (!(Endpoint_IsReadWriteAllowed()))
        {
            TEMPLATE_CLEAR_ENDPOINT();

#if !defined(INTERRUPT_CONTROL_ENDPOINT)
            USB_USBTask();
#endif

            if (BytesProcessed != NULL)
            {
                *BytesProcessed += BytesInTransfer;
                return ENDPOINT_RWSTREAM_IncompleteTransfer;
            }

            if ((ErrorCode = Endpoint_WaitUntilReady()))
              return ErrorCode;
        }
        else
        {
            TEMPLATE_TRANSFER_BYTE(DataStream);
            TEMPLATE_BUFFER_MOVE(DataStream, 1);
            Length--;
            BytesInTransfer++;
        }
    }

    return ENDPOINT_RWSTREAM_NoError;
}

#undef TEMPLATE_FUNC_NAME
#undef TEMPLATE_BUFFER_TYPE
#undef TEMPLATE_TRANSFER_BYTE
#undef TEMPLATE_CLEAR_ENDPOINT
#undef TEMPLATE_BUFFER_OFFSET
#undef TEMPLATE_BUFFER_MOVE

#endif

#define TEMPLATE_FUNC_NAME                        Endpoint_Write_EStream_BE
#define TEMPLATE_BUFFER_TYPE                      const void*
#define TEMPLATE_CLEAR_ENDPOINT()                 Endpoint_ClearIN()
#define TEMPLATE_BUFFER_OFFSET(Length)            (Length - 1)
#define TEMPLATE_BUFFER_MOVE(BufferPtr, Amount)   BufferPtr -= Amount
#define TEMPLATE_TRANSFER_BYTE(BufferPtr)         Endpoint_Write_8(eeprom_read_byte(BufferPtr))

#if defined(TEMPLATE_FUNC_NAME)

uint8_t TEMPLATE_FUNC_NAME (TEMPLATE_BUFFER_TYPE const Buffer,
                            uint16_t Length,
                            uint16_t* const BytesProcessed)
{
    uint8_t* DataStream      = ((uint8_t*)Buffer + TEMPLATE_BUFFER_OFFSET(Length));
    uint16_t BytesInTransfer = 0;
    uint8_t  ErrorCode;

    if ((ErrorCode = Endpoint_WaitUntilReady()))
      return ErrorCode;

    if (BytesProcessed != NULL)
    {
        Length -= *BytesProcessed;
        TEMPLATE_BUFFER_MOVE(DataStream, *BytesProcessed);
    }

    while (Length)
    {
        if (!(Endpoint_IsReadWriteAllowed()))
        {
            TEMPLATE_CLEAR_ENDPOINT();

#if !defined(INTERRUPT_CONTROL_ENDPOINT)
            USB_USBTask();
#endif

            if (BytesProcessed != NULL)
            {
                *BytesProcessed += BytesInTransfer;
                return ENDPOINT_RWSTREAM_IncompleteTransfer;
            }

            if ((ErrorCode = Endpoint_WaitUntilReady()))
              return ErrorCode;
        }
        else
        {
            TEMPLATE_TRANSFER_BYTE(DataStream);
            TEMPLATE_BUFFER_MOVE(DataStream, 1);
            Length--;
            BytesInTransfer++;
        }
    }

    return ENDPOINT_RWSTREAM_NoError;
}

#undef TEMPLATE_FUNC_NAME
#undef TEMPLATE_BUFFER_TYPE
#undef TEMPLATE_TRANSFER_BYTE
#undef TEMPLATE_CLEAR_ENDPOINT
#undef TEMPLATE_BUFFER_OFFSET
#undef TEMPLATE_BUFFER_MOVE

#endif

#define TEMPLATE_FUNC_NAME                        Endpoint_Read_EStream_LE
#define TEMPLATE_BUFFER_TYPE                      void*
#define TEMPLATE_CLEAR_ENDPOINT()                 Endpoint_ClearOUT()
#define TEMPLATE_BUFFER_OFFSET(Length)            0
#define TEMPLATE_BUFFER_MOVE(BufferPtr, Amount)   BufferPtr += Amount
#define TEMPLATE_TRANSFER_BYTE(BufferPtr)  eeprom_update_byte(BufferPtr, Endpoint_Read_8())

#if defined(TEMPLATE_FUNC_NAME)

uint8_t TEMPLATE_FUNC_NAME (TEMPLATE_BUFFER_TYPE const Buffer,
                            uint16_t Length,
                            uint16_t* const BytesProcessed)
{
    uint8_t* DataStream      = ((uint8_t*)Buffer + TEMPLATE_BUFFER_OFFSET(Length));
    uint16_t BytesInTransfer = 0;
    uint8_t  ErrorCode;

    if ((ErrorCode = Endpoint_WaitUntilReady()))
      return ErrorCode;

    if (BytesProcessed != NULL)
    {
        Length -= *BytesProcessed;
        TEMPLATE_BUFFER_MOVE(DataStream, *BytesProcessed);
    }

    while (Length)
    {
        if (!(Endpoint_IsReadWriteAllowed()))
        {
            TEMPLATE_CLEAR_ENDPOINT();

#if !defined(INTERRUPT_CONTROL_ENDPOINT)
            USB_USBTask();
#endif

            if (BytesProcessed != NULL)
            {
                *BytesProcessed += BytesInTransfer;
                return ENDPOINT_RWSTREAM_IncompleteTransfer;
            }

            if ((ErrorCode = Endpoint_WaitUntilReady()))
              return ErrorCode;
        }
        else
        {
            TEMPLATE_TRANSFER_BYTE(DataStream);
            TEMPLATE_BUFFER_MOVE(DataStream, 1);
            Length--;
            BytesInTransfer++;
        }
    }

    return ENDPOINT_RWSTREAM_NoError;
}

#undef TEMPLATE_FUNC_NAME
#undef TEMPLATE_BUFFER_TYPE
#undef TEMPLATE_TRANSFER_BYTE
#undef TEMPLATE_CLEAR_ENDPOINT
#undef TEMPLATE_BUFFER_OFFSET
#undef TEMPLATE_BUFFER_MOVE

#endif

#define  TEMPLATE_FUNC_NAME                        Endpoint_Read_EStream_BE
#define  TEMPLATE_BUFFER_TYPE                      void*
#define  TEMPLATE_CLEAR_ENDPOINT()                 Endpoint_ClearOUT()
#define  TEMPLATE_BUFFER_OFFSET(Length)            (Length - 1)
#define  TEMPLATE_BUFFER_MOVE(BufferPtr, Amount)   BufferPtr -= Amount
#define  TEMPLATE_TRANSFER_BYTE(BufferPtr)  eeprom_update_byte(BufferPtr, Endpoint_Read_8())

#if defined(TEMPLATE_FUNC_NAME)

uint8_t TEMPLATE_FUNC_NAME (TEMPLATE_BUFFER_TYPE const Buffer,
                            uint16_t Length,
                            uint16_t* const BytesProcessed)
{
    uint8_t* DataStream      = ((uint8_t*)Buffer + TEMPLATE_BUFFER_OFFSET(Length));
    uint16_t BytesInTransfer = 0;
    uint8_t  ErrorCode;

    if ((ErrorCode = Endpoint_WaitUntilReady()))
      return ErrorCode;

    if (BytesProcessed != NULL)
    {
        Length -= *BytesProcessed;
        TEMPLATE_BUFFER_MOVE(DataStream, *BytesProcessed);
    }

    while (Length)
    {
        if (!(Endpoint_IsReadWriteAllowed()))
        {
            TEMPLATE_CLEAR_ENDPOINT();

#if !defined(INTERRUPT_CONTROL_ENDPOINT)
            USB_USBTask();
#endif

            if (BytesProcessed != NULL)
            {
                *BytesProcessed += BytesInTransfer;
                return ENDPOINT_RWSTREAM_IncompleteTransfer;
            }

            if ((ErrorCode = Endpoint_WaitUntilReady()))
              return ErrorCode;
        }
        else
        {
            TEMPLATE_TRANSFER_BYTE(DataStream);
            TEMPLATE_BUFFER_MOVE(DataStream, 1);
            Length--;
            BytesInTransfer++;
        }
    }

    return ENDPOINT_RWSTREAM_NoError;
}

#undef TEMPLATE_FUNC_NAME
#undef TEMPLATE_BUFFER_TYPE
#undef TEMPLATE_TRANSFER_BYTE
#undef TEMPLATE_CLEAR_ENDPOINT
#undef TEMPLATE_BUFFER_OFFSET
#undef TEMPLATE_BUFFER_MOVE

#endif



#endif

#define  TEMPLATE_FUNC_NAME                        Endpoint_Write_Control_Stream_LE
#define  TEMPLATE_BUFFER_OFFSET(Length)            0
#define  TEMPLATE_BUFFER_MOVE(BufferPtr, Amount)   BufferPtr += Amount
#define  TEMPLATE_TRANSFER_BYTE(BufferPtr)         Endpoint_Write_8(*BufferPtr)

#if defined(TEMPLATE_FUNC_NAME)

uint8_t TEMPLATE_FUNC_NAME (const void* const Buffer,
                            uint16_t Length)
{
    uint8_t* DataStream     = ((uint8_t*)Buffer + TEMPLATE_BUFFER_OFFSET(Length));
    bool     LastPacketFull = false;

    if (Length > USB_ControlRequest.wLength)
      Length = USB_ControlRequest.wLength;
    else if (!(Length))
      Endpoint_ClearIN();

    while (Length || LastPacketFull)
    {
        uint8_t USB_DeviceState_LCL = USB_DeviceState;

        if (USB_DeviceState_LCL == DEVICE_STATE_Unattached)
          return ENDPOINT_RWCSTREAM_DeviceDisconnected;
        else if (USB_DeviceState_LCL == DEVICE_STATE_Suspended)
          return ENDPOINT_RWCSTREAM_BusSuspended;
        else if (Endpoint_IsSETUPReceived())
          return ENDPOINT_RWCSTREAM_HostAborted;
        else if (Endpoint_IsOUTReceived())
          break;

        if (Endpoint_IsINReady())
        {
            uint16_t BytesInEndpoint = Endpoint_BytesInEndpoint();

            while (Length && (BytesInEndpoint < USB_Device_ControlEndpointSize))
            {
                TEMPLATE_TRANSFER_BYTE(DataStream);
                TEMPLATE_BUFFER_MOVE(DataStream, 1);
                Length--;
                BytesInEndpoint++;
            }

            LastPacketFull = (BytesInEndpoint == USB_Device_ControlEndpointSize);
            Endpoint_ClearIN();
        }
    }

    while (!(Endpoint_IsOUTReceived()))
    {
        uint8_t USB_DeviceState_LCL = USB_DeviceState;

        if (USB_DeviceState_LCL == DEVICE_STATE_Unattached)
          return ENDPOINT_RWCSTREAM_DeviceDisconnected;
        else if (USB_DeviceState_LCL == DEVICE_STATE_Suspended)
          return ENDPOINT_RWCSTREAM_BusSuspended;
        else if (Endpoint_IsSETUPReceived())
          return ENDPOINT_RWCSTREAM_HostAborted;
    }

    return ENDPOINT_RWCSTREAM_NoError;
}

#undef TEMPLATE_BUFFER_OFFSET
#undef TEMPLATE_BUFFER_MOVE
#undef TEMPLATE_FUNC_NAME
#undef TEMPLATE_TRANSFER_BYTE

#endif


#define  TEMPLATE_FUNC_NAME                        Endpoint_Write_Control_Stream_BE
#define  TEMPLATE_BUFFER_OFFSET(Length)            (Length - 1)
#define  TEMPLATE_BUFFER_MOVE(BufferPtr, Amount)   BufferPtr -= Amount
#define  TEMPLATE_TRANSFER_BYTE(BufferPtr)         Endpoint_Write_8(*BufferPtr)

#if defined(TEMPLATE_FUNC_NAME)

uint8_t TEMPLATE_FUNC_NAME (const void* const Buffer,
                            uint16_t Length)
{
    uint8_t* DataStream     = ((uint8_t*)Buffer + TEMPLATE_BUFFER_OFFSET(Length));
    bool     LastPacketFull = false;

    if (Length > USB_ControlRequest.wLength)
      Length = USB_ControlRequest.wLength;
    else if (!(Length))
      Endpoint_ClearIN();

    while (Length || LastPacketFull)
    {
        uint8_t USB_DeviceState_LCL = USB_DeviceState;

        if (USB_DeviceState_LCL == DEVICE_STATE_Unattached)
          return ENDPOINT_RWCSTREAM_DeviceDisconnected;
        else if (USB_DeviceState_LCL == DEVICE_STATE_Suspended)
          return ENDPOINT_RWCSTREAM_BusSuspended;
        else if (Endpoint_IsSETUPReceived())
          return ENDPOINT_RWCSTREAM_HostAborted;
        else if (Endpoint_IsOUTReceived())
          break;

        if (Endpoint_IsINReady())
        {
            uint16_t BytesInEndpoint = Endpoint_BytesInEndpoint();

            while (Length && (BytesInEndpoint < USB_Device_ControlEndpointSize))
            {
                TEMPLATE_TRANSFER_BYTE(DataStream);
                TEMPLATE_BUFFER_MOVE(DataStream, 1);
                Length--;
                BytesInEndpoint++;
            }

            LastPacketFull = (BytesInEndpoint == USB_Device_ControlEndpointSize);
            Endpoint_ClearIN();
        }
    }

    while (!(Endpoint_IsOUTReceived()))
    {
        uint8_t USB_DeviceState_LCL = USB_DeviceState;

        if (USB_DeviceState_LCL == DEVICE_STATE_Unattached)
          return ENDPOINT_RWCSTREAM_DeviceDisconnected;
        else if (USB_DeviceState_LCL == DEVICE_STATE_Suspended)
          return ENDPOINT_RWCSTREAM_BusSuspended;
        else if (Endpoint_IsSETUPReceived())
          return ENDPOINT_RWCSTREAM_HostAborted;
    }

    return ENDPOINT_RWCSTREAM_NoError;
}

#undef TEMPLATE_BUFFER_OFFSET
#undef TEMPLATE_BUFFER_MOVE
#undef TEMPLATE_FUNC_NAME
#undef TEMPLATE_TRANSFER_BYTE

#endif


#define  TEMPLATE_FUNC_NAME                        Endpoint_Read_Control_Stream_LE
#define  TEMPLATE_BUFFER_OFFSET(Length)            0
#define  TEMPLATE_BUFFER_MOVE(BufferPtr, Amount)   BufferPtr += Amount
#define  TEMPLATE_TRANSFER_BYTE(BufferPtr)         *BufferPtr = Endpoint_Read_8()

#if defined(TEMPLATE_FUNC_NAME)

uint8_t TEMPLATE_FUNC_NAME (void* const Buffer,
                            uint16_t Length)
{
    uint8_t* DataStream = ((uint8_t*)Buffer + TEMPLATE_BUFFER_OFFSET(Length));

    if (!(Length))
      Endpoint_ClearOUT();

    while (Length)
    {
        uint8_t USB_DeviceState_LCL = USB_DeviceState;

        if (USB_DeviceState_LCL == DEVICE_STATE_Unattached)
          return ENDPOINT_RWCSTREAM_DeviceDisconnected;
        else if (USB_DeviceState_LCL == DEVICE_STATE_Suspended)
          return ENDPOINT_RWCSTREAM_BusSuspended;
        else if (Endpoint_IsSETUPReceived())
          return ENDPOINT_RWCSTREAM_HostAborted;

        if (Endpoint_IsOUTReceived())
        {
            while (Length && Endpoint_BytesInEndpoint())
            {
                TEMPLATE_TRANSFER_BYTE(DataStream);
                TEMPLATE_BUFFER_MOVE(DataStream, 1);
                Length--;
            }

            Endpoint_ClearOUT();
        }
    }

    while (!(Endpoint_IsINReady()))
    {
        uint8_t USB_DeviceState_LCL = USB_DeviceState;

        if (USB_DeviceState_LCL == DEVICE_STATE_Unattached)
          return ENDPOINT_RWCSTREAM_DeviceDisconnected;
        else if (USB_DeviceState_LCL == DEVICE_STATE_Suspended)
          return ENDPOINT_RWCSTREAM_BusSuspended;
    }

    return ENDPOINT_RWCSTREAM_NoError;
}

#undef TEMPLATE_BUFFER_OFFSET
#undef TEMPLATE_BUFFER_MOVE
#undef TEMPLATE_FUNC_NAME
#undef TEMPLATE_TRANSFER_BYTE

#endif


#define  TEMPLATE_FUNC_NAME                        Endpoint_Read_Control_Stream_BE
#define  TEMPLATE_BUFFER_OFFSET(Length)            (Length - 1)
#define  TEMPLATE_BUFFER_MOVE(BufferPtr, Amount)   BufferPtr -= Amount
#define  TEMPLATE_TRANSFER_BYTE(BufferPtr)         *BufferPtr = Endpoint_Read_8()

#if defined(TEMPLATE_FUNC_NAME)

uint8_t TEMPLATE_FUNC_NAME (void* const Buffer,
                            uint16_t Length)
{
    uint8_t* DataStream = ((uint8_t*)Buffer + TEMPLATE_BUFFER_OFFSET(Length));

    if (!(Length))
      Endpoint_ClearOUT();

    while (Length)
    {
        uint8_t USB_DeviceState_LCL = USB_DeviceState;

        if (USB_DeviceState_LCL == DEVICE_STATE_Unattached)
          return ENDPOINT_RWCSTREAM_DeviceDisconnected;
        else if (USB_DeviceState_LCL == DEVICE_STATE_Suspended)
          return ENDPOINT_RWCSTREAM_BusSuspended;
        else if (Endpoint_IsSETUPReceived())
          return ENDPOINT_RWCSTREAM_HostAborted;

        if (Endpoint_IsOUTReceived())
        {
            while (Length && Endpoint_BytesInEndpoint())
            {
                TEMPLATE_TRANSFER_BYTE(DataStream);
                TEMPLATE_BUFFER_MOVE(DataStream, 1);
                Length--;
            }

            Endpoint_ClearOUT();
        }
    }

    while (!(Endpoint_IsINReady()))
    {
        uint8_t USB_DeviceState_LCL = USB_DeviceState;

        if (USB_DeviceState_LCL == DEVICE_STATE_Unattached)
          return ENDPOINT_RWCSTREAM_DeviceDisconnected;
        else if (USB_DeviceState_LCL == DEVICE_STATE_Suspended)
          return ENDPOINT_RWCSTREAM_BusSuspended;
    }

    return ENDPOINT_RWCSTREAM_NoError;
}

#undef TEMPLATE_BUFFER_OFFSET
#undef TEMPLATE_BUFFER_MOVE
#undef TEMPLATE_FUNC_NAME
#undef TEMPLATE_TRANSFER_BYTE

#endif


#if defined(ARCH_HAS_FLASH_ADDRESS_SPACE)
#define  TEMPLATE_FUNC_NAME                        Endpoint_Write_Control_PStream_LE
#define  TEMPLATE_BUFFER_OFFSET(Length)            0
#define  TEMPLATE_BUFFER_MOVE(BufferPtr, Amount)   BufferPtr += Amount
#define  TEMPLATE_TRANSFER_BYTE(BufferPtr)         Endpoint_Write_8(pgm_read_byte(BufferPtr))

#if defined(TEMPLATE_FUNC_NAME)

uint8_t TEMPLATE_FUNC_NAME (const void* const Buffer,
                            uint16_t Length)
{
    uint8_t* DataStream     = ((uint8_t*)Buffer + TEMPLATE_BUFFER_OFFSET(Length));
    bool     LastPacketFull = false;

    if (Length > USB_ControlRequest.wLength)
      Length = USB_ControlRequest.wLength;
    else if (!(Length))
      Endpoint_ClearIN();

    while (Length || LastPacketFull)
    {
        uint8_t USB_DeviceState_LCL = USB_DeviceState;

        if (USB_DeviceState_LCL == DEVICE_STATE_Unattached)
          return ENDPOINT_RWCSTREAM_DeviceDisconnected;
        else if (USB_DeviceState_LCL == DEVICE_STATE_Suspended)
          return ENDPOINT_RWCSTREAM_BusSuspended;
        else if (Endpoint_IsSETUPReceived())
          return ENDPOINT_RWCSTREAM_HostAborted;
        else if (Endpoint_IsOUTReceived())
          break;

        if (Endpoint_IsINReady())
        {
            uint16_t BytesInEndpoint = Endpoint_BytesInEndpoint();

            while (Length && (BytesInEndpoint < USB_Device_ControlEndpointSize))
            {
                TEMPLATE_TRANSFER_BYTE(DataStream);
                TEMPLATE_BUFFER_MOVE(DataStream, 1);
                Length--;
                BytesInEndpoint++;
            }

            LastPacketFull = (BytesInEndpoint == USB_Device_ControlEndpointSize);
            Endpoint_ClearIN();
        }
    }

    while (!(Endpoint_IsOUTReceived()))
    {
        uint8_t USB_DeviceState_LCL = USB_DeviceState;

        if (USB_DeviceState_LCL == DEVICE_STATE_Unattached)
          return ENDPOINT_RWCSTREAM_DeviceDisconnected;
        else if (USB_DeviceState_LCL == DEVICE_STATE_Suspended)
          return ENDPOINT_RWCSTREAM_BusSuspended;
        else if (Endpoint_IsSETUPReceived())
          return ENDPOINT_RWCSTREAM_HostAborted;
    }

    return ENDPOINT_RWCSTREAM_NoError;
}

#undef TEMPLATE_BUFFER_OFFSET
#undef TEMPLATE_BUFFER_MOVE
#undef TEMPLATE_FUNC_NAME
#undef TEMPLATE_TRANSFER_BYTE

#endif

#define  TEMPLATE_FUNC_NAME                        Endpoint_Write_Control_PStream_BE
#define  TEMPLATE_BUFFER_OFFSET(Length)            (Length - 1)
#define  TEMPLATE_BUFFER_MOVE(BufferPtr, Amount)   BufferPtr -= Amount
#define  TEMPLATE_TRANSFER_BYTE(BufferPtr)         Endpoint_Write_8(pgm_read_byte(BufferPtr))
#endif

#if defined(TEMPLATE_FUNC_NAME)

uint8_t TEMPLATE_FUNC_NAME (const void* const Buffer,
                            uint16_t Length)
{
    uint8_t* DataStream     = ((uint8_t*)Buffer + TEMPLATE_BUFFER_OFFSET(Length));
    bool     LastPacketFull = false;

    if (Length > USB_ControlRequest.wLength)
      Length = USB_ControlRequest.wLength;
    else if (!(Length))
      Endpoint_ClearIN();

    while (Length || LastPacketFull)
    {
        uint8_t USB_DeviceState_LCL = USB_DeviceState;

        if (USB_DeviceState_LCL == DEVICE_STATE_Unattached)
          return ENDPOINT_RWCSTREAM_DeviceDisconnected;
        else if (USB_DeviceState_LCL == DEVICE_STATE_Suspended)
          return ENDPOINT_RWCSTREAM_BusSuspended;
        else if (Endpoint_IsSETUPReceived())
          return ENDPOINT_RWCSTREAM_HostAborted;
        else if (Endpoint_IsOUTReceived())
          break;

        if (Endpoint_IsINReady())
        {
            uint16_t BytesInEndpoint = Endpoint_BytesInEndpoint();

            while (Length && (BytesInEndpoint < USB_Device_ControlEndpointSize))
            {
                TEMPLATE_TRANSFER_BYTE(DataStream);
                TEMPLATE_BUFFER_MOVE(DataStream, 1);
                Length--;
                BytesInEndpoint++;
            }

            LastPacketFull = (BytesInEndpoint == USB_Device_ControlEndpointSize);
            Endpoint_ClearIN();
        }
    }

    while (!(Endpoint_IsOUTReceived()))
    {
        uint8_t USB_DeviceState_LCL = USB_DeviceState;

        if (USB_DeviceState_LCL == DEVICE_STATE_Unattached)
          return ENDPOINT_RWCSTREAM_DeviceDisconnected;
        else if (USB_DeviceState_LCL == DEVICE_STATE_Suspended)
          return ENDPOINT_RWCSTREAM_BusSuspended;
        else if (Endpoint_IsSETUPReceived())
          return ENDPOINT_RWCSTREAM_HostAborted;
    }

    return ENDPOINT_RWCSTREAM_NoError;
}

#undef TEMPLATE_BUFFER_OFFSET
#undef TEMPLATE_BUFFER_MOVE
#undef TEMPLATE_FUNC_NAME
#undef TEMPLATE_TRANSFER_BYTE

#endif

#if defined(ARCH_HAS_EEPROM_ADDRESS_SPACE)
#define  TEMPLATE_FUNC_NAME                        Endpoint_Write_Control_EStream_LE
#define  TEMPLATE_BUFFER_OFFSET(Length)            0
#define  TEMPLATE_BUFFER_MOVE(BufferPtr, Amount)   BufferPtr += Amount
#define  TEMPLATE_TRANSFER_BYTE(BufferPtr)         Endpoint_Write_8(eeprom_read_byte(BufferPtr))

#if defined(TEMPLATE_FUNC_NAME)

uint8_t TEMPLATE_FUNC_NAME (const void* const Buffer,
                            uint16_t Length)
{
    uint8_t* DataStream     = ((uint8_t*)Buffer + TEMPLATE_BUFFER_OFFSET(Length));
    bool     LastPacketFull = false;

    if (Length > USB_ControlRequest.wLength)
      Length = USB_ControlRequest.wLength;
    else if (!(Length))
      Endpoint_ClearIN();

    while (Length || LastPacketFull)
    {
        uint8_t USB_DeviceState_LCL = USB_DeviceState;

        if (USB_DeviceState_LCL == DEVICE_STATE_Unattached)
          return ENDPOINT_RWCSTREAM_DeviceDisconnected;
        else if (USB_DeviceState_LCL == DEVICE_STATE_Suspended)
          return ENDPOINT_RWCSTREAM_BusSuspended;
        else if (Endpoint_IsSETUPReceived())
          return ENDPOINT_RWCSTREAM_HostAborted;
        else if (Endpoint_IsOUTReceived())
          break;

        if (Endpoint_IsINReady())
        {
            uint16_t BytesInEndpoint = Endpoint_BytesInEndpoint();

            while (Length && (BytesInEndpoint < USB_Device_ControlEndpointSize))
            {
                TEMPLATE_TRANSFER_BYTE(DataStream);
                TEMPLATE_BUFFER_MOVE(DataStream, 1);
                Length--;
                BytesInEndpoint++;
            }

            LastPacketFull = (BytesInEndpoint == USB_Device_ControlEndpointSize);
            Endpoint_ClearIN();
        }
    }

    while (!(Endpoint_IsOUTReceived()))
    {
        uint8_t USB_DeviceState_LCL = USB_DeviceState;

        if (USB_DeviceState_LCL == DEVICE_STATE_Unattached)
          return ENDPOINT_RWCSTREAM_DeviceDisconnected;
        else if (USB_DeviceState_LCL == DEVICE_STATE_Suspended)
          return ENDPOINT_RWCSTREAM_BusSuspended;
        else if (Endpoint_IsSETUPReceived())
          return ENDPOINT_RWCSTREAM_HostAborted;
    }

    return ENDPOINT_RWCSTREAM_NoError;
}

#undef TEMPLATE_BUFFER_OFFSET
#undef TEMPLATE_BUFFER_MOVE
#undef TEMPLATE_FUNC_NAME
#undef TEMPLATE_TRANSFER_BYTE

#endif


#define  TEMPLATE_FUNC_NAME                        Endpoint_Write_Control_EStream_BE
#define  TEMPLATE_BUFFER_OFFSET(Length)            (Length - 1)
#define  TEMPLATE_BUFFER_MOVE(BufferPtr, Amount)   BufferPtr -= Amount
#define  TEMPLATE_TRANSFER_BYTE(BufferPtr)         Endpoint_Write_8(eeprom_read_byte(BufferPtr))

#if defined(TEMPLATE_FUNC_NAME)

uint8_t TEMPLATE_FUNC_NAME (const void* const Buffer,
                            uint16_t Length)
{
    uint8_t* DataStream     = ((uint8_t*)Buffer + TEMPLATE_BUFFER_OFFSET(Length));
    bool     LastPacketFull = false;

    if (Length > USB_ControlRequest.wLength)
      Length = USB_ControlRequest.wLength;
    else if (!(Length))
      Endpoint_ClearIN();

    while (Length || LastPacketFull)
    {
        uint8_t USB_DeviceState_LCL = USB_DeviceState;

        if (USB_DeviceState_LCL == DEVICE_STATE_Unattached)
          return ENDPOINT_RWCSTREAM_DeviceDisconnected;
        else if (USB_DeviceState_LCL == DEVICE_STATE_Suspended)
          return ENDPOINT_RWCSTREAM_BusSuspended;
        else if (Endpoint_IsSETUPReceived())
          return ENDPOINT_RWCSTREAM_HostAborted;
        else if (Endpoint_IsOUTReceived())
          break;

        if (Endpoint_IsINReady())
        {
            uint16_t BytesInEndpoint = Endpoint_BytesInEndpoint();

            while (Length && (BytesInEndpoint < USB_Device_ControlEndpointSize))
            {
                TEMPLATE_TRANSFER_BYTE(DataStream);
                TEMPLATE_BUFFER_MOVE(DataStream, 1);
                Length--;
                BytesInEndpoint++;
            }

            LastPacketFull = (BytesInEndpoint == USB_Device_ControlEndpointSize);
            Endpoint_ClearIN();
        }
    }

    while (!(Endpoint_IsOUTReceived()))
    {
        uint8_t USB_DeviceState_LCL = USB_DeviceState;

        if (USB_DeviceState_LCL == DEVICE_STATE_Unattached)
          return ENDPOINT_RWCSTREAM_DeviceDisconnected;
        else if (USB_DeviceState_LCL == DEVICE_STATE_Suspended)
          return ENDPOINT_RWCSTREAM_BusSuspended;
        else if (Endpoint_IsSETUPReceived())
          return ENDPOINT_RWCSTREAM_HostAborted;
    }

    return ENDPOINT_RWCSTREAM_NoError;
}

#undef TEMPLATE_BUFFER_OFFSET
#undef TEMPLATE_BUFFER_MOVE
#undef TEMPLATE_FUNC_NAME
#undef TEMPLATE_TRANSFER_BYTE

#endif


#define  TEMPLATE_FUNC_NAME                        Endpoint_Read_Control_EStream_LE
#define  TEMPLATE_BUFFER_OFFSET(Length)            0
#define  TEMPLATE_BUFFER_MOVE(BufferPtr, Amount)   BufferPtr += Amount
#define  TEMPLATE_TRANSFER_BYTE(BufferPtr) eeprom_update_byte(BufferPtr, Endpoint_Read_8())

#if defined(TEMPLATE_FUNC_NAME)

uint8_t TEMPLATE_FUNC_NAME (void* const Buffer,
                            uint16_t Length)
{
    uint8_t* DataStream = ((uint8_t*)Buffer + TEMPLATE_BUFFER_OFFSET(Length));

    if (!(Length))
      Endpoint_ClearOUT();

    while (Length)
    {
        uint8_t USB_DeviceState_LCL = USB_DeviceState;

        if (USB_DeviceState_LCL == DEVICE_STATE_Unattached)
          return ENDPOINT_RWCSTREAM_DeviceDisconnected;
        else if (USB_DeviceState_LCL == DEVICE_STATE_Suspended)
          return ENDPOINT_RWCSTREAM_BusSuspended;
        else if (Endpoint_IsSETUPReceived())
          return ENDPOINT_RWCSTREAM_HostAborted;

        if (Endpoint_IsOUTReceived())
        {
            while (Length && Endpoint_BytesInEndpoint())
            {
                TEMPLATE_TRANSFER_BYTE(DataStream);
                TEMPLATE_BUFFER_MOVE(DataStream, 1);
                Length--;
            }

            Endpoint_ClearOUT();
        }
    }

    while (!(Endpoint_IsINReady()))
    {
        uint8_t USB_DeviceState_LCL = USB_DeviceState;

        if (USB_DeviceState_LCL == DEVICE_STATE_Unattached)
          return ENDPOINT_RWCSTREAM_DeviceDisconnected;
        else if (USB_DeviceState_LCL == DEVICE_STATE_Suspended)
          return ENDPOINT_RWCSTREAM_BusSuspended;
    }

    return ENDPOINT_RWCSTREAM_NoError;
}

#undef TEMPLATE_BUFFER_OFFSET
#undef TEMPLATE_BUFFER_MOVE
#undef TEMPLATE_FUNC_NAME
#undef TEMPLATE_TRANSFER_BYTE

#endif


#define  TEMPLATE_FUNC_NAME                        Endpoint_Read_Control_EStream_BE
#define  TEMPLATE_BUFFER_OFFSET(Length)            (Length - 1)
#define  TEMPLATE_BUFFER_MOVE(BufferPtr, Amount)   BufferPtr -= Amount
#define  TEMPLATE_TRANSFER_BYTE(BufferPtr) eeprom_update_byte(BufferPtr, Endpoint_Read_8())

#if defined(TEMPLATE_FUNC_NAME)

uint8_t TEMPLATE_FUNC_NAME (void* const Buffer,
                            uint16_t Length)
{
    uint8_t* DataStream = ((uint8_t*)Buffer + TEMPLATE_BUFFER_OFFSET(Length));

    if (!(Length))
      Endpoint_ClearOUT();

    while (Length)
    {
        uint8_t USB_DeviceState_LCL = USB_DeviceState;

        if (USB_DeviceState_LCL == DEVICE_STATE_Unattached)
          return ENDPOINT_RWCSTREAM_DeviceDisconnected;
        else if (USB_DeviceState_LCL == DEVICE_STATE_Suspended)
          return ENDPOINT_RWCSTREAM_BusSuspended;
        else if (Endpoint_IsSETUPReceived())
          return ENDPOINT_RWCSTREAM_HostAborted;

        if (Endpoint_IsOUTReceived())
        {
            while (Length && Endpoint_BytesInEndpoint())
            {
                TEMPLATE_TRANSFER_BYTE(DataStream);
                TEMPLATE_BUFFER_MOVE(DataStream, 1);
                Length--;
            }

            Endpoint_ClearOUT();
        }
    }

    while (!(Endpoint_IsINReady()))
    {
        uint8_t USB_DeviceState_LCL = USB_DeviceState;

        if (USB_DeviceState_LCL == DEVICE_STATE_Unattached)
          return ENDPOINT_RWCSTREAM_DeviceDisconnected;
        else if (USB_DeviceState_LCL == DEVICE_STATE_Suspended)
          return ENDPOINT_RWCSTREAM_BusSuspended;
    }

    return ENDPOINT_RWCSTREAM_NoError;
}

#undef TEMPLATE_BUFFER_OFFSET
#undef TEMPLATE_BUFFER_MOVE
#undef TEMPLATE_FUNC_NAME
#undef TEMPLATE_TRANSFER_BYTE

#endif

#endif

#if defined(USB_CAN_BE_DEVICE)



uint8_t USB_Device_ConfigurationNumber;

#if !defined(NO_DEVICE_SELF_POWER)
bool    USB_Device_CurrentlySelfPowered;
#endif

#if !defined(NO_DEVICE_REMOTE_WAKEUP)
bool    USB_Device_RemoteWakeupEnabled;
#endif

static void USB_Device_SetAddress(void)
{
	uint8_t DeviceAddress = (USB_ControlRequest.wValue & 0x7F);

	USB_Device_SetDeviceAddress(DeviceAddress);

	Endpoint_ClearSETUP();

	Endpoint_ClearStatusStage();

	while (!(Endpoint_IsINReady()));

	USB_Device_EnableDeviceAddress(DeviceAddress);

	USB_DeviceState = (DeviceAddress) ? DEVICE_STATE_Addressed : DEVICE_STATE_Default;
}

#define CDC_TX_EPADDR                  (ENDPOINT_DIR_IN  | 1)
#define CDC_RX_EPADDR                  (ENDPOINT_DIR_OUT | 2)
#define CDC_NOTIFICATION_EPADDR        (ENDPOINT_DIR_IN  | 3)
#define CDC_TXRX_EPSIZE                64
#define CDC_NOTIFICATION_EPSIZE        8



void EVENT_USB_Device_ConfigurationChanged(void)
{
	Endpoint_ConfigureEndpoint(CDC_TX_EPADDR, EP_TYPE_BULK, CDC_TXRX_EPSIZE, 1);
	Endpoint_ConfigureEndpoint(CDC_RX_EPADDR, EP_TYPE_BULK, CDC_TXRX_EPSIZE, 1);
	Endpoint_ConfigureEndpoint(CDC_NOTIFICATION_EPADDR, EP_TYPE_INTERRUPT, CDC_NOTIFICATION_EPSIZE, 1);
}

static void USB_Device_SetConfiguration(void)
{
	#if defined(FIXED_NUM_CONFIGURATIONS)
	if ((uint8_t)USB_ControlRequest.wValue > FIXED_NUM_CONFIGURATIONS)
	  return;
	#else
	USB_Descriptor_Device_t* DevDescriptorPtr;

	#if defined(ARCH_HAS_MULTI_ADDRESS_SPACE)
		#if defined(USE_FLASH_DESCRIPTORS)
			#define MemoryAddressSpace  MEMSPACE_FLASH
		#elif defined(USE_EEPROM_DESCRIPTORS)
			#define MemoryAddressSpace  MEMSPACE_EEPROM
		#elif defined(USE_RAM_DESCRIPTORS)
			#define MemoryAddressSpace  MEMSPACE_RAM
		#else
			uint8_t MemoryAddressSpace;
		#endif
	#endif

	if (CALLBACK_USB_GetDescriptor((DTYPE_Device << 8), 0, (void*)&DevDescriptorPtr
#if defined(ARCH_HAS_MULTI_ADDRESS_SPACE) && \
	    !(defined(USE_FLASH_DESCRIPTORS) || defined(USE_EEPROM_DESCRIPTORS) || defined(USE_RAM_DESCRIPTORS))
	                               , &MemoryAddressSpace
	#endif
	                               ) == NO_DESCRIPTOR)
	{
		return;
	}

#if defined(ARCH_HAS_MULTI_ADDRESS_SPACE)
	if (MemoryAddressSpace == MEMSPACE_FLASH)
	{
		if (((uint8_t)USB_ControlRequest.wValue > pgm_read_byte(&DevDescriptorPtr->NumberOfConfigurations)))
		  return;
	}
	else if (MemoryAddressSpace == MEMSPACE_EEPROM)
	{
		if (((uint8_t)USB_ControlRequest.wValue > eeprom_read_byte(&DevDescriptorPtr->NumberOfConfigurations)))
		  return;
	}
	else
	{
		if ((uint8_t)USB_ControlRequest.wValue > DevDescriptorPtr->NumberOfConfigurations)
		  return;
	}
	#else
	if ((uint8_t)USB_ControlRequest.wValue > DevDescriptorPtr->NumberOfConfigurations)
	  return;
	#endif
	#endif

	Endpoint_ClearSETUP();

	USB_Device_ConfigurationNumber = (uint8_t)USB_ControlRequest.wValue;

	Endpoint_ClearStatusStage();

	if (USB_Device_ConfigurationNumber)
	  USB_DeviceState = DEVICE_STATE_Configured;
	else
	  USB_DeviceState = (USB_Device_IsAddressSet()) ? DEVICE_STATE_Configured : DEVICE_STATE_Powered;

	EVENT_USB_Device_ConfigurationChanged();
}

void USB_Device_SendRemoteWakeup(void)
{
    if (!(USB_Options & USB_OPT_MANUAL_PLL))
    {
        USB_PLL_On();
        while (!(USB_PLL_IsReady()));
    }

    USB_CLK_Unfreeze();

    UDCON |= (1 << RMWKUP);
    while (UDCON & (1 << RMWKUP));
}

void USB_INT_DisableAllInterrupts(void)
{
    USBCON &= ~(1 << VBUSTE);
    UDIEN   = 0;
}

void USB_INT_ClearAllInterrupts(void)
{
    USBINT = 0;
    UDINT  = 0;
}

void USB_GetNextDescriptorOfType(uint16_t* const BytesRem, void** const CurrConfigLoc,
                                 const uint8_t Type)
{
    while (*BytesRem)
    {
        USB_GetNextDescriptor(BytesRem, CurrConfigLoc);

        if (DESCRIPTOR_TYPE(*CurrConfigLoc) == Type)
          return;
    }
}

void USB_GetNextDescriptorOfTypeBefore(uint16_t* const BytesRem,
         void** const CurrConfigLoc, const uint8_t Type, const uint8_t BeforeType)
{
    while (*BytesRem)
    {
        USB_GetNextDescriptor(BytesRem, CurrConfigLoc);

        if (DESCRIPTOR_TYPE(*CurrConfigLoc) == Type)
        {
            return;
        }
        else if (DESCRIPTOR_TYPE(*CurrConfigLoc) == BeforeType)
        {
            *BytesRem = 0;
            return;
        }
    }
}

void USB_GetNextDescriptorOfTypeAfter(uint16_t* const BytesRem,
                                      void** const CurrConfigLoc,
                                      const uint8_t Type,
                                      const uint8_t AfterType)
{
    USB_GetNextDescriptorOfType(BytesRem, CurrConfigLoc, AfterType);

    if (*BytesRem)
      USB_GetNextDescriptorOfType(BytesRem, CurrConfigLoc, Type);
}

uint8_t USB_GetNextDescriptorComp(uint16_t* const BytesRem, void** const CurrConfigLoc,
                                  ConfigComparatorPtr_t const ComparatorRoutine)
{
    uint8_t ErrorCode;

    while (*BytesRem)
    {
        uint8_t *PrevDescLoc  = (uint8_t *)*CurrConfigLoc;
        uint16_t PrevBytesRem = *BytesRem;

        USB_GetNextDescriptor(BytesRem, CurrConfigLoc);

        if ((ErrorCode = ComparatorRoutine(*CurrConfigLoc)) != DESCRIPTOR_SEARCH_NotFound)
        {
            if (ErrorCode == DESCRIPTOR_SEARCH_Fail)
            {
                *CurrConfigLoc = PrevDescLoc;
                *BytesRem      = PrevBytesRem;
            }

            return ErrorCode;
        }
    }

    return DESCRIPTOR_SEARCH_COMP_EndOfDescriptor;
}


static void USB_Device_GetConfiguration(void)
{
	Endpoint_ClearSETUP();
	Endpoint_Write_8(USB_Device_ConfigurationNumber);
	Endpoint_ClearIN();
	Endpoint_ClearStatusStage();
}

static void USB_Device_GetInternalSerialDescriptor(void)
{
	struct
	{
		USB_Descriptor_Header_t Header;
		uint16_t                UnicodeString[INTERNAL_SERIAL_LENGTH_BITS / 4];
	} SignatureDescriptor;

	SignatureDescriptor.Header.Type = DTYPE_String;
	SignatureDescriptor.Header.Size = USB_STRING_LEN(INTERNAL_SERIAL_LENGTH_BITS / 4);
	USB_Device_GetSerialString(SignatureDescriptor.UnicodeString);
	Endpoint_ClearSETUP();
	Endpoint_Write_Control_Stream_LE(&SignatureDescriptor, sizeof(SignatureDescriptor));
	Endpoint_ClearOUT();
}

bool Endpoint_ConfigureEndpointTable(const USB_Endpoint_Table_t* const Table,
                                     const uint8_t Entries)
{
    for (uint8_t i = 0; i < Entries; i++)
    {
        if (!(Table[i].Address))
          continue;

        if (!(Endpoint_ConfigureEndpoint(Table[i].Address, Table[i].Type, Table[i].Size, Table[i].Banks)))
          return false;
    }

    return true;
}

bool Endpoint_ConfigureEndpoint_Prv(const uint8_t Number,
                                    const uint8_t UECFG0XData,
                                    const uint8_t UECFG1XData)
{
    for (uint8_t EPNum = Number; EPNum < ENDPOINT_TOTAL_ENDPOINTS; EPNum++)
    {
        uint8_t UECFG0XTemp;
        uint8_t UECFG1XTemp;
        uint8_t UEIENXTemp;

        Endpoint_SelectEndpoint(EPNum);

        if (EPNum == Number)
        {
            UECFG0XTemp = UECFG0XData;
            UECFG1XTemp = UECFG1XData;
            UEIENXTemp  = 0;
        }
        else
        {
            UECFG0XTemp = UECFG0X;
            UECFG1XTemp = UECFG1X;
            UEIENXTemp  = UEIENX;
        }

        if (!(UECFG1XTemp & (1 << ALLOC)))
          continue;

        Endpoint_DisableEndpoint();
        UECFG1X &= ~(1 << ALLOC);

        Endpoint_EnableEndpoint();
        UECFG0X = UECFG0XTemp;
        UECFG1X = UECFG1XTemp;
        UEIENX  = UEIENXTemp;

        if (!(Endpoint_IsConfigured()))
          return false;
    }

    Endpoint_SelectEndpoint(Number);
    return true;
}

#if (ARCH == ARCH_AVR8)
void USB_Init(void)
{
    USB_OTGPAD_Off();

    if (!(USB_Options & USB_OPT_REG_DISABLED))
        USB_REG_On();
    else
        USB_REG_Off();

    if (!(USB_Options & USB_OPT_MANUAL_PLL))
    {
        PLLFRQ = (1 << PDIV2);
    }

    USB_IsInitialized = true;
    USB_ResetInterface();
}

void USB_Disable(void)
{
    USB_INT_DisableAllInterrupts();
    USB_INT_ClearAllInterrupts();
    USB_Detach();
    USB_Controller_Disable();

    if (!(USB_Options & USB_OPT_MANUAL_PLL))
      USB_PLL_Off();

    if (!(USB_Options & USB_OPT_REG_KEEP_ENABLED))
      USB_REG_Off();

    USB_OTGPAD_Off();
    USB_IsInitialized = false;
}

static void USB_Init_Device(void)
{
    USB_DeviceState = DEVICE_STATE_Unattached;
    USB_Device_ConfigurationNumber = 0;
    USB_Device_RemoteWakeupEnabled = false;
    USB_Device_CurrentlySelfPowered = false;

    if (USB_Options & USB_DEVICE_OPT_LOWSPEED)
      USB_Device_SetLowSpeed();
    else
      USB_Device_SetFullSpeed();

    USB_INT_Enable(USB_INT_VBUSTI);

    Endpoint_ConfigureEndpoint(ENDPOINT_CONTROLEP, EP_TYPE_CONTROL,
                               USB_Device_ControlEndpointSize, 1);

    USB_INT_Clear(USB_INT_SUSPI);
    USB_INT_Enable(USB_INT_SUSPI);
    USB_INT_Enable(USB_INT_EORSTI);
    USB_Attach();
}

void USB_ResetInterface(void)
{
    USB_INT_DisableAllInterrupts();
    USB_INT_ClearAllInterrupts();
    USB_Controller_Reset();
    USB_CLK_Unfreeze();

    if (USB_CurrentMode == USB_MODE_Device)
    {
        if (!(USB_Options & USB_OPT_MANUAL_PLL))
        {
            USB_PLL_Off();
        }

        USB_Init_Device();
    }
    else if (USB_CurrentMode == USB_MODE_Host)
    {
    }

    #if (defined(USB_SERIES_4_AVR) || defined(USB_SERIES_6_AVR) || defined(USB_SERIES_7_AVR))
    USB_OTGPAD_On();
    #endif
}



#endif

void Endpoint_ClearEndpoints(void)
{
    UEINT = 0;

    for (uint8_t EPNum = 0; EPNum < ENDPOINT_TOTAL_ENDPOINTS; EPNum++)
    {
        Endpoint_SelectEndpoint(EPNum);
        UEIENX  = 0;
        UEINTX  = 0;
        UECFG1X = 0;
        Endpoint_DisableEndpoint();
    }
}

void Endpoint_ClearStatusStage(void)
{
    if (USB_ControlRequest.bmRequestType & REQDIR_DEVICETOHOST)
    {
        while (!(Endpoint_IsOUTReceived()))
        {
            if (USB_DeviceState == DEVICE_STATE_Unattached)
              return;
        }

        Endpoint_ClearOUT();
    }
    else
    {
        while (!(Endpoint_IsINReady()))
        {
            if (USB_DeviceState == DEVICE_STATE_Unattached)
              return;
        }

        Endpoint_ClearIN();
    }
}

uint8_t Endpoint_WaitUntilReady(void)
{
    uint8_t  TimeoutMSRem = USB_STREAM_TIMEOUT_MS;

    uint16_t PreviousFrameNumber = USB_Device_GetFrameNumber();

    for (;;)
    {
        if (Endpoint_GetEndpointDirection() == ENDPOINT_DIR_IN)
        {
            if (Endpoint_IsINReady())
              return ENDPOINT_READYWAIT_NoError;
        }
        else
        {
            if (Endpoint_IsOUTReceived())
              return ENDPOINT_READYWAIT_NoError;
        }

        uint8_t USB_DeviceState_LCL = USB_DeviceState;

        if (USB_DeviceState_LCL == DEVICE_STATE_Unattached)
          return ENDPOINT_READYWAIT_DeviceDisconnected;
        else if (USB_DeviceState_LCL == DEVICE_STATE_Suspended)
          return ENDPOINT_READYWAIT_BusSuspended;
        else if (Endpoint_IsStalled())
          return ENDPOINT_READYWAIT_EndpointStalled;

        uint16_t CurrentFrameNumber = USB_Device_GetFrameNumber();

        if (CurrentFrameNumber != PreviousFrameNumber)
        {
            PreviousFrameNumber = CurrentFrameNumber;

            if (!(TimeoutMSRem--))
              return ENDPOINT_READYWAIT_Timeout;
        }
    }
}

static void USB_Device_GetDescriptor(void)
{
	const void* DescriptorPointer;
	uint16_t    DescriptorSize;

	#if defined(ARCH_HAS_MULTI_ADDRESS_SPACE) && \
	    !(defined(USE_FLASH_DESCRIPTORS) || defined(USE_EEPROM_DESCRIPTORS) || defined(USE_RAM_DESCRIPTORS))
	uint8_t DescriptorAddressSpace;
	#endif

	#if !defined(NO_INTERNAL_SERIAL) && (USE_INTERNAL_SERIAL != NO_DESCRIPTOR)
	if (USB_ControlRequest.wValue == ((DTYPE_String << 8) | USE_INTERNAL_SERIAL))
	{
		USB_Device_GetInternalSerialDescriptor();
		return;
	}
	#endif

	if ((DescriptorSize = CALLBACK_USB_GetDescriptor(USB_ControlRequest.wValue, USB_ControlRequest.wIndex,
	             &DescriptorPointer
	#if defined(ARCH_HAS_MULTI_ADDRESS_SPACE) && \
	    !(defined(USE_FLASH_DESCRIPTORS) || defined(USE_EEPROM_DESCRIPTORS) || defined(USE_RAM_DESCRIPTORS))
	           , &DescriptorAddressSpace
	#endif
)) == NO_DESCRIPTOR)
	{
		return;
	}

	Endpoint_ClearSETUP();

	#if defined(USE_RAM_DESCRIPTORS) || !defined(ARCH_HAS_MULTI_ADDRESS_SPACE)
	Endpoint_Write_Control_Stream_LE(DescriptorPointer, DescriptorSize);
	#elif defined(USE_EEPROM_DESCRIPTORS)
	Endpoint_Write_Control_EStream_LE(DescriptorPointer, DescriptorSize);
	#elif defined(USE_FLASH_DESCRIPTORS)
	Endpoint_Write_Control_PStream_LE(DescriptorPointer, DescriptorSize);
	#else
	if (DescriptorAddressSpace == MEMSPACE_FLASH)
	  Endpoint_Write_Control_PStream_LE(DescriptorPointer, DescriptorSize);
	else if (DescriptorAddressSpace == MEMSPACE_EEPROM)
	  Endpoint_Write_Control_EStream_LE(DescriptorPointer, DescriptorSize);
	else
	  Endpoint_Write_Control_Stream_LE(DescriptorPointer, DescriptorSize);
	#endif

	Endpoint_ClearOUT();
}

static void USB_Device_ClearSetFeature(void)
{
	switch (USB_ControlRequest.bmRequestType & CONTROL_REQTYPE_RECIPIENT)
	{
#if !defined(NO_DEVICE_REMOTE_WAKEUP)
		case REQREC_DEVICE:
		{
			if ((uint8_t)USB_ControlRequest.wValue == FEATURE_SEL_DeviceRemoteWakeup)
			  USB_Device_RemoteWakeupEnabled = (USB_ControlRequest.bRequest == REQ_SetFeature);
			else
			  return;

			break;
		}
#endif
#if !defined(CONTROL_ONLY_DEVICE)
		case REQREC_ENDPOINT:
		{
			if ((uint8_t)USB_ControlRequest.wValue == FEATURE_SEL_EndpointHalt)
			{
				uint8_t EndpointIndex = ((uint8_t)USB_ControlRequest.wIndex & ENDPOINT_EPNUM_MASK);

				if (EndpointIndex == ENDPOINT_CONTROLEP || EndpointIndex >= ENDPOINT_TOTAL_ENDPOINTS)
				  return;

				Endpoint_SelectEndpoint(EndpointIndex);

				if (Endpoint_IsEnabled())
				{
					if (USB_ControlRequest.bRequest == REQ_SetFeature)
					{
						Endpoint_StallTransaction();
					}
					else
					{
						Endpoint_ClearStall();
						Endpoint_ResetEndpoint(EndpointIndex);
						Endpoint_ResetDataToggle();
					}
				}
			}

			break;
		}
		#endif
		default:
			return;
	}

	Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);
	Endpoint_ClearSETUP();
	Endpoint_ClearStatusStage();
}

static void USB_Device_GetStatus(void)
{
	uint8_t CurrentStatus = 0;

	switch (USB_ControlRequest.bmRequestType)
	{
		case (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_DEVICE):
		{
			#if !defined(NO_DEVICE_SELF_POWER)
			if (USB_Device_CurrentlySelfPowered)
			  CurrentStatus |= FEATURE_SELFPOWERED_ENABLED;
			#endif

			#if !defined(NO_DEVICE_REMOTE_WAKEUP)
			if (USB_Device_RemoteWakeupEnabled)
			  CurrentStatus |= FEATURE_REMOTE_WAKEUP_ENABLED;
			#endif
			break;
		}
		case (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_ENDPOINT):
		{
			#if !defined(CONTROL_ONLY_DEVICE)
			uint8_t EndpointIndex = ((uint8_t)USB_ControlRequest.wIndex & ENDPOINT_EPNUM_MASK);

			if (EndpointIndex >= ENDPOINT_TOTAL_ENDPOINTS)
				return;

			Endpoint_SelectEndpoint(EndpointIndex);

			CurrentStatus = Endpoint_IsStalled();

			Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);
			#endif

			break;
		}
		default:
			return;
	}

    Endpoint_ClearSETUP();
    Endpoint_Write_16_LE(CurrentStatus);
    Endpoint_ClearIN();
    Endpoint_ClearStatusStage();
}

//extern uint8_t                 RNDISMessageBuffer[];
extern RNDIS_Message_Header_t* MessageHeader;
extern bool                    ResponseReady;
extern uint8_t                 CurrRNDISState;

static const uint32_t PROGMEM AdapterSupportedOIDList[]  =
                            {
                                OID_GEN_SUPPORTED_LIST,
                                OID_GEN_PHYSICAL_MEDIUM,
                                OID_GEN_HARDWARE_STATUS,
                                OID_GEN_MEDIA_SUPPORTED,
                                OID_GEN_MEDIA_IN_USE,
                                OID_GEN_MAXIMUM_FRAME_SIZE,
                                OID_GEN_MAXIMUM_TOTAL_SIZE,
                                OID_GEN_LINK_SPEED,
                                OID_GEN_TRANSMIT_BLOCK_SIZE,
                                OID_GEN_RECEIVE_BLOCK_SIZE,
                                OID_GEN_VENDOR_ID,
                                OID_GEN_VENDOR_DESCRIPTION,
                                OID_GEN_CURRENT_PACKET_FILTER,
                                OID_GEN_MAXIMUM_TOTAL_SIZE,
                                OID_GEN_MEDIA_CONNECT_STATUS,
                                OID_GEN_XMIT_OK,
                                OID_GEN_RCV_OK,
                                OID_GEN_XMIT_ERROR,
                                OID_GEN_RCV_ERROR,
                                OID_GEN_RCV_NO_BUFFER,
                                OID_802_3_PERMANENT_ADDRESS,
                                OID_802_3_CURRENT_ADDRESS,
                                OID_802_3_MULTICAST_LIST,
                                OID_802_3_MAXIMUM_LIST_SIZE,
                                OID_802_3_RCV_ERROR_ALIGNMENT,
                                OID_802_3_XMIT_ONE_COLLISION,
                                OID_802_3_XMIT_MORE_COLLISIONS,
                            };

uint8_t RNDISMessageBuffer[sizeof(AdapterSupportedOIDList) + sizeof(RNDIS_Query_Complete_t)];
RNDIS_Message_Header_t* MessageHeader = (RNDIS_Message_Header_t*)&RNDISMessageBuffer;
bool ResponseReady = false;



static bool ProcessNDISQuery(const uint32_t OId, void* QueryData, uint16_t QuerySize,
                             void* ResponseData, uint16_t* ResponseSize)
{
    switch (OId)
    {
        case OID_GEN_SUPPORTED_LIST:
            *ResponseSize = sizeof(AdapterSupportedOIDList);
            memcpy_P(ResponseData, AdapterSupportedOIDList, sizeof(AdapterSupportedOIDList));
            return true;
        case OID_GEN_PHYSICAL_MEDIUM:
            *ResponseSize = sizeof(uint32_t);
            *((uint32_t*)ResponseData) = 0;
            return true;
        case OID_GEN_HARDWARE_STATUS:
            *ResponseSize = sizeof(uint32_t);
            *((uint32_t*)ResponseData) = NDIS_HardwareStatus_Ready;
            return true;
        case OID_GEN_MEDIA_SUPPORTED:
        case OID_GEN_MEDIA_IN_USE:
            *ResponseSize = sizeof(uint32_t);
            *((uint32_t*)ResponseData) = REMOTE_NDIS_MEDIUM_802_3;
            return true;
        case OID_GEN_VENDOR_ID:
            *ResponseSize = sizeof(uint32_t);
            *((uint32_t*)ResponseData) = 0x00FFFFFF;
            return true;
        case OID_GEN_MAXIMUM_FRAME_SIZE:
        case OID_GEN_TRANSMIT_BLOCK_SIZE:
        case OID_GEN_RECEIVE_BLOCK_SIZE:
            *ResponseSize = sizeof(uint32_t);
            *((uint32_t*)ResponseData) = ETHERNET_FRAME_SIZE_MAX;
            return true;
        case OID_GEN_VENDOR_DESCRIPTION:
            *ResponseSize = sizeof(AdapterVendorDescription);
            memcpy_P(ResponseData, AdapterVendorDescription, sizeof(AdapterVendorDescription));
            return true;
        case OID_GEN_MEDIA_CONNECT_STATUS:
            *ResponseSize = sizeof(uint32_t);
            *((uint32_t*)ResponseData) = REMOTE_NDIS_MEDIA_STATE_CONNECTED;
            return true;
        case OID_GEN_LINK_SPEED:
            *ResponseSize = sizeof(uint32_t);
            *((uint32_t*)ResponseData) = 100000;
            return true;
        case OID_802_3_PERMANENT_ADDRESS:
        case OID_802_3_CURRENT_ADDRESS:
            *ResponseSize = sizeof(MAC_Address_t);
            memcpy_P(ResponseData, &AdapterMACAddress, sizeof(MAC_Address_t));
            return true;
        case OID_802_3_MAXIMUM_LIST_SIZE:
            *ResponseSize = sizeof(uint32_t);
            *((uint32_t*)ResponseData) = 1;
            return true;
        case OID_GEN_CURRENT_PACKET_FILTER:
            *ResponseSize = sizeof(uint32_t);
            *((uint32_t*)ResponseData) = CurrPacketFilter;
            return true;
        case OID_GEN_XMIT_OK:
        case OID_GEN_RCV_OK:
        case OID_GEN_XMIT_ERROR:
        case OID_GEN_RCV_ERROR:
        case OID_GEN_RCV_NO_BUFFER:
        case OID_802_3_RCV_ERROR_ALIGNMENT:
        case OID_802_3_XMIT_ONE_COLLISION:
        case OID_802_3_XMIT_MORE_COLLISIONS:
            *ResponseSize = sizeof(uint32_t);
            *((uint32_t*)ResponseData) = 0;
            return true;
        case OID_GEN_MAXIMUM_TOTAL_SIZE:
            *ResponseSize = sizeof(uint32_t);
            *((uint32_t*)ResponseData) = (sizeof(RNDISMessageBuffer) + ETHERNET_FRAME_SIZE_MAX);
            return true;
        default:
            return false;
    }
}

static bool ProcessNDISSet(uint32_t OId, void* SetData, uint16_t SetSize)
{
    switch (OId)
    {
        case OID_GEN_CURRENT_PACKET_FILTER:
            CurrPacketFilter = *((uint32_t*)SetData);
            CurrRNDISState = ((CurrPacketFilter) ? RNDIS_Data_Initialized : RNDIS_Initialized);
            return true;
        case OID_802_3_MULTICAST_LIST:
            return true;
        default:
            return false;
    }
}

void ProcessRNDISControlMessage(void)
{
    switch (MessageHeader->MessageType)
    {
        case REMOTE_NDIS_INITIALIZE_MSG:
        {
            ResponseReady = true;

            RNDIS_Initialize_Message_t *
            INITIALIZE_Message  = (RNDIS_Initialize_Message_t*)&RNDISMessageBuffer;

            RNDIS_Initialize_Complete_t *
            INITIALIZE_Response = (RNDIS_Initialize_Complete_t*)&RNDISMessageBuffer;

            INITIALIZE_Response->MessageType           = REMOTE_NDIS_INITIALIZE_CMPLT;
            INITIALIZE_Response->MessageLength         = sizeof(RNDIS_Initialize_Complete_t);
            INITIALIZE_Response->RequestId             = INITIALIZE_Message->RequestId;
            INITIALIZE_Response->Status                = REMOTE_NDIS_STATUS_SUCCESS;

            INITIALIZE_Response->MajorVersion          = REMOTE_NDIS_VERSION_MAJOR;
            INITIALIZE_Response->MinorVersion          = REMOTE_NDIS_VERSION_MINOR;
            INITIALIZE_Response->DeviceFlags           = REMOTE_NDIS_DF_CONNECTIONLESS;
            INITIALIZE_Response->Medium                = REMOTE_NDIS_MEDIUM_802_3;
            INITIALIZE_Response->MaxPacketsPerTransfer = 1;

            INITIALIZE_Response->MaxTransferSize = (sizeof(RNDIS_Packet_Message_t) +
                ETHERNET_FRAME_SIZE_MAX);

            INITIALIZE_Response->PacketAlignmentFactor = 0;
            INITIALIZE_Response->AFListOffset          = 0;
            INITIALIZE_Response->AFListSize            = 0;

            CurrRNDISState = RNDIS_Initialized;
        }
            break;
        case REMOTE_NDIS_HALT_MSG:
            ResponseReady = false;
            MessageHeader->MessageLength = 0;
            CurrRNDISState = RNDIS_Uninitialized;
            break;
        case REMOTE_NDIS_QUERY_MSG:
        {
            ResponseReady = true;
            RNDIS_Query_Message_t*  QUERY_Message  = (RNDIS_Query_Message_t*)&RNDISMessageBuffer;
            RNDIS_Query_Complete_t* QUERY_Response = (RNDIS_Query_Complete_t*)&RNDISMessageBuffer;
            uint32_t                Query_Oid      = QUERY_Message->Oid;

            void *QueryData = &RNDISMessageBuffer[sizeof(RNDIS_Message_Header_t) +
                                                    QUERY_Message->InformationBufferOffset];
            void*     ResponseData = &RNDISMessageBuffer[sizeof(RNDIS_Query_Complete_t)];
            uint16_t  ResponseSize;

            QUERY_Response->MessageType         = REMOTE_NDIS_QUERY_CMPLT;
            QUERY_Response->MessageLength       = sizeof(RNDIS_Query_Complete_t);

            if (ProcessNDISQuery(Query_Oid, QueryData, QUERY_Message->InformationBufferLength,
                                 ResponseData, &ResponseSize))
            {
                QUERY_Response->Status                  = REMOTE_NDIS_STATUS_SUCCESS;
                QUERY_Response->MessageLength          += ResponseSize;

                QUERY_Response->InformationBufferLength = ResponseSize;
                QUERY_Response->InformationBufferOffset = (sizeof(RNDIS_Query_Complete_t) -
                    sizeof(RNDIS_Message_Header_t));
            }
            else
            {
                QUERY_Response->Status = REMOTE_NDIS_STATUS_NOT_SUPPORTED;
                QUERY_Response->InformationBufferLength = 0;
                QUERY_Response->InformationBufferOffset = 0;
            }
        }
            break;
        case REMOTE_NDIS_SET_MSG:
        {
            ResponseReady = true;

            RNDIS_Set_Message_t*  SET_Message  = (RNDIS_Set_Message_t*)&RNDISMessageBuffer;
            RNDIS_Set_Complete_t* SET_Response = (RNDIS_Set_Complete_t*)&RNDISMessageBuffer;
            uint32_t              SET_Oid      = SET_Message->Oid;

            SET_Response->MessageType       = REMOTE_NDIS_SET_CMPLT;
            SET_Response->MessageLength     = sizeof(RNDIS_Set_Complete_t);
            SET_Response->RequestId         = SET_Message->RequestId;

            void* SetData = &RNDISMessageBuffer[sizeof(RNDIS_Message_Header_t) +
                                        SET_Message->InformationBufferOffset];

            if (ProcessNDISSet(SET_Oid, SetData, SET_Message->InformationBufferLength))
                SET_Response->Status = REMOTE_NDIS_STATUS_SUCCESS;
            else
                SET_Response->Status = REMOTE_NDIS_STATUS_NOT_SUPPORTED;
        }
            break;
        case REMOTE_NDIS_RESET_MSG:
        {
            ResponseReady = true;
            RNDIS_Reset_Complete_t* RESET_Response = (RNDIS_Reset_Complete_t*)&RNDISMessageBuffer;
            RESET_Response->MessageType         = REMOTE_NDIS_RESET_CMPLT;
            RESET_Response->MessageLength       = sizeof(RNDIS_Reset_Complete_t);
            RESET_Response->Status              = REMOTE_NDIS_STATUS_SUCCESS;
            RESET_Response->AddressingReset     = 0;
        }
            break;
        case REMOTE_NDIS_KEEPALIVE_MSG:
        {
            ResponseReady = true;

            RNDIS_KeepAlive_Message_t *
            KEEPALIVE_Message  = (RNDIS_KeepAlive_Message_t*)&RNDISMessageBuffer;

            RNDIS_KeepAlive_Complete_t *
            KEEPALIVE_Response = (RNDIS_KeepAlive_Complete_t*)&RNDISMessageBuffer;

            KEEPALIVE_Response->MessageType     = REMOTE_NDIS_KEEPALIVE_CMPLT;
            KEEPALIVE_Response->MessageLength   = sizeof(RNDIS_KeepAlive_Complete_t);
            KEEPALIVE_Response->RequestId       = KEEPALIVE_Message->RequestId;
            KEEPALIVE_Response->Status          = REMOTE_NDIS_STATUS_SUCCESS;
        }
            break;
    }
}



void EVENT_USB_Device_ControlRequest(void)
{
	switch (USB_ControlRequest.bRequest)
	{
    case RNDIS_REQ_SendEncapsulatedCommand:
        if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
        {
            Endpoint_ClearSETUP();
            Endpoint_Read_Control_Stream_LE(RNDISMessageBuffer, USB_ControlRequest.wLength);
            Endpoint_ClearIN();
            ProcessRNDISControlMessage();
        }

        break;
    case RNDIS_REQ_GetEncapsulatedResponse:
        if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				if (!(MessageHeader->MessageLength))
				{
					RNDISMessageBuffer[0] = 0;
					MessageHeader->MessageLength = 1;
				}

				Endpoint_ClearSETUP();
				Endpoint_Write_Control_Stream_LE(RNDISMessageBuffer, MessageHeader->MessageLength);
				Endpoint_ClearOUT();
				MessageHeader->MessageLength = 0;
			}

			break;
	}
}

void USB_Device_ProcessControlRequest(void)
{
	uint8_t* RequestHeader = (uint8_t*)&USB_ControlRequest;

    for (uint8_t RequestHeaderByte = 0; RequestHeaderByte < sizeof(USB_Request_Header_t); RequestHeaderByte++)
	  *(RequestHeader++) = Endpoint_Read_8();

	EVENT_USB_Device_ControlRequest();

	if (Endpoint_IsSETUPReceived())
	{
		uint8_t bmRequestType = USB_ControlRequest.bmRequestType;

		switch (USB_ControlRequest.bRequest)
		{
			case REQ_GetStatus:
				if ((bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_DEVICE)) ||
					(bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_ENDPOINT)))
				{
					USB_Device_GetStatus();
				}

				break;
			case REQ_ClearFeature:
			case REQ_SetFeature:
				if ((bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_DEVICE)) ||
					(bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_ENDPOINT)))
				{
					USB_Device_ClearSetFeature();
				}

				break;
			case REQ_SetAddress:
				if (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_DEVICE))
				  USB_Device_SetAddress();

				break;
			case REQ_GetDescriptor:
				if ((bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_DEVICE)) ||
					(bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_INTERFACE)))
				{
					USB_Device_GetDescriptor();
				}

				break;
			case REQ_GetConfiguration:
				if (bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_DEVICE))
				  USB_Device_GetConfiguration();

				break;
			case REQ_SetConfiguration:
				if (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_DEVICE))
				  USB_Device_SetConfiguration();

				break;

			default:
				break;
		}
	}

	if (Endpoint_IsSETUPReceived())
	{
		Endpoint_ClearSETUP();
		Endpoint_StallTransaction();
	}
}
#endif


typedef struct
{
    uint8_t Octets[6]; /**< Individual bytes of a MAC address */
} RNDIS_MAC_Address_t;

typedef struct
{
    uint8_t Octets[4]; /**< Individual bytes of an IP address */
} IP_Address_t;

void DecodeARPHeader(void* InDataStart);
void DecodeIPHeader(void* InDataStart);
void DecodeICMPHeader(void* InDataStart);
void DecodeTCPHeader(void* InDataStart);
void DecodeUDPHeader(void* InDataStart);
void DecodeDHCPHeader(void* InDataStart);

#define BROADCAST_IP_ADDRESS             {0xFF, 0xFF, 0xFF, 0xFF}
#define DEFAULT_TTL                      128
#define IP_COMPARE(IP1, IP2)             (memcmp(IP1, IP2, sizeof(IP_Address_t)) == 0)

typedef struct
{
    unsigned     HeaderLength   : 4;
    unsigned     Version        : 4;
    uint8_t      TypeOfService;
    uint16_t     TotalLength;
    uint16_t     Identification; /**< Identifue for identifying fragmented packets */
    unsigned     FragmentOffset : 13; /**< Offset of this IP fragment */
    unsigned     Flags          : 3; /**< Fradicate if a packet is fragmented */

    uint8_t      TTL; /**< Maximum allowable hops to reach the packet destination */
    uint8_t      Protocol; /**< Encapsulated protocol type */
    uint16_t     HeaderChecksum; /**< Ethernet checksum of the IP header */

    IP_Address_t SourceAddress; /**< Source protocol IP address of the packet */
    IP_Address_t DestinationAddress; /**< Destination protocol IP address of the packet */
} IP_Header_t;

int16_t IP_ProcessIPPacket(void* InDataStart, void* OutDataStart);

#define ARP_OPERATION_REQUEST            1
#define ARP_OPERATION_REPLY              2

typedef struct
{
    uint16_t      HardwareType; /**< Hardware type constant, indicating the hardware used */
    uint16_t      ProtocolType; /**< Protocol being resolved, usually ETHERTYPE_IPV4 */

    uint8_t       HLEN; /**< Length in bytes of the source/destination hardware addresses */
    uint8_t       PLEN; /**< Length in bytes of the source/destination protocol addresses */
    uint16_t      Operation;

    MAC_Address_t SHA; /**< Sender's hardware address */
    IP_Address_t  SPA; /**< Sender's protocol address */
    MAC_Address_t THA; /**< Target's hardware address */
    IP_Address_t  TPA; /**< Target's protocol address */
} ARP_Header_t;

int16_t ARP_ProcessARPPacket(void* InDataStart, void* OutDataStart);


#define ICMP_TYPE_ECHOREPLY              0
#define ICMP_TYPE_DESTINATIONUNREACHABLE 3
#define ICMP_TYPE_SOURCEQUENCH           4
#define ICMP_TYPE_REDIRECTMESSAGE        5
#define ICMP_TYPE_ECHOREQUEST            8
#define ICMP_TYPE_TIMEEXCEEDED           11

typedef struct
{
    uint8_t Type; /**< ICMP message type, an \c ICMP_TYPE_* constant */
    uint8_t Code; /**< ICMP message code, indicating the message value */
    uint16_t Checksum; /**< Ethernet checksum of the ICMP message */
    uint16_t Id; /**< Id of the ICMP message */
    uint16_t Sequence; /**P message, to link together message responses */
} ICMP_Header_t;

int16_t ICMP_ProcessICMPPacket(void* InDataStart,
                               void* OutDataStart);



#define UDP_PORT_DHCP_REQUEST 67
#define UDP_PORT_DHCP_REPLY   68
#define UDP_PORT_DNS          53

typedef struct
{
    uint16_t SourcePort; /**< Packet source port */
    uint16_t DestinationPort; /**< Packet destination port */
    uint16_t Length; /**< Total packet length, in bytes */
    uint16_t Checksum; /**< Optional UDP packet checksum */
} UDP_Header_t;

int16_t UDP_ProcessUDPPacket(void* IPHeaderInStart,
                             void* UDPHeaderInStart,
                             void* UDPHeaderOutStart);

#define DHCP_OP_BOOTREQUEST       0x01
#define DHCP_OP_BOOTREPLY         0x02
#define DHCP_HTYPE_ETHERNET       0x01
#define DHCP_MAGIC_COOKIE         0x63825363
#define DHCP_OPTION_SUBNETMASK    1
#define DHCP_OPTION_LEASETIME     51
#define DHCP_OPTION_MESSAGETYPE   53
#define DHCP_OPTION_DHCPSERVER    54
#define DHCP_OPTION_PAD           0
#define DHCP_OPTION_END           255
#define DHCP_MESSAGETYPE_DISCOVER 1
#define DHCP_MESSAGETYPE_OFFER    2
#define DHCP_MESSAGETYPE_REQUEST  3
#define DHCP_MESSAGETYPE_DECLINE  4
#define DHCP_MESSAGETYPE_ACK      5
#define DHCP_MESSAGETYPE_NACK     6
#define DHCP_MESSAGETYPE_RELEASE  7

typedef struct
{
    uint8_t  Operation; /**< DHCP operation_OP_BOOTREQUEST or DHCP_OP_BOOTREPLY */
    uint8_t  HardwareType; /**< Hardware carrier type constant */
    uint8_t  HardwareAddressLength;  /**< Lf a hardware (MAC) address on the network */
    uint8_t  Hops; /**< Number of hops required to reach the server, unused */
    uint32_t TransactionID; /**< Unique ID tching between sent and received packets */
    uint16_t ElapsedSeconds; /**< Elapsed seconds since the request was made */
    uint16_t Flags; /**< BOOTP packet flags */
    IP_Address_t ClientIP; /**< Client IP address, if already leased an IP */
    IP_Address_t YourIP; /**< Client IP address */
    IP_Address_t NextServerIP; /**< Legacy BOOTP protocol field, unused for DHCP */
    IP_Address_t RelayAgentIP; /**< Legacy BOOTP protocol field, unused for DHCP */
    uint8_t ClientHardwareAddress[16];
    uint8_t ServerHostnameString[64]; /**< Legacy BOOTP protocol field, unused for DHCP */
    uint8_t BootFileName[128]; /**< Legacy BOOTP protocol field, unused for DHCP */
    uint32_t Cookie; /**< Magic BOOTP protocol cookie to indicate a valid packet */
} DHCP_Header_t;

int16_t DHCP_ProcessDHCPPacket(void* IPHeaderInStart,
                                       void* DHCPHeaderInStart,
                                       void* DHCPHeaderOutStart);

#define MAX_OPEN_TCP_PORTS              1
#define MAX_TCP_CONNECTIONS             3
#define TCP_WINDOW_SIZE                 512
#define TCP_PORT_HTTP                   SwapEndian_16(80)
#define TCP_PACKETDIR_IN                false
#define TCP_PACKETDIR_OUT               true
#define TCP_FLAG_CWR                    (1 << 7)
#define TCP_FLAG_ECE                    (1 << 6)
#define TCP_FLAG_URG                    (1 << 5)
#define TCP_FLAG_ACK                    (1 << 4)
#define TCP_FLAG_PSH                    (1 << 3)
#define TCP_FLAG_RST                    (1 << 2)
#define TCP_FLAG_SYN                    (1 << 1)
#define TCP_FLAG_FIN                    (1 << 0)

#define TCP_APP_HAS_RECEIVED_PACKET(Buffer)  (Buffer->Ready && (Buffer->Direction == TCP_PACKETDIR_IN))


        #define TCP_APP_HAVE_CAPTURED_BUFFER(Buffer) (!(Buffer->Ready) && Buffer->InUse && \
                                                      (Buffer->Direction == TCP_PACKETDIR_OUT))

        #define TCP_APP_CAN_CAPTURE_BUFFER(Buffer)   Buffer->InUse

        #define TCP_APP_CAPTURE_BUFFER(Buffer)       do { Buffer->Direction = TCP_PACKETDIR_OUT; Buffer->InUse = true; } while (0)

        #define TCP_APP_RELEASE_BUFFER(Buffer)       do { Buffer->InUse = false; } while (0)

        #define TCP_APP_SEND_BUFFER(Buffer, Len)     do { Buffer->Direction = TCP_PACKETDIR_OUT; Buffer->Length = Len; Buffer->Ready = true; } while (0)

        #define TCP_APP_CLEAR_BUFFER(Buffer)         do { Buffer->Ready = false; Buffer->Length = 0; } while (0)

        #define TCP_APP_CLOSECONNECTION(Connection)  do { Connection->State = TCP_Connection_Closing;  } while (0)

        enum TCP_PortStates_t
        {   
            TCP_Port_Closed            = 0,
            TCP_Port_Open              = 1,
        };

        enum TCP_ConnectionStates_t
        {
            TCP_Connection_Listen      = 0, /**< Listening for a connection from a host */
            TCP_Connection_SYNSent     = 1, /**< Unused */
            TCP_Connection_SYNReceived = 2, /**< SYN received, waiting for ACK */
            TCP_Connection_Established = 3, /**< Connection established in both directions */
            TCP_Connection_FINWait1    = 4, /**< Closing, waiting for ACK */
            TCP_Connection_FINWait2    = 5, /**< Closing, waiting for FIN ACK */
            TCP_Connection_CloseWait   = 6, /**< Closing, waiting for ACK */
            TCP_Connection_Closing     = 7, /**< Unused */
            TCP_Connection_LastACK     = 8, /**< Unused */
            TCP_Connection_TimeWait    = 9, /**< Unused */
            TCP_Connection_Closed      = 10, /**< Connection closed in both directions */
        };

        typedef struct
        {
            uint16_t               Length; /**< Length of data in the TCP application buffer */
            uint8_t                Data[TCP_WINDOW_SIZE]; /**< TCP application data buffer */
            bool                   Direction;
            bool                   Ready;
            bool                   InUse;
        } TCP_ConnectionBuffer_t;

        typedef struct
        {
            uint32_t               SequenceNumberIn;
            uint32_t               SequenceNumberOut;
            TCP_ConnectionBuffer_t Buffer; /**< Connection application data buffer */
        } TCP_ConnectionInfo_t;

        typedef struct
        {
            uint16_t               Port; /**< Connection port number on the device */
            uint16_t               RemotePort; /**< Connection port number on the host */
            IP_Address_t           RemoteAddress; /**<n protocol IP address of the host */
            TCP_ConnectionInfo_t   Info; /**< Cormation, including application buffer */
            uint8_t                State; /**< from the \ref TCP_ConnectionStates_t enum */
        } TCP_ConnectionState_t;

        /** Type define for a TCP port state. */
        typedef struct
        {
            uint16_t               Port; /**< TCP port number on the device */
            uint8_t                State; /**<  value from the \ref TCP_PortStates_t enum */
            void                   (*ApplicationHandler) (TCP_ConnectionState_t* ConnectionState,
                                                          TCP_ConnectionBuffer_t* Buffer);
        } TCP_PortState_t;

        typedef struct
        {
            uint16_t               SourcePort; /**< Source port of the TCP packet */
            uint16_t               DestinationPort; /**< Destination port of the TCP packet */
            uint32_t               SequenceNumber; /**< Data sequence number of the packet */
            uint32_t               AcknowledgmentNumber; /**< Dent number of the packet */
            unsigned               Reserved   : 4; /**< Reserved, must be all 0 */
            unsigned               DataOffset : 4; /**< Offset e header, in 4 byte chunks */
            uint8_t                Flags; /**< TCP packet flags */
            uint16_t               WindowSize; /**< Current dataining in reception buffer) */
            uint16_t               Checksum; /**< TCP checksum */
            uint16_t               UrgentPointer; /**< Urgent data pointer */
        } TCP_Header_t;

        void TCP_Init(void);
        void TCP_Task(void);
        bool TCP_SetPortState(const uint16_t Port,
                                       const uint8_t State,
                           void (*Handler)(TCP_ConnectionState_t*, TCP_ConnectionBuffer_t*));
        uint8_t               TCP_GetPortState(const uint16_t Port);
        bool                  TCP_SetConnectionState(const uint16_t Port,
                                                     const IP_Address_t* RemoteAddress,
                                                     const uint16_t RemotePort,
                                                     const uint8_t State);
        uint8_t               TCP_GetConnectionState(const uint16_t Port,
                                                     const IP_Address_t* RemoteAddress,
                                                     const uint16_t RemotePort);
        TCP_ConnectionInfo_t* TCP_GetConnectionInfo(const uint16_t Port,
                                                    const IP_Address_t* RemoteAddress,
                                                    const uint16_t RemotePort);
        int16_t               TCP_ProcessTCPPacket(void* IPHeaderInStart,
                                                   void* TCPHeaderInStart,
                                                   void* TCPHeaderOutStart);

        #if defined(INCLUDE_FROM_TCP_C)
            static uint16_t TCP_Checksum16(void* TCPHeaderOutStart,
                                           const IP_Address_t* SourceAddress,
                                           const IP_Address_t* DestinationAddress,
                                           uint16_t TCPOutSize);
        #endif


#define  HTTP_REPLY_BLOCK_SIZE     128

//void Webserver_Init(void);
void Webserver_ApplicationCallback(TCP_ConnectionState_t* const ConnectionState,
                                           TCP_ConnectionBuffer_t* const Buffer);


#define BROADCAST_MAC_ADDRESS            {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
#define MAC_COMPARE(MAC1, MAC2)          (memcmp(MAC1, MAC2, sizeof(MAC_Address_t)) == 0)

        /** Maximum size of an incoming or outgoing Ethernet frame in bytes. */
        //#define ETHERNET_FRAME_SIZE_MAX          100

        /** Minimum size of an Ethernet packet in bytes, to conform to the Ethernet V2 packet standard. */
        #define ETHERNET_VER2_MINSIZE            0x0600

        /** Return value for all sub protocol handling routines, indicating that no response packet has been generated. */
        #define NO_RESPONSE                      0

        /** Return value for all sub protocol handling routines, indicating that the packet has not yet been handled. */
        #define NO_PROCESS                       -1

        typedef struct
        {
            uint8_t  FrameData[ETHERNET_FRAME_SIZE_MAX]; /**< Ethernet frame contents. */
            uint16_t FrameLength; /**< Length net frame stored in the buffer. */
        } Ethernet_Frame_Info_t;

        /** Type define for an Ethernet frame header. */
        typedef struct
        {
            MAC_Address_t Destination; /**< Physical MAC address of the packet recipient */
            MAC_Address_t Source; /**< Physics MAC address of the packet source */
            uint16_t      EtherType; /**< Ethernet packet sub-protocol type, for Ethernet V2 packets */
        } Ethernet_Frame_Header_t;

    /* External Variables: */
        extern Ethernet_Frame_Info_t FrameIN;
        extern Ethernet_Frame_Info_t FrameOUT;

        extern const MAC_Address_t ServerMACAddress;
        extern const IP_Address_t  ServerIPAddress;
        extern const MAC_Address_t BroadcastMACAddress;
        extern const IP_Address_t  BroadcastIPAddress;
        extern const IP_Address_t  ClientIPAddress;

void     Ethernet_ProcessPacket(void);
uint16_t Ethernet_Checksum16(void* Data,
                             uint16_t Bytes);

//void ProcessRNDISControlMessage(void);
#if 0
            static bool ProcessNDISQuery(const uint32_t OId,
                                         void* QueryData,
                                         uint16_t QuerySize,
                                         void* ResponseData,
                                         uint16_t* ResponseSize);
#endif
#if 0
            static bool ProcessNDISSet(const uint32_t OId,
                                       void* SetData,
                                       uint16_t SetSize);
#endif
void RNDIS_Task(void);
void Ethernet_Task(void);

void EVENT_USB_Device_Connect(void);
void EVENT_USB_Device_Disconnect(void);
//void EVENT_USB_Device_ConfigurationChanged(void);
//void EVENT_USB_Device_ControlRequest(void);


typedef struct
{
    USB_Descriptor_Configuration_Header_t Config;
    USB_Descriptor_Interface_t            CDC_CCI_Interface;
    USB_CDC_Descriptor_FunctionalHeader_t CDC_Functional_Header;
    USB_CDC_Descriptor_FunctionalACM_t    CDC_Functional_ACM;
    USB_CDC_Descriptor_FunctionalUnion_t  CDC_Functional_Union;
    USB_Descriptor_Endpoint_t             CDC_NotificationEndpoint;
    USB_Descriptor_Interface_t            CDC_DCI_Interface;
    USB_Descriptor_Endpoint_t             RNDIS_DataOutEndpoint;
    USB_Descriptor_Endpoint_t             RNDIS_DataInEndpoint;
} USB_Descriptor_Configuration_t;

        enum InterfaceDescriptors_t
        {
            INTERFACE_ID_CDC_CCI = 0, /**< CDC CCI interface descriptor ID */
            INTERFACE_ID_CDC_DCI = 1, /**< CDC DCI interface descriptor ID */
        };

        enum StringDescriptors_t
        {
            STRING_ID_Language     = 0, /**< Supported ing descriptor ID (must be zero) */
            STRING_ID_Manufacturer = 1, /**< Manufacturer string ID */
            STRING_ID_Product      = 2, /**< Product string ID */
        };

        uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                            const uint16_t wIndex,
                                            const void** const DescriptorAddress)
                                            ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);

int16_t IP_ProcessIPPacket(void* InDataStart,
                           void* OutDataStart)
{
    DecodeIPHeader(InDataStart);

    IP_Header_t* IPHeaderIN  = (IP_Header_t*)InDataStart;
    IP_Header_t* IPHeaderOUT = (IP_Header_t*)OutDataStart;
    uint16_t HeaderLengthBytes = (IPHeaderIN->HeaderLength * sizeof(uint32_t));
    int16_t  RetSize = NO_RESPONSE;

    if (!(IP_COMPARE(&IPHeaderIN->DestinationAddress, &ServerIPAddress)) &&
        !(IP_COMPARE(&IPHeaderIN->DestinationAddress, &BroadcastIPAddress)))
    {
        return NO_RESPONSE;
    }

    switch (IPHeaderIN->Protocol)
    {
        case PROTOCOL_ICMP:
            RetSize = ICMP_ProcessICMPPacket(&((uint8_t*)InDataStart)[HeaderLengthBytes],
                                             &((uint8_t*)OutDataStart)[sizeof(IP_Header_t)]);
            break;
        case PROTOCOL_TCP:
            RetSize = TCP_ProcessTCPPacket(InDataStart,
                                           &((uint8_t*)InDataStart)[HeaderLengthBytes],
                                           &((uint8_t*)OutDataStart)[sizeof(IP_Header_t)]);
            break;
        case PROTOCOL_UDP:
            RetSize = UDP_ProcessUDPPacket(InDataStart,
                                           &((uint8_t*)InDataStart)[HeaderLengthBytes],
                                           &((uint8_t*)OutDataStart)[sizeof(IP_Header_t)]);
            break;
    }

    if (RetSize > 0)
    {
        IPHeaderOUT->TotalLength        = SwapEndian_16(sizeof(IP_Header_t) + RetSize);
        IPHeaderOUT->TypeOfService      = 0;
        IPHeaderOUT->HeaderLength       = (sizeof(IP_Header_t) / sizeof(uint32_t));
        IPHeaderOUT->Version            = 4;
        IPHeaderOUT->Flags              = 0;
        IPHeaderOUT->FragmentOffset     = 0;
        IPHeaderOUT->Identification     = 0;
        IPHeaderOUT->HeaderChecksum     = 0;
        IPHeaderOUT->Protocol           = IPHeaderIN->Protocol;
        IPHeaderOUT->TTL                = DEFAULT_TTL;
        IPHeaderOUT->SourceAddress      = IPHeaderIN->DestinationAddress;
        IPHeaderOUT->DestinationAddress = IPHeaderIN->SourceAddress;

        IPHeaderOUT->HeaderChecksum     = Ethernet_Checksum16(IPHeaderOUT, sizeof(IP_Header_t));

        /* Return the size of the response so far */
        return (sizeof(IP_Header_t) + RetSize);
    }

    return RetSize;
}

void DecodeARPHeader(void* InDataStart)
{
    #if !defined(NO_DECODE_ARP)
    ARP_Header_t* ARPHeader = (ARP_Header_t*)InDataStart;

    printf_P(PSTR("   \\\r\n    ARP\r\n"));

    if (!(IP_COMPARE(&ARPHeader->TPA, &ServerIPAddress)) &&
        !(MAC_COMPARE(&ARPHeader->THA, &ServerMACAddress)))
    {
        printf_P(PSTR("    + NOT ADDRESSED TO DEVICE\r\n"));
        return;
    }

    printf_P(PSTR("    + Protocol: %x\r\n"), SwapEndian_16(ARPHeader->ProtocolType));
    printf_P(PSTR("    + Operation: %u\r\n"), SwapEndian_16(ARPHeader->Operation));

    if (SwapEndian_16(ARPHeader->ProtocolType) == ETHERTYPE_IPV4)
    {
        printf_P(PSTR("    + SHA MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n"), ARPHeader->SHA.Octets[0],
                                                                   ARPHeader->SHA.Octets[1],
                                                                     ARPHeader->SHA.Octets[2],
                                                                     ARPHeader->SHA.Octets[3],
                                                                     ARPHeader->SHA.Octets[4],
                                                                     ARPHeader->SHA.Octets[5]);

        printf_P(PSTR("    + SPA IP: %u.%u.%u.%u\r\n"), ARPHeader->SPA.Octets[0],
                                                        ARPHeader->SPA.Octets[1],
                                                        ARPHeader->SPA.Octets[2],
                                                        ARPHeader->SPA.Octets[3]);

        printf_P(PSTR("    + THA MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n"), ARPHeader->THA.Octets[0],
                                                                           ARPHeader->THA.Octets[1],
                                                                           ARPHeader->THA.Octets[2],
                                                                           ARPHeader->THA.Octets[3],
                                                                           ARPHeader->THA.Octets[4],
                                                                           ARPHeader->THA.Octets[5]);

        printf_P(PSTR("    + TPA IP: %u.%u.%u.%u\r\n"), ARPHeader->TPA.Octets[0],
                                                        ARPHeader->TPA.Octets[1],
                                                        ARPHeader->TPA.Octets[2],
                                                        ARPHeader->TPA.Octets[3]);
    }
    #endif
}

void DecodeIPHeader(void* InDataStart)
{
    #if !defined(NO_DECODE_IP)
    IP_Header_t* IPHeader  = (IP_Header_t*)InDataStart;

    uint16_t HeaderLengthBytes = (IPHeader->HeaderLength * sizeof(uint32_t));

    printf_P(PSTR("   \\\r\n    IP\r\n"));

    if (!(IP_COMPARE(&IPHeader->DestinationAddress, &ServerIPAddress)))
    {
        printf_P(PSTR("    + NOT ADDRESSED TO DEVICE\r\n"));
        return;
    }

    printf_P(PSTR("    + Header Length: %u Bytes\r\n"), HeaderLengthBytes);
    printf_P(PSTR("    + Packet Version: %u\r\n"), IPHeader->Version);
    printf_P(PSTR("    + Total Length: %u\r\n"), SwapEndian_16(IPHeader->TotalLength));

    printf_P(PSTR("    + Protocol: %u\r\n"), IPHeader->Protocol);
    printf_P(PSTR("    + TTL: %u\r\n"), IPHeader->TTL);

    printf_P(PSTR("    + IP Src: %u.%u.%u.%u\r\n"), IPHeader->SourceAddress.Octets[0],
                                                    IPHeader->SourceAddress.Octets[1],
                                                    IPHeader->SourceAddress.Octets[2],
                                                    IPHeader->SourceAddress.Octets[3]);

    printf_P(PSTR("    + IP Dst: %u.%u.%u.%u\r\n"), IPHeader->DestinationAddress.Octets[0],
                                                    IPHeader->DestinationAddress.Octets[1],
                                                    IPHeader->DestinationAddress.Octets[2],
                                                    IPHeader->DestinationAddress.Octets[3]);
    #endif
}

void DecodeICMPHeader(void* InDataStart)
{
    #if !defined(NO_DECODE_ICMP)
    ICMP_Header_t* ICMPHeader  = (ICMP_Header_t*)InDataStart;

    printf_P(PSTR("    \\\r\n     ICMP\r\n"));

    printf_P(PSTR("     + Type: %u\r\n"), ICMPHeader->Type);
    printf_P(PSTR("     + Code: %u\r\n"), ICMPHeader->Code);
    #endif
}

void DecodeTCPHeader(void* InDataStart)
{
    #if !defined(NO_DECODE_TCP)
    TCP_Header_t* TCPHeader  = (TCP_Header_t*)InDataStart;

    uint16_t               HeaderLengthBytes = (TCPHeader->DataOffset * sizeof(uint32_t));

    printf_P(PSTR("    \\\r\n     TCP\r\n"));

    printf_P(PSTR("     + Header Length: %u Bytes\r\n"), HeaderLengthBytes);

    printf_P(PSTR("     + Source Port: %u\r\n"), SwapEndian_16(TCPHeader->SourcePort));
    printf_P(PSTR("     + Destination Port: %u\r\n"), SwapEndian_16(TCPHeader->DestinationPort));

    printf_P(PSTR("     + Sequence Number: %lu\r\n"), SwapEndian_32(TCPHeader->SequenceNumber));
    printf_P(PSTR("     + Acknowledgment Number: %lu\r\n"), SwapEndian_32(TCPHeader->AcknowledgmentNumber));

    printf_P(PSTR("     + Flags: 0x%02X\r\n"), TCPHeader->Flags);

    if (TCP_GetPortState(TCPHeader->DestinationPort) == TCP_Port_Closed)
      printf_P(PSTR("     + NOT LISTENING ON DESTINATION PORT\r\n"));
    #endif
}

void DecodeUDPHeader(void* InDataStart)
{
    #if !defined(NO_DECODE_UDP)
    UDP_Header_t* UDPHeader = (UDP_Header_t*)InDataStart;

    printf_P(PSTR("    \\\r\n     UDP\r\n"));

    printf_P(PSTR("     + Source Port: %u\r\n"), SwapEndian_16(UDPHeader->SourcePort));
    printf_P(PSTR("     + Destination Port: %u\r\n"), SwapEndian_16(UDPHeader->DestinationPort));

    printf_P(PSTR("     + Data Length: %d\r\n"), SwapEndian_16(UDPHeader->Length));
    #endif
}

void DecodeDHCPHeader(void* InDataStart)
{
    #if !defined(NO_DECODE_DHCP)
    uint8_t* DHCPOptions = ((uint8_t*)InDataStart + sizeof(DHCP_Header_t));

    printf_P(PSTR("     \\\r\n      DHCP\r\n"));

    while (DHCPOptions[0] != DHCP_OPTION_END)
    {
        if (DHCPOptions[0] == DHCP_OPTION_MESSAGETYPE)
        {
            switch (DHCPOptions[2])
            {
                case DHCP_MESSAGETYPE_DISCOVER:
                    printf_P(PSTR("      + DISCOVER\r\n"));
                    break;
                case DHCP_MESSAGETYPE_REQUEST:
                    printf_P(PSTR("      + REQUEST\r\n"));
                    break;
                case DHCP_MESSAGETYPE_RELEASE:
                    printf_P(PSTR("      + RELEASE\r\n"));
                    break;
                case DHCP_MESSAGETYPE_DECLINE:
                    printf_P(PSTR("      + DECLINE\r\n"));
                    break;
            }
        }

        DHCPOptions += ((DHCPOptions[0] == DHCP_OPTION_PAD) ? 1 : (DHCPOptions[1] + 2));
    }
    #endif
}

TCP_PortState_t        PortStateTable[MAX_OPEN_TCP_PORTS];
TCP_ConnectionState_t  ConnectionStateTable[MAX_TCP_CONNECTIONS];

static uint16_t TCP_Checksum16(void* TCPHeaderOutStart,
                               const IP_Address_t* SourceAddress,
                               const IP_Address_t* DestinationAddress,
                               uint16_t TCPOutSize)
{
    uint32_t Checksum = 0;
    Checksum += ((uint16_t*)SourceAddress)[0];
    Checksum += ((uint16_t*)SourceAddress)[1];
    Checksum += ((uint16_t*)DestinationAddress)[0];
    Checksum += ((uint16_t*)DestinationAddress)[1];
    Checksum += SwapEndian_16(PROTOCOL_TCP);
    Checksum += SwapEndian_16(TCPOutSize);

    for (uint16_t CurrWord = 0; CurrWord < (TCPOutSize >> 1); CurrWord++)
      Checksum += ((uint16_t*)TCPHeaderOutStart)[CurrWord];

    if (TCPOutSize & 0x01)
      Checksum += (((uint16_t*)TCPHeaderOutStart)[TCPOutSize >> 1] & 0x00FF);

    while (Checksum & 0xFFFF0000)
      Checksum = ((Checksum & 0xFFFF) + (Checksum >> 16));

    return ~Checksum;
}

void TCP_Task(void)
{
    /* Run each application in sequence, to process incoming and generate outgoing packets */
    for (uint8_t CSTableEntry = 0; CSTableEntry < MAX_TCP_CONNECTIONS; CSTableEntry++)
    {
        /* Find the corresponding port entry in the port table */
        for (uint8_t PTableEntry = 0; PTableEntry < MAX_OPEN_TCP_PORTS; PTableEntry++)
        {
            /* Run the application handler for the port */
            if ((PortStateTable[PTableEntry].Port  == ConnectionStateTable[CSTableEntry].Port) &&
                (PortStateTable[PTableEntry].State == TCP_Port_Open))
            {
                PortStateTable[PTableEntry].ApplicationHandler(&ConnectionStateTable[CSTableEntry],
                                                               &ConnectionStateTable[CSTableEntry].Info.Buffer);
            }
        }
    }

    /* Bail out early if there is already a frame waiting to be sent in the Ethernet OUT buffer */
    if (FrameOUT.FrameLength)
      return;

    for (uint8_t CSTableEntry = 0; CSTableEntry < MAX_TCP_CONNECTIONS; CSTableEntry++)
    {
        /* For each completely received packet, pass it along to the listening application */
        if ((ConnectionStateTable[CSTableEntry].Info.Buffer.Direction == TCP_PACKETDIR_OUT) &&
            (ConnectionStateTable[CSTableEntry].Info.Buffer.Ready))
        {
            Ethernet_Frame_Header_t* FrameOUTHeader = (Ethernet_Frame_Header_t*)&FrameOUT.FrameData;
            IP_Header_t*             IPHeaderOUT    = (IP_Header_t*)&FrameOUT.FrameData[sizeof(Ethernet_Frame_Header_t)];
            TCP_Header_t*            TCPHeaderOUT   = (TCP_Header_t*)&FrameOUT.FrameData[sizeof(Ethernet_Frame_Header_t) +
                                                                                         sizeof(IP_Header_t)];
            void*                    TCPDataOUT     = &FrameOUT.FrameData[sizeof(Ethernet_Frame_Header_t) +
                                                                          sizeof(IP_Header_t) +
                                                                          sizeof(TCP_Header_t)];

            uint16_t PacketSize = ConnectionStateTable[CSTableEntry].Info.Buffer.Length;

            /* Fill out the TCP data */
            TCPHeaderOUT->SourcePort           = ConnectionStateTable[CSTableEntry].Port;
            TCPHeaderOUT->DestinationPort      = ConnectionStateTable[CSTableEntry].RemotePort;
            TCPHeaderOUT->SequenceNumber       = SwapEndian_32(ConnectionStateTable[CSTableEntry].Info.SequenceNumberOut);
            TCPHeaderOUT->AcknowledgmentNumber = SwapEndian_32(ConnectionStateTable[CSTableEntry].Info.SequenceNumberIn);
            TCPHeaderOUT->DataOffset           = (sizeof(TCP_Header_t) / sizeof(uint32_t));
            TCPHeaderOUT->WindowSize           = SwapEndian_16(TCP_WINDOW_SIZE);

            TCPHeaderOUT->Flags                = TCP_FLAG_ACK;
            TCPHeaderOUT->UrgentPointer        = 0;
            TCPHeaderOUT->Checksum             = 0;
            TCPHeaderOUT->Reserved             = 0;

            memcpy(TCPDataOUT, ConnectionStateTable[CSTableEntry].Info.Buffer.Data, PacketSize);

            ConnectionStateTable[CSTableEntry].Info.SequenceNumberOut += PacketSize;

            TCPHeaderOUT->Checksum             = TCP_Checksum16(TCPHeaderOUT, &ServerIPAddress,
                                                                &ConnectionStateTable[CSTableEntry].RemoteAddress,
                                                                (sizeof(TCP_Header_t) + PacketSize));

            PacketSize += sizeof(TCP_Header_t);

            /* Fill out the response IP header */
            IPHeaderOUT->TotalLength        = SwapEndian_16(sizeof(IP_Header_t) + PacketSize);
            IPHeaderOUT->TypeOfService      = 0;
            IPHeaderOUT->HeaderLength       = (sizeof(IP_Header_t) / sizeof(uint32_t));
            IPHeaderOUT->Version            = 4;
            IPHeaderOUT->Flags              = 0;
            IPHeaderOUT->FragmentOffset     = 0;
            IPHeaderOUT->Identification     = 0;
            IPHeaderOUT->HeaderChecksum     = 0;
            IPHeaderOUT->Protocol           = PROTOCOL_TCP;
            IPHeaderOUT->TTL                = DEFAULT_TTL;
            IPHeaderOUT->SourceAddress      = ServerIPAddress;
            IPHeaderOUT->DestinationAddress = ConnectionStateTable[CSTableEntry].RemoteAddress;

            IPHeaderOUT->HeaderChecksum     = Ethernet_Checksum16(IPHeaderOUT, sizeof(IP_Header_t));

            PacketSize += sizeof(IP_Header_t);
            FrameOUTHeader->Source          = ServerMACAddress;
            FrameOUTHeader->Destination     = (MAC_Address_t){{0x02, 0x00, 0x02, 0x00, 0x02, 0x00}};
            FrameOUTHeader->EtherType       = SwapEndian_16(ETHERTYPE_IPV4);

            PacketSize += sizeof(Ethernet_Frame_Header_t);

            /* Set the response length in the buffer and indicate that a response is ready to be sent */
            FrameOUT.FrameLength            = PacketSize;

            ConnectionStateTable[CSTableEntry].Info.Buffer.Ready = false;

            break;
        }
    }
}

void TCP_Init(void)
{
    for (uint8_t PTableEntry = 0; PTableEntry < MAX_OPEN_TCP_PORTS; PTableEntry++)
        PortStateTable[PTableEntry].State = TCP_Port_Closed;

    for (uint8_t CSTableEntry = 0; CSTableEntry < MAX_TCP_CONNECTIONS; CSTableEntry++)
        ConnectionStateTable[CSTableEntry].State = TCP_Connection_Closed;
}

bool TCP_SetPortState(const uint16_t Port,
                      const uint8_t State,
                      void (*Handler)(TCP_ConnectionState_t*, TCP_ConnectionBuffer_t*))
{
    for (uint8_t PTableEntry = 0; PTableEntry < MAX_OPEN_TCP_PORTS; PTableEntry++)
    {
        if (PortStateTable[PTableEntry].Port == Port)
        {
            PortStateTable[PTableEntry].State = State;
            PortStateTable[PTableEntry].ApplicationHandler = Handler;
            return true;
        }
    }

    if (State == TCP_Port_Open)
    {
        for (uint8_t PTableEntry = 0; PTableEntry < MAX_OPEN_TCP_PORTS; PTableEntry++)
        {
            if (PortStateTable[PTableEntry].State == TCP_Port_Closed)
            {
                PortStateTable[PTableEntry].Port  = Port;
                PortStateTable[PTableEntry].State = State;
                PortStateTable[PTableEntry].ApplicationHandler = Handler;
                return true;
            }
        }

        return false;
    }
    else
    {
        return true;
    }
}

uint8_t TCP_GetPortState(const uint16_t Port)
{
    /* Note, Port number should be specified in BIG endian to simplify network code */

    for (uint8_t PTableEntry = 0; PTableEntry < MAX_OPEN_TCP_PORTS; PTableEntry++)
    {
        /* Find existing entry for the port in the table, return the port status if found */
        if (PortStateTable[PTableEntry].Port == Port)
          return PortStateTable[PTableEntry].State;
    }

    /* Port not in table, assume closed */
    return TCP_Port_Closed;
}

bool TCP_SetConnectionState(const uint16_t Port,
                            const IP_Address_t* RemoteAddress,
                            const uint16_t RemotePort,
                            const uint8_t State)
{
    /* Note, Port number should be specified in BIG endian to simplify network code */

    for (uint8_t CSTableEntry = 0; CSTableEntry < MAX_TCP_CONNECTIONS; CSTableEntry++)
    {
        if ((ConnectionStateTable[CSTableEntry].Port == Port) &&
             IP_COMPARE(&ConnectionStateTable[CSTableEntry].RemoteAddress, RemoteAddress) &&
             ConnectionStateTable[CSTableEntry].RemotePort == RemotePort)
        {
            ConnectionStateTable[CSTableEntry].State = State;
            return true;
        }
    }

    for (uint8_t CSTableEntry = 0; CSTableEntry < MAX_TCP_CONNECTIONS; CSTableEntry++)
    {
        if (ConnectionStateTable[CSTableEntry].State == TCP_Connection_Closed)
        {
            ConnectionStateTable[CSTableEntry].Port          = Port;
            ConnectionStateTable[CSTableEntry].RemoteAddress = *RemoteAddress;
            ConnectionStateTable[CSTableEntry].RemotePort    = RemotePort;
            ConnectionStateTable[CSTableEntry].State         = State;
            return true;
        }
    }

    return false;
}

uint8_t TCP_GetConnectionState(const uint16_t Port,
                               const IP_Address_t* RemoteAddress,
                               const uint16_t RemotePort)
{
    for (uint8_t CSTableEntry = 0; CSTableEntry < MAX_TCP_CONNECTIONS; CSTableEntry++)
    {
        if ((ConnectionStateTable[CSTableEntry].Port == Port) &&
             IP_COMPARE(&ConnectionStateTable[CSTableEntry].RemoteAddress, RemoteAddress) &&
             ConnectionStateTable[CSTableEntry].RemotePort == RemotePort)

        {
            return ConnectionStateTable[CSTableEntry].State;
        }
    }

    return TCP_Connection_Closed;
}

TCP_ConnectionInfo_t* TCP_GetConnectionInfo(const uint16_t Port,
                                            const IP_Address_t* RemoteAddress,
                                            const uint16_t RemotePort)
{
    for (uint8_t CSTableEntry = 0; CSTableEntry < MAX_TCP_CONNECTIONS; CSTableEntry++)
    {
        if ((ConnectionStateTable[CSTableEntry].Port == Port) &&
             IP_COMPARE(&ConnectionStateTable[CSTableEntry].RemoteAddress, RemoteAddress) &&
             ConnectionStateTable[CSTableEntry].RemotePort == RemotePort)
        {
            return &ConnectionStateTable[CSTableEntry].Info;
        }
    }

    return NULL;
}

int16_t TCP_ProcessTCPPacket(void* IPHeaderInStart, void* TCPHeaderInStart,
                             void* TCPHeaderOutStart)
{
    IP_Header_t*  IPHeaderIN   = (IP_Header_t*)IPHeaderInStart;
    TCP_Header_t* TCPHeaderIN  = (TCP_Header_t*)TCPHeaderInStart;
    TCP_Header_t* TCPHeaderOUT = (TCP_Header_t*)TCPHeaderOutStart;
    TCP_ConnectionInfo_t* ConnectionInfo;
    DecodeTCPHeader(TCPHeaderInStart);
    bool PacketResponse = false;

    /* Check if the destination port is open and allows incoming connections */
    if (TCP_GetPortState(TCPHeaderIN->DestinationPort) == TCP_Port_Open)
    {
        /* Detect SYN from host to start a connection */
        if (TCPHeaderIN->Flags & TCP_FLAG_SYN)
          TCP_SetConnectionState(TCPHeaderIN->DestinationPort, &IPHeaderIN->SourceAddress, TCPHeaderIN->SourcePort, TCP_Connection_Listen);

        /* Detect RST from host to abort existing connection */
        if (TCPHeaderIN->Flags & TCP_FLAG_RST)
        {
            if (TCP_SetConnectionState(TCPHeaderIN->DestinationPort, &IPHeaderIN->SourceAddress,
                                       TCPHeaderIN->SourcePort, TCP_Connection_Closed))
            {
                TCPHeaderOUT->Flags = (TCP_FLAG_RST | TCP_FLAG_ACK);
                PacketResponse = true;
            }
        }
        else
        {
            /* Process the incoming TCP packet based on the current connection state for the sender and port */
            switch (TCP_GetConnectionState(TCPHeaderIN->DestinationPort, &IPHeaderIN->SourceAddress, TCPHeaderIN->SourcePort))
            {
                case TCP_Connection_Listen:
                    if (TCPHeaderIN->Flags == TCP_FLAG_SYN)
                    {
                        /* SYN connection starts a connection with a peer */
                        if (TCP_SetConnectionState(TCPHeaderIN->DestinationPort, &IPHeaderIN->SourceAddress,
                                                   TCPHeaderIN->SourcePort, TCP_Connection_SYNReceived))
                        {
                            TCPHeaderOUT->Flags = (TCP_FLAG_SYN | TCP_FLAG_ACK);

                            ConnectionInfo = TCP_GetConnectionInfo(TCPHeaderIN->DestinationPort, &IPHeaderIN->SourceAddress, TCPHeaderIN->SourcePort);

                            ConnectionInfo->SequenceNumberIn  = (SwapEndian_32(TCPHeaderIN->SequenceNumber) + 1);
                            ConnectionInfo->SequenceNumberOut = 0;
                            ConnectionInfo->Buffer.InUse      = false;
                        }
                        else
                        {
                            TCPHeaderOUT->Flags = TCP_FLAG_RST;
                        }

                        PacketResponse      = true;
                    }

                    break;
                case TCP_Connection_SYNReceived:
                    if (TCPHeaderIN->Flags == TCP_FLAG_ACK)
                    {
                        /* ACK during the connection process completes the connection to a peer */

                        TCP_SetConnectionState(TCPHeaderIN->DestinationPort, &IPHeaderIN->SourceAddress,
                                               TCPHeaderIN->SourcePort, TCP_Connection_Established);

                        ConnectionInfo = TCP_GetConnectionInfo(TCPHeaderIN->DestinationPort, &IPHeaderIN->SourceAddress,
                                                               TCPHeaderIN->SourcePort);

                        ConnectionInfo->SequenceNumberOut++;
                    }

                    break;
                case TCP_Connection_Established:
                    if (TCPHeaderIN->Flags == (TCP_FLAG_FIN | TCP_FLAG_ACK))
                    {
                        /* FIN ACK when connected to a peer starts the finalization process */

                        TCPHeaderOUT->Flags = (TCP_FLAG_FIN | TCP_FLAG_ACK);
                        PacketResponse      = true;

                        TCP_SetConnectionState(TCPHeaderIN->DestinationPort, &IPHeaderIN->SourceAddress,
                                               TCPHeaderIN->SourcePort, TCP_Connection_CloseWait);

                        ConnectionInfo = TCP_GetConnectionInfo(TCPHeaderIN->DestinationPort, &IPHeaderIN->SourceAddress,
                                                               TCPHeaderIN->SourcePort);

                        ConnectionInfo->SequenceNumberIn++;
                        ConnectionInfo->SequenceNumberOut++;
                    }
                    else if ((TCPHeaderIN->Flags == TCP_FLAG_ACK) || (TCPHeaderIN->Flags == (TCP_FLAG_ACK | TCP_FLAG_PSH)))
                    {
                        ConnectionInfo = TCP_GetConnectionInfo(TCPHeaderIN->DestinationPort, &IPHeaderIN->SourceAddress,
                                                               TCPHeaderIN->SourcePort);

                        /* Check if the buffer is currently in use either by a buffered data to send, or receive */
                        if ((ConnectionInfo->Buffer.InUse == false) && (ConnectionInfo->Buffer.Ready == false))
                        {
                            ConnectionInfo->Buffer.Direction = TCP_PACKETDIR_IN;
                            ConnectionInfo->Buffer.InUse     = true;
                            ConnectionInfo->Buffer.Length    = 0;
                        }

                        if ((ConnectionInfo->Buffer.Direction == TCP_PACKETDIR_IN) &&
                            (ConnectionInfo->Buffer.Length != TCP_WINDOW_SIZE))
                        {
                            uint16_t IPOffset   = (IPHeaderIN->HeaderLength * sizeof(uint32_t));
                            uint16_t TCPOffset  = (TCPHeaderIN->DataOffset * sizeof(uint32_t));
                            uint16_t DataLength = (SwapEndian_16(IPHeaderIN->TotalLength) - IPOffset - TCPOffset);

                            /* Copy the packet data into the buffer */
                            memcpy(&ConnectionInfo->Buffer.Data[ConnectionInfo->Buffer.Length],
                                   &((uint8_t*)TCPHeaderInStart)[TCPOffset],
                                   DataLength);

                            ConnectionInfo->SequenceNumberIn += DataLength;
                            ConnectionInfo->Buffer.Length    += DataLength;

                            /* Check if the buffer is full or if the PSH flag is set, if so indicate buffer ready */
                            if ((!(TCP_WINDOW_SIZE - ConnectionInfo->Buffer.Length)) || (TCPHeaderIN->Flags & TCP_FLAG_PSH))
                            {
                                ConnectionInfo->Buffer.InUse = false;
                                ConnectionInfo->Buffer.Ready = true;

                                TCPHeaderOUT->Flags = TCP_FLAG_ACK;
                                PacketResponse      = true;
                            }
                        }
                        else
                        {
                            /* Buffer is currently in use by the application, defer processing of the incoming packet */
                            return NO_PROCESS;
                        }
                    }

                    break;
                case TCP_Connection_Closing:
                        ConnectionInfo = TCP_GetConnectionInfo(TCPHeaderIN->DestinationPort, &IPHeaderIN->SourceAddress,
                                                               TCPHeaderIN->SourcePort);

                        TCPHeaderOUT->Flags = (TCP_FLAG_ACK | TCP_FLAG_FIN);
                        PacketResponse      = true;

                        ConnectionInfo->Buffer.InUse = false;

                        TCP_SetConnectionState(TCPHeaderIN->DestinationPort, &IPHeaderIN->SourceAddress,
                                               TCPHeaderIN->SourcePort, TCP_Connection_FINWait1);

                    break;
                case TCP_Connection_FINWait1:
                    if (TCPHeaderIN->Flags == (TCP_FLAG_FIN | TCP_FLAG_ACK))
                    {
                        ConnectionInfo = TCP_GetConnectionInfo(TCPHeaderIN->DestinationPort, &IPHeaderIN->SourceAddress,
                                                               TCPHeaderIN->SourcePort);

                        TCPHeaderOUT->Flags = TCP_FLAG_ACK;
                        PacketResponse      = true;

                        ConnectionInfo->SequenceNumberIn++;
                        ConnectionInfo->SequenceNumberOut++;

                        TCP_SetConnectionState(TCPHeaderIN->DestinationPort, &IPHeaderIN->SourceAddress,
                                               TCPHeaderIN->SourcePort, TCP_Connection_Closed);
                    }

                    else if (TCPHeaderIN->Flags == TCP_FLAG_ACK)
                    {
                        TCP_SetConnectionState(TCPHeaderIN->DestinationPort, &IPHeaderIN->SourceAddress,
                                               TCPHeaderIN->SourcePort, TCP_Connection_FINWait2);
                    }

                    break;
                case TCP_Connection_FINWait2:
                    if (TCPHeaderIN->Flags == (TCP_FLAG_FIN | TCP_FLAG_ACK))
                    {
                        ConnectionInfo = TCP_GetConnectionInfo(TCPHeaderIN->DestinationPort, &IPHeaderIN->SourceAddress,
                                                               TCPHeaderIN->SourcePort);

                        TCPHeaderOUT->Flags = TCP_FLAG_ACK;
                        PacketResponse      = true;

                        ConnectionInfo->SequenceNumberIn++;
                        ConnectionInfo->SequenceNumberOut++;

                        TCP_SetConnectionState(TCPHeaderIN->DestinationPort, &IPHeaderIN->SourceAddress,
                                               TCPHeaderIN->SourcePort, TCP_Connection_Closed);
                    }

                    break;
                case TCP_Connection_CloseWait:
                    if (TCPHeaderIN->Flags == TCP_FLAG_ACK)
                    {
                        TCP_SetConnectionState(TCPHeaderIN->DestinationPort, &IPHeaderIN->SourceAddress,
                                               TCPHeaderIN->SourcePort, TCP_Connection_Closed);
                    }

                    break;
            }
        }
    }
    else
    {
        /* Port is not open, indicate via a RST/ACK response to the sender */
        TCPHeaderOUT->Flags = (TCP_FLAG_RST | TCP_FLAG_ACK);
        PacketResponse      = true;
    }

    if (PacketResponse)
    {
        ConnectionInfo = TCP_GetConnectionInfo(TCPHeaderIN->DestinationPort, &IPHeaderIN->SourceAddress,
                                               TCPHeaderIN->SourcePort);

        TCPHeaderOUT->SourcePort           = TCPHeaderIN->DestinationPort;
        TCPHeaderOUT->DestinationPort      = TCPHeaderIN->SourcePort;
        TCPHeaderOUT->SequenceNumber       = SwapEndian_32(ConnectionInfo->SequenceNumberOut);
        TCPHeaderOUT->AcknowledgmentNumber = SwapEndian_32(ConnectionInfo->SequenceNumberIn);
        TCPHeaderOUT->DataOffset           = (sizeof(TCP_Header_t) / sizeof(uint32_t));

        if (!(ConnectionInfo->Buffer.InUse))
          TCPHeaderOUT->WindowSize         = SwapEndian_16(TCP_WINDOW_SIZE);
        else
          TCPHeaderOUT->WindowSize         = SwapEndian_16(TCP_WINDOW_SIZE - ConnectionInfo->Buffer.Length);

        TCPHeaderOUT->UrgentPointer        = 0;
        TCPHeaderOUT->Checksum             = 0;
        TCPHeaderOUT->Reserved             = 0;

        TCPHeaderOUT->Checksum             = TCP_Checksum16(TCPHeaderOUT, &IPHeaderIN->DestinationAddress,
                                                            &IPHeaderIN->SourceAddress, sizeof(TCP_Header_t));

        return sizeof(TCP_Header_t);
    }

    return NO_RESPONSE;
}





int16_t ARP_ProcessARPPacket(void* InDataStart,
                             void* OutDataStart)
{
    DecodeARPHeader(InDataStart);

    ARP_Header_t* ARPHeaderIN  = (ARP_Header_t*)InDataStart;
    ARP_Header_t* ARPHeaderOUT = (ARP_Header_t*)OutDataStart;

    /* Ensure that the ARP request is a IPv4 request packet */
    if ((SwapEndian_16(ARPHeaderIN->ProtocolType) == ETHERTYPE_IPV4) &&
        (SwapEndian_16(ARPHeaderIN->Operation) == ARP_OPERATION_REQUEST))
    {
        /* If the ARP packet is requesting the MAC or IP of the virtual webserver, return the response */
        if (IP_COMPARE(&ARPHeaderIN->TPA, &ServerIPAddress) ||
            MAC_COMPARE(&ARPHeaderIN->THA, &ServerMACAddress))
        {
            /* Fill out the ARP response header */
            ARPHeaderOUT->HardwareType = ARPHeaderIN->HardwareType;
            ARPHeaderOUT->ProtocolType = ARPHeaderIN->ProtocolType;
            ARPHeaderOUT->HLEN         = ARPHeaderIN->HLEN;
            ARPHeaderOUT->PLEN         = ARPHeaderIN->PLEN;
            ARPHeaderOUT->Operation    = SwapEndian_16(ARP_OPERATION_REPLY);

            /* Copy over the sender MAC/IP to the target fields for the response */
            ARPHeaderOUT->THA = ARPHeaderIN->SHA;
            ARPHeaderOUT->TPA = ARPHeaderIN->SPA;

            /* Copy over the new sender MAC/IP - MAC and IP addresses of the virtual webserver */
            ARPHeaderOUT->SHA = ServerMACAddress;
            ARPHeaderOUT->SPA = ServerIPAddress;

            /* Return the size of the response so far */
            return sizeof(ARP_Header_t);
        }
    }

    return NO_RESPONSE;
}

int16_t DHCP_ProcessDHCPPacket(void* IPHeaderInStart,
                               void* DHCPHeaderInStart,
                               void* DHCPHeaderOutStart)
{
    IP_Header_t*   IPHeaderIN    = (IP_Header_t*)IPHeaderInStart;
    DHCP_Header_t* DHCPHeaderIN  = (DHCP_Header_t*)DHCPHeaderInStart;
    DHCP_Header_t* DHCPHeaderOUT = (DHCP_Header_t*)DHCPHeaderOutStart;

    uint8_t* DHCPOptionsINStart = (uint8_t*)((uint8_t *)DHCPHeaderInStart + sizeof(DHCP_Header_t));

    uint8_t* DHCPOptionsOUTStart = (uint8_t*)((uint8_t *)DHCPHeaderOutStart +
        sizeof(DHCP_Header_t));

    DecodeDHCPHeader(DHCPHeaderInStart);

    /* Zero out the response DHCP packet, as much of it is legacy and left at 0 */
    memset(DHCPHeaderOUT, 0, sizeof(DHCP_Header_t));

    /* Fill out the response DHCP packet */
    DHCPHeaderOUT->HardwareType          = DHCPHeaderIN->HardwareType;
    DHCPHeaderOUT->Operation             = DHCP_OP_BOOTREPLY;
    DHCPHeaderOUT->HardwareAddressLength = DHCPHeaderIN->HardwareAddressLength;
    DHCPHeaderOUT->Hops                  = 0;
    DHCPHeaderOUT->TransactionID         = DHCPHeaderIN->TransactionID;
    DHCPHeaderOUT->ElapsedSeconds        = 0;
    DHCPHeaderOUT->Flags                 = DHCPHeaderIN->Flags;
    DHCPHeaderOUT->YourIP                = ClientIPAddress;
    memmove(&DHCPHeaderOUT->ClientHardwareAddress, &DHCPHeaderIN->ClientHardwareAddress, sizeof(MAC_Address_t));
    DHCPHeaderOUT->Cookie                = SwapEndian_32(DHCP_MAGIC_COOKIE);

    /* Alter the incoming IP packet header so that the corrected IP source and destinations are used - this means that
       when the response IP header is generated, it will use the corrected addresses and not the null/broatcast addresses */
    IPHeaderIN->SourceAddress      = ClientIPAddress;
    IPHeaderIN->DestinationAddress = ServerIPAddress;

    /* Process the incoming DHCP packet options */
    while (DHCPOptionsINStart[0] != DHCP_OPTION_END)
    {
        /* Find the Message Type DHCP option, to determine the type of DHCP packet */
        if (DHCPOptionsINStart[0] == DHCP_OPTION_MESSAGETYPE)
        {
            if ((DHCPOptionsINStart[2] == DHCP_MESSAGETYPE_DISCOVER) || (DHCPOptionsINStart[2] == DHCP_MESSAGETYPE_REQUEST))
            {
                *(DHCPOptionsOUTStart++) = DHCP_OPTION_MESSAGETYPE;
                *(DHCPOptionsOUTStart++) = 1;
                *(DHCPOptionsOUTStart++) = (DHCPOptionsINStart[2] == DHCP_MESSAGETYPE_DISCOVER) ? DHCP_MESSAGETYPE_OFFER
                                                                                                : DHCP_MESSAGETYPE_ACK;

                *(DHCPOptionsOUTStart++) = DHCP_OPTION_SUBNETMASK;
                *(DHCPOptionsOUTStart++) = sizeof(IP_Address_t);
                *(DHCPOptionsOUTStart++) = 0xFF;
                *(DHCPOptionsOUTStart++) = 0xFF;
                *(DHCPOptionsOUTStart++) = 0xFF;
                *(DHCPOptionsOUTStart++) = 0x00;
                *(DHCPOptionsOUTStart++) = DHCP_OPTION_LEASETIME;
                *(DHCPOptionsOUTStart++) = sizeof(uint32_t);
                *(DHCPOptionsOUTStart++) = 0x00;
                *(DHCPOptionsOUTStart++) = 0x01;
                *(DHCPOptionsOUTStart++) = 0x51;
                *(DHCPOptionsOUTStart++) = 0x80;
                *(DHCPOptionsOUTStart++) = DHCP_OPTION_DHCPSERVER;
                *(DHCPOptionsOUTStart++) = sizeof(IP_Address_t);
                memcpy(DHCPOptionsOUTStart, &ServerIPAddress, sizeof(IP_Address_t));
                DHCPOptionsOUTStart     += sizeof(IP_Address_t);
                *(DHCPOptionsOUTStart++) = DHCP_OPTION_END;
                return (sizeof(DHCP_Header_t) + 18 + sizeof(IP_Address_t));
            }
        }

        DHCPOptionsINStart += ((DHCPOptionsINStart[0] == DHCP_OPTION_PAD) ? 1 : (DHCPOptionsINStart[1] + 2));
    }

    return NO_RESPONSE;
}

int16_t UDP_ProcessUDPPacket(void* IPHeaderInStart,
                             void* UDPHeaderInStart,
                             void* UDPHeaderOutStart)
{
    UDP_Header_t* UDPHeaderIN  = (UDP_Header_t*)UDPHeaderInStart;
    UDP_Header_t* UDPHeaderOUT = (UDP_Header_t*)UDPHeaderOutStart;

    int16_t RetSize = NO_RESPONSE;

    DecodeUDPHeader(UDPHeaderInStart);

    switch (SwapEndian_16(UDPHeaderIN->DestinationPort))
    {
        case UDP_PORT_DHCP_REQUEST:
            RetSize = DHCP_ProcessDHCPPacket(IPHeaderInStart,
                                             &((uint8_t*)UDPHeaderInStart)[sizeof(UDP_Header_t)],
                                             &((uint8_t*)UDPHeaderOutStart)[sizeof(UDP_Header_t)]);
            break;
    }

    /* Check to see if the protocol processing routine has filled out a response */
    if (RetSize > 0)
    {
        /* Fill out the response UDP packet header */
        UDPHeaderOUT->SourcePort      = UDPHeaderIN->DestinationPort;
        UDPHeaderOUT->DestinationPort = UDPHeaderIN->SourcePort;
        UDPHeaderOUT->Checksum        = 0;
        UDPHeaderOUT->Length          = SwapEndian_16(sizeof(UDP_Header_t) + RetSize);

        /* Return the size of the response so far */
        return (sizeof(UDP_Header_t) + RetSize);
    }

    return NO_RESPONSE;
}

/** HTTP server response header, for transmission before the page contents. This indicates to the host that a page exists at the
 *  given location, and gives extra connection information.
 */
const char PROGMEM HTTP200Header[] = "HTTP/1.1 200 OK\r\n"
                                     "Server: LUFA RNDIS\r\n"
                                     "Content-type: text/html\r\n"
                                     "Connection: close\r\n\r\n";

/** HTTP server response header, for transmission before a resource not found error. This indicates to the host that the given
 *  given URL is invalid, and gives extra error information.
 */
const char PROGMEM HTTP404Header[] = "HTTP/1.1 404 Not Found\r\n"
                                     "Server: LUFA RNDIS\r\n"
                                     "Connection: close\r\n\r\n";

const char PROGMEM HTTPPage[]   =
        "<html>"
        "   <head>"
        "       <title>"
        "           LUFA Webserver Demo"
        "       </title>"
        "   </head>"
        "   <body>"
        "       <h1>Hello from your USB AVR!</h1>"
        "       <p>"
        "           Hello! Welcome to the LUFA RNDIS Demo Webserver test page, running on your USB AVR via the LUFA library. This demonstrates the HTTP webserver, TCP/IP stack and RNDIS demo all running atop the LUFA USB stack."
        "           <br /><br />"
        "           <small>Project Information: <a href=\"http://www.lufa-lib.org\">http://www.lufa-lib.org</a>.</small>"
        "           <hr />"
        "           <i>LUFA Version: </i>" LUFA_VERSION_STRING
        "       </p>"
        "   </body>"
        "</html>";


void Webserver_Init(void)
{
    TCP_SetPortState(TCP_PORT_HTTP, TCP_Port_Open, Webserver_ApplicationCallback);
}

/** Indicates if a given request equals the given HTTP command.
 *
 *  \param[in] RequestHeader  HTTP request made by the host
 *  \param[in] Command        HTTP command to compare the request to
 *
 *  \return Boolean \c true if the command matches the request, \c false otherwise
 */
static bool IsHTTPCommand(uint8_t* RequestHeader, const char* Command)
{
    return (strncmp((char*)RequestHeader, Command, strlen(Command)) == 0);
}

void Webserver_ApplicationCallback(TCP_ConnectionState_t* const ConnectionState,
                                   TCP_ConnectionBuffer_t* const Buffer)
{
    char*          BufferDataStr = (char*)Buffer->Data;
    static uint8_t PageBlock     = 0;

    /* Check to see if a packet has been received on the HTTP port from a remote host */
    if (TCP_APP_HAS_RECEIVED_PACKET(Buffer))
    {
        if (IsHTTPCommand(Buffer->Data, "GET"))
        {
            if (IsHTTPCommand(Buffer->Data, "GET / "))
            {
                PageBlock = 0;
                strcpy_P(BufferDataStr, HTTP200Header);
                TCP_APP_SEND_BUFFER(Buffer, strlen(BufferDataStr));
                TCP_APP_CAPTURE_BUFFER(Buffer);
            }
            else
            {
                /* Copy the HTTP 404 response header into the packet buffer */
                strcpy_P(BufferDataStr, HTTP404Header);

                /* Send the buffer contents to the host */
                TCP_APP_SEND_BUFFER(Buffer, strlen(BufferDataStr));

                /* All data sent, close the connection */
                TCP_APP_CLOSECONNECTION(ConnectionState);
            }
        }
        else if (IsHTTPCommand(Buffer->Data, "HEAD"))
        {
            if (IsHTTPCommand(Buffer->Data, "HEAD / "))
            {
                strcpy_P(BufferDataStr, HTTP200Header);
                TCP_APP_SEND_BUFFER(Buffer, strlen(BufferDataStr));
            }
            else
            {
                strcpy_P(BufferDataStr, HTTP404Header);
                TCP_APP_SEND_BUFFER(Buffer, strlen(BufferDataStr));
            }

            TCP_APP_CLOSECONNECTION(ConnectionState);
        }
        else if (IsHTTPCommand(Buffer->Data, "TRACE"))
        {
            TCP_APP_SEND_BUFFER(Buffer, Buffer->Length);
            TCP_APP_CLOSECONNECTION(ConnectionState);
        }
        else
        {
            TCP_APP_CLEAR_BUFFER(Buffer);
        }
    }
    else if (TCP_APP_HAVE_CAPTURED_BUFFER(Buffer))
    {
        uint16_t RemLength = strlen_P(&HTTPPage[PageBlock * HTTP_REPLY_BLOCK_SIZE]);
        uint16_t Length;
        Length = MIN(RemLength, HTTP_REPLY_BLOCK_SIZE);
        strncpy_P(BufferDataStr, &HTTPPage[PageBlock * HTTP_REPLY_BLOCK_SIZE], Length);
        TCP_APP_SEND_BUFFER(Buffer, Length);

        if (PageBlock++ == (sizeof(HTTPPage) / HTTP_REPLY_BLOCK_SIZE))
        {
            TCP_APP_RELEASE_BUFFER(Buffer);
            TCP_APP_CLOSECONNECTION(ConnectionState);
        }
    }
}


int16_t ICMP_ProcessICMPPacket(void* InDataStart,
                               void* OutDataStart)
{
    ICMP_Header_t* ICMPHeaderIN  = (ICMP_Header_t*)InDataStart;
    ICMP_Header_t* ICMPHeaderOUT = (ICMP_Header_t*)OutDataStart;

    DecodeICMPHeader(InDataStart);

    if (ICMPHeaderIN->Type == ICMP_TYPE_ECHOREQUEST)
    {
        ICMPHeaderOUT->Type     = ICMP_TYPE_ECHOREPLY;
        ICMPHeaderOUT->Code     = 0;
        ICMPHeaderOUT->Checksum = 0;
        ICMPHeaderOUT->Id       = ICMPHeaderIN->Id;
        ICMPHeaderOUT->Sequence = ICMPHeaderIN->Sequence;

        intptr_t DataSize = FrameIN.FrameLength - ((((intptr_t)InDataStart + sizeof(ICMP_Header_t)) - (intptr_t)FrameIN.FrameData));

        memmove(&((uint8_t*)OutDataStart)[sizeof(ICMP_Header_t)],
                &((uint8_t*)InDataStart)[sizeof(ICMP_Header_t)],
                DataSize);

        ICMPHeaderOUT->Checksum = Ethernet_Checksum16(ICMPHeaderOUT, (DataSize + sizeof(ICMP_Header_t)));

        return (DataSize + sizeof(ICMP_Header_t));
    }

    return NO_RESPONSE;
}


Ethernet_Frame_Info_t FrameIN;
Ethernet_Frame_Info_t FrameOUT;
const MAC_Address_t ServerMACAddress    = {SERVER_MAC_ADDRESS};
const IP_Address_t  ServerIPAddress     = {SERVER_IP_ADDRESS};
const MAC_Address_t BroadcastMACAddress = {BROADCAST_MAC_ADDRESS};
const IP_Address_t  BroadcastIPAddress  = {BROADCAST_IP_ADDRESS};
const IP_Address_t  ClientIPAddress     = {CLIENT_IP_ADDRESS};

void Ethernet_ProcessPacket(void)
{
    Ethernet_Frame_Header_t* FrameINHeader  = (Ethernet_Frame_Header_t*)&FrameIN.FrameData;
    Ethernet_Frame_Header_t* FrameOUTHeader = (Ethernet_Frame_Header_t*)&FrameOUT.FrameData;
    int16_t                  RetSize        = NO_RESPONSE;

    if ((MAC_COMPARE(&FrameINHeader->Destination, &ServerMACAddress) ||
         MAC_COMPARE(&FrameINHeader->Destination, &BroadcastMACAddress)) &&
        (SwapEndian_16(FrameIN.FrameLength) > ETHERNET_VER2_MINSIZE))
    {
        switch (SwapEndian_16(FrameINHeader->EtherType))
        {
            case ETHERTYPE_ARP:
                RetSize = ARP_ProcessARPPacket(&FrameIN.FrameData[sizeof(Ethernet_Frame_Header_t)],
                                     &FrameOUT.FrameData[sizeof(Ethernet_Frame_Header_t)]);
                break;
            case ETHERTYPE_IPV4:
                RetSize = IP_ProcessIPPacket(&FrameIN.FrameData[sizeof(Ethernet_Frame_Header_t)],
                                       &FrameOUT.FrameData[sizeof(Ethernet_Frame_Header_t)]);
                break;
        }

        if (RetSize > 0)
        {
            FrameOUTHeader->Source          = ServerMACAddress;
            FrameOUTHeader->Destination     = FrameINHeader->Source;
            FrameOUTHeader->EtherType       = FrameINHeader->EtherType;
            FrameOUT.FrameLength            = (sizeof(Ethernet_Frame_Header_t) + RetSize);
        }
    }

    if (RetSize != NO_PROCESS)
    {
        FrameIN.FrameLength = 0;
    }
}

uint16_t Ethernet_Checksum16(void* Data,
                             uint16_t Bytes)
{
    uint16_t* Words    = (uint16_t*)Data;
    uint32_t  Checksum = 0;

    for (uint16_t CurrWord = 0; CurrWord < (Bytes >> 1); CurrWord++)
      Checksum += Words[CurrWord];

    while (Checksum & 0xFFFF0000)
      Checksum = ((Checksum & 0xFFFF) + (Checksum >> 16));

    return ~Checksum;
}








const USB_Descriptor_Device_t PROGMEM DeviceDescriptor =
{
    .Header                 = {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},

    .USBSpecification       = VERSION_BCD(1,1,0),
    .Class                  = CDC_CSCP_CDCClass,
    .SubClass               = CDC_CSCP_NoSpecificSubclass,
    .Protocol               = CDC_CSCP_NoSpecificProtocol,

    .Endpoint0Size          = FIXED_CONTROL_ENDPOINT_SIZE,

    .VendorID               = 0x03EB,
    .ProductID              = 0x204C,
    .ReleaseNumber          = VERSION_BCD(0,0,1),

    .ManufacturerStrIndex   = STRING_ID_Manufacturer,
    .ProductStrIndex        = STRING_ID_Product,
    .SerialNumStrIndex      = NO_DESCRIPTOR,

    .NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor =
{
    .Config =
        {
            .Header                 = {.Size = sizeof(USB_Descriptor_Configuration_Header_t), .Type = DTYPE_Configuration},

            .TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
            .TotalInterfaces        = 2,

            .ConfigurationNumber    = 1,
            .ConfigurationStrIndex  = NO_DESCRIPTOR,

            .ConfigAttributes       = (USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_SELFPOWERED),

            .MaxPowerConsumption    = USB_CONFIG_POWER_MA(100)
        },

    .CDC_CCI_Interface =
        {
            .Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

            .InterfaceNumber        = INTERFACE_ID_CDC_CCI,
            .AlternateSetting       = 0,

            .TotalEndpoints         = 1,

            .Class                  = CDC_CSCP_CDCClass,
            .SubClass               = CDC_CSCP_ACMSubclass,
            .Protocol               = CDC_CSCP_VendorSpecificProtocol,

            .InterfaceStrIndex      = NO_DESCRIPTOR
        },

    .CDC_Functional_Header =
        {
            .Header                 = {.Size = sizeof(USB_CDC_Descriptor_FunctionalHeader_t), .Type = DTYPE_CSInterface},
            .Subtype                = CDC_DSUBTYPE_CSInterface_Header,

            .CDCSpecification       = VERSION_BCD(1,1,0),
        },

    .CDC_Functional_ACM =
        {
            .Header                 = {.Size = sizeof(USB_CDC_Descriptor_FunctionalACM_t), .Type = DTYPE_CSInterface},
            .Subtype                = CDC_DSUBTYPE_CSInterface_ACM,

            .Capabilities           = 0x00,
        },

    .CDC_Functional_Union =
        {
            .Header                 = {.Size = sizeof(USB_CDC_Descriptor_FunctionalUnion_t), .Type = DTYPE_CSInterface},
            .Subtype                = CDC_DSUBTYPE_CSInterface_Union,

            .MasterInterfaceNumber  = INTERFACE_ID_CDC_CCI,
            .SlaveInterfaceNumber   = INTERFACE_ID_CDC_DCI,
        },
    .CDC_NotificationEndpoint =
        {
            .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

            .EndpointAddress        = CDC_NOTIFICATION_EPADDR,
            .Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
            .EndpointSize           = CDC_NOTIFICATION_EPSIZE,
            .PollingIntervalMS      = 0xFF
        },

    .CDC_DCI_Interface =
        {
            .Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

            .InterfaceNumber        = INTERFACE_ID_CDC_DCI,
            .AlternateSetting       = 0,

            .TotalEndpoints         = 2,

            .Class                  = CDC_CSCP_CDCDataClass,
            .SubClass               = CDC_CSCP_NoDataSubclass,
            .Protocol               = CDC_CSCP_NoDataProtocol,

            .InterfaceStrIndex      = NO_DESCRIPTOR
        },

    .RNDIS_DataOutEndpoint =
        {
            .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

            .EndpointAddress        = CDC_RX_EPADDR,
            .Attributes             = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
            .EndpointSize           = CDC_TXRX_EPSIZE,
            .PollingIntervalMS      = 0x05
        },

    .RNDIS_DataInEndpoint =
        {
            .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

            .EndpointAddress        = CDC_TX_EPADDR,
            .Attributes             = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
            .EndpointSize           = CDC_TXRX_EPSIZE,
            .PollingIntervalMS      = 0x05
        }
};

#if 0
const USB_Descriptor_String_t PROGMEM
LanguageString = USB_STRING_DESCRIPTOR_ARRAY(LANGUAGE_ID_ENG);
#endif

template <size_t S> struct DescString
{
    uint8_t size;
    uint8_t type;
    wchar_t UnicodeString[S];
};

static const DescString<2> PROGMEM LanguageString =
{
    USB_STRING_LEN(1),
    DTYPE_String,
    (wchar_t)0x0409
};

static const DescString<12> PROGMEM ManufacturerString =
{
    USB_STRING_LEN(11),
    DTYPE_String,
    L"Dean Camera"
};

static const DescString<16> PROGMEM ProductString =
{
    USB_STRING_LEN(15),
    DTYPE_String,
    L"LUFA RNDIS Demo"
};


#if 0
const USB_Descriptor_String_t PROGMEM
ManufacturerString = USB_STRING_DESCRIPTOR(L"Dean Camera");

const USB_Descriptor_String_t PROGMEM
ProductString = USB_STRING_DESCRIPTOR(L"LUFA RNDIS CDC Demo");
#endif

uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                    const uint16_t wIndex,
                                    const void** const DescriptorAddress)
{
    const uint8_t  DescriptorType   = (wValue >> 8);
    const uint8_t  DescriptorNumber = (wValue & 0xFF);

    const void* Address = NULL;
    uint16_t    Size    = NO_DESCRIPTOR;

    switch (DescriptorType)
    {
        case DTYPE_Device:
            Address = &DeviceDescriptor;
            Size    = sizeof(USB_Descriptor_Device_t);
            break;
        case DTYPE_Configuration:
            Address = &ConfigurationDescriptor;
            Size    = sizeof(USB_Descriptor_Configuration_t);
            break;
        case DTYPE_String:
            switch (DescriptorNumber)
            {
                case STRING_ID_Language:
                    Address = &LanguageString;
                    Size    = pgm_read_byte(&LanguageString.size);
                    break;
                case STRING_ID_Manufacturer:
                    Address = &ManufacturerString;
                    Size    = pgm_read_byte(&ManufacturerString.size);
                    break;
                case STRING_ID_Product:
                    Address = &ProductString;
                    Size    = pgm_read_byte(&ProductString.size);
                    break;
            }

            break;
    }

    *DescriptorAddress = Address;
    return Size;
}

int main(void)
{
    USB_Init();
	TCP_Init();
	Webserver_Init();
	sei();

	for (;;)
	{
        Ethernet_Task();
        TCP_Task();
        RNDIS_Task();
        USB_USBTask();
	}
}

void EVENT_USB_Device_Connect(void)
{
}

void EVENT_USB_Device_Disconnect(void)
{
}





void RNDIS_Task(void)
{
	Endpoint_SelectEndpoint(CDC_NOTIFICATION_EPADDR);

	if (Endpoint_IsINReady() && ResponseReady)
	{
		USB_Request_Header_t Notification = (USB_Request_Header_t)
			{
				.bmRequestType = (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE),
				.bRequest      = RNDIS_NOTIF_ResponseAvailable,
				.wValue        = 0,
				.wIndex        = 0,
				.wLength       = 0,
			};

		Endpoint_Write_Stream_LE(&Notification, sizeof(Notification), NULL);
		Endpoint_ClearIN();
		ResponseReady = false;
	}

	if ((CurrRNDISState == RNDIS_Data_Initialized) && !(MessageHeader->MessageLength))
	{
		RNDIS_Packet_Message_t RNDISPacketHeader;
		Endpoint_SelectEndpoint(CDC_RX_EPADDR);

		if (Endpoint_IsOUTReceived() && !(FrameIN.FrameLength))
		{
			Endpoint_Read_Stream_LE(&RNDISPacketHeader, sizeof(RNDIS_Packet_Message_t), NULL);

			if (RNDISPacketHeader.DataLength > ETHERNET_FRAME_SIZE_MAX)
			{
				Endpoint_StallTransaction();
				return;
			}

			Endpoint_Read_Stream_LE(FrameIN.FrameData, RNDISPacketHeader.DataLength, NULL);
			Endpoint_ClearOUT();
			FrameIN.FrameLength = RNDISPacketHeader.DataLength;
		}

		Endpoint_SelectEndpoint(CDC_TX_EPADDR);

		if (Endpoint_IsINReady() && FrameOUT.FrameLength)
		{
			memset(&RNDISPacketHeader, 0, sizeof(RNDIS_Packet_Message_t));
			RNDISPacketHeader.MessageType   = REMOTE_NDIS_PACKET_MSG;
			RNDISPacketHeader.MessageLength = (sizeof(RNDIS_Packet_Message_t) + FrameOUT.FrameLength);
			RNDISPacketHeader.DataOffset    = (sizeof(RNDIS_Packet_Message_t) - sizeof(RNDIS_Message_Header_t));
			RNDISPacketHeader.DataLength    = FrameOUT.FrameLength;
			Endpoint_Write_Stream_LE(&RNDISPacketHeader, sizeof(RNDIS_Packet_Message_t), NULL);
			Endpoint_Write_Stream_LE(FrameOUT.FrameData, RNDISPacketHeader.DataLength, NULL);
			Endpoint_ClearIN();
			FrameOUT.FrameLength = 0;
		}
	}
}

void Ethernet_Task(void)
{
	if (USB_DeviceState != DEVICE_STATE_Configured)
        return;

	if (FrameIN.FrameLength)
		Ethernet_ProcessPacket();
}

ISR(USB_GEN_vect, ISR_BLOCK)
{
    if (USB_INT_HasOccurred(USB_INT_SOFI) && USB_INT_IsEnabled(USB_INT_SOFI))
    {
        USB_INT_Clear(USB_INT_SOFI);
    }

    if (USB_INT_HasOccurred(USB_INT_VBUSTI) && USB_INT_IsEnabled(USB_INT_VBUSTI))
    {
        USB_INT_Clear(USB_INT_VBUSTI);

        if (USB_VBUS_GetStatus())
        {
            if (!(USB_Options & USB_OPT_MANUAL_PLL))
            {
                USB_PLL_On();
                while (!(USB_PLL_IsReady()));
            }

            USB_DeviceState = DEVICE_STATE_Powered;
            EVENT_USB_Device_Connect();
        }
        else
        {
            if (!(USB_Options & USB_OPT_MANUAL_PLL))
              USB_PLL_Off();

            USB_DeviceState = DEVICE_STATE_Unattached;
            EVENT_USB_Device_Disconnect();
        }
    }

    if (USB_INT_HasOccurred(USB_INT_SUSPI) && USB_INT_IsEnabled(USB_INT_SUSPI))
    {
        USB_INT_Disable(USB_INT_SUSPI);
        USB_INT_Enable(USB_INT_WAKEUPI);
        USB_CLK_Freeze();

        if (!(USB_Options & USB_OPT_MANUAL_PLL))
          USB_PLL_Off();

        USB_DeviceState = DEVICE_STATE_Suspended;
    }

    if (USB_INT_HasOccurred(USB_INT_WAKEUPI) && USB_INT_IsEnabled(USB_INT_WAKEUPI))
    {
        if (!(USB_Options & USB_OPT_MANUAL_PLL))
        {
            USB_PLL_On();
            while (!(USB_PLL_IsReady()));
        }

        USB_CLK_Unfreeze();
        USB_INT_Clear(USB_INT_WAKEUPI);
        USB_INT_Disable(USB_INT_WAKEUPI);
        USB_INT_Enable(USB_INT_SUSPI);

        if (USB_Device_ConfigurationNumber)
          USB_DeviceState = DEVICE_STATE_Configured;
        else
          USB_DeviceState = (USB_Device_IsAddressSet()) ? DEVICE_STATE_Addressed : DEVICE_STATE_Powered;

    }

    if (USB_INT_HasOccurred(USB_INT_EORSTI) && USB_INT_IsEnabled(USB_INT_EORSTI))
    {
        USB_INT_Clear(USB_INT_EORSTI);
        USB_DeviceState                = DEVICE_STATE_Default;
        USB_Device_ConfigurationNumber = 0;
        USB_INT_Clear(USB_INT_SUSPI);
        USB_INT_Disable(USB_INT_SUSPI);
        USB_INT_Enable(USB_INT_WAKEUPI);

        Endpoint_ConfigureEndpoint(ENDPOINT_CONTROLEP, EP_TYPE_CONTROL,
            USB_Device_ControlEndpointSize, 1);

    }
}

ISR(USB_COM_vect, ISR_BLOCK)
{
    uint8_t PrevSelectedEndpoint = Endpoint_GetCurrentEndpoint();
    Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);
    USB_INT_Disable(USB_INT_RXSTPI);
    GlobalInterruptEnable();
    USB_Device_ProcessControlRequest();
    Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);
    USB_INT_Enable(USB_INT_RXSTPI);
    Endpoint_SelectEndpoint(PrevSelectedEndpoint);
}



