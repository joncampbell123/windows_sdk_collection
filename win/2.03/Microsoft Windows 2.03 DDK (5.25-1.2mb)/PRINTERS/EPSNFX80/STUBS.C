#include "generic.h"

/* these output routines call the brute to do all the work; they
   1.  fake the lpDevice as a memory bitmap
   2.  change x, y coordinates to support banding
*/

ASPECT aspect = { MAXSTYLEERR, HYPOTENUSE, XMAJOR, YMAJOR };
NEAR PASCAL OffsetClipRect(LPRECT, short, short);


#if NO_BITBLT
#else
FAR PASCAL Bitblt(lpDevice, DstxOrg, DstyOrg, lpSrcDev, SrcxOrg, SrcyOrg, xExt, yExt, Rop, lpBrush, lpDrawmode)
LPDEVICE lpDevice; /* --> to destination bitmap descriptor */
short   DstxOrg      ; /* Destination origin - x coordinate    */
short   DstyOrg      ; /* Destination origin - y coordinate    */
BITMAP far *lpSrcDev     ; /* --> to source bitmap descriptor      */
short   SrcxOrg      ; /* Source origin - x coordinate         */
short   SrcyOrg      ; /* Source origin - y coordinate         */
short   xExt         ; /* x extent of the BLT                  */
short   yExt         ; /* x extent of the BLT                  */
long    Rop          ; /* Raster operation descriptor          */
long    lpBrush      ; /* --> to a physical brush (pattern)    */
long    lpDrawmode;
{
        register short status;

        /* only allow memory bitmap as source DC since 
	   we are banding device */
	 if (lpSrcDev && lpSrcDev->bmType != DEV_MEMORY)
	 	return 0;

        if (!fake(&lpDevice, &DstxOrg, &DstyOrg))
                return FALSE;
        status = dmBitblt(lpDevice, DstxOrg, DstyOrg, lpSrcDev, SrcxOrg, SrcyOrg, xExt, yExt, Rop, lpBrush, lpDrawmode);
        return status;
}
#endif


#if NO_PIXEL
#else
FAR PASCAL   Pixel(lpDevice, x, y, Color, lpDrawMode)
LPDEVICE lpDevice;
short   x;
short   y;
long    Color;
long    lpDrawMode;
{
        register short status;

        if (!fake(&lpDevice, &x, &y))
                return FALSE;
        status = dmPixel(lpDevice, x, y, Color, lpDrawMode);
        return status;
}
#endif

#if NO_OUTPUT
#else
FAR PASCAL  Output(lpDevice, style, count, lpPoints, lpPPen, lpPBrush, lpDrawMode, lpClipRect)
LPDEVICE lpDevice;                /* --> to the destination */
short   style   ;                /* Output operation                   */
short   count   ;                /* # of points                        */
LPPOINT lpPoints;                /* --> to a set of points             */
long    lpPPen  ;                /* --> to physical pen                */
long    lpPBrush;                /* --> to physical brush              */
long    lpDrawMode;              /* --> to a Drawing mode              */
long    lpClipRect;              /* --> to a clipping rectange if <> 0 */
{
        short status = OEM_FAILED;
        HANDLE hPoints = 0;

        if (style != OS_SCANLINES)
                goto exit;

        if (lpDevice->epType)
        {
                register short i;
                register short far *p;

                if (lpDevice->epMode & DRAFTFLAG)
                        return FALSE;

                /* make our own copy of the points */
                if (!(hPoints = (HANDLE) GlobalAlloc(GMEM_MOVEABLE, (long)(count * sizeof(POINT)))))
                        return FALSE;

                p = (short far *) GlobalLock(hPoints);

                Copy((LPSTR)p, (LPSTR)lpPoints, count * sizeof(POINT));

                lpPoints = (LPPOINT) p;

		p++;
		*p++ -= lpDevice->epYOffset;
		i = (count << 1) - 2;
		for (; i; --i)
			*p++ -= lpDevice->epXOffset;

                lpDevice->epBmpHdr.bmBits = lpDevice->epBmp;
                lpDevice->epMode |= GRXFLAG;
                lpDevice = (LPDEVICE) &lpDevice->epBmpHdr;
        }
        status = dmOutput(lpDevice, style, count, lpPoints, lpPPen, lpPBrush, lpDrawMode, *((long *)&aspect));

        if (hPoints)
                {
                GlobalUnlock(hPoints);
                GlobalFree(hPoints);
                }
exit:
        return status;
}
#endif

#if NO_STRBLT
#else
long FAR PASCAL   StrBlt(lpDevice, x, y, lpClipRect, lpString, count, lpFont, lpDrawMode, lpXform)
LPDEVICE lpDevice;
short   x          ;
short   y          ;
LPRECT  lpClipRect ;
LPSTR   lpString   ;
short   count      ;
LPFONTINFO  lpFont     ;
LPDRAWMODE  lpDrawMode ;     /* includes background mode and bkColor */
LPTEXTXFORM lpXform    ;
{
        short status;
        RECT cliprect;
	
        if (count > 0)
                {
                if (lpDevice->epType && (lpDevice->epMode & DRAFTFLAG))
                        return DraftStrblt(lpDevice, x, y, lpString, count, lpFont, lpDrawMode, lpXform, lpClipRect);
                Copy((LPSTR)&cliprect, (LPSTR)lpClipRect, sizeof(RECT));
                lpClipRect = (LPRECT)&cliprect;
                if (OffsetClipRect(lpClipRect, lpDevice->epXOffset, lpDevice->epYOffset) <= 0)
                        return 0;
                }
        if (lpFont->dfDevice)
                {
                short offset;

                /* if your printer fires the top NPINS in graphics mode you need to do nothing,
                   otherwise you have to do something like y += 2 if your NPINS is 7
                   and it fires the bottom 7 pins out of a total of 9 pins for
                   hardware fonts.  We are using graphics mode as our basis coordination
                   system */
                /* this should only occur in portrait mode */
                /* only collect texts that fall in this band, if text falls over
                   two bands put it in the top band */
                if (count > 0 && (y < lpDevice->epYOffset || y >= lpDevice->epYOffset + BAND_HEIGHT || x >= lpDevice->epPageWidth))
                        return 0;
                return chStrBlt(lpDevice, x, y, lpString, count,lpFont, lpDrawMode,lpXform, (LPRECT) lpClipRect);
                }
        if (lpDevice->epType)
        {
            lpDevice->epBmpHdr.bmBits = lpDevice->epBmp;
            y -= lpDevice->epYOffset;
            x -= lpDevice->epXOffset;
            lpDevice->epMode |= GRXFLAG;
            lpDevice = (LPDEVICE) &lpDevice->epBmpHdr;
        }
        return dmStrBlt(lpDevice, x, y, lpClipRect, lpString, count, lpFont, lpDrawMode, lpXform);
}
#endif

#if NO_SCANLR
#else
FAR PASCAL  ScanLR(lpDevice, x, y, Color, DirStyle)
LPDEVICE lpDevice;
short   x       ;
short   y       ;
long    Color   ;
short   DirStyle;
{
        short status;

        if (!fake(&lpDevice, &x, &y))
                return FALSE;
        return dmScanLR(lpDevice, x, y, Color, DirStyle);
}
#endif

#if NO_ENUMOBJ
#else
FAR PASCAL EnumObj(lpDevice, style, lpCallbackFunc, lpClientData)
LPDEVICE lpDevice;
short   style;
long    lpCallbackFunc;
long    lpClientData;
{
        return dmEnumObj((lpDevice->epType ? (LPDEVICE) &lpDevice->epBmpHdr: lpDevice), style, lpCallbackFunc, lpClientData);
}
#endif

#if NO_COLORINFO
#else
FAR PASCAL ColorInfo(lpDevice, ColorIn, lpPhysBits)
LPDEVICE lpDevice;
long    ColorIn;
long    lpPhysBits;
{
        return dmColorInfo((lpDevice->epType ? (LPDEVICE) &lpDevice->epBmpHdr: lpDevice), ColorIn, lpPhysBits);
}
#endif

#if NO_REALIZEOBJECT
#else
FAR PASCAL RealizeObject(lpDevice, Style, lpInObj, lpOutObj, lpTextXForm)
LPDEVICE   lpDevice;
short   Style;
LPSTR lpInObj;
LPSTR lpOutObj;
LPSTR lpTextXForm;
{
        if (Style == -OBJ_FONT)
                return 0;
        if (Style == OBJ_FONT)
                {
                /* hardware fonts in portrait mode only */
                if (lpDevice->epType != DEV_PORT)
                        return 0;
                return chRealizeObject(lpDevice, (LPLOGFONT) lpInObj,(LPFONTINFO) lpOutObj, (LPTEXTXFORM) lpTextXForm);
                }

        return dmRealizeObject((lpDevice->epType ? (LPDEVICE) &lpDevice->epBmpHdr: lpDevice), Style, lpInObj, lpOutObj, lpTextXForm);
}
#endif

#if NO_FAKE
#else
short NEAR PASCAL fake(lplpDevice, x, y)
LPDEVICE far *lplpDevice;
short far *x, far *y;
{
    register LPDEVICE lpDevice;

    lpDevice = *lplpDevice;

    if (lpDevice->epType)
    {
        if (lpDevice->epMode & DRAFTFLAG)
                return FALSE;
        lpDevice->epBmpHdr.bmBits = lpDevice->epBmp;
        lpDevice->epMode |= GRXFLAG;
        if (y)
                *y -= lpDevice->epYOffset;
        if (x)
                *x -= lpDevice->epXOffset;
        *lplpDevice = (LPDEVICE) &lpDevice->epBmpHdr;
    }
    return TRUE;
}
#endif
