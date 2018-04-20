#include "fatty.h"
#include "misc.h"
#include "stream.h"
#include <stdio.h>
#include "md5sum.h"
#include "xmodem.h"
#include "ymodem.h"

#ifndef ENABLE_YRECEIVER
#define ENABLE_YRECEIVER
#endif

#ifndef ENABLE_YSENDER
#define ENABLE_YSENDER
#endif

#ifndef MD5SUM
#define MD5SUM
#endif

UartBase *g_uart1;
UartStream *g_dout;

#if defined (__AVR_ATmega2560__)
class Uart1 : public UartBase
{
public:
    Uart1() : UartBase(p_ubrr1, p_udr1, p_ucsr1a, p_ucsr1b) { }
};
#endif

class Buffer
{
public:
    char buf[50];
    uint8_t i = 0;
    void keystroke(char c);
    bool comp(const char *s);
    void token2(char *buf);
    bool enter() const { return buf[i - 1] == 0x0d ? true : false; }
    volatile void clear() { i = 0; for (uint8_t j = 0; j < 50; j++) buf[j] = 0;  }
};

void Buffer::token2(char *dst)
{
    uint8_t j = 0;
    while (buf[j++] != ' ');
    while (my_isspace(buf[j++]));
    uint8_t start = j - 1;

    while (buf[j] != ' ' && buf[j] != 0x0d && buf[j] != 0x0a && buf[j] != 0)
        j++;

    uint8_t end = j;
    uint8_t len = end - start;
    char *p_start = buf + start;
    my_strncpy(dst, p_start, len);
}

void Buffer::keystroke(char c)
{
    if (c == 0x08)
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
    
    for (size_t i = 0; i < len; i++)
        if (buf[i] != s[i])
            return false;

    if (buf[len] != 0x0a && buf[len] != 0x0d && buf[len] != ' ')    // separators
        return false;

    return true;
}

static void printDirectory(Fyle dir, int numTabs, ostream &os)
{
    while (true)
    {
        Fyle entry = dir.openNextFile();

        if (!entry)
            break;
        
        for (uint8_t i = 0; i < numTabs; i++)
            os << "\t";

        if (entry.isDirectory())
            os.put('[');

        os << entry.name();

        if (entry.isDirectory())
            os << "]\r\n";

        if (entry.isDirectory())
        {
#if 0
            os.put('/');
            printDirectory(entry, numTabs + 1, os);
#endif
        }
        else
        {
            for (uint8_t i = my_strlen(entry.name()); i < 15; i++)
                os.put(' ');

            //os << "\t\t";
            uint32_t size = entry.size();
            hex32(size, os);
            os << "\r\n";
        }
        entry.close();
    }
}

static void cat(istream &is, ostream &os)
{
    for (int c; (c = is.get()) != -1;)
    {
        if (c == 0x0a)
            os.put(0x0d);

        os.put(c);
    }
}

static void od(istream &is, ostream &os)
{
    bool available = true;
    uint8_t buf[16];
    uint8_t cnt = 0;
    
    while (available)
    {
        for (uint8_t i = 0; i < 16; i++)
        {
            int c = is.get();

            if (c == -1)
            {
                available = false;
                cnt = i;
                break;
            }

            buf[i] = c;
            cnt = 16;
        }
    
        for (uint8_t i = 0; i < cnt; i++)
        {
            os.put(nibble(buf[i] >> 4 & 0xf));
            os.put(nibble(buf[i] & 0xf));
            os.put(' ');
        }

        os << " >";

        for (uint8_t i = 0; i < cnt; i++)
        {
            if (my_isprint(buf[i]))
                os.put(buf[i]);
            else
                os.put('.');
        }

        os << "<\r\n";
    }
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
    FyleOfstream ofs;
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

int App::run()
{
    Board b;
    Sd2Card sd(&b.pin4);
    Fatty zd(&sd);
    DefaultUart s;
    *p_ucsr9a |= 1<<u2x9;
    *p_ubrr9 = 16;
    s.enableTransmit();
    s.enableRead();
    UartStream cout(&s);
    UartIStream cin(&s);
    zei();
#if defined (__AVR_ATmega2560__)
    UartBase uart1(p_ubrr1, p_udr1, p_ucsr1a, p_ucsr1b);
    uart1.enableTransmit();
    g_uart1 = &uart1;
    *p_ucsr1a |= 1<<u2x1;
    *p_ubrr1 = 16;
    UartStream dout(&uart1);
    g_dout = &dout;
#endif
    bool ret = zd.begin();
    
    if (!ret)
    {
        cout << "init failed!\r\n";
        return 0;
    }

    cout << "$ ";

    while (true)
    {
        char c = cin.get();
        _buf.keystroke(c);
        cout.put(c);

        if (_buf.enter())
        {
            *g_dout << _buf.buf;
            *g_dout << "\r\n";
            cout.put(0x0a);

            if (_buf.comp("ls"))
            {
                Fyle myFile = zd.open("/");
                printDirectory(myFile, 0, cout);
            }

            if (_buf.comp("cat"))
                _cat(zd, cout);

            if (_buf.comp("od"))
                _od(zd, cout);
#ifdef ENABLE_YRECEIVER
            if (_buf.comp("rb"))
                _rb(zd, &cin, &cout);
#endif
#ifdef ENABLE_YSENDER
            if (_buf.comp("sb"))
                _sb(zd, &cin, &cout);
#endif
#ifdef ENABLE_XRECEIVER
            if (_buf.comp("rx"))
                _rx(zd, &cin, &cout);
#endif
#ifdef ENABLE_XSENDER
            if (_buf.comp("sx"))
                _sx(zd, &cin, &cout);
#endif
#ifdef MD5SUM
            if (_buf.comp("md5sum"))
                _md5sum(zd, cout);
#endif
            if (_buf.comp("mkdir"))
                _mkdir(zd);

            if (_buf.comp("rm"))
                _rm(zd, cout);

            _buf.clear();
            cout << "$ ";
        }
    }
        
    return 0;
}

int main()
{
    App app;
    return app.run();
}




