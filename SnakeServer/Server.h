#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>

#include "snake.h"

class Server : public QTcpServer
{
    Q_OBJECT
   public:
    Server();
    QTcpSocket *socket;

   private:
    QMap<qintptr, QTcpSocket *> Sockets;
    QMap<qintptr, QVector<QPoint>> _Dots;
    QSet<qintptr> _count;

    QVector<QPoint> convertToDots(QStringList &str);
    QString convertToString(QVector<QPoint> &dots);
    QByteArray Data;
    void SendToClient(QString str);
    void SendEnemyCoordinates();

   public slots:
    void incomingConnection(qintptr SocketDescriptor);
    void slotReadyRead();
};

#endif  // SERVER_H
