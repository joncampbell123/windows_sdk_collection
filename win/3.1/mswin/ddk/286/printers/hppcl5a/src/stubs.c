/**[f******************************************************************
* stubs.c -
*
* Copyright (C) 1988,1989 Aldus Corporation.
* Copyright (C) 1988-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.
* Company confidential.
*
**f]*****************************************************************/
  
/*
 * $Header: stubs.c,v 3.890 92/02/06 16:11:10 dtk FREEZE $
 */

/*
 * $Log:	stubs.c,v $
 * Revision 3.890  92/02/06  16:11:10  16:11:10  dtk (Doug Kaltenecker)
 * Win3.1 Freeze
 * 
 * Revision 3.871  91/12/18  14:32:47  14:32:47  daniels (Susan Daniels)
 * Fix Bug #737 and #545: White text prints as black with high res monitor.
 * Added code sent by MS.
 * Fix in ColorInfo().
 * 
 * Revision 3.870  91/11/08  11:42:55  11:42:55  dtk (Doug Kaltenecker)
 * 3.1 Release Candidate 1
 * 
 * Revision 3.866  91/11/08  09:37:51  09:37:51  daniels (Susan Daniels)
 * Bug #698: Excel print preview shows color.  Fix is to make the fix for
 * bug #545 Windows 3.0 specific.  Turned in winbug for bug in 256 color 
 * monitor.
 * 
 * Revision 3.865  91/11/01  13:50:52  13:50:52  dtk (Doug Kaltenecker)
 * Beta release for Windows 3.1
 * 
 * Revision 3.862  91/10/25  13:46:15  13:46:15  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.861  91/10/25  13:30:09  13:30:09  daniels (Susan Daniels)
 * Fix #646: UAE in low mem conditions -- also MS #15037.
 * 
 * Revision 3.852  91/10/09  14:58:41  14:58:41  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.850  91/10/04  16:48:41  16:48:41  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE
 * 
 * Revision 3.841  91/10/04  16:31:32  16:31:32  dtk (Doug Kaltenecker)
 * Re-added the check for a valid lpPhysBits before filling it in 
 * in the routine ColorInfo.
 * 
 * Revision 3.840  91/09/28  14:16:13  14:16:13  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE 3 FOR WINWORD
 * 
 * Revision 3.831  91/09/27  16:41:46  16:41:46  tshannon (Terry Shannon)
 * Fixed 256 color monitor white text probelm, and excel grayscale problem.
 * 
 * Revision 3.830  91/09/19  10:29:14  10:29:14  dtk (Doug Kaltenecker)
 * Added check for a valid pointer to lpPhysBits in ColorInfo.
 * Fixed the GP Fault with ATM.
 * 
 * Revision 3.824  91/09/18  13:48:33  13:48:33  daniels (Susan Daniels)
 * Work around for Win30 for BUG #646.  Put back in test to skip Bltbit()
 * for TEXTBANDs if we're in WIndows 30.         i
 * 
 * Revision 3.823  91/09/18  09:36:18  09:36:18  tshannon (Terry Shannon)
 * Fixed ColorInfo to return proper colors. Fixed bug where 
 * white text would not print on 256 color monitors. 
 * 
 * Revision 3.822  91/09/16  10:32:40  10:32:40  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE 2 FOR WIN30.
 * 
 * Revision 3.820  91/09/06  14:11:06  14:11:06  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE FOR WINWORD 2.0
 * 
 * Revision 3.812  91/08/22  14:31:03  14:31:03  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.807  91/08/08  10:30:23  10:30:23  dtk (Doug Kaltenecker)
 * PREBETA3
 * 
 * Revision 3.802  91/07/22  12:53:11  12:53:11  oakeson (Ken Oakeson)
 * BETA
 * 
 * Revision 3.799  91/07/02  11:50:39  11:50:39  daniels (Susan Daniels)
 * Beta
 * 
 * Revision 3.796  91/06/26  11:24:58  11:24:58  stevec (Steve Claiborne)
 * BETA
 * 
 * Revision 3.791  91/06/24  10:23:39  10:23:39  oakeson (Ken Oakeson)
 * Avoid bailing out of BitBlt for text bands so that driver-owned memory
 * for TrueType bitmaps can be zeroed out by GDI
 * 
 */
  
/**********************************************************************
*
* 08 nov 91   SD      Made fix for BUG #545 Windows 3.0 specific.  
* 18 oct 91   SD      Modified check below for Win31 so that it only
*                     continues through Bitblt if the textband is being
*                     cleared (i.e., Rop == BLACKNESS).
* 18 sep 91   SD      Made removal of check to return from Bitblt() if
*                     TEXTBAND conditional on WIN31.  Fixes low memory
*                     UAE in Windows 30.  BUG # 646.
* 20 Jul 90  SJC      Removed PCL_* - don't need to lock segments.
* 09 may 90   SD          Check epType in Pixel(), be certain it's not
*                         a bitmap; Fix from MS. BUGFIX
* 02 oct 89    clarkc      EnuumObj moved to EnumObj.c
* 30 aug 89    clarkc      Replaced FatalExit(6) with return in Output()
* 06 may 89    peterbe     Add () pair in #define MAXSTYLEERR
* 27 apr 89    peterbe     Tabs @ 8 spaces, other cleanup.
*   1-17-89    jimmat  Added PCL_* entry points to lock/unlock data seg.
*/
  
/*
*  NOTE:  Currently in this module only the Output routine has a PCL_*
*     entry point to lock/unlock the driver's data segment.  The
*     other exported routines were not given these entry points
*     because at this time they do not appear to use the data seg.
*     If that should change (like uncommenting the debug msgs),
*     they should then lock/unlock the data seg.
*
*/
//#define DEBUG
#include "generic.h"
#include "resource.h"
#define FONTMAN_UTILS
#include "fontman.h"
  
short FixBandBitmap(LPDEVICE far *, short far *, short far *);
  
int FAR PASCAL Output(LPDEVICE, short, short, LPPOINT, void FAR *, void FAR *, LPDRAWMODE, LPRECT);
  
  
extern int display_pbrush_size;
  
// from scanline.asm
int NEAR PASCAL do_scanlines(BITMAP FAR *lpbm, short count, LPPOINT lpPoints, LPSTR lpPBrush, LPDRAWMODE lpDrawMode);
  
/*  debugs
*/
#define DBGtrace(msg) DBMSG(msg)
  
/* char *faceescape = "\033(s0T";
*/
  
/*  these output routines call the brute to do all the work; they
*  1.  fake the lpDevice as a memory bitmap
*  2.  change x, y coordinates to support banding
*/
#define MAXTEMPPOINTS   128     // temporary points workspace
short rgpoint[2 * MAXTEMPPOINTS];
  
int NEAR PASCAL DoPatBlt(
LPDEVICE lpDevice,
short DstX,
short DstY,
short ExtX,
short ExtY,
void FAR *lpBrush,
WORD rop2)
{
    int y;
    POINT points[2];
    DRAWMODE dm;
#ifdef DEBUG_FUNCT
    DB(("Entering DoPatBlt\n"));
#endif
  
    dm.Rop2 = rop2;
  
    points[1].xcoord = DstX;
    points[1].ycoord = DstX + ExtX;
  
    for (y = DstY; y < (DstY + ExtY); y++ ) {
        points[0].ycoord = y;
        do_scanlines((BITMAP FAR *)lpDevice, 2, points, (LPSTR)lpBrush + display_pbrush_size, &dm);
    }
  
#ifdef DEBUG_FUNCT
    DB(("Entering DoPatBlt\n"));
#endif
    return TRUE;
}
  
  
#define HIWORD(dw)      ((DWORD)(dw) >> 16)
  
int FAR PASCAL Bitblt(
LPDEVICE lpDevice,
short DstxOrg,
short DstyOrg,
BITMAP FAR *lpSrcDev,
short SrcxOrg,
short SrcyOrg,
short xExt,
short yExt,
DWORD Rop,
void FAR *lpBrush,
LPDRAWMODE lpDrawmode)
{
    WORD rop2;
#ifdef DEBUG_FUNCT
    DB(("Entering Bitblt\n"));
#endif
  
#if defined(WIN31)  
  if(TEXTBAND && Rop != BLACKNESS) {
      DBMSG(("epNband==TEXTBAND, exiting BitBlit \n"));
      return TRUE;
  }
#else
    if(TEXTBAND) {
        DBMSG(("epNband==TEXTBAND, exiting BitBlit \n"));
        return TRUE;
    }
#endif

    //  only allow memory bitmap as source DC
    //  what happens when the source is the device but from
    //  another band?
    //
    //  if (lpSrcDev && lpSrcDev->bmType != DEV_MEMORY)
    //          return 0;
  
    DBMSG(("Calling FixBandBitmap\n"));
    if (!FixBandBitmap(&lpDevice, &DstxOrg, &DstyOrg))
        return TRUE;
    DBMSG(("Returned from FixBandBitmap\n"));
  
    // special case PatBlt() to get nice halftone brush fills
    // instead of the ugly display dithers
  
    /* sjc ~~~ */
    if (lpBrush &&
    ((LPPBRUSH)((LPSTR)lpBrush + display_pbrush_size))->bMyBrush) {
        switch ((WORD)HIWORD(Rop)) {
  
            case HIWORD(PATCOPY):
                rop2 = R2_COPYPEN;  // P
                break;
            case HIWORD(PATINVERT):
                rop2 = R2_XORPEN;   // DPx
                break;
            case HIWORD(0x000500A9):
                rop2 = R2_NOTMERGEPEN;  // DPon
                break;
            case HIWORD(0x00500325):
                rop2 = R2_MASKPENNOT;   // PDna
                break;
            case HIWORD(0x000F0001):
                rop2 = R2_NOTCOPYPEN;   // Pn
                break;
            case HIWORD(0x000A0329):
                rop2 = R2_MASKNOTPEN;   // DPna
                break;
            case HIWORD(0x005F00E9):
                rop2 = R2_NOTMASKPEN;   // DPan
                break;
            case HIWORD(0x00A000C9):
                rop2 = R2_MASKPEN;  // DPa
                break;
            case HIWORD(0x00A50065):
                rop2 = R2_NOTXORPEN;    // DPxn
                break;
            case HIWORD(0x00AF0229):
                rop2 = R2_MERGENOTPEN;  // DPno
                break;
            case HIWORD(0x00F50225):
                rop2 = R2_MERGEPENNOT;  // PDno
                break;
            case HIWORD(0x00FA0089):
                rop2 = R2_MERGEPEN; // DPo
                break;
            default:
                goto NORMAL;
        }
  
        return DoPatBlt(lpDevice, DstxOrg, DstyOrg, xExt, yExt, lpBrush, rop2);
    }
    NORMAL:
#ifdef DEBUG_FUNCT
    DB(("Entering Bitblt\n"));
#endif
    return dmBitblt(lpDevice, DstxOrg, DstyOrg, lpSrcDev, SrcxOrg, SrcyOrg,
    xExt, yExt, Rop, lpBrush, lpDrawmode);
}
  
#define HYPOTENUSE  14
#define YMAJOR      10
#define XMAJOR      10
#define MAXSTYLEERR (HYPOTENUSE*2)
  
ASPECT aspect = { MAXSTYLEERR, HYPOTENUSE, XMAJOR, YMAJOR };
  
  
far PASCAL Pixel(lpDevice, x, y, Color, lpDrawMode)
LPDEVICE lpDevice;
short x;
short y;
DWORD Color;
LPDRAWMODE lpDrawMode;
{
    BITMAP FAR *lpBitmap;
    short status;
    short nWidth = FALSE;
#ifdef DEBUG_FUNCT
    DB(("Entering Pixel\n"));
#endif
  
    /*  If the application is using the BANDINFO escape and this is a
    *  text band, then the only graphics that could be coming from
    *  GDI would be those necessary to simulate text (i.e., vector
    *  fonts) or a text attribute (i.e., strike-through).  If this is
    *  the case, then we have to set a special flag (epGDItext) which
    *  indicates to the banding code that we will have to ask the
    *  application to send down text on the graphic bands, just so
    *  we can band through GDI's simulations.  If the application is
    *  not using the BANDINFO escape, then the state of this flag does
    *  not matter because the application will be sending text and
    *  graphics on every band.
    */
    if (lpDevice->epType && TEXTBAND) /*5/09/90 BUGFIX*/
        lpDevice->epGDItext = TRUE;
  
    /*  HACK OF DOOM, really ugly deception follows:
    If in Landscape mode and on the final band, the bitmap may not be
    byte aligned, i.e. bmWidth != bmWidthBytes * 8.  This currently
    causes problems with display drivers clipping some of the pixels
    set because the display drivers ASSUME that the bitmap is using
    bits 0 thru bmWidth-1, which is not the case here.  So be sneaky
    (read that ugly) and temporarily tell the driver that the width
    IS byte aligned, and no clipping takes place.  If we can pursuade
    the display drivers to check vs. bmWidthBytes instead of bmWidth,
    we'll save a lot of time since they'd only add 3 SHL instructions
    instead of the painful stuff below.  27 Nov 1989   Clark Cyr */
  
    if (lpDevice->epType &&
        (lpDevice->epType == (short)DEV_LAND) &&
        (lpDevice->epNband >= lpDevice->epNumBands - 1))
    {
        lpBitmap = &lpDevice->epBmpHdr;
        nWidth = lpBitmap->bmWidth;
        lpBitmap->bmWidth = lpBitmap->bmWidthBytes * 8;
    }
  
    /*  DBGtrace(("Setting Pixel,x=%d,y=%d\n", x, y));
    */
    if (!FixBandBitmap(&lpDevice, &x, &y))
        return TRUE;
  
    status = dmPixel(lpDevice, x, y, Color, lpDrawMode);
    if (nWidth)
    {
        lpBitmap->bmWidth = nWidth;
    }
#ifdef DEBUG_FUNCT
    DB(("Exiting Pixel\n"));
#endif
    return status;
    //  return dmPixel(lpDevice, x, y, Color, lpDrawMode);
}
  
int FAR PASCAL Output(
LPDEVICE lpDevice,
short style,
short count,
LPPOINT lpPoints,
void FAR *lpPPen,
void FAR *lpPBrush,
LPDRAWMODE lpDrawMode,
LPRECT lpClipRect)
{
    short status;
    HANDLE hPoints = 0;
    RECT rcClip;
  
#ifdef DEBUG_FUNCT
    DB(("Entering Output\n"));
#endif
  
    if (lpDevice->epType)
    {
        register short i;
        short far * p;
  
        /* set flag to indicate graphics on PAGE */
        lpDevice->epMode |= ANYGRX;
  
        if ((TEXTBAND) || (lpDevice->epNband==1))
            return TRUE;
  
        if (lpDevice->epMode & DRAFTFLAG)
            return FALSE;
  
        /* make our own copy of the points
        */
        if (count > MAXTEMPPOINTS)
        {
            if (!(hPoints = (HANDLE)GlobalAlloc(GMEM_MOVEABLE,
                (DWORD)(count * sizeof(POINT)))))
                return FALSE;
  
            if (!(p = (short far *)GlobalLock(hPoints)))
            {
                GlobalFree(hPoints);
                return FALSE;
            }
        }
        else
            p = rgpoint;
  
        if ( lpDevice->epYOffset || lpDevice->epXOffset )
        {
            lmemcpy((LPSTR)p, (LPSTR)lpPoints, count * sizeof(POINT));
            lpPoints = (LPPOINT)p;
  
            if (style == OS_SCANLINES)
            {
                p++;
                if ( lpDevice->epYOffset )  // No need to transform band one
                    *p++ -= lpDevice->epYOffset >> lpDevice->epScaleFac;
                else
                    p++;
                i = (count << 1) - 2;
  
                if ( lpDevice->epXOffset )
                {
                    for ( ; i; --i)
                        *p++ -= lpDevice->epXOffset >> lpDevice->epScaleFac;
                }
            }
            else if (style == OS_BEGINNSCAN || style == OS_ENDNSCAN)
                lpPoints = NULL;   // Do nothing; fall through to call to dmOutput
            else if (style == OS_POLYLINE)
            {
                LPPOINT lppt = lpPoints;
  
                for (i = count; i; i--, lppt++)
                {
                    lppt->xcoord -= lpDevice->epXOffset >> lpDevice->epScaleFac;
                    lppt->ycoord -= lpDevice->epYOffset >> lpDevice->epScaleFac;
                }
            }
            else
            {
                if (hPoints) {
                    GlobalUnlock(hPoints);
                    GlobalFree(hPoints);
                }
                return FALSE;
            }
        }
  
        /* set flag to indicate graphics in BAND */
        lpDevice->epMode |= GRXFLAG;
  
        /* point to location of actual storage for bitmap */
        /* set up in Enable() for Pmode */
        if (RealMode)
            lpDevice->epBmpHdr.bmBits = lpDevice->epBmp;
  
        if (lpClipRect)
        {
            rcClip.left = lpClipRect->left - (lpDevice->epXOffset
            >> lpDevice->epScaleFac);
            rcClip.right = lpClipRect->right - (lpDevice->epXOffset
            >> lpDevice->epScaleFac);
            rcClip.top = lpClipRect->top - (lpDevice->epYOffset
            >> lpDevice->epScaleFac);
            rcClip.bottom = lpClipRect->bottom - (lpDevice->epYOffset
            >> lpDevice->epScaleFac);
            lpClipRect = &rcClip;
        }
        //        lpDevice->epBmpHdr.bmBits=lpDevice->epBmp;
        lpDevice = (LPDEVICE) &lpDevice->epBmpHdr;
    }
  
    // DBGtrace(("global_grayscale=%d\n",global_grayscale));
  
    if (style == OS_SCANLINES &&
        lpPBrush &&
        //      global_grayscale &&     // This line removed to force HP brush patterns.
    ((LPPBRUSH)((LPSTR)lpPBrush + display_pbrush_size))->bMyBrush) {
        status = do_scanlines((BITMAP FAR *)lpDevice, count, lpPoints, (LPSTR)lpPBrush + display_pbrush_size, lpDrawMode);
    } else {     // This else should never be executed. Left in for proto code testing.
        status = dmOutput(lpDevice, style, count, lpPoints, lpPPen, lpPBrush,
        lpDrawMode, lpClipRect);
    }
  
    if (hPoints)
    {
        GlobalUnlock(hPoints);
        GlobalFree(hPoints);
    }
  
#ifdef DEBUG_FUNCT
    DB(("Exiting Output\n"));
#endif
    return status;
}
  
int far PASCAL DeviceBitmap(lpDevice, command, lpBitmap, lpBits)
LPDEVICE lpDevice;
int command;
BITMAP far *lpBitmap;
BYTE far *lpBits;
{
#ifdef DEBUG_FUNCT
    DB(("Entering DeviceBitmap\n"));
#endif
    return (0);
}
  
  
int far PASCAL FastBorder(lpRect, borderWidth, borderDepth,
rasterOp, lpDevice, lpPBrush, lpDrawMode, lpClipRect)
LPRECT lpRect;
WORD borderWidth;
WORD borderDepth;
DWORD rasterOp;
LPDEVICE lpDevice;
LPSTR lpPBrush;
LPDRAWMODE lpDrawMode;
LPRECT lpClipRect;
{
#ifdef DEBUG_FUNCT
    DB(("Entering FastBorder\n"));
#endif
    return (0);
}
  
  
int far PASCAL SetAttribute(lpDevice, stateNum, index, attribute)
LPDEVICE lpDevice;
int stateNum;
int index;
int attribute;
{
#ifdef DEBUG_FUNCT
    DB(("Entering SetAttribute\n"));
#endif
    return (0);
}
  
  
far PASCAL ScanLR(lpDevice, x, y, Color, DirStyle)
LPDEVICE lpDevice;
short x;
short y;
long Color;
short DirStyle;
{
    BITMAP FAR *lpBitmap;// = &lpDevice->epBmpHdr;
    short status;
    short nWidth = FALSE;
#ifdef DEBUG_FUNCT
    DB(("Entering ScanLR\n"));
#endif
  
    /* DBGtrace(("In ScanLR\n"));
    */
  
    /*  HACK OF DOOM, see commentary in Pixel() routine above.
    27 Nov 1989   Clark Cyr */
  
    // sjc - ms 7-90
    if (lpDevice->epType &&
        (lpDevice->epType == (short)DEV_LAND) &&
        (lpDevice->epNband >= lpDevice->epNumBands - 1))
    {
        lpBitmap = &lpDevice->epBmpHdr;
        nWidth = lpBitmap->bmWidth;
        lpBitmap->bmWidth = lpBitmap->bmWidthBytes * 8;
    }
    // sjc - ms 7-90
    if (!FixBandBitmap(&lpDevice, &x, &y))
        return TRUE;
    status = dmScanLR(lpDevice, x, y, Color, DirStyle);
    if (nWidth)
    {
        lpBitmap->bmWidth = nWidth;
    }
#ifdef DEBUG_FUNCT
    DB(("Exiting ScanLR\n"));
#endif
    return status;
    //  return dmScanLR(lpDevice, x, y, Color, DirStyle);
}
  
#if 0
/* Moved to EnumObj.c.     2 Oct 1989  Clark Cyr     */
  
far PASCAL EnumObj(lpDevice, style, lpCallbackFunc, lpClientData)
LPDEVICE lpDevice;
short style;
long lpCallbackFunc;
long lpClientData;
{
  
#ifdef DEBUG_FUNCT
    DB(("Entering EnumObj\n"));
#endif
    return dmEnumObj((lpDevice->epType ? (LPDEVICE) & lpDevice->epBmpHdr :
    lpDevice), style, lpCallbackFunc, lpClientData);
}
#endif
  
  
DWORD FAR PASCAL ColorInfo(LPDEVICE lpDevice, DWORD ColorIn, DWORD FAR *lpPhysBits)
{
#if defined (WIN31)
    {
        LPBITMAP lpBmp;
        long ret;
        if  (lpDevice->epType)
            lpBmp = (LPBITMAP)&lpDevice->epBmpHdr;
        else
            lpBmp = (LPBITMAP)lpDevice;

        ret = (dmColorInfo(lpBmp, 
                               ColorIn, 
                               lpPhysBits));
        if (lpBmp->bmPlanes == 1)
        {
            if (lpPhysBits)
            {
                BYTE gray;
                gray = INTENSITY(GetRValue(ColorIn),
                                 GetGValue(ColorIn),
                                 GetBValue(ColorIn));
                ret = RGB(gray, gray, gray);
                *lpPhysBits = (*lpPhysBits & 0xFF000000) | ret;
            }
        }
        return (ret);
    }
#else
DWORD colorreturn;

    colorreturn = dmColorInfo((lpDevice->epType ? (LPDEVICE) & lpDevice->epBmpHdr :	lpDevice), 
                               ColorIn, 
                               lpPhysBits);
    if (lpPhysBits)
        *lpPhysBits = ((*lpPhysBits & 0xFF000000) | ColorIn); 

    return(ColorIn);
#endif
}
  
short FixBandBitmap(LPDEVICE far *lplpDevice, short far *x, short far *y)
{
    register LPDEVICE lpDevice;
  
#ifdef DEBUG_FUNCT
    DB(("Entering FixBandBitmap\n"));
#endif
    lpDevice = *lplpDevice;
  
    // if this is our pdevice (not a memory bitmap) do some stuff
  
    if (lpDevice->epType) {
  
        lpDevice->epMode |= ANYGRX;
  
        if (lpDevice->epMode & DRAFTFLAG) {
            DBMSG(("DRAFTFLAG set\n"));
            return FALSE;
        }
  
        if (lpDevice->epNband == 1) {
            DBMSG(("epNband==1\n"));
            return FALSE;
        }
  
        if (RealMode)
            lpDevice->epBmpHdr.bmBits = lpDevice->epBmp;
  
        /*point to lpDevice location for storing the bitmap*/
  
        if (y)
            *y -= lpDevice->epYOffset >> lpDevice->epScaleFac;
  
        if (x)
            *x -= lpDevice->epXOffset >> lpDevice->epScaleFac;
  
        lpDevice->epMode |= GRXFLAG;
  
        // fake bitmap within LPDEVICE as the actual bitmap
  
        *lplpDevice = (LPDEVICE)&lpDevice->epBmpHdr;
    }
  
#ifdef DEBUG_FUNCT
    DB(("Entering FixBandBitmap\n"));
#endif
    return TRUE;
}


