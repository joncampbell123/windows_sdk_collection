/*
 * CHATCH.CPP
 *
 * Implementation of a class to manage a hatch border around an
 * in-place editing window.  This is a modification of the OLE2UI
 * hatch window because we want to use this window to forward
 * messages.
 *
 * Copyright (c)1993-1994 Microsoft Corporation, All Right Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#define INC_OLE2
#include <windows.h>
#include <ole2.h>
#include <ole2ui.h>     //Need for OleUIDrawShading, etc.

#include "classlib.h"


/*
 * FHatchWindowRegister
 *
 * Purpose:
 *  Registers the hatch window class for use with CHatchWin.
 *
 * Parameters:
 *  hInst           HINSTANCE under which to register.
 *
 * Return Value:
 *  BOOL            TRUE if successful, FALSE otherwise.
 */

BOOL FHatchWindowRegister(HINSTANCE hInst)
    {
    WNDCLASS    wc;

    //Must have CS_DBLCLKS for border!
    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.hInstance     = hInst;
    wc.cbClsExtra    = 0;
    wc.lpfnWndProc   = HatchWndProc;
    wc.cbWndExtra    = CBHATCHWNDEXTRA;
    wc.hIcon         = NULL;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = SZCLASSHATCHWIN;

    return RegisterClass(&wc);
        return FALSE;
    }




/*
 * CHatchWin:CHatchWin
 * CHatchWin::~CHatchWin
 *
 * Constructor Parameters:
 *  hInst           HINSTANCE of the application we're in.
 */

CHatchWin::CHatchWin(HINSTANCE hInst)
    : CWindow(hInst)
    {
    m_hWnd=NULL;
    m_hWndKid=NULL;
    m_hWndAssociate=NULL;
    m_uID=0;

    m_dBorder=GetProfileInt(TEXT("windows")
        , TEXT("OleInPlaceBorderWidth"), HATCHWIN_BORDERWIDTHDEFAULT);

    return;
    }


CHatchWin::~CHatchWin(void)
    {
    /*
     * Chances are this was already destroyed when a document
     * was destroyed.
     */
    if (NULL!=m_hWnd && IsWindow(m_hWnd))
        DestroyWindow(m_hWnd);

    return;
    }



/*
 * CHatchWin::FInit
 *
 * Purpose:
 *  Instantiates a hatch window within a given parent with a
 *  default rectangle.  This is not initially visible.
 *
 * Parameters:
 *  hWndParent      HWND of the parent of this window
 *  uID             UINT identifier for this window (send in
 *                  notifications to associate window).
 *  hWndAssoc       HWND of the initial associate.
 *
 * Return Value:
 *  BOOL            TRUE if the function succeeded, FALSE otherwise.
 */

BOOL CHatchWin::FInit(HWND hWndParent, UINT uID, HWND hWndAssoc)
    {
    m_hWnd=CreateWindowEx(WS_EX_NOPARENTNOTIFY, SZCLASSHATCHWIN
        , SZCLASSHATCHWIN, WS_CHILD | WS_CLIPSIBLINGS
        | WS_CLIPCHILDREN, 0, 0, 100, 100, hWndParent, (HMENU)uID
        , m_hInst, this);

    m_uID=uID;
    m_hWndAssociate=hWndAssoc;

    return (NULL!=m_hWnd);
    }




/*
 * CHatchWin::HwndAssociateSet
 * CHatchWin::HwndAssociateGet
 *
 * Purpose:
 *  Sets (Set) or retrieves (Get) the associate window of the
 *  hatch window.
 *
 * Parameters: (Set only)
 *  hWndAssoc       HWND to set as the associate.
 *
 * Return Value:
 *  HWND            Previous (Set) or current (Get) associate
 *                  window.
 */

HWND CHatchWin::HwndAssociateSet(HWND hWndAssoc)
    {
    HWND    hWndT=m_hWndAssociate;

    m_hWndAssociate=hWndAssoc;
    return hWndT;
    }

HWND CHatchWin::HwndAssociateGet(void)
    {
    return m_hWndAssociate;
    }





/*
 * CHatchWin::RectsSet
 *
 * Purpose:
 *  Changes the size and position of the hatch window and the child
 *  window within it using a position rectangle for the child and
 *  a clipping rectangle for the hatch window and child.  The hatch
 *  window occupies prcPos expanded by the hatch border and clipped
 *  by prcClip.  The child window is fit to prcPos to give the
 *  proper scaling, but it clipped to the hatch window which
 *  therefore clips it to prcClip without affecting the scaling.
 *
 * Parameters:
 *  prcPos          LPRECT providing the position rectangle.
 *  prcClip         LPRECT providing the clipping rectangle.
 *
 * Return Value:
 *  None
 */

void CHatchWin::RectsSet(LPRECT prcPos, LPRECT prcClip)
    {
    RECT    rc;
    RECT    rcPos;

    //Calculate the rectangle for the hatch window, then clip it.
    rcPos=*prcPos;
    InflateRect(&rcPos, m_dBorder, m_dBorder);
    IntersectRect(&rc, &rcPos, prcClip);

    SetWindowPos(m_hWnd, NULL, rc.left, rc.top, rc.right-rc.left
        , rc.bottom-rc.top, SWP_NOZORDER | SWP_NOACTIVATE);

    /*
     * Set the rectangle of the child window to be at m_dBorder
     * from the top and left but with the same size as prcPos
     * contains.  The hatch window will clip it.
     */

    SetWindowPos(m_hWndKid, NULL, rcPos.left-rc.left+m_dBorder
        , rcPos.top-rc.top+m_dBorder, prcPos->right-prcPos->left
        , prcPos->bottom-prcPos->top, SWP_NOZORDER | SWP_NOACTIVATE);

    return;
    }



/*
 * CHatchWin::ChildSet
 *
 * Purpose:
 *  Assigns a child window to this hatch window.
 *
 * Parameters:
 *  hWndKid         HWND of the child window.
 *
 * Return Value:
 *  None
 */

void CHatchWin::ChildSet(HWND hWndKid)
    {
    m_hWndKid=hWndKid;

    if (NULL!=hWndKid)
        {
        SetParent(hWndKid, m_hWnd);

        //Insure this is visible when the hatch window becomes visible.
        ShowWindow(hWndKid, SW_SHOW);
        }

    return;
    }


/*
 * HatchWndProc
 *
 * Purpose:
 *  Standard window procedure for the Hatch Window
 */

LRESULT APIENTRY HatchWndProc(HWND hWnd, UINT iMsg
    , WPARAM wParam, LPARAM lParam)
    {
    PCHatchWin  phw;
    int         d;
    HDC         hDC;
    PAINTSTRUCT ps;
    RECT        rc;
            
    phw=(PCHatchWin)GetWindowLong(hWnd, HWWL_STRUCTURE);

    switch (iMsg)
        {
        case WM_CREATE:
            phw=(PCHatchWin)((LPCREATESTRUCT)lParam)->lpCreateParams;
            SetWindowLong(hWnd, HWWL_STRUCTURE, (LONG)phw);
            break;


        case WM_PAINT: 
            hDC=BeginPaint(hWnd, &ps);
            GetClientRect(hWnd, &rc);

            d=phw->m_dBorder;

            //Always draw the hatching.
            OleUIDrawShading(&rc, hDC, OLEUI_SHADE_BORDERIN, d);

            EndPaint(hWnd, &ps);
            break;


        case WM_SETFOCUS:
            //We need this since the container will SetFocus to us.
            if (NULL!=phw->m_hWndKid)
                SetFocus(phw->m_hWndKid);

            break;


        case WM_LBUTTONDBLCLK:
            /*
             * If the double click was within m_dBorder of an
             * edge, send the HWN_BORDERDOUBLECLICKED notification.
             *
             * Because we're always sized just larger than our child
             * window by the border width, we can only *get* this
             * message when the mouse is on the border.  So we can
             * just send the notification.
             */

            if (NULL!=phw->m_hWndAssociate)
                {
                SendCommand(phw->m_hWndAssociate, phw->m_uID
                    , HWN_BORDERDOUBLECLICKED, hWnd);
                }

            break;


        default:
            return DefWindowProc(hWnd, iMsg, wParam, lParam);
        }
    
    return 0L;
    }
