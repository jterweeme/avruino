#include "cdc.h"
#include "misc.h"

#define __LPM_classic__(addr) \
(__extension__({ \
    uint16_t __addr16 = (uint16_t)(addr); \
    uint8_t __result; \
    __asm__ __volatile__ \
    ( \
        "lpm" "\n\t" \
        "mov %0, r0" "\n\t" \
        : "=r" (__result) \
        : "z" (__addr16) \
        : "r0" \
    ); \
    __result; \
}))

#define __LPM(addr) __LPM_classic__(addr)
#define pgm_read_bite_near(address_short) __LPM((uint16_t)(address_short))
#define pgm_read_bite(address_short) pgm_read_bite_near(address_short)

static constexpr uint8_t
    CDC_CSCP_CDCClass = 0x02,
    CDC_CSCP_NoSpecificSubclass = 0x00,
    CDC_CSCP_ACMSubclass = 0x02,
    CDC_CSCP_ATCommandProtocol = 0x01,
    CDC_CSCP_NoSpecificProtocol = 0x00,
    CDC_CSCP_VendorSpecificProtocol = 0xFF,
    CDC_CSCP_CDCDataClass = 0x0A,
    CDC_CSCP_NODATASUBCLASS = 0x00,
    CDC_CSCP_NoDataProtocol = 0x00,
    CDC_REQ_SendEncapsulatedCommand = 0x00,
    CDC_REQ_GetEncapsulatedResponse = 0x01,
    CDC_REQ_SetLineEncoding = 0x20,
    CDC_REQ_GetLineEncoding = 0x21,
    CDC_REQ_SetControlLineState = 0x22,
    CDC_REQ_SENDBREAK = 0x23,
    CDC_DSUBTYPE_CSInterface_Header = 0x00,
    CDC_DSUBTYPE_CSInterface_ACM = 0x02,
    CDC_DSUBTYPE_CSInterface_Union = 0x06,
    CDC_NOTIFICATION_EPADDR = ENDPOINT_DIR_IN | 2,
    CDC_TX_EPADDR = ENDPOINT_DIR_IN | 3,
    CDC_RX_EPADDR = ENDPOINT_DIR_OUT | 4,
    CDC_NOTIFICATION_EPSIZE = 8,
    CDC_TXRX_EPSIZE = 16;

static const DescDev __attribute__ ((__progmem__)) DeviceDescriptor =
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

static const MyConf __attribute__ ((__progmem__)) myConf =
{
    {
        sizeof(DescConf),
        DTYPE_CONFIGURATION,
        sizeof(MyConf),
        2,
        1,
        NO_DESCRIPTOR,
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
        CDC_CSCP_ATCommandProtocol,
        NO_DESCRIPTOR
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
        0x06,
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
        CDC_CSCP_NODATASUBCLASS,
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

static const DescString<2> __attribute__ ((__progmem__)) languageString =
{
    USB_STRING_LEN(1),
    DTYPE_STRING,
    (wchar_t)0x0409
};

static const DescString<12> __attribute__ ((__progmem__)) manufacturerString =
{
    USB_STRING_LEN(11),
    DTYPE_STRING,
    L"Dean Camera"
};

static const DescString<23> __attribute__ ((__progmem__)) productString =
{
    USB_STRING_LEN(22),
    DTYPE_STRING,
    L"LUFA USB-RS232 Adapter"
};

uint16_t CDC::getDesc(uint16_t wValue, uint16_t wIndex, const void ** const descAddress)
{
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
        switch (wValue & 0xff)
        {
        case STRING_ID_LANGUAGE:
            addr = &languageString;
            size = pgm_read_bite(&languageString.size);
            break;
        case STRING_ID_MANUFACTURER:
            addr = &manufacturerString;
            size = pgm_read_bite(&manufacturerString.size);
            break;
        case STRING_ID_PRODUCT:
            addr = &productString;
            size = pgm_read_bite(&productString.size);
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

    if ((*p_ueintx & 1<<rwal) == 0)
    {
        *p_ueintx &= ~(1<<txini | 1<<fifocon);
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

    bool bankFull = !(*p_ueintx & 1<<rwal);
    *p_ueintx &= ~(1<<txini | 1<<fifocon);

    if (bankFull)
    {
        if ((ErrorCode = waitUntilReady()) != ENDPOINT_READYWAIT_NoError)
            return ErrorCode;

        *p_ueintx &= ~(1<<txini | 1<<fifocon);
    }

    return ENDPOINT_READYWAIT_NoError;
}

int16_t CDC::receive()
{
    if ((state != DEVICE_STATE_Configured) || !(_lineEncoding.BaudRateBPS))
        return -1;

    int16_t ReceivedByte = -1;
    _outpoint.select();

    if (*p_ueintx & 1<<rxouti) // is OUT received?
    {
        if (bytesInEndpoint())
            ReceivedByte = read8();

        if (!(bytesInEndpoint()))
            *p_ueintx &= ~(1<<rxouti | 1<<fifocon);
    }

    return ReceivedByte;
}

CDC::CDC() :
    _inpoint(CDC_TX_EPADDR, CDC_TXRX_EPSIZE, EP_TYPE_BULK, 0),
    _outpoint(CDC_RX_EPADDR, CDC_TXRX_EPSIZE, EP_TYPE_BULK, 0),
    _notif(CDC_NOTIFICATION_EPADDR, CDC_NOTIFICATION_EPSIZE, EP_TYPE_INTERRUPT, 0)
{
    *p_usbcon &= ~(1<<otgpade);
    *p_uhwcon |= 1<<uvrege;    // enable USB pad regulator
    *p_usbcon &= ~(1<<vbuste);     // disable VBUS transition interrupt
    *p_udien = 0;
    *p_usbint = 0;
    *p_udint = 0;
    *p_usbcon &= ~(1<<usbe);   // disable USB controller clock inputs
    *p_usbcon |= 1<<usbe;      // enable USB controller clock inputs
    *p_usbcon &= ~(1<<frzclk); // enable clock inputs
    *p_pllcsr = 0;
    state = DEVICE_STATE_UNATTACHED;
    confNum = 0;
    _remoteWakeupEn = false;
    USB_Device_CurrentlySelfPowered = false;
    *p_udcon &= ~(1<<lsm);
    *p_usbcon |= 1<<vbuste;    // enable VBUS transition interrupt
    configureEndpoint(_control);
    *p_udint &= ~(1<<suspi);   // clear suspend interrupt flag
    *p_udien |= 1<<suspe;      // enable SUSPI interrupt
    *p_udien |= 1<<eorste;     // enable EORSTI interrupt
    *p_udcon &= ~(1<<detach);  // reconnect device
    *p_usbcon |= 1<<otgpade;   // otgpad on
    zei();

    if ((state != DEVICE_STATE_Configured) || !(_lineEncoding.BaudRateBPS))
        return;

    _inpoint.select();

    if (*p_ueintx & 1<<txini)
        flush();

    if (state == DEVICE_STATE_UNATTACHED)
        return;

    uint8_t prevEndpoint = getCurrentEndpoint();
    _control.select();

    if (*p_ueintx & 1<<rxstpi)
        procCtrlReq();

    selectEndpoint(prevEndpoint);
}

void CDC::customCtrl()
{
    if ((*p_ueintx & 1<<rxstpi) == 0)
        return;

    if (_ctrlReq.wIndex != _control.addr)
        return;

    switch (_ctrlReq.bRequest)
    {
    case CDC_REQ_GetLineEncoding:
        if (_ctrlReq.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
        {
            *p_ueintx &= ~(1<<rxstpi);

            while ((*p_ueintx & 1<<txini) == 0)
                ;

            write32(_lineEncoding.BaudRateBPS);
            write8(_lineEncoding.CharFormat);
            write8(_lineEncoding.ParityType);
            write8(_lineEncoding.DataBits);
            *p_ueintx &= ~(1<<txini | 1<<fifocon);
            clearStatusStage();
        }
        break;
    case CDC_REQ_SetLineEncoding:
        if (_ctrlReq.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
        {
            *p_ueintx &= ~(1<<rxstpi);

            while ((*p_ueintx & 1<<rxouti) == 0)
                if (state == DEVICE_STATE_UNATTACHED)
                    return;

            _lineEncoding.BaudRateBPS = read32();
            _lineEncoding.CharFormat = read8();
            _lineEncoding.ParityType = read8();
            _lineEncoding.DataBits = read8();
            *p_ueintx &= ~(1<<rxouti | 1<<fifocon);
            clearStatusStage();
        }

        break;
    case CDC_REQ_SetControlLineState:
        if (_ctrlReq.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
        {
            *p_ueintx &= ~(1<<rxstpi);
            clearStatusStage();
        }
        break;
    case CDC_REQ_SENDBREAK:
        if (_ctrlReq.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
        {
            *p_ueintx &= ~(1<<rxstpi);
            clearStatusStage();
        }

        break;
    }
}

static inline void *myMemset(void *s, int c, size_t n)
{
    uint8_t *p = (uint8_t *)s;

    while (n--)
        *p++ = (uint8_t)c;

    return s;
}

void CDC::configure()
{
    myMemset(&_lineEncoding, 0, sizeof(_lineEncoding));
    configureEndpoint(_inpoint);
    configureEndpoint(_outpoint);
    configureEndpoint(_notif);
}







