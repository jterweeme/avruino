#include "xmodem.h"

static constexpr uint8_t
    SOH = 0x01, STX = 0x02, EOT = 0x04, ACK = 0x06, NAK = 0x15, CAN = 0x18,
    SYNC_TIMEOUT = 30, MAX_RETRY = 30;

#ifdef ENABLE_XRECEIVER
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



