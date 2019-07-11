#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <util/delay.h>
#include "webservice1.h"
#include "client.h"
#include "stream.h"

WebService1::WebService1(ostream *serial) : _serial(serial)
{
}

void WebService1::dispatch(Client &client)
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
                    client << "Content-Type: application/json\r\n";
                    client << "Connection: close\r\n";
                    client << "Refresh: 5\r\n\r\n";
                    client << "{\r\n";
                    client << "  \"machine\": \"Obelix\",";
                    client << "  \"channels\": [\r\n";

                    for (uint8_t analogChannel = 0; analogChannel < 6; analogChannel++)
                    {
                        client << "    {\r\n      \"value\": \"";
                        uint8_t count = _count++;
                        client.put(nibble(count >> 4 & 0xf));
                        client.put(nibble(count >> 0xf));
                        client << "\"\r\n    }";
                        if (analogChannel < 5)
                            client << ",";
                        client << "\r\n";
                    }

                    client << "  ]\r\n}\r\n";
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


