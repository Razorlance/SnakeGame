#ifndef SNAKEBOT_H
#define SNAKEBOT_H

#include <QApplication>
#include <QCloseEvent>
#include <QColor>
#include <QComboBox>
#include <QDebug>
#include <QDir>
#include <QFont>
#include <QFormLayout>
#include <QInputDialog>
#include <QKeyEvent>
#include <QLineEdit>
#include <QListWidget>
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

#include "ui_snakebot.h"

using namespace std;

class SnakeBot : public QMainWindow
{
    Q_OBJECT
    enum Directions
    {
        left = 0,
        right = 1,
        up = 2,
        down = 3
    };

   public:
    SnakeBot(QWidget* parent = nullptr);
    ~SnakeBot();

    bool stop = false;

   public slots:
    void slotReadyRead();

   protected:
    void paintEvent(QPaintEvent* event) override;

   private:
    Ui::SnakeBot* _ui;
    QTcpSocket* _socket;
    QByteArray _data;
    quint16 _nextBlockSize;
    QString _ip;
    int _port;

    QVector<QPoint> _homeDots;
    QMap<int, QVector<QPoint>> _enemiesDots;
    QMap<int, int> _enemiesCrashed;

    // The size of a field
    static const int _WIDTH = 25;  // The size of points
    static const int _HEIGHT = 25;
    static const int _FIELD_WIDTH = 25;  // The number of points
    static const int _FIELD_HEIGHT = 25;

    bool _stillGame = false;
    bool _await = false;
    int _crashed = 0;
    int _id;

    QVector<QPoint> _fruits;
    QString _input;
    QString _snakeName;
    QString _mode;
    int _type;
    QVector<QPoint> _convertFruits(const QStringList&);
    QVector<QPoint> _convertHomeDots(const QStringList&);
    QVector<QPoint> _convertEnemyDots(const QStringList&);

    void _drawSnake();
    void _step();
    void _gameOver();
    void _noWinner();
    void _oneWinner(const QString&);
    void _startClient();
    void _connectToServer();
    void _wrongServer();
    void _sendToServer();
    void _modeOption(QComboBox*, QLineEdit*, QLabel*, QComboBox*, QLabel*);
    void _validName(QLineEdit*, QPushButton*);
    void _validIP(QLineEdit*, QPushButton*);
    void _validPort(QLineEdit*, QPushButton*);
    void _countDownDialog();
    bool _bot();
    bool _checkMove(Directions tmp);

    bool _validIP(const QString&);
    bool _validPort(const QString&);
    bool _isNumber(const QString&);

    Directions _direction;

    enum Colours
    {
        blue = 0,
        red = 1,
        green = 2,
        magenta = 3
    };

    Colours _colour;
};
#endif  // SNAKEBOT_H
