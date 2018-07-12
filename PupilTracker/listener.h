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

class CPupilTrackerMainFrame;

#define MESSAGEDEVICELOST WM_USER+90



using namespace DShowLib;

class CListener : public GrabberListener
{
	public:

		CListener();
		virtual ~CListener();
		int getFramesCounted() { return MediaSampleDesc.FrameNumber; };

		Pupil				pupil;				// the pupil itself
		Pupil				purkinje;			// technically not a pupil, but same idea of tracking


		bool				record;				// are we recording?
		bool				pupilfind;			// are we finding a pupil (true) or purkinje (false)
		bool				freeze;				// is the pupil locked?
		int					recIndex;			// where did we begin recording?
		
		std::vector<SYSTEMTIME>	timestamps;


	protected:

		Settings*			pupil_settings;
		Settings*			purkinje_settings;

		BYTE* opts;								// checkbox options for overlay items
		BYTE* buf_size;							// buffer size for jitter reduction

		CWnd*				m_pParent;
		CStatic*			m_pVideoOut;
		SIZE				m_WindowSize;		// Size of the window in which to draw the buffer.
		tsMediaSampleDesc	MediaSampleDesc;
		SYSTEMTIME			getTimeStamp();
		
		int Width, Height;						// video frame size				

		void DoImageProcessing(smart_ptr<MemBuffer> pBuffer);			// pre-frank
		void frank(BYTE* pImageData, Settings *setting);					// core pupil calculation
		virtual void DoFurtherProcessing(smart_ptr<MemBuffer> pBuffer);	// post-frank, for subclasses
		virtual void overlayCallback(Grabber& caller, smart_ptr<OverlayBitmap> pBitmap,	const tsMediaSampleDesc& MediaSampleDesc);
		void frameReady(Grabber& param, smart_ptr<MemBuffer> pBuffer, DWORD FrameNumber);
		void deviceLost(Grabber& param);
	
	public:

		void freezePupil();
		void setSnap(bool b);
		void setFreeze(bool b);
		void SetParent(CWnd* pParent);

	private:

		bool snap;				// for snapshots
		CString outputfile;
		void makeSnapshot(smart_ptr<MemBuffer> pBuffer, DWORD FrameNumber);
		
		int *x, *y;				// some heap memory for frank
		
};


#endif // !defined(AFX_LISTENER_H__3E017E1D_6B0A_472C_9F9C_0C5F9A8DFB23__INCLUDED_)
