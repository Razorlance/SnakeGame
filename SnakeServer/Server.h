#ifndef SERVER_H
#define SERVER_H

#include <QGuiApplication>
#include <QApplication>
#include <QFormLayout>
#include <QPushButton>
#include <QTcpServer>
#include <QTcpSocket>
#include <QLineEdit>
#include <QComboBox>
#include <QDialog>
#include <QScreen>
#include <QVector>
#include <QWidget>
#include <QQueue>
#include <QTimer>
#include <QTime>

#include "snake.h"

class Server : public QTcpServer
{
    Q_OBJECT
   public:
    Server();
    // Write a destructor for a server
    //    ~Server();

    QTcpSocket *socket;

   protected:
    void timerEvent(QTimerEvent *event) override;

   private:
    QMap<qintptr, QTcpSocket *> _Sockets;
    QMap<qintptr, QVector<QPoint>> _Dots;
    QMap<qintptr, Snake *> _PlayerList;
    QMap<qintptr, QTcpSocket *> _ViewerList;
    QSet<qintptr> _count;
    QPoint _fruitPos;
    QVector<QPoint> _fruits;
    QSet<int> _crashed;

    QTimer *_gameTimer;
    int _timer;
    int _gameTime;
    int _type;
    int _port;
    int _playerCount = 0;
    int _botCount = 0;
    int _viewerCount = 0;  // ?
    int _totalCount;
    static const int _delay = 500;
    static const int _width = 25;  // The size of points
    static const int _height = 25;
    static const int _field_width = 22;  // The number of points
    static const int _field_height = 25;

    Snake _Player1;
    Snake _Player2;
    Snake _Player3;
    Snake _Player4;

    QQueue<Snake *> _Players;

    QString _convertToString(QVector<QPoint> &dots);
    QByteArray _Data;
    quint16 _nextBlockSize;

    void _SendData();
    void _SendData(QString str);
    void _SendClientBack(QTcpSocket *clientSocket);
    void _checkBoundary();
    void _locateFruit(int);
    void _initiateGame();
    void _eatFruit();
    void _endGame();
    void _move();

   public slots:
    void incomingConnection(qintptr SocketDescriptor) override;
    void slotReadyRead();
//   private slots:
//    void timer_function();
};

#endif  // SERVER_H
