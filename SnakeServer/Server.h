#ifndef SERVER_H
#define SERVER_H

#include <QQueue>
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
    //    ~Server();

    QTcpSocket *socket;

   protected:
    void timerEvent(QTimerEvent *event) override;

   private:
    QMap<qintptr, QTcpSocket *> Sockets;
    QMap<qintptr, QVector<QPoint>> _Dots;
    QMap<qintptr, Snake *> PlayerList;
    QSet<qintptr> _count;
    QPoint _fruitPos;

    int _timer;
    static const int _delay = 1000;
    static const int _width = 25;  // The size of points
    static const int _height = 24;
    static const int _field_width = 25;  // The number of points
    static const int _field_height = 25;

    Snake Player1;
    Snake Player2;
    Snake Player3;
    Snake Player4;

    QQueue<Snake *> Players;

    QVector<QPoint> convertToDots(QStringList &str);
    QString convertToString(QVector<QPoint> &dots);
    QByteArray Data;
    quint16 nextBlockSize;
    void SendToClient(QString str);
    void SendEnemyCoordinates();
    void SendEnemyDirections();
    void SendHomeCoordinates();
    bool checkBoundary();
    void locateFruit();
    void initiateGame();
    void endGame();
    void move();

   public slots:
    void incomingConnection(qintptr SocketDescriptor) override;
    void slotReadyRead();
};

#endif  // SERVER_H
