// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993-1995  Microsoft Corporation.  All Rights Reserved.
//
//  MODULE:   misc.c
//
//  PURPOSE:  Contains all helper functions "global" to the application.
//
//  FUNCTIONS:
//    CenterWindow   - Center one window over another.
//	  DrawResizeLine - Handles drawing the resizing line while it is being 
//					   moved
//
//  COMMENTS:
//
//

#include <windows.h>            // required for all Windows applications
#include <windowsx.h>
#include "globals.h"            // prototypes specific to this application



//
//  FUNCTION: CenterWindow(HWND, HWND)
//
//  PURPOSE:  Center one window over another.
//
//  PARAMETERS:
//    hwndChild - The handle of the window to be centered.
//    hwndParent- The handle of the window to center on.
//
//  RETURN VALUE:
//
//    TRUE  - Success
//    FALSE - Failure
//
//  COMMENTS:
//
//    Dialog boxes take on the screen position that they were designed
//    at, which is not always appropriate. Centering the dialog over a
//    particular window usually results in a better position.
//

BOOL CenterWindow(HWND hwndChild, HWND hwndParent)
{
    RECT    rcChild, rcParent;
    int     cxChild, cyChild, cxParent, cyParent;
    int     cxScreen, cyScreen, xNew, yNew;
    HDC     hdc;

    // Get the Height and Width of the child window
    GetWindowRect(hwndChild, &rcChild);
    cxChild = rcChild.right - rcChild.left;
    cyChild = rcChild.bottom - rcChild.top;

    // Get the Height and Width of the parent window
    GetWindowRect(hwndParent, &rcParent);
    cxParent = rcParent.right - rcParent.left;
    cyParent = rcParent.bottom - rcParent.top;

    // Get the display limits
    hdc = GetDC(hwndChild);
    cxScreen = GetDeviceCaps(hdc, HORZRES);
    cyScreen = GetDeviceCaps(hdc, VERTRES);
    ReleaseDC(hwndChild, hdc);

    // Calculate new X position, then adjust for screen
    xNew = rcParent.left + ((cxParent - cxChild) / 2);
    if (xNew < 0)
    {
        xNew = 0;
    }
    else if ((xNew + cxChild) > cxScreen)
    {
        xNew = cxScreen - cxChild;
    }

    // Calculate new Y position, then adjust for screen
    yNew = rcParent.top  + ((cyParent - cyChild) / 2);
    if (yNew < 0)
    {
        yNew = 0;
    }
    else if ((yNew + cyChild) > cyScreen)
    {
        yNew = cyScreen - cyChild;
    }

    // Set it, and return
    return SetWindowPos(hwndChild,
                        NULL,
                        xNew, yNew,
                        0, 0,
                        SWP_NOSIZE | SWP_NOZORDER);
}



//
//  FUNCTION: DrawResizeLine(HWND, LONG)
//
//  PURPOSE:  Draws a transparent line while the resizing line is being
//			  moved.
//
//  PARAMETERS:
//    hwnd      - window where the resizing line is being drawn
//    yPos      - vertical position to draw the line at
//
//  RETURN VALUE:
//	  nothing
//
//  COMMENTS:
//
//    This function draws using the R2_NOT style so the line is made up
//    of the reverse of the pixels it is over.  This function also handles
//    removing the line due to the R2_NOT style since it just reverses
//	  the pixels.  
//

void DrawResizeLine(HWND hwnd, LONG yPos)
{
	HPEN hPen, hPenOld;		// pen for line and previous pen
	HDC hdc;				// window's HDC
	RECT rc;				// window client rectangle

	// Create a pen to draw the resizing line with
	hPen = CreatePen(PS_SOLID, 5, RGB(0, 0, 0));

	// get the window's client area so we know how wide to make the line
	GetClientRect(hwnd, &rc);

	hdc = GetDC(hwnd);					// get the window's DC
	hPenOld = SelectObject(hdc, hPen);	// use the pen we just created
	SetROP2(hdc, R2_NOT);				// just invert the pixels

	MoveToEx(hdc, 0, yPos, NULL);
	LineTo(hdc, rc.right, yPos);

	// clean up after ourselves
	SelectObject(hdc, hPenOld);			// put the old pen back
	SetROP2(hdc, R2_NOP);	   			// reset the ROP2 code
	ReleaseDC(hwnd, hdc);
		
	DeleteObject(hPen);					// delete the pen
}


