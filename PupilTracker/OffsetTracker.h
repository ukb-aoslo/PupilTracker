#pragma once
#include "listener.h"

using namespace std;

class OffsetTracker: public CStatic
{

public:

	OffsetTracker();
	~OffsetTracker();

	void InvalidateCtrl();

private:

	double magnif;
	float conv;

	COLORREF	bkgrndCol;
	HPEN		hPenGreen, hPenGrey, hPenWht, hPenClay;
	CFont		consBig, consSmall, headFont;
	CBrush		brushBack;

	CPen		hPenPal[40];

	int			m_nClientWidth, m_nClientHeight;
	CRect		m_rectClient;

	CDC			m_dcGrid;
	CDC			m_dcPlot;

	CBitmap		m_bitmapGrid;
	CBitmap		m_bitmapPlot;

	CBitmap		*m_pbitmapOldGrid;
	CBitmap		*m_pbitmapOldPlot;

public:

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);

};


