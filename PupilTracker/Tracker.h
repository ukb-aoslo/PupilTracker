#pragma once
#include "listener.h"
#include "OffsetTracker.h"
#include "PupilDiaTracker.h"
#include "Pupil.h"
#include "Parameters.h"
#include "SockClient.h"


class Tracker : public CListener
{

public:

	Tracker();
	~Tracker();

	Pupil			median_pupil;			// reduce jitter in overlay

	bool			bufchange;				// respect live parameter change
	double			magnif;					// magnification of the video image (pixel / mm)
	
	int				display;				// counter helper variable for blinking rec status
	long int		frames;					// counting of processed frames
	
	coords<double, double> offsetMM;

protected:

	void reduceJitter();
	Pupil getMedian(Pupil p);

private:
	void DoFurtherProcessing(smart_ptr<MemBuffer> pBuffer);
	void overlayCallback(Grabber& param, smart_ptr<OverlayBitmap> pBitmap, const tsMediaSampleDesc&);
	void postOffset();
	void postDiameter();

};