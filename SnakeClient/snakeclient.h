#ifndef SNAKECLIENT_H
#define SNAKECLIENT_H

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QInputDialog>
#include <QKeyEvent>
#include <QMainWindow>
#include <QMessageBox>
#include <QPainter>
#include <QPoint>
#include <QScreen>
#include <QSize>
#include <QTcpSocket>
#include <QTime>
#include <QVector>
#include <QWidget>

#include "ui_snakeclient.h"

// QT_BEGIN_NAMESPACE
// namespace Ui
//{
// class SnakeClient;
//}
using namespace std;
// QT_END_NAMESPACE

class SnakeClient : public QMainWindow
{
    Q_OBJECT

   public:
    SnakeClient(QWidget* parent = nullptr);
    ~SnakeClient();

   public slots:
    void slotReadyRead();
    QVector<QPoint> getMap();

   protected:
    void keyPressEvent(QKeyEvent* event) override;
    //    void timerEvent(QTimerEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

   private:
    Ui::SnakeClient* ui;

    QTcpSocket* socket;
    QByteArray Data;

    // The size of a field
    static const int _width = 25;  // The size of points
    static const int _height = 24;
    static const int _field_width = 25;  // The number of points
    static const int _field_height = 25;
    static const int _delay = 500;

    int _timer;
    int _score = 0;

    QString _input;
    bool _stillGame;
    bool _await = false;
    QVector<QPoint> _homeDots;
    QVector<QPoint> _enemyDots;
    QPoint _fruitPos = QPoint(0, 0);

    QString convertToString(QVector<QPoint>&);
    QVector<QPoint> convertToDots(QStringList&);
    void initiateGame();
    void drawSnake();
    void locateFruit();
    void move();
    void step();
    //    void checkBoundary();
    void gameOver();
    void eatFruit();

    void SendToServer(QVector<QPoint> _dots);

    enum Directions
    {
        left,
        right,
        up,
        down
    };

    Directions _direction;
};
#endif  // SNAKECLIENT_H
