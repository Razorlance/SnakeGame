#ifndef SNAKESERVER_H
#define SNAKESERVER_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class SnakeServer; }
QT_END_NAMESPACE

class SnakeServer : public QMainWindow
{
    Q_OBJECT

public:
    SnakeServer(QWidget *parent = nullptr);
    ~SnakeServer();

private:
    Ui::SnakeServer *ui;
};
#endif // SNAKESERVER_H
