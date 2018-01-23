#include "stream.h"

int main()
{
    CDC cdc;
    USBStream cout(&cdc);

    while (true)
        cout.writeString("pharmacist\r\n");

    return 0;
}




