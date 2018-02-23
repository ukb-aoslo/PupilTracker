// Graph.cpp : implementation file
#include "stdafx.h"
#include "Graph.h"
#include "Schaeffel.h"
extern std::mutex g_mutex;

// Graph

IMPLEMENT_DYNAMIC(Graph, CStatic)

Graph::Graph()
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
	ON_MESSAGE(WM_UPDATE_CONTROL, OnUpdateControl)
	ON_WM_CREATE()
END_MESSAGE_MAP()

LRESULT Graph::OnUpdateControl(WPARAM wParam, LPARAM lParam)
{
	RedrawWindow();
	return 0;
}


void Graph::freeze() {


	if (!frozen) {
		frozen = true;
		frozen_pupil = PupilDia.back();
	}

	else{
		frozen = false;
		PupilDia.clear();
		frame_count = 0;
		turn = 0;
	}

}


void Graph::Paint(CDC* dc)
{

	dc->SetBkColor(RGB(0, 0, 0));
	dc->SetBkMode(OPAQUE);

	// plots pupil over time in green

	CStringW szText;
	HPEN hPenWht, hPenMag, hPenGrn;
	RECT rect;
	GetClientRect(&rect);

	SetTextColor(*dc, RGB(255, 255, 255));
	szText.Format(TEXT("Pupil Diameter [mm]:"));
	dc->TextOutW(rect.right / 2 - 50, 10, szText);

	hPenWht = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));				// white pen
	hPenMag = CreatePen(PS_SOLID, 1, RGB(255, 0, 255));					// magenta pen
	hPenGrn = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));					// green pen
	SelectObject(*dc, hPenWht);

	for (int i = 0; i < 6; i++)
	{
		MoveToEx(*dc, 20, 181 - 28 * i, NULL);
		LineTo(*dc, 25, 181 - 28 * i);
		szText.Format(TEXT("%d"), i + 4);
		dc->TextOutW(10, 172 - 28 * i, szText);
	}

	for (int i = 0; i < 16; i++)
	{
		MoveToEx(*dc, 25 + 62 * i, rect.bottom - 48, NULL);
		LineTo(*dc, 25 + 62 * i, rect.bottom - 48 - 5);
		szText.Format(TEXT("%d"), i * 2);
		dc->TextOutW(22 + 62 * i, rect.bottom - 45, szText);
	}

	MoveToEx(*dc, 25, rect.bottom - 48, NULL);
	LineTo(*dc, 1024, rect.bottom - 48);

	szText.Format(TEXT("time [sec]"));
	dc->TextOutW(490, rect.bottom - 20, szText);

	if (!PupilDia.empty() && g_mutex.try_lock()) {

		szText.Format(TEXT("%.1f"), PupilDia.back());
		dc->TextOutW(rect.right / 2 + 90, 10, szText);

		SelectObject(*dc, hPenGrn);

		size_t s1 = PupilDia.size();
		int graph_coords;

		if (!frozen) {

			for (size_t i = turn; i < s1; i++) {
				graph_coords = (int)(PupilDia[i] * 28);
				if (i == turn) MoveToEx(*dc, 25, 293 - graph_coords, NULL);
				LineTo(*dc, i - turn + 25,
					293 - graph_coords
				);
			}
		}

		else {

			for (size_t i = turn; i < s1; i++) {
				graph_coords = 293 - (int)(PupilDia[i] * 28);
				if (i == turn) MoveToEx(*dc, 25, graph_coords, NULL);
				LineTo(*dc, i - turn + 25,
					graph_coords
				);
			}

			for (size_t i = turn; i < s1; i++) {
				SetPixel(*dc, i - turn + 25,
					293 - (int)(frozen_pupil * 28),
					RGB(255, 0, 255));
			}

		}


		if (frame_count > rect.right - 40) {
			turn = turn + frame_count;
			frame_count = 0;
		}

		frame_count++;

		g_mutex.unlock();

	}
	
	DeleteObject(hPenGrn);
	DeleteObject(hPenMag);
	DeleteObject(hPenWht);

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

void Graph::addPupilDia(double x)
{
	PupilDia.push_back(x);
}


BOOL Graph::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return true;

	return CStatic::OnEraseBkgnd(pDC);
}
