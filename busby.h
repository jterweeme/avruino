#ifndef _BUSBY_H_
#define _BUSBY_H_
#include "board.h"

#define USE_LUFA_CONFIG_HEADER
#define F_USB F_CPU
#define USB_SERIES_4_AVR
#define USB_CAN_BE_DEVICE

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/boot.h>
#include <math.h>
#include <util/delay.h>
#include <stdio.h>
#include "misc.h"

static const uint8_t ENDPOINT_DIR_MASK = 0x80;
static const uint8_t ENDPOINT_DIR_OUT = 0x00;
static const uint8_t ENDPOINT_DIR_IN = 0x80;
static const uint8_t PIPE_DIR_MASK = 0x80;
static const uint8_t PIPE_DIR_OUT = 0x00;
static const uint8_t PIPE_DIR_IN = 0x80;
static const uint8_t EP_TYPE_MASK = 0x03;
static const uint8_t EP_TYPE_CONTROL = 0x00;
static const uint8_t EP_TYPE_ISOCHRONOUS = 0x01;
static const uint8_t EP_TYPE_BULK = 0x02;
static const uint8_t EP_TYPE_INTERRUPT = 0x03;
static const uint8_t USB_PLL_PSC = 0;
static const uint8_t USB_OPT_REG_DISABLED = 1 << 1;
static const uint8_t USB_OPT_REG_ENABLED = 0 << 1;
static const uint8_t USB_OPT_MANUAL_PLL = 1 << 2;
static const uint8_t USB_OPT_AUTO_PLL = 0 << 2;
static const uint8_t USB_STREAM_TIMEOUT_MS = 100;

enum USB_Interrupts_t
{
    USB_INT_VBUSTI = 0,
    USB_INT_WAKEUPI = 2,
    USB_INT_SUSPI = 3,
    USB_INT_EORSTI = 4,
    USB_INT_SOFI = 5,
    USB_INT_RXSTPI = 6,
};

enum USB_DescriptorMemorySpaces_t
{
    MEMSPACE_FLASH = 0,
    MEMSPACE_EEPROM = 1,
    MEMSPACE_RAM = 2,
};

extern uint8_t USB_Device_ConfigurationNumber;
extern bool USB_Device_RemoteWakeupEnabled;
extern bool USB_Device_CurrentlySelfPowered;
void USB_Device_ProcessControlRequest();

enum CDC_Descriptor_ClassSubclassProtocol_t
{
    CDC_CSCP_CDCClass = 0x02,
    CDC_CSCP_NoSpecificSubclass = 0x00,
    CDC_CSCP_ACMSubclass = 0x02,
    CDC_CSCP_ATCommandProtocol = 0x01,
    CDC_CSCP_NoSpecificProtocol = 0x00,
    CDC_CSCP_VendorSpecificProtocol = 0xFF,
    CDC_CSCP_CDCDataClass = 0x0A,
    CDC_CSCP_NoDataSubclass = 0x00,
    CDC_CSCP_NoDataProtocol = 0x00,
};

enum CDC_ClassRequests_t
{
    CDC_REQ_SendEncapsulatedCommand = 0x00,
    CDC_REQ_GetEncapsulatedResponse = 0x01,
    CDC_REQ_SetLineEncoding = 0x20,
    CDC_REQ_GetLineEncoding = 0x21,
    CDC_REQ_SetControlLineState = 0x22,
    CDC_REQ_SendBreak = 0x23,
};

enum CDC_ClassNotifications_t
{
    CDC_NOTIF_SerialState = 0x20,
};

enum CDC_DescriptorSubtypes_t
{
    CDC_DSUBTYPE_CSInterface_Header           = 0x00,
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

struct USB_Request_Header_t
{
    uint8_t  bmRequestType;
    uint8_t  bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} __attribute__ ((packed));

enum USB_Control_Request_t
{
    REQ_GetStatus = 0,
    REQ_ClearFeature = 1,
    REQ_SetFeature = 3,
    REQ_SetAddress          = 5,
    REQ_GetDescriptor = 6,
    REQ_SetDescriptor       = 7,
    REQ_GetConfiguration    = 8,
    REQ_SetConfiguration    = 9,
    REQ_GetInterface        = 10,
    REQ_SetInterface        = 11,
    REQ_SynchFrame          = 12,
};

enum USB_Feature_Selectors_t
{
    FEATURE_SEL_EndpointHalt       = 0x00,
    FEATURE_SEL_DeviceRemoteWakeup = 0x01,
    FEATURE_SEL_TestMode = 0x02,
};

#define GCC_MEMORY_BARRIER()                  __asm__ __volatile__("" ::: "memory");
#define GCC_IS_COMPILE_CONST(x)               __builtin_constant_p(x)

#define ATTR_WARN_UNUSED_RESULT      __attribute__ ((warn_unused_result))
#define ATTR_NON_NULL_PTR_ARG(...)   __attribute__ ((nonnull (__VA_ARGS__)))
#define ATTR_ALWAYS_INLINE           __attribute__ ((always_inline))
#define ATTR_CONST                   __attribute__ ((const))
#define ATTR_WEAK                    __attribute__ ((weak))
#define ATTR_NO_INIT                     __attribute__ ((section (".noinit")))
#define ATTR_ALIAS(Func)                 __attribute__ ((alias( #Func )))
#define ATTR_PACKED                      __attribute__ ((packed))

#if defined(USE_LUFA_CONFIG_HEADER)
#define USB_DEVICE_ONLY
#define USE_FLASH_DESCRIPTORS
static const uint8_t FIXED_CONTROL_ENDPOINT_SIZE = 8;
#define DEVICE_STATE_AS_GPIOR            0
static const uint8_t FIXED_NUM_CONFIGURATIONS = 1;
#endif

typedef uint8_t uint_reg_t;

static constexpr uint16_t le16_to_cpu(uint16_t x) { return x; }
static constexpr uint32_t le32_to_cpu(uint32_t x) { return x; }
static constexpr uint16_t cpu_to_le16(uint16_t x) { return x; }
static constexpr uint32_t cpu_to_le32(uint32_t x) { return x; }
static constexpr uint16_t LE16_TO_CPU(uint16_t x) { return x; }
static constexpr uint32_t LE32_TO_CPU(uint32_t x) { return x; }
static constexpr uint16_t CPU_TO_LE16(uint16_t x) { return x; }
static constexpr uint32_t CPU_TO_LE32(uint32_t x) { return x; }

static inline uint16_t SwapEndian_16(const uint16_t Word)
{
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

static inline uint32_t SwapEndian_32(const uint32_t DWord)
{
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

static inline uint8_t BitReverse(uint8_t Byte)
{
    Byte = (((Byte & 0xF0) >> 4) | ((Byte & 0x0F) << 4));
    Byte = (((Byte & 0xCC) >> 2) | ((Byte & 0x33) << 2));
    Byte = (((Byte & 0xAA) >> 1) | ((Byte & 0x55) << 1));
    return Byte;
}

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

static inline uint_reg_t GetGlobalInterruptMask(void)
{
    GCC_MEMORY_BARRIER();
    return SREG;
}

static inline void SetGlobalInterruptMask(const uint_reg_t GlobalIntState)
{
    GCC_MEMORY_BARRIER();
    SREG = GlobalIntState;
    GCC_MEMORY_BARRIER();
}

static inline void GlobalInterruptEnable(void)
{
    GCC_MEMORY_BARRIER();
    sei();
    GCC_MEMORY_BARRIER();
}

static inline void GlobalInterruptDisable(void)
{
    GCC_MEMORY_BARRIER();
    cli();
    GCC_MEMORY_BARRIER();
}

static const uint8_t NO_DESCRIPTOR = 0;
#define USB_CONFIG_POWER_MA(mA)           ((mA) >> 1)
#define USB_STRING_LEN(UnicodeChars) (sizeof(USB_Descriptor_Header_t) + ((UnicodeChars) << 1))

#define VERSION_BCD(x)   CPU_TO_LE16((VERSION_TENS(x) << 12)  | (VERSION_ONES(x) << 8) | \
                              (VERSION_TENTHS(x) << 4) | (VERSION_HUNDREDTHS(x) << 0) )

static const uint16_t LANGUAGE_ID_ENG = 0x0409;
static const uint8_t USB_CONFIG_ATTR_RESERVED = 0x80;
static const uint8_t USB_CONFIG_ATTR_SELFPOWERED = 0x40;
static const uint8_t USB_CONFIG_ATTR_REMOTEWAKEUP = 0x20;
static const uint8_t ENDPOINT_ATTR_NO_SYNC = 0 << 2;
static const uint8_t ENDPOINT_ATTR_ASYNC = 1 << 2;
static const uint8_t ENDPOINT_ATTR_ADAPTIVE = 2 << 2;
static const uint8_t ENDPOINT_ATTR_SYNC = 3 << 2;
static const uint8_t ENDPOINT_USAGE_DATA = 0 << 4;
static const uint8_t ENDPOINT_USAGE_FEEDBACK = 1 << 4;
static const uint8_t ENDPOINT_USAGE_IMPLICIT_FEEDBACK = 2 << 4;

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
    uint8_t Size;
    uint8_t Type;
} ATTR_PACKED USB_Descriptor_Header_t;

typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
} ATTR_PACKED USB_StdDescriptor_Header_t;

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
} __attribute__ ((packed));

struct USB_StdDescriptor_Device_t
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdUSB;
    uint8_t bDeviceClass;
    uint8_t bDeviceSubClass;
    uint8_t bDeviceProtocol;
    uint8_t bMaxPacketSize0;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
    uint8_t iManufacturer;
    uint8_t iProduct;
    uint8_t iSerialNumber;
    uint8_t bNumConfigurations;
} __attribute__ ((packed));

struct USB_Descriptor_DeviceQualifier_t
{
    USB_Descriptor_Header_t Header;
    uint16_t USBSpecification;
    uint8_t Class;
    uint8_t SubClass;
    uint8_t Protocol;
    uint8_t Endpoint0Size;
    uint8_t NumberOfConfigurations;
    uint8_t Reserved;
} __attribute__ ((packed));

typedef struct
{
uint8_t bLength;
uint8_t bDescriptorType;
uint16_t bcdUSB;
uint8_t bDeviceClass;
uint8_t bDeviceSubClass;
uint8_t bDeviceProtocol;
uint8_t bMaxPacketSize0;
uint8_t bNumConfigurations;
uint8_t bReserved;
} ATTR_PACKED USB_StdDescriptor_DeviceQualifier_t;

typedef struct
{
USB_Descriptor_Header_t Header;
uint16_t TotalConfigurationSize;
uint8_t TotalInterfaces;
uint8_t ConfigurationNumber;
uint8_t ConfigurationStrIndex;
uint8_t ConfigAttributes;
uint8_t MaxPowerConsumption;
} ATTR_PACKED USB_Descriptor_Configuration_Header_t;

typedef struct
{
uint8_t bLength;
uint8_t bDescriptorType;
uint16_t wTotalLength;
uint8_t bNumInterfaces;
uint8_t bConfigurationValue;
uint8_t iConfiguration;
uint8_t bmAttributes;
uint8_t bMaxPower;
} ATTR_PACKED USB_StdDescriptor_Configuration_Header_t;

typedef struct
{
USB_Descriptor_Header_t Header;
uint8_t InterfaceNumber;
uint8_t AlternateSetting;
uint8_t TotalEndpoints;
uint8_t Class;
uint8_t SubClass;
uint8_t Protocol;
uint8_t InterfaceStrIndex;
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

struct USB_Descriptor_Interface_Association_t
{
    USB_Descriptor_Header_t Header;
    uint8_t FirstInterfaceIndex;
    uint8_t TotalInterfaces;
    uint8_t Class;
    uint8_t SubClass;
    uint8_t Protocol;
    uint8_t IADStrIndex;
} __attribute__ ((packed));

struct USB_StdDescriptor_Interface_Association_t
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bFirstInterface;
    uint8_t bInterfaceCount;
    uint8_t bFunctionClass;
    uint8_t bFunctionSubClass;
    uint8_t bFunctionProtocol;
    uint8_t iFunction;
} __attribute__ ((packed));

typedef struct
{
    USB_Descriptor_Header_t Header;
    uint8_t EndpointAddress;
    uint8_t Attributes;
    uint16_t EndpointSize;
    uint8_t PollingIntervalMS;
} ATTR_PACKED USB_Descriptor_Endpoint_t;

typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bEndpointAddress;
    uint8_t bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t bInterval;
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

#define VERSION_TENS(x)                   (int)((int)(x) / 10)
#define VERSION_ONES(x)                   (int)((int)(x) % 10)
#define VERSION_TENTHS(x)                 (int)((x - (int)x) * 10)
#define VERSION_HUNDREDTHS(x)             (int)((x * 100) - ((int)(x * 10) * 10))



enum USB_Device_States_t
{
    DEVICE_STATE_Unattached  = 0,
    DEVICE_STATE_Powered = 1,
    DEVICE_STATE_Default = 2,
    DEVICE_STATE_Addressed = 3,
    DEVICE_STATE_Configured = 4,
    DEVICE_STATE_Suspended = 5,
};

enum USB_Modes_t
{
    USB_MODE_None   = 0,
    USB_MODE_Device = 1,
    USB_MODE_Host   = 2,
    USB_MODE_UID    = 3,
};

static const uint8_t USB_DEVICE_OPT_LOWSPEED = 1 << 0;
static const uint8_t USB_DEVICE_OPT_FULLSPEED = 0 << 0;
static const uint8_t USE_INTERNAL_SERIAL = 0xDC;
static const uint8_t INTERNAL_SERIAL_LENGTH_BITS = 80;
static const uint8_t INTERNAL_SERIAL_START_ADDRESS = 0x0e;

uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue, const uint8_t wIndex,
       const void** const DescriptorAddress) ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);

void USB_Device_SendRemoteWakeup();

static inline bool USB_Device_IsAddressSet()
{
    return (UDADDR & (1 << ADDEN));
}

    static inline void USB_Device_GetSerialString(uint16_t* const UnicodeString)
    {
        uint_reg_t CurrentGlobalInt = GetGlobalInterruptMask();
        GlobalInterruptDisable();
        uint8_t SigReadAddress = INTERNAL_SERIAL_START_ADDRESS;

        for (uint8_t SerialCharNum = 0;
            SerialCharNum < (INTERNAL_SERIAL_LENGTH_BITS / 4); SerialCharNum++)
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



uint8_t Endpoint_Write_EStream_LE(const void* const Buffer, uint16_t Length,
                              uint16_t* const BytesProcessed) ATTR_NON_NULL_PTR_ARG(1);

uint8_t Endpoint_Write_EStream_BE(const void* const Buffer, uint16_t Length,
                                   uint16_t* const BytesProcessed) ATTR_NON_NULL_PTR_ARG(1);

uint8_t Endpoint_Read_EStream_LE(void* const Buffer, uint16_t Length,
                              uint16_t* const BytesProcessed) ATTR_NON_NULL_PTR_ARG(1);

uint8_t Endpoint_Read_EStream_BE(void* const Buffer, uint16_t Length,
                                   uint16_t* const BytesProcessed) ATTR_NON_NULL_PTR_ARG(1);



uint8_t Endpoint_Write_PStream_LE(const void* const Buffer, uint16_t Length,
                                uint16_t* const BytesProcessed);

uint8_t Endpoint_Write_PStream_BE(const void* const Buffer, uint16_t Length,
                             uint16_t* const BytesProcessed) ATTR_NON_NULL_PTR_ARG(1);

uint8_t Endpoint_Write_Control_PStream_LE(const void* const Buffer,
                                       uint16_t Length) ATTR_NON_NULL_PTR_ARG(1);

uint8_t Endpoint_Write_Control_PStream_BE(const void* const Buffer,
                                              uint16_t Length) ATTR_NON_NULL_PTR_ARG(1);


extern volatile bool USB_IsInitialized;
extern USB_Request_Header_t USB_ControlRequest;

#define _GET_DEVICE_GPIOR_NAME2(y) GPIOR ## y
#define _GET_DEVICE_GPIOR_NAME(x) _GET_DEVICE_GPIOR_NAME2(x)
#define USB_DeviceState _GET_DEVICE_GPIOR_NAME(DEVICE_STATE_AS_GPIOR)

void USB_USBTask();

typedef struct
{
    uint8_t  Address;
    uint16_t Size;
    uint8_t  Type;
    uint8_t  Banks;
} USB_Endpoint_Table_t;

static const uint8_t ENDPOINT_EPNUM_MASK = 0x0f;
static const uint8_t ENDPOINT_CONTROLEP = 0;

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

void Endpoint_ClearEndpoints();

bool Endpoint_ConfigureEndpoint_Prv(const uint8_t Number,
                                    const uint8_t UECFG0XData, const uint8_t UECFG1XData);

static const uint8_t ENDPOINT_TOTAL_ENDPOINTS = 7;

enum Endpoint_WaitUntilReady_ErrorCodes_t
{
    ENDPOINT_READYWAIT_NoError = 0,
    ENDPOINT_READYWAIT_EndpointStalled = 1,
    ENDPOINT_READYWAIT_DeviceDisconnected = 2,
    ENDPOINT_READYWAIT_BusSuspended = 3,
    ENDPOINT_READYWAIT_Timeout = 4,
};

static inline bool Endpoint_ConfigureEndpoint(const uint8_t Address,
                         const uint8_t Type, const uint16_t Size, const uint8_t Banks)
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


static inline void Endpoint_ResetEndpoint(const uint8_t Address)
{
    UERST = (1 << (Address & ENDPOINT_EPNUM_MASK));
    UERST = 0;
}



bool Endpoint_ConfigureEndpointTable(const USB_Endpoint_Table_t* const Table,
                    const uint8_t Entries);


uint8_t Endpoint_WaitUntilReady(void);



struct CDC_LineEncoding_t
{
    uint32_t BaudRateBPS;
    uint8_t CharFormat;
    uint8_t ParityType;
    uint8_t DataBits;
} __attribute__ ((packed));

typedef struct
{
    struct
    {
        uint8_t ControlInterfaceNumber;
        USB_Endpoint_Table_t DataINEndpoint;
        USB_Endpoint_Table_t DataOUTEndpoint;
        USB_Endpoint_Table_t NotificationEndpoint;
    } Config;

    struct
    {
        struct
        {
            uint16_t HostToDevice;
            uint16_t DeviceToHost;
        } ControlLineStates;

        CDC_LineEncoding_t LineEncoding;

    } State;

} USB_ClassInfo_CDC_Device_t;



class USB : public Terminal
{
    void createStream(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo, FILE* const Stream);
    FILE cdcStream;
    static inline uint16_t Endpoint_Read_16_LE();
    static inline uint16_t Endpoint_Read_16_BE();
public:
    static const uint8_t USB_Options = USB_DEVICE_OPT_FULLSPEED | USB_OPT_REG_ENABLED |
            USB_OPT_AUTO_PLL;

    static void USB_Init_Device();
    static void Endpoint_ClearStatusStage();
    static inline void Endpoint_EnableEndpoint() { UECONX |= (1 << EPEN); }
    static inline void Endpoint_DisableEndpoint() { UECONX &= ~(1 << EPEN); }
    static inline bool Endpoint_IsEnabled() { return ((UECONX & (1 << EPEN)) ? true : false); }
    static inline uint8_t Endpoint_GetBusyBanks() { return (UESTA0X & (0x03 << NBUSYBK0)); }
    static inline void Endpoint_AbortPendingIN();
    static inline void Endpoint_ClearIN() { UEINTX &= ~((1 << TXINI) | (1 << FIFOCON)); }
    static inline void Endpoint_ClearOUT() { UEINTX &= ~((1 << RXOUTI) | (1 << FIFOCON)); }
    static inline void Endpoint_StallTransaction() { UECONX |= (1 << STALLRQ); }
    static inline void Endpoint_ClearStall(){ UECONX |= (1 << STALLRQC); }
    static inline bool Endpoint_IsStalled() { return ((UECONX & (1 << STALLRQ)) ? true : false); }
    static inline void Endpoint_ResetDataToggle() { UECONX |= (1 << RSTDT); }
    static inline bool Endpoint_IsINReady() { return ((UEINTX & (1 << TXINI)) ? true : false); }
    static inline bool Endpoint_IsOUTReceived();
    static inline bool Endpoint_IsSETUPReceived();
    static inline bool Endpoint_IsReadWriteAllowed();
    static inline bool Endpoint_IsConfigured();
    static inline uint8_t Endpoint_GetEndpointInterrupts() { return UEINT; }
    static inline bool Endpoint_HasEndpointInterrupted(const uint8_t Address);
    static inline void Endpoint_ClearSETUP() { UEINTX &= ~(1 << RXSTPI); }
    static inline void Endpoint_SetEndpointDirection(const uint8_t DirectionMask);
    static inline uint8_t Endpoint_Read_8() { return UEDATX; }

    static inline void USB_Device_EnableDeviceAddress(const uint8_t Address)
    { (void)Address; UDADDR |= (1 << ADDEN); }

    static inline void USB_Device_SetDeviceAddress(const uint8_t Address)
    { UDADDR = (UDADDR & (1 << ADDEN)) | (Address & 0x7F); }

    static inline uint16_t USB_Device_GetFrameNumber() { return UDFNUM; }
    static inline void USB_Device_EnableSOFEvents() { USB_INT_Enable(USB_INT_SOFI); }
    static inline void USB_Device_DisableSOFEvents() { USB_INT_Disable(USB_INT_SOFI); }
    static inline void USB_Device_SetLowSpeed() { UDCON |=  (1 << LSM); }
    static inline void USB_Device_SetFullSpeed() { UDCON &= ~(1 << LSM); }
    static inline void Endpoint_Write_8(const uint8_t Data) { UEDATX = Data; }

    static inline uint8_t Endpoint_GetEndpointDirection()
    {
        return (UECFG0X & (1 << EPDIR)) ? ENDPOINT_DIR_IN : ENDPOINT_DIR_OUT;
    }

    static inline uint8_t Endpoint_GetCurrentEndpoint()
    {
        return ((UENUM & ENDPOINT_EPNUM_MASK) | Endpoint_GetEndpointDirection());
    }

    static inline void Endpoint_SelectEndpoint(const uint8_t Address)
    {
        UENUM = (Address & ENDPOINT_EPNUM_MASK);
    }

    static inline void Endpoint_Discard_8();
    static inline void Endpoint_Write_16_LE(const uint16_t Data);
    static inline void Endpoint_Write_16_BE(const uint16_t Data);
    static inline void Endpoint_Discard_16();
    static inline uint32_t Endpoint_Read_32_LE();
    static inline uint32_t Endpoint_Read_32_BE();
    static inline void Endpoint_Write_32_LE(const uint32_t Data);
    static uint8_t Endpoint_Discard_Stream(uint16_t Length, uint16_t* const BytesProcessed);
    static uint8_t Endpoint_Null_Stream(uint16_t Length, uint16_t* const BytesProcessed);
    static inline void Endpoint_Write_32_BE(const uint32_t Data);
    static inline void Endpoint_Discard_32();
    static void CDC_Device_USBTask(USB_ClassInfo_CDC_Device_t * const CDCInterfaceInfo);
    static inline void disableAllInterrupts() { USBCON &= ~(1<<VBUSTE); UDIEN = 0; }
    static inline void clearAllInterrupts() { USBINT = 0; }
    static USB_ClassInfo_CDC_Device_t cdcDevice;
    static uint8_t CDC_Device_Flush(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo);
    void myPutc(char c);
    uint8_t readByte();
    void resetInterface();
    USB();
    void inline flush() { CDC_Device_Flush(&cdcDevice); }
    void init();
    void inline createStream() { createStream(&cdcDevice, &cdcStream); }
    void inline cdcTask() { CDC_Device_USBTask(&cdcDevice); }
    static inline void USB_INT_Clear(const uint8_t Interrupt);
    static inline bool USB_INT_IsEnabled(const uint8_t Interrupt);

    static uint8_t Endpoint_Write_Stream_LE(const void* const Buffer, uint16_t Length,
                       uint16_t* const BytesProcessed) ATTR_NON_NULL_PTR_ARG(1);

    static uint8_t Endpoint_Write_Stream_BE(const void* const Buffer, uint16_t Length,
                       uint16_t* const BytesProcessed) ATTR_NON_NULL_PTR_ARG(1);

    static uint8_t Endpoint_Read_Stream_LE(void* const Buffer,  uint16_t Length,
                        uint16_t* const BytesProcessed) ATTR_NON_NULL_PTR_ARG(1);

    static uint8_t Endpoint_Read_Stream_BE(void* const Buffer, uint16_t Length,
                                  uint16_t* const BytesProcessed) ATTR_NON_NULL_PTR_ARG(1);

    static inline void USB_OTGPAD_Off() {    USBCON &= ~(1 << OTGPADE); }
    static inline void USB_CLK_Freeze()    {    USBCON |=  (1 << FRZCLK); }
    static inline void USB_CLK_Unfreeze(void){    USBCON &= ~(1 << FRZCLK);}
    static inline void USB_Controller_Enable(void) {    USBCON |=  (1 << USBE); }
    static inline void USB_Controller_Disable(void) {    USBCON &= ~(1 << USBE); }

    static inline void USB_Controller_Reset(void)
    {
        USBCON &= ~(1 << USBE);
        USBCON |=  (1 << USBE);
    }

    static inline bool USB_VBUS_GetStatus() { return ((USBSTA & (1 << VBUS)) ? true : false); }
    static inline void USB_Detach() { UDCON  |=  (1 << DETACH); }
    static inline void USB_Attach() { UDCON  &= ~(1 << DETACH); }

    static inline void USB_PLL_On()
    {
        PLLCSR = USB_PLL_PSC;
        PLLCSR = (USB_PLL_PSC | (1 << PLLE));
    }

    static inline void USB_PLL_Off() { PLLCSR = 0; }
    static inline bool USB_PLL_IsReady() { return ((PLLCSR & (1 << PLOCK)) ? true : false); }
    static inline void USB_REG_On() { UHWCON |=  (1 << UVREGE); }
    static inline void USB_REG_Off(){    UHWCON &= ~(1 << UVREGE); }
    static inline void USB_OTGPAD_On() {    USBCON |=  (1 << OTGPADE); }
    static void USB_INT_Enable(const uint8_t Interrupt);
    static void USB_INT_Disable(const uint8_t Interrupt);
    static bool USB_INT_HasOccurred(const uint8_t Interrupt);
    static uint8_t Endpoint_Write_Control_Stream_LE(const void* const Buffer, uint16_t Length);
    static uint8_t Endpoint_Write_Control_Stream_BE(const void* const Buffer, uint16_t Length);
    static uint8_t Endpoint_Read_Control_Stream_LE(void* const Buffer, uint16_t Length);
    static uint8_t Endpoint_Read_Control_Stream_BE(void* const Buffer, uint16_t Length);
    uint8_t Endpoint_Write_Control_EStream_LE(const void* const Buffer, uint16_t Length);
    uint8_t Endpoint_Write_Control_EStream_BE(const void* const Buffer, uint16_t Length);
    uint8_t Endpoint_Read_Control_EStream_LE(void* const Buffer, uint16_t Length);
    uint8_t Endpoint_Read_Control_EStream_BE(void* const Buffer, uint16_t Length);
};

#endif

