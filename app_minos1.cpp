#include "fatty.h"
#include "misc.h"
#include "stream.h"

static ZD *g_zd;

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
    bool enter() const { return buf[i - 1] == 0x0d ? true : false; }
    void clear() { i = 0; buf[i] = 0; }
};

class XModem
{
private:
    istream *_is;
    ostream *_os;
    uint8_t packetNo = 1, checksumBuf = 0;
    long pos = 0;
    uint16_t packetLen = 128;
    int crcBuf = 0;
    uint8_t oldChecksum;
    char sync();
    void output(uint8_t inChar);
    char waitACK();
public:
    XModem(istream *is, ostream *os) : _is(is), _os(os) { }
    void send(Fyle file);
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
        oldChecksum = 1;
    }
    else
    {
        oldChecksum = 0;
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

void XModem::send(Fyle file)
{
    uint8_t tryNo;
    char c;
    file.seek(0);
    char *fn = file.name(); // moet eigenlijk strcpy

    if (sync() != 0)
        goto err;

    _os->put(SOH);
    _os->put((uint8_t)0);
    _os->put(0xff);

    size_t i;
    for (i = 0; i < strlen(fn); i++)
        output(fn[i]);

    for (; i < 128; i++)
        output((uint8_t)0);

    if (oldChecksum)
    {
        _os->put((char)255 - checksumBuf);
    }
    else
    {
        _os->put((char)(crcBuf >> 8));
        _os->put((char)(crcBuf & 0xff));
    }

    waitACK();
    
    if (sync() != 0)
        goto err;

    while (file.available())
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

            for (i = 0; i < packetLen; i++)
            {
                if (file.available())
                {
                    c = file.read();
                    output(c);
                }
                else
                {
                    output(0xff);
                }
            }

            if (oldChecksum)
            {
                _os->put((char)255 - checksumBuf);
            }
            else
            {
                _os->put((char)(crcBuf >> 8));
                _os->put((char)(crcBuf & 0xff));
            }

            c = waitACK();

            if (++tryNo > MAX_RETRY)
                goto err;
        }
        while (c != ACK);

        packetNo++;
    }

    tryNo = 0;

    do
    {
        _os->put(EOT);
        c = waitACK();
        tryNo++;

        if (tryNo == SYNC_TIMEOUT)
            goto err;
    }
    while (c != ACK);

    tryNo = 0;
err:
    _os->writeString("onzin\r\n");
}

void Buffer::keystroke(char c)
{
    buf[i++] = c;
    buf[i] = 0;
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

int main()
{
    ZD zd;
    g_zd = &zd;
    *p_tccr0b = 1<<cs02;
    *p_timsk0 |= 1<<toie0;
    zei();
    Buffer buf;
    DefaultUart s;
    *p_ubrr0 = 25;
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
            uint8_t len = strlen(buf.buf) - 1; // leave out the trailing 0x0d

            if (strncmp(buf.buf, "ls", len) == 0)
            {
                Fyle myFile = zd.open("/");
                printDirectory(myFile, 0, cout);
            }

            if (strncmp(buf.buf, "cat", len) == 0)
            {
                FyleIfstream myFile;
                myFile.open("cdc.cpp");
                cat(myFile, cout);
                myFile.close();
            }

            if (strncmp(buf.buf, "sb", len) == 0)
            {
                Fyle myFile = zd.open("svn.elf");
                XModem x(&cin, &cout);
                x.send(myFile);
                myFile.close();
            }

            buf.clear();
        }
    }
        

    return 0;
}

extern "C" void TIMER0_OVF __attribute__ ((signal, used, externally_visible));
void TIMER0_OVF
{
    g_zd->tick();
}


