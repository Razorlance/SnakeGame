#include "snakeclient.h"
#include "./ui_snakeclient.h"

/*
 * TODO:
 * 1. Think about the boundaries of the window. Disallow the change of size of the window maybe?
 * 5. Make the win situation.
 * 6. Improve label position.
*/

SnakeClient::SnakeClient(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SnakeClient)
{
    ui->setupUi(this);
    this->resize(_width * _field_width, _height * _field_height);
    this->setWindowTitle("Snake Game");

    _moveBlock = false;

    QInputDialog* startWindow = new QInputDialog();
    startWindow->setLabelText(tr("Enter your name:"));
    startWindow->setWindowTitle(tr("Start"));
    startWindow->setTextEchoMode(QLineEdit::Normal);
    startWindow->adjustSize();
    startWindow->move(QGuiApplication::primaryScreen()->geometry().center() - startWindow->rect().center());
    QString text = "";
    if(startWindow->exec() == QDialog::Accepted)
        text = startWindow->textValue();

    if (text.isEmpty())
        text = "Player";
    ui->userName->setText(text);

    initiateGame();
}

SnakeClient::~SnakeClient()
{
    delete ui;
}

void SnakeClient::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();

    if (_moveBlock)
        return;

    if ((key == Qt::Key_Left || key == Qt::Key_A) && _direction != right)
        _direction = left;

    if ((key == Qt::Key_Right || key == Qt::Key_D) && _direction != left)
        _direction = right;

    if ((key == Qt::Key_Up || key == Qt::Key_W) && _direction != down)
        _direction = up;

    if ((key == Qt::Key_Down || key == Qt::Key_S) && _direction != up)
        _direction = down;

    _moveBlock = true;
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
        painter.drawEllipse(_fruitPos.x() * _width, _fruitPos.y() * _height, _width, _height);

        for (size_t i = 0; i < _dots.size(); i++)
        {
            if (!i)
            {
                painter.setBrush(Qt::white);
                painter.drawEllipse(_dots[i].x() * _width, _dots[i].y() * _height, _width, _height);
            }
            else
            {
                painter.setBrush(Qt::darkBlue);
                painter.drawEllipse(_dots[i].x() * _width, _dots[i].y() * _height, _width, _height);
            }
        }
    }
    else
    {
        this->close();
        gameOver();
    }

    _moveBlock = false;
}

void SnakeClient::locateFruit()
{
    QTime time = QTime::currentTime();
    qsrand((uint) time.msec());

    _fruitPos.rx() = qrand() % _width;
    _fruitPos.ry() = qrand() % _height;

    for (size_t i = 0; i < _dots.size(); i++)
    {
        if (_dots[i].x() == _fruitPos.x() && _dots[i].y() == _fruitPos.y())
            locateFruit();
    }
}

void SnakeClient::move()
{
    for (size_t i = _dots.size() - 1; i > 0; i--)
    {
        _dots[i] = _dots[i - 1];
    }

    switch(_direction)
    {
    case left:
        _dots[0].rx()--;
        ui->userName->setGeometry(_dots[0].x() * _width - 5, _dots[0].y() * _height - 15,
                ui->userName->geometry().width(), ui->userName->geometry().height());
        break;
    case right:
        _dots[0].rx()++;
        ui->userName->setGeometry(_dots[0].x() * _width - 5, _dots[0].y() * _height - 15,
                ui->userName->geometry().width(), ui->userName->geometry().height());
        break;
    case up:
        _dots[0].ry()--;
        ui->userName->setGeometry(_dots[0].x() * _width - 5, _dots[0].y() * _height - 15,
                ui->userName->geometry().width(), ui->userName->geometry().height());
        break;
    case down:
        _dots[0].ry()++;
        ui->userName->setGeometry(_dots[0].x() * _width - 5, _dots[0].y() * _height + 25,
                ui->userName->geometry().width(), ui->userName->geometry().height());
        break;
    }
}

void SnakeClient::checkBoundary()
{
    if (_dots.size() > 4) // We can run into ourself only if there are more then 3 dots
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
