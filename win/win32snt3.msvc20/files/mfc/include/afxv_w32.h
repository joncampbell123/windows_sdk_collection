// Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation,
// All rights reserved.

// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp and/or WinHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

// afxv_w32.h - target version/configuration control for Win32

#ifdef _WINDOWS_
	#error WINDOWS.H already included.  MFC apps must not #include <windows.h>
#endif

// STRICT is the only supported option (NOSTRICT is no longer supported)
#ifndef STRICT
#define STRICT 1
#endif

// certain parts of WINDOWS.H are necessary
#undef NOKERNEL
#undef NOGDI
#undef NOUSER
#undef NOSOUND
#undef NOCOMM
#undef NODRIVERS
#undef NOLOGERROR
#undef NOPROFILER
#undef NOMEMMGR
#undef NOLFILEIO
#undef NOOPENFILE
#undef NORESOURCE
#undef NOATOM
#undef NOLANGUAGE
#undef NOLSTRING
#undef NODBCS
#undef NOKEYBOARDINFO
#undef NOGDICAPMASKS
#undef NOCOLOR
#undef NOGDIOBJ
#undef NODRAWTEXT
#undef NOTEXTMETRIC
#undef NOSCALABLEFONT
#undef NOBITMAP
#undef NORASTEROPS
#undef NOMETAFILE
#undef NOSYSMETRICS
#undef NOSYSTEMPARAMSINFO
#undef NOMSG
#undef NOWINSTYLES
#undef NOWINOFFSETS
#undef NOSHOWWINDOW
#undef NODEFERWINDOWPOS
#undef NOVIRTUALKEYCODES
#undef NOKEYSTATES
#undef NOWH
#undef NOMENUS
#undef NOSCROLL
#undef NOCLIPBOARD
#undef NOICONS
#undef NOMB
#undef NOSYSCOMMANDS
#undef NOMDI
#undef NOCTLMGR
#undef NOWINMESSAGES

#ifndef WIN32
#define WIN32
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifdef _MAC
#define _WIN32NLS
#define _WIN32REG
#define _WLM_NOFORCE_LIBS
#endif

/////////////////////////////////////////////////////////////////////////////
// Turn off warnings for /W4
// To resume any of these warning: #pragma warning(default: 4xxx)
// which should be placed after the AFX include files

#ifndef ALL_WARNINGS
#pragma warning(disable: 4201)  // winnt.h uses nameless structs
#endif

#ifdef _UNICODE
#define UNICODE         // UNICODE is used by Windows headers
#endif

#include <windows.h>

//REVIEW_DAYTONA: HFINDFILE is new in later Win32 headers
#ifndef INVALID_HFINDFILE

typedef HANDLE HFINDFILE;
#define INVALID_HFINDFILE ((HFINDFILE)-1)

#else   //!INVALID_HFINDFILE

//REVIEW_CHICAGO: the same new headers that define INVALID_HFINDFILE,
//  do not define HTASK...

#ifndef _HTASK
DECLARE_HANDLE(HTASK);
#define _HTASK
#endif

#endif  //INVALID_HFINDFILE

#ifndef EXPORT
#define EXPORT
#endif

#include <tchar.h>      // used for ANSI v.s. UNICODE abstraction
#ifdef _MBCS
#include <mbctype.h>
#include <mbstring.h>
#endif

/////////////////////////////////////////////////////////////////////////////
// Now for the Windows API specific parts

// WM_CTLCOLOR for 16 bit API compatability
#define WM_CTLCOLOR     0x0019

// Win32 uses macros with parameters for this, which breaks C++ code.
#ifdef GetWindowTask
#undef GetWindowTask
inline HTASK GetWindowTask(HWND hWnd)
	{ return (HTASK)::GetWindowThreadProcessId(hWnd, NULL); }
#endif

// Win32 uses macros with parameters for this, which breaks C++ code.
#ifdef GetNextWindow
#undef GetNextWindow
inline HWND GetNextWindow(HWND hWnd, UINT nDirection)
	{ return ::GetWindow(hWnd, nDirection); }
#endif

// Win32 now includes lstrcpyn, but original Windows/NT didn't
#undef lstrcpyn
#define lstrcpyn afx_lstrcpyn
LPCTSTR WINAPI afx_lstrcpyn(LPTSTR, LPCTSTR, int);

// Avoid Windows NT 3.1 bug in FindResource
#undef FindResource
#define FindResource AfxFindResource
HRSRC WINAPI
AfxFindResource(HINSTANCE hInstance, LPCTSTR lpstrName, LPCTSTR lpstrType);

// Avoid Chicago mapping CToolBar::DrawState to DrawState[A/W]
#ifdef DrawState
#undef DrawState
inline BOOL WINAPI DrawState(HDC hdc, HBRUSH hbr, DRAWSTATEPROC lpOutputFunc,
	LPARAM lData, WPARAM wData, int x, int y, int cx, int cy, UINT fuFlags)
#ifdef UNICODE
	{ return ::DrawStateW(hdc, hbr, lpOutputFunc, lData, wData, x, y, cx, cy,
		fuFlags); }
#else
	{ return ::DrawStateA(hdc, hbr, lpOutputFunc, lData, wData, x, y, cx, cy,
		fuFlags); }
#endif

#endif

//REVIEW_CHICAGO: old Win32 headers #define try _try, which is not correct.
#ifdef try
#undef try
#endif

/////////////////////////////////////////////////////////////////////////////
