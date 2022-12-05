#pragma once
#include <iostream>
extern "C" {
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

#include "XGraph.h"

#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swscale.lib")

class FFmpegCapture
{
public:
	FFmpegCapture();
	~FFmpegCapture();
	void init(int width, int height, XGraph* pD3D);
	void getDeviceInfo();
	void selectDevice(CString deviceName);
	void initCodec();
	void initScale();
	void startCaputre();
	void stop();

	int playerWidth = 0;
	int playerHeight = 0;
	bool isStop = false;
	XGraph* pDirectX = nullptr;
	AVFormatContext* pIptFmtCtx = nullptr;
	AVCodecParameters* pCodecParam = nullptr;
	AVCodecContext* pCodecCtx = nullptr;
	SwsContext* pSWSCtx = nullptr;

};

