#include "enter.h"

#include <QVector>

#include "ui_enter.h"

enter::enter(QWidget *parent) : QWidget(parent), ui(new Ui::enter)
{
    ui->setupUi(this);
    SnakeClient w;
}

enter::~enter() { delete ui; }

// DONE check whether ip and port are correct (ip - от нуля до 255 каждая
// ячейка, port - от нуля до ...) DONE if not QMessageBox / QWarning

bool enter::isNumber(QString str)
{
    //  if str = "abc" (consists of letters) then str.toInt() = 0
    if (str == "0")
        return true;
    for (QString i : str)
    {
        if (not(i.toInt() > 0 && i.toInt() <= 9))
            return false;
    }
    return true;
}

bool enter::ValidIP(QString ip)
{
    QVector<QString> v = ip.split('.');
    if (v.size() != 4)
        return false;
    for (QString &str : v)
    {
        // qDebug() << str.toInt();
        if (str.toInt() > 255 || str.toInt() < 0 || not(isNumber(str)))
            return false;
    }
    return true;

    /*QHostAddress address(ip);
    if (QAbstractSocket::IPv4Protocol == address.protocol())
        return true;
    else if (QAbstractSocket::IPv6Protocol == address.protocol())
        return true;
    else
        return false;*/
}

bool enter::ValidPort(int port)
{
    if (port >= 0 && port <= 65535)
        return true;
    return false;
}

void enter::on_PlayerButton_clicked()
{
    // w.ConnectToServer("127.0.0.1", ui->port->text().toInt()); // ip
    QMessageBox msgBox;
    if ((ValidIP(ui->ip->displayText()) && ValidPort(ui->port->text().toInt())))
    {
        w.ConnectToServer(ui->ip->displayText(), ui->port->text().toInt(),
                          ui->playerName->text());

        w.show();
        this->close();
    }
    else
    {
        msgBox.setText("The IP address or Port is not correct");
        msgBox.exec();
    }
    qDebug() << "connecting";
}

void enter::on_CancelButton_clicked() { this->hide(); }

// NEEDED TO REWRITE: WHAT TO DO WHEN IT IS A VIEWER
void enter::on_ViewerButton_clicked()
{
    QMessageBox msgBox;
    if ((ValidIP(ui->ip->displayText()) && ValidPort(ui->port->text().toInt())))
        w.ConnectToServer(ui->ip->displayText(), ui->port->text().toInt(),
                          ui->playerName->text());
    else
    {
        msgBox.setText("The IP address or Port is not correct");
        msgBox.exec();
    }
    qDebug() << "connecting";
}
