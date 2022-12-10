#include "OpenGLRender.h"
#include "FFmpegRender.h"


int main(int argc, char* argv[]) {
	int width = 720;
	int height = 480;
	FFmpegRender myFFmpeg("demo.mp4", width, height, NULL);
	// Init openGL
	OpenGLRender myOpenGl(width, height, &myFFmpeg);
	//myOpenGl.initWindow4Shader120(argc, argv);
	//myOpenGl.initWindow4Shader330(argc, argv);
	myOpenGl.initWindow4Shader430(argc, argv);
	return 0;
}