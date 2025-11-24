/*
 * LISTS.C
 *
 * This file implements a generalized multi-collumn listbox with a standard
 * frame window.
 */
#include <windows.h>
#include <windowsx.h>
#include <string.h>
#include <stdlib.h>
#include "ddespy.h"
#include "globals.h"
#include "lists.h"

int  CompareItems(PSTR psz1, PSTR psz2, INT SortCol, INT cCols);
int  CmpCols(PSTR psz1, PSTR psz2, INT SortCol);
void DrawLBItem(LPDRAWITEMSTRUCT lpdis);
long CALLBACK MCLBClientWndProc(HWND hwnd, UINT msg, WPARAM wParam, LONG lPAram);

UINT cyHeading;

HWND CreateMCLBFrame(
                    HWND hwndParent,
                    LPSTR lpszTitle,        // frame title string
                    UINT dwStyle,          // frame styles
                    HICON hIcon,
                    HBRUSH hbrBkgnd,        // background for heading.
                    LPSTR lpszHeadings)     // tab delimited list of headings.  The number of
                        // headings indicate the number of collumns.
{
    static BOOL fRegistered = FALSE;
    MCLBCREATESTRUCT mclbcs;

    if (!fRegistered) {
        WNDCLASS wc;
        HDC hdc;
        TEXTMETRIC tm;

        wc.style = WS_OVERLAPPED | CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = MCLBClientWndProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 4;
        wc.hInstance = hInst;
        wc.hIcon = hIcon;
        wc.hCursor = NULL;
        wc.hbrBackground = hbrBkgnd;
        wc.lpszMenuName = NULL;
        wc.lpszClassName = RefString(IDS_LISTCLASS);
        RegisterClass(&wc);

        hdc = GetDC(GetDesktopWindow());
        GetTextMetrics(hdc, &tm);
        cyHeading = tm.tmHeight;
        ReleaseDC(GetDesktopWindow(), hdc);

        fRegistered = TRUE;
    }
    mclbcs.lpszHeadings = lpszHeadings;

    return(CreateWindow(RefString(IDS_LISTCLASS), lpszTitle, dwStyle,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            hwndParent, NULL, hInst, (LPSTR)&mclbcs));
}


LONG  CALLBACK MCLBClientWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    MCLBSTRUCT *pmclb;
    RECT rc;
    INT  i;

    if (msg == WM_CREATE) {
        PSTR psz;
        MCLBCREATESTRUCT FAR *pcs;

        pcs = (MCLBCREATESTRUCT FAR *)((LPCREATESTRUCT)lParam)->lpCreateParams;
        pmclb = (MCLBSTRUCT *)LocalAlloc(LPTR, sizeof(MCLBSTRUCT));
        psz = (PSTR)LocalAlloc(LPTR, _fstrlen(pcs->lpszHeadings) + 1);
        _fstrcpy((LPSTR)psz, pcs->lpszHeadings);
        pmclb->pszHeadings = psz;
        pmclb->cCols = 1;
        while ((psz = strchr(psz, '\t'))) {
            pmclb->cCols++;
            psz++;
        }
        pmclb->SortCol = 0;
        SetWindowLong(hwnd, 0, (UINT)pmclb);
        GetClientRect(hwnd, &rc);
        pmclb->hwndLB = CreateWindow(RefString(IDS_LBOX), szNULL, MYLBSTYLE | WS_VISIBLE,
                0, 0, 0, 0, hwnd, (HMENU)pmclb->cCols, hInst, NULL);
        return(pmclb->hwndLB ? 0 : -1);
    }

    pmclb = (MCLBSTRUCT *)GetWindowLong(hwnd, 0);

    switch (msg) {
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            DRAWITEMSTRUCT dis;

            BeginPaint(hwnd, &ps);
            SetBkMode(ps.hdc, TRANSPARENT);
            dis.hwndItem = hwnd;
            dis.hDC = ps.hdc;
            GetClientRect(hwnd, &dis.rcItem);
            dis.rcItem.bottom = dis.rcItem.top + cyHeading;
            dis.CtlType = ODT_BUTTON;   // hack to avoid erasure
            dis.CtlID = pmclb->cCols;
            dis.itemID = 0;
            dis.itemAction = ODA_DRAWENTIRE;
            dis.itemData = (UINT)(LPSTR)pmclb->pszHeadings;
            dis.itemState = 0;
            DrawLBItem(&dis);
            EndPaint(hwnd, &ps);
        }
        break;

    case WM_SIZE:
        MoveWindow(pmclb->hwndLB, 0, cyHeading, LOWORD(lParam),
                HIWORD(lParam) - cyHeading, TRUE);
        break;

    case WM_LBUTTONDOWN:
        {
            HWND hwndLB;
            INT i;

            // determine which collumn the mouse landed and sort on that collumn.

            SendMessage(hwnd, WM_SETREDRAW, 0, 0);
            GetClientRect(hwnd, &rc);
            InflateRect(&rc, -1, -1);
            pmclb->SortCol = LOWORD(lParam) * pmclb->cCols / (rc.right - rc.left);
            hwndLB = CreateWindow(RefString(IDS_LBOX), szNULL, MYLBSTYLE, 1, cyHeading + 1,
                    rc.right - rc.left, rc.bottom - rc.top - cyHeading,
                    hwnd, (HMENU)pmclb->cCols, hInst, NULL);
            for (i = (INT)SendMessage(pmclb->hwndLB, LB_GETCOUNT, 0, 0); i; i--) {
                SendMessage(hwndLB, LB_ADDSTRING, 0,
                    SendMessage(pmclb->hwndLB, LB_GETITEMDATA, i - 1, 0));
                SendMessage(pmclb->hwndLB, LB_SETITEMDATA, i - 1, 0);
            }
            ShowWindow(hwndLB, SW_SHOW);
            ShowWindow(pmclb->hwndLB, SW_HIDE);
            DestroyWindow(pmclb->hwndLB);
            pmclb->hwndLB = hwndLB;
            SendMessage(hwnd, WM_SETREDRAW, 1, 0);
            InvalidateRect(hwnd, NULL, FALSE);
        }
        break;

    case WM_DELETEITEM:

        if ((UINT)((LPDELETEITEMSTRUCT)lParam)->itemData)
            LocalFree(LocalHandle((PVOID)((LPDELETEITEMSTRUCT)lParam)->itemData));
        break;

    case WM_MEASUREITEM:
        ((LPMEASUREITEMSTRUCT)lParam)->itemHeight = cyHeading;
        break;

    case WM_DRAWITEM:
        GetClientRect(hwnd, &rc);
        // This fudge makes the collumns line up with the heading.
        ((LPDRAWITEMSTRUCT)lParam)->rcItem.right = rc.right;
        DrawLBItem((LPDRAWITEMSTRUCT)lParam);
        return(DefWindowProc(hwnd, msg, wParam, lParam));
        break;

    case WM_COMPAREITEM:
        return(CompareItems((PSTR)((LPCOMPAREITEMSTRUCT)lParam)->itemData1,
                (PSTR)((LPCOMPAREITEMSTRUCT)lParam)->itemData2,
                pmclb->SortCol,
                pmclb->cCols));
        break;

    case WM_DESTROY:
        LocalFree(LocalHandle((PVOID)pmclb->pszHeadings));
        LocalFree(LocalHandle((PVOID)pmclb));
        break;

    case WM_CLOSE:
        for (i = 0; i < IT_COUNT && (hwndTrack[i] != hwnd); i++) {
            ;
        }
        pro.fTrack[i] = FALSE;
        hwndTrack[i] = NULL;
        SetFilters();
        DestroyWindow(hwnd);
        break;

    default:
        return(DefWindowProc(hwnd, msg, wParam, lParam));
    }
}




/*
 * Make this return FALSE if addition not needed.
 *
 * if pszSearch != NULL, searches for pszSearch - collumns may contain
 * wild strings - "*"
 * If found, the string is removed from the LB.
 * Adds pszReplace to LB.
 */
VOID AddMCLBText(PSTR pszSearch, PSTR pszReplace, HWND hwndLBFrame)
{
    MCLBSTRUCT *pmclb;
    INT lit;
    PSTR psz;

    pmclb = (MCLBSTRUCT *)GetWindowLong(hwndLBFrame, 0);

    SendMessage(pmclb->hwndLB, WM_SETREDRAW, 0, 0);
    if (pszSearch != NULL) {
        lit = (INT)SendMessage(pmclb->hwndLB, LB_FINDSTRING, (WPARAM)-1, (LONG)(LPSTR)pszSearch);
        if (lit >= 0) {
            SendMessage(pmclb->hwndLB, LB_DELETESTRING, lit, 0);
        }
    }
    psz = (PSTR)LocalAlloc(LPTR, strlen(pszReplace) + 1);
    strcpy(psz, pszReplace);
    SendMessage(pmclb->hwndLB, WM_SETREDRAW, 1, 0);
    SendMessage(pmclb->hwndLB, LB_ADDSTRING, 0, (LONG)(LPSTR)psz);
}


/*
 * This function assumes that the text in cCol is an ASCII number.  0 is
 * returned if it is not found.
 */
INT GetMCLBColValue(PSTR pszSearch, HWND hwndLBFrame, INT  cCol)
{
    MCLBSTRUCT *pmclb;
    PSTR psz;
    INT lit;

    pmclb = (MCLBSTRUCT *)GetWindowLong(hwndLBFrame, 0);

    lit = (INT)SendMessage(pmclb->hwndLB, LB_FINDSTRING, (WPARAM)-1, (LONG)(LPSTR)pszSearch);
    if (lit < 0) {
        return(0);
    }
    psz = (PSTR)SendMessage(pmclb->hwndLB, LB_GETITEMDATA, lit, 0);
    while (--cCol && (psz = strchr(psz, '\t') + 1)) {
        ;
    }
    if (psz) {
        return(atoi(psz));
    } else {
        return(0);
    }
}



/*
 * Returns fFoundAndRemoved
 */
BOOL DeleteMCLBText(PSTR pszSearch, HWND hwndLBFrame)
{
    MCLBSTRUCT *pmclb;
    INT lit;

    pmclb = (MCLBSTRUCT *)GetWindowLong(hwndLBFrame, 0);
    lit = (INT)SendMessage(pmclb->hwndLB, LB_FINDSTRING, (WPARAM)-1,
            (LONG)(LPSTR)pszSearch);
    if (lit >= 0) {
        SendMessage(pmclb->hwndLB, LB_DELETESTRING, lit, 0);
        return(TRUE);
    }
    return(FALSE);
}


/*
 * Returns >0 if item1 comes first, <0 if item2 comes first, 0 if ==.
 */
INT CompareItems(PSTR psz1, PSTR psz2, INT SortCol, INT cCols)
{
    INT i, Col;

    i = CmpCols(psz1, psz2, SortCol);
    if (i != 0) {
        return(i);
    }
    for (Col = 0; Col < cCols; Col++) {
        if (Col == SortCol) {
            continue;
        }
        i = CmpCols(psz1, psz2, Col);
        if (i != 0) {
            return(i);
        }
    }
    return(0);
}


INT CmpCols(PSTR psz1, PSTR psz2, INT SortCol)
{
    PSTR psz, pszT1, pszT2;
    INT iRet;

    while (SortCol--) {
        psz = strchr(psz1, '\t');
        if (psz != NULL) {
            psz1 = psz + 1;
        } else {
            psz1 = psz1 + strlen(psz1);
        }
        psz = strchr(psz2, '\t');
        if (psz != NULL) {
            psz2 = psz + 1;
        } else {
            psz2 = psz2 + strlen(psz2);
        }
    }
    pszT1 = strchr(psz1, '\t');
    pszT2 = strchr(psz2, '\t');

    if (pszT1) {
        *pszT1 = '\0';
    }
    if (pszT2) {
        *pszT2 = '\0';
    }

    if (!strcmp(RefString(IDS_WILD), psz1) || !strcmp(RefString(IDS_WILD), psz2)) {
        iRet = 0;
    } else {
        iRet = strcmp(psz1, psz2);
    }

    if (pszT1) {
        *pszT1 = '\t';
    }
    if (pszT2) {
        *pszT2 = '\t';
    }

    return(iRet);
}



VOID DrawLBItem(LPDRAWITEMSTRUCT lpdis)
{
    RECT rcDraw;
    INT cxSection;
    PSTR psz, pszEnd;

    if (!lpdis->itemData)
        return;
    if ((lpdis->itemAction & ODA_DRAWENTIRE) ||
            ((lpdis->itemAction & ODA_SELECT) &&
            (lpdis->itemState & ODS_SELECTED))) {
        rcDraw = lpdis->rcItem;
        if (lpdis->CtlType != ODT_BUTTON) { // hack to avoid erasure
            HBRUSH hbr;

            hbr = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
            FillRect(lpdis->hDC, &lpdis->rcItem, hbr);
            DeleteObject(hbr);
        }
        cxSection = (rcDraw.right - rcDraw.left) / lpdis->CtlID;
        psz = (PSTR)(UINT)lpdis->itemData;
        rcDraw.right = rcDraw.left + cxSection;
        while (pszEnd = strchr(psz, '\t')) {
            *pszEnd = '\0';
            DrawText(lpdis->hDC, psz, -1, &rcDraw, DT_LEFT);
            OffsetRect(&rcDraw, cxSection, 0);
            *pszEnd = '\t';
            psz = pszEnd + 1;
        }
        DrawText(lpdis->hDC, psz, -1, &rcDraw, DT_LEFT);

        if (lpdis->itemState & ODS_SELECTED)
            InvertRect(lpdis->hDC, &lpdis->rcItem);

        if (lpdis->itemState & ODS_FOCUS)
            DrawFocusRect(lpdis->hDC, &lpdis->rcItem);

    } else if (lpdis->itemAction & ODA_SELECT) {

        InvertRect(lpdis->hDC, &lpdis->rcItem);

    } else if (lpdis->itemAction & ODA_FOCUS) {

        DrawFocusRect(lpdis->hDC, &lpdis->rcItem);

    }
}


