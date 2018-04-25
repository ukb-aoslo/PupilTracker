#pragma once
#include "afxwin.h"
#include "Gaze.h"
#include "Graph.h"
#include "Schaeffel.h"
#include "ChildView.h"
#include "Method.h"
#include "Parameters.h"
#include "HistoryCombo.h"

class CPupilTrackerMainFrame : public CFrameWndEx, public DShowLib::GrabberListener
{

// Construction
public:
	CPupilTrackerMainFrame();	// standard constructor
	~CPupilTrackerMainFrame();

	DShowLib::Grabber					m_cGrabber;		// The instance of the Grabber class.
	smart_ptr<FrameHandlerSink>			m_pSink;
	smart_ptr<MemBufferCollection>		m_pMemBuf;
	Schaeffel*							m_pListener;
	Gaze*								m_pGaze;
	Graph*								m_pGraph;
	smart_com<IFrameFilter>				m_pDeBayer;
	Parameters*							m_pParams;
	bool								m_cGazeOn, m_cGraphOn;

	void updateWindowTitle();
	void drawOverlay(DShowLib::tPathPosition pos);
	void setSink();
	void reAdjustView();
	void setFilter();
	void initCam();
	
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
	CHistoryCombo				m_comboBox;
		
public:

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void setParams();

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

};