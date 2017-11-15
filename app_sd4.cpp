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

    File dataFile = sdClass.open("LUCKY.TXT");

    if (dataFile)
    {
        while (dataFile.available())
            serial.myPutc(dataFile.read());

        dataFile.close();
    }

    while (true) { }
    return 0;
}

int main()
{
    App app;
	return app.run();
}





