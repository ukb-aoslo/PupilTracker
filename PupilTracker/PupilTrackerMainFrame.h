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
	
	smart_com<IFrameFilter>				DeBayer;
	Parameters							params;
	CString								prefix;

	bool								offsetTrackingEnabled,
										pupilDiaTrackingEnabled;

	void updateWindowTitle();
	void drawOverlay(DShowLib::tPathPosition pos);
	void setSink();
	void reAdjustView();
	void setFilter();
	void initCam();
	void Save();
	
protected:
	
	HICON m_hIcon;
	// Generated message map functions
	afx_msg void OnPaint();
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()

	void OnPlay();
	void OnStop();

	// For Window construction
	CChildView*					m_wndView;
	CStatusBar					m_wndStatusBar;
	CMFCToolBar					m_wndToolBar;

public:

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnBnClickedButtondevice();
	afx_msg void OnBnClickedButtonimagesettings();
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewParameters();
	afx_msg void OnRecord();
	afx_msg void OnUpdateRecord(CCmdUI *pCmdUI);
	afx_msg void OnMakeSnapshot();
	afx_msg void OnUpdateMakeSnapshot(CCmdUI *pCmdUI);

protected:
	afx_msg LRESULT OnAfxWmResettoolbar(WPARAM wParam, LPARAM lParam);

public:
	afx_msg void OnEditFindCombo();
	afx_msg void OnEditFind();
};