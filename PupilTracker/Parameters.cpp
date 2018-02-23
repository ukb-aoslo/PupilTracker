// Parameters.cpp : implementation file
//

#include "stdafx.h"
#include "PupilTracker.h"
#include "Parameters.h"
#include "afxdialogex.h"
#include "PupilTrackerMainFrame.h"

// Parameters dialog
extern std::mutex g_mutex;
IMPLEMENT_DYNAMIC(Parameters, CDialogEx)

Parameters::Parameters(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{
	m_pParent = pParent;
	m_pPupilTracker = (CPupilTrackerMainFrame*)m_pParent;
	s = (Schaeffel*)m_pPupilTracker->m_pListener;
	thresh = &s->threshold;
	cur_thresh = s->threshold;
	spot_size = &s->spot_size;
	cur_spot_size = s->spot_size;
	box_size = &s->box_size;
	cur_box_size = s->box_size;
	opts = &s->opts;
	cur_opts = s->opts;
	buf_size = &s->buf_size;
	cur_buf_size = s->buf_size;
}

Parameters::~Parameters()
{
}

void Parameters::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

}

BEGIN_MESSAGE_MAP(Parameters, CDialogEx)
	ON_BN_CLICKED(IDC_OK, &Parameters::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CANCEL, &Parameters::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CHECK1, &Parameters::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_CHECK2, &Parameters::OnBnClickedCheck2)
	ON_BN_CLICKED(IDC_CHECK3, &Parameters::OnBnClickedCheck3)
	ON_BN_CLICKED(IDC_CHECK4, &Parameters::OnBnClickedCheck4)
	ON_WM_SHOWWINDOW()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_EN_CHANGE(IDC_EDIT1, &Parameters::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT2, &Parameters::OnEnChangeEdit2)
	ON_BN_CLICKED(IDC_DEFAULTPARAMS, &Parameters::OnBnClickedDefaultparams)
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
	SetDlgItemInt(IDE_BUFVAL, *buf_size);
	SetDlgItemInt(IDC_EDIT1, *box_size);
	SetDlgItemInt(IDC_EDIT2, *spot_size);

	short j = 1;
	for (int i = IDC_CHECK1; i <= IDC_CHECK4; i++) {
		CheckDlgButton(i, (*opts & j));
		j=j<<1;
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
	AfxGetApp()->WriteProfileBinary(L"Settings", L"Schaeffel_opts", (LPBYTE)opts, 1);
	AfxGetApp()->WriteProfileBinary(L"Settings", L"Schaeffel_bufsize", (LPBYTE)buf_size, 1);
	AfxGetApp()->WriteProfileBinary(L"Settings", L"Schaeffel_boxsize", (LPBYTE)box_size, 1);
	AfxGetApp()->WriteProfileBinary(L"Settings", L"Schaeffel_spotsize", (LPBYTE)spot_size, 1);

	ShowWindow(SW_HIDE);

}

void Parameters::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	s->threshold = cur_thresh;
	s->opts = cur_opts;
	s->buf_size = cur_buf_size;
	s->box_size = cur_box_size;
	s->spot_size = cur_spot_size;
	ShowWindow(SW_HIDE);
}

void Parameters::OnShowWindow(BOOL bShow, UINT nStatus)
{
	// TODO: Add your message handler code here
	CDialogEx::OnShowWindow(bShow, nStatus);
	
	short j = 1;
	for (int i = IDC_CHECK1; i <= IDC_CHECK4; i++) {
		CheckDlgButton(i, (*opts & j));
		j = j << 1;
	}

	cur_thresh = s->threshold;
	cur_spot_size = s->spot_size;
	cur_box_size = s->box_size;
	SetDlgItemInt(IDE_THRESHVAL, *thresh);
	SetDlgItemInt(IDE_BUFVAL, *buf_size);
	CSliderCtrl* pSlider;
	pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER1);
	pSlider->SetPos(*thresh);
	pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER2);
	pSlider->SetPos(*buf_size);

}

void Parameters::OnBnClickedCheck1()
{
	// TODO: Add your control notification handler code here

	CButton* cBox = (CButton*)GetDlgItem(IDC_CHECK1);
	if (cBox->GetCheck()) s->opts += BoxBoundary;
	else s->opts -= BoxBoundary;

}

void Parameters::OnBnClickedCheck2()
{
	// TODO: Add your control notification handler code here
	CButton* cBox = (CButton*)GetDlgItem(IDC_CHECK2);
	if (cBox->GetCheck()) s->opts += FrameCounter;
	else s->opts -= FrameCounter;
}

void Parameters::OnBnClickedCheck3()
{
	// TODO: Add your control notification handler code here
	CButton* cBox = (CButton*)GetDlgItem(IDC_CHECK3);
	if (cBox->GetCheck()) s->opts += PupilPixels;
	else s->opts -= PupilPixels;
}

void Parameters::OnBnClickedCheck4()
{
	// TODO: Add your control notification handler code here
	CButton* cBox = (CButton*)GetDlgItem(IDC_CHECK4);
	if (cBox->GetCheck()) s->opts += AverageBrightness;
	else s->opts -= AverageBrightness;
}

void Parameters::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	CSliderCtrl* pSlider;
	pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER2);
	*buf_size = pSlider->GetPos();
	s->bufchange = true;
	SetDlgItemInt(IDE_BUFVAL, *buf_size);
	
}


void Parameters::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if (GetDlgItemInt(IDC_EDIT1) < 255)
		s->box_size = GetDlgItemInt(IDC_EDIT1);
	else SetDlgItemInt(IDC_EDIT1, cur_box_size, 0);

}


void Parameters::OnEnChangeEdit2()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if (GetDlgItemInt(IDC_EDIT2) < 100)
		s->spot_size = GetDlgItemInt(IDC_EDIT2);
	else SetDlgItemInt(IDC_EDIT2, cur_spot_size, 0);

}


void Parameters::OnBnClickedDefaultparams()
{
	// TODO: Add your control notification handler code here

	SetDlgItemInt(IDC_EDIT1, 130, 0);
	SetDlgItemInt(IDC_EDIT2, 30, 0);
	SetDlgItemInt(IDE_THRESHVAL, 180, 0);
	SetDlgItemInt(IDE_BUFVAL, 0, 0);
	s->threshold = 180;
	CSliderCtrl* pSlider;
	pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER1);
	pSlider->SetPos(180);
	pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER2);
	pSlider->SetPos(0);
	while (!g_mutex.try_lock());
	s->buf_size = 0;
	g_mutex.unlock();

}
