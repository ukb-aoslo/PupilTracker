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
	void reduceJitter();
	double getMedian(std::deque<double> val);

	SIZE dim;							// video size

	short opts;							// checkbox options for overlay items
	BYTE buf_size;						// buffer size for jitter reduction

	double m_ave_bright;					
	double pupil_right;
	
	std::deque<double>pupil_rightBuf;	// buffers for jitter reduction in overlay
	double pBuf;						// median of pupil buffer
	bool bufchange;						// take into account live parameter change

	int m_iBitmapWidth;
	int m_iBitmapHeight;

	double magnif;						// magnification of the video image (pixel/mm)
	BYTE box_size;

	BYTE spot_size;						// minimal size of spot to be accepted
	BYTE threshold;						// detection threshold
	
	long int frames;					// count of processed frames

	int Width, Height;					// video frame size
	
	int pixel[1024];					// to read pixels at certain places for ave_brightness

	int x[10000], y[10000], max;		// to mark pixels brighter than factor*m_ave_bright
	int xp[1000], yp[1000], xyp;		// to find pixels in a pupil
	
	// for pupil center coarse and fine
	double ave_xp_right, ave_yp_right;	// coarse pupil centers
	double ave_x_right, ave_y_right;	// precision pupil centers

	std::deque<double>ave_x_rightBuf;	// buffers for jitter reduction in overlay
	std::deque<double>ave_y_rightBuf;
	double xBuf;						// median of ave_x_right buffer
	double yBuf;						// median of ave_y_right buffer

	// freeze stuff
	bool frozen;
	double ave_x_right_fr, ave_y_right_fr;
	double pupil_right_fr;


protected:
	
	void DoImageProcessing(smart_ptr<MemBuffer> pBuffer);
	void frameReady(Grabber& param, smart_ptr<MemBuffer> pBuffer, DWORD FrameNumber);
	virtual void overlayCallback(Grabber& param, smart_ptr<OverlayBitmap> pBitmap, const tsMediaSampleDesc&);

};