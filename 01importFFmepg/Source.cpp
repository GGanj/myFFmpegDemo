#include <iostream>
#include <fstream>

extern "C" {
// FFmpeg ʹ��C��д����˵�����lib��ͷ�ļ�ʱ��Ҫʹ��C���
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
}

#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")

// ����YUV�ļ�
void parseYUV420(const char* path, int width, int height) {
	// �Զ����ƶ�ȡYUV�ļ�
	FILE* pFileRead;
	int ret = fopen_s(&pFileRead, path, "rb+");
	if (ret == 0) {
		std::cout << "Open yuv file " << path << " succeed" << std::endl;
	}
	else {
		std::cout << "Open yuv file " << path << " failed" << std::endl;
		return;
	}
	FILE* pFileWrieteY;
	ret = fopen_s(&pFileWrieteY, "yuv420Y.y", "wb+");
	FILE* pFileWrieteU;
	ret = fopen_s(&pFileWrieteU, "yuv420U.y", "wb+");
	FILE* pFileWrieteV;
	ret = fopen_s(&pFileWrieteV, "yuv420V.y", "wb+");
	
	// ��֡��ȡYUV��ֵ
	int frame = 0;
	unsigned char* pBuffer = (unsigned char*)malloc(width * height * 3 / 2);
	while (frame < 1) {
		fread(pBuffer, 1, width * height, pFileRead);

		fwrite(pBuffer, 1, width * height, pFileWrieteY);
		fwrite(pBuffer + width * height, 1, width * height / 4, pFileWrieteU);
		fwrite(pBuffer + width * height * (1 + 1 / 4), 1, width * height / 4, pFileWrieteV);

		++frame;
	}

	free(pBuffer);
	pBuffer = nullptr;
	fclose(pFileRead);
	fclose(pFileWrieteY);
	fclose(pFileWrieteU);
	fclose(pFileWrieteV);
}

// �������ض�ý���ļ�

void openLocalResource(const char* path) {
	// �°汾�Ѿ��Ƴ�
	//av_register_all();

	AVFormatContext* pFormat = nullptr;
	// ���ļ�����ȡ��ʽ������
	int ret = avformat_open_input(&pFormat, path, NULL, NULL);
	if (ret < 0) {
		std::cout << "Step1: open " << path << " failed" << std::endl;
		return;
	}
	std::cout << "Step1: open " << path << " succeed" << std::endl;
	// microsecond
	int time = pFormat->duration; 
	std::cout << "Step1: Duration: " << (time / 1000000 / 60) << " mins : " << (time / 1000000 % 60) << " secondes" << std::endl;
	av_dump_format(pFormat, NULL, path, 0);
	std::cout << std::endl;


	// ����������Ѱ�����Ľ�����Ϣ
	ret = avformat_find_stream_info(pFormat, NULL);
	if (ret < 0) {
		std::cout << "Step2: find stream info failed" << std::endl;
		return;
	}
	std::cout << "Step2: find stream info succeed" << std::endl;


	// ���ļ����н���




	avformat_close_input(&pFormat);
}

// ����������Դ

void openNetResource(const char* url) {
	// һ����ѡ��ȫ�������ʼ�����ܣ����ڵ��þɰ汾��GnuTLS��OpenSSL��ʱ����ʹ��
	// avformat_network_init();
	// ������Դ��Ҫ�ȶ��崫�䷽ʽ
	AVDictionary* opt = nullptr;
	av_dict_set(&opt, "rtsp_transport", "tcp", 0);
	av_dict_set(&opt, "max_delay", "550", 0);

	AVFormatContext* pFormat = nullptr;
	int ret = avformat_open_input(&pFormat, url, NULL, &opt);
	if (ret < 0) {
		std::cout << "Step1: open " << url << " failed" << std::endl;
		return;
	}
	std::cout << "Step1: open " << url << " succeed" << std::endl;

}

int main() {

	while (1) {
		std::cout << "******************************" << std::endl;
		std::cout << "** 0. avcodec configuration **" << std::endl;
		std::cout << "** 1. parse YUV420          **" << std::endl;
		std::cout << "** 2. parse local resource  **" << std::endl;
		std::cout << "** 3. parse net resource    **" << std::endl;
		std::cout << "** 4. exit                  **" << std::endl;
		std::cout << "******************************" << std::endl;

		int i;
		std::cin >> i;
		if (i == 4 ) {
			break;
		}

		switch (i) {
		case 0:
			// �鿴FFmpeg�Ŀ��ò���
			std::cout << avcodec_configuration() << std::endl;
			break;
		case 1: 
			// ����YUV420
			parseYUV420("demo.yuv", 720, 480);
			break;
		case 2:
			openLocalResource("demo.mp4");
			break;
		case 3:
			openNetResource("http://devimages.apple.com.edgekey.net/streaming/examples/bipbop_4x3/gear2/prog_index.m3u8");
			break;
		}
	}

	return 0;
}