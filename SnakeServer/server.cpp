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
    //    out << str;
    //    for (auto &socket : Sockets.values())
    //        socket->write(str.toLocal8Bit());
    //    for (int i = 0; i < Sockets.size(); i++)

    //        Sockets[i]->write(str.toLocal8Bit());

    //    for (QTcpSocket *i : Sockets)
    //    {
    //        Data.clear();
    //        QDataStream out(&Data, QIODevice::WriteOnly);
    //        out.setVersion(QDataStream::Qt_6_3);
    //        out << str;
    //        i->write(Data);
    //    }
    for (QMap<qintptr, QTcpSocket *>::Iterator it = Sockets.begin();
         it != Sockets.end(); it++)
    {
        Data.clear();
        QDataStream out(&Data, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_6_3);
        out << str;
        it.value()->write(Data);
    }
}

void Server::SendEnemyCoordinates()
{
    qDebug() << "Sending enemy coordinates...";
    for (QMap<qintptr, QTcpSocket *>::Iterator it = Sockets.begin();
         it != Sockets.end(); it++)
    {
        Data.clear();
        QDataStream out(&Data, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_6_3);
        for (QMap<qintptr, QVector<QPoint>>::Iterator it1 = _Dots.begin();
             it1 != _Dots.end(); it1++)
        {
            qDebug() << it.key() << " " << it1.key();
            if (it.key() == it1.key())
                continue;
            qDebug() << "g " + convertToString(it1.value());
            out << "g " + convertToString(it1.value());
            it.value()->write(Data);
        }
    }
}

QVector<QPoint> Server::convertToDots(QStringList &str)
{
    QVector<QPoint> dots(2);
    //    for (size_t i = 1; i < str.size(); i+= 2){
    dots[0] = QPoint(str[1].toInt(), str[2].toInt());
    dots[1] = QPoint(str[3].toInt(), str[4].toInt());
    //    }
    return dots;
}

QString Server::convertToString(QVector<QPoint> &dots)
{
    QString str("");
    for (const QPoint &dot : dots)
        str += QString::number(dot.x()) + " " + QString::number(dot.y()) + " ";
    return str.trimmed();
}

void Server::incomingConnection(qintptr SocketDescriptor)
{
    socket = new QTcpSocket;
    socket->setSocketDescriptor(SocketDescriptor);

    connect(socket, &QTcpSocket::readyRead, this, &Server::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket,
            &QTcpSocket::deleteLater);
    Sockets[SocketDescriptor] = socket;
    qDebug() << "Client connected" << SocketDescriptor;
    this->nextPendingConnection();
    if (Sockets.size() == 2)
    {
        qDebug() << "Starting the game...";

        qDebug() << "Sending command to start...";
        //        for (qintptr &socket : Sockets.keys())
        SendToClient("r 1");
    }
}

void Server::slotReadyRead()
{
    socket = (QTcpSocket *)sender();
    this->nextPendingConnection();
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_3);
    if (in.status() == QDataStream::Ok)
    {
        qDebug() << "Reading from " << socket->socketDescriptor() << "...";
        QString str;
        in >> str;
        QStringList L = str.split(" ");
        qDebug() << str;

        if (L[0] == 'i')
        {
            _Dots[socket->socketDescriptor()] = convertToDots(L);
            _count.insert(socket->socketDescriptor());
            SendEnemyCoordinates();
        }
    }
    else
        qDebug() << "Error";
    //    SendToClient("c 1");
    if (_count.size() == Sockets.size())
    {
        qDebug() << "Continue the game";
        SendToClient("c 1");
        _count.clear();
    }
}
