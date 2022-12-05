#include "OpenGLRender.h"
#include "FFmpegRender.h"


int main(int argc, char* argv[]) {
	int width = 720;
	int height = 480;
	FFmpegRender myFFmpeg("demo.mp4", width, height, NULL);
	// Init openGL
	OpenGLRender myOpenGl(width, height, &myFFmpeg);
	myOpenGl.initWindow(argc, argv);


	return 0;
}