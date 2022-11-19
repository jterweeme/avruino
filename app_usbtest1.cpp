#include "cdc.h"
#include "stream.h"

int main()
{
    CDC cdc;
    USBStream cout(&cdc);

    while (true)
        cout << "pharmacist\r\n";

    return 0;
}




