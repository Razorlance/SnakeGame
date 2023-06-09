#include "snakebot.h"

#include "./ui_snakebot.h"

SnakeBot::SnakeBot(QWidget *parent) : QMainWindow(parent), _ui(new Ui::SnakeBot)
{
    _ui->setupUi(this);
    setFixedSize(625, 625);
    _socket = new QTcpSocket(this);
    connect(_socket, &QTcpSocket::readyRead, this, &SnakeBot::slotReadyRead);
    connect(_socket, &QTcpSocket::disconnected, _socket,
            &QTcpSocket::deleteLater);
    this->resize(_WIDTH * _FIELD_WIDTH, _HEIGHT * _FIELD_HEIGHT);
    this->setWindowTitle("Snake Game");
    _startClient();
}

void SnakeBot::_startClient()
{
    _ui->timerLabel->setFixedWidth(100);
    _ui->player1Label->setFixedWidth(100);
    _ui->player2Label->setFixedWidth(100);
    _ui->player3Label->setFixedWidth(100);
    _ui->player4Label->setFixedWidth(100);

    QDialog *startWindow = new QDialog();
    startWindow->setFixedSize(330, 200);
    startWindow->setWindowTitle("Start Window");
    startWindow->adjustSize();
    startWindow->move(QGuiApplication::primaryScreen()->geometry().center() -
                      startWindow->rect().center());

    QFormLayout *form = new QFormLayout(startWindow);
    QComboBox *mode = new QComboBox(startWindow);
    QLineEdit *ip = new QLineEdit(startWindow);
    QLineEdit *port = new QLineEdit(startWindow);
    QLineEdit *name = new QLineEdit(startWindow);
    QComboBox *type = new QComboBox;
    QVector<QString> vec = {"Game Mode:", "IP:", "Port:"};
    QVector<QWidget *> widgetList = {mode, ip, port};

    QPushButton *button = new QPushButton("Start", startWindow);
    QObject::connect(button, &QPushButton::clicked, startWindow,
                     &QDialog::accept);

    QLabel *typeLabel = new QLabel("Game Type:");
    QLabel *nameLabel = new QLabel("Enter bot's name:");

    mode->addItem("Bot");

    ip->setText("127.0.0.1");
    port->setText("33221");

    type->addItem("Test Bot");
    type->addItem("1:Bot");
    type->addItem("Bot:Bot");
    type->addItem("Bot:3 Bots");

    name->setStyleSheet("QLineEdit { border-radius: 5px; }");
    ip->setStyleSheet("QLineEdit { border-radius: 5px; }");
    port->setStyleSheet("QLineEdit { border-radius: 5px; }");

    for (int i = 0; i < vec.length(); i++)
    {
        QLabel *label = new QLabel(vec[i]);
        QFont font = label->font();
        font.setPointSize(15);
        font.setBold(true);
        font.setFamily("Copperplate");
        label->setFont(font);
        form->addRow(label, widgetList[i]);
    }

    QFont font = nameLabel->font();
    font.setPointSize(15);
    font.setBold(true);
    font.setFamily("Copperplate");
    nameLabel->setFont(font);
    nameLabel->setStyleSheet("QLabel { color : black; }");
    typeLabel->setFont(font);
    typeLabel->setStyleSheet("QLabel { color : black; }");
    form->addRow(nameLabel, name);
    form->addRow(typeLabel, type);
    form->addWidget(button);

    _validName(name, button);
    _validIP(ip, button);
    _validPort(port, button);

    if (startWindow->exec() == QDialog::Rejected)
    {
        qDebug() << "Must be stopped";
        stop = true;
        return;
    }

    _snakeName = name->text();
    _mode = mode->currentText();
    _ip = ip->text();
    _port = port->text().toInt();

    if (_snakeName.size() == 0)
        _snakeName = "Bot";

    if (type->currentText() == "Test Bot")
        _type = 0;

    else if (type->currentText() == "1:Bot")
        _type = 3;

    else if (type->currentText() == "Bot:Bot")
        _type = 4;

    else if (type->currentText() == "Bot:3 Bots")
        _type = 5;

    _ui->player1Label->setStyleSheet("QLabel { color : blue; }");
    _ui->player2Label->setStyleSheet("QLabel { color : red; }");
    _ui->player3Label->setStyleSheet("QLabel { color : green; }");
    _ui->player4Label->setStyleSheet("QLabel { color : orange; }");
    _nextBlockSize = 0;
    qDebug() << "Connecting to server...";
    _connectToServer();
}

void SnakeBot::_validName(QLineEdit *name, QPushButton *button)
{
    connect(name, &QLineEdit::textChanged, this,
            [this, name, button](const QString &text) {
                if (text.length() > 7 || text.count(" ") >= 1)
                {
                    // If the length of the text exceeds 7, set the text color
                    // to red
                    name->setStyleSheet("QLineEdit { color: red; }");
                    button->setEnabled(false);
                }

                else
                {
                    // If the length of the text is 7 or less, set the text
                    // color back to white
                    name->setStyleSheet("QLineEdit { color: black; }");
                    button->setEnabled(true);
                }
            });
}

void SnakeBot::_validIP(QLineEdit *ip, QPushButton *button)
{
    connect(ip, &QLineEdit::textChanged, this,
            [this, ip, button](const QString &text) {
                if (!_validIP(text))
                {
                    ip->setStyleSheet("QLineEdit { color: red; }");
                    button->setEnabled(false);
                }
                else
                {
                    ip->setStyleSheet("QLineEdit { color: black; }");
                    button->setEnabled(true);
                }
            });
}

void SnakeBot::_validPort(QLineEdit *port, QPushButton *button)
{
    connect(port, &QLineEdit::textChanged, this,
            [this, port, button](const QString &text) {
                if (!_validPort(text))
                {
                    port->setStyleSheet("QLineEdit { color: red; }");
                    button->setEnabled(false);
                }
                else
                {
                    port->setStyleSheet("QLineEdit { color: black; }");
                    button->setEnabled(true);
                }
            });
}

bool SnakeBot::_validIP(const QString &ip)
{
    QVector<QString> v = ip.split('.');

    if ((v.size() != 4) || (v[3] == ""))
        return false;

    for (QString &str : v)
        if (str.toInt() > 255 || str.toInt() < 0 || not(_isNumber(str)))
            return false;

    return true;
}

bool SnakeBot::_validPort(const QString &port)
{
    for (const QChar &i : port)
    {
        if (i.isLetter())
            return false;
    }

    if (port.toInt() >= 0 && port.toInt() <= 65535)
        return true;

    return false;
}

bool SnakeBot::_isNumber(const QString &str)
{
    if (str == "0")
        return true;

    for (const QChar &i : str)
    {
        if (!i.isDigit())
            return false;
    }

    return true;
}

SnakeBot::~SnakeBot() { delete _ui; }

void SnakeBot::_connectToServer()
{
    qDebug() << _ip << _port << _snakeName;
    _socket->connectToHost(_ip, _port);

    _data.clear();
    QDataStream out(&_data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);

    // 0 - player
    // 1 - viewer
    // 2 - bot

    QString dataToSend = "t " + QString::number(_type) + ";p 2 " + _snakeName;
    qDebug() << dataToSend;

    out << quint16(0) << dataToSend;
    out.device()->seek(0);
    out << quint16(_data.size() - sizeof(quint16));
    //    _socket->waitForBytesWritten();
    _socket->write(_data);
}

void SnakeBot::_sendToServer()
{
    _data.clear();
    QDataStream out(&_data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);
    QString dataToSend = "d " + QString::number(_direction);
    qDebug() << dataToSend;
    out << quint16(0) << dataToSend;
    out.device()->seek(0);
    out << quint16(_data.size() - sizeof(quint16));
    //    _socket->waitForBytesWritten();
    _socket->write(_data);
}

void SnakeBot::slotReadyRead()
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
            //            _socket->waitForBytesWritten();

            if (_input == "wrong")
                _wrongServer();

            _nextBlockSize = 0;
            qDebug() << _input;
            QStringList commandList = _input.split(';');

            for (QString &c : commandList)
            {
                QStringList l = c.split(' ');

                if (l[0] == 'e')
                {
                    _gameOver();
                    break;
                }

                if (l[0] == 'd')
                {
                    _noWinner();
                    break;
                }

                if (l[0] == 'w')
                {
                    _oneWinner(l[1]);
                    break;
                }

                if (l[0] == 's')
                {
                    _ui->timerLabel->setText(l[1]);
                }

                if (l[0] == 'f')
                {
                    _fruits = _convertFruits(l);
                }

                if (l[0] == 'c')
                {
                    qDebug() << "Await is true";
                    _await = l[1].toInt();
                }

                if (l[0] == 'g')
                {
                    if (l[2] == '1')
                        _enemiesCrashed[l[1].toInt()] = 1;

                    if (!_enemiesCrashed[l[1].toInt()])
                    {
                        QVector<QPoint> newDots = _convertEnemyDots(l);

                        if (newDots.size() > _enemiesDots[l[1].toInt()].size())
                        {
                            if (l[1].toInt() == 1)
                            {
                                QString text = _ui->player1Label->text();
                                QStringList parts = text.split(":");
                                _ui->player1Label->setText(
                                    parts[0] + ":" +
                                    QString::number(newDots.size() - 2));
                            }

                            if (l[1].toInt() == 2)
                            {
                                QString text = _ui->player2Label->text();
                                QStringList parts = text.split(":");
                                _ui->player2Label->setText(
                                    parts[0] + ":" +
                                    QString::number(newDots.size() - 2));
                            }

                            if (l[1].toInt() == 3)
                            {
                                QString text = _ui->player3Label->text();
                                QStringList parts = text.split(":");
                                _ui->player3Label->setText(
                                    parts[0] + ":" +
                                    QString::number(newDots.size() - 2));
                            }

                            if (l[1].toInt() == 4)
                            {
                                QString text = _ui->player4Label->text();
                                QStringList parts = text.split(":");
                                _ui->player4Label->setText(
                                    parts[0] + ":" +
                                    QString::number(newDots.size() - 2));
                            }
                        }

                        _enemiesDots[l[1].toInt()] = newDots;
                    }
                }

                if (l[0] == 'h')
                {
                    if (l[1] == '1')
                        _crashed = 1;

                    if (!_crashed)
                    {
                        QVector<QPoint> newDots = _convertHomeDots(l);

                        if (newDots.size() > _homeDots.size())
                        {
                            if (_id == 1)
                            {
                                QString text = _ui->player1Label->text();
                                QStringList parts = text.split(":");
                                _ui->player1Label->setText(
                                    parts[0] + ":" +
                                    QString::number(newDots.size() - 2));
                            }

                            if (_id == 2)
                            {
                                QString text = _ui->player2Label->text();
                                QStringList parts = text.split(":");
                                _ui->player2Label->setText(
                                    parts[0] + ":" +
                                    QString::number(newDots.size() - 2));
                            }

                            if (_id == 3)
                            {
                                QString text = _ui->player3Label->text();
                                QStringList parts = text.split(":");
                                _ui->player3Label->setText(
                                    parts[0] + ":" +
                                    QString::number(newDots.size() - 2));
                            }

                            if (_id == 4)
                            {
                                QString text = _ui->player4Label->text();
                                QStringList parts = text.split(":");
                                _ui->player4Label->setText(
                                    parts[0] + ":" +
                                    QString::number(newDots.size() - 2));
                            }
                        }

                        _homeDots = newDots;
                    }
                }

                if (l[0] == 'r')
                {
                    if (l[1].toInt() == 1)
                    {
                        _id = 1;
                        _direction = right;
                        _colour = blue;
                        _ui->player1Label->setText(_snakeName + ":0");
                    }

                    else if (l[1].toInt() == 2)
                    {
                        _id = 2;
                        _direction = left;
                        _colour = red;
                        _ui->player2Label->setText(_snakeName + ":0");
                    }

                    else if (l[1].toInt() == 3)
                    {
                        _id = 3;
                        _direction = up;
                        _colour = green;
                        _ui->player3Label->setText(_snakeName + ":0");
                    }

                    else if (l[1].toInt() == 4)
                    {
                        _id = 4;
                        _direction = down;
                        _colour = magenta;
                        _ui->player4Label->setText(_snakeName + ":0");
                    }

                    _stillGame = l[1].toInt();
                }

                if (l[0] == 'n')
                {
                    // Fix enemy name splitted into spaces

                    if (l[1].toInt() == 1)
                        _ui->player1Label->setText(l[2] + ":0");

                    else if (l[1].toInt() == 2)
                        _ui->player2Label->setText(l[2] + ":0");

                    if (l[1].toInt() == 3)
                        _ui->player3Label->setText(l[2] + ":0");

                    else if (l[1].toInt() == 4)
                        _ui->player4Label->setText(l[2] + ":0");
                }
            }
            _step();
            break;
        }
    }
    else
        qDebug() << "Error";
}

void SnakeBot::_drawSnake()
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
            if (!_crashed && _colour == blue)
                painter.setBrush(Qt::darkBlue);

            else if (!_crashed && _colour == red)
                painter.setBrush(Qt::darkRed);

            else if (!_crashed && _colour == green)
                painter.setBrush(Qt::darkGreen);

            else if (!_crashed && _colour == magenta)
                painter.setBrush(Qt::darkMagenta);

            else
                painter.setBrush(Qt::darkGray);

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
                if (it.key() == 1 && !_enemiesCrashed[it.key()])
                    painter.setBrush(Qt::darkBlue);

                else if (it.key() == 2 && !_enemiesCrashed[it.key()])
                    painter.setBrush(Qt::darkRed);

                else if (it.key() == 3 && !_enemiesCrashed[it.key()])
                    painter.setBrush(Qt::darkGreen);

                else if (it.key() == 4 && !_enemiesCrashed[it.key()])
                    painter.setBrush(Qt::darkMagenta);

                else
                    painter.setBrush(Qt::darkGray);

                painter.drawEllipse(it.value()[i].x() * _WIDTH,
                                    it.value()[i].y() * _HEIGHT, _WIDTH,
                                    _HEIGHT);
            }
        }
    }
}

bool SnakeBot::_checkMove(Directions tmp)
{
    if (tmp == up && _homeDots[0].y() == 0)
        return false;

    if (tmp == down && _homeDots[0].y() == _FIELD_HEIGHT - 1)
        return false;

    if (tmp == left && _homeDots[0].x() == 0)
        return false;

    if (tmp == right && _homeDots[0].x() == _FIELD_WIDTH - 1)
        return false;

    if (tmp == right)
    {
        QPoint p(_homeDots[0].x() + 1, _homeDots[0].y());
        for (size_t i = 1; i < _homeDots.size(); i++)
            if (p == _homeDots[i])
                return false;

        for (auto snake : _enemiesDots)
            for (size_t i = 0; i < snake.size(); i++)
                if (p == snake[i])
                    return false;
    }

    if (tmp == left)
    {
        QPoint p(_homeDots[0].x() - 1, _homeDots[0].y());
        for (size_t i = 1; i < _homeDots.size(); i++)
            if (p == _homeDots[i])
                return false;

        for (auto snake : _enemiesDots)
            for (size_t i = 0; i < snake.size(); i++)
                if (p == snake[i])
                    return false;
    }

    if (tmp == up)
    {
        QPoint p(_homeDots[0].x(), _homeDots[0].y() - 1);
        for (size_t i = 1; i < _homeDots.size(); i++)
            if (p == _homeDots[i])
                return false;

        for (auto snake : _enemiesDots)
            for (size_t i = 0; i < snake.size(); i++)
                if (p == snake[i])
                    return false;
    }

    if (tmp == down)
    {
        QPoint p(_homeDots[0].x(), _homeDots[0].y() + 1);
        for (size_t i = 1; i < _homeDots.size(); i++)
            if (p == _homeDots[i])
                return false;

        for (auto snake : _enemiesDots)
            for (size_t i = 0; i < snake.size(); i++)
                if (p == snake[i])
                    return false;
    }

    return true;
}

bool SnakeBot::_bot()
{
    Directions dir;
    size_t length = abs(_homeDots[0].x() - _fruits[0].x()) +
                    abs(_homeDots[0].y() - _fruits[0].y());
    QPoint goal = _fruits[0];

    for (size_t i = 1; i < _fruits.size(); i++)
    {
        if (length > abs(_homeDots[0].x() - _fruits[i].x()) +
                         abs(_homeDots[0].y() - _fruits[i].y()))
        {
            goal = _fruits[i];
            length = abs(_homeDots[0].x() - _fruits[i].x()) +
                     abs(_homeDots[0].y() - _fruits[i].y());
        }
    }

    if (_homeDots[0].x() == goal.x())
    {
        dir = (_homeDots[0].y() > goal.y() ? up : down);
        if (!_checkMove(dir))
        {
            if (_checkMove(left))
                dir = left;

            else if (_checkMove(dir == down ? up : down))
                dir = (dir == down ? up : down);

            else if (_checkMove(right))
                dir = right;
        }
    }

    else if (_homeDots[0].y() == goal.y())
    {
        dir = (_homeDots[0].x() > goal.x() ? left : right);
        if (!_checkMove(dir))
        {
            if (_checkMove(up))
                dir = up;

            else if (_checkMove(dir == left ? right : left))
                dir = (dir == left ? right : left);

            else if (_checkMove(down))
                dir = down;
        }
    }

    else
    {
        dir = (_homeDots[0].x() > goal.x() ? left : right);
        if (!_checkMove(dir))
        {
            Directions tmp = _homeDots[0].y() > goal.y() ? up : down;

            if (_checkMove(tmp))
                dir = tmp;

            else if (_checkMove(tmp == up ? down : up))
                dir = (tmp == up ? down : up);

            else if (_checkMove(dir == left ? right : left))
                dir = (dir == left ? right : left);
        }
    }

    if (!_checkMove(dir))
        return false;

    else if (dir != _direction)
    {
        _direction = dir;
        return true;
    }

    else
        return false;
}

void SnakeBot::_step()
{
    if (_stillGame & _await)
    {
        _await = false;
        this->repaint();

        if (_bot())
            _sendToServer();
    }
}

void SnakeBot::_gameOver()
{
    this->repaint();
    QMessageBox endOfGame;
    endOfGame.setText("Game Over!");
    endOfGame.exec();
    this->close();
}

void SnakeBot::_noWinner()
{
    this->repaint();
    QMessageBox endOfGame;
    endOfGame.setText("The Game Ended With A Draw!");
    endOfGame.exec();
    this->close();
}

void SnakeBot::_oneWinner(const QString &winner)
{
    this->repaint();
    QMessageBox endOfGame;
    endOfGame.setText("Game Over. The Winner Is " + winner + "!");
    endOfGame.exec();
    this->close();
}

void SnakeBot::_wrongServer()
{
    QMessageBox endOfGame;
    endOfGame.setText("You have chosen the wrong server");
    endOfGame.exec();
    QApplication::exit(1234);
}

void SnakeBot::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    _drawSnake();
}

QVector<QPoint> SnakeBot::_convertFruits(const QStringList &str)
{
    QVector<QPoint> dots;
    for (size_t i = 1; i < str.size(); i += 2)
        dots.push_back(QPoint(str[i].toInt(), str[i + 1].toInt()));
    return dots;
}

QVector<QPoint> SnakeBot::_convertHomeDots(const QStringList &str)
{
    QVector<QPoint> dots;
    for (size_t i = 2; i < str.size(); i += 2)
        dots.push_back(QPoint(str[i].toInt(), str[i + 1].toInt()));
    return dots;
}

QVector<QPoint> SnakeBot::_convertEnemyDots(const QStringList &str)
{
    QVector<QPoint> dots;
    for (size_t i = 3; i < str.size(); i += 2)
        dots.push_back(QPoint(str[i].toInt(), str[i + 1].toInt()));
    return dots;
}
