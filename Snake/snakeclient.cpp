#include "snakeclient.h"
#include "./ui_snakeclient.h"

SnakeClient::SnakeClient(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SnakeClient)
{
    ui->setupUi(this);
}

SnakeClient::~SnakeClient()
{
    delete ui;
}

