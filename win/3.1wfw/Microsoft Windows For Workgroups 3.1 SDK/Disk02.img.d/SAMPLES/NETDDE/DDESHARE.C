/*****************************************************************************\
*                                                                             *
* ddeshare.c -  WFWRK Network DDE Share Manager                               *
*                                                                             *
*               Version 3.1                                                   *
*                                                                             *
*        Copyright (c) 1992-1993, Microsoft Corp.  All rights reserved.       *
*                                                                             *
\*****************************************************************************/

#define  STRICT

#include "windows.h"
#include "ddeshare.h"
#include "nddeapi.h"
#include "dialogs.h"


/* function declarations from dialogs.c */
extern BOOL FAR PASCAL AddShareDlg(HWND hDlg, unsigned message, WORD wParam,
    LONG lParam);
extern BOOL FAR PASCAL AboutDlg(HWND hDlg, UINT message, WPARAM wParam,
    LPARAM lParam);

/* forward declarations in this file */
extern VOID RefreshShareWindow ( VOID );
extern VOID ShowErrMsg ( HWND hwnd, char * s, UINT code );

HINSTANCE hInst;               /* global copy of instance handle */
HWND      hwndListBox;         /* handle of listbox filling application */
HWND      hwndApp;             /* global copy of main app window handle */
HACCEL    hAccel;              /* handle to accelerator table */

#define SMLBUF    128

char    szBuf[SMLBUF];         /* big enough for long text line */
char    szBuf2[SMLBUF];         /* big enough for long text line */

int PASCAL WinMain ( HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmdLine,
                                                        int nCmdShow)
{
    MSG msg;                     

    if (!hPrev)             
        if (!InitApplication(hInstance)) 
            return (FALSE);         

    if (!InitInstance(hInstance, nCmdShow))
        return (FALSE);

    while ( GetMessage(&msg, NULL, NULL, NULL) ) {
        if ( !TranslateAccelerator( hwndApp, hAccel, &msg ) ) {
            TranslateMessage(&msg);       
            DispatchMessage(&msg);       
        }
    }
    return (msg.wParam);       
}


BOOL InitApplication( HINSTANCE hInstance )
{
    WNDCLASS  wc;

    wc.style = NULL;            
    wc.lpfnWndProc = (WNDPROC) MainWndProc;       
                    
    wc.cbClsExtra = 0;          
    wc.cbWndExtra = 0;          
    wc.hInstance = hInstance;       
    wc.hIcon = LoadIcon( hInstance, MAKEINTRESOURCE(ID_ICON) );
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName =  "DdeShareMenu";   
    wc.lpszClassName = "DdeShareWClass"; 
    return (RegisterClass(&wc));
}


BOOL InitInstance( HINSTANCE hInstance, int nCmdShow )
{
    HWND        hWnd;           

    hInst = hInstance;

    hAccel = LoadAccelerators( hInstance, MAKEINTRESOURCE(ID_ACCS) );

    hwndApp = hWnd = CreateWindow(
        "DdeShareWClass",        
        "Network DDE Share Manager",   
        WS_OVERLAPPEDWINDOW,        
        CW_USEDEFAULT,          
        CW_USEDEFAULT,          
        300,          
        220,          
        NULL,               
        NULL,               
        hInstance,              
        NULL                
    );

    if (!hWnd)
        return FALSE;

    ShowWindow(hWnd, nCmdShow);  
    UpdateWindow(hWnd);      
    return TRUE;           
}


long FAR PASCAL MainWndProc(hWnd, message, wParam, lParam)
HWND hWnd;                  
unsigned message;              
WORD wParam;                  
LONG lParam;                  
{
    DLGPROC lpDlgProc;
    int idx;

    switch (message) {

    case WM_CREATE:
        /* create listbox (which will fill client area */
        hwndListBox = CreateWindow (
            "listbox",
            NULL,
            WS_CHILD|LBS_STANDARD|LBS_DISABLENOSCROLL|LBS_NOINTEGRALHEIGHT,
            0, 0, 0, 0,
            hWnd,
            (HMENU)IDLISTBOX,
            hInst,
            NULL
        );
        ShowWindow ( hwndListBox, SW_SHOW );
        RefreshShareWindow();
        break;

    case WM_SETFOCUS:
        SetFocus ( hwndListBox );
        break;

    case WM_SIZE:
        /* size listbox to fill client area */
        MoveWindow ( hwndListBox, -1, -1,
            LOWORD(lParam)+2, HIWORD(lParam)+2, TRUE);
        break;

    case WM_INITMENU:
        /* only allow del and getinfo if an entry is selected */
        idx = (int)SendMessage( hwndListBox,LB_GETCURSEL,0,0L );
        EnableMenuItem( (HMENU)wParam, IDM_DELETE, idx != LB_ERR ? FALSE:TRUE );
        EnableMenuItem( (HMENU)wParam, IDM_PROPERTIES,
            idx != LB_ERR ? FALSE:TRUE );
        break;

    case WM_COMMAND:       
        switch ( wParam ) {

        case IDM_ABOUT:
            /* vanilla about box */
            if ( lpDlgProc = (DLGPROC)MakeProcInstance( (FARPROC)AboutDlg,
                                                                hInst )) {
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUT), hWnd,
                                            (DLGPROC)lpDlgProc);
                FreeProcInstance( (FARPROC)lpDlgProc );
            }
            break;

        case IDM_ADDSHARE:
            /* just call the dialog box proc with a NULL param */
            if ( lpDlgProc = (DLGPROC)MakeProcInstance( (FARPROC)AddShareDlg,
                                                                hInst )) {
                if ( DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_ADDSHARE),
                                            hWnd, lpDlgProc, 0L ) )
                    RefreshShareWindow();
                FreeProcInstance( (FARPROC)lpDlgProc);
            }
            break;

        case IDM_DELETE:
            /* use NDdeShareDel to delete the selected share */
            idx = (int)SendMessage( hwndListBox,LB_GETCURSEL,0,0L );
            if ( idx == LB_ERR )
                break; /* just in case - menu should not allow this */

            SendMessage(hwndListBox, LB_GETTEXT, idx, (LPARAM)(LPSTR)szBuf );

            /* ask for confirmation */
            wsprintf(szBuf2, "Really Delete \"%s\"?", (LPSTR)szBuf );
            MessageBeep(MB_ICONEXCLAMATION);
            if ( MessageBox ( hWnd, szBuf2, "Delete",
                                MB_YESNO | MB_ICONEXCLAMATION ) == IDNO )
                break;

            /* make the call and report error if any */
            ShowErrMsg ( hWnd, "Deleting Share",
                                NDdeShareDel ( NULL, szBuf, 0 ) );
            RefreshShareWindow();
            break;

        case IDM_PROPERTIES:
            /* call dialog proc with ptr to name of share */
            idx = (int)SendMessage( hwndListBox,LB_GETCURSEL,0,0L );
            if ( idx == LB_ERR )
                break;
            SendMessage(hwndListBox,LB_GETTEXT, idx, (LPARAM)(LPSTR)szBuf );
            if ( lpDlgProc = (DLGPROC)MakeProcInstance( (FARPROC)AddShareDlg,
                                                                hInst )) {
                if (DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_ADDSHARE),
                            hWnd, lpDlgProc, (LPARAM)(LPSTR)szBuf))
                    RefreshShareWindow();
                FreeProcInstance( (FARPROC)lpDlgProc );
            }
            break;

        case IDM_REFRESH:
            RefreshShareWindow();
            break;

        case IDLISTBOX:
            /* capture double clicks as properties shortcut */
            switch ( HIWORD(lParam) ) {
                case LBN_DBLCLK:
                    SendMessage ( hWnd, WM_COMMAND, IDM_PROPERTIES, 0L );
                break;
            }
            break;

        case IDM_EXIT:
            SendMessage ( hWnd, WM_CLOSE, 0, 0L );
            break;
        }
        break;

    case WM_DESTROY:          
        PostQuitMessage(0);
        break;

    default:              
        return (DefWindowProc(hWnd, message, wParam, lParam));
    }
    return (NULL);
}

/* This function empties the listbox, enumerates shares and fills
   the listbox with the share names. It should be called whenever
   the list of shares changes. */

VOID RefreshShareWindow ( VOID )
{
    UINT RetCode;
    DWORD entries;
    DWORD avail;
    LPSTR s;
    HANDLE h;
    
    /* call with 0 buffer to see how much space we need */
    RetCode = NDdeShareEnum ( NULL, 0, NULL, 0, &entries, &avail );

    if ( RetCode == NDDE_BUF_TOO_SMALL ) { /* we expect this error */
        if ( h = GlobalAlloc ( GHND, avail ) ) {
            /* we got the handle */
            if ( s = GlobalLock ( h ) ) {
                /* we got a pointer, make the call with real buffer  */
                RetCode = NDdeShareEnum ( NULL, 0, s, avail, &entries, &avail );

                /* show err msg if any */
                ShowErrMsg ( hwndApp, "Enumerating Shares", RetCode );

                /* if OK, empty listbox and step through buffer, adding */
                if ( RetCode == NDDE_NO_ERROR ) {
                    SendMessage ( hwndListBox, LB_RESETCONTENT, 0, 0L );
                    for ( ; *s; s += lstrlen(s) + 1 ) {
                        SendMessage(hwndListBox, LB_ADDSTRING, 0, (LPARAM)s );
                    }
                }
                GlobalUnlock(h);
            }
            GlobalFree(h);
        }
    }
    /* else do nothing - internal error or no shares */
}

/* This function pops up a messagebox with text obtained by
   calling NDdeGetErrorString if an error code is passed in */

VOID ShowErrMsg ( HWND hwnd, char * s, UINT code )
{
    if ( code == NDDE_NO_ERROR )
        return;

    MessageBeep(MB_ICONEXCLAMATION);
    if ( NDdeGetErrorString ( code, szBuf, SMLBUF ) == NDDE_NO_ERROR )
        MessageBox ( hwnd, szBuf, s, MB_ICONEXCLAMATION | MB_OK );
}

