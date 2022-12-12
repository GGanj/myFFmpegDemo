#pragma once
#include <fstream>
#include <iostream>
#include <mutex>

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

struct  FrameInfor
{
    AVFrame*_data;
    int     _dataSize;
    int     _width;
    int     _height;
    int		 _pts;
    double  _timeBase;

};

class FFmpegRender
{
public:
    FFmpegRender(const char* path, int width, int height);
	void initRender();
	void initCodec();
	void initScale();
    void startRender();
    template <class T>
    void startRender(T& readFrame) {
        AVFrame* pOriginFrame = av_frame_alloc();
        AVFrame* pYUVFrame = av_frame_alloc();
        int frameSize = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, playerWidth, playerHeight, 1);
        uint8_t* pBuffer = static_cast<uint8_t*>(av_malloc(frameSize));
        int ret = av_image_fill_arrays(pYUVFrame->data, pYUVFrame->linesize, pBuffer, AV_PIX_FMT_YUV420P, playerWidth, playerHeight, 1);
        AVPacket* pPacket = new AVPacket();
        int frameCount = 0;
        FrameInfor infor;
        while (av_read_frame(pFormat, pPacket) >= 0) {
            if (pPacket->stream_index == AVMEDIA_TYPE_VIDEO) {
                avcodec_send_packet(pCodecContext, pPacket);
                ret = avcodec_receive_frame(pCodecContext, pOriginFrame);
                if (ret < 0) {
                    continue;
                }
                ret = sws_scale(pSWSContext, pOriginFrame->data, pOriginFrame->linesize, 0,
                    pCodecContext->height, pYUVFrame->data, pYUVFrame->linesize);
                if (ret < 0) {
                    std::cout << "Sws scale fialed" << std::endl;
                }
                std::cout << frameCount++ << std::endl;
                //
                infor._data = pYUVFrame;
                infor._dataSize = 0;
                infor._width = playerWidth;
                infor._height = playerHeight;
                infor._pts = pYUVFrame->pts;
                infor._timeBase = av_q2d(pFormat->streams[videoStream]->time_base);
                readFrame(&infor);
                Sleep(1);
            }
            av_packet_unref(pPacket);
        }
    }

	const char* pFilePath = nullptr;
	int playerWidth = 0;
	int playerHeight = 0;
	AVFormatContext* pFormat = nullptr;
    int videoStream = -1;
	AVCodecParameters* pCodecParameters = nullptr;
	AVCodecContext* pCodecContext = nullptr;
	SwsContext* pSWSContext = nullptr;
};

