QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += \
    ../dependency/ffmpeg-5.1.2-full_build-shared/include/ \
    ../dependency/OpenGL/x64/include/ \
    ../dependency/OpenGL/x64/

SOURCES += \
    FFmpegRender.cpp \
    OpenGLRender.cpp \
    Shader430.cpp \
    main.cpp \
    mainwindow.cpp \
    subwindow.cpp

HEADERS += \
    FFmpegRender.h \
    OpenGLRender.h \
    Shader430.h \
    mainwindow.h \
    subwindow.h

FORMS += \
    mainwindow.ui \
    subwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

LIBS += \
    -L$${PWD}/../dependency/ffmpeg-5.1.2-full_build-shared/lib -lavformat \
    -L$${PWD}/../dependency/OpenGL/x64/lib -lglew32

DLLSRCDIR = $${PWD}/../dependency/ffmpeg-5.1.2-full_build-shared/bin/*.dll
GLSRCDIR = $${PWD}/../dependency/OpenGL/x64/bin/*.dll
MP4SRCDIR = $${PWD}/../dependency/*.mp4
CONFIG(debug, debug | release) {
  TARDIR = $${OUT_PWD}/debug
} else {
  TARDIR = $${OUT_PWD}/release
}
win32 {
  DLLSRCDIR = $$replace(DLLSRCDIR, /, \\)
  GLSRCDIR = $$replace(GLSRCDIR, /, \\)
  MP4SRCDIR = $$replace(MP4SRCDIR, /, \\)
  TARDIR = $$replace(TARDIR, /, \\)
}


QMAKE_POST_LINK += \
    $${QMAKE_COPY_FILE} $${DLLSRCDIR} $${TARDIR} \
    && $${QMAKE_COPY_FILE} $${GLSRCDIR} $${TARDIR} \
    && $${QMAKE_COPY_FILE} $${MP4SRCDIR} $${TARDIR}
