// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993-1995  Microsoft Corporation.  All Rights Reserved.
//
//  MODULE: client.c
//
//  PURPOSE: Contains all input/drawing routines for the GDI Input sample.
//
//  FUNCTIONS:
//    CreateClientWindow  - Creates the client window that handles input.
//    ClientWndProc       - Processes messages for the client window.
//    CmdDrawMode         - Changes the current drawing mode.
//    CmdFill             - Toggles the fill mode.
//    CmdCreatePen        - Puts up Pen Style dialog box.
//    CmdCreateBrush      - Puts up Brush Style dialog box.
//    MsgClientCreate     - Creates initial pens & brushes.
//    MsgClientDestroy    - Frees owned objects.
//    MsgClientKeyDown    - Looks for Escape key to cancel drawing.
//    MsgClientPaint      - Paints the client window.
//    MsgClientSize       - Updates scrollbars when window is resized
//    MsgClientScroll     - Handles scrolling
//
//    MsgClientLButtonDown
//    MsgClientMouseMove => Pass mouse messages to the current handlers.
//    MsgClientLButtonUp
//
//    PixelLBDown
//    PixelMouseMove    ==> Draw individual pixels with SetPixelV.
//    PixelLBUp
//
//    BezierLBDown
//    BezierMouseMove   ==> Select 4 points for drawing a Bezier curve.
//    BezierLBUp
//
//    RectLBDown
//    RectMouseMove     ==> Select 2 points for a line, rect, or ellipse.
//    RectLBUp
//
//    LineDraw              Given 2 points, draw a line, rect, or ellipse.
//    RectDraw          ==> These functions all take identical parameters
//    EllipseDraw           so they can be used interchangeably.
//
//    StartRubberBand     - Initiates drawing procedure.
//    EndRubberBand       - Ends drawing procedure.
//    ClientNewDrawing    - Called when the current picture changes
//    SetupScrollBars     - Turns scrollbars on/off
//
//  COMMENTS:
//

#include <windows.h>            // required for all Windows applications
#include <windowsx.h>
#include <commctrl.h>           // prototypes and defs for common controls
#include "globals.h"            // prototypes specific to this application
#include "resource.h"
#include "toolbar.h"            // prototypes for the tool bar
#include "statbar.h"            // prototypes for the status bar
#include "pendlg.h"             // ID's and prototypes for the Pen dialog
#include "brushdlg.h"           // ID's and prototypes for the Brush dialog
#include "palette.h"            // prototypes for palette message handlers
#include "metafile.h"

// Global variables local to this file

HPEN    hpenBlack;              // Useful stock objects
HBRUSH  hbrNull;

HPEN    hpenDraw;               // for drawing lines and frames
HBRUSH  hbrReal;                // for filling interiors
HBRUSH  hbrDraw;                // equals either hbrReal or hbrNull

LOGPEN  logPen;
LOGBRUSH logBrush;

HDC     hdcRB;                  // DC for Rubber Banding
BOOL    bDrawing;               // Flag to indicate when we are drawing
UINT    uDrawMode;              // IDM_LINE, IDM_RECT, etc.

HDC     hdcBitmap;              // Used with hbmBitmap
HBITMAP hbmBitmap;              // All drawing is cached here for faster screen updates
UINT    cxBitmap;               // Width of bitmap
UINT    cyBitmap;               // Height of bitmap
HBITMAP hbmStock;               // Stock 1x1 bitmap that mem DC's are initialized with

HDC     hdcEMF;                 // DC for recording to metafile
UINT    cxPict;                 // Dimensions of current metafile
UINT    cyPict;

#define BEZ_MAXPOINTS   4       // # of points in a Bezier curve
POINT   pPoints[BEZ_MAXPOINTS]; // Stores coordinates for various drawing fns
UINT    cPoints;                // Current # of points (for bezier)

// for scrolling
int cxHorzScrollPos = 0;
int cyVertScrollPos = 0;


// The following 3 function pointers are used to point to the current
// mouse handling functons.  Different drawing types can each define a
// set of mouse functions.  When a new drawing mode is selected, these
// pointers are set to point to the correct functions for that mode.
// The main mouse message handlers then dereference these pointers to
// call the correct handlers.

LRESULT  (*pfnLBDown)   (HWND, POINT);      // current mouse handler
LRESULT  (*pfnLBUp)     (HWND, POINT);      // functions for rubber
LRESULT  (*pfnMouseMove)(HWND, POINT);      // banding and such


// The Line, Rectangle, and Ellipse modes use a common set of mouse
// handlers that select 2 points.  The following function pointer is
// used so that the correct drawing function is always used.  Like the
// mouse function pointers, this is set when the drawing mode is
// selected.

BOOL (*pfnDrawRect)(HDC, POINT, POINT);     // current drawing function


// Mouse handling functions for different types of drawing.
LRESULT  PixelLBDown(HWND, POINT);          // Pixels
LRESULT  PixelLBUp(HWND, POINT);
LRESULT  PixelMouseMove(HWND, POINT);

LRESULT  BezierLBDown(HWND, POINT);         // Bezier Curves
LRESULT  BezierLBUp(HWND, POINT);
LRESULT  BezierMouseMove(HWND, POINT);

LRESULT  RectLBDown(HWND, POINT);           // 2-point selection
LRESULT  RectLBUp(HWND, POINT);             // Used for Lines, Rects,
LRESULT  RectMouseMove(HWND, POINT);        // and Ellipses

// Generic drawing functions for use with the above Rect functions
BOOL     LineDraw(HDC, POINT, POINT);
BOOL     RectDraw(HDC, POINT, POINT);
BOOL     EllipseDraw(HDC, POINT, POINT);


// Other helper functions
VOID StartRubberBand(HWND);
VOID EndRubberBand(HWND hwnd);
VOID SetupScrollBars(HWND, UINT, UINT);

// Client window message handling functions
static LRESULT MsgClientCreate      (HWND, UINT, WPARAM, LPARAM);
static LRESULT MsgClientDestroy     (HWND, UINT, WPARAM, LPARAM);
static LRESULT MsgClientMouseMove   (HWND, UINT, WPARAM, LPARAM);
static LRESULT MsgClientLButtonDown (HWND, UINT, WPARAM, LPARAM);
static LRESULT MsgClientLButtonUp   (HWND, UINT, WPARAM, LPARAM);
static LRESULT MsgClientKeyDown     (HWND, UINT, WPARAM, LPARAM);
static LRESULT MsgClientPaint       (HWND, UINT, WPARAM, LPARAM);
static LRESULT MsgClientSize        (HWND, UINT, WPARAM, LPARAM);
static LRESULT MsgClientScroll      (HWND, UINT, WPARAM, LPARAM);


// Client window message table definitions.
MSD rgmsdClient[] =
{
    {WM_MOUSEMOVE,      MsgClientMouseMove    },
    {WM_LBUTTONDOWN,    MsgClientLButtonDown  },
    {WM_LBUTTONUP,      MsgClientLButtonUp    },
    {WM_KEYDOWN,        MsgClientKeyDown      },
    {WM_CREATE,         MsgClientCreate       },
    {WM_DESTROY,        MsgClientDestroy      },
    {WM_PAINT,          MsgClientPaint        },
    {WM_SIZE,           MsgClientSize         },
    {WM_HSCROLL,        MsgClientScroll       },
    {WM_VSCROLL,        MsgClientScroll       },
};


MSDI msdiClient =
{
    sizeof(rgmsdClient) / sizeof(MSD),
    rgmsdClient,
    edwpWindow
};


//
//  FUNCTION: CreateClientWindow(HWND)
//
//  PURPOSE: Create the client window.
//
//  PARAMETERS:
//    hwndParent - The parent (main) window.
//
//  RETURN VALUE:
//    HWND of client window or null if failure.
//
//  COMMENTS:
//

HWND CreateClientWindow(HWND hwndParent)
{
    return CreateWindowEx(0,
                          "ClientWndClass",
                          NULL,
                          WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_HSCROLL | WS_VSCROLL,
                          -100, -100, 10, 10,   // dummy initial coordinates
                          hwndParent,
                          (HMENU)-1,
                          hInst,
                          NULL);
}


//
//  FUNCTION: ClientWndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the client window.
//
//  PARAMETERS:
//    hwnd     - window handle
//    uMessage - message number
//    wparam   - additional information (dependant on message number)
//    lparam   - additional information (dependant on message number)
//
//  RETURN VALUE:
//    The return value depends on the message number.  If the message
//    is implemented in the message dispatch table, the return value is
//    the value returned by the message handling function.  Otherwise,
//    the return value is the value returned by the default window procedure.
//
//  COMMENTS:
//    Call the DispMessage() function with the client window's message
//    dispatch information (msdiClient) and the message specific information.
//

LRESULT CALLBACK ClientWndProc(HWND   hwnd,
                               UINT   uMessage,
                               WPARAM wparam,
                               LPARAM lparam)
{
    return DispMessage(&msdiClient, hwnd, uMessage, wparam, lparam);
}


//
//  FUNCTION: CmdDrawMode(HWND, WORD, WORD, HWND)
//
//  PURPOSE: Handles WM_COMMAND messages to change drawing mode.
//
//  PARAMETERS:
//    hwnd - The main window.
//    wCommand - IDM_LINE, IDM_RECT, etc.
//    wNotify  - Notification number (unused)
//    hwndCtrl - NULL (unused)
//
//  RETURN VALUE:
//    Always returns 0 - command handled.
//
//  COMMENTS:
//

LRESULT CmdDrawMode(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    HMENU hMenu;

    if (wCommand == uDrawMode)  // First see if the mode has changed
        return 0;

    // Update the menu and toolbar states

    hMenu = GetMenu(hwnd);
    CheckMenuItem(hMenu, uDrawMode, MF_BYCOMMAND | MF_UNCHECKED);
    uDrawMode = wCommand;
    CheckMenuItem(hMenu, uDrawMode, MF_BYCOMMAND | MF_CHECKED);

    SendMessage(hWndToolbar, TB_CHECKBUTTON, uDrawMode, MAKELONG(TRUE, 0));

    // Set the pointers for the mouse handler functions to
    // the appropriate functions.

    switch(uDrawMode)
    {
        case IDM_PIXEL:
            pfnLBDown     = PixelLBDown;
            pfnLBUp       = PixelLBUp;
            pfnMouseMove  = PixelMouseMove;
            break;

        case IDM_BEZIER:
            cPoints       = 0;
            pfnLBDown     = BezierLBDown;
            pfnLBUp       = BezierLBUp;
            pfnMouseMove  = BezierMouseMove;
            break;

        case IDM_RECT:
            pfnDrawRect   = RectDraw;
            pfnLBDown     = RectLBDown;
            pfnLBUp       = RectLBUp;
            pfnMouseMove  = RectMouseMove;
            break;

        case IDM_ELLIPSE:
            pfnDrawRect   = EllipseDraw;
            pfnLBDown     = RectLBDown;
            pfnLBUp       = RectLBUp;
            pfnMouseMove  = RectMouseMove;
            break;

        default:
        case IDM_LINE:
            pfnDrawRect   = LineDraw;
            pfnLBDown     = RectLBDown;
            pfnLBUp       = RectLBUp;
            pfnMouseMove  = RectMouseMove;
            break;
    }

    return 0;
}


//
//  FUNCTION: CmdFill(HWND, WORD, WORD, HWND)
//
//  PURPOSE: Handles the IDM_FILL and IDM_NOFILL command messages.
//    Controls whether closed objects (rect's and ellipses) are filled
//    with a brush when drawn.
//
//  PARAMETERS:
//    hwnd     - The main window.
//    wCommand - IDM_FILL or IDM_NOFILL
//    wNotify  - Notification number (unused)
//    hwndCtrl - NULL (unused)
//
//  RETURN VALUE:
//    Always returns 0 - command handled.
//
//  COMMENTS:
//    The toolbar has 2 buttons (IDM_FILL and IDM_NOFILL) but the
//    menu only has IDM_FILL, so this function will turn filling
//    OFF if wCommand is IDM_NOFILL, and TOGGLE the ON/OFF state
//    if wCommand is IDM_FILL.
//

LRESULT CmdFill(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    static BOOL bFill;

    if (IDM_NOFILL == wCommand)
        bFill = FALSE;                          // turn filling OFF
    else
        bFill = !bFill;                         // toggle fill state

    // Update the menu and toolbar states

    CheckMenuItem(GetMenu(hwnd),
                  IDM_FILL,
                  MF_BYCOMMAND | (bFill ? MF_CHECKED : MF_UNCHECKED));

    SendMessage(hWndToolbar,
                TB_CHECKBUTTON,
                (bFill ? IDM_FILL : IDM_NOFILL),
                MAKELONG(TRUE, 0));

    // Update hbrDraw
    hbrDraw = (bFill ? hbrReal : hbrNull);
    SelectObject(hdcBitmap, hbrDraw);

    // If we're recording to a metafile, select the brush
    if (hdcEMF)
        SelectObject(hdcEMF, hbrDraw);

    return 0;
}


//
//  FUNCTION: CmdCreatePen(HWND, WORD, WORD, HWND)
//
//  PURPOSE: Handles the IDM_CREATEPEN command message. Puts up a dialog
//    to let the user set the pen style to use for drawing.
//
//  PARAMETERS:
//    hwnd     - The main window.
//    wCommand - IDM_CREATEPEN (unused)
//    wNotify  - Notification number (unused)
//    hwndCtrl - NULL (unused)
//
//  RETURN VALUE:
//    Always returns 0 - command handled.
//
//  COMMENTS:
//

LRESULT CmdCreatePen(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    LOGPEN lp = logPen;     // copy for the dialog to munge on

    if (DialogBoxParam(hInst, "PenDlg", hwnd, (DLGPROC)PenDlg, (LPARAM)&lp))
    {
        HPEN hpenNew = CreatePenIndirect(&lp);      // create new pen
        if (!hpenNew)
        {
            // Should report an error here...
            return 0;
        }

        // Select the new pen for drawing (deselect old pen before deleting)
        SelectObject(hdcBitmap, hpenNew);

        // If we're recording to a metafile, select the new pen
        if (hdcEMF)
            SelectObject(hdcEMF, hpenNew);

        DeleteObject(hpenDraw);                     // delete old pen
        hpenDraw = hpenNew;                         // save changes
        logPen = lp;
    }

    return 0;
}


//
//  FUNCTION: CmdCreateBrush(HWND, WORD, WORD, HWND)
//
//  PURPOSE: Handles the IDM_CREATEBRUSH command message. Puts up a dialog
//    to let the user set the brush style to use for drawing.
//
//  PARAMETERS:
//    hwnd - The main window.
//    wCommand - IDM_CREATEBRUSH (unused)
//    wNotify  - Notification number (unused)
//    hwndCtrl - NULL (unused)
//
//  RETURN VALUE:
//    Always returns 0 - command handled.
//
//  COMMENTS:
//

LRESULT CmdCreateBrush(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    LOGBRUSH lb = logBrush; // copy for the dialog to munge on

    if (DialogBoxParam(hInst, "BrushDlg", hwnd, (DLGPROC)BrushDlg, (LPARAM)&lb))
    {
        HBRUSH hbrNew = CreateBrushIndirect(&lb);   // create new brush
        if (!hbrNew)
        {
            // Should report an error here...
            return 0;
        }

        // If "Fill Objects" is turned on, we need to update hbrDraw
        if (GetMenuState(GetMenu(hwnd), IDM_FILL, MF_BYCOMMAND) & MF_CHECKED)
        {
            hbrDraw = hbrNew;                       // save new handle
            SelectObject(hdcBitmap, hbrDraw);       // select new brush
                                                    // (deselect old brush)

            // If we're recording to a metafile, select the new brush
            if (hdcEMF)
                SelectObject(hdcEMF, hbrDraw);
        }

        DeleteObject(hbrReal);                      // delete old brush
        hbrReal = hbrNew;                           // save changes
        logBrush = lb;
    }

    return 0;
}


//
//  FUNCTION: MsgClientCreate(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Handles the WM_CREATE message for the client window.
//
//  PARAMETERS:
//
//    hwnd      - Window handle  (Unused)
//    uMessage  - Message number (Unused)
//    wparam    - Extra data     (Unused)
//    lparam    - Extra data     (Unused)
//
//  RETURN VALUE:
//
//  COMMENTS:
//    Initializes various objects.
//

LRESULT MsgClientCreate(HWND   hwnd,
                        UINT   uMessage,
                        WPARAM wparam,
                        LPARAM lparam)
{
    HDC hdcTmp;
    RECT rc;

    // Initialize pens and brushes

    hpenBlack = GetStockObject(BLACK_PEN);
    hbrNull   = GetStockObject(NULL_BRUSH);

    hpenDraw  = hpenBlack;
    hbrReal   = GetStockObject(GRAY_BRUSH);

    hbrDraw = hbrReal;

    GetObject(hpenDraw, sizeof(logPen), &logPen);
    GetObject(hbrReal, sizeof(logBrush), &logBrush);

    // Create off-screen bitmap and DC

    hdcTmp = GetDC(hwnd);   // Screen DC for reference

    GetClientRect(hwnd, &rc);

    cxBitmap = max(rc.right, 1);
    cyBitmap = max(rc.bottom, 1);
    hbmBitmap = CreateCompatibleBitmap(hdcTmp, cxBitmap, cyBitmap);

    if (!hbmBitmap)
    {
        DeleteObject(hpenDraw);
        DeleteObject(hbrReal);

        OutputDebugString("MsgClientCreate: Unable to create bitmap\r\n");
        return -1;
    }

    // Create memory DC to hold bitmap
    hdcBitmap = CreateCompatibleDC(hdcTmp);
    ReleaseDC(hwnd, hdcTmp);  // Done with this

    // Initialize memory DC
    SelectObject(hdcBitmap, hpenDraw);
    SelectObject(hdcBitmap, hbrDraw);
    SetBkMode(hdcBitmap, TRANSPARENT);
    hbmStock = SelectObject(hdcBitmap, hbmBitmap);

    // Initialize bitmap and create a metafile dc for recording
    ClientNewDrawing();

    return 0;
}


//
//  FUNCTION: MsgClientDestroy(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Handles WM_DESTROY message for the client window.
//
//  PARAMETERS:
//
//    hwnd      - Window handle  (Unused)
//    uMessage  - Message number (Unused)
//    wparam    - Extra data     (Unused)
//    lparam    - Extra data     (Unused)
//
//  RETURN VALUE:
//
//    Always returns 0 - Message handled
//
//  COMMENTS:
//    General clean up happens here.
//

LRESULT MsgClientDestroy(HWND   hwnd,
                         UINT   uMessage,
                         WPARAM wparam,
                         LPARAM lparam)
{
    // If we're still recording a metafile, just close and delete it.
    // QuerySaveFile() should prompt the user to save changes prior
    // to getting to this point.
    if (hdcEMF)
    {
        DeleteEnhMetaFile(CloseEnhMetaFile(hdcEMF));
	}

    // Reset bitmap DC before deleting objects

    SelectObject(hdcBitmap, hpenBlack);
    SelectObject(hdcBitmap, hbrNull);
    SelectObject(hdcBitmap, hbmStock);

    if (hPalette)
    {
        SelectPalette(hdcBitmap, GetStockObject(DEFAULT_PALETTE), FALSE);
		DeleteObject(hPalette);
	}

    DeleteObject(hpenDraw);   // Delete pens/brushes
    DeleteObject(hbrReal);

    DeleteObject(hbmBitmap);  // Delete bitmap and memory DC
    DeleteDC(hdcBitmap);

    return 0;
}


//
//  FUNCTION: MsgClientKeyDown(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Handles WM_KEYDOWN message.
//
//  PARAMETERS:
//
//    hwnd      - Window handle
//    uMessage  - Message number (Unused)
//    wparam    - Key code (VK_xxx)
//    lparam    - Extra data     (Unused)
//
//  RETURN VALUE:
//
//  COMMENTS:
//    Looks for VK_ESCAPE key only.  If user hits the escape key
//    while drawing, cancel the drawing operation.
//

LRESULT MsgClientKeyDown(HWND   hwnd,
                         UINT   uMessage,
                         WPARAM wparam,
                         LPARAM lparam)
{
    if (VK_ESCAPE == wparam && bDrawing)
    {
        // End drawing.
        EndRubberBand(hwnd);
        cPoints = 0;

        // Force a repaint of the whole window to remove any junk
        // left by the cancelled drawing operation.
        InvalidateRect(hwnd, NULL, TRUE);
    }

    return 0;
}


//
//  FUNCTION: MsgClientPaint(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Handles WM_PAINT message.
//
//  PARAMETERS:
//
//    hwnd      - Window handle
//    uMessage  - Message number (Unused)
//    wparam    - Extra data     (Unused)
//    lparam    - Extra data     (Unused)
//
//  RETURN VALUE:
//
//  COMMENTS:
//    Calls PaintClientWindow() to do the actual painting.
//

LRESULT MsgClientPaint(HWND   hwnd,
                       UINT   uMessage,
                       WPARAM wparam,
                       LPARAM lparam)
{
    PAINTSTRUCT ps;
    HPALETTE hpalSave = NULL;   // Handle to previous palette

    BeginPaint(hwnd, &ps);

    if (hPalette)
    {
        hpalSave = SelectPalette(ps.hdc, hPalette, TRUE);
        RealizePalette(ps.hdc);
    }

    BitBlt(ps.hdc,                              // Destination DC
           ps.rcPaint.left,                     // Dest origin
           ps.rcPaint.top,
           ps.rcPaint.right - ps.rcPaint.left,  // Dest extents
           ps.rcPaint.bottom - ps.rcPaint.top,
           hdcBitmap,                           // Source DC
           ps.rcPaint.left + cxHorzScrollPos,   // Source origin
           ps.rcPaint.top  + cyVertScrollPos,
           SRCCOPY);                            // ROP code

    if (hPalette)
        SelectPalette(ps.hdc, hpalSave, TRUE);

    EndPaint(hwnd, &ps);
    return 0;
}


//
//  FUNCTION: MsgClientSize(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Repaints the client window and adjusts the scrollbars
//    when the main window's size has changed.
//
//  PARAMETERS:
//    hwnd - handle of Client window.
//    uMessage - WM_SIZE.
//    wParam - size flag (unused)
//    lParam - width/height of client area (unused)
//
//  RETURN VALUE:
//    0
//
//  COMMENTS:
//    If the window has been enlarged, this will enlarge the bitmap to
//    match the new window size.  The bitmap is never made smaller.
//    SetupScrollBars turns the scrollbars on/off as necessary. This can
//    cause more WM_SIZE messages, so a flag is used to prevent re-entrancy.
//

LRESULT MsgClientSize(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    HDC hdcTmp;
    HBITMAP hbmNew;
    HPALETTE hpalSave;
    UINT cxNew, cyNew;
    UINT cxWindow = LOWORD(lparam);
    UINT cyWindow = HIWORD(lparam);
    static BOOL bInSize = FALSE;

    // Are we already processing a WM_SIZE message?
    if (bInSize)
        return 0;

    bInSize = TRUE;

    if (hEMF)
    {        
        // If current thumb positions would cause blank space
        // at right or bottom of client area, repaint
        if ((cxHorzScrollPos + cxWindow > cxPict)
              || (cyVertScrollPos + cyWindow > cyPict))
            InvalidateRect(hwnd, NULL, TRUE);
    }

    // Set up the scroll bars appropriately.
    SetupScrollBars(hwnd, cxPict, cyPict);

    // The off-screen bitmap shoule be at least as big as the window
    // and the current metafile, if any.
    cxNew = max(cxPict, cxWindow);
    cyNew = max(cyPict, cyWindow);

    // See if we need a bigger bitmap
    if (cxBitmap >= cxNew && cyBitmap >= cyNew)
        goto SizeExit;

    // Use a screen DC to create the new bitmap so we get the
    // correct color format.
    hdcTmp = GetDC(hwnd);
    hbmNew = CreateCompatibleBitmap(hdcTmp, cxNew, cyNew);
    ReleaseDC(hwnd, hdcTmp);

    // If we couldn't create a new bitmap, just bail out now and leave
    // the old bitmap intact (that's better than having nothing).
    if (hbmNew == NULL)
        goto SizeExit;

    // Now we need a memory DC to copy old bitmap to new one.
    hdcTmp = CreateCompatibleDC(NULL);

    // Select new bitmap into permanent DC (this deselects the old bitmap)
    // and then select the old bitmap into temporary DC.
    SelectObject(hdcBitmap, hbmNew);
    SelectObject(hdcTmp, hbmBitmap);

    if (hPalette)
        hpalSave = SelectPalette(hdcTmp, hPalette, FALSE);

    // Initialize the 'extra' parts of the new bitmap with white

    if (cxWindow > cxBitmap)
        PatBlt(hdcBitmap, cxBitmap, 0, cxWindow, cyWindow, WHITENESS);

    if (cyWindow > cyBitmap)
        PatBlt(hdcBitmap, 0, cyBitmap, cxWindow, cyWindow, WHITENESS);

    // Copy old bitmap to new one
    BitBlt(hdcBitmap,
           0, 0,
           cxBitmap, cyBitmap,
           hdcTmp,
           0, 0,
           SRCCOPY);

    if (hEMF)
    {
        RECT rc = {0, 0, cxPict, cyPict};

        ExcludeClipRect(hdcBitmap, 0, 0, cxBitmap, cyBitmap);
        PlayEnhMetaFile(hdcBitmap, hEMF, &rc);
        SelectClipRgn(hdcBitmap, NULL);
    }

    // Clean up
    if (hPalette)
        SelectPalette(hdcTmp, hpalSave, TRUE);
    SelectObject(hdcTmp, hbmStock);
    DeleteObject(hbmBitmap);
    DeleteDC(hdcTmp);

    // Save new info
    hbmBitmap = hbmNew;
    cxBitmap = cxNew;
    cyBitmap = cyNew;

SizeExit:
    bInSize = FALSE;
    return 0;
}


//
//  FUNCTION: MsgClientScroll(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Scrolls the client window in response to scroll message.
//
//  PARAMETERS:
//    hwnd - handle of Client window. 
//    uMessage - the scroll message.
//    wParam - contains scroll position and scrollbar type
//    lParam - (unused)
//
//  RETURN VALUE:
//    None
//
//  COMMENTS:
//    MsgClientScroll performs scrolling in both horiziontal and vertical directions.
//    If the user clicks on one of the scrolling arrows, the window is scrolled
//    by (1 / SCROLL_RATIO) of the client area.  For example, if 
//    SCROLL_RATIO == 4, then the client area is moved over a 1/4 of the 
//    width or height (as the case may be) of the screen.  If the user pages
//    up/down the window is scrolled an amount equal to the client area's
//    width or height as the case may be. If the user moves the thumb to an 
//    absolute position, the window is scrolled accordingly.
//
//

#define SCROLL_RATIO 8

LRESULT MsgClientScroll(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    int nScrollCode;    // SB_HORZ if doing horizontal, SB_VERT if doing vertical
    int nPos;           // Scrollbar position.
    int nLine;          // # of pixels for LINEUP/LINEDOWN
    int nMove;          // How much to move.
    SCROLLINFO si;      // Current scroll info: nMin, nMax, nPage, nPos

    nScrollCode = (uMessage == WM_HSCROLL ? SB_HORZ : SB_VERT);

    // Get current scroll info
    si.cbSize = sizeof(si);
    si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS;

    GetScrollInfo(hwnd, nScrollCode, &si);

    // One page is always visible, so the true range that we allow scrolling over
    // is actually min to (max - page size), so adjust for that here.
    si.nMax -= si.nPage;

    nPos = si.nPos;     // Save the current position

    // On a SB_LINEUP/SB_LINEDOWN we will move the picture by
    //  1 / SCROLL_RATIO of the page size (i.e. if SCROLL_RATIO
    //  is 4, it will scroll one quarter of the window)

    nLine = si.nPage / SCROLL_RATIO;
    if (nLine == 0)
        nLine = 1;

    switch (GET_WM_HSCROLL_CODE(wparam, lparam))
    {
        case SB_LINEDOWN:             // One line right/down.
            nMove = nLine;
            break;

        case SB_LINEUP:               // One line left/up.
            nMove = -nLine;
            break;

        case SB_PAGEDOWN:             // One page right/down.
            nMove = si.nPage;
            break;

        case SB_PAGEUP:               // One page left/up.
            nMove = -(int)si.nPage;
            break;

        case SB_THUMBPOSITION:        // Absolute position.
            nMove = GET_WM_HSCROLL_POS(wparam, lparam) - nPos;
            break;

        default:                      // No change.
            nMove = 0;
            break;
    }

    if (nMove)
    {
        nPos += nMove;              // Calculate new scroll pos

        if (nPos < si.nMin)         // Make sure min <= pos <= max
        {
            nMove -= nPos - si.nMin;
            nPos = si.nMin;
        }

        if (nPos > si.nMax)
        {
            nMove -= nPos - si.nMax;
            nPos = si.nMax;
        }

        if (nMove)                  // Update the window's scroll pos and redraw
        {
            SetScrollPos(hwnd, nScrollCode, nPos, TRUE);

            // Set the global scroll pos variable
            if (nScrollCode == SB_HORZ)
                cxHorzScrollPos = nPos;
            else 
                cyVertScrollPos = nPos;

            RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_NOERASE);
        }
    }
    return 0;
}


//
//  FUNCTION: MsgClientLButtonDown(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Handles WM_LBUTTONDOWN message.
//
//  PARAMETERS:
//
//    hwnd      - Window handle
//    uMessage  - Message number (Unused)
//    wparam    - Extra data     (Unused)
//    lparam    - Mouse coordinates
//
//  RETURN VALUE:
//
//  COMMENTS:
//    Initiates dragging/drawing operation by saving the mouse
//    position and capturing mouse input.
//

LRESULT MsgClientLButtonDown(HWND   hwnd,
                             UINT   uMessage,
                             WPARAM wparam,
                             LPARAM lparam)
{
    POINT pt;

    // If we aren't already drawing, set focus to the client window
    // so we can get Escape key messages should the user decide to
    // quit drawing.

    if (!bDrawing)
        SetFocus(hwnd);

    // Get mouse position
    pt.x = (int)(short)LOWORD(lparam) + cxHorzScrollPos;
    pt.y = (int)(short)HIWORD(lparam) + cyVertScrollPos;

    // Call the current LBDown handler
    return (*pfnLBDown)(hwnd, pt);
}


//
//  FUNCTION: MsgClientMouseMove(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Handles WM_MOUSEMOVE message.
//
//  PARAMETERS:
//
//    hwnd      - Window handle
//    uMessage  - Message number (Unused)
//    wparam    - Extra data     (Unused)
//    lparam    - Mouse coordinates
//
//  RETURN VALUE:
//
//  COMMENTS:
//    Performs "rubber-banding" by erasing the previous image and
//    redrawing the object using an XOR ROP code.
//

LRESULT MsgClientMouseMove(HWND   hwnd,
                           UINT   uMessage,
                           WPARAM wparam,
                           LPARAM lparam)
{
    POINT pt;
    static char szBuf[20] ;     // Array for formatting mouse coordinates

    // Get new mouse position
    pt.x = (int)(short)LOWORD(lparam) + cxHorzScrollPos;
    pt.y = (int)(short)HIWORD(lparam) + cyVertScrollPos;

    // Update the status bar with the new position
    wsprintf(szBuf, "%d,%d", pt.x, pt.y);
    UpdateStatusBar(szBuf, 2, 0);

    // Call the current MouseMove handler
    return (*pfnMouseMove)(hwnd, pt);
}


//
//  FUNCTION: MsgClientLButtonUp(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Handles WM_LBUTTONUP message.
//
//  PARAMETERS:
//
//    hwnd      - Window handle
//    uMessage  - Message number (Unused)
//    wparam    - Extra data     (Unused)
//    lparam    - Mouse coordinates
//
//  RETURN VALUE:
//
//  COMMENTS:
//    Erases previous rubber-band image and draws the object in
//      the final position.
//

LRESULT MsgClientLButtonUp(HWND   hwnd,
                           UINT   uMessage,
                           WPARAM wparam,
                           LPARAM lparam)
{
    POINT pt;

    // Get new mouse position
    pt.x = (int)(short)LOWORD(lparam) + cxHorzScrollPos;
    pt.y = (int)(short)HIWORD(lparam) + cyVertScrollPos;

    // Call the current LBUp handler
    return (*pfnLBUp)(hwnd, pt);
}


//
//  FUNCTION: xxxLBDown(HWND, POINT)
//  FUNCTION: xxxMouseMove(HWND, POINT)
//  FUNCTION: xxxLBUp(HWND, POINT)
//
//  PURPOSE: Handles WM_LBUTTONDOWN, WM_MOUSEMOVE, and WM_LBUTTONUP
//
//  PARAMETERS:
//
//    hwnd - Window handle
//    pt   - Mouse coordinates
//
//  RETURN VALUE:
//    Always return 0
//
//  COMMENTS:
//    These functions are called by the actual Msgxxx mouse message
//    handler functions (above) to perform input processing.  Three
//    sets of these functions are implemented here.  One to input
//    individual pixels; one to select 2 points (used to draw a
//    line, rectangle, or ellipse); and one to select 4 points (for
//    drawing Bezier curves).  Additional functions could be "plugged
//    in" to support other types of drawing.
//
//    In general, xxxLBDown initializes the drawing process, for example
//    by saving the initial coordinates and calling StartRubberBand;
//    xxxMouseMove updates the coordinates; and xxxLBUp performs any
//    final processing and clean up (EndRubberBand).
//

//
//  FUNCTION: PixelLBDown(HWND, POINT)
//  FUNCTION: PixelMouseMove(HWND, POINT)
//  FUNCTION: PixelLBUp(HWND, POINT)
//
//  PURPOSE: Perform single point selection (individual pixels).
//

LRESULT PixelLBDown(HWND hwnd, POINT pt)
{
    // Get a DC and capture mouse input
    StartRubberBand(hwnd);
    SetROP2(hdcRB, R2_COPYPEN);

    // Draw the pixel with the current pen color into both the
    // rubber-banding DC and the bitmap DC.
    SetPixelV(hdcRB, pt.x, pt.y, logPen.lopnColor);
    SetPixelV(hdcBitmap, pt.x, pt.y, logPen.lopnColor);

    // If we're recording to a metafile, draw there as well
    if (hdcEMF)
        SetPixelV(hdcEMF, pt.x, pt.y, logPen.lopnColor);

    return 0;
}


LRESULT PixelMouseMove(HWND hwnd, POINT pt)
{
    // If we are currently drawing, draw the pixel
    if (bDrawing)
    {
        SetPixelV(hdcRB, pt.x, pt.y, logPen.lopnColor);
        SetPixelV(hdcBitmap, pt.x, pt.y, logPen.lopnColor);

        // If we're recording to a metafile, draw there as well
        if (hdcEMF)
            SetPixelV(hdcEMF, pt.x, pt.y, logPen.lopnColor);
    }

    return 0;
}


LRESULT PixelLBUp(HWND hwnd, POINT pt)
{
    // Don't draw the pixel again here (it's already drawn at LBDown
    // and MouseMove time).  Just need to clean up.

    if (bDrawing)
    {
        bModify = TRUE;
        EndRubberBand(hwnd);
    }

    return 0;
}


//
//  FUNCTION: BezierLBDown(HWND, POINT)
//  FUNCTION: BezierMouseMove(HWND, POINT)
//  FUNCTION: BezierLBUp(HWND, POINT)
//
//  PURPOSE: Perform 4-point selection for drawing Bezier curves.
//

LRESULT BezierLBDown(HWND hwnd, POINT pt)
{
    // If this is the first click, initialize rubber banding.

    if (0 == cPoints)
        StartRubberBand(hwnd);

    return 0;
}


LRESULT BezierMouseMove(HWND hwnd, POINT pt)
{
    int i;

    if (!bDrawing || 0 == cPoints)              // Are we currently drawing?
        return 0;

    // Are we rubber banding a Bezier?
    if (cPoints >= 2)
        PolyBezier(hdcRB, pPoints, BEZ_MAXPOINTS); // Erase previous Bezier

    // Erase previous line segment
    LineDraw(hdcRB, pPoints[cPoints - 1], pPoints[cPoints]);

    // Set the rest of the points to be the same as this one
    for (i = cPoints; i < BEZ_MAXPOINTS; i++)
        pPoints[i] = pt;

    // Draw new line segment
    LineDraw(hdcRB, pPoints[cPoints - 1], pPoints[cPoints]);

    // Are we rubber banding a Bezier?
    if (cPoints >= 2)
        PolyBezier(hdcRB, pPoints, BEZ_MAXPOINTS);

    return 0;
}


LRESULT BezierLBUp(HWND hwnd, POINT pt)
{
    int i;

    if (!bDrawing)          // Are we currently drawing?
        return 0;

    if (cPoints >= 2)       // Are we rubber banding a Bezier?
    {
        // Erase previous Bezier
        PolyBezier(hdcRB, pPoints, BEZ_MAXPOINTS);

        if (cPoints == BEZ_MAXPOINTS - 1) // Is this the last point?
        {
            // If so, then erase all line segments.
            for (i = BEZ_MAXPOINTS - 1; i > 0; i--)
                LineDraw(hdcRB, pPoints[i - 1], pPoints[i]);
        }
    }
    // else do *not* erase the previous line segments

    // Save new position and increment # of points
    pPoints[cPoints++] = pt;

    if (cPoints < BEZ_MAXPOINTS)    // Still more points to get.
    {
        // Set the rest of the points to be the same as the current one
        for (i = cPoints; i < BEZ_MAXPOINTS; i++)
            pPoints[i] = pt;

        // Draw new line segment
        LineDraw(hdcRB, pPoints[cPoints - 1], pPoints[cPoints]);

        // Once we have at least 2 points let's draw the Bezier curve
        if (cPoints >= 2)
            PolyBezier(hdcRB, pPoints, BEZ_MAXPOINTS);
    }
    else    // Finish this Bezier and quit drawing.
    {
        // Setup DC for 'real' drawing
        SetROP2(hdcRB, R2_COPYPEN);
        SetBkMode(hdcRB, TRANSPARENT);
        SelectObject(hdcRB, hpenDraw);

        // Draw Bezier in permanent position
        PolyBezier(hdcRB, pPoints, cPoints);

        // Draw it again into the bitmap DC
        PolyBezier(hdcBitmap, pPoints, cPoints);

        // If we're recording to a metafile, draw there as well
        if (hdcEMF)
            PolyBezier(hdcEMF, pPoints, cPoints);

        // De-select the pen and clean up rubber banding stuff.
        SelectObject(hdcRB, hpenBlack);
        EndRubberBand(hwnd);
        cPoints = 0;
        bModify = TRUE;
    }

    return 0;
}


//
//  FUNCTION: RectLBDown(HWND, UINT, WPARAM, LPARAM)
//  FUNCTION: RectMouseMove(HWND, UINT, WPARAM, LPARAM)
//  FUNCTION: RectLBUp(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Perform 2-point selection for drawing lines, rectangles,
//    and ellipses.
//

LRESULT RectLBDown(HWND hwnd, POINT pt)
{
    // Current position = starting position = mouse position
    pPoints[0] = pPoints[1] = pt;

    // Initialize rubber banding
    StartRubberBand(hwnd);

    // Draw initial state
    (*pfnDrawRect)(hdcRB, pPoints[0], pPoints[1]);

    return 0;
}


LRESULT RectMouseMove(HWND hwnd, POINT pt)
{
    if (!bDrawing)          // Are we currently drawing?
        return 0;

    // Un-draw previous position by redrawing
    (*pfnDrawRect)(hdcRB, pPoints[0], pPoints[1]);

    // Save new position
    pPoints[1] = pt;

    // Draw in new position
    (*pfnDrawRect)(hdcRB, pPoints[0], pPoints[1]);

    return 0;
}


LRESULT RectLBUp(HWND hwnd, POINT pt)
{
    if (!bDrawing)          // Are we currently drawing?
        return 0;

    // Un-draw previous position by redrawing
    (*pfnDrawRect)(hdcRB, pPoints[0], pPoints[1]);

    // Save new position
    pPoints[1] = pt;

    // Setup DC for 'real' drawing
    SetROP2(hdcRB, R2_COPYPEN);
    SetBkMode(hdcRB, TRANSPARENT);
    SelectObject(hdcRB, hpenDraw);
    SelectObject(hdcRB, hbrDraw);

    // Draw object in permanent position
    (*pfnDrawRect)(hdcRB, pPoints[0], pPoints[1]);

    // Draw it again into the bitmap DC
    (*pfnDrawRect)(hdcBitmap, pPoints[0], pPoints[1]);

    // If we're recording to a metafile, draw there as well
    if (hdcEMF)
        (*pfnDrawRect)(hdcEMF, pPoints[0], pPoints[1]);

    // De-select the pen and brush by selecting stock objects
    // and clean up rubber banding stuff.
    SelectObject(hdcRB, hpenBlack);
    SelectObject(hdcRB, hbrNull);
    EndRubberBand(hwnd);
    bModify = TRUE;

    return 0;
}


//
//  FUNCTION: xxxxDraw(HDC hdc, POINT pt1, POINT pt2)
//
//  PURPOSE: Draws a Line, Rectangle, or Ellipse at the coordinates
//    given by pt1 and pt2.
//
//  PARAMETERS:
//    hdc   - Device context to draw into.
//    pt1   - Beginning point (upper-left corner of rect)
//    pt2   - Ending point    (lower-right corner of rect)
//
//  RETURN VALUE:
//    TRUE for success, FALSE otherwise.
//
//  COMMENTS:
//    Assumes the DC is already correctly set up.
//
//

BOOL LineDraw(HDC hdc, POINT pt1, POINT pt2)
{
    MoveToEx(hdc, pt1.x, pt1.y, NULL);
    return LineTo(hdc, pt2.x, pt2.y);
}


BOOL RectDraw(HDC hdc, POINT pt1, POINT pt2)
{
    return Rectangle(hdc, pt1.x, pt1.y, pt2.x, pt2.y);
}


BOOL EllipseDraw(HDC hdc, POINT pt1, POINT pt2)
{
    return Ellipse(hdc, pt1.x, pt1.y, pt2.x, pt2.y);
}


//
//  FUNCTION: StartRubberBand(HWND)
//
//  PURPOSE: Sets up DC for rubber banding and captures the mouse.
//
//  PARAMETERS:
//    hwnd - Window for GetDC
//
//  RETURN VALUE:
//    None
//
//  COMMENTS:
//

VOID StartRubberBand(HWND hwnd)
{
    if (bDrawing)   // If we're already drawing, just return.
        return;

    hdcRB = GetDC(hwnd);                        // Get a DC to draw to

    // adjust the viewport origin for scrolled view
    SetViewportOrgEx(hdcRB, -cxHorzScrollPos, -cyVertScrollPos, NULL); 

    // Select our logical palette so palette-relative colors work correctly
    if (hPalette)
        SelectPalette(hdcRB, hPalette, TRUE);

    // R2_NOT causes drawing with a pen to invert the screen pixels.
    // This makes it easy to erase something by drawing it a 2nd time.
    SetROP2(hdcRB, R2_NOT);

    // For rubber banding with R2_NOT, we use a single pixel pen and
    // a NULL brush.
    SelectObject(hdcRB, hpenBlack);
    SelectObject(hdcRB, hbrNull);

    SetCapture(hwnd);                           // Capture mouse input
    bDrawing = TRUE;
}


//
//  FUNCTION: EndRubberBand(HWND, BOOL)
//
//  PURPOSE: Releases rubber banding DC and mouse capture.
//
//  PARAMETERS:
//    hwnd - Window for ReleaseDC
//    bSave - TRUE ==> save edits; FALSE ==> don't save
//
//  RETURN VALUE:
//    None
//
//  COMMENTS:
//

VOID EndRubberBand(HWND hwnd)
{
    // clean up the rubber-band DC
    if (hPalette)
        SelectPalette(hdcRB, GetStockObject(DEFAULT_PALETTE), TRUE);

    ReleaseDC(hwnd, hdcRB);

    // turn off drawing
    ReleaseCapture();
    bDrawing = FALSE;
}


//
//  FUNCTION: ClientNewDrawing(VOID)
//
//  PURPOSE: Reset everything... we've either just created a new
//    document or opened an existing one.
//
//  PARAMETERS:
//    None
//
//  RETURN VALUE:
//    None
//
//  COMMENTS:
//

VOID ClientNewDrawing(VOID)
{
    RECT rc;
    UINT cxNew, cyNew;
    HBITMAP hbmNew;
    HPALETTE hPal = NULL;
    HCURSOR hcurSave = SetCursor(hcursHourGlass);

    // Assume the user has already been asked to save changes
    if (hdcEMF)
    {
        DeleteEnhMetaFile(CloseEnhMetaFile(hdcEMF));
        hdcEMF = NULL;
    }

    if (hWndClient)
        GetClientRect(hWndClient, &rc);
    else
    {
        rc.right = rc.bottom = 0;
    }

    if (hEMF)
    {
        cxPict = PictureWidth(hEMF);
        cyPict = PictureHeight(hEMF);
        hPal   = CreateMetaPalette(hEMF);
    }
    else
    {
        cxPict = 0;
        cyPict = 0;
    }

    // Use a default palette if necessary
    if (bPalDevice && !hPal)
    {
        HDC hdc;

        hdc = GetDC(NULL);
        hPal = CreateHalftonePalette(hdc);
        ReleaseDC(NULL, hdc);
    }

    // Calculate necessary bitmap dimensions
    cxNew = max(cxPict, (UINT)rc.right);
    cyNew = max(cyPict, (UINT)rc.bottom);

    // Create a new bitmap if the current one isn't big enough
    if (cxNew > cxBitmap || cyNew > cyBitmap)
    {
        HDC hdcTemp = GetDC(NULL);

        hbmNew = CreateCompatibleBitmap(hdcTemp, cxNew, cyNew);
        ReleaseDC(NULL, hdcTemp);

        if (hbmNew)
        {
            // de-select and delete old bitmap
            SelectObject(hdcBitmap, hbmNew);
            DeleteObject(hbmBitmap);

            // save new bitmap info
            hbmBitmap = hbmNew;
            cxBitmap = cxNew;
            cyBitmap = cyNew;
        }
    }

    // Do we have a palette?
    if (hPal)
    {
        // Select the new palette (de-select the old one)
        SelectPalette(hdcBitmap, hPal, FALSE);

        if (hPalette)                       // Delete old palette
            DeleteObject(hPalette);

        hPalette = hPal;                    // Save new palette

        // Force foreground realization
        if (hWndClient)
            PostMessage(GetParent(hWndClient), WM_QUERYNEWPALETTE, 0, 0);
    }

    // Erase bitmap with white    
    PatBlt(hdcBitmap, 0, 0, cxBitmap, cyBitmap, WHITENESS);

    // Initialize bitmap
    if (hEMF)
    {
        SetRect(&rc, 0, 0, cxPict, cyPict);
        PlayEnhMetaFile(hdcBitmap, hEMF, &rc);
    }

    // Get a metafile DC to record changes to
    hdcEMF = GetDCFromEMF(hWndClient, hEMF, hPalette);

    // Initialize metafile DC
    SelectObject(hdcEMF, hpenDraw);
    SelectObject(hdcEMF, hbrDraw);
    SetBkMode(hdcEMF, TRANSPARENT);

    if (hWndClient)
	{
        // Turn scrollbars on/off
        SetupScrollBars(hWndClient, cxPict, cyPict);

        // Update the screen
        InvalidateRect(hWndClient, NULL, TRUE);
    }

    // Restore cursor
    SetCursor(hcurSave);
}


//
//  FUNCTION: SetScrollBar(HWND, int, UINT, UINT)
//
//  PURPOSE: Calculate scroll bar range and thumb size depending on the
//    size of the window and the size of the picture to display.
//
//  PARAMETERS:
//    hwnd     - handle of client window.
//    nCode    - either SB_HORZ or SB_VERT
//    cxWindow - dimension of client area (horz or vert)
//    cxPict   - dimension of picture (horz or vert)
//
//  RETURN VALUE:
//    The new scroll position.
//
//  COMMENTS:
//    Works for either horizontal or vertical scroll bars.  If no scroll
//    bar is necessary, the scroll bar is disabled rather than hidden.
//

int SetScrollBar(HWND hWnd, int nCode, UINT cxWindow, UINT cxPict)
{
    SCROLLINFO si;

    si.cbSize = sizeof(si);
    si.fMask  = SIF_RANGE | SIF_PAGE | SIF_DISABLENOSCROLL;
    si.nMin   = 0;
    si.nMax   = cxPict;
    si.nPage  = cxWindow;
    si.nPos   = 0;

    // Update the scrollbar and return new scroll position
    return SetScrollInfo(hWnd, nCode, &si, TRUE /* fRepaint */);
}


//
//  FUNCTION: SetupScrollBars(HWND, UINT, UINT)
//
//  PURPOSE: Adjust scroll bar ranges and thumb sizes depending on the size
//              of the window and the picture to display.
//
//  PARAMETERS:
//    hwnd - handle of client window.
//    cxPict - width of picture.
//    cyPict - height of picture.
//
//  RETURN VALUE:
//    none.
//
//  COMMENTS:
//    Calls SetScrollBar for each scroll bar to do the work
//

VOID SetupScrollBars(HWND hWnd, UINT cxPict, UINT cyPict)
{
    RECT       rcClient;                // Client area rectangle

    GetClientRect(hWnd, &rcClient);

    cxHorzScrollPos = SetScrollBar(hWnd, SB_HORZ, rcClient.right,  cxPict);
    cyVertScrollPos = SetScrollBar(hWnd, SB_VERT, rcClient.bottom, cyPict);
}


//
//  FUNCTION: GetCurrentEMF(VOID)
//
//  PURPOSE: Return a handle to a metafile containing the current
//    modifications.
//
//  PARAMETERS:
//    None
//
//  RETURN VALUE:
//    Returns a handle to an enhanced metafile or NULL
//
//  COMMENTS:
//

HENHMETAFILE GetCurrentEMF(VOID)
{
    HENHMETAFILE hemf;

    // Have we made any modifications?
    if (!bModify)
        return hEMF;

    if (!hdcEMF)
        return NULL;

    // Get metafile with current modifications from the metafile DC
    hemf = CloseEnhMetaFile(hdcEMF);

    // Create a new metafile DC and initialize it
    hdcEMF = GetDCFromEMF(hWndClient, hemf, hPalette);

    SelectObject(hdcEMF, hpenDraw);
    SelectObject(hdcEMF, hbrDraw);
    SetBkMode(hdcEMF, TRANSPARENT);

    return hemf;
}
