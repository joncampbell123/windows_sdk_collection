// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993 - 1996  Microsoft Corporation.  All Rights Reserved.
//
//  MODULE:   preview.c
//
//  PURPOSE:  Implement WYSIWYG print preview using the RichEdit control
//
//  FUNCTIONS:
//    InitPreview             - Register the preview window class
//    GetNextPage             - Format a page for previewing
//    PrintPreview            - Do the print preview
//    EnableNextPrev          - Update Next & Prev buttons in preview dialog
//    SetStatbarPage          - Update page # on status bar
//    CmdPreviewNext          - Handle "Next" button click
//    CmdPreviewPrev          - Handle "Prev" button click
//    CmdPreviewFilePrint     - Handle "Print..." button click
//    CmdPreviewClose         - Handle "Close" button click
//    MsgPreviewInitDialog    - Handle WM_INITDIALOG for preview dialog
//    MsgPreviewSize          - Handle WM_SIZE for preview dialog
//    MsgPreviewCtlColorDlg   - Handle WM_CTLCOLORDLG for preview dialog
//    MsgPreviewDestroy       - Handle WM_DESTROY for preview dialog
//    PaintPreview            - Paint contents of the preview window
//    MsgPrevWndNCCreate      - Handle WM_NCCREATE for preview window
//    MsgPrevWndPaint         - Handle WM_PAINT for preview window
//    MsgPreviewCommand       - Handle WM_COMMAND for preview dialog
//    PreviewDlgProc          - Dialog proc for the preview dialog
//    PreviewWndProc          - Window proc for the preview window
//
//  COMMENTS:
//
 
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <richedit.h>
#include "globals.h"
#include "resource.h"

#define RECTWIDTH(pr)  ((pr)->right - (pr)->left)
#define RECTHEIGHT(pr) ((pr)->bottom - (pr)->top)

BOOL CALLBACK PreviewDlgProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK PreviewWndProc(HWND, UINT, WPARAM, LPARAM);

// Global variables local to this module
static HBRUSH hbrPattern = NULL;
static TCHAR szPrevClass[] = TEXT("PrevWndClass");

// ID of the status bar on the preview dialog
#define IDC_PREVSTATBAR 2100

// allocate memory for this many pages at a time
#define PAGECHUNK 8

// define structure to hold data for the print preview
typedef struct _PREVINFO
{
    HWND hwndOwner;     // window that owns us
    FORMATRANGE fr;     // used with EM_FORMATRANGE for printing
    HWND hEdit;         // RichEdit control window
    int cchText;        // # of chars in hEdit
    SIZE sizePhysPage;  // full size of print paper (pixels)
    SIZE sizePhysInch;  // resolution (DPI) of printer (pixels)
    int iCurPage;       // page currently being previewed
    BOOL fAllPages;     // TRUE indicates all pages formatted
    int cPages;         // # of pages formatted so far (starts from zero)
    int cPageMax;       // # of elements in pPageIndex
    LPINT pPageIndex;   // starting character indexes of each page
} PREVINFO, *LPPREVINFO;


//
//  FUNCTION: InitPreview(HINSTANCE)
//
//  PURPOSE: Register a window class used during print preview
//
//  PARAMETERS:
//    hInstance - Application instance
//
//  RETURN VALUE:
//    TRUE on success, otherwise FALSE
//
//  COMMENTS:
//
//

BOOL InitPreview(HINSTANCE hInstance)
{
    WNDCLASS wc;

    wc.style         = CS_HREDRAW | CS_VREDRAW; // Class style(s).
    wc.lpfnWndProc   = (WNDPROC)PreviewWndProc; // Window Procedure
    wc.cbClsExtra    = 0;                       // No per-class extra data.
    wc.cbWndExtra    = 0;                       // No per-window extra data.
    wc.hInstance     = hInstance;               // Owner of this class
    wc.hIcon         = NULL;                    // no icon
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = szPrevClass;             // Name to register as

    // Register the window class and return FALSE if unsuccesful.

    return RegisterClass(&wc);
}


//
//  FUNCTION: GetNextPage(LPPREVINFO)
//
//  PURPOSE: Format another page for previewing and update PREVINFO.
//
//  PARAMETERS:
//    ppi           - pointer to PREVINFO struct
//    fPaint        - FALSE prevents drawing to pi.fr.hdc (format only)
//
//  RETURN VALUE:
//    TRUE for success, FALSE if all pages already formatted.
//    Also sets pi.fr.chrg to range of characters for new page and
//    updates pi.cPages.
//
//  COMMENTS:
//    pi and pi.fr must be initialized appropriately before calling
//    this function.
//
//

BOOL GetNextPage(LPPREVINFO ppi, BOOL fPaint)
{
    LPVOID pv;
    RECT rcTmp;

    // Are there any more pages to format?
    if (ppi->fAllPages)
        return FALSE;

    // Make sure there's room in ppi->pPageIndex for another page
    if (ppi->cPages >= ppi->cPageMax - 1)
    {
        pv = (LPVOID)LocalReAlloc((HLOCAL)ppi->pPageIndex,
                                  (ppi->cPageMax + PAGECHUNK) * sizeof(int),
                                  LMEM_MOVEABLE);
        if (!pv)
            return FALSE;

        ppi->pPageIndex = pv;
        ppi->cPageMax += PAGECHUNK;
    }

    // Set the CHARRANGE to format the next page
    ppi->fr.chrg.cpMin = ppi->pPageIndex[ppi->cPages];
    ppi->fr.chrg.cpMax = -1;

    rcTmp = ppi->fr.rc; // EM_FORMATRANGE tends to modify fr.rc.bottom

    // Tell the RichEdit control for format the page
    ppi->fr.chrg.cpMax = SendMessage(ppi->hEdit, 
                                     EM_FORMATRANGE, 
                                     fPaint,
                                     (LPARAM)&ppi->fr);

    ppi->fr.rc = rcTmp; // restore fr.rc

    // Update page info
    ppi->cPages++;
    ppi->pPageIndex[ppi->cPages] = ppi->fr.chrg.cpMax;

    // All pages formatted?
    ppi->fAllPages = ppi->fr.chrg.cpMax >= ppi->cchText;

    return TRUE;
}


//
//  FUNCTION: PrintPreview(HWND, HWND, HDC)
//
//  PURPOSE: Do WYSIWYG print preview for the current document
//
//  PARAMETERS:
//    hwndOwner     - Window that will own the preview dialog
//    hwndEdit      - RichEdit control that contains the document to preview
//    hdcTarget     - DC (usually printer DC) used for formatting the text
//
//  RETURN VALUE:
//    If the function succeeds, the return value is the nResult parameter
//    given in the call to the EndDialog function used to terminate the
//    dialog box; otherwise, it is -1. 
//
//  COMMENTS:
//
//

int PrintPreview(HWND hwndOwner, HWND hwndEdit, HDC hdcTarget)
{
    PREVINFO pi;
    HCURSOR hcurSave;
    int iCurSel;
    int nResult;

    hcurSave = SetCursor(hcursHourGlass);

    // Set up PREVINFO struct for the preview dialog
    pi.hwndOwner = hwndOwner;
    pi.fr.hdc = NULL;
    pi.fr.hdcTarget = hdcTarget;
    pi.hEdit = hwndEdit;

    pi.sizePhysPage.cx = GetDeviceCaps(hdcTarget, PHYSICALWIDTH);
    if (!pi.sizePhysPage.cx)
        pi.sizePhysPage.cx = GetDeviceCaps(hdcTarget, HORZRES);
    pi.sizePhysPage.cy = GetDeviceCaps(hdcTarget, PHYSICALHEIGHT);
    if (!pi.sizePhysPage.cy)
        pi.sizePhysPage.cy = GetDeviceCaps(hdcTarget, VERTRES);
    pi.sizePhysInch.cx = GetDeviceCaps(hdcTarget, LOGPIXELSX);
    pi.sizePhysInch.cy = GetDeviceCaps(hdcTarget, LOGPIXELSY);

    pi.iCurPage = -1;
    pi.fAllPages = FALSE;
    pi.cPages = 0;
    pi.cPageMax = PAGECHUNK;
    pi.pPageIndex = (LPINT)LocalAlloc(LPTR, pi.cPageMax*sizeof(int));
    if (!pi.pPageIndex)
    {
        SetCursor(hcurSave);      // Remove the hourglass
        return -1;
    }
    pi.pPageIndex[0] = 0;   // first page always starts at zero

    // Set page rect to phys page size in twips
    pi.fr.rcPage.left   = 0;
    pi.fr.rcPage.top    = 0;
    pi.fr.rcPage.right  = MulDiv(pi.sizePhysPage.cx, 1440, pi.sizePhysInch.cx);
    pi.fr.rcPage.bottom = MulDiv(pi.sizePhysPage.cy, 1440, pi.sizePhysInch.cy);

    // Set up 3/4" horizontal and 1" vertical margins, but leave a minimum of 1"
    // printable space in each direction.  Otherwise, use full page.
    pi.fr.rc = pi.fr.rcPage; // start with full page
    if (pi.fr.rcPage.right > 2*3*1440/4 + 1440)
        pi.fr.rc.right -= (pi.fr.rc.left = 3*1440/4);
    if (pi.fr.rcPage.bottom > 3*1440)
        pi.fr.rc.bottom -= (pi.fr.rc.top = 1440);

    // Find out real size of document in characters
    pi.cchText = SendMessage(hwndEdit, WM_GETTEXTLENGTH, 0, 0);

    // OPTIONAL:
    // Format through the page containing the beginning of the current
    // selection (iCurSel).  Other pages will be formatted later as necessary.

#ifndef NOCURSEL
    // Get current selection for preview (show this page first)
    SendMessage(hwndEdit, EM_EXGETSEL, 0, (LPARAM)&pi.fr.chrg);
    iCurSel = pi.fr.chrg.cpMin; // remember first char of selection
#else
    iCurSel = 0;    // always preview first page first
#endif

    // Get a screen DC to work with
    pi.fr.hdc = GetDC(NULL);

    while (GetNextPage(&pi, FALSE))
    {
        pi.iCurPage++;
    
        if (pi.fr.chrg.cpMax > iCurSel)
            break;
    }

    ReleaseDC(NULL, pi.fr.hdc);
    pi.fr.hdc = NULL;

    SetCursor(hcurSave);      // Remove the hourglass

    // Do the preview
    nResult = DialogBoxParam(hInst,
                             MAKEINTRESOURCE(IDD_PRINTPREVIEW),
                             hwndOwner,
                             PreviewDlgProc,
                             (LPARAM)&pi);

    // Done with this now
    LocalFree((HLOCAL)pi.pPageIndex);

    return nResult;
}


//
//  FUNCTION: EnableNextPrev(HWND, LPPREVINFO)
//
//  PURPOSE: Helper function to enable/disable the Next & Prev buttons
//
//  PARAMETERS:
//    hDlg          - handle to preview dialog
//    ppi           - pointer to PREVINFO struct
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//
//

void EnableNextPrev(HWND hDlg, LPPREVINFO ppi)
{
    EnableWindow(GetDlgItem(hDlg, IDC_PREVIEW_NEXT),
                 !ppi->fAllPages || ppi->iCurPage < ppi->cPages - 1);

    EnableWindow(GetDlgItem(hDlg, IDC_PREVIEW_PREV),
                 ppi->iCurPage > 0);
}


//
//  FUNCTION: SetStatbarPage(HWND, int)
//
//  PURPOSE: Helper function to set the page # on the status bar
//
//  PARAMETERS:
//    hDlg          - handle to preview dialog
//    iPage         - page number to display (zero based)
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//
//

void SetStatbarPage(HWND hDlg, int iPage)
{
    TCHAR szPage[32];
    HWND hwndStatbar = GetDlgItem(hDlg, IDC_PREVSTATBAR);

    if (hwndStatbar)
    {
        wsprintf(szPage, TEXT("Page %d"), iPage + 1);
        SendMessage(hwndStatbar,
                    SB_SETTEXT,
                    0,
                    (LPARAM)szPage);
    }
}


//
//  FUNCTION: CmdPreviewNext(HWND, WORD, WORD, HWND)
//
//  PURPOSE: Handle clicks on "Next" button in preview dialog.
//
//  PARAMETERS:
//    hwnd     - The window.
//    wCommand - IDC_PREVIEW_PREV (unused)
//    wNotify  - EN_*
//    hwndCtrl - NULL (unused)
//
//  RETURN VALUE:
//
//  COMMENTS:
//

LRESULT CmdPreviewNext(HWND hdlg, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    LPPREVINFO ppi = (LPPREVINFO)GetWindowLong(hdlg, GWL_USERDATA);
    HWND hwndPreview;

    if (!ppi->fAllPages || ppi->iCurPage < ppi->cPages - 1)
    {
        ppi->iCurPage++;

        if (hwndPreview = GetDlgItem(hdlg, IDC_PREVIEW))
            InvalidateRect(hwndPreview, NULL, TRUE);

        EnableNextPrev(hdlg, ppi);
        SetStatbarPage(hdlg, ppi->iCurPage);
    }

    return TRUE;
}


//
//  FUNCTION: CmdPreviewPrev(HWND, WORD, WORD, HWND)
//
//  PURPOSE: Handle clicks on "Prev" button in preview dialog.
//
//  PARAMETERS:
//    hwnd     - The window.
//    wCommand - IDC_PREVIEW_PREV (unused)
//    wNotify  - EN_*
//    hwndCtrl - NULL (unused)
//
//  RETURN VALUE:
//
//  COMMENTS:
//

LRESULT CmdPreviewPrev(HWND hdlg, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    LPPREVINFO ppi = (LPPREVINFO)GetWindowLong(hdlg, GWL_USERDATA);
    HWND hwndPreview;

    if (ppi->iCurPage > 0)
    {
        ppi->iCurPage--;

        if (hwndPreview = GetDlgItem(hdlg, IDC_PREVIEW))
            InvalidateRect(hwndPreview, NULL, TRUE);

        EnableNextPrev(hdlg, ppi);
        SetStatbarPage(hdlg, ppi->iCurPage);
    }
    return TRUE;
}


//
//  FUNCTION: CmdPreviewFilePrint(HWND, WORD, WORD, HWND)
//
//  PURPOSE: Handle clicks on "Print" button in preview dialog.
//
//  PARAMETERS:
//    hwnd     - The window.
//    wCommand - IDC_PREVIEW_PREV (unused)
//    wNotify  - EN_*
//    hwndCtrl - NULL (unused)
//
//  RETURN VALUE:
//
//  COMMENTS:
//

LRESULT CmdPreviewFilePrint(HWND hdlg, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    LPPREVINFO ppi = (LPPREVINFO)GetWindowLong(hdlg, GWL_USERDATA);

    PostMessage(ppi->hwndOwner, WM_COMMAND, IDM_FILEPRINT, 0);
    return EndDialog(hdlg, TRUE);
}


//
//  FUNCTION: CmdPreviewClose(HWND, WORD, WORD, HWND)
//
//  PURPOSE: Handle clicks on "Close" button in preview dialog.
//
//  PARAMETERS:
//    hwnd     - The window.
//    wCommand - IDC_PREVIEW_PREV (unused)
//    wNotify  - EN_*
//    hwndCtrl - NULL (unused)
//
//  RETURN VALUE:
//
//  COMMENTS:
//

LRESULT CmdPreviewClose(HWND hdlg, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    return EndDialog(hdlg, TRUE);
}


//
//  FUNCTION: MsgPreviewInitDialog(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Handle WM_INITDIALOG message for the preview dialog.
//
//  PARAMETERS:
//    hwnd     - The window handing the message.
//    uMessage - The message number. (unused).
//    wparam   - Message specific data (unused).
//    lparam   - Message specific data (unused).
//
//  RETURN VALUE:
//
//  COMMENTS:
//
//

LRESULT MsgPreviewInitDialog(HWND hdlg, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    LPPREVINFO ppi;
    RECT rc;
    HWND hwnd;
    TCHAR szTitle[MAX_PATH];
    HBITMAP hbm;
    WORD wBitPattern[] = { 0x55,    // alternating pixel checkerboard
                           0xaa,
                           0x55,
                           0xaa,
                           0x55,
                           0xaa,
                           0x55,
                           0xaa };

    // Save the PREVINFO pointer in window extra bytes
    SetWindowLong(hdlg, GWL_USERDATA, lparam);
    ppi = (LPPREVINFO)lparam;
    if (!ppi)
        return EndDialog(hdlg, 0);

    // Create pattern brush for drawing margins
    if (hbm = CreateBitmap(8, 8, 1, 1, wBitPattern))
    {
        hbrPattern = CreatePatternBrush(hbm);
        DeleteObject(hbm);
    }

    // Add the WS_EX_STATICEDGE & SS_ETCHEDHORZ styles to IDC_SEPARATOR. This
    // makes a nice separator line between the buttons and the preview area.
    // (Wish VC++ would do this for me in the dialog script...)
    if (hwnd = GetDlgItem(hdlg, IDC_SEPARATOR))
    {
        SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_STATICEDGE);
        SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) | SS_ETCHEDHORZ);
    }

    // Disable stuff that won't be implemented here...
    EnableWindow(GetDlgItem(hdlg, IDC_PREVIEW_NUMPAGE), FALSE);
    EnableWindow(GetDlgItem(hdlg, IDC_PREVIEW_ZOOMIN),  FALSE);
    EnableWindow(GetDlgItem(hdlg, IDC_PREVIEW_ZOOMOUT), FALSE);

    // Set the caption text to "doc name - app name"
    if (!GetFileTitle(GetFName(), szTitle, MAX_PATH))
    {
        lstrcat(szTitle, TEXT(" - "));
        lstrcat(szTitle, szAppName);
        SetWindowText(hdlg, szTitle);
    }

    // Create status window to display page numbers
    CreateStatusWindow(WS_CHILD | WS_VISIBLE | WS_BORDER | SBS_SIZEGRIP,
                       szTitle,
                       hdlg,
                       IDC_PREVSTATBAR);
    SetStatbarPage(hdlg, ppi->iCurPage);

    // Create preview window
    CreateWindowEx(WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE,
    		       szPrevClass,             // window class name
                   "",                      // no caption text
                   WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,   // style
                   0, 0, 100, 100,          // temporary size/position
                   hdlg,                    // parent
                   (HMENU)IDC_PREVIEW,      // child ID
                   hInst,                   // owning instance
                   ppi);            // user data

    // Adjust dialog to cover entire main window
    GetWindowRect(ppi->hwndOwner, &rc);
    MoveWindow(hdlg,
               rc.left,
               rc.top,
               RECTWIDTH(&rc),
               RECTHEIGHT(&rc),
               FALSE);

    // Enable/disable Next and Prev buttons as appropriate
    EnableNextPrev(hdlg, ppi);

    return TRUE;
}


//
//  FUNCTION: MsgPreviewSize(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Handle WM_SIZE message for the preview dialog.
//
//  PARAMETERS:
//    hwnd     - The window handing the message.
//    uMessage - The message number. (unused).
//    wparam   - Message specific data (unused).
//    lparam   - Message specific data (unused).
//
//  RETURN VALUE:
//
//  COMMENTS:
//
//

LRESULT MsgPreviewSize(HWND hdlg, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    LPPREVINFO ppi = (LPPREVINFO)GetWindowLong(hdlg, GWL_USERDATA);
    HWND hwnd;
    RECT rc, rcStatbar = {0, 0, 0, 0};
    int nTmp, cyEdge = GetSystemMetrics(SM_CYEDGE);

    // Tell status bar to reposition itself
    hwnd = GetDlgItem(hdlg, IDC_PREVSTATBAR);
    if (hwnd)
    {
        SendMessage(hwnd, WM_SIZE, wparam, lparam);
        GetWindowRect(hwnd, &rcStatbar);    // get new coordinates
    }

    // Adjust size/position of separator
    hwnd = GetDlgItem(hdlg, IDC_SEPARATOR);
    GetWindowRect(hwnd, &rc);
    MapWindowPoints(GetDesktopWindow(), hdlg, (LPPOINT)&rc, 2);
    MoveWindow(hwnd, 0, rc.top, LOWORD(lparam), cyEdge, TRUE);

    // Adjust size/position of preview window
    if (hwnd = GetDlgItem(hdlg, IDC_PREVIEW))
    {
        // Calculate available client area for preview window
        nTmp = rc.top + 2*cyEdge;
        GetClientRect(hdlg, &rc);
        rc.top = nTmp;
        rc.bottom -= cyEdge + RECTHEIGHT(&rcStatbar);
        rc.right -= (rc.left = GetSystemMetrics(SM_CXEDGE));
            
        // Calculate the rect for the preview "page" to be as large as
        // possible but still with the correct aspect ratio and centered
        // within the client area.
        if (rc.bottom * ppi->sizePhysPage.cx < rc.right * ppi->sizePhysPage.cy)
        {
            nTmp = rc.right;
            rc.right = MulDiv(rc.bottom, ppi->sizePhysPage.cx, ppi->sizePhysPage.cy);
            rc.left = (nTmp - rc.right) / 2;
            rc.right += rc.left;
        }
        else
        {
            nTmp = rc.bottom;
            rc.bottom = MulDiv(rc.right, ppi->sizePhysPage.cy, ppi->sizePhysPage.cx);
            rc.top = (nTmp - rc.bottom) / 2;
            rc.bottom += rc.top;
        }
       
        // Move preview window to new position
        MoveWindow(hwnd,
                   rc.left,
                   rc.top,
                   RECTWIDTH(&rc),
                   RECTHEIGHT(&rc),
                   TRUE);
    }

    return TRUE;
}


//
//  FUNCTION: MsgPreviewCtlColorDlg(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Handle WM_CTLCOLORDLG message for the preview dialog.
//
//  PARAMETERS:
//    hwnd     - The window handing the message.
//    uMessage - The message number. (unused).
//    wparam   - Message specific data (unused).
//    lparam   - Message specific data (unused).
//
//  RETURN VALUE:
//
//  COMMENTS:
//
//

LRESULT MsgPreviewCtlColorDlg(HWND hdlg, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    return (LRESULT)GetSysColorBrush(COLOR_BTNFACE);
}


//
//  FUNCTION: MsgPreviewDestroy(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Handle WM_DESTROY message for the preview dialog.
//
//  PARAMETERS:
//    hwnd     - The window handing the message.
//    uMessage - The message number. (unused).
//    wparam   - Message specific data (unused).
//    lparam   - Message specific data (unused).
//
//  RETURN VALUE:
//
//  COMMENTS:
//
//

LRESULT MsgPreviewDestroy(HWND hdlg, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    LPPREVINFO ppi = (LPPREVINFO)GetWindowLong(hdlg, GWL_USERDATA);

    // Reset the RichEdit control
    SendMessage(ppi->hEdit, EM_FORMATRANGE, TRUE, 0);

    if (hbrPattern)
    {
        DeleteObject(hbrPattern);
        hbrPattern = NULL;
    }

    return TRUE;
}


//
//  FUNCTION: PaintPreview(HDC, LPPREVINFO)
//
//  PURPOSE: Paint the current text page in the preview window
//
//  PARAMETERS:
//    hdc           - device context to draw to
//    ppi           - pointer to PREVINFO struct
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//
//

void PaintPreview(HWND hwnd, HDC hdc, LPPREVINFO ppi)
{
    HDC hdcT;
    RECT rcMargin, rcClient;
    BOOL bFormatDone = FALSE;

    if (!hwnd || !hdc || !ppi)
        return;

    if (ppi->iCurPage < 0 || (ppi->fAllPages && ppi->iCurPage > ppi->cPages - 1))
        return;

    GetClientRect(hwnd, &rcClient);

    // Set up the DC
    SetTextColor(hdc, RGB(0,0,0));
    SetBkColor(hdc, RGB(255,255,255));
    SetBkMode(hdc, TRANSPARENT);

    // Make the window the same logical size as the page
    SetMapMode(hdc, MM_ANISOTROPIC);
    SetWindowExtEx(hdc,
                   MulDiv(ppi->sizePhysPage.cx,
                          GetDeviceCaps(hdc, LOGPIXELSX),
                          ppi->sizePhysInch.cx),
                   MulDiv(ppi->sizePhysPage.cy,
                          GetDeviceCaps(hdc, LOGPIXELSY),
                          ppi->sizePhysInch.cy),
                   NULL);
    SetViewportExtEx(hdc, rcClient.right, rcClient.bottom, NULL);

    // Prepare for drawing
    hdcT = ppi->fr.hdc;
    ppi->fr.hdc = hdc;
    rcMargin = ppi->fr.rc;

    // Do we need to format a page we haven't formatted yet?
    if (ppi->iCurPage > ppi->cPages - 1)
    {
        bFormatDone = GetNextPage(ppi, TRUE);   // TRUE ==> do drawing here

        // Make sure everything is set correctly
        ppi->iCurPage = ppi->cPages - 1;
        EnableNextPrev(GetParent(hwnd), ppi);
    }

    // If we didn't draw the page during GetNextPage above, draw it now
    if (!bFormatDone)
    {
        // Set the CHARRANGE for the page to format
        ppi->fr.chrg.cpMin = ppi->pPageIndex[ppi->iCurPage];
        ppi->fr.chrg.cpMax = ppi->pPageIndex[ppi->iCurPage + 1];

        // Tell the RichEdit control to format the page.
        SendMessage(ppi->hEdit,
                    EM_FORMATRANGE,
                    TRUE,
                    (LPARAM)&ppi->fr);
    }

    // Restore these now
    ppi->fr.hdc = hdcT;
    ppi->fr.rc = rcMargin;  // EM_FORMATRANGE tends to change fr.rc.bottom

    // Set up for drawing margins

    if (hbrPattern)
        SelectObject(hdc, hbrPattern);  // alternating pixels

    SetMapMode(hdc, MM_TEXT);

    // convert rcMargin to screen pixels
    rcMargin.left   = MulDiv(rcMargin.left,
                             ppi->sizePhysInch.cx * rcClient.right,
                             1440 * ppi->sizePhysPage.cx);
    rcMargin.top    = MulDiv(rcMargin.top,
                             ppi->sizePhysInch.cy * rcClient.bottom,
                             1440 * ppi->sizePhysPage.cy);
    rcMargin.right  = MulDiv(rcMargin.right,
                             ppi->sizePhysInch.cx * rcClient.right,
                             1440 * ppi->sizePhysPage.cx);
    rcMargin.bottom = MulDiv(rcMargin.bottom,
                             ppi->sizePhysInch.cy * rcClient.bottom,
                             1440 * ppi->sizePhysPage.cy);

    // Draw margins
    PatBlt(hdc, 0, rcMargin.top,    rcClient.right, 1, PATCOPY);
    PatBlt(hdc, 0, rcMargin.bottom, rcClient.right, 1, PATCOPY);
    PatBlt(hdc, rcMargin.left,  0, 1, rcClient.bottom, PATCOPY);
    PatBlt(hdc, rcMargin.right, 0, 1, rcClient.bottom, PATCOPY);

    // deselect hbrPattern
    SelectObject(hdc, GetStockObject(WHITE_BRUSH));
}


//
//  FUNCTION: MsgPrevWndNCCreate(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Handle WM_NCCREATE message for the preview window. Stores the
//    user data in the CREATESTRUCT pointed to by lparam in the window
//    extra bytes.
//
//  PARAMETERS:
//    hwnd     - The window handing the message.
//    uMessage - The message number. (unused).
//    wparam   - Message specific data (unused).
//    lparam   - Message specific data (unused).
//
//  RETURN VALUE:
//
//  COMMENTS:
//
//

LRESULT MsgPrevWndNCCreate(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    SetWindowLong(hwnd,
                  GWL_USERDATA,
                  (LPARAM)((LPCREATESTRUCT)lparam)->lpCreateParams);
    return 1;
}


//
//  FUNCTION: MsgPrevWndPaint(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Handle WM_PAINT messages for the preview window
//
//  PARAMETERS:
//    hwnd     - The window handing the message.
//    uMessage - The message number. (unused).
//    wparam   - Message specific data (unused).
//    lparam   - Message specific data (unused).
//
//  RETURN VALUE:
//
//  COMMENTS:
//
//

LRESULT MsgPrevWndPaint(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    LPPREVINFO ppi = (LPPREVINFO)GetWindowLong(hwnd, GWL_USERDATA);
    PAINTSTRUCT ps;

    BeginPaint(hwnd, &ps);
    PaintPreview(hwnd, ps.hdc, ppi);
    EndPaint(hwnd, &ps);
    return 0;
}


//
// Message dispatch tables, window procedures, and command dispatch handlers
//

static CMD rgcmdPreviewDlg[] =
{
    {IDC_PREVIEW_NEXT, CmdPreviewNext},
    {IDC_PREVIEW_PREV, CmdPreviewPrev},
    {IDM_FILEPRINT,    CmdPreviewFilePrint},
    {IDCANCEL,         CmdPreviewClose},
    {IDOK,             CmdPreviewClose},
};

CMDI cmdiPreviewDlg =
{
    sizeof(rgcmdPreviewDlg) / sizeof(CMD),
    rgcmdPreviewDlg,
    edwpNone
};


//
//  FUNCTION: MsgPreviewCommand(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Handle the WM_COMMAND messages for the preview dialog.
//
//  PARAMETERS:
//    hwnd     - window handle
//    uMessage - message number (Unused)
//    GET_WM_COMMAND_ID(wparam,lparam)   - Command identifier
//    GET_WM_COMMAND_HWND(wparam,lparam) - Control handle
//
//  RETURN VALUE:
//
//  COMMENTS:
//    Uses the combination of the command structure defined in mditable.c
//    and the DispCommand function defined in WndProc.c to handle all
//    commands directed to an MDI child window.
//
//

LRESULT MsgPreviewCommand(HWND hdlg, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    return DispCommand(&cmdiPreviewDlg, hdlg, wparam, lparam);
}


static MSD rgmsdPreviewDlg[] =
{
    {WM_COMMAND,     MsgPreviewCommand    },
    {WM_CTLCOLORDLG, MsgPreviewCtlColorDlg},
    {WM_SIZE,        MsgPreviewSize       },
    {WM_INITDIALOG,  MsgPreviewInitDialog },
    {WM_DESTROY,     MsgPreviewDestroy    },
};

MSDI msdiPreviewDlg =
{
    sizeof(rgmsdPreviewDlg) / sizeof(MSD),
    rgmsdPreviewDlg,
    edwpNone
};


//
//  FUNCTION: PreviewDlgProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Dialog procedure for the preview dialog
//
//  PARAMETERS:
//    standard window params
//
//  RETURN VALUE:
//    TRUE if message processed, otherwise FALSE
//
//  COMMENTS:
//
//

BOOL CALLBACK PreviewDlgProc(HWND hdlg, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    return DispMessage(&msdiPreviewDlg, hdlg, uMessage, wparam, lparam);
}


static MSD rgmsdPrevWnd[] =
{
    {WM_PAINT,      MsgPrevWndPaint   },
    {WM_NCCREATE,   MsgPrevWndNCCreate},
};

MSDI msdiPrevWnd =
{
    sizeof(rgmsdPrevWnd) / sizeof(MSD),
    rgmsdPrevWnd,
    edwpWindow
};


//
//  FUNCTION: PreviewWndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Window procedure for the preview window in the preview dialog
//
//  PARAMETERS:
//    standard window params
//
//  RETURN VALUE:
//    depends on message
//
//  COMMENTS:
//
//

LRESULT CALLBACK PreviewWndProc(HWND hwnd,
                                UINT uMessage,
                                WPARAM wparam,
                                LPARAM lparam)
{
    return DispMessage(&msdiPrevWnd, hwnd, uMessage, wparam, lparam);
}
