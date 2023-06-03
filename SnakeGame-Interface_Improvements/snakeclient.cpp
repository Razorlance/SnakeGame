#include "snakeclient.h"
#include "./ui_snakeclient.h"

/*
 * TODO:
 * 1. Think about the boundaries of the window. Disallow the change of size of the window maybe? !!!!!!!!!! DOSALLOWED
 * 5. Make the win situation.
 * 6. Improve label position.
*/

SnakeClient::SnakeClient(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SnakeClient)
{
    ui->setupUi(this);
    setFixedSize(630, 630);
    this->resize(_width * _field_width, _height * _field_height);
    this->setWindowTitle("Snake Game");

    _moveBlock = false;

    ui->player1Label->setFixedWidth(100);
    ui->player2Label->setFixedWidth(100);
    ui->player3Label->setFixedWidth(100);
    ui->player4Label->setFixedWidth(100);

    QDialog* startWindow = new QDialog();
    startWindow->setFixedSize(330, 220);
    startWindow->setWindowTitle("Start Window");
    startWindow->adjustSize();
    startWindow->move(QGuiApplication::primaryScreen()->geometry().center() - startWindow->rect().center());

    QFormLayout *form = new QFormLayout(startWindow);
    QLineEdit* name = new QLineEdit(startWindow);
    QComboBox* mode = new QComboBox(startWindow);
    QLineEdit* ip = new QLineEdit(startWindow);
    QLineEdit* port = new QLineEdit(startWindow);
    QPushButton *button = new QPushButton("Start", startWindow);
    QObject::connect(button, &QPushButton::clicked, startWindow, &QDialog::accept);
    QComboBox *type = new QComboBox;
    QVector<QString> vec = {"Enter your name:", "Game Mode:", "IP:", "Port:"};
    QVector<QWidget*> widgetList = {name, mode, ip, port};

    QLabel *typeLabel = new QLabel("Game Type:");

    mode->addItem("Viewer");
    mode->addItem("Player");

    ip->setText("127.0.0.1");
    port->setText("33221");

    type->addItem("1:1");
    type->addItem("1:3");
    type->addItem("1:Bot");
    type->addItem("Bot:Bot");

    type->hide();

    name->setStyleSheet("QLineEdit { border-radius: 5px; }");
    ip->setStyleSheet("QLineEdit { border-radius: 5px; }");
    port->setStyleSheet("QLineEdit { border-radius: 5px; }");
    for (int i = 0; i < vec.length(); i++)
    {
        QLabel *label = new QLabel(vec[i]);
        QFont font = label->font();
        font.setPointSize(20);
        font.setBold(true);
        font.setFamily("Copperplate");
        label->setFont(font);
        form->addRow(label, widgetList[i]);
    }
//    form->addRow(label, name);
//    form->addRow("Enter your mode of the game:", mode);
//    form->addRow("IP:", ip);
//    form->addRow("Port:", port);
//    form->addRow("Game Type:", type);
    QFont font = typeLabel->font();
    font.setPointSize(20);
    font.setBold(true);
    font.setFamily("Copperplate");
    typeLabel->setFont(font);
    typeLabel->setStyleSheet("QLabel { color : grey; }");
    form->addRow(typeLabel, type);
    form->addWidget(button);

    validName(name, button);
    showType(mode, type, typeLabel);
    validIP(ip, button);
    validPort(port, button);

    if (startWindow->exec() == QDialog::Rejected)
    {
        QApplication::quit();
        return;
    }
    countDownDialog();

    QString nameText = name->text();
    QString modeText = mode->currentText();
    QString ipText = ip->text();
    QString portText = port->text();

    if (nameText.size() == 0)
        nameText = "Player";
    _name = nameText;
    _mode = modeText;

    if (modeText == "Player")
    {
        QString typeText = type->currentText();
        if (typeText.split(":")[1] == "1")
        {
            ui->player1Label->setText(nameText);
        }
        if (typeText.split(":")[0] == "Bot" && typeText.split(":")[1] == "Bot")
        {
            ui->player1Label->setText("Bot1");
            ui->player2Label->setText("Bot2");
        }
        if (typeText.split(":")[0] != "Bot" && typeText.split(":")[1] == "Bot")
        {
            ui->player1Label->setText(nameText);
            ui->player2Label->setText("Bot");
        }
        if (typeText.split(":")[1] == "3")
        {
            ui->player1Label->setText(nameText);
            //...
        }
    }
    else
    {
        //ui->player1Label->setText("");
    }

    ui->player1Label->setStyleSheet("QLabel { color : blue; }");
    ui->player2Label->setStyleSheet("QLabel { color : red; }");
    ui->player3Label->setStyleSheet("QLabel { color : green; }");
    ui->player4Label->setStyleSheet("QLabel { color : orange; }");

    //initiateGame();
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
        //ui->userName->setGeometry(_dots[0].x() * _width - 5, _dots[0].y() * _height - 15,
        //        ui->userName->geometry().width(), ui->userName->geometry().height());
        break;
    case right:
        _dots[0].rx()++;
        //ui->userName->setGeometry(_dots[0].x() * _width - 5, _dots[0].y() * _height - 15,
        //        ui->userName->geometry().width(), ui->userName->geometry().height());
        break;
    case up:
        _dots[0].ry()--;
        //ui->userName->setGeometry(_dots[0].x() * _width - 5, _dots[0].y() * _height - 15,
        //        ui->userName->geometry().width(), ui->userName->geometry().height());
        break;
    case down:
        _dots[0].ry()++;
        //ui->userName->setGeometry(_dots[0].x() * _width - 5, _dots[0].y() * _height + 25,
        //        ui->userName->geometry().width(), ui->userName->geometry().height());
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
        if (_mode == "Player")
        {
            QString nameText = _name;
            QString nameScore = nameText + " " + QString::number(_score);
            ui->player1Label->setText(nameScore);
        }
        locateFruit();
    }
}

void SnakeClient::showType(QComboBox *mode, QComboBox *type, QLabel *typeLabel)
{
    connect(mode, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this, mode, type, typeLabel](int index)
    {
    // If the selected item is "Player", show the player options combo box. Otherwise, hide it.
    if (mode->currentText() == "Player")
    {
        type->show();
        typeLabel->setStyleSheet("QLabel { color : white; }");
    }
    else
    {
        type->hide();
        typeLabel->setStyleSheet("QLabel { color : grey; }");
    }
    });
}

void SnakeClient::validName(QLineEdit *name, QPushButton *button)
{
    connect(name, &QLineEdit::textChanged, this, [this, name, button](const QString &text)
    {
    if (text.length() > 7)
    {
        // If the length of the text exceeds 7, set the text color to red
        name->setStyleSheet("QLineEdit { color: red; }");
        button->setEnabled(false);
    }
    else
    {
        // If the length of the text is 7 or less, set the text color back to white
        name->setStyleSheet("QLineEdit { color: white; }");
        button->setEnabled(true);
    }
    });
}

void SnakeClient::validIP(QLineEdit *ip, QPushButton *button)
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
        ip->setStyleSheet("QLineEdit { color: white; }");
        button->setEnabled(true);
    }
    });
}

void SnakeClient::validPort(QLineEdit *port, QPushButton *button)
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
        port->setStyleSheet("QLineEdit { color: white; }");
        button->setEnabled(true);
    }
    });
}

void SnakeClient::countDownDialog()
{
    QDialog *readyDialog = new QDialog;
    readyDialog->setWindowTitle("Ready Steady Go");
    readyDialog->setFixedSize(200, 100);

    QLabel *label = new QLabel("3", readyDialog);
    label->setFont(QFont("Copperplate", 50));
    label->setAlignment(Qt::AlignCenter);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    readyDialog->setLayout(layout);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [readyDialog, label, timer]()
    {
        static int countdown = 3;
        countdown--;
        label->setText(QString::number(countdown));
        if (countdown <= 0)
        {
            readyDialog->accept();
            timer->deleteLater();
        }
    });


    timer->start(1000);

    if (readyDialog->exec() == QDialog::Rejected)
    {
        QApplication::quit();
        return;
    }

    initiateGame();
    //readyDialog->exec();
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

bool SnakeClient::_validPort(const QString& port)
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
