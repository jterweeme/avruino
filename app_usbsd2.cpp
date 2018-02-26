/*
USB SD card reader
*/

#include "usbsd.h"

int main(void)
{
    USBSD usbsd;
    zei();  // dit is nodig!

	while (true)
        usbsd.MassStorage_Task();
}

extern "C" void USB_COM __attribute__ ((signal, used, externally_visible));
void USB_COM
{
    USB::instance->com();
}

extern "C" void USB_GEN __attribute__ ((signal, used, externally_visible));
void USB_GEN
{
    USB::instance->gen();
}




