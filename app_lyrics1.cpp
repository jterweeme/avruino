#include "sd.h"

class App : public Board
{
    Timer0 timer0;
    Sd2Card card;
    SdVolume volume;
    SdFile root;
    SDClass sdClass;
    SerialWrapper serial;
public:
    App();
    int run();
};

struct Header_t
{
    uint32_t chunkID;
    uint32_t chunkSize;
    uint16_t formatType;
    uint16_t tracks;
    uint16_t timeDivision;
} __attribute__ ((packed));

class CHeader
{
public:
    Header_t header;
    CHeader() { }
    void read(File &inFile);
};

class KarFile
{
public:
    CHeader header;
    void read(File &iStream);
};

void CHeader::read(File &inFile)
{
    inFile.read((char *)&header, 14);
}

void KarFile::read(File &iStream)
{
    header.read(iStream);
}

App::App()
{
    sei();
    TCCR0A |= (1 << WGM01) | (1<<WGM00);
    TCCR0B |= (1 << CS01) | (1<<CS00);
    TIMSK0 |= 1<<TOIE0;
    pin10.direction(OUTPUT);
}

int App::run()
{
    if (!sdClass.begin(4))
    {
        serial.puts("Kan kaart niet lezen\r\n");
        return 0;
    }

    File dataFile = sdClass.open("VOGUE.KAR");

    if (dataFile)
    {
        KarFile karFile;
        karFile.read(dataFile);
        dataFile.close();
        serial.printf("%u\r\n", karFile.header.header.chunkID);
    }

    string bye("Lyrics entered by ");
    serial.printf("%u\r\n", bye.size());
    serial.printf("%u\r\n", bye.capacity());
    serial.printf("%s\r\n", bye.c_str());
    bye.reserve(10);
    bye.append(string("Jasper"));
    serial.printf("%s\r\n", bye.c_str());
    while (true) { }
    return 0;
}

int main()
{
    App app;
	return app.run();
}





