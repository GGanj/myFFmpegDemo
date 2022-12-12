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
   present();
}

//////////////////////////////////////
/////////////////初始化OpenGL
bool OpenGLRender::begin()
{
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
   return true;
}

//////////////////////////////////////
///////////////////结束OpenGL
void OpenGLRender::openGLEnd()
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

void OpenGLRender::present()
{
   SwapBuffers(_hDC);
}


GLuint OpenGLRender::createTexture(int w, int h) {
    GLuint myTextureID = -1;
//    glOrtho(0, playerWidth, playerHeight, 0, -100, 100);
    // 生成纹理索引, 参数为: 生成个数，以及纹理ID
    glGenTextures(1, &myTextureID);
    // 将有目标纹理绑定到已生成的纹理ID上
    // 目标纹理只能为GL_TEXTURE_1D、GL_TEXTURE_2D、GL_TEXTURE_3D或GL_TEXTURE_CUBE_MAP
    glBindTexture(GL_TEXTURE_2D, myTextureID);
    // 纹理过滤函数, 图像从纹理空间映射到帧缓冲空间时可能会出现失真, 使用该函数能够确定如何将纹理映射为像素
    // 参数1: 操作的目标纹理类型
    // 参数2: 指定响应模式,
    //        GL_TEXTURE_WRAP_S        S方向上的贴图模式,
    //        GL_TEXTURE_WRAP_T        T方向上的贴图模式
    // 	      GL_TEXTURE_MAG_FILTER    放大过滤
    //        GL_TEXTURE_MIN_FILTER    缩小过滤
    // 参数3: 指定应对该模式的方法: ref https://blog.csdn.net/dominiced/article/details/79640058
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // 生成一个2D纹理图像, 参数为:
    //    目标纹理类型(必须为GL_TEXTURE_2D), 执行细节级别, 纹理颜色组件,
    //    纹理图像宽度, 纹理图像高度, 纹理边框宽度,
    //    像素数据的颜色格式, 像素数据类型, 指向图像数据的指针
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, w, h, 0, GL_ALPHA, GL_UNSIGNED_BYTE, 0);

    return myTextureID;
}

void OpenGLRender::updateTexture(FrameInfor *infor) {
    glBindTexture(GL_TEXTURE_2D, textureY);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, playerWidth, playerHeight, GL_ALPHA, GL_UNSIGNED_BYTE, infor->_data->data[0]);

    glBindTexture(GL_TEXTURE_2D, textureU);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, playerWidth / 2, playerHeight / 2, GL_ALPHA, GL_UNSIGNED_BYTE, infor->_data->data[1]);

    glBindTexture(GL_TEXTURE_2D, textureV);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, playerWidth / 2, playerHeight / 2, GL_ALPHA, GL_UNSIGNED_BYTE, infor->_data->data[2]);
}

void OpenGLRender::initGLRender(HWND hWnd,HDC hDC, int width, int height, FFmpegRender* p) {
    _hWnd   =   hWnd;
    _hDC    =   hDC;
    playerWidth = width;
    playerHeight = height;
    pMyFFmpeg = p;

    // 生成 Opengl Context 环境, 以便调用glewInit
    begin();
    // 清除缓冲区颜色，并设置为红色
    glClearColor(1, 0, 0, 1);
    SwapBuffers(_hDC);

    GLenum glew_ret = glewInit();
    if(glew_ret != GLEW_OK) {
        std::cout << glGetError() << std::endl;
        return;
    }

    textureY = createTexture(playerWidth, playerHeight);
    textureU = createTexture(playerWidth / 2, playerHeight / 2);
    textureV = createTexture(playerWidth / 2, playerHeight / 2);

    // TODO init shader
    myShader430.initialize();
}

void OpenGLRender::renderFrame() {
    // 清除当前缓冲区内的值
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureY);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureU);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textureV);

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

    myShader430.begin();

    glm::mat4 orth  = glm::ortho<float>(0, playerWidth, playerHeight, 0, -100, 100);
    glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(orth));

    glUniform1i(myShader430._textureY, 0);
    glUniform1i(myShader430._textureU, 1);
    glUniform1i(myShader430._textureV, 2);
    {
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), vertex);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), texture);
        glDrawArrays(GL_QUADS, 0, 4);
    }
    myShader430.end();

    // 交换缓冲区
    present();
}

void OpenGLRender::startRender() {
    auto func = [&](FrameInfor* pInfo){
        if(isStop != true) {
            updateTexture(pInfo);
            renderFrame();
        }
    };
    pMyFFmpeg->startRender(func);
}

void OpenGLRender::stopRender() {
    isStop = true;
}
