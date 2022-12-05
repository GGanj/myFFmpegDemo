#include "XGraph.h"



XGraph::XGraph()
{
}


XGraph::~XGraph()
{
}

int XGraph::InitD3D(HWND hwnd, unsigned long lWidth, unsigned long lHeight)
{
		HRESULT lRet;
		InitializeCriticalSection(&m_critial);
		Clear();

		_d3d = Direct3DCreate9(D3D_SDK_VERSION);
		if (_d3d == NULL)
			return -1;

		D3DPRESENT_PARAMETERS d3dpp;
		ZeroMemory(&d3dpp, sizeof(d3dpp));
		d3dpp.Windowed = TRUE;
		d3dpp.SwapEffect = D3DSWAPEFFECT_COPY;
		d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
		GetClientRect(hwnd, &m_rtViewport);
		lRet = _d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp, &_device);
		if (FAILED(lRet))
			return -1;

		//yuv420p   D3DFMT_X8R8G8B8
		lRet = _device->CreateOffscreenPlainSurface(
			lWidth, lHeight,
			(D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2'),
			D3DPOOL_DEFAULT,
			&_surfaceRender,
			NULL);
		
		if (FAILED(lRet))
			return -1;
		return 0;
}

void XGraph::Clear()
{
	EnterCriticalSection(&m_critial);
	if (_surfaceRender)
		_surfaceRender->Release();
	if (_device)
		_device->Release();
	if (_d3d)
		_d3d->Release();
	LeaveCriticalSection(&m_critial);

}
