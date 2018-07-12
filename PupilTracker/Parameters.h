#pragma once
#include "Tracker.h"
#include "Settings.h"

class CPupilTrackerMainFrame;

// Parameters dialog

enum OverlayOpts {

	BoxBoundary = 1 << 0,
	FrameCounter = 1 << 1,
	PupilPixels = 1 << 2,
	AverageBrightness = 1 << 3

};
class Tracker;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

	class Parameters : public CDialogEx
	{
		DECLARE_DYNAMIC(Parameters)

	public:

		Parameters(CWnd* pParent = NULL);   // standard constructor
		virtual ~Parameters();

		Settings pupil { 75, 30, 10 };
		Settings purkinje { 250, 4, 100 };

		BYTE buf_size;
		BYTE opts;

	private:

		CWnd* m_pParent;

		Settings cur_pupil;
		Settings cur_purkinje;


	CString filename, prefix;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedDefaultparams();

	afx_msg void OnCheck1();
	afx_msg void OnCheck2();
	afx_msg void OnCheck3();
	afx_msg void OnCheck4();
	};
