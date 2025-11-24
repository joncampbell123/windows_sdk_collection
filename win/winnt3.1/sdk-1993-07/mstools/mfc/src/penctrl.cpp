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
#include "afxpen.h"

#ifdef AFX_CORE_SEG
#pragma code_seg(AFX_CORE_SEG)
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CHEdit

IMPLEMENT_DYNAMIC(CHEdit, CEdit)

WNDPROC* CHEdit::GetSuperWndProcAddr()
{
	static WNDPROC pfnSuper;
	return &pfnSuper;
}

BOOL CHEdit::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	return CWnd::Create("HEDIT", NULL, dwStyle, rect, pParentWnd, nID);
}

/////////////////////////////////////////////////////////////////////////////
// CBEdit

IMPLEMENT_DYNAMIC(CBEdit, CHEdit)

WNDPROC* CBEdit::GetSuperWndProcAddr()
{
	static WNDPROC pfnSuper;
	return &pfnSuper;
}

BOOL CBEdit::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	return CWnd::Create("BEDIT", NULL, dwStyle, rect, pParentWnd, nID);
}

/////////////////////////////////////////////////////////////////////////////
