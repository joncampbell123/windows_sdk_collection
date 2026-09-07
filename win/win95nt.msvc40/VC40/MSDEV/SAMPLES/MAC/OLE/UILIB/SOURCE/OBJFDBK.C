/*
 * OBJFDBK.C
 *
 * Miscellaneous API's to generate UI feedback effects for OLE objects. This
 * is part of the OLE 2.0 User Interface Support Library.
 * The following feedback effects are supported:
 *      1. Object selection handles (OleUIDrawHandles)
 *      2. Open Object window shading (OleUIDrawShading)
 *
 * Copyright (c)1992-1994 Microsoft Corporation, All Right Reserved
 */

#if defined(USEHEADER)
#include "OleHeaders.h"
#endif

#if defined(__MWERKS__) && !defined(__powerc)
#pragma pointers_in_D0
#endif


#if defined(_MSC_VER) && defined(__powerc)
#include <msvcmac.h>
#endif



#define STRICT  1

#include <ole2.h>

#include "ole2ui.h"
#include "uidebug.h"
#include "common.h"


OLEDBGDATA

static void DrawHandle(GrafPtr drawPort, short x, short y, short cSize, Boolean bInvert);

/*
 * OleUIDrawHandles
 *
 * Purpose:
 *  Draw handles or/and boundary around Container Object when selected
 *
 * Parameters:
 *  lpRect      Dimensions of Container Object
 *  hdc         HDC of Container Object (MM_TEXT mapping mode)
 *  dwFlags-
 *      Exclusive flags
 *          OLEUI_HANDLES_INSIDE    Draw handles on inside of rect
 *          OLEUI_HANDLES_OUTSIDE   Draw handles on outside of rect
 *      Optional flags
 *          OLEUI_HANDLES_NOBORDER  Draw handles only, no rect
 *          OLEUI_HANDLES_USEINVERSE
 *              use invert for handles and rect, o.t. use COLOR_WINDOWTEXT
 *  cSize       size of handle box
 *
 * Return Value: null
 *
 */
STDAPI_(void) OleUIDrawHandles(
	Rect*  			pRect,
	GrafPtr    		drawPort,
	unsigned long   dwFlags,
	short    		cSize
)
{
	Rect    	rc;
	GrafPtr		oldPort;
	PenState	oldPS;
	Boolean    	bInvert = (dwFlags & OLEUI_HANDLES_USEINVERSE);
#ifdef UIDLL
   void*    oldQD = SetLpqdFromA5();
#endif

	rc = *pRect;

	if (dwFlags & OLEUI_HANDLES_OUTSIDE)
		InsetRect(&rc, (short)-cSize, (short)-cSize);

	// Draw the handles inside the rectangle boundary
	DrawHandle(drawPort, rc.left, rc.top, cSize, bInvert);
	DrawHandle(drawPort, rc.left, (short)(rc.top+(rc.bottom-rc.top-cSize)/2), cSize, bInvert);
	DrawHandle(drawPort, rc.left, (short)(rc.bottom-cSize), cSize, bInvert);
	DrawHandle(drawPort, (short)(rc.left+(rc.right-rc.left-cSize)/2), rc.top, cSize, bInvert);
	DrawHandle(drawPort, (short)(rc.left+(rc.right-rc.left-cSize)/2), (short)(rc.bottom-cSize), cSize, bInvert);
	DrawHandle(drawPort, (short)(rc.right-cSize), rc.top, cSize, bInvert);
	DrawHandle(drawPort, (short)(rc.right-cSize), (short)(rc.top+(rc.bottom-rc.top-cSize)/2), cSize, bInvert);
	DrawHandle(drawPort, (short)(rc.right-cSize), (short)(rc.bottom-cSize), cSize, bInvert);

	if (!(dwFlags & OLEUI_HANDLES_NOBORDER)) {
		GetPort(&oldPort);
		SetPort(drawPort);
		
		GetPenState(&oldPS);
		
		PenNormal();
		FrameRect(&rc);
		
		SetPenState(&oldPS);
		
		SetPort(oldPort);
	}

#ifdef UIDLL
   RestoreLpqd(oldQD);
#endif

}


/*
 * DrawHandle
 *
 * Purpose:
 *  Draw a handle box at the specified coordinate
 *
 * Parameters:
 *  drawPort    GrafPort to be drawn into
 *  x, y        upper left corner coordinate of the handle box
 *  cSize       size of handle box
 *  bInvert     use patXor if true, use patCopy if false
 *
 * Return Value: null
 *
 */
static void DrawHandle(GrafPtr drawPort, short x, short y, short cSize, Boolean bInvert)
{
	GrafPtr		oldPort;
	Rect		rc;
	PenState 	oldPS;
#ifdef UIDLL
   void*    oldQD = SetLpqdFromA5();
#endif

	GetPort(&oldPort);
	SetPort(drawPort);

	rc.top = y;
	rc.left = x;
	rc.bottom = y + cSize;
	rc.right = x + cSize;
	
	if (!bInvert) {
		FillRect(&rc, (ConstPatternParam)&qd.black);
	}
	else {
		GetPenState(&oldPS);
		PenMode(patXor);
		PenPat((ConstPatternParam)&qd.black);
		PaintRect(&rc);
		SetPenState(&oldPS);
	}
	
	SetPort(oldPort);
#ifdef UIDLL
   RestoreLpqd(oldQD);
#endif
}


/*
 * OleUIDrawShading
 *
 * Purpose:
 *  Shade the object when it is in in-place editing. Borders are drawn
 *  on the Object rectangle. The right and bottom edge of the rectangle
 *  are excluded in the drawing.
 *
 * Parameters:
 *  lpRect      Dimensions of Container Object
 *  hdc         HDC for drawing
 *  dwFlags-
 *      Exclusive flags
 *          OLEUI_SHADE_FULLRECT    Shade the whole rectangle
 *          OLEUI_SHADE_BORDERIN    Shade cWidth pixels inside rect
 *          OLEUI_SHADE_BORDEROUT   Shade cWidth pixels outside rect
 *      Optional flags
 *          OLEUI_SHADE_USEINVERSE
 *              use PATINVERT instead of the hex value
 *  cWidth      width of border in pixel
 *
 * Return Value: null
 *
 */
STDAPI_(void) OleUIDrawShading(Rect* pRect, GrafPtr drawPort, unsigned long dwFlags, short cWidth)
{
	Rect    	rc;
	GrafPtr		oldPort;
	Pattern    	penPat = {0x11, 0x22, 0x44, 0x88, 0x11, 0x22, 0x44, 0x88};
	PenState	oldPS;
#ifdef UIDLL
   void*    oldQD = SetLpqdFromA5();
#endif

	rc = *pRect;
	GetPort(&oldPort);
	SetPort(drawPort);
	
	GetPenState(&oldPS);
	PenPat((ConstPatternParam)&penPat);
	
	if (dwFlags == OLEUI_SHADE_FULLRECT) {
		PenMode(patBic);
		PaintRect(&rc);
		PenMode(patXor);
		PaintRect(pRect);

	} else {    // either inside or outside rect

		Rect	rcDraw;
		
		if (dwFlags == OLEUI_SHADE_BORDEROUT)
			InsetRect(&rc, (short)-cWidth, (short)-cWidth);

		/* top bar */
		rcDraw = rc;
		rcDraw.bottom = rc.top + cWidth;
		PenMode(patBic);
		PaintRect(&rcDraw);
		PenMode(patXor);
		PaintRect(&rcDraw);
		
		/* left bar */
		rcDraw = rc;
		rcDraw.right = rc.left + cWidth;
		PenMode(patBic);
		PaintRect(&rcDraw);
		PenMode(patXor);
		PaintRect(&rcDraw);

		/* right bar */
		rcDraw = rc;
		rcDraw.left = rc.right - cWidth;
		PenMode(patBic);
		PaintRect(&rcDraw);
		PenMode(patXor);
		PaintRect(&rcDraw);

		/* bottom bar */
		rcDraw = rc;
		rcDraw.top = rc.bottom - cWidth;
		PenMode(patBic);
		PaintRect(&rcDraw);
		PenMode(patXor);
		PaintRect(&rcDraw);
	}

	SetPenState(&oldPS);
	
	SetPort(oldPort);
#ifdef UIDLL
   RestoreLpqd(oldQD);
#endif
}


/*
 * OleUIShowObject
 *
 * Purpose:
 *  Draw the ShowObject effect around the object
 *
 * Parameters:
 *  lprc        rectangle for drawing
 *  hdc         HDC for drawing
 *  fIsLink     linked object (TRUE) or embedding object (FALSE)
 *
 * Return Value: null
 *
 */
STDAPI_(void) OleUIShowObject(Rect* pRect, GrafPtr drawPort, Boolean fIsLink)
{
	Rect    	rc;
	GrafPtr		oldPort;
	PenState	oldPS;
#ifdef UIDLL
   void*    oldQD = SetLpqdFromA5();
#endif

	rc = *pRect;
	GetPort(&oldPort);
	SetPort(drawPort);
	
	GetPenState(&oldPS);
	PenNormal();
	
	if (fIsLink) {
		Pattern    	penPat = {0xf0, 0xf0, 0xf0, 0xf0, 0x0f, 0x0f, 0x0f, 0x0f};

		PenPat((ConstPatternParam)&penPat);
	}

	FrameRect(pRect);
	
	SetPenState(&oldPS);
	SetPort(oldPort);
#ifdef UIDLL
   RestoreLpqd(oldQD);
#endif
}
