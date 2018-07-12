// Gaze.cpp : implementation file
#include "stdafx.h"
#include "OffsetTracker.h"

// Gaze


OffsetTracker::OffsetTracker()
{

	hPenGreen = CreatePen(PS_SOLID, 1, RGB(0, 60, 0));				// dark green pen
	hPenGrey = CreatePen(PS_SOLID, 1, RGB(156, 152, 196));			// earth pen
	hPenWht = CreatePen(PS_SOLID, 1, RGB(150, 150, 150));			// white pen

	conv = 9.3f;
	magnif = (double)1 / MM_PER_PIXEL;
	
	bkgrndCol = RGB(0, 0, 0);
	brushBack.CreateSolidBrush(bkgrndCol);

	// create trail palette
	for (int i = 0; i < 39; i++) {
		hPenPal[i].CreatePen(PS_SOLID, 1, RGB(0, 60 + i * 4.875, 0));
	}

	// color the head of the snake in some yellow
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

	CClientDC* dc = new CClientDC(this);

	if (m_dcGrid.GetSafeHdc() == NULL)
	{
		m_dcGrid.CreateCompatibleDC(dc);
		m_bitmapGrid.CreateCompatibleBitmap(dc, m_nClientWidth, m_nClientHeight);
		m_pbitmapOldGrid = m_dcGrid.SelectObject(&m_bitmapGrid);
	}

	dc->SetBkColor(RGB(0, 0, 0));
	dc->SetBkMode(OPAQUE);

	m_dcGrid.FillRect(m_rectClient, &brushBack);

	CString szText;


	SelectObject(*dc, GetStockObject(NULL_BRUSH));
	SelectObject(*dc, hPenClay);
	MoveToEx(*dc, 0, 0, NULL);
	LineTo(*dc, 0, m_rectClient.bottom);

	// plot trail
	//if (ready) {

	//		Ellipse(*dc, rect.Width() / 2 - (int)(gazeBuffer.front().x * conv) - 4,
	//			rect.Height() / 2 + (int)(gazeBuffer.front().y * conv) - 4,
	//			rect.Width() / 2 - (int)(gazeBuffer.front().x * conv) + 4,
	//			rect.Height() / 2 + (int)(gazeBuffer.front().y * conv) + 4);
	//
	//}

	// plot axes in white
	SelectObject(*dc, hPenWht);
	CFont* pOldFont = dc->GetCurrentFont();


	// gaze x/y plot

	dc->SelectObject(&headFont);
	SetTextColor(*dc, RGB(254, 152, 0));
	szText.Format(TEXT("PUPIL OFFSET"));
	dc->TextOutW(m_rectClient.Width() / 2 - 60, 10, szText);


	dc->SelectObject(&consBig);

	szText.Format(TEXT("x:"));
	dc->TextOutW(m_rectClient.Width() / 2 - 80, m_rectClient.Height() / 11, szText);
	szText.Format(TEXT("y:"));
	dc->TextOutW(m_rectClient.Width() / 2 + 30, m_rectClient.Height() / 11, szText);

	dc->SelectObject(consSmall);


	// plot the axes for the x/y gaze plot


	SelectObject(*dc, hPenGrey);

	float radius = 36;

	for (int i = 1; i < 5; i++)
	{
		Ellipse(*dc, m_rectClient.Width() / 2 - i*radius, m_rectClient.Height() / 2 - i*radius,
			m_rectClient.Width() / 2 + i*radius, m_rectClient.Height() / 2 + i*radius);
	}


	MoveToEx(*dc, m_rectClient.Width() / 2 - 144, m_rectClient.Height() / 2, NULL);		// horizontal bar
	LineTo(*dc, m_rectClient.Width() / 2 + 144, m_rectClient.Height() / 2);
	MoveToEx(*dc, m_rectClient.Width() / 2, m_rectClient.Height() / 2 - 144, NULL);		// vertical bar
	LineTo(*dc, m_rectClient.Width() / 2, m_rectClient.Height() - 97);

	// labels gaze axes with numbers
	szText.Format(TEXT("0.4"));
	dc->TextOutW(m_rectClient.Width() / 2 - 14, 80, szText);
	dc->TextOutW(m_rectClient.Width() - 55, m_rectClient.Height() / 2 - 8, szText);

	szText.Format(TEXT("-0.4"));
	dc->TextOutW(m_rectClient.Width() / 2 - 19, m_rectClient.Height() - 96, szText);
	dc->TextOutW(22, m_rectClient.Height() / 2 - 8, szText);


	szText.Format(TEXT("Current Pos [px]"));
	dc->TextOutW(15, m_rectClient.Height() - 40, szText);
	szText.Format(TEXT("x:"));
	dc->TextOutW(15, m_rectClient.Height() - 20, szText);
	szText.Format(TEXT("y:"));
	dc->TextOutW(70, m_rectClient.Height() - 20, szText);

	szText.Format(TEXT("Locked Pos [px]"));
	dc->TextOutW(m_rectClient.Width() / 2 + 60, m_rectClient.Height() - 40, szText);
	szText.Format(TEXT("x:"));
	dc->TextOutW(m_rectClient.Width() / 2 + 60, m_rectClient.Height() - 20, szText);
	szText.Format(TEXT("y:"));
	dc->TextOutW(m_rectClient.Width() / 2 + 115, m_rectClient.Height() - 20, szText);


	
		dc->SelectObject(&consBig);

		//// offset
		//szText.Format(TEXT("%.2f"), -1 * offset.x);
		//dc->TextOutW(rect.Width() / 2 - 60, rect.Height() / 11, szText);
		//szText.Format(TEXT("%.2f"), -1 * offset.y);
		//dc->TextOutW(rect.Width() / 2 + 50, rect.Height() / 11, szText);

		//dc->SelectObject(&consSmall);

		//// current position
		//SetTextColor(*dc, RGB(204, 96, 97));
		//szText.Format(TEXT("%.1f"), current.x);
		//dc->TextOutW(30, rect.Height() - 20, szText);
		//szText.Format(TEXT("%.1f"), current.y);
		//dc->TextOutW(85, rect.Height() - 20, szText);

		//// locked position
		//SetTextColor(*dc, RGB(155, 152, 254));
		//szText.Format(TEXT("%.1f"), locked.x);
		//dc->TextOutW(rect.Width() / 2 + 75, rect.Height() - 20, szText);
		//szText.Format(TEXT("%.1f"), locked.y);
		//dc->TextOutW(rect.Width() / 2 + 130, rect.Height() - 20, szText);

		//// draw offset into x/y plot
		////for (size_t i = 0, y = 0; i < 40; i++, y++) {

		////SelectObject(*dc, hPenPal[y]);
		//Ellipse(*dc, rect.Width() / 2 - (int)(offset.x * conv) - 4,
		//	rect.Height() / 2 + (int)(offset.y * conv) - 4,
		//	rect.Width() / 2 - (int)(offset.x * conv) + 4,
		//	rect.Height() / 2 + (int)(offset.y * conv) + 4);
		//gazeBuffer.pop_front();
		////}



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


void OffsetTracker::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

	m_nClientHeight = m_rectClient.Height();
	m_nClientWidth = m_rectClient.Width();

}
