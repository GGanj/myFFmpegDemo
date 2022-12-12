#include "qplayer.h"
#include "ui_qplayer.h"

QPlayer::QPlayer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QPlayer)
{
    ui->setupUi(this);

    int width = 800;
    int height = 600;
    HWND hWnd = (HWND)this->QWidget::winId();
    myGLRender.Begin(hWnd, GetDC(hWnd), width, height);
    myFFmpeg = new FFmpegRender("demo.mp4", width, height);
    myFFmpeg->initRender();
    myFFmpeg->initCodec();
    myFFmpeg->initScale();

    QObject::connect(&myTimer, SIGNAL(timeout()), this, SLOT(render()));
    //_timer.setInterval(1);
    myTimer.start(1);
}

QPlayer::~QPlayer()
{
    delete ui;
    myGLRender.OpenGLEnd();
}

void QPlayer::render() {
    auto func = std::bind(&OpenGLRender::render, myGLRender, std::placeholders::_1);
    myFFmpeg->startRender(func);
}

