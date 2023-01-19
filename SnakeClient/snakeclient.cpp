#include "snakeclient.h"
#include "./ui_snakeclient.h"

#include <QDebug>
#include <QMessageBox>
#include <QInputDialog>
#include <QDir>
#include <QSize>
#include <SFML/Graphics.hpp>

SnakeClient::SnakeClient(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SnakeClient)
{
    // create the window
        sf::RenderWindow window(sf::VideoMode(600, 600), "SFML WORKS!!!");
        sf::CircleShape shape(100.f);
        shape.setFillColor(sf::Color::Green);

        // run the program as long as the window is open
        while (window.isOpen())
        {
            // check all the window's events that were triggered since the last iteration of the loop
            sf::Event event;
            while (window.pollEvent(event))
            {
                // "close requested" event: we close the window
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            // clear the window with black color
            window.clear(sf::Color::Black);

            // draw everything here...
            window.draw(shape);

            // end the current frame
            window.display();
        }

    ui->setupUi(this);
    this->resize(_width * field_width, _height * field_height);
    bool ok;
    //QInputDialog d;
    //d.setPos;
    QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                         tr("User name:"), QLineEdit::Normal,
                                         QDir::home().dirName(), &ok);
    if (ok && !text.isEmpty())
        ui->textLabel->setText(text);
}

SnakeClient::~SnakeClient()
{
    delete ui;
}



//doesn't work(((((((((
void SnakeClient::keyPressEvent(QKeyEvent *event)
{
    qDebug() << "works2";
    /*int key = event->key();
    qDebug() << event->key();
    qDebug() << "hello";

    if (event->key() == Qt::Key_Up)
    {
        qDebug() << "13";
    }*/
    switch(event->key())
        {
            case Qt::Key_Up:
                qDebug() << "Key Up pressed";
                break;

            case Qt::Key_Right:
                qDebug() << "Key Right pressed";
                break;
            case Qt::Key_Down:
                qDebug() << "Key Down pressed";
                break;

            case Qt::Key_Left:
                qDebug() << "Key Left pressed";
                break;

            /*default:
                qDebug() << "Key pressed:" << event->key();
                break;*/
        }

}
