#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    pSubWind = new SubWindow(this);
    pSubWind->resize(720, 480);
    pSubWind->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

