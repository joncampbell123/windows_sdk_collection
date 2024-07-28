/**[f******************************************************************
* dlgutils.c -
*
* Copyright (C) 1988,1989 Aldus Corporation
* Copyright (C) 1988-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.
* Company confidential.
*
**f]*****************************************************************/
  
// History
//  13 sep 91   SD          Increased buffer size for localization. BUG #655.
//  29 aug 91   SD          Clean up.  Removed forward declaration for
//                          GenericWncProc().  Removed def of ghInstance.
//                          Moved #include for build.h above windows.h.
//  13 aug 91   SD          Change GenericWndProc to AboutDlg() and 
//                          AboutDlgFn() so that Win31 can have printer name
//                          in the About box. (BUG #558)
//  27 apr 89   peterbe     Tabs are 8 spaces
/***************************************************************************/
/******************************   dlgutils.c   *****************************/
/*
*  DlgUtils:  Dialog utilities.
*/
  
#include "nocrap.h"
#undef NOPOINT
#undef NORECT
#undef NOSYSMETRICS
#undef NOWINMESSAGES
#undef NOCTLMGR
#include "build.h"     /*BUG #558 */
#include "windows.h"
#include "resource.h"  /*BUG #558 */
#include "debug.h"
#include "strings.h"   /*BUG #558 */
#include "dlgutils.h"

#define LOCAL static     /* BUG #558 */
  
short FAR PASCAL AboutDlg(HANDLE, HWND, LPPCLDEVMODE);
static HWND GetBiggestParent(HWND);

#if defined (WIN31)
LOCAL LPPCLDEVMODE glpDevmode;      /* BUG #558 */
extern HANDLE hLibInst;
#endif

/* Forward declaration */  
int FAR PASCAL AboutDlgFn(HWND, unsigned, WORD, long);
  
/*************************************************************************/
/****************************   Global procs   ***************************/
  
/*  AboutDlg
*/
short FAR PASCAL
AboutDlg(HANDLE hMd, HWND hWndParent, LPPCLDEVMODE lpDevmode) {
  
    FARPROC lpAboutDlgFn;
    short response;

#ifdef DEBUG_FUNCT
    DB(("Entering AboutDlg\n"));
#endif

#if defined (WIN31)  
    glpDevmode = lpDevmode;
#endif

    lpAboutDlgFn = MakeProcInstance(AboutDlgFn, hMd);
    response = DialogBox(hMd, MAKEINTRESOURCE(SFABOUT), hWndParent, lpAboutDlgFn);
    FreeProcInstance(lpAboutDlgFn);
  
#ifdef DEBUG_FUNCT
    DB(("Exiting AboutDlg\n"));
#endif
    return(response);
}
 
  
/*  CenterDlg
*
*  Center dialog over parent window.
*/
void FAR PASCAL CenterDlg (hDlg)
HWND hDlg;
{
    HWND hParentWnd;
    RECT DlgWndRect;
    RECT ParentWndRect;
    POINT DlgLoc;
    short DlgHeight;
    short DlgWidth;
    short ScrTop;
    short ScrLeft;
    short ScrBot;
    short ScrRight;
  
#ifdef DEBUG_FUNCT
    DB(("Entering CenterDlg\n"));
#endif
    /*  Locate the biggest parent window we can find.
    */
    hParentWnd = GetBiggestParent(hDlg);
  
    /*  Get bounding rectangles of dialog and parent window.
    */
    GetWindowRect(hDlg, (LPRECT) &DlgWndRect);
    GetClientRect(hParentWnd, (LPRECT) &ParentWndRect);
  
    /*  Calculate upper left corner of dialog so it will
    *  be centered over the parent window.
    */
    DlgWidth = DlgWndRect.right - DlgWndRect.left;
    DlgHeight = DlgWndRect.bottom - DlgWndRect.top;
  
    DlgLoc.x = ParentWndRect.left;
    DlgLoc.y = ParentWndRect.top;
    DlgLoc.x +=
    (ParentWndRect.right - ParentWndRect.left - DlgWidth) / 2;
    DlgLoc.y +=
    ((ParentWndRect.bottom - ParentWndRect.top - DlgHeight) * 2) / 5;
  
    /*  Convert to equivalent screen coordinates.
    */
    ClientToScreen(hParentWnd, (LPPOINT) &DlgLoc);
  
    /*  Limit the dialog from going off screen.
    */
    ScrLeft = 0;
    ScrTop = 0;
    ScrRight = (short)GetSystemMetrics(SM_CXSCREEN);
    ScrBot = (short)GetSystemMetrics(SM_CYSCREEN);
  
    if (DlgLoc.x < ScrLeft)
    {
        DlgLoc.x = ScrLeft;
    }
    else if (DlgLoc.x > (ScrRight - DlgWidth))
    {
        DlgLoc.x = ScrRight - DlgWidth;
    }
  
    if (DlgLoc.y < ScrTop)
    {
        DlgLoc.y = ScrTop;
    }
    else if (DlgLoc.y > (ScrBot - DlgHeight))
    {
        DlgLoc.y = ScrBot - DlgHeight;
    }
  
    /*  Center dialog.
    */
    MoveWindow(hDlg, DlgLoc.x, DlgLoc.y, DlgWidth, DlgHeight,
    IsWindowVisible(hDlg));
#ifdef DEBUG_FUNCT
    DB(("Exiting CenterDlg\n"));
#endif
}
  
/*  AboutDlgFn
*
*  Dialog function for the About dialog box.
*/
int FAR PASCAL AboutDlgFn(hWnd, wMsg, wParam, lParam)
HWND hWnd;
unsigned wMsg;
WORD wParam;
long lParam;
{
#ifdef DEBUG_FUNCT
    DB(("Entering AboutDlgFn\n"));
#endif

#if defined (WIN31)
                char buf[14];                         /*  BUG #655 */
                char title[CCHDEVICENAME];
#endif /* WIN31 */

    switch (wMsg)
    {
        case WM_INITDIALOG:
            CenterDlg(hWnd);
#if defined (WIN31)
                lstrcpy((LPSTR)title,(LPSTR)glpDevmode->dm.dmDeviceName);
                LoadString(hLibInst,IDS_DRIVER,buf,sizeof(buf));
                lstrcat((LPSTR)title, (LPSTR)buf);
                SetDlgItemText(hWnd,IDDRIVER,(LPSTR)title);
#endif /* WIN31 */
            break;
  
        case WM_COMMAND:
            EndDialog(hWnd, wParam);
            break;
  
        default:
            return FALSE;
    }
  
#ifdef DEBUG_FUNCT
    DB(("Exiting AboutDlgFn\n"));
#endif
    return TRUE;
}


/*  BUG 558: This function is no longer used. */
/*  GenericWndProc
*
*  Generic window proc for handling informative non-modal dialogs.
*/
/* 
 *int FAR PASCAL GenericWndProc(hWnd, wMsg, wParam, lParam)
 *HWND hWnd;
 *unsigned wMsg;
 *WORD wParam;
 *long lParam;
 *{
 *#ifdef DEBUG_FUNCT
 *    DB(("Entering GenericWndProc\n"));
 *#endif
 *    DBMSG(("GenericWndProc(%d,%d,%d,%ld)\n",
 *   (HWND)hWnd, (unsigned)wMsg, (WORD)wParam, lParam));
 * 
 *   switch (wMsg)
 *   {
 *       case WM_INITDIALOG:
 *           CenterDlg(hWnd);
 *           break;
 * 
 *       case WM_COMMAND:
 *           EndDialog(hWnd, wParam);
 *           break;
 * 
 *       default:
 *           return FALSE;
 *   }
 * 
 *#ifdef DEBUG_FUNCT
 *   DB(("Exiting GenericWndProc\n"));
 *#endif
 *   return TRUE;
 *}
 */  
  
/*************************************************************************/
/****************************   Local procs   ****************************/
  
  
/*  GetBiggestParent
*
*  Keep calling GetParent() until we've gotten the biggest window
*  we can find.
*/
static HWND GetBiggestParent(hCurrentWnd)
HWND hCurrentWnd;
{
    HWND hSaveWnd = hCurrentWnd;
    HWND hBiggestWnd = hCurrentWnd;
    RECT currentRect;
    short currentWidth;
    short biggestWidth = 0;
    WORD redundantLoop = 0;
#ifdef DEBUG_FUNCT
    DB(("Entering GetBiggestParent\n"));
#endif
  
    while ((++redundantLoop < 32) &&
        (hCurrentWnd = GetParent(hCurrentWnd)))
    {
        GetWindowRect(hCurrentWnd, (LPRECT) &currentRect);
  
        currentWidth = currentRect.right - currentRect.left;
  
        if (currentWidth > biggestWidth)
        {
            hBiggestWnd = hCurrentWnd;
            biggestWidth = currentWidth;
        }
        DBMSG(("...inside GetBiggestParent(%d): current width=%d, biggest=%d\n",
        redundantLoop, currentWidth, biggestWidth));
    }
  
    if (redundantLoop > 31)
    {
        hBiggestWnd = GetParent(hSaveWnd);
    }
  
#ifdef DEBUG_FUNCT
    DB(("Exiting GetBiggestParent\n"));
#endif
    return (hBiggestWnd);
}
  
