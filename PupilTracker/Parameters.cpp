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
	
	opts = 0x0f;
	buf_size = 0;

	UINT n;
	LPBYTE ppData;

	// load overlay options from registry
	
	if (AfxGetApp()->GetProfileBinary(L"Settings", L"Schaeffel_opts", &ppData, &n)) {
		opts = (BYTE)ppData[0];
		delete ppData;
	}

	if (AfxGetApp()->GetProfileBinary(L"Settings", L"Schaeffel_bufsize", &ppData, &n)) {
		buf_size = (BYTE)ppData[0];
		delete ppData;
	}

	// load pupil settings from registry

	if (AfxGetApp()->GetProfileBinary(L"Settings", L"pupil_spotsize", &ppData, &n)) {
		pupil.spot_size = (BYTE)ppData[0];
		delete ppData;
	}
	
	if (AfxGetApp()->GetProfileBinary(L"Settings", L"pupil_boxsize", &ppData, &n)) {
		pupil.box_size = (BYTE)ppData[0];
		delete ppData;
	}

	if (AfxGetApp()->GetProfileBinary(L"Settings", L"pupil_thresh", &ppData, &n)) {
		pupil.threshold = (BYTE)ppData[0];
		delete ppData;
	}

	// load purkinje settings from registry

	if (AfxGetApp()->GetProfileBinary(L"Settings", L"purkinje_spotsize", &ppData, &n)) {
		purkinje.spot_size = (BYTE)ppData[0];
		delete ppData;
	}

	if (AfxGetApp()->GetProfileBinary(L"Settings", L"purkinje_boxsize", &ppData, &n)) {
		purkinje.box_size = (BYTE)ppData[0];
		delete ppData;
	}

	if (AfxGetApp()->GetProfileBinary(L"Settings", L"purkinje_thresh", &ppData, &n)) {
		purkinje.threshold = (BYTE)ppData[0];
		delete ppData;
	}

}

Parameters::~Parameters()

{
}


// Parameters message handlers
BEGIN_MESSAGE_MAP(Parameters, CDialogEx)
	ON_BN_CLICKED(IDC_OK, &Parameters::OnBnClickedOk)
	ON_WM_SHOWWINDOW()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_DEFAULTPARAMS, &Parameters::OnBnClickedDefaultparams)
	ON_COMMAND(IDC_CHECK1, &Parameters::OnCheck1)
	ON_COMMAND(IDC_CHECK2, &Parameters::OnCheck2)
	ON_COMMAND(IDC_CHECK3, &Parameters::OnCheck3)
	ON_COMMAND(IDC_CHECK4, &Parameters::OnCheck4)
END_MESSAGE_MAP()

void Parameters::DoDataExchange(CDataExchange* pDX)
{

	CDialogEx::DoDataExchange(pDX);
	
	DDX_Text(pDX, IDC_EDIT1, pupil.box_size);
	DDX_Text(pDX, IDC_EDIT2, pupil.spot_size);
	DDX_Text(pDX, IDE_THRESHVAL, pupil.threshold);
	DDX_Text(pDX, IDE_THRESHVAL2, purkinje.threshold);
	DDX_Text(pDX, IDC_EDIT3, purkinje.box_size);
	DDX_Text(pDX, IDC_EDIT4, purkinje.spot_size);
	DDX_Text(pDX, IDE_BUFVAL, buf_size);
	DDV_MinMaxByte(pDX, pupil.box_size, 10, 255);
	DDV_MinMaxByte(pDX, pupil.spot_size, 1, 100);
	DDV_MinMaxByte(pDX, purkinje.box_size, 10, 255);
	DDV_MinMaxByte(pDX, purkinje.spot_size, 1, 100);
	DDV_MinMaxByte(pDX, buf_size, 0, 100);

}



BOOL Parameters::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// TODO:  Add extra initialization here

	CSliderCtrl* pSlider;
	pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER1);
	pSlider->SetRange(0,255);
	pSlider->SetTicFreq(1);
	pSlider->SetPos(pupil.threshold);
	pSlider->ModifyStyle(0, TBS_DOWNISLEFT, 0);
	pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER3);
	pSlider->SetRange(0, 255);
	pSlider->SetTicFreq(1);
	pSlider->SetPos(purkinje.threshold);
	pSlider->ModifyStyle(0, TBS_DOWNISLEFT, 0);


	short j = 1;
	for (int i = IDC_CHECK1; i <= IDC_CHECK4; i++) {
		CheckDlgButton(i, (opts & j));
		j <<= 1;
		
	}


	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void Parameters::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	CSliderCtrl* pSlider = (CSliderCtrl*)pScrollBar;
	//pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER1);
	if (pSlider->GetDlgCtrlID() == IDC_SLIDER1) {
		pupil.threshold = pSlider->GetPos();
		SetDlgItemInt(IDE_THRESHVAL, pupil.threshold);
	}
	if (pSlider->GetDlgCtrlID() == IDC_SLIDER3) {
		purkinje.threshold = pSlider->GetPos();
		SetDlgItemInt(IDE_THRESHVAL2, purkinje.threshold);
	}

	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);

}

void Parameters::OnBnClickedOk()
{
	CWnd* pwndCtrl = GetFocus();
	CWnd* pwndCtrlNext = pwndCtrl;
	CSliderCtrl* pSlider;
	int ctrl_ID = pwndCtrl->GetDlgCtrlID();

	switch (ctrl_ID) {
	case IDE_THRESHVAL:
		pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER1);
		UpdateData(TRUE);
		pSlider->SetPos(pupil.threshold);
		break;
	case IDE_THRESHVAL2:
		pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER3);
		UpdateData(TRUE);
		pSlider->SetPos(purkinje.threshold);
		break;

	case IDC_EDIT1:
		//pwndCtrlNext = GetDlgItem(IDC_EDIT2);
		break;
	case IDC_EDIT2:
		//pwndCtrlNext = GetDlgItem(IDC_EDIT3);
		break;
	case IDC_EDIT3:
		//pwndCtrlNext = GetDlgItem(IDC_EDIT4);
		break;
	case IDC_EDIT4:
		//pwndCtrlNext = GetDlgItem(IDC_EDIT1);
		break;
	case IDC_OK:
		CDialogEx::OnOK();
		
		AfxGetApp()->WriteProfileBinary(L"Settings", L"pupil_thresh", &pupil.threshold, 1);
		AfxGetApp()->WriteProfileBinary(L"Settings", L"pupil_spotsize", &pupil.spot_size, 1);
		AfxGetApp()->WriteProfileBinary(L"Settings", L"pupil_boxsize", &pupil.box_size, 1);

		AfxGetApp()->WriteProfileBinary(L"Settings", L"purkinje_thresh", &purkinje.threshold, 1);
		AfxGetApp()->WriteProfileBinary(L"Settings", L"purkinje_spotsize", &purkinje.spot_size, 1);
		AfxGetApp()->WriteProfileBinary(L"Settings", L"purkinje_boxsize", &purkinje.box_size, 1);

		AfxGetApp()->WriteProfileBinary(L"Settings", L"Schaeffel_opts", &opts, 1);
		AfxGetApp()->WriteProfileBinary(L"Settings", L"Schaeffel_bufsize", &buf_size, 1);

		//ShowWindow(SW_HIDE);
		break;

	default:
		break;
	}
	//pwndCtrlNext->SetFocus();
	UpdateData(TRUE);

}


void Parameters::OnShowWindow(BOOL bShow, UINT nStatus)
{
	// TODO: Add your message handler code here
	CDialogEx::OnShowWindow(bShow, nStatus);

	CSliderCtrl* pSlider;
	pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER1);
	pSlider->SetPos(pupil.threshold);
	pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER3);
	pSlider->SetPos(purkinje.threshold);
	pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER2);
	pSlider->SetPos(buf_size);

}


void Parameters::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	CSliderCtrl* pSlider;
	pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER2);
	buf_size = pSlider->GetPos();
	SetDlgItemInt(IDE_BUFVAL, buf_size);
	
}



void Parameters::OnBnClickedDefaultparams()
{
	// TODO: Add your control notification handler code here

	pupil.threshold = 100;
	pupil.spot_size = 30;
	pupil.box_size = 10;

	purkinje.threshold = 250;
	purkinje.spot_size = 4;
	purkinje.box_size = 100;

	SetDlgItemInt(IDC_EDIT1, pupil.box_size, 0);
	SetDlgItemInt(IDC_EDIT2, pupil.spot_size, 0);
	SetDlgItemInt(IDC_EDIT3, purkinje.box_size, 0);
	SetDlgItemInt(IDC_EDIT4, purkinje.spot_size, 0);

	SetDlgItemInt(IDE_THRESHVAL, pupil.threshold, 0);
	SetDlgItemInt(IDE_THRESHVAL2, purkinje.threshold, 0);
	SetDlgItemInt(IDE_BUFVAL, 0, 0);

	CSliderCtrl* pSlider;
	pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER1);
	pSlider->SetPos(pupil.threshold);
	pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER2);
	pSlider->SetPos(0);
	pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER3);
	pSlider->SetPos(purkinje.threshold);

}



void Parameters::OnCheck1()
{
	// TODO: Add your command handler code here

	CButton* cBox = (CButton*)GetDlgItem(IDC_CHECK1);
	if (cBox->GetCheck()) opts += BoxBoundary;
	else opts -= BoxBoundary;

}


void Parameters::OnCheck2()
{
	// TODO: Add your command handler code here
	CButton* cBox = (CButton*)GetDlgItem(IDC_CHECK2);
	if (cBox->GetCheck()) opts += FrameCounter;
	else opts -= FrameCounter;

}


void Parameters::OnCheck3()
{
	// TODO: Add your command handler code here
	CButton* cBox = (CButton*)GetDlgItem(IDC_CHECK3);
	if (cBox->GetCheck()) opts += PupilPixels;
	else opts -= PupilPixels;
}


void Parameters::OnCheck4()
{
	// TODO: Add your command handler code here
	CButton* cBox = (CButton*)GetDlgItem(IDC_CHECK4);
	if (cBox->GetCheck()) opts += AverageBrightness;
	else opts -= AverageBrightness;
}
