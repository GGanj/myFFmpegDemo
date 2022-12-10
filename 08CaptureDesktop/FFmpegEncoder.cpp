#include "FFmpegEncoder.h"

FFmpegEncoder::FFmpegEncoder() {}

FFmpegEncoder::FFmpegEncoder(int width, int height, int fps, const char* input, const char* output) :
 playerWidth(width), playerHeight(height), playerFps(fps), pIptFileName(input), pOptFileName(output) {}

FFmpegEncoder::~FFmpegEncoder() {}

void FFmpegEncoder::openFileStream() {
	if (fopen_s(&pIptFileStream, pIptFileName, "rb") != 0) {
		std::cout << "Open input file fail" << std::endl;
	}
}

void FFmpegEncoder::initH264Encodec() {
	const AVCodec* pEncodec = avcodec_find_encoder(AV_CODEC_ID_H264);
	if (pEncodec == nullptr) {
		std::cout << "Find H264 encoder fail" << std::endl;
		return;
	}
	pEncodecCtx = avcodec_alloc_context3(pEncodec);
	if (pEncodecCtx == nullptr) {
		std::cout << "Alloc codec context fail" << std::endl;
		return;
	}

	pEncodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
	pEncodecCtx->codec_id = AV_CODEC_ID_H264;
	pEncodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
	// ����ѹ��������
	pEncodecCtx->bit_rate = 400000000;
	pEncodecCtx->width = playerWidth;
	pEncodecCtx->height = playerHeight;
	pEncodecCtx->time_base = {1, playerFps};
	pEncodecCtx->framerate = {playerFps, 1};

	// ���û������С, �ؼ�֡
	pEncodecCtx->gop_size = 50;

	// �������B֡
	pEncodecCtx->max_b_frames = 0;


	pEncodecCtx->thread_count = 12;
	// ȫ�ֱ�����Ϣ
	pEncodecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

	int ret = avcodec_open2(pEncodecCtx, pEncodec, NULL);
	if (ret < 0) {
		std::cout << "Open H264 codec fail" << std::endl;
	}
}


void FFmpegEncoder::initH265Encodec() {
	const AVCodec* pEncodec = avcodec_find_encoder(AV_CODEC_ID_H265);
	if (pEncodec == nullptr) {
		std::cout << "Find H265 encoder fail" << std::endl;
		return;
	}
	pEncodecCtx = avcodec_alloc_context3(pEncodec);
	if (pEncodecCtx == nullptr) {
		std::cout << "Alloc codec context fail" << std::endl;
		return;
	}

	pEncodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
	pEncodecCtx->codec_id = AV_CODEC_ID_H265;
	pEncodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
	// ����ѹ��������
	pEncodecCtx->bit_rate = 400000000;
	pEncodecCtx->width = playerWidth;
	pEncodecCtx->height = playerHeight;
	pEncodecCtx->time_base = { 1, playerFps };
	pEncodecCtx->framerate = { playerFps, 1 };

	// ���û������С, �ؼ�֡
	pEncodecCtx->gop_size = 250;

	// H265��Ҫָ��
	pEncodecCtx->qmin = 10;
	pEncodecCtx->qmax = 51;
	// �������B֡
	pEncodecCtx->max_b_frames = 3;

	pEncodecCtx->thread_count = 12;
	AVDictionary* pDict = nullptr;
	av_dict_set(&pDict, "x265-params", "qp=20", 0);
	av_dict_set(&pDict, "preset", "ultrafast", 0);
	av_dict_set(&pDict, "tune", "zero-latency", 0);

	// ȫ�ֱ�����Ϣ
	pEncodecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

	int ret = avcodec_open2(pEncodecCtx, pEncodec, NULL);
	if (ret < 0) {
		std::cout << "Open H264 codec fail" << std::endl;
	}
}

void FFmpegEncoder::initScaler() {
	pMySwsCtx = sws_getContext(playerWidth, playerHeight, AV_PIX_FMT_BGRA,
		playerWidth, playerHeight, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
}

void FFmpegEncoder::initOptFmtCtx() {
	// ��������������ڴ�
	avformat_alloc_output_context2(&pOptFmtCtx, 0, 0, pOptFileName);
	// ������Ƶ��
	AVStream* pVideoStream = avformat_new_stream(pOptFmtCtx, NULL);

	pVideoStream->id = 0;
	pVideoStream->codecpar->codec_tag = 0;
	avcodec_parameters_from_context(pVideoStream->codecpar, pEncodecCtx);

	// ��ӡFormatContext��Ϣ������ļ���
	av_dump_format(pOptFmtCtx, 0, pOptFileName, 1);
}

void FFmpegEncoder::openOptFile() {
	// д����Ƶ�ļ�ͷ����Ϣ
	int ret = avio_open(&pOptFmtCtx->pb, pOptFileName, AVIO_FLAG_WRITE);
	if (ret < 0) {
		std::cout << "avio_open fail" << std::endl;
		return;
	}
	ret = avformat_write_header(pOptFmtCtx, NULL);
	if (ret < 0) {
		std::cout << "avformat_write_header fail" << std::endl;
		return;
	}
}

void FFmpegEncoder::startEncoder() {
	// ��������
	unsigned char* pRgbBuff = new unsigned char[playerWidth * playerHeight * 4];
	// �����֡����
	AVFrame* pYUVFrame = av_frame_alloc();
	pYUVFrame->format = AV_PIX_FMT_YUV420P;
	pYUVFrame->width = playerWidth;
	pYUVFrame->height = playerHeight;
	int ret = av_frame_get_buffer(pYUVFrame, 32);
	if (ret < 0) {
		std::cout << "Get YUV frame buff fail" << std::endl;
		return;
	}

	int p = 0;
	while (true) {
		int len = fread(pRgbBuff, 1, playerWidth * playerHeight * 4, pIptFileStream);
		if (len <= 0) {
			std::cout << "read buff <= 0" << std::endl;
			break;
		}
		uint8_t* inData[AV_NUM_DATA_POINTERS] = { 0 };
		int inLineSize[AV_NUM_DATA_POINTERS] = { 0 };
		inData[0] = pRgbBuff;
		inLineSize[0] = playerWidth * 4;
		int currHeight = sws_scale(pMySwsCtx, inData, inLineSize, 0, playerHeight,
			pYUVFrame->data, pYUVFrame->linesize);
		if (currHeight <= 0) {
			std::cout << "sws_scale fail" << std::endl;
			return;
		}
		std::cout << "sws_scale " << currHeight << " lines" << std::endl;

		// ������Ƶ֡
		pYUVFrame->pts = p;
		p += 3600;
		writeFreameToFmt(pYUVFrame);
	}

	// д����Ƶ����
	av_write_trailer(pOptFmtCtx);
	delete[] pRgbBuff;
	return;
}

void FFmpegEncoder::writeFreameToFmt(AVFrame* pYUVFrame) {
	int ret = avcodec_send_frame(pEncodecCtx, pYUVFrame);
	if (ret < 0) {
		std::cout << "avcodec_send_frame fail" << std::endl;
		return;
	}
	AVPacket* pPacket = av_packet_alloc();
	ret = avcodec_receive_packet(pEncodecCtx, pPacket);
	if (ret < 0) {
		std::cout << "avcodev_receive_packet fail" << std::endl;
		av_packet_free(&pPacket);
		return;
	}

	av_interleaved_write_frame(pOptFmtCtx, pPacket);
	std::cout << "write packet into format context, size = " << pPacket->size << std::endl;
	av_packet_free(&pPacket);
}

void FFmpegEncoder::endEncoder() {
	// �ر���Ƶ�����
	avio_close(pOptFmtCtx->pb);

	avformat_free_context(pOptFmtCtx);
	avcodec_close(pEncodecCtx);
	avcodec_free_context(&pEncodecCtx);
	sws_freeContext(pMySwsCtx);
}