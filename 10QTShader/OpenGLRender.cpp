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
/////////////////��ʼ��OpenGL
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
///////////////////����OpenGL
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
//////////// ����������

void OpenGLRender::present()
{
   SwapBuffers(_hDC);
}


GLuint OpenGLRender::createTexture(int w, int h) {
    GLuint myTextureID = -1;
//    glOrtho(0, playerWidth, playerHeight, 0, -100, 100);
    // ������������, ����Ϊ: ���ɸ������Լ�����ID
    glGenTextures(1, &myTextureID);
    // ����Ŀ������󶨵������ɵ�����ID��
    // Ŀ������ֻ��ΪGL_TEXTURE_1D��GL_TEXTURE_2D��GL_TEXTURE_3D��GL_TEXTURE_CUBE_MAP
    glBindTexture(GL_TEXTURE_2D, myTextureID);
    // ������˺���, ͼ�������ռ�ӳ�䵽֡����ռ�ʱ���ܻ����ʧ��, ʹ�øú����ܹ�ȷ����ν�����ӳ��Ϊ����
    // ����1: ������Ŀ����������
    // ����2: ָ����Ӧģʽ,
    //        GL_TEXTURE_WRAP_S        S�����ϵ���ͼģʽ,
    //        GL_TEXTURE_WRAP_T        T�����ϵ���ͼģʽ
    // 	      GL_TEXTURE_MAG_FILTER    �Ŵ����
    //        GL_TEXTURE_MIN_FILTER    ��С����
    // ����3: ָ��Ӧ�Ը�ģʽ�ķ���: ref https://blog.csdn.net/dominiced/article/details/79640058
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // ����һ��2D����ͼ��, ����Ϊ:
    //    Ŀ����������(����ΪGL_TEXTURE_2D), ִ��ϸ�ڼ���, ������ɫ���,
    //    ����ͼ����, ����ͼ��߶�, ����߿���,
    //    �������ݵ���ɫ��ʽ, ������������, ָ��ͼ�����ݵ�ָ��
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

    // ���� Opengl Context ����, �Ա����glewInit
    begin();
    // �����������ɫ��������Ϊ��ɫ
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
    // �����ǰ�������ڵ�ֵ
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

    // ����������
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
