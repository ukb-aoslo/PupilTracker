// PupilTrackerDlg.cpp : implementation file
//
#include "stdafx.h"
#include "PupilTracker.h"
#include "PupilTrackerMainFrame.h"


// Save some typing...
using namespace DShowLib;

BEGIN_MESSAGE_MAP(CPupilTrackerMainFrame, CFrameWndEx)
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
	ON_REGISTERED_MESSAGE(AFX_WM_RESETTOOLBAR, &CPupilTrackerMainFrame::OnAfxWmResettoolbar)
	ON_COMMAND(ID_EDIT_FIND_COMBO, &CPupilTrackerMainFrame::OnEditFindCombo)
	ON_COMMAND(ID_EDIT_FIND, &CPupilTrackerMainFrame::OnEditFind)
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

	offsetTrackingEnabled = true;
	pupilDiaTrackingEnabled = true;



}

CPupilTrackerMainFrame::~CPupilTrackerMainFrame()
{

	delete m_wndView;

}

void CPupilTrackerMainFrame::initCam() {

	Grabber::tVidCapDevListPtr pVidCapDevList = Grabber.getAvailableVideoCaptureDevices();
	if (pVidCapDevList == 0 || pVidCapDevList->empty())
	{
		AfxMessageBox(L"No signal found! Please plugin camera.", MB_OK); // No device available.
	}

	Grabber.openDev(pVidCapDevList->at(0));

	// Now retrieve the video formats.

	Grabber::tVidFmtListPtr pVidFmtList = Grabber.getAvailableVideoFormats();
	if (pVidFmtList == 0) // No video formats available?
	{
		std::string str = "Error : " + Grabber.getLastError().toString();
		AfxMessageBox((CString)str.c_str(), MB_OK);

	}

	std::string format, required;

	for (auto it = pVidFmtList->begin(); it != pVidFmtList->end(); it++) {
		format = it->toString();
		if (format == "Y800 (640x480)")
			required = format;
	}

	if (!Grabber.setVideoFormat(required))
		AfxMessageBox(L"Couldn't switch to the required video format!", MB_OK);

}

void CPupilTrackerMainFrame::setFilter() {


	// Get a list with all available frame filters.
	std::vector<FilterInfo> filterInfos = FilterLoader::getAvailableFrameFilters(eFC_ALL);

	// Get the index of the currently selected filter.
	if (filterInfos.size() > 0)
	{
		
		// Filters can only be set when live mode is stopped.
		bool bIsLive = Grabber.isLive();
		if (bIsLive)
			Grabber.stopLive();

		// Create the new filter instances.
		DeBayer = FilterLoader::createFilter("DeNoise");
		Grabber.setDeviceFrameFilters(DeBayer.get());

	}

}


void CPupilTrackerMainFrame::setSink() {

	// Create the frame handler sink
	Sink = FrameHandlerSink::create(eRGB8, 5);

	// disable snap mode (formerly tFrameGrabberMode::eSNAP).
	Sink->setSnapMode(false);

	// Apply the sink to the grabber.
	Grabber.setSinkType(Sink);
}


	// Resize the video window to 640*480 pixels.
	//m_cStaticVideoWindow.SetWindowPos(NULL,0,0,640,480,SWP_NOMOVE|SWP_NOZORDER);

	//m_cListener.SetDrawWindow(Grabber.getAcqSizeMaxX(), Grabber.getAcqSizeMaxY());

	// Assign this window to the Grabber object for live video display.
	//Grabber.setHWND(m_cStaticVideoWindow.m_hWnd);

	//// Adjust the live video to the size of the window.
	//CRect rect;
	//m_cStaticVideoWindow.GetClientRect( &rect);
	//Grabber.setDefaultWindowPosition(false);
	//Grabber.setWindowSize(rect.Width(), rect.Height());


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
	if(Grabber.isDevValid() && Grabber.isLive())
	{
		Grabber.stopLive();
	}

	Grabber.showDevicePage(this->m_hWnd);


	// If we have selected a valid device, save it to the file "device.xml", so
	// the application can load it automatically when it is started the next time.
	if( Grabber.isDevValid())
	{
		Grabber.saveDeviceStateToFile("device.xml");
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
	if( Grabber.isDevValid())
	{
		Grabber.showVCDPropertyPage(this->m_hWnd);
		Grabber.saveDeviceStateToFile("device.xml");
	}
	m_wndView->RedrawWindow();
	
}

//////////////////////////////////////////////////////////////////////////
// This method sets the states of the dialog's buttons.
//////////////////////////////////////////////////////////////////////////

//void CPupilTrackerMainFrame::SetButtonStates(void)
//{
//	bool bDevValid =  Grabber.isDevValid();
//	bool bIsLive   = Grabber.isLive();
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
//	if( Grabber.isDevValid())
//	{
//		
//		if (Grabber.isLive())
//		{
//			Grabber.stopLive();
//		}
//		else
//		{
//			Grabber.startLive();
//		}
//		SetButtonStates();
//	}
//}

//////////////////////////////////////////////////////////////////////////

void CPupilTrackerMainFrame::OnClose()
{

	// If live video is running, stop it.


	if (Grabber.isDevValid() && Grabber.isLive())
	{
		Grabber.stopLive();
	}

	if (Grabber.isDevOpen())
	{
		Grabber.closeDev();
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

	if (Grabber.isDevValid())
	{
		title += "The Imaging Source ";
		title += Grabber.getDev().toString().c_str();
	}

	SetWindowText(title);
}

void CPupilTrackerMainFrame::OnPlay()
{
	if (Grabber.isLive())
	{
		// This error should never happen.
		AfxMessageBox(TEXT("Grabber already in live-mode!"));
		return;
	}

	// Stop displaying the captured image in the view window's OnPaint event, 
	// as we will have live video in that window.
	
	if (!Grabber.startLive())
	{
		AfxMessageBox(CString(Grabber.getLastError().toString().c_str()));
	}


	// Redraw the displayed overlays.
	// This may be necessary because the overlay bitmap is erased 
	// when the color format of the overlay bitmap changes.

	if (Grabber.getOverlayBitmapPathPosition() & ePP_DEVICE)
	{
		drawOverlay(ePP_DEVICE);
	}
	if (Grabber.getOverlayBitmapPathPosition() & ePP_SINK)
	{
		drawOverlay(ePP_SINK);
	}
	if (Grabber.getOverlayBitmapPathPosition() & ePP_DISPLAY)
	{
		drawOverlay(ePP_DISPLAY);
	}

}

void CPupilTrackerMainFrame::OnStop()
{


	if (Grabber.isLive())
	{

		if (!Grabber.stopLive())
		{
			if (Grabber.getLastError().isError())
			{
				AfxMessageBox(Grabber.getLastError().toStringW().c_str());
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
	smart_ptr<OverlayBitmap> ob = Grabber.getOverlay(pos);

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
	
	Grabber.setHWND(m_wndView->GetSafeHwnd());
	Grabber.startLive();
	reAdjustView();

}

void CPupilTrackerMainFrame::reAdjustView(){

	CRect m_tbRect, m_vidRect, m_gazeRect, m_graphRect, m_stRect;

	Grabber.getWindowPosition(m_vidRect.left, m_vidRect.top, m_vidRect.right, m_vidRect.bottom);
	if (m_wndToolBar.IsVisible()) 
		m_wndToolBar.GetClientRect(&m_tbRect);
	if (m_wndStatusBar.IsVisible())
		m_wndStatusBar.GetClientRect(&m_stRect);

	if (offsetTrackingEnabled) {
		m_gazeRect.right = 400;
		m_gazeRect.bottom = m_vidRect.Height();
	}

	if (pupilDiaTrackingEnabled) {
		m_graphRect.right = m_vidRect.Width() + m_gazeRect.Width();
		m_graphRect.bottom = m_vidRect.Height() / 2;
	}

	CRect cRect{ 0, 0, m_vidRect.Width() + m_gazeRect.Width(), m_vidRect.Height() + m_graphRect.Height() + m_tbRect.Height() + 10 + m_stRect.Height()};
	CRect wRect;
	GetWindowRect(&wRect);
	AdjustWindowRect(&cRect, this->GetStyle(), TRUE);
	MoveWindow(wRect.left, wRect.top, cRect.right - cRect.left, cRect.bottom - cRect.top, true);

	m_wndView->wndOffset.MoveWindow(m_vidRect.Width() + 2, m_tbRect.Height()+4, m_gazeRect.right, m_gazeRect.Height(), true);
	m_wndView->wndPupilDia.MoveWindow(0, m_vidRect.Height() + m_tbRect.Height() + 4, m_graphRect.Width(), m_graphRect.Height(), true);
}

int CPupilTrackerMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	pupilTracking.SetParent(this);

	// Add listener(s) to grabber
	Grabber.addListener(&pupilTracking, CListener::eOVERLAYCALLBACK | CListener::eFRAMEREADY);


	// Set filter
	//setFilter();

	// Set the sink
	setSink();
	
	m_hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(m_hIcon, FALSE);

	
	// Try to load the previously used video capture device.
	if (!Grabber.loadDeviceStateFromFile("device.xml"))
		Grabber.showDevicePage(this->m_hWnd);

	initCam();

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
			sizeof(indicators) / sizeof(UINT)))
	{
		TRACE0("Failed to create Status Bar!\n");
		return -1;      // Fehler bei Erstellung
	}

	DWORD dwCtrlStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY;
	DWORD dwStyle = AFX_DEFAULT_TOOLBAR_STYLE;

	const CRect r1(1, 1, 1, 1);

	if (!m_wndToolBar.Create(this, dwCtrlStyle, dwStyle) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // failed to create
	}

	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndToolBar);

	// Attach View to active tracking Method

	m_wndView = new CChildView(this);

	// create a view to occupy the client area of the frame
	if (!m_wndView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}

	// Set overlay to color
	if (!Grabber.setOverlayBitmapPathPosition(ePP_DISPLAY))

	{
		LPCTSTR error = Grabber.getLastError().getStringW().c_str();
		MessageBox(error, L"Error", MB_ICONERROR);
	}

	//Grabber.getOverlay(ePP_DEVICE)->setColorMode(OverlayBitmap::eBESTFIT);
	//Grabber.getOverlay(ePP_SINK)->setColorMode(OverlayBitmap::eGRAYSCALE);
	Grabber.getOverlay(ePP_DISPLAY)->setColorMode(OverlayBitmap::eCOLOR);
	drawOverlay(ePP_DISPLAY);

	if (Grabber.getLastError().isError())
	{
		AfxMessageBox(Grabber.getLastError().toStringW().c_str());
	}

	updateWindowTitle();
	
	return 0;
}


afx_msg LRESULT CPupilTrackerMainFrame::OnAfxWmResettoolbar(WPARAM wParam, LPARAM lParam)
{

	UINT uiToolBarId = (UINT)wParam;

	switch (uiToolBarId)
	{
	case IDR_MAINFRAME:
	{
		
		CFindComboButton combo(ID_EDIT_FIND_COMBO, GetCmdMgr()->GetCmdImage(ID_EDIT_FIND), CBS_DROPDOWN);
		m_wndToolBar.ReplaceButton(ID_EDIT_FIND, combo);

	}

	break;

	}

	return 0;

}


BOOL CPupilTrackerMainFrame::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam) {
		case VK_SPACE:
		
			
			if (Grabber.isDevValid() && Grabber.isLive()) {
				pupilTracking.freezePupil();
			}
			else

				if (pupilTracking.record) {
					Save();
					pupilTracking.record = false;
				
			}

			break;
		}
		
		return __super::PreTranslateMessage(pMsg);
	}

	return __super::PreTranslateMessage(pMsg);

}

void CPupilTrackerMainFrame::OnViewParameters()
{
	// TODO: Add your command handler code here
	params.DoModal();
	
}


void CPupilTrackerMainFrame::OnRecord()
{
	// TODO: Add your command handler code here
	
	if (pupilTracking.record == true) {

		pupilTracking.record = false;
		OnStop();
		Save();
		OnPlay();

	}

	else

		pupilTracking.record = true;
		pupilTracking.recIndex = pupilTracking.pupil.offsetPX.size();

}

void CPupilTrackerMainFrame::OnUpdateRecord(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (pupilTracking.record)
		pCmdUI->SetCheck(1);
	else 
		pCmdUI->SetCheck(0);
}


void CPupilTrackerMainFrame::OnMakeSnapshot()
{
	if (Grabber.isLive())

	{
		pupilTracking.setSnap(true);
	}

}


void CPupilTrackerMainFrame::OnUpdateMakeSnapshot(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here

}


void CPupilTrackerMainFrame::OnEditFindCombo()
{
	// TODO: Add your command handler code here
	CObList listButtons;
	m_wndToolBar.GetCommandButtons(ID_EDIT_FIND_COMBO, listButtons);

}


void CPupilTrackerMainFrame::OnEditFind()
{
	// TODO: Add your command handler code here

	/*if(m_wndToolBar.IsLastCommandFromButton()) {

	}
	*/
}

void CPupilTrackerMainFrame::Save()
{
	CString name, version;
	CPupilTrackerApp* pApp = (CPupilTrackerApp*)AfxGetApp();
	pApp->GetProductAndVersion(name, version);

	CString szFileName;
	FILE *pFile;
	CFileDialog cFileDlg(
		FALSE,
		(LPCTSTR)NULL,
		(LPCTSTR)NULL,
		OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_OVERWRITEPROMPT | OFN_CREATEPROMPT,
		_T("All Files (*.*)|*.*|"),
		NULL);
	cFileDlg.m_ofn.lpstrInitialDir = _T(".\\");

	if (cFileDlg.DoModal() == IDOK)
	{
		szFileName = cFileDlg.GetPathName();
		_wfopen_s(&pFile, szFileName, L"w");

		if (pFile)
		{
			// this saves the data to the file

			fprintf(pFile, "**************************************************************************************\n");
			fprintf(pFile, "********** %ls %ls Offset Tracker Data							**********\n", name.GetString(), version.GetString());
			fprintf(pFile, "**************************************************************************************\n\n");

			fprintf(pFile, "\nNOTE: offset data is in (sub)pixels\n");
			fprintf(pFile, "\nMagnifiction [mm/px]: %2.15f", double(1/MM_PER_PIXEL));
			fprintf(pFile, "\nLocked position [px]: x:%2.1f y:%2.1f", pupilTracking.pupil.frozen_center.x, pupilTracking.pupil.frozen_center.y);

			fprintf(pFile, "\n\ndata is: time [hour:min:sec:msec] frame number, pupil size [mm], pupil center [pxx], pupil center [pxy], purkinje center [pxx], purkinje center [pxy], center offset horizontal [mm], center offset vertical [mm] \n\n");

			for (size_t i = pupilTracking.recIndex; i < pupilTracking.pupil.center.size(); i++) {
				fprintf(pFile, "%02d:%02d:%02d:%04d\t%d\t%2.2f\t%2.1f\t%2.1f\t%2.1f\t%2.1f\t%2.5f\t%2.5f\n",
					pupilTracking.timestamps[i].wHour,
					pupilTracking.timestamps[i].wMinute, 
					pupilTracking.timestamps[i].wSecond,
					pupilTracking.timestamps[i].wMilliseconds,
					i,
					pupilTracking.pupil.diameter[i],
					pupilTracking.pupil.center[i].x,
					pupilTracking.pupil.center[i].y,
					pupilTracking.purkinje.center[i].x,
					pupilTracking.purkinje.center[i].y,
					pupilTracking.pupil.offsetMM[i].x,
					pupilTracking.pupil.offsetMM[i].y);
			}

			fclose(pFile);

			MessageBox(szFileName, _T("Data saved!"), MB_ICONINFORMATION | MB_OK | MB_DEFBUTTON1);

		}

		else
		{
			MessageBox(_T("Failed to open file!"), _T("Error"), MB_ICONHAND | MB_OK | MB_DEFBUTTON1);
		}

	}
}