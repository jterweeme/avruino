#include <avr/io.h>
class App
{
public:
    App();
    int run();
};

App::App()
{
    DDRB = 0xff;
    DDRD = 0xff;
}

int App::run()
{
    PORTB = 0xff;
    PORTD = 0xff;
    return 0;
}

int main()
{
    App app;
    return app.run();
}


