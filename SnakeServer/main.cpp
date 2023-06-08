#include "server.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Server s;

    if (!s.stop)
    {
        return a.exec();
    }

    return 0;
}
