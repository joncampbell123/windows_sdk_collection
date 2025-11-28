// spin.h: C++ interface to spin control
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// WinHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifndef __SPIN__H
#define __SPIN__H

#include "muscrl32.h"       // message based API

/////////////////////////////////////////////////////////////////////////////

class CSpinControl : public CWnd
{
	DECLARE_DYNAMIC(CSpinControl)

// Constructors
public:
	CSpinControl();
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

// Attributes
	CWnd* GetAssociate();
	CWnd* SetAssociate(CWnd* pNew);
	void GetRange(INT& iMin, INT& iMax);
	INT  SetRange(INT iMin, INT iMax);
	INT  GetCurrentPos();
	INT SetCurrentPos(INT iPos);
	// there are more APIs in 'muscrl32.h' not wrapped here

// Implementation
protected:
	virtual WNDPROC* GetSuperWndProcAddr();
};

/////////////////////////////////////////////////////////////////////////////
// inlines
inline CSpinControl::CSpinControl()
	{ }

inline CWnd* CSpinControl::GetAssociate()
	{ return CWnd::FromHandle((HWND)SendMessage(MSM_HWNDASSOCIATEGET)); }
inline CWnd* CSpinControl::SetAssociate(CWnd* pNew)
	{ return CWnd::FromHandle((HWND)SendMessage(MSM_HWNDASSOCIATESET, (WPARAM)pNew->GetSafeHwnd())); }
inline void CSpinControl::GetRange(INT& iMin, INT& iMax)
	{ DWORD dw = SendMessage(MSM_DWRANGEGET);
		iMin = LOWORD(dw); iMax = HIWORD(dw);
	}
inline INT CSpinControl::SetRange(INT iMin, INT iMax)
	{ return (INT)SendMessage(MSM_DWRANGESET, (WPARAM)iMin, (LPARAM)iMax); }
inline INT CSpinControl::GetCurrentPos()
	{ return (INT)SendMessage(MSM_WCURRENTPOSGET); }
inline INT CSpinControl::SetCurrentPos(INT iPos)
	{ return (INT)SendMessage(MSM_WCURRENTPOSSET, (WPARAM)iPos); }

/////////////////////////////////////////////////////////////////////////////

#endif // __SPIN__H
