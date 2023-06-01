#ifndef SNAKE_H
#define SNAKE_H

#include <QDebug>
#include <QDir>
#include <QObject>
#include <QPoint>
#include <QSize>
#include <QTcpServer>
#include <QTime>
#include <QVector>

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
    QString _snakeName;
    int _port;
    int _id;

    // The size of a field
    static const int _WIDTH = 25;  // The size of points
    static const int _HEIGHT = 24;
    static const int _FIELD_WIDTH = 25;  // The number of points
    static const int _FIELD_HEIGHT = 25;

    QVector<QPoint> _homeDots;
    QVector<QPoint> _enemyDots;
    QMap<int, QVector<QPoint>> _enemiesDots;
};

#endif  // SNAKE_H
