// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp and/or WinHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include <malloc.h>

#ifdef AFX_INIT_SEG
#pragma code_seg(AFX_INIT_SEG)
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// System metrics etc
/////////////////////////////////////////////////////////////////////////////

AFX_DATADEF AUX_DATA afxData;

// Initialization code
AUX_DATA::AUX_DATA()
{
	HDC hDCScreen = GetDC(NULL);
	ASSERT(hDCScreen != NULL);

	// System metrics
	cxVScroll = GetSystemMetrics(SM_CXVSCROLL);
	cyHScroll = GetSystemMetrics(SM_CYHSCROLL);
	cxIcon = GetSystemMetrics(SM_CXICON);
	cyIcon = GetSystemMetrics(SM_CYICON);

	// Device metrics for screen
	cxPixelsPerInch = GetDeviceCaps(hDCScreen, LOGPIXELSX);
	cyPixelsPerInch = GetDeviceCaps(hDCScreen, LOGPIXELSY);
	SIZE size;
	VERIFY(GetTextExtentPointA(hDCScreen, "M", 1, &size));
	cySysFont = size.cy;
	ReleaseDC(NULL, hDCScreen);

	// Border attributes
	hbrLtGray = ::CreateSolidBrush(RGB(192, 192, 192));
	hbrDkGray = ::CreateSolidBrush(RGB(128, 128, 128));
	ASSERT(hbrLtGray != NULL);
	ASSERT(hbrDkGray != NULL);

	// Cached system values (updated in CFrameWnd::OnSysColorChange)
	hbrBtnFace = NULL;
	hbrBtnShadow = NULL;
	hbrBtnHilite = NULL;
	hbrWindowFrame = NULL;
	hpenBtnShadow = NULL;
	hpenBtnHilite = NULL;
	hpenBtnText = NULL;
	UpdateSysColors();

	// Standard cursors
	hcurWait = ::LoadCursor(NULL, IDC_WAIT);
	hcurArrow = ::LoadCursor(NULL, IDC_ARROW);
	ASSERT(hcurWait != NULL);
	ASSERT(hcurArrow != NULL);
	hcurHelp = NULL;    // loaded on demand

	// hi-bit of HIWORD is set if the platform is Win32s
	DWORD dwVersion = ::GetVersion();
	nWinVer = (LOBYTE(dwVersion) << 8) + HIBYTE(dwVersion);
	bWin32s = (dwVersion & 0x80000000) != 0;
	bWin4 = (BYTE)dwVersion >= 4;
	bNotWin4 = 1 - bWin4;   // for convenience
#ifndef _MAC
	bSmCaption = bWin4;
#else
	bSmCaption = TRUE;
#endif
	bWin31 = bWin32s && !bWin4; // Chicago reports Win32s

	// cxBorder2 and cyBorder are 2x borders for Win4
	cxBorder2 = bWin4 ? CX_BORDER*2 : CX_BORDER;
	cyBorder2 = bWin4 ? CY_BORDER*2 : CY_BORDER;

	// allocated on demand
	hStatusFont = NULL;
	hToolTipsFont = NULL;
	hbmMenuDot = NULL;
	hcurHelp = NULL;

#ifndef _MAC
	// load special Windows API entry points
	if (bWin4)
	{
		// need some APIs from USER32.DLL
		HMODULE hMod = GetModuleHandleA("USER32.DLL");
		ASSERT(hMod != NULL);

		// the following APIs will be used only if present
		(FARPROC&)pfnSetScrollInfo = GetProcAddress(hMod, "SetScrollInfo");
		(FARPROC&)pfnGetScrollInfo = GetProcAddress(hMod, "GetScrollInfo");
	}
#endif
}

// Call this API in your InitInstance if your applications is marked
// as a Windows 4.0 application.
// This is done by linking with /subsystem:windows,4.0.
void AFXAPI AfxEnableWin40Compatibility()
{
	if (afxData.bWin4)
	{
		// Later versions of Windows report "correct" scrollbar metrics
		// MFC assumes the old metrics, so they need to be adjusted.
		afxData.cxVScroll = GetSystemMetrics(SM_CXVSCROLL) + CX_BORDER;
		afxData.cyHScroll = GetSystemMetrics(SM_CYHSCROLL) + CY_BORDER;
	}
}

#ifdef AFX_TERM_SEG
#pragma code_seg(AFX_TERM_SEG)
#endif

// Termination code
AUX_DATA::~AUX_DATA()
{
	// cleanup standard brushes
	AfxDeleteObject((HGDIOBJ*)&hbrLtGray);
	AfxDeleteObject((HGDIOBJ*)&hbrDkGray);
	AfxDeleteObject((HGDIOBJ*)&hbrBtnFace);
	AfxDeleteObject((HGDIOBJ*)&hbrBtnShadow);
	AfxDeleteObject((HGDIOBJ*)&hbrBtnHilite);
	AfxDeleteObject((HGDIOBJ*)&hbrWindowFrame);

	// cleanup standard pens
	AfxDeleteObject((HGDIOBJ*)&hpenBtnShadow);
	AfxDeleteObject((HGDIOBJ*)&hpenBtnHilite);
	AfxDeleteObject((HGDIOBJ*)&hpenBtnText);

	// clean up objects we don't actually create
	AfxDeleteObject((HGDIOBJ*)&hStatusFont);
	AfxDeleteObject((HGDIOBJ*)&hToolTipsFont);
	AfxDeleteObject((HGDIOBJ*)&hbmMenuDot);
}

#ifdef AFX_CORE1_SEG
#pragma code_seg(AFX_CORE1_SEG)
#endif

void AUX_DATA::UpdateSysColors()
{
	clrBtnFace = ::GetSysColor(COLOR_BTNFACE);
	clrBtnShadow = ::GetSysColor(COLOR_BTNSHADOW);
	clrBtnHilite = ::GetSysColor(COLOR_BTNHIGHLIGHT);
	clrBtnText = ::GetSysColor(COLOR_BTNTEXT);
	clrWindowFrame = ::GetSysColor(COLOR_WINDOWFRAME);

	AfxDeleteObject((HGDIOBJ*)&hbrBtnFace);
	AfxDeleteObject((HGDIOBJ*)&hbrBtnShadow);
	AfxDeleteObject((HGDIOBJ*)&hbrBtnHilite);
	AfxDeleteObject((HGDIOBJ*)&hbrWindowFrame);

	hbrBtnFace = ::CreateSolidBrush(clrBtnFace);
	ASSERT(hbrBtnFace != NULL);
	hbrBtnShadow = ::CreateSolidBrush(clrBtnShadow);
	ASSERT(hbrBtnShadow != NULL);
	hbrBtnHilite = ::CreateSolidBrush(clrBtnHilite);
	ASSERT(hbrBtnHilite != NULL);
	hbrWindowFrame = ::CreateSolidBrush(clrWindowFrame);
	ASSERT(hbrWindowFrame != NULL);

	AfxDeleteObject((HGDIOBJ*)&hpenBtnShadow);
	AfxDeleteObject((HGDIOBJ*)&hpenBtnHilite);
	AfxDeleteObject((HGDIOBJ*)&hpenBtnText);

	hpenBtnShadow = ::CreatePen(PS_SOLID, 0, clrBtnShadow);
	ASSERT(hpenBtnShadow != NULL);
	hpenBtnHilite = ::CreatePen(PS_SOLID, 0, clrBtnHilite);
	ASSERT(hpenBtnHilite != NULL);
	hpenBtnText = ::CreatePen(PS_SOLID, 0, clrBtnText);
	ASSERT(hpenBtnText != NULL);
}

/////////////////////////////////////////////////////////////////////////////
// other global data

#ifdef AFX_INIT_SEG
#pragma code_seg(AFX_INIT_SEG)
#endif

AFX_DATADEF AFX_CRITICAL_SECTION _afxCriticalSection;

/////////////////////////////////////////////////////////////////////////////
