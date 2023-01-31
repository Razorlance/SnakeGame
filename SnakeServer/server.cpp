#include "server.h"

Server::Server()
{
    if (this->listen(QHostAddress::Any, 33221))
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
    for (int i = 0; i < Sockets.size(); i++)
        Sockets[i]->write(Data);
}

void Server::incomingConnection(qintptr SocketDescriptor)
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
    socket = (QTcpSocket *)sender();
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_3);
    if (in.status() == QDataStream::Ok)
    {
        qDebug() << "Reading...";
        QVector<QPoint> _dots;
        in >> _dots;
        qDebug() << _dots.size();
        for (QPoint &i : _dots)
            qDebug() << i.x() << ' ' << i.y() << '\n';
    }
    else
        qDebug() << "Error";
}
