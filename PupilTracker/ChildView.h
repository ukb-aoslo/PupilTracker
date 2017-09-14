#pragma once
#include "Method.h"

// ChildView
class CPupilTrackerMainFrame;

class CChildView : public CWnd
{
	DECLARE_DYNAMIC(CChildView)

public:
	CStatic						m_wndVideo;
	Gaze*						m_pWndGaze;
	Graph*						m_pWndGraph;

	CChildView(CPupilTrackerMainFrame* parent);
	virtual ~CChildView();
	void connectWnd(Method*);

protected:

private:
	CPupilTrackerMainFrame*		m_pParent;
	CRect						clientArea;

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);

};