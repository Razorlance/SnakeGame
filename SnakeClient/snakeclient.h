#ifndef SNAKECLIENT_H
#define SNAKECLIENT_H

#include <QApplication>
#include <QCloseEvent>
#include <QColor>
#include <QComboBox>
#include <QDebug>
#include <QDir>
#include <QFormLayout>
#include <QInputDialog>
#include <QKeyEvent>
#include <QMainWindow>
#include <QMessageBox>
#include <QPainter>
#include <QPoint>
#include <QPushButton>
#include <QScreen>
#include <QSize>
#include <QSpinBox>
#include <QTcpSocket>
#include <QTime>
#include <QVBoxLayout>
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
    void connectToServer();

   public slots:
    void slotReadyRead();

   protected:
    void keyPressEvent(QKeyEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

   private:
    Ui::SnakeClient* _ui;
    QTcpSocket* _socket;
    QByteArray _data;
    quint16 _nextBlockSize;
    QString _ip;
    int _port;

    QVector<QPoint> _homeDots;
    QMap<int, QVector<QPoint>> _enemiesDots;
    QVector<QPoint> _enemyDots;

    // The size of a field
    static const int _WIDTH = 25;  // The size of points
    static const int _HEIGHT = 24;
    static const int _FIELD_WIDTH = 25;  // The number of points
    static const int _FIELD_HEIGHT = 25;
    static const int _DELAY = 500;

    int _timer;
    int _score = 0;
    bool _stillGame;
    bool _await = false;
    int _viewer = 0;

    QPoint _fruitPos = QPoint(0, 0);
    QString _input;
    QString _snakeName;
    QString _mode;
    int _type;
    QVector<QPoint> _convertHomeDots(const QStringList&);
    QVector<QPoint> _convertEnemyDots(const QStringList&);

    void _drawSnake();
    void _step();
    void _gameOver();
    void _sendToServer();

    enum Directions
    {
        left = 0,
        right = 1,
        up = 2,
        down = 3
    };

    Directions _direction;
    Directions _enemyDirection;

    enum Colours
    {
        blue = 0,
        red = 1,
        green = 2,
        magenta = 3
    };

    Colours _colour;
};
#endif  // SNAKECLIENT_H
