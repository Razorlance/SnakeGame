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
    //    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readMessage()));
    //        connect(m_socket, SIGNAL(disconnected()), this,
    //        SLOT(disconnectByServer()));
    this->resize(_width * _field_width, _height * _field_height);
    this->setWindowTitle("Snake Game");

    /*QInputDialog *startWindow = new QInputDialog();
    startWindow->setLabelText(tr("Enter your name:"));
    startWindow->setWindowTitle(tr("Start"));
    startWindow->setTextEchoMode(QLineEdit::Normal);
    startWindow->adjustSize();
    startWindow->move(QGuiApplication::primaryScreen()->geometry().center() -
                      startWindow->rect().center());
    QString text = "";*/
    _homeDots.resize(2);
    _enemyDots.resize(2);

    /*if (startWindow->exec() == QDialog::Accepted)
    {
        text = startWindow->textValue();
        socket->connectToHost("127.0.0.1", 33221);
        //        SendToServer(_homeDots);
    }
    if (text.isEmpty())
        text = "Score: " + QString::number(_score);


    initiateGame();*/
}

SnakeClient::~SnakeClient() { delete ui; }

void SnakeClient::ConnectToServer(QString ip, int port, QString name)
{
    _ip = ip;
    _port = port;
    _snakeName = name;
    ui->userName->setText(_snakeName);
    socket->connectToHost(_ip, _port);
    SendToServer(_homeDots);
    initiateGame();
}

void SnakeClient::SendToServer(QVector<QPoint> _homeDots)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);
    // out.setVersion(QDataStream::Qt_6_3);
    QString data = "i " + convertToString(_homeDots);
    qDebug() << data;
    out << data;
    socket->write(Data);
}

void SnakeClient::slotReadyRead()
{
    QDataStream in(socket);
    //    qDebug() << "Reading...1";
    //    in.setVersion(QDataStream::Qt_6_3);
    if (in.status() == QDataStream::Ok)
    {
        qDebug() << "Reading...";
        //        QVector<QPoint> _homeDots;
        in >> _input;

        //        for (QPoint &i : _homeDots)
        //            qDebug() << i.x() << ' ' << i.y() << '\n';
    }
    else
        qDebug() << "Error";
    qDebug() << _input;
    QStringList L = _input.split(' ');
    if (L[0] == 'e')
    {
        gameOver();
    }
    if (L[0] == 'f')
    {
        _fruitPos.rx() = L[1].toInt();
        _fruitPos.ry() = L[2].toInt();
    }
    if (L[0] == 'g')
    {
        _enemyDots = convertToDots(L);
    }
    if (L[0] == 'r')
    {
        _stillGame = L[1].toInt();
    }
    if (L[0] == 'c')
    {
        qDebug() << "Await is true";
        _await = L[1].toInt();
    }
    step();
}

QVector<QPoint> SnakeClient::getMap() { return _homeDots; }

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

// void SnakeClient::timerEvent(QTimerEvent *event)
//{
//    //    Q_UNUSED(event);
//    qDebug() << "Tick" << _stillGame << _await;
//    if (_stillGame & _await)
//    {
//        //        eatFruit();
//        move();
//        //        checkBoundary();
//        _await = false;
//        this->repaint();
//    }
//    SendToServer(_homeDots);
//}

void SnakeClient::drawSnake()
{
    QPainter painter(this);

    if (_stillGame)
    {
        painter.setBrush(Qt::red);
        painter.drawEllipse(_fruitPos.x() * _width, _fruitPos.y() * _height,
                            _width, _height);

        for (size_t i = 0; i < _homeDots.size(); i++)
        {
            if (!i)
            {
                painter.setBrush(Qt::white);
                painter.drawEllipse(_homeDots[i].x() * _width,
                                    _homeDots[i].y() * _height, _width,
                                    _height);
                painter.setBrush(Qt::black);
                painter.drawEllipse(_enemyDots[i].x() * _width,
                                    _enemyDots[i].y() * _height, _width,
                                    _height);
            }
            else
            {
                painter.setBrush(Qt::darkBlue);
                painter.drawEllipse(_homeDots[i].x() * _width,
                                    _homeDots[i].y() * _height, _width,
                                    _height);
                painter.setBrush(Qt::green);
                painter.drawEllipse(_enemyDots[i].x() * _width,
                                    _enemyDots[i].y() * _height, _width,
                                    _height);
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
    for (size_t i = _homeDots.size() - 1; i > 0; i--)
    {
        _homeDots[i] = _homeDots[i - 1];
        _enemyDots[i] = _enemyDots[i - 1];
    }

    switch (_direction)
    {
        case left:
            _homeDots[0].rx()--;
            ui->userName->setGeometry(_homeDots[0].x() * _width - 5,
                                      _homeDots[0].y() * _height - 15,
                                      ui->userName->geometry().width(),
                                      ui->userName->geometry().height());

            _enemyDots[0].rx()--;
            ui->userName->setGeometry(_enemyDots[0].x() * _width - 5,
                                      _enemyDots[0].y() * _height - 15,
                                      ui->userName->geometry().width(),
                                      ui->userName->geometry().height());
            break;
        case right:
            _homeDots[0].rx()++;
            ui->userName->setGeometry(_homeDots[0].x() * _width - 5,
                                      _homeDots[0].y() * _height - 15,
                                      ui->userName->geometry().width(),
                                      ui->userName->geometry().height());
            _enemyDots[0].rx()++;
            ui->userName->setGeometry(_enemyDots[0].x() * _width - 5,
                                      _enemyDots[0].y() * _height - 15,
                                      ui->userName->geometry().width(),
                                      ui->userName->geometry().height());
            break;
        case up:
            _homeDots[0].ry()--;
            ui->userName->setGeometry(_homeDots[0].x() * _width - 5,
                                      _homeDots[0].y() * _height - 15,
                                      ui->userName->geometry().width(),
                                      ui->userName->geometry().height());
            _enemyDots[0].ry()--;
            ui->userName->setGeometry(_enemyDots[0].x() * _width - 5,
                                      _enemyDots[0].y() * _height - 15,
                                      ui->userName->geometry().width(),
                                      ui->userName->geometry().height());
            break;
        case down:
            _homeDots[0].ry()++;
            ui->userName->setGeometry(_homeDots[0].x() * _width - 5,
                                      _homeDots[0].y() * _height + 25,
                                      ui->userName->geometry().width(),
                                      ui->userName->geometry().height());
            _enemyDots[0].ry()++;
            ui->userName->setGeometry(_enemyDots[0].x() * _width - 5,
                                      _enemyDots[0].y() * _height + 25,
                                      ui->userName->geometry().width(),
                                      ui->userName->geometry().height());

            break;
    }
}

void SnakeClient::step()
{
    //    Q_UNUSED(event);
    SendToServer(_homeDots);
    qDebug() << "Tick" << convertToString(_enemyDots);
    if (_stillGame & _await)
    {
        //        eatFruit();
        move();
        //        checkBoundary();
        _await = false;
        this->repaint();
    }

    //    drawSnake();
}

// void SnakeClient::checkBoundary()
//{
//    // We can run into ourself only if there are more then 3 dots
//    if (_homeDots.size() > 4)
//    {
//        for (size_t i = 1; i < _homeDots.size(); i++)
//        {
//            if (_homeDots[0] == _homeDots[i])
//                _stillGame = false;
//        }
//    }

//    if (_homeDots[0].x() < 0)
//        _stillGame = false;
//    if (_homeDots[0].x() == _field_width)
//        _stillGame = false;
//    if (_homeDots[0].y() < 0)
//        _stillGame = false;
//    if (_homeDots[0].y() == _field_height)
//        _stillGame = false;

//    if (!_stillGame)
//        killTimer(_timer);
//}

void SnakeClient::gameOver()
{
    QMessageBox endOfGame;
    endOfGame.setText("Game Over");

    endOfGame.exec();

    this->close();
}

void SnakeClient::eatFruit()
{
    if (_fruitPos == _homeDots[0])
    {
        _homeDots.push_back(_fruitPos);
        _score++;
        locateFruit();
    }
}

void SnakeClient::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    //    _await = false;
    //    SendToServer(_homeDots);
    drawSnake();
}

QString SnakeClient::convertToString(QVector<QPoint> &dots)
{
    QString str("");
    for (const QPoint &dot : dots)
        str += QString::number(dot.x()) + " " + QString::number(dot.y()) + " ";
    return str.trimmed();
}

QVector<QPoint> SnakeClient::convertToDots(QStringList &str)
{
    QVector<QPoint> dots(2);
    //    for (size_t i = 1; i < str.size(); i+= 2){
    dots[0] = QPoint(str[3].toInt(), str[4].toInt());
    dots[1] = QPoint(str[1].toInt(), str[2].toInt());
    //    }
    return dots;
}

void SnakeClient::initiateGame()
{
    _direction = right;
    //    _stillGame = true;

    for (size_t i = 0; i < _homeDots.size(); i++)
    {
        _homeDots[i].rx() = _homeDots.size() - i - 1;
        _homeDots[i].ry() = 0;
    }
    SendToServer(_homeDots);
}
