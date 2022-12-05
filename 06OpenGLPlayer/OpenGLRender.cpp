#include "OpenGLRender.h"

static OpenGLRender* pGMyOpenGLRender = nullptr;

void renderxx() {
	auto func = std::bind(&OpenGLRender::render, pGMyOpenGLRender, std::placeholders::_1);
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

void OpenGLRender::initWindow(int& argc, char* argv[]) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(posX, posY);
	glutInitWindowSize(playerWidth , playerHeight);
	glutCreateWindow("OpenGL Demo");

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cout << "Glwe init fail, err: " << glewGetErrorString(err) << std::endl;
		return;
	};

	pMyFFmpeg->initRender();
	pMyFFmpeg->initCodec();
	pMyFFmpeg->initScale();

	// ��ʼ��OpenGL����
	initView();
	// ע��һ����ͼ����
	glutDisplayFunc(renderxx);
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

void OpenGLRender::render(AVFrame* pRGBFrame) {
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
	// ���ö�ά����
	glEnable(GL_TEXTURE_2D);
	// ��ʼ�����ı���
	glBegin(GL_QUADS);

	for (size_t i = 0; i < sizeof(vertex) / sizeof(vertex[0]); i++) {
		// ָ�����������, 
		// ����1: 0Ϊ�������, 0.5Ϊ�����е�, 1.0Ϊ�����Ҳ�
		// ����2: 0Ϊ����ײ�, 0.5Ϊ�����е�, 1.0Ϊ������
		glTexCoord2fv(glm::value_ptr(texture[i]));
		// ָ���������, 
		glVertex2fv(glm::value_ptr(vertex[i]));
	}

	// ��������
	glEnd();
	// ����������
	glutSwapBuffers();
}