#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QTime>
#include <QVector>

#include "snake.h"

class Server : public QTcpServer
{
    Q_OBJECT
   public:
    Server();
    QTcpSocket *socket;

   protected:
    void timerEvent(QTimerEvent *event) override;

   private:
    QMap<qintptr, QTcpSocket *> Sockets;
    QMap<qintptr, QVector<QPoint>> _Dots;
    QSet<qintptr> _count;
    QPoint _fruitPos;

    int _timer;
    static const int _delay = 1000;
    static const int _width = 25;  // The size of points
    static const int _height = 24;
    static const int _field_width = 25;  // The number of points
    static const int _field_height = 25;

    QVector<QPoint> convertToDots(QStringList &str);
    QString convertToString(QVector<QPoint> &dots);
    QByteArray Data;
    void SendToClient(QString str);
    void SendEnemyCoordinates();
    bool checkBoundary();
    void locateFruit();
    void initiateGame();
    void endGame();

   public slots:
    void incomingConnection(qintptr SocketDescriptor) override;
    void slotReadyRead();
};

#endif  // SERVER_H
