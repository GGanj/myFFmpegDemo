#include "pch.h"
#include "FFmpegCapture.h"

void showErrMsg(LPCWSTR pStr) {
	MessageBox(NULL, pStr, L"Error", MB_OK);
}

FFmpegCapture::FFmpegCapture() {
	avdevice_register_all();
}

FFmpegCapture::~FFmpegCapture() {}

void FFmpegCapture::init(int width, int height, XGraph* pD3D) {
	playerWidth = width;
	playerHeight = height;
	pDirectX = pD3D;
}

void FFmpegCapture::getDeviceInfo() {
	// AVDictionary 一个键值对字典，用来存储ffmpeg的各项参数
	AVDictionary* pDict = nullptr;
	// 在字典中添加设备条目
	int ret = av_dict_set(&pDict, "list_devices", "true", 0);
	if (ret < 0) {
		showErrMsg(L"Set list_devices failed");
	}
	
	AVFormatContext* pFmtCtx = avformat_alloc_context();
	// 设置为查找dshow设备
	const AVInputFormat* pIptFmt = av_find_input_format("dshow");
	// 返回值小于0
	ret = avformat_open_input(&pFmtCtx, "video=dummy", pIptFmt, &pDict);
	//if (ret < 0) {
	//	showErrMsg(L"Open video=dummy fail");
	//}

	// 似乎 avdevice_list_devices 没有被实现，执行函数会闪退
	//AVDeviceInfoList* pDeviceList = nullptr;
	//ret = avdevice_list_devices(pFmtCtx, &pDeviceList);
	//if (ret < 0) {
	//	showErrMsg(L"Get device list fail");
	//}
	
	avformat_free_context(pFmtCtx);
}

void FFmpegCapture::selectDevice(CString deviceName) {
	AVDictionary* pDict = nullptr;
	int ret = av_dict_set(&pDict, "list_options", "true", 0);
	if (ret < 0) {
		showErrMsg(L"Set list_options failed");
	}

	pIptFmtCtx = avformat_alloc_context();
	const AVInputFormat* pIptFmt = av_find_input_format("dshow");
	// 使用AVDictionary会导致无法打开上下文，不知具体原因
	 USES_CONVERSION;
	//ret = avformat_open_input(&pIptFmtCtx, W2A(L"video=" + deviceName), pIptFmt, &pDict);
	ret = avformat_open_input(&pIptFmtCtx, W2A(L"video=" + deviceName), pIptFmt, NULL);
	//if (ret < 0) {
	//	showErrMsg(L"Open video=Integrated Camera fail");
	//}

	avformat_find_stream_info(pIptFmtCtx, NULL);
	isStop = false;
}
void FFmpegCapture::initCodec() {
	int videoStream = av_find_best_stream(pIptFmtCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, NULL);
	if (videoStream < 0) {
		showErrMsg(L"Find video stream fail");
	}
	pCodecParam = pIptFmtCtx->streams[videoStream]->codecpar;
	const AVCodec* pCodec = avcodec_find_decoder(pCodecParam->codec_id);
	pCodecCtx = avcodec_alloc_context3(pCodec);
	avcodec_parameters_to_context(pCodecCtx, pCodecParam);
	int ret = avcodec_open2(pCodecCtx, pCodec, NULL);
	if (ret < 0) {
		showErrMsg(L"Open codec context fail");
	}
 }

void FFmpegCapture::initScale() {
	pSWSCtx = sws_getContext(
		pCodecParam->width, pCodecParam->height, static_cast<AVPixelFormat>(pCodecParam->format),
		playerWidth, playerHeight, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
	if (pSWSCtx == nullptr) {
		showErrMsg(L"Sws context set fail");
	}
}

void FFmpegCapture::startCaputre() {
	AVFrame* pOrigFrame = av_frame_alloc();
	AVFrame* pYUVFrame = av_frame_alloc();
	int yuvFrameSize = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, playerWidth, playerHeight, 1);
	uint8_t* pYUVFrameBuffer = static_cast<uint8_t*>(av_malloc(yuvFrameSize));
	av_image_fill_arrays(pYUVFrame->data, pYUVFrame->linesize, pYUVFrameBuffer,
		AV_PIX_FMT_YUV420P, playerWidth, playerHeight, 1);

	int frameCount = 0;
	AVPacket* pPacket = new AVPacket();
	while (isStop != true && av_read_frame(pIptFmtCtx, pPacket) >= 0) {
		if (pPacket->stream_index == AVMEDIA_TYPE_VIDEO) {
			avcodec_send_packet(pCodecCtx, pPacket);
			int ret = avcodec_receive_frame(pCodecCtx, pOrigFrame);
			if (ret != 0) {
				continue;
			}
			ret = sws_scale(pSWSCtx, pOrigFrame->data, pOrigFrame->linesize, 0,
				pCodecParam->height, pYUVFrame->data, pYUVFrame->linesize);
			if (ret < 0) {
				showErrMsg(L"Sws scale fail");
				continue;
			}
			frameCount++;
			Sleep(1);
			pDirectX->BeginRender();
			byte* pDstBuff = static_cast<byte*>(pDirectX->_d3d_rect.pBits);
			int dstLineSize = pDirectX->_d3d_rect.Pitch;
			// Y
			for (int i = 0; i < playerHeight; ++i) {
				memcpy(pDstBuff + i * dstLineSize,
					pYUVFrame->data[0] + i * playerWidth,
					playerWidth);
			}
			// U
			for (int i = 0; i < playerHeight / 2; ++i) {
				memcpy(pDstBuff + playerHeight * dstLineSize + playerHeight * dstLineSize / 4 + i * dstLineSize / 2 ,
					pYUVFrame->data[1] + i * playerWidth / 2,
					playerWidth / 2);
			}
			// V
			for (int i = 0; i < playerHeight / 2; ++i) {
				memcpy(pDstBuff + playerHeight * dstLineSize + i * dstLineSize / 2,
					pYUVFrame->data[2] + i * playerWidth / 2,
					playerWidth / 2);
			}
			pDirectX->EndRender();
		}
		av_packet_unref(pPacket);
	}
	avformat_close_input(&pIptFmtCtx);
}

void FFmpegCapture::stop() {
	isStop = true;

}