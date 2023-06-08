#include <QApplication>

#include "snakeclient.h"

int main(int argc, char* argv[])
{
    int result = 0;

    do
    {
        QApplication a(argc, argv);
        SnakeClient client;

        if (!client.stop)
        {
            client.show();
            result = a.exec();
        }

        else
            break;
    } while (result == 1234);

    return 0;
}
