
// 05CaptureDevice.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CMy05CaptureDeviceApp:
// See 05CaptureDevice.cpp for the implementation of this class
//

class CMy05CaptureDeviceApp : public CWinApp
{
public:
	CMy05CaptureDeviceApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CMy05CaptureDeviceApp theApp;
