#pragma once
#include "listener.h"


class PupilDiaTracker : public CStatic
{

public:
	PupilDiaTracker();
	~PupilDiaTracker();

	void InvalidateCtrl();

private:

	int		m_nClientWidth, m_nClientHeight;
	CRect	m_rectClient;

	CDC     m_dcGrid;
	CDC     m_dcPlot;

	CBitmap m_bitmapGrid;
	CBitmap m_bitmapPlot;

	CBitmap *m_pbitmapOldGrid;
	CBitmap *m_pbitmapOldPlot;


	HPEN		hPenWht, hPenMag, hPenBge, hPenGrey, hPenClay;
	CBrush		brushBack;
	CFont		headFont, consBig;
	COLORREF	bkgrndCol;


protected:

	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()

};
