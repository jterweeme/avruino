#include "webserver.h"
#include "client.h"
#include "fatty.h"

class Buffer
{
    char _buf[512] = {0};
    uint16_t _pos = 0;
public:
    void push(char c) { _buf[_pos++] = c; }
    char *get() { return _buf; }
    void reset();
    bool end() const;
};

Webserver::Webserver(Fatty *fs, ostream *serial) : _fs(fs), _serial(serial)
{
}

void Webserver::printDirectory(Fyle dir, uint8_t numTabs, Client &os) const
{
    os << "<table>\r\n";

    while (true)
    {
        Fyle entry = dir.openNextFile();

        if (!entry)
            break;

        for (uint8_t i = 0; i < numTabs; i++)
            os << "\t";

        os << "<tr><td><a href=\"";
        os << entry.name();
        os << "\">";
        os << entry.name();
        os << "</a></td>";

        if (entry.isDirectory())
        {
            os << "<td>DIR</td>";
#if 0
            os.put('/');
            printDirectory(entry, numTabs + 1, os);
#endif
        }
        else
        {
            os << "<td>";
            uint32_t size = entry.size();
            os.put(nibble(size >> 28 & 0xf));
            os.put(nibble(size >> 24 & 0xf));
            os.put(nibble(size >> 20 & 0xf));
            os.put(nibble(size >> 16 & 0xf));
            os.put(nibble(size >> 12 & 0xf));
            os.put(nibble(size >> 8 & 0xf));
            os.put(nibble(size >> 4 & 0xf));
            os.put(nibble(size & 0xf));
            os << "</td>";
        }
        entry.close();
        os << "<td>D</td>\r\n</tr>\r\n";
    }

    os << "</table>\r\n";
}

void Webserver::listing(Client &client) const
{
    client << "<!DOCTYPE html>\r\n";
    client << "<html>\r\n<head>\r\n<title>Listing</title>\r\n";
    client << "</head>\r\n<body>\r\n<h1>Listing</h1>\r\n";
    Fyle root = _fs->open("/");
    printDirectory(root, 0, client);
    root.close();
    client << "</body>\r\n</html>\r\n";
}

void Webserver::serveFile(Client &client, const char *fn)
{
    FyleIfstream ifs(_fs);
    ifs.open(fn);

    for (int c2; (c2 = ifs.get()) != -1;)
        client.write(c2);

    ifs.close();
}

void Webserver::contentType(Client &client, const char *ext)
{
    if (my_strncasecmp(ext, "htm", 3) == 0)
        client << "Content-Type: text/html\r\n";
    else if (my_strncasecmp(ext, "svg", 3) == 0)
        client << "Content-Type: image/svg+xml\r\n";
    else if (my_strncasecmp(ext, "jpg", 3) == 0)
        client << "Content-Type: image/jpeg\r\n";
    else if (my_strncasecmp(ext, "css", 3) == 0)
        client << "Content-Type: text/css\r\n";
    else if (my_strncasecmp(ext, "js", 2) == 0)
        client << "Content-Type: text/js\r\n";
    else if (my_strncasecmp(ext, "zip", 3) == 0)
        client << "Content-Type: application/zip\r\n";
    else if (my_strncasecmp(ext, "7z", 2) == 0)
        client << "Content-Type: application/x-7z-compressed\r\n";
    else if (my_strncasecmp(ext, "cpp", 3) == 0)
        client << "Content-Type: text/plain\r\n";
    else
        client << "Content-Type: text/html\r\n";
}

void Webserver::httpGet(Client &client, Buffer &buffer)
{
    char fn[100] = {0};
    char ext[10] = {0};
    
    for (uint16_t i = 5; i < 512; i++)
    {
        if (buffer.get()[i] != ' ')
            fn[i - 5] = buffer.get()[i];
        else
            break;
    }
    
    if (fn[0] == 0)
        strncpy(fn, "index.htm", 100);

    char *dot = my_strchr(fn, '.');
    my_strncpy(ext, dot + 1, 3);
    client << "HTTP/1.1 200 OK\r\n";
    contentType(client, ext);
    client << "Connection: close\r\n\r\n";  // let op de dubbel nl

    if (strncmp(fn, "listing", 7) == 0)
        listing(client);
    else
        serveFile(client, fn);
}

void Webserver::httpDelete(Buffer &buffer)
{
    char fn[50];

    for (uint16_t i = 8; i < 512; i++)
    {
        if (buffer.get()[i] != ' ')
            fn[i - 8] = buffer.get()[i];
        else
            break;
    }

    _fs->remove(fn);
    *_serial << buffer.get();
    *_serial << "\r\n";
    _serial->flush();
}

void Webserver::dispatch(Client &client)
{
    Buffer buffer;

    if (client)
    {
        while (client.connected())
        {
            if (client.available())
            {
                char c = client.read();
                buffer.push(c);

                if (buffer.end())
                {
                    *_serial << buffer.get();
                    *_serial << "\r\n";
                    _serial->flush();

                    if (strncmp("GET ", buffer.get(), 4) == 0)
                        httpGet(client, buffer);
                    else if (strncmp("PUT ", buffer.get(), 4) == 0)
                        client << "HTTP/1.1 200 OK\r\n\r\n";
                    else if (strncmp("DELETE ", buffer.get(), 7) == 0)
                        httpDelete(buffer);
                    else if (strncmp("POST ", buffer.get(), 5) == 0)
                        *_serial << "POST\r\n";

                    buffer.reset();
                    break;
                }
            }
        }

        for (volatile uint16_t i = 0; i < 0x4fff; i++); // delay
        client.stop();
    }   
}

bool Buffer::end() const
{
    return _buf[_pos - 4] == 0x0d && _buf[_pos - 3] == 0x0a && _buf[_pos - 2] == 0x0d &&
        _buf[_pos - 1] == 0x0a ? true : false;
}

void Buffer::reset()
{
    _pos = 0;

    for (uint16_t i = 0; i < 512; i++)
        _buf[i] = 0;
}



