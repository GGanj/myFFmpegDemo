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
	// ��ʼ��OpenGL����
	initView();
	// ע��һ����ͼ����
	glutDisplayFunc(render120);
	// ����ȫ�ֵĻص�����, ��û�д����¼�ʱ(����ʱ), �ú����ᱻ��������
	glutIdleFunc(idleFunc);
	// GLUT��Ϣѭ��
	glutMainLoop();
}

void OpenGLRender::initWindow4Shader330(int& argc, char* argv[]) {
	initWindowBase(argc, argv);

	createTexture();
	// ע��һ����ͼ����
	glutDisplayFunc(render330);
	// ����ȫ�ֵĻص�����, ��û�д����¼�ʱ(����ʱ), �ú����ᱻ��������
	glutIdleFunc(idleFunc);
	// GLUT��Ϣѭ��
	glutMainLoop();
}

void OpenGLRender::initWindow4Shader430(int& argc, char* argv[]) {
	initWindowBase(argc, argv);

	createTexture();
	// ע��һ����ͼ����
	glutDisplayFunc(render430);
	// ����ȫ�ֵĻص�����, ��û�д����¼�ʱ(����ʱ), �ú����ᱻ��������
	glutIdleFunc(idleFunc);
	// GLUT��Ϣѭ��
	glutMainLoop();
}

void OpenGLRender::initView() {
	// ���ԭ����ɫ������ˢΪ��ɫ
	glClearColor(1, 0, 0, 1);
	// ���崰�ڣ�ǰ�������Ϊ���½����꣬���������Ϊ���ڳߴ�
	glViewport(0, 0, playerWidth, playerHeight);
	// ����ͶӰ����,����Ϊ: ���ң��ϣ��£�����Զ
	glOrtho(0, playerWidth, playerHeight, 0, -100, 100);
	// ��������
	createTexture();
}

void OpenGLRender::createTexture() {
	// ������������, ����Ϊ: ���ɸ������Լ�����ID
	glGenTextures(1, &myTextrueID);
	// ����Ŀ������󶨵������ɵ�����ID��
	// Ŀ������ֻ��ΪGL_TEXTURE_1D��GL_TEXTURE_2D��GL_TEXTURE_3D��GL_TEXTURE_CUBE_MAP
	glBindTexture(GL_TEXTURE_2D, myTextrueID);
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, playerWidth, playerHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
}

void OpenGLRender::renderShader120(AVFrame* pRGBFrame) {
	// init shader120
	pShader = new Shader120();
	pShader->initialize();

	// �����ǰ�������ڵ�ֵ
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

	// ����Ŀ������󶨵������ɵ�����ID��
	glBindTexture(GL_TEXTURE_2D, myTextrueID);

	// ָ����λ�������ͼ��
	//    Ŀ����������(����ΪGL_TEXTURE_2D), ִ��ϸ�ڼ���, xƫ��ֵ, yƫ��ֵ 
	//    ��ͼ���, ��ͼ��߶�, �������ݵ���ɫ��ʽ, ������������, ָ��ͼ�����ݵ�ָ��
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 
		playerWidth, playerHeight, GL_RGB, GL_UNSIGNED_BYTE, pRGBFrame->data[0]);

	// ʹ��shader��Ⱦ
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
	// ����������
	glutSwapBuffers();
}

void OpenGLRender::renderShader330(AVFrame* pRGBFrame) {
	// init shader330
	pShader = new Shader330();
	pShader->initialize();

	// �����ǰ�������ڵ�ֵ
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

	// ����Ŀ������󶨵������ɵ�����ID��
	glBindTexture(GL_TEXTURE_2D, myTextrueID);

	// ָ����λ�������ͼ��
	//    Ŀ����������(����ΪGL_TEXTURE_2D), ִ��ϸ�ڼ���, xƫ��ֵ, yƫ��ֵ 
	//    ��ͼ���, ��ͼ��߶�, �������ݵ���ɫ��ʽ, ������������, ָ��ͼ�����ݵ�ָ��
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
		playerWidth, playerHeight, GL_RGB, GL_UNSIGNED_BYTE, pRGBFrame->data[0]);

	// ʹ��shader��Ⱦ

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
	// ����������
	glutSwapBuffers();
}

void OpenGLRender::renderShader430(AVFrame* pRGBFrame) {
	// init shader430
	pShader = new Shader430();
	pShader->initialize();

	// �����ǰ�������ڵ�ֵ
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

	// ����Ŀ������󶨵������ɵ�����ID��
	glBindTexture(GL_TEXTURE_2D, myTextrueID);

	// ָ����λ�������ͼ��
	//    Ŀ����������(����ΪGL_TEXTURE_2D), ִ��ϸ�ڼ���, xƫ��ֵ, yƫ��ֵ 
	//    ��ͼ���, ��ͼ��߶�, �������ݵ���ɫ��ʽ, ������������, ָ��ͼ�����ݵ�ָ��
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
		playerWidth, playerHeight, GL_RGB, GL_UNSIGNED_BYTE, pRGBFrame->data[0]);

	// ʹ��shader��Ⱦ

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
	// ����������
	glutSwapBuffers();
}