#include "uip_server.h"
#include "stream.h"
#include "dhcp.h"
#include "fatty.h"

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

static UIPEthernetClass eth;

static ostream *gout;

static constexpr uint8_t
    SOH = 0x01, STX = 0x02, EOT = 0x04, ACK = 0x06, NAK = 0x15, CAN = 0x18,
    SYNC_TIMEOUT = 30, MAX_RETRY = 30;

class XTSender
{
private:
    uint8_t _mode = 0;
    UIPClient * const _client;
    char _txbuf[128];
    int putsec(int sectnum, size_t cseclen);
    size_t _filbuf(istream &is) { is.read(_txbuf, 128); return is.gcount(); }
    void _calcCRC16(uint16_t &crc, uint8_t c) const;
public:
    XTSender(UIPClient *client) : _client(client) { }
    int send(istream &is);
};

void XTSender::_calcCRC16(uint16_t &crc, uint8_t c) const
{
    crc = crc ^ (uint16_t)c << 8;

    for (uint8_t i = 0; i < 8; i++)
        crc = crc & 0x8000 ? crc << 1 ^ 0x1021 : crc << 1;
}

int XTSender::putsec(int sectnum, size_t cseclen)
{
    int wcj, firstch = 0;
    char *cp;

    for (uint8_t attempts = 0; attempts <= MAX_RETRY; attempts++)
    {
        _client->write(SOH);
        _client->write(sectnum);
        _client->write(-sectnum - 1);
        int checksum = 0;
        uint16_t crc16 = 0;

        for (wcj = cseclen, cp = _txbuf; --wcj >= 0;)
        {
            _client->write(*cp);
            _calcCRC16(crc16, *cp);
            checksum += *cp++;
        }

        if (_mode == 0)
        {
            _client->write(checksum & 0xff);
        }
        else
        {
            _client->write(crc16 >> 8 & 0xff);
            _client->write(crc16 & 0xff);
        }

        firstch = _client->read();
gotnak:
        switch (firstch)
        {
        case 'C':
            _mode = 1;
            continue;
        case NAK:
            _mode = 0;
            continue;
        case ACK:
            return 0;
        default:
            break;
        }

        while (true)
        {
            firstch = _client->read();

            if (firstch == NAK || firstch == 'C')
                goto gotnak;
        }
    }
    return -1;
}

int XTSender::send(istream &is)
{
    *gout << "XTSender::send()\r\n";
    gout->flush();
    int sectnum = 0, attempts = 0, firstch;
    
    do
    {
        firstch = _client->read();

        if (firstch == 'C')
        {
            _mode = 1;
            *gout << "Got C\r\n";
            gout->flush();
            break;
        }

        if (firstch == NAK)
        {
            _mode = 0;
            *gout << "Got NAK, mode checksum\r\n";
            gout->flush();
            break;
        }
        //gout->put(firstch);
        //*gout << "\r\n";
        
        if (firstch == 'X')
            return -1;
    }
    while (firstch != 'G');

    while (_filbuf(is) > 0)
        if (putsec(++sectnum, 128) == -1)
            return -1;

    do
    {
        _client->write(EOT);
        ++attempts;
        firstch = _client->read();
    }
    while (firstch != ACK && attempts < MAX_RETRY);

    return attempts == MAX_RETRY ? -1 : 0;
}

static inline char convertCase(char c)
{
    if (my_isupper(c))
        return c + 32;

    if (my_islower(c))
        return c - 32;

    return c;
}

static void cat(istream &is, UIPClient &os)
{
    for (int c; (c = is.get()) != -1;)
    {
        if (c == 0x0a)
            os.write(0x0d);

        os.write(c);
    }
}

int main()
{
    UIPServer server = UIPServer(&eth, 23);
    *p_tccr0b = cs02; // | CS00;
    *p_timsk0 |= 1<<toie0;
    zei();
    DefaultUart s;
    *p_ucsr9a |= 1<<u2x9;
    *p_ubrr9 = 16;
    UartStream cout(&s);
    gout = &cout;

    cout << "Initialize SD Card...\r\n";
    Board b;
    Sd2Card sd(&b.pin9);
    Fatty zd(&sd);
    bool ret = zd.begin();

    if (!ret)
    {
        cout << "init failed!\r\n";
        return 0;
    }

    cout << "Initialize Ethernet\r\n";
    cout.flush();
    uint8_t mac[6] = {0,1,2,3,4,5};
    eth.init(mac);

    cout << "Starting DHCP\r\n";
    cout.flush();
    UIPUDP udp(&eth);
    DhcpClass dhcp(&udp);
    dhcp.beginWithDHCP(mac);
    eth.configure(dhcp.localIp(), dhcp.dnsServer(), dhcp.gateway(), dhcp.subnetMask2());
    eth.addresses(cout);
    cout << "\r\n";

    server.begin();

    while (true)
    {
        UIPClient client = server.available();

        if (client)
        {
            cout << "Client connected\r\n";

            if (client.connected() && client.available())
            {
                FyleIfstream myFile;
                myFile.open("dopefish.ans");
                cat(myFile, client);
                myFile.close();
#if 0
                client.write("\e[1;32m");

                for (uint8_t i = 0; i < 80; i++)
                    client.write('A');
#endif
            }

            while (client.connected())
            {
                if (client.available())
                {

                    char c = client.read();

                    if (c == 'a')
                    {
                        XTSender xs(&client);
                        FyleIfstream myFile;
                        myFile.open("nc50.7z");
                        xs.send(myFile);
                        myFile.close();
                    }
                    cout.put(convertCase(c));
                    server.write(convertCase(c));
                }
            }

            for (volatile uint16_t i = 0; i < 0x4fff; i++); // delay
            client.stop();
            cout << "Client disconnected\r\n";
        }
    }
    return 0;
}

extern "C" void TIMER0_OVF __attribute__ ((signal, used, externally_visible));
void TIMER0_OVF
{
    eth.tick2();
}



