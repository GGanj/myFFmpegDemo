#pragma once
#include <iostream>
#include <functional>
#include <stdlib.h>
#include "GL/glew.h"

#include "GL/glut.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "FFmpegRender.h"

#include "shader120.h"
#include "shader330.h"
#include "shader430.h"

#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glut32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "OpenGL32.lib")

class OpenGLRender
{
public:
	OpenGLRender(int width, int height, FFmpegRender* pFFmpeg);
	~OpenGLRender();
	void initWindowBase(int& argc, char* argv[]);
	void initWindow4Shader120(int& argc, char* argv[]);
	void initWindow4Shader330(int& argc, char* argv[]);
	void initWindow4Shader430(int& argc, char* argv[]);
	void initView();
	void createTexture();
	void renderShader120(AVFrame* pRGBFrame);
	void renderShader330(AVFrame* pRGBFrame);
	void renderShader430(AVFrame* pRGBFrame);

	int posX = 100;
	int posY = 100;
	int playerWidth;
	int playerHeight;
	GLuint myTextrueID = -1;
	FFmpegRender* pMyFFmpeg = nullptr;
	Program* pShader = nullptr;
};

