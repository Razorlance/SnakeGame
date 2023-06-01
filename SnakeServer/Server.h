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
    // Write a destructor for a server
    //    ~Server();

    QTcpSocket* socket;

protected:
    void timerEvent(QTimerEvent *event) override;

private:
    QMap<qintptr, QTcpSocket *> _Sockets;
    QMap<qintptr, QVector<QPoint>> _Dots;
    QMap<qintptr, Snake *> _PlayerList;
    QList<qintptr> _ViewerList;
    QSet<qintptr> _count;
    QPoint _fruitPos;

    int _timer;
    static const int _delay = 500;
    static const int _width = 25;  // The size of points
    static const int _height = 24;
    static const int _field_width = 21;  // The number of points
    static const int _field_height = 25;

    Snake _Player1;
    Snake _Player2;
    Snake _Player3;
    Snake _Player4;

    QQueue<Snake *> _Players;

    QString _convertToString(QVector<QPoint> &dots);
    QByteArray _Data;
    quint16 _nextBlockSize;

    void _SendData();
    void _SendData(QString str);
    bool _checkBoundary();
    void _locateFruit();
    void _initiateGame();
    void _eatFruit();
    void _endGame();
    void _move();

public slots:
    void incomingConnection(qintptr SocketDescriptor) override;
    void slotReadyRead();
};

#endif  // SERVER_H
