#include "fatty.h"
#include "misc.h"
#include "stream.h"
#include <stdio.h>

#define ENABLE_XRECEIVER
#define ENABLE_XSENDER
#define ENABLE_YSENDER

UartBase *g_uart1;
UartStream *g_dout;

static constexpr uint8_t
    SOH = 0x01, STX = 0x02, EOT = 0x04, ACK = 0x06, NAK = 0x15, CAN = 0x18,
    SYNC_TIMEOUT = 30, MAX_RETRY = 30;

class Uart1 : public UartBase
{
public:
    Uart1() : UartBase(p_ubrr1, p_udr1, p_ucsr1a, p_ucsr1b) { }
};

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

static constexpr uint32_t k[64] =
{
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

static constexpr uint32_t r[] =
{
    7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
    5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
};

class Hash
{
private:
    uint32_t _h0, _h1, _h2, _h3;
public:
    uint32_t h0() const { return _h0; }
    uint32_t h1() const { return _h1; }
    uint32_t h2() const { return _h2; }
    uint32_t h3() const { return _h3; }
    void dump(ostream &os) const;
    void add(Hash &h) { _h0 += h.h0(); _h1 += h.h1(); _h2 += h.h2(); _h3 += h.h3(); }
    Hash() { reset(); }
    Hash(uint32_t a, uint32_t b, uint32_t c, uint32_t d) : _h0(a), _h1(b), _h2(c), _h3(d) { }
    void reset() { _h0 = 0x67452301; _h1 = 0xefcdab89; _h2 = 0x98badcfe; _h3 = 0x10325476; }
};

static inline void dump32(uint32_t x, ostream &os)
{
    os.put(nibble(x >> 4 & 0xf));
    os.put(nibble(x & 0xf));
    os.put(nibble(x >> 12 & 0xf));
    os.put(nibble(x >> 8 & 0xf));
    os.put(nibble(x >> 20 & 0xf));
    os.put(nibble(x >> 16 & 0xf));
    os.put(nibble(x >> 28 & 0xf));
    os.put(nibble(x >> 24 & 0xf));
}

void Hash::dump(ostream &os) const
{
    dump32(_h0, os);
    dump32(_h1, os);
    dump32(_h2, os);
    dump32(_h3, os);
}

class Chunk
{
private:
    uint32_t _w[16];
    uint32_t leftRotate(uint32_t x, uint32_t c) { return x << c | x >> (32 - c); }
    uint32_t _to_int32(const uint8_t *bytes) const;
public:
    void clear() { for (uint8_t i = 0; i < 16; i++) _w[i] = 0; }
    uint32_t w(uint32_t g) const { return _w[g]; }
    void fillTail(uint32_t size) { _w[14] = size * 8; _w[15] = size >> 29; }
    Hash calc(Hash &hash);
    void read(const uint8_t *msg);
};

uint32_t Chunk::_to_int32(const uint8_t *bytes) const
{
    return (uint32_t)bytes[0]
        | ((uint32_t)bytes[1] << 8)
        | ((uint32_t)bytes[2] << 16)
        | ((uint32_t)bytes[3] << 24);
}

void Chunk::read(const uint8_t *msg)
{
    for (uint8_t i = 0; i < 16; i++)
        _w[i] = _to_int32(msg + i * 4);
}

Hash Chunk::calc(Hash &hash)
{
    uint32_t a, b, c, d, f, g, temp;
    a = hash.h0();
    b = hash.h1();
    c = hash.h2();
    d = hash.h3();

    for (uint8_t i = 0; i < 64; i++)
    {
        if (i < 16)
        {
            f = (b & c) | ((~b) & d);
            g = i;
        }
        else if (i < 32)
        {
            f = (d & b) | ((~d) & c);
            g = (5 * i + 1) % 16;
        }
        else if (i < 48)
        {
            f = b ^ c ^ d;
            g = (3 * i + 5) % 16;
        }
        else
        {
            f = c ^ (b | (~d));
            g = (7 * i) % 16;
        }

        temp = d;
        d = c;
        c = b;
        b = b + leftRotate((a + f + k[i] + w(g)), r[i]);
        a = temp;
    }

    Hash foo(a, b, c, d);
    return foo;
}

class Hasher
{
private:
    Hash _hash;
public:
    void reset() { _hash.reset(); }
    void stream(istream &is, ostream &os);
    Hash hash() const { return _hash; }
};

void Hasher::stream(istream &is, ostream &os)
{
    *g_dout << "Hasher::stream()\r\n";
    _hash.reset();
    bool kontinue = true;    

    for (uint32_t i = 0; kontinue; i++)
    {
        uint8_t data[64] = {0};
        is.read((char *)data, 64);
        Chunk chunk;
        uint8_t cnt = is.gcount();

        if (cnt < 56)
        {
            data[cnt] = 0x80;
            chunk.read(data);
            chunk.fillTail(i * 64 + cnt);
            kontinue = false;
        }
        else if (cnt < 64)
        {
            data[cnt] = 0x80;
            chunk.read(data);
            Hash bar = chunk.calc(_hash);
            _hash.add(bar);
            chunk.clear();
            chunk.fillTail(i * 64 + cnt);
            kontinue = false;
        }
        else
        {
            chunk.read(data);
        }

        Hash bar = chunk.calc(_hash);
        _hash.add(bar);
    }
}

#ifdef ENABLE_XRECEIVER
class XReceiver
{
private:
    istream * const _is;
    ostream * const _os;
    int _getsec();
    uint8_t _secbuf[128];
public:
    XReceiver(istream *is, ostream *os) : _is(is), _os(os) { }
    void receive(ostream &os);
};

void XReceiver::receive(ostream &os)
{
    int sectnum = 0, sectcurr;
    char sendchar = NAK;
    *_os << "waiting to receive.";

    for (volatile uint32_t i = 0; i < 0x1fffff; i++);
    
    while (true)
    {
        _os->put(sendchar);
        _os->flush();
        sectcurr = _getsec();

        if (sectcurr == ((sectnum + 1) & 0xff))
        {
            sectnum++;
            *g_dout << "Debug bericht\r\n";
            os.write((const char *)_secbuf, 128);
            os.flush();
            sendchar = ACK;
        }
        else if (sectcurr == (sectnum & 0xff))
        {
            sendchar = ACK;
        }
        else if (sectcurr == -10)
        {
            _os->put(ACK);
            _os->flush();
            return;
        }
        else
        {
            return;
        }
    }
}

int XReceiver::_getsec()
{
    for (uint8_t errors = 0; errors < 10; errors++)
    {
        uint8_t firstch = _is->get();

        if (firstch == SOH)
        {
            uint8_t sectcurr = _is->get();
            uint8_t n_sectcurr = _is->get();

            if (sectcurr + n_sectcurr == 0xff)
            {
                for (uint8_t i = 0; i < 128; i++)
                {
                    uint8_t v = _is->get();
                    _secbuf[i] = v;
                }

                _is->get();
                return sectcurr;
            }
        }
        else if (firstch == EOT)
        {
            return -10;
        }

        _os->put(NAK);
        _os->flush();
    }
    return -1;
}
#endif

#ifdef ENABLE_XSENDER
class XSender
{
private:
    char _txbuf[128];
    istream * const _is;
    ostream * const _os;
    int wctx(istream &is);
    int putsec(int sectnum, size_t cseclen);
    size_t filbuf(istream &is);
public:
    XSender(istream *is, ostream *os) : _is(is), _os(os) { }
    int send(istream &is);
};

int XSender::putsec(int sectnum, size_t cseclen)
{
    int checksum, wcj, firstch = 0;
    char *cp;

    for (uint8_t attempts = 0; attempts <= MAX_RETRY; attempts++)
    {
        _os->put(SOH);
        _os->put(sectnum);
        _os->put(-sectnum - 1);
        checksum = 0;

        for (wcj = cseclen, cp = _txbuf; --wcj >= 0;)
        {
            _os->put(*cp);
            checksum += *cp++;
        }

        _os->put(checksum);
        _os->flush();
        firstch = _is->get();
gotnak:
        switch (firstch)
        {
        case 'C':
        case NAK:
            continue;
        case ACK:
            return 0;
        default:
            break;
        }

        while (true)
        {
            firstch = _is->get();
            
            if (firstch == NAK || firstch == 'C')
                goto gotnak;
        }
    }
    return -1;
}

size_t XSender::filbuf(istream &is)
{
    is.read(_txbuf, 128);
    return is.gcount();
}

int XSender::wctx(istream &is)
{
    int sectnum = 0, attempts = 0, firstch;

    do
    {
        firstch = _is->get();

        if (firstch == 'X')
            return -1;
    }
    while (firstch != NAK && firstch != 'C' && firstch != 'G');

    while (filbuf(is) > 0)
        if (putsec(++sectnum, 128) == -1)
            return -1;

    do
    {
        _os->put(EOT);
        _os->flush();
        ++attempts;
        firstch = _is->get();
    }
    while (firstch != ACK && attempts < MAX_RETRY);

    return attempts == MAX_RETRY ? -1 : 0;
}

int XSender::send(istream &is)
{
    wctx(is);
    return 0;
}
#endif

class CRC
{
private:
    uint16_t _crc = 0;
public:
    void reset() { _crc = 0; }
    uint16_t crc() const { return _crc; }
    void update(uint8_t c);
};

void CRC::update(uint8_t c)
{
    _crc = _crc ^ (uint16_t)c << 8;

    for (uint8_t i = 0; i < 8; i++)
        _crc = _crc & 0x8000 ? _crc << 1 ^ 0x1021 : _crc << 1;
}

#ifdef ENABLE_YSENDER
class YSender
{
private:
    istream * const _is;
    ostream * const _os;
    uint8_t firstsec, _crcflag = 1;
    char txbuf[128];
    void _sync();
    int wctx(istream &is);
    int putsec(char *buf, uint8_t sectnum, size_t cseclen);
    int wctxpn(const char *fn, uint32_t filesize, uint16_t time, uint16_t mode);
    size_t filbuf(istream &is);
public:
    YSender(istream *is, ostream *os) : _is(is), _os(os) { }
    int send(char *fn);
};

size_t YSender::filbuf(istream &is)
{
    for (uint8_t i = 0; i < 128; i++)
    {
        if (is.peek() == -1)
            return i;

        txbuf[i] = is.get();
    }

    return 128;
}

void YSender::_sync()
{
    for (int c = 0; (c = _is->get()) != NAK;)
    {
        if (c == 'C')
        {
            _crcflag = 1;
            break;
        }
    }
}

int YSender::wctx(istream &is)
{
    int sectnum = 0, attempts = 0, firstch;
    firstsec = 1;
    _sync();

    while (filbuf(is) > 0)
        if (putsec(txbuf, ++sectnum, 128) == -1)
            return -1;

    do
    {
        _os->put(EOT);
        _os->flush();
        ++attempts;
        firstch = _is->get();
    }
    while (firstch != ACK && attempts < MAX_RETRY);

    return attempts == MAX_RETRY ? -1 : 0;
}

int YSender::putsec(char *buf, uint8_t sectnum, size_t cseclen)
{
    int checksum, wcj, firstch = 0;
    char *cp;

    for (uint8_t attempts = 0; attempts <= MAX_RETRY; attempts++)
    {
        _os->put(SOH);
        _os->put(sectnum);
        _os->put(~sectnum);
        checksum = 0;
        CRC crc;
        crc.reset();

        for (wcj = cseclen, cp = buf; --wcj >= 0;)
        {
            _os->put(*cp);
            crc.update(*cp);
            checksum += *cp++;
        }

        if (_crcflag)
        {
            uint16_t oldcrc = crc.crc();
            _os->put(oldcrc >> 8);
            _os->put(oldcrc);
        }
        else
        {
            _os->put(checksum);
        }

        _os->flush();
        firstch = _is->get();
gotnak:
        switch (firstch)
        {
        case 'C':
            if (firstsec)
                _crcflag = 1;
        case NAK:
            continue;
        case ACK:
            firstsec = 0;
            return 0;
        case -1:
            break;
        default:
            break;
        }

        while (true)
        {
            firstch = _is->get();
            
            if (firstch == NAK || firstch == 'C')
                goto gotnak;
        }
    }

    return -1;
}

int YSender::wctxpn(const char *fn, uint32_t filesize, uint16_t modtime, uint16_t mode)
{
    char *p, *q;
    _sync();
    q = (char *)0;

    for (p = (char *)fn, q = txbuf; *p;)
        if ((*q++ = *p++) == '/')
            q = txbuf;

    *q++ = 0;
    p = q;

    while (q < (txbuf + 128))
        *q++ = 0;

    sprintf(p, "%lu %lo %o 0 0 0", (uint32_t)filesize, (long)modtime, mode);
    return 0;
}

int YSender::send(char *fn)
{
    firstsec = 1;
    FyleIfstream ifs;
    ifs.open(fn);
    wctxpn(fn, 305280, 0, 0);
    wctx(ifs);
    ifs.close();
    wctxpn("", 0, 0, 0);
    return 0;
}
#endif

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
            os.writeString("\t");

        os.writeString(entry.name());

        if (entry.isDirectory())
        {
            os.writeString("/");
            printDirectory(entry, numTabs + 1, os);
        }
        else
        {
            os.writeString("\t\t");
            uint32_t size = entry.size();
            os.put(nibble(size >> 28 & 0xf));
            os.put(nibble(size >> 24 & 0xf));
            os.put(nibble(size >> 20 & 0xf));
            os.put(nibble(size >> 16 & 0xf));
            os.put(nibble(size >> 12 & 0xf));
            os.put(nibble(size >> 8 & 0xf));
            os.put(nibble(size >> 4 & 0xf));
            os.put(nibble(size & 0xf));
            os.writeString("\r\n");
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

        os.writeString(" >");

        for (uint8_t i = 0; i < cnt; i++)
        {
            if (my_isprint(buf[i]))
                os.put(buf[i]);
            else
                os.put('.');
        }

        os.writeString("<\r\n");
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
#ifdef ENABLE_YSENDER
    void _sb(Fatty &zd, istream *is, ostream *os);
#endif
    void _md5sum(Fatty &zd, ostream &os);
    void _rm(Fatty &zd, ostream &os);
    void _od(Fatty &zd, ostream &os);
public:
    int run();
};

void App::_cat(Fatty &zd, ostream &os)
{
    FyleIfstream myFile;
    char fn[50] = {0};
    _buf.token2(fn);
    *g_dout << fn;
    *g_dout << "\r\n";

    if (zd.exists(fn))
    {
        myFile.open(fn);
        cat(myFile, os);
        myFile.close();
    }
    else
    {
        os << "No such file\r\n";
    }
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
    FyleIfstream ifs;
    char fn[50] = {0};
    _buf.token2(fn);
    *g_dout << fn;
    *g_dout << "\r\n";
    
    if (zd.exists(fn))
    {
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
    FyleIfstream ifs;
    char fn[50] = {0};
    _buf.token2(fn);

    if (zd.exists(fn))
    {
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

#ifdef ENABLE_YSENDER
void App::_sb(Fatty &zd, istream *is, ostream *os)
{
    char fn[50] = {0};
    _buf.token2(fn);
    YSender y(is, os);
    y.send(fn);
}
#endif

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

int App::run()
{
    Board b;
    Sd2Card sd(&b.pin9);
    Fatty zd(&sd);
    *p_tccr0b = 1<<cs02;
    *p_timsk0 |= 1<<toie0;
    zei();
    DefaultUart s;
    *p_ucsr0a |= 1<<u2x0;
    *p_ubrr0 = 16;
    s.enableTransmit();
    s.enableRead();
    UartStream cout(&s);
    UartIStream cin(&s);
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
            if (_buf.comp("md5sum"))
                _md5sum(zd, cout);

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

extern "C" void TIMER0_OVF __attribute__ ((signal, used, externally_visible));
void TIMER0_OVF
{
    Fatty::instance->tick();
}


