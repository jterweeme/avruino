#include "usbjoy.h"
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#define NULL 0

static constexpr uint8_t
    INTERFACE_ID_JOYSTICK = 0;


static const uint8_t PROGMEM joystickReport[] =
{
#if 0
    GLOBAL | USAGE_PAGE | 1, 0x01,
    LOCAL  | USAGE      | 1, 0x04,
    MAIN   | COLLECTION | 1, 0x01,
    GLOBAL | USAGE_PAGE | 1, 0x09,
    LOCAL  | USAGE_MIN  | 1, 0x01,
    LOCAL  | USAGE_MAX  | 1, 0x06,
    GLOBAL | LOGICAL_MIN | 1, 0x00,
    GLOBAL | LOGICAL_MAX | 1, 0x01,
    GLOBAL | REPORT_COUNT | 1, 0x06,
    GLOBAL | REPORT_SIZE | 1, 0x01,
#else
    GLOBAL | USAGE_PAGE | 0x01, ((0x01) & 0xFF),
    LOCAL  | USAGE | 0x01, ((0x04) & 0xFF),
    MAIN   | 0xA0 | 0x01, ((0x01) & 0xFF),
    LOCAL  | 0x00 | 0x01, ((0x01) & 0xFF),
    MAIN   | 0xA0 | 0x01, ((0x00) & 0xFF),
    LOCAL  | 0x00 | 0x01, ((0x30) & 0xFF),
    LOCAL  | 0x00 | 0x01, ((0x31) & 0xFF),
    LOCAL  | 0x00 | 0x01, ((0x32) & 0xFF),
    GLOBAL | 0x10 | 0x01, ((-100) & 0xFF),
    GLOBAL | 0x20 | 0x01, 100,
    GLOBAL | 0x30 | 0x01, -1 & 0xff,
    GLOBAL | 0x40 | 0x01, ((1) & 0xFF),
    GLOBAL | 0x90 | 0x01, ((0x03) & 0xFF),
    GLOBAL | 0x70 | 0x01, ((0x08) & 0xFF),
    MAIN   | 0x80 | 0x01, (((0 << 0) | (1 << 1) | (0 << 2)) & 0xFF),
    MAIN   | 0xC0 | 0x00,
    GLOBAL | 0x00 | 0x01, 0x09,
    0x08 | 0x10 | 0x01, 0x01,
    0x08 | 0x20 | 0x01, 0x02,
    0x04 | 0x10 | 0x01, 0x00,
    0x04 | 0x20 | 0x01, 0x01,
    0x04 | 0x70 | 0x01, 0x01,
    0x04 | 0x90 | 0x01, 0x02,
    0x00 | 0x80 | 0x01, (((0 << 0) | (1 << 1) | (0 << 2)) & 0xFF),
    0x04 | 0x70 | 0x01, ((0x06) & 0xFF),
    0x04 | 0x90 | 0x01, ((0x01) & 0xFF),
    0x00 | 0x80 | 0x01, (((1<<0)) & 0xFF),
    0x00 | 0xC0 | 0x00
#endif
};

static const DescDev PROGMEM devDesc =
{
    sizeof(DescDev),
    DTYPE_DEVICE,
    0x0110,
    0,
    0,
    0,
    8,
    0x03eb,
    0x2042,
    0x0001,
    STRING_ID_MANUFACTURER,
    STRING_ID_PRODUCT,
    0,
    1
};

struct MyConf
{
    DescConf config;
    DescIface hidInterface;
    HIDDesc hidJoy;
    DescEndpoint inpoint;
}
__attribute__ ((packed));

static const MyConf PROGMEM myConf
{
    {
        sizeof(DescConf),
        DTYPE_CONFIGURATION,
        sizeof(MyConf),
        1,  // 1 interface
        1,  // configuration 1
        0,
        USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_SELFPOWERED,
        USB_CONFIG_POWER_MA(100)
    },
    {
        sizeof(DescIface),
        DTYPE_INTERFACE,
        INTERFACE_ID_JOYSTICK,
        0,  // alternate settings
        1,  // 1 endpoint
        HID_CSCP_HIDClass,
        HID_CSCP_BootSubclass,
        HID_CSCP_JoystickBootProtocol,
        0
    },
    {
        sizeof(HIDDesc),
        HID_DTYPE_HID,
        0x0111,
        0,  // country
        1,  // 1 report descriptor
        HID_DTYPE_Report,
        sizeof(joystickReport)
    },
    {
        sizeof(DescEndpoint),
        DTYPE_ENDPOINT,
        ENDPOINT_DIR_IN | 1,
        EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA,
        8,
        5
    }
};

USBJoy::USBJoy() : _inpoint(ENDPOINT_DIR_IN | 1, 8, EP_TYPE_INTERRUPT, 1)
{
    *p_uhwcon |= 1<<uvrege;    // usb reg on
    *p_usbcon &= ~(1<<vbuste); // disable INT vbuste
    *p_udien = 0;
    *p_usbint = 0;
    *p_udint = 0;
    *p_usbcon &= ~(1<<usbe);   // disable usb controller
    *p_usbcon |= 1<<usbe;      // enable usb controller
    *p_usbcon &= ~(1<<frzclk);
    *p_pllcsr = 0;
    state = DEVICE_STATE_UNATTACHED;
    confNum = 0;
    _remoteWakeupEn = false;
    USB_Device_CurrentlySelfPowered = false;
    UDCON &= ~(1<<lsm);         // full speed
    USBCON |= 1<<vbuste;        // enable vbus int
    configureEndpoint(_control);
    UDINT &= ~(1<<suspi);
    UDIEN |= 1<<suspe;
    UDIEN |= 1<<eorste;
    UDCON &= ~(1<<detach);
    USBCON |= 1<<otgpade;
    sei();
}

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

static const DescString<13> PROGMEM productString =
{
    USB_STRING_LEN(12),
    DTYPE_STRING,
    L"USB Joystick"
};

uint16_t USBJoy::getDesc(uint16_t wValue, uint16_t wIndex, const void ** const descAddr)
{
    const void *addr = NULL;
    uint16_t size = 0;

    switch (wValue >> 8)
    {
    case DTYPE_DEVICE:
        addr = &devDesc;
        size = sizeof(DescDev);
        break;
    case DTYPE_CONFIGURATION:
        addr = &myConf;
        size = sizeof(MyConf);
        break;
    case DTYPE_STRING:
        switch (wValue & 0xff)
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
    case HID_DTYPE_HID:
        addr = &myConf.hidJoy;
        size = sizeof(HIDDesc);
        break;
    case HID_DTYPE_Report:
        addr = &joystickReport;
        size = sizeof(joystickReport);
        break;
    }

    *descAddr = addr;
    return size;
}

void USBJoy::configure()
{
    configureEndpoint(_inpoint);
    *p_udien |= 1<<sofe;
}

void USBJoy::sendReport(JoyReportData &report)
{
    _inpoint.select();
    writeStream2(&report, sizeof(report), NULL);
    *p_ueintx &= ~(1<<txini | 1<<fifocon);
}


