#pragma once
#include <Windows.h>
#include "GL/glew.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "FFmpegRender.h"

#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "Gdi32.lib")
#pragma comment(lib, "User32.lib")

class OpenGLRender
{
public:
    OpenGLRender();
    ~OpenGLRender();
    bool Begin( HWND hWnd,HDC hDC, int width, int height);
    void OpenGLEnd();
    void Present();
    void render(AVFrame* pRGBFrame);
    void createTexture();
public:
    int _format;
    //  窗口句柄
    HWND _hWnd;
    //  绘制设备上下文
    HDC _hDC;
    //  OpenGL上下文
    HGLRC _hRC;

    int playerWidth;
    int playerHeight;
    GLuint myTextrueID = -1;
    FFmpegRender* pMyFFmpeg = nullptr;
};

