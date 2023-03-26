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
    _homeDots.resize(2);
    _enemyDots.resize(2);
}

SnakeClient::~SnakeClient() { delete _ui; }

void SnakeClient::connectToServer(const QString& ip, int port,
                                  const QString& name)
{
    _ui->userName->setText(name);
    _socket->connectToHost(ip, port);
    _sendToServer(_homeDots);
    _initiateGame();
}

void SnakeClient::_sendToServer(const QVector<QPoint>& _homeDots)
{
    _data.clear();
    QDataStream out(&_data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);
    QString data = "i " + _convertToString(_homeDots);
    qDebug() << data;
    out << data;
    _socket->write(_data);
}

void SnakeClient::slotReadyRead()
{
    QDataStream in(_socket);
    if (in.status() == QDataStream::Ok)
    {
        qDebug() << "Reading...";
        in >> _input;
    }
    else
        qDebug() << "Error";
    qDebug() << _input;
    QStringList l = _input.split(' ');
    if (l[0] == 'e')
        _gameOver();
    if (l[0] == 'f')
    {
        _fruitPos.rx() = l[1].toInt();
        _fruitPos.ry() = l[2].toInt();
    }
    if (l[0] == 'g')
        _enemyDots = _convertToDots(l);
    if (l[0] == 'r')
        _stillGame = l[1].toInt();
    if (l[0] == 'c')
    {
        qDebug() << "Await is true";
        _await = l[1].toInt();
    }
    _step();
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
}

void SnakeClient::_drawSnake()
{
    QPainter painter(this);

    if (_stillGame)
    {
        painter.setBrush(Qt::red);
        painter.drawEllipse(_fruitPos.x() * _WIDTH, _fruitPos.y() * _HEIGHT,
                            _WIDTH, _HEIGHT);
        for (size_t i = 0; i < _homeDots.size(); i++)
        {
            if (!i)
            {
                painter.setBrush(Qt::white);
                painter.drawEllipse(_homeDots[i].x() * _WIDTH,
                                    _homeDots[i].y() * _HEIGHT, _WIDTH,
                                    _HEIGHT);
                painter.setBrush(Qt::black);
                painter.drawEllipse(_enemyDots[i].x() * _WIDTH,
                                    _enemyDots[i].y() * _HEIGHT, _WIDTH,
                                    _HEIGHT);
            }
            else
            {
                painter.setBrush(Qt::darkBlue);
                painter.drawEllipse(_homeDots[i].x() * _WIDTH,
                                    _homeDots[i].y() * _HEIGHT, _WIDTH,
                                    _HEIGHT);
                painter.setBrush(Qt::green);
                painter.drawEllipse(_enemyDots[i].x() * _WIDTH,
                                    _enemyDots[i].y() * _HEIGHT, _WIDTH,
                                    _HEIGHT);
            }
        }
    }
    else
    {
        _gameOver();
    }
}

void SnakeClient::_locateFruit()
{
    QTime time = QTime::currentTime();
    srand((uint)time.msec());

    _fruitPos.rx() = rand() % _WIDTH;
    _fruitPos.ry() = rand() % _HEIGHT;
}

void SnakeClient::_move()
{
    for (size_t i = _homeDots.size() - 1; i > 0; i--)
    {
        _homeDots[i] = _homeDots[i - 1];
        _enemyDots[i] = _enemyDots[i - 1];
    }

    switch (_direction)
    {
        case left:
            _homeDots[0].rx()--;
            _ui->userName->setGeometry(_homeDots[0].x() * _WIDTH - 5,
                                       _homeDots[0].y() * _HEIGHT - 15,
                                       _ui->userName->geometry().width(),
                                       _ui->userName->geometry().height());

            _enemyDots[0].rx()--;
            _ui->userName->setGeometry(_enemyDots[0].x() * _WIDTH - 5,
                                       _enemyDots[0].y() * _HEIGHT - 15,
                                       _ui->userName->geometry().width(),
                                       _ui->userName->geometry().height());
            break;
        case right:
            _homeDots[0].rx()++;
            _ui->userName->setGeometry(_homeDots[0].x() * _WIDTH - 5,
                                       _homeDots[0].y() * _HEIGHT - 15,
                                       _ui->userName->geometry().width(),
                                       _ui->userName->geometry().height());
            _enemyDots[0].rx()++;
            _ui->userName->setGeometry(_enemyDots[0].x() * _WIDTH - 5,
                                       _enemyDots[0].y() * _HEIGHT - 15,
                                       _ui->userName->geometry().width(),
                                       _ui->userName->geometry().height());
            break;
        case up:
            _homeDots[0].ry()--;
            _ui->userName->setGeometry(_homeDots[0].x() * _WIDTH - 5,
                                       _homeDots[0].y() * _HEIGHT - 15,
                                       _ui->userName->geometry().width(),
                                       _ui->userName->geometry().height());
            _enemyDots[0].ry()--;
            _ui->userName->setGeometry(_enemyDots[0].x() * _WIDTH - 5,
                                       _enemyDots[0].y() * _HEIGHT - 15,
                                       _ui->userName->geometry().width(),
                                       _ui->userName->geometry().height());
            break;
        case down:
            _homeDots[0].ry()++;
            _ui->userName->setGeometry(_homeDots[0].x() * _WIDTH - 5,
                                       _homeDots[0].y() * _HEIGHT + 25,
                                       _ui->userName->geometry().width(),
                                       _ui->userName->geometry().height());
            _enemyDots[0].ry()++;
            _ui->userName->setGeometry(_enemyDots[0].x() * _WIDTH - 5,
                                       _enemyDots[0].y() * _HEIGHT + 25,
                                       _ui->userName->geometry().width(),
                                       _ui->userName->geometry().height());

            break;
    }
}

void SnakeClient::_step()
{
    _sendToServer(_homeDots);
    qDebug() << "Tick" << _convertToString(_enemyDots);
    if (_stillGame & _await)
    {
        _move();
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

void SnakeClient::_eatFruit()
{
    if (_fruitPos == _homeDots[0])
    {
        _homeDots.push_back(_fruitPos);
        _score++;
        _locateFruit();
    }
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
    QVector<QPoint> dots(2);
    dots[0] = QPoint(str[3].toInt(), str[4].toInt());
    dots[1] = QPoint(str[1].toInt(), str[2].toInt());
    return dots;
}

void SnakeClient::_initiateGame()
{
    _direction = right;
    for (size_t i = 0; i < _homeDots.size(); i++)
    {
        _homeDots[i].rx() = _homeDots.size() - i - 1;
        _homeDots[i].ry() = 0;
    }
    _sendToServer(_homeDots);
}
