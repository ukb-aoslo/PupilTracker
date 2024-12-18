#pragma once
#include "OffsetTracker.h"
#include "PupilDiaTracker.h"
// ChildView


class CChildView : public CWnd
{
	DECLARE_DYNAMIC(CChildView)

public:

	CStatic						wndVideo;
	OffsetTracker				wndOffset;
	PupilDiaTracker				wndPupilDia;
	CBitmap						noCamera;

	CChildView();
	virtual ~CChildView();

private:

	CRect						clientArea;
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	void showBuffer(const DShowLib::Grabber::tMemBufferPtr& pBuffer);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);

protected:
	
	DShowLib::Grabber::tMemBufferPtr m_pBuffer;

public:

	void drawOffline();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};