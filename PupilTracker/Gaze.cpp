// Gaze.cpp : implementation file
#include "stdafx.h"
#include "Gaze.h"
#include "Schaeffel.h"

extern std::mutex g_mutex;

// Gaze

IMPLEMENT_DYNAMIC(Gaze, CStatic)

Gaze::Gaze()
{
	record = false;
	conv = 9.3f;
}

Gaze::~Gaze()
{
}

BEGIN_MESSAGE_MAP(Gaze, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_UPDATE_CONTROL, OnUpdateControl)
	ON_WM_TIMER()
END_MESSAGE_MAP()

LRESULT Gaze::OnUpdateControl(WPARAM wParam, LPARAM lParam)
{
	RedrawWindow();
	return 0;
}

void Gaze::Paint(CDC* dc)
{

	//CPaintDC dc(this); // device context for painting
	//				   // TODO: Add your message handler code here
	//				   // Do not call CStatic::OnPaint() for painting messages
	CRect rect;
	GetClientRect(&rect);

	CString szText;
	ready = (GazePX.size() > 40);

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

	SelectObject(*dc, GetStockObject(NULL_BRUSH));
	hPen = CreatePen(PS_SOLID, 1, RGB(0, 60, 0));					// dark green pen
	hPenOld = (HPEN)SelectObject(*dc, hPen);

	// plot trail history in the back
	if (ready && g_mutex.try_lock()) {

		int history = 300;
		for (auto iter = GazePX.rbegin() + 40; iter != GazePX.rend(); iter++)
		{
			Ellipse(*dc, rect.Width() / 2 - (int)(iter->x * conv) - 4,
				rect.Height() / 2 + (int)(iter->y * conv) - 4,
				rect.Width() / 2 - (int)(iter->x * conv) + 4,
				rect.Height() / 2 + (int)(iter->y * conv) + 4);
			if (history == 0) break;
			history--;
		}
		g_mutex.unlock();
	}

	DeleteObject(hPen);

	// plot axes in white
	hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));				// white pen
	hPenOld = (HPEN)SelectObject(*dc, hPen);

	// gaze x/y plot
	SetTextColor(*dc, RGB(255, 255, 255));
	szText.Format(TEXT("Pupil Offset [px]"));
	dc->TextOutW(rect.Width() / 2 - 50, 10, szText);

	CFont* pOldFont = dc->GetCurrentFont();
	CFont  newFont;

	VERIFY(newFont.CreateFont(
		22,							// nHeight
		0,							// nWidth
		0,							// nEscapement
		0,							// nOrientation
		FW_HEAVY,					// nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("Arial")));                 // lpszFacename

	dc->SelectObject(&newFont);

	szText.Format(TEXT("x:"));
	dc->TextOutW(rect.Width() / 2 - 55, rect.Height() / 10, szText);
	szText.Format(TEXT("y:"));
	dc->TextOutW(rect.Width() / 2 + 35, rect.Height() / 10, szText);

	dc->SelectObject(pOldFont);

	// plot the axes for the x/y gaze plot

	MoveToEx(*dc, rect.Width() / 2 - 144, rect.Height() / 2, NULL);		// horizontal bar
	LineTo(*dc, rect.Width() / 2 + 144, rect.Height() / 2);
	MoveToEx(*dc, rect.Width() / 2, rect.Height() / 2 - 144, NULL);		// vertical bar
	LineTo(*dc, rect.Width() / 2, rect.Height() - 97);

	// labels gaze axes with numbers
	szText.Format(TEXT("15 px"));
	dc->TextOutW(rect.Width() / 2 - 15, 80, szText);
	dc->TextOutW(rect.Width() - 55, rect.Height() / 2 - 8, szText);

	szText.Format(TEXT("-15 px"));
	dc->TextOutW(rect.Width() / 2 - 19, rect.Height() - 97, szText);
	dc->TextOutW(14, rect.Height() / 2 - 8, szText);


	DeleteObject(hPen);											// delete white pen

																// draw circles at 20, 40 and 60 deg in gray
	hPen = CreatePen(PS_SOLID, 1, RGB(100, 100, 100));			// gray pen
	hPenOld = (HPEN)SelectObject(*dc, hPen);


	int radius = 24;

	for (int i = 1; i < 7; i++)
	{
		Ellipse(*dc, rect.Width() / 2 - i*radius, rect.Height() / 2 - i*radius,
			rect.Width() / 2 + i*radius, rect.Height() / 2 + i*radius);
	}


	DeleteObject(hPen);											// delete gray pen

	szText.Format(TEXT("Current Position"));
	dc->TextOutW(35, rect.Height() - 40, szText);
	szText.Format(TEXT("x:"));
	dc->TextOutW(35, rect.Height() - 20, szText);
	szText.Format(TEXT("y:"));
	dc->TextOutW(90, rect.Height() - 20, szText);

	szText.Format(TEXT("Locked Positon"));
	dc->TextOutW(rect.Width() / 2 + 60, rect.Height() - 40, szText);
	szText.Format(TEXT("x:"));
	dc->TextOutW(rect.Width() / 2 + 60, rect.Height() - 20, szText);
	szText.Format(TEXT("y:"));
	dc->TextOutW(rect.Width() / 2 + 115, rect.Height() - 20, szText);


	// create trail palette
	CPen hPenPal[40];
	for (int i = 0; i < 39; i++) {
		hPenPal[i].CreatePen(PS_SOLID, 1, RGB(0, 60 + i * 4.875, 0));
	}

	// color the head of the snake in some yellow
	hPenPal[39].CreatePen(PS_SOLID, 1, RGB(255, 255, 100));

	if (ready && g_mutex.try_lock())
	{
		// verbose offset between locked and current pupil

		////take average of last 15 frames
		//int ave_gazePx_x(0), ave_gazePx_y(0), ave_curPos_x(0), ave_curPos_y(0);
		//int a = 0;

		//for (auto ave_rit_gaze = GazePX.rbegin(); ave_rit_gaze != GazePX.rbegin() + 15; ave_rit_gaze++) {
		//	ave_gazePx_x += (int)ave_rit_gaze->x;
		//	ave_gazePx_y += (int)ave_rit_gaze->y;
		//	a++;
		//}

		//ave_gazePx_x /= a;
		//ave_gazePx_y /= a;

		//a = 0;

		//for (auto ave_rit_pupil = Pupil.rbegin(); ave_rit_pupil != Pupil.rbegin() + 15; ave_rit_pupil++) {
		//	ave_curPos_x += (int)ave_rit_pupil->x;
		//	ave_curPos_y += (int)ave_rit_pupil->y;
		//	a++;
		//}

		//ave_curPos_x /= a;
		//ave_curPos_y /= a;

		//CClientDC dc(this);
		//CString szText;
		//CRect rect;
		//GetClientRect(&rect);

		CFont* pOldFont = dc->GetCurrentFont();

		dc->SelectObject(&newFont);

		// offset
		szText.Format(TEXT("%.1f"), -1 * offset.x);
		dc->TextOutW(rect.Width() / 2 - 35, rect.Height() / 10, szText);
		szText.Format(TEXT("%.1f"), -1 * offset.y);
		dc->TextOutW(rect.Width() / 2 + 55, rect.Height() / 10, szText);
		dc->SelectObject(pOldFont);

		// current position
		SetTextColor(*dc, RGB(0, 255, 0));
		szText.Format(TEXT("%.1f"), current.x);
		dc->TextOutW(50, rect.Height() - 20, szText);
		szText.Format(TEXT("%.1f"), current.y);
		dc->TextOutW(105, rect.Height() - 20, szText);


		// locked position
		SetTextColor(*dc, RGB(255, 0, 255));
		szText.Format(TEXT("%.1f"), locked.x);
		dc->TextOutW(rect.Width() / 2 + 75, rect.Height() - 20, szText);
		szText.Format(TEXT("%.1f"), locked.y);
		dc->TextOutW(rect.Width() / 2 + 130, rect.Height() - 20, szText);

		// draw offset into x/y plot
		for (size_t i = GazePX.size() - 40, y = 0; i < GazePX.size(); i++, y++) {
			SelectObject(*dc, hPenPal[y]);
			Ellipse(*dc, rect.Width() / 2 - (int)(GazePX[i].x * conv) - 4,
				rect.Height() / 2 + (int)(GazePX[i].y * conv) - 4,
				rect.Width() / 2 - (int)(GazePX[i].x * conv) + 4,
				rect.Height() / 2 + (int)(GazePX[i].y * conv) + 4);
		}

		g_mutex.unlock();

	}

	DeleteObject(hPen);


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
	if (ready && (offset.x <= -3.0f || offset.x >= 3.0f) | (offset.y <= -3.0f || offset.y >= 3.0f))
			hbrBkGnd.CreateSolidBrush(RGB(255, 0, 0));
	else
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

void Gaze::addPupilCenter(double x, double y)
{
	coords c{ x,y };
	Pupil.push_back(c);
}

void Gaze::addPupilDia(double x)
{
	PupilDia.push_back(x);
	//std::cerr << "pupil diameter size: " << PupilDia.size() << "\n";
}

void Gaze::addGazePX(double x, double y)
{
	Gaze::coords c{ x,y };
	GazePX.push_back(c);
	//std::cerr << "gaze in pixel size: " << GazePX.size() << "\n";
}

void Gaze::addFrozenPupil(double x, double y)
{
	coords c{ x,y };
	FrozenPupil.push_back(c);
}

void Gaze::stop() {
	GazePX.clear();
	TimeStamps.clear();
	Pupil.clear();
	PupilDia.clear();
}

void Gaze::Save()
{
	CString szFileName;
	FILE *pFile;
	CFileDialog cFileDlg(
		FALSE,
		(LPCTSTR)NULL,
		(LPCTSTR)NULL,
		OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_OVERWRITEPROMPT | OFN_CREATEPROMPT,
		_T("All Files (*.*)|*.*|"),
		NULL);
	cFileDlg.m_ofn.lpstrInitialDir = _T(".\\");

	if (cFileDlg.DoModal() == IDOK)
	{
		szFileName = cFileDlg.GetPathName();
		_wfopen_s(&pFile, szFileName, L"w");

		if (pFile)
		{
			// this saves the data to the file

			fprintf(pFile, "***************************************************************************************\n");
			fprintf(pFile, "********** Offset Tracker Data												 **********\n");
			fprintf(pFile, "***************************************************************************************\n\n");

			fprintf(pFile, "\nNOTE: offset data is in (sub)pixels\n");

			fprintf(pFile, "\n\ndata is: time [hour:min:sec:msec] frame number, pupil size [mm], pupil center [pxx], pupil center [pxy], center offset horizontal [pxx], center offset vertical [pxy] \n");

			for (size_t i = 0; i < GazePX.size(); i++) {
				fprintf(pFile, "%02d:%02d:%02d:%04d\t%d\t%2.2f\t%2.1f\t%2.1f\t%2.1f\t%2.1f\n", TimeStamps[i].wHour, TimeStamps[i].wMinute, TimeStamps[i].wSecond, TimeStamps[i].wMilliseconds, i, PupilDia[PupilDia.size() - GazePX.size() + i], Pupil[i].x, Pupil[i].y, GazePX[i].x, GazePX[i].y);
			}

			fclose(pFile);
			
			MessageBox(szFileName, _T("Data saved!"), MB_ICONINFORMATION | MB_OK | MB_DEFBUTTON1);

		}

		else
		{
			MessageBox(_T("Failed to open file!"), _T("Error"), MB_ICONHAND | MB_OK | MB_DEFBUTTON1);
		}

	}
}


BOOL Gaze::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return true;

}

void Gaze::addTimeStamp(SYSTEMTIME t) {

	TimeStamps.push_back(t);
}

void Gaze::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent && ready)
	{
		if (g_mutex.try_lock()) {
			offset.x = GazePX.back().x;
			offset.y = GazePX.back().y;
			current.x = Pupil.back().x;
			current.y = Pupil.back().y;
			locked.x = FrozenPupil.back().x;
			locked.y = FrozenPupil.back().y;
		  g_mutex.unlock();
		}
	}

	CStatic::OnTimer(nIDEvent);
}
