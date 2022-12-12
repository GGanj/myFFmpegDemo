#ifndef SUBWINDOW_H
#define SUBWINDOW_H

#include <QWidget>
#include <QTimer>

#include "OpenGLRender.h"
#include "FFmpegRender.h"

namespace Ui {
class SubWindow;
}

class SubWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SubWindow(QWidget *parent = nullptr);
    ~SubWindow();

public slots:
    void render();

private:
    Ui::SubWindow *ui;
    QTimer myTimer;
    FFmpegRender* pFF = nullptr;
    OpenGLRender* pOg = nullptr;
};

#endif // SUBWINDOW_H
