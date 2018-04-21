#ifndef _WEBTEST1_H_
#define _WEBTEST1_H_
#include "types.h"

class Client;
class ostream;

class WebTest1
{
private:
    ostream * const _serial;
    uint8_t _count = 0;
public:
    WebTest1(ostream *serial);
    void dispatch(Client &client);
};
#endif



