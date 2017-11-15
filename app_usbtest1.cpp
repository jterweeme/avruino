#include "misc.h"
#include "busby.h"

class App
{
public:
    App() { }
    USB busby;
    int run();
};

int main()
{
    App app;
    return app.run();
}

int App::run()
{
    while (true)
        busby.puts("pharmacist\r\n");

    return 0;
}



