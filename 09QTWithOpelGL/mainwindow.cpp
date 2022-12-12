#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    pQplayer = new QPlayer(this);
    pQplayer->resize(800, 600);
    pQplayer->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

