#include "stdafx.h"
#include "PupilDiaTracker.h"
#include "resource.h"
//#include "ChildView.h"

// Paint Jobs for Pupil Diameter Tracking

PupilDiaTracker::PupilDiaTracker()
{
	m_brushBack.CreateSolidBrush(RGB(0, 0, 0));
	
	// protected bitmaps to restore the memory DC's
	m_pbitmapOldGrid = NULL;
	m_pbitmapOldPlot = NULL;

	// m_nShiftPixels determines how much the plot shifts (in terms of pixels) 
	// with the addition of a new data point

	m_dPreviousPosition = 0.0;

	// m_nShiftPixels determines how much the plot shifts (in terms of pixels) 
	// with the addition of a new data point

	m_nShiftPixels = 2;
	m_nHalfShiftPixels = m_nShiftPixels / 2;                    // protected
	m_nPlotShiftPixels = m_nShiftPixels + m_nHalfShiftPixels;	// protected

	m_penPlot.CreatePen(PS_SOLID, 0, RGB(254, 152, 0));


	// set some initial values for the scaling until "SetRange" is called.
	// these are protected variables and must be set with SetRange
	// in order to ensure that m_dRange is updated accordingly

	m_dLowerLimit = 3;
	m_dUpperLimit = 10;
	m_dRange = m_dUpperLimit - m_dLowerLimit;

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

	CClientDC dc(this);

	if (m_dcGrid.GetSafeHdc() == NULL)
	{
		m_dcGrid.CreateCompatibleDC(&dc);
		m_bitmapGrid.LoadBitmapW(IDB_BITMAP2);
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

	//dc->SelectObject(&headFont);
	//SetTextColor(*dc, RGB(254, 152, 0));
	//szText.Format(TEXT("PUPIL DIAMETER"));
	//dc->TextOutW(m_rectClient.right / 2 - 80, 10, szText);

	//SelectObject(*dc, hPenClay);

	//MoveToEx(*dc, 0, 0, NULL);
	//LineTo(*dc, m_rectClient.right, 0);

	//SelectObject(*dc, hPenWht);
	//for (int i = 0; i < 6; i++)
	//{
	//	MoveToEx(*dc, 20, 181 - 28 * i, NULL);
	//	LineTo(*dc, 25, 181 - 28 * i);
	//	szText.Format(TEXT("%d"), i + 4);
	//	dc->TextOutW(10, 172 - 28 * i, szText);
	//}

	//for (int i = 0; i < 16; i++)
	//{
	//	MoveToEx(*dc, 25 + 62 * i, m_rectClient.bottom - 48, NULL);
	//	LineTo(*dc, 25 + 62 * i, m_rectClient.bottom - 48 - 5);
	//	szText.Format(TEXT("%d"), i * 2);
	//	dc->TextOutW(22 + 62 * i, m_rectClient.bottom - 45, szText);
	//}

	//MoveToEx(*dc, 25, m_rectClient.bottom - 48, NULL);
	//LineTo(*dc, 1024, m_rectClient.bottom - 48);

	//dc->SelectObject(&consBig);


	/*
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
	oldBitmap = (CBitmap*)memDC.SelectObject(&memBitmap);

	if (memDC.GetSafeHdc() != NULL)
	{
		// first drop the grid on the memory dc
		memDC.BitBlt(0, 0, m_nClientWidth, m_nClientHeight,	&m_dcGrid, 0, 0, SRCCOPY);
		// now add the plot on top as a "pattern" via SRCPAINT.
		// works well with dark background and a light plot
		memDC.BitBlt(0, 0, m_nClientWidth, m_nClientHeight,	&m_dcPlot, 0, 0, SRCPAINT);		// SRCPAINT

		// finally send the result to the display
		dc.BitBlt(0, 0, m_nClientWidth, m_nClientHeight, &memDC, 0, 0, SRCCOPY);

	}
	
	memDC.SelectObject(oldBitmap);

}


double PupilDiaTracker::AppendPoint(double dia) {

	// append a data point to the plot
	// return the previous point

	double dPrevious;

	dPrevious = m_dCurrentPosition;
	m_dCurrentPosition = dia;
	Invalidate();

	return dPrevious;

}

void PupilDiaTracker::DrawTitle() {


	if (m_dcGrid.GetSafeHdc() != NULL) {

		m_dcGrid.SetTextColor(darkyellow);
		m_dcGrid.SetBkColor(RGB(0, 0, 0));
		m_dcGrid.SetBkMode(OPAQUE);

		CStringW szText;

		if (m_dCurrentPosition > m_dLowerLimit && m_dCurrentPosition < m_dUpperLimit) {
			szText.Format(TEXT("PUPIL DIAMETER: %.1fmm"), m_dCurrentPosition);
			m_dcGrid.TextOutW(m_nClientWidth / 2 - 120, 12, szText);
		}

		else {
			m_dcGrid.FillRect(CRect(m_nClientWidth / 2 - 120, 12, m_nClientWidth / 2 + 200, 40), &m_brushBack);
			szText.Format(TEXT("PUPIL DIAMETER: N/A"));
			m_dcGrid.TextOutW(m_nClientWidth / 2 - 120, 12, szText);
		}

	}

}

void PupilDiaTracker::DrawPoint() {

	// this does the work of "scrolling" the plot to the left
	// and appending a new data point all of the plotting is 
	// directed to the memory based bitmap associated with m_dcPlot
	// the will subsequently be BitBlt'd to the client in OnPaint

	int currX, prevX, currY, prevY;

	CPen *oldPen;
	CRect rectCleanUpPlot;

	if (m_dcPlot.GetSafeHdc() != NULL)
	{
		// shift the plot by BitBlt'ing it to itself 
		// note: the m_dcPlot covers the entire client
		//       but we only shift bitmap that is the size 
		//       of the plot rectangle
		// grab the right side of the plot (exluding m_nShiftPixels on the left)
		// move this grabbed bitmap to the left by m_nShiftPixels

   		m_dcPlot.BitBlt(m_rectPlot.left, m_rectPlot.top + 1,
			m_nPlotWidth, m_nPlotHeight, &m_dcPlot,
			m_rectPlot.left + m_nShiftPixels, m_rectPlot.top + 1,
			SRCCOPY);

		// establish a rectangle over the right side of plot
		// which now needs to be cleaned up proir to adding the new point
		rectCleanUpPlot = m_rectPlot;
		rectCleanUpPlot.left = rectCleanUpPlot.right - m_nShiftPixels;

		// fill the cleanup area with the background
		m_dcPlot.FillRect(rectCleanUpPlot, &m_brushBack);

		// draw the next line segement

		// grab the plotting pen
		oldPen = m_dcPlot.SelectObject(&m_penPlot);

		// move to the previous point
		prevX = m_rectPlot.right - m_nPlotShiftPixels;
		prevY = m_rectPlot.bottom -
			(long)((m_dPreviousPosition - m_dLowerLimit) * m_dVerticalFactor);
		m_dcPlot.MoveTo(prevX, prevY);

		// draw to the current point
		currX = m_rectPlot.right - m_nHalfShiftPixels;
		currY = m_rectPlot.bottom -
			(long)((m_dCurrentPosition - m_dLowerLimit) * m_dVerticalFactor);
		m_dcPlot.LineTo(currX, currY);

		// restore the pen 
		m_dcPlot.SelectObject(oldPen);

		// if the data leaks over the upper or lower plot boundaries
		// fill the upper and lower leakage with the background
		// this will facilitate clipping on an as needed basis
		// as opposed to always calling IntersectClipRect
		if ((prevY <= m_rectPlot.top) || (currY <= m_rectPlot.top))
			m_dcPlot.FillRect(CRect(prevX, m_rectClient.top, currX + 1, m_rectPlot.top + 1), &m_brushBack);
		if ((prevY >= m_rectPlot.bottom) || (currY >= m_rectPlot.bottom))
			m_dcPlot.FillRect(CRect(prevX, m_rectPlot.bottom + 1, currX + 1, m_rectClient.bottom + 1), &m_brushBack);

		// store the current point for connection to the next point
		m_dPreviousPosition = m_dCurrentPosition;


	}

}

void PupilDiaTracker::SetRange(double dLower, double dUpper, int nDecimalPlaces)
{
	ASSERT(dUpper > dLower);

	m_dLowerLimit = dLower;
	m_dUpperLimit = dUpper;
	m_nYDecimals = nDecimalPlaces;
	m_dRange = m_dUpperLimit - m_dLowerLimit;
	m_dVerticalFactor = (double)m_nPlotHeight / m_dRange;

	// clear out the existing garbage, re-start with a clean plot
	InvalidateCtrl();

}  // SetRange

void PupilDiaTracker::SetPlotColor(COLORREF rgb)
{
	m_penPlot.DeleteObject();
	m_penPlot.CreatePen(PS_SOLID, 1, rgb);

	// clear out the existing garbage, re-start with a clean plot
	InvalidateCtrl();

}

void PupilDiaTracker::OnSize(UINT nType, int cx, int cy)
{

	CWnd::OnSize(nType, cx, cy);

	// NOTE: OnSize automatically gets called during the setup of the control

	GetClientRect(m_rectClient);

	// set some member variables to avoid multiple function calls
	m_nClientHeight = m_rectClient.Height();
	m_nClientWidth = m_rectClient.Width();

	// the "left" coordinate and "width" will be modified in 
	// InvalidateCtrl to be based on the width of the y axis scaling
	m_rectPlot.left = 30;
	m_rectPlot.top = 46;
	m_rectPlot.right = m_rectClient.right - 20;
	m_rectPlot.bottom = m_rectClient.bottom - 35;

	// set some member variables to avoid multiple function calls
	m_nPlotHeight = m_rectPlot.Height();
	m_nPlotWidth = m_rectPlot.Width();

	// set the scaling factor for now, this can be adjusted 
	// in the SetRange functions
	m_dVerticalFactor = (double)m_nPlotHeight / m_dRange;

	if (m_pbitmapOldGrid != NULL)
		m_dcGrid.SelectObject(m_pbitmapOldGrid);
	if (m_pbitmapOldPlot != NULL)
		m_dcPlot.SelectObject(m_pbitmapOldPlot);

	m_dcGrid.DeleteDC();
	m_dcPlot.DeleteDC();
	m_bitmapPlot.DeleteObject();
	m_bitmapGrid.DeleteObject();

	SetRange((double)3, (double)10, 1);
	SetPlotColor(darkyellow);

	InvalidateCtrl();

}