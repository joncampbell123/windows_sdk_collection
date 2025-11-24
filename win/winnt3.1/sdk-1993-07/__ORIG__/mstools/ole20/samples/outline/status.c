/*************************************************************************
** 
**    OLE 2.0 Sample Code
**    
**    status.c
**    
**	  This file contains the window handlers, and various initialization
**	  and utility functions for an application status bar.
**
**	  (c) Copyright Microsoft Corp. 1992 - 1993 All Rights Reserved
**
*************************************************************************/

// Application specific include files
#include "outline.h"
#include "message.h"
#include "status.h"

// Current status message.
static char *szStatusMessage = NULL;

// Holder for string messages.
static char szMessageHold[MAX_MESSAGE];

// Window proc for status window.
LRESULT FAR PASCAL StatusWndProc
   (HWND hwnd, unsigned message, WPARAM wParam, LPARAM lParam);

// List of all constant messages.
static STATMESG ControlList[2] =
{
    {   STATUS_READY,   "Ready."    },
    {   STATUS_BLANK,   " "         }
};

// List of all system menu messages.
static STATMESG SysMenuList[16] =
{
    {   SC_SIZE,        "Change the size of the window."            },
    {   SC_MOVE,        "Move the window."                          },
    {   SC_MINIMIZE,    "Make the window iconic."                   },
    {   SC_MAXIMIZE,    "Make the window the size of the screen."   },
    {   SC_NEXTWINDOW,  "Activate the next window."                 },
    {   SC_PREVWINDOW,  "Activate the previous window."             },
    {   SC_CLOSE,       "Close this window."                        },
    {   SC_VSCROLL,     "Vertical scroll?"                          },
    {   SC_HSCROLL,     "Horizontal scroll?"                        },
    {   SC_MOUSEMENU,   "A menu for mice."                          },
    {   SC_KEYMENU,     "A menu for keys (I guess)."                },
    {   SC_ARRANGE,     "Arrange something."                        },
    {   SC_RESTORE,     "Make the window noramally sized."          },
    {   SC_TASKLIST,    "Put up the task list dialog."              },
    {   SC_SCREENSAVE,  "Save the screen!  Run for your life!"      },
    {   SC_HOTKEY,      "Boy, is this key hot!"                     }
};

// Message type for popup messages.
typedef struct {
    HMENU hmenu;
    char string[MAX_MESSAGE];
} STATPOPUP;

// List of all popup messages.
static STATPOPUP PopupList[NUM_POPUP];

static UINT nCurrentPopup = 0;



/* RegisterStatusClass
 * -------------------
 *
 * Creates classes for status window.
 *
 * HINSTANCE hInstance
 *
 * RETURNS: TRUE if class successfully registered.
 *          FALSE otherwise.
 * 
 * CUSTOMIZATION: Change class name.
 *
 */
BOOL RegisterStatusClass(HINSTANCE hInstance)
{
    WNDCLASS  wc;

    wc.lpszClassName = "ObjStatus";
    wc.lpfnWndProc   = StatusWndProc;
    wc.style         = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = NULL;
    wc.cbClsExtra    = 4;
    wc.cbWndExtra    = 0;
    wc.lpszMenuName  = NULL;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject(LTGRAY_BRUSH);

    if (!RegisterClass(&wc))
        return FALSE;

    return TRUE;
}


/* CreateStatusWindow
 * ------------------
 *
 * Creates status window.
 *
 * HWND hwndMain
 * 
 * RETURNS: HWND of status window if creation is successful.
 *          NULL otherwise.
 *
 * CUSTOMIZATION: Change class name.
 *
 */
HWND CreateStatusWindow(HWND hWndApp, HINSTANCE hInst)
{
    RECT rect;
    int width, height;
    HWND hWndStatusBar;

    szStatusMessage = ControlList[0].string;
    GetClientRect(hWndApp, &rect);
    width = rect.right - rect.left;
    height = rect.bottom - rect.top;

    hWndStatusBar = CreateWindow (
        "ObjStatus",
        "SvrStatus",
        WS_CHILD | WS_VISIBLE,
        0, height - STATUS_HEIGHT,
        width,
        STATUS_HEIGHT,
        hWndApp,
        NULL,
        hInst,
        NULL
    );

    return hWndStatusBar;
}


/* DestroyStatusWindow
 * -------------------
 *
 * Destroys status window.
 *
 * CUSTOMIZATION: None.
 *
 */
void DestroyStatusWindow(HWND hWndStatusBar)
{
    DestroyWindow(hWndStatusBar);
}


/* AssignPopupMessage
 * ------------------
 *
 * Associates a string with a popup menu handle.
 *
 * HMENU hmenuPopup
 * char *szMessage
 *
 * CUSTOMIZATION: None.
 *
 */
void AssignPopupMessage(HMENU hmenuPopup, char *szMessage)
{
    if (nCurrentPopup < NUM_POPUP) {
        PopupList[nCurrentPopup].hmenu = hmenuPopup;
        lstrcpy(PopupList[nCurrentPopup].string, szMessage);
        ++nCurrentPopup;
    }
}

static void WriteStatusMessage(HWND hWndStatusBar, LPVOID lpDoc)
{
#if defined( INPLACE_LATER )
    if (lpDoc && lpDoc->lpIpData != NULL) {
        lpDoc->lpIpData->lpFrame->lpVtbl->SetStatusText
                    (lpDoc->lpIpData->lpFrame, szStatusMessage);
    }
    else 
#endif  
    {
        InvalidateRect (hWndStatusBar, (LPRECT)NULL,  TRUE);
        UpdateWindow (hWndStatusBar);
    }
}

/* ItemMessage
 * -----------
 *
 * Prints message associated with the given item number.
 *
 * UINT wIDItem
 * LPVOID lpDoc
 *
 * CUSTOMIZATION: None.
 *
 */
void ItemMessage(HWND hWndStatusBar, UINT wIDItem, LPVOID lpDoc)
{
    UINT i;

    szStatusMessage = ControlList[1].string;
    for (i = 0; i < NUM_STATS; ++i) {
        if (wIDItem == MesgList[i].wIDItem) {
            szStatusMessage = MesgList[i].string;
            break;
        }
    }
    WriteStatusMessage(hWndStatusBar, lpDoc);
}


/* PopupMessage
 * ------------
 *
 * Prints message associated with the given popup menu.
 *
 * HMENU hmenuPopup
 * LPVOID lpDoc
 *
 * CUSTOMIZATION: None.
 *
 */
void PopupMessage(HWND hWndStatusBar, HMENU hmenuPopup, LPVOID lpDoc)
{
    UINT i;

    szStatusMessage = ControlList[1].string;
    for (i = 0; i < nCurrentPopup; ++i) {
        if (hmenuPopup == PopupList[i].hmenu) {
            szStatusMessage = PopupList[i].string;
            break;
        }
    }
    WriteStatusMessage(hWndStatusBar, lpDoc);
}


/* SysMenuMessage
 * --------------
 *
 * Prints messages to correspond to items on the system menu.
 * 
 *
 * UINT wIDItem
 * LPVOID lpDoc
 *
 * CUSTOMIZATION: None.
 *
 */
void SysMenuMessage(HWND hWndStatusBar, UINT wIDItem, LPVOID lpDoc)
{
    UINT i;

    szStatusMessage = ControlList[1].string;
    for (i = 0; i < 16; ++i) {
        if (wIDItem == SysMenuList[i].wIDItem) {
            szStatusMessage = SysMenuList[i].string;
            break;
        }
    }
    WriteStatusMessage(hWndStatusBar, lpDoc);
}


/* ControlMessage
 * --------------
 *
 * Prints general system messages.
 * 
 *
 * STATCONTROL scCommand
 * LPVOID lpDoc
 *
 * CUSTOMIZATION: Add new messages.
 *
 */
void ControlMessage(HWND hWndStatusBar, STATCONTROL scCommand, LPVOID lpDoc)
{
    UINT i;

    szStatusMessage = ControlList[1].string;
    for (i = 0; i < 2; ++i) {
        if ((UINT)scCommand == ControlList[i].wIDItem) {
            szStatusMessage = ControlList[i].string;
            break;
        }
    }
    WriteStatusMessage(hWndStatusBar, lpDoc);
}


/* StringMessage
 * -------------
 *
 * Prints messages specially sent by the application.
 * 
 * char *szMessage
 * LPVOID lpDoc
 *
 * CUSTOMIZATION: Add new messages.
 *
 */
void StringMessage(HWND hWndStatusBar, char *szMessage, LPVOID lpDoc)
{
    lstrcpy(szMessageHold, szMessage);
    szStatusMessage = szMessageHold;
    WriteStatusMessage(hWndStatusBar, lpDoc);
}



/* StatusWndProc
 * -------------
 *
 * Message handler for the statusbar window.
 *
 *
 * CUSTOMIZATION: None
 *
 */
LRESULT FAR PASCAL StatusWndProc
   (HWND hwnd, unsigned message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_PAINT) {
        RECT        rc;
        HDC         hdc;
        PAINTSTRUCT paintstruct;
        HPEN        hpenOld;
        HPEN        hpen;
        HFONT       hfontOld;
        HFONT       hfont;
        HPALETTE    hpalOld = NULL;
        POINT       point;

        BeginPaint (hwnd, &paintstruct);
        hdc = GetDC (hwnd);

        GetClientRect (hwnd, (LPRECT) &rc);

        hpenOld = SelectObject (hdc, GetStockObject (BLACK_PEN));
            
        MoveToEx (hdc, 0, 0, &point);
        LineTo (hdc, rc.right, 0);
            
        SelectObject (hdc, GetStockObject (WHITE_PEN));
        
        MoveToEx (hdc, STATUS_RRIGHT, STATUS_RTOP, &point);
        LineTo (hdc, STATUS_RRIGHT, STATUS_RBOTTOM);
        LineTo (hdc, STATUS_RLEFT-1, STATUS_RBOTTOM);
        
        hpen = CreatePen (PS_SOLID, 1, /* DKGRAY */ 0x00808080);
        SelectObject (hdc, hpen);
        
        MoveToEx (hdc, STATUS_RLEFT, STATUS_RBOTTOM-1, &point);
        LineTo (hdc, STATUS_RLEFT, STATUS_RTOP);
        LineTo (hdc, STATUS_RRIGHT, STATUS_RTOP);
        
        SetBkMode (hdc, TRANSPARENT);
        SetTextAlign (hdc, TA_LEFT | TA_TOP);
        hfont = CreateFont (STATUS_THEIGHT, 0, 0, 0, FW_NORMAL, FALSE, FALSE,
                            FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                            CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                            DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif");
                            
        hfontOld = SelectObject(hdc, hfont);
        
        TextOut (hdc, STATUS_TLEFT, STATUS_TTOP,
                 szStatusMessage,
                 lstrlen(szStatusMessage));
        
        // Restore original objects
        SelectObject (hdc, hfontOld);
        SelectObject (hdc, hpenOld);
        DeleteObject (hpen);
        DeleteObject (hfont);

        ReleaseDC (hwnd, hdc);
        EndPaint (hwnd, &paintstruct);
        
        return 0;
    }
    else {
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
}
