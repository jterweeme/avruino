#include "xmodem.h"
#include "board.h"

static constexpr uint8_t
    SOH = 0x01, STX = 0x02, EOT = 0x04, ACK = 0x06, NAK = 0x15, CAN = 0x18,
    SYNC_TIMEOUT = 30, MAX_RETRY = 30;

extern ostream *g_dout;

#ifdef ENABLE_XRECEIVER
int16_t XReceiver::_read(uint32_t timeout = 0xffffffff)
{
    for (uint32_t i = 0; i <= timeout; i++)
        if (*p_ucsr9a & 1<<rxc9)
            return *p_udr9;

    return -1;  // timeout
}

void XReceiver::receive(ostream &os)
{
    int sectnum = 0, sectcurr;
    char sendchar = NAK;
    *_os << "waiting to receive.";

    for (volatile uint32_t i = 0; i < 0xfffff; i++);

    while (true)
    {
        _os->put(sendchar);
        _os->flush();
        sectcurr = _getsec();

        if (sectcurr == ((sectnum + 1) & 0xff))
        {
            sectnum++;
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
        uint8_t firstch = _read();

        if (firstch == SOH)
        {
            uint8_t sectcurr = _read();
            uint8_t sectcurn = _read();

            if (sectcurr + sectcurn == 0xff)
            {
                for (uint8_t i = 0; i < 128; i++)
                    _secbuf[i] = _read();

                _read(); // checksum
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

int XSender::send(istream &is)
{
    int sectnum = 0, attempts = 0, firstch;

    do
    {
        firstch = _is->get();

        if (firstch == 'X')
            return -1;
    }
    while (firstch != NAK && firstch != 'C' && firstch != 'G');

    while (_filbuf(is) > 0)
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
#endif



