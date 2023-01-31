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
    QVector<QTcpSocket *> Sockets;
    QByteArray Data;
    void SendToClient(QString str);

   public slots:
    void incomingConnection(qintptr SocketDescriptor);
    void slotReadyRead();
};

#endif  // SERVER_H
