// This is a part of the Microsoft Foundation Classes C++ library. 
// Copyright (C) 1992 Microsoft Corporation 
// All rights reserved. 
//  
// This source code is only intended as a supplement to the 
// Microsoft Foundation Classes Reference and Microsoft 
// QuickHelp documentation provided with the library. 
// See these sources for detailed information regarding the 
// Microsoft Foundation Classes product. 


#include "afxwin.h"
#pragma hdrstop

#ifdef AFX_CORE_SEG
#pragma code_seg(AFX_CORE_SEG)
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


// NOTE: the IMPLEMENT_DYNAMIC lines for CListBox, CComboBox, CButton
// are in WINDOW.CPP, since they are only used by Self Draw controls we reduce
// granularity by putting them there.


/////////////////////////////////////////////////////////////////////////////
// CStatic

IMPLEMENT_DYNAMIC(CStatic, CWnd)

WNDPROC* CStatic::GetSuperWndProcAddr()
{
	static WNDPROC pfnSuper;
	return &pfnSuper;
}

BOOL CStatic::Create(LPCSTR lpText, DWORD dwStyle,
		const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	return CWnd::Create("STATIC", lpText, dwStyle, rect, pParentWnd, nID);
}

/////////////////////////////////////////////////////////////////////////////
// CButton


WNDPROC* CButton::GetSuperWndProcAddr()
{
	static WNDPROC pfnSuper;
	return &pfnSuper;
}

BOOL CButton::Create(LPCSTR lpCaption, DWORD dwStyle,
		const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	return CWnd::Create("BUTTON", lpCaption, dwStyle, rect, pParentWnd, nID);
}

// Helper for radio buttons
int CWnd::GetCheckedRadioButton(int nIDFirstButton, int nIDLastButton)
{
	for (int nID = nIDFirstButton; nID <= nIDLastButton; nID++)
	{
		if (IsDlgButtonChecked(nID))
			return nID; // id that matched
	}
	return 0; // invalid ID
}

// Derived class is responsible for implementing all of these handlers
//   for owner/self draw controls
void CButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
	{ ASSERT(FALSE); }

/////////////////////////////////////////////////////////////////////////////
// CListBox


WNDPROC* CListBox::GetSuperWndProcAddr()
{
	static WNDPROC pfnSuper;
	return &pfnSuper;
}

BOOL CListBox::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,
		UINT nID)
{
	return CWnd::Create("LISTBOX", NULL, dwStyle, rect, pParentWnd, nID);
}

// Derived class is responsible for implementing these handlers
//   for owner/self draw controls (except for the optional DeleteItem)
void CListBox::DrawItem(LPDRAWITEMSTRUCT)
	{ ASSERT(FALSE); }
void CListBox::MeasureItem(LPMEASUREITEMSTRUCT)
	{ ASSERT(FALSE); }
int CListBox::CompareItem(LPCOMPAREITEMSTRUCT)
	{ ASSERT(FALSE); return 0; }
void CListBox::DeleteItem(LPDELETEITEMSTRUCT)
	{ /* default to nothing */ }

/////////////////////////////////////////////////////////////////////////////
// CComboBox


WNDPROC* CComboBox::GetSuperWndProcAddr()
{
	static WNDPROC pfnSuper;
	return &pfnSuper;
}

BOOL CComboBox::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,
		UINT nID)
{
	return CWnd::Create("COMBOBOX", NULL, dwStyle, rect, pParentWnd, nID);
}

// Derived class is responsible for implementing these handlers
//   for owner/self draw controls (except for the optional DeleteItem)
void CComboBox::DrawItem(LPDRAWITEMSTRUCT)
	{ ASSERT(FALSE); }
void CComboBox::MeasureItem(LPMEASUREITEMSTRUCT)
	{ ASSERT(FALSE); }
int CComboBox::CompareItem(LPCOMPAREITEMSTRUCT)
	{ ASSERT(FALSE); return 0; }
void CComboBox::DeleteItem(LPDELETEITEMSTRUCT)
	{ /* default to nothing */ }

/////////////////////////////////////////////////////////////////////////////
// CEdit

IMPLEMENT_DYNAMIC(CEdit, CWnd)

WNDPROC* CEdit::GetSuperWndProcAddr()
{
	static WNDPROC pfnSuper;
	return &pfnSuper;
}

BOOL CEdit::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	return CWnd::Create("EDIT", NULL, dwStyle, rect, pParentWnd, nID);
}

/////////////////////////////////////////////////////////////////////////////
// CScrollBar

IMPLEMENT_DYNAMIC(CScrollBar, CWnd)

WNDPROC* CScrollBar::GetSuperWndProcAddr()
{
	static WNDPROC pfnSuper;
	return &pfnSuper;
}

BOOL CScrollBar::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,
		UINT nID)
{
	return CWnd::Create("SCROLLBAR", NULL, dwStyle, rect, pParentWnd, nID);
}

/////////////////////////////////////////////////////////////////////////////
// Extra CWnd support for dynamic subclassing of controls

BOOL CWnd::SubclassWindow(HWND hWnd)
{
	if (!Attach(hWnd))
		return NULL;

	// now hook into the AFX WndProc
	WNDPROC* lplpfn = GetSuperWndProcAddr();
	WNDPROC oldWndProc = (WNDPROC)::SetWindowLong(hWnd, GWL_WNDPROC,
		(DWORD)(WNDPROC)AfxWndProc);
	ASSERT(oldWndProc != (WNDPROC)AfxWndProc);
	
	if (*lplpfn == NULL)
		*lplpfn = oldWndProc;   // the first edit control created
	return TRUE;
}

BOOL CWnd::SubclassDlgItem(UINT nID, CWnd* pParent)
{
	HWND hWndControl = ::GetDlgItem(pParent->m_hWnd, nID);
	if (hWndControl == NULL)
		return FALSE;
	return SubclassWindow(hWndControl);
}

/////////////////////////////////////////////////////////////////////////////
