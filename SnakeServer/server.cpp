#include "server.h"

Server::Server()
{
    _Player1._homeDots = {QPoint(0, 0), QPoint(1, 0)};
    _Player1.direction = right;
    _Player1._id = 1;

    _Player2._homeDots = {QPoint(19, 24), QPoint(18, 24)};
    _Player2.direction = left;
    _Player2._id = 2;

    _Player3._homeDots = {QPoint(0, 24), QPoint(0, 23)};
    _Player3.direction = up;
    _Player3._id = 3;

    _Player4._homeDots = {QPoint(19, 0), QPoint(18, 1)};
    _Player4.direction = down;
    _Player4._id = 4;

    QDialog *startWindow = new QDialog();

    startWindow->setWindowTitle("Preferences Window");
    startWindow->adjustSize();
    startWindow->move(QGuiApplication::primaryScreen()->geometry().center() -
                      startWindow->rect().center());

    QFormLayout *form = new QFormLayout(startWindow);
    QComboBox *type = new QComboBox(startWindow);
    QComboBox *gameTime = new QComboBox(startWindow);
    QLineEdit *port = new QLineEdit(startWindow);

    port->setText("33221");

    type->addItem("Test Bot");
    type->addItem("1:1");
    type->addItem("1:3");
    type->addItem("1:Bot");
    type->addItem("Bot:Bot");
    type->addItem("Bot:3 Bots");

    gameTime->addItem("1");
    gameTime->addItem("2");
    gameTime->addItem("5");

    form->addRow("Specify game type:", type);
    form->addRow("Specify game time (in minutes):", gameTime);
    form->addRow("Port:", port);

    QPushButton *button = new QPushButton("OK", startWindow);
    QObject::connect(button, &QPushButton::clicked, startWindow,
                     &QDialog::accept);
    form->addWidget(button);

    if (startWindow->exec() == QDialog::Accepted)
    {
        _port = port->text().toInt();

        if (type->currentText() == "Test Bot")
        {
            _type = 0;
            _botCount = 1;
            _Players.enqueue(&_Player1);
        }

        else if (type->currentText() == "1:1")
        {
            _type = 1;
            _playerCount = 2;

            _Players.enqueue(&_Player1);
            _Players.enqueue(&_Player2);

            _Player1._enemiesDots = {{_Player2._id, _Player2._homeDots}};

            _Player2._enemiesDots = {{_Player1._id, _Player1._homeDots}};
        }

        else if (type->currentText() == "1:3")
        {
            _type = 2;
            _playerCount = 4;

            _Players.enqueue(&_Player1);
            _Players.enqueue(&_Player2);
            _Players.enqueue(&_Player3);
            _Players.enqueue(&_Player4);

            _Player1._enemiesDots = {{_Player2._id, _Player2._homeDots},
                                     {_Player3._id, _Player3._homeDots},
                                     {_Player4._id, _Player4._homeDots}};

            _Player2._enemiesDots = {{_Player1._id, _Player1._homeDots},
                                     {_Player3._id, _Player3._homeDots},
                                     {_Player4._id, _Player4._homeDots}};

            _Player3._enemiesDots = {{_Player1._id, _Player1._homeDots},
                                     {_Player2._id, _Player2._homeDots},
                                     {_Player4._id, _Player4._homeDots}};

            _Player4._enemiesDots = {{_Player1._id, _Player1._homeDots},
                                     {_Player2._id, _Player2._homeDots},
                                     {_Player3._id, _Player3._homeDots}};
        }

        else if (type->currentText() == "1:Bot")
        {
            _type = 3;
            _playerCount = 1;
            _botCount = 1;

            _Players.enqueue(&_Player1);
            _Players.enqueue(&_Player2);

            _Player1._enemiesDots = {{_Player2._id, _Player2._homeDots}};
            _Player2._enemiesDots = {{_Player1._id, _Player1._homeDots}};
        }

        else if (type->currentText() == "Bot:Bot")
        {
            _type = 4;
            _botCount = 2;

            _Players.enqueue(&_Player1);
            _Players.enqueue(&_Player2);

            _Player1._enemiesDots = {{_Player2._id, _Player2._homeDots}};
            _Player2._enemiesDots = {{_Player1._id, _Player1._homeDots}};
        }

        else if (type->currentText() == "Bot:3 Bots")
        {
            _type = 5;
            _botCount = 4;

            _Players.enqueue(&_Player1);
            _Players.enqueue(&_Player2);
            _Players.enqueue(&_Player3);
            _Players.enqueue(&_Player4);

            _Player1._enemiesDots = {{_Player2._id, _Player2._homeDots},
                                     {_Player3._id, _Player3._homeDots},
                                     {_Player4._id, _Player4._homeDots}};

            _Player2._enemiesDots = {{_Player1._id, _Player1._homeDots},
                                     {_Player3._id, _Player3._homeDots},
                                     {_Player4._id, _Player4._homeDots}};

            _Player3._enemiesDots = {{_Player1._id, _Player1._homeDots},
                                     {_Player2._id, _Player2._homeDots},
                                     {_Player4._id, _Player4._homeDots}};

            _Player4._enemiesDots = {{_Player1._id, _Player1._homeDots},
                                     {_Player2._id, _Player2._homeDots},
                                     {_Player3._id, _Player3._homeDots}};
        }

        if (gameTime->currentText() == "1")
            _seconds = 60;

        else if (gameTime->currentText() == "2")
            _seconds = 120;

        else
            _seconds = 300;
    }
    _gameTimer = new QTimer(this);
    connect(_gameTimer, SIGNAL(timeout()), this, SLOT(timer_function()));

    if (this->listen(QHostAddress::Any, _port))
        qDebug() << "Started";
    else
        qDebug() << "Error";

    // Review code
    _nextBlockSize = 0;
    _totalCount = _playerCount + _botCount;
}

void Server::timerEvent(QTimerEvent *event)
{
    if (_seconds == 0)
        _timesUp();

    else
    {
        qDebug() << "Continue the game";
        _eatFruit();
        _move();
        _checkBoundary();

        if (_crashed.size() > 0 && _crashed.size() >= _PlayerList.size() - 1)
            _endGame();

        else
        {
            _SendData();
        }
    }
}

void Server::_SendData()
{
    qDebug() << "Sending enemy coordinates, id and home coordinates...";

    for (QMap<qintptr, QTcpSocket *>::Iterator it = _Sockets.begin();
         it != _Sockets.end(); it++)
    {
        _Data.clear();
        QDataStream out(&_Data, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_6_3);
        QString homeCoordinates =
            "h " + QString::number(_PlayerList[it.key()]->_crashed) + " " +
            _convertToString(_PlayerList[it.key()]->_homeDots);

        QString dataToSend = "c 1;" + homeCoordinates + ";";

        for (QMap<int, QVector<QPoint>>::Iterator ed =
             _PlayerList[it.key()]->_enemiesDots.begin();
             ed != _PlayerList[it.key()]->_enemiesDots.end(); ed++)
        {
            dataToSend += "g " + QString::number(ed.key());

            if (_crashed.contains(ed.key()))
                dataToSend += " 1 " + _convertToString(ed.value()) + ";";

            else
                dataToSend += " 0 " + _convertToString(ed.value()) + ";";
        }

        int currentMinutes = _seconds / 60;
        int currentSeconds = _seconds % 60;
        dataToSend += "s " + QString::number(currentMinutes) + ":" + QString::number(currentSeconds) + ";";

        qDebug() << dataToSend;
        out << quint16(0) << dataToSend;
        out.device()->seek(0);
        out << quint16(_Data.size() - sizeof(quint16));
        it.value()->write(_Data);
        it.value()->waitForBytesWritten();
    }

    for (QMap<qintptr, QTcpSocket *>::Iterator it = _ViewerList.begin();
         it != _ViewerList.end(); it++)
    {
        _Data.clear();
        QDataStream out(&_Data, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_6_3);
        QString dataToSend = "c 1;";

        for (QMap<qintptr, Snake *>::Iterator it1 = _PlayerList.begin();
             it1 != _PlayerList.end(); it1++)
        {
            dataToSend += "g " + QString::number(it1.value()->_id);

            if (_crashed.contains(it1.value()->_id))
                dataToSend +=
                    " 1 " + _convertToString(it1.value()->_homeDots) + ";";

            else
                dataToSend +=
                    " 0 " + _convertToString(it1.value()->_homeDots) + ";";
        }

        int currentMinutes = _seconds / 60;
        int currentSeconds = _seconds % 60;
        dataToSend += "s " + QString::number(currentMinutes) + ":" + QString::number(currentSeconds) + ";";

        qDebug() << dataToSend;
        out << quint16(0) << dataToSend;
        out.device()->seek(0);
        out << quint16(_Data.size() - sizeof(quint16));
        it.value()->write(_Data);
        it.value()->waitForBytesWritten();
    }
}

void Server::_SendData(QString str)
{
    qDebug() << "Sending specific data...";

    for (QMap<qintptr, QTcpSocket *>::Iterator it = _Sockets.begin();
         it != _Sockets.end(); it++)
    {
        _Data.clear();
        QDataStream out(&_Data, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_6_3);

        QString dataToSend = str;

        if (str[str.size() - 1] == 'r')
        {
            dataToSend += " " + QString::number(_PlayerList[it.key()]->_id);

            for (QMap<qintptr, Snake *>::Iterator it1 = _PlayerList.begin();
                 it1 != _PlayerList.end(); it1++)
            {
                if (it.key() == it1.key())
                    continue;

                dataToSend += ";n " + QString::number(it1.value()->_id) + " " +
                              it1.value()->_snakeName;
            }
        }

        qDebug() << dataToSend;

        out << quint16(0) << dataToSend;
        out.device()->seek(0);
        out << quint16(_Data.size() - sizeof(quint16));
        it.value()->write(_Data);
        it.value()->waitForBytesWritten();
    }

    for (QMap<qintptr, QTcpSocket *>::Iterator it = _ViewerList.begin();
         it != _ViewerList.end(); it++)
    {
        _Data.clear();
        QDataStream out(&_Data, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_6_3);

        QString dataToSend = str;

        if (str[str.size() - 1] == 'r')
        {
            dataToSend.remove(dataToSend.size() - 1, 1);
            for (QMap<qintptr, Snake *>::Iterator it1 = _PlayerList.begin();
                 it1 != _PlayerList.end(); it1++)
            {
                dataToSend += "n " + QString::number(it1.value()->_id) + " " +
                              it1.value()->_snakeName + ";";
            }
        }

        qDebug() << dataToSend;

        out << quint16(0) << dataToSend;
        out.device()->seek(0);
        out << quint16(_Data.size() - sizeof(quint16));
        it.value()->write(_Data);
        it.value()->waitForBytesWritten();
    }
}

void Server::_SendClientBack(QTcpSocket *clientSocket)
{
    qDebug() << "Sending client back...";
    _Data.clear();
    QDataStream out(&_Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_3);
    QString dataToSend = "wrong";
    out << quint16(0) << dataToSend;
    out.device()->seek(0);
    out << quint16(_Data.size() - sizeof(quint16));
    clientSocket->write(_Data);
    clientSocket->waitForBytesWritten();
}

void Server::_checkBoundary()
{
    QSet<int> newCrashes = _crashed;

    for (QMap<qintptr, Snake *>::Iterator it = _PlayerList.begin();
         it != _PlayerList.end(); it++)
    {
        if (!_crashed.contains(it.value()->_id))
        {
            for (size_t i = 0; i < it.value()->_homeDots.size(); i++)
            {
                for (QMap<int, QVector<QPoint>>::Iterator ed =
                     it.value()->_enemiesDots.begin();
                     ed != it.value()->_enemiesDots.end(); ed++)
                {
                    if (!_crashed.contains(ed.key()) &&
                        ed.value().contains(it.value()->_homeDots[i]))
                    {
                        newCrashes.insert(it.value()->_id);
                        it.value()->_crashed = 1;
                    }
                }
            }

            if (it.value()->_homeDots.size() > 4)
            {
                for (size_t i = 1; i < it.value()->_homeDots.size(); i++)
                {
                    if (it.value()->_homeDots[0] == it.value()->_homeDots[i])
                    {
                        newCrashes.insert(it.value()->_id);
                        it.value()->_crashed = 1;
                    }
                }
            }

            if (it.value()->_homeDots[0].rx() < 0)
            {
                newCrashes.insert(it.value()->_id);
                it.value()->_crashed = 1;
            }

            if (it.value()->_homeDots[0].rx() == _field_width)
            {
                newCrashes.insert(it.value()->_id);
                it.value()->_crashed = 1;
            }

            if (it.value()->_homeDots[0].ry() < 0)
            {
                newCrashes.insert(it.value()->_id);
                it.value()->_crashed = 1;
            }

            if (it.value()->_homeDots[0].ry() == _field_height)
            {
                newCrashes.insert(it.value()->_id);
                it.value()->_crashed = 1;
            }
        }
    }

    _crashed = newCrashes;
}

void Server::_locateFruit(int n)
{
    for (size_t i = 0; i < n; i++)
    {
        QPoint fruit;
        QTime time = QTime::currentTime();
        srand((uint)time.msec());

        fruit.rx() = rand() % _field_width;
        fruit.ry() = rand() % _field_height;

        for (QMap<qintptr, Snake *>::Iterator it = _PlayerList.begin();
             it != _PlayerList.end(); it++)
        {
            if (it.value()->_homeDots.contains(fruit) ||
                _fruits.contains(fruit))
            {
                _locateFruit(1);
                return;
            }
        }

        _fruits.append(fruit);
    }
}

void Server::_eatFruit()
{
    for (QMap<qintptr, Snake *>::Iterator it = _PlayerList.begin();
         it != _PlayerList.end(); it++)
    {
        for (QPoint f : _fruits)
        {
            if (it.value()->_homeDots[0] == f)
            {
                it.value()->_homeDots.push_back(f);
                QVector<QPoint>::Iterator it =
                    std::find(_fruits.begin(), _fruits.end(), f);
                _fruits.erase(it);
                _locateFruit(1);
                QString fruitPosition = "f";
                for (QPoint p : _fruits)
                {
                    fruitPosition += " " + QString::number(p.rx()) + " " +
                                     QString::number(p.ry());
                }
                _SendData(fruitPosition);
            }
        }
    }
}

void Server::_initiateGame()
{
    qDebug() << "Starting the game...";
    qDebug() << "Creating a timer...";
    _timer = startTimer(_delay);
    qDebug() << "Locating fruit...";
    _locateFruit(2);
    QString fruitPosition = "f";
    qDebug() << "Sending fruits..";
    for (QPoint f : _fruits)
        fruitPosition +=
            " " + QString::number(f.rx()) + " " + QString::number(f.ry());
    qDebug() << fruitPosition;
    _SendData(fruitPosition + ";r");
    _gameTimer->start(1000);
}

void Server::_timesUp()
{
    for (QMap<qintptr, QTcpSocket *>::Iterator it = _Sockets.begin();
         it != _Sockets.end(); it++)
    {
        _Data.clear();
        QDataStream out(&_Data, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_6_3);
        int minScore = -1;
        QString winner;
        int countWinners = 0;
        QString dataToSend =
            "h " + QString::number(_PlayerList[it.key()]->_crashed) + " " +
            _convertToString(_PlayerList[it.key()]->_homeDots) + ";";

        if (!_PlayerList[it.key()]->_crashed)
            winner = _PlayerList[it.key()]->_snakeName;

        for (QMap<qintptr, Snake *>::Iterator it1 = _PlayerList.begin();
             it1 != _PlayerList.end(); it1++)
        {
            if (it.key() == it1.key())
                continue;

            dataToSend += "g " + QString::number(it1.value()->_id) + " " +
                          QString::number(it1.value()->_crashed) + " " +
                          _convertToString(it1.value()->_homeDots) + ";";

            if (it1.value()->_homeDots.size() - 2 >= minScore)
            {
                winner = it1.value()->_snakeName;
                countWinners++;
                minScore = it1.value()->_homeDots.size() - 2;
            }
        }

        if (_type == 0)
            dataToSend += "e";

        else if (countWinners > 1)
            dataToSend += "d";

        else
            dataToSend += "w " + winner;

        qDebug() << dataToSend;
        out << quint16(0) << dataToSend;
        out.device()->seek(0);
        out << quint16(_Data.size() - sizeof(quint16));
        it.value()->write(_Data);
        it.value()->waitForBytesWritten();
    }

    for (QMap<qintptr, QTcpSocket *>::Iterator it = _ViewerList.begin();
         it != _ViewerList.end(); it++)
    {
        _Data.clear();
        QDataStream out(&_Data, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_6_3);
        int minScore = -1;
        QString dataToSend = "";
        QString winner;
        int countWinners = 0;

        for (QMap<qintptr, Snake *>::Iterator it1 = _PlayerList.begin();
             it1 != _PlayerList.end(); it1++)
        {
            dataToSend += "g " + QString::number(it1.value()->_id) + " " +
                          QString::number(it1.value()->_crashed) + " " +
                          _convertToString(it1.value()->_homeDots) + ";";

            if (it1.value()->_homeDots.size() - 2 >= minScore)
            {
                countWinners++;
                winner = it1.value()->_snakeName;
                minScore = it1.value()->_homeDots.size() - 2;
            }
        }

        if (_type == 0)
            dataToSend += "e";

        else if (countWinners > 1)
            dataToSend += "d";

        else
            dataToSend += "w " + winner;

        qDebug() << dataToSend;
        out << quint16(0) << dataToSend;
        out.device()->seek(0);
        out << quint16(_Data.size() - sizeof(quint16));
        it.value()->write(_Data);
        it.value()->waitForBytesWritten();
    }

    _gameTimer->stop();
    killTimer(_timer);
}

void Server::_endGame()
{
    for (QMap<qintptr, QTcpSocket *>::Iterator it = _Sockets.begin();
         it != _Sockets.end(); it++)
    {
        _Data.clear();
        QDataStream out(&_Data, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_6_3);
        QString winner;
        QString dataToSend =
            "h " + QString::number(_PlayerList[it.key()]->_crashed) + " " +
            _convertToString(_PlayerList[it.key()]->_homeDots) + ";";

        if (!_PlayerList[it.key()]->_crashed)
            winner = _PlayerList[it.key()]->_snakeName;

        for (QMap<qintptr, Snake *>::Iterator it1 = _PlayerList.begin();
             it1 != _PlayerList.end(); it1++)
        {
            if (it.key() == it1.key())
                continue;

            dataToSend += "g " + QString::number(it1.value()->_id) + " " +
                          QString::number(it1.value()->_crashed) + " " +
                          _convertToString(it1.value()->_homeDots) + ";";

            if (!it1.value()->_crashed)
                winner = it1.value()->_snakeName;
        }

        if (_type == 0)
            dataToSend += "e";

        else if (_crashed.size() == _PlayerList.size())
            dataToSend += "d";

        else
            dataToSend += "w " + winner;

        qDebug() << dataToSend;
        out << quint16(0) << dataToSend;
        out.device()->seek(0);
        out << quint16(_Data.size() - sizeof(quint16));
        it.value()->write(_Data);
        it.value()->waitForBytesWritten();
    }

    for (QMap<qintptr, QTcpSocket *>::Iterator it = _ViewerList.begin();
         it != _ViewerList.end(); it++)
    {
        _Data.clear();
        QDataStream out(&_Data, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_6_3);
        QString dataToSend = "";
        QString winner;

        for (QMap<qintptr, Snake *>::Iterator it1 = _PlayerList.begin();
             it1 != _PlayerList.end(); it1++)
        {
            dataToSend += "g " + QString::number(it1.value()->_id) + " " +
                          QString::number(it1.value()->_crashed) + " " +
                          _convertToString(it1.value()->_homeDots) + ";";

            if (!it1.value()->_crashed)
                winner = it1.value()->_snakeName;
        }

        if (_type == 0)
            dataToSend += "e";

        else if (_crashed.size() == _PlayerList.size())
            dataToSend += "d";

        else
            dataToSend += "w " + winner;

        qDebug() << dataToSend;
        out << quint16(0) << dataToSend;
        out.device()->seek(0);
        out << quint16(_Data.size() - sizeof(quint16));
        it.value()->write(_Data);
        it.value()->waitForBytesWritten();
    }

    _gameTimer->stop();
    killTimer(_timer);
}

void Server::_move()
{
    for (QMap<qintptr, Snake *>::Iterator it = _PlayerList.begin();
         it != _PlayerList.end(); it++)
    {
        if (!_crashed.contains(it.value()->_id))
        {
            for (size_t i = it.value()->_homeDots.size() - 1; i > 0; i--)
            {
                it.value()->_homeDots[i] = it.value()->_homeDots[i - 1];
            }

            switch (it.value()->direction)
            {
            case left:
                it.value()->_homeDots[0].rx()--;
                break;
            case right:
                it.value()->_homeDots[0].rx()++;
                break;
            case up:
                it.value()->_homeDots[0].ry()--;
                break;
            case down:
                it.value()->_homeDots[0].ry()++;
                break;
            }

            for (QMap<qintptr, Snake *>::Iterator es = _PlayerList.begin();
                 es != _PlayerList.end(); es++)
            {
                if (it.key() == es.key())
                    continue;

                es.value()->_enemiesDots[it.value()->_id] =
                    it.value()->_homeDots;
            }
        }
    }

    for (QMap<qintptr, Snake *>::Iterator it = _PlayerList.begin();
         it != _PlayerList.end(); it++)
    {
        qDebug() << it.value()->_enemiesDots;
    }
}

QString Server::_convertToString(QVector<QPoint> &dots)
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
    this->nextPendingConnection();
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
            if (_nextBlockSize == 0)
            {
                if (socket->bytesAvailable() < 2)
                    break;

                in >> _nextBlockSize;
            }

            if (socket->bytesAvailable() < _nextBlockSize)
                break;

            in >> str;
            _nextBlockSize = 0;
            QStringList L = str.split(";");

            qDebug() << L;

            for (QString &c : L)
            {
                QStringList l = c.split(' ');

                if (l[0] == 't')
                {
                    if (_type != l[1].toInt())
                    {
                        _SendClientBack(socket);
                        break;
                    }
                }

                if (l[0] == 'p')
                {
                    if (l[1] == "0" && _playerCount > 0)
                    {
                        _playerCount--;
                        _Sockets[socket->socketDescriptor()] = socket;
                        Snake *S = _Players.dequeue();
                        _PlayerList[socket->socketDescriptor()] = S;
                        S->_snakeName = l[2];
                        S->socket = socket;
                        qDebug()
                            << "Client connected" << socket->socketDescriptor();
                        this->nextPendingConnection();
                    }

                    else if (l[1] == "2" && _botCount > 0)
                    {
                        _botCount--;
                        _Sockets[socket->socketDescriptor()] = socket;
                        Snake *S = _Players.dequeue();
                        _PlayerList[socket->socketDescriptor()] = S;
                        S->_snakeName = l[2];
                        S->socket = socket;
                        qDebug()
                            << "Client connected" << socket->socketDescriptor();
                        this->nextPendingConnection();
                    }

                    else
                    {
                        _ViewerList[socket->socketDescriptor()] = socket;
                        this->nextPendingConnection();
                    }

                    if (_Sockets.size() == _totalCount && !_started)
                    {
                        _started = true;
                        _initiateGame();
                    }
                }

                if (l[0] == 'd')
                {
                    _PlayerList[socket->socketDescriptor()]->direction =
                        Directions(l[1].toInt());
                }
            }

            break;
        }
    }
    else
        qDebug() << "Error";
}

void Server::timer_function()
{
    _seconds--;
    qDebug() << _seconds << "Tik Tok";
}
