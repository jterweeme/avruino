#include "cdc.h"
#include <avr/interrupt.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "misc.h"
#include <stdio.h>

static constexpr uint8_t
    CDC_DSUBTYPE_CSInterface_Header = 0x00,
    CDC_DSUBTYPE_CSInterface_CallManagement   = 0x01,
    CDC_DSUBTYPE_CSInterface_ACM = 0x02,
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
    CDC_NOTIFICATION_EPADDR = ENDPOINT_DIR_IN | 2,
    CDC_TX_EPADDR = ENDPOINT_DIR_IN | 3,
    CDC_RX_EPADDR = ENDPOINT_DIR_OUT | 4,
    CDC_NOTIFICATION_EPSIZE = 8,
    CDC_TXRX_EPSIZE = 16,
    CDC_PARITY_None  = 0,
    CDC_PARITY_Odd   = 1,
    CDC_PARITY_Even  = 2,
    CDC_PARITY_Mark  = 3,
    CDC_PARITY_Space = 4;

static const DescDev PROGMEM DeviceDescriptor =
{
    sizeof(DescDev),
    DTYPE_Device,
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

static const MyConf PROGMEM myConf =
{
    {
        sizeof(DescConf),
        DTYPE_Configuration,
        sizeof(MyConf),
        2,
        1,
        NO_DESCRIPTOR,
        USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_SELFPOWERED,
        USB_CONFIG_POWER_MA(100)
    },
    {
        sizeof(DescIface),
        DTYPE_Interface,
        0,
        0,
        1,
        CDC_CSCP_CDCClass,
        CDC_CSCP_ACMSubclass,
        CDC_CSCP_ATCommandProtocol,
        NO_DESCRIPTOR
    },
    {
        sizeof(CDC_Header),
        DTYPE_CSInterface,
        CDC_DSUBTYPE_CSInterface_Header,
        0x0110,
    },
    {
        sizeof(CDC_ACM),
        DTYPE_CSInterface,
        CDC_DSUBTYPE_CSInterface_ACM,
        0x06,
    },
    {
        sizeof(CDC_Union),
        DTYPE_CSInterface,
        CDC_DSUBTYPE_CSInterface_Union,
        0,
        1,
    },
    {
        sizeof(DescEndpoint),
        DTYPE_Endpoint,
        CDC_NOTIFICATION_EPADDR,
        EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA,
        CDC_NOTIFICATION_EPSIZE,
        0xFF
    },
    {
        sizeof(DescIface),
        DTYPE_Interface,
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
        DTYPE_Endpoint,
        CDC_RX_EPADDR,
        EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA,
        CDC_TXRX_EPSIZE,
        0x05
    },
    {
        sizeof(DescEndpoint),
        DTYPE_Endpoint,
        CDC_TX_EPADDR,
        EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA,
        CDC_TXRX_EPSIZE,
        0x05
    }
};

static const DescString<2> PROGMEM languageString =
{
    USB_STRING_LEN(1),
    DTYPE_String,
    (wchar_t)0x0409
};

static const DescString<12> PROGMEM manufacturerString =
{
    USB_STRING_LEN(11),
    DTYPE_String,
    L"Dean Camera"
};

static const DescString<23> PROGMEM productString =
{
    USB_STRING_LEN(22),
    DTYPE_String,
    L"LUFA USB-RS232 Adapter"
};

uint16_t CDC::getDescriptor(uint16_t wValue, uint8_t wIndex, const void **descAddress)
{
    const uint8_t descNumber = wValue & 0xFF;
    const void* addr = NULL;
    uint16_t size = 0;

    switch (wValue >> 8)
    {
    case DTYPE_Device:
        addr = &DeviceDescriptor;
        size = sizeof(DescDev);
        break;
    case DTYPE_Configuration:
        addr = &myConf;
        size = sizeof(myConf);
        break;
    case DTYPE_String:
        switch (descNumber)
        {
        case 0x00:
            addr = &languageString;
            size = pgm_read_byte(&languageString.size);
            break;
        case 0x01:
            addr = &manufacturerString;
            size = pgm_read_byte(&manufacturerString.size);
            break;
        case 0x02:
            addr = &productString;
            size = pgm_read_byte(&productString.size);
            break;
        }

        break;
    }

    *descAddress = addr;
    return size;
}

uint8_t CDC::sendByte(uint8_t Data)
{
    if (state != DEVICE_STATE_Configured)
        return ENDPOINT_RWSTREAM_DeviceDisconnected;

    _inpoint.select();

    if (!(UEINTX & 1<<RWAL))
    {
        UEINTX &= ~(1<<TXINI | 1<<FIFOCON);
        uint8_t ErrorCode;

        if ((ErrorCode = waitUntilReady()) != ENDPOINT_READYWAIT_NoError)
            return ErrorCode;
    }

    write8(Data);
    return ENDPOINT_READYWAIT_NoError;
}

uint8_t CDC::flush()
{
    if ((state != DEVICE_STATE_Configured) || !_lineEncoding.BaudRateBPS)
        return ENDPOINT_RWSTREAM_DeviceDisconnected;

    uint8_t ErrorCode;
    _inpoint.select();

    if (!(bytesInEndpoint()))
        return ENDPOINT_READYWAIT_NoError;

    bool bankFull = !(UEINTX & 1<<RWAL);
    UEINTX &= ~(1<<TXINI | 1<<FIFOCON);

    if (bankFull)
    {
        if ((ErrorCode = waitUntilReady()) != ENDPOINT_READYWAIT_NoError)
            return ErrorCode;

        UEINTX &= ~(1<<TXINI | 1<<FIFOCON);
    }

    return ENDPOINT_READYWAIT_NoError;
}

int16_t CDC::receive()
{
    if ((state != DEVICE_STATE_Configured) || !(_lineEncoding.BaudRateBPS))
        return -1;

    int16_t ReceivedByte = -1;
    _outpoint.select();

    if (UEINTX & 1<<RXOUTI) // is OUT received?
    {
        if (bytesInEndpoint())
            ReceivedByte = read8();

        if (!(bytesInEndpoint()))
            UEINTX &= ~(1<<RXOUTI | 1<<FIFOCON);
    }

    return ReceivedByte;
}

CDC::CDC() :
    _inpoint(CDC_TX_EPADDR, CDC_TXRX_EPSIZE, EP_TYPE_BULK, 0),
    _outpoint(CDC_RX_EPADDR, CDC_TXRX_EPSIZE, EP_TYPE_BULK, 0),
    _notif(CDC_NOTIFICATION_EPADDR, CDC_NOTIFICATION_EPSIZE, EP_TYPE_INTERRUPT, 0)
{
    *p_usbcon &= ~(1<<otgpade);
    *p_uhwcon |= 1<<UVREGE;    // enable USB pad regulator
    *p_usbcon &= ~(1<<VBUSTE);     // disable VBUS transition interrupt
    *p_udien = 0;
    *p_usbint = 0;
    *p_udint = 0;
    *p_usbcon &= ~(1<<USBE);   // disable USB controller clock inputs
    *p_usbcon |= 1<<USBE;      // enable USB controller clock inputs
    *p_usbcon &= ~(1<<FRZCLK); // enable clock inputs
    PLLCSR = 0;
    state = DEVICE_STATE_Unattached;
    USB_Device_ConfigurationNumber  = 0;
    USB_Device_RemoteWakeupEnabled  = false;
    USB_Device_CurrentlySelfPowered = false;
    *p_udcon &= ~(1<<LSM);
    *p_usbcon |= 1<<VBUSTE;    // enable VBUS transition interrupt
    configureEndpoint(_control.addr, _control.type, _control.size, 1);
    *p_udint &= ~(1<<SUSPI);   // clear suspend interrupt flag
    *p_udien |= 1<<SUSPE;      // enable SUSPI interrupt
    *p_udien |= 1<<EORSTE;     // enable EORSTI interrupt
    *p_udcon &= ~(1<<DETACH);  // reconnect device
    *p_usbcon |= 1<<OTGPADE;   // otgpad on
    sei();

    if ((state != DEVICE_STATE_Configured) || !(_lineEncoding.BaudRateBPS))
        return;

    _inpoint.select();

    if (*p_ueintx & 1<<TXINI)
        flush();

    if (state == DEVICE_STATE_Unattached)
        return;

    uint8_t prevEndpoint = getCurrentEndpoint();
    _control.select();

    if (UEINTX & 1<<RXSTPI)
        procCtrlReq();

    selectEndpoint(prevEndpoint);
}

void CDC::EVENT_USB_Device_ControlRequest()
{
    if ((UEINTX & 1<<RXSTPI) == 0)
        return;

    if (_ctrlReq.wIndex != _control.addr)
        return;

    switch (_ctrlReq.bRequest)
    {
    case CDC_REQ_GetLineEncoding:
        if (_ctrlReq.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
        {
            UEINTX &= ~(1<<RXSTPI);
            while (!(UEINTX & 1<<TXINI));
            write32(_lineEncoding.BaudRateBPS);
            write8(_lineEncoding.CharFormat);
            write8(_lineEncoding.ParityType);
            write8(_lineEncoding.DataBits);
            UEINTX &= ~(1<<TXINI | 1<<FIFOCON);
            clearStatusStage();
        }
        break;
    case CDC_REQ_SetLineEncoding:
        if (_ctrlReq.bmRequestType ==
                (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
        {
            UEINTX &= ~(1<<RXSTPI);

            while (!(UEINTX & 1<<RXOUTI))
                if (state == DEVICE_STATE_Unattached)
                    return;

            _lineEncoding.BaudRateBPS = read32();
            _lineEncoding.CharFormat = read8();
            _lineEncoding.ParityType = read8();
            _lineEncoding.DataBits = read8();
            UEINTX &= ~(1<<RXOUTI | 1<<FIFOCON);
            clearStatusStage();
        }

        break;
    case CDC_REQ_SetControlLineState:
        if (_ctrlReq.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
        {
            UEINTX &= ~(1<<RXSTPI);
            clearStatusStage();
        }
        break;
    case CDC_REQ_SendBreak:
        if (_ctrlReq.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
        {
            UEINTX &= ~(1<<RXSTPI);
            clearStatusStage();
        }

        break;
    }
}

void CDC::procCtrlReq()
{
    uint8_t* RequestHeader = (uint8_t*)&_ctrlReq;

    for (uint8_t i = 0; i < sizeof(USBRequest); i++)
        RequestHeader[i] = read8();

    EVENT_USB_Device_ControlRequest();

    if (UEINTX & 1<<RXSTPI) // endpoint isSetupReceived?
    {
        uint8_t bmRequestType = _ctrlReq.bmRequestType;

        switch (_ctrlReq.bRequest)
        {
        case REQ_GetStatus:
            if ((bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_DEVICE)) ||
                (bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_ENDPOINT)))
            {
                uint8_t CurrentStatus = 0;

                switch (_ctrlReq.bmRequestType)
                {
                case (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_DEVICE):
                    if (USB_Device_CurrentlySelfPowered)
                        CurrentStatus |= FEATURE_SELFPOWERED_ENABLED;

                    if (USB_Device_RemoteWakeupEnabled)
                        CurrentStatus |= FEATURE_REMOTE_WAKEUP_ENABLED;

                    break;
                case (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_ENDPOINT):
                    selectEndpoint((uint8_t)_ctrlReq.wIndex & ENDPOINT_EPNUM_MASK);
                    CurrentStatus = UECONX & 1<<STALLRQ;
                    selectEndpoint(ENDPOINT_CONTROLEP);
                    break;
                default:
                    return;
                }

                UEINTX &= ~(1<<RXSTPI);
                write16(CurrentStatus);
                UEINTX &= ~(1<<TXINI | 1<<FIFOCON);
                clearStatusStage();
            }

            break;
        case REQ_ClearFeature:
        case REQ_SetFeature:
            if ((bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_DEVICE)) ||
                (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_ENDPOINT)))
            {
                Device_ClearSetFeature();
            }

            break;
        case REQ_SetAddress:
            if (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_DEVICE))
            {
                uint8_t DeviceAddress = _ctrlReq.wValue & 0x7F;
                UDADDR = (UDADDR & 1<<ADDEN) | (DeviceAddress & 0x7F);
                UEINTX &= ~(1<<RXSTPI);
                clearStatusStage();
                while (!(UEINTX & 1<<TXINI));
                UDADDR |= 1<<ADDEN; // enable dev addr
                state = DeviceAddress ? DEVICE_STATE_Addressed : DEVICE_STATE_Default;
            }

            break;

        case REQ_GetDescriptor:
            if ((bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_DEVICE)) ||
                (bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_INTERFACE)))
            {
                const void *descPtr;
                uint16_t descSize;

                if (_ctrlReq.wValue == (DTYPE_String << 8 | USE_INTERNAL_SERIAL))
                {
                    SigDesc sigDesc;
                    sigDesc.type = DTYPE_String;
                    sigDesc.size = USB_STRING_LEN(INTERNAL_SERIAL_LENGTH_BITS / 4);
                    Device_GetSerialString(sigDesc.unicodeString);
                    UEINTX &= ~(1<<RXSTPI);
                    write_Control_Stream_LE(&sigDesc, sizeof(sigDesc));
                    UEINTX &= ~(1<<RXOUTI | 1<<FIFOCON);
                    return;
                }

                if ((descSize = getDescriptor(_ctrlReq.wValue, _ctrlReq.wIndex, &descPtr)) == 0)
                    return;

                UEINTX &= ~(1<<RXSTPI); // clear setup
                write_Control_PStream_LE(descPtr, descSize);
                UEINTX &= ~(1<<RXOUTI | 1<<FIFOCON);    // clear out
            }

            break;
        case REQ_GetConfiguration:
            if (bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_DEVICE))
            {
                UEINTX &= ~(1<<RXSTPI); // clear setup
                write8(USB_Device_ConfigurationNumber);
                UEINTX &= ~(1<<TXINI | 1<<FIFOCON); // endpoint clear in
                clearStatusStage();
            }

            break;

        case REQ_SetConfiguration:
            if (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_DEVICE))
            {
                if ((uint8_t)_ctrlReq.wValue > FIXED_NUM_CONFIGURATIONS)
                    return;

                UEINTX &= ~(1<<RXSTPI);
                USB_Device_ConfigurationNumber = (uint8_t)_ctrlReq.wValue;
                clearStatusStage();

                if (USB_Device_ConfigurationNumber)
                    state = DEVICE_STATE_Configured;
                else
                    state = UDADDR & 1<<ADDEN ? DEVICE_STATE_Configured : DEVICE_STATE_Powered;

                memset(&_lineEncoding, 0, sizeof(_lineEncoding));
                configureEndpoint(_inpoint);
                configureEndpoint(_outpoint);
                configureEndpoint(_notif);
            }
            break;
        default:
            break;
        }
    }

    if (UEINTX & 1<<RXSTPI)
    {
        UEINTX &= ~(1<<RXSTPI);     // clear setup
        UECONX |= 1<<STALLRQ;
    }
}









