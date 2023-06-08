#include <QApplication>

#include "snakebot.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    SnakeBot client;

    if (!client.stop)
    {
        client.show();
        return a.exec();
    }

    return 0;
}
