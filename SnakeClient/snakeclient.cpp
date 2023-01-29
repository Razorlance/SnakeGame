#include "snakeclient.h"
#include "./ui_snakeclient.h"

#include <QDebug>
#include <QMessageBox>
#include <QInputDialog>
#include <QDir>
#include <QSize>

SnakeClient::SnakeClient(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SnakeClient)
{   
    ui->setupUi(this);
    this->resize(_width * _field_width, _height * _field_height);
    this->setWindowTitle("Snake Game");
    bool ok;
    //QInputDialog d;
    //d.setPos;
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
        text = "Player 1";
    ui->textLabel->setText(text);
}

SnakeClient::~SnakeClient()
{
    delete ui;
}



//doesn't work(((((((((
void SnakeClient::keyPressEvent(QKeyEvent *event)
{
    qDebug() << "works2";
    /*int key = event->key();
    qDebug() << event->key();
    qDebug() << "hello";

    if (event->key() == Qt::Key_Up)
    {
        qDebug() << "13";
    }*/
    switch(event->key())
        {
            case Qt::Key_Up:
                qDebug() << "Key Up pressed";
                break;

            case Qt::Key_Right:
                qDebug() << "Key Right pressed";
                break;
            case Qt::Key_Down:
                qDebug() << "Key Down pressed";
                break;

            case Qt::Key_Left:
                qDebug() << "Key Left pressed";
                break;

            /*default:
                qDebug() << "Key pressed:" << event->key();
                break;*/
        }

}

void SnakeClient::initiateGame()
{
    _stillGame = true;
    _dots.resize(3);

    for (size_t i = 0; i < _dots.size(); i++)
    {
        _dots[i].rx() = _dots.size() - i;
        _dots[i].ry() = 0;
    }

    _timer = startTimer(_delay);
}
