// Gaze.cpp : implementation file
#include "stdafx.h"
#include "OffsetTracker.h"
#include "resource.h"

// Gaze


OffsetTracker::OffsetTracker()
{

	hPenGreen = CreatePen(PS_SOLID, 1, RGB(0, 60, 0));				// dark green pen
	hPenGrey = CreatePen(PS_SOLID, 1, RGB(156, 152, 196));			// earth pen
	hPenWht = CreatePen(PS_SOLID, 1, RGB(150, 150, 150));			// white pen

	magnif = (double) MM_PER_PIXEL;
	conv = 1 / MM_PER_PIXEL * 0.25 * 150;	// mapping pixels on plot with .25mm limits

	bkgrndCol = RGB(0, 0, 0);
	m_brushBack.CreateSolidBrush(bkgrndCol);

	// create trail palette
	for (int i = 0; i < 39; i++) {
		hPenPal[i].CreatePen(PS_SOLID, 1, RGB(0, 60 + i * 4.875, 0));
	}

	// color the head of the trail in some yellow
	hPenPal[39].CreatePen(PS_SOLID, 1, RGB(255, 255, 100));
	
	VERIFY(consBig.CreateFont(
		22,							// nHeight
		0,							// nWidth
		0,							// nEscapement
		0,							// nOrientation
		FW_THIN,					// nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("Consolas")));           // lpszFacename

	VERIFY(headFont.CreateFont(
		24,							// nHeight
		0,							// nWidth
		0,							// nEscapement
		0,							// nOrientation
		FW_THIN,					// nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		VARIABLE_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("Arial Narrow")));             // lpszFacename

	VERIFY(consSmall.CreateFont(
		16,							// nHeight
		0,							// nWidth
		0,							// nEscapement
		0,							// nOrientation
		FW_THIN,					// nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("Consolas")));           // lpszFacename

	// protected bitmaps to restore the memory DC's
	m_pbitmapOldGrid = NULL;
	m_pbitmapOldPlot = NULL;

}

OffsetTracker::~OffsetTracker()
{

	// just to be picky restore the bitmaps for the two memory dc's
	// (these dc's are being destroyed so there shouldn't be any leaks)
	//if (m_pbitmapOldGrid != NULL)
	//	m_dcGrid.SelectObject(m_pbitmapOldGrid);
	//if (m_pbitmapOldPlot != NULL)
	//	m_dcPlot.SelectObject(m_pbitmapOldPlot);

}


BEGIN_MESSAGE_MAP(OffsetTracker, CStatic)
ON_WM_PAINT()
ON_WM_SIZE()
END_MESSAGE_MAP()


void OffsetTracker::InvalidateCtrl() {

	CClientDC* dc = new CClientDC(this);

	if (m_dcGrid.GetSafeHdc() == NULL)
	{
		m_dcGrid.CreateCompatibleDC(dc);
		m_bitmapGrid.LoadBitmapW(IDB_BITMAP1);
		dc->SelectObject(m_bitmapGrid);
		m_dcGrid.BitBlt(0, 0, m_nClientWidth, m_nClientHeight, dc, 0, 0, SRCCOPY);
		m_pbitmapOldGrid = m_dcGrid.SelectObject(&m_bitmapGrid);
	}

	//m_dcGrid.SetBkColor(RGB(0, 0, 0));
	//m_dcGrid.SetBkMode(OPAQUE);

	//m_dcGrid.FillRect(m_rectClient, &m_brushBack);

	CString szText;

	//SelectObject(*dc, GetStockObject(NULL_BRUSH));
	//SelectObject(*dc, hPenClay);
	//MoveToEx(*dc, 0, 0, NULL);
	//LineTo(*dc, 0, m_rectClient.bottom);

	// plot trail
	//if (ready) {

	//		Ellipse(*dc, rect.Width() / 2 - (int)(gazeBuffer.front().x * conv) - 4,
	//			rect.Height() / 2 + (int)(gazeBuffer.front().y * conv) - 4,
	//			rect.Width() / 2 - (int)(gazeBuffer.front().x * conv) + 4,
	//			rect.Height() / 2 + (int)(gazeBuffer.front().y * conv) + 4);
	//
	//}


	// x/y plot

	//m_dcGrid.SelectObject(&headFont);
	//SetTextColor(m_dcGrid, RGB(254, 152, 0));
	//szText.Format(TEXT("PUPIL OFFSET"));
	//m_dcGrid.TextOutW(m_rectClient.Width() / 2 - 60, 10, szText);


	//dc->SelectObject(&consBig);

	//szText.Format(TEXT("x:"));
	//dc->TextOutW(m_rectClient.Width() / 2 - 80, m_rectClient.Height() / 11, szText);
	//szText.Format(TEXT("y:"));
	//dc->TextOutW(m_rectClient.Width() / 2 + 30, m_rectClient.Height() / 11, szText);

	//dc->SelectObject(consSmall);



	//szText.Format(TEXT("Current Pos [px]"));
	//dc->TextOutW(15, m_rectClient.Height() - 40, szText);
	//szText.Format(TEXT("x:"));
	//dc->TextOutW(15, m_rectClient.Height() - 20, szText);
	//szText.Format(TEXT("y:"));
	//dc->TextOutW(70, m_rectClient.Height() - 20, szText);

	//szText.Format(TEXT("Locked Pos [px]"));
	//dc->TextOutW(m_rectClient.Width() / 2 + 60, m_rectClient.Height() - 40, szText);
	//szText.Format(TEXT("x:"));
	//dc->TextOutW(m_rectClient.Width() / 2 + 60, m_rectClient.Height() - 20, szText);
	//szText.Format(TEXT("y:"));
	//dc->TextOutW(m_rectClient.Width() / 2 + 115, m_rectClient.Height() - 20, szText);


	//



		// at this point we are done filling the the grid bitmap, 
		// no more drawing to this bitmap is needed until the setting are changed

		// if we don't have one yet, set up a memory dc for the plot
	if (m_dcPlot.GetSafeHdc() == NULL)
	{
		m_dcPlot.CreateCompatibleDC(dc);
		m_bitmapPlot.CreateCompatibleBitmap(dc, m_nClientWidth, m_nClientHeight);
		m_pbitmapOldPlot = m_dcPlot.SelectObject(&m_bitmapPlot);
	}


//	m_dcPlot.SetBkColor(bkgrndCol);
//	m_dcPlot.FillRect(m_rectClient, &m_brushBack);

	// finally, force the plot area to redraw
	InvalidateRect(m_rectClient);
	
	delete dc;

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
	oldBitmap = (CBitmap *)memDC.SelectObject(&memBitmap);

	if (memDC.GetSafeHdc() != NULL)
	{
		// first drop the grid on the memory dc
		memDC.BitBlt(0, 0, m_nClientWidth, m_nClientHeight,
			&m_dcGrid, 0, 0, SRCCOPY);

		// now add the plot on top as a "pattern" via SRCPAINT.
		// works well with dark background and a light plot
		memDC.BitBlt(0, 0, m_nClientWidth, m_nClientHeight,
			&m_dcPlot, 0, 0, SRCPAINT);  //SRCPAINT
										 // finally send the result to the display

		dc.BitBlt(0, 0, m_nClientWidth, m_nClientHeight,
			&memDC, 0, 0, SRCCOPY);
	}

	memDC.SelectObject(oldBitmap);

}


void OffsetTracker::AddPoint(coords<double, double> dNewPoint)
{
	// store current position
	dCurrentPosition = dNewPoint;


		if (trail.size() < 40) {
			trail.push_front(dNewPoint);
		}

		else {
			m_dCurrentPosition = trail.back();
			trail.pop_back();
		}


}

void OffsetTracker::setLockedPos(coords<double, double>lockedPos) {

	dLockedPosition = lockedPos;

}

void OffsetTracker::DrawPoint() {

	if (m_dcPlot.GetSafeHdc() != NULL) {
		
		// make sure the plot bitmap is cleared
		//if (!infinity)
		//	m_dcPlot.FillRect(m_rectClient, &m_brushBack);

		// draw offset into x/y plot
		int y = 40;

		for (auto it = trail.begin(); it != trail.end(); it++) {

			SelectObject(m_dcPlot, hPenPal[y]);
			SelectObject(m_dcPlot, GetStockObject(HOLLOW_BRUSH));

			Ellipse(m_dcPlot, m_nClientWidth / 2 - (int)(it->x * conv) - 4,
				m_nClientHeight / 2 + (int)(it->y * conv) - 4,
				m_nClientWidth / 2 - (int)(it->x * conv) + 4,
				m_nClientHeight / 2 + (int)(it->y * conv) + 4);
			y--;
		
		}

	}

}

void OffsetTracker::DrawValues() {

	CString szText;

	coords<double, double> offset(dCurrentPosition - dLockedPosition);

	if (m_dcPlot.GetSafeHdc() != NULL)
	{

		m_dcPlot.SelectObject(&consBig);

		// offset
		szText.Format(TEXT("%.2f"), -1 * offset.x);
		m_dcPlot.TextOutW(m_rectClient.Width() / 2 - 60, m_rectClient.Height() / 11, szText);
		szText.Format(TEXT("%.2f"), -1 * offset.y);
		m_dcPlot.TextOutW(m_rectClient.Width() / 2 + 50, m_rectClient.Height() / 11, szText);

		m_dcPlot.SelectObject(&consSmall);

		// current position
		SetTextColor(m_dcPlot, RGB(204, 96, 97));
		szText.Format(TEXT("%.1f"), dCurrentPosition.x);
		m_dcPlot.TextOutW(30, m_rectClient.Height() - 20, szText);
		szText.Format(TEXT("%.1f"), dCurrentPosition.y);

		m_dcPlot.TextOutW(85, m_rectClient.Height() - 20, szText);

		// locked position
		SetTextColor(m_dcPlot, RGB(155, 152, 254));
		szText.Format(TEXT("%.1f"), dLockedPosition.x);
		m_dcPlot.TextOutW(m_rectClient.Width() / 2 + 75, m_rectClient.Height() - 20, szText);
		szText.Format(TEXT("%.1f"), dLockedPosition.y);
		
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