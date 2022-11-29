#include "snakeserver.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SnakeServer w;
    w.show();
    return a.exec();
}
