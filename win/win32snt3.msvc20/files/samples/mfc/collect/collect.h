// collect.h : main header file for the COLLECT application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CCollectApp:
// See collect.cpp for the implementation of this class
//

class CCollectApp : public CWinApp
{
public:
	CCollectApp();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCollectApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CCollectApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
