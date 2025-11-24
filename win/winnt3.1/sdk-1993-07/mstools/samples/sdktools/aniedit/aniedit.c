/****************************************************************************\
*
*     PROGRAM: AniEdit.c
*
*     PURPOSE: Animated Cursor Editor for Windows NT
*
*     Copyright 1993, Microsoft Corp.
*
*
* History:
*   21-Apr-1993 JonPa   Wrote it.
*
\****************************************************************************/

#include <windows.h>
#include "anidefs.h"

HANDLE hInst;
HWND ghwndMain;
HWND ghwndRateScroll = NULL;

ANICUR ganiAcon;

int gcyCursor, gcxCursor;

HBRUSH  ghbrPrevBackgnd, ghbrWindow, ghbrHighlight;
COLORREF gcrHighlightText;

TCHAR   gszModPath[MAX_PATH];     /* name for above font module */
TCHAR   gszWindowTitle[MAX_PATH] = TEXT("AniEdit");
TCHAR   gszDots[] = TEXT("...");
PFRAME  gpfrmFrames = NULL;
PCLPBRDDAT gpbdClipBoard = NULL;
TCHAR   gszCursorEditor[MAX_PATH];

//HACCEL haccel;
int giradColor = 0; /* Default to desktop color */
RADIOCOLOR garadColor[] = {
        {DLG_OPTIONS_RADIO_DESKCOL, COLOR_BACKGROUND},
        {DLG_OPTIONS_RADIO_WINCOL,  COLOR_WINDOW},
        {0, 0}
};

#if DLG_OPTIONS_RADIO_DESKCOL == 0 || DLG_OPTIONS_RADIO_WINCOL == 0
#   error("Dialog IDs must not equal zero!")
#endif


/*
 * Registry Strings
 * (Since the registry is not localized, these don't have to be read in
 *  from the strings RC)
 */
TCHAR gszAppKey[] = "Software\\Microsoft\\AniEdit";
TCHAR gszKeyCurEditor[] = "Editor";
TCHAR gszKeyPrevColor[] = "Preview Color";


/****************************************************************************
*
*     FUNCTION: WinMain(HANDLE, HANDLE, LPSTR, int)
*
*     PURPOSE: calls initialization function, processes message loop
*
*     COMMENTS:
*
*         Windows recognizes this function by name as the initial entry point
*         for the program.  This function calls the application initialization
*         routine, if no other instance of the program is running, and always
*         calls the instance initialization routine.  It then executes a message
*         retrieval and dispatch loop that is the top-level control structure
*         for the remainder of execution.  The loop is terminated when a WM_QUIT
*         message is received, at which time this function exits the application
*         instance by returning the value passed by PostQuitMessage().
*
*         If this function must abort before entering the message loop, it
*         returns the conventional value NULL.
*
* History:
*   21-Apr-1993 JonPa   Created it
*
\****************************************************************************/

int APIENTRY WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
    )
{

    MSG msg;                                 /* message                      */

    UNREFERENCED_PARAMETER( lpCmdLine );

    if (!hPrevInstance)                  /* Other instances of app running? */
        if (!InitApplication(hInstance)) /* Initialize shared things        */
            return (FALSE);              /* Exits if unable to initialize   */

    /* Perform initializations that apply to a specific instance */
    if (!InitInstance(hInstance, nCmdShow))
        return (FALSE);


    DialogBox( hInstance, MAKEINTRESOURCE(DLG_MAIN), GetDesktopWindow(),
            MainWndProc );


    /* Write user profile */
    WriteRegistry();

    //BUGBUG - unregister preview class

    if (gszModPath[0] != TEXT('\0')) {
        RemoveFontResource(gszModPath);
        PostMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
    }


    DeleteObject(ghbrPrevBackgnd);

    /* Return the value from PostQuitMessage */
    return (msg.wParam);
}


/****************************************************************************
*
*     FUNCTION: InitApplication(HANDLE)
*
*     PURPOSE: Initializes window data and registers window class
*
*     COMMENTS:
*
*         This function is called at initialization time only if no other
*         instances of the application are running.  This function performs
*         initialization tasks that can be done once for any number of running
*         instances.
*
*         In this case, we initialize a window class by filling out a data
*         structure of type WNDCLASS and calling the Windows RegisterClass()
*         function.  Since all instances of this application use the same window
*         class, we only need to do this when the first instance is initialized.
*
*
\****************************************************************************/

BOOL InitApplication(HANDLE hInstance)       /* current instance             */
{
    WNDCLASS cls;

    /*
     * Register a new window class to handle the cursor preview.
     */
    cls.style = 0;
    cls.lpfnWndProc = PreviewWndProc;
    cls.cbClsExtra = 0;
    cls.cbWndExtra = 0;
    cls.hInstance = hInstance;
    cls.hIcon = NULL;
    cls.hCursor = NULL;
    cls.hbrBackground = NULL;
    cls.lpszMenuName = NULL;
    cls.lpszClassName = szPREVIEW;
    RegisterClass(&cls);

    AniAddFontModule(hInstance);

    return TRUE;
}


void AniAddFontModule(HINSTANCE hInst) {


    if (GetModuleFileName(hInst, gszModPath, MAX_PATH))
        AddFontResource(gszModPath);
    else
        gszModPath[0] = TEXT('\0');
}


/****************************************************************************
*
*     FUNCTION:  InitInstance(HANDLE, int)
*
*     PURPOSE:  Saves instance handle and creates main window
*
*     COMMENTS:
*
*         This function is called at initialization time for every instance of
*         this application.  This function performs initialization tasks that
*         cannot be shared by multiple instances.
*
*         In this case, we save the instance handle in a static variable and
*         create and display the main program window.
*
\****************************************************************************/

BOOL InitInstance(
    HANDLE          hInstance,
    int             nCmdShow)
{

    /* Save the instance handle in static variable, which will be used in  */
    /* many subsequence calls from this application to Windows.            */

    hInst = hInstance;

    gcyCursor = GetSystemMetrics(SM_CYCURSOR);
    gcxCursor = GetSystemMetrics(SM_CXCURSOR);


    /* Load user profile */
    ReadRegistry();

#if 0
    /* Load the accel table */
    if (!(haccel = LoadAccelerators(hInstance, "AniEditAccel")))
        return FALSE;
#endif

    return (TRUE);               /* Returns the value from PostQuitMessage */

}

/* Copied from winfile:
 */
INT  APIENTRY GetHeightFromPoints( int pts)
{
    HDC hdc;
    INT height;

    hdc = GetDC (NULL);
    height = MulDiv(-pts, GetDeviceCaps (hdc, LOGPIXELSY), 72);
    ReleaseDC (NULL, hdc);

    return height;
}



/****************************************************************************
*
*     FUNCTION: MainWndProc(HWND, unsigned, WORD, LONG)
*
*     PURPOSE:  Processes messages
*
*     MESSAGES:
*
*         WM_COMMAND    - application menu (About dialog box)
*         WM_DESTROY    - destroy window
*
*     COMMENTS:
*
*         To process the IDM_ABOUT message, call MakeProcInstance() to get the
*         current instance address of the About() function.  Then call Dialog
*         box which will create the box according to the information in your
*         aniedit.rc file and turn control over to the About() function.  When
*         it returns, free the intance address.
*
* History:
*   21-Apr-1993 JonPa   Created it
*
\****************************************************************************/

BOOL APIENTRY MainWndProc(
        HWND hWnd,                /* window handle                   */
        UINT message,             /* type of message                 */
        UINT wParam,              /* additional information          */
        LONG lParam)              /* additional information          */
{
    static HWND     hwndChildApp = NULL;
    static HBRUSH   hbrBtnBar;
    static HFONT    hfontButton;

    switch (message) {
    case WM_INITDIALOG:
        ghwndMain = hWnd;

        gcrHighlightText = GetSysColor(COLOR_HIGHLIGHTTEXT);
        ghbrHighlight = CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT));
        ghbrWindow = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
        hbrBtnBar = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));

        hfontButton = CreateFont (GetHeightFromPoints(8), 0, 0, 0, 400,
                                0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                                CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                                DEFAULT_PITCH | FF_SWISS,
                                TEXT("AniEdit Button"));

        SendDlgItemMessage (hWnd, DLG_MAIN_BTNNEW, WM_SETFONT, (WPARAM)hfontButton, 0L);
        SendDlgItemMessage (hWnd, DLG_MAIN_BTNOPEN, WM_SETFONT, (WPARAM)hfontButton, 0L);
        SendDlgItemMessage (hWnd, DLG_MAIN_BTNSAVE, WM_SETFONT, (WPARAM)hfontButton, 0L);

        SendDlgItemMessage (hWnd, DLG_MAIN_BTNCUT, WM_SETFONT, (WPARAM)hfontButton, 0L);
        SendDlgItemMessage (hWnd, DLG_MAIN_BTNCOPY, WM_SETFONT, (WPARAM)hfontButton, 0L);
        SendDlgItemMessage (hWnd, DLG_MAIN_BTNPASTE, WM_SETFONT, (WPARAM)hfontButton, 0L);

        SendDlgItemMessage (hWnd, DLG_MAIN_DELFRAME, WM_SETFONT, (WPARAM)hfontButton, 0L);

        SendDlgItemMessage (hWnd, DLG_MAIN_ADDFRAME, WM_SETFONT, (WPARAM)hfontButton, 0L);
        SendDlgItemMessage (hWnd, DLG_MAIN_EDITFRAME, WM_SETFONT, (WPARAM)hfontButton, 0L);

        SendDlgItemMessage (hWnd, DLG_MAIN_PLAY, WM_SETFONT, (WPARAM)hfontButton, 0L);
        SendDlgItemMessage (hWnd, DLG_MAIN_STOP, WM_SETFONT, (WPARAM)hfontButton, 0L);

        SendDlgItemMessage (hWnd, DLG_MAIN_FRAMETXT, WM_SETFONT, (WPARAM)hfontButton, 0L);



        GetWindowText(hWnd, gszWindowTitle, COUNTOF(gszWindowTitle));

        /* cache scroll bar window handle */
        ghwndRateScroll = GetDlgItem(hWnd, DLG_MAIN_RATESPIN);

        /* limit title and author string lengths */

        SendDlgItemMessage(hWnd, DLG_MAIN_TITLE, EM_LIMITTEXT,
                COUNTOF(ganiAcon.azTitle), 0);

        SendDlgItemMessage(hWnd, DLG_MAIN_AUTHOR, EM_LIMITTEXT,
                COUNTOF(ganiAcon.azCreator), 0);

        NewAniCursor(hWnd);
        return (TRUE);

    case WM_COMMAND:           /* message: command from application menu */
        return DoCommand( hWnd, wParam, lParam );


    case AIM_SETCHILDAPP:
        /*
         * A child app has just been started.  Remeber its hwnd and defer
         * all activation to it.
         */
        DPRINT(("MT:Child HWND = 0x%lx\n", lParam));
        hwndChildApp = (HWND)lParam;

        if (hwndChildApp == NULL)
            EnableWindow(hWnd, FALSE);

        break;

    case AIM_PROCESSTERM:
        /*
         * The copy of ImagEdit that we spanwed off has just ended.
         * Time to read in the cursor file and put it back into the list.
         */

        /* "enable" our window */
        DPRINT(("MT:got AIM_PROCESSTERM\n"));

        hwndChildApp = NULL;

        EnableWindow(hWnd, TRUE);

        SetForegroundWindow(hWnd);

        /* call CreateFrameFromCursorFile to reload the modified cursor */
        if(CreateFrameFromCursorFile(hWnd, gszTempFile,  gfEditFrame))
            DeleteFile(gszTempFile);

        break;

    case WM_ACTIVATE:
        /*
         * Convert WM_ACTIVATE to WM_NCACTIVATE
         */
        switch (LOWORD(wParam)) {

        case WA_CLICKACTIVE:
            /*
             * Simulate disabled window's beep
             */
            if (IsWindow( hwndChildApp ))
                MessageBeep(MB_OK);

            wParam = TRUE;
            break;

        case WA_ACTIVE:
            wParam = TRUE;
            break;

        default:
            wParam = FALSE;
            break;
        }

        FALLTHRU(WM_NCACTIVATE);

    case WM_NCACTIVATE:
    case WM_ACTIVATEAPP:
        DPRINT(("MT:got Activate (%04x) %c %08x\n", message, wParam ? 'T' : 'F', lParam));

        if (wParam == TRUE && IsWindow( hwndChildApp )) {
            /*
             * We have a 'modal' child app upp, defer the activation to it.
             */
            DPRINT(("MT:Defering Now\n"));
            return SetForegroundWindow(hwndChildApp);
        }

        /*
         * Let DefWndProc process this message
         */
        return FALSE;

    case WM_MEASUREITEM:
        ((MEASUREITEMSTRUCT *)lParam)->itemHeight = gcyCursor + 2;
        break;

    case WM_DRAWITEM:
        DrawCursorListItem((DRAWITEMSTRUCT *)lParam);
        break;


    case WM_DELETEITEM: {
        PSTEP ps;

        if (wParam != DLG_MAIN_FRAMELIST)
            return FALSE;

        ps = (PSTEP)((LPDELETEITEMSTRUCT)lParam)->itemData;

        if (IsValidPS(ps)) {
            DestroyStep(ps);
        }
        break;
    }


    case WM_VSCROLL:
        if( (HWND)lParam == ghwndRateScroll ) {
            LONG iDelta;

            switch( LOWORD(wParam) ) {
            case SB_LINEUP:
            case SB_PAGEUP:
                iDelta = 1;
                break;

            case SB_LINEDOWN:
            case SB_PAGEDOWN:
                iDelta = -1;
                break;

            default:
                iDelta = 0;
            }

            if (iDelta != 0) {
                BOOL fOK;
                JIF jifRate = GetDlgItemInt(hWnd, DLG_MAIN_RATE, &fOK, FALSE);

                if( fOK ) {
                    if ((jifRate += iDelta) != 0) {
                        int *piSel, cSel;

                        SetDlgItemInt(hWnd, DLG_MAIN_RATE, jifRate, FALSE);

                        cSel = GetSelStepCount(hWnd);

                        if (cSel > 0 && (piSel = AllocMem(cSel * sizeof(int))) !=
                                NULL) {
                            int i;

                            ganiAcon.fDirty = TRUE;
                            GetCurrentSel(hWnd, DLG_MAIN_FRAMELIST, piSel, cSel,
                                    &cSel);

                            for( i = 0; i < cSel; i++ ) {
                                PSTEP ps = GetStep(hWnd, piSel[i]);
                                if (IsValidPS(ps)) {
                                    ps->jif = jifRate;
                                }
                            }

                            InvalidateRect(GetDlgItem(hWnd, DLG_MAIN_FRAMELIST),
                                    NULL, TRUE);
                            FreeMem(piSel);
                        }
                    }
                } else {
                    int *piSel, cSel, i;
                    JIF jifMin, jifTmp;

                    cSel = GetSelStepCount(hWnd);

                    if (cSel > 0 && (piSel = AllocMem(cSel * sizeof(int))) !=
                            NULL) {

                        ganiAcon.fDirty = TRUE;
                        GetCurrentSel(hWnd, DLG_MAIN_FRAMELIST, piSel, cSel,
                                &cSel);

                        jifMin = MAXLONG;

                        for( i = 0; i < cSel; i++ ) {
                            PSTEP ps = GetStep(hWnd, piSel[i]);
                            if (IsValidPS(ps)) {
                                jifMin = min(jifMin, ps->jif);
                            }
                        }

                        for( i = 0; i < cSel; i++ ) {
                            PSTEP ps = GetStep(hWnd, piSel[i]);
                            if (IsValidPS(ps)) {

                                jifTmp = ps->jif;

                                if (iDelta == 1) {
                                    ps->jif += (ps->jif / jifMin);
                                } else {
                                    ps->jif -= (ps->jif / jifMin);
                                }

                                /* check for over/under-flow */
                                if (ps->jif == 0) {
                                    ps->jif = jifTmp;
                                }
                            }
                        }

                        InvalidateRect(GetDlgItem(hWnd, DLG_MAIN_FRAMELIST),
                                NULL, TRUE);
                        FreeMem(piSel);
                    }
                }
            }
        }
        break;

    case WM_SYSCOMMAND:
        if (wParam == SC_CLOSE) {
            ExitCommand(hWnd);
        } else {
            return FALSE;
        }

        break;

    case WM_SYSCOLORCHANGE:
        DeleteObject(ghbrPrevBackgnd);
        DeleteObject(ghbrWindow);
        DeleteObject(ghbrHighlight);
        DeleteObject(hbrBtnBar);

        ghbrHighlight = CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT));
        gcrHighlightText = GetSysColor(COLOR_HIGHLIGHTTEXT);
        ghbrWindow = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
        ghbrPrevBackgnd = CreateSolidBrush(GetSysColor(
                garadColor[giradColor].idSys));

        hbrBtnBar = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
        break;

    case WM_ERASEBKGND: {
        RECT rc;
        GetClientRect(hWnd, &rc);
        FillRect((HDC)wParam, &rc, ghbrWindow);

        GetWindowRect(GetDlgItem(hWnd, DLG_MAIN_BTNBAR), &rc);
        ScreenToClient(hWnd, (LPPOINT)&(rc.left));
        ScreenToClient(hWnd, (LPPOINT)&(rc.right));
        FillRect((HDC)wParam, &rc, hbrBtnBar);

        break;
    }

    case WM_DESTROY:
        DeleteObject(ghbrPrevBackgnd);
        DeleteObject(ghbrWindow);
        DeleteObject(ghbrHighlight);
        DeleteObject(hbrBtnBar);
        DeleteObject(hfontButton );
        return FALSE;


    default:
        return FALSE;
    }

    return TRUE;
}


/***************************************************************************\
*
* DrawCursorListItem
*
*
* History:
* 22-Dec-1991 DarrinM       Created in the Cursors cpa.
* 22-Apr-1993 JonPa         copied into this app and tweeked it.
\***************************************************************************/

void DrawCursorListItem(
    DRAWITEMSTRUCT *pdis)
{
    COLORREF crPrev;
    static LONG cxAvgChar = 0;
    TEXTMETRIC tm;
    PSTEP ps;
    TCHAR szJif[CCH_JIF];

    /*
     * LATER: Deal with focus rect.
     */
    if (pdis->itemAction == ODA_FOCUS)
        return;


    /* find the average char width for this listbox and cache it */
    if (cxAvgChar == 0) {
        if (GetTextMetrics( pdis->hDC, &tm)) {
            cxAvgChar = tm.tmAveCharWidth;
        }
    }

    ps = (PSTEP)(pdis->itemData);

    SetBkMode(pdis->hDC, TRANSPARENT);

    if (pdis->itemState & ODS_SELECTED) {
        FillRect(pdis->hDC, &pdis->rcItem, ghbrHighlight);
        crPrev = SetTextColor(pdis->hDC, gcrHighlightText);
    } else {
        FillRect(pdis->hDC, &pdis->rcItem, ghbrWindow);
    }


    /* Draw the frame */
    DrawIcon(pdis->hDC, pdis->rcItem.left + 2, pdis->rcItem.top + 1,
            ps->pfrmFrame->hcur);


    pdis->rcItem.left += gcxCursor + 2 + ((cxAvgChar != 0) ? cxAvgChar : 8);


    /* write the rate text */
    wsprintf( szJif, "%d", (int)ps->jif );

    DrawText(pdis->hDC, szJif, strlen(szJif), &pdis->rcItem,
            DT_SINGLELINE | DT_LEFT | DT_VCENTER);


    if (pdis->itemState & ODS_SELECTED) {
        SetTextColor(pdis->hDC, crPrev);
    }
}





/***************************************************************************\
*
*     FUNCTION: FmtMessageBox( HWND hwnd, DWORD dwTitleID, UINT fuStyle,
*                   BOOL fSound, DWORD dwTextID, ... );
*
*     PURPOSE:  Formats messages with FormatMessage and then displays them
*               in a message box
*
*
*
*
* History:
* 22-Apr-1993 JonPa         Created it.
\***************************************************************************/
int FmtMessageBox( HWND hwnd, DWORD dwTitleID, LPTSTR pszTitleStr,
    UINT fuStyle, BOOL fSound, DWORD dwTextID, ... ) {
    LPTSTR pszMsg;
    LPTSTR pszTitle;
    int idRet;

    va_list marker;

    va_start( marker, dwTextID );

    if(!FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_MAX_WIDTH_MASK, hInst,
            dwTextID, 0, (LPTSTR)&pszMsg, 1, &marker))
        pszMsg = gpszUnknownError;

    va_end( marker );

    GetLastError();

    pszTitle = NULL;

    if (dwTitleID == (DWORD)-1 ||
            FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_MAX_WIDTH_MASK |
                FORMAT_MESSAGE_ARGUMENT_ARRAY,
                hInst, dwTitleID, 0, (LPTSTR)&pszTitle, 1, &pszTitleStr)) {
    }

    GetLastError();

    if (fSound) {
        MessageBeep( fuStyle & (MB_ICONASTERISK | MB_ICONEXCLAMATION |
                MB_ICONHAND | MB_ICONQUESTION | MB_OK) );
    }

    idRet = MessageBox(hwnd, pszMsg, pszTitle, fuStyle);

    if (pszTitle != NULL)
        LocalFree( pszTitle );

    if (pszMsg != gpszUnknownError)
        LocalFree( pszMsg );

    return idRet;
}

/***************************************************************************\
*
*     FUNCTION: FmtSprintf( DWORD id, ... );
*
*     PURPOSE:  sprintf but it gets the pattern string from the message rc.
*
* History:
* 03-May-1993 JonPa         Created it.
\***************************************************************************/
LPTSTR FmtSprintf( DWORD id, ... ) {
    LPTSTR pszMsg;
    va_list marker;

    va_start( marker, id );

    if(!FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_MAX_WIDTH_MASK, hInst,
            id, 0, (LPTSTR)&pszMsg, 1, &marker)) {
        GetLastError();
        pszMsg = gszDots;
    }
    va_end( marker );

    return pszMsg;
}

/***************************************************************************\
*
*     FUNCTION: PVOID AllocMem( DWORD cb );
*
*     PURPOSE:  allocates memory, checking for errors
*
* History:
*   22-Apr-1993 JonPa   Wrote it.
\***************************************************************************/
PVOID AllocMem( DWORD cb ) {
    PVOID pv = (PVOID)LocalAlloc(LPTR, cb);

    if (pv == NULL) {
        FmtMessageBox( ghwndMain, TITL_ERROR, NULL, MB_OK | MB_ICONSTOP,
                TRUE, MSG_OUTOFMEM );
    }

    return pv;
}


/***************************************************************************\
* PreviewWndProc
*
*
* History:
* 08-07-92 DarrinM      Created in CURSORS.CPL.
* 24-Apr-1993 JonPa     Copied here and tweeked.
\***************************************************************************/

LRESULT CALLBACK
PreviewWndProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam
    )
{
    HDC hdc;
    RECT rc;
    PAINTSTRUCT ps;
    PPREVIEWDATA ppd;
    static int cxBM, cyBM;
    static int cxCenter, cyCenter;

    switch (msg) {
    case WM_CREATE:
        if (!(ppd = (PPREVIEWDATA)LocalAlloc(LPTR, sizeof(PREVIEWDATA))))
            return -1;

        SetWindowLong(hwnd, GWL_USERDATA, (LONG)ppd);

        /*
         * Create a temp DC and bitmap to be used for buffering the
         * preview rendering.
         */
        cxCenter = gcxCursor;
        cyCenter = gcyCursor;

        cxBM = cxCenter * 2;
        cyBM = cyCenter * 2;

        hdc = GetDC(hwnd);
        ppd->hdcMem = CreateCompatibleDC(hdc);
        ppd->hbmMem = CreateCompatibleBitmap(hdc, cxBM, cyBM);
        ppd->hbmOld = SelectObject(ppd->hdcMem, ppd->hbmMem);
        ppd->iFrame = 0;
        ppd->hcur = NULL;
        ppd->xHot = ppd->yHot = 0;
        ReleaseDC(hwnd, hdc);
        break;

    case WM_SIZE:
        ppd = (PPREVIEWDATA)GetWindowLong(hwnd, GWL_USERDATA);

        SelectObject(ppd->hdcMem, ppd->hbmOld);
        DeleteObject(ppd->hbmMem);

        cxBM = LOWORD(lParam);
        cyBM = HIWORD(lParam);
        cxCenter = cxBM / 2;
        cyCenter = cyBM / 2;

        hdc = GetDC(hwnd);
        ppd->hbmMem = CreateCompatibleBitmap(hdc, cxBM, cyBM);
        ppd->hbmOld = SelectObject(ppd->hdcMem, ppd->hbmMem);
        ReleaseDC(hwnd, hdc);
        break;

    case WM_DESTROY:
        ppd = (PPREVIEWDATA)GetWindowLong(hwnd, GWL_USERDATA);
        SelectObject(ppd->hdcMem, ppd->hbmOld);
        DeleteObject(ppd->hbmMem);
        DeleteDC(ppd->hdcMem);
        LocalFree(ppd);
        break;

    case PM_PAUSEANIMATION:
        KillTimer(hwnd, ID_PREVIEWTIMER);
        break;

    case PM_UNPAUSEANIMATION:
        NextFrame(hwnd, TRUE);
        break;

    case PM_NEWCURSOR:
        wParam = 0;
        FALLTHRU(PM_SETSTEP);

    case PM_SETSTEP: {
        BOOL fRun = KillTimer(hwnd, ID_PREVIEWTIMER);

        ppd = (PPREVIEWDATA)GetWindowLong(hwnd, GWL_USERDATA);

        ppd->iFrame = wParam;

        NextFrame(hwnd, fRun);
        InvalidateRect(hwnd, NULL, FALSE);
        break;
    }

    case WM_TIMER:
        if (wParam != ID_PREVIEWTIMER)
            break;

        NextFrame(hwnd, TRUE);
        break;

    case WM_PAINT:
        BeginPaint(hwnd, &ps);

        ppd = (PPREVIEWDATA)GetWindowLong(hwnd, GWL_USERDATA);

        if (ppd->hcur != NULL)
        {
            rc.left = rc.top = 0;
            rc.right = cxBM;
            rc.bottom = cyBM;
            FillRect(ppd->hdcMem, &rc, ghbrPrevBackgnd);


            DrawIcon(ppd->hdcMem, cxCenter - ppd->xHot, cyCenter - ppd->yHot,
                    ppd->hcur);

            BitBlt(ps.hdc, 0, 0, cxBM, cyBM, ppd->hdcMem, 0, 0, SRCCOPY);
        }
        else
        {
            FillRect(ps.hdc, &ps.rcPaint, ghbrPrevBackgnd);
        }

        EndPaint(hwnd, &ps);
        break;

    case WM_ERASEBKGND:
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}

/*****************************************************************************\
* NextFrame
*
* Sets up for the next frame in the preview window.
*
* Arguments:
*   HWND hwnd - Dialog window handle.
*
\*****************************************************************************/

VOID
NextFrame(
    HWND hwnd, BOOL fRun
    )
{
    PPREVIEWDATA ppd;
    HWND hwndLB;
    DWORD cFrame;
    PSTEP ps;

    ppd = (PPREVIEWDATA)GetWindowLong(hwnd, GWL_USERDATA);

    //
    // Be sure there is a cursor specified.  If not, or it is
    // not an animated cursor, we are done.
    //
    hwndLB = GetDlgItem(GetParent(hwnd), DLG_MAIN_FRAMELIST);
    cFrame = SendMessage(hwndLB, LB_GETCOUNT, 0, 0);
    if (cFrame == LB_ERR || cFrame == 0) {
        ppd->hcur = NULL;
        InvalidateRect(hwnd, NULL, FALSE);
        return;
    }

    if (ppd->iFrame >= cFrame)
        ppd->iFrame = 0;

    /*
     * Find how long this frame should be displayed (i.e. get jifRate)
     */
    ps = (PSTEP)SendMessage(hwndLB, LB_GETITEMDATA, ppd->iFrame, 0);

    if (IsValidPS(ps)) {
        ppd->xHot = ps->pfrmFrame->xHotSpot;
        ppd->yHot = ps->pfrmFrame->yHotSpot;

        ppd->hcur = ps->pfrmFrame->hcur;

        if (fRun)
            SetTimer(hwnd, ID_PREVIEWTIMER, ps->jif * 16, NULL);

        ppd->iFrame += 1;
    } else {
        ppd->hcur = NULL;
    }

    /*
     * Redraw this frame of the cursor.
     */
    InvalidateRect(hwnd, NULL, FALSE);
}

/*****************************************************************************\
* ReadRegistry
*
* Opens (creates if necessary) the registry key for preferences and then
* reads the last saved values.
*
*   03-Jul-1993 JonPa   Copied from Spy, but changed greatly
*
\*****************************************************************************/

VOID ReadRegistry( VOID ) {
    DWORD dw;
    DWORD cbData;
    HKEY hkey;

    if (RegOpenKeyEx(HKEY_CURRENT_USER, gszAppKey, 0, KEY_QUERY_VALUE, &hkey)){

        lstrcpy( gszCursorEditor, gpszImagEdit );

    } else {

        cbData = sizeof(gszCursorEditor);
        if (RegQueryValueEx(hkey, gszKeyCurEditor, NULL, NULL,
            gszCursorEditor, &cbData) != ERROR_SUCCESS) {

            lstrcpy( gszCursorEditor, gpszImagEdit );

        }

        cbData = sizeof(dw);
        if (RegQueryValueEx(hkey, gszKeyPrevColor, NULL, NULL, (LPBYTE)&dw,
                &cbData) == ERROR_SUCCESS) {

            giradColor = (int)dw;
        }

        RegCloseKey(hkey);
    }

    ghbrPrevBackgnd = CreateSolidBrush(GetSysColor(garadColor[giradColor].idSys));
}



/*****************************************************************************\
* WriteRegistry
*
* Writes out preference data to the registry when the app exits, then
* closes the registry key.
*
*   03-Jul-1993 JonPa   Copied from Spy, but changed greatly
\*****************************************************************************/

VOID WriteRegistry( VOID ) {
    HKEY hkey;
    DWORD dw;

    if (RegCreateKeyEx(HKEY_CURRENT_USER, gszAppKey, 0, NULL,
                REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hkey, &dw))
        return;

    RegSetValueEx(hkey, gszKeyCurEditor, 0, REG_SZ, gszCursorEditor,
            lstrlen(gszCursorEditor)+1);

    dw = giradColor;
    RegSetValueEx(hkey, gszKeyPrevColor, 0, REG_DWORD, (LPBYTE)&dw, sizeof(dw));

    RegCloseKey(hkey);

}
