#ifndef SERVER_H
#define SERVER_H

#include <QApplication>
#include <QComboBox>
#include <QDialog>
#include <QFormLayout>
#include <QGuiApplication>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QQueue>
#include <QRandomGenerator>
#include <QScreen>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTime>
#include <QTimer>
#include <QVector>
#include <QWidget>

#include "snake.h"

class Server : public QTcpServer
{
    Q_OBJECT
   public:
    Server();
    ~Server();

    bool stop;

    QTcpSocket *socket;

   protected:
    void timerEvent(QTimerEvent *event) override;

   private:
    QMap<qintptr, QTcpSocket *> _Sockets;
    QMap<qintptr, Snake *> _PlayerList;
    QMap<qintptr, QTcpSocket *> _ViewerList;
    QVector<QPoint> _fruits;
    QSet<int> _crashed;

    QTimer *_gameTimer;
    int _timer;
    int _type;
    int _port;
    int _playerCount = 0;
    int _botCount = 0;
    int _totalCount;
    int _seconds;
    bool _started = false;
    static const int _delay = 500;
    static const int _field_width = 20;  // The number of points
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
    void _SendLateViewer(QString dataToSend, QTcpSocket *socket);
    void _SendClientBack(QTcpSocket *clientSocket);
    void _checkBoundary();
    void _locateFruit(int);
    void _initiateGame();
    void _eatFruit();
    void _timesUp();
    void _endGame();
    void _move();

   public slots:
    void incomingConnection(qintptr SocketDescriptor) override;
    void slotReadyRead();
    void timer_function();
};

#endif  // SERVER_H
