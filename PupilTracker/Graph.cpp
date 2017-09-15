// Graph.cpp : implementation file
#include "stdafx.h"
#include "Graph.h"

// Graph

IMPLEMENT_DYNAMIC(Graph, CStatic)

Graph::Graph(Gaze* g)
{
	frame_count = 0;
	turn = 0;
	frozen = false;
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

void Graph::freeze() {
	

	if (!frozen) {
		frozen = true;
		frozen_pupil = pupilsize.back();
	}
	else {
		frozen = false;
		pupilsize.clear();
		frame_count = 0;
		turn = 0;
	}

}

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
	HPEN hPenWht, hPenMag, hPenGrn;
	RECT rect;
	GetClientRect(&rect);

	SetTextColor(*dc, RGB(255, 255, 255));
	sprintf(szText, "Pupil Size [mm]:");
	TextOut(*dc, rect.right / 2 - 50, 10, CA2W(szText), strlen(szText));

	hPenWht = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));				// white pen
	hPenMag = CreatePen(PS_SOLID, 1, RGB(255, 0, 255));					// magenta pen
	hPenGrn = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));					// green pen
	SelectObject(*dc, hPenWht);

	for (int i = 0; i < 6; i++)
	{
		MoveToEx(*dc, 20, 180 - 28 * i, NULL);
		LineTo(*dc, 25, 180 - 28 * i);
		sprintf(szText, "%d", i + 2);
		TextOut(*dc, 10, 172 - 28 * i, CA2W(szText), strlen(szText));
	}

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

	pupilsize.push_back((m_pGaze->getPupilDia() - 3) * 28);
	if (!pupilsize.empty())
	MoveToEx(*dc, 25, pupilsize[0], NULL);

	sprintf(szText, "%.1f", m_pGaze->getPupilDia());
	TextOut(*dc, rect.right / 2 + 60, 10, CA2W(szText), strlen(szText));
	
	SelectObject(*dc, hPenGrn);
	
	if (!frozen) {
		
		for (int i = 0; i < frame_count; i++) {
			LineTo(*dc, i + 25,
				150 - (int)pupilsize[i + turn]
			);
		}
	}

	else {
		for (int i = 0; i < frame_count; i++) {
			LineTo(*dc, i + 25,
				150 - (int)pupilsize[i + turn]
			);
		}
		for (int i = 0; i < frame_count; i++) {
			SetPixel(*dc, i + 25,
				150 - frozen_pupil,
				RGB(255, 0, 255)
			);
		}

	}

	DeleteObject(hPenGrn);
	DeleteObject(hPenMag);
	DeleteObject(hPenWht);

	if (frame_count > rect.right - 40) {
		turn = turn + frame_count;
		frame_count = 0;
	}
	frame_count++;

}


BOOL Graph::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return true;

	return CStatic::OnEraseBkgnd(pDC);
}
