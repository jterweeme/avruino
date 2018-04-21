#ifndef _WEBSERVER_H_
#define _WEBSERVER_H_
#include "fatty.h"
#include "client.h"

class Buffer;

class Webserver
{
private:
    //EthernetServer * const _svr;
    Fatty * const _fs;
    ostream * const _serial;
    void httpGet(Client &client, Buffer &buffer);
    void contentType(Client &client, const char *ext);
    void httpDelete(Buffer &buffer);
    void serveFile(Client &client, const char *fn);
    void listing(Client &client) const;
    void printDirectory(Fyle dir, uint8_t numTabs, Client &os) const;
public:
    Webserver(Fatty *fs, ostream *serial) : _fs(fs), _serial(serial) { }
#if 0
    Webserver(EthernetServer *svr, Fatty *fs, ostream *serial)
        : _svr(svr), _fs(fs), _serial(serial) { }
#endif
    void run();
    void dispatch(Client &client);
};
#endif



