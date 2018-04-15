/*
Mega eth CS: D53
Mega SD CS: D9

Webserver, gebruikt index.html op FAT geformatteerd SD kaart
*/

#include "uip_server.h"
#include "dhcp.h"
#include "fatty.h"

class Buffer
{
    char _buf[512] = {0};
    uint16_t _pos = 0;
public:
    void push(char c) { _buf[_pos++] = c; }
    char *get() { return _buf; }
    void reset();
    bool end() const;
};

bool Buffer::end() const
{
    return _buf[_pos - 4] == 0x0d && _buf[_pos - 3] == 0x0a && _buf[_pos - 2] == 0x0d &&
        _buf[_pos - 1] == 0x0a ? true : false;
}

void Buffer::reset()
{
    _pos = 0;

    for (uint16_t i = 0; i < 512; i++)
        _buf[i] = 0;
}

static UIPEthernetClass eth;
static Fatty *g_zd;

static void printDirectory(Fyle dir, uint8_t numTabs, UIPClient &os)
{
    os.write("<table>\r\n");

    while (true)
    {
        Fyle entry = dir.openNextFile();

        if (!entry)
            break;

        for (uint8_t i = 0; i < numTabs; i++)
            os.write("\t");

        os.write("<tr><td><a href=\"");
        os.write(entry.name());
        os.write("\">");
        os.write(entry.name());
        os.write("</a></td>");

        if (entry.isDirectory())
        {
            os.write("<td>DIR</td>");
#if 0
            os.write("/");
            printDirectory(entry, numTabs + 1, os);
#endif
        }
        else
        {
            os.write("<td>");
            uint32_t size = entry.size();
            os.write(nibble(size >> 28 & 0xf));
            os.write(nibble(size >> 24 & 0xf));
            os.write(nibble(size >> 20 & 0xf));
            os.write(nibble(size >> 16 & 0xf));
            os.write(nibble(size >> 12 & 0xf));
            os.write(nibble(size >> 8 & 0xf));
            os.write(nibble(size >> 4 & 0xf));
            os.write(nibble(size & 0xf));
            os.write("</td>");
        }
        entry.close();
        os.write("<td>D</td>\r\n</tr>\r\n");
    }

    os.write("</table>\r\n");
}

static void listing(UIPClient &client, Fatty &zd)
{
    client.write("<!DOCTYPE html>\r\n");
    client.write("<html>\r\n<head>\r\n<title>Listing</title>\r\n");
    client.write("</head>\r\n<body>\r\n<h1>Listing</h1>\r\n");
    Fyle root = zd.open("/");
    printDirectory(root, 0, client);
    root.close();
    client.write("</body>\r\n</html>\r\n");
}

static void serveFile(UIPClient &client, const char *fn)
{
    FyleIfstream ifs;
    ifs.open(fn);

    int c2;
    while ((c2 = ifs.get()) != -1)
        client.write(c2);

    ifs.close();
}

static void contentType(UIPClient &client, const char *ext)
{
    if (my_strncasecmp(ext, "htm", 3) == 0)
        client.write("Content-Type: text/html\r\n");
    else if (my_strncasecmp(ext, "svg", 3) == 0)
        client.write("Content-Type: image/svg+xml\r\n");
    else if (my_strncasecmp(ext, "css", 3) == 0)
        client.write("Content-Type: text/css\r\n");
    else if (my_strncasecmp(ext, "js", 2) == 0)
        client.write("Content-Type: text/js\r\n");
    else if (my_strncasecmp(ext, "jpg", 3) == 0)
        client.write("Content-Type: image/jpeg\r\n");
    else if (my_strncasecmp(ext, "zip", 3) == 0)
        client.write("Content-Type: application/zip\r\n");
    else if (my_strncasecmp(ext, "7z", 2) == 0)
        client.write("Content-Type: application/x-7z-compressed\r\n");
    else if (my_strncasecmp(ext, "cpp", 3) == 0)
        client.write("Content-Type: text/plain\r\n");
    else
        client.write("Content-Type: text/html\r\n");
}

static void httpGet(UIPClient &client, Fatty &zd, Buffer &buffer)
{
    char fn[100] = {0};
    char ext[10] = {0};
    
    for (uint16_t i = 5; i < 512; i++)
    {
        if (buffer.get()[i] != ' ')
            fn[i - 5] = buffer.get()[i];
        else
            break;
    }
    
    if (fn[0] == 0)
        strncpy(fn, "index.htm", 100);

    char *dot = my_strchr(fn, '.');
    my_strncpy(ext, dot + 1, 3);
    client.write("HTTP/1.1 200 OK\r\n");
    contentType(client, ext);
    client.write("Connection: close\r\n\r\n");  // let op de dubbel nl

    if (strncmp(fn, "listing", 7) == 0)
        listing(client, zd);
    else
        serveFile(client, fn);
}

static void addresses(UIPEthernetClass &eth, ostream &os)
{
    uint32_t ip = eth.localIP();
    uint32_t subnet = eth.subnetMask();
    uint32_t gw = eth.gatewayIP();
    uint32_t dns = eth.dnsServerIP();
    os << "IP:      ";
    hex32(ip, os);
    os << "\r\nSubnet:  ";
    hex32(subnet, os);
    os << "\r\nGateway: ";
    hex32(gw, os);
    os << "\r\nDNS:     ";
    hex32(dns, os);
    os << "\r\n";
}

int main()
{
    // 16,000,000/16,000 = 1000
    // 16,000 / 256 = 62

    UIPServer server = UIPServer(&eth, 80);
    Board b;
    Sd2Card sd(&b.pin9);
    Fatty zd(&sd);
    g_zd = &zd;
    TCCR0B = CS02; // | CS00;
    TIMSK0 |= 1<<TOIE0;
    zei();
    DefaultUart s;
    *p_ucsr9a |= 1<<u2x9;
    *p_ubrr9 = 16;
    UartStream cout(&s);

    cout << "Initialize Ethernet\r\n";
    cout.flush();
    uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05};
    eth.init(mac);

    cout << "Starting DHCP\r\n";
    cout.flush();
    DhcpClass dhcp(&eth);
    dhcp.beginWithDHCP(mac);
    eth.configure(dhcp.getLocalIp(), dhcp.getDnsServerIp(), dhcp.getGw(), dhcp.getSubnetMask());

    addresses(eth, cout);
    cout << "\r\n";
    server.begin();
    Buffer buffer;
    bool ret = zd.begin();

    if (!ret)
    {
        cout << "init failed!\r\n";
        return 0;
    }

    while (true)
    {
        UIPClient client = server.available();

        if (client)
        {
            while (client.connected())
            {
                if (client.available())
                {
                    char c = client.read();
                    buffer.push(c);

                    if (buffer.end())
                    {
                        cout << buffer.get();
                        cout << "\r\n";
                        cout.flush();

                        if (strncmp("GET ", buffer.get(), 4) == 0)
                        {
                            httpGet(client, zd, buffer);
                        }
                        else if (strncmp("PUT ", buffer.get(), 4) == 0)
                        {
                            client.write("HTTP/1.1 200 OK\r\n\r\n");
                        }
                        else if (strncmp("DELETE ", buffer.get(), 7) == 0)
                        {
                            char fn[50];

                            for (uint16_t i = 8; i < 512; i++)
                            {
                                if (buffer.get()[i] != ' ')
                                    fn[i - 8] = buffer.get()[i];
                                else
                                    break;
                            }
                            
                            zd.remove(fn);
                            cout << buffer.get();
                            cout << "\r\n";
                            cout.flush();
                        }
                        else if (strncmp("POST ", buffer.get(), 5) == 0)
                        {
                        }

                        buffer.reset();
                        break;
                    }
                }
            }

            for (volatile uint16_t i = 0; i < 0x4fff; i++); // delay
            client.stop();
        }
    }

    return 0;
}

extern "C" void TIMER0_OVF __attribute__ ((signal, used, externally_visible));
void TIMER0_OVF
{
    eth.tick2();
}




