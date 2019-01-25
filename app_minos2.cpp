#include "w5100tcp.h"
#include "w5100udp.h"
#include "dhcp.h"
#include "fatty_util.h"
#include <stdio.h>
#include "md5sum.h"
#include "xmodem.h"
#include "ymodem.h"
#include "board.h"

#ifndef ENABLE_YRECEIVER
#define ENABLE_YRECEIVER
#endif

#ifndef ENABLE_YSENDER
#define ENABLE_YSENDER
#endif

#ifndef MD5SUM
#define MD5SUM
#endif

ostream *gout;

class Buffer
{
public:
    char buf[500];
    volatile uint16_t i = 0;
    void keystroke(char c);
    bool comp(const char *s);
    void token2(char *buf);

    bool enter() const
    {
        if (i <= 0)
            return false;

        return buf[i - 1] == 0x0d ? true : false;
    }

    volatile void clear() { i = 0; for (uint16_t j = 0; j < 500; j++) buf[j] = 0;  }
};

void Buffer::token2(char *dst)
{
    uint16_t j = 0;
    while (buf[j++] != ' ');
    while (my_isspace(buf[j++]));
    uint16_t start = j - 1;

    while (buf[j] != ' ' && buf[j] != 0x0d && buf[j] != 0x0a && buf[j] != 0)
        j++;

    uint16_t end = j;
    uint16_t len = end - start;
    char *p_start = buf + start;
    my_strncpy(dst, p_start, len);
}

void Buffer::keystroke(char c)
{
    if (c == 0x08)  // backspace
    {
        buf[--i] = 0;
        return;
    }

    buf[i++] = c;
    buf[i] = 0;
}

bool Buffer::comp(const char *s)
{
    size_t len = my_strlen(s);
#if 0
    for (size_t i = 0; i < len - 2; i++)
        if (buf[i] != s[i])
            return false;
#endif
    if (my_strncmp(buf + 1, s, len) != 0)
        return false;
#if 0
    if (buf[len] != 0x0a && buf[len] != 0x0d && buf[len] != ' ')    // separators
        return false;
#endif

    return true;
}

class App
{
private:
    Buffer _buf;
    void _cat(Fatty &zd, ostream &os);
#ifdef ENABLE_XSENDER
    void _sx(Fatty &zd, istream *is, ostream *os);
#endif
#ifdef ENABLE_XRECEIVER
    void _rx(Fatty &zd, istream *is, ostream *os);
#endif
#ifdef ENABLE_YRECEIVER
    void _rb(Fatty &zd, istream *is, ostream *os);
#endif
#ifdef ENABLE_YSENDER
    void _sb(Fatty &zd, istream *is, ostream *os);
#endif
#ifdef MD5SUM
    void _md5sum(Fatty &zd, ostream &os);
#endif
    void _more(Fatty &zd, istream &is, ostream &os);
    void _rm(Fatty &zd, ostream &os);
    void _od(Fatty &zd, ostream &os);
    void _mkdir(Fatty &zd);
public:
    int run();
};

void App::_cat(Fatty &zd, ostream &os)
{
    char fn[50] = {0};
    _buf.token2(fn);

    if (zd.exists(fn))
    {
        FyleIfstream myFile;
        myFile.open(fn);
        cat(myFile, os);
        myFile.close();
    }
    else
    {
        os << "No such file\r\n";
    }
}

void App::_mkdir(Fatty &zd)
{
    char dirname[50] = {0};
    _buf.token2(dirname);
    zd.mkdir(dirname);
}

void App::_rm(Fatty &zd, ostream &os)
{
    char fn[50] = {0};
    _buf.token2(fn);
    
    if (zd.exists(fn))
    {
        zd.remove(fn);
    }
    else
    {
        os << "No such file\r\n";
    }
}

void App::_od(Fatty &zd, ostream &os)
{
    char fn[50] = {0};
    _buf.token2(fn);
    
    if (zd.exists(fn))
    {
        FyleIfstream ifs;
        ifs.open(fn);
        od(ifs, os);
        ifs.close();
    }
    else
    {
        os << "No such file\r\n";
    }
}

#ifdef ENABLE_XSENDER
void App::_sx(Fatty &zd, istream *is, ostream *os)
{
    char fn[50] = {0};
    _buf.token2(fn);

    if (zd.exists(fn))
    {
        FyleIfstream ifs;
        ifs.open(fn);
        XSender x(is, os);
        x.send(ifs);
        ifs.close();
    }
    else
    {
        *os << "No such file\r\n";
    }
}
#endif

#ifdef ENABLE_XRECEIVER
void App::_rx(Fatty &zd, istream *is, ostream *os)
{
    char fn[50] = {0};
    _buf.token2(fn);
    XReceiver x(is, os);
    FyleOfstream ofs(&zd);
    ofs.open(fn);
    x.receive(ofs);
    ofs.close();
}
#endif

#ifdef ENABLE_YRECEIVER
void App::_rb(Fatty &zd, istream *is, ostream *os)
{
    YReceiver yr(is, os);
    yr.receive(zd);
}
#endif

#ifdef ENABLE_YSENDER
void App::_sb(Fatty &zd, istream *is, ostream *os)
{
    char fn[50] = {0};
    _buf.token2(fn);

    if (zd.exists(fn))
    {
        Fyle fyle = zd.open(fn);
        uint32_t filesize = fyle.size();
        fyle.close();
        FyleIfstream ifs;
        ifs.open(fn);
        YSender y(is, os);
        y.send(ifs, fn, filesize, 0, 0);
        ifs.close();
    }
    else
    {
        *os << "No such file\r\n";
    }
}
#endif

#ifdef MD5SUM
void App::_md5sum(Fatty &zd, ostream &os)
{
    char fn[50] = {0};
    _buf.token2(fn);

    if (zd.exists(fn))
    {
        FyleIfstream ifs;
        ifs.open(fn);
        Hasher hasher;
        hasher.reset();
        hasher.stream(ifs, os);
        hasher.hash().dump(os);
        os << "\r\n";
        ifs.close();
    }
    else
    {
        os << "File does not exists\r\n";
    }
}
#endif

void App::_more(Fatty &zd, istream &is, ostream &os)
{
    char fn[50] = {0};
    _buf.token2(fn);

    if (zd.exists(fn))
    {
        FyleIfstream ifs;
        ifs.open(fn);
        more(is, ifs, os);
        ifs.close();
    }
    else
    {
        os << "File does not exists\r\n";
    }
}

int App::run()
{
    Board b;
    Sd2Card sd(&b.pin4);
    Fatty zd(&sd);
    DefaultUart s;
    s.enableTransmit();
    s.enableRead();
    UartStream cout(&s);
    gout = &cout;
    UartIStream cin(&s);
    zei();
    
    cout << "Initialize Ethernet\r\n";
    cout.flush();
    W5100Class w5100;
    w5100.init();
    uint8_t mac[6] = {0,1,2,3,4,5};
    w5100.setMACAddress(mac);
    w5100.setIPAddress(0);

    cout << "Starting DHCP...\r\n";
    EthernetClass eth(&w5100);
    W5100UDP udp(&eth);
    DhcpClass dhcp(&udp);
    dhcp.beginWithDHCP(mac);
    w5100.setIPAddress(dhcp.localIp());
    w5100.setGatewayIp(dhcp.gateway());
    w5100.setSubnetMask(dhcp.subnetMask2());
    eth.addresses(cout);

    EthernetServer server = EthernetServer(&eth, 23);
    server.begin();

    bool go_flag = false;

    cout << "Initializing SD card...\r\n";
    bool ret = zd.begin();
    
    if (!ret)
    {
        cout << "init failed!\r\n";
        return 0;
    }

    cout << "SD Card initialized successful\r\n";

    while (true)
    {
        EthernetClient client = server.available();

        if (client)
        {
            while (client.connected())
            {
                if (client.available())
                {
                    char c = client.get();

                    if (c == '#')
                        go_flag = true;

                    if (go_flag == false)
                        continue;

                    _buf.keystroke(c);
                    client.put(c);

                    if (_buf.enter())
                    {
                        cout << _buf.buf;
                        //cout << "\r\n";
                        client.put(0x0a);
                        
                        if (_buf.comp("ls"))
                        {
                            cout << "ls verstuurd\r\n";
                            Fyle myFyle = zd.open("/");
                            printDirectory(myFyle, 0, client);
                        }

                        if (_buf.comp("cat"))
                            _cat(zd, client);

                        if (_buf.comp("more"))
                            _more(zd, client, client);

                        if (_buf.comp("od"))
                            _od(zd, client);

#ifdef ENABLE_YRECEIVER
                        if (_buf.comp("rb"))
                            _rb(zd, &client, &client);
#endif
#ifdef ENABLE_YSENDER
                        if (_buf.comp("sb"))
                            _sb(zd, &client, &client);
#endif
#ifdef ENABLE_XRECEIVER
                        if (_buf.comp("rx"))
                            _rx(zd, &client, &client);
#endif
#ifdef ENABLE_XSENDER
                        if (_buf.comp("sx"))
                            _sx(zd, &client, &client);
#endif
#ifdef MD5SUM
                        if (_buf.comp("md5sum"))
                            _md5sum(zd, client);
#endif
                        if (_buf.comp("mkdir"))
                            _mkdir(zd);

                        if (_buf.comp("rm"))
                            _rm(zd, client);

                        _buf.clear();
                        client << "$ ";
                    }
                }
            }
        }
    }
        
    return 0;
}

int main()
{
    App app;
    return app.run();
}




