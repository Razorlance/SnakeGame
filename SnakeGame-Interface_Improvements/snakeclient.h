#ifndef SNAKECLIENT_H
#define SNAKECLIENT_H

#include <QApplication>
#include <QInputDialog>
#include <QListWidget>
#include <QMessageBox>
#include <QMainWindow>
#include <QKeyEvent>
#include <QPainter>
#include <QWidget>
#include <QScreen>
#include <QVector>
#include <QPoint>
#include <QDebug>
#include <QSize>
#include <QTime>
#include <QDir>
#include <QComboBox>
#include <QPushButton>
#include <QFormLayout>
#include <QLineEdit>
#include <QFont>
#include <QTimer>
#include <QVBoxLayout>

QT_BEGIN_NAMESPACE
namespace Ui { class SnakeClient; }
using namespace std;
QT_END_NAMESPACE

class SnakeClient : public QMainWindow
{
    Q_OBJECT

public:
    SnakeClient(QWidget *parent = nullptr);
    ~SnakeClient();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void timerEvent(QTimerEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    Ui::SnakeClient *ui;

    // The size of a field
    static const int _width = 25; // The size of points
    static const int _height = 24;
    static const int _field_width = 20; // The number of points
    static const int _field_height = 25;
    static const int _delay = 100;

    int _timer;
    int _score = 0;

    bool _stillGame;
    bool _moveBlock;

    QString _name;
    QString _mode;

    QVector<QPoint> _dots;
    QPoint _fruitPos;

    void initiateGame();
    void drawSnake();
    void locateFruit();
    void move();
    void checkBoundary();
    void gameOver();
    void eatFruit();
    void modeOption(QComboBox *mode, QLineEdit *name, QLabel *nameLabel, QComboBox *type, QLabel *typeLabel);
    void validName(QLineEdit* name, QPushButton *button);
    void validIP(QLineEdit* ip, QPushButton *button);
    void validPort(QLineEdit* port, QPushButton *button);

    void countDownDialog();

    bool _validIP(const QString& ip);
    bool _validPort(const QString& port);
    bool _isNumber(const QString& str);

    enum Directions
    {
        left, right, up, down
    };

    Directions _direction;
};
#endif // SNAKECLIENT_H
