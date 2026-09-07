// ctrldemo.h : main header file for the CTRLDEMO application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


/////////////////////////////////////////////////////////////////////////////
// CCtrldemoApp:
// See ctrldemo.cpp for the implementation of this class
//
#define LISTICONCOUNT	4
#define ICONLISTFIRST	0
#define ICONLISTLAST	(ICONLISTFIRST + LISTICONCOUNT - 1)
#define ICONCOUNT (LISTICONCOUNT)

#define CELEMS(rgFoo) (sizeof(rgFoo) / sizeof(rgFoo[0]))

class CCtrldemoApp : public CWinApp
{
public:
	CCtrldemoApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCtrldemoApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CCtrldemoApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
