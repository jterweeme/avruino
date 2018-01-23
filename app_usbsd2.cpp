#include "usbsd.h"
#include <avr/interrupt.h>

int main(void)
{
#if 0
    Serial serial;
    serial.init();
    serial.write("SD card reader\r\n");
#endif
    USBSD usbsd;
    sei();  // dit is nodig!

	for (;;)
	{
        usbsd.MassStorage_Task();
	}
}






