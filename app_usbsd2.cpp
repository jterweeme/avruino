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

#ifndef BUSBY_INT
extern "C" void __vector_11() __attribute__ ((signal, used, externally_visible));
void __vector_11()
{
    USB::instance->com();
}

extern "C" void __vector_10() __attribute__ ((signal, used, externally_visible));
void __vector_10()
{
    USB::instance->gen();
}
#endif




