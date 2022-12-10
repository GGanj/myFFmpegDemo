#include "D3DCapture.h"
#include "FFmpegEncoder.h"


int main() {
	int width = 2560;
	int height = 1600;
	//D3DCapture myD3D(width, height);
	//myD3D.startCapture();
	
	FFmpegEncoder myEncoder(width, height, 25, "test.rgb", "test.mp4");
	myEncoder.openFileStream();
	myEncoder.initH264Encodec();
	//myEncoder.initH265Encodec();
	myEncoder.initScaler();
	myEncoder.initOptFmtCtx();
	myEncoder.openOptFile();
	myEncoder.startEncoder();
	return 0;
}