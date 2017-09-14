#pragma once
#include "afxcmn.h"
class CPupilTrackerMainFrame;

// Parameters dialog

class Parameters : public CDialogEx
{
	DECLARE_DYNAMIC(Parameters)

public:

	Parameters(CWnd* pParent = NULL);   // standard constructor
	virtual ~Parameters();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

private:
	CWnd* m_pParent;
	CPupilTrackerMainFrame* m_pPupilTracker;
	BYTE* thresh;
	BYTE cur_thresh;
	DWORD* opts;
	DWORD cur_opts;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
