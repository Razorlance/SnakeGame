#include <QApplication>

#include "enter.h"
#include "snakeclient.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //#SnakeClient w;
    enter h;
    h.show();
    return a.exec();
}
