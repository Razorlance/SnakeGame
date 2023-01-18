#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>

class Server : public QTcpServer
{
   public:
    Server();
    QTcpSocket *socket;

   private:
    QVector<QTcpSocket *> Sockets;
    QByteArray Data;
    void SendToClient(QString str);

   public slots:
    void incomConnection(qintptr SocketDescriptor);
    void slotReadyRead();
};

#endif  // SERVER_H
