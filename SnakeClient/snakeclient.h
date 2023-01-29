#ifndef SNAKECLIENT_H
#define SNAKECLIENT_H

#include <QDesktopWidget>
#include <QApplication>
#include <QMainWindow>
#include <QKeyEvent>
#include <QWidget>
#include <QScreen>
#include <QVector>
#include <QPoint>

QT_BEGIN_NAMESPACE
namespace Ui { class SnakeClient; }
using namespace std;
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

    // The size of a field
    static const int _width = 25; // The size of points
    static const int _height = 25;
    static const int field_width = 25; // The number of points
    static const int field_height = 25;

    bool stillGame;

    QVector<QPoint> dots;

    void initiateGame();
};
#endif // SNAKECLIENT_H
