#pragma once


// CFindComboBox

class CFindComboBox : public CComboBox
{
	DECLARE_DYNAMIC(CFindComboBox)

public:
	CFindComboBox();
	virtual ~CFindComboBox();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

};


