// paredit2.cpp : code needed to export CParsedEdit as a WndClass
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "ctrltest.h"

#include "paredit.h"

/////////////////////////////////////////////////////////////////////////////
// The C++ class CParsedEdit can be made visible to the dialog manager
//   by registering a window class for it
// The C++ class 'CParsedEditExported' is used to implement the
//   creation and destruction of a C++ object as if it were just
//   a normal Windows control.
// In order to hook in the class creation we must provide a special
//   WndProc to create the C++ object and override the PostNcDestroy
//   message to destroy it

class CParsedEditExported : public CParsedEdit      // WNDCLASS exported class
{
public:
	CParsedEditExported(HWND hWnd)
		{ VERIFY(Attach(hWnd)); }

// Implementation: (all is implementation since the public interface of
//    this class is identical to CParsedEdit)
protected:
	virtual WNDPROC* GetSuperWndProcAddr();
	static WNDPROC lpfnSuperWndProc;
	afx_msg int OnNcCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void PostNcDestroy();
	static LONG FAR PASCAL __export WndProcHook(HWND, UINT, UINT, LONG);
	DECLARE_MESSAGE_MAP();

	friend class CParsedEdit;       // for RegisterControlClass
};

/////////////////////////////////////////////////////////////////////////////
// Special create hooks

LONG FAR PASCAL __export
CParsedEditExported::WndProcHook(HWND hWnd, UINT msg, UINT wParam, LONG lParam)
{
	// create new item and attach it
	CParsedEditExported* pEdit = new CParsedEditExported(hWnd);

	// set up wndproc to AFX one, and call it
	::SetWindowLong(hWnd, GWL_WNDPROC, (DWORD)AfxWndProc);
#ifdef STRICT
	return ::CallWindowProc(AfxWndProc, hWnd, msg, wParam, lParam);
#else
	return ::CallWindowProc((FARPROC)AfxWndProc, hWnd, msg, wParam, lParam);
#endif
}


BEGIN_MESSAGE_MAP(CParsedEditExported, CParsedEdit)
	ON_WM_NCCREATE()
END_MESSAGE_MAP()

int CParsedEditExported::OnNcCreate(LPCREATESTRUCT lpCreateStruct)
{
	// special create hook
	// example of stripping the sub-style bits from the style specified
	//   in the dialog template to use for some other reason
	m_wParseStyle = LOWORD(lpCreateStruct->style);
	DWORD dwEditStyle = MAKELONG(ES_LEFT, HIWORD(lpCreateStruct->style));

	::SetWindowLong(m_hWnd, GWL_STYLE, dwEditStyle);
	lpCreateStruct->style = dwEditStyle;
	return CParsedEdit::OnNcCreate(lpCreateStruct);
}

void CParsedEditExported::PostNcDestroy()
{
	// needed to clean up
	delete this;
}

WNDPROC CParsedEditExported::lpfnSuperWndProc = NULL;
WNDPROC* CParsedEditExported::GetSuperWndProcAddr()
{
	return &lpfnSuperWndProc;
}

/////////////////////////////////////////////////////////////////////////////
// Routine to register the class
BOOL CParsedEdit::RegisterControlClass()
{
	WNDCLASS wcls;

	// Always set the super class address
	// since the second instance will not need to register the WndClass,
	//  but will still need to set the super-proc address
	if (!::GetClassInfo(NULL, "edit", &wcls))
	{
		return FALSE;
	}

	// set appropriate super class address
	CParsedEditExported::lpfnSuperWndProc = wcls.lpfnWndProc; // "EDIT" wnd proc

	// check to see if class already registered
	static const char szClass[] = "paredit";
	if (::GetClassInfo(AfxGetInstanceHandle(), szClass, &wcls))
	{
		// name already registered - ok if it was us
		return (wcls.lpfnWndProc == CParsedEditExported::WndProcHook);
	}

	// set new values
	wcls.lpfnWndProc = CParsedEditExported::WndProcHook;
	wcls.hInstance = AfxGetInstanceHandle();
	wcls.lpszClassName = szClass;
	return (RegisterClass(&wcls) != 0);
}


/////////////////////////////////////////////////////////////////////////////
