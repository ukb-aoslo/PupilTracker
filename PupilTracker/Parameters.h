#pragma once
//#include "afxcmn.h"
#include "Schaeffel.h"
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
	Schaeffel* s;
	BYTE* thresh;
	BYTE* spot_size;
	BYTE* box_size;
	BYTE* buf_size;

	short* opts;
	short cur_opts;
	BYTE cur_thresh;
	BYTE cur_spot_size;
	BYTE cur_buf_size;
	BYTE cur_box_size;

	CString filename, prefix;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedCheck2();
	afx_msg void OnBnClickedCheck3();
	afx_msg void OnBnClickedCheck4();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnBnClickedDefaultparams();
};
