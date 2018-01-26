#ifndef _USBHID_H_
#define _USBHID_H_

static const uint8_t
    HID_DTYPE_HID = 0x21,
    HID_DTYPE_Report = 0x22,
    HID_CSCP_BootSubclass = 1,
    HID_CSCP_KeyboardBootProtocol = 1,
    HID_CSCP_MouseBootProtocol = 2,
    HID_CSCP_JoystickBootProtocol = 3,
    HID_CSCP_HIDClass = 3,
    MAIN = 0,
    GLOBAL = 4,
    LOCAL = 8,
    USAGE = 0,
    USAGE_PAGE = 0,
    USAGE_MIN = 0x10,
    USAGE_MAX = 0x20,
    LOGICAL_MIN = 0x10,
    LOGICAL_MAX = 0x20,
    PHYSICAL_MIN = 0x30,
    PHYSICAL_MAX = 0x40,
    HID_INPUT = 0x80,
    HID_OUTPUT = 0x90,
    REPORT_SIZE = 0x70,
    REPORT_ID = 0x80,
    REPORT_COUNT = 0x90,
    COLLECTION = 0xa0,
    END_COLLECTION = 0xc0;

struct HIDDesc
{
    uint8_t size;
    uint8_t type;
    uint16_t HIDSpec; /**< BCD encoded vernd device complies to. */
    uint8_t CountryCode; /**< Country codce, or zero if universal. */
    uint8_t TotalReportDescriptors; /**< riptors for the interface. */
    uint8_t HIDReportType; /**< Tyort, set to \ref HID_DTYPE_Report. */
    uint16_t HIDReportLength; /**< report descriptor, in bytes. */
}
__attribute__ ((packed));

#endif



