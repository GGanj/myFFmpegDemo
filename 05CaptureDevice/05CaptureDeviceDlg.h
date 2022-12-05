
// 05CaptureDeviceDlg.h : header file
//

#pragma once
#include <thread>
#include "XGraph.h"
#include "FFmpegCapture.h"

// CMy05CaptureDeviceDlg dialog
class CMy05CaptureDeviceDlg : public CDialogEx
{
// Construction
public:
	CMy05CaptureDeviceDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MY05CAPTUREDEVICE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	void startCapture();
	void stopCapture();

	CStatic myPlayer;
	CComboBox myDeviceList;

	int playerWidth = 480;
	int playerHeight = 360;

	XGraph myXGraph;
	FFmpegCapture myFFmpegCapture;

};
