#ifndef QPLAYER_H
#define QPLAYER_H

#include <QWidget>
#include <QTimer>
#include "OpenGLRender.h"
#include "FFmpegRender.h"

namespace Ui {
class QPlayer;
}

class QPlayer : public QWidget
{
    Q_OBJECT

public:
    explicit QPlayer(QWidget *parent = nullptr);
    ~QPlayer();

public slots:
    void render();

private:
    Ui::QPlayer *ui;
    OpenGLRender myGLRender;
    QTimer myTimer;
    FFmpegRender* myFFmpeg = nullptr;
};

#endif // QPLAYER_H
