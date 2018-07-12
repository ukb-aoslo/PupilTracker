#include "stdafx.h"
#include "PupilDiaTracker.h"

// Graph


PupilDiaTracker::PupilDiaTracker()
{
	hPenGrey = CreatePen(PS_SOLID, 1, RGB(150, 150, 150));			// grey pen
	hPenWht = CreatePen(PS_SOLID, 1, RGB(254, 152, 0));				// white pen
	hPenMag = CreatePen(PS_SOLID, 1, RGB(155, 152, 254));			// lilac pen
	hPenBge = CreatePen(PS_SOLID, 1, RGB(255, 168, 115));			// beige pen
	hPenClay = CreatePen(PS_SOLID, 1, RGB(204, 96, 97));			// clay pen

	bkgrndCol = RGB(0, 0, 0);
	brushBack.CreateSolidBrush(bkgrndCol);
	
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

}

PupilDiaTracker::~PupilDiaTracker()
{

	// just to be picky restore the bitmaps for the two memory dc's
	// (these dc's are being destroyed so there shouldn't be any leaks)
	if (m_pbitmapOldGrid != NULL)
		m_dcGrid.SelectObject(m_pbitmapOldGrid);
	if (m_pbitmapOldPlot != NULL)
		m_dcPlot.SelectObject(m_pbitmapOldPlot);

}

BEGIN_MESSAGE_MAP(PupilDiaTracker, CStatic)
	ON_WM_PAINT()
	ON_WM_SIZE()
END_MESSAGE_MAP()

void PupilDiaTracker::InvalidateCtrl() {

	CClientDC* dc = new CClientDC(this);

	if (m_dcGrid.GetSafeHdc() == NULL)
	{
		m_dcGrid.CreateCompatibleDC(dc);
		m_bitmapGrid.CreateCompatibleBitmap(dc, m_nClientWidth, m_nClientHeight);
		m_pbitmapOldGrid = m_dcGrid.SelectObject(&m_bitmapGrid);
	}

	dc->SetBkColor(RGB(0, 0, 0));
	dc->SetBkMode(OPAQUE);

	CStringW szText;

	m_dcGrid.FillRect(m_rectClient, &brushBack);

	dc->SelectObject(&headFont);
	SetTextColor(*dc, RGB(254, 152, 0));
	szText.Format(TEXT("PUPIL DIAMETER"));
	dc->TextOutW(m_rectClient.right / 2 - 80, 10, szText);

	SelectObject(*dc, hPenClay);

	MoveToEx(*dc, 0, 0, NULL);
	LineTo(*dc, m_rectClient.right, 0);

	SelectObject(*dc, hPenWht);
	for (int i = 0; i < 6; i++)
	{
		MoveToEx(*dc, 20, 181 - 28 * i, NULL);
		LineTo(*dc, 25, 181 - 28 * i);
		szText.Format(TEXT("%d"), i + 4);
		dc->TextOutW(10, 172 - 28 * i, szText);
	}

	for (int i = 0; i < 16; i++)
	{
		MoveToEx(*dc, 25 + 62 * i, m_rectClient.bottom - 48, NULL);
		LineTo(*dc, 25 + 62 * i, m_rectClient.bottom - 48 - 5);
		szText.Format(TEXT("%d"), i * 2);
		dc->TextOutW(22 + 62 * i, m_rectClient.bottom - 45, szText);
	}

	MoveToEx(*dc, 25, m_rectClient.bottom - 48, NULL);
	LineTo(*dc, 1024, m_rectClient.bottom - 48);

	dc->SelectObject(&consBig);

	/*
	if (!PupilDia.empty()) {

		szText.Format(TEXT("%.1fmm"), PupilDia.back());
		dc->TextOutW(rect.right / 2 + 65, 12, szText);

		SelectObject(*dc, hPenBge);

		size_t s1 = PupilDia.size();

		int graph_coords;

		if (!frozen) {

			for (size_t i = turn; i < s1; i++) {
				graph_coords = (int)(PupilDia[i] * 28);
				if (i == turn) MoveToEx(*dc, 25, 293 - graph_coords, NULL);
				LineTo(*dc, i - turn + 25,
					293 - graph_coords
				);
			}
		}

		else {

			for (size_t i = turn; i < s1; i++) {
				graph_coords = 293 - (int)(PupilDia[i] * 28);
				if (i == turn) MoveToEx(*dc, 25, graph_coords, NULL);
				LineTo(*dc, i - turn + 25,
					graph_coords
				);
			}

			for (size_t i = turn; i < s1; i++) {
				SetPixel(*dc, i - turn + 25,
					293 - (int)(frozen_pupil * 28),
					RGB(155, 152, 254));
			}

		}


		if (frame_count > rect.right - 40) {
			turn = turn + frame_count;
			frame_count = 0;
		}

		frame_count++;

	}

	*/
}

void PupilDiaTracker::OnPaint()
{
////////////////////////////////////////////////////////////////////////////

	CPaintDC	dc(this);	// device context for painting
	CDC			memDC;
	CBitmap		memBitmap;
	CBitmap*	oldBitmap;	// bitmap originally found in CMemDC

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
			&m_dcPlot, 0, 0, SRCPAINT);		// SRCPAINT
											// finally send the result to the display
		dc.BitBlt(0, 0, m_nClientWidth, m_nClientHeight,
			&memDC, 0, 0, SRCCOPY);
	}

	memDC.SelectObject(oldBitmap);

}


void PupilDiaTracker::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

	// set some member variables to avoid multiple function calls
	m_nClientHeight = m_rectClient.Height();
	m_nClientWidth = m_rectClient.Width();

}
