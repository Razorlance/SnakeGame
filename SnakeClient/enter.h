#ifndef ENTER_H
#define ENTER_H

#include <QMessageBox>
#include <QWidget>

#include "snakeclient.h"

namespace Ui
{
class enter;
}

class enter : public QWidget
{
    Q_OBJECT

   public:
    explicit enter(QWidget *parent = nullptr);
    ~enter();

   private slots:

    void on_PlayerButton_clicked();

    void on_CancelButton_clicked();

    void on_ViewerButton_clicked();

   private:
    Ui::enter *ui;
    SnakeClient w;
    bool ValidIP(QString ip);
    bool ValidPort(int port);
    bool isNumber(QString str);
};

#endif  // ENTER_H
