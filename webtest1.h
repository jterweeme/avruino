#ifndef _WEBTEST1_H_
#define _WEBTEST1_H_
#include "stream.h"
#include "uip_client.h"

class WebTest1
{
private:
    ostream * const _serial;
    uint8_t _count = 0;
public:
    WebTest1(ostream *serial) : _serial(serial) { }
    void dispatch(UIPClient &client);
};

#endif



