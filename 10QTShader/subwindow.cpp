#include "subwindow.h"
#include "ui_subwindow.h"

SubWindow::SubWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SubWindow)
{
    ui->setupUi(this);
    int width = 720;
    int height = 480;
    pFF = new FFmpegRender("demo.mp4", width, height);
    pFF->initRender();
    pFF->initCodec();
    pFF->initScale();

    HWND hWnd = (HWND)this->QWidget::winId();
    pOg = new OpenGLRender();
    pOg->initGLRender(hWnd, GetDC(hWnd), width, height, pFF);
    QObject::connect(&myTimer, SIGNAL(timeout()), this, SLOT(render()));
    //_timer.setInterval(1);
    myTimer.start(1);
}

SubWindow::~SubWindow()
{
    delete ui;
}

void SubWindow::render() {
    pOg->startRender();
}
