#include "sd.h"

class App : public Board
{
    Timer0 timer0;
    Sd2Card card;
    SdVolume volume;
    SdFile root;
    SerialWrapper Serial;
public:
    App();
    int run();
};

App::App()
{
    sei();
    TCCR0A |= (1 << WGM01) | (1<<WGM00);
    TCCR0B |= (1 << CS01) | (1<<CS00);
    TIMSK0 |= 1<<TOIE0;
    Serial.puts("\r\nInitializing SD card... ");
    pin10.direction(OUTPUT);
}

int App::run()
{
    if (!card.init(SPI_HALF_SPEED, 4))
    {
        Serial.puts("initialization failed. Things to check:\r\n");
        Serial.puts("* is a card is inserted?\r\n");
        Serial.puts("* Is your wiring correct?\r\n");
        Serial.puts("* did you change the chipSelect pin to match your shield or module?\r\n");
        return 0;
    }
    else
    {
        Serial.puts("Wiring is correct and a card is present.\r\n");
    }

    Serial.puts("\r\nCard type: ");
    switch(card.type())
    {
    case SD_CARD_TYPE_SD1:
        Serial.puts("SD1\r\n");
        break;
    case SD_CARD_TYPE_SD2:
        Serial.puts("SD2\r\n");
        break;
    case SD_CARD_TYPE_SDHC:
        Serial.puts("SDHC\r\n");
        break;
    default:
        Serial.puts("Unknown\r\n");
    }

    if (!volume.init(card))
    {
        Serial.puts("Could not find FAT16/FAT32 partition.\r\n");
        Serial.puts("Make sure you've formatted the card\r\n");
        return 0;
    }

    uint32_t volumesize;
    Serial.printf("\r\nVolume type is FAT%u\r\n", volume.fatType());
    volumesize = volume.blocksPerCluster();
    volumesize *= volume.clusterCount();
    volumesize *= 512;
    Serial.printf("Volume size (bytes): %u\r\n", volumesize);
    volumesize /= 1024;
    Serial.printf("Volume size (Kbytes): %u\r\n");
    volumesize /= 1024;
    Serial.printf("Volume size (Mbytes): %u\r\n");
    Serial.puts("\r\nFiles found on the card (name, date and size in bytes): \r\n");
    root.openRoot(volume);
    root.ls(LS_R | LS_DATE | LS_SIZE, 0, Serial);
    while (true) { }
    return 0;
}

int main()
{
    App app;
	return app.run();
}





