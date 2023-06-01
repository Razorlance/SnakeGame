#include <QApplication>

#include "snakeclient.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    SnakeClient client;
    client.show();
    return a.exec();
}
