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
#include "bogota.h"

static inline uint8_t Endpoint_BytesToEPSizeMask(const uint16_t Bytes)
{
    uint8_t MaskVal = 0;
    uint16_t CheckBytes = 8;

    while (CheckBytes < Bytes)
    {
        MaskVal++;
        CheckBytes <<= 1;
    }

    return MaskVal << EPSIZE0;
}

static inline void Endpoint_SelectEndpoint(const uint8_t Address)
{
    UENUM = (Address & ENDPOINT_EPNUM_MASK);
}

static bool Endpoint_ConfigureEndpoint_Prv(const uint8_t Number, const uint8_t UECFG0XData,
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
        UEIENX = UEIENXTemp;

        if ((UESTA0X & 1<<CFGOK) == 0)
            return false;
    }

    Endpoint_SelectEndpoint(Number);
    return true;
}

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

static inline uint8_t Endpoint_GetEndpointDirection(void)
{
    return (UECFG0X & (1 << EPDIR)) ? ENDPOINT_DIR_IN : ENDPOINT_DIR_OUT;
}

static inline uint8_t Endpoint_GetCurrentEndpoint(void)
{
    return ((UENUM & ENDPOINT_EPNUM_MASK) | Endpoint_GetEndpointDirection());
}

static inline bool Endpoint_IsReadWriteAllowed(void)
{
    return ((UEINTX & (1 << RWAL)) ? true : false);
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

static inline void Endpoint_ClearIN(void)
{
    UEINTX &= ~((1 << TXINI) | (1 << FIFOCON));
}

static inline void Endpoint_ClearOUT(void)
{
    UEINTX &= ~((1 << RXOUTI) | (1 << FIFOCON));
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
    union { uint32_t Value; uint8_t  Bytes[4]; } Data;
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

#define FIXED_CONTROL_ENDPOINT_SIZE      8
#define USB_Device_ControlEndpointSize FIXED_CONTROL_ENDPOINT_SIZE

#define USB_DEVICE_OPT_LOWSPEED            (1 << 0)
#define USB_DEVICE_OPT_FULLSPEED               (0 << 0)

#define USE_INTERNAL_SERIAL            0xDC
#define INTERNAL_SERIAL_LENGTH_BITS    80
#define INTERNAL_SERIAL_START_ADDRESS  0x0E

static inline uint16_t USB_Device_GetFrameNumber(void)
{
    return UDFNUM;
}

static inline void USB_Device_EnableSOFEvents(void)
{
    UDIEN |= 1<<SOFE;
}

static inline void USB_Device_DisableSOFEvents(void)
{
    UDIEN &= ~(1<<SOFE);
}

static inline void USB_Device_SetDeviceAddress(const uint8_t Address)
{
    UDADDR = (UDADDR & (1 << ADDEN)) | (Address & 0x7F);
}

static inline void USB_Device_EnableDeviceAddress(const uint8_t Address)
{
    (void)Address;
    UDADDR |= 1<<ADDEN;
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

#define USE_STATIC_OPTIONS (USB_DEVICE_OPT_FULLSPEED | USB_OPT_REG_ENABLED | USB_OPT_AUTO_PLL)
#define USB_Options USE_STATIC_OPTIONS

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
    USBCON |= 1<<FRZCLK;
}

static inline void USB_Controller_Enable(void)
{
    USBCON |= 1<<USBE;
}

static inline void USB_Controller_Disable(void)
{
    USBCON &= ~(1<<USBE);
}

static inline void USB_Controller_Reset(void)
{
    USBCON &= ~(1 << USBE);
    USBCON |=  (1 << USBE);
}

#define USB_HOST_TIMEOUT_MS                1000

typedef uint8_t (* ConfigComparatorPtr_t)(void*);

static constexpr uint8_t
    HOST_SENDCONTROL_Successful         = 0,
    HOST_SENDCONTROL_DeviceDisconnected = 1,
    HOST_SENDCONTROL_PipeError          = 2,
    HOST_SENDCONTROL_SetupStalled       = 3,
    HOST_SENDCONTROL_SoftwareTimeOut    = 4,
    USB_HOST_WAITFOR_SetupSent = 0,
    USB_HOST_WAITFOR_InReceived = 1,
    USB_HOST_WAITFOR_OutReady = 2,
    HOST_GETCONFIG_Successful = 0,
    HOST_GETCONFIG_DeviceDisconnect = 1,
    HOST_GETCONFIG_PipeError = 2,
    HOST_GETCONFIG_SetupStalled = 3,
    HOST_GETCONFIG_SoftwareTimeOut = 4,
    HOST_GETCONFIG_BuffOverflow = 5,
    HOST_GETCONFIG_InvalidData = 6,
    DESCRIPTOR_SEARCH_Found = 0,
    DESCRIPTOR_SEARCH_Fail = 1,
    DESCRIPTOR_SEARCH_NotFound = 2,
    DESCRIPTOR_SEARCH_COMP_Found = 0,
    DESCRIPTOR_SEARCH_COMP_Fail = 1,
    DESCRIPTOR_SEARCH_COMP_EndOfDescriptor = 2,
    CDC_CONTROL_LINE_OUT_DTR = 1<<0,
    CDC_CONTROL_LINE_OUT_RTS = 1<<1,
    CDC_CONTROL_LINE_IN_DCD = 1<<0,
    CDC_CONTROL_LINE_IN_DSR = 1<<1,
    CDC_CONTROL_LINE_IN_BREAK = 1<<2,
    CDC_CONTROL_LINE_IN_RING = 1<<3,
    CDC_CONTROL_LINE_IN_FRAMEERROR = 1<<4,
    CDC_CONTROL_LINE_IN_PARITYERROR = 1<<5,
    CDC_CONTROL_LINE_IN_OVERRUNERROR = 1<<6,
    CDC_CSCP_CDCClass = 0x02,
    CDC_CSCP_NoSpecificSubclass = 0x00,
    CDC_CSCP_ACMSubclass = 0x02,
    CDC_CSCP_ATCommandProtocol      = 0x01,
    CDC_CSCP_NoSpecificProtocol     = 0x00,
    CDC_CSCP_VendorSpecificProtocol = 0xFF,
    CDC_CSCP_CDCDataClass           = 0x0A,
    CDC_CSCP_NoDataSubclass         = 0x00,
    CDC_CSCP_NoDataProtocol         = 0x00,
    CDC_REQ_SendEncapsulatedCommand = 0x00,
    CDC_REQ_GetEncapsulatedResponse = 0x01,
    CDC_REQ_SetLineEncoding         = 0x20,
    CDC_REQ_GetLineEncoding         = 0x21,
    CDC_REQ_SetControlLineState     = 0x22,
    CDC_REQ_SendBreak               = 0x23,
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
    uint8_t size;
    uint8_t type;
    uint8_t  Subtype;
    uint16_t CDCSpecification;
} ATTR_PACKED USB_CDC_Descriptor_FunctionalHeader_t;

typedef struct
{
    uint8_t  bFunctionLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubType;
    uint16_t bcdCDC;
} ATTR_PACKED USB_CDC_StdDescriptor_FunctionalHeader_t;

typedef struct
{
    uint8_t size;
    uint8_t type;
    uint8_t Subtype;
    uint8_t Capabilities;
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
    uint8_t size;
    uint8_t type;
    uint8_t Subtype;
    uint8_t MasterInterfaceNumber;
    uint8_t SlaveInterfaceNumber;
} ATTR_PACKED USB_CDC_Descriptor_FunctionalUnion_t;

struct USB_CDC_StdDescriptor_FunctionalUnion_t
{
    uint8_t bFunctionLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubType;
    uint8_t bMasterInterface;
    uint8_t bSlaveInterface0;
} ATTR_PACKED;

static constexpr uint8_t
    RNDIS_ERROR_LOGICAL_CMD_FAILED = 0x80,
    REMOTE_NDIS_VERSION_MAJOR = 0x01,
    REMOTE_NDIS_VERSION_MINOR = 0x00,
    ETHERNET_FRAME_SIZE_MAX = 200,
    RNDIS_REQ_SendEncapsulatedCommand = 0x00,
    RNDIS_REQ_GetEncapsulatedResponse = 0x01,
    RNDIS_Uninitialized = 0,
    RNDIS_Initialized = 1,
    RNDIS_Data_Initialized = 2,
    RNDIS_NOTIF_ResponseAvailable = 0x01,
    NDIS_HardwareStatus_Ready = 0,
    NDIS_HardwareStatus_Initializing = 1,
    NDIS_HardwareStatus_Reset = 2,
    NDIS_HardwareStatus_Closing = 3,
    NDIS_HardwareStatus_NotReady = 4;

static constexpr uint32_t
    REMOTE_NDIS_PACKET_MSG = 0x00000001UL,
    REMOTE_NDIS_INITIALIZE_MSG = 0x00000002UL,
    REMOTE_NDIS_HALT_MSG = 0x00000003,
    REMOTE_NDIS_QUERY_MSG = 0x00000004,
    REMOTE_NDIS_SET_MSG = 0x00000005,
    REMOTE_NDIS_RESET_MSG = 0x00000006,
    REMOTE_NDIS_INDICATE_STATUS_MSG = 0x00000007,
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

#define MAC_COMPARE(MAC1, MAC2)          (memcmp(MAC1, MAC2, sizeof(MacAddr)) == 0)

struct MacAddr
{
    uint8_t Octets[6];
} ATTR_PACKED;

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

class Busby
{
protected:
    virtual void customCtrl() { }
    //volatile uint8_t USB_DeviceState;
public:
    static Busby *instance;
    Busby();
    void gen();
    void com();
    void USB_DeviceTask();
    void procCtrlReq();
    uint8_t readControlStreamLE(void* const Buffer, uint16_t Length);
    uint8_t Endpoint_WaitUntilReady();
    uint8_t write_Control_Stream_LE(const void* const Buffer, uint16_t Length);
    void USB_Device_GetInternalSerialDescriptor();
    void USB_Device_SetAddress();
    void USB_Device_ClearSetFeature();
    uint16_t Endpoint_BytesInEndpoint();
    uint8_t Endpoint_Write_Control_PStream_LE(const void* const Buffer, uint16_t Length);
    uint8_t readStream(void * const buf, uint16_t len, uint16_t* const BytesProcessed);
    void USB_Init();
    virtual uint16_t getDesc(uint16_t, uint16_t, const void ** const) { return 0; }

    uint8_t Endpoint_Write_PStream_LE(const void * const Buffer,
                            uint16_t Length, uint16_t* const BytesProcessed);

    uint8_t writeStream2(const void * const Buffer, uint16_t Length,
                            uint16_t* const BytesProcessed);
};

Busby *Busby::instance;

class RNDIS : public Busby
{
public:
    void customCtrl();
    void rndisTask();
    void ethTask();
    void ProcessRNDISControlMessage();
    int16_t ICMP_ProcessICMPPacket(void* InDataStart, void* OutDataStart);
    int16_t IP_ProcessIPPacket(void* InDataStart, void* OutDataStart);
    void Ethernet_ProcessPacket();
    bool ProcessNDISSet(uint32_t OId, void* SetData, uint16_t SetSize);
    uint16_t Ethernet_Checksum16(void* Data, uint16_t Bytes);
    int16_t ARP_ProcessARPPacket(void *inStart, void *outStart);
    //uint16_t getDesc(uint16_t wValue, uint16_t wIndex, const void ** const descAddr);

    bool ProcessNDISQuery(const uint32_t OId, void* QueryData, uint16_t QuerySize,
                             void* ResponseData, uint16_t* ResponseSize);
};

static volatile uint8_t USB_DeviceState;

void Busby::USB_DeviceTask()
{
    if (USB_DeviceState == DEVICE_STATE_UNATTACHED)
        return;

    uint8_t PrevEndpoint = Endpoint_GetCurrentEndpoint();
    Endpoint_SelectEndpoint(0);

    if (UEINTX & 1<<RXSTPI)
        procCtrlReq();

    Endpoint_SelectEndpoint(PrevEndpoint);
}

Busby::Busby()
{
    instance = this;
}

uint16_t Busby::Endpoint_BytesInEndpoint()
{
    return (((uint16_t)UEBCHX << 8) | UEBCLX);
}

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

static constexpr uint8_t
    PROTOCOL_ICMP = 1,
    PROTOCOL_IGMP = 2,
    PROTOCOL_TCP = 6,
    PROTOCOL_UDP = 17,
    PROTOCOL_OSPF = 89,
    PROTOCOL_SCTP = 132;

static const MacAddr PROGMEM AdapterMACAddress = {{0x02, 0x00, 0x02, 0x00, 0x02, 0x00}};
static const char PROGMEM AdapterVendorDescription[] = "LUFA RNDIS Adapter";
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
        char *AdapterVendorDescription;
        MacAddr AdapterMACAddress;
        uint8_t *MessageBuffer;
        uint16_t MessageBufferLength;
    } Config;

    struct
    {
        bool     ResponseReady;
        uint8_t  CurrRNDISState;
        uint32_t CurrPacketFilter;
    } State;
} USB_ClassInfo_RNDIS_Device_t;

static volatile bool USB_IsInitialized;
static USBRequest USB_ControlRequest;

static inline bool Endpoint_IsOUTReceived(void)
{
    return ((UEINTX & (1 << RXOUTI)) ? true : false);
}



uint8_t Busby::Endpoint_WaitUntilReady()
{
    uint8_t  TimeoutMSRem = USB_STREAM_TIMEOUT_MS;
    uint16_t PreviousFrameNumber = USB_Device_GetFrameNumber();

    for (;;)
    {
        if (Endpoint_GetEndpointDirection() == ENDPOINT_DIR_IN)
        {
            if (UEINTX & 1<<TXINI)
                return ENDPOINT_READYWAIT_NoError;
        }
        else
        {
            if (UEINTX & 1<<RXOUTI)
                return ENDPOINT_READYWAIT_NoError;
        }

        uint8_t USB_DeviceState_LCL = USB_DeviceState;

        if (USB_DeviceState_LCL == DEVICE_STATE_UNATTACHED)
            return ENDPOINT_READYWAIT_DeviceDisconnected;

        if (USB_DeviceState_LCL == DEVICE_STATE_SUSPENDED)
            return ENDPOINT_READYWAIT_BusSuspended;

        if (UECONX & 1<<STALLRQ)
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

void Endpoint_ClearStatusStage(void);

#define RNDIS_DEVICE_MIN_MESSAGE_BUFFER_LENGTH  sizeof(AdapterSupportedOIDList) + sizeof(RNDIS_Query_Complete_t)

uint8_t Busby::writeStream2(const void * const Buffer, uint16_t Length,
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
        if (Endpoint_IsReadWriteAllowed() == 0)
        {
            Endpoint_ClearIN();

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
            Endpoint_Write_8(*DataStream);
            DataStream += 1;
            Length--;
            BytesInTransfer++;
        }
    }

    return ENDPOINT_RWSTREAM_NoError;
}

uint8_t Busby::readStream(void * const Buffer, uint16_t Length,
                            uint16_t* const BytesProcessed)
{
    uint8_t* DataStream      = (uint8_t*)Buffer;
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
        if ((Endpoint_IsReadWriteAllowed()) == 0)
        {
            Endpoint_ClearOUT();

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
            *DataStream = Endpoint_Read_8();
            DataStream += 1;
            Length--;
            BytesInTransfer++;
        }
    }

    return ENDPOINT_RWSTREAM_NoError;
}

uint8_t Busby::Endpoint_Write_PStream_LE(const void * const Buffer,
                            uint16_t Length, uint16_t* const BytesProcessed)
{
    uint8_t *DataStream = (uint8_t*)Buffer;
    uint16_t BytesInTransfer = 0;
    uint8_t ErrorCode;

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
            Endpoint_Write_8(pgm_read_byte(DataStream));
            DataStream += 1;
            Length--;
            BytesInTransfer++;
        }
    }

    return ENDPOINT_RWSTREAM_NoError;
}

static inline bool Endpoint_IsSETUPReceived(void)
{
    return ((UEINTX & (1 << RXSTPI)) ? true : false);
}

uint8_t Busby::write_Control_Stream_LE(const void* const Buffer, uint16_t Length)
{
    uint8_t *DataStream = (uint8_t*)Buffer;
    bool LastPacketFull = false;

    if (Length > USB_ControlRequest.wLength)
        Length = USB_ControlRequest.wLength;
    else if (!(Length))
        Endpoint_ClearIN();

    while (Length || LastPacketFull)
    {
        uint8_t USB_DeviceState_LCL = USB_DeviceState;

        if (USB_DeviceState_LCL == DEVICE_STATE_UNATTACHED)
            return ENDPOINT_RWCSTREAM_DeviceDisconnected;
        if (USB_DeviceState_LCL == DEVICE_STATE_SUSPENDED)
            return ENDPOINT_RWCSTREAM_BusSuspended;
        if (Endpoint_IsSETUPReceived())
            return ENDPOINT_RWCSTREAM_HostAborted;
        if (Endpoint_IsOUTReceived())
            break;

        if (UEINTX & 1<<TXINI)
        {
            uint16_t BytesInEndpoint = Endpoint_BytesInEndpoint();

            while (Length && (BytesInEndpoint < USB_Device_ControlEndpointSize))
            {
                Endpoint_Write_8(*DataStream);
                DataStream += 1;
                Length--;
                BytesInEndpoint++;
            }

            LastPacketFull = (BytesInEndpoint == USB_Device_ControlEndpointSize);
            Endpoint_ClearIN();
        }
    }

    while ((Endpoint_IsOUTReceived()) == 0)
    {
        uint8_t USB_DeviceState_LCL = USB_DeviceState;

        if (USB_DeviceState_LCL == DEVICE_STATE_UNATTACHED)
            return ENDPOINT_RWCSTREAM_DeviceDisconnected;

        if (USB_DeviceState_LCL == DEVICE_STATE_SUSPENDED)
            return ENDPOINT_RWCSTREAM_BusSuspended;

        if (Endpoint_IsSETUPReceived())
            return ENDPOINT_RWCSTREAM_HostAborted;
    }

    return ENDPOINT_RWCSTREAM_NoError;
}

uint8_t Busby::readControlStreamLE(void* const Buffer, uint16_t Length)
{
    uint8_t* DataStream = (uint8_t*)Buffer;

    if (!(Length))
      Endpoint_ClearOUT();

    while (Length)
    {
        uint8_t USB_DeviceState_LCL = USB_DeviceState;

        if (USB_DeviceState_LCL == DEVICE_STATE_UNATTACHED)
            return ENDPOINT_RWCSTREAM_DeviceDisconnected;
        if (USB_DeviceState_LCL == DEVICE_STATE_SUSPENDED)
            return ENDPOINT_RWCSTREAM_BusSuspended;
        if (Endpoint_IsSETUPReceived())
            return ENDPOINT_RWCSTREAM_HostAborted;

        if (Endpoint_IsOUTReceived())
        {
            while (Length && Endpoint_BytesInEndpoint())
            {
                *DataStream = Endpoint_Read_8();
                DataStream += 1;
                Length--;
            }

            Endpoint_ClearOUT();
        }
    }

    while ((UEINTX & 1<<TXINI) == 0)
    {
        uint8_t USB_DeviceState_LCL = USB_DeviceState;

        if (USB_DeviceState_LCL == DEVICE_STATE_UNATTACHED)
            return ENDPOINT_RWCSTREAM_DeviceDisconnected;

        if (USB_DeviceState_LCL == DEVICE_STATE_SUSPENDED)
            return ENDPOINT_RWCSTREAM_BusSuspended;
    }

    return ENDPOINT_RWCSTREAM_NoError;
}

uint8_t Busby::Endpoint_Write_Control_PStream_LE(const void* const Buffer, uint16_t Length)
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

        if (USB_DeviceState_LCL == DEVICE_STATE_UNATTACHED)
            return ENDPOINT_RWCSTREAM_DeviceDisconnected;

        if (USB_DeviceState_LCL == DEVICE_STATE_SUSPENDED)
            return ENDPOINT_RWCSTREAM_BusSuspended;

        if (Endpoint_IsSETUPReceived())
            return ENDPOINT_RWCSTREAM_HostAborted;

        if (Endpoint_IsOUTReceived())
            break;

        if (UEINTX & 1<<TXINI)
        {
            uint16_t BytesInEndpoint = Endpoint_BytesInEndpoint();

            while (Length && (BytesInEndpoint < USB_Device_ControlEndpointSize))
            {
                Endpoint_Write_8(pgm_read_byte(DataStream));
                DataStream += 1;
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

        if (USB_DeviceState_LCL == DEVICE_STATE_UNATTACHED)
            return ENDPOINT_RWCSTREAM_DeviceDisconnected;
        if (USB_DeviceState_LCL == DEVICE_STATE_SUSPENDED)
            return ENDPOINT_RWCSTREAM_BusSuspended;
        if (Endpoint_IsSETUPReceived())
            return ENDPOINT_RWCSTREAM_HostAborted;
    }

    return ENDPOINT_RWCSTREAM_NoError;
}

void Busby::USB_Device_SetAddress()
{
	uint8_t DeviceAddress = (USB_ControlRequest.wValue & 0x7F);
	USB_Device_SetDeviceAddress(DeviceAddress);
    UEINTX &= ~(1<<RXSTPI);
	Endpoint_ClearStatusStage();

	while ((UEINTX & 1<<TXINI) == 0)
        ;

	USB_Device_EnableDeviceAddress(DeviceAddress);
	USB_DeviceState = (DeviceAddress) ? DEVICE_STATE_ADDRESSED : DEVICE_STATE_Default;
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

static uint8_t USB_Device_ConfigurationNumber;
static bool USB_Device_CurrentlySelfPowered;
static bool USB_Device_RemoteWakeupEnabled;

static inline bool USB_Device_IsAddressSet(void)
{
    return UDADDR & 1<<ADDEN;
}

static void USB_Device_GetConfiguration(void)
{
    UEINTX &= ~(1<<RXSTPI);
	Endpoint_Write_8(USB_Device_ConfigurationNumber);
	Endpoint_ClearIN();
	Endpoint_ClearStatusStage();
}

void Busby::USB_Device_GetInternalSerialDescriptor()
{
	struct
	{
		USB_Descriptor_Header_t Header;
		uint16_t                UnicodeString[INTERNAL_SERIAL_LENGTH_BITS / 4];
	} SignatureDescriptor;

	SignatureDescriptor.Header.Type = DTYPE_STRING;
	SignatureDescriptor.Header.Size = USB_STRING_LEN(INTERNAL_SERIAL_LENGTH_BITS / 4);
	USB_Device_GetSerialString(SignatureDescriptor.UnicodeString);
    UEINTX &= ~(1<<RXSTPI);
	write_Control_Stream_LE(&SignatureDescriptor, sizeof(SignatureDescriptor));
	Endpoint_ClearOUT();
}

bool Endpoint_ConfigureEndpointTable(const USB_Endpoint_Table_t* const Table,
                                     const uint8_t Entries)
{
    for (uint8_t i = 0; i < Entries; i++)
    {
        if (!(Table[i].Address))
            continue;

        if (!(Endpoint_ConfigureEndpoint(Table[i].Address, Table[i].Type,
            Table[i].Size, Table[i].Banks)))
        {
            return false;
        }
    }

    return true;
}

static void USB_Init_Device()
{
    USB_DeviceState = DEVICE_STATE_UNATTACHED;
    USB_Device_ConfigurationNumber = 0;
    USB_Device_RemoteWakeupEnabled = false;
    USB_Device_CurrentlySelfPowered = false;
    UDCON &= ~(1<<LSM);
    USBCON |= 1<<VBUSTE;
    Endpoint_ConfigureEndpoint(0, EP_TYPE_CONTROL, USB_Device_ControlEndpointSize, 1);
    UDINT &= ~(1<<SUSPI);
    UDIEN |= 1<<SUSPE;
    UDIEN |= 1<<EORSTE;
    UDCON &= ~(1<<DETACH);
}

static void USB_ResetInterface()
{
    USBCON &= ~(1<<VBUSTE);
    UDIEN = 0;
    USBINT = 0;
    UDINT  = 0;
    USB_Controller_Reset();
    USBCON &= ~(1<<FRZCLK);
    PLLCSR = 0;
    USB_Init_Device();
    USBCON |= 1<<OTGPADE;
}

void Busby::USB_Init()
{
    USB_OTGPAD_Off();

    if (!(USB_Options & USB_OPT_REG_DISABLED))
        UHWCON |= 1<<UVREGE;
    else
        UHWCON &= ~(1<<UVREGE);

    if ((USB_Options & USB_OPT_MANUAL_PLL) == 0)
        PLLFRQ = 1<<PDIV2;

    USB_IsInitialized = true;
    USB_ResetInterface();
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
        while ((Endpoint_IsOUTReceived()) == 0)
            if (USB_DeviceState == DEVICE_STATE_UNATTACHED)
                return;

        Endpoint_ClearOUT();
    }
    else
    {
        while ((UEINTX & 1<<TXINI) == 0)
            if (USB_DeviceState == DEVICE_STATE_UNATTACHED)
                return;

        Endpoint_ClearIN();
    }
}

void Busby::USB_Device_ClearSetFeature()
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

			if (EndpointIndex == 0 || EndpointIndex >= ENDPOINT_TOTAL_ENDPOINTS)
			  return;

			Endpoint_SelectEndpoint(EndpointIndex);

			if (UECONX & 1<<EPEN)
			{
				if (USB_ControlRequest.bRequest == REQ_SetFeature)
				{
                    UECONX |= 1<<STALLRQ;
				}
				else
				{
                    UECONX |= 1<<STALLRQC;
                    UERST = (1 << (EndpointIndex & ENDPOINT_EPNUM_MASK));
                    UERST = 0;
                    UECONX |= 1<<RSTDT;
				}
			}
		}

		break;
	}
	default:
		return;
	}

	Endpoint_SelectEndpoint(0);
    UEINTX &= ~(1<<RXSTPI);
	Endpoint_ClearStatusStage();
}

static inline bool Endpoint_IsStalled(void)
{
    return UECONX & 1<<STALLRQ ? true : false;
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
			Endpoint_SelectEndpoint(0);
			break;
		}
		default:
			return;
	}

    UEINTX &= ~(1<<RXSTPI);
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

uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue, const uint16_t wIndex,
       const void** const DescriptorAddress) ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);

bool RNDIS::ProcessNDISQuery(const uint32_t OId, void* QueryData, uint16_t QuerySize,
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
        *ResponseSize = sizeof(MacAddr);
        memcpy_P(ResponseData, &AdapterMACAddress, sizeof(MacAddr));
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

bool RNDIS::ProcessNDISSet(uint32_t OId, void* SetData, uint16_t SetSize)
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

void RNDIS::ProcessRNDISControlMessage()
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

        INITIALIZE_Response->MaxTransferSize = sizeof(RNDIS_Packet_Message_t) +
            ETHERNET_FRAME_SIZE_MAX;

        INITIALIZE_Response->PacketAlignmentFactor = 0;
        INITIALIZE_Response->AFListOffset = 0;
        INITIALIZE_Response->AFListSize = 0;
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

void RNDIS::customCtrl()
{
	switch (USB_ControlRequest.bRequest)
	{
    case RNDIS_REQ_SendEncapsulatedCommand:
        if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS |
            REQREC_INTERFACE))
        {
            UEINTX &= ~(1<<RXSTPI);
            readControlStreamLE(RNDISMessageBuffer, USB_ControlRequest.wLength);
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

            UEINTX &= ~(1<<RXSTPI);
            write_Control_Stream_LE(RNDISMessageBuffer, MessageHeader->MessageLength);
            Endpoint_ClearOUT();
            MessageHeader->MessageLength = 0;
        }
        break;
	}
}

void Busby::procCtrlReq(void)
{
	uint8_t* RequestHeader = (uint8_t*)&USB_ControlRequest;

    for (uint8_t RequestHeaderByte = 0; RequestHeaderByte < sizeof(USBRequest); RequestHeaderByte++)
	  *(RequestHeader++) = Endpoint_Read_8();

    customCtrl();

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
                    const void *DescriptorPointer;
                    uint16_t DescriptorSize;
                
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
                
                    UEINTX &= ~(1<<RXSTPI);
                    Endpoint_Write_Control_PStream_LE(DescriptorPointer, DescriptorSize);
                    *p_ueintx &= ~(1<<rxouti | 1<<fifocon);

				}

				break;
			case REQ_GetConfiguration:
				if (bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_DEVICE))
				  USB_Device_GetConfiguration();

				break;
			case REQ_SETCONFIGURATION:
				if (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_DEVICE))
                {
                    if ((uint8_t)USB_ControlRequest.wValue > FIXED_NUM_CONFIGURATIONS)
                        return;

                    UEINTX &= ~(1<<RXSTPI);
                    USB_Device_ConfigurationNumber = (uint8_t)USB_ControlRequest.wValue;
                    Endpoint_ClearStatusStage();

                    if (USB_Device_ConfigurationNumber)
                        USB_DeviceState = DEVICE_STATE_Configured;
                    else
                        USB_DeviceState = USB_Device_IsAddressSet() ? DEVICE_STATE_Configured : DEVICE_STATE_POWERED;

                    EVENT_USB_Device_ConfigurationChanged();
                }
				break;

			default:
				break;
		}
	}

	if (Endpoint_IsSETUPReceived())
	{
        UEINTX &= ~(1<<RXSTPI);
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

struct ARP_Header_t
{
    uint16_t HardwareType;
    uint16_t ProtocolType;
    uint8_t HLEN;
    uint8_t PLEN;
    uint16_t Operation;
    MacAddr SHA;
    IP_Address_t SPA;
    MacAddr THA;
    IP_Address_t TPA;
};

static constexpr uint8_t
    ARP_OPERATION_REQUEST = 1,
    ARP_OPERATION_REPLY = 2,
    ICMP_TYPE_ECHOREPLY = 0,
    ICMP_TYPE_DESTINATIONUNREACHABLE = 3,
    ICMP_TYPE_SOURCEQUENCH = 4,
    ICMP_TYPE_REDIRECTMESSAGE = 5,
    ICMP_TYPE_ECHOREQUEST = 8,
    ICMP_TYPE_TIMEEXCEEDED = 11,
    UDP_PORT_DHCP_REQUEST = 67,
    UDP_PORT_DHCP_REPLY = 68,
    UDP_PORT_DNS = 53,
    DHCP_OP_BOOTREQUEST = 0x01,
    DHCP_OP_BOOTREPLY = 0x02,
    DHCP_HTYPE_ETHERNET = 0x01,
    DHCP_OPTION_SUBNETMASK   = 1,
    DHCP_OPTION_LEASETIME    = 51,
    DHCP_OPTION_MESSAGETYPE  = 53,
    DHCP_OPTION_DHCPSERVER   = 54,
    DHCP_OPTION_PAD          = 0,
    DHCP_OPTION_END          = 255,
    DHCP_MESSAGETYPE_DISCOVER = 1,
    DHCP_MESSAGETYPE_OFFER   = 2,
    DHCP_MESSAGETYPE_REQUEST = 3,
    DHCP_MESSAGETYPE_DECLINE = 4,
    DHCP_MESSAGETYPE_ACK     = 5,
    DHCP_MESSAGETYPE_NACK    = 6,
    DHCP_MESSAGETYPE_RELEASE = 7;

struct ICMP_Header_t
{
    uint8_t Type;
    uint8_t Code;
    uint16_t Checksum;
    uint16_t Id;
    uint16_t Sequence;
};

typedef struct
{
    uint16_t SourcePort;
    uint16_t DestinationPort;
    uint16_t Length;
    uint16_t Checksum;
} UDP_Header_t;

static constexpr uint32_t DHCP_MAGIC_COOKIE = 0x63825363;

struct DHCP_Header_t
{
    uint8_t Operation;
    uint8_t HardwareType;
    uint8_t HardwareAddressLength;
    uint8_t Hops;
    uint32_t TransactionID;
    uint16_t ElapsedSeconds;
    uint16_t Flags;
    IP_Address_t ClientIP;
    IP_Address_t YourIP;
    IP_Address_t NextServerIP;
    IP_Address_t RelayAgentIP;
    uint8_t ClientHardwareAddress[16];
    uint8_t ServerHostnameString[64];
    uint8_t BootFileName[128];
    uint32_t Cookie;
};

static constexpr uint16_t ETHERNET_VER2_MINSIZE = 0x0600;

struct Frame
{
    uint8_t FrameData[ETHERNET_FRAME_SIZE_MAX];
    uint16_t FrameLength;
};

typedef struct
{
    MacAddr Destination;
    MacAddr Source;
    uint16_t EtherType;
} Ethernet_Frame_Header_t;

static Frame frameIN;
static Frame frameOUT;

static constexpr uint8_t
    INTERFACE_ID_CDC_CCI = 0,
    INTERFACE_ID_CDC_DCI = 1,
    STRING_ID_Language     = 0,
    STRING_ID_Manufacturer = 1,
    STRING_ID_Product      = 2;

static const MacAddr serverMac = {{0,1,0,1,0,1}};
static const IP_Address_t ServerIPAddress     = {SERVER_IP_ADDRESS};
static const MacAddr BroadcastMACAddress = {{0xff,0xff,0xff,0xff,0xff,0xff}};
static const IP_Address_t BroadcastIPAddress  = {BROADCAST_IP_ADDRESS};
static const IP_Address_t ClientIPAddress     = {CLIENT_IP_ADDRESS};

int16_t UDP_ProcessUDPPacket(void* IPHeaderInStart, void* UDPHeaderInStart,
        void* UDPHeaderOutStart);

uint16_t RNDIS::Ethernet_Checksum16(void* Data, uint16_t Bytes)
{
    uint16_t* Words    = (uint16_t*)Data;
    uint32_t  Checksum = 0;

    for (uint16_t CurrWord = 0; CurrWord < (Bytes >> 1); CurrWord++)
        Checksum += Words[CurrWord];

    while (Checksum & 0xFFFF0000)
        Checksum = ((Checksum & 0xFFFF) + (Checksum >> 16));

    return ~Checksum;
}

int16_t RNDIS::IP_ProcessIPPacket(void* InDataStart, void* OutDataStart)
{
    IP_Header_t* IPHeaderIN  = (IP_Header_t*)InDataStart;
    IP_Header_t* IPHeaderOUT = (IP_Header_t*)OutDataStart;
    uint16_t HeaderLengthBytes = (IPHeaderIN->HeaderLength * sizeof(uint32_t));
    int16_t RetSize = 0;

    if (!(IP_COMPARE(&IPHeaderIN->DestinationAddress, &ServerIPAddress)) &&
        !(IP_COMPARE(&IPHeaderIN->DestinationAddress, &BroadcastIPAddress)))
    {
        return 0;
    }

    switch (IPHeaderIN->Protocol)
    {
    case PROTOCOL_ICMP:
        RetSize = ICMP_ProcessICMPPacket(&((uint8_t*)InDataStart)[HeaderLengthBytes],
                                         &((uint8_t*)OutDataStart)[sizeof(IP_Header_t)]);
        break;
    case PROTOCOL_UDP:
        RetSize = UDP_ProcessUDPPacket(InDataStart, &((uint8_t*)InDataStart)[HeaderLengthBytes],
                                       &((uint8_t*)OutDataStart)[sizeof(IP_Header_t)]);
        break;
    }

    if (RetSize > 0)
    {
        IPHeaderOUT->TotalLength = SWAPENDIAN_16(sizeof(IP_Header_t) + RetSize);
        IPHeaderOUT->TypeOfService = 0;
        IPHeaderOUT->HeaderLength = (sizeof(IP_Header_t) / sizeof(uint32_t));
        IPHeaderOUT->Version = 4;
        IPHeaderOUT->Flags = 0;
        IPHeaderOUT->FragmentOffset     = 0;
        IPHeaderOUT->Identification     = 0;
        IPHeaderOUT->HeaderChecksum     = 0;
        IPHeaderOUT->Protocol           = IPHeaderIN->Protocol;
        IPHeaderOUT->TTL                = DEFAULT_TTL;
        IPHeaderOUT->SourceAddress      = IPHeaderIN->DestinationAddress;
        IPHeaderOUT->DestinationAddress = IPHeaderIN->SourceAddress;
        IPHeaderOUT->HeaderChecksum = Ethernet_Checksum16(IPHeaderOUT, sizeof(IP_Header_t));
        return (sizeof(IP_Header_t) + RetSize);
    }

    return RetSize;
}

int16_t RNDIS::ARP_ProcessARPPacket(void *inStart, void *outStart)
{
    ARP_Header_t *ARPHeaderIN = (ARP_Header_t*)inStart;
    ARP_Header_t *ARPHeaderOUT = (ARP_Header_t*)outStart;

    if ((SWAPENDIAN_16(ARPHeaderIN->ProtocolType) == ETHERTYPE_IPV4) &&
        (SWAPENDIAN_16(ARPHeaderIN->Operation) == ARP_OPERATION_REQUEST))
    {
        if (IP_COMPARE(&ARPHeaderIN->TPA, &ServerIPAddress) ||
            MAC_COMPARE(&ARPHeaderIN->THA, &serverMac))
        {
            ARPHeaderOUT->HardwareType = ARPHeaderIN->HardwareType;
            ARPHeaderOUT->ProtocolType = ARPHeaderIN->ProtocolType;
            ARPHeaderOUT->HLEN = ARPHeaderIN->HLEN;
            ARPHeaderOUT->PLEN = ARPHeaderIN->PLEN;
            ARPHeaderOUT->Operation = SWAPENDIAN_16(ARP_OPERATION_REPLY);
            ARPHeaderOUT->THA = ARPHeaderIN->SHA;
            ARPHeaderOUT->TPA = ARPHeaderIN->SPA;
            ARPHeaderOUT->SHA = serverMac;
            ARPHeaderOUT->SPA = ServerIPAddress;
            return sizeof(ARP_Header_t);
        }
    }

    return 0;
}

int16_t DHCP_ProcessDHCPPacket(void* IPHeaderInStart, void* DHCPHeaderInStart,
                               void* DHCPHeaderOutStart)
{
    IP_Header_t *IPHeaderIN    = (IP_Header_t*)IPHeaderInStart;
    DHCP_Header_t *DHCPHeaderIN  = (DHCP_Header_t*)DHCPHeaderInStart;
    DHCP_Header_t *DHCPHeaderOUT = (DHCP_Header_t*)DHCPHeaderOutStart;

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

    memmove(&DHCPHeaderOUT->ClientHardwareAddress, &DHCPHeaderIN->ClientHardwareAddress,
        sizeof(MacAddr));

    DHCPHeaderOUT->Cookie = SWAPENDIAN_32(DHCP_MAGIC_COOKIE);
    IPHeaderIN->SourceAddress = ClientIPAddress;
    IPHeaderIN->DestinationAddress = ServerIPAddress;

    while (DHCPOptionsINStart[0] != DHCP_OPTION_END)
    {
        if (DHCPOptionsINStart[0] == DHCP_OPTION_MESSAGETYPE)
        {
            if ((DHCPOptionsINStart[2] == DHCP_MESSAGETYPE_DISCOVER) ||
                (DHCPOptionsINStart[2] == DHCP_MESSAGETYPE_REQUEST))
            {
                *(DHCPOptionsOUTStart++) = DHCP_OPTION_MESSAGETYPE;
                *(DHCPOptionsOUTStart++) = 1;

                *(DHCPOptionsOUTStart++) = (DHCPOptionsINStart[2] == DHCP_MESSAGETYPE_DISCOVER)
                    ? DHCP_MESSAGETYPE_OFFER : DHCP_MESSAGETYPE_ACK;

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

    return 0;
}

int16_t UDP_ProcessUDPPacket(void* IPHeaderInStart, void* UDPHeaderInStart,
                             void* UDPHeaderOutStart)
{
    UDP_Header_t* UDPHeaderIN  = (UDP_Header_t*)UDPHeaderInStart;
    UDP_Header_t* UDPHeaderOUT = (UDP_Header_t*)UDPHeaderOutStart;
    int16_t RetSize = 0;

    switch (SWAPENDIAN_16(UDPHeaderIN->DestinationPort))
    {
    case UDP_PORT_DHCP_REQUEST:
        RetSize = DHCP_ProcessDHCPPacket(IPHeaderInStart,
                                &((uint8_t*)UDPHeaderInStart)[sizeof(UDP_Header_t)],
                                &((uint8_t*)UDPHeaderOutStart)[sizeof(UDP_Header_t)]);
        break;
    }

    if (RetSize > 0)
    {
        UDPHeaderOUT->SourcePort      = UDPHeaderIN->DestinationPort;
        UDPHeaderOUT->DestinationPort = UDPHeaderIN->SourcePort;
        UDPHeaderOUT->Checksum        = 0;
        UDPHeaderOUT->Length          = SWAPENDIAN_16(sizeof(UDP_Header_t) + RetSize);
        return (sizeof(UDP_Header_t) + RetSize);
    }

    return 0;
}

int16_t RNDIS::ICMP_ProcessICMPPacket(void* InDataStart, void* OutDataStart)
{
    ICMP_Header_t* ICMPHeaderIN  = (ICMP_Header_t*)InDataStart;
    ICMP_Header_t* ICMPHeaderOUT = (ICMP_Header_t*)OutDataStart;

    if (ICMPHeaderIN->Type == ICMP_TYPE_ECHOREQUEST)
    {
        ICMPHeaderOUT->Type     = ICMP_TYPE_ECHOREPLY;
        ICMPHeaderOUT->Code     = 0;
        ICMPHeaderOUT->Checksum = 0;
        ICMPHeaderOUT->Id       = ICMPHeaderIN->Id;
        ICMPHeaderOUT->Sequence = ICMPHeaderIN->Sequence;

        intptr_t DataSize = frameIN.FrameLength - ((((intptr_t)InDataStart +
            sizeof(ICMP_Header_t)) - (intptr_t)frameIN.FrameData));

        memmove(&((uint8_t*)OutDataStart)[sizeof(ICMP_Header_t)],
                &((uint8_t*)InDataStart)[sizeof(ICMP_Header_t)],
                DataSize);

        ICMPHeaderOUT->Checksum = Ethernet_Checksum16(ICMPHeaderOUT,
            (DataSize + sizeof(ICMP_Header_t)));

        return (DataSize + sizeof(ICMP_Header_t));
    }

    return 0;
}

void RNDIS::Ethernet_ProcessPacket()
{
    Ethernet_Frame_Header_t *inHeader = (Ethernet_Frame_Header_t*)&frameIN.FrameData;
    Ethernet_Frame_Header_t *FrameOUTHeader = (Ethernet_Frame_Header_t*)&frameOUT.FrameData;
    int16_t retSize = 0;

    if ((MAC_COMPARE(&inHeader->Destination, &serverMac) ||
         MAC_COMPARE(&inHeader->Destination, &BroadcastMACAddress)) &&
        (SWAPENDIAN_16(frameIN.FrameLength) > ETHERNET_VER2_MINSIZE))
    {
        switch (SWAPENDIAN_16(inHeader->EtherType))
        {
        case ETHERTYPE_ARP:
            retSize = ARP_ProcessARPPacket(&frameIN.FrameData[sizeof(Ethernet_Frame_Header_t)],
                                 &frameOUT.FrameData[sizeof(Ethernet_Frame_Header_t)]);
            break;
        case ETHERTYPE_IPV4:
            retSize = IP_ProcessIPPacket(&frameIN.FrameData[sizeof(Ethernet_Frame_Header_t)],
                                   &frameOUT.FrameData[sizeof(Ethernet_Frame_Header_t)]);
            break;
        }

        if (retSize > 0)
        {
            FrameOUTHeader->Source = serverMac;
            FrameOUTHeader->Destination = inHeader->Source;
            FrameOUTHeader->EtherType = inHeader->EtherType;
            frameOUT.FrameLength = (sizeof(Ethernet_Frame_Header_t) + retSize);
        }
    }

    if (retSize != -1)
        frameIN.FrameLength = 0;
}

static const DescDev PROGMEM DeviceDescriptor =
{
    sizeof(DescDev),
    DTYPE_DEVICE,
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

struct USB_Descriptor_Configuration_t
{
    DescConf config;
    DescIface CDC_CCI_Interface;
    USB_CDC_Descriptor_FunctionalHeader_t CDC_Functional_Header;
    USB_CDC_Descriptor_FunctionalACM_t    CDC_Functional_ACM;
    USB_CDC_Descriptor_FunctionalUnion_t  CDC_Functional_Union;
    DescEndpoint             CDC_NotificationEndpoint;
    DescIface CDC_DCI_Interface;
    DescEndpoint RNDIS_DataOutEndpoint;
    DescEndpoint RNDIS_DataInEndpoint;
};

static const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor =
{
    {
        sizeof(DescConf),
        DTYPE_CONFIGURATION,
        sizeof(USB_Descriptor_Configuration_t),
        2,
        1,
        0,
        USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_SELFPOWERED,
        USB_CONFIG_POWER_MA(100)
    },
    {
        sizeof(DescIface),
        DTYPE_INTERFACE,
        INTERFACE_ID_CDC_CCI,
        0,
        1,
        CDC_CSCP_CDCClass,
        CDC_CSCP_ACMSubclass,
        CDC_CSCP_VendorSpecificProtocol,
        0
    },
    {
        sizeof(USB_CDC_Descriptor_FunctionalHeader_t),
        DTYPE_CSINTERFACE,
        CDC_DSUBTYPE_CSInterface_Header,
        0x0110,
    },
    {
        sizeof(USB_CDC_Descriptor_FunctionalACM_t),
        DTYPE_CSINTERFACE,
        CDC_DSUBTYPE_CSInterface_ACM,
        0x00,
    },
    {
        sizeof(USB_CDC_Descriptor_FunctionalUnion_t),
        DTYPE_CSINTERFACE,
        CDC_DSUBTYPE_CSInterface_Union,
        INTERFACE_ID_CDC_CCI,
        INTERFACE_ID_CDC_DCI,
    },
    {
        sizeof(DescEndpoint),
        DTYPE_ENDPOINT,
        CDC_NOTIFICATION_EPADDR,
        (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
        CDC_NOTIFICATION_EPSIZE,
        0xFF
    },
    {
        sizeof(DescIface),
        DTYPE_INTERFACE,
        INTERFACE_ID_CDC_DCI,
        0,
        2,
        CDC_CSCP_CDCDataClass,
        CDC_CSCP_NoDataSubclass,
        CDC_CSCP_NoDataProtocol,
        0
    },
    {
        sizeof(DescEndpoint),
        DTYPE_ENDPOINT,
        CDC_RX_EPADDR,
        (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
        CDC_TXRX_EPSIZE,
        0x05
    },
    {
        sizeof(DescEndpoint),
        DTYPE_ENDPOINT,
        CDC_TX_EPADDR,
        EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA,
        CDC_TXRX_EPSIZE,
        0x05
    }
};

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
            const uint16_t wIndex, const void** const DescriptorAddress)
{
    const uint8_t  DescriptorType   = (wValue >> 8);
    const uint8_t  DescriptorNumber = (wValue & 0xFF);

    const void* Address = NULL;
    uint16_t    Size    = 0;

    switch (DescriptorType)
    {
    case DTYPE_DEVICE:
        Address = &DeviceDescriptor;
        Size = sizeof(DescDev);
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

void RNDIS::rndisTask()
{
	Endpoint_SelectEndpoint(CDC_NOTIFICATION_EPADDR);

	if ((UEINTX & 1<<TXINI) && ResponseReady)
	{
		USBRequest Notification = (USBRequest)
        {
			.bmRequestType = (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE),
			.bRequest      = RNDIS_NOTIF_ResponseAvailable,
			.wValue        = 0,
			.wIndex        = 0,
			.wLength       = 0,
        };

		writeStream2(&Notification, sizeof(Notification), NULL);
		Endpoint_ClearIN();
		ResponseReady = false;
	}

	if ((CurrRNDISState == RNDIS_Data_Initialized) && !(MessageHeader->MessageLength))
	{
		RNDIS_Packet_Message_t RNDISPacketHeader;
		Endpoint_SelectEndpoint(CDC_RX_EPADDR);

		if (Endpoint_IsOUTReceived() && !(frameIN.FrameLength))
		{
			readStream(&RNDISPacketHeader, sizeof(RNDIS_Packet_Message_t), NULL);

			if (RNDISPacketHeader.DataLength > ETHERNET_FRAME_SIZE_MAX)
			{
                UECONX |= 1<<STALLRQ;
				return;
			}

			readStream(frameIN.FrameData, RNDISPacketHeader.DataLength, NULL);
			Endpoint_ClearOUT();
			frameIN.FrameLength = RNDISPacketHeader.DataLength;
		}

		Endpoint_SelectEndpoint(CDC_TX_EPADDR);

		if ((UEINTX & 1<<TXINI) && frameOUT.FrameLength)
		{
			memset(&RNDISPacketHeader, 0, sizeof(RNDIS_Packet_Message_t));
			RNDISPacketHeader.MessageType   = REMOTE_NDIS_PACKET_MSG;

			RNDISPacketHeader.MessageLength = (sizeof(RNDIS_Packet_Message_t) +
                frameOUT.FrameLength);

			RNDISPacketHeader.DataOffset = (sizeof(RNDIS_Packet_Message_t) -
                sizeof(RNDIS_Message_Header_t));

			RNDISPacketHeader.DataLength    = frameOUT.FrameLength;
			writeStream2(&RNDISPacketHeader, sizeof(RNDIS_Packet_Message_t), NULL);
			writeStream2(frameOUT.FrameData, RNDISPacketHeader.DataLength, NULL);
			Endpoint_ClearIN();
			frameOUT.FrameLength = 0;
		}
	}
}

void RNDIS::ethTask()
{
	if (USB_DeviceState != DEVICE_STATE_Configured)
        return;

	if (frameIN.FrameLength)
		Ethernet_ProcessPacket();
}

ISR(USB_GEN_vect, ISR_BLOCK)
{
    Busby::instance->gen();
}

void Busby::gen()
{
    if (*p_udint & 1<<sofi && *p_udien & 1<<sofe)
        UDINT &= ~(1<<SOFI);

    if (*p_usbint & 1<<vbusti && (USBCON & 1<<VBUSTE))
    {
        USBINT &= ~(1<<VBUSTI);

        if (USB_VBUS_GetStatus())
        {
            if (!(USB_Options & USB_OPT_MANUAL_PLL))
            {
                PLLCSR = USB_PLL_PSC;
                PLLCSR = USB_PLL_PSC | 1<<PLLE;
                while (USB_PLL_IsReady() == 0);
            }

            USB_DeviceState = DEVICE_STATE_POWERED;
        }
        else
        {
            if (!(USB_Options & USB_OPT_MANUAL_PLL))
                PLLCSR = 0;

            USB_DeviceState = DEVICE_STATE_UNATTACHED;
        }
    }

    if (*p_udint & 1<<suspi && (UDIEN & 1<<SUSPE))
    {
        UDIEN &= ~(1<<SUSPE);
        UDIEN |= 1<<WAKEUPE;
        USB_CLK_Freeze();

        if (!(USB_Options & USB_OPT_MANUAL_PLL))
            PLLCSR = 0;

        USB_DeviceState = DEVICE_STATE_SUSPENDED;
    }

    if (*p_udint & 1<<wakeupi && (UDIEN & 1<<WAKEUPE))
    {
        if (!(USB_Options & USB_OPT_MANUAL_PLL))
        {
            PLLCSR = USB_PLL_PSC;
            PLLCSR = USB_PLL_PSC | 1<<PLLE;
            while (!(USB_PLL_IsReady()));
        }

        USBCON &= ~(1<<FRZCLK);
        UDINT &= ~(1<<WAKEUPI);
        UDIEN &= ~(1<<WAKEUPE);
        UDIEN |= 1<<SUSPE;

        if (USB_Device_ConfigurationNumber)
            USB_DeviceState = DEVICE_STATE_Configured;
        else
            USB_DeviceState = USB_Device_IsAddressSet() ? DEVICE_STATE_ADDRESSED :
                DEVICE_STATE_POWERED;

    }

    if (*p_udint & 1<<eorsti && (UDIEN & 1<<EORSTE))
    {
        UDINT &= ~(1<<EORSTI);
        USB_DeviceState = DEVICE_STATE_Default;
        USB_Device_ConfigurationNumber = 0;
        UDINT &= ~(1<<SUSPI);
        UDIEN &= ~(1<<SUSPE);
        UDIEN |= 1<<WAKEUPE;
        Endpoint_ConfigureEndpoint(0, 0, USB_Device_ControlEndpointSize, 1);
    }
}

ISR(USB_COM_vect, ISR_BLOCK)
{
    Busby::instance->com();
}

void Busby::com()
{
    uint8_t PrevSelectedEndpoint = Endpoint_GetCurrentEndpoint();
    Endpoint_SelectEndpoint(0);
    *p_ueienx &= ~(1<<rxstpe);
    sei();
    procCtrlReq();
    Endpoint_SelectEndpoint(0);
    *p_ueienx |= 1<<rxstpe;
    Endpoint_SelectEndpoint(PrevSelectedEndpoint);
}

int main(void)
{
    RNDIS rndis;
    rndis.USB_Init();
    sei();

    while (true)
    {
        rndis.ethTask();
        rndis.rndisTask();
        rndis.USB_DeviceTask();
    }
}


