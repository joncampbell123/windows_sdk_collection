/*
 *    Main program
 */


#include        <afxwin.h>
#include        <afxdlgs.h>
#include        <afxcoll.h>
#include        <iostream.h>
#include        <fstream.h>

extern "C"
{
    #include <commdlg.h>
    #include <stdio.h>
    #include <string.h>
    #include <imagehlp.h>
}

#include "types.h"
#include "resource.h"
#include "ranges.h"
#include "dispobj.h"
#include "timer.h"
#include "tree.h"
#include "list.h"
#include "listwin.h"
#include "capview.h"

#define SIZESTRING  256

/************************  Data  *******************************/

CTheApp         WinAnalApp;
HCURSOR         HCursorSizer;
HCURSOR         HCursorNormal;
HICON           HIconList;
HICON           HIconTree;
LOGFONT         DefaultFont;

/***************************************************************/

void GetWindows(char * pchProgram, int iThread, CListWnd ** ppListWnd,
                CTREEWND ** pTreeWnd);

/************************  CTheApp definitions *****************/

/***    CTheApp::InitInstance
 *
 *      This function is an override of a virtual function.  The
 *      function is used to do any initialization which is needed
 *      for the CapView as a whole (as oppose to window initialization).
 *
 *      Create the main application whine and load some standard objects
 *      from the library of objects.
 */

BOOL  CTheApp::InitInstance()
{
    CFont       systemFont;

    /*
     *  Create the main window
     */
    
    m_pMainWnd = new CMainWindow();
    m_pMainWnd->ShowWindow( m_nCmdShow );
    m_pMainWnd->UpdateWindow();

    /*
     *  Load stock objects from the system
     */
    
    systemFont.CreateStockObject(SYSTEM_FONT);
    systemFont.GetObject(sizeof(LOGFONT), &DefaultFont);

    HIconList = LoadIcon(LIST_ICO);
    HIconTree = LoadIcon(TREE_ICO);
    
    HCursorSizer = LoadStandardCursor(IDC_SIZEWE);    
    HCursorNormal = LoadStandardCursor(IDC_ARROW);

    /*
     *  Process any command line which shows up
     */

    if (m_lpCmdLine && *m_lpCmdLine) {
        ((CMainWindow *) m_pMainWnd)->LoadFile(m_lpCmdLine);
    }
    
    return TRUE;
}                              /* CTheApp::InitInstance() */




/***********************  CMainWindow definitions ***************/

BEGIN_MESSAGE_MAP( CMainWindow, CMDIFrameWnd )
	ON_WM_CREATE()

	ON_COMMAND(IDM_EXIT, OnExit)
	ON_COMMAND(IDM_OPEN, OnOpen)

	ON_COMMAND(IDM_TILE, MDITile)
	ON_COMMAND(IDM_CASCADE, MDICascade)
	ON_COMMAND(IDM_ARRANGEICON, MDIIconArrange)

	ON_COMMAND(IDM_ABOUT, OnAbout)
        ON_COMMAND(IDM_HELP, OnHelp)
END_MESSAGE_MAP()

CMainWindow::CMainWindow()
{
    LoadAccelTable( "MainAccelTable" );
    Create(NULL, "Capview", WS_OVERLAPPEDWINDOW, rectDefault, NULL,
           "MdiMenuInit");
    return;
}                               /* CMainWindow::CMainWindow() */

CMainWindow::~CMainWindow()
{
    delete m_pMenuInit;
    return;
}                               /* CMainWindow::~CMainWindow() */

int CMainWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    m_pMenuInit = new CMenu();
    m_pMenuInit->LoadMenu("MdiMenuInit");
    CreateClient(lpCreateStruct, m_pMenuInit->GetSubMenu(1));
    return 0;
}                               /* CMainWindow::OnCreate() */

/*
 *  Defined the set of command which are used in response
 *      to commands from the menu bar
 */

void CMainWindow::OnAbout()
{
    ShellAbout(m_hWnd, "CapView", NULL, NULL);
    return;
}                               /* CMainWindow::OnAbout() */

void CMainWindow::OnHelp()
{
    WinHelp(m_hWnd, "capview.hlp", HELP_CONTENTS, 0);
    return;
}                               /* CMainWindow::OnHelp() */

void CMainWindow::OnExit()
{
    DestroyWindow();
    return;
}                               /* CMainWindow::OnExit() */

void CMainWindow::OnOpen()
{
    /*
     *  Attempt to open a profile file and read it.
     *  If a file or archive exception occurs, catch it and
     *  present an error message box.
     */
    
    CString szFileName, szFileTitle;
    
    /*
     *  Use CommDlg to get the file name and then call LoadFile.
     */
    
    if ( FileDlg( TRUE, SIZESTRING, szFileName.GetBuffer( SIZESTRING ),
		 SIZESTRING, szFileTitle.GetBuffer( SIZESTRING ) ) ) {
	
	szFileName.ReleaseBuffer();
	szFileTitle.ReleaseBuffer();

        LoadFile(szFileName);
    }

    return;
}                               /* CMainWindow::OnOpen() */


void CMainWindow::LoadFile(CString szFileName)
{
    ifstream    myFile;
    char        rgch[4000];
    char        rgch2[256];
    char *      pchDll;
    char *      pchFunction;
    char *      pch;
    char *      pchProgram;
    int         iDepth;
    int         iCalls;
    int         iThread;
    TIMETYPE    iTotalTime;
    TIMETYPE    iRoutineTime;
    CListWnd *  pListWnd;
    CTREEWND *  pTreeWnd;

    /*
     *  Now start processing the file
     */
    
    myFile.open(szFileName );
    if (!myFile.is_open()) {
        _snprintf(rgch, sizeof(rgch), "Cannot open file '%s'", szFileName);
        MessageBox(rgch);
        return;
    }

    /*
     *  Get the first line -- it tells us which file is being profiled
     */
    
    myFile.getline( rgch, sizeof(rgch) );
    if (strncmp(rgch, "Call Profile of ", 16) != 0) {
        /*
         *  Message box -- not correct file formatter
         */

        _snprintf(rgch, sizeof(rgch),
                  "The file '%s' is incorrectly formatted", szFileName);
        MessageBox(rgch);
        return;
    }

    /*
     * Capture the program name
     */
     
    pchProgram = &rgch[16];
    strtok(pchProgram, " ");
    pchProgram = strdup(pchProgram);

    /*
     * Process the rest of the file
     */
    
    while (TRUE) {
        /*
         *  When we can't get a line -- exit the loop
         */
        
        myFile.getline( rgch, sizeof(rgch) );
        if (!myFile.good()) {
            break;
        }

        /*
         *  Look for the first possible valid line
         */
        
        if ((strncmp( rgch, "T h r e a d  #", 14) == 0) ||
            (strncmp( rgch, "S e c t i o n  #", 16) == 0)) {
            iThread = atoi(&rgch[14]);

            /*
             *  Get the List and Tree window to be used for this threads
             *  set of data.
             */

            GetWindows(pchProgram, iThread, &pListWnd, &pTreeWnd);

            pListWnd->InitTiming();
            pTreeWnd->InitTiming();
            
            /*
             *  Skip over the headers
             */
            
            myFile.getline( rgch, sizeof(rgch) );
            myFile.getline( rgch, sizeof(rgch) );
            myFile.getline( rgch, sizeof(rgch) );
            
            /*
             * Start processing each data line
             */
            
            while (TRUE) {
                myFile.getline( rgch, sizeof(rgch) );
                pch = strtok(rgch, " \t\n");
                if (pch == NULL) {
                    break;
                }
                
                iDepth = atoi(pch);
                pchDll = strtok(NULL, " \t:");
                if (*pchDll == '*') {
                    pchDll = strtok(NULL, " \t:");
                }
                pchFunction = strtok(NULL, " \t");
                iCalls = atoi(strtok(NULL, " \t"));
                iTotalTime = strtok(NULL, " \t");
                strtok(NULL, " \t");
                iRoutineTime = strtok(NULL, " \t");

                /*
                 *  Remove undeceration from function name
                 */

                if (UnDecorateSymbolName(pchFunction, rgch2, sizeof(rgch2),
                              UNDNAME_NO_FUNCTION_RETURNS |
                              UNDNAME_32_BIT_DECODE |
                              UNDNAME_NO_ACCESS_SPECIFIERS |
                              UNDNAME_NO_MS_KEYWORDS)) {
                    pchFunction = rgch2;
                }
                
                pListWnd->AddTiming(iDepth, pchDll, pchFunction, iCalls,
                                    iTotalTime, iRoutineTime);
                pTreeWnd->AddTiming(iDepth, pchDll, pchFunction, iCalls,
                                    iTotalTime, iRoutineTime);

            }
            
            /*
             *
             */
            
            pTreeWnd->EndTiming();
            pTreeWnd->Invalidate();
            pTreeWnd->SetScrollRange(SB_HORZ, 0, 100);
            pTreeWnd->SetScrollRange(SB_VERT, 0, 100);
            pTreeWnd->ShowWindow(SW_SHOW);
            
            /*
             *
             */
            
            pListWnd->EndTiming();
            pListWnd->Invalidate();
        }
    }

    free(pchProgram);
    return;
}                               /* CMainWindow::LoadFile() */

//////////////////////////////////////////////////
//  CMainWindow::FileDlg
//  Call the commdlg routine to display File Open or File Save As
//  dialogs.  The setup is the same for either.  If bOpen is TRUE
//  then File Open is display otherwise File Save As is displayed.
//  The File Name and File Title are stored at the string pointer
//  passed in.
//
BOOL CMainWindow::FileDlg( BOOL bOpen, int nMaxFile, LPSTR szFile,
		int nMaxFileTitle, LPSTR szFileTitle )
{
    OPENFILENAME of;
    
    char szDirName[SIZESTRING];
    char szFilter[] = "Cap input files (*.cap)\0"
	  "*.cap\0"
	  "Cap end files (*.end)\0"
	  "*.end\0"
	  "All files (*.*)\0"
	  "*.*\0"
	  "\0";

    szDirName[0] = '.';

    of.lStructSize = sizeof( OPENFILENAME );
    of.hwndOwner = m_hWnd;
    of.lpstrFilter = szFilter;
    of.lpstrCustomFilter = NULL;
    of.nMaxCustFilter = 0L;
    of.nFilterIndex = 1L;
    of.lpstrFile=szFile;
    of.nMaxFile=nMaxFile;
    of.lpstrFileTitle = szFileTitle;
    of.nMaxFileTitle = nMaxFileTitle;
    of.lpstrInitialDir = szDirName;
    of.lpstrTitle = NULL;
    of.nFileOffset = 0;
    of.nFileExtension = 0;
    of.lpstrDefExt = "pb";
    
    if ( bOpen ) {
	of.Flags = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	return GetOpenFileName( &of );
    } else {
	of.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	return GetSaveFileName( &of );
    }
}                               /* CMainWindow::FileDlg() */

void CMainWindow::GetWindows(char * szProgram, int iThread,
                             CListWnd ** ppListWnd, CTREEWND ** ppTreeWnd)
{
    char        rgchKey[100];
    char        rgchTitle[100];
    int         count;
    void *      pv;
    
    *ppListWnd = new CListWnd;
    *ppTreeWnd = new CTREEWND;

    _snprintf(rgchKey, sizeof(rgchKey), "%s Thread: %d",
             szProgram, iThread);

    if (mapCInstance.Lookup(rgchKey,  pv)) {
        count = 1 + (int) pv;
        mapCInstance[rgchKey] = (void *) count;
    } else {
        count = 1;
        mapCInstance[rgchKey] = (void *) 1;
    }

    _snprintf(rgchTitle, sizeof(rgchTitle), "Tree: %s <%d>", rgchKey, count);
    if (!(*ppTreeWnd)->Create(rgchTitle, WS_HSCROLL | WS_VSCROLL,
                          rectDefault, this)) {
        delete *ppTreeWnd;
        *ppTreeWnd = NULL;
        return;
    }
            
    _snprintf(rgchTitle, sizeof(rgchTitle), "List: %s <%d>", rgchKey, count);

    if (!(*ppListWnd)->Create(rgchTitle, WS_VSCROLL, rectDefault,
                          this)) {
        delete *ppListWnd;
        *ppListWnd = NULL;
        return;
        
    }
    return;
}                               /* CMainWindow::GetWindows() */
