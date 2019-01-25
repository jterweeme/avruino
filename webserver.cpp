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

template <size_t N> class Fifo
{
    char _buf[5];
    uint8_t _count = 0;
public:
    uint8_t count() const { return _count; }
    void shift() { for (uint8_t i = 0; i < N - 1; i++) _buf[i] = _buf[i + 1]; }
    void push(char c) { shift(); _buf[N - 1] = c; if (_count < 5) ++_count; }
    char pull() { char c = _buf[0]; shift(); _count--; return c; }
    char front() const { return _buf[0]; }
    void clear() { _count = 0; my_memset(_buf, 0, 5); }
    int comp(const char *s, size_t n) { return my_strncmp(_buf + N - n, s, n); }
};

Webserver::Webserver(Fatty *fs, ostream *serial) : _fs(fs), _serial(serial)
{
}

void Webserver::_printDirectory(Fyle dir, uint8_t numTabs, ostream &os) const
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
            _printDirectory(entry, numTabs + 1, os);
#endif
        }
        else
        {
            os << "<td>";
            uint32_t size = entry.size();
            hex32(size, os);
            os << "</td>";
        }
        entry.close();
        os << "<td>D</td>\r\n</tr>\r\n";
    }

    os << "</table>\r\n";
}

void Webserver::_listing(ostream &client) const
{
    client << "<!DOCTYPE html>\r\n";
    client << "<html>\r\n<head>\r\n<title>Listing</title>\r\n";
    client << "</head>\r\n<body>\r\n<h1>Listing</h1>\r\n";
    Fyle root = _fs->open("/");
    _printDirectory(root, 0, client);
    root.close();
    client << "</body>\r\n</html>\r\n";
}

void Webserver::_serveFile(ostream &client, const char *fn)
{
    FyleIfstream ifs(_fs);
    ifs.open(fn);

    for (int16_t c2; (c2 = ifs.get()) != -1;)
        client.put(c2);

    ifs.close();
}

void Webserver::_servePHP(ostream &client, const char *fn)
{
    Fifo<5> fifo;
    FyleIfstream ifs(_fs);
    ifs.open(fn);
    // todo: open included php
    bool flag = false;

    for (int16_t c2; (c2 = ifs.get()) != -1;)
    {
        fifo.push(c2);

        if (fifo.comp("<?php", 5) == 0)
        {
            fifo.clear();
            while ((c2 = ifs.get()) != '\"');
            char buf[20] = {0};
            char *bufp = buf;
            
            while ((c2 = ifs.get()) != '\"')
                *bufp++ = (char)c2;

            uint32_t pos = ifs.tellg();
            ifs.close();
            ifs.open(buf);

            for (int16_t c3; (c3 = ifs.get()) != -1;)
                client.put(c3);

            ifs.close();
            ifs.open(fn);
            ifs.seekg(pos);
            flag = true;
        }

        if (flag)
        {
            if (fifo.comp("?>", 2) == 0)
            {
                fifo.clear();
                flag = false;
            }

            continue;
        }

        if (fifo.count() == 5)
        {
            client.put(fifo.front());
        }
    }

    fifo.pull();

    while (fifo.count())
        client.put(fifo.pull());    // drain the fifo

    ifs.close();   
}

void Webserver::_contentType(ostream &client, const char *ext)
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

void Webserver::_httpGet(ostream &client, Buffer &buffer)
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
        my_strncpy(fn, "index.htm", 100);

    char *dot = my_strchr(fn, '.');
    my_strncpy(ext, dot + 1, 3);
    client << "HTTP/1.1 200 OK\r\n";
    _contentType(client, ext);
    client << "Connection: close\r\n\r\n";  // let op de dubbel nl

    if (my_strncmp(fn, "listing", 7) == 0)
        _listing(client);
    else if (my_strncmp(ext, "php", 3) == 0)
        _servePHP(client, fn);
    else
        _serveFile(client, fn);
}

void Webserver::_httpMkcol(Buffer &buffer)
{
    char fn[50];

    for (uint16_t i = 7; i < 512; i++)
    {
        if (buffer.get()[i] != ' ')
            fn[i - 8] = buffer.get()[i];
        else
            break;
    }

    _fs->mkdir(fn + 1);
    *_serial << buffer.get();
    *_serial << "\r\n";
    _serial->flush();
}

void Webserver::_httpDelete(ostream &client, Buffer &buffer)
{
    char fn[50];

    for (uint16_t i = 8; i < 512; i++)
    {
        if (buffer.get()[i] != ' ')
            fn[i - 8] = buffer.get()[i];
        else
            break;
    }

    *_serial << fn;
    *_serial << "\r\n";
    _serial->flush();
    _fs->remove(fn);
    client << "HTTP/1.1 200 OK\r\n\r\n";
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

                    if (my_strncmp("GET ", buffer.get(), 4) == 0)
                        _httpGet(client, buffer);
                    else if (my_strncmp("PUT ", buffer.get(), 4) == 0)
                        client << "HTTP/1.1 200 OK\r\n\r\n";
                    else if (my_strncmp("DELETE ", buffer.get(), 7) == 0)
                        _httpDelete(client, buffer);
                    else if (my_strncmp("MKCOL ", buffer.get(), 6) == 0)
                        _httpMkcol(buffer);
                    else if (my_strncmp("POST ", buffer.get(), 5) == 0)
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



