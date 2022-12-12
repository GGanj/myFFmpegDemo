#pragma once
#include <Windows.h>
#include <stdlib.h>
#include "GL/glew.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Shader430.h"
#include "FFmpegRender.h"

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "Gdi32.lib")
#pragma comment(lib, "User32.lib")

class OpenGLRender
{
protected:
    bool begin();
    void openGLEnd();
    void present();
public:
    OpenGLRender();
    ~OpenGLRender();
    GLuint createTexture(int w, int h);
    void updateTexture(FrameInfor* infor);
    void initGLRender(HWND hWnd,HDC hDC, int width, int height, FFmpegRender* p);
    void renderFrame();
    void startRender();
    void stopRender();

public:
    int _format;
    //  窗口句柄
    HWND _hWnd;
    //  绘制设备上下文
    HDC _hDC;
    //  OpenGL上下文
    HGLRC _hRC;

    bool isStop = false;

    int playerWidth;
    int playerHeight;

    GLuint textureY = -1, textureU = -1, textureV = -1;
    Shader430 myShader430;
    FFmpegRender* pMyFFmpeg = nullptr;
};

