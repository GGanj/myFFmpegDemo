
// 05CaptureDeviceDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "05CaptureDevice.h"
#include "05CaptureDeviceDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMy05CaptureDeviceDlg dialog
HANDLE g_hOutput = NULL;


CMy05CaptureDeviceDlg::CMy05CaptureDeviceDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MY05CAPTUREDEVICE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMy05CaptureDeviceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PLAYER, myPlayer);
	DDX_Control(pDX, IDC_DEVICELIST, myDeviceList);
}

BEGIN_MESSAGE_MAP(CMy05CaptureDeviceDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_START, &CMy05CaptureDeviceDlg::startCapture)
	ON_BN_CLICKED(ID_STOP, &CMy05CaptureDeviceDlg::stopCapture)
END_MESSAGE_MAP()


// CMy05CaptureDeviceDlg message handlers

BOOL CMy05CaptureDeviceDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	AllocConsole();
	g_hOutput = GetStdHandle(STD_OUTPUT_HANDLE);

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	myDeviceList.AddString(L"Integrated Camera");

	myXGraph.InitD3D(myPlayer.m_hWnd, playerWidth, playerHeight);
	myFFmpegCapture.init(playerWidth, playerHeight, &myXGraph);
	myFFmpegCapture.getDeviceInfo();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMy05CaptureDeviceDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMy05CaptureDeviceDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMy05CaptureDeviceDlg::startCapture() {
	CString deviceName;
	myDeviceList.GetLBText(myDeviceList.GetCurSel(), deviceName);
	myFFmpegCapture.selectDevice(deviceName);
	myFFmpegCapture.initCodec();
	myFFmpegCapture.initScale();
	std::thread playerThread(&FFmpegCapture::startCaputre, std::ref(myFFmpegCapture));
	playerThread.detach();
}

void CMy05CaptureDeviceDlg::stopCapture() {
	myFFmpegCapture.stop();
}

