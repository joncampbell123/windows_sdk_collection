// propdlg.h : main header file for the PROPDLG application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#define WM_USER_CHANGE_OBJECT_PROPERTIES (WM_USER + 1)

/////////////////////////////////////////////////////////////////////////////
// CPropDlgApp:
// See propdlg.cpp for the implementation of this class
//

class CPropDlgApp : public CWinApp
{
public:
	CPropDlgApp();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropDlgApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPropDlgApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
