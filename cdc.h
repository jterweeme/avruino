#ifndef _CDC_H_
#define _CDC_H_

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include "busby.h"

static constexpr uint8_t
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
    CDC_REQ_SendBreak = 0x23,
    CDC_NOTIF_SerialState = 0x20,
    CDC_LINEENCODING_OneStopBit = 0,
    CDC_LINEENCODING_OneAndAHalfStopBits = 1,
    CDC_LINEENCODING_TwoStopBits = 2;

struct CDC_LineEncoding_t
{
    uint32_t BaudRateBPS;
    uint8_t CharFormat;
    uint8_t ParityType;
    uint8_t DataBits;
};

class CDC : public USB
{
private:
    Endpoint _inpoint;
    Endpoint _outpoint;
    Endpoint _notif;
    CDC_LineEncoding_t _lineEncoding;
    bool configureEndpoints();
    void EVENT_USB_Device_ControlRequest();
    void procCtrlReq();
    uint16_t getDescriptor(uint16_t wValue, uint8_t wIndex, const void **descAddr);
public:
    int16_t receive();
    uint8_t sendByte(uint8_t data);
    CDC();
    uint8_t flush();
};


#endif



