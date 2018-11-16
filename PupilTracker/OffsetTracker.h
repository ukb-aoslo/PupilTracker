#pragma once
#include "listener.h"

using namespace std;

class OffsetTracker : public CWnd
{

public:

	OffsetTracker();
	virtual ~OffsetTracker();

	deque<coords<double, double>> trail;
	coords<double, double> dCurrentPosition;
	coords<double, double> dLockedPosition;

	void AddPoint(coords<double, double> dNewPoint);
	void DrawPoint();
	void DrawValues();
	void setLockedPos(coords<double, double>lockedPos);
	void InvalidateCtrl();
	void eraseTrail();

private:

	double		magnif;		// magnification factor
	float		conv;		// mapping factor

	COLORREF	bkgrndCol;
	HPEN		hPenGreen, hPenGrey, hPenWht, hPenClay;
	CFont		consBig, consSmall, headFont;
	CBrush		m_brushBack;

	CPen		hPenPal[40];

	int			m_nClientWidth, m_nClientHeight;
	CRect		m_rectPlot;
	CRect		m_rectClient;

	CDC			m_dcGrid;
	CDC			m_dcPlot;

	CBitmap		m_bitmapGrid;
	CBitmap		m_bitmapPlot;

	CBitmap		*m_pbitmapOldGrid;
	CBitmap		*m_pbitmapOldPlot;

	coords<double, double> m_dCurrentPosition;

	DECLARE_MESSAGE_MAP()

public:

	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);

};


