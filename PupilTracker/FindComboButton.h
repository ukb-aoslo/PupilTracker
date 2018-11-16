#pragma once
#include "FindComboBox.h"
// CFindComboButton command target

class CFindComboButton : public CMFCToolBarComboBoxButton
{
	DECLARE_SERIAL(CFindComboButton)

public:

	CFindComboButton() : CMFCToolBarComboBoxButton() {};

	CFindComboButton(UINT uiID,
		int iImage,
		DWORD dwStyle,
		int iWidth) : CMFCToolBarComboBoxButton(uiID, iImage, dwStyle, iWidth){};
	virtual ~CFindComboButton();

	CFindComboBox* findComboBox;
	virtual CFindComboBox* CFindComboButton::CreateCombo(CWnd* pWndParent, const CRect &rect);


	BOOL PreTranslateMessage(MSG * pMsg);

};
