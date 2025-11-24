/*
 -  S E N D M A I L . C P P
 -  Copyright (C) 1994 Microsoft Corporation
 -
 *  Purpose:
 *      Main source module for SendApp.  This is an MFC application.
 *
 */



#ifndef CHICAGO
    #define CTL3D
#endif

#define SMALL_ATTACHMENT_SIZE     32000
#define LARGE_ATTACHMENT_SIZE   1000000
#define HUGE_ATTACHMENT_SIZE    2600000



#ifdef WIN32
#ifdef CHICAGO
#define _INC_OLE
#endif
#define INC_OLE2
#define INC_RPC
#endif

#include <afxwin.h>     
#include <windowsx.h>
#include <string.h>

#ifdef WIN16
#include <compobj.h>
#endif

#ifdef WIN32
#include <objbase.h>
#include <objerror.h>
#ifdef CHICAGO
#include <ole2.h>
#endif
#endif



#ifdef WIN16
    #include <direct.h>
#endif
#include "strtbl.h"
#include <mapidefs.h>
#include <commdlg.h>
#include <mapi.h>
#include <mapiwin.h>
#include <time.h>
#include "pvalloc.h"
#ifndef CHICAGO
    #include <ctl3d.h>
#endif
#include "smplmapi.h"
#include "resource.h"
#include "sendmail.h"


/*
 -
 -  Global data
 -
 */

CTheApp                 theApp;
LHANDLE                 lhSession   = NULL;
HWND                    hWnd;
HINSTANCE               hLib        = NULL;
BOOL                    fLoggedOn   = FALSE;
static HCURSOR          hWaitCur;
BOOL                    fKillThread = FALSE;
static lpMapiRecipDesc  lpRecips    = NULL;
ULONG                   cRecips     = 0;
BOOL                    fCustomAttachment = FALSE;
OPENFILENAME            ofn;
char *                  szMax = "Maximum number of characters";
char                    szFileName[256] = "";

LPFNMAPILOGON           lpfnMAPILogon = NULL;
LPFNMAPILOGOFF          lpfnMAPILogoff = NULL;
LPFNMAPISENDMAIL        lpfnMAPISendMail = NULL;
LPFNMAPISENDDOCUMENTS   lpfnMAPISendDocuments = NULL;
LPFNMAPIFINDNEXT        lpfnMAPIFindNext = NULL;
LPFNMAPIREADMAIL        lpfnMAPIReadMail = NULL;
LPFNMAPISAVEMAIL        lpfnMAPISaveMail = NULL;
LPFNMAPIDELETEMAIL      lpfnMAPIDeleteMail = NULL;
LPFNMAPIFREEBUFFER      lpfnMAPIFreeBuffer = NULL;
LPFNMAPIADDRESS         lpfnMAPIAddress = NULL;
LPFNMAPIDETAILS         lpfnMAPIDetails = NULL;
LPFNMAPIRESOLVENAME     lpfnMAPIResolveName = NULL;


/*
 -
 -  Main message loop
 -
 */

BEGIN_MESSAGE_MAP( CMainWindow, CFrameWnd )
    ON_COMMAND( IDM_EXIT,   OnExit )
    ON_COMMAND( IDM_SEND,   OnSend )
    ON_COMMAND( IDM_ABOUT,  OnAbout )
    ON_WM_DESTROY()
END_MESSAGE_MAP()


/*
 -
 -  CMainWindow Methods.
 -
 */

/*
 -  InitInstance
 -
 *  Purpose:
 *      When any CTheApp object is created, this member function is
 *      automatically  called.  The main window of the application
 *      is created and shown here.
 *
 *  Parameters:
 *      None
 *
 *  Returns:
 *      TRUE if the initialization is successful.
 *
 */

BOOL CTheApp::InitInstance()
{
    TRACE( "SendMail\n" );
    m_pMainWnd = new CMainWindow();

    m_pMainWnd->ShowWindow( m_nCmdShow );
    m_pMainWnd->UpdateWindow();

    CoInitialize(NULL); // Init the 'new' operator for OLE 2.0 compatibility

#ifdef CTL3D
    Ctl3dRegister( AfxGetInstanceHandle() );   // Register Ctl3D controls
    Ctl3dAutoSubclass( AfxGetInstanceHandle());
#endif

    return InitMapiDll();
}


/*
 -  CMainWindow::
 -  CMainWindow
 -
 *  Purpose:
 *      Create the window with the appropriate style, size, menu, etc.
 *
 *  Parameters:
 *      None
 *
 *
 */

CMainWindow::CMainWindow()
{
    CRect rect;
    int nL, nT, nR, nB = 100;

    LoadAccelTable( "MainAccelTable" );

    nL = GetPrivateProfileInt("Location", "Left", 5, "SendMail.ini");
    nT = GetPrivateProfileInt("Location", "Top", 5, "SendMail.ini");
    nR = GetPrivateProfileInt("Location", "Right", 300, "SendMail.ini");
    nB = GetPrivateProfileInt("Location", "Bottom", 75, "SendMail.ini");

    rect.SetRect(nL, nT, nR, nB);
    Create( NULL, "MAPI",
        WS_OVERLAPPEDWINDOW, rect, NULL, MAKEINTRESOURCE(AFX_IDI_STD_FRAME) );

    hWnd = m_hWnd;

#ifdef CTL3D
    Ctl3dColorChange();
#else
    CMainWindow::OnSysColorChange();
#endif

}


/*
 -
 -  CMainWindow::
 -  OnClose
 -
 -  Purpose:
 -      Gets the windows coordinates, and deletes all temporary files.
 -
 -  Parameters:
 -      None
 -
 -  Returns:
 -      Void
 -
 */

void CMainWindow::OnClose()
{
    ULONG           ulResult;
    CRect           lpRect;
    char            szBuf[256];
    char            szTempPath[256];
    WIN32_FIND_DATA pfd;
    HANDLE          hFind;

    GetTempPath(256,szTempPath);
#ifdef WIN32
    SetCurrentDirectory(szTempPath);
#else
    _chdir(szTempPath);
#endif

    lstrcat(szTempPath,"*.XXX");
    while((hFind = FindFirstFile(szTempPath, &pfd)) != (HANDLE)INVALID_HANDLE_VALUE)
    {
        (void)DeleteFile(pfd.cFileName);
        FindClose(hFind);
    }

    GetWindowRect( lpRect );
    wsprintf((LPSTR) szBuf, (LPSTR) "%d", lpRect.left);
    WritePrivateProfileString("Location", "Left", (LPSTR) szBuf, "SendMail.ini");

    wsprintf((LPSTR) szBuf, (LPSTR) "%d", lpRect.top);
    WritePrivateProfileString("Location", "Top", (LPSTR) szBuf, "SendMail.ini");

    wsprintf((LPSTR) szBuf, (LPSTR) "%d", lpRect.right);
    WritePrivateProfileString("Location", "Right", (LPSTR) szBuf, "SendMail.ini");

    wsprintf((LPSTR) szBuf, (LPSTR) "%d", lpRect.bottom);
    WritePrivateProfileString("Location", "Bottom", (LPSTR) szBuf, "SendMail.ini");

    if(MAPIFreeBuffer( lpRecips ))
        MessageBox( "Recipients not freed", "Stress Mailer", MB_OK );

    if(lhSession)
    {
        ulResult = MAPILogoff( lhSession, (ULONG)(void *)m_hWnd, MAPI_LOGOFF_UI, 0 );
        lhSession = 0;
    }

    if(hLib)
        FreeLibrary(hLib);

    CoUninitialize();
    DeinitSimpleMAPI ();
    DestroyWindow();
}



/*
 -  ExitInstance
 -
 *  Purpose:
 *
 *  Parameters:
 *      None
 *
 *  Returns:
 *      TRUE if the Exit is successful.
 *
 */

int CTheApp::ExitInstance()
{

#ifdef CTL3D
    Ctl3dUnregister(AfxGetInstanceHandle());
#endif
    return CWinApp::ExitInstance();
}


/*
 -  CMainWindow::
 -  OnExit
 -
 *  Purpose:
 *      Asks the application to Close itself.
 *
 */

void CMainWindow::OnExit()
{
    OnClose();
    SendMessage( WM_CLOSE );
}


/*
 -  CMainWindow::
 -  PostNcDestroy
 *
 *  Purpose:
 *      In order to avoid an access violation when ending this application
 *      it it necessary to catch the WM_DESTROY window message.
 *
 *      Cleanup of the main window will occur in the OnClose function, with
 *      the DestroyWindow() call.
 *
 *      ExitInstance() will perform the remaining cleanup.
 *
 */

void CMainWindow::PostNcDestroy()
{
    //don't delete this
}


/*
 -
 -  CMainWindow::
 -  OnAbout
 -
 -  Purpose:
 -      A CModalDialog object for the About dialog.
 -
 -  Parameters:
 -      None
 -
 -  Returns:
 -      Void
 -
 */

void CMainWindow::OnAbout()
{
    CModalDialog about( "AboutBox", this );
    about.DoModal();
}


/*
 -
 -  CMainWindow::
 -  OnSend
 -
 -  Purpose:
 -      Brings up the Profile Logon dialog.
 -      Sets the logged on flag to true.
 -      Calls the SendMail dialog
 -
 -  Parameters:
 -      None
 -
 -  Returns:
 -      Void
 -
 */

void CMainWindow::OnSend()
{
    ULONG       ulResult;
    char        szBuf[128];

    if( !fLoggedOn )
    {
        if( (ulResult = MAPILogon( (ULONG)(void *) hWnd, NULL, NULL, MAPI_LOGON_UI | MAPI_NEW_SESSION,
                   0, &lhSession )) != SUCCESS_SUCCESS )
        {
            if( ulResult )
            {
                wsprintf( szBuf, "%s %lu", "Error: MAPILogon", ulResult );
                MessageBox( szBuf, "Stress Mailer", MB_OK );
                return;
            }
        }
    }

    fLoggedOn = TRUE;

    CSendMailDialog sendmail( this );
    sendmail.DoModal();

    return;
}


/*
 -
 -  CSendMailDialog message loop
 -
 */

BEGIN_MESSAGE_MAP( CSendMailDialog, CModalDialog )
    ON_COMMAND( IDOK,        OnOK )
    ON_COMMAND( IDC_ADDRESS, OnAddress )
    ON_COMMAND( IDCANCEL,    OnCancel )
    ON_COMMAND( IDC_STOP,    OnStop )
    ON_COMMAND( IDC_FILE,    OnFile )
END_MESSAGE_MAP()


/*
 -
 -  CSendMailDialog::
 -  OnInitDialog
 -
 -  Purpose:
 -      Sets some initial dialog settings
 -      Zeros and Nulls the recipient lists
 -
 -  Parameters:
 -      None
 -
 -  Returns:
 -      Void
 -
 */

BOOL CSendMailDialog::OnInitDialog()
{
    static char     *rgszSubject[6] = {
                                        "Message from SendMail",
                                        "NULL",
                                        "Empty String (nothing)",
                                        "A single character - A",
                                        "Maximum number of characters",
                                        "Alternate (minimum messages: 5)"
                                        };

    static char     *rgszMessageType[] = {
                                            "IPM",
                                            "IPX",
                                            "IPC",
                                            "MCI",
                                            "NULL",
                                            "Empty String (nothing)",
                                            "A single characters - A",
                                            "Maximum number of characters",
                                            "Alternate (minimum messages: 7)"
                                            };

    static char     *rgszMessageText[] = {
                                            "Body text",
                                            "NULL",
                                            "Empty String (nothing)",
                                            "A single character - A",
                                            "Maximum number of characters",
                                            "Alternate (minimum messages: 5)"
                                            };

    if( !CDialog::OnInitDialog() )
        return (FALSE);

    hWaitCur = LoadCursor( NULL, IDC_WAIT );

    SetDlgItemInt( IDC_MESSAGES, 5, FALSE );
    SetDlgItemInt( IDC_SECONDS, 0, FALSE );
    CheckRadioButton( IDC_NOATTACHMENT, IDC_LSATTACHMENT, IDC_NOATTACHMENT );

    /* Listbox for Subject */
    for( int i=0; i<(sizeof(rgszSubject)/sizeof(char *)); ++i )
        SendDlgItemMessage(IDC_SUBJECT, CB_ADDSTRING, 0, ((LPARAM)(LPSTR)rgszSubject[i]));
    SendDlgItemMessage(IDC_SUBJECT, CB_SETCURSEL, 0, 0L);

    /* Listbox for Message Type */
    for( i=0; i<(sizeof(rgszMessageType)/sizeof(char *)); ++i )
        SendDlgItemMessage(IDC_MESSAGETYPE, CB_ADDSTRING, 0, ((LPARAM)(LPSTR)rgszMessageType[i]));
    SendDlgItemMessage(IDC_MESSAGETYPE, CB_SETCURSEL, 0, 0L);

    /* Listbox for Message Text */
    for( i=0; i<(sizeof(rgszMessageText)/sizeof(char *)); ++i )
        SendDlgItemMessage(IDC_MESSAGETEXT, CB_ADDSTRING, 0, ((LPARAM)(LPSTR)rgszMessageText[i]));
    SendDlgItemMessage(IDC_MESSAGETEXT, CB_SETCURSEL, 0, 0L);

    SendDlgItemMessage(IDC_STATUS, LB_SETHORIZONTALEXTENT,
                       (WPARAM)2000, 0);

    lpRecips = NULL;
    cRecips = 0;

    return (TRUE);
}


/*
 -
 -  CSendMailDialog::
 -  OnStop
 -
 -  Purpose:
 -      Kills the thread that is doing the sending.
 -
 -  Parameters:
 -      None
 -
 -  Returns:
 -      Void
 -
 */

void CSendMailDialog::OnStop()
{
    fKillThread = TRUE;
}


/*
 -
 -  CSendMailDialog::
 -  OnFile
 -
 -  Purpose:
 -      Bring up command dialog to select a specific attachment for the
 -      sendnote.
 -
 -  Parameters:
 -      None
 -
 -  Returns:
 -      void
 -
 */

void CSendMailDialog::OnFile()
{
    static char szFileTitle[16];
    static char szDirName[256] = "";
    static char * szFilter = "All Files (*.*)\0""*.*\0""\0";

    if (!szDirName[0])
        GetSystemDirectory ((LPSTR) szDirName, 255);
    else
        lstrcpy (szFileName, szFileTitle);

    ofn.lStructSize = sizeof (OPENFILENAME);
    ofn.hwndOwner = 0;
    ofn.hInstance = 0;
    ofn.lpstrFilter = (LPSTR) szFilter;
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter = 0L;
    ofn.nFilterIndex = 1L;
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = 256;
    ofn.lpstrFileTitle = szFileTitle;
    ofn.nMaxFileTitle = 16;
    ofn.lpstrInitialDir = szDirName;
    ofn.lpstrTitle = "Attach";
    ofn.nFileOffset = 0;
    ofn.nFileExtension = 0;
    ofn.lpstrDefExt = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

    if(GetOpenFileName (&ofn))
        fCustomAttachment = TRUE;
}


/*
 -
 -  CSendMailDialog::
 -  OnOK
 -
 -  Purpose:
 -      Gets the number of messages, subject, timing delay, and
 -          recipients from the SendMail dialog
 -      Creates a Simple MAPI message data structure
 -      Calls MAPISendMail x times
 -      Logs the start, status, and end of the test
 -
 -  Parameters:
 -      None
 -
 -  Returns:
 -      Void
 -
 */

void CSendMailDialog::OnOK()
{
    ULONG           ulAttach = 0;
    HCURSOR         hOldCur;
    LPTHREADDATA    lptd;
    ULONG           ulItem = 0;
    BOOL            fMultipleAttachments = FALSE;
    int             i = 0;
    ULONG           len = 0;
    ULONG           idx = 0;
    LONG            cb = 0;
    OFSTRUCT        lpOpenBuff1;
    OFSTRUCT        lpOpenBuff2;
    HFILE           hFile1;
    HFILE           hFile2;
    char *          pszFileName1;
    char *          pszFileName2;
    WIN32_FIND_DATA pfd;
    HANDLE          hFind;
    char            szTempPath[256];

#ifdef WIN32
    HANDLE          hThrd;
#else
    CHiddenDialog   *pHiddenDialog = NULL;
#endif

    hOldCur = SetCursor(hWaitCur);
    GetTempPath(256,szTempPath);

#ifdef WIN32
    SetCurrentDirectory(szTempPath);
#else
    _chdir(szTempPath);
#endif

    if( lpRecips == NULL )
    {
        MessageBox( "No recipients have been selected", "Stress Mailer", MB_OK );
        return;
    }

    lptd = (LPTHREADDATA)PvAlloc(sizeof(THREADDATA));

    if(!lptd)
    {
        MessageBox("Error: PvAlloc failed to allocate lptd.", "Stress Mailer", MB_OK );
        goto free;
    }

    lptd->fSubjectAlt = FALSE;
    lptd->fMessageTypeAlt = FALSE;
    lptd->fNoteTextAlt = FALSE;

    //  Number of messages
    lptd->cMessages = GetDlgItemInt( IDC_MESSAGES, NULL, FALSE );

    //  Time delay between messages
    lptd->nSeconds = GetDlgItemInt( IDC_SECONDS, NULL, FALSE );

    //  Determine whether or not to mail everybody at 1 time
    lptd->fSendToAll = IsDlgButtonChecked( IDC_GROUPSEND );
    lptd->lpRecips = lpRecips;
    lptd->cRecips  = cRecips;

    lptd->lpmsg = (MapiMessage FAR *)PvAlloc(sizeof(MapiMessage));

    if(!lptd->lpmsg)
    {
        MessageBox("Error: PvAlloc failed to allocate lpmsg.", "Stress Mailer", MB_OK );
        goto free;
    }

    memset( lptd->lpmsg, 0, sizeof( MapiMessage ) );

    lptd->hWnd = m_hWnd;

    //  Get subject
    //  The list box strings describe the subject, they do not give the literal
    //  contents for the subject
    ulItem = SendDlgItemMessage( IDC_SUBJECT, CB_GETCURSEL, 0, 0L );
    switch( ulItem )
    {
        case 0 : lptd->lpmsg->lpszSubject = (LPSTR) PvAlloc(sizeof("Message from SendMail")+10);
                 lstrcpy(lptd->lpmsg->lpszSubject, "Message from SendMail");
                 break;

        case 1 : PvFree(lptd->lpmsg->lpszSubject);
                 lptd->lpmsg->lpszSubject = NULL;
                 break;

        case 2 : lstrcpy(lptd->lpmsg->lpszSubject, "");
                 break;

        case 3 : lptd->lpmsg->lpszSubject = (LPSTR) PvAlloc(sizeof("A"));
                 lstrcpy(lptd->lpmsg->lpszSubject, "A");
                 break;

        case 4 : lptd->lpmsg->lpszSubject = (LPSTR)PvAlloc(256);
                 lstrcpy(lptd->lpmsg->lpszSubject,szMax);
                 _fmemset(lptd->lpmsg->lpszSubject+strlen(szMax), 99, 256-strlen(szMax));
                 lptd->lpmsg->lpszSubject[255] = '\0';
                 break;

        case 5 : //Will cycle through all subjects in rgszSubject
                 lptd->fSubjectAlt = TRUE;
                 lptd->lpmsg->lpszSubject = (LPSTR)PvAlloc(256);
                 break;

        default : lptd->lpmsg->lpszSubject = (LPSTR)PvAlloc(255);
                  GetDlgItemText( IDC_SUBJECT, lptd->lpmsg->lpszSubject, 255 );
    }

    //  Get message type
    ulItem = SendDlgItemMessage( IDC_MESSAGETYPE, CB_GETCURSEL, 0, 0L );
    switch( ulItem )
    {
        case 0 : lptd->lpmsg->lpszMessageType = (LPSTR) PvAlloc(4);
                 lstrcpy(lptd->lpmsg->lpszMessageType, "IPM");
                 break;

        case 1 : lptd->lpmsg->lpszMessageType = (LPSTR) PvAlloc(4);
                 lstrcpy(lptd->lpmsg->lpszMessageType, "IPX");
                 break;

        case 2 : lptd->lpmsg->lpszMessageType = (LPSTR) PvAlloc(4);
                 lstrcpy(lptd->lpmsg->lpszMessageType, "IPC");
                 break;

        case 3 : lptd->lpmsg->lpszMessageType = (LPSTR) PvAlloc(4);
                 lstrcpy(lptd->lpmsg->lpszMessageType, "MCI");
                 break;

        case 4 : PvFree(lptd->lpmsg->lpszMessageType);
                 lptd->lpmsg->lpszMessageType = NULL;
                 break;

        case 5 : lstrcpy(lptd->lpmsg->lpszMessageType, "");
                 break;

        case 6 : lptd->lpmsg->lpszMessageType = (LPSTR)PvAlloc(256);
                 lstrcpy(lptd->lpmsg->lpszMessageType,szMax);
                 _fmemset(lptd->lpmsg->lpszMessageType+strlen(szMax), 99, 256-strlen(szMax));
                 lptd->lpmsg->lpszMessageType[255] = '\0';
                 break;

        case 7 : //Will cycle through all message types in rgszMessageType
                 lptd->fMessageTypeAlt = TRUE;
                 lptd->lpmsg->lpszMessageType = (LPSTR)PvAlloc(256);
                 break;

        default : lptd->lpmsg->lpszMessageType = (LPSTR)PvAlloc(255);
                  GetDlgItemText( IDC_MESSAGETYPE, lptd->lpmsg->lpszMessageType, 255 );
    }

    //  Get message text
    ulItem = SendDlgItemMessage( IDC_MESSAGETEXT, CB_GETCURSEL, 0, 0L );
    switch( ulItem )
    {
        case 0 : lptd->lpmsg->lpszNoteText = (LPSTR) PvAlloc(sizeof("Body text"));
                 lstrcpy(lptd->lpmsg->lpszNoteText, "Body text");
                 break;

        case 1 : PvFree(lptd->lpmsg->lpszNoteText);
                 lptd->lpmsg->lpszNoteText = NULL;
                 break;

        case 2 : lstrcpy(lptd->lpmsg->lpszNoteText, "");
                 break;

        case 3 : lstrcpy(lptd->lpmsg->lpszNoteText, "A");
                 break;

        case 4 : lptd->lpmsg->lpszNoteText = (LPSTR)PvAlloc(65000);
                 lstrcpy(lptd->lpmsg->lpszNoteText,szMax);
                 _fmemset(lptd->lpmsg->lpszNoteText+strlen(szMax), 99, size_t(65000-strlen(szMax)));
                 lptd->lpmsg->lpszNoteText[64999] = '\0';
                 break;

        case 5 : //Will cycle through all message texts in rgszMessageText
                 lptd->fNoteTextAlt = TRUE;
                 lptd->lpmsg->lpszNoteText = (LPSTR)PvAlloc(65000);
                 break;

        default : lptd->lpmsg->lpszNoteText = (LPSTR)PvAlloc(255);
                  GetDlgItemText( IDC_MESSAGETEXT, lptd->lpmsg->lpszNoteText, 255 );
    }

    //  Check to see if multiple attachments are requested
    fMultipleAttachments = IsDlgButtonChecked( IDC_MULTIPLE );

    //  Find out what type of attachment is requested
    ulAttach = GetCheckedRadioButton( IDC_NOATTACHMENT, IDC_LSATTACHMENT );

    //  Count the number of attachments
    lptd->lpmsg->nFileCount = 0;
    if( (ulAttach != IDC_NOATTACHMENT) || fCustomAttachment )
    {
        if( (ulAttach == IDC_SATTACHMENT) ||
            (ulAttach == IDC_LATTACHMENT) ||
            (ulAttach == IDC_HATTACHMENT) )
            ++lptd->lpmsg->nFileCount;

        if( ulAttach == IDC_LSATTACHMENT )
            lptd->lpmsg->nFileCount = 2;

        if( fMultipleAttachments )
            lptd->lpmsg->nFileCount *= 5;

        if( fCustomAttachment )
            ++lptd->lpmsg->nFileCount;

        lptd->lpmsg->lpFiles = (MapiFileDesc FAR *)PvAlloc(lptd->lpmsg->nFileCount*sizeof(MapiFileDesc));
        if(!lptd->lpmsg->lpFiles)
        {
            MessageBox("Error: PvAlloc failed to allocate lpFiles", "Stress Mailer", MB_OK );
            return;
        }

        if( fCustomAttachment && (lptd->lpmsg->nFileCount == 1) )
            goto Custom;

        //  Create attachments
        pszFileName1 = (LPSTR)PvAlloc(256);
        lstrcpy(pszFileName1, "file1.XXX");
        if( (hFile1 = OpenFile(pszFileName1, &lpOpenBuff1, OF_CREATE | OF_READWRITE)) == HFILE_ERROR )
        {
            MessageBox("Error: Could not create data file", "Stress Mailer", MB_OK );
            goto free;
        }

        switch( ulAttach )
        {
            case IDC_SATTACHMENT : FillFile( SMALL_ATTACHMENT_SIZE, hFile1 );
                                   rename(pszFileName1, "SMALL.XXX");
                                   lstrcpy(pszFileName1, "SMALL.XXX");
                                   break;

            case IDC_LATTACHMENT : FillFile( LARGE_ATTACHMENT_SIZE, hFile1 );
                                   rename(pszFileName1, "LARGE.XXX");
                                   lstrcpy(pszFileName1, "LARGE.XXX");
                                   break;

            case IDC_HATTACHMENT : FillFile( HUGE_ATTACHMENT_SIZE, hFile1 );
                                   rename(pszFileName1, "HUGE.XXX");
                                   lstrcpy(pszFileName1, "HUGE.XXX");
                                   break;

            case IDC_LSATTACHMENT : pszFileName2 = (LPSTR)PvAlloc(256);
                                    lstrcpy(pszFileName2, "file2.XXX");
                                    if( (hFile2 = OpenFile(pszFileName2, &lpOpenBuff2, OF_CREATE | OF_READWRITE )) == HFILE_ERROR )
                                    {
                                        MessageBox("Error: Could not create data file", "Stress Mailer", MB_OK );
                                        goto free;
                                    }
                                    FillFile( SMALL_ATTACHMENT_SIZE, hFile1 );
                                    rename(pszFileName1, "SMALL.XXX");
                                    lstrcpy( pszFileName1, "SMALL.XXX");
                                    FillFile( LARGE_ATTACHMENT_SIZE, hFile2 );
                                    rename(pszFileName2, "LARGE.XXX");
                                    lstrcpy( pszFileName2, "LARGE.XXX");
                                    break;
        }  // Case

        if( fCustomAttachment )
            idx = lptd->lpmsg->nFileCount-1;
        else
            idx = lptd->lpmsg->nFileCount;

        for( i=0; i<idx; ++i )
        {
            lptd->lpmsg->lpFiles[i].ulReserved = 0;
            lptd->lpmsg->lpFiles[i].flFlags = 0;
            lptd->lpmsg->lpFiles[i].nPosition = (ULONG) -1;
            if( (ulAttach==IDC_LSATTACHMENT) && (i%2==0) )
            {
                lptd->lpmsg->lpFiles[i].lpszPathName = pszFileName2;
                lptd->lpmsg->lpFiles[i].lpszFileName = pszFileName2;
            }
            else
            {
                lptd->lpmsg->lpFiles[i].lpszPathName = pszFileName1;
                lptd->lpmsg->lpFiles[i].lpszFileName = pszFileName1;
            }
            lptd->lpmsg->lpFiles[i].lpFileType = NULL;
        }

    Custom:
        if( fCustomAttachment )
        {
            lptd->lpmsg->lpFiles[i].lpszPathName = (LPSTR)PvAlloc(256);
            lptd->lpmsg->lpFiles[i].ulReserved = 0;
            lptd->lpmsg->lpFiles[i].flFlags = 0;
            lptd->lpmsg->lpFiles[i].nPosition = (ULONG) -1;
            memcpy(lptd->lpmsg->lpFiles[i].lpszPathName, ofn.lpstrFile, 256);
            lptd->lpmsg->lpFiles[i].lpszFileName = lptd->lpmsg->lpFiles[i].lpszPathName;
            lptd->lpmsg->lpFiles[i].lpFileType = NULL;
        }
    }

    /* (LPTHREAD_START_ROUTINE) */
#ifdef WIN32
    hThrd = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)DoSend, lptd,
                0, &lptd->dwThreadId );
    if( hThrd )
#else
    pHiddenDialog = new CHiddenDialog(lptd);
    if(pHiddenDialog)
#endif

    {
        GetDlgItem(IDOK)->EnableWindow( FALSE );
        GetDlgItem(IDC_ADDRESS)->EnableWindow( FALSE );
        return;
    }
    else
        MessageBox( "Cannot create thread", "Stress Mailer", MB_OK );

free:
        PvFree( lptd->lpmsg->lpszNoteText );
        PvFree( lptd->lpmsg->lpszMessageType );
        PvFree( lptd->lpmsg->lpszSubject );
        PvFree( lptd->lpmsg->lpFiles );
        PvFree( lptd->lpmsg );
        PvFree( lptd );

        while((hFind = FindFirstFile(szTempPath, &pfd)) != (HANDLE)INVALID_HANDLE_VALUE)
        {
            (void)DeleteFile(pfd.cFileName);
            FindClose(hFind);
        }

#ifdef WIN16
        delete pHiddenDialog;
#endif

        PvFree(pszFileName1);
        PvFree(pszFileName2);
}


/*
 -
 -  CSendMailDialog::
 -  OnAddress
 -
 -  Purpose:
 -      Call MAPIAddress to allow the user to select the recipients.
 -      Creates a buffer that contains all recipients to be displayed
 -          in the SendMail dialog.
 -
 -  Parameters:
 -      None
 -
 -  Returns:
 -      Void
 -
 */

void CSendMailDialog::OnAddress()
{
    char            szDisplayName[255];
    ULONG           ulResult;
    static ULONG    ulTestNumber;
    HCURSOR         hOldCur;
    char            szBuf[128];

    hOldCur = SetCursor(hWaitCur);

    lpNewRecips = NULL;
    cNewRecips = 0;

    ulResult = MAPIAddress( lhSession, (ULONG)(void *) this->m_hWnd, "Address Book", 1,
        "&To:", cRecips, lpRecips, 0, 0, &cNewRecips, &lpNewRecips);

    if( ulResult )
    {
        if( ulResult != MAPI_E_USER_ABORT )
        {
            wsprintf( szBuf, "%s %lu", "Error: MAPIAddress", ulResult );
            MessageBox( szBuf, "Stress Mailer", MB_OK );
            return;
        }
    }
    else
        if( cNewRecips )
        {
            MAPIFreeBuffer( lpRecips );
            lpRecips = lpNewRecips;
            cRecips = cNewRecips;
            lpNewRecips =  NULL;
            cNewRecips = 0;
        }
        else
        {
            this->SetDlgItemText( IDC_RECIPIENTS, '\0' );
            return;
        }

    szDisplayName[0] = '\0';

    for( int idx=0; idx<cRecips; idx++ )
    {
        if( lpRecips[idx].ulRecipClass == MAPI_TO )
        {
            if(strlen(szDisplayName+strlen(lpRecips[idx].lpszName)+2)>255)
                break;
            lstrcat( szDisplayName, lpRecips[idx].lpszName );
            lstrcat( szDisplayName, "; " );
        }
    }

    if( *szDisplayName )
    {
        szDisplayName[lstrlen(szDisplayName)-2] = '\0';
        this->SetDlgItemText( IDC_RECIPIENTS, szDisplayName );
    }

    return;
}


/*
 -
 -  CSendMailDialog::
 -  OnCancel
 -
 -  Purpose:
 -      Frees the recipient structure, and zeros and NULLs it out
 -
 -  Parameters:
 -      None
 -
 -  Returns:
 -      Void
 -
 */

void CSendMailDialog::OnCancel()
{
    MAPIFreeBuffer( lpRecips );
    lpRecips = NULL;
    cRecips = 0;

    EndDialog(IDOK);
}


#ifdef WIN16
//  The HiddenDialog is used in 16bit to simulate multiple threads.
/*
 -
 -  CHiddenDialog::
 -  OnInitDialog
 -
 -  Purpose:
 -
 -  Parameters:
 -      None
 -
 -  Returns:
 -      Void
 -
 */

BOOL CHiddenDialog::OnInitDialog()
{
    DoSend(lptd2);
    EndDialog(TRUE);
    return TRUE;
}


/*
 -  CHiddenDialog
 -  OnCancel
 -
 *  Purpose:
 *      Cancels the CHiddenDialog dialog
 *
 */

void CHiddenDialog::OnCancel()
{
    delete this;
}
#endif


/*
 -  DoSend
 -
 *  Purpose:
 *      Sends the message and updates the status listbox
 *
 */

long DoSend( LPTHREADDATA lptd )
{
    int             iMessage, jRecipient, idx;
    ULONG           cSend = 0;
    ULONG           ulResult;
    int             cRun = 0;
    int             cFailed = 0;
    char            szBufS[256];
    char            szResult[256];
    char            szBuffer[256];
    char            szSubject[256];
    LPSTR           lpszTemp1, lpszTemp2, lpszTemp3;
    int             nCount = 0;

    SendDlgItemMessage( lptd->hWnd, IDC_STATUS, LB_RESETCONTENT, 0, 0 );

    if( lptd->fSendToAll )
    {
        lptd->lpmsg->nRecipCount = lptd->cRecips;
        cSend = 1;
    }
    else
    {
        lptd->lpmsg->nRecipCount = 1;
        cSend = lptd->cRecips;
    }

    lstrcpy(szBuffer, "Tests beginning...");
    SendDlgItemMessage( lptd->hWnd, IDC_STATUS, LB_ADDSTRING, 0, (LPARAM)szBuffer );

    lstrcpy(szSubject, lptd->lpmsg->lpszSubject);
    for( jRecipient=1; jRecipient <= lptd->cMessages; ++jRecipient )
    {
        idx = jRecipient-1;
        // Set up subject field if alternating was selected
        if( lptd->fSubjectAlt )
        {
            switch( idx%5 )
            {
                case 0: lstrcpy(lptd->lpmsg->lpszSubject, "Message from SendMail");
                        break;
                case 1: lpszTemp1 = lptd->lpmsg->lpszSubject;
                        lptd->lpmsg->lpszSubject = NULL;
                        break;
                case 2: lptd->lpmsg->lpszSubject = lpszTemp1;
                        lstrcpy(lptd->lpmsg->lpszSubject, "");
                        break;
                case 3: lstrcpy(lptd->lpmsg->lpszSubject, "A");
                        break;
                case 4: lstrcpy(lptd->lpmsg->lpszSubject,szMax);
                        _fmemset(lptd->lpmsg->lpszSubject+strlen(szMax), 99, 256-strlen(szMax));
                        lptd->lpmsg->lpszSubject[255] = '\0';
                        break;
            } //switch
        } //if
        else
            wsprintf(lptd->lpmsg->lpszSubject,"%s - %d", szSubject, jRecipient);

        // Set up message type field if alternating was selected
        if( lptd->fMessageTypeAlt )
        {
            switch( jRecipient%7 )
            {
                case 0: lstrcpy(lptd->lpmsg->lpszMessageType, "IPM");
                        break;
                case 1: lstrcpy(lptd->lpmsg->lpszMessageType, "IPX");
                        break;
                case 2: lstrcpy(lptd->lpmsg->lpszMessageType, "IPC");
                        break;
                case 3: lstrcpy(lptd->lpmsg->lpszMessageType, "MCI");
                        break;
                case 4: lpszTemp2 = lptd->lpmsg->lpszMessageType;
                        lptd->lpmsg->lpszMessageType = NULL;
                        break;
                case 5: lptd->lpmsg->lpszMessageType = lpszTemp2;
                        lstrcpy(lptd->lpmsg->lpszMessageType, "");
                        break;
                case 6: lstrcpy(lptd->lpmsg->lpszMessageType,szMax);
                        _fmemset(lptd->lpmsg->lpszMessageType+strlen(szMax), 99, 256-strlen(szMax));
                        lptd->lpmsg->lpszMessageType[255] = '\0';
                        break;
            } //switch
        } //if

        // Set up note text field if alternating was selected
        if( lptd->fNoteTextAlt )
        {
            switch( jRecipient%5 )
            {
                case 0: lstrcpy(lptd->lpmsg->lpszNoteText, "Body text");
                        break;
                case 1: lpszTemp3 = lptd->lpmsg->lpszNoteText;
                        lptd->lpmsg->lpszNoteText = NULL;
                        break;
                case 2: lptd->lpmsg->lpszNoteText = lpszTemp3;
                        lstrcpy(lptd->lpmsg->lpszNoteText, "");
                        break;
                case 3: lstrcpy(lptd->lpmsg->lpszNoteText, "A");
                        break;
                case 4: lstrcpy(lptd->lpmsg->lpszNoteText,szMax);
                        _fmemset(lptd->lpmsg->lpszNoteText+strlen(szMax), 99, size_t(65000-strlen(szMax)));
                        lptd->lpmsg->lpszNoteText[64999] = '\0';
                        break;
            } //switch
        } //if

        for( iMessage=1; iMessage<=cSend; ++iMessage )
        {
            lptd->lpmsg->lpRecips = &lptd->lpRecips[iMessage-1];
            ulResult = MAPISendMail( lhSession, (ULONG)(void *) lptd->hWnd, lptd->lpmsg, 0, 0);
            wsprintf( szBufS, "Message: %d of %d, Recipient %d of %d ", jRecipient, lptd->cMessages, iMessage, cSend );
            SendDlgItemMessage( lptd->hWnd, IDC_STATUS, LB_ADDSTRING, 0, (LPARAM) szBufS );
            nCount = (WORD) SendDlgItemMessage( lptd->hWnd, IDC_STATUS, LB_GETCOUNT, 0, (LPARAM) 0L);
            SendDlgItemMessage( lptd->hWnd, IDC_STATUS, LB_SETCURSEL, nCount-1, (LPARAM) 0L );
            if( ulResult )
            {
                //  This builds a string from the ulResult
                if(!GetString( "MAPIErrors", ulResult, szResult ))
                {
                    lstrcpy(  szResult, "??" );
                    wsprintf( szBuffer, " %04X", ulResult );
                    lstrcat(  szResult, szBuffer );
                }

                wsprintf( szBufS, "ERROR: MAPISendMail returned: %s", szResult );
                SendDlgItemMessage( lptd->hWnd, IDC_STATUS, LB_ADDSTRING, 0, (LPARAM) szBufS );
                ++cFailed;
            }
            if( fKillThread )
            {
                lstrcpy(szBuffer,"User canceled test.");
                SendDlgItemMessage( lptd->hWnd, IDC_STATUS, LB_ADDSTRING, 0, (LPARAM)szBuffer );
                goto KillThread;
            }
            //  Sleep requires an interval in milliseconds
            Sleep( lptd->nSeconds * 1000);
            if( fKillThread )
            {
                lstrcpy(szBuffer,"User canceled test.");
                SendDlgItemMessage( lptd->hWnd, IDC_STATUS, LB_ADDSTRING, 0, (LPARAM)szBuffer );
                goto KillThread;
            }
        }
    }

    jRecipient--;
    iMessage--;

KillThread:
    cRun = jRecipient*iMessage;
    wsprintf( szBufS, "Test(s) completed, Succeeded: %d  Failed: %d ", cRun-cFailed, cFailed );
    SendDlgItemMessage( lptd->hWnd, IDC_STATUS, LB_ADDSTRING, 0, (LPARAM) szBufS );
    nCount = (WORD) SendDlgItemMessage( lptd->hWnd, IDC_STATUS, LB_GETCOUNT, 0, (LPARAM) 0L);
    SendDlgItemMessage( lptd->hWnd, IDC_STATUS, LB_SETCURSEL, nCount-1, (LPARAM) 0L );

    EnableWindow( GetDlgItem(lptd->hWnd, IDOK), TRUE );
    EnableWindow( GetDlgItem(lptd->hWnd, IDC_ADDRESS), TRUE );

    PvFree( lptd->lpmsg->lpszNoteText );
    PvFree( lptd->lpmsg->lpszMessageType );
    PvFree( lptd->lpmsg->lpszSubject );
    for(iMessage=0; iMessage<lptd->lpmsg->nFileCount; ++iMessage)
        PvFree(lptd->lpmsg->lpFiles[iMessage].lpszPathName);
    PvFree( lptd->lpmsg->lpFiles );


    PvFree( lptd->lpmsg );
    PvFree( lptd );

    fKillThread = FALSE;

#ifdef WIN32
    ExitThread( 0 );
#endif

    return(0L);
}


void FillFile( ULONG ulsize, HFILE hFile )
{
    ULONG       i = 0;
    char        szBuffer[100];
    int         j = 33;

    for(i=0; i<ulsize; i+=100)
    {
        if(j == 256)
            j = 33;
        _fmemset(szBuffer, j, 100);
        _lwrite(hFile, szBuffer, 100);
        ++j;
    }
    _lclose(hFile);
}
