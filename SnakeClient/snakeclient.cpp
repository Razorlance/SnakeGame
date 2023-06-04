#include "snakeclient.h"

#include "./ui_snakeclient.h"

/*
 * TODO:
 * 1. Think about the boundaries of the window. Disallow the change of size of
 * the window maybe?
 * 2. Read about endPaint().
 * 3. Close endOfGame with MainWindow.
 * 4. Make so that fruits do not spawn under the snake.
 * 5. Make the win situation.
 * 6. Improve label position.
 * 7. Update score on the label.
 * 8. Improve move. Do not allow it to go up-down or left-right when it's going
 * down-up or right-left.
 * 9. Remove all setLabel, setWindow etc. from constructor
 */

SnakeClient::SnakeClient(QWidget* parent)
    : QMainWindow(parent), _ui(new Ui::SnakeClient)
{
    _ui->setupUi(this);
    _socket = new QTcpSocket(this);
    connect(_socket, &QTcpSocket::readyRead, this, &SnakeClient::slotReadyRead);
    connect(_socket, &QTcpSocket::disconnected, _socket,
            &QTcpSocket::deleteLater);
    this->resize(_WIDTH * _FIELD_WIDTH, _HEIGHT * _FIELD_HEIGHT);
    this->setWindowTitle("Snake Game");
    _startClient();
}
SnakeClient::~SnakeClient() { delete _ui; }

void SnakeClient::_startClient()
{
    QDialog* startWindow = new QDialog();

    startWindow->setWindowTitle("Start Window");
    startWindow->adjustSize();
    startWindow->move(QGuiApplication::primaryScreen()->geometry().center() -
                      startWindow->rect().center());

    QFormLayout* form = new QFormLayout(startWindow);
    QLineEdit* name = new QLineEdit(startWindow);
    QComboBox* mode = new QComboBox(startWindow);
    QLineEdit* ip = new QLineEdit(startWindow);
    QLineEdit* port = new QLineEdit(startWindow);

    ip->setText("127.0.0.1");
    port->setText("33221");

    mode->addItem("Player");
    mode->addItem("Viewer");

    form->addRow("Enter your name:", name);
    form->addRow("Enter your mode of the game:", mode);
    form->addRow("IP:", ip);
    form->addRow("Port:", port);

    QPushButton* button = new QPushButton("OK", startWindow);
    // QPushButton *buttonCancel = new QPushButton("Cancel", startWindow);
    QObject::connect(button, &QPushButton::clicked, startWindow,
                     &QDialog::accept);
    form->addWidget(button);

    if (startWindow->exec() == QDialog::Accepted)
    {
        _snakeName = name->text();
        _mode = mode->currentText();
        _ip = ip->text();
        _port = port->text().toInt();

        // int numPlayersInt = numPlayers->value();
        // QString::number(input3Value)); //if string of # players will be
        // needed
        if (_snakeName.size() == 0)
            _snakeName = "Player";

        if (_mode == "Player")
        {
            _viewer = 0;

            QDialog* gameType = new QDialog();
            gameType->setWindowTitle("Game Type");
            gameType->adjustSize();
            gameType->move(
                QGuiApplication::primaryScreen()->geometry().center() -
                gameType->rect().center());
            QFormLayout* form2 = new QFormLayout(gameType);
            QComboBox* type = new QComboBox(gameType);
            type->addItem("1:1");
            type->addItem("1:3");
            type->addItem("1:Bot");
            type->addItem("Bot:Bot");
            form2->addRow("Choose Game Type", type);
            QPushButton* buttonOK = new QPushButton("OK", gameType);
            // QPushButton *buttonCancel = new QPushButton("Cancel",
            // startWindow);
            QObject::connect(buttonOK, &QPushButton::clicked, gameType,
                             &QDialog::accept);
            form2->addWidget(buttonOK);
            gameType->exec();

            _snakeName = name->text();
            _mode = mode->currentText();
            _ip = ip->text();
            _port = port->text().toInt();

            if (type->currentText() == "1:1")
                _type = 1;

            else if (type->currentText() == "1:3")
                _type = 3;

            else if (type->currentText() == "1:Bot")
                _type = 4;

            else if (type->currentText() == "Bot:Bot")
                _type = 5;

            if (_snakeName.size() == 0)
                _snakeName = "Player";

            _ui->player1Label->setStyleSheet("QLabel { color : blue; }");
            _ui->player2Label->setStyleSheet("QLabel { color : red; }");
            _ui->player3Label->setStyleSheet("QLabel { color : green; }");
            _ui->player4Label->setStyleSheet("QLabel { color : orange; }");
            _nextBlockSize = 0;
            qDebug() << "Connecting to server...";
            connectToServer();
        }
        else
        {
            _viewer = 1;
        }
    }
}

void SnakeClient::connectToServer()
{
    qDebug() << _ip << _port << _viewer << _snakeName;
    _socket->connectToHost(_ip, _port);

    _data.clear();
    QDataStream out(&_data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);
    // 0 - player
    // 1 -  viewer
    // 2 - bot
    QString dataToSend = "t " + QString::number(_type) + ";p " +
                         QString::number(_viewer) + " " + _snakeName;
    qDebug() << dataToSend;

    out << quint16(0) << dataToSend;
    out.device()->seek(0);
    out << quint16(_data.size() - sizeof(quint16));
    _socket->waitForBytesWritten();
    _socket->write(_data);
}

void SnakeClient::_sendToServer()
{
    _data.clear();
    QDataStream out(&_data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);
    QString dataToSend = "d " + QString::number(_direction);
    qDebug() << dataToSend;
    out << quint16(0) << dataToSend;
    out.device()->seek(0);
    out << quint16(_data.size() - sizeof(quint16));
    _socket->waitForBytesWritten();
    _socket->write(_data);
}

void SnakeClient::slotReadyRead()
{
    QDataStream in(_socket);
    if (in.status() == QDataStream::Ok)
    {
        qDebug() << "Reading...";

        while (true)
        {
            if (_nextBlockSize == 0)
            {
                if (_socket->bytesAvailable() < 2)
                    break;

                in >> _nextBlockSize;
            }

            if (_socket->bytesAvailable() < _nextBlockSize)
                break;

            in >> _input;
            _socket->waitForBytesWritten();

            if (_input == "wrong")
                _wrongServer();

            _nextBlockSize = 0;
            qDebug() << _input;
            QStringList commandList = _input.split(';');

            for (QString& c : commandList)
            {
                QStringList l = c.split(' ');

                if (l[0] == 'e')
                    _gameOver();

                if (l[0] == 'f')
                {
                    //                    _fruitPos.rx() = l[1].toInt();
                    //                    _fruitPos.ry() = l[2].toInt();
                    _fruits = _convertHomeDots(l);
                }

                if (l[0] == 'c')
                {
                    qDebug() << "Await is true";
                    _await = l[1].toInt();
                }

                if (!_viewer)
                {
                    if (l[0] == 'g')
                    {
                        _enemiesDots[l[1].toInt()] = _convertEnemyDots(l);
                    }

                    if (l[0] == 'h')
                    {
                        _homeDots = _convertHomeDots(l);
                    }

                    if (l[0] == 'r')
                    {
                        if (l[1].toInt() == 1)
                        {
                            _direction = right;
                            _colour = blue;
                            _ui->player1Label->setText(_snakeName);
                        }

                        else if (l[1].toInt() == 2)
                        {
                            _direction = left;
                            _colour = red;
                            _ui->player2Label->setText(_snakeName);
                        }

                        else if (l[1].toInt() == 3)
                        {
                            _direction = up;
                            _colour = green;
                            _ui->player3Label->setText(_snakeName);
                        }

                        else if (l[1].toInt() == 4)
                        {
                            _direction = down;
                            _colour = magenta;
                            _ui->player4Label->setText(_snakeName);
                        }

                        _stillGame = l[1].toInt();
                    }

                    if (l[0] == 'n')
                    {
                        // Fix enemy name splitted into spaces

                        if (l[1].toInt() == 1)
                            _ui->player1Label->setText(l[2]);

                        else if (l[1].toInt() == 2)
                            _ui->player2Label->setText(l[2]);

                        else if (l[1].toInt() == 3)
                            _ui->player3Label->setText(l[2]);

                        else if (l[1].toInt() == 4)
                            _ui->player4Label->setText(l[2]);
                    }
                }

                else
                {
                    if (l[0] == 'n')
                    {
                        // Fix enemy name splitted into spaces

                        if (l[1].toInt() == 1)
                            _ui->player1Label->setText(l[2]);

                        else if (l[1].toInt() == 2)
                            _ui->player2Label->setText(l[2]);

                        else if (l[1].toInt() == 3)
                            _ui->player3Label->setText(l[2]);

                        else if (l[1].toInt() == 4)
                            _ui->player4Label->setText(l[2]);
                    }

                    if (l[0] == 'g')
                    {
                        _enemiesDots[l[1].toInt()] = _convertEnemyDots(l);
                    }
                    // Fix _stillGame place in the code
                    _stillGame = true;
                }
            }
            _step();
            break;
        }
    }
    else
        qDebug() << "Error";
}

void SnakeClient::keyPressEvent(QKeyEvent* event)
{
    if (!_viewer)
    {
        int key = event->key();

        if ((key == Qt::Key_Left || key == Qt::Key_A) && _direction != right)
            _direction = left;

        if ((key == Qt::Key_Right || key == Qt::Key_D) && _direction != left)
            _direction = right;

        if ((key == Qt::Key_Up || key == Qt::Key_W) && _direction != down)
            _direction = up;

        if ((key == Qt::Key_Down || key == Qt::Key_S) && _direction != up)
            _direction = down;

        _sendToServer();
    }
}

void SnakeClient::_drawSnake()
{
    QPainter painter(this);
    for (QPoint f : _fruits)
    {
        painter.setBrush(Qt::red);
        painter.drawEllipse(f.x() * _WIDTH, f.y() * _HEIGHT, _WIDTH, _HEIGHT);
    }
    for (size_t i = 0; i < _homeDots.size(); i++)
    {
        if (i == 0)
        {
            painter.setBrush(Qt::lightGray);
            painter.drawEllipse(_homeDots[i].x() * _WIDTH,
                                _homeDots[i].y() * _HEIGHT, _WIDTH, _HEIGHT);
        }

        else
        {
            if (_colour == blue)
                painter.setBrush(Qt::darkBlue);

            else if (_colour == red)
                painter.setBrush(Qt::darkRed);

            else if (_colour == green)
                painter.setBrush(Qt::darkGreen);

            else if (_colour == magenta)
                painter.setBrush(Qt::darkMagenta);

            painter.drawEllipse(_homeDots[i].x() * _WIDTH,
                                _homeDots[i].y() * _HEIGHT, _WIDTH, _HEIGHT);
        }
    }

    for (QMap<int, QVector<QPoint>>::Iterator it = _enemiesDots.begin();
         it != _enemiesDots.end(); it++)
    {
        for (size_t i = 0; i < it.value().size(); i++)
        {
            if (i == 0)
            {
                painter.setBrush(Qt::white);
                painter.drawEllipse(it.value()[i].x() * _WIDTH,
                                    it.value()[i].y() * _HEIGHT, _WIDTH,
                                    _HEIGHT);
            }

            else
            {
                if (it.key() == 1)
                    painter.setBrush(Qt::darkBlue);

                else if (it.key() == 2)
                    painter.setBrush(Qt::darkRed);

                else if (it.key() == 3)
                    painter.setBrush(Qt::darkGreen);

                else if (it.key() == 4)
                    painter.setBrush(Qt::darkMagenta);

                painter.drawEllipse(it.value()[i].x() * _WIDTH,
                                    it.value()[i].y() * _HEIGHT, _WIDTH,
                                    _HEIGHT);
            }
        }
    }
}

void SnakeClient::_step()
{
    qDebug() << "Tick" << _enemyDirection;
    if (_stillGame & _await)
    {
        _await = false;
        this->repaint();
    }
}

void SnakeClient::_gameOver()
{
    QMessageBox endOfGame;
    endOfGame.setText("Game Over");
    endOfGame.exec();
    this->close();
}

void SnakeClient::_wrongServer()
{
    QMessageBox wrongServer;
    wrongServer.setText("Choose another server or game type");
    wrongServer.exec();
    this->hide();
    _startClient();
}

void SnakeClient::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    _drawSnake();
}

QVector<QPoint> SnakeClient::_convertHomeDots(const QStringList& str)
{
    QVector<QPoint> dots;
    for (size_t i = 1; i < str.size(); i += 2)
        dots.push_back(QPoint(str[i].toInt(), str[i + 1].toInt()));
    return dots;
}

QVector<QPoint> SnakeClient::_convertEnemyDots(const QStringList& str)
{
    QVector<QPoint> dots;
    for (size_t i = 2; i < str.size(); i += 2)
        dots.push_back(QPoint(str[i].toInt(), str[i + 1].toInt()));
    return dots;
}
