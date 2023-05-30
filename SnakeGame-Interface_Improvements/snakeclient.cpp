#include "snakeclient.h"
#include "./ui_snakeclient.h"
#include <QPushButton>
#include <QFormLayout>
#include <QDebug>
#include <QComboBox>
#include <QSpinBox>
#include <QVBoxLayout>

/*
 * TODO:
 * 1. Think about the boundaries of the window. Disallow the change of size of the window maybe?
 * 5. Make the win situation.
 * 6. Improve label position.
*/

SnakeClient::SnakeClient(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SnakeClient)
    , _playerList(new QListWidget)
{
    ui->setupUi(this);
    this->resize(_width * _field_width, _height * _field_height);
    this->setWindowTitle("Snake Game");

    _moveBlock = false;

    QDialog* startWindow = new QDialog();

    //QVBoxLayout* layout = new QVBoxLayout();

    startWindow->setWindowTitle("Start Window");
    startWindow->adjustSize();
    startWindow->move(QGuiApplication::primaryScreen()->geometry().center() - startWindow->rect().center());

    QFormLayout *form = new QFormLayout(startWindow);
    QLineEdit* name = new QLineEdit(startWindow);
    QComboBox* mode = new QComboBox(startWindow);
    QLineEdit* ip = new QLineEdit(startWindow);
    QLineEdit* port = new QLineEdit(startWindow);
    QComboBox *type = new QComboBox(startWindow);

    ip->setText("127.0.0.1");
    port->setText("33221");

    //numPlayers->setRange(1, 4);

    mode->addItem("Player");
    mode->addItem("Viewer");
    type->addItem("1:1");
    type->addItem("1:Bot");
    type->addItem("1:3");
    form->addRow("Enter your name:", name);
    form->addRow("Enter your mode of the game:", mode);
    form->addRow("IP:", ip);
    form->addRow("Port:", port);
    form->addRow("Game Type:", type);


    QPushButton *button = new QPushButton("OK", startWindow);
    //QPushButton *buttonCancel = new QPushButton("Cancel", startWindow);
    QObject::connect(button, &QPushButton::clicked, startWindow, &QDialog::accept);
    form->addWidget(button);

    QString nameText = "";
    QString modeText = "";
    if (startWindow->exec() == QDialog::Accepted)
    {
        QString nameText = name->text();
        QString modeText = mode->currentText();
        QString ipText = ip->text();
        QString portText = port->text();
        QString typeText = type->currentText();
        //int numPlayersInt = numPlayers->value();
        // QString::number(input3Value)); //if string of # players will be needed
        if (nameText.size() == 0)
            nameText = "Player";
        if (modeText == "Player")
        {
            /*
            QMessageBox msgBox;
            if ((_validIP(ipText) &&
                 _validPort(portText.toInt())))
            {
                _w.connectToServer(ipText, portText.toInt(),
                                   nameText);
                _w.show();
                this->close();
            }
            else
            {
                msgBox.setText("The IP address or Port is not correct");
                msgBox.exec();
            }
            qDebug() << "connecting";*/
        }
        if (modeText == "Viewer")
        {
            /*
            //NEEDED TO REWRITE: WHAT TO DO WHEN IT IS A VIEWER
            QMessageBox msgBox;
            if ((_validIP(ipText) &&
                 _validPort(portText.toInt())))
                _w.connectToServer(ipText, portText.toInt(),
                                   nameText);
            else
            {
                msgBox.setText("The IP address or Port is not correct");
                msgBox.exec();
            }
            qDebug() << "connecting";
            */
        }
        ui->player1Label->setStyleSheet("QLabel { color : blue; }");
        ui->player2Label->setStyleSheet("QLabel { color : red; }");
        ui->player3Label->setStyleSheet("QLabel { color : green; }");
        ui->player4Label->setStyleSheet("QLabel { color : orange; }");
        if (typeText.split(":")[1] == "1")
        {
            ui->player1Label->setText(nameText);
            //Adjusting the fond to the size of listWidget
            /*
            for(int i = 0; i < ui->listWidget->count(); i++) // for future when there will be not just 1 Player
            {
                QListWidgetItem* item = ui->listWidget->item(i);
                //item->setSizeHint(QSize(item->sizeHint().width(), ui->listWidget->height() / ui->listWidget->count()));
                //item->setSizeHint(QSize(117, 4190300));
                item->setSizeHint(QSize(_width * _field_width, _height * _field_height / 4));
                QFont font;
                font.setPointSize((27));
                font.setBold(true);
                item->setFont(font);
                item->setForeground(Qt::blue);
            }*/
            //QString nameScore = nameText + " " + QString::number(_score);
            //ui->listWidget->item(0)->setText(nameScore);
        }
        if (typeText.split(":")[1] == "Bot")
        {
            ui->player1Label->setText(nameText);
            ui->player2Label->setText("Bot");
            //ui->listWidget->addItem(nameText);
            //ui->listWidget->addItem(...);
        }
        if (typeText.split(":")[1] == "3")
        {
            //ui->listWidget->addItem(nameText);
            //ui->listWidget->addItem(...);
            //ui->listWidget->addItem(...);
        }
        ui->userName->setText(nameText);
        //ui->labelName->setText(nameText);
    }
//    else
//    {
//        QCoreApplication::exit(); // does not help, need to close the mainWindow when the cancel (red) button is pressed
                                    // now if it is pressed, the game still opens
//    }



    initiateGame();


//    QInputDialog* startWindow = new QInputDialog();
//    startWindow->setWindowTitle(tr("Start"));
//    startWindow->setLabelText(tr("Enter your name:"));
//    startWindow->setTextEchoMode(QLineEdit::Normal);
//    startWindow->adjustSize();
//    startWindow->move(QGuiApplication::primaryScreen()->geometry().center() - startWindow->rect().center());
//    QString text = "";
//    if (startWindow->exec() == QDialog::Accepted)
//        text = startWindow->textValue();

//    if (text.isEmpty())
//        text = "Player";
//    ui->userName->setText(text);
//    initiateGame();

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
    srand((uint) time.msec()); //qsrand -> srand

    _fruitPos.rx() = rand() % _width;  //qrand ->rand
    _fruitPos.ry() = rand() % _height; //qrand ->rand

    if (_fruitPos.x() >= _field_width)
        locateFruit();

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
    //QString nameText = ui->listWidget->currentItem()->text();

    if (_fruitPos == _dots[0])
    {
        _dots.push_back(_fruitPos);
        _score++;
        QString nameText = ui->userName->text();
        QString nameScore = nameText + " " + QString::number(_score);
        ui->player1Label->setText(nameScore);
        //ui->listWidget->item(0)->setText(nameScore);
        locateFruit();
    }
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

bool SnakeClient::_validIP(const QString &ip)
{
    QVector<QString> v = ip.split('.');
    if (v.size() != 4)
        return false;
    for (QString& str : v)
        if (str.toInt() > 255 || str.toInt() < 0 || not(_isNumber(str)))
            return false;
    return true;
}

bool SnakeClient::_validPort(int port)
{
    if (port >= 0 && port <= 65535)
        return true;
    return false;
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
