#include "stdafx.h"
#include "CustomToolbar.h"
#include "resource.h"

CustomToolbar::CustomToolbar()
{
	content = new CString();
}

BEGIN_MESSAGE_MAP(CustomToolbar, CMFCToolBar)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

CustomToolbar::~CustomToolbar()
{
	delete content;
}

BOOL CustomToolbar::Customize()
{
	// TODO:  Add your specialized creation code here
	
	#define COMBO_WIDTH 85			// the width of the combobox
	#define EDIT_WIDTH 30			// width of edit boxes
	#define EDIT_HEIGHT 20			// height of edit box
	#define STATIC_WIDTH1 16		
	#define STATIC_WIDTH2 27
	#define STATIC_HEIGHT 12
	#define SPACING 5

	staticFont.CreatePointFont(85, L"DEFAULT", NULL);
	editFont.CreatePointFont(80, L"DEFAULT", NULL);

	CRect staticRect;
	CRect editRect;
	CRect tempRect1, tempRect2;
	CRect sep;

	int index = 0;

	while (GetItemID(index) != ID_TOOLBAR_END) {

		switch (GetItemID(index)) {

		case ID_BLACK_OR_WHITE:
			SetButtonInfo(index, ID_BLACK_OR_WHITE, TBBS_CHECKBOX, 11);
			break;

		case ID_FINDCOMBO:

			//next convert that button to a seperator and get its position
			SetButtonInfo(index, ID_FINDCOMBO, TBBS_SEPARATOR, COMBO_WIDTH);
			GetItemRect(index, &editRect);

			//expand the rectangle to allow the combo box room to drop down
			editRect.left += 50;
			editRect.top += SPACING;
			editRect.bottom += 200;
			editRect.right += COMBO_WIDTH;

			// then .Create the combo box and show it
			if (!BAK_ID.Create(WS_CHILD | WS_VISIBLE | CBS_AUTOHSCROLL |
				CBS_DROPDOWNLIST | CBS_HASSTRINGS,
				editRect, this, ID_FINDCOMBO))
			{
				TRACE0("Failed to create combo-box\n");
				return FALSE;
			}

			BAK_ID.ShowWindow(SW_SHOW);

			//fill the combo box
			BAK_ID.AddString(L"BAK8015");
			BAK_ID.AddString(L"BAK9019");
			BAK_ID.AddString(L"BAK8020");
			BAK_ID.AddString(L"BAK8044");
			BAK_ID.AddString(L"BAK8080");
			BAK_ID.SetCurSel(0);

			break;

		case ID_MX_STATIC:
			
			SetButtonInfo(index, ID_MX_STATIC, TBBS_SEPARATOR, NULL);
			
			GetItemRect(index, &staticRect);
			staticRect.left += COMBO_WIDTH + SPACING;
			staticRect.right = staticRect.left + STATIC_WIDTH1;
			staticRect.top = SPACING;
			staticRect.bottom = staticRect.top + STATIC_HEIGHT;

			tempRect1 = staticRect;
			
			// then .Create the Edit box and show it
			if (!mx.Create(L"MX", WS_CHILD | WS_VISIBLE, staticRect, this, ID_MX_STATIC))
			{
				TRACE0("Failed to create static\n");
				return FALSE;
			}

			mx.SetFont(&staticFont);
			mx.ShowWindow(SW_SHOW);

			break;

		case ID_MX:

			SetButtonInfo(index, ID_MX, TBBS_SEPARATOR, EDIT_WIDTH);
			editRect.top -= 2;
			editRect.left = staticRect.right + SPACING;
			editRect.right = editRect.left + EDIT_WIDTH;
			editRect.bottom = EDIT_HEIGHT;

			tempRect2 = editRect;

			// then .Create the Edit box and show it
			if (!MX.Create(WS_CHILD | WS_BORDER | WS_VISIBLE, editRect, this, ID_MX))
			{
				TRACE0("Failed to create edit-box\n");
				return FALSE;
			}

			MX.SetFont(&editFont);
			MX.ShowWindow(SW_SHOW);

			break;

		case ID_XOFF_STATIC:

			SetButtonInfo(index, ID_XOFF_STATIC, TBBS_SEPARATOR, EDIT_WIDTH);
			staticRect.left = editRect.left + SPACING + EDIT_WIDTH;
			staticRect.right = staticRect.left + STATIC_WIDTH2;

			// then .Create the Edit box and show it
			if (!xoff.Create(L"XOFF", WS_CHILD | WS_VISIBLE,
				staticRect, this, ID_XOFF_STATIC))
			{
				TRACE0("Failed to create static\n");
				return FALSE;
			}
			
			xoff.SetFont(&staticFont);
			xoff.ShowWindow(SW_SHOW);

			break;

		case ID_XOFF:

			SetButtonInfo(index, ID_XOFF, TBBS_SEPARATOR, EDIT_WIDTH);
			editRect.left = staticRect.right + SPACING;
			editRect.right = editRect.left + EDIT_WIDTH;

			// then .Create the Edit box and show it
			if (!XOFF.Create(WS_CHILD | WS_BORDER | WS_VISIBLE,	editRect, this, ID_XOFF))
			{
				TRACE0("Failed to create edit-box\n");
				return FALSE;
			}
			
			XOFF.SetFont(&editFont);
			XOFF.ShowWindow(SW_SHOW);

			break;

		case ID_MY_STATIC:

			SetButtonInfo(index, ID_MY_STATIC, TBBS_SEPARATOR, EDIT_WIDTH);

			tempRect1.top += STATIC_HEIGHT + 4;
			tempRect1.bottom += STATIC_HEIGHT + 4;
			
			// then .Create the Edit box and show it
			if (!my.Create(L"MY", WS_CHILD | WS_VISIBLE,
				tempRect1, this, ID_MY_STATIC))
			{
				TRACE0("Failed to create static\n");
				return FALSE;
			}
			
			my.SetFont(&staticFont);
			my.ShowWindow(SW_SHOW);

			break;

		case ID_MY:

			SetButtonInfo(index, ID_MY, TBBS_SEPARATOR,	EDIT_WIDTH);

			tempRect2.top += STATIC_HEIGHT + 4;
			tempRect2.bottom += STATIC_HEIGHT + 4;

			// then .Create the Edit box and show it
			if (!MY.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, tempRect2, this, ID_MY))
			{
				TRACE0("Failed to create edit-box\n");
				return FALSE;
			}

			MY.SetFont(&editFont);
			MY.ShowWindow(SW_SHOW);

			break;

		case ID_YOFF_STATIC:

			SetButtonInfo(index, ID_YOFF_STATIC, TBBS_SEPARATOR, EDIT_WIDTH);

			staticRect.top += STATIC_HEIGHT + 4;
			staticRect.bottom += STATIC_HEIGHT + 4;

			// then .Create the Edit box and show it
			if (!yoff.Create(L"YOFF", WS_CHILD | WS_VISIBLE, staticRect, this, ID_YOFF_STATIC))
			{
				TRACE0("Failed to create static\n");
				return FALSE;
			}

			yoff.SetFont(&staticFont);
			yoff.ShowWindow(SW_SHOW);

			break;

		case ID_YOFF:

			SetButtonInfo(index, ID_YOFF, TBBS_SEPARATOR, EDIT_WIDTH);

			editRect.top += STATIC_HEIGHT + 4;
			editRect.bottom += STATIC_HEIGHT + 4;

			// then .Create the Edit box and show it
			if (!YOFF.Create(WS_CHILD | WS_VISIBLE | WS_BORDER,
				editRect, this, ID_YOFF))
			{
				TRACE0("Failed to create edit-box\n");
				return FALSE;
			}

			YOFF.SetFont(&editFont);
			YOFF.ShowWindow(SW_SHOW);

			break;

		}

		index++;

	}

	return TRUE;

}

void CustomToolbar::OnFillBackground(CDC* pDC){

	CRect rect;
	GetClientRect(&rect);
	CBrush blackBrush{ RGB(15,15,15) };
	pDC->FillRect(rect, &blackBrush);

}

BOOL CustomToolbar::PreTranslateMessage(MSG* pMsg)
{

	// TODO: Add your specialized code here and/or call the base class
	CWnd* wnd = GetFocus();
	CEdit* editBox = (CEdit*)wnd;
	CComboBox* findCombo = (CComboBox*)wnd;
	
	switch (pMsg->wParam) {

	case VK_RETURN:

		switch (wnd->GetDlgCtrlID()) {


		case ID_MX:
			editBox->GetWindowTextW(*content);
			GetParent()->PostMessage(UPDATE_MX_VALUE, (WPARAM)content, 0);
			SetFocus();
			break;

		case ID_XOFF:
			editBox->GetWindowTextW(*content);
			GetParent()->PostMessage(UPDATE_XOFF_VALUE, (WPARAM)content, 0);
			SetFocus();
			break;

		case ID_MY:
			editBox->GetWindowTextW(*content);
			GetParent()->PostMessage(UPDATE_MY_VALUE, (WPARAM)content, 0);
			SetFocus();
			break;

		case ID_YOFF:
			editBox->GetWindowTextW(*content);
			GetParent()->PostMessage(UPDATE_YOFF_VALUE, (WPARAM)content, 0);
			SetFocus();
			break;

		}
	
		break;

	case VK_TAB:

		switch (wnd->GetDlgCtrlID()) {

		case ID_MX:
			editBox->GetWindowTextW(*content);
			GetParent()->PostMessage(UPDATE_MX_VALUE, (WPARAM)content, 0);
			XOFF.SetFocus();
			break;
		
		case ID_MY:
			editBox->GetWindowTextW(*content);
			GetParent()->PostMessage(UPDATE_MY_VALUE, (WPARAM)content, 0);
			YOFF.SetFocus();
			break;

		case ID_XOFF:
			editBox->GetWindowTextW(*content);
			GetParent()->PostMessage(UPDATE_XOFF_VALUE, (WPARAM)content, 0);
			MY.SetFocus();
			break;

		case ID_YOFF:
			editBox->GetWindowTextW(*content);
			GetParent()->PostMessage(UPDATE_YOFF_VALUE, (WPARAM)content, 0);
			MX.SetFocus();
			break;

		}

	}

	switch (pMsg->message) {
		
	case WM_LBUTTONUP:
		if (wnd->GetDlgCtrlID() == ID_MX || ID_MY || ID_XOFF || ID_YOFF)
			editBox->SetSel(0, 4);

		break;

	}

	return CMFCToolBar::PreTranslateMessage(pMsg);

}


HBRUSH CustomToolbar::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CMFCToolBar::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	if (pWnd->GetDlgCtrlID() == ID_MX | ID_MY | ID_XOFF | ID_YOFF )
	{
		pDC->SetBkMode(OPAQUE);
		pDC->SetTextColor(lightgrey);
		pDC->SetBkColor(background);
		
	}

	switch (nCtlColor) {

	case CTLCOLOR_STATIC:

		if (pWnd->GetDlgCtrlID() == ID_MX_STATIC | ID_MY_STATIC | ID_YOFF_STATIC | ID_XOFF_STATIC)
		
		pDC->SetTextColor(lightgrey);
		pDC->SetBkColor(background);

		break;


	case CTLCOLOR_LISTBOX:

			pDC->SetTextColor(black);
			pDC->SetBkMode(TRANSPARENT);

		break;

	}

	return hbr;

}
