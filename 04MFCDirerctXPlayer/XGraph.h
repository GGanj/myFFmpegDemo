#pragma once

/*
	B站:千魅太子哥   

所有源码下载=>群公告

	QQ:2475398711

	QQ交流群:168509288
*/

#include <stdio.h>
#include <tchar.h>
#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")
class XGraph
{
public:
	XGraph();
	~XGraph();
public:
	RECT m_rtViewport;
	CRITICAL_SECTION  m_critial;

	IDirect3D9 *			_d3d = NULL;
	IDirect3DDevice9 *		_device = NULL;
	IDirect3DSurface9 *		_surfaceRender = NULL;
	IDirect3DSurface9 *		_pBuffer = NULL;
	D3DLOCKED_RECT			_d3d_rect;
	HRESULT					_Ret;
	

	int InitD3D(HWND hwnd, unsigned long lWidth, unsigned long lHeight);
	
	BOOL BeginRender()
	{
		if (_surfaceRender == NULL)
			return false;
		_Ret = _surfaceRender->LockRect(&_d3d_rect, NULL, D3DLOCK_DONOTWAIT);
		if (FAILED(_Ret))
			return false;
		return true;
	}
	BOOL EndRender()
	{
		_Ret = _surfaceRender->UnlockRect();
		if (FAILED(_Ret))
			return -1;

		if (_device == NULL)
			return -1;

		_device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
		_device->BeginScene();

		_device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &_pBuffer);
		_device->StretchRect(_surfaceRender, NULL, _pBuffer, &m_rtViewport, D3DTEXF_LINEAR);
		_device->EndScene();
		_device->Present(NULL, NULL, NULL, NULL);
	}

	void Clear();
	
};

