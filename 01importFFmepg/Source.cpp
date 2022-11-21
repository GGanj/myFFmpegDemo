#include <iostream>
#include <fstream>

extern "C" {
// FFmpeg 使用C编写，因此导入其lib的头文件时需要使用C风格
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
}

#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")

// 解析YUV文件
void parseYUV420(const char* path, int width, int height) {
	// 以二进制读取YUV文件
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
	
	// 按帧读取YUV的值
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

// 解析本地多媒体文件

void openLocalResource(const char* path) {
	// 新版本已经移除
	//av_register_all();

	AVFormatContext* pFormat = nullptr;
	// 打开文件，获取格式上下文
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


	// 从上下文中寻找流的解码信息
	ret = avformat_find_stream_info(pFormat, NULL);
	if (ret < 0) {
		std::cout << "Step2: find stream info failed" << std::endl;
		return;
	}
	std::cout << "Step2: find stream info succeed" << std::endl;


	// 流文件进行解码




	avformat_close_input(&pFormat);
}

// 解析网络资源

void openNetResource(const char* url) {
	// 一个可选的全局网络初始化功能，仅在调用旧版本的GnuTLS和OpenSSL库时必须使用
	// avformat_network_init();
	// 网络资源需要先定义传输方式
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
			// 查看FFmpeg的可用参数
			std::cout << avcodec_configuration() << std::endl;
			break;
		case 1: 
			// 解析YUV420
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