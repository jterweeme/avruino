#ifndef _CDC_H_
#define _CDC_H_

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include "busby.h"

struct LineEncoding
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
    LineEncoding _lineEncoding;
    void customCtrl();
    void configure();
    uint16_t getDesc(uint16_t wValue, uint16_t wIndex, const void ** const descAddr);
public:
    int16_t receive();
    uint8_t sendByte(uint8_t data);
    uint8_t flush();
    CDC();
};


#endif



