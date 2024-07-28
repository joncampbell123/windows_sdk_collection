/*/   STUBS.C for TTY
 +-----------------------------------------+
 | Fralc Consultores (c), Mexico 1986-1989 |
 +-----------------------------------------+
/*/

// ------------------------------------------------------------------------
// Copyright (C) 1989-1990, Microsoft Corporation. All rights reserved.
// ------------------------------------------------------------------------
//	Microsoft History (latest first)
//	02 feb 90	peterbe		Removed ExtTextOut() (#ifdef) since
//					we don't support 3.0 graphics.
//					StrBlt() calls DraftStrBlt() (to print)
//					and chStrBlt() (to get width) directly.
//	04 dec 89	peterbe		Added debug statements, use hppcl
//					debug library and debug.inc.
//	20 oct 89	peterbe		Checked in.
// ------------------------------------------------------------------------

#include "generic.h"




#ifdef DEBUG
 #include "debug.h"
 #define DBGstub(msg) DBMSG(msg)
#else
 #define DBGstub(msg) /* zip */
#endif

/* these output routines call the brute to do all the work; they
   1.  fake the lpdv as a memory bitmap
   2.  change x, y coordinates to support banding
*/

ASPECT aspect = { MAXSTYLEERR, HYPOTENUSE, XMAJOR, YMAJOR };
NEAR PASCAL OffsetClipRect(LPRECT, short, short);

int FAR cdecl wsprintf(LPSTR,LPSTR,...);

extern int fBorrame;

FAR PASCAL devBitblt(
	 LPDV lpdv, /* --> to destination bitmap descriptor */
	 short	 DstxOrg      , /* Destination origin - x coordinate    */
	 short	 DstyOrg      , /* Destination origin - y coordinate    */
	 BITMAP far *lpSrcDev , /* --> to source bitmap descriptor      */
	 short	 SrcxOrg      , /* Source origin - x coordinate         */
	 short	 SrcyOrg      , /* Source origin - y coordinate         */
	 short	 xExt	      , /* x extent of the BLT                  */
	 short	 yExt	      , /* x extent of the BLT                  */
	 long	 Rop	      , /* Raster operation descriptor          */
	 long	 lpBrush      , /* --> to a physical brush (pattern)    */
	 long	 lpDrawmode)
{
    return 0;
}



FAR PASCAL   Pixel(
		   LPDV lpdv,
		   short   x,
		   short   y,
		   long	   Color,
		   long	   lpDrawMode)
{
    return 0;
#if 0
    register short status;

    if (!fake(&lpdv, &x, &y))
	return FALSE;
    status = dmPixel(lpdv, x, y, Color, lpDrawMode);
    return status;
#endif
}

FAR PASCAL  Output(
    LPDV lpdv,	 /* --> to the destination */
    short   style   ,	 /* Output operation                   */
    short   count   ,	 /* # of points                        */
    LPPOINT lpPoints,	 /* --> to a set of points             */
    long    lpPPen  ,	 /* --> to physical pen                */
    long    lpPBrush,	 /* --> to physical brush              */
    long    lpDrawMode,	 /* --> to a Drawing mode              */
    long    lpClipRect)	 /* --> to a clipping rectange if <> 0 */
{
    return 0;
#if 0
    short status = (short)OEM_FAILED;
    HANDLE hPoints = 0;

    if (style != OS_SCANLINES)
	    goto exit;

    return FALSE;

exit:
	return status;
#endif
}






// All tty text output is done with StrBlt(), which is really a stub
// for ExtTextOut()


int far PASCAL FastBorder(
    LPRECT lpRect,
    WORD borderWidth,
    WORD borderDepth,
    DWORD rasterOp,
    LPDV lpdv,
    LPSTR lpPBrush,
    LPDRAWMODE lpDrawMode,
    LPRECT lpClipRect)
{
    return (0);
}

FAR PASCAL  ScanLR(
		  LPDV lpdv,
		  short	  x	  ,
		  short	  y	  ,
		  long	  Color	  ,
		  short	  DirStyle )
{
    return 0;
#if 0
    if (!fake(&lpdv, &x, &y))
	return FALSE;
    return dmScanLR(lpdv, x, y, Color, DirStyle);
#endif
}

FAR PASCAL EnumObj(
    LPDV lpdv,
    short   style,
    long    lpCallbackFunc,
    long    lpClientData )
{
    return 1;
}

FAR PASCAL ColorInfo(
		     LPDV lpdv,
		     long    ColorIn,
		     long    lpPhysBits)
{
    return dmColorInfo(
	(lpdv->iType ? ((LPDV)&lpdv->epBmpHdr): lpdv),
	ColorIn, lpPhysBits);
}

FAR PASCAL RealizeObject(
			 LPDV	lpdv,
			 short	 Style,
			 LPSTR lpInObj,
			 LPSTR lpOutObj,
			 LPSTR lpTextXForm)
{
    if (Style == -OBJ_FONT)
	    return 0;
    if (Style == OBJ_FONT)
	    {
	    /* hardware fonts in portrait mode only */
	    return chRealizeObject(lpdv, (LPLOGFONT) lpInObj,
		(LPFONTINFO) lpOutObj, (LPTEXTXFORM) lpTextXForm);
	    }
    return 0;
}
