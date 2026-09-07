// ctrldemo.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ctrldemo.h"

#include "propsht.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCtrldemoApp

BEGIN_MESSAGE_MAP(CCtrldemoApp, CWinApp)
	//{{AFX_MSG_MAP(CCtrldemoApp)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCtrldemoApp construction

CCtrldemoApp::CCtrldemoApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CCtrldemoApp object

CCtrldemoApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CCtrldemoApp initialization

BOOL CCtrldemoApp::InitInstance()
{
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CAllControlsSheet	allcontrolssheet(_T("Common Controls Sample"));
	m_pMainWnd = &allcontrolssheet;
	allcontrolssheet.DoModal();
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CCtrldemoApp commands

