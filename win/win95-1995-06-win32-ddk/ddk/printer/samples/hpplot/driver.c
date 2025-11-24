/* (c) Copyright 1987 MICROGRAFX, Inc., All Rights Reserved.  Permission to
   use this work for any purpose must be obtained in writing from MICROGRAFX,
   1820 N. Greenville Ave., Richardson, Tx.  75081.

   This file is Micrografx proprietary information and permission for
   Hewlett-Packard to distribute any part of this file outside its peripheral
   division must obtained in writing from MICROGRAFX, 1820 N. Greenville Ave.,
   Richardson, Tx.  75081.

*******************************************************************************
*******************************************************************************

				  driver

*******************************************************************************
*******************************************************************************

HISTORY:
    05SEP89 (NVF)
	Eliminated rounding error due to subtracting 1 from x & y points in
	OS_ARC handler.

*/

#define PRINTDRIVER
#define NOPQ
#define NOINSTALLFILES
#include <print.h>
#include <gdidefs.inc>

#include "device.h"
#include "driver.h"
#include "color.h"
#include "control.h"
#include "dialog.h"
#include "glib.h"
#include "initlib.h"
#include "object.h"
#include "output.h"
#include "print.h"
#include "profile.h"
#include "text.h"
#include "utils.h"

/* ****************************** History ********************************** */
/* 10/31/87 (RWM) - signoff                                                  */
/* ***************************** Constants ********************************* */

#define LOCAL

/* **************************** Local Data ********************************* */
/* *************************** Exported Data ******************************* */

char MessageBuffer [BUFSIZE];
int WriteError = 0;                     /* Negative if WriteError       */

/* ***************************  DEBUG ROUTINES  **************************** */

#ifdef NICK_DEBUG

OutputASpace()
{
    OutputDebugString(" ");
}

OutputAReturn()
{
    OutputDebugString("\r\n");
}
#endif

/* *************************** Local Routines ****************************** */
LOCAL BOOL FAR PASCAL DetermineClipping (LPPDEVICE,LPRECT,LPPOINT);

LOCAL void FAR PASCAL ConstrainHLS (lpHLS)
  /* Constrain an HLS color to certain regions of the color wheel.
     If the color is grey, we force it to white or black.
     If not, we force it out to the rim of the wheel. */
LPHLSBLOCK lpHLS;
{
    if ((lpHLS->Saturation) < 500)
    {
	    /* It's grey. */
	if ((lpHLS->Lightness) < 500)
	    (lpHLS->Lightness) = 0;
	else
	    (lpHLS->Lightness) = 1000;
	(lpHLS->Saturation) = 0;
	(lpHLS->Hue) = 0;
    }
    else
    {
	/* Force it out to the rim. */
	(lpHLS->Lightness) = 500;
	(lpHLS->Saturation) = 1000;
    }
}

LOCAL void FAR PASCAL RGB_to_HLS (Color,lpHLSResult)
  /* This is a support function for the color distance calculation function.
     It will convert the given RGB color into HLS format and copy the result
     to the supplied HLS structure pointer. */
DWORD Color;
LPHLSBLOCK lpHLSResult;
{
int Maximum,
    Minimum,
    Difference,
    Sum,
    Red,
    Green,
    Blue,
    red,
    green,
    blue;

/* Map a a color from the RGB to the HLS color space.
   This algorithm is from appendix B of the SIGGRAPH CORE spec.
   NOTE: We keep all fractionals in the range 0:1000. */
    Red   = (int)((Color&0x000000FF)*1000L/255L);
    Green = (int)(((Color>>8)&0x000000FF)*1000L/255L);
    Blue  = (int)(((Color>>16)&0x000000FF)*1000L/255L);
    if (Red > Green)
    {
	if (Red > Blue) Maximum = Red;
	    else Maximum = Blue;
	if (Green < Blue) Minimum = Green;
	    else Minimum = Blue;
    }
    else
    {
	if (Red < Blue) Minimum = Red;
	    else Minimum = Blue;
	if (Green > Blue) Maximum = Green;
	    else Maximum = Blue;
    }
    if (Difference = (Maximum - Minimum))
    {
	red   = (int)(1000L*(Maximum -   Red) / Difference);
	green = (int)(1000L*(Maximum - Green) / Difference);
	blue  = (int)(1000L*(Maximum -  Blue) / Difference);
    }
    Sum = (Maximum + Minimum);
    (lpHLSResult->Lightness) = Sum/2;
    if (!Difference)
	(lpHLSResult->Saturation) = 0;
    else if ((lpHLSResult->Lightness) <= 500)
	(lpHLSResult->Saturation) = (int)(1000L*Difference / Sum);
    else
	(lpHLSResult->Saturation) = (int)(1000L*Difference / (2000 - Sum));
    if (!(lpHLSResult->Saturation))
	(lpHLSResult->Hue) = 0;
    else if (Red == Maximum)
	(lpHLSResult->Hue) = 2000+blue-green;
    else if (Green == Maximum)
	(lpHLSResult->Hue) = 4000+red-blue;
    else
	(lpHLSResult->Hue) = 6000+green-red;
/* Convert Hue to degrees. */
    (lpHLSResult->Hue) = (int)(((lpHLSResult->Hue)*6L)/100);
    while ((lpHLSResult->Hue) < 0)
	(lpHLSResult->Hue) += 360;
    while ((lpHLSResult->Hue) >= 360)
	(lpHLSResult->Hue) -= 360;
}

#ifdef COLORSORTING
LOCAL BOOL FAR PASCAL ColorsRemain (lpPDevice)
  /* This function will search the color table for the current color and mark
     it as not current and used.  In addition, it returns a boolean value
     indicating whether or not any unused colors remain. */
LPPDEVICE lpPDevice;
{
    short Index;
    BOOL Result = FALSE;

    for (Index = 0; Index < lpPDevice->NumColors; Index++)
    {
	if (lpPDevice->ColorTable [Index].Current)
	{
	    lpPDevice->ColorTable [Index].Current = FALSE;
	    lpPDevice->ColorTable [Index].Used = TRUE;
	}
	if (!lpPDevice->ColorTable [Index].Used)
	    Result = TRUE;
    }
    return (Result);
}

LOCAL void FAR PASCAL sort_color_table (lpPDevice)
LPPDEVICE lpPDevice;
{
    BOOL ColorMoved = TRUE;

    while (ColorMoved)
    {
	short Index;

	for (Index=0,ColorMoved=FALSE; Index < lpPDevice->NumColors - 1; Index++)
	{
	    DWORD CurrPhysColor = lpPDevice->ColorTable [Index].PhysicalColor,
		  NextPhysColor = lpPDevice->ColorTable [Index+1].PhysicalColor,
		  CurrSortColor = lpPDevice->Setup.Carousel [HIWORD (CurrPhysColor)].
		    Pen [LOWORD (CurrPhysColor) - 1].Color | HIWORD (CurrPhysColor),
		  NextSortColor = lpPDevice->Setup.Carousel [HIWORD (NextPhysColor)].
		    Pen [LOWORD (NextPhysColor) - 1].Color | HIWORD (NextPhysColor);

	    if (CurrSortColor > NextSortColor)
	    {
		COLORENTRY Temp;

		Temp = lpPDevice->ColorTable [Index];
		lpPDevice->ColorTable [Index] = lpPDevice->ColorTable [Index+1];
		lpPDevice->ColorTable [Index + 1] = Temp;
		ColorMoved = TRUE;
	    }
	}
    }
}

LOCAL void FAR PASCAL AddColorEntry (lpPDevice,PhysicalColor)
  /* This function will add the given physical color to the color table (if
     necessary).  If a color is not currently realized, this function will
     call the device specific function next_color to determine if the driver
     is ready to perform operations on the given physical color.  */
LPPDEVICE lpPDevice;
PCOLOR PhysicalColor;
{
    if (lpPDevice->NumColors == 0)
    {
	lpPDevice->NumColors = 1;
	lpPDevice->ColorTable [0].PhysicalColor = PhysicalColor;
	lpPDevice->ColorTable [0].Used = FALSE;
	lpPDevice->ColorTable [0].Current = lpPDevice->ColorRealized =
	  next_color (lpPDevice,PhysicalColor);
    }
    else
    {
	short Index;
	BOOL NotFound = TRUE;

	for (Index = 0; Index < lpPDevice->NumColors && NotFound; Index++)
	{
	    if (lpPDevice->ColorTable [Index].PhysicalColor == PhysicalColor)
		NotFound = FALSE;
	}
	if (NotFound)
	{
	    Index = lpPDevice->NumColors++;
	    lpPDevice->ColorTable [Index].PhysicalColor = PhysicalColor;
	    lpPDevice->ColorTable [Index].Used = FALSE;
	    if (lpPDevice->ColorRealized)
		lpPDevice->ColorTable [Index].Current = FALSE;
	    else
		lpPDevice->ColorTable [Index].Current = lpPDevice->
		  ColorRealized = next_color (lpPDevice,PhysicalColor);
	}
    }
}

LOCAL BOOL FAR PASCAL CurrentColor (lpPDevice,PhysicalColor)
  /* This function determines if the given physical color is the current
     output color. If the color table is not yet fully constructed, the given
     physical color is passed to AddColorEntry function to insure the color
     is in the table prior to testing. */
LPPDEVICE lpPDevice;
PCOLOR PhysicalColor;
{
    short Index;
    BOOL NotFound = TRUE,
	 Current;

#ifdef EXCLUDEWHITE
    if (PhysicalColor == 0x00FFFFFF)    /* RGB White */
	return (FALSE);
#endif
    if (!lpPDevice->ColorTableBuilt)
	AddColorEntry (lpPDevice,PhysicalColor);
    for (Index = 0; Index < lpPDevice->NumColors && NotFound; Index++)
    {
	if (lpPDevice->ColorTable [Index].PhysicalColor == PhysicalColor)
	{
	    Current = lpPDevice->ColorTable [Index].Current;
	    NotFound = FALSE;
	}
    }
    if (Current && !lpPDevice->ColorRealized)
    {
	next_color (lpPDevice,PhysicalColor);
	lpPDevice->ColorRealized = TRUE;
    }
    return (Current);
}
#endif

LOCAL void FAR PASCAL CloseClipRect (lpPDevice)
LPPDEVICE lpPDevice;
{
    check_line_construction (lpPDevice);
    PutJob (lpPDevice,lpPDevice->hJob,"IW",2);
}

LOCAL BOOL FAR PASCAL DetermineClipping (lpPDevice,lpClipRect,lpPoints)
LPPDEVICE lpPDevice;
LPRECT lpClipRect;
LPPOINT lpPoints;
{
    BOOL Result = FALSE;

    if (lpClipRect)
    {
	if (lpPoints->x < lpClipRect->left || lpPoints->y < lpClipRect->top ||
	  (lpPoints + 1)->x > lpClipRect->right || (lpPoints + 1)->y >
	  lpClipRect->bottom)
	{
	    POINT Origin, OOrigin;

	    Result = TRUE;
	    check_line_construction (lpPDevice);
	PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) "IW",2);
	Origin.x = (lpClipRect->left < lpClipRect->right) ? lpClipRect->left
	  : lpClipRect->right;
	if (lpPDevice->Setup.Orientation == 0)        /* Portrait */
	    Origin.y = (lpClipRect->top < lpClipRect->bottom) ? lpClipRect->top
	      : lpClipRect->bottom;
	else
	    Origin.y = (lpClipRect->bottom > lpClipRect->top) ? lpClipRect->
	      bottom : lpClipRect->top;
	OOrigin.x = Origin.x;
	OOrigin.y = Origin.y;
	construct_point (lpPDevice,(LPPOINT) &Origin,FALSE);
	PutJob (lpPDevice,lpPDevice->hJob,(LPSTR) ",",1);
	Origin.x = (lpClipRect->right > lpClipRect->left) ? lpClipRect->
	  right : lpClipRect->left;
	if (lpPDevice->Setup.Orientation == 0)
	    Origin.y = (lpClipRect->bottom > lpClipRect->top) ? lpClipRect->
	      bottom : lpClipRect->top;
	else
	    Origin.y = (lpClipRect->top < lpClipRect->bottom) ? lpClipRect->top
	      : lpClipRect->bottom;
	if ((Origin.x == OOrigin.x) && (Origin.y == OOrigin.y)) {
	    ++Origin.x;
	    ++Origin.y;
	}
	construct_point (lpPDevice,(LPPOINT) &Origin,FALSE);
	    lpPDevice->LineInConstruction = FALSE;
	}
    }
    return (Result);
}
/* ************************** Exported Routines **************************** */

void FAR PASCAL get_bounds (lpPDevice,lpBounds,lpPoints,nPoints)
LPPDEVICE lpPDevice;
LPPOINT lpBounds,
	lpPoints;
short nPoints;
{
    short Index;

    lpBounds->x = (lpBounds+1)->x = lpPoints->x;
    lpBounds->y = (lpBounds+1)->y = (lpPoints++)->y;
    for (Index = 1; Index < nPoints; Index++,lpPoints++)
    {
	if (lpPoints->x < lpBounds->x)
	    lpBounds->x = lpPoints->x;
	else if (lpPoints->x > (lpBounds+1)->x)
	    (lpBounds+1)->x = lpPoints->x;
	if (lpPoints->y < lpBounds->y)
	    lpBounds->y = lpPoints->y;
	else if (lpPoints->y > (lpBounds+1)->y)
	    (lpBounds+1)->y = lpPoints->y;
    }
}

DWORD FAR PASCAL ColorInfo (lpPDevice,ColorIn,lpPColor)
LPPDEVICE lpPDevice;
DWORD ColorIn;
LPPCOLOR lpPColor;
{
#ifdef EXCLUDEWHITE
    if (ColorIn == 0x00FFFFFF)
    {
	if (lpPColor)
	    *lpPColor = ColorIn;
	return (ColorIn);
    }
    else
#endif

return (lpPColor ? get_nearest_rgb (lpPDevice,ColorIn,lpPColor) :
      physical_to_rgb (lpPDevice,ColorIn));

}

int FAR PASCAL EnumObj (lpPDevice,Style,lpCallbackFunc,lpData)
  /* This function is used to enumerate the pens and brushes available on the
     device.  For each object belonging to the given style, the callback
     function is called with the information for that object.  The callback
     function is called until there are no more objects or the callback function
     returns zero. */
LPPDEVICE lpPDevice;
int Style;
FARPROC lpCallbackFunc;
LPSTR lpData;
{
    return (Style == OBJ_PEN ? enum_pens (lpPDevice,lpCallbackFunc,lpData) :
      enum_brushes (lpPDevice,lpCallbackFunc,lpData));
}


int FAR PASCAL EnumDFonts (lpPDevice,lpFacename,lpCallbackFunc,lpData)
  /* This function is used to enumerate the fonts available on the device.  For
     each appropriate font, the callback function is called with the information
     for that font.  The callback function is called until there are no more
     fonts or the callback function returns zero.  */
LPPDEVICE lpPDevice;
LPSTR lpFacename;
FARPROC lpCallbackFunc;
LPSTR lpData;
{
	return (enum_fonts (lpPDevice,lpFacename,lpCallbackFunc,(LPSTR) lpData));
}

void FAR PASCAL MyBitBlt (lpDstDev,DstxOrg,DstyOrg,lpSrcDev,SrcxOrg,SrcyOrg,
  xext,yext,Rop,lpPBrush,lpDrawMode)
  /* This function transfers bits delimited by a source rectangle from the
     source bitmap to the area delimited by a destination rectangle on the
     destination bitmap.  The type of transfer is controlled by the raster
     operation that allows specification of all possible Boolean operations
     on three variables (source, destination, and the pattern in the brush).
     Note that the source and destination may overlap, so the implementation
     must be careful about the direction in which bits are copied. */
LPPDEVICE lpDstDev;
int DstxOrg,
    DstyOrg;
LPPDEVICE lpSrcDev;
int SrcxOrg,
    SrcyOrg,
    xext,
    yext;
DWORD Rop;
LPSTR lpPBrush;
LPDRAWMODE lpDrawMode;
{

     draw_bitmap (lpDstDev,DstxOrg,DstyOrg,lpSrcDev,SrcxOrg,SrcyOrg,xext,yext,
      Rop,(LPPBRUSH) lpPBrush,lpDrawMode);
}

// not fully implemented

WORD FAR PASCAL DevGetCharWidth(lpPDevice, lpBuffer, wFirstChar, wLastChar, lpFont, 
				lpDrawMode, lpTextXForm)
LPPDEVICE   lpPDevice;
LPWORD      lpBuffer;
WORD        wFirstChar;
WORD        wLastChar;
LPFONTINFO  lpFont;
LPDRAWMODE  lpDrawMode;
LPTEXTXFORM lpTextXForm;
{
        int i;
        short Width;

        if (!lpBuffer || !lpFont)
                return 0;

        Width = lpFont->dfPixWidth;

        for (i=(int)wFirstChar; (i<=(int)wLastChar) && (*lpBuffer); i++)
                *lpBuffer++=Width;

        return 1;
}


DWORD FAR PASCAL DevExtTextOut(lpPDevice,DestxOrg,DestyOrg,lpClipRect,lpString,
  Count,lpFont,lpDrawMode,lpTextXForm, lpCharWidths, lpOpaque, wOptions)
  /* This function transfers the pattern for each character in the string from
     the font bitmap to the destination device, starting at the origin passed.
     In each character pattern, a one bit specifies character foreground, and
     a zero bit specifies character background. */
LPPDEVICE lpPDevice;
int DestxOrg,
    DestyOrg;
LPRECT lpClipRect;
LPSTR lpString;
int Count;
LPFONTINFO lpFont;
LPDRAWMODE lpDrawMode;
LPTEXTXFORM lpTextXForm;
LPSHORT lpCharWidths;
LPRECT  lpOpaque;
WORD wOptions;
{
    DWORD Result = 0;

    if (Count > 0)
    {
#ifdef COLORSORTING
       if (CurrentColor (lpPDevice,lpDrawMode->TextColor))
	{
#endif
	draw_text (lpPDevice,DestxOrg,DestyOrg,lpClipRect,lpString,Count,lpFont,
	  lpDrawMode,lpTextXForm);
	lpPDevice->PenPos.x = -1;
#ifdef COLORSORTING
	}
#endif
    }
//    else
	Result = get_text_extent (lpPDevice,DestxOrg,DestyOrg,lpClipRect,
	  lpString,-Count,lpFont,lpDrawMode,lpTextXForm);
    return (Result);
}

short FAR PASCAL Output (lpPDevice,Style,Count,lpPoints,lpPPen,lpPBrush,
  lpDrawMode,lpWrongClipRect)
  /* This function is used to draw all line-drawing primitives.  The driver-
     specific functions and the styles they support are as follows:
	draw_arc        ARC
	draw_chord      CHORD
	draw_ellipse    CIRCLE, ELLIPSE
	draw_pie        PIE
	draw_polygon    ALTERNATE_FILL_POLYGON, WINDING_NUMBER_FILL_POLYGON
	draw_polyline   POLYLINE, SCANLINES
	draw_polymarker POLYMARKER, MARKER
	draw_rectangle  RECTANGLE
     If color sorting is implemented, the driver-specific funtions will not be
     called if the primitive is not the correct color.  Scan lines are converted
     into to two point lines then passed to the draw_polyline function.  If
     line optimization is implemented, all lines are inverted if necessary to
     reduce pen movement and scan lines are connected if their ends are
     adjacent. */
LPPDEVICE lpPDevice;
int Style,
    Count;
LPPOINT lpPoints;
LPSTR lpPPen,
      lpPBrush;
LPDRAWMODE lpDrawMode;
LPRECT lpWrongClipRect;
{
    BOOL Clipping;
    RECT NewClipRect;
    LPPOINT lpWorkPoints;
    POINT WorkPoints [5],
	  Bounds [2];
    LPRECT lpClipRect;
    int SaveStyle = Style,
	SaveCount = Count;
    short SavePenStyle,
	  SaveBrushStyle,
	  Index;
#ifdef NICK_DEBUG
    char buf[64];
#endif

    switch (Style) {
	case OS_ARC                         :
	case OS_SCANLINES                   :
	case OS_RECTANGLE                  :
	case OS_ELLIPSE                   :
	case OS_MARKER                      :
	case OS_POLYLINE                    :
	case OS_WINDING_NUMBER_FILL_POLYGON:
	case OS_ALTERNATE_FILL_POLYGON      :
	case OS_PIE                         :
	case OS_POLYMARKER                  :
	case OS_CHORD                       :
	case OS_CIRCLE                      :
	break;
	default:
#ifdef NICK_DEBUG
	    wsprintf(buf, "Error -- style = %d \r\n", Style);
	    OutputDebugString(buf);
#endif
	    return(0);
	break;
    }
#ifdef NICK_DEBUG
	OutputDebugString("In Output() -- ");
    switch (Style) {
	case OS_ARC                         :
	OutputDebugString("Style:ARC\r\n");
	break;
	case OS_SCANLINES                   :
	OutputDebugString("Style:SCANLINES\r\n");
	break;
	case OS_RECTANGLE                  :
	OutputDebugString("Style:RECTANGLE\r\n");
	break;
	case OS_ELLIPSE                   :
	OutputDebugString("Style:ELLIPSE\r\n");
	break;
	case OS_MARKER                      :
	OutputDebugString("Style:MARKER\r\n");
	break;
	case OS_POLYLINE                    :
	OutputDebugString("Style:POLYLINE\r\n");
	break;
	case OS_WINDING_NUMBER_FILL_POLYGON:
	OutputDebugString("Style:FILL_POLY_WIND\r\n");
	break;
	case OS_ALTERNATE_FILL_POLYGON      :
	OutputDebugString("Style:FILL_POLY_ALT\r\n");
	break;
	case OS_PIE                         :
	OutputDebugString("Style:PIE\r\n");
	break;
	case OS_POLYMARKER                  :
	OutputDebugString("Style:POLYMARKER\r\n");
	break;
	case OS_CHORD                       :
	OutputDebugString("Style:CHORD\r\n");
	break;
	case OS_CIRCLE                      :
	OutputDebugString("Style:CIRCLE\r\n");
	break;
	default:
	OutputDebugString("Style:FUNKY\r\n");
	buf[0] = Count / 100;
	buf[1] = (Count - buf[0]*100) / 10;
	buf[2] = (Count - buf[0]*100 - buf[1]*10);
	buf[0] += '0';
	buf[1] += '0';
	buf[2] += '0';
	buf[3]  = 13;
	buf[4]  = 10;
	buf[5] = 0;
	OutputDebugString(buf);
	break;
    }
#       endif

    if (lpWrongClipRect)
    {
#       ifdef NICK_DEBUG
	OutputDebugString("In lpWrongClipRect() -- ");
#       endif
	NewClipRect.top = lpWrongClipRect->top;
	NewClipRect.left = lpWrongClipRect->left;
	NewClipRect.right = lpWrongClipRect->right - 1;
	NewClipRect.bottom = lpWrongClipRect->bottom - 1;
	if (NewClipRect.left == NewClipRect.right)
	    NewClipRect.right++;
	if (NewClipRect.top == NewClipRect.bottom)
	    NewClipRect.bottom++;
	lpClipRect = (LPRECT) &NewClipRect;
    }
    else
	lpClipRect = (LPRECT) NULL;
#       ifdef NICK_DEBUG
	OutputDebugString("Out of lpWrongClipRect() -- \r\n");
        if (!lpPoints)
	    OutputDebugString("lpPoints is NULL \r\n");
	if (!Count)
	    OutputDebugString("Count is NULL \r\n");
#       endif
    for (Index = 0; Index < 5; Index++) {
#       ifdef NICK_DEBUG
	OutputDebugString("*");
#       endif
	WorkPoints [Index] = *(lpPoints + Index);
     }
#       ifdef NICK_DEBUG
	OutputDebugString("\r\nOut of WorkPoints() -- \r\n");
#       endif
    lpWorkPoints = (LPPOINT) WorkPoints;
    if (Style == OS_POLYLINE)
    {
#       ifdef NICK_DEBUG
//        OutputDebugString("In Output() -- Polyline");
#       endif
#ifdef COLORSORTING
	if (CurrentColor (lpPDevice,((LPPPEN) lpPPen)->PhysicalColor))
	{
#endif
#ifdef LINEOPTIMIZE
	short LastPoint = Count - 1;
	int rstr_swap;
	
	rstr_swap = 0;

	if (GetDistance (lpPDevice->PenPos.x,lpPDevice->PenPos.y,lpPoints->x,
	  lpPoints->y) > GetDistance (lpPDevice->PenPos.x,lpPDevice->PenPos.y,
	  (lpPoints + LastPoint)->x,(lpPoints + LastPoint)->y))
	{
	    short Index;
	    POINT Temp;

	    rstr_swap = -1;
	    for (Index = 0; Index < Count/2; Index++)
	    {
		Temp = *(lpPoints + Index);
		*(lpPoints + Index) = *(lpPoints + LastPoint - Index);
		*(lpPoints + LastPoint - Index) = Temp;
	    }
	}
#endif
	get_bounds (lpPDevice,(LPPOINT) Bounds,lpPoints,Count);
	Clipping = DetermineClipping (lpPDevice,lpClipRect,(LPPOINT) Bounds);
	draw_polyline (lpPDevice,Count,lpPoints,(LPPPEN) lpPPen,(LPPBRUSH) NULL,
	  lpDrawMode,lpClipRect);
	if (Clipping) {
	    CloseClipRect (lpPDevice);

	    /* added this due to 7475a clipping problems --ssn 7/31/91 */
	    if (lpPDevice->Setup.Plotter + COLORPRO == HP7475A) {
		lpPDevice->PenPos.x = -1;       /* invalidate pen position */
		lpPDevice->PenPos.y = -1;
		lpPDevice->LineInConstruction = FALSE;
	    }
	}
	

#ifdef LINEOPTIMIZE
	/* added the 'if' check due to 7475 clipping problems */
	/* --ssn 7/31/91 */
	if (lpPDevice->Setup.Plotter + COLORPRO != HP7475A && !Clipping)
	    lpPDevice->PenPos = *(lpPoints + LastPoint);

	if (rstr_swap) {
	    short Index;
	    POINT Temp;
	    
	    for (Index = 0; Index < Count/2; Index++)
	    {
		Temp = *(lpPoints + Index);
		*(lpPoints + Index) = *(lpPoints + LastPoint - Index);
		*(lpPoints + LastPoint - Index) = Temp;
	    }
	}
#endif
#ifdef COLORSORTING
	}
#endif
    }
    else if (Style == OS_SCANLINES)
    {
#       ifdef SCAN_DEBUG
{
        char mybuf[64];
        LPPOINT lpPHold;
        int counthold = Count;

        OutputDebugString("In Output() -- SCANLINES \n\r");
        lpPHold = lpPoints;
        while (counthold)
        {
                wsprintf(mybuf, "Point is %d %d \n\r", lpPHold->x, lpPHold->y);
                OutputDebugString(mybuf);
                lpPHold++;
                counthold--;
        }
}

#       endif
#ifdef COLORSORTING
	if (lpPBrush ? CurrentColor (lpPDevice,((LPPBRUSH) lpPBrush)->
	  PhysicalColor) : CurrentColor (lpPDevice,((LPPPEN) lpPPen)->
	  PhysicalColor))
	{
#endif
	PPEN PhysicalPen;
	POINT WorkPoint [2];
	short Index;
#ifdef LINEOPTIMIZE
	short PenThickness = lpPDevice->CurrentPenThickness + 1;
	POINT PenPos;

	if (!lpPPen)
	{
	    PhysicalPen.PhysicalColor = ((LPPBRUSH) lpPBrush)->PhysicalColor;
	    PhysicalPen.Style = 0;
	    PhysicalPen.Width.x = PhysicalPen.Width.y = 0;
	    ((LPPPEN) lpPPen) = (LPPPEN) &PhysicalPen;
	}
	if (((LPPPEN) lpPPen)->Width.x != 0)
	{
	    PhysicalPen.PhysicalColor = ((LPPPEN) lpPPen)->PhysicalColor;
	    PhysicalPen.Style = ((LPPPEN) lpPPen)->Style;
	    PhysicalPen.Width.x = PhysicalPen.Width.y = 0;
	    ((LPPPEN) lpPPen) = (LPPPEN) &PhysicalPen;
	}
	((LPPPEN) lpPPen)->Style = PS_SOLID;
	WorkPoint [0].y = WorkPoint [1].y = (lpPoints++)->y;
	for (Index = 1; Index < Count; Index++,lpPoints++)
	{
	    PenPos = lpPDevice->PenPos;
	    WorkPoint [0].x = lpPoints->x;
	    WorkPoint [1].x = lpPoints->y;
	    if ( GetDistance (PenPos.x,PenPos.y,WorkPoint [0].x,WorkPoint [0].y)
	      > GetDistance (PenPos.x,PenPos.y,WorkPoint [1].x,WorkPoint [1].y))
	    {
		POINT Temp;

		Temp = WorkPoint [1];
		WorkPoint [1] = WorkPoint [0];
		WorkPoint [0] = Temp;
	    }
	    if (ABS (PenPos.y - WorkPoint [0].y) < PenThickness &&
	      ABS (PenPos.x - WorkPoint [0].x) < PenThickness)
	    {
		POINT ConnectPoint [2];

		ConnectPoint [0] = PenPos;
		ConnectPoint [1] = WorkPoint [0];
		draw_polyline (lpPDevice,2,(LPPOINT) ConnectPoint,(LPPPEN)
		  lpPPen,(LPPBRUSH) lpPBrush,lpDrawMode,lpClipRect);
		lpPDevice->PenPos = ConnectPoint [1];
	    }
	    draw_polyline (lpPDevice,2,(LPPOINT) WorkPoint,(LPPPEN) lpPPen,
	      (LPPBRUSH) lpPBrush,lpDrawMode,lpClipRect);
	    lpPDevice->PenPos = WorkPoint [1];
	}
#else
	WorkPoint [0].y = WorkPoint [1].y = (lpPoints++)->y;
	for (Index = 1; Index < Count; Index++,lpPoints++)
	{
	    WorkPoint [0].x = lpPoints->x;
	    WorkPoint [1].x = lpPoints->y;
	    draw_polyline (lpPDevice,2,(LPPOINT) WorkPoint,(LPPPEN) lpPPen,
	      (LPPBRUSH) lpPBrush,lpDrawMode,lpClipRect);
	}
#endif
#ifdef COLORSORTING
	}
#endif
    }
    else if (Style == OS_ARC)
    {
#       ifdef NICK_DEBUG
	OutputDebugString("In Output() -- ARC");
#       endif
#ifdef COLORSORTING
	if (CurrentColor (lpPDevice,((LPPPEN) lpPPen)->PhysicalColor))
	{
#endif
/*
	(lpWorkPoints + 1)->x -= 1;
	(lpWorkPoints + 1)->y -= 1;
*/
	Clipping = DetermineClipping (lpPDevice,lpClipRect,lpWorkPoints);
	draw_arc (lpPDevice,lpWorkPoints,(LPPPEN) lpPPen,lpDrawMode,lpClipRect);
	if (Clipping)
	    CloseClipRect (lpPDevice);
#ifdef COLORSORTING
	}
#endif
    }
    else if (Style == OS_PIE)
    {
#       ifdef NICK_DEBUG
	OutputDebugString("In Output() -- PIE");
#       endif
#ifdef COLORSORTING
	if (CurrentColor (lpPDevice,((LPPPEN) lpPPen)->PhysicalColor) ||
	  CurrentColor (lpPDevice,((LPPBRUSH) lpPBrush)->PhysicalColor))
	{
	    SavePenStyle = ((LPPPEN) lpPPen)->Style;
	    SaveBrushStyle = ((LPPBRUSH) lpPBrush)->Style;
	    if (!CurrentColor (lpPDevice,((LPPPEN) lpPPen)->PhysicalColor))
		((LPPPEN) lpPPen)->Style = PS_NULL;
	    else if (!CurrentColor (lpPDevice,((LPPBRUSH) lpPBrush)->
	      PhysicalColor))
		((LPPBRUSH) lpPBrush)->Style = BS_HOLLOW;
#endif
	(lpWorkPoints + 1)->x -= 1;
	(lpWorkPoints + 1)->y -= 1;
	Clipping = DetermineClipping (lpPDevice,lpClipRect,lpWorkPoints);
	draw_pie (lpPDevice,lpWorkPoints,(LPPPEN) lpPPen,(LPPBRUSH) lpPBrush,
	  lpDrawMode,lpClipRect);
	if (Clipping)
	    CloseClipRect (lpPDevice);
#ifdef COLORSORTING
	    ((LPPPEN) lpPPen)->Style = SavePenStyle;
	    ((LPPBRUSH) lpPBrush)->Style = SaveBrushStyle;
	}
#endif
    }
    else if (Style == OS_CHORD)
    {
#       ifdef NICK_DEBUG
	OutputDebugString("In Output() -- CHORD");
#       endif
#ifdef COLORSORTING
	if (CurrentColor (lpPDevice,((LPPPEN) lpPPen)->PhysicalColor) ||
	  CurrentColor (lpPDevice,((LPPBRUSH) lpPBrush)->PhysicalColor))
	{
	    if (!CurrentColor (lpPDevice,((LPPPEN) lpPPen)->PhysicalColor))
		lpPPen = NULL;
	    else if (!CurrentColor (lpPDevice,((LPPBRUSH) lpPBrush)->
	      PhysicalColor))
		lpPBrush = NULL;
#endif
	(lpWorkPoints + 1)->x -= 1;
	(lpWorkPoints + 1)->y -= 1;
	Clipping = DetermineClipping (lpPDevice,lpClipRect,lpWorkPoints);
	draw_chord (lpPDevice,lpWorkPoints,(LPPPEN) lpPPen,(LPPBRUSH) lpPBrush,
	  lpDrawMode,lpClipRect);
	if (Clipping)
	    CloseClipRect (lpPDevice);
#ifdef COLORSORTING
	}
#endif
    }
    else if (Style == OS_CIRCLE || Style == OS_ELLIPSE)
    {
#       ifdef NICK_DEBUG
	OutputDebugString("In Output() -- CIRCLE & ELLIPSE");
#       endif
#ifdef COLORSORTING
	if (CurrentColor (lpPDevice,((LPPBRUSH) lpPBrush)->PhysicalColor) ||
	  CurrentColor (lpPDevice,((LPPPEN) lpPPen)->PhysicalColor))
	{
	    SavePenStyle = ((LPPPEN) lpPPen)->Style;
	    SaveBrushStyle = ((LPPBRUSH) lpPBrush)->Style;
	    if (!CurrentColor (lpPDevice,((LPPPEN) lpPPen)->PhysicalColor))
		((LPPPEN) lpPPen)->Style = PS_NULL;
	    else if (!CurrentColor (lpPDevice,((LPPBRUSH) lpPBrush)->
	      PhysicalColor))
		((LPPBRUSH) lpPBrush)->Style = BS_HOLLOW;
#endif
	(lpWorkPoints + 1)->x -= 1;
	(lpWorkPoints + 1)->y -= 1;
	Clipping = DetermineClipping (lpPDevice,lpClipRect,lpWorkPoints);
	draw_ellipse (lpPDevice,lpWorkPoints,(LPPPEN) lpPPen,(LPPBRUSH) lpPBrush,
	  lpDrawMode,lpClipRect);
	if (Clipping)
	    CloseClipRect (lpPDevice);
#ifdef COLORSORTING
	    ((LPPPEN) lpPPen)->Style = SavePenStyle;
	    ((LPPBRUSH) lpPBrush)->Style = SaveBrushStyle;
	}
#endif
    }
    else if (Style == OS_ALTERNATE_FILL_POLYGON || Style ==
      OS_WINDING_NUMBER_FILL_POLYGON)
    {
#       ifdef NICK_DEBUG
	OutputDebugString("In Output() -- FILL_POLYGON");
#       endif
#ifdef COLORSORTING
	if (CurrentColor (lpPDevice,((LPPPEN) lpPPen)->PhysicalColor) ||
	  CurrentColor (lpPDevice,((LPPBRUSH) lpPBrush)->PhysicalColor))
	{
	    SavePenStyle = ((LPPPEN) lpPPen)->Style;
	    SaveBrushStyle = ((LPPBRUSH) lpPBrush)->Style;
	    if (!CurrentColor (lpPDevice,((LPPPEN) lpPPen)->PhysicalColor))
		((LPPPEN) lpPPen)->Style = PS_NULL;
	    else if (!CurrentColor (lpPDevice,((LPPBRUSH) lpPBrush)->
	      PhysicalColor))
		((LPPBRUSH) lpPBrush)->Style = BS_HOLLOW;
#endif
	get_bounds (lpPDevice,(LPPOINT) Bounds,lpPoints,Count);
	Clipping = DetermineClipping (lpPDevice,lpClipRect,(LPPOINT) Bounds);
	draw_polygon (lpPDevice,Style,Count,lpPoints,(LPPPEN) lpPPen,(LPPBRUSH)
	  lpPBrush,lpDrawMode,lpClipRect);
	if (Clipping)
	    CloseClipRect (lpPDevice);
#ifdef COLORSORTING
	    ((LPPPEN) lpPPen)->Style = SavePenStyle;
	    ((LPPBRUSH) lpPBrush)->Style = SaveBrushStyle;
	}
#endif
    }
    else if (Style == OS_RECTANGLE)
    {
#       ifdef NICK_DEBUG
	OutputDebugString("In Output() -- RECTANGLE\r\n");
#       endif
#ifdef COLORSORTING
	if (CurrentColor (lpPDevice,((LPPBRUSH) lpPBrush)->PhysicalColor) ||
	  CurrentColor (lpPDevice,((LPPPEN) lpPPen)->PhysicalColor))
	{
	    SavePenStyle = ((LPPPEN) lpPPen)->Style;
	    SaveBrushStyle = ((LPPBRUSH) lpPBrush)->Style;
	    if (!CurrentColor (lpPDevice,((LPPPEN) lpPPen)->PhysicalColor))
		((LPPPEN) lpPPen)->Style = PS_NULL;
	    else if (!CurrentColor (lpPDevice,((LPPBRUSH) lpPBrush)->
	      PhysicalColor))
		((LPPBRUSH) lpPBrush)->Style = BS_HOLLOW;
#endif
	(lpWorkPoints + 1)->x -= 1;
	(lpWorkPoints + 1)->y -= 1;
	Clipping = DetermineClipping (lpPDevice,lpClipRect,lpWorkPoints);
	draw_rectangle (lpPDevice,lpWorkPoints,(LPPPEN) lpPPen,(LPPBRUSH) lpPBrush,
	  lpDrawMode,lpClipRect);
	if (Clipping)
	    CloseClipRect (lpPDevice);
#ifdef COLORSORTING
	    ((LPPPEN) lpPPen)->Style = SavePenStyle;
	    ((LPPBRUSH) lpPBrush)->Style = SaveBrushStyle;
	}
#endif
    }
    else if (Style == OS_POLYMARKER || Style == OS_MARKER)
	draw_polymarker (lpPDevice,Count,lpPoints,(LPPPEN) lpPPen,lpDrawMode,
	  lpClipRect);
#       ifdef NICK_DEBUG
	OutputDebugString("OUT OF Output()\r\n"); 
#       endif
    return (1);
}

DWORD FAR PASCAL Pixel (lpPDevice,X,Y,PhysColor,lpDrawMode)
  /* This function sets or retrieves the color of the specified pixel.  If
     lpDrawMode is not NULL, this routine sets the given pixel to the color
     given by PhysColor, using the binary raster operation given by lpDrawMode.
     If lpDrawMode is NULL, the function returns the physical color of the
     pixel given by X and Y. */
LPPDEVICE lpPDevice;
int X,
    Y;
DWORD PhysColor;
LPDRAWMODE lpDrawMode;
{
    return (lpDrawMode ? set_pixel (lpPDevice,X,Y,PhysColor,lpDrawMode),0 :
      get_pixel (lpPDevice,X,Y));
}

int FAR PASCAL ScanLR (lpPDevice,X,Y,PhysColor,Style)
  /* This function scans the device surface in a left to right direction from
     the given pixel looking for the first pixel having (or not having) the
     given color. */
LPPDEVICE lpPDevice;
int X,
    Y;
DWORD PhysColor;
int Style;
{
    return (scan_pixels (lpPDevice,X,Y,PhysColor,Style));
}

int FAR PASCAL Enable (lpStruct,Style,lpDeviceType,lpOutputFile,lpData)
  /* This function initializes a support module or returns information about
     the module as defined by the value of the Style.  If Style is 0, this
     function initializes the support module and its attached graphic
     peripheral for use by GDI routines.  If Style is 1, it fills a GDIINFO
     data structure with information about the support module and peripheral.
     */
LPSTR lpStruct;
int Style;
LPSTR lpDeviceType,
      lpOutputFile,
      lpData;
{
    int Result;

#ifdef NICK_DEBUG
OutputDebugString("In Enable \n\r");
#endif

    // fill in PDEVICE
    if (Style == 0x0000 || Style == 0x8000)     /* initialize device */
    {
	Result = initialize_device ((LPPDEVICE) lpStruct,lpDeviceType,
	  lpOutputFile,(LPENVIRONMENT) lpData);
	((LPPDEVICE) lpStruct)->Spooling = GetSpoolState ();
#ifdef COLORSORTING
	((LPPDEVICE) lpStruct)->NumColors = 0;
#endif
    }
    else   // get GDIINFO
	Result = initialize_gdi ((LPGDIINFO) lpStruct,lpDeviceType,lpOutputFile,
	  (LPENVIRONMENT) lpData);
    return (Result);
}

void FAR PASCAL Disable (lpPDevice)
  /* Restores the supported module and its attached graphic peripheral to the
     state prior to the enable call with the passed PDEVICE data structure
     pointer. */
LPPDEVICE lpPDevice;
{
	disable_device (lpPDevice);
}

int FAR PASCAL RealizeObject (lpPDevice,Style,lpInObj,lpOutObj,lpTextXForm)
  /* This function directs the driver to create an attribute structure that
     will be used when drawing output primitives or return the size of such
     a structure.

     If lpOutObj is a nonzero value, it is assumed to be a long pointer to a
     data structure to be filled with the physical attributes of an object.
     The Style specifies the type of object to be realized and lpInObj is a
     long pointer to a structure defining the logical attributes of the object.
     This function must translate the logical attributes into sufficient
     information to accurately describe a physical object for use by the Output
     function when drawing.

     If lpOutObj is NULL, this function is expected to return the size (in
     bytes) of the data structure needed to realize the physical object.  After
     recieving the object size, GDI allocates space for the realized object and
     calls RealizeObject again requesting the information. */
LPPDEVICE lpPDevice;
int Style;
LPSTR lpInObj,
      lpOutObj;
LPTEXTXFORM lpTextXForm;
{
    short Result;
#ifdef NICK_DEBUG
OutputDebugString("In RealizeObject \n\r");
#endif
    switch (Style)
    {
	case OBJ_PEN:
	    Result = (lpOutObj ? realize_pen (lpPDevice,(LPLOGPEN) lpInObj,
	      (LPPPEN) lpOutObj) : get_physical_pen_size (lpPDevice,(LPLOGPEN)
	      lpInObj));
	    break;

	case OBJ_BRUSH:
	    Result = (lpOutObj ? realize_brush (lpPDevice,(LPLOGBRUSH) lpInObj,
	      (LPPBRUSH) lpOutObj) : get_physical_brush_size (lpPDevice,
	      (LPLOGBRUSH) lpInObj));
            break;

	case OBJ_FONT:
	    Result = (lpOutObj ? realize_font (lpPDevice,(LPLOGFONT) lpInObj,
	      (LPFONTINFO) lpOutObj,lpTextXForm) : get_physical_font_size
	      (lpPDevice,(LPLOGFONT) lpInObj,lpTextXForm));
	    break;
    }
    return (Result);
}

int FAR PASCAL Control (lpPDevice,function,lpInData,lpOutData)
  /* This function handles the printing control functions.  The driver-specific
     functions and the given control function they handle are as follows:
	abort_doc               ABORTDOC
	device_control          All device-defined control functions
	draft_mode              DRAFTMODE
	end_doc                 ENDDOC
	flush_ouput             FLUSHOUTPUT
	get_color_table         GETCOLORTABLE
	get_phys_page_size      GETPHYSPAGESIZE
	get_printing_offset     GETPRINTINGOFFSET
	new_frame               NEWFRAME
	next_band, next_color   NEXTBAND
	set_abort_proc          SETABORTPROC
	set_color_table         SETCOLORTABLE
	start_doc               STARTDOC
	query_esc_support       QUERYESCSUPPORT
     If color sorting is implemented, at each next_band request the driver
     shell will determine if additional colors exist that have not yet been
     output.  If a color is found that the driver-specific function next_color
     realizes, the driver shell will request the current band again and output
     all primitives for the newly realized color.  Otherwise, the driver shell
     will call the driver-specific function next_band. */
LPPDEVICE lpPDevice;
int function;
LPSTR lpInData,
      lpOutData;
{
    short Result;
#       ifdef NICK_DEBUG
char buf[64];
	OutputDebugString("In control -- ");
#       endif
    switch (function)
    {
	case NEWFRAME:
#       ifdef NICK_DEBUG
	OutputDebugString("In new_frame\r\n");
#       endif
	    Result = new_frame (lpPDevice);
	    break;

	case ABORTDOC:
#       ifdef NICK_DEBUG
	OutputDebugString("In abort_doc\r\n");
#       endif
	    Result = abort_doc (lpPDevice);
	    break;

	case NEXTBAND:
	{
#       ifdef NICK_DEBUG
	OutputDebugString("In next_band\r\n");
#       endif
#ifdef COLORSORTING
	    if (!ColorsRemain (lpPDevice))
	    {
		lpPDevice->ColorTableBuilt = FALSE;
		lpPDevice->NumColors = 0;
#endif
	    Result = next_band (lpPDevice,(LPRECT) lpOutData);
#ifdef COLORSORTING
		CopyRect ((LPRECT) &lpPDevice->CurrentBand,(LPRECT) lpOutData);
	    }
	    else
	    {
		short Index;
		BOOL Realized = FALSE;

		if (!lpPDevice->ColorTableBuilt)
		{
		    sort_color_table (lpPDevice);
		    lpPDevice->ColorTableBuilt = TRUE;
		}

		while (!Realized)
		{
		    for (Index = 0; Index < lpPDevice->NumColors && !Realized;
		      Index++)
		    {
			if (!lpPDevice->ColorTable [Index].Used)
			    lpPDevice->ColorTable [Index].Current = Realized =
			      next_color (lpPDevice,lpPDevice->ColorTable
			      [Index].PhysicalColor);
		    }
		}

		CopyRect ((LPRECT) lpOutData,(LPRECT) &lpPDevice->CurrentBand);
		Result = sizeof (RECT);
	    }
#endif
	    break;
	}

	case SETCOLORTABLE:
#       ifdef NICK_DEBUG
	OutputDebugString("In set_color_table\r\n");
#       endif
	    Result = set_color_table (lpPDevice,(LPCOLORTABLEENTRY) lpInData);
	    break;

	case GETCOLORTABLE:
#       ifdef NICK_DEBUG
	OutputDebugString("In get_color_table\r\n");
#       endif
	    Result = get_color_table (lpPDevice,(LPINT) lpInData,(DWORD FAR *)
	      lpOutData);
	    break;

	case FLUSHOUTPUT:
#       ifdef NICK_DEBUG
	OutputDebugString("In flush_output\r\n");
#       endif
	    Result = flush_output (lpPDevice);
	    break;

	case DRAFTMODE:
#       ifdef NICK_DEBUG
	OutputDebugString("In draft_mode\r\n");
#       endif
	    Result = draft_mode (lpPDevice,(LPINT) lpInData);
	    break;

	case QUERYESCSUPPORT:
#       ifdef NICK_DEBUG
        {
        char buf[64];
        wsprintf(buf, "In query_esc_support %d \r\n", (int) *lpInData);
        OutputDebugString(buf);
        }
#       endif
	    Result = query_esc_support (lpPDevice,(LPINT) lpInData);
	    break;

	case SETABORTPROC:
#       ifdef NICK_DEBUG
	OutputDebugString("In set_abort_proc\r\n");
#       endif
	    Result = set_abort_proc (lpPDevice,(HANDLE FAR *) lpInData);
	    break;

	case STARTDOC:
#       ifdef NICK_DEBUG
	OutputDebugString("In start_doc\r\n");
#       endif
	    Result = start_doc (lpPDevice, (LPSTR)lpInData, (LPSTR)lpOutData);
#       ifdef NICK_DEBUG
        wsprintf(buf, "start_doc result is: %d ", Result);
        OutputDebugString(buf);
	OutputAReturn();
#       endif
	    break;

	case ENDDOC:
#       ifdef NICK_DEBUG
	OutputDebugString("in end_doc\r\n");
#       endif
	    Result = end_doc (lpPDevice);
	    break;

	case GETPHYSPAGESIZE:
#       ifdef NICK_DEBUG
	OutputDebugString("in get_phys_page_size\r\n");
#       endif
	    Result = get_phys_page_size (lpPDevice,(LPPOINT) lpOutData);
	    break;

	case GETPRINTINGOFFSET:
#       ifdef NICK_DEBUG
	OutputDebugString("in get_printing_offset\r\n");
#       endif
	    Result = get_printing_offset (lpPDevice,(LPPOINT) lpOutData);
	    break;

	case GETVECTORPENSIZE:
	case GETVECTORBRUSHSIZE:
#       ifdef NICK_DEBUG
	OutputDebugString("in get_vector_pen_size\r\n");
#       endif
	    Result = get_vector_pen_size (lpPDevice,(LPPOINT) lpOutData);
	    break;

	default:
#       ifdef NICK_DEBUG
	OutputDebugString("in device_control\r\n");
#       endif
	    Result = device_control (lpPDevice,function,lpInData,lpOutData);
    }
    return (Result);
}

short FAR PASCAL StartJob (lpPDevice,lpPort,lpDocName,hDC)
  /* This function returns a handle to the print file that the driver will
     use on all subsequent output requests. */
LPPDEVICE lpPDevice;
LPSTR lpPort,
      lpDocName;
HANDLE hDC;
{
    short Result;

#ifdef NICK_DEBUG
    char mybuf[64];
#endif

/*  OFSTRUCT FileStatus;

    if (lpPDevice->Spooling) */
    
    WriteError = 0;

#ifdef NICK_DEBUG
    OutputDebugString("In OpenJob\r\n");
#endif
    
    Result = OpenJob (lpPort,lpDocName,hDC);

#ifdef NICK_DEBUG

    wsprintf(mybuf, "OpenJob returned %lu \r\n", Result);
    OutputDebugString(mybuf);

#endif
   
    if (!Result)
	WriteError = -1;                /* Global WriteError flag       */
    return (Result);
}

BOOL FAR PASCAL StartPlotPage (lpPDevice,hJob)
  /* This function call is required when using the windows spooler to start
     the current spool page.  If output is going directly to a file, no
     operation is performed. */
LPPDEVICE lpPDevice;
HANDLE hJob;
{
/*  BOOL Result = (lpPDevice->Spooling) ? StartSpoolPage (hJob) : TRUE; */
    BOOL Result = StartSpoolPage (hJob);
    lpPDevice->SpoolPageStarted = TRUE;
#ifdef NICK_DEBUG
    OutputDebugString("In StartPlotPage: ");
    OutputAReturn();
#endif

    return (Result);
}

BOOL FAR PASCAL EndPlotPage (lpPDevice,hJob)
  /* This function call is required to complete the spooling process for the
     current spool page.  If output is going directly to a file, no operation
     is performed. */
LPPDEVICE lpPDevice;
HANDLE hJob;
{
/*  BOOL Result = (lpPDevice->Spooling) ? EndSpoolPage (hJob) : TRUE; */
    BOOL Result = EndSpoolPage (hJob);
    lpPDevice->SpoolPageStarted = FALSE;
#ifdef NICK_DEBUG
    OutputDebugString("In EndPlotPage\r\n");
#endif

    return (Result);
}

short FAR PASCAL EndJob (lpPDevice,hJob)
  /* This function is called when all output is completed (usually at the
     end_doc function).  If spooling, the spoolers CloseJob function is called,
     otherwise the file to which the output is being directed is closed. */
LPPDEVICE lpPDevice;
HANDLE hJob;
{
/*  short Result = (lpPDevice->Spooling) ? (CloseJob (hJob),0) : close (hJob);*/
    short Result = 0;

    WriteError = 0;
    CloseJob (hJob);
    return (Result);
}

short FAR PASCAL PutJob (lpPDevice,hJob,lpData,length)
  /* This function is called to output data to the device.  If spooling, the
     data goes out via WriteSpool, otherwise the Lwrite function is used to
     write directly to the output file. */
LPPDEVICE lpPDevice;
HANDLE hJob;
LPSTR lpData;
short length;
{
/*  short Result = (lpPDevice->Spooling) ? WriteSpool (hJob,lpData,length) :
      Lwrite (hJob,lpData,length); */
short Result;
#ifdef NICK_DEBUG
char buf[64];
#endif

    if (WriteError < 0) return(WriteError);
    Result = WriteSpool (hJob,lpData,length);
    if (Result < 0) WriteError = Result;
#ifdef NICK_DEBUG
        wsprintf(buf, "Write Spool Returned %d \n\r", Result);
        OutputDebugString(buf);
#endif
    return (Result);
}

void FAR PASCAL PutDialog (lpPDevice,hJob,lpData,length)
  /* This function will prompt the user with the given message by the spooler
     at the appropriate time in the output stream.  If not spooling, this
     function performs no operation. */
LPPDEVICE lpPDevice;
HANDLE hJob;
LPSTR lpData;
short length;
{
   WriteDialog (hJob,lpData,length);
}

BOOL FAR PASCAL GetSpoolState ()
  /* This function will locate the spooler status in the WIN.INI file and
     return it as a boolean value: TRUE meaning spooler activated, FALSE
     meaning otherwise */
{
    short Result = TRUE;
    char ReturnedString [2];

    GetProfileString ((LPSTR) "windows",(LPSTR) "spooler",(LPSTR) "Y",(LPSTR)
      ReturnedString,2);
    if (ReturnedString [0] == 'N' || ReturnedString [0] == 'n')
	Result = FALSE;
    return (Result);
}

int FAR PASCAL ColorDistance (RGBColor1,RGBColor2)
  /* This function will calculate the distance in the three-dimensional color
     space between two given RGB color values. */
DWORD RGBColor1,
      RGBColor2;
{
    HLSBLOCK HLSColor1,
	     HLSColor2;

    RGB_to_HLS(RGBColor1, (LPHLSBLOCK)&HLSColor1);
    ConstrainHLS((LPHLSBLOCK)&HLSColor1);
    RGB_to_HLS(RGBColor2, (LPHLSBLOCK)&HLSColor2);
    ConstrainHLS((LPHLSBLOCK)&HLSColor2);
    return (ABS((HLSColor1.Hue) - (HLSColor2.Hue)) |
      ((ABS((HLSColor1.Lightness) - (HLSColor2.Lightness))/500) << 13));
}

short FAR PASCAL GetDistance (X1,Y1,X2,Y2)
  /* This function returns the distance, in specified coordinates, between the
     two given points. */
short X1,
      X2,
      Y1,
      Y2;
{
    short Height = Y2 - Y1,
	  Width = X2 - X1,
	  SaveSine = Sine (Arctan (Width,Height,1,1)),
	  Length = (Height == 0 || SaveSine == 0) ? Width : 
		   (short)(Height * (long)TRIG_SCALE / SaveSine);

    return (ABS (Length));
}

PSTR FAR PASCAL GetString (Message)
  /* This function will load a string defined in the resource script file with
     the given Message number and place it in a global defined variable to be
     used by the caller. */
short Message;
{
    LoadString (hModule,Message,(LPSTR) MessageBuffer,BUFSIZE);
    return (MessageBuffer);
}
