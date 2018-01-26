#include "usbkb.h"
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "misc.h"
#include "usbhid.h"

static const DescDev PROGMEM devDesc =
{
    sizeof(DescDev),
    DTYPE_DEVICE,
    0x0110,
    0,
    0,
    0,
    8,
    0x03EB,
    0x2042,
    0x0001,
    STRING_ID_MANUFACTURER,
    STRING_ID_PRODUCT,
    0,
    1
};

static const uint8_t PROGMEM KeyboardReport[] =
{
    GLOBAL | USAGE_PAGE     | 1, 0x01,
    LOCAL  | USAGE          | 1, 0x06,
    MAIN   | COLLECTION     | 1, 0x01,
    GLOBAL | USAGE_PAGE     | 1, 0x07,
    LOCAL  | USAGE_MIN      | 1, 0xE0,
    LOCAL  | USAGE_MAX      | 1, 0xE7,
    GLOBAL | LOGICAL_MIN    | 1, 0x00,
    GLOBAL | LOGICAL_MAX    | 1, 0x01,
    GLOBAL | REPORT_SIZE    | 1, 0x01,
    GLOBAL | REPORT_COUNT   | 1, 0x08,
    MAIN   | HID_INPUT      | 1, 1<<1,
    GLOBAL | REPORT_COUNT   | 1, 0x01,
    GLOBAL | REPORT_SIZE    | 1, 0x08,
    MAIN   | HID_INPUT      | 1, 1<<0,
    GLOBAL | USAGE_PAGE     | 1, 0x08,
    LOCAL  | USAGE_MIN      | 1, 0x01,
    LOCAL  | USAGE_MAX      | 1, 0x05,
    GLOBAL | REPORT_COUNT   | 1, 0x05,
    GLOBAL | REPORT_SIZE    | 1, 0x01, 
    MAIN   | OUTPUT         | 1, 1<<1,
    GLOBAL | REPORT_COUNT   | 1, 0x01,
    GLOBAL | REPORT_SIZE    | 1, 0x03,
    MAIN   | OUTPUT         | 1, 1<<0,
    GLOBAL | LOGICAL_MIN    | 1, 0x00,
    GLOBAL | LOGICAL_MAX    | 1, 0x65,
    GLOBAL | USAGE_PAGE     | 1, 0x07,
    LOCAL  | USAGE_MIN      | 1, 0x00,
    LOCAL  | USAGE_MAX      | 1, 0x65,
    GLOBAL | REPORT_COUNT   | 1, 0x06,
    GLOBAL | REPORT_SIZE    | 1, 0x08,
    MAIN   | HID_INPUT      | 1, 0,
    MAIN   | END_COLLECTION | 0
};

struct MyConf
{
    DescConf config;
    DescIface hidInterface;
    HIDDesc HID_KeyboardHID;
    DescEndpoint HID_ReportINEndpoint;
    DescEndpoint HID_ReportOUTEndpoint;
}
__attribute__ ((packed));

static const MyConf PROGMEM myConf =
{
    {
        sizeof(DescConf),
        DTYPE_CONFIGURATION,
        sizeof(MyConf),
        1,
        1,
        0,
        USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_SELFPOWERED,
        USB_CONFIG_POWER_MA(100)
    },
    {
        sizeof(DescIface),
        DTYPE_INTERFACE,
        INTERFACE_ID_Keyboard,
        0,
        2,
        HID_CSCP_HIDClass,
        HID_CSCP_BootSubclass,
        HID_CSCP_KeyboardBootProtocol,
        0
    },
    {
        sizeof(HIDDesc),
        HID_DTYPE_HID,
        0x0111,
        0,
        1,
        HID_DTYPE_Report,
        sizeof(KeyboardReport)
    },
    {
        sizeof(DescEndpoint),
        DTYPE_ENDPOINT,
        ENDPOINT_DIR_IN | 1,
        EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA,
        8,
        5
    },
    {
        sizeof(DescEndpoint),
        DTYPE_ENDPOINT,
        ENDPOINT_DIR_OUT | 2,
        EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA,
        8,
        5
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

static const DescString<13> PROGMEM productString =
{
    USB_STRING_LEN(12),
    DTYPE_STRING,
    L"USB Keyboard"
};

USBKB::USBKB() :
    _inpoint(KEYBOARD_IN_EPADDR, KEYBOARD_EPSIZE, EP_TYPE_INTERRUPT, 1),
    _outpoint(KEYBOARD_OUT_EPADDR, KEYBOARD_EPSIZE, EP_TYPE_INTERRUPT, 1)
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
    *p_udcon &= ~(1<<lsm);         // full speed
    *p_usbcon |= 1<<vbuste;        // enable vbus int
    configureEndpoint(_control);
    *p_udint &= ~(1<<suspi);
    *p_udien |= 1<<suspe;
    *p_udien |= 1<<eorste;
    *p_udcon &= ~(1<<detach);
    *p_usbcon |= 1<<otgpade;
    sei();
}

uint16_t USBKB::getDesc(uint16_t wValue, uint16_t wIndex, const void ** const descAddr)
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
        addr = &myConf.HID_KeyboardHID;
        size = sizeof(HIDDesc);
        break;
    case HID_DTYPE_Report:
        addr = &KeyboardReport;
        size = sizeof(KeyboardReport);
        break;
    }

    *descAddr = addr;
    return size;
}

void USBKB::sendReport(KBReport &report)
{
    _inpoint.select();
    writeStream2(&report, sizeof(report), NULL);
    *p_ueintx &= ~(1<<txini | 1<<fifocon);
    _outpoint.select();

    if (*p_ueintx & 1<<rxouti)
    {
        if (*p_ueintx & 1<<rwal)    // r/w allowed?
            read8();

        UEINTX &= ~(1<<RXOUTI | 1<<FIFOCON);
    }
}

void USBKB::configure()
{
    configureEndpoint(_inpoint);
    configureEndpoint(_outpoint);
    *p_udien |= 1<<sofe;
}




