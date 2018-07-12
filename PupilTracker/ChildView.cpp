// ChildView.cpp : implementation file
//
#include "stdafx.h"
#include "PupilTracker.h"
#include "ChildView.h"
#include "PupilTrackerMainFrame.h"

// ChildView

IMPLEMENT_DYNAMIC(CChildView, CWnd)

CChildView::CChildView(CPupilTrackerMainFrame* parent): m_pParent(parent)
{
}

CChildView::~CChildView()
{
}

BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// ChildView message handlers


BOOL CChildView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class


	if (!wndVideo.Create(_T("Video"), dwStyle, rect , pParentWnd, NULL)) {
		TRACE0("Failed to create view window\n");
		return -1;
	}
	
	if(!wndOffset.Create(_T("Gaze"), dwStyle, rect, pParentWnd, NULL)){
		TRACE0("Failed to create view window\n");
	return -1;
	}

	if (!wndPupilDia.Create(_T("Graph"), dwStyle, rect, pParentWnd, NULL)) {
		TRACE0("Failed to create view window\n");
		return -1;

	}
		
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

}

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!__super::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1), NULL);

	return TRUE;
}


void CChildView::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	CWnd::OnWindowPosChanging(lpwndpos);

	// TODO: Add your message handler code here
	/*vidWidth = m_pParent->Grabber.getAcqSizeMaxX();
	vidHeight = m_pParent->Grabber.getAcqSizeMaxY();*/
	CRect offsetRect, vidRect;

	wndOffset.GetClientRect(&offsetRect);
	wndVideo.GetClientRect(&vidRect);

	wndOffset.MoveWindow(CRect(vidRect.Width(), lpwndpos->y + 2, vidRect.Width() + offsetRect.Width(), vidRect.Height()), TRUE);

}


void CChildView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	RedrawWindow();
	CWnd::OnTimer(nIDEvent);
}

