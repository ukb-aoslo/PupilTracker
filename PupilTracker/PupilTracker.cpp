// PupilTracker.cpp : Implements the classes for the application.
//
#include "stdafx.h"
#include "PupilTracker.h"
#include "PupilTrackerMainFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CPupilTrackerApp

// for global thread synchronization (necessary because of listener callback-functions)
std::mutex g_mutex;

BEGIN_MESSAGE_MAP(CPupilTrackerApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CPupilTrackerApp construction

CPupilTrackerApp::CPupilTrackerApp()
{
}


// The one and only CPupilTrackerApp object

CPupilTrackerApp theApp;

// CPupilTrackerApp initialization

BOOL CPupilTrackerApp::InitInstance()
{
	// INDISPENSABLE DEBUG CODE FOR CLEARING MEMORY LEAKS
//#ifdef _DEBUG
//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//	_CrtSetBreakAlloc(452);
//#endif
//

	// Init IC Imaging Control 

	if( !DShowLib::InitLibrary() )
	{
		AfxMessageBox( TEXT("The IC Imaging Control Class Library could not be initialized.\n(invalid license key?)") );
		exit( 1 );
	}
	// At the end of the program, the IC Imaging Control Class Library must be cleaned up
	// by a call to ExitLibrary().
	atexit(DShowLib::ExitLibrary );

	// InitCommonControls() is required on Windows XP, if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("AG Harmening"));

	
	CPupilTrackerMainFrame* pFrame = new CPupilTrackerMainFrame();
	m_pMainWnd = pFrame;
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();

	return TRUE;
}

bool CPupilTrackerApp::GetProductAndVersion(CString & strProductName, CString & strProductVersion)
{
	// get the filename of the executable containing the version resource
	TCHAR szFilename[MAX_PATH + 1] = { 0 };
	if (GetModuleFileName(NULL, szFilename, MAX_PATH) == 0)
	{
		TRACE("GetModuleFileName failed with error %d\n", GetLastError());
		return false;
	}

	// allocate a block of memory for the version info
	DWORD dummy;
	DWORD dwSize = GetFileVersionInfoSize(szFilename, &dummy);
	if (dwSize == 0)
	{
		TRACE("GetFileVersionInfoSize failed with error %d\n", GetLastError());
		return false;

	}
	std::vector<BYTE> data(dwSize);

	// load the version info
	if (!GetFileVersionInfo(szFilename, NULL, dwSize, &data[0]))
	{
		TRACE("GetFileVersionInfo failed with error %d\n", GetLastError());
		return false;
	}

	// get the name and version strings
	LPVOID pvProductName = NULL;
	unsigned int iProductNameLen = 0;
	LPVOID pvProductVersion = NULL;
	unsigned int iProductVersionLen = 0;

	// replace "040904e4" with the language ID of your resources
	if (!VerQueryValue(&data[0], _T("\\StringFileInfo\\040904e4\\ProductName"), &pvProductName, &iProductNameLen) ||
		!VerQueryValue(&data[0], _T("\\StringFileInfo\\040904e4\\ProductVersion"), &pvProductVersion, &iProductVersionLen))
	{
		TRACE("Can't obtain ProductName and ProductVersion from resources\n");
		return false;
	}

	strProductName.SetString((LPCWSTR)pvProductName, iProductNameLen);
	strProductVersion.SetString((LPCWSTR)pvProductVersion, iProductVersionLen);

	return true;
}
