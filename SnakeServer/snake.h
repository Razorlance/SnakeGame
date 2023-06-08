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
    ~Snake();
    Directions direction;
    QTcpSocket* socket;

public slots:
    QString _snakeName;
    int _id;
    int _crashed = 0;
    QVector<QPoint> _homeDots;
    QMap<int, QVector<QPoint>> _enemiesDots;
};

#endif  // SNAKE_H
