#ifndef ENTER_H
#define ENTER_H

#include <QMessageBox>
#include <QWidget>

#include "snakeclient.h"

namespace Ui
{
    class enter;
}

class Enter : public QWidget
{
    Q_OBJECT

   public:
    explicit Enter(QWidget* parent = nullptr);
    ~Enter();

   private slots:
    void on_PlayerButton_clicked();
    void on_CancelButton_clicked();
    void on_ViewerButton_clicked();

   private:
    Ui::enter* _ui;
    SnakeClient _w;
    bool _validIP(const QString& ip);
    bool _validPort(int port);
    bool _isNumber(const QString& str);
};

#endif  // ENTER_H
