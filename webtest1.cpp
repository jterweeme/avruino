#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <util/delay.h>
#include "webtest1.h"

void WebTest1::dispatch(UIPClient &client)
{
    if (client)
    {
        *_serial << "Client\r\n";
        bool currentLineIsBlank = true;

        while (client.connected())
        {
            if (client.available())
            {
                char c = client.read();

                if (c == '\n' && currentLineIsBlank)
                {
                    client.write("HTTP/1.1 200 OK\r\n");
                    client.write("Content-Type: text/html\r\n");
                    client.write("Connection: close\r\n");
                    client.write("Refresh: 5\r\n\r\n");
                    client.write("<!DOCTYPE HTML>\r\n");
                    client.write("<html>\r\n");

                    for (uint8_t analogChannel = 0; analogChannel < 6; analogChannel++)
                    {
                        client.write("analog input ");
                        client.write(nibble(analogChannel & 0xf));
                        client.write(" is ");
                        uint8_t count = _count++;
                        client.write(nibble(count >> 4 & 0xf));
                        client.write(nibble(count & 0xf));
                        client.write("<br />\r\n");
                    }

                    client.write("</html>\r\n");
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


