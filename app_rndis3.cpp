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
#include "busby.h"

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <util/delay.h>

static constexpr uint8_t
#ifndef _BUSBY_H_
    FIXED_NUM_CONFIGURATIONS = 1,
    ENDPOINT_DIR_MASK   = 0x80,
    ENDPOINT_DIR_OUT    = 0x00,
    ENDPOINT_DIR_IN     = 0x80,
    EP_TYPE_MASK        = 0x03,
    EP_TYPE_CONTROL     = 0x00,
    EP_TYPE_ISOCHRONOUS = 0x01,
    EP_TYPE_BULK        = 0x02,
    EP_TYPE_INTERRUPT   = 0x03,
#endif
    USB_MODE_None   = 0,
    USB_MODE_Device = 1,
    USB_MODE_Host   = 2,
    USB_MODE_UID    = 3;

class Serial
{
public:
    static void init();
    static void write(char c);
    static void writeString(const char *s);
};

void Serial::init()
{
    UBRR1 = 103;
    UCSR1B |= 1<<TXEN1;
}

void Serial::write(char c)
{
    while ((UCSR1A & 1<<UDRE1) == 0)
        ;

    UDR1 = c;
}

void Serial::writeString(const char *s)
{
    while (*s)
        write(*s++);
}

static Serial g_serial;

#define USE_STATIC_OPTIONS (USB_DEVICE_OPT_FULLSPEED | USB_OPT_REG_ENABLED | USB_OPT_AUTO_PLL)
#define USB_DEVICE_ONLY
#define USE_FLASH_DESCRIPTORS
#define FIXED_CONTROL_ENDPOINT_SIZE      8



#define SWAPENDIAN_16(x)   (uint16_t)((((x) & 0xFF00) >> 8) | (((x) & 0x00FF) << 8))

#define SWAPENDIAN_32(x) \
  (uint32_t)((((x) & 0xFF000000UL) >> 24UL) | (((x) & 0x00FF0000UL) >> 8UL) | \
                           (((x) & 0x0000FF00UL) << 8UL)  | (((x) & 0x000000FFUL) << 24UL))

#define GCC_MEMORY_BARRIER()                  __asm__ __volatile__("" ::: "memory");
#define GCC_IS_COMPILE_CONST(x)               __builtin_constant_p(x)
#define ATTR_WARN_UNUSED_RESULT      __attribute__ ((warn_unused_result))
#define ATTR_NON_NULL_PTR_ARG(...)   __attribute__ ((nonnull (__VA_ARGS__)))
#define ATTR_PACKED                      __attribute__ ((packed))

static inline uint8_t GetGlobalInterruptMask(void)
{
    GCC_MEMORY_BARRIER();
    return SREG;
}

static inline void SetGlobalInterruptMask(const uint8_t GlobalIntState)
{
    GCC_MEMORY_BARRIER();
    SREG = GlobalIntState;
    GCC_MEMORY_BARRIER();
}

#define MIN(x, y)               (((x) < (y)) ? (x) : (y))
#define STRINGIFY(x)            #x
#define STRINGIFY_EXPANDED(x)   STRINGIFY(x)
#define CONCAT(x, y)            x ## y
#define CONCAT_EXPANDED(x, y)   CONCAT(x, y)
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

struct USB_Request_Header_t
{
    uint8_t  bmRequestType;
    uint8_t  bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} ATTR_PACKED;

static constexpr uint8_t
#ifndef _BUSBY_H_
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
    REQ_SynchFrame          = 12,
#endif
    FEATURE_SEL_EndpointHalt       = 0x00,
#ifndef _BUSBY_H_
    FEATURE_SEL_DeviceRemoteWakeup = 0x01,
#endif
    FEATURE_SEL_TestMode           = 0x02,
    USB_INT_VBUSTI = 0,
    USB_INT_WAKEUPI = 2,
    USB_INT_SUSPI   = 3,
    USB_INT_EORSTI  = 4,
    USB_INT_SOFI    = 5,
    USB_INT_RXSTPI  = 6;

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


static constexpr uint8_t

    DEVICE_STATE_Unattached = 0,
    DEVICE_STATE_Powered = 1,
#ifndef _BUSBY_H_
    DEVICE_STATE_Default = 2,
#endif
    DEVICE_STATE_Addressed = 3,
#ifndef _BUSBY_H_
    DEVICE_STATE_Configured = 4,
#endif
    DEVICE_STATE_Suspended = 5;



static constexpr uint8_t
#ifndef _BUSBY_H_
    USB_CONFIG_ATTR_RESERVED         = 0x80,
    USB_CONFIG_ATTR_SELFPOWERED      = 0x40,
    USB_CONFIG_ATTR_REMOTEWAKEUP     = 0x20,
    ENDPOINT_ATTR_NO_SYNC            = (0 << 2),
    ENDPOINT_ATTR_ASYNC              = (1 << 2),
    ENDPOINT_ATTR_ADAPTIVE           = (2 << 2),
    ENDPOINT_ATTR_SYNC               = (3 << 2),
    ENDPOINT_USAGE_DATA              = (0 << 4),
    ENDPOINT_USAGE_FEEDBACK          = (1 << 4),
    ENDPOINT_USAGE_IMPLICIT_FEEDBACK = (2 << 4),
    DTYPE_Device = 0x01,
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
#endif
    USB_CSCP_NoDeviceClass          = 0x00,
    USB_CSCP_NoDeviceSubclass       = 0x00,
    USB_CSCP_NoDeviceProtocol       = 0x00,
    USB_CSCP_VendorSpecificClass    = 0xFF,
    USB_CSCP_VendorSpecificSubclass = 0xFF,
    USB_CSCP_VendorSpecificProtocol = 0xFF,
    USB_CSCP_IADDeviceClass         = 0xEF,
    USB_CSCP_IADDeviceSubclass      = 0x02,
    USB_CSCP_IADDeviceProtocol      = 0x01;

typedef struct
{
    uint8_t Size;
    uint8_t Type;
} ATTR_PACKED USB_Descriptor_Header_t;

typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
} ATTR_PACKED USB_StdDescriptor_Header_t;

typedef struct
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint16_t bcdUSB;
    uint8_t  bDeviceClass;
    uint8_t  bDeviceSubClass;
    uint8_t  bDeviceProtocol;
    uint8_t  bMaxPacketSize0;
    uint16_t idVendor;
    uint16_t idProduct;
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
    uint8_t  bLength;
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
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bInterfaceNumber;
    uint8_t bAlternateSetting;
    uint8_t bNumEndpoints;
    uint8_t bInterfaceClass;
    uint8_t bInterfaceSubClass;
    uint8_t bInterfaceProtocol;
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
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bFirstInterface;
    uint8_t bInterfaceCount;
    uint8_t bFunctionClass;
    uint8_t bFunctionSubClass;
    uint8_t bFunctionProtocol;
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
    uint8_t  bLength;
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

#define USB_CONFIG_POWER_MA(mA)           ((mA) >> 1)
#define USB_STRING_LEN(UnicodeChars)   (sizeof(USB_Descriptor_Header_t) + ((UnicodeChars) << 1))
#define ARCH_HAS_EEPROM_ADDRESS_SPACE
#define ARCH_HAS_FLASH_ADDRESS_SPACE
#define ARCH_HAS_MULTI_ADDRESS_SPACE
#define ARCH_LITTLE_ENDIAN
#define ENDPOINT_EPNUM_MASK 0x0F
#define ENDPOINT_CONTROLEP 0

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

bool Endpoint_ConfigureEndpoint_Prv(const uint8_t Number,
                                                const uint8_t UECFG0XData,
                                                const uint8_t UECFG1XData);

#define ENDPOINT_CONTROLEP_DEFAULT_SIZE     8
#define ENDPOINT_TOTAL_ENDPOINTS        7

#ifndef _BUSBY_H_
static constexpr uint8_t
    ENDPOINT_READYWAIT_NoError = 0,
    ENDPOINT_READYWAIT_EndpointStalled = 1,
    ENDPOINT_READYWAIT_DeviceDisconnected      = 2,
    ENDPOINT_READYWAIT_BusSuspended            = 3,
    ENDPOINT_READYWAIT_Timeout                 = 4;
#endif

#define FEATURE_SELFPOWERED_ENABLED     (1 << 0)
#define FEATURE_REMOTE_WAKEUP_ENABLED   (1 << 1)


static void USB_DeviceTask(void);

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

static inline uint16_t Endpoint_BytesInEndpoint(void)
{
    return (((uint16_t)UEBCHX << 8) | UEBCLX);
}

static inline uint8_t Endpoint_GetEndpointDirection(void)
{
    return (UECFG0X & (1 << EPDIR)) ? ENDPOINT_DIR_IN : ENDPOINT_DIR_OUT;
}

static inline uint8_t Endpoint_GetCurrentEndpoint(void)
{
    return ((UENUM & ENDPOINT_EPNUM_MASK) | Endpoint_GetEndpointDirection());
}

static inline void Endpoint_SelectEndpoint(const uint8_t Address)
{
    UENUM = (Address & ENDPOINT_EPNUM_MASK);
}

static inline void Endpoint_ResetEndpoint(const uint8_t Address)
{
    UERST = (1 << (Address & ENDPOINT_EPNUM_MASK));
    UERST = 0;
}

static inline bool Endpoint_IsEnabled(void)
{
    return ((UECONX & (1 << EPEN)) ? true : false);
}

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

static inline bool Endpoint_IsReadWriteAllowed(void)
{
    return ((UEINTX & (1 << RWAL)) ? true : false);
}

static inline bool Endpoint_IsConfigured(void)
{
    return ((UESTA0X & (1 << CFGOK)) ? true : false);
}

static inline uint8_t Endpoint_GetEndpointInterrupts(void)
{
    return UEINT;
}

static inline bool Endpoint_HasEndpointInterrupted(const uint8_t Address)
{
    return ((Endpoint_GetEndpointInterrupts() &
        (1 << (Address & ENDPOINT_EPNUM_MASK))) ? true : false);
}

static inline bool Endpoint_IsINReady(void)
{
    return ((UEINTX & (1 << TXINI)) ? true : false);
}

static inline bool Endpoint_IsOUTReceived(void)
{
    return ((UEINTX & (1 << RXOUTI)) ? true : false);
}

static inline bool Endpoint_IsSETUPReceived(void)
{
    return ((UEINTX & (1 << RXSTPI)) ? true : false);
}

static inline void Endpoint_ClearSETUP(void)
{
    UEINTX &= ~(1 << RXSTPI);
}

static inline void Endpoint_ClearIN(void)
{
    UEINTX &= ~((1 << TXINI) | (1 << FIFOCON));
}

static inline void Endpoint_ClearOUT(void)
{
    UEINTX &= ~((1 << RXOUTI) | (1 << FIFOCON));
}

static inline void Endpoint_ClearStall(void)
{
    UECONX |= (1 << STALLRQC);
}

static inline bool Endpoint_IsStalled(void)
{
    return ((UECONX & (1 << STALLRQ)) ? true : false);
}

static inline void Endpoint_SetEndpointDirection(const uint8_t DirectionMask)
{
    UECFG0X = ((UECFG0X & ~(1 << EPDIR)) | (DirectionMask ? (1 << EPDIR) : 0));
}

static inline uint8_t Endpoint_Read_8(void)
{
    return UEDATX;
}

static inline void Endpoint_Write_8(const uint8_t Data)
{
    UEDATX = Data;
}

static inline void Endpoint_Discard_8(void)
{
    uint8_t Dummy;
    Dummy = UEDATX;
    (void)Dummy;
}

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

static inline void Endpoint_Write_16_LE(const uint16_t Data)
{
    UEDATX = (Data & 0xFF);
    UEDATX = (Data >> 8);
}

static inline void Endpoint_Write_16_BE(const uint16_t Data)
{
    UEDATX = (Data >> 8);
    UEDATX = (Data & 0xFF);
}

static inline void Endpoint_Discard_16(void)
{
    uint8_t Dummy;
    Dummy = UEDATX;
    Dummy = UEDATX;
    (void)Dummy;
}

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

static inline void Endpoint_Write_32_LE(const uint32_t Data)
{
    UEDATX = (Data &  0xFF);
    UEDATX = (Data >> 8);
    UEDATX = (Data >> 16);
    UEDATX = (Data >> 24);
}

static inline void Endpoint_Write_32_BE(const uint32_t Data)
{
    UEDATX = (Data >> 24);
    UEDATX = (Data >> 16);
    UEDATX = (Data >> 8);
    UEDATX = (Data &  0xFF);
}

static inline void Endpoint_Discard_32(void)
{
    uint8_t Dummy;
    Dummy = UEDATX;
    Dummy = UEDATX;
    Dummy = UEDATX;
    Dummy = UEDATX;
    (void)Dummy;
}

#define USB_Device_ControlEndpointSize FIXED_CONTROL_ENDPOINT_SIZE

#define USB_DEVICE_OPT_LOWSPEED            (1 << 0)
#define USB_DEVICE_OPT_FULLSPEED               (0 << 0)

#define USE_INTERNAL_SERIAL            0xDC
#define INTERNAL_SERIAL_LENGTH_BITS    80
#define INTERNAL_SERIAL_START_ADDRESS  0x0E


void USB_Device_SendRemoteWakeup(void);

static inline uint16_t USB_Device_GetFrameNumber(void)
{
    return UDFNUM;
}

static inline void USB_Device_EnableSOFEvents(void)
{
    USB_INT_Enable(USB_INT_SOFI);
}

static inline void USB_Device_DisableSOFEvents(void)
{
    USB_INT_Disable(USB_INT_SOFI);
}

static inline void USB_Device_SetLowSpeed(void)
{
    UDCON |= 1<<LSM;
}

static inline void USB_Device_SetFullSpeed(void)
{
    UDCON &= ~(1 << LSM);
}

static inline void USB_Device_SetDeviceAddress(const uint8_t Address)
{
    UDADDR = (UDADDR & (1 << ADDEN)) | (Address & 0x7F);
}

static inline void USB_Device_EnableDeviceAddress(const uint8_t Address)
{
    (void)Address;
    UDADDR |= (1 << ADDEN);
}

static inline bool USB_Device_IsAddressSet(void)
{
    return (UDADDR & (1 << ADDEN));
}

static inline void USB_Device_GetSerialString(uint16_t* const UnicodeString)
{
    uint8_t CurrentGlobalInt = GetGlobalInterruptMask();
    sei();
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

        UnicodeString[SerialCharNum] = (SerialByte >= 10) ?
                (('A' - 10) + SerialByte) : ('0' + SerialByte);
    }

    SetGlobalInterruptMask(CurrentGlobalInt);
}

static constexpr uint8_t
    MEMSPACE_FLASH = 0,
    MEMSPACE_EEPROM = 1,
    MEMSPACE_RAM = 2;


void USB_Device_ProcessControlRequest(void);


#define USB_PLL_PSC                (1 << PINDIV)
#define USB_OPT_REG_DISABLED               (1 << 1)
#define USB_OPT_REG_ENABLED                (0 << 1)
#define USB_OPT_REG_KEEP_ENABLED           (1 << 3)
#define USB_OPT_MANUAL_PLL                 (1 << 2)
#define USB_OPT_AUTO_PLL                   (0 << 2)
#define USB_STREAM_TIMEOUT_MS       100

static inline bool USB_VBUS_GetStatus(void)
{
    return ((USBSTA & (1 << VBUS)) ? true : false);
}

static inline void USB_Attach(void)
{
    UDCON  &= ~(1 << DETACH);
}

void USB_Disable(void);
void USB_ResetInterface(void);

#define USB_CurrentMode USB_MODE_Device
#define USB_Options USE_STATIC_OPTIONS

static inline void USB_PLL_On(void)
{
    PLLCSR = USB_PLL_PSC;
    PLLCSR = (USB_PLL_PSC | (1 << PLLE));
}

static inline bool USB_PLL_IsReady(void)
{
    return ((PLLCSR & (1 << PLOCK)) ? true : false);
}

static inline void USB_OTGPAD_Off(void)
{
    USBCON &= ~(1 << OTGPADE);
}

static inline void USB_CLK_Freeze(void)
{
    USBCON |=  (1 << FRZCLK);
}

static inline void USB_CLK_Unfreeze(void)
{
    USBCON &= ~(1 << FRZCLK);
}

static inline void USB_Controller_Enable(void)
{
    USBCON |=  (1 << USBE);
}

static inline void USB_Controller_Disable(void)
{
    USBCON &= ~(1 << USBE);
}

static inline void USB_Controller_Reset(void)
{
    USBCON &= ~(1 << USBE);
    USBCON |=  (1 << USBE);
}

#define USB_HOST_TIMEOUT_MS                1000

static constexpr uint8_t
    HOST_SENDCONTROL_Successful         = 0,
    HOST_SENDCONTROL_DeviceDisconnected = 1,
    HOST_SENDCONTROL_PipeError          = 2,
    HOST_SENDCONTROL_SetupStalled       = 3,
    HOST_SENDCONTROL_SoftwareTimeOut    = 4;

void Endpoint_ClearStatusStage(void);
uint8_t Endpoint_WaitUntilReady(void);

uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
           const uint16_t wIndex,
       const void** const DescriptorAddress) ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);


static constexpr uint8_t
    USB_HOST_WAITFOR_SetupSent = 0,
    USB_HOST_WAITFOR_InReceived = 1,
    USB_HOST_WAITFOR_OutReady = 2;

#define DESCRIPTOR_PCAST(DescriptorPtr, Type) ((Type*)(DescriptorPtr))
#define DESCRIPTOR_CAST(DescriptorPtr, Type)  (*DESCRIPTOR_PCAST(DescriptorPtr, Type))

#define DESCRIPTOR_TYPE(DescriptorPtr) \
   DESCRIPTOR_PCAST(DescriptorPtr, USB_Descriptor_Header_t)->Type

#define DESCRIPTOR_SIZE(DescriptorPtr) \
   DESCRIPTOR_PCAST(DescriptorPtr, USB_Descriptor_Header_t)->Size

typedef uint8_t (* ConfigComparatorPtr_t)(void*);

static constexpr uint8_t
    HOST_GETCONFIG_Successful       = 0,
    HOST_GETCONFIG_DeviceDisconnect = 1,
    HOST_GETCONFIG_PipeError        = 2,
    HOST_GETCONFIG_SetupStalled     = 3,
    HOST_GETCONFIG_SoftwareTimeOut  = 4,
    HOST_GETCONFIG_BuffOverflow = 5,
    HOST_GETCONFIG_InvalidData = 6,
    DESCRIPTOR_SEARCH_Found = 0,
    DESCRIPTOR_SEARCH_Fail = 1,
    DESCRIPTOR_SEARCH_NotFound = 2,
    DESCRIPTOR_SEARCH_COMP_Found = 0,
    DESCRIPTOR_SEARCH_COMP_Fail = 1,
    DESCRIPTOR_SEARCH_COMP_EndOfDescriptor = 2;

static inline void USB_GetNextDescriptor(uint16_t* const BytesRem, void** CurrConfigLoc)
{
    uint16_t CurrDescriptorSize = DESCRIPTOR_CAST(*CurrConfigLoc, USB_Descriptor_Header_t).Size;

    if (*BytesRem < CurrDescriptorSize)
        CurrDescriptorSize = *BytesRem;

    *CurrConfigLoc  = (void*)((uintptr_t)*CurrConfigLoc + CurrDescriptorSize);
    *BytesRem      -= CurrDescriptorSize;
}


static volatile bool        USB_IsInitialized;
static USB_Request_Header_t USB_ControlRequest;
static volatile uint8_t     USB_DeviceState;



void USB_USBTask(void)
{
    USB_DeviceTask();
}

static void USB_DeviceTask(void)
{
    if (USB_DeviceState == DEVICE_STATE_Unattached)
        return;

    uint8_t PrevEndpoint = Endpoint_GetCurrentEndpoint();
    Endpoint_SelectEndpoint(0);

    if (Endpoint_IsSETUPReceived())
        USB_Device_ProcessControlRequest();

    Endpoint_SelectEndpoint(PrevEndpoint);
}

uint8_t USB_Host_GetDeviceStatus(uint8_t* const FeatureStatus) ATTR_NON_NULL_PTR_ARG(1);
uint8_t USB_Host_ClearEndpointStall(const uint8_t EndpointAddress);
uint8_t USB_Host_SetInterfaceAltSetting(const uint8_t InterfaceIndex, const uint8_t AltSetting);

static constexpr uint8_t
#ifndef _BUSBY_H_
    ENDPOINT_RWSTREAM_NoError = 0,
    ENDPOINT_RWSTREAM_EndpointStalled    = 1,
    ENDPOINT_RWSTREAM_DeviceDisconnected = 2,
    ENDPOINT_RWSTREAM_BusSuspended       = 3,
    ENDPOINT_RWSTREAM_Timeout            = 4,
    ENDPOINT_RWSTREAM_IncompleteTransfer = 5,
    ENDPOINT_RWCSTREAM_NoError            = 0,
    ENDPOINT_RWCSTREAM_HostAborted        = 1,
    ENDPOINT_RWCSTREAM_DeviceDisconnected = 2,
    ENDPOINT_RWCSTREAM_BusSuspended       = 3,
#endif
    CDC_CONTROL_LINE_OUT_DTR =        (1 << 0),
    CDC_CONTROL_LINE_OUT_RTS =        (1 << 1),
    CDC_CONTROL_LINE_IN_DCD =         (1 << 0),
    CDC_CONTROL_LINE_IN_DSR =         (1 << 1),
    CDC_CONTROL_LINE_IN_BREAK =       (1 << 2),
    CDC_CONTROL_LINE_IN_RING =        (1 << 3),
    CDC_CONTROL_LINE_IN_FRAMEERROR =  (1 << 4),
    CDC_CONTROL_LINE_IN_PARITYERROR = (1 << 5),
    CDC_CONTROL_LINE_IN_OVERRUNERROR = (1 << 6);

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

static constexpr uint8_t
    CDC_NOTIF_SerialState = 0x20,
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
    CDC_LINEENCODING_OneStopBit          = 0,
    CDC_LINEENCODING_OneAndAHalfStopBits = 1,
    CDC_LINEENCODING_TwoStopBits         = 2,
    CDC_PARITY_None  = 0,
    CDC_PARITY_Odd   = 1,
    CDC_PARITY_Even  = 2,
    CDC_PARITY_Mark  = 3,
    CDC_PARITY_Space = 4;

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
    uint8_t bFunctionLength;
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

struct USB_CDC_StdDescriptor_FunctionalUnion_t
{
    uint8_t bFunctionLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubType;
    uint8_t bMasterInterface;
    uint8_t bSlaveInterface0;
} ATTR_PACKED;

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

static constexpr uint32_t
    REMOTE_NDIS_PACKET_MSG               = 0x00000001UL,
    REMOTE_NDIS_INITIALIZE_MSG           = 0x00000002UL,
    REMOTE_NDIS_HALT_MSG = 0x00000003,
    REMOTE_NDIS_QUERY_MSG               = 0x00000004,
    REMOTE_NDIS_SET_MSG                 = 0x00000005,
    REMOTE_NDIS_RESET_MSG               = 0x00000006,
    REMOTE_NDIS_INDICATE_STATUS_MSG     = 0x00000007,
    REMOTE_NDIS_KEEPALIVE_MSG = 0x00000008,
    REMOTE_NDIS_INITIALIZE_CMPLT = 0x80000002,
    REMOTE_NDIS_QUERY_CMPLT = 0x80000004,
    REMOTE_NDIS_SET_CMPLT = 0x80000005,
    REMOTE_NDIS_RESET_CMPLT = 0x80000006,
    REMOTE_NDIS_KEEPALIVE_CMPLT = 0x80000008,
    REMOTE_NDIS_STATUS_SUCCESS = 0x00000000,
    REMOTE_NDIS_STATUS_FAILURE = 0xC0000001,
    REMOTE_NDIS_STATUS_INVALID_DATA = 0xC0010015,
    REMOTE_NDIS_STATUS_NOT_SUPPORTED = 0xC00000BB,
    REMOTE_NDIS_STATUS_MEDIA_CONNECT = 0x4001000B,
    REMOTE_NDIS_STATUS_MEDIA_DISCONNECT = 0x4001000C,
    REMOTE_NDIS_MEDIA_STATE_CONNECTED    = 0x00000000,
    REMOTE_NDIS_MEDIA_STATE_DISCONNECTED = 0x00000001,
    REMOTE_NDIS_MEDIUM_802_3             = 0x00000000,
    REMOTE_NDIS_DF_CONNECTIONLESS        = 0x00000001,
    REMOTE_NDIS_DF_CONNECTION_ORIENTED   = 0x00000002,
    REMOTE_NDIS_PACKET_DIRECTED          = 0x00000001,
    REMOTE_NDIS_PACKET_MULTICAST         = 0x00000002,
    REMOTE_NDIS_PACKET_ALL_MULTICAST     = 0x00000004,
    REMOTE_NDIS_PACKET_BROADCAST         = 0x00000008,
    REMOTE_NDIS_PACKET_SOURCE_ROUTING    = 0x00000010,
    REMOTE_NDIS_PACKET_PROMISCUOUS       = 0x00000020,
    REMOTE_NDIS_PACKET_SMT               = 0x00000040,
    REMOTE_NDIS_PACKET_ALL_LOCAL         = 0x00000080,
    REMOTE_NDIS_PACKET_GROUP             = 0x00001000,
    REMOTE_NDIS_PACKET_ALL_FUNCTIONAL    = 0x00002000,
    REMOTE_NDIS_PACKET_FUNCTIONAL        = 0x00004000,
    REMOTE_NDIS_PACKET_MAC_FRAME         = 0x00008000,
    OID_GEN_SUPPORTED_LIST               = 0x00010101,
    OID_GEN_HARDWARE_STATUS              = 0x00010102,
    OID_GEN_MEDIA_SUPPORTED              = 0x00010103,
    OID_GEN_MEDIA_IN_USE                 = 0x00010104,
    OID_GEN_MAXIMUM_FRAME_SIZE           = 0x00010106,
    OID_GEN_LINK_SPEED                   = 0x00010107,
    OID_GEN_TRANSMIT_BLOCK_SIZE          = 0x0001010A,
    OID_GEN_RECEIVE_BLOCK_SIZE           = 0x0001010B,
    OID_GEN_VENDOR_ID                    = 0x0001010C,
    OID_GEN_VENDOR_DESCRIPTION           = 0x0001010D,
    OID_GEN_CURRENT_PACKET_FILTER        = 0x0001010E,
    OID_GEN_MAXIMUM_TOTAL_SIZE           = 0x00010111,
    OID_GEN_MEDIA_CONNECT_STATUS         = 0x00010114,
    OID_GEN_PHYSICAL_MEDIUM              = 0x00010202,
    OID_GEN_XMIT_OK                      = 0x00020101,
    OID_GEN_RCV_OK                       = 0x00020102,
    OID_GEN_XMIT_ERROR                   = 0x00020103,
    OID_GEN_RCV_ERROR                    = 0x00020104,
    OID_GEN_RCV_NO_BUFFER                = 0x00020105,
    OID_802_3_PERMANENT_ADDRESS          = 0x01010101,
    OID_802_3_CURRENT_ADDRESS            = 0x01010102,
    OID_802_3_MULTICAST_LIST             = 0x01010103,
    OID_802_3_MAXIMUM_LIST_SIZE          = 0x01010104,
    OID_802_3_RCV_ERROR_ALIGNMENT        = 0x01020101,
    OID_802_3_XMIT_ONE_COLLISION         = 0x01020102,
    OID_802_3_XMIT_MORE_COLLISIONS       = 0x01020103;

static constexpr uint8_t
    ETHERNET_FRAME_SIZE_MAX = 200,
    RNDIS_REQ_SendEncapsulatedCommand = 0x00,
    RNDIS_REQ_GetEncapsulatedResponse = 0x01,
    RNDIS_Uninitialized    = 0,
    RNDIS_Initialized      = 1,
    RNDIS_Data_Initialized = 2,
    RNDIS_NOTIF_ResponseAvailable = 0x01;

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
    uint32_t MessageType;
    uint32_t MessageLength;
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

static constexpr uint16_t
    ETHERTYPE_IPV4             = 0x0800,
    ETHERTYPE_ARP              = 0x0806,
    ETHERTYPE_RARP             = 0x8035,
    ETHERTYPE_APPLETALK        = 0x809b,
    ETHERTYPE_APPLETALKARP     = 0x80f3,
    ETHERTYPE_IEEE8021Q        = 0x8100,
    ETHERTYPE_NOVELLIPX        = 0x8137,
    ETHERTYPE_NOVELL           = 0x8138,
    ETHERTYPE_IPV6             = 0x86DD,
    ETHERTYPE_COBRANET         = 0x8819,
    ETHERTYPE_PROVIDERBRIDGING = 0x88a8,
    ETHERTYPE_MPLSUNICAST      = 0x8847,
    ETHERTYPE_MPLSMULTICAST    = 0x8848,
    ETHERTYPE_PPPoEDISCOVERY   = 0x8863,
    ETHERTYPE_PPPoESESSION     = 0x8864,
    ETHERTYPE_EAPOVERLAN       = 0x888E,
    ETHERTYPE_HYPERSCSI        = 0x889A,
    ETHERTYPE_ATAOVERETHERNET  = 0x88A2,
    ETHERTYPE_ETHERCAT         = 0x88A4,
    ETHERTYPE_SERCOSIII        = 0x88CD,
    ETHERTYPE_CESoE            = 0x88D8,
    ETHERTYPE_MACSECURITY      = 0x88E5,
    ETHERTYPE_FIBRECHANNEL     = 0x8906,
    ETHERTYPE_QINQ             = 0x9100,
    ETHERTYPE_VLLT             = 0xCAFE;

#define PROTOCOL_ICMP                    1
#define PROTOCOL_IGMP                    2
#define PROTOCOL_TCP                     6
#define PROTOCOL_UDP                     17
#define PROTOCOL_OSPF                    89
#define PROTOCOL_SCTP                    132



static const MAC_Address_t PROGMEM AdapterMACAddress     = {ADAPTER_MAC_ADDRESS};
static const char PROGMEM AdapterVendorDescription[]     = "LUFA RNDIS Adapter";

static uint8_t CurrRNDISState = RNDIS_Uninitialized;
static uint32_t CurrPacketFilter = 0;

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

uint8_t RNDIS_Device_ReadPacket(USB_ClassInfo_RNDIS_Device_t* const RNDISInterfaceInfo,
                                            void* Buffer,
                                            uint16_t* const PacketLength) ATTR_NON_NULL_PTR_ARG(1);

uint8_t RNDIS_Device_SendPacket(USB_ClassInfo_RNDIS_Device_t* const RNDISInterfaceInfo,
                                            void* Buffer,
                                            const uint16_t PacketLength) ATTR_NON_NULL_PTR_ARG(1);

#define RNDIS_DEVICE_MIN_MESSAGE_BUFFER_LENGTH  sizeof(AdapterSupportedOIDList) + sizeof(RNDIS_Query_Complete_t)



#define TEMPLATE_BUFFER_MOVE(BufferPtr, Amount)   BufferPtr += Amount
#define TEMPLATE_TRANSFER_BYTE(BufferPtr)         Endpoint_Write_8(*BufferPtr)

uint8_t Endpoint_Write_Stream_LE(const void * const Buffer,
                            uint16_t Length,
                            uint16_t* const BytesProcessed)
{
    uint8_t* DataStream = (uint8_t*)Buffer;
    uint16_t BytesInTransfer = 0;
    uint8_t  ErrorCode;

    if ((ErrorCode = Endpoint_WaitUntilReady()))
      return ErrorCode;

    if (BytesProcessed != NULL)
    {
        Length -= *BytesProcessed;
        DataStream += *BytesProcessed;
    }

    while (Length)
    {
        if (!(Endpoint_IsReadWriteAllowed()))
        {
            Endpoint_ClearIN();
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

#undef TEMPLATE_TRANSFER_BYTE
#undef TEMPLATE_BUFFER_MOVE

#define TEMPLATE_BUFFER_TYPE                      void*
#define TEMPLATE_CLEAR_ENDPOINT()                 Endpoint_ClearOUT()
#define TEMPLATE_BUFFER_OFFSET(Length)            0
#define TEMPLATE_BUFFER_MOVE(BufferPtr, Amount)   BufferPtr += Amount
#define TEMPLATE_TRANSFER_BYTE(BufferPtr)         *BufferPtr = Endpoint_Read_8()

uint8_t Endpoint_Read_Stream_LE(void * const Buffer, uint16_t Length,
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
            Endpoint_ClearOUT();
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

#if defined(ARCH_HAS_FLASH_ADDRESS_SPACE)
#define TEMPLATE_FUNC_NAME                        Endpoint_Write_PStream_LE
#define TEMPLATE_BUFFER_TYPE                      const void*
#define TEMPLATE_CLEAR_ENDPOINT()                 Endpoint_ClearIN()
#define TEMPLATE_BUFFER_OFFSET(Length)            0
#define TEMPLATE_BUFFER_MOVE(BufferPtr, Amount)   BufferPtr += Amount
#define TEMPLATE_TRANSFER_BYTE(BufferPtr)         Endpoint_Write_8(pgm_read_byte(BufferPtr))

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

#define  TEMPLATE_FUNC_NAME                        Endpoint_Read_EStream_BE
#define  TEMPLATE_BUFFER_TYPE                      void*
#define  TEMPLATE_CLEAR_ENDPOINT()                 Endpoint_ClearOUT()
#define  TEMPLATE_BUFFER_OFFSET(Length)            (Length - 1)
#define  TEMPLATE_BUFFER_MOVE(BufferPtr, Amount)   BufferPtr -= Amount
#define  TEMPLATE_TRANSFER_BYTE(BufferPtr)  eeprom_update_byte(BufferPtr, Endpoint_Read_8())

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

#define  TEMPLATE_BUFFER_MOVE(BufferPtr, Amount)   BufferPtr += Amount
#define  TEMPLATE_TRANSFER_BYTE(BufferPtr)         Endpoint_Write_8(*BufferPtr)

uint8_t Endpoint_Write_Control_Stream_LE(const void* const Buffer, uint16_t Length)
{
    uint8_t* DataStream     = (uint8_t*)Buffer;
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
        if (USB_DeviceState_LCL == DEVICE_STATE_Suspended)
            return ENDPOINT_RWCSTREAM_BusSuspended;
        if (Endpoint_IsSETUPReceived())
            return ENDPOINT_RWCSTREAM_HostAborted;
        if (Endpoint_IsOUTReceived())
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


#define  TEMPLATE_BUFFER_OFFSET(Length)            0
#define  TEMPLATE_BUFFER_MOVE(BufferPtr, Amount)   BufferPtr += Amount
#define  TEMPLATE_TRANSFER_BYTE(BufferPtr)         *BufferPtr = Endpoint_Read_8()

uint8_t Endpoint_Read_Control_Stream_LE(void* const Buffer,
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
#undef TEMPLATE_TRANSFER_BYTE

#if defined(ARCH_HAS_FLASH_ADDRESS_SPACE)
#define  TEMPLATE_FUNC_NAME                        Endpoint_Write_Control_PStream_LE
#define  TEMPLATE_BUFFER_OFFSET(Length)            0
#define  TEMPLATE_BUFFER_MOVE(BufferPtr, Amount)   BufferPtr += Amount
#define  TEMPLATE_TRANSFER_BYTE(BufferPtr)         Endpoint_Write_8(pgm_read_byte(BufferPtr))

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

#define  TEMPLATE_FUNC_NAME                        Endpoint_Write_Control_EStream_LE
#define  TEMPLATE_BUFFER_OFFSET(Length)            0
#define  TEMPLATE_BUFFER_MOVE(BufferPtr, Amount)   BufferPtr += Amount
#define  TEMPLATE_TRANSFER_BYTE(BufferPtr)         Endpoint_Write_8(eeprom_read_byte(BufferPtr))

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


#define  TEMPLATE_FUNC_NAME                        Endpoint_Read_Control_EStream_LE
#define  TEMPLATE_BUFFER_OFFSET(Length)            0
#define  TEMPLATE_BUFFER_MOVE(BufferPtr, Amount)   BufferPtr += Amount
#define  TEMPLATE_TRANSFER_BYTE(BufferPtr) eeprom_update_byte(BufferPtr, Endpoint_Read_8())


uint8_t Endpoint_Read_Control_EStream_LE(void* const Buffer, uint16_t Length)
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

static uint8_t USB_Device_ConfigurationNumber;
static bool    USB_Device_CurrentlySelfPowered;
static bool    USB_Device_RemoteWakeupEnabled;

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
	if ((uint8_t)USB_ControlRequest.wValue > FIXED_NUM_CONFIGURATIONS)
	  return;

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

	SignatureDescriptor.Header.Type = DTYPE_STRING;
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

        UECONX &= ~(1<<EPEN);
        UECFG1X &= ~(1 << ALLOC);
        UECONX |= 1<<EPEN;
        UECFG0X = UECFG0XTemp;
        UECFG1X = UECFG1XTemp;
        UEIENX  = UEIENXTemp;

        if (!(Endpoint_IsConfigured()))
          return false;
    }

    Endpoint_SelectEndpoint(Number);
    return true;
}


void USB_Init(void)
{
    USB_OTGPAD_Off();

    if (!(USB_Options & USB_OPT_REG_DISABLED))
        UHWCON |= 1<<UVREGE;
    else
        UHWCON &= ~(1<<UVREGE);

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
    UDCON |= 1<<DETACH;
    USB_Controller_Disable();

    if (!(USB_Options & USB_OPT_MANUAL_PLL))
        PLLCSR = 0;

    if (!(USB_Options & USB_OPT_REG_KEEP_ENABLED))
        UHWCON &= ~(1<<UVREGE);
        //USB_REG_Off();

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
    Endpoint_ConfigureEndpoint(0, EP_TYPE_CONTROL, USB_Device_ControlEndpointSize, 1);
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
            PLLCSR = 0;
        }

        USB_Init_Device();
    }
    else if (USB_CurrentMode == USB_MODE_Host)
    {
    }

    USBCON |= 1<<OTGPADE;
}


void Endpoint_ClearEndpoints(void)
{
    UEINT = 0;

    for (uint8_t EPNum = 0; EPNum < ENDPOINT_TOTAL_ENDPOINTS; EPNum++)
    {
        Endpoint_SelectEndpoint(EPNum);
        UEIENX  = 0;
        UEINTX  = 0;
        UECFG1X = 0;
        UECONX &= ~(1<<EPEN);
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
            if (USB_DeviceState == DEVICE_STATE_Unattached)
                return;

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

    if (USB_ControlRequest.wValue == ((DTYPE_STRING << 8) | USE_INTERNAL_SERIAL))
    {
        USB_Device_GetInternalSerialDescriptor();
        return;
    }

    if ((DescriptorSize = CALLBACK_USB_GetDescriptor(USB_ControlRequest.wValue,
        USB_ControlRequest.wIndex, &DescriptorPointer)) == 0)
    {
        return;
    }

    Endpoint_ClearSETUP();
    Endpoint_Write_Control_PStream_LE(DescriptorPointer, DescriptorSize);
	Endpoint_ClearOUT();
}

static void USB_Device_ClearSetFeature(void)
{
	switch (USB_ControlRequest.bmRequestType & CONTROL_REQTYPE_RECIPIENT)
	{
		case REQREC_DEVICE:
		{
			if ((uint8_t)USB_ControlRequest.wValue == FEATURE_SEL_DeviceRemoteWakeup)
			  USB_Device_RemoteWakeupEnabled = (USB_ControlRequest.bRequest == REQ_SetFeature);
			else
			  return;
        }
			break;
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
                        UECONX |= 1<<STALLRQ;
					}
					else
					{
						Endpoint_ClearStall();
						Endpoint_ResetEndpoint(EndpointIndex);
                        UECONX |= 1<<RSTDT;
					}
				}
			}

			break;
		}
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
			if (USB_Device_CurrentlySelfPowered)
			  CurrentStatus |= FEATURE_SELFPOWERED_ENABLED;

			if (USB_Device_RemoteWakeupEnabled)
			  CurrentStatus |= FEATURE_REMOTE_WAKEUP_ENABLED;

			break;
		}
		case (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_ENDPOINT):
		{
			uint8_t EndpointIndex = ((uint8_t)USB_ControlRequest.wIndex & ENDPOINT_EPNUM_MASK);

			if (EndpointIndex >= ENDPOINT_TOTAL_ENDPOINTS)
				return;

			Endpoint_SelectEndpoint(EndpointIndex);
			CurrentStatus = Endpoint_IsStalled();
			Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);
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
static RNDIS_Message_Header_t* MessageHeader = (RNDIS_Message_Header_t*)&RNDISMessageBuffer;
static bool ResponseReady = false;

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

static void ProcessRNDISControlMessage(void)
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
        if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS |
            REQREC_INTERFACE))
        {
            Endpoint_ClearSETUP();
            Endpoint_Read_Control_Stream_LE(RNDISMessageBuffer, USB_ControlRequest.wLength);
            Endpoint_ClearIN();
            ProcessRNDISControlMessage();
        }

        break;
    case RNDIS_REQ_GetEncapsulatedResponse:
        if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS |
                REQREC_INTERFACE))
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
			case REQ_GETSTATUS:
				if ((bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_DEVICE)) ||
					(bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_ENDPOINT)))
				{
					USB_Device_GetStatus();
				}

				break;
			case REQ_CLEARFEATURE:
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
			case REQ_SETCONFIGURATION:
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
        UECONX |= 1<<STALLRQ;
	}
}

typedef struct
{
    uint8_t Octets[6]; /**< Individual bytes of a MAC address */
} RNDIS_MAC_Address_t;

typedef struct
{
    uint8_t Octets[4]; /**< Individual bytes of an IP address */
} IP_Address_t;

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
#define TCP_PORT_HTTP                   SWAPENDIAN_16(80)
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

#define BROADCAST_MAC_ADDRESS            {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
#define MAC_COMPARE(MAC1, MAC2)          (memcmp(MAC1, MAC2, sizeof(MAC_Address_t)) == 0)
#define ETHERNET_VER2_MINSIZE            0x0600
#define NO_RESPONSE                      0
#define NO_PROCESS                       -1

typedef struct
{
    uint8_t  FrameData[ETHERNET_FRAME_SIZE_MAX]; /**< Ethernet frame contents. */
    uint16_t FrameLength; /**< Length net frame stored in the buffer. */
} Ethernet_Frame_Info_t;

typedef struct
{
    MAC_Address_t Destination;
    MAC_Address_t Source;
    uint16_t      EtherType;
} Ethernet_Frame_Header_t;

void     Ethernet_ProcessPacket(void);
uint16_t Ethernet_Checksum16(void* Data, uint16_t Bytes);

void RNDIS_Task(void);
void Ethernet_Task(void);

static Ethernet_Frame_Info_t FrameIN;
static Ethernet_Frame_Info_t FrameOUT;

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
    INTERFACE_ID_CDC_CCI = 0,
    INTERFACE_ID_CDC_DCI = 1,
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

static const MAC_Address_t ServerMACAddress    = {SERVER_MAC_ADDRESS};
static const IP_Address_t  ServerIPAddress     = {SERVER_IP_ADDRESS};
static const MAC_Address_t BroadcastMACAddress = {BROADCAST_MAC_ADDRESS};
static const IP_Address_t  BroadcastIPAddress  = {BROADCAST_IP_ADDRESS};
static const IP_Address_t  ClientIPAddress     = {CLIENT_IP_ADDRESS};



static int16_t IP_ProcessIPPacket(void* InDataStart, void* OutDataStart)
{
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
    case PROTOCOL_UDP:
        RetSize = UDP_ProcessUDPPacket(InDataStart,
                                       &((uint8_t*)InDataStart)[HeaderLengthBytes],
                                       &((uint8_t*)OutDataStart)[sizeof(IP_Header_t)]);
        break;
    }

    if (RetSize > 0)
    {
        IPHeaderOUT->TotalLength        = SWAPENDIAN_16(sizeof(IP_Header_t) + RetSize);
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
}

void DecodeIPHeader(void* InDataStart)
{
}

void DecodeICMPHeader(void* InDataStart)
{
}

void DecodeUDPHeader(void* InDataStart)
{
}

int16_t ARP_ProcessARPPacket(void* InDataStart,
                             void* OutDataStart)
{
    DecodeARPHeader(InDataStart);

    ARP_Header_t* ARPHeaderIN  = (ARP_Header_t*)InDataStart;
    ARP_Header_t* ARPHeaderOUT = (ARP_Header_t*)OutDataStart;

    /* Ensure that the ARP request is a IPv4 request packet */
    if ((SWAPENDIAN_16(ARPHeaderIN->ProtocolType) == ETHERTYPE_IPV4) &&
        (SWAPENDIAN_16(ARPHeaderIN->Operation) == ARP_OPERATION_REQUEST))
    {
        if (IP_COMPARE(&ARPHeaderIN->TPA, &ServerIPAddress) ||
            MAC_COMPARE(&ARPHeaderIN->THA, &ServerMACAddress))
        {
            /* Fill out the ARP response header */
            ARPHeaderOUT->HardwareType = ARPHeaderIN->HardwareType;
            ARPHeaderOUT->ProtocolType = ARPHeaderIN->ProtocolType;
            ARPHeaderOUT->HLEN         = ARPHeaderIN->HLEN;
            ARPHeaderOUT->PLEN         = ARPHeaderIN->PLEN;
            ARPHeaderOUT->Operation    = SWAPENDIAN_16(ARP_OPERATION_REPLY);

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

    uint8_t* DHCPOptionsINStart = (uint8_t*)((uint8_t *)DHCPHeaderInStart +
        sizeof(DHCP_Header_t));

    uint8_t* DHCPOptionsOUTStart = (uint8_t*)((uint8_t *)DHCPHeaderOutStart +
        sizeof(DHCP_Header_t));

    memset(DHCPHeaderOUT, 0, sizeof(DHCP_Header_t));
    DHCPHeaderOUT->HardwareType          = DHCPHeaderIN->HardwareType;
    DHCPHeaderOUT->Operation             = DHCP_OP_BOOTREPLY;
    DHCPHeaderOUT->HardwareAddressLength = DHCPHeaderIN->HardwareAddressLength;
    DHCPHeaderOUT->Hops                  = 0;
    DHCPHeaderOUT->TransactionID         = DHCPHeaderIN->TransactionID;
    DHCPHeaderOUT->ElapsedSeconds        = 0;
    DHCPHeaderOUT->Flags                 = DHCPHeaderIN->Flags;
    DHCPHeaderOUT->YourIP                = ClientIPAddress;
    memmove(&DHCPHeaderOUT->ClientHardwareAddress, &DHCPHeaderIN->ClientHardwareAddress, sizeof(MAC_Address_t));
    DHCPHeaderOUT->Cookie                = SWAPENDIAN_32(DHCP_MAGIC_COOKIE);

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

int16_t UDP_ProcessUDPPacket(void* IPHeaderInStart, void* UDPHeaderInStart,
                             void* UDPHeaderOutStart)
{
    UDP_Header_t* UDPHeaderIN  = (UDP_Header_t*)UDPHeaderInStart;
    UDP_Header_t* UDPHeaderOUT = (UDP_Header_t*)UDPHeaderOutStart;

    int16_t RetSize = NO_RESPONSE;

    DecodeUDPHeader(UDPHeaderInStart);

    switch (SWAPENDIAN_16(UDPHeaderIN->DestinationPort))
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
        UDPHeaderOUT->Length          = SWAPENDIAN_16(sizeof(UDP_Header_t) + RetSize);

        /* Return the size of the response so far */
        return (sizeof(UDP_Header_t) + RetSize);
    }

    return NO_RESPONSE;
}

int16_t ICMP_ProcessICMPPacket(void* InDataStart, void* OutDataStart)
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

        intptr_t DataSize = FrameIN.FrameLength - ((((intptr_t)InDataStart +
            sizeof(ICMP_Header_t)) - (intptr_t)FrameIN.FrameData));

        memmove(&((uint8_t*)OutDataStart)[sizeof(ICMP_Header_t)],
                &((uint8_t*)InDataStart)[sizeof(ICMP_Header_t)],
                DataSize);

        ICMPHeaderOUT->Checksum = Ethernet_Checksum16(ICMPHeaderOUT,
            (DataSize + sizeof(ICMP_Header_t)));

        return (DataSize + sizeof(ICMP_Header_t));
    }

    return NO_RESPONSE;
}

void Ethernet_ProcessPacket(void)
{
    Ethernet_Frame_Header_t* FrameINHeader  = (Ethernet_Frame_Header_t*)&FrameIN.FrameData;
    Ethernet_Frame_Header_t* FrameOUTHeader = (Ethernet_Frame_Header_t*)&FrameOUT.FrameData;
    int16_t                  RetSize        = NO_RESPONSE;

    if ((MAC_COMPARE(&FrameINHeader->Destination, &ServerMACAddress) ||
         MAC_COMPARE(&FrameINHeader->Destination, &BroadcastMACAddress)) &&
        (SWAPENDIAN_16(FrameIN.FrameLength) > ETHERNET_VER2_MINSIZE))
    {
        switch (SWAPENDIAN_16(FrameINHeader->EtherType))
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

uint16_t Ethernet_Checksum16(void* Data, uint16_t Bytes)
{
    uint16_t* Words    = (uint16_t*)Data;
    uint32_t  Checksum = 0;

    for (uint16_t CurrWord = 0; CurrWord < (Bytes >> 1); CurrWord++)
        Checksum += Words[CurrWord];

    while (Checksum & 0xFFFF0000)
        Checksum = ((Checksum & 0xFFFF) + (Checksum >> 16));

    return ~Checksum;
}

struct USB_Descriptor_Device_t
{
    USB_Descriptor_Header_t Header;
    uint16_t USBSpecification;
    uint8_t  Class;
    uint8_t  SubClass;
    uint8_t  Protocol;
    uint8_t  Endpoint0Size;
    uint16_t VendorID;
    uint16_t ProductID;
    uint16_t ReleaseNumber;
    uint8_t  ManufacturerStrIndex;
    uint8_t  ProductStrIndex;
    uint8_t  SerialNumStrIndex;
    uint8_t  NumberOfConfigurations;
} ATTR_PACKED;

static const USB_Descriptor_Device_t PROGMEM DeviceDescriptor =
{
    {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_DEVICE},
    0x0110,
    CDC_CSCP_CDCClass,
    CDC_CSCP_NoSpecificSubclass,
    CDC_CSCP_NoSpecificProtocol,
    FIXED_CONTROL_ENDPOINT_SIZE,
    0x03EB,
    0x204C,
    0x0001,
    STRING_ID_Manufacturer,
    STRING_ID_Product,
    0,
    FIXED_NUM_CONFIGURATIONS
};

const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor =
{
    .Config =
        {
            .Header =
            {
                .Size = sizeof(USB_Descriptor_Configuration_Header_t),
                .Type = DTYPE_CONFIGURATION
            },

            .TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
            .TotalInterfaces        = 2,
            .ConfigurationNumber    = 1,
            .ConfigurationStrIndex  = 0,
            .ConfigAttributes       = (USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_SELFPOWERED),
            .MaxPowerConsumption    = USB_CONFIG_POWER_MA(100)
        },

    .CDC_CCI_Interface =
        {
            .Header = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_INTERFACE},

            .InterfaceNumber        = INTERFACE_ID_CDC_CCI,
            .AlternateSetting       = 0,

            .TotalEndpoints         = 1,
            .Class                  = CDC_CSCP_CDCClass,
            .SubClass               = CDC_CSCP_ACMSubclass,
            .Protocol               = CDC_CSCP_VendorSpecificProtocol,
            .InterfaceStrIndex      = 0
        },

    .CDC_Functional_Header =
        {
            .Header =
            {
                .Size = sizeof(USB_CDC_Descriptor_FunctionalHeader_t),
                .Type = DTYPE_CSINTERFACE
            },
            .Subtype                = CDC_DSUBTYPE_CSInterface_Header,

            .CDCSpecification       = 0x0110,
        },

    .CDC_Functional_ACM =
        {
            .Header =
            {.Size = sizeof(USB_CDC_Descriptor_FunctionalACM_t), .Type = DTYPE_CSINTERFACE},
            .Subtype                = CDC_DSUBTYPE_CSInterface_ACM,

            .Capabilities           = 0x00,
        },

    .CDC_Functional_Union =
        {
            .Header =
            {.Size = sizeof(USB_CDC_Descriptor_FunctionalUnion_t), .Type = DTYPE_CSINTERFACE},
            .Subtype                = CDC_DSUBTYPE_CSInterface_Union,

            .MasterInterfaceNumber  = INTERFACE_ID_CDC_CCI,
            .SlaveInterfaceNumber   = INTERFACE_ID_CDC_DCI,
        },
    .CDC_NotificationEndpoint =
        {
            .Header = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_ENDPOINT},

            .EndpointAddress        = CDC_NOTIFICATION_EPADDR,
            .Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
            .EndpointSize           = CDC_NOTIFICATION_EPSIZE,
            .PollingIntervalMS      = 0xFF
        },

    .CDC_DCI_Interface =
        {
            .Header = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_INTERFACE},

            .InterfaceNumber        = INTERFACE_ID_CDC_DCI,
            .AlternateSetting       = 0,

            .TotalEndpoints         = 2,

            .Class                  = CDC_CSCP_CDCDataClass,
            .SubClass               = CDC_CSCP_NoDataSubclass,
            .Protocol               = CDC_CSCP_NoDataProtocol,

            .InterfaceStrIndex      = 0
        },

    .RNDIS_DataOutEndpoint =
        {
            .Header = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_ENDPOINT},

            .EndpointAddress        = CDC_RX_EPADDR,
            .Attributes             = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
            .EndpointSize           = CDC_TXRX_EPSIZE,
            .PollingIntervalMS      = 0x05
        },

    .RNDIS_DataInEndpoint =
        {
            .Header = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_ENDPOINT},

            .EndpointAddress        = CDC_TX_EPADDR,
            .Attributes             = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
            .EndpointSize           = CDC_TXRX_EPSIZE,
            .PollingIntervalMS      = 0x05
        }
};

#ifndef _BUSBY_H_
template <size_t S> struct DescString
{
    uint8_t size;
    uint8_t type;
    wchar_t UnicodeString[S];
};
#endif

static const DescString<2> PROGMEM LanguageString =
{
    USB_STRING_LEN(1),
    DTYPE_STRING,
    (wchar_t)0x0409
};

static const DescString<12> PROGMEM ManufacturerString =
{
    USB_STRING_LEN(11),
    DTYPE_STRING,
    L"Dean Camera"
};

static const DescString<16> PROGMEM ProductString =
{
    USB_STRING_LEN(15),
    DTYPE_STRING,
    L"LUFA RNDIS Demo"
};

uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                    const uint16_t wIndex,
                                    const void** const DescriptorAddress)
{
    const uint8_t  DescriptorType   = (wValue >> 8);
    const uint8_t  DescriptorNumber = (wValue & 0xFF);

    const void* Address = NULL;
    uint16_t    Size    = 0;

    switch (DescriptorType)
    {
    case DTYPE_DEVICE:
        Address = &DeviceDescriptor;
        Size    = sizeof(USB_Descriptor_Device_t);
        break;
    case DTYPE_CONFIGURATION:
        Address = &ConfigurationDescriptor;
        Size    = sizeof(USB_Descriptor_Configuration_t);
        break;
    case DTYPE_STRING:
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
    g_serial.init();
    g_serial.writeString("Startup\r\n");
    USB_Init();
	sei();

	for (;;)
	{
        Ethernet_Task();
        RNDIS_Task();
        USB_USBTask();
	}
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
                UECONX |= 1<<STALLRQ;
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

			RNDISPacketHeader.MessageLength = (sizeof(RNDIS_Packet_Message_t) +
                FrameOUT.FrameLength);

			RNDISPacketHeader.DataOffset = (sizeof(RNDIS_Packet_Message_t) -
                sizeof(RNDIS_Message_Header_t));

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
        }
        else
        {
            if (!(USB_Options & USB_OPT_MANUAL_PLL))
                PLLCSR = 0;

            USB_DeviceState = DEVICE_STATE_Unattached;
        }
    }

    if (USB_INT_HasOccurred(USB_INT_SUSPI) && USB_INT_IsEnabled(USB_INT_SUSPI))
    {
        USB_INT_Disable(USB_INT_SUSPI);
        USB_INT_Enable(USB_INT_WAKEUPI);
        USB_CLK_Freeze();

        if (!(USB_Options & USB_OPT_MANUAL_PLL))
            PLLCSR = 0;

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
            USB_DeviceState = USB_Device_IsAddressSet() ? DEVICE_STATE_Addressed :
                DEVICE_STATE_Powered;

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
    sei();
    USB_Device_ProcessControlRequest();
    Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);
    USB_INT_Enable(USB_INT_RXSTPI);
    Endpoint_SelectEndpoint(PrevSelectedEndpoint);
}



