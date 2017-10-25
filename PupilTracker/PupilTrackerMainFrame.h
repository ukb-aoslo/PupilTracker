#pragma once
#include "afxwin.h"
#include "Gaze.h"
#include "Graph.h"
#include "ChildView.h"
#include "Method.h"
#include "Parameters.h"

class CPupilTrackerMainFrame : public CFrameWnd, public DShowLib::GrabberListener
{

// Construction
public:
	CPupilTrackerMainFrame();	// standard constructor
	~CPupilTrackerMainFrame();

	DShowLib::Grabber					m_cGrabber;		// The instance of the Grabber class.

	Method								m_cMethod;		// Tracking method will be managed by this object.
	smart_ptr<FrameHandlerSink>			m_pSink;
	CListener*							m_pListener;
	Parameters*							m_pParams;
	bool								m_cGazeOn, m_cGraphOn;

	void updateWindowTitle();
	void drawOverlay(DShowLib::tPathPosition pos);
	void setSink();
	void reAdjustView();
	
protected:
	
	HICON m_hIcon;
	// Generated message map functions
	afx_msg void OnPaint();
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()

	void OnPlay();
	void OnStop();

	// For Window construction
	CChildView*	m_wndView;
	CMenu		m_Menu;
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
		
public:
	afx_msg void OnBnClickedButtondevice();
	afx_msg void OnBnClickedButtonimagesettings();
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnViewParameters();
	void setParams();
	afx_msg void OnRecord();
	afx_msg void OnUpdateRecord(CCmdUI *pCmdUI);
};