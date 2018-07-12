#pragma once
#include "FindComboBox.h"
// CFindComboButton command target

class CFindComboButton : public CMFCToolBarComboBoxButton
{
	DECLARE_SERIAL(CFindComboButton)

public:

	CFindComboButton();
	virtual ~CFindComboButton();
	CFindComboButton(int uid, int image, DWORD style);
	CComboBox* CreateCombo(CWnd* pWndParent, const CRect &rect);

};
