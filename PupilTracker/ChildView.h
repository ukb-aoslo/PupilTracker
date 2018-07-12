#pragma once

// ChildView
class CPupilTrackerMainFrame;

class CChildView : public CWnd
{
	DECLARE_DYNAMIC(CChildView)

public:

	CStatic						wndVideo;
	CStatic						wndOffset;
	CStatic						wndPupilDia;

	CChildView(CPupilTrackerMainFrame* parent);
	virtual ~CChildView();

protected:

private:

	CPupilTrackerMainFrame*		m_pParent;
	CRect						clientArea;

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

};