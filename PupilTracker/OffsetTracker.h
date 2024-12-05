#pragma once
#include "listener.h"

using namespace std;


class OffsetTracker : public CWnd
{

public:

	OffsetTracker();
	virtual ~OffsetTracker();

	void AddPupilPositions(coords<double, double> current, coords<double, double> locked, double res);
	void AddPurkinjePositions(coords<double, double> current, coords<double, double> locked, double res);
	void PlotOffset();
	void DrawValues();
	void InvalidateCtrl();
	void eraseTrail();

	struct TCO {
		double mx, my, xoff, yoff;
		double x, y;
	} tco = { 0, 0, 0, 0, 0, 0 };


private:

	deque<coords<double, double>> trail;

	coords<double, double> m_dCurrentPupilPosition;
	coords<double, double> m_dLockedPupilPosition;
	coords<double, double> m_dCurrentPurkinjePosition;
	coords<double, double> m_dLockedPurkinjePosition;
	coords<double, double> m_dPupilOffset;
	coords<double, double> m_dPurkinjeOffset;

	CFont		bigValueFont;
	CFont		smallValueFont;

	double		conv;

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

	DECLARE_MESSAGE_MAP();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);

};


