// Graph.cpp : implementation file
#include "stdafx.h"
#include "Graph.h"

// Graph

IMPLEMENT_DYNAMIC(Graph, CStatic)

Graph::Graph(Gaze* g)
{
	frame_count = 0;
	m_pGaze = g;
}

Graph:: Graph() 
{
	delete m_pGaze;
}

Graph::~Graph()
{
}


BEGIN_MESSAGE_MAP(Graph, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



void Graph::OnPaint()
{

	CPaintDC dc(this); // device context for painting
	CRect    rc;
	CDC      memDC;
	CBitmap  memBM;
	CBrush   hbrBkGnd;

	GetClientRect(&rc);

	memDC.CreateCompatibleDC(&dc);

	memBM.CreateCompatibleBitmap(&dc, rc.right - rc.left, rc.bottom - rc.top);
	CBitmap *pBM = memDC.SelectObject(&memBM);
	hbrBkGnd.CreateSolidBrush(RGB(0, 0, 0));
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


void Graph::Paint(CDC* dc)
{
	dc->SetBkColor(RGB(0, 0, 0));
	dc->SetBkMode(OPAQUE);

   // plots pupil over time in blue

	char szText[100];
	HPEN hPen, hPenOld;
	RECT rect;
	GetClientRect(&rect);

	SetTextColor(*dc, RGB(100, 200, 255));
	sprintf(szText, "Pupil Size [mm]");
	TextOut(*dc, rect.right / 2 - 50, 10, CA2W(szText), strlen(szText));

	hPen = CreatePen(PS_SOLID, 1, RGB(100, 200, 255));					// blue pen
	hPenOld = (HPEN)SelectObject(*dc, hPen);
	SelectObject(*dc, GetStockObject(NULL_BRUSH));

	for (int i = 0; i < 6; i++)
	{
		MoveToEx(*dc, 20, 180 - 28 * i, NULL);
		LineTo(*dc, 25, 180 - 28 * i);
		sprintf(szText, "%d", i + 2);
		TextOut(*dc, 10, 172 - 28 * i, CA2W(szText), strlen(szText));
	}

	SetTextColor(*dc, RGB(255, 255, 255));
	SetDCPenColor(*dc, RGB(255, 255, 255));

	for (int i = 0; i < 16; i++)
	{
		MoveToEx(*dc, 25 + 62 * i, rect.bottom - 48, NULL);
		LineTo(*dc, 25 + 62 * i, rect.bottom - 48 - 5);
		sprintf(szText, "%d", i * 2);
		TextOut(*dc, 22 + 62 * i, rect.bottom - 45, CA2W(szText), strlen(szText));
	}

	MoveToEx(*dc, 25, rect.bottom - 48, NULL);
	LineTo(*dc, 1024, rect.bottom - 48);

	sprintf(szText, "time [sec]");
	TextOut(*dc, 490, rect.bottom - 20, CA2W(szText), strlen(szText));

	float pupil = (m_pGaze->getPupilDia() - 3) * 28;

	for (int i = 0; i < frame_count; i++) {
		Ellipse(*dc, i-1 + 27,
			100 -(int)pupil - 1,
			i+1 + 27,
			100 -(int)pupil + 1);
	}

	DeleteObject(hPen);											// delete blue pen

	if (frame_count > rect.right - 40)
		frame_count = 0;
	frame_count++;

}


BOOL Graph::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return true;

	return CStatic::OnEraseBkgnd(pDC);
}
