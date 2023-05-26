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
    nextBlockSize = 0;
    _homeDots.resize(2);
    _enemyDots.resize(2);
}

SnakeClient::~SnakeClient() { delete _ui; }

void SnakeClient::connectToServer(const QString& ip, int port,
                                  const QString& name)
{
    _ui->userName->setText(name);
    _socket->connectToHost(ip, port);
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
            if (nextBlockSize == 0)
            {
                if (_socket->bytesAvailable() < 2)
                    break;
                in >> nextBlockSize;
            }
            if (_socket->bytesAvailable() < nextBlockSize)
                break;
            in >> _input;
            _socket->waitForBytesWritten();
            nextBlockSize = 0;
            qDebug() << _input;
            QStringList commandList = _input.split(';');
            for (QString& c : commandList)
            {
                QStringList l = c.split(' ');
                if (l[0] == 'e')
                    _gameOver();
                if (l[0] == 'd')
                {
                    _enemyDirection = Directions(l[1].toInt());
                }
                if (l[0] == 'f')
                {
                    _fruitPos.rx() = l[1].toInt();
                    _fruitPos.ry() = l[2].toInt();
                }
                if (l[0] == 'g')
                {
                    _enemyDots = _convertToDots(l);
                }
                if (l[0] == 'h')
                {
                    _homeDots = _convertToDots(l);
                }
                if (l[0] == 'r')
                    _stillGame = l[1].toInt();
                if (l[0] == 'c')
                {
                    qDebug() << "Await is true";
                    _await = l[1].toInt();
                }
            }
            _step();
            break;
        }
    }
    else
        qDebug() << "Error";
}

QVector<QPoint> SnakeClient::getMap() { return _homeDots; }

void SnakeClient::keyPressEvent(QKeyEvent* event)
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

void SnakeClient::_drawSnake()
{
    QPainter painter(this);

    painter.setBrush(Qt::red);
    painter.drawEllipse(_fruitPos.x() * _WIDTH,
                        _fruitPos.y() * _HEIGHT,
                        _WIDTH, _HEIGHT);

    for (size_t i = 0; i < _homeDots.size(); i++)
    {
        if (i != 0)
        {
            painter.setBrush(Qt::white);
            painter.drawEllipse(_homeDots[i].x() * _WIDTH,
                                _homeDots[i].y() * _HEIGHT,
                                _WIDTH, _HEIGHT);
        }

        else
        {
            painter.setBrush(Qt::darkBlue);
            painter.drawEllipse(_homeDots[i].x() * _WIDTH,
                                _homeDots[i].y() * _HEIGHT,
                                _WIDTH, _HEIGHT);
        }
    }

    for (size_t i = 0; i < _enemyDots.size(); i++)
    {
        if (i != 0)
        {
            painter.setBrush(Qt::black);
            painter.drawEllipse(_enemyDots[i].x() * _WIDTH,
                                _enemyDots[i].y() * _HEIGHT,
                                _WIDTH, _HEIGHT);
        }

        else
        {
            painter.setBrush(Qt::green);
            painter.drawEllipse(_enemyDots[i].x() * _WIDTH,
                                _enemyDots[i].y() * _HEIGHT,
                                _WIDTH, _HEIGHT);
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

QString SnakeClient::_convertToString(const QVector<QPoint>& dots)
{
    QString str("");
    for (const QPoint& dot : dots)
        str += QString::number(dot.x()) + " " + QString::number(dot.y()) + " ";
    return str.trimmed();
}

QVector<QPoint> SnakeClient::_convertToDots(const QStringList& str)
{
    QVector<QPoint> dots;
    for (size_t i = 1; i < str.size(); i += 2)
        dots.push_back(QPoint(str[i].toInt(), str[i + 1].toInt()));
    return dots;
}
