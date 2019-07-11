#ifndef _WEBTEST1_H_
#define _WEBTEST1_H_
#include "types.h"

class Client;
class ostream;

class WebService1
{
private:
    ostream * const _serial;    // voor debug/log berichten
    uint8_t _count = 0;
public:
    WebService1(ostream *serial);
    void dispatch(Client &client);
};
#endif



