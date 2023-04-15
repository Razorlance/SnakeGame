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
#include <QTcpServer>
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

enum Directions
{
    left = 0,
    right = 1,
    up = 2,
    down = 3
};

class Snake
{
   public:
    Snake();
    Snake(const Snake& other) { direction = other.direction; }
    ~Snake();
    Directions direction;
    QTcpSocket* socket;

   public slots:
    void slotReadyRead();
    QVector<QPoint> getMap();

    QByteArray _data;
    QString _ip;
    int _port;

    // The size of a field
    static const int _WIDTH = 25;  // The size of points
    static const int _HEIGHT = 24;
    static const int _FIELD_WIDTH = 25;  // The number of points
    static const int _FIELD_HEIGHT = 25;
    static const int _DELAY = 500;

    int _timer;
    int _score = 0;

    QString _input;
    QString _snakeName;
    bool _stillGame;
    bool _await = false;
    QVector<QPoint> _homeDots;
    QVector<QPoint> _enemyDots;
    QPoint _fruitPos = QPoint(0, 0);

    QString _convertToString(const QVector<QPoint>&);
    QVector<QPoint> _convertToDots(const QStringList&);
    //    void _initiateGame();
    //    void _drawSnake();
    //    void _locateFruit();
    void _move();
    void _step();
    //    void _gameOver();
    //    void _eatFruit();
};

#endif  // SNAKE_H
