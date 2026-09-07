// np.h : main header file for the NP application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

// application defines
#define COMBO_HEIGHT 100	// width of edit control for combobox
#define COMBO_WIDTH  160	// drop down height
#define HISTORY_COUNT 10	// number of entries for the last search selections
#define TIMER_ID 2			// timer ID for clock on status bar

/////////////////////////////////////////////////////////////////////////////
// CNotepadApp:
// See np.cpp for the implementation of this class
//

class CNotepadApp : public CWinApp
{
public:
	CNotepadApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNotepadApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CNotepadApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
