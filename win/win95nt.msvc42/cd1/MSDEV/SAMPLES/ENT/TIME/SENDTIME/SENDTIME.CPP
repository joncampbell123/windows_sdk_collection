// SendTime.cpp : Defines the class behaviors for the application.
//
//THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER
//EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
//MERCHANTIBILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
//
//Copyright (C) 1996 Microsoft Corporation. All Rights Reserved.

#include "stdafx.h"
#include "SendTime.h"
#include "SendTDlg.h"
#include "cl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSendTimeApp

BEGIN_MESSAGE_MAP(CSendTimeApp, CWinApp)
        //{{AFX_MSG_MAP(CSendTimeApp)
                // NOTE - the ClassWizard will add and remove mapping macros here.
                //    DO NOT EDIT what you see in these blocks of generated code!
        //}}AFX_MSG
        ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSendTimeApp construction

CSendTimeApp::CSendTimeApp()
{
        // TODO: add construction code here,
        // Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSendTimeApp object

CSendTimeApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSendTimeApp initialization

BOOL CSendTimeApp::InitInstance()
{
        // Standard initialization
        // If you are not using these features and wish to reduce the size
        //  of your final executable, you should remove from the following
        //  the specific initialization routines you do not need.
        HRESULT hRes = CoInitializeEx(NULL, COINIT_MULTITHREADED);
        ASSERT(!FAILED(hRes));

#ifdef _AFXDLL
        Enable3dControls();         // Call this when using MFC in a shared DLL
#else
        Enable3dControlsStatic();   // Call this when linking to MFC statically
#endif

        CSendTimeDlg dlg;
        m_pMainWnd = &dlg;

        dlg.myComLoc.AllocEnv();
        dlg.myComLoc.Open();

        int nResponse = dlg.DoModal();

        if (nResponse == IDOK)
        {
                // TODO: Place code here to handle when the dialog is
                //  dismissed with OK
                dlg.myComLoc.Close();
                dlg.myComLoc.FreeEnv();
        }
        else
        if (nResponse == IDCANCEL)
        {
                // TODO: Place code here to handle when the dialog is
                //  dismissed with Cancel
                dlg.myComLoc.Close();
                dlg.myComLoc.FreeEnv();
        }

        // Since the dialog has been closed, return FALSE so that we exit the
        //  application, rather than start the application's message pump.
        CoUninitialize();
        return FALSE;
}
