// PupilTrackerDlg.cpp : implementation file

#include "stdafx.h"
#include "PupilTracker.h"
#include "PupilTrackerMainFrame.h"


// Save some typing...
using namespace DShowLib;

BEGIN_MESSAGE_MAP(CPupilTrackerMainFrame, CFrameWnd)
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_COMMAND (ID_CAMERA_DEVICES, OnBnClickedButtondevice)
	ON_COMMAND (ID_CAMERA_SETTINGS, OnBnClickedButtonimagesettings)
	ON_COMMAND (ID_STOP, OnStop)
	ON_COMMAND (ID_PLAY, OnPlay)
	ON_COMMAND (ID_VIEW_PARAMETERS, &CPupilTrackerMainFrame::OnViewParameters)
	ON_COMMAND (ID_RECORD, &CPupilTrackerMainFrame::OnRecord)
	ON_COMMAND (ID_MAKE_SNAPSHOT, &CPupilTrackerMainFrame::OnMakeSnapshot)
	ON_UPDATE_COMMAND_UI (ID_RECORD, &CPupilTrackerMainFrame::OnUpdateRecord)
	ON_UPDATE_COMMAND_UI (ID_MAKE_SNAPSHOT, &CPupilTrackerMainFrame::OnUpdateMakeSnapshot)
	ON_REGISTERED_MESSAGE(AFX_WM_RESETTOOLBAR, &CPupilTrackerMainFrame::OnAfxWmResettoolbar)
	ON_COMMAND(ID_EDIT_FIND_COMBO, &CPupilTrackerMainFrame::OnEditFindCombo)
	ON_COMMAND(ID_EDIT_FIND, &CPupilTrackerMainFrame::OnEditFind)
	ON_MESSAGE(MESSAGEDEVICELOST, OnDeviceLost)
	ON_MESSAGE(MESSAGE_OFFSETMM_PROCESSED, &CPupilTrackerMainFrame::OnMessageOffsetmmProcessed)
	ON_MESSAGE(MESSAGE_PUPILDIA_PROCESSED, &CPupilTrackerMainFrame::OnMessagePupilDiaProcessed)
	ON_MESSAGE(MESSAGE_OFFSET_LOCKEDPOS, &CPupilTrackerMainFrame::OnMessageOffsetLockedPos)
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_PICK_FOLDER, &CPupilTrackerMainFrame::OnPickFolder)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_LINK1,
};

CPupilTrackerMainFrame::CPupilTrackerMainFrame(){
	// Set active tracking method

	offsetMM = NULL;
	m_pSock_AOMCONTROL = NULL;
	offsetTrackingEnabled = true;
	pupilDiaTrackingEnabled = true;
	outputDir = _T(".\\");

}

CPupilTrackerMainFrame::~CPupilTrackerMainFrame()
{

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
	Sink = FrameHandlerSink::create(eRGB8, 3);

	// disable snap mode (formerly tFrameGrabberMode::eSNAP).
	Sink->setSnapMode(false);

	// Apply the sink to the grabber.
	Grabber.setSinkType(Sink);
}



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
		CFrameWndEx::OnPaint();
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

	m_wndView.RedrawWindow();
	
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
	OnStop();
	if (m_pSock_AOMCONTROL != NULL)
		delete m_pSock_AOMCONTROL;
	CFrameWndEx::OnClose();

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

	m_wndView.wndVideo.ShowWindow(TRUE);

	// Stop displaying the captured image in the view window's OnPaint event, 
	// as we will have live video in that window.
	
	if (!Grabber.startLive())
	{
		AfxMessageBox(CString(Grabber.getLastError().toString().c_str()));
	}

	else
		pupilTracking.init(Grabber.getAcqSizeMaxX(), Grabber.getAcqSizeMaxY());

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
	
	if (Grabber.isDevValid() && Grabber.isLive())
	{
		m_wndView.showBuffer(Sink->getLastAcqMemBuffer());
		
		if (!Grabber.stopLive())
		{
			if (Grabber.getLastError().isError())
			{
				AfxMessageBox(Grabber.getLastError().toStringW().c_str());
			}

		}
	
	}

}



void CPupilTrackerMainFrame::drawOverlay(DShowLib::tPathPosition pos)
{

	smart_ptr<OverlayBitmap> ob = Grabber.getOverlay(pos);
	ob->setEnable(true);

}


void CPupilTrackerMainFrame::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CFrameWndEx::OnShowWindow(bShow, nStatus);
	// TODO: Add your message handler code here

	reAdjustView();
}

void CPupilTrackerMainFrame::reAdjustView(){

	//Grabber.stopLive();
	CRect m_tbRect, m_vidRect, m_offsetRect, m_graphRect, m_stRect;
	CSize tbSize;

	Grabber.getWindowPosition(m_vidRect.left, m_vidRect.top, m_vidRect.right, m_vidRect.bottom);
	if (m_wndToolBar.IsVisible()) 
		tbSize = m_wndToolBar.CalcSize(false);
		//m_wndToolBar.GetClientRect(&m_tbRect);
	if (m_wndStatusBar.IsVisible()) {
		m_wndStatusBar.GetClientRect(&m_stRect);
	}

	if (offsetTrackingEnabled) {
		m_offsetRect.right = 400;
		m_offsetRect.bottom = m_vidRect.Height();
	}

	if (pupilDiaTrackingEnabled) {
		m_graphRect.right = m_vidRect.Width() + m_offsetRect.Width();
		m_graphRect.bottom = m_vidRect.Height() / 2;
	}

	CRect cRect{ 0, 0, m_vidRect.right + m_offsetRect.Width(), m_vidRect.Height() + m_graphRect.Height() + 40 + tbSize.cy };
	CRect wRect;
	GetWindowRect(&wRect);
	AdjustWindowRectEx(&cRect, this->GetStyle(), TRUE, this->GetExStyle());

	MoveWindow(wRect.left, wRect.top, cRect.Width(), cRect.Height(), true);

	//Resize the video window to 640*480 pixels.
	m_wndView.wndVideo.SetWindowPos(NULL,0, tbSize.cy + 3 ,640,480, SWP_NOZORDER);

	//m_cListener.SetDrawWindow(Grabber.getAcqSizeMaxX(), Grabber.getAcqSizeMaxY());

	// Assign this window to the Grabber object for live video display.
	Grabber.setHWND(m_wndView.wndVideo.GetSafeHwnd());

	//// Adjust the live video to the size of the window.
	Grabber.setDefaultWindowPosition(false);
	Grabber.setWindowSize(m_vidRect.Width(), m_vidRect.Height());

	m_wndView.wndOffset.MoveWindow(m_vidRect.Width(), m_tbRect.Height() + tbSize.cy + 3, m_offsetRect.right, m_offsetRect.Height(), false);
	m_wndView.wndPupilDia.MoveWindow(0, m_vidRect.Height() + tbSize.cy + 3, m_graphRect.Width(), m_graphRect.Height() + 40, false);

	
}

int CPupilTrackerMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Add listener(s) to grabber

	
	Grabber.addListener(&pupilTracking, CListener::eOVERLAYCALLBACK | CListener::eFRAMEREADY);
	pupilTracking.SetParent(this);

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

	DWORD dwCtrlStyle = WS_VISIBLE | WS_CHILD | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY;
	DWORD dwStyle = AFX_DEFAULT_TOOLBAR_STYLE;


	if (!m_wndToolBar.Create(this, dwCtrlStyle, dwStyle) || !m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // failed to create
	}

	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndToolBar);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create Status Bar!\n");
		return -1;      // Fehler bei Erstellung
	}
	
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));
	
	// Attach View to active tracking Method

	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
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

	// try to remote connect to AOMControl in intervals
	//OnTimer(NULL);
	SetTimer(0, 2000, NULL);

	reAdjustView();
	updateWindowTitle();
	OnPlay();
		
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

	if (pMsg->message == WM_KEYDOWN) {

		switch (pMsg->wParam) {
		case VK_SPACE:

			if (pupilTracking.record && pupilTracking.freeze) {
				pupilTracking.record = false;
				OnStop();
				Save();
				pupilTracking.pupil.reset();
				OnPlay();

			}

			if (Grabber.isDevValid() && Grabber.isLive()) {
				pupilTracking.freezePupil();
			}

			break;

		}

		return true;

	}
	
	return CFrameWndEx::PreTranslateMessage(pMsg);

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
		pupilTracking.pupil.reset();
		OnPlay();

	}

	else

		pupilTracking.record = true;
		pupilTracking.recIndex = pupilTracking.pupil.center.size();

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

//	if(m_wndToolBar.IsLastCommandFromButton()) 
	{

	}

}

void CPupilTrackerMainFrame::getSysTime(CString &buf) {

	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, 80, "%Y_%m_%d_%H_%M_%S", timeinfo);
	buf = buffer;

}

void CPupilTrackerMainFrame::Save() {

	CString name, version, filepath;
	CPupilTrackerApp* pApp = (CPupilTrackerApp*)AfxGetApp();
	pApp->GetProductAndVersion(name, version);
	wstringstream sstream;

	CString hours,
			minutes,
			seconds,
			milliseconds,
			pupildiameter,
			pupilCenter_x,
			pupilCenter_y,
			purkinje_center_x,
			purkinje_center_y,
			pupil_offsetMM_x,
			pupil_offsetMM_y;

	CString header[7];

	header[0].Format(_T("**************************************************************************************\n"));
	header[1].Format(_T("********** %ls %ls Offset Tracker Data							**********\n"), name.GetString(), version.GetString());
	header[2].Format(_T("**************************************************************************************\n\n"));

	header[3].Format(_T("\nNOTE: offset data is in (sub)pixels\n"));
	header[4].Format(_T("\nMagnifiction [mm/px]: %2.15f"), MM_PER_PIXEL);
	header[5].Format(_T("\nLocked position [px]: x:%2.1f y:%2.1f"), pupilTracking.pupil.frozen_center.x, pupilTracking.pupil.frozen_center.y);
	header[6].Format(_T("\n\ndata is: time [hour:min:sec:msec] frame number, pupil size [mm], pupil center [pxx], pupil center [pxy], purkinje center [pxx], purkinje center [pxy], center offset horizontal [mm], center offset vertical [mm]\n\n"));

	for (int i = 0; i < 7; i++)
		sstream << header[i].GetString();

		for (size_t i = pupilTracking.recIndex; i < pupilTracking.pupil.center.size(); i++)
		{

			hours.Format(_T("\t%02d:"), pupilTracking.timestamps[i].wHour);
			minutes.Format(_T("%02d:"), pupilTracking.timestamps[i].wMinute);
			seconds.Format(_T("%02d:"), pupilTracking.timestamps[i].wSecond);
			milliseconds.Format(_T("%04d"), pupilTracking.timestamps[i].wMilliseconds);
			pupildiameter.Format(_T("\t%f"), pupilTracking.pupil.diameter[i]);
			pupilCenter_x.Format(_T("\t%2.2f"), pupilTracking.pupil.center[i].x);
			pupilCenter_y.Format(_T("\t%2.1f"), pupilTracking.pupil.center[i].y);
			purkinje_center_x.Format(_T("\t%2.1f"), pupilTracking.purkinje.center[i].x);
			purkinje_center_y.Format(_T("\t%2.1f"), pupilTracking.purkinje.center[i].y);
			pupil_offsetMM_x.Format(_T("\t%2.5f"), pupilTracking.pupil.offsetMM[i].x);
			pupil_offsetMM_y.Format(_T("\t%2.5f\n"), pupilTracking.pupil.offsetMM[i].y);

			sstream
				<< i
				<< hours.GetString()
				<< minutes.GetString()
				<< seconds.GetString()
				<< milliseconds.GetString()
				<< pupildiameter.GetString()
				<< pupilCenter_x.GetString()
				<< pupilCenter_y.GetString()
				<< purkinje_center_x.GetString()
				<< purkinje_center_y.GetString()
				<< pupil_offsetMM_x.GetString()
				<< pupil_offsetMM_y.GetString();

		}

		try {

			if (outputDir == ".\\")
				OnPickFolder();

			fileName = "output.txt";
		
			getSysTime(timestamp);
			fileName = timestamp + "_" + fileName;

			CStdioFile outputFile(outputDir + fileName, CFile::modeCreate | CFile::modeWrite | CFile::typeText);
			outputFile.WriteString(sstream.str().c_str());
			outputFile.Close();
			AfxMessageBox(_T("Data saved to:\n") + fileName, MB_OK | MB_ICONINFORMATION);

		}

		catch (CFileException* pe)

		{

			TCHAR szCause[255];
			CString strFormatted;
			pe->GetErrorMessage(szCause, 255);

			strFormatted = _T("The data file could not be saved because of this error: \n");
			strFormatted += szCause;
			AfxMessageBox(strFormatted);

		}

}


//
//void CPupilTrackerMainFrame::Save()
//{
//	CString name, version;
//	CPupilTrackerApp* pApp = (CPupilTrackerApp*)AfxGetApp();
//	pApp->GetProductAndVersion(name, version);
//
//	CString szFileName;
//	FILE *pFile = NULL;
//	CFileDialog cFileDlg(
//		FALSE,
//		_T("output"),
//		_T("*.txt"),
//		OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_OVERWRITEPROMPT | OFN_CREATEPROMPT,
//		_T("All Files (*.*)|*.*|"),
//		FALSE
//	);
//
//	cFileDlg.m_ofn.lpstrInitialDir = _T(".\\");
//	if (cFileDlg.DoModal() == IDOK)
//	{
//		szFileName = cFileDlg.GetPathName();
//		_wfopen_s(&pFile, szFileName, L"w");
//
//		if (pFile)
//		{
//			// this saves the data to the file
//
//			fprintf(pFile, "**************************************************************************************\n");
//			fprintf(pFile, "********** %ls %ls Offset Tracker Data							**********\n", name.GetString(), version.GetString());
//			fprintf(pFile, "**************************************************************************************\n\n");
//
//			fprintf(pFile, "\nNOTE: offset data is in (sub)pixels\n");
//			fprintf(pFile, "\nMagnification [mm/px]: %2.15f", MM_PER_PIXEL);
//			fprintf(pFile, "\nLocked position [px]: x:%2.1f y:%2.1f", pupilTracking.pupil.frozen_center.x, pupilTracking.pupil.frozen_center.y);
//
//			fprintf(pFile, "\n\ndata is: time [hour:min:sec:msec], frame number, pupil size [mm], pupil center [pxx], pupil center [pxy], purkinje center [pxx], purkinje center [pxy], center offset horizontal [mm], center offset vertical [mm] \n\n");
//
//			for (size_t i = pupilTracking.recIndex; i < pupilTracking.pupil.center.size(); i++) {
//				fprintf(pFile, "%02d:%02d:%02d:%04d\t%d\t%2.2f\t%2.1f\t%2.1f\t%2.1f\t%2.1f\t%2.5f\t%2.5f\n",
//					pupilTracking.timestamps[i].wHour,
//					pupilTracking.timestamps[i].wMinute, 
//					pupilTracking.timestamps[i].wSecond,
//					pupilTracking.timestamps[i].wMilliseconds,
//					i,
//					pupilTracking.pupil.diameter[i],
//					pupilTracking.pupil.center[i].x,
//					pupilTracking.pupil.center[i].y,
//					pupilTracking.purkinje.center[i].x,
//					pupilTracking.purkinje.center[i].y,
//					pupilTracking.pupil.offsetMM[i].x,
//					pupilTracking.pupil.offsetMM[i].y);
//			}
//
//			fclose(pFile);
//
//			MessageBox(szFileName, _T("Data saved!"), MB_ICONINFORMATION | MB_OK | MB_DEFBUTTON1);
//
//	}
//
//		else
//		{
//			MessageBox(_T("Failed to open file!"), _T("Error"), MB_ICONHAND | MB_OK | MB_DEFBUTTON1);
//		}
//		
//	}
//
//}

void CPupilTrackerMainFrame::OnTimer(UINT_PTR nIDEvent)
{

	// TODO: Add your message handler code here and/or call default
	// try to connect to ICANDI 
	if (m_pSock_AOMCONTROL == NULL) {
		m_pSock_AOMCONTROL = new CSockClient();
		CSockClient::SocketInit();

		if (!m_pSock_AOMCONTROL->Create())
		{
			TCHAR szErrorMsg[WSA_ERROR_LEN];
			CSockClient::WSAGetLastErrorMessage(szErrorMsg);

			CString error;
			error.Format(L"Socket Create/Connect failed Error:\n%s", szErrorMsg);
			AfxMessageBox(error, MB_OK | MB_ICONERROR, 0);
			//TRACE(_T("Socket Create/Connect failed Error: %s"), szErrorMsg);

		}

		if (m_pSock_AOMCONTROL && !m_pSock_AOMCONTROL->IsConnected())
			m_pSock_AOMCONTROL->Connect(L"127.0.0.1", 8015);

	}



	if (m_pSock_AOMCONTROL->shutdown)
		delete m_pSock_AOMCONTROL;

	//KillTimer(1);
	//if (Grabber.loadDeviceStateFromFile("device.xml"))
	//{
	//	// Display the device's name in the caption bar of the application.
	//	updateWindowTitle();
	//	Sleep(2000);
	//	Grabber.startLive();
	//	//smart_com<IKsPropertySet> pksps = 0;
	//	//m_cGrabber.getDev().getInternalInterface( pksps );
	//	//set_blacklevel_manual(pksps.get());
	//}
	//else
	//{
	//	// Display the device's name in the caption bar of the application.
	//	SetWindowText(L"No Device found! Is the camera plugged in?");
	//	SetTimer(1, 500, NULL);
	//}

	CFrameWndEx::OnTimer(nIDEvent);
}


afx_msg LRESULT CPupilTrackerMainFrame::OnMessageOffsetmmProcessed(WPARAM wParam, LPARAM lParam)
{

	if (wParam != 0) {

		offsetMM = reinterpret_cast<coords<double, double>*>(wParam);
		m_wndView.wndOffset.AddPoint(*offsetMM);
		m_wndView.wndOffset.DrawPoint();
		//m_wndView.wndOffset.DrawValues();
	}

	//else {
	//	coords <double, double> c{ 0.0, 0.0 };
	//	m_wndView.wndOffset.AddPoint(c);
	//	m_wndView.wndOffset.DrawPoint();
	//	m_wndView.wndOffset.DrawValues();
	//}

	m_wndView.wndOffset.InvalidateCtrl();

	return 0;
	
}

afx_msg LRESULT CPupilTrackerMainFrame::OnMessageOffsetLockedPos(WPARAM wParam, LPARAM lParam) {

	if (wParam != 0) {

		coords<double, double> *lockedPos;
		lockedPos = reinterpret_cast<coords<double, double>*>(wParam);
		m_wndView.wndOffset.setLockedPos(*lockedPos);
	}

	return 0;

}


afx_msg LRESULT CPupilTrackerMainFrame::OnMessagePupilDiaProcessed(WPARAM wParam, LPARAM lParam)
{

	if (wParam != 0) {
		double pupilDia = wParam;
		m_wndView.wndPupilDia.AppendPoint(pupilDia);
		m_wndView.wndPupilDia.DrawPoints();
	}
	else {
		m_wndView.wndPupilDia.AppendPoint(0);
		m_wndView.wndPupilDia.DrawPoints();
	}

	return 0;

}

LRESULT CPupilTrackerMainFrame::OnDeviceLost(WPARAM wParam, LPARAM lParam)
{
	SetTimer(1, 500, NULL);
	return 0;
}


void CPupilTrackerMainFrame::OnSetFocus(CWnd* pOldWnd)
{
	m_wndView.SetFocus();
	// TODO: Add your message handler code here
}


BOOL CPupilTrackerMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// TODO: Add your specialized code here and/or call the base class
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return true;
	return CFrameWndEx::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


void CPupilTrackerMainFrame::OnPickFolder()
{
	// TODO: Add your command handler code here
	BROWSEINFO bi = { 0 };
	bi.lpszTitle = _T("Choose Output Directory!");
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	if (pidl != 0)
	{
		TCHAR path[MAX_PATH];
		if (SHGetPathFromIDList(pidl, path))
		{
			CString message = path + fileName;
		}

		IMalloc * imalloc = 0;
		if (SUCCEEDED(SHGetMalloc(&imalloc)))
		{
			imalloc->Free(pidl);
			imalloc->Release();
		}

		outputDir = path;

	}

}
