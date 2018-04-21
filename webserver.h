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
    void httpGet(Client &client, Buffer &buffer);
    void contentType(Client &client, const char *ext);
    void httpDelete(Buffer &buffer);
    void serveFile(Client &client, const char *fn);
    void listing(Client &client) const;
    void printDirectory(Fyle dir, uint8_t numTabs, Client &os) const;
public:
    Webserver(EthernetServer *svr, Fatty *fs, ostream *serial)
        : _svr(svr), _fs(fs), _serial(serial) { }
    void run();
    EthernetClient available() { return _svr->available(); }
    void dispatch(Client &client);
};
#endif



