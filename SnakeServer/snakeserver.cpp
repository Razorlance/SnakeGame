#include "snakeserver.h"
#include "./ui_snakeserver.h"

SnakeServer::SnakeServer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SnakeServer)
{
    ui->setupUi(this);
}

SnakeServer::~SnakeServer()
{
    delete ui;
}

