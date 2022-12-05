#pragma once
#include <iostream>
#include <functional>
#include <stdlib.h>
#include "GL/glew.h"

#include "GL/glut.h"
#include "glm/glm.hpp"
#include "glm\gtc\type_ptr.hpp"

#include "FFmpegRender.h"

#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glut32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "OpenGL32.lib")
class OpenGLRender
{
public:
	OpenGLRender(int width, int height, FFmpegRender* pFFmpeg);
	~OpenGLRender();
	void initWindow(int& argc, char* argv[]);
	void initView();
	void createTexture();
	void render(AVFrame* pRGBFrame);

	int posX = 100;
	int posY = 100;
	int playerWidth;
	int playerHeight;
	GLuint myTextrueID = -1;
	FFmpegRender* pMyFFmpeg = nullptr;
};

