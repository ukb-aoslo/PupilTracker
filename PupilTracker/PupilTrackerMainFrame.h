#pragma once
#include "ChildView.h"
#include "Parameters.h"
#include "FindComboButton.h"

class CPupilTrackerMainFrame : public CFrameWndEx
{

public:

	CPupilTrackerMainFrame();	
	~CPupilTrackerMainFrame();

	DShowLib::Grabber					Grabber;
	smart_ptr<FrameHandlerSink>			Sink;
	smart_ptr<MemBufferCollection>		MemBuf;

	Tracker								pupilTracking;

	CSockClient*						m_pSock_AOMCONTROL;		// for communication with AOMCONTROL
	CSockClient*						m_pSock_ICANDI;			// and ICANDI
	
	smart_com<IFrameFilter>				DeBayer;
	Parameters							params;
	
	CString								prefix;
	CString								outputDir;
	CString								fileName;
	CString								timestamp;

	coords<double, double>*				offsetMM;

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
	afx_msg void OnEditFindCombo();
	afx_msg void OnEditFind();

protected:
	afx_msg LRESULT OnAfxWmResettoolbar(WPARAM wParam, LPARAM lParam);

private:
	LRESULT OnDeviceLost(WPARAM Brightness, LPARAM lParam);
	afx_msg LRESULT OnMessageOffsetmmProcessed(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMessagePupilDiaProcessed(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMessageOffsetLockedPos(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	afx_msg void OnPickFolder();
	afx_msg void OnButtonToggleLayers();
	afx_msg void OnButtonEraseTrail();
};