#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <util/delay.h>
#include "busby.h"
#include "w5100hw.h"
#include "misc.h"
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

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

struct USB_CDC_StdDescriptor_FunctionalACM_t
{
    uint8_t bFunctionLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubType;
    uint8_t bmCapabilities;
} __attribute__ ((packed));

struct USB_CDC_Descriptor_FunctionalUnion_t
{
    uint8_t size;
    uint8_t type;
    uint8_t Subtype;
    uint8_t MasterInterfaceNumber;
    uint8_t SlaveInterfaceNumber;
} __attribute__ ((packed));

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

#define MAC_COMPARE(MAC1, MAC2)          (memcmp(MAC1, MAC2, sizeof(MacAddr)) == 0)

struct MacAddr
{
    uint8_t Octets[6];
} __attribute__ ((packed));

struct RNDIS_Message_Header_t
{
    uint32_t MessageType;
    uint32_t MessageLength;
} __attribute__ ((packed));

struct RNDIS_Packet_Message_t
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
} __attribute__ ((packed));

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

//Busby *Busby::instance;

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
    W5100Class _w5100;
    //Enc28J60Network _eth;
public:
    RNDIS();
    void customCtrl();
    void configure();
    void rndisTask();
    void ProcessRNDISControlMessage();
    bool ProcessNDISSet(uint32_t OId, void* SetData, uint16_t SetSize);
    uint16_t Ethernet_Checksum16(void* Data, uint16_t Bytes);
    bool ResponseReady = false;
    uint16_t getDesc(uint16_t wValue, uint16_t wIndex, const void ** const descAddr);
    void usbTask();

    bool ProcessNDISQuery(const uint32_t OId, void* QueryData, uint16_t QuerySize,
                             void* ResponseData, uint16_t* ResponseSize);
};

RNDIS::RNDIS() :
    _inpoint(CDC_TX_EPADDR, CDC_TXRX_EPSIZE, EP_TYPE_BULK, 1),
    _outpoint(CDC_RX_EPADDR, CDC_TXRX_EPSIZE, EP_TYPE_BULK, 1),
    _notif(CDC_NOTIFICATION_EPADDR, CDC_NOTIFICATION_EPSIZE, EP_TYPE_INTERRUPT, 1)
{
    uint8_t mac[6] = {0,1,2,3,4,5};
    _w5100.init();
    _w5100.setMACAddress(mac);
    *p_usbcon &= ~(1<<otgpade);
    *p_uhwcon |= 1<<uvrege;
    *p_pllfrq |= 1<<pdiv2;
    USBCON &= ~(1<<VBUSTE);
    UDIEN = 0;
    USBINT = 0;
    UDINT = 0;
    USBCON &= ~(1<<USBE);
    USBCON |= 1<<USBE;
    USBCON &= ~(1<<FRZCLK);
    PLLCSR = 0;
    state = DEVICE_STATE_UNATTACHED;
    confNum = 0;
    _remoteWakeupEn = false;
    USB_Device_CurrentlySelfPowered = false;
    UDCON &= ~(1<<LSM);
    USBCON |= 1<<VBUSTE;
    configureEndpoint(_control);
    //Endpoint_ConfigureEndpoint(0, EP_TYPE_CONTROL, USB_Device_ControlEndpointSize, 1);
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
    ETHERTYPE_IEEE8021Q        = 0x8100,
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

#if 0
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
#endif

#define RNDIS_DEVICE_MIN_MESSAGE_BUFFER_LENGTH  sizeof(AdapterSupportedOIDList) + sizeof(RNDIS_Query_Complete_t)

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
            UEINTX &= ~(1<<RXSTPI);
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

            UEINTX &= ~(1<<RXSTPI);
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
    configureEndpoint(_inpoint);
    configureEndpoint(_outpoint);
    configureEndpoint(_notif);
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
#define IP_COMPARE(IP1, IP2)             (memcmp(IP1, IP2, sizeof(IP_Address_t)) == 0)

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

void RNDIS::rndisTask()
{
	selectEndpoint(CDC_NOTIFICATION_EPADDR);

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
        *p_ueintx &= ~(1<<txini | 1<<fifocon);
		ResponseReady = false;
	}

	if ((CurrRNDISState == RNDIS_Data_Initialized) && !MessageHeader->MessageLength)
	{
		RNDIS_Packet_Message_t RNDISPacketHeader;
		selectEndpoint(CDC_RX_EPADDR);

		if ((*p_ueintx & 1<<rxouti) && !frameIN.FrameLength)
		{
			readStream(&RNDISPacketHeader, sizeof(RNDIS_Packet_Message_t), NULL);

			if (RNDISPacketHeader.DataLength > ETHERNET_FRAME_SIZE_MAX)
			{
                UECONX |= 1<<STALLRQ;
				return;
			}

			//readStream(frameIN.FrameData, RNDISPacketHeader.DataLength, NULL);
#if 0
            uint8_t *dataStream = (uint8_t *)buf;
            uint16_t bytesInTransfer = 0;
            uint8_t errorCode = waitUntilReady();

            if (errorCode)
                return errorCode;

            while (len)
            {
                if ((*p_ueintx & 1<<rwal) == 0)    // read-write allowed?
                {
                    *p_ueintx &= ~(1<<rxouti | 1<<fifocon);    // clear out
                    errorCode = waitUntilReady();

                    if (errorCode)
                        return errorCode;
                }
                else
                {
                    *dataStream = read8();
                    dataStream += 1;
                    len--;
                    bytesInTransfer++;
                }
            }

			Endpoint_ClearOUT();
			frameIN.FrameLength = RNDISPacketHeader.DataLength;
#endif
		}

		selectEndpoint(CDC_TX_EPADDR);

		if ((UEINTX & 1<<TXINI) && frameOUT.FrameLength)
		{
			my_memset(&RNDISPacketHeader, 0, sizeof(RNDIS_Packet_Message_t));
			RNDISPacketHeader.MessageType = REMOTE_NDIS_PACKET_MSG;

			RNDISPacketHeader.MessageLength = (sizeof(RNDIS_Packet_Message_t) +
                frameOUT.FrameLength);

			RNDISPacketHeader.DataOffset = (sizeof(RNDIS_Packet_Message_t) -
                sizeof(RNDIS_Message_Header_t));

			RNDISPacketHeader.DataLength = frameOUT.FrameLength;
			writeStream2(&RNDISPacketHeader, sizeof(RNDIS_Packet_Message_t), NULL);
			writeStream2(frameOUT.FrameData, RNDISPacketHeader.DataLength, NULL);
			*p_ueintx &= ~(1<<txini | 1<<fifocon);
			frameOUT.FrameLength = 0;
		}
	}
}

void RNDIS::usbTask()
{
    if (state == DEVICE_STATE_UNATTACHED)
        return;

    uint8_t PrevEndpoint = getCurrentEndpoint();
    selectEndpoint(0);

    if (UEINTX & 1<<RXSTPI)
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
    sei();

    while (true)
    {
        rndis.rndisTask();
        rndis.usbTask();
    }
}


