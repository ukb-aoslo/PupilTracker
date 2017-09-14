// Parameters.cpp : implementation file
//

#include "stdafx.h"
#include "PupilTracker.h"
#include "Parameters.h"
#include "afxdialogex.h"
#include "PupilTrackerMainFrame.h"

// Parameters dialog

IMPLEMENT_DYNAMIC(Parameters, CDialogEx)

Parameters::Parameters(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{
	m_pPupilTracker = NULL;
	m_pParent = pParent;
	m_pPupilTracker = (CPupilTrackerMainFrame*)m_pParent;
	Schaeffel*s = (Schaeffel*)m_pPupilTracker->m_cMethod.getListener();
	thresh = &s->threshold;
	cur_thresh = s->threshold;
	opts = &s->opts;
	cur_opts = s->opts;
}

Parameters::~Parameters()
{
}

void Parameters::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

}


BEGIN_MESSAGE_MAP(Parameters, CDialogEx)
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_OK, &Parameters::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CANCEL, &Parameters::OnBnClickedCancel)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// Parameters message handlers


BOOL Parameters::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// TODO:  Add extra initialization here
	CSliderCtrl* pSlider;
	pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER1);
	pSlider->SetRange(0,255);
	pSlider->SetTicFreq(1);
	pSlider->SetPos(*thresh);
	pSlider->ModifyStyle(0, TBS_DOWNISLEFT, 0);
	SetDlgItemInt(IDE_THRESHVAL, *thresh);

	for (int i = IDC_CHECK1; i < IDC_CHECK4; i++) {
		CheckDlgButton(i, (*opts << i));
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void Parameters::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	CSliderCtrl* pSlider;
	pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER1);
	*thresh = pSlider->GetPos();
	SetDlgItemInt(IDE_THRESHVAL, *thresh);
	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}



void Parameters::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	AfxGetApp()->WriteProfileBinary(L"Settings", L"Schaeffel_thresh", (LPBYTE)thresh, 1);
	AfxGetApp()->WriteProfileBinary(L"Settings", L"Schaeffel_opts", (LPBYTE)opts, 4);
	ShowWindow(SW_HIDE);
}


void Parameters::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	m_pPupilTracker = (CPupilTrackerMainFrame*)m_pParent;
	Schaeffel*s = (Schaeffel*)m_pPupilTracker->m_cMethod.getListener();
	s->threshold = cur_thresh;
	s->opts = cur_opts;
	ShowWindow(SW_HIDE);
}


void Parameters::OnShowWindow(BOOL bShow, UINT nStatus)
{
	// TODO: Add your message handler code here
	CDialogEx::OnShowWindow(bShow, nStatus);
	m_pPupilTracker = (CPupilTrackerMainFrame*)m_pParent;
	Schaeffel*s = (Schaeffel*)m_pPupilTracker->m_cMethod.getListener();
	cur_thresh = s->threshold;
	SetDlgItemInt(IDE_THRESHVAL, *thresh);
	CSliderCtrl* pSlider;
	pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER1);
	pSlider->SetPos(*thresh);

}
