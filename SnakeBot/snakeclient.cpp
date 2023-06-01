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
    :   QMainWindow(parent),
    _ui(new Ui::SnakeClient)
{
    _ui->setupUi(this);
    _socket = new QTcpSocket(this);
    connect(_socket, &QTcpSocket::readyRead, this, &SnakeClient::slotReadyRead);
    connect(_socket, &QTcpSocket::disconnected, _socket,
            &QTcpSocket::deleteLater);
    this->resize(_WIDTH * _FIELD_WIDTH, _HEIGHT * _FIELD_HEIGHT);
    this->setWindowTitle("Snake Game");
    _nextBlockSize = 0;
}

SnakeClient::~SnakeClient() { delete _ui; }

void SnakeClient::connectToServer(const QString& ip,
                                  int port,
                                  const QString& name,
                                  int viewer)
{
    _viewer = viewer;
    _socket->connectToHost(ip, port);
    _data.clear();
    QDataStream out(&_data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);
    QString data = "v " + QString::number(viewer);
    qDebug() << data;
    out << quint16(0) << data;
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
    QString data = "d " + QString::number(_direction);
    qDebug() << data;
    out << quint16(0) << data;
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
                    _fruitPos.rx() = l[1].toInt();
                    _fruitPos.ry() = l[2].toInt();
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
                        }

                        else if (l[1].toInt() == 2)
                        {
                            _direction = left;
                            _colour = red;
                        }

                        else if (l[1].toInt() == 3)
                        {
                            _direction = up;
                            _colour = green;
                        }

                        else if (l[1].toInt() == 4)
                        {
                            _direction = down;
                            _colour = magenta;
                        }

                        _stillGame = l[1].toInt();
                    }
                }

                else
                {
                    _enemiesDots[l[1].toInt()] = _convertEnemyDots(l);
                    _await = true;
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

    painter.setBrush(Qt::red);
    painter.drawEllipse(_fruitPos.x() * _WIDTH,
                        _fruitPos.y() * _HEIGHT,
                        _WIDTH, _HEIGHT);

    for (size_t i = 0; i < _homeDots.size(); i++)
    {
        if (i == 0)
        {
            painter.setBrush(Qt::white);
            painter.drawEllipse(_homeDots[i].x() * _WIDTH,
                                _homeDots[i].y() * _HEIGHT,
                                _WIDTH, _HEIGHT);
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
                                _homeDots[i].y() * _HEIGHT,
                                _WIDTH, _HEIGHT);
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
                                    it.value()[i].y() * _HEIGHT,
                                    _WIDTH, _HEIGHT);
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
                                    it.value()[i].y() * _HEIGHT,
                                    _WIDTH, _HEIGHT);
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

void SnakeClient::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    _drawSnake();
}

QVector<QPoint> SnakeClient::_convertHomeDots(const QStringList &str)
{
    QVector<QPoint> dots;
    for (size_t i = 1; i < str.size(); i += 2)
        dots.push_back(QPoint(str[i].toInt(), str[i + 1].toInt()));
    return dots;
}

QVector<QPoint> SnakeClient::_convertEnemyDots(const QStringList &str)
{
    QVector<QPoint> dots;
    for (size_t i = 2; i < str.size(); i += 2)
        dots.push_back(QPoint(str[i].toInt(), str[i + 1].toInt()));
    return dots;
}
