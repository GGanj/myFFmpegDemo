#include "OpenGLRender.h"

static OpenGLRender* pGMyOpenGLRender = nullptr;

template <class T>
void renderTemplate() {
	auto func = std::bind(&T, pGMyOpenGLRender, std::placeholders::_1);
	pGMyOpenGLRender->pMyFFmpeg->startRender(func);
}

void render120() {
	auto func = std::bind(&OpenGLRender::renderShader120, pGMyOpenGLRender, std::placeholders::_1);
	pGMyOpenGLRender->pMyFFmpeg->startRender(func);
}

void render330() {
	auto func = std::bind(&OpenGLRender::renderShader330, pGMyOpenGLRender, std::placeholders::_1);
	pGMyOpenGLRender->pMyFFmpeg->startRender(func);
}

void render430() {
	auto func = std::bind(&OpenGLRender::renderShader430, pGMyOpenGLRender, std::placeholders::_1);
	pGMyOpenGLRender->pMyFFmpeg->startRender(func);
}

void idleFunc() {
	std::cout << "Idle" << std::endl;
}

OpenGLRender::OpenGLRender(int width, int height, FFmpegRender* pFFmpeg) : 
	playerWidth(width), playerHeight(height), pMyFFmpeg(pFFmpeg) {
	pGMyOpenGLRender = this;
}

OpenGLRender::~OpenGLRender() {}


void OpenGLRender::initWindowBase(int& argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(posX, posY);
	glutInitWindowSize(playerWidth, playerHeight);
	glutCreateWindow("OpenGL Demo");

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cout << "Glwe init fail, err: " << glewGetErrorString(err) << std::endl;
		return;
	};

	//pMyFFmpeg->initRender();
	pMyFFmpeg->initNetRender();
	pMyFFmpeg->initCodec();
	pMyFFmpeg->initScale();
}

void OpenGLRender::initWindow4Shader120(int& argc, char* argv[]) {
	initWindowBase(argc, argv);
	// 初始化OpenGL窗口
	initView();
	// 注册一个绘图函数
	glutDisplayFunc(render120);
	// 设置全局的回调函数, 当没有窗口事件时(空闲时), 该函数会被持续调用
	glutIdleFunc(idleFunc);
	// GLUT消息循环
	glutMainLoop();
}

void OpenGLRender::initWindow4Shader330(int& argc, char* argv[]) {
	initWindowBase(argc, argv);

	createTexture();
	// 注册一个绘图函数
	glutDisplayFunc(render330);
	// 设置全局的回调函数, 当没有窗口事件时(空闲时), 该函数会被持续调用
	glutIdleFunc(idleFunc);
	// GLUT消息循环
	glutMainLoop();
}

void OpenGLRender::initWindow4Shader430(int& argc, char* argv[]) {
	initWindowBase(argc, argv);

	createTexture();
	// 注册一个绘图函数
	glutDisplayFunc(render430);
	// 设置全局的回调函数, 当没有窗口事件时(空闲时), 该函数会被持续调用
	glutIdleFunc(idleFunc);
	// GLUT消息循环
	glutMainLoop();
}

void OpenGLRender::initView() {
	// 清除原有颜色，重新刷为红色
	glClearColor(1, 0, 0, 1);
	// 定义窗口，前两项参数为左下角坐标，后两项参数为窗口尺寸
	glViewport(0, 0, playerWidth, playerHeight);
	// 正射投影画面,参数为: 左，右，上，下，近，远
	glOrtho(0, playerWidth, playerHeight, 0, -100, 100);
	// 创建纹理
	createTexture();
}

void OpenGLRender::createTexture() {
	// 生成纹理索引, 参数为: 生成个数，以及纹理ID
	glGenTextures(1, &myTextrueID);
	// 将有目标纹理绑定到已生成的纹理ID上
	// 目标纹理只能为GL_TEXTURE_1D、GL_TEXTURE_2D、GL_TEXTURE_3D或GL_TEXTURE_CUBE_MAP
	glBindTexture(GL_TEXTURE_2D, myTextrueID);
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, playerWidth, playerHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
}

void OpenGLRender::renderShader120(AVFrame* pRGBFrame) {
	// init shader120
	pShader = new Shader120();
	pShader->initialize();

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

	// 使用shader渲染
	pShader->begin();
	glEnableVertexAttribArray(dynamic_cast<Shader120*>(pShader)->_position);
	glEnableVertexAttribArray(dynamic_cast<Shader120*>(pShader)->_uv);
	glUniform1f(dynamic_cast<Shader330*>(pShader)->_texture, 0.0);

	glVertexAttribPointer(dynamic_cast<Shader120*>(pShader)->_position, 
		2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), vertex);
	glVertexAttribPointer(dynamic_cast<Shader120*>(pShader)->_uv,
		2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), texture);
	
	glDrawArrays(GL_QUADS, 0, 4);

	pShader->end();

	glDisableVertexAttribArray(dynamic_cast<Shader120*>(pShader)->_position);
	glDisableVertexAttribArray(dynamic_cast<Shader120*>(pShader)->_uv);
	// 交换缓冲区
	glutSwapBuffers();
}

void OpenGLRender::renderShader330(AVFrame* pRGBFrame) {
	// init shader330
	pShader = new Shader330();
	pShader->initialize();

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

	// 使用shader渲染

	glm::mat4 oth = glm::ortho<float>(0, playerWidth, playerHeight, 0, -100, 100);
	pShader->begin();
	glUniformMatrix4fv(dynamic_cast<Shader330*>(pShader)->_matrix, 1, GL_FALSE, glm::value_ptr(oth));
	glEnableVertexAttribArray(dynamic_cast<Shader330*>(pShader)->_position);
	glEnableVertexAttribArray(dynamic_cast<Shader330*>(pShader)->_uv);
	glUniform1f(dynamic_cast<Shader330*>(pShader)->_texture, 0.0);

	glVertexAttribPointer(dynamic_cast<Shader330*>(pShader)->_position,
		2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), vertex);
	glVertexAttribPointer(dynamic_cast<Shader330*>(pShader)->_uv,
		2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), texture);

	glDrawArrays(GL_QUADS, 0, 4);

	pShader->end();

	glDisableVertexAttribArray(dynamic_cast<Shader330*>(pShader)->_position);
	glDisableVertexAttribArray(dynamic_cast<Shader330*>(pShader)->_uv);
	// 交换缓冲区
	glutSwapBuffers();
}

void OpenGLRender::renderShader430(AVFrame* pRGBFrame) {
	// init shader430
	pShader = new Shader430();
	pShader->initialize();

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

	// 使用shader渲染

	glm::mat4 oth = glm::ortho<float>(0, playerWidth, playerHeight, 0, -100, 100);
	pShader->begin();
	// layout(location = 0) uniform mat4 _matrix
	glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(oth));
	// layout(location = 0) in vec2 _position
	glEnableVertexAttribArray(0);
	// layout(location = 1) in vec2 _uv
	glEnableVertexAttribArray(1);
	// layout(location = 1) uniform sampler2D _texture
	glUniform1f(1, 0.0);
	// layout(location = 0) in vec2 _position
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), vertex);
	// layout(location = 1) in vec2 _uv
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), texture);

	glDrawArrays(GL_QUADS, 0, 4);

	pShader->end();

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	// 交换缓冲区
	glutSwapBuffers();
}