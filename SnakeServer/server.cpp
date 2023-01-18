#include "server.h"

Server::Server()
{
    if (this->listen(QHostAddress::Any, 2233))
        qDebug() << "Started";
    else
        qDebug() << "Error";
}

void Server::SendToClient(QString str)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_3);
    out << str;
    socket->write(Data);
}

void Server::incomConnection(qintptr SocketDescriptor)
{
    socket = new QTcpSocket;
    socket->setSocketDescriptor(SocketDescriptor);
    connect(socket, &QTcpSocket::readyRead, this, &Server::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket,
            &QTcpSocket::deleteLater);
    Sockets.push_back(socket);
    qDebug() << "Client connected" << SocketDescriptor;
}

void Server::slotReadyRead()
{
    socket = (QTcpSocket*)sender();
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_3);
    if (in.status() == QDataStream::Ok)
    {
        qDebug() << "Reading...";
        QString str;
        in >> str;
        qDebug() << str;
    }
    else
        qDebug() << "Error";
}
