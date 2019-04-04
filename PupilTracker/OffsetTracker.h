#pragma once
#include "listener.h"

using namespace std;

class OffsetTracker : public CWnd
{

public:

	OffsetTracker();
	virtual ~OffsetTracker();

	void AddPositions(coords<double, double> current, coords<double, double> locked);
	void DrawOffset();
	void DrawValues();
	void DrawTitle();
	void InvalidateCtrl();
	void eraseTrail();

private:

	deque<coords<double, double>> trail;
	coords<double, double> m_dCurrentPosition;
	coords<double, double> m_dLockedPosition;
	coords<double, double> m_dOffset;

	double		magnif;		// magnification factor
	double		conv;		// mapping factor

	HPEN		hPenGreen, hPenGrey, hPenWht, hPenClay;

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


protected:

	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()

};


