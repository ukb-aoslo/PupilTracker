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
	//m_pWndGaze = NULL;
	//m_pWndGraph = NULL;
}

BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()

// ChildView message handlers

void CChildView::connectWnd(Method* m)
{
	m_pWndGaze = m->getGaze();
	m_pWndGraph = m->getGraph();
}

BOOL CChildView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class
			
	if (!m_wndVideo.Create(_T("Video"), dwStyle, rect , pParentWnd, 1234)) {
		TRACE0("Failed to create view window\n");
		return -1;
	}
	
	if(!m_pWndGaze->Create(_T("Gaze"), dwStyle, rect, pParentWnd, 1234)){
		TRACE0("Failed to create view window\n");
	return -1;
	}

	if (!m_pWndGraph->Create(_T("Graph"), dwStyle, rect, pParentWnd, 1234)) {
		TRACE0("Failed to create view window\n");
		return -1;
	}

	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

}

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), NULL, NULL);

	return TRUE;
}


void CChildView::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	CWnd::OnWindowPosChanging(lpwndpos);

	// TODO: Add your message handler code here
	/*vidWidth = m_pParent->m_cGrabber.getAcqSizeMaxX();
	vidHeight = m_pParent->m_cGrabber.getAcqSizeMaxY();*/
	CRect gazeRect, vidRect;

	m_pWndGaze->GetClientRect(&gazeRect);
	m_wndVideo.GetClientRect(&vidRect);

	m_pWndGaze->MoveWindow(CRect(vidRect.Width(), lpwndpos->y + 2, vidRect.Width() + gazeRect.Width(), vidRect.Height()), TRUE);
}