#ifndef SNAKECLIENT_H
#define SNAKECLIENT_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class SnakeClient; }
QT_END_NAMESPACE

class SnakeClient : public QMainWindow
{
    Q_OBJECT

public:
    SnakeClient(QWidget *parent = nullptr);
    ~SnakeClient();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    Ui::SnakeClient *ui;

    // размер поля
    static const int _width = 25; // размеры точек
    static const int _height = 25;
    static const int field_width = 25; // количество точек
    static const int field_height = 25;
};
#endif // SNAKECLIENT_H
