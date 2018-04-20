#ifndef _WEBSERVER_H_
#define _WEBSERVER_H_
#include "fatty.h"
#include "EthernetServer.h"

class Buffer;

class Webserver
{
private:
    EthernetServer * const _svr;
    Fatty * const _fs;
    ostream * const _serial;
    void httpGet(EthernetClient &client, Buffer &buffer);
    void contentType(EthernetClient &client, const char *ext);
    void httpDelete(Buffer &buffer);
    void serveFile(EthernetClient &client, const char *fn);
    void listing(EthernetClient &client) const;
    void printDirectory(Fyle dir, uint8_t numTabs, EthernetClient &os) const;
public:
    Webserver(EthernetServer *svr, Fatty *fs, ostream *serial)
        : _svr(svr), _fs(fs), _serial(serial) { }
    void run();
};

#endif



