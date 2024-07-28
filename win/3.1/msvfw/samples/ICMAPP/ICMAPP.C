/*----------------------------------------------------------------------------*\
 *
 *  ICMAPP:
 *
 *    Sample app showing the use of DrawDib and the
 *    VfW Image compression/decompression APIs
 *
 *    ICMAPP is a MDI aplication that demonstates the following:
 *
 *      - loading/saving windows bitmaps (ie DIBs) see DIB.C and DIB.H
 *
 *      - using DRAWDIB to draw compressed/uncompressed images
 *
 *      - calling ICImageCompress and ICImageDecompress and ICCompressorChoose
 *
 *----------------------------------------------------------------------------*/

/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1992, 1993  Microsoft Corporation.  All Rights Reserved.
 * 
 **************************************************************************/

#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>

#define NOAVIFILE
#include <vfw.h>

#include "icmapp.h"
#include "dib.h"
#include "pro.h"

static HCURSOR hcurSave;
#define StartWait() hcurSave = SetCursor(LoadCursor(NULL,IDC_WAIT))
#define EndWait()   SetCursor(hcurSave)

#define BOUND(x,min,max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))
#define SWAP(x,y)   ((x)^=(y)^=(x)^=(y))
#define ALIGNB(x)   (((x) + 7) & ~0x07)

#ifndef MKFOURCC
#define MKFOURCC( ch0, ch1, ch2, ch3 )                                    \
		( (DWORD)(BYTE)(ch0) | ( (DWORD)(BYTE)(ch1) << 8 ) |	\
		( (DWORD)(BYTE)(ch2) << 16 ) | ( (DWORD)(BYTE)(ch3) << 24 ) )
#endif

char    szAppName[]  ="ICMAPP";     /*  app's name */
char    szDocClass[] ="ChildChild"; /*  the class of the child */

char    szOpenFilter[] = "Bitmaps\0*.dib;*.bmp;*.rle\0"
                         "All\0*.*\0";

char    szSaveFilter[] = "Bitmaps\0*.dib;*.bmp;*.rle\0"
                         "All\0*.*\0";

HANDLE  hInstApp;           /* Instance handle */
HACCEL  hAccelApp;
HWND    hwndApp;            /* Handle to parent window */
HWND    hwndMdi;            /* Handle to MDI client window */

OFSTRUCT     of;
OPENFILENAME ofn;
char         achFileName[128];
BOOL	     gfStretchToWindow;

static HANDLE CopyHandle(HANDLE h);

/*----------------------------------------------------------------------------*\
\*----------------------------------------------------------------------------*/

long FAR PASCAL _export AppWndProc(HWND, UINT, WPARAM, LPARAM);
long FAR PASCAL _export mdiDocWndProc(HWND, UINT, WPARAM, LPARAM);
HWND mdiCreateDoc(LPSTR szClass, LPSTR szTitle, LPARAM l);
int ErrMsg (LPSTR sz,...);

/*----------------------------------------------------------------------------*\
|   AppAbout( hDlg, msg, wParam, lParam )                                      |
|                                                                              |
|   Description:                                                               |
|       This function handles messages belonging to the "About" dialog box.    |
|       The only message that it looks for is WM_COMMAND, indicating the use   |
|       has pressed the "OK" button.  When this happens, it takes down         |
|       the dialog box.                                                        |
|                                                                              |
|   Arguments:                                                                 |
|       hDlg            window handle of about dialog window                   |
|       msg             message number                                         |
|       wParam          message-dependent                                      |
|       lParam          message-dependent                                      |
|                                                                              |
|   Returns:                                                                   |
|       TRUE if message has been processed, else FALSE                         |
|                                                                              |
\*----------------------------------------------------------------------------*/
BOOL FAR PASCAL _export AppAbout(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_COMMAND:
            EndDialog(hwnd,TRUE);
            return TRUE;

        case WM_INITDIALOG:
	    return TRUE;
    }
    return FALSE;
}

/*----------------------------------------------------------------------------*\
|   AppInit ( hInstance, hPrevInstance )				       |
|                                                                              |
|   Description:                                                               |
|       This is called when the application is first loaded into               |
|       memory.  It performs all initialization that doesn't need to be done   |
|       once per instance.                                                     |
|                                                                              |
|   Arguments:                                                                 |
|	hPrevInstance	instance handle of previous instance		       |
|       hInstance       instance handle of current instance                    |
|                                                                              |
|   Returns:                                                                   |
|       TRUE if successful, FALSE if not                                       |
|                                                                              |
\*----------------------------------------------------------------------------*/
BOOL AppInit(HANDLE hInst, HANDLE hPrev, LPSTR szCmd, int sw)
{
    WNDCLASS    cls;
    WORD	wVer;

    /* first let's make sure we are running on 1.1 */
    wVer = HIWORD(VideoForWindowsVersion());
    if (wVer < 0x010a){
	    /* oops, we are too old, blow out of here */
	    MessageBeep(MB_ICONHAND);
	    MessageBox(NULL, "Video for Windows version is too old",
		       "ICMApp Error", MB_OK|MB_ICONSTOP);
	    return FALSE;
    }

    /* Save instance handle for DialogBox */
    hInstApp = hInst;

    hAccelApp = LoadAccelerators(hInstApp, "AppAccel");

    if (!hPrev) {
        cls.hCursor        = LoadCursor(NULL,IDC_ARROW);
        cls.hIcon          = LoadIcon(hInst,"AppIcon");
        cls.lpszMenuName   = "AppMenu";
        cls.lpszClassName  = szAppName;
        cls.hbrBackground  = (HBRUSH)COLOR_APPWORKSPACE+1;
        cls.hInstance      = hInst;
        cls.style          = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
        cls.lpfnWndProc    = AppWndProc;
        cls.cbClsExtra     = 0;
	cls.cbWndExtra	   = 0;

        if (!RegisterClass(&cls))
            return FALSE;

        cls.hCursor        = LoadCursor(NULL,IDC_ARROW);
        cls.hIcon          = LoadIcon(hInst,"DocIcon");
        cls.lpszMenuName   = NULL;
        cls.lpszClassName  = szDocClass;
        cls.hbrBackground  = (HBRUSH)COLOR_WINDOW+1;
        cls.hInstance      = hInst;
        cls.style          = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
        cls.lpfnWndProc    = mdiDocWndProc;
        cls.cbClsExtra     = 0;
	cls.cbWndExtra	   = sizeof(LONG);

        if (!RegisterClass(&cls))
            return FALSE;
    }

    ProInit(hPrev, hInst);

    hwndApp = CreateWindow(szAppName,szAppName,
	       WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
	       CW_USEDEFAULT,0,
	       CW_USEDEFAULT,0,
	       (HWND)NULL,	  /* no parent */
	       (HMENU)NULL,	  /* use class menu */
               (HANDLE)hInst,     /* handle to window instance */
	       (LPSTR)NULL	  /* no params to pass on */
	     );

    /* Make window visible according to the way the app is activated */
    ShowWindow(hwndApp,sw);

    if (szCmd && szCmd[0])
        mdiCreateDoc(szDocClass, szCmd, 0);

    return TRUE;
}

/*----------------------------------------------------------------------------*\
|   WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )                  |
|                                                                              |
|   Description:                                                               |
|       The main procedure for the App.  After initializing, it just goes      |
|       into a message-processing loop until it gets a WM_QUIT message         |
|       (meaning the app was closed).                                          |
|                                                                              |
|   Arguments:                                                                 |
|       hInstance       instance handle of this instance of the app            |
|       hPrevInstance   instance handle of previous instance, NULL if first    |
|       lpszCmdLine     ->null-terminated command line                         |
|       cmdShow         specifies how the window is initially displayed        |
|                                                                              |
|   Returns:                                                                   |
|       The exit code as specified in the WM_QUIT message.                     |
|                                                                              |
\*----------------------------------------------------------------------------*/
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int sw)
{
    MSG     msg;

    if (!AppInit(hInstance,hPrevInstance,szCmdLine,sw))
       return FALSE;

    /*
     * Polling messages from event queue
     */
    for (;;)
    {
        if (PeekMessage(&msg, NULL, 0, 0,PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;

            if (hAccelApp && TranslateAccelerator(hwndApp, hAccelApp, &msg))
                continue;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            // idle time here, DONT BE A PIG!
            WaitMessage();
        }
    }

    return msg.wParam;
}

/*----------------------------------------------------------------------------*\
\*----------------------------------------------------------------------------*/

BOOL fDialog(HWND hwnd,int id,FARPROC fpfn)
{
    BOOL	f;
    HANDLE	hInst;

    hInst = (HANDLE)GetWindowWord(hwnd,GWW_HINSTANCE);
    fpfn  = MakeProcInstance(fpfn,hInst);
    f = DialogBox(hInst,MAKEINTRESOURCE(id),hwnd,(DLGPROC)fpfn);
    FreeProcInstance (fpfn);
    return f;
}

/*----------------------------------------------------------------------------*\
\*----------------------------------------------------------------------------*/

#define mdiGetCreateParam(lParam) \
 (((LPMDICREATESTRUCT)(((LPCREATESTRUCT)lParam)->lpCreateParams))->lParam)

/*----------------------------------------------------------------------------*\
|   mdiCreateChild()							       |
|									       |
|   Description:                                                               |
|                                                                              |
|   Arguments:                                                                 |
|                                                                              |
|   Returns:                                                                   |
|	HWND if successful, NULL otherwise				       |
|									       |
\*----------------------------------------------------------------------------*/

HWND mdiCreateChild(
    HWND  hwndMdi,
    LPSTR szClass,
    LPSTR szTitle,
    DWORD dwStyle,
    int   x,
    int   y,
    int   dx,
    int   dy,
    WORD  sw,
    HMENU hmenu,
    LPARAM l)
{
    MDICREATESTRUCT mdics;

    mdics.szClass   = szClass;
    mdics.szTitle   = szTitle;
    mdics.hOwner    = (HANDLE)GetWindowWord(hwndMdi, GWW_HINSTANCE);
    mdics.x         = x;
    mdics.y         = y;
    mdics.cx        = dx;
    mdics.cy        = dy;
    mdics.style     = dwStyle;
    mdics.lParam    = l;

    return (HWND)SendMessage(hwndMdi,WM_MDICREATE,0,(LONG)(LPVOID)&mdics);
}

/*----------------------------------------------------------------------------*\
|   mdiCreateDoc()                                                           |
|									       |
|   Description:                                                               |
|                                                                              |
|   Arguments:                                                                 |
|                                                                              |
|   Returns:                                                                   |
|	HWND if successful, NULL otherwise				       |
|									       |
|   Comments: We check the high word of the return value from WM_MDIGETACTIVE  |
|   to determine whether we need to create the new window maximized or not.    |
|									       |
\*----------------------------------------------------------------------------*/

HWND mdiCreateDoc(LPSTR szClass, LPSTR szTitle, LPARAM l)
{
    // Be sure to 
    return mdiCreateChild(hwndMdi,szClass,szTitle,
	HIWORD(SendMessage(hwndMdi, WM_MDIGETACTIVE, 0, 0L)) ? WS_MAXIMIZE: 0L,
        CW_USEDEFAULT,0,CW_USEDEFAULT,0,SW_NORMAL, NULL,l);
}

/*----------------------------------------------------------------------------*\
|   mdiCreateClient()                                                           |
|									       |
|   Description:                                                               |
|                                                                              |
|   Arguments:                                                                 |
|                                                                              |
|   Returns:                                                                   |
|	HWND if successful, NULL otherwise				       |
|									       |
\*----------------------------------------------------------------------------*/
HWND FAR PASCAL mdiCreateClient(HWND hwndP, HMENU hmenuWindow)
{
    CLIENTCREATESTRUCT ccs;

    ccs.hWindowMenu = hmenuWindow;
    ccs.idFirstChild = 10000;

    return CreateWindow ("MDICLIENT",
			 NULL,
			 WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE,
			 0, 0, 0, 0,
			 hwndP,
			 0,
			 GetWindowInstance(hwndP),
			 (LPVOID)&ccs);
}

/*----------------------------------------------------------------------------*\
\*----------------------------------------------------------------------------*/

#define mdiActiveDoc(hwnd) \
    (HWND)SendMessage(hwnd,WM_MDIGETACTIVE,0,0L)

/*----------------------------------------------------------------------------*\
\*----------------------------------------------------------------------------*/

LONG NEAR PASCAL mdiSendMessage(HWND hwndMdi, HWND hwnd, unsigned msg, WORD wParam, LONG lParam)
{
    if (hwnd == (HWND)-1)
    {
        for (hwnd = GetWindow(hwndMdi, GW_CHILD); hwnd; hwnd = GetWindow(hwnd, GW_HWNDNEXT))
            SendMessage(hwnd, msg, wParam, lParam);

        return 0L;
    }
    else
    {
        if (hwnd == NULL)
            hwnd = mdiActiveDoc(hwndMdi);

        if (hwnd)
            return SendMessage(hwnd, msg, wParam, lParam);
    }
}


LONG CALLBACK _export PreviewStatusProc(LPARAM lParam, UINT message, LONG l)
{
    switch (message) {
	MSG msg;
	
	case ICSTATUS_START:
	    ProOpen((HWND) lParam, 0);
	    ProSetText(ID_STATUS1, "Compressing....");
	    break;
	    
	case ICSTATUS_STATUS:
	    ProSetBarPos((int) l);
	    while(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
	    {
		if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE)
		    return 1;
		if (msg.message == WM_SYSCOMMAND && (msg.wParam & 0xFFF0) == SC_CLOSE)
		    return 1;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	    }
	    break;
	    
	case ICSTATUS_END:
	    ProClose();
	    break;

	case ICSTATUS_YIELD:

	    break;
    }

    return 0;
}


/*----------------------------------------------------------------------------*\
|   AppWndProc( hwnd, msg, wParam, lParam )                                    |
|                                                                              |
|   Description:                                                               |
|       The window proc for the app's main (tiled) window.  This processes all |
|       of the parent window's messages.                                       |
|									       |
|   Arguments:                                                                 |
|       hwnd            window handle for the parent window                    |
|       msg             message number                                         |
|       wParam          message-dependent                                      |
|       lParam          message-dependent                                      |
|                                                                              |
|   Returns:                                                                   |
|       0 if processed, nonzero if ignored                                     |
|                                                                              |
\*----------------------------------------------------------------------------*/
long FAR PASCAL _export AppWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    UINT            f;
    PAINTSTRUCT     ps;
    HDC             hdc;
    HMENU           hmenu;
    HANDLE          h;
    HWND            hwndActive;

    switch (msg)
    {
	case WM_COMMAND:
	    switch(wParam)
	    {
		case MENU_ABOUT:
                    fDialog(hwnd,ABOUTBOX,(FARPROC)AppAbout);
		    break;

		case MENU_EXIT:
                    PostMessage(hwnd,WM_CLOSE,0,0L);
                    break;

                case MENU_CLOSE:
                    mdiSendMessage(hwndMdi,NULL,WM_CLOSE,0,0);
                    break;

                case MENU_CLOSEALL:
                    while (hwndActive = mdiActiveDoc(hwndMdi))
                        SendMessage(hwndActive,WM_CLOSE,0,0);
                    break;

                case MENU_PASTE:
                    if (!OpenClipboard(hwnd))
                        break;

                    if (h = GetClipboardData(CF_DIB))
                        mdiCreateDoc(szDocClass,"Clipboard", (LPARAM) (UINT) CopyHandle(h));
                    else if (h = GetClipboardData(CF_BITMAP))
                        mdiCreateDoc(szDocClass,"Clipboard", (LPARAM) (UINT) DibFromBitmap(h, BI_RGB, 0, GetClipboardData(CF_PALETTE), 0));

                    CloseClipboard();
                    break;

                case MENU_NEW:
                    mdiCreateDoc(szDocClass, "Untitled", 0);
                    break;

                case MENU_OPEN:
                    /* prompt user for file to open */
                    ofn.lStructSize = sizeof(OPENFILENAME);
                    ofn.hwndOwner = hwnd;
                    ofn.hInstance = NULL;
                    ofn.lpstrFilter = szOpenFilter;
                    ofn.lpstrCustomFilter = NULL;
                    ofn.nMaxCustFilter = 0;
                    ofn.nFilterIndex = 0;
                    ofn.lpstrFile = achFileName;
                    ofn.nMaxFile = sizeof(achFileName);
                    ofn.lpstrFileTitle = NULL;
                    ofn.nMaxFileTitle = 0;
                    ofn.lpstrInitialDir = NULL;
                    ofn.lpstrTitle = "Open";
                    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
                    ofn.nFileOffset = 0;
                    ofn.nFileExtension = 0;
                    ofn.lpstrDefExt = NULL;
                    ofn.lCustData = 0;
                    ofn.lpfnHook = NULL;
                    ofn.lpTemplateName = NULL;

                    if (GetOpenFileName(&ofn))
                    {
                        mdiCreateDoc(szDocClass, achFileName, 0);
                    }
                    break;

                case WM_MDITILE:
                case WM_MDICASCADE:
                case WM_MDIICONARRANGE:
                    SendMessage(hwndMdi, (UINT)wParam, 0, 0);
                    break;

                default:
                    mdiSendMessage(hwndMdi,NULL,msg,wParam,lParam);
                    break;
	    }
            break;

        case WM_PALETTECHANGED:
            return mdiSendMessage(hwndMdi, (HWND)-1, msg, wParam, lParam);

        case WM_QUERYNEWPALETTE:
            return mdiSendMessage(hwndMdi, NULL, msg, wParam, lParam);

        case WM_INITMENU:

	    f = (IsClipboardFormatAvailable(CF_DIB) ||
		IsClipboardFormatAvailable(CF_BITMAP))
		? MF_ENABLED : MF_GRAYED;

	    EnableMenuItem((HMENU)wParam, MENU_PASTE, f);

            hwndActive = mdiActiveDoc(hwndMdi);

            if(hwndActive)
            {
                f = MF_ENABLED;
                EnableMenuItem((HMENU)wParam, MENU_CLOSE, f);
                EnableMenuItem((HMENU)wParam, MENU_CLOSEALL, f);
                SendMessage (hwndActive,msg,wParam,lParam);
            }
	    else
	    {
		f = MF_GRAYED;
		EnableMenuItem((HMENU)wParam, MENU_CLOSE, f);
                EnableMenuItem((HMENU)wParam, MENU_CLOSEALL, f);
		EnableMenuItem((HMENU)wParam, MENU_SAVE, f);
                EnableMenuItem((HMENU)wParam, MENU_COPY, f);
                EnableMenuItem((HMENU)wParam, MENU_COMPRESS, f);
                EnableMenuItem((HMENU)wParam, MENU_DECOMPRESS, f);

	    }
            break;

       case WM_CREATE:
            hmenu = GetMenu(hwnd);
            hwndMdi = mdiCreateClient(hwnd, GetSubMenu(hmenu, GetMenuItemCount(hmenu)-1));
            break;

       case WM_SIZE:
            MoveWindow(hwndMdi,0,0,LOWORD(lParam),HIWORD(lParam),TRUE);
            break;

       case WM_DESTROY:
	    PostQuitMessage(0);
	    break;

       case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            return 0;
    }
    return DefFrameProc(hwnd,hwndMdi,msg,wParam,lParam);
}

/*----------------------------------------------------------------------------*\
\*----------------------------------------------------------------------------*/
LPSTR FileName(LPSTR szPath)
{
    LPSTR   sz;

    #define SLASH(c)     ((c) == '/' || (c) == '\\')

    for (sz=szPath; *sz; sz++)
        ;
    for (; sz>=szPath && !SLASH(*sz) && *sz!=':'; sz--)
        ;
    return ++sz;
}

/*----------------------------------------------------------------------------*\
|   ErrMsg - Opens a Message box with a error message in it.  The user can     |
|	     select the OK button to continue or the CANCEL button to kill     |
|	     the parent application.					       |
\*----------------------------------------------------------------------------*/
int ErrMsg (LPSTR sz,...)
{
    char ach[128];
    wvsprintf(ach,sz,(LPSTR)(&sz+1));   /* Format the string */
    MessageBox (NULL,ach,NULL,MB_OK|MB_ICONEXCLAMATION);
    return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

typedef struct
{
    HWND                hwnd;
    HDRAWDIB            hdd;
    HANDLE              hdib;
    LPBITMAPINFOHEADER  lpbi;
    BITMAPINFOHEADER    bi;
    RECT                rcDraw;
    RECT                rcSource;
    int                 iZoom;
    BOOL                fCanDecompress;
    BOOL                fCanCompress;
    char                achFileName[128];
}   DIBINFO, *PDIBINFO;

void InitSize(PDIBINFO pdi);
BOOL InitDib(PDIBINFO pdi, HANDLE hdib);
void SizeWindowToImage(PDIBINFO pdi);

/*----------------------------------------------------------------------------*\
|   mdiDocProc( hwnd, msg, wParam, lParam )                                    |
|                                                                              |
|   Description:                                                               |
|	The window proc for a MDI child window				       |
|									       |
|   Arguments:                                                                 |
|       hwnd            window handle for the parent window                    |
|       msg       message number                                               |
|       wParam          message-dependent                                      |
|       lParam          message-dependent                                      |
|                                                                              |
|   Returns:                                                                   |
|       0 if processed, nonzero if ignored                                     |
|                                                                              |
\*----------------------------------------------------------------------------*/

long FAR PASCAL _export mdiDocWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    PDIBINFO        pdi;
    UINT            f;
    PAINTSTRUCT     ps;
    HDC             hdc;
    int             iMax;
    int             iMin;
    int             iPos;
    int             dn;
    RECT            rc;
    COMPVARS        compvars;
    HANDLE          hdib;

    pdi = (PDIBINFO)GetWindowLong(hwnd, 0);

    switch (msg)
    {
        case WM_CREATE:
            pdi = (PDIBINFO)LocalAlloc(LPTR,sizeof(DIBINFO));
            SetWindowLong(hwnd,0,(LONG)(UINT)pdi);

	    if (pdi == NULL)
		return -1;

            pdi->hwnd = hwnd;
            GetWindowText(hwnd, pdi->achFileName, sizeof(pdi->achFileName));

            if (!InitDib(pdi, (HANDLE)mdiGetCreateParam(lParam)))
	    {
                ErrMsg("Unable to open '%s'",(LPSTR)pdi->achFileName);
                return -1;
            }
	    break;
																 
        case WM_SIZE:
            InitSize(pdi);
	    break;

        case WM_CLOSE:
            break;

        case WM_DESTROY:
            if (pdi->hdib)
                GlobalFree(pdi->hdib);

            if (pdi->hdd)
                DrawDibClose(pdi->hdd);

	    LocalFree((HLOCAL)pdi);
	    SetWindowLong(hwnd, 0, 0);
            break;

        case WM_INITMENU:
            f = pdi->hdib ? MF_ENABLED : MF_GRAYED;
            EnableMenuItem((HMENU)wParam, MENU_COPY,  f);
            EnableMenuItem((HMENU)wParam, MENU_SAVE,  f);

            EnableMenuItem((HMENU)wParam, MENU_DECOMPRESS, pdi->fCanDecompress ? MF_ENABLED : MF_GRAYED);
            EnableMenuItem((HMENU)wParam, MENU_COMPRESS,   pdi->fCanCompress ? MF_ENABLED : MF_GRAYED);

            CheckMenuItem((HMENU)wParam, MENU_ZOOMW,   gfStretchToWindow    ? MF_CHECKED : MF_UNCHECKED);
            CheckMenuItem((HMENU)wParam, MENU_ZOOM1,   pdi->iZoom == ZOOM   ? MF_CHECKED : MF_UNCHECKED);
            CheckMenuItem((HMENU)wParam, MENU_ZOOM2,   pdi->iZoom == ZOOM*2 ? MF_CHECKED : MF_UNCHECKED);
            CheckMenuItem((HMENU)wParam, MENU_ZOOM12,  pdi->iZoom == ZOOM/2 ? MF_CHECKED : MF_UNCHECKED);
            break;

        case WM_COMMAND:
            switch(wParam)
            {
                case MENU_COMPRESS:

		    compvars.cbSize = sizeof(COMPVARS);	// validate it
		    compvars.dwFlags = 0;

                    f = ICCompressorChoose(
                                hwnd,               // parent window for dialog
                                0,                  // no special boxes
                                pdi->lpbi,          // only if can compress this
                                0,		// !!! DibPtr(pdi->lpbi) -> PAVI
                                &compvars,          // data structure
                                NULL);              // title

                    if (f)
                    {
			FARPROC lpfnHook = NULL;
			
                        StartWait();

			UpdateWindow(hwndMdi);

			lpfnHook = MakeProcInstance((FARPROC) PreviewStatusProc,
 							hInstApp);
			
			ICSetStatusProc(compvars.hic,
					0,
					(LPARAM) (UINT) hwndApp,
					lpfnHook);
			
                        hdib = ICImageCompress(
                            compvars.hic,	// compressor to use
                            0,                  // flags
                            (LPBITMAPINFO)pdi->lpbi, // format to compress from
                            DibPtr(pdi->lpbi),  // bits to compress
                            NULL,               // output format (default)
                            compvars.lQ,        // quality to use.
                            NULL);                 // size of output (whatever)

                        EndWait();

			ICSetStatusProc(compvars.hic,
					0,
					(LPARAM) (UINT) hwndMdi,
					NULL);

			FreeProcInstance(lpfnHook);

			
			ICCompressorFree(&compvars);

                        if (hdib)
                            mdiCreateDoc(szDocClass, pdi->achFileName, (LPARAM)(UINT)hdib);
			else
			    ErrMsg("Unable to compress DIB.");
                    }
                    break;

                case MENU_DECOMPRESS:

                    StartWait();

                    hdib = ICImageDecompress(
                            NULL,               // decompressor to use (any)
                            0,                  // flags
                            (LPBITMAPINFO)pdi->lpbi, // format to decompress from
                            DibPtr(pdi->lpbi),  // bits to decompress
                            NULL);              // format to compress to. (any)

                    EndWait();

                    if (hdib)
                        mdiCreateDoc(szDocClass, pdi->achFileName, (LPARAM)(UINT)hdib);

                    break;
		    
		// toggle stretch to window
                case MENU_ZOOMW:
		    gfStretchToWindow = !gfStretchToWindow;
		    pdi->iZoom = 0;
                    SizeWindowToImage(pdi);
                    InvalidateRect(hwnd, NULL, TRUE);
                    break;

                case MENU_ZOOM1:
                case MENU_ZOOM2:
                case MENU_ZOOM12:
                    pdi->iZoom = (int)wParam - MENU_ZOOM;
                    SizeWindowToImage(pdi);
                    InvalidateRect(hwnd, NULL, TRUE);
                    break;

                case MENU_COPY:
                    if (!OpenClipboard(hwnd))
                        break;

                    EmptyClipboard();
                    SetClipboardData(CF_DIB,CopyHandle(pdi->hdib));
                    CloseClipboard();
                    break;

                case MENU_SAVE:
		    /* set filename to NULL if you are off the */
		    /* clipboard, else set it to the filename  */
		    if (lstrcmp(pdi->achFileName, "Clipboard") == 0){
			    achFileName[0] = '\0';
		    } else {
			    lstrcpy(achFileName, pdi->achFileName);
		    }

                    /* prompt user for file to open */
                    ofn.lStructSize = sizeof(OPENFILENAME);
                    ofn.hwndOwner = hwnd;
                    ofn.hInstance = NULL;
		    ofn.lpstrFilter = szSaveFilter;
                    ofn.lpstrCustomFilter = NULL;
                    ofn.nMaxCustFilter = 0;
                    ofn.nFilterIndex = 0;
                    ofn.lpstrFile = achFileName;
                    ofn.nMaxFile = sizeof(achFileName);
                    ofn.lpstrFileTitle = NULL;
                    ofn.nMaxFileTitle = 0;
                    ofn.lpstrInitialDir = NULL;
                    ofn.lpstrTitle = "Save Dib";
                    ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
                    ofn.nFileOffset = 0;
                    ofn.nFileExtension = 0;
                    ofn.lpstrDefExt = NULL;
                    ofn.lCustData = 0;
                    ofn.lpfnHook = NULL;
                    ofn.lpTemplateName = NULL;

                    if (GetSaveFileName(&ofn))
                    {
			WriteDIB(achFileName,pdi->hdib);
                    } else {
			    DWORD dw;

			    dw = CommDlgExtendedError();
		    }
                    break;
            }
            break;

	case WM_KEYDOWN:
            switch (wParam)
            {
                case VK_UP:    PostMessage (hwnd,WM_VSCROLL,SB_LINEUP,0L);   break;
                case VK_DOWN:  PostMessage (hwnd,WM_VSCROLL,SB_LINEDOWN,0L); break;
                case VK_PRIOR: PostMessage (hwnd,WM_VSCROLL,SB_PAGEUP,0L);   break;
                case VK_NEXT:  PostMessage (hwnd,WM_VSCROLL,SB_PAGEDOWN,0L); break;

                case VK_HOME:  PostMessage (hwnd,WM_HSCROLL,SB_PAGEUP,0L);   break;
                case VK_END:   PostMessage (hwnd,WM_HSCROLL,SB_PAGEDOWN,0L); break;
                case VK_LEFT:  PostMessage (hwnd,WM_HSCROLL,SB_LINEUP,0L);   break;
                case VK_RIGHT: PostMessage (hwnd,WM_HSCROLL,SB_LINEDOWN,0L); break;
	    }
	    break;

	case WM_KEYUP:
            switch (wParam)
            {
	       case VK_UP:
	       case VK_DOWN:
	       case VK_PRIOR:
	       case VK_NEXT:
                  PostMessage (hwnd,WM_VSCROLL,SB_ENDSCROLL,0L);
		  break;

	       case VK_HOME:
	       case VK_END:
	       case VK_LEFT:
	       case VK_RIGHT:
                  PostMessage (hwnd,WM_HSCROLL,SB_ENDSCROLL,0L);
		  break;
	    }
	    break;

        case WM_VSCROLL:
            GetScrollRange(hwnd,SB_VERT,&iMin,&iMax);
            iPos = GetScrollPos(hwnd,SB_VERT);
            GetClientRect(hwnd,&rc);

            switch (wParam)
            {
                case SB_LINEDOWN:      dn =  rc.bottom / 16 + 1; break;
                case SB_LINEUP:        dn = -rc.bottom / 16 + 1; break;
                case SB_PAGEDOWN:      dn =  rc.bottom / 2  + 1; break;
                case SB_PAGEUP:        dn = -rc.bottom / 2  + 1; break;
                case SB_THUMBTRACK:
                case SB_THUMBPOSITION: dn = LOWORD(lParam)-iPos; break;
                default:               dn = 0;
            }
            if (dn = BOUND(iPos+dn,iMin,iMax) - iPos)
            {
                ScrollWindow(hwnd,0,-dn,NULL,NULL);
                SetScrollPos(hwnd,SB_VERT,iPos+dn,TRUE);
                UpdateWindow(hwnd);
            }
            break;

        case WM_HSCROLL:
            GetScrollRange(hwnd,SB_HORZ,&iMin,&iMax);
            iPos = GetScrollPos(hwnd,SB_HORZ);
            GetClientRect(hwnd,&rc);

            switch (wParam) {
                case SB_LINEDOWN:      dn =  rc.right / 16 + 1; break;
                case SB_LINEUP:        dn = -rc.right / 16 + 1; break;
                case SB_PAGEDOWN:      dn =  rc.right / 2  + 1; break;
                case SB_PAGEUP:        dn = -rc.right / 2  + 1; break;
                case SB_THUMBTRACK:
                case SB_THUMBPOSITION: dn = LOWORD(lParam)-iPos; break;
                default:               dn = 0;
            }
            dn = ALIGNB(dn);
            if (dn = BOUND(iPos+dn,iMin,iMax) - iPos)
            {
                ScrollWindow(hwnd,-dn,0,NULL,NULL);
                SetScrollPos(hwnd,SB_HORZ,iPos+dn,TRUE);
                UpdateWindow(hwnd);
            }
            break;

        case WM_CHILDACTIVATE:
        case WM_ACTIVATE:
            SendMessage(hwnd, WM_QUERYNEWPALETTE, 0, 0L);
            break;

        case WM_PALETTECHANGED:
            if (pdi == NULL || pdi->hdd == NULL)
                break;

            hdc = GetDC(hwnd);

            if (f = DrawDibRealize(pdi->hdd, hdc, TRUE))
                InvalidateRect(hwnd,NULL,TRUE);

            ReleaseDC(hwnd,hdc);

            return f;

        case WM_QUERYNEWPALETTE:
            if (pdi == NULL || pdi->hdd == NULL)
                break;

            hdc = GetDC(hwnd);

            if (f = DrawDibRealize(pdi->hdd, hdc, FALSE))
                InvalidateRect(hwnd,NULL,TRUE);

            ReleaseDC(hwnd,hdc);

            return f;

	case WM_ERASEBKGND:
	    hdc = (HDC) wParam;
            SaveDC(hdc);

            ExcludeClipRect(hdc,
                pdi->rcDraw.left,  pdi->rcDraw.top,
                pdi->rcDraw.right, pdi->rcDraw.bottom);

	    DefWindowProc(hwnd, msg, wParam, lParam);

            RestoreDC(hdc, -1);
            return 0L;

        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);

            if (pdi->hdib)
            {
                SetWindowOrg(hdc,GetScrollPos(hwnd,SB_HORZ),GetScrollPos(hwnd,SB_VERT));

                f = DrawDibDraw(pdi->hdd,hdc,
                    pdi->rcDraw.left,
                    pdi->rcDraw.top,
                    pdi->rcDraw.right -pdi->rcDraw.left,
                    pdi->rcDraw.bottom-pdi->rcDraw.top,
                    pdi->lpbi, NULL,
                    pdi->rcSource.left,
                    pdi->rcSource.top,
                    pdi->rcSource.right -pdi->rcSource.left,
                    pdi->rcSource.bottom-pdi->rcSource.top,
                    (hwnd == mdiActiveDoc(hwndMdi)) ? 0 : DDF_BACKGROUNDPAL);

                if (!f)
                    FillRect(hdc, &pdi->rcDraw, GetStockObject(DKGRAY_BRUSH));
            }

            EndPaint(hwnd, &ps);
            return 0;
    }
    return DefMDIChildProc(hwnd,msg,wParam,lParam);
}

/*----------------------------------------------------------------------------*\
\*----------------------------------------------------------------------------*/

void GetRealClientRect(HWND hwnd, LPRECT lprc)
{
    DWORD dwStyle;

    dwStyle = GetWindowLong(hwnd,GWL_STYLE);
    GetClientRect(hwnd,lprc);

    if (dwStyle & WS_HSCROLL)
        lprc->bottom += GetSystemMetrics(SM_CYHSCROLL);

    if (dwStyle & WS_VSCROLL)
        lprc->right  += GetSystemMetrics(SM_CXVSCROLL);
}

void InitSize(PDIBINFO pdi)
{
    RECT rc;
    int i,iRangeV,iRangeH;
    static int iSem=0;

    if (pdi->hdib == NULL || iSem)
        return;

    iSem++;

    //
    // stretch to window?
    //
    if (gfStretchToWindow || IsIconic(pdi->hwnd)) {
        SetScrollRange(pdi->hwnd,SB_VERT,0,0,TRUE);
        SetScrollRange(pdi->hwnd,SB_HORZ,0,0,TRUE);
        GetClientRect(pdi->hwnd, &pdi->rcDraw);
	pdi->iZoom = 0;
    }
    else if (pdi->iZoom) {
        SetRect(&pdi->rcDraw,0, 0,
            (int)pdi->bi.biWidth * pdi->iZoom / ZOOM,
            (int)pdi->bi.biHeight * pdi->iZoom / ZOOM);

        GetRealClientRect(pdi->hwnd,&rc);

        for (i=0; i<2; i++)
        {
            iRangeV = pdi->rcDraw.bottom - rc.bottom;
            iRangeH = pdi->rcDraw.right - rc.right;

            if (iRangeH < 0) iRangeH = 0;
            if (iRangeV < 0) iRangeV = 0;

            if (GetScrollPos(pdi->hwnd,SB_VERT) > iRangeV ||
                GetScrollPos(pdi->hwnd,SB_HORZ) > iRangeH)
                InvalidateRect(pdi->hwnd,NULL,TRUE);

            SetScrollRange(pdi->hwnd,SB_VERT,0,iRangeV,TRUE);
            SetScrollRange(pdi->hwnd,SB_HORZ,0,iRangeH,TRUE);

            GetClientRect(pdi->hwnd,&rc);
        }
    }

    iSem--;
}

/*----------------------------------------------------------------------------*\
\*----------------------------------------------------------------------------*/

BOOL InitDib(PDIBINFO pdi, HANDLE hdib)
{
    char ach[80];
    DWORD dw;
    BOOL f;

    if (hdib == NULL)
        hdib = OpenDIB(pdi->achFileName);

    if (hdib == NULL)
        return FALSE;

    pdi->hdd   = DrawDibOpen();
    pdi->hdib  = hdib;
    pdi->iZoom = ZOOM;
    pdi->lpbi  = (LPVOID)GlobalLock(hdib);
    pdi->fCanCompress   = pdi->lpbi->biCompression == 0;
    pdi->fCanDecompress = pdi->lpbi->biCompression != 0;

    //
    //  fix up the default DIB fields
    //
    if (pdi->lpbi->biClrUsed == 0 && pdi->lpbi->biBitCount <= 8)
        pdi->lpbi->biClrUsed = (1 << (int)pdi->lpbi->biBitCount);

    f = DrawDibBegin(pdi->hdd,NULL,-1,-1,pdi->lpbi,-1,-1,0);

    if (!f)
        ErrMsg("Unable to draw this type of DIB");

    pdi->bi = *pdi->lpbi;
    SetRect(&pdi->rcSource,  0, 0, (int)pdi->bi.biWidth, (int)pdi->bi.biHeight);

    switch (dw = pdi->bi.biCompression)
    {
        case BI_RGB:    dw = mmioFOURCC('N', 'o', 'n', 'e'); break;
        case BI_RLE4:   dw = mmioFOURCC('R', 'l', 'e', '4'); break;
        case BI_RLE8:   dw = mmioFOURCC('R', 'l', 'e', '8'); break;
    }

    wsprintf(ach, "%ls (%dx%dx%d '%4.4ls' %dk)",
            FileName(pdi->achFileName),
            (int)pdi->bi.biWidth,
            (int)pdi->bi.biHeight,
            (int)pdi->bi.biBitCount,
            (LPSTR)&dw,
            (int)(pdi->bi.biSizeImage/1024));

    SetWindowText(pdi->hwnd, ach);
    SizeWindowToImage(pdi);
    return TRUE;
}

void SizeWindowToImage(PDIBINFO pdi)
{
    RECT rc;
    RECT rcMdi,rcChild;

    /* don't size window if it's currently maximized */
    if (!IsZoomed(pdi->hwnd) && !IsIconic(pdi->hwnd) && pdi->iZoom > 0)
    {
        SetRect(&rc, 0, 0,
            (int)pdi->bi.biWidth  * pdi->iZoom/ZOOM,
            (int)pdi->bi.biHeight * pdi->iZoom/ZOOM);

        AdjustWindowRect(&rc, GetWindowLong(pdi->hwnd, GWL_STYLE), FALSE);

	/* Determine size of MDI Client area and image window */
	GetClientRect(hwndMdi, &rcMdi);
	rc.right  -= rc.left;
	rc.bottom -= rc.top;

	/* Make sure window is positioned so that entire window is seen */
	GetWindowRect(pdi->hwnd, &rcChild);
	ScreenToClient(hwndMdi, (LPPOINT)&rcChild);
	if (rc.right > rcMdi.right)
	{
	    rc.right = rcMdi.right+2;
	    rcChild.left = -1;
	}
	if (rc.bottom > rcMdi.bottom)
	{
	    rc.bottom = rcMdi.bottom+2;
	    rcChild.top = -1;
	}
        SetWindowPos(pdi->hwnd, NULL,
            rcChild.left, rcChild.top, rc.right, rc.bottom,
            SWP_NOACTIVATE|SWP_NOZORDER);
    }

    InitSize(pdi);
}

static HANDLE CopyHandle(HANDLE h)
{
    HANDLE hCopy;

    if (hCopy = GlobalAlloc(GHND,GlobalSize(h)))
        hmemcpy(GlobalLock(hCopy), GlobalLock(h), GlobalSize(h));

    return hCopy;
}

/*****************************************************************************
 *
 * dprintf() is called by the DPF macro if DEBUG is defined at compile time.
 *
 * The messages will be send to COM1: like any debug message. To 
 * enable debug output, add the following to WIN.INI :
 *
 * [debug]
 * ICSAMPLE=1
 *
 ****************************************************************************/

#ifdef DEBUG

#define MODNAME "ICMAPP"

static void FAR cdecl dprintf(LPSTR szFormat, ...)
{
    char ach[128];

    static BOOL fDebug = -1;

    if (fDebug == -1)
        fDebug = GetProfileInt("Debug", MODNAME, FALSE);

    if (!fDebug)
        return;

    lstrcpy(ach, MODNAME ": ");
    wvsprintf(ach+lstrlen(ach),szFormat,(LPSTR)(&szFormat+1));
    lstrcat(ach, "\r\n");

    OutputDebugString(ach);
}

#endif
