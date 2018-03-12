#include "fatty.h"
#include "misc.h"
#include "stream.h"

static constexpr uint8_t
    SOH = 0x01, STX = 0x02, EOT = 0x04, ENQ = 0x05, ACK = 0x06, LF = 0x0a,
    CR = 0x0d, DLE = 0x10, XON = 0x11, XOFF = 0x13, NAK = 0x15, CAN = 0x18,
    SYNC_TIMEOUT = 30, MAX_RETRY = 30;

class Buffer
{
public:
    char buf[50];
    uint8_t i = 0;
    void keystroke(char c);
    bool comp(const char *s);
    void token2(char *buf);
    bool enter() const { return buf[i - 1] == 0x0d ? true : false; }
    void clear() { i = 0; buf[i] = 0; }
};

class XModem
{
private:
    istream * const _is;
    ostream * const _os;
    uint8_t packetNo = 1, checksumBuf = 0;
    long pos = 0;
    uint16_t packetLen = 128;
    int crcBuf = 0;
    uint8_t _oldChecksum;   // is the old checksum or the new crc version being used?
    char sync();
    void output(uint8_t c);
    char waitACK();
    void _sendCRC();
public:
    XModem(istream *is, ostream *os) : _is(is), _os(os) { }
    void send(ifstream &file, const char *fn, bool block0);
    void receive(Fatty *zd);
};

void XModem::output(uint8_t c)
{
    checksumBuf += c;
    crcBuf = crcBuf ^ (int)c << 8;
    
    for (uint8_t i = 0; i < 8; i++)
        crcBuf = crcBuf & 0x8000 ? crcBuf << 1 ^ 0x1021 : crcBuf << 1;
    
    _os->put(c);
}

char XModem::sync()
{
    char c;

    do
    {
        char tryNo = 0;
        c = _is->get();
        tryNo++;

        if (tryNo == SYNC_TIMEOUT)
            return -1;
    }
    while (c != 'C' && c != NAK);

    packetLen = 128;

    if (c == NAK)
    {
        _oldChecksum = 1;
    }
    else
    {
        _oldChecksum = 0;
        char tryNo = 0;
        tryNo = _is->get();

        if (tryNo == 'K')
            packetLen = 1024;
    }

    return 0;
}

char XModem::waitACK()
{
    uint8_t i = 0;
    char c;

    do
    {
        c = _is->get();

        if (++i > 200 || c == CAN)
            return -1;
    }
    while (c != NAK && c != ACK && c != 'C');

    return c;
}

void XModem::_sendCRC()
{
    if (_oldChecksum)
    {
        _os->put((char)255 - checksumBuf);
    }
    else
    {
        _os->put((char)(crcBuf >> 8));
        _os->put((char)(crcBuf & 0xff));
    }
}

void XModem::send(ifstream &file, const char *fn, bool block0)
{
    uint8_t tryNo;
    int16_t c;

    if (block0)
    {
        if (sync() != 0)    // wait for receiver ping
            goto err;

        _os->put(SOH);
        _os->put((uint8_t)0);
        _os->put(0xff);

        size_t i;
        for (i = 0; i < strlen(fn); i++)
            output(fn[i]);

        for (; i < 128; i++)
            output((uint8_t)0);

        _sendCRC();
        waitACK();
    }
    else
    {
        _oldChecksum = 1;   // xmodem
    }

    if (sync() != 0)
        goto err;

    while (file.peek() != -1)
    {
        tryNo = 0;

        do
        {
            checksumBuf = 0;
            crcBuf = 0;

            if (packetLen == 128)
                _os->put(SOH);
            else
                _os->put(STX);

            _os->put(packetNo);
            _os->put(~packetNo);

            for (uint16_t i = 0; i < packetLen; i++)
            {
                if (file.peek() != -1)
                {
                    c = file.get();
                    output(c);
                }
                else
                {
                    output(0xff);
                }
            }
            _sendCRC();
            c = waitACK();

            if (++tryNo > MAX_RETRY)
                goto err;
        }
        while (c != ACK);

        packetNo++;
    }

    tryNo = 0;
#if 1
    do
    {
        _os->put(EOT);
        c = waitACK();
        tryNo++;

        if (tryNo == SYNC_TIMEOUT)
            goto err;
    }
    while (c != ACK);

    if (block0)
    {
        if (sync() != 0)    // wait for receiver ping
            goto err;

        _os->put(SOH);
        _os->put((uint8_t)0);
        _os->put(0xff);

        for (uint8_t i = 0; i < 128; i++)
            output((uint8_t)0);

        _sendCRC();
        waitACK();
    }

#else
    _os->put(EOT);
#endif
    tryNo = 0;
err:
    _os->writeString("onzin\r\n");
}

void XModem::receive(Fatty *zd)
{
    while (true)
    {
        _os->put('C');

        for (volatile uint32_t i = 0; i < 0xfffff; i++)
            ;

        if (*p_ucsr0a & 1<<rxc0)
            break;
    }

    uint8_t c;
    c = _is->get(); // soh

    if (c != SOH)
        return;

    c = _is->get(); // frame no
    
    if (c != 0)
        return;

    c = _is->get(); // ~frame no

    if (c != 0xff)
        return;

    char fn[50] = {0};

    uint8_t i = 0;
    for (; i < 128; i++)
    {
        char c = _is->get();

        if (c)
            fn[i] = c;
        else
            break;
    }

    for (; i < 128; i++)
    {
        _is->get();
    }
}

void Buffer::token2(char *dst)
{
    uint8_t i = 0;

    while (buf[i++] != ' ')
        ;

    while (myIsspace(buf[i++]))
        ;

    uint8_t start = i - 1;

    while (buf[i] != ' ' && buf[i] != 0x0d && buf[i] != 0x0a && buf[i] != 0)
        i++;

    uint8_t end = i;
    uint8_t len = end - start;
    char *p_start = buf + start;
    strncpy(dst, p_start, len);
}

void Buffer::keystroke(char c)
{
    buf[i++] = c;
    buf[i] = 0;
}

bool Buffer::comp(const char *s)
{
    size_t len = strlen(s);
    
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
            if (myIsprint(buf[i]))
                os.put(buf[i]);
            else
                os.put('.');
        }

        os.writeString("<\r\n");
    }
}

int main()
{
    Board b;
    Sd2Card sd(&b.pin9);
    Fatty zd(&sd);
    *p_tccr0b = 1<<cs02;
    *p_timsk0 |= 1<<toie0;
    zei();
    Buffer buf;
    DefaultUart s;
    *p_ucsr0a |= 1<<u2x0;
    *p_ubrr0 = 16;
    s.enableTransmit();
    s.enableRead();
    UartStream cout(&s);
    UartIStream cin(&s);
    bool ret = zd.begin();
    
    if (!ret)
    {
        cout.writeString("init failed!\r\n");
        return 0;
    }

    while (true)
    {
        char c = cin.get();
        buf.keystroke(c);
        cout.put(c);

        if (buf.enter())
        {
            cout.put(0x0a);

            if (buf.comp("ls"))
            {
                Fyle myFile = zd.open("/");
                printDirectory(myFile, 0, cout);
            }

            if (buf.comp("cat"))
            {
                FyleIfstream myFile;
                char fn[50];
                buf.token2(fn);
                myFile.open(fn);
                cat(myFile, cout);
                myFile.close();
            }

            if (buf.comp("od"))
            {
                FyleIfstream ifs;
                char fn[50];
                buf.token2(fn);
                ifs.open(fn);
                od(ifs, cout);
                ifs.close();
            }

            if (buf.comp("sb"))
            {
                FyleIfstream myFile;
                char fn[50];
                buf.token2(fn);
                myFile.open(fn);
                XModem x(&cin, &cout);
                x.send(myFile, fn, true);
                myFile.close();
            }

            if (buf.comp("sx"))
            {
                FyleIfstream ifs;
                char fn[50];
                buf.token2(fn);
                ifs.open(fn);
                XModem x(&cin, &cout);
                x.send(ifs, fn, false);
                ifs.close();
            }

            if (buf.comp("rb"))
            {
                XModem x(&cin, &cout);
                x.receive(&zd);
            }

            if (buf.comp("rm"))
            {
                char fn[50];
                buf.token2(fn);
                zd.remove(fn);
            }

            buf.clear();
        }
    }
        

    return 0;
}

extern "C" void TIMER0_OVF __attribute__ ((signal, used, externally_visible));
void TIMER0_OVF
{
    Fatty::instance->tick();
}


