#ifndef _WEBSERVER_H_
#define _WEBSERVER_H_
#include "types.h"

class Buffer;
class Client;
class ostream;
class Fatty;
class Fyle;

class Webserver
{
private:
    Fatty * const _fs;
    ostream * const _serial;
    void httpGet(Client &client, Buffer &buffer);
    void contentType(Client &client, const char *ext);
    void httpDelete(Buffer &buffer);
    void serveFile(Client &client, const char *fn);
    void listing(Client &client) const;
    void printDirectory(Fyle dir, uint8_t numTabs, Client &os) const;
public:
    Webserver(Fatty *fs, ostream *serial);
    void dispatch(Client &client);
};
#endif



