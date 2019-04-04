#pragma once
#include "stdafx.h"
#include "ChildView.h"
#include "Parameters.h"
#include <SockClient.h>
#include <SockListener.h>

class CPupilTrackerMainFrame : public CFrameWndEx
{

private:

	// data that is received from listener thread and used for plotting

	coords<double, double>*				current;
	coords<double, double>*				locked;
	double*								pupilDia;

public:

	CPupilTrackerMainFrame();	
	~CPupilTrackerMainFrame();

	DShowLib::Grabber					Grabber;
	smart_ptr<FrameHandlerSink>			Sink;
	smart_ptr<MemBufferCollection>		MemBuf;

	Tracker								pupilTracking;

	CSockClient*						m_pSock_ICANDI;			// and ICANDI
	CSockListener						m_pSock_AOMCONTROL;		// for communication with AOMCONTROL
	
	smart_com<IFrameFilter>				DeNoise;
	Parameters							params;
	
	CString								prefix;
	CString								outputDir;
	CString								fileName;
	CString								timestamp;


	bool								offsetTrackingEnabled,
										pupilDiaTrackingEnabled,
										overlayEnabled;

	void updateWindowTitle();
	void drawOverlay(DShowLib::tPathPosition pos);
	void setSink();
	void reAdjustView();
	void setFilter();
	void initCam();
	void Save();
	void getSysTime(CString &buf);
	
protected:
	
	HICON m_hIcon;

	// Generated message map functions
	afx_msg void OnPaint();
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()

	void OnPlay();
	void OnStop();

	// For Window construction
	CChildView							m_wndView;
	CMFCStatusBar						m_wndStatusBar;
	CMFCToolBar							m_wndToolBar;

public:

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnBnClickedButtondevice();
	afx_msg void OnBnClickedButtonimagesettings();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewParameters();
	afx_msg void OnRecord();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnUpdateRecord(CCmdUI *pCmdUI);
	afx_msg void OnMakeSnapshot();
	afx_msg void OnUpdateMakeSnapshot(CCmdUI *pCmdUI);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnPickFolder();
	afx_msg void OnButtonToggleLayers();
	afx_msg void OnButtonEraseTrail();
	afx_msg void OnToggleBeamoverlay();
	afx_msg void OnUpdatePage(CCmdUI *pCmdUI);
	afx_msg void OnButtonPurkinjeAssist();
	afx_msg void OnResetPupil();

private:
	LRESULT OnDeviceLost(WPARAM Brightness, LPARAM lParam);
	afx_msg LRESULT OnMessageOffsetProcessed(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMessagePupilDiaProcessed(WPARAM wParam, LPARAM lParam);

protected:
	afx_msg LRESULT OnAfxWmResettoolbar(WPARAM wParam, LPARAM lParam);
};