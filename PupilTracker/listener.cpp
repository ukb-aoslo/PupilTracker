//////////////////////////////////////////////////////////////////////
// Listener.cpp: implementation of the CListener class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Listener.h"
#include "PupilTrackerMainFrame.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CListener::CListener()
{

	x = NULL;
	y = NULL;

	pupil.pixels = NULL;
	purkinje.pixels = NULL;

	snap = false;
	freeze = false;
	record = false;
	
	m_pParent = NULL;

}

//////////////////////////////////////////////////////////////////////////

CListener::~CListener() 
{

	if (pupil.pixels != NULL)
		free(pupil.pixels);
	if (purkinje.pixels != NULL)
		free(purkinje.pixels);
	if (x != NULL)
		free(x);
	if (y != NULL)
		free(y);

}

void CListener::SetParent(CWnd *pParent)
{

	m_pParent = (CPupilTrackerMainFrame*)pParent;

	pupil_settings = &m_pParent->params.pupil;
	purkinje_settings = &m_pParent->params.purkinje;
	opts = &m_pParent->params.opts;
	buf_size = &m_pParent->params.buf_size;

}


//////////////////////////////////////////////////////////////////////////
// Notify parent, device is lost.
//////////////////////////////////////////////////////////////////////////

void CListener::deviceLost( Grabber& param ) 
{

	if( m_pParent != NULL)
		m_pParent->PostMessage(MESSAGEDEVICELOST, 0, 0);

}

//////////////////////////////////////////////////////////////////////////
// Callback handler.
//////////////////////////////////////////////////////////////////////////

void CListener::frameReady( Grabber& param, smart_ptr<MemBuffer> pBuffer, DWORD FrameNumber)
{

	pBuffer->lock();
	
	// is it time to make a snapshot?
	if (snap) 
		makeSnapshot(pBuffer, FrameNumber);
	
	// go find that pupil / purkinje
	DoImageProcessing(pBuffer);

	pBuffer->unlock();

}


//////////////////////////////////////////////////////////////////////////
/*! The overlayCallback() method draws the number of the current frame. The
frame count is a member of the tsMediaSampleDesc structure that is passed
to overlayCallback() by the Grabber.*/

void CListener::overlayCallback(Grabber& caller, smart_ptr<OverlayBitmap> pBitmap,
	const tsMediaSampleDesc& MediaSampleDesc) 
{
}


void CListener::frank(BYTE* pImageData, Settings *setting) {


	double pupil_right = 0;
	int max;		// for pupil center coarse and fine

	double ave_xp_right, ave_yp_right;	// coarse pupil center
	double ave_x_right, ave_y_right;	// precision pupil center

	int n = 0;
	int i, ii;

	pupilfind ? ZeroMemory(pupil.pixels, sizeof(coords<int, int>) * (Width * Height))
		: ZeroMemory(purkinje.pixels, sizeof(coords<int, int>) * (Width * Height));

	ZeroMemory(x, sizeof(int) * (Width * Height));
	ZeroMemory(y, sizeof(int) * (Width * Height));



	/*****************************************************************************/
	/*    Core pupil/purkinje detection happens here							 */
	/*****************************************************************************/

	// step 1: find pixels (20 off from frame margin) that are brighter than average * 0.4
	// and store their coordinates in an array of x[n], y[n] declared in Listener.h

	n = 0;

	for (ii = 20; ii < Height - 20; ii = ii + 5)
	{
		for (i = 20; i < Width - 20; i = i + 5)
		{
			if (pImageData[ii * Width + i] > setting->threshold)
			{
				x[n] = i;
				y[n] = ii;
				if (n < (Width * Height))n++;
			}
		}
	}

	max = n;

	// step 2: identify those pixels marked in step 1 
	// that are surrounded by at least 'spot_size/2' bright pixels
	// in the vertical and the horizontal direction
	// then accept only those where the vertical and horiz diameter are very similar

	int count_up, count_down, count_left, count_right;

	// the box_size condition is necessary to prevent that pixels are later read outside the frame

	n = 0;
	for (i = 0; i < max; i++)
	{
		count_up = 0, count_down = 0, count_left = 0, count_right = 0;

		if (y[i] < Height - setting->box_size &&
			y[i] > setting->box_size &&
			x[i] < Width - setting->box_size &&
			x[i] > setting->box_size)
		{
			for (ii = 0; ii < setting->spot_size; ii++)
			{
				if (pImageData[y[i] * Width + x[i] + ii] > setting->threshold) count_right++;
				if (pImageData[y[i] * Width + x[i] - ii] > setting->threshold) count_left++;
				if (pImageData[(y[i] + ii) * Width + x[i]] > setting->threshold) count_up++;
				if (pImageData[(y[i] - ii) * Width + x[i]] > setting->threshold) count_down++;
			}

		}

		if (count_right > setting->spot_size / 2
			&& count_left > setting->spot_size / 2
			&& count_up > setting->spot_size / 2
			&& count_down > setting->spot_size / 2
			&& (abs((count_right + count_left) - (count_up + count_down)) < 10)
			)
		{
			coords<int, int> c{ x[i], y[i] };
			if (pupilfind)
				pupil.pixels[n] = c;
			else
				purkinje.pixels[n] = c;
			if (n < (Width * Height))
				n++;
		}

	}

	// step 3: find center of pupils roughly 
	// determine center of mass of pixels in left and right half of the frame
	// in those pixels that were found in step 2 
	// global variables, coarse coordinates of the right and left pupil centers

	ave_xp_right = 0, ave_yp_right = 0;

	int n_right = 0; // number of pixels in left and right screen whose positions are averaged

	if (pupilfind) {

		for (int i = 0; i < n; i++) {
			ave_xp_right = ave_xp_right + pupil.pixels[i].x;
			ave_yp_right = ave_yp_right + pupil.pixels[i].y;
			n_right++;
		}

	}

	else {

		for (int i = 0; i < n; i++) {
			ave_xp_right = ave_xp_right + purkinje.pixels[i].x;
			ave_yp_right = ave_yp_right + purkinje.pixels[i].y;
			n_right++;
		}
	}

	if (n_right > 0)							// only if pixels are found, the average can be calculated
	{
		ave_xp_right = ave_xp_right / n_right; // coarse coordinates 
		ave_yp_right = ave_yp_right / n_right; // of the right pupil centers
	}

	// step 4

	double pupil_dia = 2 * (sqrt(double(n_right * 25) / PI));
	int searchbox = (int)(pupil_dia + setting->spot_size * 2.2);

	ave_x_right = 0, ave_y_right = 0; // high resolution coordinates
	n_right = 0;
	pupil_right = 0;	// number of pixels whose 
						// positions are averaged (same variables as already used in step 3)
	int bright;        // pixels that are presumed to belong to the pupil
	int inPupil;		// check valid pImageData range

	if (ave_xp_right > 0)
	{
		for (ii = 0; ii < searchbox; ii++)
		{
			for (i = 0; i < searchbox; i++)
			{
				inPupil = ((int)ave_yp_right - searchbox / 2 + ii) * Width + (int)ave_xp_right - searchbox / 2 + i;
				if (inPupil > Width * Height || inPupil < 0)
					break;
				bright = pImageData[inPupil];

				if (bright > setting->threshold) // locate pixels that are brighter than threshold 
				{
					ave_x_right = ave_x_right + (ave_xp_right - (double)(searchbox / 2) + i);
					ave_y_right = ave_y_right + (ave_yp_right - (double)(searchbox / 2) + ii);
					n_right++;
				}
			}
		}
	}

	// final evaluation

	if (n_right > 0) // verify that pixels were found
	{

		if (pupilfind) { // is it a pupil or purkinje to store?

			pupil.current_center.x = ave_x_right / ((double)n_right);
			pupil.current_center.y = Height - ave_y_right / ((double)n_right);

			// pupil diameter in Pixels determined from n_right: circle area pi*r*r
			pupil.current_diameter = 2 * (sqrt(double(n_right) / PI));

		}

		else {

			purkinje.current_center.x = ave_x_right / ((double)n_right);
			purkinje.current_center.y = Height - ave_y_right / ((double)n_right);

			// pupil diameter in Pixels determined from n_right: circle area pi*r*r
			purkinje.current_diameter = 2 * (sqrt(double(n_right) / PI));

		}
	}

	else if (pupilfind) {

		pupil.current_center.x = 0;
		pupil.current_center.y = 0;
		pupil.current_diameter = 0;

	}

		else {

			purkinje.current_center.x = 0;
			purkinje.current_center.y = 0;
			purkinje.current_diameter = 0;

	}



}

void CListener::init(int cx, int cy) {
	
	Width = cx;
	Height = cy;

	if (x != NULL)
		free (x);
	if (y != NULL)
		free (y);
	if (pupil.pixels != NULL)
		free(pupil.pixels);
	if (purkinje.pixels != NULL)
		free(purkinje.pixels);

	x = (int *)malloc(sizeof(int) * (Width * Height));
	y = (int *)malloc(sizeof(int) * (Width * Height));
	pupil.pixels = (coords<int, int>*) malloc(sizeof(coords<int, int>) * (Width * Height));
	purkinje.pixels = (coords<int, int>*) malloc(sizeof(coords<int,int>) * (Width * Height));

}

void CListener::DoImageProcessing(smart_ptr<MemBuffer> pBuffer)
{

	//// Get the bitmap info header from the membuffer. It contains the bits per pixel,
	//// width and height.

	smart_ptr<BITMAPINFOHEADER> pInf = createBitmapInfoHeader(pBuffer->getFrameType());

	//// Now retrieve a pointer to the image. For organization of the image data, please
	//// refer to:
	//// http://www.imagingcontrol.com/ic/docs/html/class/Pixelformat.htm

	BYTE* pImageData = pBuffer->getPtr();

	if (pInf->biPlanes != 1) {
		AfxMessageBox(_T("Wrong color format!\nNeed 8-bit mono!"), MB_ICONERROR);
		PostQuitMessage(0);
	}

	// make negative image for black pupil tracker
	//for (int i = 0; i < iImageSize; i++)
	//{
	//	//pImageData[i] = 255 - pImageData[i];
	//	if (pImageData[i] > 133) pImageData[i] = 255;
	//}


	// calculate pupil center and diameter
	pupilfind = true;
	frank(pImageData, pupil_settings);

	// subclass processing goes here
	DoFurtherProcessing(pBuffer);
	
	// add timestamp
	timestamps.push_back(getTimeStamp());
	
	// store pupil calculation results 
	pupil.center.push_back(pupil.current_center);
	pupil.diameter.push_back(pupil.current_diameter);

	// now use frank to obtain purkinje as well
	pupilfind = false;
	frank(pImageData, purkinje_settings);

	// store purkinje calculation results
	purkinje.center.push_back(purkinje.current_center);
	purkinje.diameter.push_back(purkinje.current_diameter);
	   
}

SYSTEMTIME CListener::getTimeStamp() {

	FILETIME ft;
	SYSTEMTIME st;
	GetSystemTimeAsFileTime(&ft);
	FileTimeToSystemTime(&ft, &st);
	GetLocalTime(&st);
	return st;

}

void CListener::makeSnapshot(smart_ptr<MemBuffer> pBuffer, DWORD FrameNumber) {

	char filename[MAX_PATH];
	sprintf_s(filename, "image%02i.bmp", FrameNumber);
	saveToFileBMP(*pBuffer, filename);
	snap = false;

}

void CListener::DoFurtherProcessing(smart_ptr<MemBuffer> pBuffer) {

	// whatever calculations may be necessary in certain subclasses
	Sleep (10);

}

void CListener::freezePupil() {

	// toggle between normal and frozen state
	
	freeze ? freeze = false : freeze = true;
	
	if (freeze){

			pupil.frozen_center.x = pupil.current_center.x;
			pupil.frozen_center.y = pupil.current_center.y;
			pupil.frozen_diameter = pupil.current_diameter;

			if (m_pParent != NULL)
				m_pParent->PostMessage(MESSAGE_OFFSET_LOCKEDPOS, 0, reinterpret_cast<WPARAM>(&pupil.frozen_center));

	}

}

void CListener::setSnap(bool b) {

	snap = b;

}

void CListener::setBeam() {

	AOSLO_beam.current_center = pupil.current_center;
	AOSLO_beam.current_diameter = pupil.current_diameter;

}
