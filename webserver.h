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
    void _httpGet(ostream &client, Buffer &buffer);
    void _contentType(ostream &client, const char *ext);
    void _httpDelete(ostream &client, Buffer &buffer);
    void _httpMkcol(Buffer &buffer);
    void _serveFile(ostream &client, const char *fn);
    void _servePHP(ostream &client, const char *fn);
    void _listing(ostream &client) const;
    void _printDirectory(Fyle dir, uint8_t numTabs, ostream &os) const;
public:
    Webserver(Fatty *fs, ostream *serial);
    void dispatch(Client &client);
};
#endif



