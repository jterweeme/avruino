#include "board.h"
#include "cdc.h"
#include "stream.h"
#include <stdio.h>

static const char PI[] = "3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328230664709384460955058223172535940812848111745";

int main()
{
    CDC cdc;
    USBStream cout(&cdc);
    uint16_t i = 0;

    while (true)
    {
        while (true)
        {
            uint8_t byte = cdc.receive();

            if (byte == 255)
                continue;

            cout.write(byte);
            cout.flush();
        
            if (byte != PI[i++])
                break;
        }

        cout.writeString("\r\nHelaas!\r\n");
        char buf[50];
        snprintf(buf, 50, "%u\r\n", i);
        cout.writeString(buf);
        cout.flush();
        i = 0;
    }

    return 0;
}



