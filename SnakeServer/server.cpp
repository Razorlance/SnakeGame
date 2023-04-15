#include "server.h"

Server::Server()
{
    if (this->listen(QHostAddress::Any, 33221))
        qDebug() << "Started";
    else
        qDebug() << "Error";

    Players.enqueue(&Player1);
    Players.enqueue(&Player2);
    nextBlockSize = 0;

    //    Players.enqueue(&Player3);
    //    Players.enqueue(&Player4);

    //    Player1.direction = right;
    //    Player2.direction = down;

    //    Player3.direction = left;
    //    Player4.direction = up;
}

void Server::timerEvent(QTimerEvent *event)
{
    //    qDebug() << "Tick" << _stillGame << _await;
    //    if (_stillGame & _await)
    //    {
    //        eatFruit();
    //        move();
    //        checkBoundary();
    //        _await = false;
    //        this->repaint();
    //    }
    //    SendToServer(_homeDots);

    SendHomeCoordinates();
    SendEnemyCoordinates();
    SendEnemyDirections();

    if (checkBoundary())
    {
        //        if (_count.size() == Sockets.size())
        //        {
        qDebug() << "Continue the game";
        move();
        SendToClient("c 1");
        _count.clear();
        //        }
    }
    else
        endGame();
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
        out << quint16(0) << str;
        out.device()->seek(0);
        out << quint16(Data.size() - sizeof(quint16));
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
        for (QMap<qintptr, Snake *>::Iterator it1 = PlayerList.begin();
             it1 != PlayerList.end(); it1++)
        {
            it.value()->setSocketOption(QAbstractSocket::LowDelayOption, 1);
            qDebug() << it.key() << " " << it1.key();
            if (it.key() == it1.key())
                continue;
            qDebug() << "g " + convertToString(it1.value()->_homeDots);
            out << quint16(0) << "g " + convertToString(it1.value()->_homeDots);
            out.device()->seek(0);
            out << quint16(Data.size() - sizeof(quint16));
            it.value()->write(Data);
        }
    }
}

void Server::SendEnemyDirections()
{
    qDebug() << "Sending enemy coordinates...";
    for (QMap<qintptr, QTcpSocket *>::Iterator it = Sockets.begin();
         it != Sockets.end(); it++)
    {
        Data.clear();
        QDataStream out(&Data, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_6_3);
        for (QMap<qintptr, Snake *>::Iterator it1 = PlayerList.begin();
             it1 != PlayerList.end(); it1++)
        {
            qDebug() << it.key() << " " << it1.key();
            if (it.key() == it1.key())
                continue;
            qDebug() << "d " + QString::number(it1.value()->direction);
            out << quint16(0) << "d " + QString::number(it1.value()->direction);
            out.device()->seek(0);
            out << quint16(Data.size() - sizeof(quint16));
            it.value()->write(Data);
        }
    }
}

void Server::SendHomeCoordinates()
{
    qDebug() << "Sending home coordinates...";
    for (QMap<qintptr, QTcpSocket *>::Iterator it = Sockets.begin();
         it != Sockets.end(); it++)
    {
        Data.clear();
        QDataStream out(&Data, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_6_3);
        for (QMap<qintptr, Snake *>::Iterator it1 = PlayerList.begin();
             it1 != PlayerList.end(); it1++)
        {
            qDebug() << it.key() << " " << it1.key();
            if (it.key() == it1.key())
            {
                qDebug() << "h " + convertToString(it1.value()->_homeDots);
                out << quint16(0)
                    << "h " + convertToString(it1.value()->_homeDots);
                out.device()->seek(0);
                out << quint16(Data.size() - sizeof(quint16));
                it.value()->write(Data);
            }
        }
    }
}

bool Server::checkBoundary()
{
    return true;  // Costyl
    for (QMap<qintptr, Snake *>::Iterator it = PlayerList.begin();
         it != PlayerList.end(); it++)
    {
        if (it.value()->_homeDots.size() > 4)
        {
            for (size_t i = 1; i < it.value()->_homeDots.size(); i++)
            {
                if (it.value()->_homeDots[0] == it.value()->_homeDots[i])
                {
                    return false;
                }
            }
        }
        if (it.value()->_homeDots[0].rx() < 0)
        {
            return false;
        }
        if (it.value()->_homeDots[0].rx() == _field_width)
        {
            return false;
        }
        if (it.value()->_homeDots[0].ry() < 0)
        {
            return false;
        }
        if (it.value()->_homeDots[0].ry() == _field_height)
        {
            return false;
        }
    }
    return true;
}

void Server::locateFruit()
{
    QTime time = QTime::currentTime();
    srand((uint)time.msec());

    _fruitPos.rx() = rand() % _width;
    _fruitPos.ry() = rand() % _height;
    SendToClient("f " + QString::number(_fruitPos.rx()) + " " +
                 QString::number(_fruitPos.ry()));
}

void Server::initiateGame()
{
    qDebug() << "Starting the game...";
    qDebug() << "Creating a timer...";
    _timer = startTimer(_delay);
    qDebug() << "Locating fruit...";
    locateFruit();
    //        for (qintptr &socket : Sockets.keys())
    qDebug() << "Sending command to start..";
    SendToClient("r 1");
}

void Server::endGame()
{
    SendToClient("e");
    killTimer(_timer);
}

void Server::move()
{
    for (size_t i = Player1._homeDots.size() - 1; i > 0; i--)
    {
        Player1._homeDots[i] = Player1._homeDots[i - 1];
        Player1._enemyDots[i] = Player1._enemyDots[i - 1];

        Player2._homeDots[i] = Player2._homeDots[i - 1];
        Player2._enemyDots[i] = Player2._enemyDots[i - 1];
    }
    //    Player1._homeDots[0].rx()++;
    //    Player1._enemyDots[0].ry()++;

    //    Player2._homeDots[0].ry()++;
    //    Player2._enemyDots[0].rx()++;
    switch (Player1.direction)
    {
        case left:
            Player1._homeDots[0].rx()--;
            break;
        case right:
            Player1._homeDots[0].rx()++;
            break;
        case up:
            Player1._homeDots[0].ry()--;
            break;
        case down:
            Player1._homeDots[0].ry()++;
            break;
    }
    switch (Player2.direction)
    {
        case left:
            Player2._homeDots[0].rx()--;
            break;
        case right:
            Player2._homeDots[0].rx()++;
            break;
        case up:
            Player2._homeDots[0].ry()--;
            break;
        case down:
            Player2._homeDots[0].ry()++;
            break;
    }
    Player1._enemyDots = Player2._homeDots;
    Player2._enemyDots = Player1._homeDots;
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
    socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);

    Sockets[SocketDescriptor] = socket;
    Snake *S = Players.dequeue();
    PlayerList[SocketDescriptor] = S;
    S->socket = socket;
    qDebug() << "Client connected" << SocketDescriptor;

    this->nextPendingConnection();
    if (Sockets.size() == 2)
        initiateGame();
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
        //        in >> str;
        while (true)
        {
            if (nextBlockSize == 0)
            {
                if (socket->bytesAvailable() < 2)
                    break;
                in >> nextBlockSize;
            }
            if (socket->bytesAvailable() < nextBlockSize)
                break;
            in >> str;
            nextBlockSize = 0;
            QStringList L = str.split(" ");
            qDebug() << str;
            if (L[0] == 'd')
            {
                PlayerList[socket->socketDescriptor()]->direction =
                    Directions(L[1].toInt());
                _count.insert(socket->socketDescriptor());
            }
            if (L[0] == 'i')
            {
                _Dots[socket->socketDescriptor()] = convertToDots(L);
                //            SendEnemyCoordinates();
            }
            break;
        }
    }
    else
        qDebug() << "Error";
    //    SendToClient("c 1");
}
