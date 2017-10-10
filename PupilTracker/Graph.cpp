// Graph.cpp : implementation file
#include "stdafx.h"
#include "Graph.h"
#include "Schaeffel.h"

// Graph

IMPLEMENT_DYNAMIC(Graph, CStatic)


Graph:: Graph() 
{
	frame_count = 0;
	turn = 0;
	frozen = false;

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
		frozen_pupil = PupilDia.back();
	}
	else {
		frozen = false;
		PupilDia.clear();
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

	// plots pupil over time in green

	char szText[100];
	HPEN hPenWht, hPenMag, hPenGrn;
	RECT rect;
	GetClientRect(&rect);

	SetTextColor(*dc, RGB(255, 255, 255));
	sprintf(szText, "Pupil Diameter [mm]:");
	TextOut(*dc, rect.right / 2 - 50, 10, CA2W(szText), strlen(szText));

	hPenWht = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));				// white pen
	hPenMag = CreatePen(PS_SOLID, 1, RGB(255, 0, 255));					// magenta pen
	hPenGrn = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));					// green pen
	SelectObject(*dc, hPenWht);

	for (int i = 0; i < 6; i++)
	{
		MoveToEx(*dc, 20, 181 - 28 * i, NULL);
		LineTo(*dc, 25, 181 - 28 * i);
		sprintf(szText, "%d", i + 4);
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

	if (!PupilDia.empty()) {

		sprintf(szText, "%.1f", PupilDia.back());
		TextOut(*dc, rect.right / 2 + 90, 10, CA2W(szText), strlen(szText));

		SelectObject(*dc, hPenGrn);

		int s1 = PupilDia.size();
		int graph_coords;

		if (!frozen) {
			for (size_t i = turn; i < s1; i++) {
				graph_coords = (int)(PupilDia[i] * 28);
				if (i == turn) MoveToEx(*dc, 25, 293 - graph_coords, NULL);
				LineTo(*dc, i-turn + 25,
				 293 - graph_coords
				);
			}
		}

		else {
			for (size_t i = turn; i < s1; i++) {
				graph_coords = 293 - (int)(PupilDia[i] * 28);
				if (i == turn) MoveToEx(*dc, 25, graph_coords, NULL);
				LineTo(*dc, i-turn + 25,
					graph_coords
				);
			}
			for (size_t i = turn; i < s1; i++) {
				SetPixel(*dc, i-turn + 25,
					294 - frozen_pupil * 28,
					RGB(255, 0, 255));

			}

		}
		


	if (frame_count > rect.right - 40) {
		turn = turn + frame_count;
		frame_count = 0;
		}
	
	frame_count++;

	
	}

	DeleteObject(hPenGrn);
	DeleteObject(hPenMag);
	DeleteObject(hPenWht);
}

void Graph::addPupilDia(float x)
{
	PupilDia.push_back(x);
}


BOOL Graph::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return true;

	return CStatic::OnEraseBkgnd(pDC);
}
