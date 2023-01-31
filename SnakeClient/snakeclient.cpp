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

SnakeClient::SnakeClient(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::SnakeClient)
{
    ui->setupUi(this);

    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::readyRead, this, &SnakeClient::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket,
            &QTcpSocket::deleteLater);

    this->resize(_width * _field_width, _height * _field_height);
    this->setWindowTitle("Snake Game");

    QInputDialog *startWindow = new QInputDialog();
    startWindow->setLabelText(tr("Enter your name:"));
    startWindow->setWindowTitle(tr("Start"));
    startWindow->setTextEchoMode(QLineEdit::Normal);
    startWindow->adjustSize();
    startWindow->move(QGuiApplication::primaryScreen()->geometry().center() -
                      startWindow->rect().center());
    QString text = "";
    if (startWindow->exec() == QDialog::Accepted)
    {
        text = startWindow->textValue();
        socket->connectToHost("127.0.0.1", 33221);
        SendToServer(getMap());
    }
    if (text.isEmpty())
        text = "Score: " + QString::number(_score);
    ui->userName->setText(text);

    initiateGame();
}

SnakeClient::~SnakeClient() { delete ui; }

void SnakeClient::SendToServer(QVector<QPoint> _dots)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_3);
    out << _dots;
    socket->write(Data);
}

void SnakeClient::slotReadyRead()
{
    QDataStream in(socket);
    qDebug() << "Reading...1";
    in.setVersion(QDataStream::Qt_6_3);
    if (in.status() == QDataStream::Ok)
    {
        qDebug() << "Reading...";
        QVector<QPoint> _dots;
        in >> _dots;
        for (QPoint &i : _dots)
            qDebug() << i.x() << ' ' << i.y() << '\n';
    }
    else
        qDebug() << "Error";
}

QVector<QPoint> SnakeClient::getMap() { return _dots; }

void SnakeClient::keyPressEvent(QKeyEvent *event)
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

void SnakeClient::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);

    if (_stillGame)
    {
        eatFruit();
        move();
        checkBoundary();
    }

    this->repaint();
}

void SnakeClient::drawSnake()
{
    QPainter painter(this);

    if (_stillGame)
    {
        painter.setBrush(Qt::red);
        painter.drawEllipse(_fruitPos.x() * _width, _fruitPos.y() * _height,
                            _width, _height);

        for (size_t i = 0; i < _dots.size(); i++)
        {
            if (!i)
            {
                painter.setBrush(Qt::white);
                painter.drawEllipse(_dots[i].x() * _width,
                                    _dots[i].y() * _height, _width, _height);
            }
            else
            {
                painter.setBrush(Qt::darkBlue);
                painter.drawEllipse(_dots[i].x() * _width,
                                    _dots[i].y() * _height, _width, _height);
            }
        }
    }
    else
    {
        gameOver();
    }
}

void SnakeClient::locateFruit()
{
    QTime time = QTime::currentTime();
    srand((uint)time.msec());

    _fruitPos.rx() = rand() % _width;
    _fruitPos.ry() = rand() % _height;
}

void SnakeClient::move()
{
    for (size_t i = _dots.size() - 1; i > 0; i--)
    {
        _dots[i] = _dots[i - 1];
    }

    switch (_direction)
    {
        case left:
            _dots[0].rx()--;
            ui->userName->setGeometry(_dots[0].x() * _width - 5,
                                      _dots[0].y() * _height - 15,
                                      ui->userName->geometry().width(),
                                      ui->userName->geometry().height());
            break;
        case right:
            _dots[0].rx()++;
            ui->userName->setGeometry(_dots[0].x() * _width - 5,
                                      _dots[0].y() * _height - 15,
                                      ui->userName->geometry().width(),
                                      ui->userName->geometry().height());
            break;
        case up:
            _dots[0].ry()--;
            ui->userName->setGeometry(_dots[0].x() * _width - 5,
                                      _dots[0].y() * _height - 15,
                                      ui->userName->geometry().width(),
                                      ui->userName->geometry().height());
            break;
        case down:
            _dots[0].ry()++;
            ui->userName->setGeometry(_dots[0].x() * _width - 5,
                                      _dots[0].y() * _height + 25,
                                      ui->userName->geometry().width(),
                                      ui->userName->geometry().height());
            break;
    }
}

void SnakeClient::checkBoundary()
{
    if (_dots.size() >
        4)  // We can run into ourself only if there are more then 3 dots
    {
        for (size_t i = 1; i < _dots.size(); i++)
        {
            if (_dots[0] == _dots[i])
                _stillGame = false;
        }
    }

    if (_dots[0].x() < 0)
        _stillGame = false;
    if (_dots[0].x() == _field_width)
        _stillGame = false;
    if (_dots[0].y() < 0)
        _stillGame = false;
    if (_dots[0].y() == _field_height)
        _stillGame = false;

    if (!_stillGame)
        killTimer(_timer);
}

void SnakeClient::gameOver()
{
    QMessageBox endOfGame;
    endOfGame.setText("Game Over");
    endOfGame.exec();
    this->close();
}

void SnakeClient::eatFruit()
{
    if (_fruitPos == _dots[0])
    {
        _dots.push_back(_fruitPos);
        _score++;
        locateFruit();
    }
}

void SnakeClient::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    drawSnake();
}

void SnakeClient::initiateGame()
{
    _direction = right;
    _stillGame = true;
    _dots.resize(2);

    for (size_t i = 0; i < _dots.size(); i++)
    {
        _dots[i].rx() = _dots.size() - i - 1;
        _dots[i].ry() = 0;
    }

    locateFruit();

    _timer = startTimer(_delay);
}
