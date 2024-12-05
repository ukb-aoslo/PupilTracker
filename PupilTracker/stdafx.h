// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#define _WIN32_WINNT _WIN32_WINNT_MAXVER
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#define WM_UPDATE_CONTROL   WM_APP + 0x10

#define UPDATE_MX_VALUE		WM_USER + 100
#define UPDATE_XOFF_VALUE	WM_USER + 101
#define UPDATE_MY_VALUE		WM_USER + 102
#define UPDATE_YOFF_VALUE	WM_USER + 103

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <tisudshl.h>		// IC Imaging Control Class Library
#include <afxdlgs.h>
#include <afxcontrolbars.h>

#include <queue>
#include <iostream>

#include <WinSock2.h>
#include <WinSock2Async.h>

#define BUFFER_SIZE 1024

// user defines
#define PI (atan(1) * 4)

// palette to support users with achromatopsia
static COLORREF white = RGB(255, 255, 255);
static COLORREF lightblue = RGB(3, 159, 200);
static COLORREF darkblue = RGB(2, 119, 181);
static COLORREF rose = RGB(249, 191, 226);
static COLORREF lightyellow = RGB(254, 255, 79);
static COLORREF lightgreen = RGB(3, 141, 99);
static COLORREF darkgreen = RGB(3, 106, 84);
static COLORREF darkyellow = RGB(252, 191, 73);
static COLORREF black = RGB(0, 0, 0);
static COLORREF darkmagenta = RGB(179, 89, 187);
static COLORREF darkazure = RGB(55, 181, 155);
static COLORREF background = RGB(15, 15, 15);
static COLORREF lightgrey = RGB(150, 150, 150);


//#ifdef _DEBUG
//#define _CRTDBG_MAP_ALLOC  
//#include <stdlib.h>  
//#include <crtdbg.h>  
//#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
//// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
//// allocations to be of _CLIENT_BLOCK type
//#else
//#define DBG_NEW new
//#endif

