#pragma once
#include "listener.h"
#include "Gaze.h"
#include "Graph.h"

	enum OverlayOpts {
		BoxBoundary =  1 << 0,
		FrameCounter = 1 << 1,
		PupilPixels = 1 << 2,
		AverageBrightness = 1 << 3,
	};

class Schaeffel : public CListener
{
public:
	Schaeffel(Gaze* gz, Graph* gr);
	~Schaeffel();

	Gaze* m_pGaze;
	Graph* m_pGraph;
	void freezePupil();

	SIZE dim;							// video size

	DWORD opts;

	double m_ave_bright;					
	double pupil_right;
	
	int m_iBitmapWidth;
	int m_iBitmapHeight;

	float magnif;						// magnification of the video image (pixel/mm)
	int box_size;

	BYTE threshold;
	
	int count_old;						// count of processed frames

	int Width, Height;					// video frame size
	
	int pixel[1024];					// to read pixels at certain places for ave_brightness

	int x[10000], y[10000], max;		// to mark pixels brighter than factor*m_ave_bright
	int xp[1000], yp[1000], xyp;		// to find pixels in a pupil
	
	// for pupil center coarse and fine
	float ave_xp_right, ave_yp_right;	// coarse pupil centers
	float ave_x_right, ave_y_right;		// precision pupil centers

	// gaze stuff
	float x_gaze_r, y_gaze_r;			// final float gaze in pixels

	// freeze stuff
	bool frozen;
	float ave_x_right_fr, ave_y_right_fr;
	float pupil_right_fr;


protected:
	
	void DoImageProcessing(smart_ptr<MemBuffer> pBuffer);
	void frameReady(Grabber& param, smart_ptr<MemBuffer> pBuffer, DWORD FrameNumber);
	virtual void overlayCallback(Grabber& param, smart_ptr<OverlayBitmap> pBitmap, const tsMediaSampleDesc&);

};