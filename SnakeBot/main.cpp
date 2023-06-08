#include <QApplication>

#include "snakebot.h"

int main(int argc, char* argv[])
{
    //    int result = 0;

    //    do
    //    {
    QApplication a(argc, argv);
    SnakeBot bot;

    //    if (!client.stop)
    //    {
    bot.show();
    //    a.exec();
    //    }

    //        else
    //            break;
    //    } while (result == 1234);

    return a.exec();
}
