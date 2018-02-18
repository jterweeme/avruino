#include "busby.h"
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "misc.h"
#include <stdio.h>
#include <string.h>

#define ADAPTER_MAC_ADDRESS              {0x02, 0x00, 0x02, 0x00, 0x02, 0x00}

#define ETHERNET_FRAME_SIZE_MAX               1500

#define REMOTE_NDIS_MEDIA_STATE_CONNECTED     0x00000000UL

#define REMOTE_NDIS_DF_CONNECTIONLESS         0x00000001UL
#define REMOTE_NDIS_MEDIUM_802_3              0x00000000UL

#define REMOTE_NDIS_VERSION_MINOR             0x00
#define REMOTE_NDIS_VERSION_MAJOR             0x01

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

#define REMOTE_NDIS_INITIALIZE_MSG            0x00000002UL
#define REMOTE_NDIS_HALT_MSG                  0x00000003UL
#define REMOTE_NDIS_QUERY_MSG                 0x00000004UL
#define REMOTE_NDIS_SET_MSG                   0x00000005UL
#define REMOTE_NDIS_RESET_MSG                 0x00000006UL
#define REMOTE_NDIS_KEEPALIVE_MSG             0x00000008UL

#define REMOTE_NDIS_PACKET_MSG                0x00000001UL
#define REMOTE_NDIS_INITIALIZE_MSG            0x00000002UL
#define REMOTE_NDIS_HALT_MSG                  0x00000003UL
#define REMOTE_NDIS_QUERY_MSG                 0x00000004UL
#define REMOTE_NDIS_SET_MSG                   0x00000005UL
#define REMOTE_NDIS_RESET_MSG                 0x00000006UL
#define REMOTE_NDIS_INDICATE_STATUS_MSG       0x00000007UL
#define REMOTE_NDIS_KEEPALIVE_MSG             0x00000008UL

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

uint8_t  CurrRNDISState;

#define    RNDIS_Uninitialized     0
#define    RNDIS_Initialized       1
#define    RNDIS_Data_Initialized  2

enum NDIS_Hardware_Status_t
{
    NDIS_HardwareStatus_Ready,
    NDIS_HardwareStatus_Initializing,
    NDIS_HardwareStatus_Reset,
    NDIS_HardwareStatus_Closing,
    NDIS_HardwareStatus_NotReady
};


static constexpr uint8_t
    RNDIS_REQ_SendEncapsulatedCommand = 0x00,
    RNDIS_REQ_GetEncapsulatedResponse = 0x01,
    CDC_CSCP_CDCClass = 0x02,
    CDC_CSCP_NoSpecificSubclass = 0x00,
    CDC_CSCP_ACMSubclass = 0x02,
    CDC_CSCP_ATCommandProtocol = 0x01,
    CDC_CSCP_NoSpecificProtocol = 0x00,
    CDC_CSCP_VendorSpecificProtocol = 0xFF,
    CDC_CSCP_CDCDataClass = 0x0A,
    CDC_CSCP_NoDataSubclass = 0x00,
    CDC_CSCP_NoDataProtocol = 0x00,
    CDC_REQ_SendEncapsulatedCommand = 0x00,
    CDC_REQ_GetEncapsulatedResponse = 0x01,
    CDC_REQ_SetLineEncoding = 0x20,
    CDC_REQ_GetLineEncoding = 0x21,
    CDC_REQ_SetControlLineState = 0x22,
    CDC_REQ_SENDBREAK = 0x23,
    CDC_NOTIF_SerialState = 0x20,
    CDC_LINEENCODING_OneStopBit = 0,
    CDC_LINEENCODING_OneAndAHalfStopBits = 1,
    CDC_LINEENCODING_TwoStopBits = 2,
    CDC_DSUBTYPE_CSInterface_Header = 0x00,
    CDC_DSUBTYPE_CSInterface_CallManagement = 0x01,
    CDC_DSUBTYPE_CSInterface_ACM = 0x02,
    CDC_DSUBTYPE_CSInterface_DirectLine       = 0x03,
    CDC_DSUBTYPE_CSInterface_Union            = 0x06,
    CDC_DSUBTYPE_CSInterface_CountrySelection = 0x07,
    CDC_DSUBTYPE_CSInterface_USBTerminal = 0x09,
    CDC_DSUBTYPE_CSInterface_NetworkChannel   = 0x0A,
    CDC_DSUBTYPE_CSInterface_ProtocolUnit     = 0x0B,
    CDC_DSUBTYPE_CSInterface_ExtensionUnit    = 0x0C,
    CDC_DSUBTYPE_CSInterface_MultiChannel     = 0x0D,
    CDC_DSUBTYPE_CSInterface_CAPI             = 0x0E,
    CDC_DSUBTYPE_CSInterface_Ethernet         = 0x0F,
    CDC_DSUBTYPE_CSInterface_ATM              = 0x10,
    CDC_NOTIFICATION_EPADDR = ENDPOINT_DIR_IN | 2,
    CDC_TX_EPADDR = ENDPOINT_DIR_IN | 3,
    CDC_RX_EPADDR = ENDPOINT_DIR_OUT | 4,
    CDC_NOTIFICATION_EPSIZE = 8,
    CDC_TXRX_EPSIZE = 16;

struct CDC_Header
{
    uint8_t size;
    uint8_t type;
    uint8_t subtype;
    uint16_t cdcSpec;
}
__attribute__ ((packed));

struct CDC_ACM
{
    uint8_t size;
    uint8_t type;
    uint8_t Subtype;
    uint8_t Capabilities;
}
__attribute__ ((packed));

struct CDC_Union
{
    uint8_t size;
    uint8_t type;
    uint8_t Subtype;
    uint8_t MasterInterfaceNumber;
    uint8_t SlaveInterfaceNumber;
}
__attribute__ ((packed));

struct MyConf
{
    DescConf config;
    DescIface cci;
    CDC_Header cdcHeader;
    CDC_ACM cdcACM;
    CDC_Union cdcUnion;
    DescEndpoint notif;
    DescIface dci;
    DescEndpoint outpoint;
    DescEndpoint inpoint;
}
__attribute__ ((packed));

struct RNDIS_Message_Header_t
{
    uint32_t MessageType;
    uint32_t MessageLength;
}
__attribute__ ((packed));

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
    0x204B,
    0x0001,
    0x01,
    0x02,
    USE_INTERNAL_SERIAL,
    FIXED_NUM_CONFIGURATIONS
};

const MyConf PROGMEM myConf =
{
    {
        sizeof(DescConf),
        DTYPE_CONFIGURATION,
        sizeof(MyConf),
        2,
        1,
        0,
        USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_SELFPOWERED,
        USB_CONFIG_POWER_MA(100)
    },
    {
        sizeof(DescIface),
        DTYPE_INTERFACE,
        0,
        0,
        1,
        CDC_CSCP_CDCClass,
        CDC_CSCP_ACMSubclass,
        CDC_CSCP_VendorSpecificProtocol,
        0
    },
    {
        sizeof(CDC_Header),
        DTYPE_CSINTERFACE,
        CDC_DSUBTYPE_CSInterface_Header,
        0x0110,
    },
    {
        sizeof(CDC_ACM),
        DTYPE_CSINTERFACE,
        CDC_DSUBTYPE_CSInterface_ACM,
        0x00,
    },
    {
        sizeof(CDC_Union),
        DTYPE_CSINTERFACE,
        CDC_DSUBTYPE_CSInterface_Union,
        0,
        1,
    },
    {
        sizeof(DescEndpoint),
        DTYPE_ENDPOINT,
        CDC_NOTIFICATION_EPADDR,
        EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA,
        CDC_NOTIFICATION_EPSIZE,
        0xFF
    },
    {
        sizeof(DescIface),
        DTYPE_INTERFACE,
        1,
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
        EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA,
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

static const DescString<2> PROGMEM languageString =
{
    USB_STRING_LEN(1),
    DTYPE_STRING,
    (wchar_t)0x0409
};

static const DescString<12> PROGMEM manufacturerString =
{
    USB_STRING_LEN(11),
    DTYPE_STRING,
    L"Dean Camera"
};

static const DescString<23> PROGMEM productString =
{
    USB_STRING_LEN(22),
    DTYPE_STRING,
    L"LUFA USB-RS232 Adapter"
};

static const uint32_t PROGMEM AdapterZupportedOIDList[]  =
{
    OID_GEN_SUPPORTED_LIST, OID_GEN_PHYSICAL_MEDIUM, OID_GEN_HARDWARE_STATUS,
    OID_GEN_MEDIA_SUPPORTED, OID_GEN_MEDIA_IN_USE, OID_GEN_MAXIMUM_FRAME_SIZE,
    OID_GEN_MAXIMUM_TOTAL_SIZE, OID_GEN_LINK_SPEED, OID_GEN_TRANSMIT_BLOCK_SIZE,
    OID_GEN_RECEIVE_BLOCK_SIZE, OID_GEN_VENDOR_ID, OID_GEN_VENDOR_DESCRIPTION,
    OID_GEN_CURRENT_PACKET_FILTER, OID_GEN_MAXIMUM_TOTAL_SIZE, OID_GEN_MEDIA_CONNECT_STATUS,
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

struct RNDIS_Query_Complete_t
{
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t RequestId;
    uint32_t Status;
    uint32_t InformationBufferLength;
    uint32_t InformationBufferOffset;
} __attribute__ ((packed));

struct RNDIS_Query_Message_t
{
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t RequestId;
    uint32_t Oid;
    uint32_t InformationBufferLength;
    uint32_t InformationBufferOffset;
    uint32_t DeviceVcHandle;
}
__attribute__ ((packed));

uint32_t CurrPacketFilter = 0;

uint8_t RNDISMessageBuffer[108 + sizeof(RNDIS_Query_Complete_t)];
RNDIS_Message_Header_t *MessageHeader = (RNDIS_Message_Header_t *)&RNDISMessageBuffer;

class RNDIS : public USB
{
private:
    void ProcessRNDISControlMessage();
    Endpoint _txpoint;
    Endpoint _rxpoint;
    Endpoint _notif;
    void customCtrl();
    void configure();
    uint16_t getDesc(uint16_t wValue, uint16_t wIndex, const void ** const descAddr);
public:
    void send();
    RNDIS();
};

RNDIS::RNDIS() :
    _txpoint(CDC_TX_EPADDR, EP_TYPE_BULK, CDC_TXRX_EPSIZE, 1),
    _rxpoint(CDC_RX_EPADDR, EP_TYPE_BULK, CDC_TXRX_EPSIZE, 1),
    _notif(CDC_NOTIFICATION_EPADDR, EP_TYPE_INTERRUPT, 8, 1)
{
    *p_usbcon &= ~(1<<otgpade);
    *p_uhwcon |= 1<<uvrege;
    *p_pllfrq |= 1<<pdiv2;
    *p_usbcon &= ~(1<<vbuste);
    *p_udien = 0;
    *p_usbint = 0;
    *p_udint = 0;
    *p_usbcon &= ~(1<<usbe);
    *p_usbcon |= 1<<usbe;
    *p_usbcon &= ~(1<<frzclk);
    *p_pllcsr = 0;
    state = DEVICE_STATE_UNATTACHED;
    confNum = 0;
    _remoteWakeupEn = false;
    USB_Device_CurrentlySelfPowered = false;
    *p_udcon &= ~(1<<lsm);
    *p_usbcon |= 1<<vbuste;
    configureEndpoint(0, 0, 8, 1);
    *p_udint &= ~(1<<suspi);
    *p_udien |= 1<<suspe;
    *p_udien |= 1<<eorste;
    *p_udcon &= ~(1<<detach);
    *p_usbcon |= 1<<otgpade;
}

static bool ResponseReady = false;

struct RNDIS_Initialize_Message_t
{
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t RequestId;
    uint32_t MajorVersion;
    uint32_t MinorVersion;
    uint32_t MaxTransferSize;
}
__attribute__ ((packed));

struct RNDIS_Initialize_Complete_t
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
} __attribute__ ((packed));

struct MAC_Address_t
{
    uint8_t Octets[6]; /**< Individual bytes of a MAC address */
}
__attribute__ ((packed));

const MAC_Address_t PROGMEM AdapterMACAddress     = {ADAPTER_MAC_ADDRESS};
const char PROGMEM AdapterVendorDescription[]     = "LUFA RNDIS Adapter";


bool ProcessNDISQuery(const uint32_t OId, void* QueryData, uint16_t QuerySize,
                             void* ResponseData, uint16_t* ResponseSize)
{
    switch (OId)
    {
    case OID_GEN_SUPPORTED_LIST:
        *ResponseSize = 108;
        memcpy_P(ResponseData, AdapterZupportedOIDList, 108);
        return true;
    case OID_GEN_PHYSICAL_MEDIUM:
        *ResponseSize = sizeof(uint32_t);
        *((uint32_t*)ResponseData) = 0;
        return true;
    case OID_GEN_HARDWARE_STATUS:
        *ResponseSize = sizeof(uint32_t);
        *(uint32_t*)ResponseData = NDIS_HardwareStatus_Ready;
        return true;
    case OID_GEN_MEDIA_SUPPORTED:
    case OID_GEN_MEDIA_IN_USE:
        *ResponseSize = sizeof(uint32_t);
        *(uint32_t*)ResponseData = REMOTE_NDIS_MEDIUM_802_3;
        return true;
    case OID_GEN_VENDOR_ID:
        *ResponseSize = sizeof(uint32_t);
        *(uint32_t*)ResponseData = 0x00FFFFFF;
        return true;
    case OID_GEN_MAXIMUM_FRAME_SIZE:
    case OID_GEN_TRANSMIT_BLOCK_SIZE:
    case OID_GEN_RECEIVE_BLOCK_SIZE:
        *ResponseSize = sizeof(uint32_t);
        *(uint32_t*)ResponseData = ETHERNET_FRAME_SIZE_MAX;
        return true;
    case OID_GEN_VENDOR_DESCRIPTION:
        *ResponseSize = sizeof(AdapterVendorDescription);
        memcpy_P(ResponseData, AdapterVendorDescription, sizeof(AdapterVendorDescription));
        return true;
    case OID_GEN_MEDIA_CONNECT_STATUS:
        *ResponseSize = sizeof(uint32_t);
        *(uint32_t*)ResponseData = REMOTE_NDIS_MEDIA_STATE_CONNECTED;
        return true;
    case OID_GEN_LINK_SPEED:
        *ResponseSize = sizeof(uint32_t);
        *(uint32_t*)ResponseData = 100000;
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

struct RNDIS_Set_Message_t
{
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t RequestId;
    uint32_t Oid;
    uint32_t InformationBufferLength;
    uint32_t InformationBufferOffset;
    uint32_t DeviceVcHandle;
} __attribute__ ((packed));

struct RNDIS_Set_Complete_t
{
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t RequestId;
    uint32_t Status;
} __attribute__ ((packed));

bool ProcessNDISSet(uint32_t OId, void* SetData, uint16_t SetSize)
{
    switch (OId)
    {
    case OID_GEN_CURRENT_PACKET_FILTER:
        CurrPacketFilter = *((uint32_t*)SetData);
        CurrRNDISState = CurrPacketFilter ? RNDIS_Data_Initialized : RNDIS_Initialized;
        return true;
    case OID_802_3_MULTICAST_LIST:
        return true;
    default:
        return false;
    }
}

struct RNDIS_Reset_Complete_t
{
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t Status;
    uint32_t AddressingReset;
} __attribute__ ((packed));

struct RNDIS_KeepAlive_Message_t
{
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t RequestId;
}
__attribute__ ((packed));

struct RNDIS_KeepAlive_Complete_t
{
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t RequestId;
    uint32_t Status;
} __attribute__ ((packed));

void RNDIS::ProcessRNDISControlMessage()
{
#if 0
    char buf[50];
    snprintf(buf, 50, "%lu\r\n", MessageHeader->MessageType);
    Serial::instance->write(buf);
#endif
    switch (MessageHeader->MessageType)
    {
    case REMOTE_NDIS_INITIALIZE_MSG:
    {
        ResponseReady = true;
        RNDIS_Initialize_Message_t *INITIALIZE_Message = (RNDIS_Initialize_Message_t*)&RNDISMessageBuffer;
        RNDIS_Initialize_Complete_t* INITIALIZE_Response = (RNDIS_Initialize_Complete_t*)&RNDISMessageBuffer;

        INITIALIZE_Response->MessageType           = REMOTE_NDIS_INITIALIZE_CMPLT;
        INITIALIZE_Response->MessageLength         = sizeof(RNDIS_Initialize_Complete_t);
        INITIALIZE_Response->RequestId             = INITIALIZE_Message->RequestId;
        INITIALIZE_Response->Status                = REMOTE_NDIS_STATUS_SUCCESS;
        INITIALIZE_Response->MajorVersion          = REMOTE_NDIS_VERSION_MAJOR;
        INITIALIZE_Response->MinorVersion          = REMOTE_NDIS_VERSION_MINOR;
        INITIALIZE_Response->DeviceFlags           = REMOTE_NDIS_DF_CONNECTIONLESS;
        INITIALIZE_Response->Medium                = REMOTE_NDIS_MEDIUM_802_3;
        INITIALIZE_Response->MaxPacketsPerTransfer = 1;
        INITIALIZE_Response->MaxTransferSize       = (sizeof(RNDIS_Packet_Message_t) + ETHERNET_FRAME_SIZE_MAX);
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
        uint32_t Query_Oid = QUERY_Message->Oid;

        void *QueryData = &RNDISMessageBuffer[sizeof(RNDIS_Message_Header_t) +
                                             QUERY_Message->InformationBufferOffset];

        void *ResponseData = &RNDISMessageBuffer[sizeof(RNDIS_Query_Complete_t)];
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

        void *SetData = &RNDISMessageBuffer[sizeof(RNDIS_Message_Header_t) +
                                                   SET_Message->InformationBufferOffset];

        if (ProcessNDISSet(SET_Oid, SetData, SET_Message->InformationBufferLength))
            SET_Response->Status        = REMOTE_NDIS_STATUS_SUCCESS;
        else
            SET_Response->Status        = REMOTE_NDIS_STATUS_NOT_SUPPORTED;
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
        ResponseReady = true;

        RNDIS_KeepAlive_Message_t *KEEPALIVE_Message = (RNDIS_KeepAlive_Message_t*)&RNDISMessageBuffer;
        RNDIS_KeepAlive_Complete_t *KEEPALIVE_Response = (RNDIS_KeepAlive_Complete_t*)&RNDISMessageBuffer;

        KEEPALIVE_Response->MessageType     = REMOTE_NDIS_KEEPALIVE_CMPLT;
        KEEPALIVE_Response->MessageLength   = sizeof(RNDIS_KeepAlive_Complete_t);
        KEEPALIVE_Response->RequestId       = KEEPALIVE_Message->RequestId;
        KEEPALIVE_Response->Status          = REMOTE_NDIS_STATUS_SUCCESS;
        break;
    }
}

void RNDIS::customCtrl()
{
#if 0
    char buf[50];
    snprintf(buf, 50, "%u\r\n", _ctrlReq.bRequest);
    Serial::instance->write(buf);
#endif
    switch (_ctrlReq.bRequest)
    {
    case RNDIS_REQ_SendEncapsulatedCommand:
        if (_ctrlReq.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
        {
            *p_ueintx &= ~(1<<rxstpi);  // clear setup
            readControlStreamLE(RNDISMessageBuffer, _ctrlReq.wLength);
            *p_ueintx &= ~(1<<txini | 1<<fifocon);  // clear in
            ProcessRNDISControlMessage();
        }

        break;
    case RNDIS_REQ_GetEncapsulatedResponse:
        if (_ctrlReq.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
        {
            if ((MessageHeader->MessageLength) == 0)
            {
                RNDISMessageBuffer[0] = 0;
                MessageHeader->MessageLength = 1;
            }

            *p_ueintx &= ~(1<<rxstpi);  // clear setup
            write_Control_Stream_LE(RNDISMessageBuffer, MessageHeader->MessageLength);
            *p_ueintx &= ~(1<<rxouti | 1<<fifocon); // clear out
            MessageHeader->MessageLength = 0;
        }

        break;
    }
}

uint16_t RNDIS::getDesc(uint16_t wValue, uint16_t wIndex, const void ** const descAddr)
{
    const uint8_t descNumber = wValue & 0xFF;
    const void* addr = NULL;
    uint16_t size = 0;

    switch (wValue >> 8)
    {
    case DTYPE_DEVICE:
        addr = &DeviceDescriptor;
        size = sizeof(DescDev);
        break;
    case DTYPE_CONFIGURATION:
        addr = &myConf;
        size = sizeof(myConf);
        break;
    case DTYPE_STRING:
        switch (descNumber)
        {
        case STRING_ID_LANGUAGE:
            addr = &languageString;
            size = pgm_read_byte(&languageString.size);
            break;
        case STRING_ID_MANUFACTURER:
            addr = &manufacturerString;
            size = pgm_read_byte(&manufacturerString.size);
            break;
        case STRING_ID_PRODUCT:
            addr = &productString;
            size = pgm_read_byte(&productString.size);
            break;
        }

        break;
    }

    *descAddr = addr;
    return size;
}

void RNDIS::configure()
{
    configureEndpoint(_txpoint);
    configureEndpoint(_rxpoint);
    configureEndpoint(_notif);
}

static RNDIS *g_rndis;

uint8_t frame[63] = {"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"};

void RNDIS::send()
{
    _txpoint.select();

    if (*p_ueintx & 1<<txini)   // in ready?
    {
        RNDIS_Packet_Message_t packet;
        memset(&packet, 0, sizeof(packet));
        packet.MessageType = REMOTE_NDIS_PACKET_MSG;
        packet.MessageLength = sizeof(packet) + sizeof(frame);
        packet.DataOffset = sizeof(packet) - sizeof(RNDIS_Message_Header_t);
        packet.DataLength = sizeof(frame);
        writeStream2(&packet, sizeof(packet), NULL);
        writeStream2(frame, packet.DataLength, NULL);
        *p_ueintx &= ~(1<<txini | 1<<fifocon);  // clear in
    }
}

ISR(TIMER1_OVF_vect)
{
    //Serial::instance->write("Debug bericht\r\n");
    g_rndis->send();
}

int main()
{
#if 0
    Serial serial;
    serial.init();
    Serial::instance->write("RNDIS app\r\n");
#endif
    TCCR1B = 1<<CS12 | 1<<CS10;
    TIMSK1 |= 1<<TOIE1;
    RNDIS rndis;
    g_rndis = &rndis;
    sei();

    while (true)
        ;

    return 0;
}



