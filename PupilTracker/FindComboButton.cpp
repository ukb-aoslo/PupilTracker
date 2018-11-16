// FindComboButton.cpp : implementation file
//

#include "stdafx.h"
#include "PupilTracker.h"
#include "FindComboButton.h"

// CFindComboButton

IMPLEMENT_SERIAL(CFindComboButton, CMFCToolBarComboBoxButton, 0)

CFindComboButton::~CFindComboButton()
{
}

CFindComboBox* CFindComboButton::CreateCombo(CWnd* pWndParent, const CRect &rect) {
	
	CFont pFont;

	VERIFY(pFont.CreateFont(
		22,							// nHeight
		0,							// nWidth
		0,							// nEscapement
		0,							// nOrientation
		FW_THIN,					// nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("Consolas")));           // lpszFacename
	
	findComboBox = new CFindComboBox();
	findComboBox->Create(CBS_DROPDOWN, rect, pWndParent, ID_EDIT_FIND_COMBO);
	return findComboBox;

}
