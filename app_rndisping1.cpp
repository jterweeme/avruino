#include <avr/pgmspace.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <util/delay.h>
#include "busby.h"

#define NULL 0

#define SWAPENDIAN_16(x)   (uint16_t)((((x) & 0xFF00) >> 8) | (((x) & 0x00FF) << 8))

#define USB_OPT_REG_DISABLED               (1 << 1)
#define USB_OPT_REG_ENABLED                (0 << 1)
#define USB_OPT_REG_KEEP_ENABLED           (1 << 3)
#define USB_OPT_MANUAL_PLL                 (1 << 2)
#define USB_OPT_AUTO_PLL                   (0 << 2)
#define USB_STREAM_TIMEOUT_MS       100

#define USB_DEVICE_OPT_LOWSPEED            (1 << 0)
#define USB_DEVICE_OPT_FULLSPEED               (0 << 0)

#define USE_STATIC_OPTIONS (USB_DEVICE_OPT_FULLSPEED | USB_OPT_REG_ENABLED | USB_OPT_AUTO_PLL)
#define USB_Options USE_STATIC_OPTIONS

#define FIXED_CONTROL_ENDPOINT_SIZE      8
#define USB_Device_ControlEndpointSize FIXED_CONTROL_ENDPOINT_SIZE

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

static constexpr uint8_t
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
    CDC_DSUBTYPE_CSInterface_ACM = 0x02,
    CDC_DSUBTYPE_CSInterface_Union = 0x06,
    CDC_PARITY_None  = 0,
    CDC_PARITY_Odd   = 1,
    CDC_PARITY_Even  = 2,
    CDC_PARITY_Mark  = 3,
    CDC_PARITY_Space = 4,
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

typedef struct
{
    uint8_t  Address;
    uint16_t Size;
    uint8_t  Type;
    uint8_t  Banks;
} USB_Endpoint_Table_t;

struct USB_CDC_Descriptor_FunctionalHeader_t
{
    uint8_t size;
    uint8_t type;
    uint8_t  Subtype;
    uint16_t CDCSpecification;
} __attribute__ ((packed));

struct USB_CDC_StdDescriptor_FunctionalHeader_t
{
    uint8_t  bFunctionLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubType;
    uint16_t bcdCDC;
} __attribute__ ((packed));

struct USB_CDC_Descriptor_FunctionalACM_t
{
    uint8_t size;
    uint8_t type;
    uint8_t Subtype;
    uint8_t Capabilities;
} __attribute__ ((packed));

typedef struct
{
    uint8_t bFunctionLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubType;
    uint8_t bmCapabilities;
} __attribute__ ((packed)) USB_CDC_StdDescriptor_FunctionalACM_t;

typedef struct
{
    uint8_t size;
    uint8_t type;
    uint8_t Subtype;
    uint8_t MasterInterfaceNumber;
    uint8_t SlaveInterfaceNumber;
} __attribute__ ((packed)) USB_CDC_Descriptor_FunctionalUnion_t;

struct USB_CDC_StdDescriptor_FunctionalUnion_t
{
    uint8_t bFunctionLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubType;
    uint8_t bMasterInterface;
    uint8_t bSlaveInterface0;
} __attribute__ ((packed));

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
    OID_GEN_SUPPORTED_LIST         = 0x00010101,
    OID_GEN_HARDWARE_STATUS        = 0x00010102,
    OID_GEN_MEDIA_SUPPORTED        = 0x00010103,
    OID_GEN_MEDIA_IN_USE           = 0x00010104,
    OID_GEN_MAXIMUM_FRAME_SIZE     = 0x00010106,
    OID_GEN_LINK_SPEED             = 0x00010107,
    OID_GEN_TRANSMIT_BLOCK_SIZE    = 0x0001010A,
    OID_GEN_RECEIVE_BLOCK_SIZE     = 0x0001010B,
    OID_GEN_VENDOR_ID              = 0x0001010C,
    OID_GEN_VENDOR_DESCRIPTION     = 0x0001010D,
    OID_GEN_CURRENT_PACKET_FILTER  = 0x0001010E,
    OID_GEN_MAXIMUM_TOTAL_SIZE     = 0x00010111,
    OID_GEN_MEDIA_CONNECT_STATUS   = 0x00010114,
    OID_GEN_PHYSICAL_MEDIUM        = 0x00010202,
    OID_GEN_XMIT_OK                = 0x00020101,
    OID_GEN_RCV_OK                 = 0x00020102,
    OID_GEN_XMIT_ERROR             = 0x00020103,
    OID_GEN_RCV_ERROR              = 0x00020104,
    OID_GEN_RCV_NO_BUFFER          = 0x00020105,
    OID_802_3_PERMANENT_ADDRESS    = 0x01010101,
    OID_802_3_CURRENT_ADDRESS      = 0x01010102,
    OID_802_3_MULTICAST_LIST       = 0x01010103,
    OID_802_3_MAXIMUM_LIST_SIZE    = 0x01010104,
    OID_802_3_RCV_ERROR_ALIGNMENT  = 0x01020101,
    OID_802_3_XMIT_ONE_COLLISION   = 0x01020102,
    OID_802_3_XMIT_MORE_COLLISIONS = 0x01020103;

static inline int myMemcmp(const void* s1, const void* s2,size_t n)
{
    const uint8_t *p1 = (const uint8_t *)s1, *p2 = (const uint8_t *)s2;

    while (n--)
    {
        if (*p1 != *p2)
            return *p1 - *p2;

        p1++,p2++;
    }

    return 0;
}

#define MAC_COMPARE(MAC1, MAC2)          (myMemcmp(MAC1, MAC2, sizeof(MacAddr)) == 0)

struct MacAddr
{
    uint8_t Octets[6];
} __attribute__ ((packed));

typedef struct
{
    uint32_t MessageType;
    uint32_t MessageLength;
} __attribute__ ((packed)) RNDIS_Message_Header_t;

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
} __attribute__ ((packed)) RNDIS_Packet_Message_t;

typedef struct
{
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t RequestId;
    uint32_t MajorVersion;
    uint32_t MinorVersion;
    uint32_t MaxTransferSize;
} __attribute__ ((packed)) RNDIS_Initialize_Message_t;

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
} __attribute__ ((packed)) RNDIS_Initialize_Complete_t;

typedef struct
{
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t RequestId;
} __attribute__ ((packed)) RNDIS_KeepAlive_Message_t;

typedef struct
{
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t RequestId;
    uint32_t Status;
} __attribute__ ((packed)) RNDIS_KeepAlive_Complete_t;

typedef struct
{
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t Status;
    uint32_t AddressingReset;
} __attribute__ ((packed)) RNDIS_Reset_Complete_t;

typedef struct
{
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t RequestId;
    uint32_t Oid;
    uint32_t InformationBufferLength;
    uint32_t InformationBufferOffset;
    uint32_t DeviceVcHandle;
} __attribute__ ((packed)) RNDIS_Set_Message_t;

typedef struct
{
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t RequestId;
    uint32_t Status;
} __attribute__ ((packed)) RNDIS_Set_Complete_t;

typedef struct
{
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t RequestId;
    uint32_t Oid;
    uint32_t InformationBufferLength;
    uint32_t InformationBufferOffset;
    uint32_t DeviceVcHandle;
} __attribute__ ((packed)) RNDIS_Query_Message_t;

typedef struct
{
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t RequestId;
    uint32_t Status;
    uint32_t InformationBufferLength;
    uint32_t InformationBufferOffset;
} __attribute__ ((packed)) RNDIS_Query_Complete_t;

#define RNDIS_DEVICE_MIN_MESSAGE_BUFFER_LENGTH  sizeof(AdapterSupportedOIDList) + \
    sizeof(RNDIS_Query_Complete_t)

#define CDC_TX_EPADDR                  (ENDPOINT_DIR_IN  | 1)
#define CDC_RX_EPADDR                  (ENDPOINT_DIR_OUT | 2)
#define CDC_NOTIFICATION_EPADDR        (ENDPOINT_DIR_IN  | 3)
#define CDC_TXRX_EPSIZE                64
#define CDC_NOTIFICATION_EPSIZE        8



class RNDIS : public USB
{
private:
    Endpoint _inpoint;
    Endpoint _outpoint;
    Endpoint _notif;
public:
    RNDIS();
    void customCtrl();
    void configure();
    void rndisTask();
    void ethTask();
    void ProcessRNDISControlMessage();
    int16_t ICMP_ProcessICMPPacket(void* InDataStart, void* OutDataStart);
    int16_t IP_ProcessIPPacket(void* InDataStart, void* OutDataStart);
    void Ethernet_ProcessPacket();
    bool ProcessNDISSet(uint32_t OId, void* SetData, uint16_t SetSize);
    uint16_t Ethernet_Checksum16(void* Data, uint16_t Bytes);
    int16_t ARP_ProcessARPPacket(void *inStart, void *outStart);
    bool ResponseReady = false;
    uint16_t getDesc(uint16_t wValue, uint16_t wIndex, const void ** const descAddr);
    void usbTask();

    bool ProcessNDISQuery(const uint32_t OId, void* QueryData, uint16_t QuerySize,
                             void* ResponseData, uint16_t* ResponseSize);
};

RNDIS::RNDIS() :
    _inpoint(CDC_TX_EPADDR, EP_TYPE_BULK, CDC_TXRX_EPSIZE, 1),
    _outpoint(CDC_RX_EPADDR, EP_TYPE_BULK, CDC_TXRX_EPSIZE, 1),
    _notif(CDC_NOTIFICATION_EPADDR, EP_TYPE_INTERRUPT, CDC_NOTIFICATION_EPSIZE, 1)
{
    *p_usbcon &= ~(1<<otgpade);

    if (!(USB_Options & USB_OPT_REG_DISABLED))
        UHWCON |= 1<<UVREGE;
    else
        UHWCON &= ~(1<<UVREGE);

    if ((USB_Options & USB_OPT_MANUAL_PLL) == 0)
        PLLFRQ = 1<<PDIV2;

    USBCON &= ~(1<<VBUSTE);
    UDIEN = 0;
    USBINT = 0;
    UDINT  = 0;
    USBCON &= ~(1 << USBE);
    USBCON |=  (1 << USBE);
    USBCON &= ~(1<<FRZCLK);
    PLLCSR = 0;
    state = DEVICE_STATE_UNATTACHED;
    confNum = 0;
    _remoteWakeupEn = false;
    USB_Device_CurrentlySelfPowered = false;
    UDCON &= ~(1<<LSM);
    USBCON |= 1<<VBUSTE;
    configureEndpoint(_control);
    UDINT &= ~(1<<SUSPI);
    UDIEN |= 1<<SUSPE;
    UDIEN |= 1<<EORSTE;
    UDCON &= ~(1<<DETACH);
    USBCON |= 1<<OTGPADE;
}

#define LUFA_VERSION_RELEASE_TYPE_DEVELOPMENT   0
#define LUFA_VERSION_RELEASE_TYPE_BETA          1
#define LUFA_VERSION_RELEASE_TYPE_FULL          2
#define LUFA_VERSION_INTEGER              0x170418
#define LUFA_VERSION_STRING               "170418"
#define LUFA_VERSION_RELEASE_TYPE         LUFA_VERSION_RELEASE_TYPE_FULL

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

#ifndef PROGMEM
#define PROGMEM __attribute__ ((__progmem__))
#endif

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

void EVENT_USB_Device_ConfigurationChanged(void)
{
	Endpoint_ConfigureEndpoint(CDC_TX_EPADDR, EP_TYPE_BULK, CDC_TXRX_EPSIZE, 1);
	Endpoint_ConfigureEndpoint(CDC_RX_EPADDR, EP_TYPE_BULK, CDC_TXRX_EPSIZE, 1);
	Endpoint_ConfigureEndpoint(CDC_NOTIFICATION_EPADDR, EP_TYPE_INTERRUPT, CDC_NOTIFICATION_EPSIZE, 1);
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

        void *ResponseData = &RNDISMessageBuffer[sizeof(RNDIS_Query_Complete_t)];
        uint16_t ResponseSize;

        QUERY_Response->MessageType = REMOTE_NDIS_QUERY_CMPLT;
        QUERY_Response->MessageLength = sizeof(RNDIS_Query_Complete_t);

        if (ProcessNDISQuery(Query_Oid, QueryData, QUERY_Message->InformationBufferLength,
                             ResponseData, &ResponseSize))
        {
            QUERY_Response->Status = REMOTE_NDIS_STATUS_SUCCESS;
            QUERY_Response->MessageLength += ResponseSize;
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
        RNDIS_Set_Message_t *SET_Message = (RNDIS_Set_Message_t*)&RNDISMessageBuffer;
        RNDIS_Set_Complete_t *SET_Response = (RNDIS_Set_Complete_t*)&RNDISMessageBuffer;
        uint32_t SET_Oid = SET_Message->Oid;
        SET_Response->MessageType = REMOTE_NDIS_SET_CMPLT;
        SET_Response->MessageLength = sizeof(RNDIS_Set_Complete_t);
        SET_Response->RequestId = SET_Message->RequestId;

        void *SetData = &RNDISMessageBuffer[sizeof(RNDIS_Message_Header_t) +
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
        KEEPALIVE_Message = (RNDIS_KeepAlive_Message_t*)&RNDISMessageBuffer;

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
	switch (_ctrlReq.bRequest)
	{
    case RNDIS_REQ_SendEncapsulatedCommand:
        if (_ctrlReq.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS |
            REQREC_INTERFACE))
        {
            *p_ueintx &= ~(1<<rxstpi);
            readControlStreamLE(RNDISMessageBuffer, _ctrlReq.wLength);
            *p_ueintx &= ~(1<<txini | 1<<fifocon);
            ProcessRNDISControlMessage();
        }

        break;
    case RNDIS_REQ_GetEncapsulatedResponse:
        if (_ctrlReq.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS |
                REQREC_INTERFACE))
        {
            if (!(MessageHeader->MessageLength))
            {
                RNDISMessageBuffer[0] = 0;
                MessageHeader->MessageLength = 1;
            }

            *p_ueintx &= ~(1<<rxstpi);
            write_Control_Stream_LE(RNDISMessageBuffer, MessageHeader->MessageLength);
            *p_ueintx &= ~(1<<rxouti | 1<<fifocon);
            MessageHeader->MessageLength = 0;
        }
        break;
	}
}

uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue, const uint16_t wIndex,
       const void** const DescriptorAddress);

uint16_t RNDIS::getDesc(uint16_t wValue, uint16_t wIndex, const void ** const descAddress)
{
    return CALLBACK_USB_GetDescriptor(wValue, wIndex, descAddress);
}

void RNDIS::configure()
{
#if 0
    configureEndpoint(_inpoint);
    configureEndpoint(_outpoint);
    configureEndpoint(_notif);
#endif
    EVENT_USB_Device_ConfigurationChanged();
}

typedef struct
{
    uint8_t Octets[6];
} RNDIS_MAC_Address_t;

typedef struct
{
    uint8_t Octets[4];
} IP_Address_t;


#define DEFAULT_TTL                      128
#define IP_COMPARE(IP1, IP2)             (myMemcmp(IP1, IP2, sizeof(IP_Address_t)) == 0)

typedef struct
{
    unsigned     HeaderLength   : 4;
    unsigned     Version        : 4;
    uint8_t      TypeOfService;
    uint16_t     TotalLength;
    uint16_t     Identification;
    unsigned     FragmentOffset : 13;
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
    DHCP_OPTION_SUBNETMASK = 1,
    DHCP_OPTION_LEASETIME = 51,
    DHCP_OPTION_MESSAGETYPE = 53,
    DHCP_OPTION_DHCPSERVER = 54,
    DHCP_OPTION_PAD = 0,
    DHCP_OPTION_END = 255,
    DHCP_MESSAGETYPE_DISCOVER = 1,
    DHCP_MESSAGETYPE_OFFER = 2,
    DHCP_MESSAGETYPE_REQUEST = 3,
    DHCP_MESSAGETYPE_DECLINE = 4,
    DHCP_MESSAGETYPE_ACK = 5,
    DHCP_MESSAGETYPE_NACK = 6,
    DHCP_MESSAGETYPE_RELEASE = 7;

struct ICMP_Header_t
{
    uint8_t Type;
    uint8_t Code;
    uint16_t Checksum;
    uint16_t Id;
    uint16_t Sequence;
};

static constexpr uint16_t ETHERNET_VER2_MINSIZE = 0x0600;

struct Frame
{
    uint8_t FrameData[ETHERNET_FRAME_SIZE_MAX];
    uint16_t len;
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
    STRING_ID_Language = 0,
    STRING_ID_Manufacturer = 1,
    STRING_ID_Product = 2;

static const MacAddr serverMac = {{0,1,0,1,0,1}};
static const IP_Address_t ServerIPAddress = {{10, 0, 0, 2}};
static const MacAddr BroadcastMACAddress = {{0xff,0xff,0xff,0xff,0xff,0xff}};
static const IP_Address_t BroadcastIPAddress = {{0xFF, 0xFF, 0xFF, 0xFF}};
static const IP_Address_t ClientIPAddress = {{10, 0, 0, 1}};

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
    }

    if (RetSize > 0)
    {
        IPHeaderOUT->TotalLength = SWAPENDIAN_16(sizeof(IP_Header_t) + RetSize);
        IPHeaderOUT->TypeOfService = 0;
        IPHeaderOUT->HeaderLength = (sizeof(IP_Header_t) / sizeof(uint32_t));
        IPHeaderOUT->Version = 4;
        IPHeaderOUT->Flags = 0;
        IPHeaderOUT->FragmentOffset = 0;
        IPHeaderOUT->Identification = 0;
        IPHeaderOUT->HeaderChecksum = 0;
        IPHeaderOUT->Protocol = IPHeaderIN->Protocol;
        IPHeaderOUT->TTL = DEFAULT_TTL;
        IPHeaderOUT->SourceAddress = IPHeaderIN->DestinationAddress;
        IPHeaderOUT->DestinationAddress = IPHeaderIN->SourceAddress;
        IPHeaderOUT->HeaderChecksum = Ethernet_Checksum16(IPHeaderOUT, sizeof(IP_Header_t));
        return sizeof(IP_Header_t) + RetSize;
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

static inline void *myMemcpy(void *dest, const void *src, size_t n)
{
    char *dp = (char *)dest;
    const char *sp = (const char *)src;

    while (n--)
        *dp++ = *sp++;

    return dest;
}

static inline void *myMemmove(void *dest, const void *src, size_t n)
{
    return myMemcpy(dest, src, n);
}

int16_t RNDIS::ICMP_ProcessICMPPacket(void* InDataStart, void* OutDataStart)
{
    ICMP_Header_t *ICMPHeaderIN = (ICMP_Header_t*)InDataStart;
    ICMP_Header_t *ICMPHeaderOUT = (ICMP_Header_t*)OutDataStart;

    if (ICMPHeaderIN->Type == ICMP_TYPE_ECHOREQUEST)
    {
        ICMPHeaderOUT->Type = ICMP_TYPE_ECHOREPLY;
        ICMPHeaderOUT->Code = 0;
        ICMPHeaderOUT->Checksum = 0;
        ICMPHeaderOUT->Id = ICMPHeaderIN->Id;
        ICMPHeaderOUT->Sequence = ICMPHeaderIN->Sequence;

        intptr_t DataSize = frameIN.len - ((((intptr_t)InDataStart +
            sizeof(ICMP_Header_t)) - (intptr_t)frameIN.FrameData));

        myMemmove(&((uint8_t*)OutDataStart)[sizeof(ICMP_Header_t)],
                &((uint8_t*)InDataStart)[sizeof(ICMP_Header_t)], DataSize);

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
        (SWAPENDIAN_16(frameIN.len) > ETHERNET_VER2_MINSIZE))
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
            frameOUT.len = (sizeof(Ethernet_Frame_Header_t) + retSize);
        }
    }

    if (retSize != -1)
        frameIN.len = 0;
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
    USB_CDC_Descriptor_FunctionalACM_t CDC_Functional_ACM;
    USB_CDC_Descriptor_FunctionalUnion_t CDC_Functional_Union;
    DescEndpoint CDC_NotificationEndpoint;
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

static inline void *myMemset(void *s, int c, size_t n)
{
    uint8_t *p = (uint8_t *)s;

    while (n--)
        *p++ = (uint8_t)c;

    return s;
}

void RNDIS::rndisTask()
{
	selectEndpoint(CDC_NOTIFICATION_EPADDR);

	if ((*p_ueintx & 1<<txini) && ResponseReady)
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
        *p_ueintx &= ~(1<<txini | 1<<fifocon);
		ResponseReady = false;
	}

	if ((CurrRNDISState == RNDIS_Data_Initialized) && !(MessageHeader->MessageLength))
	{
		RNDIS_Packet_Message_t RNDISPacketHeader;
		selectEndpoint(CDC_RX_EPADDR);

		if ((*p_ueintx & 1<<rxouti) && !frameIN.len)
		{
			readStream(&RNDISPacketHeader, sizeof(RNDIS_Packet_Message_t), NULL);

			if (RNDISPacketHeader.DataLength > ETHERNET_FRAME_SIZE_MAX)
			{
                *p_ueconx |= 1<<stallrq;
				return;
			}

			readStream(frameIN.FrameData, RNDISPacketHeader.DataLength, NULL);
            *p_ueintx &= ~(1<<rxouti | 1<<fifocon);
			frameIN.len = RNDISPacketHeader.DataLength;
		}

        selectEndpoint(CDC_TX_EPADDR);

        if ((*p_ueintx & 1<<txini) && frameOUT.len)
        {
            myMemset(&RNDISPacketHeader, 0, sizeof(RNDIS_Packet_Message_t));
            RNDISPacketHeader.MessageType   = REMOTE_NDIS_PACKET_MSG;
            RNDISPacketHeader.MessageLength = sizeof(RNDIS_Packet_Message_t) + frameOUT.len;

			RNDISPacketHeader.DataOffset = (sizeof(RNDIS_Packet_Message_t) -
                sizeof(RNDIS_Message_Header_t));

			RNDISPacketHeader.DataLength    = frameOUT.len;
			writeStream2(&RNDISPacketHeader, sizeof(RNDIS_Packet_Message_t), NULL);
			writeStream2(frameOUT.FrameData, RNDISPacketHeader.DataLength, NULL);
            *p_ueintx &= ~(1<<txini | 1<<fifocon);
			frameOUT.len = 0;
		}
	}
}

void RNDIS::ethTask()
{
	if (state != DEVICE_STATE_Configured)
        return;

	if (frameIN.len)
		Ethernet_ProcessPacket();
}

void RNDIS::usbTask()
{
    if (state == DEVICE_STATE_UNATTACHED)
        return;

    uint8_t PrevEndpoint = getCurrentEndpoint();
    selectEndpoint(0);

    if (*p_ueintx & 1<<rxstpi)
        procCtrlReq();

    selectEndpoint(PrevEndpoint);
}

extern "C" void USB_COM __attribute__ ((signal, used, externally_visible));
void USB_COM
{
    USB::instance->com();
}

extern "C" void USB_GEN __attribute__ ((signal, used, externally_visible));
void USB_GEN
{
    USB::instance->gen();
}

int main(void)
{
    RNDIS rndis;
    zei();

    while (true)
    {
        rndis.ethTask();
        rndis.rndisTask();
        rndis.usbTask();
    }
}


