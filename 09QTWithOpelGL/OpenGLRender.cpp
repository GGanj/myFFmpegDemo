#include "OpenGLRender.h"

OpenGLRender::OpenGLRender()
{
   _format     =   0;
   _hWnd       =   0;
   _hDC        =   0;
   _hRC        =   0;
}

OpenGLRender::~OpenGLRender()
{
   Present();
}

//////////////////////////////////////
/////////////////初始化OpenGL
bool OpenGLRender::Begin( HWND hWnd,HDC hDC, int width, int height)
{
   _hWnd   =   hWnd;
   _hDC    =   hDC;
   playerWidth = width;
   playerHeight = height;
   unsigned PixelFormat;
   PIXELFORMATDESCRIPTOR pfd =
   {
       sizeof(PIXELFORMATDESCRIPTOR),
       1,
       PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
       PFD_TYPE_RGBA,
       32,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       24,
       8,
       0,
       PFD_MAIN_PLANE,
       0,
       0,
       0,
       0
   };
   if(_format == 0)
   {
       PixelFormat = ChoosePixelFormat( _hDC, &pfd );
   }
   else
   {
       PixelFormat = _format;
   }
   if(!SetPixelFormat( _hDC, PixelFormat, &pfd ))
   {
       return false;
   }
   _hRC = wglCreateContext( _hDC );
   if(!wglMakeCurrent( _hDC, _hRC ))
   {
       return false;
   }
   createTexture();
   return true;
}

//////////////////////////////////////
///////////////////结束OpenGL
void OpenGLRender::OpenGLEnd()
{
   if( _hRC != NULL )
   {
       wglMakeCurrent( NULL, NULL );
       wglDeleteContext( _hRC );
       _hRC = NULL;
   }
   if( _hDC != NULL )
   {
       ReleaseDC( _hWnd, _hDC );
       _hDC = NULL;
   }
}

////////////////////////////////
//////////// 交换缓冲区

void OpenGLRender::Present()
{
   SwapBuffers(_hDC);
}

void OpenGLRender::createTexture() {
    glClearColor(1, 0, 0, 1);
    SwapBuffers(_hDC);
//    glOrtho(0, playerWidth, playerHeight, 0, -100, 100);
//    // 生成纹理索引, 参数为: 生成个数，以及纹理ID
//    glGenTextures(1, &myTextrueID);
//    // 将有目标纹理绑定到已生成的纹理ID上
//    // 目标纹理只能为GL_TEXTURE_1D、GL_TEXTURE_2D、GL_TEXTURE_3D或GL_TEXTURE_CUBE_MAP
//    glBindTexture(GL_TEXTURE_2D, myTextrueID);
//    // 纹理过滤函数, 图像从纹理空间映射到帧缓冲空间时可能会出现失真, 使用该函数能够确定如何将纹理映射为像素
//    // 参数1: 操作的目标纹理类型
//    // 参数2: 指定响应模式,
//    //        GL_TEXTURE_WRAP_S        S方向上的贴图模式,
//    //        GL_TEXTURE_WRAP_T        T方向上的贴图模式
//    // 	      GL_TEXTURE_MAG_FILTER    放大过滤
//    //        GL_TEXTURE_MIN_FILTER    缩小过滤
//    // 参数3: 指定应对该模式的方法: ref https://blog.csdn.net/dominiced/article/details/79640058
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

//    // 生成一个2D纹理图像, 参数为:
//    //    目标纹理类型(必须为GL_TEXTURE_2D), 执行细节级别, 纹理颜色组件,
//    //    纹理图像宽度, 纹理图像高度, 纹理边框宽度,
//    //    像素数据的颜色格式, 像素数据类型, 指向图像数据的指针
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, playerWidth, playerHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
}

void OpenGLRender::render(AVFrame* pRGBFrame) {
    // 清除当前缓冲区内的值
    glClear(GL_COLOR_BUFFER_BIT);

    glm::vec2 vertex[] = {
        glm::vec2 {0.0f, 0.0f},
        glm::vec2 {0.0f, playerHeight},
        glm::vec2 {playerWidth, playerHeight},
        glm::vec2 {playerWidth, 0.0f}
    };

    glm::vec2 texture[] = {
        glm::vec2 {0.0f, 0.0f},
        glm::vec2 {0.0f, 1.0f},
        glm::vec2 {1.0f, 1.0f},
        glm::vec2 {1.0f, 0.0f}
    };

    // 将有目标纹理绑定到已生成的纹理ID上
    glBindTexture(GL_TEXTURE_2D, myTextrueID);

    // 指定二位纹理的子图像
    //    目标纹理类型(必须为GL_TEXTURE_2D), 执行细节级别, x偏移值, y偏移值
    //    子图宽度, 子图像高度, 像素数据的颜色格式, 像素数据类型, 指向图像数据的指针
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
        playerWidth, playerHeight, GL_RGB, GL_UNSIGNED_BYTE, pRGBFrame->data[0]);
    // 启用二维纹理
    glEnable(GL_TEXTURE_2D);
    // 开始绘制四边形
    glBegin(GL_QUADS);

    for (size_t i = 0; i < sizeof(vertex) / sizeof(vertex[0]); i++) {
        // 指定纹理的坐标,
        // 参数1: 0为纹理左侧, 0.5为纹理中点, 1.0为纹理右侧
        // 参数2: 0为纹理底部, 0.5为纹理中点, 1.0为纹理顶部
        glTexCoord2fv(glm::value_ptr(texture[i]));
        // 指定点的坐标,
        glVertex2fv(glm::value_ptr(vertex[i]));
    }

    // 结束绘制
    glEnd();
    // 交换缓冲区
    SwapBuffers(_hDC);
}
