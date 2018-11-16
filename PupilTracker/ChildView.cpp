// ChildView.cpp : implementation file
//
#include "stdafx.h"
#include "resource.h"
#include "PupilTracker.h"
#include "ChildView.h"
#include "PupilTrackerMainFrame.h"

// ChildView

IMPLEMENT_DYNAMIC(CChildView, CWnd)

CChildView::CChildView()
{
	m_pBuffer = NULL;


}

CChildView::~CChildView()
{
	m_pBuffer.destroy();
}

BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// ChildView message handlers


BOOL CChildView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class

	dwStyle = WS_CHILD | WS_VISIBLE;

	if (!wndVideo.Create(_T(""),dwStyle, rect, pParentWnd, NULL)) {
		TRACE0("Failed to create view window\n");
		return -1;
	}

	if(!wndOffset.Create(_T("STATIC"), _T("Offset"),dwStyle, rect, pParentWnd, NULL)) {
		TRACE0("Failed to create view window\n");
		return -1;
	}

	if (!wndPupilDia.Create(_T("STATIC"), _T("Diameter"),dwStyle, rect, pParentWnd, NULL)) {
		TRACE0("Failed to create view window\n");
		return -1;
	}


		
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

}

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs)
{

	if (!__super::PreCreateWindow(cs))
		return FALSE;

	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1), NULL);

	return TRUE;

}

void CChildView::showBuffer(const DShowLib::Grabber::tMemBufferPtr& pBuffer)
{

	m_pBuffer = pBuffer;
	wndVideo.ShowWindow(FALSE);
	Invalidate();

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

	
	//wndOffset.MoveWindow(CRect(vidRect.Width(), lpwndpos->y + 2, vidRect.Width() + offsetRect.Width(), vidRect.Height()), TRUE);

}

void CChildView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CWnd::OnPaint() for painting messages


	if (m_pBuffer != 0)	{

		smart_ptr<BITMAPINFOHEADER> pInf = m_pBuffer->getBitmapInfoHeader();
		
		void* pBuf = m_pBuffer->getPtr();

		int nLines = SetDIBitsToDevice(
			dc.GetSafeHdc(),					// Handle to the device
			0,
			0,
			pInf->biWidth,	// Source rectangle width
			pInf->biHeight, // Source rectangle height
			0,				// X-coordinate of lower-left corner of the source rect
			0,				// Y-coordinate of lower-left corner of the source rect
			0,				// First scan line in array
			pInf->biHeight, // Number of scan lines
			m_pBuffer->getPtr(),	// Modified address of array with DIB bits
			reinterpret_cast<LPBITMAPINFO>(&*pInf),	// Address of structure with bitmap info
			DIB_RGB_COLORS	// RGB or palette indices
		);

		if (nLines == GDI_ERROR)
		{
			AfxMessageBox(L"GDI_ERROR");
		}

	}


}

void CChildView::drawOffline() {

	wndVideo.ShowWindow(FALSE);

	CPaintDC hdc(this);
	CDC dc;
	BITMAP bm;
	dc.CreateCompatibleDC(&hdc);

	CRect vidRect;

	noCamera.LoadBitmapW(IDB_BITMAP3);
	wndVideo.GetClientRect(&vidRect);
	dc.SelectObject(&noCamera);

	noCamera.GetObject(sizeof(bm), &bm);

	BitBlt(hdc, vidRect.Width()/2 - bm.bmWidth/2,
		vidRect.Height()/2 - bm.bmHeight/2,
		bm.bmWidth,
		bm.bmHeight,
		dc,
		0,
		0,
		SRCPAINT);

	noCamera.Detach();

}

BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return true;
}
