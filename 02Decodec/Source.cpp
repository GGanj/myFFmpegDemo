#include <iostream>
#include <fstream>

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


int main() {
	AVFormatContext* pFormat = nullptr;
	// ���ļ�����ȡ��ʽ������
	int ret = avformat_open_input(&pFormat, "demo.mp4", NULL, NULL);
	if (ret < 0) {
		std::cout << "Open file failed " << std::endl;
	}
	std::cout << "Open file succeed " << std::endl;

	// ����������Ѱ�����Ľ�����Ϣ -> h264/h265/height/width...
	ret = avformat_find_stream_info(pFormat, NULL);
	if (ret < 0) {
		std::cout << "Find stream info failed" << std::endl;
	}
	std::cout << "Find stream info succeed" << std::endl;

	// Ѱ����Ƶ��
	int videoStream = -1; 
	videoStream = av_find_best_stream(pFormat, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, NULL);
	if (videoStream < 0) {
		std::cout << "Find video stream failed" << std::endl;
	}
	std::cout << "Find video stream succeed" << std::endl;

	// Ѱ����Ƶ��������
	const AVCodec* pMyCodec = avcodec_find_decoder(pFormat->streams[videoStream]->codecpar->codec_id);
	if (pMyCodec == nullptr) {
		std::cout << "Find decoder failed" << std::endl;
	}
	std::cout << "Find decoder succeed" << std::endl;


	// ʹ�ý����������õ������������ģ����°汾streams�����е�AVCodecContext���滻ΪAVCodecParamters
	AVCodecParameters* pMyCodecParameters = pFormat->streams[videoStream]->codecpar;
	// �����ҵ��Ľ��������������������ģ���ʹ��AVCodecParameter��������
	AVCodecContext* pMyCodecContext = avcodec_alloc_context3(pMyCodec);
	avcodec_parameters_to_context(pMyCodecContext, pMyCodecParameters);
	// ʹ�ý�������������
	ret = avcodec_open2(pMyCodecContext, pMyCodec, NULL);
	if (ret < 0) {
		std::cout << "Open codec text failed" << std::endl;
	}
	std::cout << "Open codec success" << std::endl;

	// ����ת��������
	SwsContext* pMySWSContext = sws_getContext(pMyCodecParameters->width, pMyCodecParameters->height, static_cast<AVPixelFormat>(pMyCodecParameters->format),
		pMyCodecParameters->width, pMyCodecParameters->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
	if (pMySWSContext == nullptr) {
		std::cout << "Sws context set fail" << std::endl;
	}
	std::cout << "Sws context set succeed" << std::endl;

	// ������Ƶ
	// ����֡�ռ�
	AVFrame* pOriginFrame = av_frame_alloc();
	AVFrame* pYUVFrame = av_frame_alloc();
	// һ֡�ĳߴ�
	int frameSize = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pMyCodecParameters->width, pMyCodecParameters->height, 1);
	// �������֡���ݵĻ�����
	uint8_t* pBuffer = static_cast<uint8_t*>(av_malloc(frameSize));
	// ֡�е�ͼ��ָ��ָ�����ݻ�����
	ret = av_image_fill_arrays(pYUVFrame->data, pYUVFrame->linesize, pBuffer, AV_PIX_FMT_YUV420P, pMyCodecParameters->width, pMyCodecParameters->height, 1);
	if (ret < 0) {
		std::cout << "Fill buffer failed" << std::endl;
	}
	std::cout << "Fill buffer succeed" << std::endl;

	// ����������
	AVPacket* pPacket = new AVPacket();

	// ��ȡ֡
	int frameCount = 0;
	FILE* pFy = nullptr;
	fopen_s(&pFy, "demo_y.y", "wb+");
	FILE* pFu = nullptr;
	fopen_s(&pFu, "demo_u.y", "wb+");
	FILE* pFv = nullptr; 
	fopen_s(&pFv, "demo_v.y", "wb+");
	while (av_read_frame(pFormat, pPacket) >= 0) {
		if (pPacket->stream_index == AVMEDIA_TYPE_VIDEO) {
			// ��ѹ����packetת��Ϊδѹ����frame
			avcodec_send_packet(pMyCodecContext, pPacket);
			ret = avcodec_receive_frame(pMyCodecContext, pOriginFrame);
			if (ret != 0) {
				std::cout << "Read pakcet fialed" << std::endl;
			}
			std::cout << "Read packet success" << std::endl;
			// ��֡ת��
			ret = sws_scale(pMySWSContext, pOriginFrame->data, pOriginFrame->linesize, 0, 
				pMyCodecParameters->height, pYUVFrame->data, pYUVFrame->linesize);
			if (ret < 0) {
				std::cout << "Sws scale fialed" << std::endl;
			}
			fwrite(pYUVFrame->data[0], 1, pMyCodecParameters->height * pMyCodecParameters->width, pFy);
			fwrite(pYUVFrame->data[1], 1, pMyCodecParameters->height * pMyCodecParameters->width / 4, pFu);
			fwrite(pYUVFrame->data[2], 1, pMyCodecParameters->height * pMyCodecParameters->width / 4, pFv);
			std::cout << "Sws_scale success, frame count " << frameCount++ << std::endl;
		}
	}


	// �ͷ���Դ
	fclose(pFy);
	pFy = nullptr;
	fclose(pFu);
	pFu = nullptr;
	fclose(pFv);
	pFv = nullptr;

	av_packet_unref(pPacket);
	pPacket = nullptr;

	av_frame_free(&pOriginFrame);
	pOriginFrame = nullptr;
	av_frame_free(&pYUVFrame);
	pYUVFrame = nullptr;
	av_free(pBuffer);
	pBuffer = nullptr;

	sws_freeContext(pMySWSContext);
	pMySWSContext = nullptr;
	avcodec_close(pMyCodecContext);
	pMyCodecContext = nullptr;
	avformat_close_input(&pFormat);
	pFormat = nullptr;
	pMyCodecParameters = nullptr;

	return 0;
}