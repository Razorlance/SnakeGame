#include "server.h"

Server::Server()
{
    if (this->listen(QHostAddress::Any, 33221))
        qDebug() << "Started";
    else
        qDebug() << "Error";

    // Starts drawing after the next

    Players.enqueue(&Player1);
    Players.enqueue(&Player2);
    Players.enqueue(&Player3);
    Players.enqueue(&Player4);
    nextBlockSize = 0;

    Player1._homeDots = {QPoint(0, 0), QPoint(1, 0)};
    Player2._homeDots = {QPoint(0, 8), QPoint(1, 8)};
    Player3._homeDots = {QPoint(0, 16), QPoint(1, 16)};
    Player4._homeDots = {QPoint(0, 24), QPoint(1, 24)};
}

void Server::timerEvent(QTimerEvent *event)
{
    if (checkBoundary())
    {
        qDebug() << "Continue the game";
        move();
        eatFruit();
        SendData();
        _count.clear();
    }
    else
        endGame();
}

void Server::SendData()
{
    qDebug() << "Sending enemy coordinates, directions and home coordinates...";
    for (QMap<qintptr, QTcpSocket *>::Iterator it = Sockets.begin();
         it != Sockets.end(); it++)
    {
        Data.clear();
        QDataStream out(&Data, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_6_3);
        QString homeCoordinates =
            "h " + convertToString(PlayerList[it.key()]->_homeDots);
        for (QMap<qintptr, Snake *>::Iterator it1 = PlayerList.begin();
             it1 != PlayerList.end(); it1++)
        {
            qDebug() << it.key() << " " << it1.key();

            if (it.key() == it1.key())
                continue;
            // 11123 12332 123123 123123
            // Player1: socket, home, enemy, direction
            // Player1 -> Player2, Player3, Player4
            QString enemyCoordinates =
                "g " + convertToString(it1.value()->_homeDots);

            QString enemyDirections =
                "d " + QString::number(it1.value()->direction);

            QString dataToSend = homeCoordinates + ";" + enemyCoordinates +
                                 ";" + enemyDirections + +";c 1";

            qDebug() << dataToSend;

            qDebug() << dataToSend;
            out << quint16(0) << dataToSend;
            out.device()->seek(0);
            out << quint16(Data.size() - sizeof(quint16));
            it.value()->write(Data);
            it.value()->waitForBytesWritten();
        }
    }
}

void Server::SendData(QString str)
{
    qDebug() << "Sending specific data...";
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

            qDebug() << str;
            out << quint16(0) << str;
            out.device()->seek(0);
            out << quint16(Data.size() - sizeof(quint16));
            it.value()->write(Data);
            it.value()->waitForBytesWritten();
        }
    }
}

bool Server::checkBoundary()
{
    // Look at it carefully

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
}

void Server::eatFruit()
{
    for (Snake *Player : PlayerList.values())
    {
        if (Player->_homeDots[0] == _fruitPos)
        {
            Player->_homeDots.push_back(_fruitPos);
            locateFruit();
            QString fruitPosition = "f " + QString::number(_fruitPos.rx()) +
                                    " " + QString::number(_fruitPos.ry());
            SendData(fruitPosition);
        }
    }
}

void Server::initiateGame()
{
    qDebug() << "Starting the game...";
    qDebug() << "Creating a timer...";
    _timer = startTimer(_delay);
    qDebug() << "Locating fruit...";
    locateFruit();
    QString fruitPosition = "f " + QString::number(_fruitPos.rx()) + " " +
                            QString::number(_fruitPos.ry());
    //        for (qintptr &socket : Sockets.keys())
    qDebug() << "Sending command to start..";
    SendData("r 1;" + fruitPosition);
}

void Server::endGame()
{
    SendData("e");
    killTimer(_timer);
}

void Server::move()
{
    // Fix for() loop
    for (size_t i = Player1._homeDots.size() - 1; i > 0; i--)
    {
        Player1._homeDots[i] = Player1._homeDots[i - 1];
        Player1._enemyDots[i] = Player1._enemyDots[i - 1];

        Player2._homeDots[i] = Player2._homeDots[i - 1];
        Player2._enemyDots[i] = Player2._enemyDots[i - 1];
    }

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
            }
            break;
        }
    }
    else
        qDebug() << "Error";
}

/*
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

    //    SendHomeCoordinates();
    //    SendEnemyCoordinates();
    //    SendEnemyDirections();

    if (checkBoundary())
    {
        //        if (_count.size() == Sockets.size())
        //        {
        qDebug() << "Continue the game";
        move();
        //        SendToClient("c 1");
        SendData();
        _count.clear();
        //        }
    }
    else
        endGame();
    //    SendData();
}
*/
