// Gaze.cpp : implementation file
#include "stdafx.h"
#include "Gaze.h"

// Gaze

IMPLEMENT_DYNAMIC(Gaze, CStatic)

Gaze::Gaze()
{
	magnif = (float)22.2;	    // magnification pixel per mm for helmet gaze tracker Oct 26, 2005
								// old magnif was 26.8
	box_size = 100;             // radius in which the pupil edges can be located 
	Hirschberg_ratio = 12;

}

Gaze::~Gaze()
{
}

BEGIN_MESSAGE_MAP(Gaze, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

int Gaze::getBoxSize() {
	return box_size;
}

int Gaze::getHirschbergRatio() {
	return Hirschberg_ratio;
}

void Gaze::Paint(CDC* dc)
{
	//CPaintDC dc(this); // device context for painting
	//				   // TODO: Add your message handler code here
	//				   // Do not call CStatic::OnPaint() for painting messages
	CRect rect;
	GetClientRect(&rect);

	char szText[100];

	//paint it black
	dc->SetBkColor(RGB(0, 0, 0));
	dc->SetBkMode(OPAQUE);
	HPEN hPen, hPenOld;


	/************************** title of the screen display *******************************************/

	//SetTextColor(dc, RGB(255, 0, 0));
	//sprintf(szText, "black pupil tracker, for Rodenstock, version Oct 28, 2005 ");
	//TextOut(dc, 5, 415, CA2W(szText), strlen(szText));

	//SetTextColor(dc, RGB(255, 255, 255));								// white text
	//sprintf(szText, "position of center of black pupil in video frame is tracked");
	//TextOut(dc, 5, 430, CA2W(szText), strlen(szText));
	//sprintf(szText, "Hirschberg ratio = %ddeg/mm, pixel magnification = %2.2f", Hirschberg_ratio, magnif);
	//TextOut(dc, 5, 445, CA2W(szText), strlen(szText));

	/**********************************************************************************/
	/*                     now come the different plots over time                     */
	/**********************************************************************************/

	// plot axes in white
	hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));					// white pen
	hPenOld = (HPEN)SelectObject(*dc, hPen);
	SelectObject(*dc, GetStockObject(NULL_BRUSH));

	// gaze x/y plot
	SetTextColor(*dc, RGB(255, 0, 150));
	sprintf(szText, "Pupil Position [px]");
	TextOut(*dc, rect.Width() / 2 - 55, rect.Height() *.05f, CA2W(szText), strlen(szText));

	// plot the axes for the x/y gaze plot
	SetTextColor(*dc, RGB(255, 255, 255));

	MoveToEx(*dc, rect.Width() / 2, box_size - 15, NULL); // vert
	LineTo(*dc, rect.Width() / 2, rect.Height() - box_size + 15);
	MoveToEx(*dc, rect.Width()*.10f, rect.Height() / 2, NULL);        // horiz
	LineTo(*dc, rect.Width() - 45, rect.Height() / 2);

	// labels gaze axes with numbers
	sprintf(szText, "30 px");
	TextOut(*dc, rect.Width() / 2 + 3, box_size - 20, CA2W(szText), strlen(szText));
	TextOut(*dc, rect.Width() - 60, rect.Height() / 2 - 20, CA2W(szText), strlen(szText));

	sprintf(szText, "-30 px");
	TextOut(*dc, rect.Width() / 2 + 3, rect.Height() - box_size + 5, CA2W(szText), strlen(szText));
	TextOut(*dc, 35, rect.Height() / 2 - 20, CA2W(szText), strlen(szText));

	DeleteObject(hPen);											// delete white pen

																// draw circles at 20, 40 and 60 deg in gray
	hPen = CreatePen(PS_SOLID, 1, RGB(100, 100, 100));			// gray pen
	hPenOld = (HPEN)SelectObject(*dc, hPen);
	SelectObject(*dc, GetStockObject(NULL_BRUSH));

	int radius = 24; 	// note, currently 80 deg map on 'x_shift' (=384) pixels
						// or 4.8 pixel per degree, or 30 deg = 144 pixels
						// and 5 deg = 24 pixels
	for (int i = 1; i < 7; i++)
	{
		Ellipse(*dc, rect.Width() / 2 - i*radius, rect.Height() / 2 - i*radius,
			rect.Width() / 2 + i*radius, rect.Height() / 2 + i*radius);
	}


	DeleteObject(hPen);											// delete gray pen



	hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 150));					// pink pen
	hPenOld = (HPEN)SelectObject(*dc, hPen);
	SelectObject(*dc, GetStockObject(NULL_BRUSH));

	CPen hPenPal[40];
	for (int i = 1; i < 40; i++) {
		hPenPal[i].CreatePen(PS_SOLID, 1, RGB(255 - (i*6.375), 0, 150 - (i*3.75)));
	}

	// then in x/y plot
	if (GazePX.size() > 80 &&
		GazePX.back().x < 60 &&
		GazePX.back().x > -60 &&
		GazePX.back().y < 60 &&
		GazePX.back().y > -60)
	{
		std::vector<coords>::reverse_iterator rit;
		std::vector<coords>::reverse_iterator pre_rit = GazePX.rbegin();
		int i = 1;
		for (rit = GazePX.rbegin()+1; rit != GazePX.rbegin() + 40; ++rit)
		{
			Ellipse(*dc, rect.Width() / 2 - (int)(rit->x * 4.83) - 2,
				rect.Height() / 2 + (int)(rit->y * 4.83) - 2,
				rect.Width() / 2 - (int)(rit->x * 4.83) + 2,
				rect.Height() / 2 + (int)(rit->y * 4.83) + 2);
			if ((std::abs(rit->x) - std::abs(pre_rit->x)) > 5 |
				(std::abs(rit->y) - std::abs(pre_rit->y)) > 5 )  {
				i++;
				pre_rit = rit;
			}
			else {
				i=1;
			}
			SelectObject(*dc, hPenPal[i]);
		}
	}

	DeleteObject(hPen);											// delete pink pen
}

void Gaze::OnPaint() {

	CPaintDC dc(this); // device context for painting
	CRect    rc;
	CDC      memDC;
	CBitmap  memBM;
	CBrush   hbrBkGnd;

	GetClientRect(&rc);

	memDC.CreateCompatibleDC(&dc);

	memBM.CreateCompatibleBitmap(&dc, rc.right - rc.left, rc.bottom - rc.top);
	CBitmap *pBM = memDC.SelectObject(&memBM);
	hbrBkGnd.CreateSolidBrush(RGB(0,0,0));
	memDC.FillRect(&rc, &hbrBkGnd);
	Paint(&memDC);

	dc.BitBlt(rc.left, rc.top,
		rc.right - rc.left,
		rc.bottom - rc.top,
		&memDC,
		0, 0,
		SRCCOPY);

	memDC.SelectObject(pBM);
	memBM.DeleteObject();

}

void Gaze::addPupilCenter(float x, float y)
{
	coords c{ x,y };
	Pupil.push_back(c);
}

void Gaze::addPupilDia(float x)
{
	PupilDia.push_back(x);
}

float Gaze::getPupilDia()
{
	if(!PupilDia.empty())
		return PupilDia.back();
	return 0;
}

void Gaze::addGazePX(float x, float y)
{
	Gaze::coords c{ x,y };
	GazePX.push_back(c);
}

Gaze::coords Gaze::getGazePX() {
	if(!GazePX.empty())
		return GazePX.back();
	return coords();
}


// Gaze message handlers

void Gaze::Save(FILE* pFile)
{
	fprintf(pFile, "***************************************************************************************\n");
	fprintf(pFile, "********** Gaze Tracker Data						version October 26, 2005 **********\n");
	fprintf(pFile, "***************************************************************************************\n\n");

	fprintf(pFile, "\nNOTE: gaze data are in degrees, measured from the distance of pupil center to Purkinje image center\n");

	fprintf(pFile, "\n\ndata are: frame number, pupil size [mm], gaze horizontal [deg], gaze vertical [deg] \n");

	for (int i = 0; i < Pupil.size(); i++) {
		fprintf(pFile, "%d	%2.2f	%2.1f	%2.1f\n", i, Pupil[i], GazePX[i].x, GazePX[i].y);
	}

}

BOOL Gaze::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return true;

}
