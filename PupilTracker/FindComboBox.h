#pragma once
// CFindComboBox

class CFindComboBox : public CComboBox
{
	DECLARE_DYNAMIC(CFindComboBox)

public:
	CFindComboBox();
	virtual ~CFindComboBox();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	DECLARE_MESSAGE_MAP()

};