#include "stdafx.h"
#include "OffsetTracker.h"
#include "resource.h"
#include "ChildView.h"


// Paint Jobs for Offset Tracking

OffsetTracker::OffsetTracker()
{

	smallValueFont.CreatePointFont(124, _T("Arial Narrow"));
	
	LOGFONT logFont;
	smallValueFont.GetLogFont(&logFont);
	logFont.lfWeight = FW_BOLD;
	logFont.lfHeight = 25;
	
	bigValueFont.CreateFontIndirect(&logFont);
	
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

	m_dCurrentPupilPosition = { 0, 0 };
	m_dCurrentPurkinjePosition = { 0, 0 };

	m_dLockedPupilPosition = { 0, 0 };
	m_dLockedPurkinjePosition = { 0, 0 };

	// mapping pixels on plot with .5mm range
	//conv = (150 / 0.5) * res;	
	conv = 10;

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

void OffsetTracker::AddPupilPositions(coords<double, double> currentPupil, coords<double, double> lockedPupil, double res)
{
	// store current offset position in a queue

	m_dCurrentPupilPosition = currentPupil;
	m_dLockedPupilPosition = lockedPupil;

	if (lockedPupil.x != 0 && lockedPupil.y != 0)
		m_dPupilOffset = currentPupil - lockedPupil;
	else
		m_dPupilOffset.x = m_dPupilOffset.y = 0;

		if (trail.size() < 40) {
			trail.push_front(m_dPupilOffset);
		}

		else {
			trail.pop_back();
			trail.push_front(m_dPupilOffset);
		}

	Invalidate();

}

void OffsetTracker::AddPurkinjePositions(coords<double, double> currentPurkinje, coords<double, double> lockedPurkinje, double res) {

	m_dCurrentPurkinjePosition = currentPurkinje;
	m_dLockedPurkinjePosition = lockedPurkinje;

	if (lockedPurkinje.x != 0 && lockedPurkinje.y != 0)
		m_dPurkinjeOffset = currentPurkinje - lockedPurkinje;
	else
		m_dPurkinjeOffset.x = m_dPurkinjeOffset.y = 0;

	tco.x = tco.mx * (currentPurkinje.x - 320) + tco.xoff;
	tco.y = tco.my * (currentPurkinje.y - 240) + tco.yoff;

}


void OffsetTracker::PlotOffset() {

	if (m_dcPlot.GetSafeHdc() != NULL) {
		
		// make sure the plot bitmap is cleared
		//if (!infinity)
		//m_dcPlot.FillRect(m_rectClient, &m_brushBack);

		// draw offset into x/y plot
		SelectObject(m_dcPlot, GetStockObject(HOLLOW_BRUSH));
		int y = 0;

		for (auto it = trail.rbegin(); it != trail.rend(); it++) {

			SelectObject(m_dcPlot, hPenPal[y]);

			Ellipse(m_dcPlot, m_nClientWidth / 2 - (conv * it->x) - 3,
				m_nClientHeight / 2 + (conv * it->y) - 3,
				m_nClientWidth / 2 - (conv * it->x) + 3,
				m_nClientHeight / 2 + (conv * it->y) + 3);
			y++;
		
		}

	}

}

void OffsetTracker::DrawValues() {

	#define MARGIN_TOP 19
	#define MARGIN_BOTTOM 24
	
	CString szText;

	m_dcPlot.SetBkColor(black);
	m_dcPlot.FillRect(CRect(0, 0, 400, 45), &m_brushBack);
	m_dcPlot.FillRect(CRect(0, 437, 400, 480), &m_brushBack);

	if (m_dcPlot.GetSafeHdc() != NULL)

	{

		m_dcPlot.SelectObject(&smallValueFont);
				 
		// current PUPIL position
		SetTextColor(m_dcPlot, lightblue);
		szText.Format(TEXT("%.1f"), m_dCurrentPupilPosition.x);
		m_dcPlot.TextOutW(m_rectClient.left + 81, m_rectClient.top + MARGIN_TOP, szText);
		szText.Format(TEXT("%.1f"), m_dCurrentPupilPosition.y);
		m_dcPlot.TextOutW(m_rectClient.left + 128, m_rectClient.top + MARGIN_TOP, szText);

		// locked PUPIL position
		SetTextColor(m_dcPlot, lightgreen);
		szText.Format(TEXT("%.1f "), m_dLockedPupilPosition.x);
		m_dcPlot.TextOutW(m_rectClient.left + 192, m_rectClient.top + MARGIN_TOP, szText);
		szText.Format(TEXT("%.1f "), m_dLockedPupilPosition.y);
		m_dcPlot.TextOutW(m_rectClient.left + 239, m_rectClient.top + MARGIN_TOP, szText);

		// pupil OFFSET
		m_dcPlot.SelectObject(&bigValueFont);
		SetTextColor(m_dcPlot, darkazure);
		szText.Format(L"%5.1f", -1 * m_dPupilOffset.x);
		m_dcPlot.TextOutW(m_rectClient.left + 295, m_rectClient.top + MARGIN_TOP - 3, szText);
		szText.Format(L"%5.1f", -1 * m_dPupilOffset.y);
		m_dcPlot.TextOutW(m_rectClient.left + 347, m_rectClient.top + MARGIN_TOP - 3, szText);

		// current PURKINJE position
		m_dcPlot.SelectObject(&smallValueFont);
		SetTextColor(m_dcPlot, lightblue);
		szText.Format(TEXT("%.1f"), m_dCurrentPurkinjePosition.x);
		m_dcPlot.TextOutW(m_rectClient.left + 144, m_rectClient.bottom - MARGIN_BOTTOM, szText);
		szText.Format(TEXT("%.1f"), m_dCurrentPurkinjePosition.y);
		m_dcPlot.TextOutW(m_rectClient.left + 200, m_rectClient.bottom - MARGIN_BOTTOM, szText);

		// TCO
		m_dcPlot.SelectObject(&bigValueFont);
		SetTextColor(m_dcPlot, darkmagenta);
		szText.Format(L"%5.1f", tco.x);
		m_dcPlot.TextOutW(m_rectClient.left + 267, m_rectClient.bottom - MARGIN_BOTTOM - 3, szText);
		szText.Format(L"%5.1f", tco.y);
		m_dcPlot.TextOutW(m_rectClient.left + 332, m_rectClient.bottom - MARGIN_BOTTOM - 3, szText);

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
