// Listener.h: interface for the CListener class.
// 
// The CListener class is derived from GrabberListener. It overwrites
// the "frameReady()" method. In the frameReady method, the method
// "saveImage()" is called.
// "saveImage()" saves the specified buffer to a BMP file and calls a "Sleep(250)" 
// to simulate time consuming image processing. "saveImage()" is also called
// by the main() function of this example to save all buffers that have
// not been processed in the frameReady method.
//
// This class also overwrites the overlayCallback method to draw a 
// frame counter.
//
// The CListener object is registered with the parameter 
// eFRAMEREADY|eOVERLAYCALLBACK . 
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LISTENER_H__3E017E1D_6B0A_472C_9F9C_0C5F9A8DFB23__INCLUDED_)
#define AFX_LISTENER_H__3E017E1D_6B0A_472C_9F9C_0C5F9A8DFB23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdlib.h>
#include <stdio.h>
#include "tisudshl.h"
#include "Pupil.h"
#include "Settings.h"

#include <iostream>
#include <stdio.h>

class CPupilTrackerMainFrame;

#define MESSAGEDEVICELOST WM_USER+90
#define MESSAGE_PUPIL_PROCESSED WM_USER+91
#define MESSAGE_PURKINJE_PROCESSED WM_USER+92
#define MESSAGE_PUPILDIA_PROCESSED WM_USER+93

using namespace DShowLib;

class CListener : public GrabberListener
{
	public:

		CListener();
		~CListener();

		int getFramesCounted() { return MediaSampleDesc.FrameNumber; };

		Pupil				pupil;				// the pupil itself
		Pupil				purkinje;			// technically not a pupil, but same storage class
		Pupil				AOSLO_beam;			// beam coming out of the AOSLO

		CPoint				purkinjePoints[4];  // assistance for TCO experiments

		BYTE*				m_pBkgndBuff;	// pointer to backgound image

		bool				record;				// are we recording?
		bool				freeze;				// is the pupil locked?
		bool				purkinje_assist;	// need purkinje assistance?
		bool				beam;				// show supposed AOSLO beam
		bool				black;				// black or white pupil tracking?
		int					Width, Height;		// video frame size	
		int					recIndex;			// where did we begin recording?
		double				purkinje_dist;		// half distance of neighboring spots for purkinje validation assist (in mm)
				
		std::vector<SYSTEMTIME>	timestamps;

	protected:

		CPupilTrackerMainFrame*	m_pParent;

		Settings*			pupil_settings;
		Settings*			purkinje_settings;

		BYTE*				opts;				// checkbox options for overlay items
		BYTE*				buf_size;			// buffer size for jitter reduction
		double*				mm_per_pix;

		SIZE				m_WindowSize;		// Size of the window in which to draw the buffer.
		tsMediaSampleDesc	MediaSampleDesc;
		SYSTEMTIME			getTimeStamp();
		
		void		DoImageProcessing(smart_ptr<MemBuffer> pBuffer);	// pre-processing
		void		frank(BYTE* pImageData, Settings* setting);			// core pupil calculation
		void		getPurkinje(BYTE* pImageData, Settings* setting);	// find purkinje inside beam
		virtual void DoFurtherProcessing(smart_ptr<MemBuffer> pBuffer);	// postprocessing, for subclasses
		virtual void overlayCallback(Grabber& caller, smart_ptr<OverlayBitmap> pBitmap,	const tsMediaSampleDesc& MediaSampleDesc);
		void		frameReady(Grabber& param, smart_ptr<MemBuffer> pBuffer, DWORD FrameNumber);
		void		deviceLost(Grabber& param);
	
	public:

		int	calcThresh();										// auto threshold calculation
		void freezePupil();
		void setBeam();
		void setSnap(bool b);
		void setParent(CWnd* pParent);
		void init(int cx, int cy);
		void SubtractBackground(BYTE* pImageData);

	private:

		CString outputfile;
		bool	snap;				// for snapshots
		
		void	makeSnapshot(smart_ptr<MemBuffer> pBuffer, DWORD FrameNumber);
		
		int *x, *y;				// some heap memory for frank

		int average_purkinje_thresh; // for automatic threshold calculation
		
};


#endif // !defined(AFX_LISTENER_H__3E017E1D_6B0A_472C_9F9C_0C5F9A8DFB23__INCLUDED_)
