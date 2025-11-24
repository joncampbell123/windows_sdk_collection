// Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation,
// All rights reserved.

// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifndef WINVER
#error Please do not include this file independently, #include <afxwin.h>
#endif

// WM_CTLCOLOR for 16 bit API compatability.
#define WM_CTLCOLOR	    0x0019

// We emulate HTASK for compatibility, even though Win32 has no notion of it.
DECLARE_HANDLE(HTASK);

// Windows NT uses macros to stub these out, which breaks C++ code.
#undef GetSysModalWindow
inline HWND GetSysModalWindow(void)
	{ return NULL;}

#undef SetSysModalWindow
inline HWND SetSysModalWindow(HWND)
	{ return NULL; }

// Windows NT uses macros with parameters for these, which breaks C++ code.

#undef GetNextWindow
inline HWND GetNextWindow(HWND hWnd, UINT uFlag)
	{ return ::GetWindow(hWnd, uFlag); }

#undef MessageBox
inline int MessageBox(HWND hWnd, LPCSTR lpText, LPCSTR lpTitle, UINT fuStyle)
#ifdef UNICODE
	{ return ::MessageBoxExW(hWnd, lpText, lpTitle, fuStyle, 0); }
#else
	{ return ::MessageBoxExA(hWnd, lpText, lpTitle, fuStyle, 0); }
#endif

// These are necessary because WINDOWS.H is not included consistently
//	when the CString class is defined, but is included when the CString
//	class is implemented.

#undef LoadString
inline	int LoadString(HINSTANCE hInstance, UINT uID,
		LPSTR lpBuffer, int nBufferMax)
#ifdef UNICODE
	{ return ::LoadStringW(hInstance, uID, lpBuffer, nBufferMax); }
#else
	{ return ::LoadStringA(hInstance, uID, lpBuffer, nBufferMax); }
#endif

#undef AnsiToOem
inline BOOL AnsiToOem(LPCSTR lpcstr, LPSTR lpstr)
	{ return ::CharToOemA(lpcstr, lpstr); }

#undef OemToAnsi
inline BOOL OemToAnsi(LPCSTR lpcstr, LPSTR lpstr)
	{ return ::OemToCharA(lpcstr, lpstr); }

//////////////////////////////////////////////////////////////////////////////
// NOTE:
// The remaining declarations and definitions will not be present in
// the final product.  These are a required because of textual inconsistencies
// in the Windows NT interface files.  Please ignore these.

//REVIEW_NT: maybe someday NT will add these to their headers!

extern "C" 
{
BOOL    WINAPI SubtractRect(RECT FAR*, const RECT FAR*, const RECT FAR*);
}

/////////////////////////////////////////////////////////////////////////////
