/*
RC522 kaartlezer

Uno pinout:
CS: D10


*/

#include "rc522.h"
#include "stream.h"

ostream *g_debug;

int main()
{
    MFRC522 rc522;
    rc522.init();
    DefaultUart uart;
    UartStream cout(&uart);
    g_debug = &cout;
    cout.dinges("MFRC522 version: 0x");
    cout.flush();
    uint8_t v = rc522.readReg(VersionReg);
    hex8(v, cout);
    cout << "\r\n";
    cout.flush();

    while (true)
    {
#if 0
        cout.dinges("Is New Card Present?\r\n");
        cout.flush();
#endif
        if (!rc522.isNewCardPresent())
            continue;
#if 1
        cout.dinges("Kaart aanwezig!\r\n");
        cout.flush();
#endif
        if (!rc522.readCardSerial())
            continue;

        rc522.dump(cout);
    }
}


