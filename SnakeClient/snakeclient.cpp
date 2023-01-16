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
    this->resize(_width * field_width, _height * field_height);
    bool ok;
    //QInputDialog d;
    //d.setPos;
    QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                         tr("User name:"), QLineEdit::Normal,
                                         QDir::home().dirName(), &ok);
    if (ok && !text.isEmpty())
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
