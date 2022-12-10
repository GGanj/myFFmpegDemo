#pragma once
#include <iostream>
#include <d3d9.h>

#pragma comment(lib, "d3d9.lib")

class D3DCapture
{
public:
	D3DCapture(int width, int height) : screenWidth(width), screenHeight(height) {}
	void screenCaputre(void* data) {
		if (pD3d9Obj == nullptr) {
			pD3d9Obj = Direct3DCreate9(D3D_SDK_VERSION);
			if (pD3d9Obj == nullptr) {
				std::cout << "Create D3D9 object fail" << std::endl;
				return;
			}
		}

		if (pGraphDevice == nullptr) {
			D3DPRESENT_PARAMETERS pa;
			ZeroMemory(&pa, sizeof(D3DPRESENT_PARAMETERS));
			pa.Windowed = true;
			pa.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
			pa.SwapEffect = D3DSWAPEFFECT_DISCARD;
			pa.hDeviceWindow = GetDesktopWindow();
			pD3d9Obj->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 0,
				D3DCREATE_HARDWARE_VERTEXPROCESSING, &pa, &pGraphDevice);

			if (pGraphDevice == nullptr) {
				std::cout << "Create graph device object fail" << std::endl;
				return;
			}
		}

		// 创建离屏表面
		//int width = GetSystemMetrics(SM_CXSCREEN);
		//int height = GetSystemMetrics(SM_CYSCREEN);
		if (pSurface == nullptr) {
			pGraphDevice->CreateOffscreenPlainSurface(screenWidth, screenHeight, 
				D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &pSurface, 0);
			if (pSurface == nullptr) {
				std::cout << "Create off screen surface object fail" << std::endl;
				return;
			}
		}

		// 抓取屏幕
		HRESULT ret = pGraphDevice->GetFrontBufferData(0, pSurface);
		if (ret != D3D_OK) {
			std::cout << "Get front buffer data fail" << std::endl;
			return;
		}
		// 锁定并取出数据
		D3DLOCKED_RECT rect;
		ZeroMemory(&rect, sizeof(rect));
		if (pSurface->LockRect(&rect, 0, 0) != S_OK) {
			std::cout << "D3D Lock fail" << std::endl;
		}
		memcpy(data, rect.pBits, screenWidth * screenHeight * 4);
		pSurface->UnlockRect();
		pSurface->Release();
		pSurface = nullptr;
	}

	void startCapture() {
		FILE* pF = nullptr;
		fopen_s(&pF, "test.rgb", "wb+");
		int size = screenWidth * screenHeight * 4;
		char* pBuff = new char[size];
		for (int i = 0; i < 100; ++i) {
			std::cout << "Capture Frame " << i << std::endl;
			screenCaputre(pBuff);
			fwrite(pBuff, 1, size, pF);
			Sleep(10);
		}
		if (pF != nullptr) {
			fclose(pF);
			pF = nullptr;
		};
	}

public:
	int screenWidth = 1920;
	int screenHeight = 1080;
	// Directx3D对象
	IDirect3D9* pD3d9Obj = nullptr;
	// 显卡设备对象
	IDirect3DDevice9* pGraphDevice = nullptr;
	// 离屏表面对象
	IDirect3DSurface9* pSurface = nullptr;
};

