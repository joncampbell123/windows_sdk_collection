// Microsoft Foundation Classes C++ library. 
// Copyright (C) 1992 Microsoft Corporation, 
// All rights reserved. 

// This source code is only intended as a supplement to the 
// Microsoft Foundation Classes Reference and Microsoft 
// QuickHelp documentation provided with the library. 
// See these sources for detailed information regarding the 
// Microsoft Foundation Classes product. 

 
#ifndef __AFXPEN_H__
#define __AFXPEN_H__

#ifndef _NTWIN

/////////////////////////////////////////////////////////////////////////////
// Classes declared in this file

	//CEdit
		class CHEdit;           // Handwriting Edit control
			class CBEdit;       // Boxed Handwriting Edit control

/////////////////////////////////////////////////////////////////////////////
// Make sure 'afxwin.h' is included first

#ifndef __AFXWIN_H__
#include "afxwin.h"
#endif

#include "penwin.h"

/////////////////////////////////////////////////////////////////////////////
// CHEdit - Handwriting Edit control

class CHEdit : public CEdit
{
	DECLARE_DYNAMIC(CHEdit)

// Constructors
public:
	CHEdit();
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

// Attributes
	// inflation between client area and writing window
	BOOL GetInflate(LPRECTOFS lpRectOfs);
	BOOL SetInflate(LPRECTOFS lpRectOfs);

	// Recognition context (lots of options here)
	BOOL GetRC(LPRC lpRC);
	BOOL SetRC(LPRC lpRC);

	// Underline mode (HEdit only)
	BOOL GetUnderline();
	BOOL SetUnderline(BOOL bUnderline = TRUE);

// Operations
	HANDLE GetInkHandle();
	BOOL SetInkMode(HPENDATA hPenDataInitial = NULL);       // start inking
	BOOL StopInkMode(UINT hep);

// Implementation
protected:
	virtual WNDPROC* GetSuperWndProcAddr();
};

/////////////////////////////////////////////////////////////////////////////
// CBEdit - Boxed Handwriting Edit control

class CBEdit : public CHEdit
{
	DECLARE_DYNAMIC(CBEdit)

// Constructors
public:
	CBEdit();
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

// Attributes
	// converting from logical to byte positions
	DWORD CharOffset(UINT nCharPosition);       // logical -> byte
	DWORD CharPosition(UINT nCharOffset);       // byte -> logical

	// BOXLAYOUT info
	void GetBoxLayout(LPBOXLAYOUT lpBoxLayout);
	BOOL SetBoxLayout(LPBOXLAYOUT lpBoxLayout);
	
// Operations
	void DefaultFont(BOOL bRepaint);            // set default font

// Implementation
private:
	BOOL GetUnderline();            // disabled in HBEdit
	BOOL SetUnderline(BOOL bUnderline); // disabled in HBEdit
protected:
	virtual WNDPROC* GetSuperWndProcAddr();
};

/////////////////////////////////////////////////////////////////////////////
// Inlines

inline CHEdit::CHEdit()
	{ }
inline BOOL CHEdit::GetInflate(LPRECTOFS lpRectOfs)
	{ return (BOOL)::SendMessage(m_hWnd, WM_HEDITCTL,
		HE_GETINFLATE, (LPARAM)lpRectOfs); }
inline HANDLE CHEdit::GetInkHandle()
	{ return (HANDLE)::SendMessage(m_hWnd, WM_HEDITCTL, 
		HE_GETINKHANDLE, 0); }
inline BOOL CHEdit::GetRC(LPRC lpRC)
	{ return (BOOL)::SendMessage(m_hWnd, WM_HEDITCTL,
		HE_GETRC, (LPARAM)lpRC); }
inline BOOL CHEdit::GetUnderline()
	{ return (BOOL)::SendMessage(m_hWnd, WM_HEDITCTL,
		HE_GETUNDERLINE, 0); }
inline BOOL CHEdit::SetInflate(LPRECTOFS lpRectOfs)
	{ return (BOOL)::SendMessage(m_hWnd, WM_HEDITCTL,
		HE_SETINFLATE, (LPARAM)lpRectOfs); }
inline BOOL CHEdit::SetInkMode(HPENDATA hPenDataInitial)
	{ return (BOOL)::SendMessage(m_hWnd, WM_HEDITCTL,
		HE_SETINKMODE, MAKELONG(hPenDataInitial, 0)); }
inline BOOL CHEdit::SetRC(LPRC lpRC)
	{ return (BOOL)::SendMessage(m_hWnd, WM_HEDITCTL,
		HE_SETRC, (LPARAM)lpRC); }
inline BOOL CHEdit::SetUnderline(BOOL bUnderline)
	{ return (BOOL)::SendMessage(m_hWnd, WM_HEDITCTL,
		HE_SETUNDERLINE, MAKELONG(bUnderline, 0)); }
inline BOOL CHEdit::StopInkMode(UINT hep)
	{ return (BOOL)::SendMessage(m_hWnd, WM_HEDITCTL,
		HE_STOPINKMODE, MAKELONG(hep, 0)); }

inline CBEdit::CBEdit()
	{ }
inline DWORD CBEdit::CharOffset(UINT nCharPosition)
	{ return (DWORD)::SendMessage(m_hWnd, WM_HEDITCTL,
		HE_CHAROFFSET, MAKELONG(nCharPosition, 0)); }
inline DWORD CBEdit::CharPosition(UINT nCharOffset)
	{ return (DWORD)::SendMessage(m_hWnd, WM_HEDITCTL,
		HE_CHARPOSITION, MAKELONG(nCharOffset, 0)); }
inline void CBEdit::GetBoxLayout(LPBOXLAYOUT lpBoxLayout)
	{ ::SendMessage(m_hWnd, WM_HEDITCTL,
		HE_GETBOXLAYOUT, (LPARAM)lpBoxLayout); }
inline void CBEdit::DefaultFont(BOOL bRepaint)
	{ ::SendMessage(m_hWnd, WM_HEDITCTL,
		HE_DEFAULTFONT, MAKELONG(bRepaint, 0)); }
inline BOOL CBEdit::SetBoxLayout(LPBOXLAYOUT lpBoxLayout)
	{ return (BOOL)::SendMessage(m_hWnd, WM_HEDITCTL,
		HE_SETBOXLAYOUT, (LPARAM)lpBoxLayout); }

//////////////////////////////////////////////////////////////////////////////

#endif //!_NTWIN

#endif //__AFXPEN_H__
