#include "enter.h"

#include <QVector>

#include "ui_enter.h"

Enter::Enter(QWidget* parent) : QWidget(parent)
                              , _ui(new Ui::enter)
{
    _ui->setupUi(this);
    SnakeClient w;
}

Enter::~Enter() { delete _ui; }

// DONE check whether ip and port are correct (ip - от нуля до 255 каждая
// ячейка, port - от нуля до ...) DONE if not QMessageBox / QWarning

bool Enter::_isNumber(const QString& str)
{
    if (str == "0")
        return true;
    for (const QString& i : str)
    {
        if (!isdigit(i.toInt()))
            return false;
    }
    return true;
}

bool Enter::_validIP(const QString& ip)
{
    QVector<QString> v = ip.split('.');
    if (v.size() != 4)
        return false;
    for (QString &str : v)
        if (str.toInt() > 255 || str.toInt() < 0 || not(_isNumber(str)))
            return false;
    return true;
}

bool Enter::_validPort(int port)
{
    if (port >= 0 && port <= 65535)
        return true;
    return false;
}

void Enter::on_PlayerButton_clicked()
{
    QMessageBox msgBox;
    if ((_validIP(_ui->ip->displayText()) && _validPort(_ui->port->text().toInt())))
    {
        _w.connectToServer(_ui->ip->displayText(),
                           _ui->port->text().toInt(),
                           _ui->playerName->text());
        _w.show();
        this->close();
    }
    else
    {
        msgBox.setText("The IP address or Port is not correct");
        msgBox.exec();
    }
    qDebug() << "connecting";
}

void Enter::on_CancelButton_clicked() { this->hide(); }

// NEEDED TO REWRITE: WHAT TO DO WHEN IT IS A VIEWER
void Enter::on_ViewerButton_clicked()
{
    QMessageBox msgBox;
    if ((_validIP(_ui->ip->displayText()) && _validPort(_ui->port->text().toInt())))
        _w.connectToServer(_ui->ip->displayText(),
                           _ui->port->text().toInt(),
                           _ui->playerName->text());
    else
    {
        msgBox.setText("The IP address or Port is not correct");
        msgBox.exec();
    }
    qDebug() << "connecting";
}
