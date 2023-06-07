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
    setFixedSize(625, 625);
    _socket = new QTcpSocket(this);
    connect(_socket, &QTcpSocket::readyRead, this, &SnakeClient::slotReadyRead);
    connect(_socket, &QTcpSocket::disconnected, _socket,
            &QTcpSocket::deleteLater);
    this->resize(_WIDTH * _FIELD_WIDTH, _HEIGHT * _FIELD_HEIGHT);
    this->setWindowTitle("Snake Game");
    _startClient();
}

void SnakeClient::_startClient()
{
    _ui->player1Label->setFixedWidth(100);
    _ui->player2Label->setFixedWidth(100);
    _ui->player3Label->setFixedWidth(100);
    _ui->player4Label->setFixedWidth(100);

    QDialog* startWindow = new QDialog();
    startWindow->setFixedSize(330, 200);
    startWindow->setWindowTitle("Start Window");
    startWindow->adjustSize();
    startWindow->move(QGuiApplication::primaryScreen()->geometry().center() - startWindow->rect().center());

    QFormLayout *form = new QFormLayout(startWindow);
    QComboBox* mode = new QComboBox(startWindow);
    QLineEdit* ip = new QLineEdit(startWindow);
    QLineEdit* port = new QLineEdit(startWindow);
    QLineEdit* name = new QLineEdit(startWindow);
    QComboBox *type = new QComboBox;
    QVector<QString> vec = {"Game Mode:", "IP:", "Port:"};
    QVector<QWidget*> widgetList = {mode, ip, port};

    QPushButton *button = new QPushButton("Start", startWindow);
    QObject::connect(button, &QPushButton::clicked, startWindow, &QDialog::accept);

    QLabel* typeLabel = new QLabel("Game Type:");
    QLabel* nameLabel = new QLabel("Enter bot's name:");

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

    // Doesn't stop after closing

    _validName(name, button);
    _validIP(ip, button);
    _validPort(port, button);

    if (startWindow->exec() == QDialog::Rejected)
    {
        QApplication::quit();
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

void SnakeClient::_validName(QLineEdit *name, QPushButton *button)
{
    connect(name, &QLineEdit::textChanged, this, [this, name, button](const QString &text)
    {
        if (text.length() > 7 || text.count(" ") >= 1)
        {
            // If the length of the text exceeds 7, set the text color to red
            name->setStyleSheet("QLineEdit { color: red; }");
            button->setEnabled(false);
        }

        else
        {
            // If the length of the text is 7 or less, set the text color back to white
            name->setStyleSheet("QLineEdit { color: black; }");
            button->setEnabled(true);
        }
    });
}

void SnakeClient::_validIP(QLineEdit *ip, QPushButton *button)
{
    connect(ip, &QLineEdit::textChanged, this, [this, ip, button](const QString &text)
    {
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

void SnakeClient::_validPort(QLineEdit *port, QPushButton *button)
{
    connect(port, &QLineEdit::textChanged, this, [this, port, button](const QString &text)
    {
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

bool SnakeClient::_validIP(const QString &ip)
{
    QVector<QString> v = ip.split('.');

    if ((v.size() != 4) || (v[3] == ""))
        return false;

    for (QString& str : v)
        if (str.toInt() > 255 || str.toInt() < 0 || not(_isNumber(str)))
            return false;

    return true;
}

bool SnakeClient::_validPort(const QString &port)
{
    for (const QChar& i : port)
    {
        if (i.isLetter())
            return false;
    }

    if (port.toInt() >= 0 && port.toInt() <= 65535)
        return true;

    return false;
}

bool SnakeClient::_isNumber(const QString &str)
{
    if (str == "0")
        return true;

    for (const QChar& i : str)
    {
        if (!i.isDigit())
            return false;
    }

    return true;
}

SnakeClient::~SnakeClient() { delete _ui; }

void SnakeClient::_connectToServer()
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
                        _enemiesDots[l[1].toInt()] = _convertEnemyDots(l);
                    }
                }

                if (l[0] == 'h')
                {
                    if (l[1] == '1')
                        _crashed = 1;

                    if (!_crashed)
                        _homeDots = _convertHomeDots(l);
                }

                if (l[0] == 'r')
                {
                    if (l[1].toInt() == 1)
                    {
                        _direction = right;
                        _colour = blue;
                        _ui->player1Label->setText(_snakeName + ": 0");
                    }

                    else if (l[1].toInt() == 2)
                    {
                        _direction = left;
                        _colour = red;
                        _ui->player2Label->setText(_snakeName + ": 0");
                    }

                    else if (l[1].toInt() == 3)
                    {
                        _direction = up;
                        _colour = green;
                        _ui->player3Label->setText(_snakeName + ": 0");
                    }

                    else if (l[1].toInt() == 4)
                    {
                        _direction = down;
                        _colour = magenta;
                        _ui->player4Label->setText(_snakeName + ": 0");
                    }

                    _stillGame = l[1].toInt();
                    _numberOfPlayers++;
                }

                if (l[0] == 'n')
                {
                    // Fix enemy name splitted into spaces

                    if (l[1].toInt() == 1)
                        _ui->player1Label->setText(l[2] + ": 0");

                    else if (l[1].toInt() == 2)
                        _ui->player2Label->setText(l[2] + ": 0");

                    if (l[1].toInt() == 3)
                        _ui->player3Label->setText(l[2] + ": 0");

                    else if (l[1].toInt() == 4)
                        _ui->player4Label->setText(l[2] + ": 0");

                    _numberOfPlayers++;
                }
            }
            _step();
            break;
        }
    }
    else
        qDebug() << "Error";
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

void SnakeClient::_step()
{
    if (_stillGame & _await)
    {
        _await = false;

        if (_type == 0 || _numberOfPlayers > 1)
            this->repaint();
    }
}

void SnakeClient::_gameOver()
{
    this->repaint();
    QMessageBox endOfGame;
    endOfGame.setText("Game Over!");
    endOfGame.exec();
    this->close();
}

void SnakeClient::_noWinner()
{
    this->repaint();
    QMessageBox endOfGame;
    endOfGame.setText("The Game Ended With A Draw!");
    endOfGame.exec();
    this->close();
}

void SnakeClient::_oneWinner(const QString& winner)
{
    this->repaint();
    QMessageBox endOfGame;
    endOfGame.setText("Game Over. The Winner Is " + winner + "!");
    endOfGame.exec();
    this->close();
}

void SnakeClient::_wrongServer()
{
    QMessageBox endOfGame;
    endOfGame.setText("You have chosen the wrong server");
    endOfGame.exec();
    this->close();
}

void SnakeClient::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    _drawSnake();
}

QVector<QPoint> SnakeClient::_convertFruits(const QStringList &str)
{
    QVector<QPoint> dots;
    for (size_t i = 1; i < str.size(); i += 2)
        dots.push_back(QPoint(str[i].toInt(), str[i + 1].toInt()));
    return dots;
}

QVector<QPoint> SnakeClient::_convertHomeDots(const QStringList& str)
{
    QVector<QPoint> dots;
    for (size_t i = 2; i < str.size(); i += 2)
        dots.push_back(QPoint(str[i].toInt(), str[i + 1].toInt()));
    return dots;
}

QVector<QPoint> SnakeClient::_convertEnemyDots(const QStringList& str)
{
    QVector<QPoint> dots;
    for (size_t i = 3; i < str.size(); i += 2)
        dots.push_back(QPoint(str[i].toInt(), str[i + 1].toInt()));
    return dots;
}
