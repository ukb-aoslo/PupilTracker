#pragma once
class CustomToolbar :
	public CMFCToolBar
{

public:
	CustomToolbar();
	~CustomToolbar();

	CFont editFont, staticFont;
	CEdit MX, XOFF, MY, YOFF;
	CStatic mx, xoff, my, yoff;
	CComboBox BAK_ID;

	CBrush m_brush;

	BOOL Customize();

private: 
	CString* content;

public:
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnFillBackground(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
