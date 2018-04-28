#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <util/delay.h>
#include "webtest1.h"
#include "client.h"
#include "stream.h"

WebTest1::WebTest1(ostream *serial) : _serial(serial)
{
}

void WebTest1::dispatch(Client &client)
{
    if (client)
    {
        *_serial << "Client\r\n";
        _serial->flush();
        bool currentLineIsBlank = true;

        while (client.connected())
        {
            if (client.available())
            {
                char c = client.read();

                if (c == '\n' && currentLineIsBlank)
                {
                    client << "HTTP/1.1 200 OK\r\n";
                    client << "Content-Type: text/html\r\n";
                    client << "Connection: close\r\n";
                    client << "Refresh: 5\r\n\r\n";
                    client << "<!DOCTYPE HTML>\r\n";
                    client << "<html>\r\n";

                    for (uint8_t analogChannel = 0; analogChannel < 6; analogChannel++)
                    {
                        client << "analog input ";
                        client.put(nibble(analogChannel & 0xf));
                        client << " is ";
                        uint8_t count = _count++;
                        client.put(nibble(count >> 4 & 0xf));
                        client.put(nibble(count & 0xf));
                        client << "<br />\r\n";
                    }

                    client << "</html>\r\n";
                    break;
                }

                if (c == '\n')
                    currentLineIsBlank = true;
                else if (c != '\r')
                    currentLineIsBlank = false;
            }
        }

        _delay_ms(1);
        client.stop();
    }

}


