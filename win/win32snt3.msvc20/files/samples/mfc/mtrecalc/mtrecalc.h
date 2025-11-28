// mtrecalc.h : main header file for the MTRECALC application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#define WM_USER_RECALC_IN_PROGRESS (WM_USER + 1)
#define WM_USER_RECALC_DONE (WM_USER + 2)
#define WM_USER_RECALC_NOW (WM_USER + 3)

/////////////////////////////////////////////////////////////////////////////
// CRecalcApp:
// See mtrecalc.cpp for the implementation of this class
//

class CRecalcApp : public CWinApp
{
public:
	CRecalcApp();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CRecalcApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CRecalcApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
