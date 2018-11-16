// FindComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "PupilTracker.h"
#include "FindComboBox.h"


// CFindComboBox

IMPLEMENT_DYNAMIC(CFindComboBox, CComboBox)

CFindComboBox::CFindComboBox()
{
}

CFindComboBox::~CFindComboBox()
{
}

BEGIN_MESSAGE_MAP(CFindComboBox, CComboBox)
END_MESSAGE_MAP()

// CFindComboBox message handlers

BOOL CFindComboBox::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam) {

		case VK_ESCAPE:
			AfxGetApp()->GetMainWnd()->SetFocus();
			break;
		case VK_RETURN:
			//CFindComboButton* button = (CFindComboButton*)GetDlgItem(ID_EDIT_FIND);
			//CString test = button->GetText();
			AfxGetApp()->GetMainWnd()->PostMessageW(WM_COMMAND, ID_EDIT_FIND_COMBO);
			break;

		}

	}
	
	/*return CFindComboButton::PreTranslateMessage(pMsg);*/
	return true;
	
}

