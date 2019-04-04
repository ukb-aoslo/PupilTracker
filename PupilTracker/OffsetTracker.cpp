#include "stdafx.h"
#include "OffsetTracker.h"
#include "resource.h"
#include "ChildView.h"

// Paint Jobs for Offset Tracking

OffsetTracker::OffsetTracker()
{

	magnif = (double) MM_PER_PIXEL;
	conv = 150/0.25 * magnif;	// mapping pixels on plot with .25mm range

	
	m_brushBack.CreateSolidBrush(black);

	// create trail palette
	for (int i = 0; i < 39; i++) {
		hPenPal[i].CreatePen(PS_SOLID, 1, RGB(0, 60 + i * 4.875, 0));
	}

	// color the head of the trail in some yellow
	hPenPal[39].CreatePen(PS_SOLID, 1, darkyellow);

	// protected bitmaps to restore the memory DC's
	m_pbitmapOldGrid = NULL;
	m_pbitmapOldPlot = NULL;

}

OffsetTracker::~OffsetTracker()
{

	// just to be picky restore the bitmaps for the two memory dc's
	// (these dc's are being destroyed so there shouldn't be any leaks)
	if (m_pbitmapOldGrid != NULL)
		m_dcGrid.SelectObject(m_pbitmapOldGrid);
	if (m_pbitmapOldPlot != NULL)
		m_dcPlot.SelectObject(m_pbitmapOldPlot);

}


BEGIN_MESSAGE_MAP(OffsetTracker, CStatic)
ON_WM_PAINT()
ON_WM_SIZE()
END_MESSAGE_MAP()


void OffsetTracker::InvalidateCtrl() {

	CClientDC dc(this);

	if (m_dcGrid.GetSafeHdc() == NULL)
	{
		m_dcGrid.CreateCompatibleDC(&dc);
		m_bitmapGrid.LoadBitmapW(IDB_BITMAP1);
		dc.SelectObject(m_bitmapGrid);
		m_dcGrid.BitBlt(0, 0, m_nClientWidth, m_nClientHeight, &dc, 0, 0, SRCCOPY);
		m_pbitmapOldGrid = m_dcGrid.SelectObject(&m_bitmapGrid);
	}


	// if we don't have one yet, set up a memory dc for the plot

	if (m_dcPlot.GetSafeHdc() == NULL)
	{
		m_dcPlot.CreateCompatibleDC(&dc);
		m_bitmapPlot.CreateCompatibleBitmap(&dc, m_nClientWidth, m_nClientHeight);
		m_pbitmapOldPlot = m_dcPlot.SelectObject(&m_bitmapPlot);
	}

}

void OffsetTracker::OnPaint()
{
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages

	////////////////////////////////////////////////////////////////////////////

	CPaintDC dc(this);  // device context for painting
	CDC memDC;
	CBitmap memBitmap;
	CBitmap* oldBitmap; // bitmap originally found in CMemDC

						// no real plotting work is performed here, 
						// just putting the existing bitmaps on the client

						// to avoid flicker, establish a memory dc, draw to it 
						// and then BitBlt it to the client

	memDC.CreateCompatibleDC(&dc);
	memBitmap.CreateCompatibleBitmap(&dc, m_nClientWidth, m_nClientHeight);
	oldBitmap = (CBitmap*)memDC.SelectObject(&memBitmap);

	if (memDC.GetSafeHdc() != NULL)
	{
		// first drop the grid on the memory dc
		memDC.BitBlt(0, 0, m_nClientWidth, m_nClientHeight,	&m_dcGrid, 0, 0, SRCCOPY);

		// now add the plot on top as a "pattern" via SRCPAINT.
		// works well with dark background and a light plot
		memDC.BitBlt(0, 0, m_nClientWidth, m_nClientHeight,	&m_dcPlot, 0, 0, SRCPAINT);  //SRCPAINT
		
		// finally send the result to the display
		dc.BitBlt(0, 0, m_nClientWidth, m_nClientHeight, &memDC, 0, 0, SRCCOPY);

	}

	memDC.SelectObject(oldBitmap);

}

void OffsetTracker::DrawTitle() {

	m_dcGrid.SetBkColor(RGB(0, 0, 0));
	m_dcGrid.SetBkMode(TRANSPARENT);

	CString szText;

	// x/y plot

	SetTextColor(m_dcGrid, darkyellow);
	szText.Format(TEXT("PUPIL OFFSET"));
	m_dcGrid.TextOutW(m_rectClient.Width() / 2 - 40, 10, szText);

	szText.Format(TEXT("x:"));
	m_dcGrid.TextOutW(m_rectClient.Width() / 2 - 80, m_rectClient.Height() / 11, szText);
	szText.Format(TEXT("y:"));
	m_dcGrid.TextOutW(m_rectClient.Width() / 2 + 30, m_rectClient.Height() / 11, szText);

	szText.Format(TEXT("Current Pos [px]"));
	m_dcGrid.TextOutW(15, m_rectClient.Height() - 40, szText);
	szText.Format(TEXT("x:"));
	m_dcGrid.TextOutW(15, m_rectClient.Height() - 20, szText);
	szText.Format(TEXT("y:"));
	m_dcGrid.TextOutW(70, m_rectClient.Height() - 20, szText);

	szText.Format(TEXT("Locked Pos [px]"));
	m_dcGrid.TextOutW(m_rectClient.Width() / 2 + 60, m_rectClient.Height() - 40, szText);
	szText.Format(TEXT("x:"));
	m_dcGrid.TextOutW(m_rectClient.Width() / 2 + 60, m_rectClient.Height() - 20, szText);
	szText.Format(TEXT("y:"));
	m_dcGrid.TextOutW(m_rectClient.Width() / 2 + 115, m_rectClient.Height() - 20, szText);

	// at this point we are done filling the the grid bitmap, 
	// no more drawing to this bitmap is needed until the setting are changed

}


void OffsetTracker::AddPositions(coords<double, double> current, coords<double, double> locked)
{
	// store current offset position in a queue

	m_dCurrentPosition = current;
	m_dLockedPosition = locked;

	if (locked.x != 0 && locked.y != 0)
		m_dOffset = current - locked;
	else
		m_dOffset.x = m_dOffset.y = 0;

		if (trail.size() < 40) {
			trail.push_front(m_dOffset);
		}

		else {
			trail.pop_back();
			trail.push_front(m_dOffset);
		}


	Invalidate();

}

void OffsetTracker::DrawOffset() {

	if (m_dcPlot.GetSafeHdc() != NULL) {
		
		// make sure the plot bitmap is cleared
		//if (!infinity)
		//m_dcPlot.FillRect(m_rectClient, &m_brushBack);

		// draw offset into x/y plot
		SelectObject(m_dcPlot, GetStockObject(HOLLOW_BRUSH));
		int y = 39;

		for (auto it = trail.begin(); it != trail.end(); it++) {

			SelectObject(m_dcPlot, hPenPal[y]);

			Ellipse(m_dcPlot, m_nClientWidth / 2 - (int)(it->x * conv) - 3,
				m_nClientHeight / 2 + (int)(it->y * conv) - 3,
				m_nClientWidth / 2 - (int)(it->x * conv) + 3,
				m_nClientHeight / 2 + (int)(it->y * conv) + 3);
			y--;
		
		}

	}

}

void OffsetTracker::DrawValues() {

	CString szText;
	
	m_dcPlot.SetBkColor(black);

	if (m_dcPlot.GetSafeHdc() != NULL)
	{
		 
		SetTextColor(m_dcPlot, darkblue);

		// offset
		szText.Format(L"%.2f", -1 * m_dOffset.x);
		m_dcPlot.TextOutW(m_rectClient.Width() / 2 - 60, m_rectClient.Height() / 11, szText);
		szText.Format(L"%.2f", -1 * m_dOffset.y);
		m_dcPlot.TextOutW(m_rectClient.Width() / 2 + 50, m_rectClient.Height() / 11, szText);

		// current position
		SetTextColor(m_dcPlot, lightblue);
		szText.Format(TEXT("%.1f"), m_dCurrentPosition.x);
		m_dcPlot.TextOutW(30, m_rectClient.Height() - 20, szText);
		szText.Format(TEXT("%.1f"), m_dCurrentPosition.y);

		m_dcPlot.TextOutW(85, m_rectClient.Height() - 20, szText);

		// locked position
		SetTextColor(m_dcPlot, lightgreen);
		szText.Format(TEXT("%.1f"), m_dLockedPosition.x);
		m_dcPlot.TextOutW(m_rectClient.Width() / 2 + 75, m_rectClient.Height() - 20, szText);
		szText.Format(TEXT("%.1f"), m_dLockedPosition.y);
		
		m_dcPlot.TextOutW(m_rectClient.Width() / 2 + 130, m_rectClient.Height() - 20, szText);

	}

}



void OffsetTracker::OnSize(UINT nType, int cx, int cy)
{

	CWnd::OnSize(nType, cx, cy);

	// NOTE: OnSize automatically gets called during the setup of the control

	GetClientRect(m_rectClient);

	// set some member variables to avoid multiple function calls
	m_nClientHeight = m_rectClient.Height();
	m_nClientWidth = m_rectClient.Width();

	// the "left" coordinate and "width" will be modified in 
	// InvalidateCtrl to be based on the width of the y axis scaling
	m_rectPlot.left = 20;
	m_rectPlot.top = 10;
	m_rectPlot.right = m_rectClient.right - 10;
	m_rectPlot.bottom = m_rectClient.bottom - 25;

	// set some member variables to avoid multiple function calls

	if (m_pbitmapOldGrid != NULL)
		m_dcGrid.SelectObject(m_pbitmapOldGrid);
	if (m_pbitmapOldPlot != NULL)
		m_dcPlot.SelectObject(m_pbitmapOldPlot);

	m_dcGrid.DeleteDC();
	m_dcPlot.DeleteDC();
	m_bitmapPlot.DeleteObject();
	m_bitmapGrid.DeleteObject();
	InvalidateCtrl();

}

void OffsetTracker::eraseTrail() {

	if (m_dcPlot.GetSafeHdc() != NULL) {

		// make sure the plot bitmap is cleared
		m_dcPlot.FillRect(m_rectClient, &m_brushBack);
	}

}