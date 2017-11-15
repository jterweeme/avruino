#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include "busby.h"

#define USB_CurrentMode USB_MODE_Device
#define USB_Device_ControlEndpointSize FIXED_CONTROL_ENDPOINT_SIZE

static const uint8_t CONTROL_REQTYPE_DIRECTION = 0x80;
static const uint8_t CONTROL_REQTYPE_TYPE = 0x60;
static const uint8_t CONTROL_REQTYPE_RECIPIENT = 0x1f;
static const uint8_t REQDIR_HOSTTODEVICE = 0 << 7;
static const uint8_t REQDIR_DEVICETOHOST = 1 << 7;
static const uint8_t REQTYPE_STANDARD = 0 << 5;
static const uint8_t REQTYPE_CLASS = 1 << 5;
static const uint8_t REQTYPE_VENDOR = 2 << 5;
static const uint8_t REQREC_DEVICE = 0 << 0;
static const uint8_t REQREC_INTERFACE = 1 << 0;
static const uint8_t REQREC_ENDPOINT = 2 << 0;
static const uint8_t REQREC_OTHER = 3 << 0;
static const uint8_t FEATURE_SELFPOWERED_ENABLED = 1 << 0;
static const uint8_t FEATURE_REMOTE_WAKEUP_ENABLED = 1 << 1;

typedef struct
{
    USB_Descriptor_Header_t Header;
    uint8_t                 Subtype;
    uint16_t                CDCSpecification;
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

typedef struct
{
    uint8_t bFunctionLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubType;
    uint8_t bMasterInterface;
    uint8_t bSlaveInterface0;
} ATTR_PACKED USB_CDC_StdDescriptor_FunctionalUnion_t;


bool
CDC_Device_ConfigureEndpoints(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo)
                ATTR_NON_NULL_PTR_ARG(1);

extern "C" {
void
CDC_Device_ProcessControlRequest(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo)
ATTR_NON_NULL_PTR_ARG(1);

void
EVENT_CDC_Device_LineEncodingChanged(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo)
ATTR_NON_NULL_PTR_ARG(1);


void
EVENT_CDC_Device_ControLineStateChanged(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo)
ATTR_NON_NULL_PTR_ARG(1);

}


uint8_t
CDC_Device_SendData(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo,
                     const void* const Buffer,
               const uint16_t Length) ATTR_NON_NULL_PTR_ARG(1) ATTR_NON_NULL_PTR_ARG(2);

uint8_t
CDC_Device_SendString(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo,
               const char* const String) ATTR_NON_NULL_PTR_ARG(1) ATTR_NON_NULL_PTR_ARG(2);

uint8_t
CDC_Device_SendByte(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo,
                         const uint8_t Data) ATTR_NON_NULL_PTR_ARG(1);

uint16_t CDC_Device_BytesReceived(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo);
int16_t CDC_Device_ReceiveByte(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo);

void
CDC_Device_SendControlLineStateChange(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo)
ATTR_NON_NULL_PTR_ARG(1);

static const uint8_t CDC_NOTIFICATION_EPADDR = ENDPOINT_DIR_IN | 2;
static const uint8_t CDC_TX_EPADDR = ENDPOINT_DIR_IN | 3;
static const uint8_t CDC_RX_EPADDR = ENDPOINT_DIR_OUT | 4;
static const uint8_t CDC_NOTIFICATION_EPSIZE = 8;
static const uint8_t CDC_TXRX_EPSIZE = 16;


uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue, const uint8_t wIndex,
                                    const void** const DescriptorAddress)
                                    ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);


struct USB_Descriptor_Configuration_t
{
    USB_Descriptor_Configuration_Header_t    Config;
    USB_Descriptor_Interface_t               CDC_CCI_Interface;
    USB_CDC_Descriptor_FunctionalHeader_t    CDC_Functional_Header;
    USB_CDC_Descriptor_FunctionalACM_t       CDC_Functional_ACM;
    USB_CDC_Descriptor_FunctionalUnion_t     CDC_Functional_Union;
    USB_Descriptor_Endpoint_t                CDC_NotificationEndpoint;
    USB_Descriptor_Interface_t               CDC_DCI_Interface;
    USB_Descriptor_Endpoint_t                CDC_DataOutEndpoint;
    USB_Descriptor_Endpoint_t                CDC_DataInEndpoint;
} __attribute__ ((packed));


extern "C"
{

    void CDC_Device_Event_Stub(void) ATTR_CONST;

    void EVENT_CDC_Device_LineEncodingChanged(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo)
                        ATTR_WEAK ATTR_NON_NULL_PTR_ARG(1) ATTR_ALIAS(CDC_Device_Event_Stub);

    void EVENT_CDC_Device_ControLineStateChanged(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo)
                     ATTR_WEAK ATTR_NON_NULL_PTR_ARG(1) ATTR_ALIAS(CDC_Device_Event_Stub);
}

static int CDC_Device_putchar(char c, FILE* Stream) ATTR_NON_NULL_PTR_ARG(2);
static int CDC_Device_getchar(FILE* Stream) ATTR_NON_NULL_PTR_ARG(1);

void USB::init()
{
    USB_OTGPAD_Off();

    if (!(USB_Options & USB_OPT_REG_DISABLED))
        USB_REG_On();
    else
        USB_REG_Off();

    if (!(USB_Options & USB_OPT_MANUAL_PLL))
        PLLFRQ = (1 << PDIV2);

    USB_IsInitialized = true;
    resetInterface();
}

void USB::resetInterface()
{
    USB::disableAllInterrupts();
    USB::clearAllInterrupts();
    USB_Controller_Reset();
    USB_CLK_Unfreeze();

    if (USB_CurrentMode == USB_MODE_Device)
    {
        if (!(USB_Options & USB_OPT_MANUAL_PLL))
            USB_PLL_Off();

        USB_Init_Device();
    }

    USB_OTGPAD_On();
}

void USB::USB_Init_Device()
{
    USB_DeviceState = DEVICE_STATE_Unattached;
    USB_Device_ConfigurationNumber  = 0;
    USB_Device_RemoteWakeupEnabled  = false;
    USB_Device_CurrentlySelfPowered = false;

    if (USB_Options & USB_DEVICE_OPT_LOWSPEED)
        USB::USB_Device_SetLowSpeed();
    else
        USB::USB_Device_SetFullSpeed();

    USB_INT_Enable(USB_INT_VBUSTI);

    Endpoint_ConfigureEndpoint(ENDPOINT_CONTROLEP, EP_TYPE_CONTROL,
                        USB_Device_ControlEndpointSize, 1);

    USB::USB_INT_Clear(USB_INT_SUSPI);
    USB_INT_Enable(USB_INT_SUSPI);
    USB_INT_Enable(USB_INT_EORSTI);
    USB_Attach();
}

void CDC_Device_ProcessControlRequest(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo)
{
    if (!(USB::Endpoint_IsSETUPReceived()))
        return;

    if (USB_ControlRequest.wIndex != CDCInterfaceInfo->Config.ControlInterfaceNumber)
        return;

    switch (USB_ControlRequest.bRequest)
    {
        case CDC_REQ_GetLineEncoding:
            if (USB_ControlRequest.bmRequestType ==
                (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
            {
                USB::Endpoint_ClearSETUP();
                while (!(USB::Endpoint_IsINReady()));
                USB::Endpoint_Write_32_LE(CDCInterfaceInfo->State.LineEncoding.BaudRateBPS);
                USB::Endpoint_Write_8(CDCInterfaceInfo->State.LineEncoding.CharFormat);
                USB::Endpoint_Write_8(CDCInterfaceInfo->State.LineEncoding.ParityType);
                USB::Endpoint_Write_8(CDCInterfaceInfo->State.LineEncoding.DataBits);
                USB::Endpoint_ClearIN();
                USB::Endpoint_ClearStatusStage();
            }
            break;
        case CDC_REQ_SetLineEncoding:
            if (USB_ControlRequest.bmRequestType ==
                    (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
            {
                USB::Endpoint_ClearSETUP();

                while (!(USB::Endpoint_IsOUTReceived()))
                {
                    if (USB_DeviceState == DEVICE_STATE_Unattached)
                      return;
                }

                CDCInterfaceInfo->State.LineEncoding.BaudRateBPS = USB::Endpoint_Read_32_LE();
                CDCInterfaceInfo->State.LineEncoding.CharFormat  = USB::Endpoint_Read_8();
                CDCInterfaceInfo->State.LineEncoding.ParityType  = USB::Endpoint_Read_8();
                CDCInterfaceInfo->State.LineEncoding.DataBits    = USB::Endpoint_Read_8();
                USB::Endpoint_ClearOUT();
                USB::Endpoint_ClearStatusStage();
                EVENT_CDC_Device_LineEncodingChanged(CDCInterfaceInfo);
            }

            break;
        case CDC_REQ_SetControlLineState:
            if (USB_ControlRequest.bmRequestType ==
                    (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
            {
                USB::Endpoint_ClearSETUP();
                USB::Endpoint_ClearStatusStage();
                CDCInterfaceInfo->State.ControlLineStates.HostToDevice = USB_ControlRequest.wValue;
                EVENT_CDC_Device_ControLineStateChanged(CDCInterfaceInfo);
            }
            break;
        case CDC_REQ_SendBreak:
            if (USB_ControlRequest.bmRequestType ==
                    (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
            {
                USB::Endpoint_ClearSETUP();
                USB::Endpoint_ClearStatusStage();
            }

            break;
    }
}

bool CDC_Device_ConfigureEndpoints(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo)
{
    memset(&CDCInterfaceInfo->State, 0x00, sizeof(CDCInterfaceInfo->State));
    CDCInterfaceInfo->Config.DataINEndpoint.Type = EP_TYPE_BULK;
    CDCInterfaceInfo->Config.DataOUTEndpoint.Type = EP_TYPE_BULK;
    CDCInterfaceInfo->Config.NotificationEndpoint.Type = EP_TYPE_INTERRUPT;

    if (!(Endpoint_ConfigureEndpointTable(&CDCInterfaceInfo->Config.DataINEndpoint, 1)))
        return false;

    if (!(Endpoint_ConfigureEndpointTable(&CDCInterfaceInfo->Config.DataOUTEndpoint, 1)))
        return false;

    if (!(Endpoint_ConfigureEndpointTable(&CDCInterfaceInfo->Config.NotificationEndpoint, 1)))
        return false;

    return true;
}

void USB::CDC_Device_USBTask(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo)
{
    if ((USB_DeviceState != DEVICE_STATE_Configured) ||
            !(CDCInterfaceInfo->State.LineEncoding.BaudRateBPS))
    {
        return;
    }

    Endpoint_SelectEndpoint(CDCInterfaceInfo->Config.DataINEndpoint.Address);

    if (Endpoint_IsINReady())
        USB::CDC_Device_Flush(CDCInterfaceInfo);
}

uint8_t CDC_Device_SendString(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo,
                              const char* const String)
{
    if ((USB_DeviceState != DEVICE_STATE_Configured) ||
        !(CDCInterfaceInfo->State.LineEncoding.BaudRateBPS))
    {
        return ENDPOINT_RWSTREAM_DeviceDisconnected;
    }

    USB::Endpoint_SelectEndpoint(CDCInterfaceInfo->Config.DataINEndpoint.Address);
    return USB::Endpoint_Write_Stream_LE(String, strlen(String), NULL);
}

uint8_t CDC_Device_SendData(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo,
                            const void* const Buffer,
                            const uint16_t Length)
{
    if ((USB_DeviceState != DEVICE_STATE_Configured) ||
            !(CDCInterfaceInfo->State.LineEncoding.BaudRateBPS))
    {
        return ENDPOINT_RWSTREAM_DeviceDisconnected;
    }

    USB::Endpoint_SelectEndpoint(CDCInterfaceInfo->Config.DataINEndpoint.Address);
    return USB::Endpoint_Write_Stream_LE(Buffer, Length, NULL);
}

uint8_t CDC_Device_SendByte(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo,
                            const uint8_t Data)
{
    if ((USB_DeviceState != DEVICE_STATE_Configured) ||
        !(CDCInterfaceInfo->State.LineEncoding.BaudRateBPS))
    {
        return ENDPOINT_RWSTREAM_DeviceDisconnected;
    }

    USB::Endpoint_SelectEndpoint(CDCInterfaceInfo->Config.DataINEndpoint.Address);

    if (!(USB::Endpoint_IsReadWriteAllowed()))
    {
        USB::Endpoint_ClearIN();
        uint8_t ErrorCode;

        if ((ErrorCode = Endpoint_WaitUntilReady()) != ENDPOINT_READYWAIT_NoError)
          return ErrorCode;
    }

    USB::Endpoint_Write_8(Data);
    return ENDPOINT_READYWAIT_NoError;
}

uint8_t USB::CDC_Device_Flush(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo)
{
    if ((USB_DeviceState != DEVICE_STATE_Configured) || !(CDCInterfaceInfo->State.LineEncoding.BaudRateBPS))
      return ENDPOINT_RWSTREAM_DeviceDisconnected;

    uint8_t ErrorCode;

    Endpoint_SelectEndpoint(CDCInterfaceInfo->Config.DataINEndpoint.Address);

    if (!(Endpoint_BytesInEndpoint()))
      return ENDPOINT_READYWAIT_NoError;

    bool BankFull = !(Endpoint_IsReadWriteAllowed());

    Endpoint_ClearIN();

    if (BankFull)
    {
        if ((ErrorCode = Endpoint_WaitUntilReady()) != ENDPOINT_READYWAIT_NoError)
            return ErrorCode;

        Endpoint_ClearIN();
    }

    return ENDPOINT_READYWAIT_NoError;
}

uint16_t CDC_Device_BytesReceived(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo)
{
    if ((USB_DeviceState != DEVICE_STATE_Configured) ||
        !(CDCInterfaceInfo->State.LineEncoding.BaudRateBPS))
    {
        return 0;
    }

    USB::Endpoint_SelectEndpoint(CDCInterfaceInfo->Config.DataOUTEndpoint.Address);

    if (USB::Endpoint_IsOUTReceived())
    {
        if (!(Endpoint_BytesInEndpoint()))
        {
            USB::Endpoint_ClearOUT();
            return 0;
        }
        else
        {
            return Endpoint_BytesInEndpoint();
        }
    }
    else
    {
        return 0;
    }
}

int16_t CDC_Device_ReceiveByte(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo)
{
    if ((USB_DeviceState != DEVICE_STATE_Configured) ||
            !(CDCInterfaceInfo->State.LineEncoding.BaudRateBPS))
    {
        return -1;
    }

    int16_t ReceivedByte = -1;
    USB::Endpoint_SelectEndpoint(CDCInterfaceInfo->Config.DataOUTEndpoint.Address);

    if (USB::Endpoint_IsOUTReceived())
    {
        if (Endpoint_BytesInEndpoint())
          ReceivedByte = USB::Endpoint_Read_8();

        if (!(Endpoint_BytesInEndpoint()))
          USB::Endpoint_ClearOUT();
    }

    return ReceivedByte;
}

void CDC_Device_SendControlLineStateChange(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo)
{
    if ((USB_DeviceState != DEVICE_STATE_Configured) || !(CDCInterfaceInfo->State.LineEncoding.BaudRateBPS))
      return;

    USB::Endpoint_SelectEndpoint(CDCInterfaceInfo->Config.NotificationEndpoint.Address);

    USB_Request_Header_t Notification = (USB_Request_Header_t)
        {
            .bmRequestType = (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE),
            .bRequest      = CDC_NOTIF_SerialState,
            .wValue        = CPU_TO_LE16(0),
            .wIndex        = CPU_TO_LE16(0),
            .wLength = CPU_TO_LE16(sizeof(CDCInterfaceInfo->State.ControlLineStates.DeviceToHost)),
        };

    USB::Endpoint_Write_Stream_LE(&Notification, sizeof(USB_Request_Header_t), NULL);
    USB::Endpoint_Write_Stream_LE(&CDCInterfaceInfo->State.ControlLineStates.DeviceToHost,
                             sizeof(CDCInterfaceInfo->State.ControlLineStates.DeviceToHost),
                             NULL);
    USB::Endpoint_ClearIN();
}

void USB::createStream(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo, FILE* const Stream)
{
    //*Stream = (FILE)FDEV_SETUP_STREAM(CDC_Device_putchar, CDC_Device_getchar, _FDEV_SETUP_RW);
    Stream->put = CDC_Device_putchar;
    Stream->get = CDC_Device_getchar;
    Stream->flags = _FDEV_SETUP_RW;
    Stream->udata = 0;
    fdev_set_udata(Stream, CDCInterfaceInfo);
}

static int CDC_Device_putchar(char c, FILE* Stream)
{
    return CDC_Device_SendByte((USB_ClassInfo_CDC_Device_t*)fdev_get_udata(Stream), c) ? _FDEV_ERR : 0;
}

static int CDC_Device_getchar(FILE* Stream)
{
    int16_t ReceivedByte = CDC_Device_ReceiveByte((USB_ClassInfo_CDC_Device_t*)fdev_get_udata(Stream));

    if (ReceivedByte < 0)
      return _FDEV_EOF;

    return ReceivedByte;
}

void CDC_Device_Event_Stub(void)
{
}

USB::USB()
{
    MCUSR &= ~(1 << WDRF);
    wdt_disable();
    clock_prescale_set(clock_div_1);
    init();
    createStream();
    GlobalInterruptEnable();
    cdcTask();
    USB_USBTask();
}

void USB::myPutc(char c)
{
    CDC_Device_SendByte((USB_ClassInfo_CDC_Device_t*)fdev_get_udata(&cdcStream), c);
}

uint8_t USB::readByte()
{
    return CDC_Device_ReceiveByte((USB_ClassInfo_CDC_Device_t*)fdev_get_udata(&cdcStream));
}

USB_ClassInfo_CDC_Device_t USB::cdcDevice =
{
    {
        0,
        {CDC_TX_EPADDR, CDC_TXRX_EPSIZE, 1,},
        {CDC_RX_EPADDR, CDC_TXRX_EPSIZE, 1,},
        {CDC_NOTIFICATION_EPADDR, CDC_NOTIFICATION_EPSIZE, 1,},
    },
};

void EVENT_USB_Device_ConfigurationChanged()
{
    bool ConfigSuccess = true;
    ConfigSuccess &= CDC_Device_ConfigureEndpoints(&USB::cdcDevice);
}

void EVENT_USB_Device_ControlRequest()
{
    CDC_Device_ProcessControlRequest(&USB::cdcDevice);
}

const USB_Descriptor_Device_t PROGMEM DeviceDescriptor =
{
    {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},
    VERSION_BCD(01.10),
    CDC_CSCP_CDCClass,
    CDC_CSCP_NoSpecificSubclass,
    CDC_CSCP_NoSpecificProtocol,
    FIXED_CONTROL_ENDPOINT_SIZE,
    .VendorID               = 0x03EB,
    .ProductID              = 0x204B,
    .ReleaseNumber          = VERSION_BCD(00.01),
    .ManufacturerStrIndex   = 0x01,
    .ProductStrIndex        = 0x02,
    .SerialNumStrIndex      = USE_INTERNAL_SERIAL,
    .NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor =
{
    .Config =
    {
         {.Size = sizeof(USB_Descriptor_Configuration_Header_t), .Type = DTYPE_Configuration},
             sizeof(USB_Descriptor_Configuration_t),
             2,
             1,
            .ConfigurationStrIndex  = NO_DESCRIPTOR,
            .ConfigAttributes       = (USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_SELFPOWERED),
            .MaxPowerConsumption    = USB_CONFIG_POWER_MA(100)
    },

    .CDC_CCI_Interface =
    {
        .Header      = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},
        .InterfaceNumber        = 0,
        .AlternateSetting       = 0,
        .TotalEndpoints         = 1,
        .Class                  = CDC_CSCP_CDCClass,
        .SubClass               = CDC_CSCP_ACMSubclass,
        .Protocol               = CDC_CSCP_ATCommandProtocol,
        .InterfaceStrIndex      = NO_DESCRIPTOR
    },

    .CDC_Functional_Header =
    {
    .Header = {.Size = sizeof(USB_CDC_Descriptor_FunctionalHeader_t), .Type = DTYPE_CSInterface},
        .Subtype      = CDC_DSUBTYPE_CSInterface_Header,
        .CDCSpecification       = VERSION_BCD(01.10),
    },

    .CDC_Functional_ACM =
    {
    .Header = {.Size = sizeof(USB_CDC_Descriptor_FunctionalACM_t), .Type = DTYPE_CSInterface},
        .Subtype = CDC_DSUBTYPE_CSInterface_ACM,
        .Capabilities = 0x06,
    },

    .CDC_Functional_Union =
    {
    .Header = {.Size = sizeof(USB_CDC_Descriptor_FunctionalUnion_t), .Type = DTYPE_CSInterface},
        .Subtype = CDC_DSUBTYPE_CSInterface_Union,
        .MasterInterfaceNumber  = 0,
        .SlaveInterfaceNumber   = 1,
    },

    .CDC_NotificationEndpoint =
        {
            .Header      = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},
            .EndpointAddress        = CDC_NOTIFICATION_EPADDR,
            .Attributes     = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
            .EndpointSize           = CDC_NOTIFICATION_EPSIZE,
            .PollingIntervalMS      = 0xFF
        },

    .CDC_DCI_Interface =
        {
            .Header  = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},
            .InterfaceNumber        = 1,
            .AlternateSetting       = 0,
            .TotalEndpoints         = 2,
            .Class                  = CDC_CSCP_CDCDataClass,
            .SubClass               = CDC_CSCP_NoDataSubclass,
            .Protocol               = CDC_CSCP_NoDataProtocol,

            .InterfaceStrIndex      = NO_DESCRIPTOR
        },

    .CDC_DataOutEndpoint =
        {
            .Header        = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},
            .EndpointAddress        = CDC_RX_EPADDR,
            .Attributes             = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
            .EndpointSize           = CDC_TXRX_EPSIZE,
            .PollingIntervalMS      = 0x05
        },

    .CDC_DataInEndpoint =
    {
        .Header    = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},
        .EndpointAddress        = CDC_TX_EPADDR,
        .Attributes             = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
        .EndpointSize           = CDC_TXRX_EPSIZE,
        .PollingIntervalMS      = 0x05
    }
};

const USB_Descriptor_String_t PROGMEM LanguageString =
{
    .Header                 = {.Size = USB_STRING_LEN(1), .Type = DTYPE_String}
    //.UnicodeString          = (wchar_t)0x0409
};

const USB_Descriptor_String_t PROGMEM ManufacturerString =
{
    {.Size = USB_STRING_LEN(11), .Type = DTYPE_String},
     //L"Dean Camera"
};

const USB_Descriptor_String_t PROGMEM ProductString =
{
    .Header                 = {.Size = USB_STRING_LEN(22), .Type = DTYPE_String},
    //.UnicodeString          = L"LUFA USB-RS232 Adapter"
};

uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                    const uint8_t wIndex,
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
                case 0x00:
                    Address = &LanguageString;
                    Size    = pgm_read_byte(&LanguageString.Header.Size);
                    break;
                case 0x01:
                    Address = &ManufacturerString;
                    Size    = pgm_read_byte(&ManufacturerString.Header.Size);
                    break;
                case 0x02:
                    Address = &ProductString;
                    Size    = pgm_read_byte(&ProductString.Header.Size);
                    break;
            }

            break;
    }
   *DescriptorAddress = Address;
    return Size;
}

static void USB_DeviceTask();
volatile bool USB_IsInitialized;
USB_Request_Header_t USB_ControlRequest;

void USB_USBTask(void)
{
    USB_DeviceTask();
}

static void USB_DeviceTask(void)
{
    if (USB_DeviceState == DEVICE_STATE_Unattached)
        return;

    uint8_t PrevEndpoint = USB::Endpoint_GetCurrentEndpoint();
    USB::Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);

    if (USB::Endpoint_IsSETUPReceived())
        USB_Device_ProcessControlRequest();

    USB::Endpoint_SelectEndpoint(PrevEndpoint);
}

void USB_Device_SendRemoteWakeup(void)
{
    if (!(USB::USB_Options & USB_OPT_MANUAL_PLL))
    {
        USB::USB_PLL_On();
        while (!(USB::USB_PLL_IsReady()));
    }

    USB::USB_CLK_Unfreeze();
    UDCON |= (1 << RMWKUP);
    while (UDCON & (1 << RMWKUP));
}

uint8_t Endpoint_Discard_Stream(uint16_t Length, uint16_t* const BytesProcessed)
{
    uint8_t  ErrorCode;
    uint16_t BytesInTransfer = 0;

    if ((ErrorCode = Endpoint_WaitUntilReady()))
        return ErrorCode;

    if (BytesProcessed != NULL)
        Length -= *BytesProcessed;

    while (Length)
    {
        if (!(USB::Endpoint_IsReadWriteAllowed()))
        {
            USB::Endpoint_ClearOUT();

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
            USB::Endpoint_Discard_8();
            Length--;
            BytesInTransfer++;
        }
    }

    return ENDPOINT_RWSTREAM_NoError;
}

uint8_t Endpoint_Null_Stream(uint16_t Length, uint16_t* const BytesProcessed)
{
    uint8_t  ErrorCode;
    uint16_t BytesInTransfer = 0;

    if ((ErrorCode = Endpoint_WaitUntilReady()))
      return ErrorCode;

    if (BytesProcessed != NULL)
      Length -= *BytesProcessed;

    while (Length)
    {
        if (!(USB::Endpoint_IsReadWriteAllowed()))
        {
            USB::Endpoint_ClearIN();

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
            USB::Endpoint_Write_8(0);
            Length--;
            BytesInTransfer++;
        }
    }

    return ENDPOINT_RWSTREAM_NoError;
}

#define  TEMPLATE_BUFFER_TYPE                      const void*
#define  TEMPLATE_CLEAR_ENDPOINT()                 USB::Endpoint_ClearIN()
#define  TEMPLATE_BUFFER_OFFSET(Length)            0
#define  TEMPLATE_BUFFER_MOVE(BufferPtr, Amount)   BufferPtr += Amount
#define  TEMPLATE_TRANSFER_BYTE(BufferPtr)         USB::Endpoint_Write_8(*BufferPtr)

uint8_t USB::Endpoint_Write_Stream_LE (TEMPLATE_BUFFER_TYPE const Buffer,
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
        if (!(USB::Endpoint_IsReadWriteAllowed()))
        {
            TEMPLATE_CLEAR_ENDPOINT();

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

#undef TEMPLATE_BUFFER_TYPE
#undef TEMPLATE_TRANSFER_BYTE
#undef TEMPLATE_CLEAR_ENDPOINT
#undef TEMPLATE_BUFFER_OFFSET
#undef TEMPLATE_BUFFER_MOVE

#define  TEMPLATE_BUFFER_TYPE                      void*
#define  TEMPLATE_CLEAR_ENDPOINT()                 USB::Endpoint_ClearOUT()
#define  TEMPLATE_BUFFER_OFFSET(Length)            0
#define  TEMPLATE_BUFFER_MOVE(BufferPtr, Amount)   BufferPtr += Amount
#define  TEMPLATE_TRANSFER_BYTE(BufferPtr)         *BufferPtr = USB::Endpoint_Read_8()

uint8_t Endpoint_Read_Stream_LE(TEMPLATE_BUFFER_TYPE const Buffer,
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
        if (!(USB::Endpoint_IsReadWriteAllowed()))
        {
            TEMPLATE_CLEAR_ENDPOINT();

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

#undef TEMPLATE_BUFFER_TYPE
#undef TEMPLATE_TRANSFER_BYTE
#undef TEMPLATE_CLEAR_ENDPOINT
#undef TEMPLATE_BUFFER_OFFSET
#undef TEMPLATE_BUFFER_MOVE

uint8_t Endpoint_Write_Control_Stream_LE(const void* const Buffer, uint16_t Length)
{
    uint8_t* DataStream = (uint8_t*)Buffer;
    bool LastPacketFull = false;

    if (Length > USB_ControlRequest.wLength)
        Length = USB_ControlRequest.wLength;
    else if (!(Length))
        USB::Endpoint_ClearIN();

    while (Length || LastPacketFull)
    {
        uint8_t USB_DeviceState_LCL = USB_DeviceState;

        if (USB_DeviceState_LCL == DEVICE_STATE_Unattached)
            return ENDPOINT_RWCSTREAM_DeviceDisconnected;
        else if (USB_DeviceState_LCL == DEVICE_STATE_Suspended)
            return ENDPOINT_RWCSTREAM_BusSuspended;
        else if (USB::Endpoint_IsSETUPReceived())
            return ENDPOINT_RWCSTREAM_HostAborted;
        else if (USB::Endpoint_IsOUTReceived())
            break;

        if (USB::Endpoint_IsINReady())
        {
            uint16_t BytesInEndpoint = Endpoint_BytesInEndpoint();

            while (Length && (BytesInEndpoint < USB_Device_ControlEndpointSize))
            {
                USB::Endpoint_Write_8(*DataStream);
                DataStream++;
                Length--;
                BytesInEndpoint++;
            }

            LastPacketFull = (BytesInEndpoint == USB_Device_ControlEndpointSize);
            USB::Endpoint_ClearIN();
        }
    }

    while (!(USB::Endpoint_IsOUTReceived()))
    {
        uint8_t USB_DeviceState_LCL = USB_DeviceState;

        if (USB_DeviceState_LCL == DEVICE_STATE_Unattached)
          return ENDPOINT_RWCSTREAM_DeviceDisconnected;
        else if (USB_DeviceState_LCL == DEVICE_STATE_Suspended)
          return ENDPOINT_RWCSTREAM_BusSuspended;
    }

    return ENDPOINT_RWCSTREAM_NoError;
}

uint8_t Endpoint_Read_Control_Stream_LE(void* const Buffer, uint16_t Length)
{
    uint8_t* DataStream = (uint8_t*)Buffer;

    if (!(Length))
        USB::Endpoint_ClearOUT();

    while (Length)
    {
        uint8_t USB_DeviceState_LCL = USB_DeviceState;

        if (USB_DeviceState_LCL == DEVICE_STATE_Unattached)
            return ENDPOINT_RWCSTREAM_DeviceDisconnected;
        else if (USB_DeviceState_LCL == DEVICE_STATE_Suspended)
            return ENDPOINT_RWCSTREAM_BusSuspended;
        else if (USB::Endpoint_IsSETUPReceived())
            return ENDPOINT_RWCSTREAM_HostAborted;

        if (USB::Endpoint_IsOUTReceived())
        {
            while (Length && Endpoint_BytesInEndpoint())
            {
                *DataStream = USB::Endpoint_Read_8();
                DataStream++;
                Length--;
            }

            USB::Endpoint_ClearOUT();
        }
    }

    while (!(USB::Endpoint_IsINReady()))
    {
        uint8_t USB_DeviceState_LCL = USB_DeviceState;

        if (USB_DeviceState_LCL == DEVICE_STATE_Unattached)
          return ENDPOINT_RWCSTREAM_DeviceDisconnected;
        else if (USB_DeviceState_LCL == DEVICE_STATE_Suspended)
          return ENDPOINT_RWCSTREAM_BusSuspended;
    }

    return ENDPOINT_RWCSTREAM_NoError;
}

uint8_t Endpoint_Write_Control_PStream_LE(const void* const Buffer, uint16_t Length)
{
    uint8_t* DataStream = (uint8_t*)Buffer;
    bool LastPacketFull = false;

    if (Length > USB_ControlRequest.wLength)
        Length = USB_ControlRequest.wLength;
    else if (!(Length))
        USB::Endpoint_ClearIN();

    while (Length || LastPacketFull)
    {
        uint8_t USB_DeviceState_LCL = USB_DeviceState;

        if (USB_DeviceState_LCL == DEVICE_STATE_Unattached)
            return ENDPOINT_RWCSTREAM_DeviceDisconnected;
        else if (USB_DeviceState_LCL == DEVICE_STATE_Suspended)
            return ENDPOINT_RWCSTREAM_BusSuspended;
        else if (USB::Endpoint_IsSETUPReceived())
            return ENDPOINT_RWCSTREAM_HostAborted;
        else if (USB::Endpoint_IsOUTReceived())
            break;

        if (USB::Endpoint_IsINReady())
        {
            uint16_t BytesInEndpoint = Endpoint_BytesInEndpoint();

            while (Length && (BytesInEndpoint < USB_Device_ControlEndpointSize))
            {
                USB::Endpoint_Write_8(pgm_read_byte(DataStream));
                DataStream++;
                Length--;
                BytesInEndpoint++;
            }

            LastPacketFull = (BytesInEndpoint == USB_Device_ControlEndpointSize);
            USB::Endpoint_ClearIN();
        }
    }

    while (!(USB::Endpoint_IsOUTReceived()))
    {
        uint8_t USB_DeviceState_LCL = USB_DeviceState;

        if (USB_DeviceState_LCL == DEVICE_STATE_Unattached)
          return ENDPOINT_RWCSTREAM_DeviceDisconnected;
        else if (USB_DeviceState_LCL == DEVICE_STATE_Suspended)
          return ENDPOINT_RWCSTREAM_BusSuspended;
    }

    return ENDPOINT_RWCSTREAM_NoError;
}


ISR(USB_GEN_vect, ISR_BLOCK)
{
    if (USB::USB_INT_HasOccurred(USB_INT_SOFI) && USB::USB_INT_IsEnabled(USB_INT_SOFI))
    {
        USB::USB_INT_Clear(USB_INT_SOFI);
        //EVENT_USB_Device_StartOfFrame();
    }

    if (USB::USB_INT_HasOccurred(USB_INT_VBUSTI) && USB::USB_INT_IsEnabled(USB_INT_VBUSTI))
    {
        USB::USB_INT_Clear(USB_INT_VBUSTI);

        if (USB::USB_VBUS_GetStatus())
        {
            if (!(USB::USB_Options & USB_OPT_MANUAL_PLL))
            {
                USB::USB_PLL_On();
                while (!(USB::USB_PLL_IsReady()));
            }

            USB_DeviceState = DEVICE_STATE_Powered;
            //EVENT_USB_Device_Connect();
        }
        else
        {
            if (!(USB::USB_Options & USB_OPT_MANUAL_PLL))
                USB::USB_PLL_Off();

            USB_DeviceState = DEVICE_STATE_Unattached;
            //EVENT_USB_Device_Disconnect();
        }
    }

    if (USB::USB_INT_HasOccurred(USB_INT_SUSPI) && USB::USB_INT_IsEnabled(USB_INT_SUSPI))
    {
        USB::USB_INT_Disable(USB_INT_SUSPI);
        USB::USB_INT_Enable(USB_INT_WAKEUPI);
        USB::USB_CLK_Freeze();

        if (!(USB::USB_Options & USB_OPT_MANUAL_PLL))
            USB::USB_PLL_Off();

        USB_DeviceState = DEVICE_STATE_Suspended;
        //EVENT_USB_Device_Suspend();
    }

    if (USB::USB_INT_HasOccurred(USB_INT_WAKEUPI) && USB::USB_INT_IsEnabled(USB_INT_WAKEUPI))
    {
        if (!(USB::USB_Options & USB_OPT_MANUAL_PLL))
        {
            USB::USB_PLL_On();
            while (!(USB::USB_PLL_IsReady()));
        }

        USB::USB_CLK_Unfreeze();
        USB::USB_INT_Clear(USB_INT_WAKEUPI);
        USB::USB_INT_Disable(USB_INT_WAKEUPI);
        USB::USB_INT_Enable(USB_INT_SUSPI);

        if (USB_Device_ConfigurationNumber)
            USB_DeviceState = DEVICE_STATE_Configured;
        else
            USB_DeviceState = (USB_Device_IsAddressSet()) ? DEVICE_STATE_Configured : DEVICE_STATE_Powered;

        //EVENT_USB_Device_WakeUp();
    }

    if (USB::USB_INT_HasOccurred(USB_INT_EORSTI) && USB::USB_INT_IsEnabled(USB_INT_EORSTI))
    {
        USB::USB_INT_Clear(USB_INT_EORSTI);
        USB_DeviceState = DEVICE_STATE_Default;
        USB_Device_ConfigurationNumber = 0;
        USB::USB_INT_Clear(USB_INT_SUSPI);
        USB::USB_INT_Disable(USB_INT_SUSPI);
        USB::USB_INT_Enable(USB_INT_WAKEUPI);

        Endpoint_ConfigureEndpoint(ENDPOINT_CONTROLEP, EP_TYPE_CONTROL,
                            USB_Device_ControlEndpointSize, 1);

        USB::USB_INT_Enable(USB_INT_RXSTPI);
        //EVENT_USB_Device_Reset();
    }
}

ISR(USB_COM_vect, ISR_BLOCK)
{
    uint8_t PrevSelectedEndpoint = USB::Endpoint_GetCurrentEndpoint();
    USB::Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);
    USB::USB_INT_Disable(USB_INT_RXSTPI);
    GlobalInterruptEnable();
    USB_Device_ProcessControlRequest();
    USB::Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);
    USB::USB_INT_Enable(USB_INT_RXSTPI);
    USB::Endpoint_SelectEndpoint(PrevSelectedEndpoint);
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

bool Endpoint_ConfigureEndpoint_Prv(const uint8_t Number, const uint8_t UECFG0XData,
                                    const uint8_t UECFG1XData)
{
    for (uint8_t EPNum = Number; EPNum < ENDPOINT_TOTAL_ENDPOINTS; EPNum++)
    {
        uint8_t UECFG0XTemp;
        uint8_t UECFG1XTemp;
        uint8_t UEIENXTemp;

        USB::Endpoint_SelectEndpoint(EPNum);

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

        USB::Endpoint_DisableEndpoint();
        UECFG1X &= ~(1 << ALLOC);
        USB::Endpoint_EnableEndpoint();
        UECFG0X = UECFG0XTemp;
        UECFG1X = UECFG1XTemp;
        UEIENX  = UEIENXTemp;

       if (!(USB::Endpoint_IsConfigured()))
          return false;
    }

    USB::Endpoint_SelectEndpoint(Number);
    return true;
}

void Endpoint_ClearEndpoints(void)
{
    UEINT = 0;

    for (uint8_t EPNum = 0; EPNum < ENDPOINT_TOTAL_ENDPOINTS; EPNum++)
    {
        USB::Endpoint_SelectEndpoint(EPNum);
        UEIENX  = 0;
        UEINTX  = 0;
        UECFG1X = 0;
        USB::Endpoint_DisableEndpoint();
    }
}

void USB::Endpoint_ClearStatusStage()
{
    if (USB_ControlRequest.bmRequestType & REQDIR_DEVICETOHOST)
    {
        while (!(USB::Endpoint_IsOUTReceived()))
        {
            if (USB_DeviceState == DEVICE_STATE_Unattached)
              return;
        }

        USB::Endpoint_ClearOUT();
    }
    else
    {
        while (!(USB::Endpoint_IsINReady()))
        {
            if (USB_DeviceState == DEVICE_STATE_Unattached)
              return;
        }

        USB::Endpoint_ClearIN();
    }
}

uint8_t Endpoint_WaitUntilReady(void)
{
    uint16_t TimeoutMSRem = USB_STREAM_TIMEOUT_MS;
    uint16_t PreviousFrameNumber = USB::USB_Device_GetFrameNumber();

    for (;;)
    {
        if (USB::Endpoint_GetEndpointDirection() == ENDPOINT_DIR_IN)
        {
            if (USB::Endpoint_IsINReady())
                return ENDPOINT_READYWAIT_NoError;
        }
        else
        {
            if (USB::Endpoint_IsOUTReceived())
                return ENDPOINT_READYWAIT_NoError;
        }

        uint8_t USB_DeviceState_LCL = USB_DeviceState;

        if (USB_DeviceState_LCL == DEVICE_STATE_Unattached)
            return ENDPOINT_READYWAIT_DeviceDisconnected;
        else if (USB_DeviceState_LCL == DEVICE_STATE_Suspended)
            return ENDPOINT_READYWAIT_BusSuspended;
        else if (USB::Endpoint_IsStalled())
            return ENDPOINT_READYWAIT_EndpointStalled;

        uint16_t CurrentFrameNumber = USB::USB_Device_GetFrameNumber();

        if (CurrentFrameNumber != PreviousFrameNumber)
        {
            PreviousFrameNumber = CurrentFrameNumber;

            if (!(TimeoutMSRem--))
                return ENDPOINT_READYWAIT_Timeout;
        }
    }
}

static void USB_Device_SetAddress();
static void USB_Device_SetConfiguration();
static void USB_Device_GetConfiguration();
static void USB_Device_GetDescriptor();
static void USB_Device_GetStatus();
static void USB_Device_ClearSetFeature();
static void USB_Device_GetInternalSerialDescriptor();
uint8_t USB_Device_ConfigurationNumber;
bool USB_Device_CurrentlySelfPowered;
bool USB_Device_RemoteWakeupEnabled;

void USB_Device_ProcessControlRequest()
{
    uint8_t* RequestHeader = (uint8_t*)&USB_ControlRequest;

    for (uint8_t RequestHeaderByte = 0; RequestHeaderByte < sizeof(USB_Request_Header_t);
                   RequestHeaderByte++)
    {
        *(RequestHeader++) = USB::Endpoint_Read_8();
    }

    EVENT_USB_Device_ControlRequest();

    if (USB::Endpoint_IsSETUPReceived())
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

    if (USB::Endpoint_IsSETUPReceived())
    {
        USB::Endpoint_ClearSETUP();
        USB::Endpoint_StallTransaction();
    }
}

static void USB_Device_SetAddress()
{
    uint8_t DeviceAddress = (USB_ControlRequest.wValue & 0x7F);
    USB::USB_Device_SetDeviceAddress(DeviceAddress);
    USB::Endpoint_ClearSETUP();
    USB::Endpoint_ClearStatusStage();
    while (!(USB::Endpoint_IsINReady()));
    USB::USB_Device_EnableDeviceAddress(DeviceAddress);
    USB_DeviceState = (DeviceAddress) ? DEVICE_STATE_Addressed : DEVICE_STATE_Default;
}

static void USB_Device_SetConfiguration()
{
    if ((uint8_t)USB_ControlRequest.wValue > FIXED_NUM_CONFIGURATIONS)
        return;


    USB::Endpoint_ClearSETUP();
    USB_Device_ConfigurationNumber = (uint8_t)USB_ControlRequest.wValue;
    USB::Endpoint_ClearStatusStage();

    if (USB_Device_ConfigurationNumber)
        USB_DeviceState = DEVICE_STATE_Configured;
    else
        USB_DeviceState=(USB_Device_IsAddressSet())?DEVICE_STATE_Configured:DEVICE_STATE_Powered;

    EVENT_USB_Device_ConfigurationChanged();
}

static void USB_Device_GetConfiguration()
{
	USB::Endpoint_ClearSETUP();
	USB::Endpoint_Write_8(USB_Device_ConfigurationNumber);
	USB::Endpoint_ClearIN();
	USB::Endpoint_ClearStatusStage();
}

static void USB_Device_GetInternalSerialDescriptor()
{
    struct
    {
        USB_Descriptor_Header_t Header;
        uint16_t UnicodeString[INTERNAL_SERIAL_LENGTH_BITS / 4];
    }
    SignatureDescriptor;

	SignatureDescriptor.Header.Type = DTYPE_String;
	SignatureDescriptor.Header.Size = USB_STRING_LEN(INTERNAL_SERIAL_LENGTH_BITS / 4);
	USB_Device_GetSerialString(SignatureDescriptor.UnicodeString);
	USB::Endpoint_ClearSETUP();
	Endpoint_Write_Control_Stream_LE(&SignatureDescriptor, sizeof(SignatureDescriptor));
	USB::Endpoint_ClearOUT();
}

static void USB_Device_GetDescriptor(void)
{
	const void* DescriptorPointer;
	uint16_t DescriptorSize;

	if (USB_ControlRequest.wValue == ((DTYPE_String << 8) | USE_INTERNAL_SERIAL))
	{
		USB_Device_GetInternalSerialDescriptor();
		return;
	}

    if ((DescriptorSize = CALLBACK_USB_GetDescriptor(USB_ControlRequest.wValue,
                          USB_ControlRequest.wIndex, &DescriptorPointer)) == NO_DESCRIPTOR)
	{
		return;
	}

	USB::Endpoint_ClearSETUP();
    Endpoint_Write_Control_PStream_LE(DescriptorPointer, DescriptorSize);
    USB::Endpoint_ClearOUT();
}

static void USB_Device_GetStatus(void)
{
    uint8_t CurrentStatus = 0;

    switch (USB_ControlRequest.bmRequestType)
    {
        case (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_DEVICE):
            if (USB_Device_CurrentlySelfPowered)
                CurrentStatus |= FEATURE_SELFPOWERED_ENABLED;

            if (USB_Device_RemoteWakeupEnabled)
                CurrentStatus |= FEATURE_REMOTE_WAKEUP_ENABLED;

            break;
        case (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_ENDPOINT):
            USB::Endpoint_SelectEndpoint((uint8_t)USB_ControlRequest.wIndex & ENDPOINT_EPNUM_MASK);
            CurrentStatus = USB::Endpoint_IsStalled();
            USB::Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);
			break;
        default:
            return;
	}

	USB::Endpoint_ClearSETUP();
	USB::Endpoint_Write_16_LE(CurrentStatus);
	USB::Endpoint_ClearIN();
	USB::Endpoint_ClearStatusStage();
}

uint32_t USB::Endpoint_Read_32_LE()
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

static void USB_Device_ClearSetFeature(void)
{
	switch (USB_ControlRequest.bmRequestType & CONTROL_REQTYPE_RECIPIENT)
	{
    case REQREC_DEVICE:
        if ((uint8_t)USB_ControlRequest.wValue == FEATURE_SEL_DeviceRemoteWakeup)
            USB_Device_RemoteWakeupEnabled = (USB_ControlRequest.bRequest == REQ_SetFeature);
        else
            return;

        break;
    case REQREC_ENDPOINT:
        if ((uint8_t)USB_ControlRequest.wValue == FEATURE_SEL_EndpointHalt)
        {
            uint8_t EndpointIndex = ((uint8_t)USB_ControlRequest.wIndex & ENDPOINT_EPNUM_MASK);

            if (EndpointIndex == ENDPOINT_CONTROLEP)
                return;

            USB::Endpoint_SelectEndpoint(EndpointIndex);

            if (USB::Endpoint_IsEnabled())
            {
                if (USB_ControlRequest.bRequest == REQ_SetFeature)
                {
                    USB::Endpoint_StallTransaction();
                }
                else
                {
                    USB::Endpoint_ClearStall();
                    Endpoint_ResetEndpoint(EndpointIndex);
                    USB::Endpoint_ResetDataToggle();
                }
            }
        }

        break;
    default:
        return;
	}

    USB::Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);
    USB::Endpoint_ClearSETUP();
    USB::Endpoint_ClearStatusStage();
}

bool USB::Endpoint_HasEndpointInterrupted(const uint8_t Address)
{
    return ((Endpoint_GetEndpointInterrupts() &
            (1 << (Address & ENDPOINT_EPNUM_MASK))) ? true : false);
}

void USB::Endpoint_SetEndpointDirection(const uint8_t DirectionMask)
{
    UECFG0X = ((UECFG0X & ~(1 << EPDIR)) | (DirectionMask ? (1 << EPDIR) : 0));
}

void Endpoint_Write_32_BE(const uint32_t Data)
{
    UEDATX = (Data >> 24);
    UEDATX = (Data >> 16);
    UEDATX = (Data >> 8);
    UEDATX = (Data &  0xFF);
}

void USB::Endpoint_Write_16_LE(const uint16_t Data)
{
    UEDATX = (Data & 0xFF);
    UEDATX = (Data >> 8);
}

bool USB::Endpoint_IsOUTReceived()
{
    return ((UEINTX & (1 << RXOUTI)) ? true : false);
}

void USB::Endpoint_Discard_32()
{
    uint8_t Dummy;
    Dummy = UEDATX;
    Dummy = UEDATX;
    Dummy = UEDATX;
    Dummy = UEDATX;
    (void)Dummy;
}

void USB::USB_INT_Clear(const uint8_t Interrupt)
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

bool USB::USB_INT_IsEnabled(const uint8_t Interrupt)
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

bool USB::Endpoint_IsSETUPReceived()
{
    return ((UEINTX & (1 << RXSTPI)) ? true : false);
}

bool USB::Endpoint_IsReadWriteAllowed()
{
    return ((UEINTX & (1 << RWAL)) ? true : false);
}

bool USB::Endpoint_IsConfigured() {
    return ((UESTA0X & (1 << CFGOK)) ? true : false);
}

void USB::Endpoint_Write_32_LE(const uint32_t Data)
{
    UEDATX = (Data &  0xFF);
    UEDATX = (Data >> 8);
    UEDATX = (Data >> 16);
    UEDATX = (Data >> 24);
}

void USB::Endpoint_Write_16_BE(const uint16_t Data)
{
    UEDATX = (Data >> 8);
    UEDATX = (Data & 0xFF);
}

void USB::Endpoint_Discard_16()
{
    uint8_t Dummy;
    Dummy = UEDATX;
    Dummy = UEDATX;
    (void)Dummy;
}

void USB::Endpoint_AbortPendingIN()
{
    while (Endpoint_GetBusyBanks() != 0)
    {
        UEINTX |= (1 << RXOUTI);
        while (UEINTX & (1 << RXOUTI));
    }
}

uint16_t USB::Endpoint_Read_16_LE()
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

uint16_t USB::Endpoint_Read_16_BE()
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

uint32_t USB::Endpoint_Read_32_BE()
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

void USB::USB_INT_Enable(const uint8_t Interrupt)
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

void USB::USB_INT_Disable(const uint8_t Interrupt)
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

bool USB::USB_INT_HasOccurred(const uint8_t Interrupt)
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

void USB::Endpoint_Discard_8()
{
    uint8_t Dummy;
    Dummy = UEDATX;
    (void)Dummy;
}





