#ifndef SNAKE_H
#define SNAKE_H

#include <QDebug>
#include <QDir>
//#include <QKeyEvent>
//#include <QMainWindow>
//#include <QMessageBox>
#include <QObject>
//#include <QPainter>
#include <QPoint>
//#include <QScreen>
#include <QSize>
#include <QTime>
#include <QVector>
//#include <QWidget>

// QT_BEGIN_NAMESPACE
// namespace Ui
//{
// class SnakeClient;
//}
// using namespace std;
// QT_END_NAMESPACE

class Snake
{
   public:
    Snake();
    ~Snake();

   protected:
    void keyPressEvent(QEvent* event);
    void timerEvent(QTimerEvent* event);

   private:
    //    Ui::SnakeClient* ui;

    // The size of a field
    static const int _width = 25;  // The size of points
    static const int _height = 24;
    static const int _field_width = 25;  // The number of points
    static const int _field_height = 25;
    static const int _delay = 150;

    int _timer;
    int _score = 0;

    bool _stillGame;

    QVector<QPoint> _dots;
    QPoint _fruitPos;

    void initiateGame();
    void drawSnake();
    void locateFruit();
    void move();
    void checkBoundary();
    void gameOver();
    void eatFruit();

    enum Directions
    {
        left,
        right,
        up,
        down
    };

    Directions _direction;
};

#endif  // SNAKE_H
