#pragma once
#include <Windows.h>
#include <fstream>
#include <iostream>
#include <mutex>

#include "XGraph.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swscale.lib")

class FFmpegRender
{
public:
	FFmpegRender(const char* path, int width, int height, XGraph* p);
	void initRender();
	void initCodec();
	void initScale();
	void startRender();

	const char* pFilePath = nullptr;
	int playerWidth = 0;
	int playerHeight = 0;
	AVFormatContext* pFormat = nullptr;
	AVCodecParameters* pCodecParameters = nullptr;
	AVCodecContext* pCodecContext = nullptr;
	SwsContext* pSWSContext = nullptr;
	XGraph* pXGraph = nullptr;
	std::mutex myMutex;
};

