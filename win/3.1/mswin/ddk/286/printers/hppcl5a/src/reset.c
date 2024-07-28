/**[f******************************************************************
* reset.c -
*
* Copyright (C) 1988,1989 Aldus Corporation.
* Copyright (C) 1988-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.
* Company confidential.
*
**f]*****************************************************************/
/***************************************************************************/
/*******************************   reset.c   *******************************/
//
//  Reset: Enable and Disable driver, called on CreateIC/DC.
//
//  rev:
//
// 18 dec 91  SD     Make GetEnvironment parameter dependent on Win31.  Needed
//                   because Win31 used device rather than port.
// 22 oct 91  SD     Make dpVersion dependent upon Win31.  Needed by WinWord 2.0
//                   for paper bins support.
// 14 aug 91  SD     BUG #560:  Paper size and source commands need to be
//                   monitored and only sent when necessary within a document.
//                   Sending on each page was killing IIISi throughput.
// 11 feb 91  SD       Added ELI support.
// 20 Jul 90  SJC      Removed PCL_* - don't need to lock segments.
// 09 may 90   SD    Check ExtDeviceMode() return value;  Fix from MS. BUGFIX
// 03 feb 90    KLO Changed lmemcpy() to lstrncpy()
// 04 jan 90    clarkc  introduced USEEXTDEVMODE, using ExtDevMode to initialize
// 30 nov 89    peterbe Visual edge calls in ifdef now.
// 19 sep 89    peterbe Changed LITTLESPUD to HPLJIIP
// 15 sep 89    peterbe It's a HP LJ II P if (lpStuff->prtCaps & HP LJ IIP)
// 06 sep 89    peterbe Add code to allocate space for scanline buffer
//          and init. epLineBuf.  (LITTLESPUD)
// 07 aug 89    peterbe Changed lstrcmp() to lstrcmpi().
//  11-14-86    msd added calls to Init/FreeFontSummary
//   1-13-89    jimmat  Reduced # of redundant strings by adding lclstr.h
//   1-17-89    jimmat  Added PCL_* entry points to lock/unlock data seg.
//   1-18-89    jimmat  Space for the epBuf is only added to the DEVICE
//          struct when in LANDSCAPE (that's when it's used).
//   1-25-89    jimmat  Use global hLibInst instead of GetModuleHandle() -
//          No longer requires lclstr.h.
//   2-07-89    jimmat  Driver Initialization changes.
//   2-24-89    jimmat  Removed parameters to lp_enbl(), lp_disable().
//
  
/*
* $Header: reset.c,v 3.890 92/02/06 16:11:14 dtk FREEZE $
*/
  
/*
* $Log:	reset.c,v $
 * Revision 3.890  92/02/06  16:11:14  16:11:14  dtk (Doug Kaltenecker)
 * Win3.1 Freeze
 * 
 * Revision 3.873  91/12/18  13:52:46  13:52:46  daniels (Susan Daniels)
 * Change parameter to GetEnvironment for Win31 -- use device not port.
 * 
 * Revision 3.872  91/12/04  13:09:27  13:09:27  dtk (Doug Kaltenecker)
 * Changed 3.1 version number from 310 to 30AA
 * 
 * Revision 3.871  91/12/02  16:44:52  16:44:52  dtk (Doug Kaltenecker)
 * Changed ifdeff TTs to ifdef WIN31s changed the dpText field of 
 * the GDIINFO structure to no longer tell GDI that we can strikeout
 * and underline if it's in WIN31 so it will simulate it with TT.
 * 
 * Revision 3.871  91/11/22  13:18:25  13:18:25  dtk (Doug Kaltenecker)
 * Win 3.1 Post Beta 3 version.
 * 
 * Revision 3.870  91/11/08  11:43:00  11:43:00  dtk (Doug Kaltenecker)
 * 3.1 Release Candidate 1
 * 
 * Revision 3.866  91/11/05  13:43:39  13:43:39  daniels (Susan Daniels)
 * Removed a debuging section that was meant to be temporary but was
 * still there.
 * 
 * Revision 3.865  91/11/01  13:50:57  13:50:57  dtk (Doug Kaltenecker)
 * Beta release for Windows 3.1
 * 
 * Revision 3.862  91/10/25  13:46:19  13:46:19  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.860  91/10/23  09:47:43  09:47:43  dtk (Doug Kaltenecker)
 * WinWird Release
 * 
 * Revision 3.853  91/10/22  16:52:26  16:52:26  daniels (Susan Daniels)
 * Making dpVersion dependent upon whether Win31 or Win30.  WinWord 2.0 uses
 * this to determine how to handle bin stuff.
 * 
 * Revision 3.852  91/10/09  14:58:46  14:58:46  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.850  91/10/04  16:48:48  16:48:48  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE
 * 
 * Revision 3.840  91/09/28  14:16:18  14:16:18  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE 3 FOR WINWORD
 * 
 * Revision 3.830  91/09/24  13:29:15  13:29:15  dtk (Doug Kaltenecker)
 * Put ifdef TT's around the TC_TT_ABLE stuff in getgdiinfo.
 * 
 * Revision 3.830  91/09/18  16:32:33  16:32:33  dtk (Doug Kaltenecker)
 * RELEASE
 * 
 * Revision 3.822  91/09/16  10:32:46  10:32:46  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE 2 FOR WIN30.
 * 
 * Revision 3.820  91/09/06  14:11:10  14:11:10  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE FOR WINWORD 2.0
 * 
 * Revision 3.812  91/08/22  14:31:08  14:31:08  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.808  91/08/16  13:09:29  13:09:29  daniels (Susan Daniels)
 * Fix Bug 560: Eli printing too slow.  Send down escape sequences for 
 * paper size and tray only on first page or when they change.
 * 
 * Revision 3.807  91/08/08  10:30:28  10:30:28  dtk (Doug Kaltenecker)
 * PREBETA3
 * 
 * Revision 3.802  91/07/29  11:13:13  11:13:13  oakeson (Ken Oakeson)
 * Added BYTE to GetGdiInfo params
 * 
 * Revision 3.800  91/07/21  12:36:43  12:36:43  dtk (Doug Kaltenecker)
 * added checks for tt as raster for enableing tc_tt_able,
 * underlineing and strikeout
 * 
 * Revision 3.799  91/07/02  11:50:44  11:50:44  daniels (Susan Daniels)
 * Beta
 * 
 * Revision 3.797  91/07/01  16:31:10  16:31:10  dtk (Doug Kaltenecker)
 * added support for TT as graphics
 * 
 * Revision 3.796  91/06/26  11:25:03  11:25:03  stevec (Steve Claiborne)
 * BETA
 * 
 * Revision 3.791  91/06/13  09:02:16  09:02:16  stevec (Steve Claiborne)
 * Changed RC_GDI15_OUTPUT to RC_GDI20_OUTPUT per MS request.  SJC
 * 
 * Revision 3.790  91/06/11  16:02:19  16:02:19  stevec (Steve Claiborne)
 * Freeze
 * 
 * Revision 3.786  91/06/11  15:42:46  15:42:46  dtk (Doug Kaltenecker)
 * Prettified files!
 * 
* Revision 3.785  91/05/22  14:56:05  14:56:05  stevec (Steve Claiborne)
* Beta version to MS
*
* Revision 3.780  91/05/17  13:59:46  13:59:46  stevec (Steve Claiborne)
* *** empty log message ***
*
* Revision 3.776  91/05/17  10:55:14  10:55:14  daniels (Susan Daniels)
* Changing version number in GDIINFO to 3.1 (0x310).
*
* Revision 3.775  91/04/05  14:30:05  14:30:05  stevec (Steve Claiborne)
* Beta release to MS
*
* Revision 3.770  91/03/25  15:35:06  15:35:06  stevec (Steve Claiborne)
* maintance release
*
* Revision 3.760  91/03/12  07:51:43  07:51:43  stevec (Steve Claiborne)
* Maintance release
*
* Revision 3.755  91/03/03  07:45:10  07:45:10  stevec (Steve Claiborne)
* March 3 Freeze
*
* Revision 3.722  91/02/28  10:13:54  10:13:54  stevec (Steve Claiborne)
* Fixed bug #147 - GDIINFO struct 6 bytes too short.
*
* Revision 3.721  91/02/11  19:12:19  19:12:19  daniels (Susan Daniels)
* Adding ELI
*
* Revision 3.720  91/02/11  09:14:28  09:14:28  stevec (Steve Claiborne)
* Aldus version
*
* Revision 3.711  91/02/08  16:26:36  16:26:36  stevec (Steve Claiborne)
* Added debuging
*
* Revision 3.710  91/02/04  15:46:50  15:46:50  stevec (Steve Claiborne)
* Aldus freeze
*
* Revision 3.701  91/02/04  12:37:11  12:37:11  oakeson (Ken Oakeson)
* set epPubTrans to FALSE
*
* Revision 3.700  91/01/19  08:59:34  08:59:34  stevec (Steve Claiborne)
* Release
*
* Revision 3.685  91/01/14  15:42:22  15:42:22  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.681  91/01/14  10:16:44  10:16:44  stevec (Steve Claiborne)
* Updated the copy right stmt.
*
* Revision 3.680  91/01/10  16:15:50  16:15:50  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.670  90/12/14  14:53:16  14:53:16  stevec (Steve Claiborne)
* freeze for 12-14-90 ver. 3.670
*
* Revision 3.665  90/12/10  15:34:58  15:34:58  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.660  90/12/07  14:49:30  14:49:30  stevec (Steve Claiborne)
* Freeze 12-7-90
*
* Revision 3.650  90/11/30  08:11:24  08:11:24  stevec (Steve Claiborne)
* Freeze 3.650, 11-30-90
*
* Revision 3.604  90/11/19  08:51:46  08:51:46  tshannon (Terry Shannon)
* Tuned gray scaling for HP Laserjets.  Also added lighten gray scale
* button.  Terry Shannon 11-19-90
*
* Revision 3.603  90/10/25  17:17:18  17:17:18  oakeson (Ken Oakeson)
* Removed #ifdef from around truetype fields
*
* Revision 3.602  90/08/24  11:37:57  11:37:57  daniels (Susan Daniels)
* message.txt
*
* Revision 3.601  90/08/14  15:35:52  15:35:52  oakeson (Ken Oakeson)
* Added TrueType support
*
* Revision 3.600  90/08/03  11:08:52  11:08:52  stevec (Steve Claiborne)
* This is the Aug. 3 release ver. 3.600
*
* Revision 3.551  90/07/31  14:32:25  14:32:25  stevec (Steve Claiborne)
* Modified to allow user to turn on/off grayscaling
*
* Revision 3.540  90/07/25  12:33:21  12:33:21  stevec (Steve Claiborne)
* Experimental freeze 3.54
*
* Revision 3.522  90/07/21  10:55:11  10:55:11  stevec (Steve Claiborne)
* Removed PCL_* lock functions
* Addes support for huge bitmaps
* Added support for print direction
* Added support for DIBs
*
* Revision 3.520  90/06/13  16:51:27  16:51:27  root ()
* 5_2_release
*
*
*    Rev 1.3   09 May 1990 16:20:10   daniels
* Check ExtDeviceMode() for return value;  MS fix.  BUGFIX
*
*    Rev 1.3   09 May 1990 10:50:54   daniels
* Check ExtDeviceMode() return value.  Fix from MS went into 3.42.
*
*    Rev 1.2   20 Feb 1990 15:34:20   vordaz
* Support for downloadables.
*/
  
#include "generic.h"
#include "resource.h"
#define FONTMAN_ENABLE
#define FONTMAN_DISABLE
#include "fontman.h"
#include "strings.h"
#include "environ.h"
#include "utils.h"
#include "dump.h"             /*  LaserPort  */
#include "paper.h"
#include "truetype.h"
#include "version.h"
#include "build.h"

// Defined RC_GDI20_OUTPUT per Microsoft SJC 
#ifndef RC_GDI20_OUTPUT 
#define RC_GDI20_OUTPUT RC_GDI15_OUTPUT 
#endif 
  
DWORD FAR PASCAL GetFreeSpace(WORD);
  
#define GMEM_NOT_BANKED     0x1000
#define min(a,b)        ((a) < (b) ? (a) : (b))
  
#define USEEXTDEVMODE 1
  
#define LOCAL static
  
#define HIWORD(l)   ((WORD)(((DWORD)(l)>>16)&0xFFFF))
  
/*  Local debug structure.
*/
  
#ifdef DEBUG
    #define LOCAL_DEBUG
#endif
  
#ifdef LOCAL_DEBUG
    #define DBGsizeof(msg) DBMSG(msg)
    #define DBGentry(msg) DBMSG(msg)
    #define DBGenable(msg) DBMSG(msg)
#else
    #define DBGsizeof(msg) /*null*/
    #define DBGentry(msg) /*null*/
    #define DBGenable(msg) /*null*/
#endif
  
void PASCAL GetGdiInfo(GDIINFO FAR *, short, short, BYTE);
short FAR PASCAL ExtDeviceMode(HWND, HANDLE, LPPCLDEVMODE,
LPSTR, LPSTR, LPPCLDEVMODE, LPSTR, WORD);
  
/* need these to determine display driver abilities.
*/
  
HDC FAR PASCAL GetDC(HWND);
void FAR PASCAL ReleaseDC(HWND,HDC);
WORD FAR PASCAL GetDeviceCaps(HDC,WORD);
  
#define RASTERCAPS 38
  
#define RC_DIBTODEV 0x00200     /* device can do DIBs */
#define RC_STRETCHDIB   0x02000     /* device can do StretclBlt() */
  
GDIINFO GDIdefault = {
#if defined(WIN31)
    0x30A,       /* dpVersion */
#else
    0x300,       /* dpVersion */
#endif
    DT_RASPRINTER,                  /* devices classification */
    0,                              /* dpHorzSize = page width in millimeters */
    0,                              /* dpVertSize = page height in millimeters */
    0,                              /* dpHorzRes = pixel width of page */
    0,                              /* dpVertRes = pixel height of page */
    1,                              /* dpBitsPixel = bits per pixel */
    1,                              /* dpPlanes = # of bit planes */
    17,                             /* dpNumBrushes = # of brushes */
    2,                              /* dpNumPens = # of pens on the device */
    0,                              /* futureuse (not documented) */
    4,                              /* dpNumFonts = # of fonts for device */
    2,                              /* dpNumColors = # of colors in color tbl */
    0,                              /* dpDEVICEsize = size of device desciptor */
    CC_NONE,                        /* dpCurves = no curve capabilities */
    LC_NONE,                        /* dpLines = no line capabilities */
    //    LC_POLYLINE | LC_STYLED,  /* dpLines = styled lines */
    PC_SCANLINE,                    /* dpPolygonals = scanline capabilities */
    TC_OP_CHARACTER | TC_CR_90,     /* dpText */
    CP_RECTANGLE,                   /* dpClip = rectangle clipping */
    RC_STRETCHDIB | RC_DIBTODEV |
    RC_BITBLT | RC_BANDING |
    RC_SCALING | RC_GDI20_OUTPUT,
    XASPECT,                        /* dpAspectY = x major distance */
    YASPECT,                        /* dpAspectX = y major distance */
    XYASPECT,                       /* dpAspectXY = hypotenuse distance */
    MAXSTYLELEN,                    /* dpStyleLen = Len of segment for line style */
    { 254, 254 },                   /* dpMLoWin: tenths of millimeter in an inch */
    { HDPI, -VDPI },                /* dpMLoVpt: resolution in dots per inch */
    { 2540, 2540 },                 /* dpMHiWin: hundreths of millimeter in inch */
    { HDPI, -VDPI },                /* dpMHiVpt: resolution in dots per inch */
    { 100, 100 },                   /* dpELoWin: hundreths of an inch in an inch */
    { HDPI, -VDPI },                /* dpELoVpt: resolution in dots per inch */
    { 1000, 1000 },                 /* dpEHiWin: thousandths of inch in an inch */
    { HDPI, -VDPI },                /* dpEHiVpt: resolution in dots per inch */
    { 1440, 1440 },                 /* dpTwpWin: twips in an inch */
    { HDPI, -VDPI },                /* dpTwpVpt: resolution in dots per inch */
    HDPI,                           /* dpLogPixelsX */
    VDPI,                           /* dpLogPixelsY */
    DC_SPDevice,                    /* dpDCManage: 1 PDevice needed per file */
    0,                              /* dpCaps1: can do TT fonts through DDI or brute */
    0, 0, 0, 0,                     /* futureuse4 to futureuse 7 */
    0, 0, 0                         /* dpPalColors, dpPalReserved,
    dpPalResolution */
};
  
#if defined(DEBUG)
static void dumplpDevice(lpDevice)
LPDEVICE lpDevice;
{
    DBMSG(("lpDevice=%lp\n", lpDevice));
    DBMSG(("epType = %d\n", lpDevice->epType));
    DBMSG(("epBmpHdr = %p\n", lpDevice->epBmpHdr));
    DBMSG(("  epBmpHdr.bmType = %d\n", lpDevice->epBmpHdr.bmType));
    DBMSG(("  epBmpHdr.bmWidth = %d\n",
    (unsigned)lpDevice->epBmpHdr.bmWidth));
    DBMSG(("  epBmpHdr.bmHeight = %d\n",
    (unsigned)lpDevice->epBmpHdr.bmHeight));
    DBMSG(("  epBmpHdr.bmWidthBytes = %d\n",
    (unsigned)lpDevice->epBmpHdr.bmWidthBytes));
    DBMSG(("  epBmpHdr.bmPlanes = %d\n", (BYTE)lpDevice->epBmpHdr.bmPlanes));
    DBMSG(("  epBmpHdr.bmBits = %lp\n", lpDevice->epBmpHdr.bmBits));
    DBMSG(("  epBmpHdr.bmWidthPlanes = %ld\n",
    (unsigned long)lpDevice->epBmpHdr.bmWidthPlanes));
    DBMSG(("  epBmpHdr.bmlpBDevice = %lp\n", lpDevice->epBmpHdr.bmlpPDevice));
    /*  DBMSG(("  epBmpHdr.bmSegmentIndex = %d\n",
    (unsigned)lpDevice->epBmpHdr.bmSegmentIndex));
    DBMSG(("  epBmpHdr.bmScanSegment = %d\n",
    (unsigned)lpDevice->epBmpHdr.bmScanSegment));
    DBMSG(("  epBmpHdr.bmFillBytes = %d\n",
    (unsigned)lpDevice->epBmpHdr.bmFillBytes));
    */
    DBMSG(("  epBmpHdr.futureUse4 = %d\n",
    (unsigned)lpDevice->epBmpHdr.futureUse4));
    DBMSG(("  epBmpHdr.futureUse5 = %d\n",
    (unsigned)lpDevice->epBmpHdr.futureUse5));
    DBMSG(("epPF = %p\n", lpDevice->epPF));
    DBMSG(("ephDC = %d\n", lpDevice->ephDC));
    DBMSG(("epMode = %d\n", lpDevice->epMode));
    DBMSG(("epNband = %d\n", lpDevice->epNband));
    DBMSG(("epXOffset = %d\n", lpDevice->epXOffset));
    DBMSG(("epYOffset = %d\n", lpDevice->epYOffset));
    DBMSG(("epJob = %d\n", lpDevice->epJob));
    DBMSG(("epDoc = %d\n", lpDevice->epDoc));
    DBMSG(("epPtr = %d\n", (unsigned)lpDevice->epPtr));
    DBMSG(("epXerr = %d\n", lpDevice->epXerr));
    DBMSG(("epECtl = %d\n", lpDevice->epECtl));
    DBMSG(("epCurx = %d\n", lpDevice->epCurx));
    DBMSG(("epCury = %d\n", lpDevice->epCury));
    DBMSG(("epHFntSum = %d\n", lpDevice->epHFntSum));
}
#endif
  
short FAR PASCAL Enable(LPDEVICE,short,LPSTR,LPSTR,LPPCLDEVMODE);
int   FAR PASCAL Disable(LPDEVICE);
  
/*  Enable
*
*  GDI calls this proc when the application does a CreateIC or CreateDC.
*/
short FAR PASCAL
Enable(lpDevice, style, lpDeviceType, lpOutputFile, lpStuff)
LPDEVICE lpDevice;
short style;
LPSTR lpDeviceType;
LPSTR lpOutputFile;
LPPCLDEVMODE lpStuff;
{
    PAPERFORMAT pf;
    PCLDEVMODE tEnviron;
    extern HANDLE hLibInst;
    short iBandDepth;/* sjc - ms */
  
#ifdef DEBUG_FUNCT
    DB(("Entering Enable\n"));
#endif
    ProfSetup(128,0);  /* sjc - what? */
    ProfStart();/* sjc - ms */
  
    DBGentry(("************ ProtectMode=%d\n",ProtectMode));
    DBGentry(("Enable(%lp,%d,%lp,%lp,%lp)\n", lpDevice, style, lpDeviceType,
    lpOutputFile, lpStuff));
    DBGenable(("Enable(): lpDeviceType=%ls\n", lpDeviceType));
    DBGenable(("Enable(): lpOutputFile=%ls\n", lpOutputFile));
  
    #ifdef LOCAL_DEBUG
    DBGenable(("Enable(): style=%d", style));
    if (style & InquireInfo)
        DBGenable((", InquireInfo"));
    if (style & EnableDevice)
        DBGenable((", EnableDevice"));
    if (style & InfoContext)
        DBGenable((", InfoContext"));
    DBGenable(("\n"));
    #endif
  
    /*  If the caller has passed in an environment, make sure it's valid.
    *  If one wasn't passed in, then build one.
    */
  
#if USEEXTDEVMODE
    if (ExtDeviceMode((HWND) NULL, (HANDLE) NULL, (LPPCLDEVMODE) &tEnviron,
        lpDeviceType, (LPSTR) lpOutputFile, (LPPCLDEVMODE) lpStuff,
        (LPSTR) NULL, (WORD) DM_COPY | DM_MODIFY) < 0)
        return(FALSE);                  /* 5/9/90 BUGFIX */
    lpStuff = &tEnviron;
#else
    if (lpStuff) {
        if (lpStuff->dm.dmSize != sizeof(DEVMODE) ||
            lpStuff->dm.dmDriverVersion != VNUMint ||
            lpStuff->dm.dmSpecVersion != DM_SPECVERSION ||
            lpStuff->dm.dmDriverExtra != sizeof(PCLDEVMODE)-sizeof(DEVMODE) ||
        lstrcmpi((LPSTR)lpDeviceType, (LPSTR)lpStuff->dm.dmDeviceName)) {
            DBGenable(("Enable: returning FALSE, incomming "
            "environment invalid\n"));
            return FALSE;
        }
    }
    else
    {
#ifdef WIN31
        if (!GetEnvironment(lpDeviceType,(LPSTR)&tEnviron,sizeof(PCLDEVMODE)) ||
            lstrcmpi(lpDeviceType, tEnviron.dm.dmDeviceName))
        {
            MakeEnvironment(&tEnviron, lpDeviceType, lpOutputFile, NULL);
            SetEnvironment(lpDeviceType, (LPSTR)&tEnviron, sizeof(PCLDEVMODE));
        }
#else /* WIN30 */
        if (!GetEnvironment(lpOutputFile,(LPSTR)&tEnviron,sizeof(PCLDEVMODE)) ||
            lstrcmpi(lpDeviceType, tEnviron.dm.dmDeviceName))
        {
            MakeEnvironment(&tEnviron, lpDeviceType, lpOutputFile, NULL);
            SetEnvironment(lpOutputFile, (LPSTR)&tEnviron, sizeof(PCLDEVMODE));
        }
#endif /* WIN30 */
        lpStuff = &tEnviron;
    }
#endif
  
  
    /*  Read the paper format resource.
    */
    if (!GetPaperFormat(&pf, hLibInst, lpStuff->paperInd,
        lpStuff->dm.dmPaperSize,lpStuff->dm.dmOrientation))
        return FALSE;
  
    iBandDepth=DEF_BANDDEPTH;/* sjc - ms */
  
    global_grayscale=lpStuff->grayscale;
    global_brighten = lpStuff->brighten;
  
    DBGenable(("********** lpDevice->epgrayscale=%d\n",global_grayscale));
    DBGenable(("********** lpStuff->grayscale=%d\n",lpStuff->grayscale));
  
    if (style & InquireInfo) {
        unsigned cbPDevice;    /* sjc - ms */
  
        /*  fill in GDIInfo structure
        */
        /* added ttRaster to GetGdiInfo call for setting TC_TT_ABLE bit - dtk 7-12
         */
        GetGdiInfo((GDIINFO FAR *)lpDevice, lpStuff->dm.dmOrientation,
        lpStuff->prtCaps, lpStuff->TTRaster);
  
        /* give GDIINFO needed size of LPDEVICE structure
        */
        // sjc - ms
        if (style & InfoContext)
            cbPDevice = sizeof(DEVICEHDR);
        else
        {
            cbPDevice = sizeof(DEVICE);
            if (RealMode)
                cbPDevice += (unsigned)ComputeBandBitmapSize(NULL,&pf,
                lpStuff->prtResFac,lpStuff->dm.dmOrientation,iBandDepth);
            if (lpStuff->dm.dmOrientation == DMORIENT_LANDSCAPE)
                cbPDevice += MAX_BAND_WIDTH;
            if (lpStuff->prtCaps & HPLJIIP)
                cbPDevice += ComputeLineBufSize(&pf, lpStuff);
        }
  
  
        ((GDIINFO far *)lpDevice)->dpDEVICEsize = cbPDevice;
#if 0
        /* what used to here... ick!
        */
        (style & InfoContext) ?
        sizeof(DEVICEHDR) :
        // basic DEVICE struct size + size of band buffer +
        ( sizeof(DEVICE) + ComputeBandBitmapSize(&pf, lpStuff) +
        // size of transpose buffer for landscape +
        (lpStuff->dm.dmOrientation == DMORIENT_LANDSCAPE ?
        MAX_BAND_WIDTH : 0) +
        // size of scanline buffer for special printers
        (lpStuff->prtCaps & HPLJIIP ?
        ComputeLineBufSize(&pf, lpStuff) : 0) );
#endif
  
        //////
        DBGsizeof(("sizeof LPDEVICE structure %d\n",
        ((GDIINFO far *)lpDevice)->dpDEVICEsize));
        if (lpStuff->prtCaps & HPLJIIP)
        {
            DBGsizeof(("..It's a HP LJ II P printer..\n"));
            DBGsizeof(("  Scale factor prtResFac %d\n",
            lpStuff->prtResFac));
            DBGsizeof(("  sizeof epLineBuf in LPDEVICE %d\n",
            ComputeLineBufSize(&pf, lpStuff) ));
        }
        //////
  
        /* Give the paper size in millimeters (25.4 mm/inch)
        */
        ((GDIINFO far *)lpDevice)->dpHorzSize =
        (short)ldiv(labdivc((long)pf.xImage,(long)254,(long)HDPI),(long)10);
  
        ((GDIINFO far *)lpDevice)->dpVertSize =
        (short)ldiv(labdivc((long)pf.yImage,(long)254,(long)VDPI),(long)10);
  
        /*  Give the image area in device units, i.e. dots
        */
        ((GDIINFO far *)lpDevice)->dpHorzRes = pf.xImage;
        ((GDIINFO far *)lpDevice)->dpVertRes = pf.yImage;
  
        DBGenable(("dpHorzRes is %d, dpVertRes %d\n", pf.xImage, pf.yImage));
        DBGenable(("Enable(): ...returning INFO\n"));
  
        return sizeof (GDIINFO);
    }
  
    #ifdef LOCAL_DEBUG
  
    // show bit in PCLDEVMODE.prtCaps
    if (lpStuff->prtCaps & HPLJIIP)
        DBGsizeof(("--- PCLDEVMODE.prtCaps: printer is a HP LJ II P\n"));
    else
        DBGsizeof(("--- PCLDEVMODE.prtCaps: printer is NOT a HP LJ IIP\n"));
  
    #endif
  
    #ifdef LOCAL_DEBUGer
    DBGenable(("Enable(): prtCaps(%d)...\n", (WORD)lpStuff->prtCaps));
    if (lpStuff->prtCaps & HPJET)
        DBMSG(("   HPJET       printer has capabilities of a laserjet\n"));
    if (lpStuff->prtCaps & HPPLUS)
        DBMSG(("   HPPLUS      printer has capabilities of a laserjet plus\n"));
    if (lpStuff->prtCaps & HP500)
        DBMSG(("   HP500       printer has capabilities of a laserjet 500\n"));
    if (lpStuff->prtCaps & LOTRAY)
        DBMSG(("   LOTRAY      lower tray is handled\n"));
    if (lpStuff->prtCaps & NOSOFT)
        DBMSG(("   NOSOFT      printer does *not* support downloadable fonts\n"));
    if (lpStuff->prtCaps & NOMAN)
        DBMSG(("   NOMAN       manual feed is *not* supported\n"));
    if (lpStuff->prtCaps & NOBITSTRIP)
        DBMSG(("   NOBITSTRIP  printer cannot support internal bit stripping\n"));
    if (lpStuff->prtCaps & HPEMUL)
        DBMSG(("   HPEMUL      printer emulates an hplaserjet\n"));
    if (lpStuff->prtCaps & ANYDUPLEX)
        DBMSG(("   ANYDUPLEX printer can print duplex\n"));
    if (lpStuff->prtCaps & AUTOSELECT)
        DBMSG((
        "   AUTOSELECT  printer selects paper bin based on paper size (auto select)\n"));
    if (lpStuff->prtCaps & BOTHORIENT)
        DBMSG(("   BOTHORIENT  printer can print fonts in any orientation\n"));
    if (lpStuff->prtCaps & HPSERIESII)
        DBMSG(("   HPSERIESII  printer has capabilities of a Series II\n"));
    #endif
  
    /*  fill in LPDEVICE structure
    */
    DBGenable(("Initializing LPDEVICE\n"));
    lmemset((LPSTR)lpDevice, 0, sizeof (DEVICEHDR));
  
    lmemcpy((LPSTR)&lpDevice->epPF, (LPSTR)&pf, sizeof(PAPERFORMAT));
  
    iBandDepth=lpDevice->epBandDepth=DEF_BANDDEPTH;//Default-ALL modes
    // fill in epBuf -- offset of landscape buffer
    if (lpStuff->dm.dmOrientation == DMORIENT_LANDSCAPE)
    { // landscape: requires special buffer for transposed part of band
        lpDevice->epType = (short)DEV_LAND;
        if (!(style & InfoContext))
            lpDevice->epBuf = sizeof(DEVICE) +  // real mode: bits here
            ( RealMode ?
            (WORD)ComputeBandBitmapSize(lpDevice, &pf,
            lpStuff->prtResFac,
            lpStuff->dm.dmOrientation, iBandDepth)
            : 0 );   // protect mode: bits elsewhere
  
    #ifdef LOCAL_DEBUG
        if (!(style & InfoContext))
            DBGsizeof(("__(Landscape) offset of epBuf in DEVICE %d\n",
            lpDevice->epBuf ));
    #endif
    }
    else
    { // portrait: doesn't require buffer for transposed band.
        lpDevice->epType = (short)DEV_PORT;
        if (!(style & InfoContext))
            lpDevice->epBuf = 0;
  
    #ifdef LOCAL_DEBUG
        if (!(style & InfoContext))
            DBGsizeof(("__(Portrait) offset of epBuf in DEVICE %d\n",
            lpDevice->epBuf ));
    #endif
    }
  
    /*  Set up banding bitmap.  This proc requires
    *  epType and epPF to be set. sjc - ms
    */
    ComputeBandingParameters (lpDevice, lpStuff->prtResFac);
  
  
    // fill in epLineBuf -- offset to special scanline buffer.
  
    if (!(style & InfoContext))
    {
        lpDevice->epLineBuf =
        (lpStuff->prtCaps & HPLJIIP) ?  // needs scanline buffer?
        ((lpStuff->dm.dmOrientation == DMORIENT_LANDSCAPE) ?
        // Landscape: just after transpose buffer
        (lpDevice->epBuf) + MAX_BAND_WIDTH :
        // Portrait: just after banding buffer
        sizeof(DEVICE) +    // real mode: bits are here
        ( RealMode ?
        (WORD)ComputeBandBitmapSize(lpDevice, &pf,
        lpStuff->prtResFac,
        lpStuff->dm.dmOrientation,iBandDepth)
        : 0 )    // protect mode: bits elsewhere
        )  :
        0;              // no scanline buffer.
  
        DBGsizeof(("__Offset of epLineBuf in DEVICE %d\n",
        lpDevice->epLineBuf ));
    }
  
    /*convert KB to bytes*/
    lpDevice->epAvailMem = lmul((long)lpStuff->availmem, ((long)1 << 10));
  
    if (lpStuff->dm.dmPrintQuality == DMRES_DRAFT)
        lpDevice->epMode |= DRAFTFLAG;
  
    /* Initialize TrueType values */
    lpDevice->epCurTTFont = -1;
    lpDevice->epNextTTFont = TTBASE;
    lpDevice->epTTRaster = lpStuff->TTRaster; /* 6-19 dtk */
    lpDevice->epTTFSum = NULL;
    lpDevice->epFontBmpMem = NULL;
  
    lpDevice->epFreeMem = lpDevice->epAvailMem;
    lpDevice->epScaleFac = lpStuff->prtResFac;
    lpDevice->epCopies = lpStuff->dm.dmCopies;
    lpDevice->epTray = lpStuff->dm.dmDefaultSource;
    lpDevice->epPaper = lpStuff->dm.dmPaperSize;
    lpDevice->epPgChng = FALSE;                     /* Added for BUG #560 */
  
    lpDevice->epPubTrans = FALSE;
  
    lpDevice->epMaxPgSoft = lpStuff->maxPgSoft;
    lpDevice->epMaxSoft = lpStuff->maxSoft;
    lpDevice->epCaps = lpStuff->prtCaps;
    lpDevice->epCaps2 = lpStuff->prtCaps2;            /*Tetra II*/
    lpDevice->epRearTray = lpStuff->reartray;        /* ELI */
    lpDevice->epOffset = lpStuff->offset;                /* ELI */
    lpDevice->epDuplex = lpStuff->dm.dmDuplex;
    lpDevice->epTxWhite = lpStuff->txwhite;
    lpDevice->epOptions = lpStuff->options;
    lpDevice->epJust = fromdrawmode;
  
    /*  Turn off the options bit for the DP-TEK LaserPort
    *  if the card is not present.
    */
#ifdef VISUALEDGE
    if ((lpDevice->epOptions & OPTIONS_DPTEKCARD) && !lp_enbl()) /* LaserPort */
  
        // if VISUALEDGE isn't defined, ALWAYS turn this bit off.
  
#endif
  
        lpDevice->epOptions &= ~(OPTIONS_DPTEKCARD);
  
  
    /*  Force soft fonts, even to a standard laserjet, if the
    *  option bit is set.  This exists so users can load up
    *  PFM files for cartridge fonts on their standard laserjet.
    */
    if (lpDevice->epOptions & OPTIONS_FORCESOFT)
        lpDevice->epCaps &= ~(NOSOFT);
  
    if (lpOutputFile)
    {
        /* TETRA -- changed lmemcpy to lstrncpy -- KLO */
        lstrncpy(lpDevice->epPort, lpOutputFile, NAME_LEN);
        lpDevice->epPort[NAME_LEN-1] = '\0';
    }
    else
        LoadString(hLibInst, NULL_PORT, (LPSTR)lpDevice->epPort, NAME_LEN);
  
    /* TETRA -- changed lmemcpy to lstrncpy -- KLO */
    lstrncpy(lpDevice->epDevice, lpDeviceType, NAME_LEN);
    lpDevice->epDevice[NAME_LEN-1] = '\0';
  
    lpDevice->epLPFntSum = 0L;
    lpDevice->epHWidths = 0;
  
    if (lpDevice->epHFntSum =
        GetFontSummary(lpOutputFile, lpDeviceType, lpStuff, hLibInst))
    {
        DBGenable(("Enable(): ...returning ENABLED\n"));
        return TRUE;
    }
    else
    {
        DBGenable(("Enable(): ...returning *not* ENABLED\n"));
        return FALSE;
    }
#ifdef DEBUG_FUNCT
    DB(("Exiting Enable\n"));
#endif
}
  
/*  Disable
*/
far PASCAL Disable(lpDevice)
LPDEVICE lpDevice;
{
#ifdef DEBUG_FUNCT
    DB(("Entering Disable\n"));
#endif
    DBGentry(("Disable(%lp)\n", lpDevice));
  
#ifdef VISUALEDGE
    if (lpDevice->epOptions & OPTIONS_DPTEKCARD)
        lp_disable();                   /* LaserPort */
#endif
  
    if (lpDevice->epHWidths)
    {
        GlobalFree(lpDevice->epHWidths);
        lpDevice->epHWidths = 0;
    }
  
    if (ProtectMode && HIWORD(lpDevice->epBmpHdr.bmBits))
        GlobalFree(HIWORD(lpDevice->epBmpHdr.bmBits));
  
    FreeFontSummary(lpDevice);
  
    ProfStop();
    ProfFinish();
    ProfFlush();
  
#ifdef DEBUG_FUNCT
    DB(("Entering Disable\n"));
#endif
    return TRUE;
}
  
  
/*  GetGdiInfo
*
*  Get the default GDIINFO structure.

*  dtk - added ttRaster to GetGdiInfo call for 
*  setting TC_TT_ABLE bit - 7-12
*/
void PASCAL GetGdiInfo(lpdp, orient, prtCaps, ttRaster)
GDIINFO FAR *lpdp;
short orient;
short prtCaps;
BYTE ttRaster;
{
    HDC hdcScreen;
  
#ifdef DEBUG_FUNCT
    DB(("Entering GetGdiInfo\n"));
#endif
    lmemcpy((LPSTR)lpdp, (LPSTR) &GDIdefault, sizeof (GDIINFO));
  
  
    /* determine if the display driver (ie, the brute routines)
    * will support bitmaps larger than 64k.  If so, we'll set the
    * bit for it.
    */
    hdcScreen = GetDC(NULL);
    if (GetDeviceCaps(hdcScreen,RASTERCAPS) & RC_BITMAP64)
        lpdp->dpRaster |= RC_BITMAP64;
    ReleaseDC(NULL,hdcScreen);
  
    if (orient == DMORIENT_LANDSCAPE)
    {
        /*  reverse necessary horizontal and vertical fields
        */
        PTTYPE far *iptr;
        short temp, index;
  
        if (HDPI != VDPI)
        {
            /*  exchange x and y values
            */
            temp = lpdp->dpAspectX;
            lpdp->dpAspectX = lpdp->dpAspectY;
            lpdp->dpAspectY = temp;
        }
  
        /*  exchange x and y settings for page measurement values
        *  invert 1 <--> 2, 3 <--> 4 for landscape
        */
  
        /*  exchange x and y values
        */
        for (iptr = &(lpdp->dpMLoWin), index = 0;
            index < 10;
            iptr++, index++)
        {
            if (iptr->ycoord >= 0)
            {
                temp = iptr->ycoord;
                iptr->ycoord = iptr->xcoord;
                iptr->xcoord = temp;
            }
            else
            {
                temp = -iptr->ycoord;
                iptr->ycoord = -iptr->xcoord;
                iptr->xcoord = temp;
            }
        }
    }
  

#ifdef WIN31

    /* if the checkbox is checked, turn on tc_tt_able - dtk
     */
    if (ttRaster)
        lpdp->dpCaps1 = TC_TT_ABLE;

#else

    lpdp->dpText |= TC_SO_ABLE;
    lpdp->dpText |= TC_UA_ABLE;

#endif


#ifdef DEBUG_FUNCT
    DB(("Exiting GetGdiInfo\n"));
#endif
}
