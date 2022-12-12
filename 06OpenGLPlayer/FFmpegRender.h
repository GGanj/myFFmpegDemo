#pragma once
#include <iostream>
#include <functional>
#include <windows.h>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

//#include "OpenGLRender.h"

#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swscale.lib")


class FFmpegRender
{
public:
	FFmpegRender(const char* path, int width, int height, void* p);
	void initRender();
	void initCodec();
	void initScale();
	void startRender();
	template <class T>
	void startRender(T& rFunc) {
		AVFrame* pOriginFrame = av_frame_alloc();
		AVFrame* pRGBFrame = av_frame_alloc();
		int frameSize = av_image_get_buffer_size(AV_PIX_FMT_RGB24, playerWidth, playerHeight, 1);
		uint8_t* pBuffer = static_cast<uint8_t*>(av_malloc(frameSize));
		int ret = av_image_fill_arrays(pRGBFrame->data, pRGBFrame->linesize, pBuffer, AV_PIX_FMT_RGB24, playerWidth, playerHeight, 1);
		AVPacket* pPacket = new AVPacket();
		int frameCount = 0;
		int go = 0;
		while (av_read_frame(pFormat, pPacket) >= 0) {
			if (pPacket->stream_index == AVMEDIA_TYPE_VIDEO) {
				avcodec_send_packet(pCodecContext, pPacket);
				ret = avcodec_receive_frame(pCodecContext, pOriginFrame);
				if (ret < 0) {
					continue;
				}
				ret = sws_scale(pSWSContext, pOriginFrame->data, pOriginFrame->linesize, 0,
					pCodecContext->height, pRGBFrame->data, pRGBFrame->linesize);
				if (ret < 0) {
					std::cout << "Sws scale fialed" << std::endl;
				}
				std::cout << frameCount++ << std::endl;
				// wait directx release the pDstBuffer
				rFunc(pRGBFrame);
				Sleep(2);
			}
			av_packet_unref(pPacket);
		}
	}

	const char* pFilePath = nullptr;
	int playerWidth = 0;
	int playerHeight = 0;
	AVFormatContext* pFormat = nullptr;
	AVCodecParameters* pCodecParameters = nullptr;
	AVCodecContext* pCodecContext = nullptr;
	SwsContext* pSWSContext = nullptr;
};

