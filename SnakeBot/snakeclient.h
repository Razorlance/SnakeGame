#ifndef SNAKECLIENT_H
#define SNAKECLIENT_H

#include <QApplication>
#include <QInputDialog>
#include <QCloseEvent>
#include <QFormLayout>
#include <QListWidget>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTcpSocket>
#include <QComboBox>
#include <QKeyEvent>
#include <QLineEdit>
#include <QPainter>
#include <QSpinBox>
#include <QScreen>
#include <QVector>
#include <QWidget>
#include <QColor>
#include <QDebug>
#include <QPoint>
#include <QFont>
#include <QSize>
#include <QTime>
#include <QDir>

#include "ui_snakeclient.h"

using namespace std;

class SnakeClient : public QMainWindow
{
    Q_OBJECT

public:
    SnakeClient(QWidget* parent = nullptr);
    ~SnakeClient();

public slots:
    void slotReadyRead();

protected:
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
    static const int _HEIGHT = 25;
    static const int _FIELD_WIDTH = 25;  // The number of points
    static const int _FIELD_HEIGHT = 25;
    static const int _DELAY = 500;

    int _timer;
    int _score = 0;
    bool _stillGame;
    bool _await = false;

    QVector<QPoint> _fruits;
    QString _input;
    QString _snakeName;
    QString _mode;
    int _type;
    QVector<QPoint> _convertDots(const QStringList&);
    QVector<QPoint> _convertEnemyDots(const QStringList&);

    void _drawSnake();
    void _step();
    void _gameOver();
    void _startClient();
    void _connectToServer();
    void _wrongServer();
    void _sendToServer();
    void _modeOption(QComboBox*, QLineEdit*, QLabel*, QComboBox*, QLabel*);
    void _validName(QLineEdit*, QPushButton*);
    void _validIP(QLineEdit*, QPushButton*);
    void _validPort(QLineEdit*, QPushButton*);
    void _countDownDialog();

    bool _validIP(const QString&);
    bool _validPort(const QString&);
    bool _isNumber(const QString&);
    bool _bot();


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