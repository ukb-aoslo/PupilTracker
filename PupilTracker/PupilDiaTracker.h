#pragma once
#include "listener.h"


class PupilDiaTracker : public CWnd
{

public:
	PupilDiaTracker();
	virtual ~PupilDiaTracker();


	struct THEPOINT
	{
		coords<double, double> 	dCurrentPosition;   // current position
		coords<double, double> 	dPreviousPosition;  // previous position
		double	dLowerLimit;
		double	dUpperLimit;
		double	dRange;
		double	dVerticalFactor;
		int		nYDecimals;
		CPen	penPlot;
		bool	bUsed;
	} m_Point[40];


	void InvalidateCtrl();
	void DrawPoint();
	void DrawPoints();
	double AppendPoint(double dia);

private:

	CDC			m_dcGrid;
	CDC			m_dcPlot;


	int m_nHalfShiftPixels;
	int m_nPlotShiftPixels;
	int m_nPlotHeight;
	int m_nPlotWidth;
	int m_nShiftPixels;          // amount to shift with each new point 


	int			m_nClientWidth, m_nClientHeight;

	CBrush		m_brushBack;
	CRect		m_rectPlot;
	CRect		m_rectClient;

	CBitmap		m_bitmapGrid;
	CBitmap		m_bitmapPlot;

	CBitmap		*m_pbitmapOldGrid;
	CBitmap		*m_pbitmapOldPlot;

	HPEN		hPenWht, hPenMag, hPenBge, hPenGrey, hPenClay;
	CBrush		brushBack;
	CFont		headFont, consBig;
	double		m_dCurrentPosition;
	
	double m_dRange;
	double m_dVerticalFactor;


protected:

	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()

};
