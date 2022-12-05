
// 04MFCDirerctXPlayerDlg.h : header file
//

#pragma once
#include "XGraph.h"
#include "FFmpegRender.h"

// CMy04MFCDirerctXPlayerDlg dialog
class CMy04MFCDirerctXPlayerDlg : public CDialogEx
{
// Construction
public:
	CMy04MFCDirerctXPlayerDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MY04MFCDIRERCTXPLAYER_DIALOG };
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

	afx_msg void OnBnClickedSubmitbtn();
	CStatic myPlayer;
	XGraph myXGraph;
	FFmpegRender myFFmpegRender;
};
