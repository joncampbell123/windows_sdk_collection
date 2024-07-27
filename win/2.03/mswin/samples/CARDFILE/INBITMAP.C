#include "index.h"

/****************************************************************/
/*                                                              */
/*  Windows Cardfile - Written by Mark Cliggett                 */
/*  (c) Copyright Microsoft Corp. 1985 - All Rights Reserved    */
/*                                                              */
/****************************************************************/

POINT   dragPt;
int     fBMDown;
int     xmax;
int     ymax;

FAR BMMouse(hWindow, message, wParam, pt)
HWND hWindow;
int message;
WORD wParam;
POINT pt;
    {
    RECT rect;
    int iCard;
    HDC hDC;
    int t;

    switch(message)
        {
        case WM_LBUTTONDOWN:
            if (PtInRect((LPRECT)&dragRect, pt))
                {
                SetCapture(hWindow);
                GetClientRect(hWindow, (LPRECT)&rect);
                xmax = rect.right - CharFixWidth;
                ymax = rect.bottom - CharFixHeight;
                fBMDown = TRUE;
                dragPt = pt;
                }
            break;
        case WM_LBUTTONUP:
            if (fBMDown)
                {
                ReleaseCapture();
                if (dragRect.top != CurCard.yBitmap || dragRect.left != CurCard.xBitmap)
                    {
                    SetRect((LPRECT)&rect, CurCard.xBitmap, CurCard.yBitmap, CurCard.xBitmap+CurCard.cxBitmap, CurCard.yBitmap+CurCard.cyBitmap);
                    CurCard.xBitmap = dragRect.left;
                    CurCard.yBitmap = dragRect.top;
                    InvalidateRect(hCardWnd, (LPRECT)&rect, TRUE);
                    InvalidateRect(hCardWnd, (LPRECT)&dragRect, TRUE);
                    if (CurCard.hBitmap)
                        CurCardHead.flags |= FDIRTY;
                    }
                dragPt.x = CurCard.xBitmap;
                dragPt.y = CurCard.yBitmap;
                fBMDown = FALSE;
                }
            break;
        case WM_MOUSEMOVE:
            if (fBMDown)
                {
                t = dragRect.left + pt.x - dragPt.x;
                if (t > xmax)
                    pt.x = xmax - dragRect.left + dragPt.x;
                else if (t < CharFixWidth - (dragRect.right - dragRect.left))
                    pt.x = CharFixWidth - (dragRect.right - dragRect.left) - dragRect.left + dragPt.x;

                t = dragRect.top + pt.y - dragPt.y;
                if (t > ymax)
                    pt.y = ymax - dragRect.top + dragPt.y;
                else if (t < CharFixHeight - (dragRect.bottom - dragRect.top))
                    pt.y = CharFixHeight - (dragRect.bottom - dragRect.top) - dragRect.top + dragPt.y;

                if (dragPt.x != pt.x || dragPt.y != pt.y)
                    {
                    hDC = GetDC(hCardWnd);
                    DrawXorRect(hDC, &dragRect);
                    OffsetRect((LPRECT)&dragRect, pt.x - dragPt.x, pt.y - dragPt.y);
                    dragPt = pt;
                    DrawXorRect(hDC, &dragRect);
                    ReleaseDC(hCardWnd, hDC);
                    }
                }
            break;
        }
    }

FAR TurnOnBitmapRect()
    {
    if (CurCard.hBitmap)
        SetRect((LPRECT)&dragRect, CurCard.xBitmap, CurCard.yBitmap, CurCard.xBitmap+CurCard.cxBitmap, CurCard.yBitmap+CurCard.cyBitmap);
    else
        SetRect((LPRECT)&dragRect, 5, 5, 5+CharFixWidth, 5+CharFixHeight);
    XorBitmapRect();
    }

FAR TurnOffBitmapRect()
    {
    XorBitmapRect();
    }

XorBitmapRect()
    {
    HDC hDC;

    hDC = GetDC(hCardWnd);
    DrawXorRect(hDC, &dragRect);
    ReleaseDC(hCardWnd, hDC);
    }

FAR DrawXorRect(hDC, pRect)
HDC hDC;
RECT *pRect;
    {
    int     x,y;
    POINT   point;

    SelectObject(hDC, hbrWhite);

    /* left,top must be -1 to not overlap. then add a border of 1. 17-Oct-1987. */
    point.x = pRect->left - 2;
    point.y = pRect->top -2;
    x = pRect->right - point.x + 2;
    y = pRect->bottom - point.y + 2;

    PatBlt(hDC, point.x, point.y, x, 1, PATINVERT);
    PatBlt(hDC, point.x, point.y, 1, y, PATINVERT);

    point.x = pRect->right + 1;
    point.y = pRect->bottom + 1;
    x -= 1;
    y -= 1;

    PatBlt(hDC, point.x, point.y, -x, 1, PATINVERT);
    PatBlt(hDC, point.x, point.y, 1, -y, PATINVERT);
    }

FAR BMKey(wParam)
WORD wParam;
    {
    int x;
    int y;
    BOOL fShift;
    BOOL fControl;
    WORD wEditParam;

    x = dragRect.left;
    y = dragRect.top;

    switch(wParam)
        {
        case VK_INSERT:
        case VK_DELETE:
        /* Hack in accelerators for standard edit functions when in
         * picture mode.  These used to not be needed, since these
         * keys were truly accelerators, but when the new functionality
         * was added to the edit control, these keys were required to
         * go directly to the edit control, and thus could not be accels.
         * This fix was added to still support these accels in picture mode.
         * 22-May-1987. davidhab.
         */
            fShift = GetKeyState(VK_SHIFT) < 0;
            fControl = GetKeyState(VK_CONTROL) < 0;
            if (wParam == VK_DELETE && fShift && !fControl)
                wEditParam = CUT;
            else if (wParam == VK_INSERT && !fShift && fControl)
                wEditParam = COPY;
            else if (wParam == VK_INSERT && fShift && !fControl)
                wEditParam = PASTE;
            else
                return(FALSE);
            PostMessage(hIndexWnd, WM_COMMAND, wEditParam, 0L);
            return(TRUE);

        case VK_UP:
            y -= CharFixHeight;
            break;
        case VK_DOWN:
            y += CharFixHeight;
            break;
        case VK_LEFT:
            x -= CharFixWidth;
            break;
        case VK_RIGHT:
            x += CharFixWidth;
            break;
        default:
            return(FALSE);
        }

    if (x > (LINELENGTH-1) * CharFixWidth)
        x = (LINELENGTH-1) * CharFixWidth;
    else if (x < CharFixWidth - (dragRect.right - dragRect.left))
        x = CharFixWidth - (dragRect.right - dragRect.left);

    if (y > 10 * CharFixHeight)
        y = 10 * CharFixHeight;
    else if (y < CharFixHeight - (dragRect.bottom - dragRect.top))
        y = CharFixHeight - (dragRect.bottom - dragRect.top);

    if (x != dragRect.left || y != dragRect.top)
        {
        InvalidateRect(hCardWnd, (LPRECT)&dragRect, TRUE);
        CurCard.xBitmap = x;
        CurCard.yBitmap = y;
        OffsetRect((LPRECT)&dragRect, x-dragRect.left, y-dragRect.top);
        InvalidateRect(hCardWnd, (LPRECT)&dragRect, TRUE);
        }
    CurCardHead.flags |= FDIRTY;
    return(TRUE);
    }
