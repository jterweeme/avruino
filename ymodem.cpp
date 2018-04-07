#include "ymodem.h"
#include "fatty.h"
#include <stdio.h>
#include <string.h>

extern ostream *g_dout;

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

int YSender::send(istream &is, const char *fn, uint32_t filesize, uint16_t time, uint16_t mode)
{
    firstsec = 1;
    wctxpn(fn, filesize, time, mode);   // write header to buffer
    putsec(0, 128);                     // send the buffer
    wctx(is);                           // receive the body
    wctxpn("", 0, 0, 0);                // write empty header to buffer
    putsec(0, 128);                     // send the buffer
    return 0;
}
#endif

#ifdef ENABLE_YRECEIVER
int16_t YReceiver::_read(uint32_t timeout)
{
    for (uint32_t i = 0; i <= timeout; i++)
        if (*p_ucsr0a & 1<<rxc0)
            return *p_udr0;

    return -1;  // timeout
}

int YReceiver::wcrx(ostream &os)
{
    int sectnum = 0, sectcurr;
    char sendchar = 'C';
    uint32_t bytes_received = 0;

    while (true)
    {
        _os->put(sendchar);
        _os->flush();
        sectcurr = _getsec();

        if (sectcurr == ((sectnum + 1) & 0xff))
        {
            sectnum++;
            uint8_t blklen = 128;

            if (_filesize - bytes_received < blklen)
                blklen = _filesize - bytes_received;

            os.write((const char *)_secbuf, blklen);
            bytes_received += blklen;
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
            return 0;
        }
        else
        {
            return -1;
        }
    }
}

void YReceiver::receive(Fatty &zd)
{
    FyleOfstream ofs;
    *_os << "waiting to receive.";
    _os->flush();
    _wcrxpn();
    _procheader();
    ofs.open(_fn);
    wcrx(ofs);
    ofs.close();
    _wcrxpn();
}

int YReceiver::_wcrxpn()
{
    //_read(1);
et_tu:
    _firstsec = 1;
    _os->put('C');
    _os->flush();
    
    for (int c; (c = _getsec()) != 0;)
    {
        if (c == -10)
        {
            _os->put(ACK);
            _os->flush();
            _read(0xffffffff);
            goto et_tu;
        }
        return -1;
    }
    _os->put(ACK);
    _os->flush();
    return 0;
}

int YReceiver::_getsec()
{
    for (uint8_t errors = 0; errors < 10; errors++)
    {
        uint8_t firstch = _read(0xffffffff);
#if 0
        g_dout->put(nibble(firstch >> 4 & 0xf));
        g_dout->put(nibble(firstch & 0xf));
        *g_dout << "\r\n";
#endif
        if (firstch == SOH)
        {
            uint8_t sectcurr = _read(0xffffffff);
            uint8_t sectcurn = _read(0xffffffff);

            if (sectcurr + sectcurn == 0xff)
            {
                for (uint8_t i = 0; i < 128; i++)
                {
                    uint8_t v = _read(0xffffffff);
                    _secbuf[i] = v;
                }

                _read(0xffffffff);
                _read(0xffffffff);
                return sectcurr;
            }
        }
        else if (firstch == EOT)
        {
            return -10;
        }

        for (uint16_t cnt = 0; cnt < 1000 && _read(0xfffff) != -1; cnt++);

        if (_firstsec)
        {
            _os->put('C');
            _os->flush();
        }
        else
        {
            _os->put(NAK);
            _os->flush();
        }

    }
    return -1;

}

void YReceiver::_procheader()
{
    strcpy(_fn, (char *)_secbuf);
    char *nameend = (char *)_secbuf + 1 + strlen((char *)_secbuf);
    
    if (*nameend)
        sscanf(nameend, "%ld%lo%o", &_filesize, &_modtime, &_mode);
}
#endif


