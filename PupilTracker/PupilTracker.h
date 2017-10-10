// PupilTracker.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CPupilTrackerApp:
// See PupilTracker.cpp for the implementation of this class.
//

class CPupilTrackerApp : public CWinApp
{
public:
	CPupilTrackerApp();
	bool GetProductAndVersion(CString & strProductName, CString & strProductVersion);


// Overrides
	public:
	virtual BOOL InitInstance();


// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CPupilTrackerApp theApp;