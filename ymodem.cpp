#include "ymodem.h"
#include "fatty.h"
#include <stdio.h>

static constexpr uint8_t
    SOH = 0x01, STX = 0x02, EOT = 0x04, ACK = 0x06, NAK = 0x15, CAN = 0x18,
    SYNC_TIMEOUT = 30, MAX_RETRY = 30;

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
size_t YSender::filbuf(istream &is)
{
    is.read(_txbuf, 128);
    return is.gcount();
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
    //_sync();

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

int YSender::putsec(uint8_t sectnum, size_t cseclen)
{
    int checksum, wcj, firstch = 0;
    char *cp;

    for (uint8_t attempts = 0; attempts <= MAX_RETRY; attempts++)
    {
        _os->put(SOH);
        _os->put(sectnum);
        _os->put(-sectnum - 1);
        checksum = 0;
        CRC crc;
        crc.reset();

        for (wcj = cseclen, cp = _txbuf; --wcj >= 0;)
        {
            _os->put(*cp);
            crc.update(*cp);
            checksum += *cp++;
        }

        uint16_t oldcrc = crc.crc();
        _os->put(oldcrc >> 8);
        _os->put(oldcrc);
        _os->flush();
        firstch = _is->get();
gotnak:
        switch (firstch)
        {
        case 'C':
        case NAK:
            continue;
        case ACK:
            firstsec = 0;
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

int YSender::wctxpn(const char *fn, uint32_t filesize, uint16_t modtime, uint16_t mode)
{
    char *p, *q;
    _sync();
    q = (char *)0;

    for (p = (char *)fn, q = _txbuf; *p;)
        if ((*q++ = *p++) == '/')
            q = _txbuf;

    *q++ = 0;
    p = q;

    while (q < (_txbuf + 128))
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
    putsec(0, 128);
    wctx(ifs);
    ifs.close();
    wctxpn("", 0, 0, 0);
    putsec(0, 128);
    return 0;
}
#endif



