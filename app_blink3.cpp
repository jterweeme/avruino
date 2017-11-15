#include "board.h"

class App : public Board
{
public:
    App() { pinA4.direction(OUTPUT); }
    int run();
};

int App::run()
{
    while (true)
    {
        pinA4.toggle();
        Utility::delay(0x7fff);
    }

    return 0;
}

int main()
{
    App app;
    return app.run();
}


