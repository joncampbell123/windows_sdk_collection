/**[f******************************************************************
* device.h -
*
* Copyright (C) 1988,1989 Aldus Corporation
* Copyright (C) 1988-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
*             All rights reserved.
* Company confidential.
*
**f]*****************************************************************/
  
/*********************************************************************
*  18 dec 91  SD       New definition of INTENSITY from MS to fix #737
*                      along with change to ColorInfo in stubs.c.
*  20 sep 91  SD       Added epPgChng and epBandDepth to DEVICEHDR.  
*                      Removed epByteFill from DEVICE to force epBmp to
*                      word align.
*  15 aug 91  SD       BUG 560:  Added BYTE field epPgChng to LPDEVICE to 
*                      monitor changes to orientation, paper tray, and paper 
*                      size that may occur within a document.
*  23 jan 91  SD       ELI - Added epRearTray and epOffset to DEVICEHDR and
*                            DEVICE.
*  30 jan 90  VO       Added epLastSize to DEVICEHDR and LPDEVICE
*                      to monitor the size of the last selected font.
*  18 dec 89  SD       Added second printer capabilities field to
*                      DEVICEHDR and LPDEVICE structures.
*
*  05 sep 89   peterbe Bring some comments up to date.
*          Added epLineBuf.
*  17 apr 89   peterbe Change tabs, cleanup.
*   1-18-89    jimmat  Now space for epBuf is only allocated if the printer
*          is in landscape mode.
*/
  
/*
* $Header: device.h,v 3.890 92/02/06 16:13:27 dtk FREEZE $
*/
  
/*
* $Log:	device.h,v $
 * Revision 3.890  92/02/06  16:13:27  16:13:27  dtk (Doug Kaltenecker)
 * Win3.1 Freeze
 * 
 * Revision 3.871  91/12/18  17:51:09  17:51:09  daniels (Susan Daniels)
 * Fix #737:  New definition of INTENSITY from MS.
 * 
 * Revision 3.870  91/11/08  11:45:16  11:45:16  dtk (Doug Kaltenecker)
 * 3.1 Release Candidate 1
 * 
 * Revision 3.865  91/11/01  13:53:14  13:53:14  dtk (Doug Kaltenecker)
 * Beta release for Windows 3.1
 * 
 * Revision 3.862  91/10/25  13:48:33  13:48:33  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.860  91/10/23  09:50:09  09:50:09  dtk (Doug Kaltenecker)
 * WinWird Release
 * 
 * Revision 3.852  91/10/09  15:01:01  15:01:01  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.850  91/10/04  16:51:18  16:51:18  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE
 * 
 * Revision 3.840  91/09/28  14:18:31  14:18:31  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE 3 FOR WINWORD
 * 
 * Revision 3.830  91/09/24  13:27:45  13:27:45  dtk (Doug Kaltenecker)
 * Took out the SCALABLEFONTINFO definition.
 * 
 * Revision 3.830  91/09/20  15:21:42  15:21:42  daniels (Susan Daniels)
 * Made DEVICE and DEVICEHDR agree.  Removed ByteFill.
 * 
 * Revision 3.830  91/09/18  16:34:51  16:34:51  dtk (Doug Kaltenecker)
 * RELEASE
 * 
 * Revision 3.822  91/09/16  10:36:02  10:36:02  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE 2 FOR WIN30.
 * 
 * Revision 3.820  91/09/06  14:13:32  14:13:32  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE FOR WINWORD 2.0
 * 
 * Revision 3.812  91/08/22  14:33:24  14:33:24  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.808  91/08/16  13:10:25  13:10:25  daniels (Susan Daniels)
 * Fix BUG 560: Eli printing too slow.  Add epPgChng field to DEVICE.
 * 
 * Revision 3.807  91/08/08  10:32:40  10:32:40  dtk (Doug Kaltenecker)
 * PREBETA3
 * 
 * Revision 3.802  91/07/22  12:55:49  12:55:49  oakeson (Ken Oakeson)
 * BETA
 * 
 * Revision 3.800  91/07/21  13:04:14  13:04:14  dtk (Doug Kaltenecker)
 * added lpscalablefontinfo 
 * 
 * Revision 3.799  91/07/02  11:53:10  11:53:10  daniels (Susan Daniels)
 * Beta
 * 
 * Revision 3.797  91/07/01  16:24:19  16:24:19  dtk (Doug Kaltenecker)
 * added fields to lpdevice for TT as graphics and TT memory tracking
 * 
 * Revision 3.790  91/06/11  16:04:44  16:04:44  stevec (Steve Claiborne)
 * Freeze
 * 
 * Revision 3.786  91/06/11  15:46:04  15:46:04  dtk (Doug Kaltenecker)
 * Prettified files!
 * 
* Revision 3.785  91/05/22  14:58:21  14:58:21  stevec (Steve Claiborne)
* Beta version to MS
*
* Revision 3.780  91/05/15  15:58:29  15:58:29  stevec (Steve Claiborne)
* Beta
*
* Revision 3.775  91/04/05  14:32:22  14:32:22  stevec (Steve Claiborne)
* Beta release to MS
*
* Revision 3.770  91/03/25  15:37:21  15:37:21  stevec (Steve Claiborne)
* maintance release
*
* Revision 3.761  91/03/20  15:31:59  15:31:59  stevec (Steve Claiborne)
* Fixed bug #168 - tick marks showing up on right side of paper in
* real mode.
*
* Revision 3.760  91/03/12  07:54:10  07:54:10  stevec (Steve Claiborne)
* Maintance release
*
* Revision 3.755  91/03/03  07:47:26  07:47:26  stevec (Steve Claiborne)
* March 3 Freeze
*
* Revision 3.721  91/02/11  19:10:29  19:10:29  daniels (Susan Daniels)
* Adding ELI
*
* Revision 3.720  91/02/11  09:16:44  09:16:44  stevec (Steve Claiborne)
* Aldus version
*
* Revision 3.710  91/02/04  15:48:57  15:48:57  stevec (Steve Claiborne)
* Aldus freeze
*
* Revision 3.701  91/02/04  12:34:51  12:34:51  oakeson (Ken Oakeson)
* Added epPubTrans for extra WN chars
*
* Revision 3.700  91/01/19  09:01:39  09:01:39  stevec (Steve Claiborne)
* Release
*
* Revision 3.685  91/01/14  15:44:37  15:44:37  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.681  91/01/14  10:18:50  10:18:50  stevec (Steve Claiborne)
* Updated the copy right stmt.
*
* Revision 3.680  91/01/10  16:17:57  16:17:57  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.670  90/12/14  14:55:29  14:55:29  stevec (Steve Claiborne)
* freeze for 12-14-90 ver. 3.670
*
* Revision 3.665  90/12/10  15:37:08  15:37:08  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.660  90/12/07  14:51:38  14:51:38  stevec (Steve Claiborne)
* Freeze 12-7-90
*
* Revision 3.650  90/11/30  08:13:28  08:13:28  stevec (Steve Claiborne)
* Freeze 3.650, 11-30-90
*
* Revision 3.603  90/10/25  17:12:09  17:12:09  oakeson (Ken Oakeson)
* Removed #ifdef for truetype fields
*
* Revision 3.602  90/08/24  13:21:01  13:21:01  daniels (Susan Daniels)
* ../message.txt
*
* Revision 3.601  90/08/14  15:24:43  15:24:43  oakeson (Ken Oakeson)
* Added TrueType support
*
* Revision 3.600  90/08/03  11:11:13  11:11:13  stevec (Steve Claiborne)
* This is the Aug. 3 release ver. 3.600
*
* Revision 3.550  90/07/27  11:32:29  11:32:29  root ()
* Experimental freeze 3.55
*
* Revision 3.540  90/07/25  12:36:56  12:36:56  stevec (Steve Claiborne)
* Experimental freeze 3.54
*
* Revision 3.521  90/07/21  13:42:02  13:42:02  stevec (Steve Claiborne)
* Added support for DIB's
*
* Revision 3.520  90/06/13  16:54:18  16:54:18  root ()
* 5_2_release
*
*
*    Rev 1.1   20 Feb 1990 15:47:28   vordaz
* Support for downloadables.
*/
  
/* hplaserjet's own device.h */
#define BLOCK_SIZE  512
#define LINE_LEN    80
  
#define DEV_PORT    0x8888
#define DEV_LAND    0x8889
  
#define DRAFTFLAG   0x01
#define BREAKFLAG   0x02
#define GRXFLAG     0x04    /* flag to see if any graphics output to the buffer */
#define LOWRES      0x08
#define TEXTFLAG    0x10    /* text output to the buffer */
#define INFO        0x20
#define ANYGRX      0x40    /* any graphics in the entire page */
  
  
typedef TEXTXFORM far * LPTEXTXFORM;
typedef LOGFONT   far * LPLOGFONT;
typedef FONTINFO  far * LPFONTINFO;
typedef DRAWMODE  far * LPDRAWMODE;
typedef TEXTMETRIC far * LPTEXTMETRIC;
// moved to physical.c for win31 
//typedef SCALABLEFONTINFO  far * LPSCALABLEFONTINFO;
  
//      Font weights lightest to darkest.
#define FW_DONTCARE     0
#define FW_THIN         100
#define FW_EXTRALIGHT       200
#define FW_LIGHT        300
#define FW_NORMAL       400
#define FW_MEDIUM       500
#define FW_SEMIBOLD     600
#define FW_BOLD         700
#define FW_EXTRABOLD        800
#define FW_HEAVY        900
  
#define FW_ULTRALIGHT       FW_EXTRALIGHT
#define FW_REGULAR      FW_NORMAL
#define FW_DEMIBOLD     FW_SEMIBOLD
#define FW_ULTRABOLD        FW_EXTRABOLD
#define FW_BLACK        FW_HEAVY
  
BOOL FAR PASCAL TextOut(HANDLE, short, short, LPSTR, short);
  
int     FAR PASCAL SetRect(LPRECT, int, int, int, int);
int     FAR PASCAL SetRectEmpty(LPRECT);
int     FAR PASCAL CopyRect(LPRECT, LPRECT);
int     FAR PASCAL InflateRect(LPRECT, int, int);
int     FAR PASCAL IntersectRect(LPRECT, LPRECT, LPRECT);
int     FAR PASCAL UnionRect(LPRECT, LPRECT, LPRECT);
int     FAR PASCAL OffsetRect(LPRECT, int, int);
BOOL        FAR PASCAL IsRectEmpty(LPRECT);
BOOL        FAR PASCAL PtInRect(LPRECT, LPPOINT);
  
  
  
/* heap structure :
  
base                                                    base + start
|                                                            |
\|/                                                          \|/
string chars (ELEMENT)--->                              index ---->
*/
  
typedef struct
{
    FONTINFO fontInfo;
    short indFontSummary;   /* ind of selected font */
    SYMBOLSET symbolSet;    /* symbol set: USASCII or Roman 8 */
    BOOL ZCART_hack;        /* Z cartridge hack */
    BOOL QUOTE_hack;        /* Typographic quotes hack */
    /*** Tetra ***/
    SCALEINFO scaleInfo;   /* scalable font flag structure */
    BOOL isextpfm;      /* TRUE if the PFM is not in a resource */
} PRDFONTINFO, far * LPPRDFONTINFO;
  
typedef struct {
    char InitStyleError;
    char Hypoteneuse;
    char XMajorDistance;
    char YMajorDistance;
} ASPECT;
  
typedef enum {
    fromdrawmode,
    justifywordbreaks,
    justifyletters
} JUSTBREAKTYPE;
  
typedef struct {
    short extra;
    short rem;
    short err;
    WORD count;
    WORD ccount;
} JUSTBREAKREC;
  
typedef JUSTBREAKTYPE FAR *LPJUSTBREAKTYPE;
typedef JUSTBREAKREC FAR *LPJUSTBREAKREC;
  
  
/*****************************************************************/
/*  *** REMEMBER *** IF YOU PUT IT HERE, PUT IT IN DEVICE.I TOO ***
*/
  
typedef struct {
    short epType;           /* DEV_LAND means landscape device
                               DEV_PORT means portrait device */
    BITMAP epBmpHdr;        /* bitmap structure */
    PAPERFORMAT epPF;
    short ephDC;            /* apps's callback abort proc */
    short epMode;           /* draft mode */
    short epNband;          /* nth band */
    short epXOffset;
    short epYOffset;
    short epJob;            /* job number */
    short epDoc;            /* current status of the document */
    unsigned epPtr;         /* spool buffer pointer */
    short epXerr;           /* running round off err */
    short epYerr;           /* running round off err */
    short ephMd;            /* module handle */
    short epECtl;           /* last escape control selected */
/*** Tetra II begin ***/
    short epLastSize;       /* size of last selected font */
/*** Tetra II end ***/
  
    /* TrueType support fields */
    short epCurTTFont;      /* current TT font id */
    short epNextTTFont;     /* next ID to assign */
    BYTE epTTRaster;        /* for TT as raster 6-19 dtk */
    HANDLE epTTFSum;        /* handle to TrueType font summary */
    HANDLE epFontBmpMem;    /* handle of font bitmap memory */
  
    short epCurx;
    short epCury;
    short epBandDepth;      /* height of band in scan lines */
    short epNumBands;       /* number of print bands on page */
    short epLastBandSz;     /* size of last band (does not have to be 64) */
    short epScaleFac;       /* 0=300 dpi ptr res, 1= 150, 2 = 75 */
    short epCopies;         /* number of copies */
    short epTray;           /* chosen tray */
    short epPaper;          /* chosen paper size:  20=LETTER, 21=DINA4,
                               22=LEGAL, 23=B5, 24=EXEC, 25=A3, 26=LEDGER */
    BYTE epPgChng;          /* 0=pg setup has not changed; 1=change */
    BYTE epPubTrans;        // TRUE = add pub chars to WN symbol set (sigh)
    BYTE epFontSub;         /* set if warning message has already been given
                               for substituted fonts on the page*/
    short epPgSoftNum;      /* number of soft fonts used on current page */
    short epTotSoftNum;     /* # soft fonts downloaded, incl. permanent */
    short epMaxPgSoft;      /* maximum soft fonts per page */
    short epMaxSoft;        /* maximum soft fonts */
    BYTE epGDItext;         /* set if GDI simulates text attributes */
    BYTE epOpaqText;        /* set if we got an opaque rectangle */
    RECT epGrxRect;         /* enclosing rect for page graphics */
    BYTE epRearTray;        /* ELI 0=no, 1=yes;  use rear tray for output */
    BYTE epOffset;          /* ELI 0=no, 1=yes;  offset output for this job */
    short epCaps;           /* printer capabilites */
    short epCaps2;          /* second set of printer capabilities */
    short epOptions;        /* options bits from options dialog */
    short epDuplex;         /* 0=no duplex, 1=duplex printing */
    short epPageCount;
    long epAvailMem;        /* initial available memory at reset time */
    long epFreeMem;
    short epTxWhite;        /* white text intensity */
    HANDLE epHFntSum;       /* handle fontSummary struct */
    LPSTR epLPFntSum;       /* locked pointer fontSummary struct */
    JUSTBREAKTYPE epJust;   /* kind of justification */
    JUSTBREAKREC epJustWB;  /* justification rec for word breaks */
    JUSTBREAKREC epJustLTR; /* justification rec for letters */
    HANDLE epHWidths;       /* widths for ExtTextOut() */
    char epDevice[NAME_LEN];
    char epPort[NAME_LEN];
} DEVICEHDR;
  
  
// NOTE:  This data structure (LPDEVICE) is duplicated in "device.i" for
//        use by "dumputil.a" and "lasport.a".  ANY changes to this
//    structure should also be made in "device.i".
//
// NOTE 2: IMPORTANT - when adding fields to this structure, you MUST ensure
//        that epBmp is on a word boundary.  This can be arrived at by adding
//        up the number of bytes associated with each field element, or can
//        be guessed at by assuming the structure is correct and compensating
//        for ONLY the new fields.  If epBmp is on an odd byte boundary,
//        simply uncomment the epByteFill field.  If epBmp is on an even byte
//        boundary, comment out the epByteFill field.  The symptom of incorrect
//        boundary calculation is printing "tick marks" on the right hand side
//        of a page in real mode at all DPI's.  Do exactly the same changes to
//        device.i.
  
typedef struct {
    short epType;           /* DEV_LAND means landscape device
                               DEV_PORT means portrait device */
    BITMAP epBmpHdr;        /* bitmap structure */
    PAPERFORMAT epPF;
    short ephDC;            /* apps's callback abort proc */
    short epMode;           /* draft mode */
    short epNband;          /* nth band */
    short epXOffset;
    short epYOffset;
    short epJob;            /* job number */
    short epDoc;            /* current status of the document */
    unsigned epPtr;         /* spool buffer pointer */
    short epXerr;
    short epYerr;
    short ephMd;            /* module handle */
    short epECtl;           /* last escape control selected */
/*** Tetra II begin ***/
    short epLastSize;       /* size of the last selected font */
/*** Tetra II end ***/
  
    /* TrueType support fields */
    short epCurTTFont;      /* current TT font id */
    short epNextTTFont;     /* next ID to assign */
    BYTE epTTRaster;        /* for TT as raster 6-19 dtk */
    HANDLE epTTFSum;        /* handle to TrueType font summary */
    HANDLE epFontBmpMem;    /* handle of font bitmap memory */

  
    short epCurx;
    short epCury;
                            // sjc - ms 7-90
    short epBandDepth;      // height of band in scan lines  3/30/90 t-bobp
    short epNumBands;       /* number of print bands on page */
    short epLastBandSz;     /* size of last band (does not have to be 64) */
    short epScaleFac;       /* 0=300 dpi ptr res, 1= 150, 2 = 75 */
    short epCopies;         /* number of copies */
    short epTray;
    short epPaper;          // chosen paper size:  20=LETTER, 21=DINA4,
                            // 22=LEGAL, 23=B5, 24=EXEC, 25=A3, 26=LEDGER
    BYTE epPgChng;          // 0=page setup has not changed; 
                            // 1=page setup has changed.
    BYTE epPubTrans;        // TRUE = add pub chars to WN symbol set (sigh)
    BYTE epFontSub;         // set if warning message has already been given
                            // for substituted fonts on the page
    short epPgSoftNum;      /* number of soft fonts used on current page */
    short epTotSoftNum;     // # soft fonts downloaded, including permanent
    short epMaxPgSoft;      /* maximum soft fonts per page */
    short epMaxSoft;        /* maximum soft fonts */
    BYTE epGDItext;         /* set if GDI simulates text attributes */
    BYTE epOpaqText;        /* set if we got an opaque rectangle */
    RECT epGrxRect;         /* enclosing rect for page graphics */
    BYTE epRearTray;        /*  ELI -- 0=no, 1=yes for use rear tray for output */
    BYTE epOffset;          /*  ELI -- 0=no, 1=yes for offset this job */
    short epCaps;           /* printer capabilites */
    short epCaps2;          /* second set of printer capabilities */
    short epOptions;        /* options bits from options dialog */
    short epDuplex;         /* 0=no duplex, 1=duplex printing */
    short epPageCount;
                            /*memory usage variables*/
    long epAvailMem;        /* initial available memory at reset time */
    long epFreeMem;
    short   epTxWhite;      /* white text intensity */
    HANDLE epHFntSum;       /* handle fontSummary struct */
    LPSTR epLPFntSum;       /* locked pointer fontSummary struct */
    JUSTBREAKTYPE epJust;   /* kind of justification */
    JUSTBREAKREC epJustWB;  /* justification rec for word breaks */
    JUSTBREAKREC epJustLTR; /* justification rec for letters */
    HANDLE epHWidths;       /* widths for ExtTextOut() */
    char epDevice[NAME_LEN];
    char epPort[NAME_LEN];
    char epSpool[SPOOL_SIZE];
    short epBuf;            // OFFSET in DEVICE struct to landscape mode
                            // buffer (transposed slice of epBmp[]).
    short epLineBuf;        // OFFSET in DEVICE struct to special graphics
                            // buffer for 1 scanline.
                            // sjc - ms 7-90
//    char epByteFill;
    char epBmp[1];          /* size of bitmap determined at run time */
} DEVICE, far *LPDEVICE;
  
DWORD FAR PASCAL dmColorInfo(LPDEVICE lpDevice, DWORD rgb, DWORD FAR *lpco);
int FAR PASCAL  dmOutput(LPDEVICE, short, short, LPPOINT, void FAR *, void FAR *, LPDRAWMODE, LPRECT);
int far PASCAL  dmBitblt(LPDEVICE, short, short, BITMAP FAR *, short, short, short, short, DWORD, void FAR *, LPDRAWMODE);
int far PASCAL  dmPixel(LPDEVICE, short, short, DWORD, LPDRAWMODE);
  
far PASCAL  dmEnumDFonts(LPDEVICE, long, long, long);
far PASCAL  dmEnumObj(LPDEVICE, short, long, long);
far PASCAL  dmRealizeObject(LPDEVICE, short, LPSTR, LPSTR, LPSTR);
LONG far PASCAL  dmStrBlt(LPDEVICE, short, short, LPRECT, LPSTR, short, LPFONTINFO, LPDRAWMODE, LPTEXTXFORM);
far PASCAL  dmScanLR(LPDEVICE, short, short, long, short);
far PASCAL  dmTranspose(LPSTR, LPSTR, short);
// sjc - ms 7-90
#define INTENSITY(r,g,b)    (BYTE)(((WORD)((r)<<2)+(r)+(WORD)((g)<<3)+(g)+(WORD)((b)<<1))>>4)  
//#define INTENSITY(r,g,b)    (BYTE)(((WORD)((r) * 30) + (WORD)((g) * 59) + (WORD)((b) * 11))/100)
#define RGB(r,g,b)      ((DWORD)(((BYTE)(r)|((WORD)(g)<<8))|(((DWORD)(BYTE)(b))<<16)))
#define GetRValue(rgb)      ((BYTE)(rgb))
#define GetGValue(rgb)      ((BYTE)(((WORD)(rgb)) >> 8))
#define GetBValue(rgb)      ((BYTE)((rgb)>>16))
  
  
#define BRUSH_XSIZE 16  // note, scanline.asm assumes these dimensions
#define BRUSH_YSIZE 16
  
typedef struct {
    BYTE    Pattern[BRUSH_XSIZE / 8][BRUSH_YSIZE];
    BOOL    bMyBrush;       // marker, this is ours
} PBRUSH, FAR *LPPBRUSH;
  
  
/* Binary raster ops (Rop2 from DRAWMODE) */
#define R2_BLACK        1   /*  0       */
#define R2_NOTMERGEPEN      2   /* DPon     */
#define R2_MASKNOTPEN       3   /* DPna     */
#define R2_NOTCOPYPEN       4   /* PN       */
#define R2_MASKPENNOT       5   /* PDna     */
#define R2_NOT          6   /* Dn       */
#define R2_XORPEN       7   /* DPx      */
#define R2_NOTMASKPEN       8   /* DPan     */
#define R2_MASKPEN      9   /* DPa      */
#define R2_NOTXORPEN        10  /* DPxn     */
#define R2_NOP          11  /* D        */
#define R2_MERGENOTPEN      12  /* DPno     */
#define R2_COPYPEN      13  /* P        */
#define R2_MERGEPENNOT      14  /* PDno     */
#define R2_MERGEPEN     15  /* DPo      */
#define R2_WHITE        16  /*  1       */
  
  
  
/*  Ternary raster operations */
#define SRCCOPY         (DWORD)0x00CC0020 /* dest = source           */
#define SRCPAINT        (DWORD)0x00EE0086 /* dest = source OR dest       */
#define SRCAND          (DWORD)0x008800C6 /* dest = source AND dest      */
#define SRCINVERT       (DWORD)0x00660046 /* dest = source XOR dest      */
#define SRCERASE        (DWORD)0x00440328 /* dest = source AND (NOT dest )   */
#define NOTSRCCOPY      (DWORD)0x00330008 /* dest = (NOT source)         */
#define NOTSRCERASE     (DWORD)0x001100A6 /* dest = (NOT src) AND (NOT dest) */
#define MERGECOPY       (DWORD)0x00C000CA /* dest = (source AND pattern)     */
#define MERGEPAINT      (DWORD)0x00BB0226 /* dest = (NOT source) OR dest     */
#define PATCOPY         (DWORD)0x00F00021 /* dest = pattern          */
#define PATPAINT        (DWORD)0x00FB0A09 /* dest = DPSnoo           */
#define PATINVERT       (DWORD)0x005A0049 /* dest = pattern XOR dest     */
#define DSTINVERT       (DWORD)0x00550009 /* dest = (NOT dest)       */
#define BLACKNESS       (DWORD)0x00000042 /* dest = BLACK            */
#define WHITENESS       (DWORD)0x00FF0062 /* dest = WHITE            */
  
extern int display_pbrush_size;
  
// 8x8 53 lpi 45 deg dot from Urchley (classic 45 deg halftone)
  
#define DX_CLUSTER 8
#define DY_CLUSTER 8
#define SCREEN dot_8x8_45
#define SCREENX(x)  (x & 0x07)
#define SCREENY(y)  (y & 0x07)
#define max(a,b)        ((a) > (b) ? (a) : (b))
#define min(a,b)        ((a) < (b) ? (a) : (b))
  
extern BYTE bit_index[];
extern BYTE dot_8x8_45[DX_CLUSTER][DY_CLUSTER];
