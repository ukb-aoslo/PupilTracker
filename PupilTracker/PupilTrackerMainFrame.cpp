// PupilTrackerDlg.cpp : implementation file
//
#include "stdafx.h"
#include "PupilTracker.h"
#include "PupilTrackerMainFrame.h"

extern std::mutex g_mutex;

// Save some typing...
using namespace DShowLib;

BEGIN_MESSAGE_MAP(CPupilTrackerMainFrame, CFrameWnd)
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_WM_CREATE()
	ON_COMMAND (ID_CAMERA_DEVICES, OnBnClickedButtondevice)
	ON_COMMAND (ID_CAMERA_SETTINGS, OnBnClickedButtonimagesettings)
	ON_COMMAND (ID_STOP, OnStop)
	ON_COMMAND (ID_PLAY, OnPlay)
	ON_COMMAND (ID_VIEW_PARAMETERS, &CPupilTrackerMainFrame::OnViewParameters)
	ON_COMMAND (ID_RECORD, &CPupilTrackerMainFrame::OnRecord)
	ON_UPDATE_COMMAND_UI (ID_RECORD, &CPupilTrackerMainFrame::OnUpdateRecord)
	ON_COMMAND (ID_MAKE_SNAPSHOT, &CPupilTrackerMainFrame::OnMakeSnapshot)
	ON_UPDATE_COMMAND_UI (ID_MAKE_SNAPSHOT, &CPupilTrackerMainFrame::OnUpdateMakeSnapshot)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // Statusleistenanzeige
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

CPupilTrackerMainFrame::CPupilTrackerMainFrame(){
	// Set active tracking method
	
	m_cGazeOn = true;
	m_cGraphOn = true;

	// Add the C object to the the Grabber object
	m_pGaze = new Gaze();
	m_pGraph = new Graph();
	m_pListener = new Schaeffel(m_pGaze, m_pGraph);
	
	// Add listener(s) to grabber
	m_cGrabber.addListener(m_pListener, DShowLib::GrabberListener::eOVERLAYCALLBACK | DShowLib::GrabberListener::eFRAMEREADY);
	
	// Set filter
	//setFilter();

	// Set the sink
	setSink();

	// Set the tracking parameters
	setParams();

	DWORD dwStyle = (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_CLIPCHILDREN);

	Create(NULL,
		NULL,
		dwStyle,
		CRect(),
		NULL,
		NULL
		);

	m_pParams = new Parameters(this);
	m_pParams->Create(IDD_DIALOG1);
	
	m_hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(m_hIcon, FALSE);

}

CPupilTrackerMainFrame::~CPupilTrackerMainFrame()
{
	delete m_pListener;
	delete m_pGaze;
	delete m_pGraph;
	delete m_wndView;
	delete m_pParams;

}

void CPupilTrackerMainFrame::initCam() {

	Grabber::tVidCapDevListPtr pVidCapDevList = m_cGrabber.getAvailableVideoCaptureDevices();
	if (pVidCapDevList == 0 || pVidCapDevList->empty())
	{
		AfxMessageBox(L"No signal found! Please plugin camera.", MB_OK); // No device available.
	}

	m_cGrabber.openDev(pVidCapDevList->at(0));

	// Now retrieve the video formats.

	Grabber::tVidFmtListPtr pVidFmtList = m_cGrabber.getAvailableVideoFormats();
	if (pVidFmtList == 0) // No video formats available?
	{
		std::string str = "Error : " + m_cGrabber.getLastError().toString();
		AfxMessageBox((CString)str.c_str(), MB_OK);

	}

	std::string format, required;

	for (auto it = pVidFmtList->begin(); it != pVidFmtList->end(); it++) {
		format = it->toString();
		if (format == "Y800 (640x480)")
			required = format;
	}

	if (!m_cGrabber.setVideoFormat(required))
		AfxMessageBox(L"Couldn't switch to the required video format!", MB_OK);

}

void CPupilTrackerMainFrame::setFilter() {


	// Get a list with all available frame filters.
	std::vector<FilterInfo> filterInfos = FilterLoader::getAvailableFrameFilters(eFC_ALL);

	// Get the index of the currently selected filter.
	if (filterInfos.size() > 0)
	{
		
		// Filters can only be set when live mode is stopped.
		bool bIsLive = m_cGrabber.isLive();
		if (bIsLive)
			m_cGrabber.stopLive();

		// Create the new filter instances.
		m_pDeBayer = FilterLoader::createFilter("DeNoise");
		m_cGrabber.setDeviceFrameFilters(m_pDeBayer.get());

	}

}


void CPupilTrackerMainFrame::setSink() {

	// Create the frame handler sink
	m_pSink = FrameHandlerSink::create(eRGB8, 5);

	// disable snap mode (formerly tFrameGrabberMode::eSNAP).
	m_pSink->setSnapMode(false);

	// Apply the sink to the grabber.
	m_cGrabber.setSinkType(m_pSink);
}


	// Resize the video window to 640*480 pixels.
	//m_cStaticVideoWindow.SetWindowPos(NULL,0,0,640,480,SWP_NOMOVE|SWP_NOZORDER);

	//m_cListener.SetDrawWindow(m_cGrabber.getAcqSizeMaxX(), m_cGrabber.getAcqSizeMaxY());

	// Assign this window to the Grabber object for live video display.
	//m_cGrabber.setHWND(m_cStaticVideoWindow.m_hWnd);

	//// Adjust the live video to the size of the window.
	//CRect rect;
	//m_cStaticVideoWindow.GetClientRect( &rect);
	//m_cGrabber.setDefaultWindowPosition(false);
	//m_cGrabber.setWindowSize(rect.Width(), rect.Height());


void CPupilTrackerMainFrame::OnPaint() 
{

	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle.
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
		CFrameWnd::OnPaint();
	}
	
}

//////////////////////////////////////////////////////////////////////////
// Show the device select dialog, if the "Device" button has been clicked.
// If live video is displayed, it has to be stopped.
//////////////////////////////////////////////////////////////////////////

void CPupilTrackerMainFrame::OnBnClickedButtondevice()
{
	// If live video is running, stop it.
	if(m_cGrabber.isDevValid() && m_cGrabber.isLive())
	{
		m_cGrabber.stopLive();
	}

	m_cGrabber.showDevicePage(this->m_hWnd);


	// If we have selected a valid device, save it to the file "device.xml", so
	// the application can load it automatically when it is started the next time.
	if( m_cGrabber.isDevValid())
	{
		m_cGrabber.saveDeviceStateToFile("device.xml");
	}

	// Now display the device's name in the caption bar of the application.
	reAdjustView();
	updateWindowTitle();
	OnPlay();

}

//////////////////////////////////////////////////////////////////////////
// Show the image settings dialog of IC Imaging Control. 
//////////////////////////////////////////////////////////////////////////

void CPupilTrackerMainFrame::OnBnClickedButtonimagesettings()
{
	if( m_cGrabber.isDevValid())
	{
		m_cGrabber.showVCDPropertyPage(this->m_hWnd);
		m_cGrabber.saveDeviceStateToFile("device.xml");
	}
	m_wndView->RedrawWindow();
	
}

//////////////////////////////////////////////////////////////////////////
// This method sets the states of the dialog's buttons.
//////////////////////////////////////////////////////////////////////////

//void CPupilTrackerMainFrame::SetButtonStates(void)
//{
//	bool bDevValid =  m_cGrabber.isDevValid();
//	bool bIsLive   = m_cGrabber.isLive();
//
//	m_cButtonSettings.EnableWindow(bDevValid);
//	m_cButtonLive.EnableWindow(bDevValid);
//	
//	if( !bDevValid )
//	{
//		m_cButtonLive.SetWindowText(TEXT("Live Start"));
//	}
//
//
//	if( bIsLive )
//	{
//		m_cButtonLive.SetWindowText(TEXT("Live Stop"));
//	}
//	else
//	{
//		m_cButtonLive.SetWindowText(TEXT("Live Start"));
//	}
//
//
//}

//////////////////////////////////////////////////////////////////////////
// This method starts and stops the live video.
//////////////////////////////////////////////////////////////////////////

//void CPupilTrackerMainFrame::OnBnClickedButtonlivevideo()
//{
//
//	if( m_cGrabber.isDevValid())
//	{
//		
//		if (m_cGrabber.isLive())
//		{
//			m_cGrabber.stopLive();
//		}
//		else
//		{
//			m_cGrabber.startLive();
//		}
//		SetButtonStates();
//	}
//}

//////////////////////////////////////////////////////////////////////////

void CPupilTrackerMainFrame::OnClose()
{

	// If live video is running, stop it.


	if (m_cGrabber.isDevValid() && m_cGrabber.isLive())
	{
		m_cGrabber.stopLive();
	}

	if (m_cGrabber.isDevOpen())
	{
		m_cGrabber.closeDev();
	}


	CFrameWnd::OnClose();

}

void CPupilTrackerMainFrame::updateWindowTitle()
{
	// Display the current used video capture device in the window title bar.
	
	CString name, version;
	
	CPupilTrackerApp* pApp = (CPupilTrackerApp*)AfxGetApp();
	pApp->GetProductAndVersion(name, version);

	CString title = name + " v" + version + " using ";

	if (m_cGrabber.isDevValid())
	{
		title += "The Imaging Source ";
		title += m_cGrabber.getDev().toString().c_str();
	}

	SetWindowText(title);
}

void CPupilTrackerMainFrame::OnPlay()
{
	if (m_cGrabber.isLive())
	{
		// This error should never happen.
		AfxMessageBox(TEXT("Grabber already in live-mode!"));
		return;
	}

	// Stop displaying the captured image in the view window's OnPaint event, 
	// as we will have live video in that window.
	
	if (!m_cGrabber.startLive())
	{
		AfxMessageBox(CString(m_cGrabber.getLastError().toString().c_str()));
	}


	// Redraw the displayed overlays.
	// This may be necessary because the overlay bitmap is erased 
	// when the color format of the overlay bitmap changes.
	if (m_cGrabber.getOverlayBitmapPathPosition() & ePP_DEVICE)
	{
		drawOverlay(ePP_DEVICE);
	}
	if (m_cGrabber.getOverlayBitmapPathPosition() & ePP_SINK)
	{
		drawOverlay(ePP_SINK);
	}
	if (m_cGrabber.getOverlayBitmapPathPosition() & ePP_DISPLAY)
	{
		drawOverlay(ePP_DISPLAY);
	}

}

void CPupilTrackerMainFrame::OnStop()
{


	if (m_cGrabber.isLive())
	{

		if (!m_cGrabber.stopLive())
		{
			if (m_cGrabber.getLastError().isError())
			{
				AfxMessageBox(m_cGrabber.getLastError().toStringW().c_str());
			}

		}

	}

}


void CPupilTrackerMainFrame::OnDestroy()
{
	
	WINDOWPLACEMENT wp;
	GetWindowPlacement(&wp);
	AfxGetApp()->WriteProfileBinary(L"Settings", L"WP", (LPBYTE)&wp, sizeof(wp));

	CFrameWnd::OnDestroy();

	// TODO: Add your message handler code here
}

void CPupilTrackerMainFrame::drawOverlay(DShowLib::tPathPosition pos)
{
	smart_ptr<OverlayBitmap> ob = m_cGrabber.getOverlay(pos);

	ob->setEnable(true);

}


void CPupilTrackerMainFrame::OnShowWindow(BOOL bShow, UINT nStatus)
{
	static bool bOnce = true;

	if (bShow && !IsWindowVisible()
		&& bOnce)
	{
		bOnce = false;

		WINDOWPLACEMENT *lwp;
		UINT nl;

		if (AfxGetApp()->GetProfileBinary(L"Settings", L"WP", (LPBYTE*)&lwp, &nl))
		{
			SetWindowPlacement(lwp);
			delete[] lwp;
		}
	}

	CFrameWnd::OnShowWindow(bShow, nStatus);
	// TODO: Add your message handler code here
	
	m_cGrabber.setHWND(m_wndView->GetSafeHwnd());
	m_cGrabber.startLive();
	reAdjustView();

	m_wndView->m_pWndGaze->SetTimer(1, 333, NULL);

}

void CPupilTrackerMainFrame::reAdjustView(){

	CRect m_tbRect, m_vidRect, m_gazeRect, m_graphRect, m_stRect;

	m_cGrabber.getWindowPosition(m_vidRect.left, m_vidRect.top, m_vidRect.right, m_vidRect.bottom);
	if (m_wndToolBar.IsVisible()) 
		m_wndToolBar.GetClientRect(&m_tbRect);
	if (m_wndStatusBar.IsVisible())
		m_wndStatusBar.GetClientRect(&m_stRect);

	if (m_cGazeOn) {
		m_gazeRect.right = 400;
		m_gazeRect.bottom = m_vidRect.Height();
	}

	if (m_cGraphOn) {
		m_graphRect.right = m_vidRect.Width() + m_gazeRect.Width();
		m_graphRect.bottom = m_vidRect.Height() / 2;
	}

	CRect cRect{ 0, 0, m_vidRect.Width() + m_gazeRect.Width(), m_vidRect.Height() + m_graphRect.Height() + m_tbRect.Height() + 10 + m_stRect.Height()};
	CRect wRect;
	GetWindowRect(&wRect);
	AdjustWindowRect(&cRect, this->GetStyle(), TRUE);
	MoveWindow(wRect.left, wRect.top, cRect.right - cRect.left, cRect.bottom - cRect.top, true);

	m_wndView->m_pWndGaze->MoveWindow(m_vidRect.Width() + 2, m_tbRect.Height()+4, m_gazeRect.right, m_gazeRect.Height(), true);
	m_wndView->m_pWndGraph->MoveWindow(0, m_vidRect.Height() + m_tbRect.Height() + 4, m_graphRect.Width(), m_graphRect.Height(), true);
}

int CPupilTrackerMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// Try to load the previously used video capture device.
	if (!m_cGrabber.loadDeviceStateFromFile("device.xml"))
		m_cGrabber.showDevicePage(this->m_hWnd);

	// must be done here, not in constructor like it should
	initCam();

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
			sizeof(indicators) / sizeof(UINT)))
	{
		TRACE0("Failed to create Status Bar!\n");
		return -1;      // Fehler bei Erstellung
	}

	DWORD dwCtrlStyle = TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | CBRS_SIZE_DYNAMIC | CBRS_FLYBY;
	DWORD dwStyle = AFX_DEFAULT_TOOLBAR_STYLE;

	const CRect r1(1, 1, 1, 1);

	if (!m_wndToolBar.CreateEx(this, dwCtrlStyle, dwStyle, r1) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // failed to create
	}

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndToolBar);

	//CRect rect;
	//rect.top = 1;
	//rect.right = 200;
	//rect.top = 1;
	//rect.bottom = rect.top + 250 /*drop height*/;

	//if (!m_comboBox.Create(CBS_DROPDOWNLIST | CBS_SORT | WS_VISIBLE |
	//	WS_TABSTOP | WS_VSCROLL, rect, &m_wndToolBar, ID_FINDCOMBO))
	//{
	//	TRACE(_T("Failed to create combo-box\n"));
	//	return FALSE;
	//}

	// Attach View to active tracking Method
	m_wndView = new CChildView(this);

	// create a view to occupy the client area of the frame
	if (!m_wndView->Create(NULL, NULL, WS_VISIBLE | WS_CHILD, CRect(), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}

	// Set overlay to color
	if (!m_cGrabber.setOverlayBitmapPathPosition(ePP_DISPLAY))
	{
		LPCTSTR error = m_cGrabber.getLastError().getStringW().c_str();
		MessageBox(error, L"Error", MB_ICONERROR);
	}

	//m_cGrabber.getOverlay(ePP_DEVICE)->setColorMode(OverlayBitmap::eBESTFIT);
	//m_cGrabber.getOverlay(ePP_SINK)->setColorMode(OverlayBitmap::eGRAYSCALE);
	m_cGrabber.getOverlay(ePP_DISPLAY)->setColorMode(OverlayBitmap::eCOLOR);
	drawOverlay(ePP_DISPLAY);

	if (m_cGrabber.getLastError().isError())
	{
		AfxMessageBox(m_cGrabber.getLastError().toStringW().c_str());
	}

	updateWindowTitle();
	
	return 0;
}

BOOL CPupilTrackerMainFrame::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam) {
		case VK_SPACE:
		
			
			if (m_cGrabber.isDevValid() && m_cGrabber.isLive()) {
				m_pListener->freezePupil();
				m_pGraph->freeze();
			}

			break;
		}
		
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
	
}

void CPupilTrackerMainFrame::OnViewParameters()
{
	// TODO: Add your command handler code here
	m_pParams->ShowWindow(SW_SHOW);
	
}


void CPupilTrackerMainFrame::setParams()
{
	
	UINT n;
	LPBYTE ppData;
	
	if (AfxGetApp()->GetProfileBinary(L"Settings", L"Schaeffel_thresh", &ppData, &n)) {
		m_pListener->threshold = (BYTE)ppData[0];
	}

	delete ppData;

	if (AfxGetApp()->GetProfileBinary(L"Settings", L"Schaeffel_opts", &ppData, &n)) {
		m_pListener->opts = (BYTE)ppData[0];
	}

	delete ppData;

	if (AfxGetApp()->GetProfileBinary(L"Settings", L"Schaeffel_bufsize", &ppData, &n)) {
		m_pListener->buf_size = (BYTE)ppData[0];
	}
	
	delete ppData;

	if (AfxGetApp()->GetProfileBinary(L"Settings", L"Schaeffel_spotsize", &ppData, &n)) {
		m_pListener->spot_size = (BYTE)ppData[0];
	}

	delete ppData;

	if (AfxGetApp()->GetProfileBinary(L"Settings", L"Schaeffel_boxsize", &ppData, &n)) {
		m_pListener->box_size = (BYTE)ppData[0];
	}

	delete ppData;

}


void CPupilTrackerMainFrame::OnRecord()
{
	// TODO: Add your command handler code here
	
	if (m_pListener->m_pGaze->record == true) {
		m_pListener->m_pGaze->record = false;
		OnStop();
		m_pListener->m_pGaze->Save();
		OnPlay();
	}
	else
		m_pListener->m_pGaze->record = true;
		m_pListener->m_pGaze->recIndex = m_pGaze->getGazePxSize();

}



void CPupilTrackerMainFrame::OnUpdateRecord(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (m_pGaze->record)
		pCmdUI->SetCheck(1);
	else 
		pCmdUI->SetCheck(0);
}


void CPupilTrackerMainFrame::OnMakeSnapshot()
{
	if (m_cGrabber.isLive())
	{
		m_pListener->snap = true;
	}

}


void CPupilTrackerMainFrame::OnUpdateMakeSnapshot(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here

}

