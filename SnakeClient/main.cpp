#include <QApplication>

#include "snakeclient.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    SnakeClient client;

    if (!client.stop)
    {
        client.show();
        return a.exec();
    }

    return 0;
}
