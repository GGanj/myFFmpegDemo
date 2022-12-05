#include "pch.h"
#include "FFmpegRender.h"

void showErrMsg(LPCWSTR pStr) {
	MessageBox(NULL, pStr, L"Error", MB_OK);
}

FFmpegRender::FFmpegRender() {}

FFmpegRender::FFmpegRender(const char* path, int width, int height, XGraph* p): 
	pFilePath(path), playerWidth(width), playerHeight(height), pXGraph(p) {}

void FFmpegRender::initRender() {
	int ret = avformat_open_input(&pFormat, pFilePath, NULL, NULL);
	if (ret < 0) {
		showErrMsg(L"Open stream file failed");
	}
	avformat_find_stream_info(pFormat, NULL);
}

void FFmpegRender::initRender(const char* path, int width, int height, XGraph* p) {
	pFilePath = path;
	playerWidth = width;
	playerHeight = height;
	pXGraph = p;
	int ret = avformat_open_input(&pFormat, pFilePath, NULL, NULL);
	if (ret < 0) {
		showErrMsg(L"Open stream file failed");
	}
	avformat_find_stream_info(pFormat, NULL);
}

void FFmpegRender::initCodec() {
	int videoStream = -1;
	videoStream = av_find_best_stream(pFormat, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, NULL);
	if (videoStream < 0) {
		showErrMsg(L"Find video stream failed");
	}
	pCodecParameters = pFormat->streams[videoStream]->codecpar;
	const AVCodec* pCodec = avcodec_find_decoder(pCodecParameters->codec_id);
	pCodecContext = avcodec_alloc_context3(pCodec);
	avcodec_parameters_to_context(pCodecContext, pCodecParameters);
	int ret = avcodec_open2(pCodecContext, pCodec, NULL);
	if (videoStream < 0) {
		showErrMsg(L"Open codec text failed");
	}
}

void FFmpegRender::initScale() {
	pSWSContext = sws_getContext(pCodecParameters->width, pCodecParameters->height,
		static_cast<AVPixelFormat>(pCodecParameters->format),
		playerWidth, playerHeight, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
	if (pSWSContext == nullptr) {
		showErrMsg(L"Sws context set fail");
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
				showErrMsg(L"Sws scale fialed");
			}
			frameCount++;
			// wait directx release the pDstBuffer
			Sleep(5);
			pXGraph->BeginRender();
			byte* pDstBuffer = (byte*)pXGraph->_d3d_rect.pBits;
			int direcLineSize = pXGraph->_d3d_rect.Pitch;
			byte* pYStr = pYUVFrame->data[0];
			byte* pUStr = pYUVFrame->data[1];
			byte* pVStr = pYUVFrame->data[2];
			// Y
			for (int i = 0; i < playerHeight; ++i) {
				memcpy(pDstBuffer + i * direcLineSize, 
					pYStr + i * playerWidth, 
					playerWidth);
			}
			// U 可能是ffmpeg->I420转为D3D->YV12格式，u存储在v之后
			for (int i = 0; i < playerHeight / 2; ++i) {
				memcpy(pDstBuffer + playerHeight * direcLineSize + playerHeight * direcLineSize / 4 + i * direcLineSize / 2,
					pUStr + i * playerWidth / 2,
					//pYStr + playerHeight * playerWidth + i * playerWidth / 2,
					playerWidth / 2);
			}
			// V
			for (int i = 0; i < playerHeight / 2; ++i) {
				memcpy(pDstBuffer + playerHeight * direcLineSize + i * direcLineSize / 2, 
					pVStr + i * playerWidth / 2,
					//pYStr + playerHeight * playerWidth + playerHeight * playerWidth / 4 + i * playerWidth / 2,
					playerWidth / 2);
			}
			pXGraph->EndRender();
		}
		av_packet_unref(pPacket);
	}
}