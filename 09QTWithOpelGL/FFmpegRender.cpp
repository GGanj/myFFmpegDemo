#include "FFmpegRender.h"

void showErrMsg(const char* pStr) {
    std::cout << pStr << std::endl;
}


FFmpegRender::FFmpegRender(const char* path, int width, int height):
    pFilePath(path), playerWidth(width), playerHeight(height) {
}

void FFmpegRender::initRender() {
	int ret = avformat_open_input(&pFormat, pFilePath, NULL, NULL);
	if (ret < 0) {
        showErrMsg("Open stream file failed");
	}
	avformat_find_stream_info(pFormat, NULL);
}

void FFmpegRender::initCodec() {
	int videoStream = -1;
	videoStream = av_find_best_stream(pFormat, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, NULL);
	if (videoStream < 0) {
        showErrMsg("Find video stream failed");
	}
	pCodecParameters = pFormat->streams[videoStream]->codecpar;
	const AVCodec* pCodec = avcodec_find_decoder(pCodecParameters->codec_id);
	pCodecContext = avcodec_alloc_context3(pCodec);
	avcodec_parameters_to_context(pCodecContext, pCodecParameters);
	int ret = avcodec_open2(pCodecContext, pCodec, NULL);
    if (ret < 0) {
        showErrMsg("Open codec text failed");
	}
}

void FFmpegRender::initScale() {
	pSWSContext = sws_getContext(pCodecParameters->width, pCodecParameters->height,
		static_cast<AVPixelFormat>(pCodecParameters->format),
        playerWidth, playerHeight, AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
	if (pSWSContext == nullptr) {
        showErrMsg("Sws context set fail");
	}
}

void FFmpegRender::startRender() {
    AVFrame* pOriginFrame = av_frame_alloc();
    AVFrame* pYUVFrame = av_frame_alloc();
    int frameSize = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, playerWidth, playerHeight, 1);
    uint8_t* pBuffer = static_cast<uint8_t*>(av_malloc(frameSize));
    int ret = av_image_fill_arrays(pYUVFrame->data, pYUVFrame->linesize, pBuffer, AV_PIX_FMT_YUV420P, playerWidth, playerHeight, 1);
    AVPacket* pPacket = new AVPacket();
    int frameCount = 0;
    while (av_read_frame(pFormat, pPacket) >= 0) {
        if (pPacket->stream_index == AVMEDIA_TYPE_VIDEO) {
            avcodec_send_packet(pCodecContext, pPacket);
            ret = avcodec_receive_frame(pCodecContext, pOriginFrame);
            if (ret != 0) {
                continue;
            }
            ret = sws_scale(pSWSContext, pOriginFrame->data, pOriginFrame->linesize, 0,
                pCodecParameters->height, pYUVFrame->data, pYUVFrame->linesize);
            if (ret < 0) {
                showErrMsg("Sws scale fialed");
            }
            frameCount++;
            // wait directx release the pDstBuffer

        }
        av_packet_unref(pPacket);
    }
}
