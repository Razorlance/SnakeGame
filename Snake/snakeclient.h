#ifndef SNAKECLIENT_H
#define SNAKECLIENT_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class SnakeClient; }
QT_END_NAMESPACE

class SnakeClient : public QMainWindow
{
    Q_OBJECT

public:
    SnakeClient(QWidget *parent = nullptr);
    ~SnakeClient();

private:
    Ui::SnakeClient *ui;
};
#endif // SNAKECLIENT_H
