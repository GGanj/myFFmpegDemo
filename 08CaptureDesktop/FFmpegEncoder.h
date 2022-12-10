#pragma once

extern "C" {
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
}
#include <fstream>
#include <iostream>

#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swscale.lib")

class FFmpegEncoder
{
public:
	FFmpegEncoder();
	FFmpegEncoder(int width, int height, int fps, const char* input, const char* output);
	~FFmpegEncoder();
	void openFileStream();
	void initH264Encodec();
	void initH265Encodec();
	void initScaler();
	void initOptFmtCtx();
	void openOptFile();
	void startEncoder();
	void writeFreameToFmt(AVFrame* pYUVFrame);
	void endEncoder();
public:
	const char* pIptFileName = nullptr;
	const char* pOptFileName = nullptr;
	FILE* pIptFileStream = nullptr;
	int playerWidth = 0;
	int playerHeight = 0;
	int playerFps = 0;
	AVFormatContext* pOptFmtCtx = nullptr;
	AVCodecContext* pEncodecCtx = nullptr;
	SwsContext* pMySwsCtx = nullptr;
};

