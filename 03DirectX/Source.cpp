#include <Windows.h>

#include "XGraph.h"
#include "FFmpegRender.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT msgID, WPARAM wParam, LPARAM lParam) {
	switch (msgID) {
	case WM_DESTROY:
		PostMessage(hWnd, WM_QUIT, wParam, lParam);
		break;
	}
	return DefWindowProc(hWnd, msgID, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hIns, HINSTANCE hPreIns, LPSTR lpCmdLine, int nCmdShow) {

	WNDCLASS wc = { 0 };
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.hCursor = NULL;
	wc.hIcon = NULL;
	wc.hInstance = hIns;
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = L"DirectXDemo";
	wc.lpszMenuName = NULL;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&wc);

	HWND hWnd = CreateWindowEx(0, L"DirectXDemo", L"DirectX Demo", WS_OVERLAPPEDWINDOW, 100, 100, 1200, 800, NULL, NULL, hIns, NULL);

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	XGraph myXGraph;
	myXGraph.InitD3D(hWnd, 1200, 800);
	FFmpegRender myFFmpegRender("demo.mp4", 1200, 800, &myXGraph);
	myFFmpegRender.initRender();
	myFFmpegRender.initCodec();
	myFFmpegRender.initScale();
	
	MSG nMsg = { 0 };
	while (GetMessage(&nMsg, NULL, 0, 0)) {
		if (PeekMessage(&nMsg, NULL, 0, 0, PM_NOREMOVE)) {
				TranslateMessage(&nMsg);
				DispatchMessage(&nMsg);
		}
		else {
			// idle
			myFFmpegRender.startRender();
		}
	}
	myXGraph.Clear();
	return 0;
}