#ifndef _USBJOY_H_
#define _USBJOY_H_
#include "usbhid.h"
#include "busby.h"

struct JoyReportData
{
    int8_t x;
    int8_t y;
    int8_t z;
    uint8_t button;
}
__attribute__ ((packed));

class USBJoy : public USB
{
private:
    Endpoint _inpoint;
    uint16_t getDesc(uint16_t value, uint16_t wIndex, const void ** const descAddr);
    void configure();
public:
    void sendReport(JoyReportData &report);
    USBJoy();
};

#endif



