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

using namespace std;

class SnakeClient : public QMainWindow
{
    Q_OBJECT

   public:
    SnakeClient(QWidget* parent = nullptr);
    ~SnakeClient();
    void connectToServer(const QString& ip, int port, const QString& SnakeName);

   public slots:
    void slotReadyRead();
    QVector<QPoint> getMap();

   protected:
    void keyPressEvent(QKeyEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

   private:
    Ui::SnakeClient* _ui;
    QTcpSocket* _socket;
    QByteArray _data;
    QString _ip;
    int _port;

    QVector<QPoint> _homeDots;
    QVector<QPoint> _enemyDots;

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

    QPoint _fruitPos = QPoint(0, 0);

    QString _convertToString(const QVector<QPoint>&);
    QVector<QPoint> _convertToDots(const QStringList&);
    void _initiateGame();
    void _drawSnake();
    void _locateFruit();
    void _move();
    void _step();
    void _gameOver();
    void _eatFruit();

    void _sendToServer(const QVector<QPoint>& _dots);

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
