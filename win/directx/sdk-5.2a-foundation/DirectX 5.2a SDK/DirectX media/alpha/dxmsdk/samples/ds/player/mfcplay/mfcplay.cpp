//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992 - 1997  Microsoft Corporation.  All Rights Reserved.
//
//--------------------------------------------------------------------------;
//
// player.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "mfcplay.h"

#include "mainfrm.h"
#include "mfcdoc.h"
#include "mfcvw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlayerApp

BEGIN_MESSAGE_MAP(CPlayerApp, CWinApp)
	//{{AFX_MSG_MAP(CPlayerApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlayerApp construction

CPlayerApp::CPlayerApp()
{
	// Place all significant initialization in InitInstance

    m_pPlayerDoc = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPlayerApp object

CPlayerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CPlayerApp initialization

BOOL CPlayerApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	Enable3dControls();

	LoadStdProfileSettings( 0 );  // Load standard INI file options (including MRU)

    // Initialize the quartz library
    CoInitialize(NULL);
	
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CPlayerDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CPlayerView));
	AddDocTemplate(pDocTemplate);

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes();

	// simple command line parsing
	if (m_lpCmdLine[0] == '\0')
	{
		// create a new (empty) document
		OnFileNew();
	}
	else
	{
		// open an existing document
		if(!OpenDocumentFile(m_lpCmdLine))
                    return FALSE;
	}

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	return TRUE;
}

int CPlayerApp::ExitInstance( )
{
    CoUninitialize();

    return CWinApp::ExitInstance();
}

void CPlayerApp::OnDocumentCreated( CPlayerDoc *pPlayerDoc )
{
    // Single documents only
    // If you want to convert to an MDI you will need to hold a list
    // of all created documents and change CPlayerApp::Run to build
    // up a list of event handles to wait for.
    ASSERT( m_pPlayerDoc == NULL );
    m_pPlayerDoc = pPlayerDoc;

}

void CPlayerApp::OnDocumentDestroyed( CPlayerDoc *pPlayerDoc )
{
    // Single documents only
    ASSERT( m_pPlayerDoc == pPlayerDoc );
    m_pPlayerDoc = NULL;

}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CPlayerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

int CPlayerApp::Run()
{   // Overridden to check for Graph events as well as messages

	if (m_pMainWnd == NULL && AfxOleGetUserCtrl())
	{
		// Not launched /Embedding or /Automation, but has no main window!
		TRACE0("Warning: m_pMainWnd is NULL in CPlayerApp::Run - quitting application.\n");
		AfxPostQuitMessage(0);
	}

    BOOL bIdle = TRUE;
    LONG lIdleCount = 0;
    HANDLE  ahObjects[1];               // handles that need to be waited on
    const int cObjects = 1;             // no of objects that we are waiting on

    // message loop lasts until we get a WM_QUIT message
    // upon which we shall return from the function
    while (TRUE) {

        // If we don't have an event handle then process idle
        // routines until a message arrives or until the idle routines
        // stop (when we block until a message arrives). The graph event
        // handle can only be created in response to a message
        if( (ahObjects[ 0 ] = m_pPlayerDoc->GetGraphEventHandle()) == NULL ){
    		while (    bIdle
    		        && !::PeekMessage(&m_msgCur, NULL, NULL, NULL, PM_NOREMOVE))
    		{
    			// call OnIdle while in bIdle state
    			if (!OnIdle(lIdleCount++)){
    				bIdle = FALSE;
                    WaitMessage();
                }
    		}
        } else {
            // wait for any message sent or posted to this queue
            // or for a graph notification. If there is no message or event
            // and we are idling then we process the idle time routines
            DWORD result;

            result = MsgWaitForMultipleObjects( cObjects
                                              , ahObjects
                                              , FALSE
                                              , (bIdle ? 0 : INFINITE)
                                              , QS_ALLINPUT
                                              );
            if( result != (WAIT_OBJECT_0 + cObjects) ){
                // not a message...

                if( result == WAIT_OBJECT_0 )
                    m_pPlayerDoc->OnGraphNotify();

                else if( result == WAIT_TIMEOUT )
                    if(!OnIdle(lIdleCount++))
                        bIdle = FALSE;

                continue;
            }
        }


        // When here, we either have a message or no event handle
        // has been created yet.

        // read all of the messages in this next loop
        // removing each message as we read it
		do
		{
			// pump message, but quit on WM_QUIT
			if (!PumpMessage())
				return ExitInstance();

			// reset "no idle" state after pumping "normal" message
			if (IsIdleMessage(&m_msgCur))
			{
				bIdle = TRUE;
				lIdleCount = 0;
			}

		} while (::PeekMessage(&m_msgCur, NULL, NULL, NULL, PM_NOREMOVE));

    } // end of the always while-loop


}
/////////////////////////////////////////////////////////////////////////////
// CPlayerApp commands
