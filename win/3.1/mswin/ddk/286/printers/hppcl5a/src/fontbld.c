/**[f******************************************************************
* fontbld.c -
*
* Copyright (C) 1988,1989 Aldus Corporation.
* Copyright (C) 1988-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.  Company confidential.
*
**f]*****************************************************************/
  
/*
* $Header: fontbld.c,v 3.890 92/02/06 16:11:28 dtk FREEZE $
*/
  
/*
* $Log:	fontbld.c,v $
 * Revision 3.890  92/02/06  16:11:28  16:11:28  dtk (Doug Kaltenecker)
 * Win3.1 Freeze
 * 
 * Revision 3.871  92/02/06  15:53:28  15:53:28  dtk (Doug Kaltenecker)
 * Changed dingbats to symbol charset.
 * 
 * Revision 3.870  91/11/08  11:43:11  11:43:11  dtk (Doug Kaltenecker)
 * 3.1 Release Candidate 1
 * 
 * Revision 3.865  91/11/01  13:51:09  13:51:09  dtk (Doug Kaltenecker)
 * Beta release for Windows 3.1
 * 
 * Revision 3.862  91/10/25  13:46:30  13:46:30  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.860  91/10/23  09:47:56  09:47:56  dtk (Doug Kaltenecker)
 * WinWird Release
 * 
 * Revision 3.852  91/10/09  14:58:59  14:58:59  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.850  91/10/04  16:49:02  16:49:02  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE
 * 
 * Revision 3.840  91/09/28  14:16:30  14:16:30  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE 3 FOR WINWORD
 * 
 * Revision 3.830  91/09/18  16:32:45  16:32:45  dtk (Doug Kaltenecker)
 * RELEASE
 * 
 * Revision 3.822  91/09/16  10:33:03  10:33:03  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE 2 FOR WIN30.
 * 
 * Revision 3.820  91/09/06  14:11:23  14:11:23  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE FOR WINWORD 2.0
 * 
 * Revision 3.812  91/08/22  14:31:21  14:31:21  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.807  91/08/08  10:30:39  10:30:39  dtk (Doug Kaltenecker)
 * PREBETA3
 * 
 * Revision 3.802  91/07/22  12:53:30  12:53:30  oakeson (Ken Oakeson)
 * BETA
 * 
 * Revision 3.799  91/07/02  11:50:57  11:50:57  daniels (Susan Daniels)
 * Beta
 * 
 * Revision 3.796  91/06/26  11:25:15  11:25:15  stevec (Steve Claiborne)
 * BETA
 * 
 * Revision 3.790  91/06/11  16:02:32  16:02:32  stevec (Steve Claiborne)
 * Freeze
 * 
 * Revision 3.786  91/06/11  15:43:03  15:43:03  dtk (Doug Kaltenecker)
 * Prettified files!
 * 
* Revision 3.785  91/05/22  14:56:16  14:56:16  stevec (Steve Claiborne)
* Beta version to MS
*
* Revision 3.780  91/05/15  15:56:23  15:56:23  stevec (Steve Claiborne)
* Beta
*
* Revision 3.775  91/04/05  14:30:17  14:30:17  stevec (Steve Claiborne)
* Beta release to MS
*
* Revision 3.770  91/03/25  15:35:17  15:35:17  stevec (Steve Claiborne)
* maintance release
*
* Revision 3.760  91/03/12  07:51:57  07:51:57  stevec (Steve Claiborne)
* Maintance release
*
* Revision 3.755  91/03/03  07:45:22  07:45:22  stevec (Steve Claiborne)
* March 3 Freeze
*
* Revision 3.720  91/02/11  09:14:39  09:14:39  stevec (Steve Claiborne)
* Aldus version
*
* Revision 3.711  91/02/08  16:25:26  16:25:26  stevec (Steve Claiborne)
* Added debuging
*
* Revision 3.710  91/02/04  15:47:00  15:47:00  stevec (Steve Claiborne)
* Aldus freeze
*
* Revision 3.700  91/01/19  08:59:42  08:59:42  stevec (Steve Claiborne)
* Release
*
* Revision 3.685  91/01/14  15:42:33  15:42:33  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.681  91/01/14  10:16:54  10:16:54  stevec (Steve Claiborne)
* Updated the copy right stmt.
*
* Revision 3.680  91/01/10  16:16:00  16:16:00  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.670  90/12/14  14:53:25  14:53:25  stevec (Steve Claiborne)
* freeze for 12-14-90 ver. 3.670
*
* Revision 3.665  90/12/10  15:35:08  15:35:08  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.661  90/12/10  10:04:32  10:04:32  stevec (Steve Claiborne)
* Removed some unreferenced local variables.  SJC
*
* Revision 3.660  90/12/07  14:49:39  14:49:39  stevec (Steve Claiborne)
* Freeze 12-7-90
*
* Revision 3.650  90/11/30  08:11:34  08:11:34  stevec (Steve Claiborne)
* Freeze 3.650, 11-30-90
*
* Revision 3.603  90/10/25  10:36:21  10:36:21  oakeson (Ken Oakeson)
* When building font summary with status dialog box, no longer print
* "0pt" after scalable fonts
*
* Revision 3.602  90/08/29  17:42:16  17:42:16  oakeson (Ken Oakeson)
* Initialized new font summary field (hCharDL)
*
*/
  
/***************************************************************************/
/******************************   fontbld.c   ******************************/
//
//  Fontbld: This module builds up the font summary data used by realize
//  and enumfonts to choose a font.
//
// 03 feb 90    KLO Changed lmemcpy() to lstrncpy()
// 07 aug 89    peterbe Changed lstrcmp() to lstrcmpi().
//   1-26-89    jimmat  Removed some unused code.
//   1-30-89    jimmat  Changes to locate soft fonts in Font Installer WIN.INI
//          section instead of HPPCL section.
//   2-02-89    jimmat  Changes to allow HP cartridge info to be removed from
//          driver resources ("externalize HP cartridges").
//   2-07-89    jimmat  Driver Initialization changes;
//   2-20-89    jimmat  Driver/Font Installer use same WIN.INI section (again)!
//
  
/*** Tetra begin ***/
//#define DEBUG
/*** Tetra end ***/
  
#include "generic.h"
#include "resource.h"
#include "fontman.h"
#include "fonts.h"
#include "strings.h"
#include "memoman.h"
#include "fontpriv.h"
#include "fntutils.h"
#include "utils.h"
#include "lclstr.h"
  
  
/*  Utilities
*/
#include "getint.c"
  
  
/*  Local debug structure (mediumdumpfs must be
*  enabled to get longdumpfs).
*/
/*** Tetra begin ***/
#ifdef DEBUG
    #define LOCAL_DEBUG
//    #define DBGdumpfontsummary
//    #define DBGmediumdumpfs
//    #define DBGlongdumpfs
#endif
/*** Tetra end ***/
  
  
#ifdef LOCAL_DEBUG
    #define DBGentry(msg) DBMSG(msg)
    #define DBGerr(msg) /*DBMSG(msg)*/
    #define DBGbuildfont(msg) /*DBMSG(msg)*/
    #define DBGsoftfont(msg) DBMSG(msg)
    #define DBGloadUniqueName(msg) /*DBMSG(msg)*/
    #define DBGexpandLockFS(msg) /*DBMSG(msg)*/
    #define DBGcartridge(msg) /*DBMSG(msg)*/
#else
    #define DBGentry(msg) /*null*/
    #define DBGerr(msg) /*null*/
    #define DBGbuildfont(msg) /*null*/
    #define DBGsoftfont(msg) /*null*/
    #define DBGloadUniqueName(msg) /*null*/
    #define DBGexpandLockFS(msg) /*null*/
    #define DBGcartridge(msg) /* null */
#endif
  
  
/*  Forward local procs.
*/
LOCAL HANDLE loadFontEntries(LPHANDLE, HANDLE, LPPCLDEVMODE, LPSTR, HANDLE);
LOCAL BOOL insertFontItem(LPHANDLE, LPFONTINFO, LPLFS, LPFNT);
LOCAL void loadFontNameTable(LPHANDLE, LPPCLDEVMODE, LPLFS, LPFNT);
LOCAL void initLoadSoftFont(LPLFS, LPSTR, HANDLE);
LOCAL BOOL loadSoftFont(LPFONTINFO, short, LPLFS, LPFNT);
LOCAL void initLoadResourceFont(LPLFS, LPPCLDEVMODE);
LOCAL BOOL loadFontPFM(int, LPFONTINFO, short, LPFNT, LPSTR);
LOCAL BOOL loadResourceFont(LPFONTINFO, HANDLE, short, LPLFS, LPFNT);
LOCAL BOOL loadUniqueName(LPSTR, LPFNT, FNTNameList);
LOCAL BOOL dupEscape(LPFNT, LPSTR);
LOCAL SYMBOLSET ssFromEscape(LPSTR);
LOCAL LPFONTSUMMARYHDR expandLockFontSum(LPHANDLE, LPLFS, DWORD);
LOCAL BOOL loadCartridgeFont(LPFONTINFO, short, LPLFS, LPFNT);
LOCAL void initLoadCartridgeFont(LPLFS, LPPCLDEVMODE);
  
  
#ifdef DIALOG_MESSAGES
LOCAL void UpdateLotsOfFontsDlg(HWND, LPFNT, LPFONTINFO, WORD, WORD, LPSTR, LPSTR, LPSTR);
int FAR PASCAL DlgLotsOfFonts (HWND, unsigned, WORD, LONG);
#endif
  
#ifdef DEBUG
LOCAL void DBGdumpFSItem(LPFONTSUMMARYHDR, short);
LOCAL void DBGdumpFontNameTable(LPFONTSUMMARYHDR);
#endif
  
/*  global vars
*/
LOCAL BOOL gAlreadyWorking = FALSE;
extern HANDLE hLibInst;         /* module instance handle */
  
  
#if 0
extern void far pascal OutputDebugString(LPSTR);
extern int far pascal wvsprintf(LPSTR,LPSTR,LPSTR);
static void near cdecl dpf(LPSTR lpfmt, ...)
{
    char sz[160];
  
    wvsprintf(sz,lpfmt,(LPSTR)(&lpfmt+1));
    OutputDebugString(sz);
}
#endif
  
  
/***************************************************************************/
/**************************   Global Procedures   **************************/
  
  
/*  buildFontSummary
*
*  Build the fontSummary data structure.
*/
HANDLE FAR PASCAL
buildFontSummary(LPPCLDEVMODE lpEnviron, HANDLE hLS, LPSTR lpPortNm,
HANDLE hMd) {
  
    short err = 0;
    HANDLE hFontSummary = 0;
    LPFONTSUMMARYHDR lpFontSummary = 0L;
  
#ifdef DEBUG_FUNCT
    DB(("Entering buildFontSummary\n"));
#endif
    DBGentry(("BuildFontSummary(%lp,%lp,%d): %ls, %ls\n",
    lpEnviron, lpPortNm, (HANDLE)hMd, (LPSTR)lpEnviron, lpPortNm));
  
    DBGbuildfont(("BuildFontSummary(): gAlreadyWorking=%s\n",
    gAlreadyWorking ? "TRUE" : "FALSE"));
    DBGbuildfont(("BuildFontSummary(): Attempting to allocate hFontSummary\n"));
  
    /*  Initially allocate fontSummary header.
    */
    if (hFontSummary =
        GlobalAlloc(GMEM_MOVEABLE | GMEM_LOWER | GMEM_DDESHARE | GMEM_ZEROINIT,
        (DWORD)(sizeof(FONTSUMMARYHDR) - sizeof(FONTSUMMARY))))
    {
        /*  Memory successfully allocated, build fontSummary.
        */
        DBGbuildfont(("BuildFontSummary(): Loading hFontSummary\n"));
  
        /*  Load up the fontSummary struct -- after this call, hLS
        *  is invalid.
        */
        if (err = loadFontEntries(&hFontSummary,hLS,lpEnviron,lpPortNm,hMd))
        {
            /*  Could not load fontSummary data.
            *
            *  Error alert here if you so desire (some apps don't
            *  have a window up at this point).
            */
            DBGerr(("BuildFontSummary(): *failed* to load lpFontSummary\n"));
            goto backout0;
        }
    }
    else
    {
        DBGerr(("BuildFontSummary(): *failed* to alloc hFontSummary\n"));
        goto backout1;
    }
  
    /*  Font summary information successfully located.
    */
    DBGbuildfont(("buildFontSummary(): fontSummary built, size=%ld bytes\n",
    GlobalSize(hFontSummary)));
  
    lpFontSummary = 0L;
    goto backout2;
  
    backout0:
    GlobalFree(hFontSummary);
  
    backout1:
    hFontSummary = 0;
    lpFontSummary = 0L;
  
    DBGbuildfont(("BuildFontSummary(): FontSummary *not* located\n"));
  
    backout2:
    DBGbuildfont(("...end of BuildFontSummary, return %d\n", hFontSummary));
  
#ifdef DEBUG_FUNCT
    DB(("Exiting buildFontSummary\n"));
#endif
    return (hFontSummary);
}
  
#ifdef DEBUG
/*  DBGdumpFontSummary
*
*  Dump font summary information.
*/
void FAR PASCAL DBGdumpFontSummary(lpFontSummary, ind)
LPFONTSUMMARYHDR lpFontSummary;
short ind;
{
#ifdef DEBUG_FUNCT
    DB(("Entering DBGdumpFontSummary\n"));
#endif
    if (ind < 0)
    {
        /*  Dump all items in the font summary.
        */
        DBMSG(("numOpenDC=%d, len=%d, tblSz=%d, 1stSft=%d\n",
        lpFontSummary->numOpenDC, lpFontSummary->len,
        lpFontSummary->softfonts, lpFontSummary->firstSoft));
        for (ind = 0; ind < lpFontSummary->len; ++ind)
            DBGdumpFSItem(lpFontSummary, ind);
        DBMSG(("Font Name Table:\n"));
        DBGdumpFontNameTable(lpFontSummary);
    }
    else if (ind < lpFontSummary->len)
    {
        DBGdumpFSItem(lpFontSummary, ind);
    }
    else if (ind == lpFontSummary->len)
    {
        DBGdumpFontNameTable(lpFontSummary);
    }
    else
    {
        DBMSG(("DBGdumpFontSummary(): no valid index (%d) specified\n", ind));
    }
#ifdef DEBUG_FUNCT
    DB(("Exiting DBGdumpFontSummary\n"));
#endif
}
#endif
  
/***************************************************************************/
/**************************   Local Procedures   ***************************/
  
  
/*  loadFontEntries
*
*  Fill in the fontSummary data structure, it looks like this:
*
*          HEADER
*          font 1
*          font 2
*          .
*          .
*          font n
*          fontNameTable
*
*  The header contains the length (# fonts), version, and # open DC's for
*  the structure.  Each font contains just enough information for proc
*  RealizeObject() to select a font without loading in any resources.
*
*  The font name table contains the names of all the fonts -- each font has
*  an index into this table (it was done this way to compact the amount
*  of space used by the names, many fonts also share the same name).
*/
LOCAL HANDLE loadFontEntries(lpHFS, hLS, lpEnviron, lpPortNm, hMd)
LPHANDLE lpHFS;
HANDLE hLS;
LPPCLDEVMODE lpEnviron;
LPSTR lpPortNm;
HANDLE hMd;
{
    HANDLE hFT;
    HANDLE hFontInfo;
    LPLFS lpLS;
    LPFNT lpFT;
    LPFONTINFO lpFontInfo;
    short status = 0;
    short fontCount = 0;
    short orient = DMORIENT_PORTRAIT;
#ifdef DIALOG_MESSAGES
    HWND hWnd=0;
    char point[16];
    char bold[16];
    char italic[16];
#endif
  
#ifdef DEBUG_FUNCT
    DB(("Entering loadFontEntries\n"));
#endif
    DBGentry(("loadFontEntries(%lp,%d,%lp,%lp,%d)\n",
    lpHFS, (WORD)hLS, lpEnviron, lpPortNm, (unsigned)hMd));
  
    /*  Make sure the LoadFontState struct was previously allocated.
    */
    if (!hLS)
    {
        status = FM_MEMALLOC_FAIL;
        goto backout6;
    }
  
    /*  Lock down LoadFontState struct (created in getFileFontSummary()).
    */
    if (!(lpLS = (LPLFS)GlobalLock(hLS)))
    {
        DBGerr(("loadFontEntries(): Could *not* lock LoadFontState\n"));
        status = FM_MEMLOCK_FAIL;
        goto backout5;
    }
  
    /*  Load the list of soft fonts from the Font Installer's section
    *  of the win.ini file.
    */
    if (!(lpLS->hWinSF = InitWinSF(lpLS->appName)))
    {
        DBGerr(("loadFontEntries(): Missing key names from win.ini\n"));
        status = FM_MEMALLOC_FAIL;
        goto backout4;
    }
  
    /*  Allocate FontNameTable struct.
    */
    if (!(hFT = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT,
        (DWORD)sizeof(FONTNAMETABLE))))
    {
        DBGerr(("loadFontEntries(): Could *not* alloc FontNameTable\n"));
        status = FM_MEMALLOC_FAIL;
        goto backout3a;
    }
  
    if (!(lpFT = (LPFNT)GlobalLock(hFT)))
    {
        DBGerr(("loadFontEntries(): Could *not* lock FontNameTable\n"));
        status = FM_MEMLOCK_FAIL;
        goto backout3;
    }
  
    /*  Allocate (but don't lock) font name table.
    */
    if (!(lpFT->hNMTBL = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT,
        (DWORD)FNTBL_LEN)))
    {
        DBGerr(("loadFontEntries(): Could *not* alloc font name table\n"));
        status = FM_MEMALLOC_FAIL;
        goto backout2;
    }
    lpFT->availMem = FNTBL_LEN;
  
    /*  Allocate FontInfo struct.
    */
    if (!(hFontInfo = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT,
        (DWORD)sizeof(FONTINFO))))
    {
        DBGerr(("loadFontEntries(): Could *not* alloc FontInfo\n"));
        status = FM_MEMALLOC_FAIL;
        goto backout1a;
    }
  
    if (!(lpFontInfo = (LPFONTINFO)GlobalLock(hFontInfo)))
    {
        DBGerr(("loadFontEntries(): Could *not* lock FontInfo\n"));
        status = FM_MEMLOCK_FAIL;
        goto backout1;
    }
  
    /*  Set orientation based upon the printer's capabilities.
    */
    orient = lpEnviron->dm.dmOrientation;
    if (lpEnviron->prtCaps & BOTHORIENT)
        orient = 0;
  
    /*  Load fonts info (ROM and cartridge) from resource file.
    */
    initLoadResourceFont(lpLS, lpEnviron);
  
    while ((fontCount++ <= MAX_FONTS) &&
        loadResourceFont(lpFontInfo, hMd, orient, lpLS, lpFT) &&
        insertFontItem(lpHFS, lpFontInfo, lpLS, lpFT))
        ;
  
    /* !!!craigc: load 3rd party cartridge fonts remaining after
    * load of resource fonts
    */
    initLoadCartridgeFont(lpLS, lpEnviron);
  
    while ((fontCount++ <= MAX_FONTS) &&
        loadCartridgeFont(lpFontInfo, orient, lpLS, lpFT) &&
        insertFontItem(lpHFS, lpFontInfo, lpLS, lpFT))
        ;
  
    /*  gAlreadyWorking == TRUE when we've been enabled once already
    *  and we tried to put up an informative dialog.  This causes
    *  Write to re-enable the driver...so, on this recursive call
    *  we build a fontSummary structure which lacks soft fonts.  It
    *  contains enough information to realize fonts.
    */
    if (gAlreadyWorking)
        lpEnviron->prtCaps |= NOSOFT;
  
    /*  Load soft fonts (only if printer can handle them).
    */
    if (!(lpEnviron->prtCaps & NOSOFT))
    {
        initLoadSoftFont(lpLS, lpPortNm, hMd);
  
#ifdef DIALOG_MESSAGES
        /*  If we have a lot of fonts to load and the PageMaker startup
        *  screen is up, then put up a message informing the user what
        *  is happening.
        */
        if ((lpLS->softfonts > LOTSOF_FONTS) && !gAlreadyWorking &&
            LoadString(hMd, IDS_POINT, point, sizeof(point)) &&
            LoadString(hMd, IDS_BOLD, bold, sizeof(bold)) &&
            LoadString(hMd, IDS_ITALIC, italic, sizeof(italic)) &&
            (hWnd = CreateDialog(hMd, (LPSTR)((DWORD)LOTSOF_FONTSDLG), 0,
            DlgLotsOfFonts)))
        {
            WORD sfcount = 0;
  
            gAlreadyWorking = TRUE;
            ShowWindow(hWnd, SHOW_OPENWINDOW);
            UpdateWindow(hWnd);
  
            {
                char captionText[64];
                short len;
  
                len = GetWindowText(hWnd, captionText, sizeof(captionText));
                if (sizeof(captionText) > len+1)
                    /* TETRA -- changed lmemcpy to lstrncpy -- KLO */
                    lstrncpy(&captionText[len], lpPortNm,sizeof(captionText)-len-1);
                captionText[sizeof(captionText)-1] = '\0';
                SetWindowText(hWnd, captionText);
            }
  
            while ((fontCount++ <= MAX_FONTS) &&
                loadSoftFont(lpFontInfo, orient, lpLS, lpFT) &&
                insertFontItem(lpHFS, lpFontInfo, lpLS, lpFT))
            {
                UpdateLotsOfFontsDlg(hWnd, lpFT, lpFontInfo, ++sfcount,
                lpLS->softfonts, point, bold, italic);
            }
  
            if (sfcount < lpLS->softfonts)
            {
                UpdateLotsOfFontsDlg(hWnd, lpFT, lpFontInfo, lpLS->softfonts,
                lpLS->softfonts, point, bold, italic);
            }
            DestroyWindow(hWnd);
  
            gAlreadyWorking = FALSE;
        }
        else
        {
#endif
            while ((fontCount++ <= MAX_FONTS) &&
                loadSoftFont(lpFontInfo, orient, lpLS, lpFT) &&
                insertFontItem(lpHFS, lpFontInfo, lpLS, lpFT))
                ;
  
#ifdef DIALOG_MESSAGES
        }
#endif
    }
  
    /*  Copy in the font table, plus initialize all the variables
    *  in the fontSummary header.
    */
    loadFontNameTable(lpHFS, lpEnviron, lpLS, lpFT);
  
    status = lpLS->status;
  
    GlobalUnlock(hFontInfo);
    backout1:
    GlobalFree(hFontInfo);
    backout1a:
    GlobalFree(lpFT->hNMTBL);
    backout2:
    GlobalUnlock(hFT);
    backout3:
    GlobalFree(hFT);
    backout3a:
    EndWinSF(lpLS->hWinSF);
    backout4:
    GlobalUnlock(hLS);
    backout5:
    GlobalFree(hLS);
    hLS = 0;
    backout6:
#ifdef DEBUG_FUNCT
    DB(("Exiting loadFontEntries\n"));
#endif
    return (status);
}
  
/*  insertFontItem
*
*  Drop the font item into the fontSummary data structure.  The fonts
*  are placed in order of ascending face name, pixel height.
*/
LOCAL BOOL insertFontItem(lpHFS, lpFont, lpLS, lpFT)
LPHANDLE lpHFS;
LPFONTINFO lpFont;
LPLFS lpLS;
LPFNT lpFT;
{
    LPFONTSUMMARYHDR lpFontSummary;
    LPFONTSUMMARY lpSummary;
    LPSTR lpSource, lpDest, lpNMTBL;
    short ind, last, ind2;
  
#ifdef DEBUG_FUNCT
    DB(("Entering insertFontItem\n"));
#endif
    DBGentry(("insertFontItem(%lp,%lp,%lp,%lp)\n",
    lpHFS, lpFont, lpLS, lpFT));
  
    if (lpLS->status)
        return FALSE;
  
    /*  Make space for the font information (and lock fontSummary).
    */
    if (!(lpFontSummary =
        expandLockFontSum(lpHFS, lpLS, (DWORD)sizeof(FONTSUMMARY))))
    {
        DBGerr(("insertFontItem(): failed to expand fontSummary\n"));
        lpLS->status = FM_MEMALLOC_FAIL;
        return FALSE;
    }
  
    /*  New space created and locked.  Increment count of fonts.
    */
    last = lpLS->numFontsLoaded;
    ++lpLS->numFontsLoaded;
  
    /*  Lock down font name table.
    */
    if (!(lpNMTBL = GlobalLock(lpFT->hNMTBL)))
    {
        DBGerr(("insertFontItem(): failed to lock name table\n"));
        lpLS->status = FM_MEMLOCK_FAIL;
        return FALSE;
    }
  
    /*  Locate the position in the fontSummary where this new font
    *  should be inserted.
    */
    lpSummary = &lpFontSummary->f[0];
    lpSource = &lpNMTBL[lpFT->indName];
  
    for (ind = 0; ind < last; ++ind, ++lpSummary)
    {
        lpDest = &lpNMTBL[lpSummary->indName];
  
        if (lstrcmpi(lpSource,lpDest) < 0)
            break;
    }
  
    /*  Unlock font name table.
    */
    GlobalUnlock (lpFT->hNMTBL);
    lpNMTBL = 0L;
  
    /*  Bump all fonts after the insertion point.
    */
    for (ind2 = last; ind2 > ind; --ind2)
        lpFontSummary->f[ind2] = lpFontSummary->f[ind2-1];
  
    /*  Load/initialize non-metric data;
    */
    lpSummary->offset = lpFT->offset;
    lpSummary->symbolSet = lpFT->symbolSet;
    lpSummary->fontID = lpFT->fontID;
    lpSummary->indName = lpFT->indName;
    lpSummary->indEscape = lpFT->indEscape;
    lpSummary->indPFMPath = lpFT->indPFMPath;
    lpSummary->indDLPath = lpFT->indDLPath;
    lpSummary->indPFMName = lpFT->indPFMName;
    lpSummary->indDLName = lpFT->indDLName;
    lpSummary->ZCART_hack = lpFT->ZCART_hack;
    lpSummary->QUOTE_hack = lpFT->QUOTE_hack;
  
    /*** Tetra begin ***/
    lpSummary->scaleInfo.scalable = lpFT->scaleInfo.scalable;
    if (lpFT->scaleInfo.emMasterUnits)
        lpSummary->scaleInfo.emMasterUnits = lpFT->scaleInfo.emMasterUnits;
    else
        lpSummary->scaleInfo.emMasterUnits = 0;
    /*** Tetra end ***/
  
    lpSummary->lPCMOffset = lpFT->lPCMOffset;
    lpSummary->LRUcount = -1;
    lpSummary->memUsage = lpFT->memUsage;
  
    lpSummary->hCharDL = 0;
  
    lpSummary->onPage = 0;
    lpSummary->indPrevSoft = -1;
    lpSummary->indNextSoft = -1;
    lpSummary->hExtMetrics = 0;
    lpSummary->hWidthTable = 0;
    lpSummary->hPairKernTable = 0;
    lpSummary->hTrackKernTable = 0;
  
    /*  Load metric data.
    */
    lpSummary->dfType           = lpFont->dfType;
    lpSummary->dfPoints         = lpFont->dfPoints;
    lpSummary->dfVertRes        = lpFont->dfVertRes;
    lpSummary->dfHorizRes       = lpFont->dfHorizRes;
    lpSummary->dfAscent         = lpFont->dfAscent;
    lpSummary->dfInternalLeading    = lpFont->dfInternalLeading;
    lpSummary->dfExternalLeading    = lpFont->dfExternalLeading;
    lpSummary->dfItalic         = lpFont->dfItalic;
    lpSummary->dfUnderline      = lpFont->dfUnderline;
    lpSummary->dfStrikeOut      = lpFont->dfStrikeOut;
    lpSummary->dfWeight         = lpFont->dfWeight;

    /* Check for a Zaph Dingbat font and change charset to
     * symbol 'cause GDI forces the set to symbol. - dtk 2/92
     */
    if (!lstrcmpi((LPSTR)lpSource, "ITC Zapf Dingbats") ||
        !lstrcmpi((LPSTR)lpSource, "ITCZapfDingbats") ||
        !lstrcmpi((LPSTR)lpSource, "Zapf Dingbats") ||
        !lstrcmpi((LPSTR)lpSource, "ZapfDingbats") ||
        !lstrcmpi((LPSTR)lpSource, "Symbol"))

        lpSummary->dfCharSet = 2; // symbol charset
    else
        lpSummary->dfCharSet    = lpFont->dfCharSet;

    lpSummary->dfPixWidth       = lpFont->dfPixWidth;
    lpSummary->dfPixHeight      = lpFont->dfPixHeight;
    lpSummary->dfPitchAndFamily     = lpFont->dfPitchAndFamily;
    lpSummary->dfAvgWidth       = lpFont->dfAvgWidth;
    lpSummary->dfMaxWidth       = lpFont->dfMaxWidth;
    lpSummary->dfFirstChar      = lpFont->dfFirstChar;
    lpSummary->dfLastChar       = lpFont->dfLastChar;
    lpSummary->dfDefaultChar        = lpFont->dfDefaultChar;
    lpSummary->dfBreakChar      = lpFont->dfBreakChar;
  
    /*  Unlock fontSummary.
    */
    GlobalUnlock (*lpHFS);
  
#ifdef DEBUG_FUNCT
    DB(("Exiting insertFontItem\n"));
#endif
    return TRUE;
}
  
/*  loadFontNameTable
*
*  Append the font name table to the end of the fontSummary struct.  Also
*  fill in the header info for the fontSummary struct.
*/
LOCAL void loadFontNameTable(lpHFS, lpEnviron, lpLS, lpFT)
LPHANDLE lpHFS;
LPPCLDEVMODE lpEnviron;
LPLFS lpLS;
LPFNT lpFT;
{
    LPFONTSUMMARYHDR lpFontSummary;
    LPSTR lpNMTBL;
    short len;
  
#ifdef DEBUG_FUNCT
    DB(("Entering loadFontName\n"));
#endif
    DBGentry(("loadFontNameTable(%lp,%lp,%lp,%lp)\n",
    lpHFS, lpEnviron, lpLS, lpFT));
  
    if (lpLS->status)
        return;
  
    /*  Add more memory to fontSummary struct.
    */
    if (!(lpFontSummary = expandLockFontSum(lpHFS, lpLS, (DWORD)lpFT->tblSize)))
    {
        DBGerr(("loadFontNameTable(): failed to expand fontSummary\n"));
        lpLS->status = FM_MEMALLOC_FAIL;
        return;
    }
  
    /*  Lock down font name table.
    */
    if (!(lpNMTBL = GlobalLock(lpFT->hNMTBL)))
    {
        DBGerr(("loadFontNameTable(): failed to lock name table\n"));
        lpLS->status = FM_MEMLOCK_FAIL;
        return;
    }
  
    /*  Copy the font name table to the end of the fontSummary struct.
    */
    len = lpLS->numFontsLoaded;
    lmemcpy((LPSTR)&lpFontSummary->f[len], lpNMTBL, lpFT->tblSize);
  
    /*  Unlock font name table.
    */
    GlobalUnlock (lpFT->hNMTBL);
    lpNMTBL = 0L;
  
    /*  Load fontSummary header.
    */
    lpFontSummary->numOpenDC = 1;
    lpFontSummary->len = len;
    lpFontSummary->firstSoft = -1;
    lpFontSummary->newFS = TRUE;
    lpFontSummary->softfonts = lpLS->softfonts;
    lmemcpy((LPSTR) &lpFontSummary->environ, (LPSTR)lpEnviron,
    sizeof(PCLDEVMODE));
  
    /*  Debug stuff: dump the whole damn thing.
    */
    #ifdef DBGdumpfontsummary
    DBMSG(("\n"));
    DBMSG(("BuildFontSummary(): New font summary structure created\n"));
    DBGdumpFontSummary(lpFontSummary, -1);
    #endif
  
    /*  Unlock fontSummary.
    */
    GlobalUnlock (*lpHFS);
#ifdef DEBUG_FUNCT
    DB(("Exiting loadFontName\n"));
#endif
}
  
/*  initLoadSoftFont
*
*/
LOCAL void initLoadSoftFont(lpLS, lpPortName, hModule)
LPLFS lpLS;
LPSTR lpPortName;
HANDLE hModule;
{
#ifdef DEBUG_FUNCT
    DB(("Entering initLoadSoftFonts\n"));
#endif
    DBGentry(("initLoadSoftFont(%lp,%lp,%d)\n", lpLS, lpPortName, hModule));
  
    if (lpLS->status)
        return;
#ifdef DEBUG_FUNCT
    DB(("Exiting initLoadSoftFonts\n"));
#endif
}
  
  
/*
*  loadFontPFM
*
*  This function is used by loadSoftFont and loadCartridgeFont as the
*  operation of the two are very similar.  Management of soft fonts is
*  on a font-by-font basis, so PFM's appear in individual files with
*  no seek offsets.  PCM files, however, contain a number of PFMs and
*  specify a seek offset to the beginning of the PFM.
*
*  The seek offset should already be in lpFT->lPCMOffset, and should
*  be zero for SF pfm files.
*
*
*/
  
LOCAL BOOL
loadFontPFM(int hFile, LPFONTINFO lpFontInfo, short orient, LPFNT lpFT,
LPSTR lpDL) {
  
    int err;
    short emOrientation;
    long lPCM=lpFT->lPCMOffset;
    DWORD seek, extMOffset, drvInfo;
    LPSTR lpBuf = (LPSTR) lpFT->workBuf;
    BOOL fontLoaded = FALSE, hasEsc = FALSE;
#ifdef DEBUG_FUNCT
    DB(("Entering loadFontPFM\n"));
#endif
  
    DBGentry(("loadFontPFM(%x,%lp,%x,%lp) (seeks offset by %ld)\n",
    hFile,lpFontInfo,orient,lpFT,lPCM));
  
    /* read header stuff */
    err = _lread(hFile, (LPSTR) lpFontInfo, sizeof(FONTINFO));
  
    /*  Move file pointer to after width table in file.
    *  (for fixed pitch fonts the table does not exist)
    */
    seek = sizeof(PFMHEADER) - 2;
    if (lpFontInfo->dfPitchAndFamily & 0x1)
    {
        seek += (lpFontInfo->dfLastChar - lpFontInfo->dfFirstChar + 2) * 2;
        DBGcartridge(("proportional"));
    }
    else
        DBGcartridge(("fixed"));
  
    DBGcartridge((" font, seek offset %ld (relative to %ld == %ld)\n",seek,
    lPCM,lPCM+seek));
  
    _llseek(hFile, lPCM+seek, 0);
    err += _lread(hFile, lpBuf, sizeof(PFMEXTENSION));
  
    /*  Get offsets to struct in file we want to read.
    */
    extMOffset = ((LPPFMEXTENSION)lpBuf)->dfExtMetricsOffset;
    drvInfo = ((LPPFMEXTENSION)lpBuf)->dfDriverInfo;
  
    DBGcartridge(("extMOffset=%ld, drvInfo=%ld\n",extMOffset,drvInfo));
  
    if (extMOffset)
    {
        /*  Extended text metrics: orientation.
        */
        _llseek(hFile, lPCM+extMOffset, 0);
        err += _lread(hFile, lpBuf, sizeof(EXTTEXTMETRIC));
  
        emOrientation = ((LPEXTTEXTMETRIC)lpBuf)->emOrientation;
  
        /*** Tetra begin ***/
        lpFT->scaleInfo.scalable =
        (((LPEXTTEXTMETRIC)lpBuf)->emMinScale != ((LPEXTTEXTMETRIC)lpBuf)->emMaxScale);
        lpFT->scaleInfo.emMasterUnits = ((LPEXTTEXTMETRIC)lpBuf)->emMasterUnits;
        /*** Tetra end ***/
  
        err -= sizeof(EXTTEXTMETRIC);
    }
    else
    {
        /*  No extended text metrics, allow any orientation.
        */
        DBGerr(("loadFontPFM(): no extTextMetric, making default\n"));
        emOrientation = 0;
  
        /*** Tetra begin ***/
        lpFT->scaleInfo.scalable = FALSE;
        lpFT->scaleInfo.emMasterUnits = 0;
        /*** Tetra end ***/
  
    }
  
    lpFT->indEscape = -1;
  
    if (drvInfo) {
  
        int terr = 0;
  
        _llseek(hFile, lPCM+drvInfo, 0);
        terr = _lread(hFile, lpBuf, sizeof(DRIVERINFO)) - sizeof(DRIVERINFO);
  
#ifdef MEMORY_MANAGER
        /* Tetra - divide memusage by two for on-demand char downloading */
        lpFT->memUsage = ((LPDRIVERINFO)lpBuf)->epMemUsage / 2;
#else
        lpFT->memUsage = 0L;
#endif
        lpFT->symbolSet = ((LPDRIVERINFO)lpBuf)->xtbl.symbolSet;
  
        /*  If this PFM is in a PCM, and it doesn't have any extended
        *  text metrics, and the driver info doesn't have a translation
        *  table, then the lastchar field of the transtbl has been
        *  hacked to contain the Z-hack, Quote-hack, and font
        *  orientation flags.  This was done to the .PCM files created
        *  for the existing HP cartridges (A,B,...Z1A) when they were
        *  "externalized" from the driver's resources.  The orientation
        *  is there because we didn't want to create ext text metrics
        *  for all the fonts in the 24 internal cartridges, and the
        *  Z/Quote-hack flags are there to minimize other driver changes.
        */
  
        if (!terr && lPCM && !extMOffset && !((LPDRIVERINFO)lpBuf)->xtbl.len) {
            emOrientation = ((LPDRIVERINFO)lpBuf)->xtbl.lastchar & 3;
            lpFT->ZCART_hack =
            (((LPDRIVERINFO)lpBuf)->xtbl.lastchar & 0x10) == 0x10;
            lpFT->QUOTE_hack =
            (((LPDRIVERINFO)lpBuf)->xtbl.lastchar & 0x20) == 0x20;
            DBGsoftfont(("loadFontPFM(): slimy driverinfo hack, emOrient=%d, "
            "Zhack=%d, Qhack=%d\n",emOrientation,lpFT->ZCART_hack,
            lpFT->QUOTE_hack));
        }
  
  
        /*  If an escape string exists and the file is permanently downloaded,
        *  then load the escape string so we'll treat it like a ROM/cartridge
        *  font.
        */
  
        if (!terr && ((LPDRIVERINFO)lpBuf)->epEscape && (!lpDL || !*lpDL)) {
  
            int i = -1;
  
            terr = 0;
            _llseek(hFile, lPCM+((LPDRIVERINFO)lpBuf)->epEscape, 0);
  
            do  {
                ++i;
                terr += _lread(hFile, (LPSTR)&lpBuf[i], 1) - 1;
            } while (!terr && lpBuf[i] && (i < WBUF_LEN-1));
  
            lpBuf[i+1] = '\0';
  
            DBGsoftfont(("loadFontPFM(): PFM file contains an escape string"
            " (%ls)\n", lpBuf));
  
            /* Remember if lpBuf contains an escape string, used later */
  
            hasEsc = (!terr && i);
        }
  
        if (terr) {
  
            /*  If we detected an err while trying to read driver info, then
            *  zero drvInfo ptr so we'll use the defaults.
            */
            DBGerr(("loadFontPFM(): could *not* load driver info at %ld\n",
            drvInfo));
            drvInfo = 0L;
        }
    }
  
    if (!drvInfo) {
  
        /*  No driver info, make best guess.
        */
        DBGerr(("loadFontPFM(): no driverInfo, making default\n"));
  
#ifdef MEMORY_MANAGER
        /* Tetra - divide memusage by two for on-demand char downloading */
        lpFT->memUsage =
        FontMEM((lpFontInfo->dfLastChar - lpFontInfo->dfFirstChar),
        lpFontInfo->dfAvgWidth, lpFontInfo->dfPixHeight) / 2;
#else
        lpFT->memUsage = 0L;
#endif
  
        if (lpFontInfo->dfLastChar < 255)
            lpFT->symbolSet = epsymGENERIC7;
        else
            lpFT->symbolSet = epsymGENERIC8;
  
        lpFT->indEscape = -1;
    }
  
    /*  err == 0 if we read all the bytes we were supposed to read. */
  
    err -= sizeof(PFMEXTENSION) + sizeof(FONTINFO) ;
  
    DBGsoftfont(("loadFontPFM(): err=%d, extMOffset=%ld, drvInfo=%ld, "
    "indEscape=%d\n", err, extMOffset, drvInfo, lpFT->indEscape));
    DBGsoftfont(("               emOrientation=%d, memUsage=%ld, "
    "symbolSet=%d\n", emOrientation, lpFT->memUsage,
    lpFT->symbolSet));
  
    /*  Continue if no errs so far and this font has a face name and
    *  the right orientation.
    */
  
    if (!err && lpFontInfo->dfFace && ((orient == 0) || (emOrientation == 0) ||
        ((orient == DMORIENT_PORTRAIT)  && (emOrientation == 1)) ||
    ((orient == DMORIENT_LANDSCAPE) && (emOrientation == 2)))) {
  
        /* lpBuf might contain the font escape string, load into font
        * summary if it does (after we decide we want the font, but before
        * we overwrite lpBuf).
        */
  
        if (hasEsc)
            loadUniqueName(lpBuf, lpFT, fnt_escape);
  
        _llseek(hFile, lPCM+(long)lpFontInfo->dfFace, 0);
        lmemset(lpBuf, 0, WBUF_LEN);
  
        /*  Read font name, 'err' counts number of chars read. */
  
        do {
            _lread(hFile, lpBuf, 1);
        } while (*(lpBuf++) && (++err < WBUF_LEN));
  
        *lpBuf = '\0';
        lpBuf = (LPSTR) lpFT->workBuf;
  
        fontLoaded = loadUniqueName(lpBuf, lpFT, fnt_fontname);
    }
  
#ifdef DEBUG_FUNCT
    DB(("Exiting loadFontPFM\n"));
#endif
    return fontLoaded;
}
  
  
/*  loadSoftFont
*
*  Load the next soft font listed in the WIN.INI file.  The format of
*  the names in the file are as follows:
*
*      SoftFont1=myfont1.pfm,cuor7nm.usp
*      SoftFont2=thefont.pfm,ccmnu8.usp
*      SoftFont3=permfont.pfm
*      SoftFont4=lefont.pfm
*
*  The key names are SoftFont1, SoftFont2, etc.  The .pfm files are the
*  first argument following each key name, and the download font files are
*  the files following the .pfm file.  If no download font is listed,
*  then the file is permanently downloaded to the printer, so we still read
*  the .pfm but we never attempt to download it.
*/
LOCAL BOOL loadSoftFont(lpFontInfo, orient, lpLS, lpFT)
LPFONTINFO lpFontInfo;
short orient;
LPLFS lpLS;
LPFNT lpFT;
{
    HANDLE hWinSF = lpLS->hWinSF;
    LPSTR lpBuf = (LPSTR) lpFT->workBuf;
    LPSTR lpPFM, lpDL;
    BOOL fontLoaded = FALSE;
    int hFile, err;
  
#ifdef DEBUG_FUNCT
    DB(("Entering loadSoftFont\n"));
#endif
    DBGentry(("loadSoftFont(%lp,%d,%lp,%lp)\n",
    lpFontInfo, orient, lpLS, lpFT));
  
    if (lpLS->status)
        return FALSE;
  
    lpFT->lPCMOffset=0L;        /* not in PCM; don't offset */
  
    /*** Tetra begin ***/
    lpFT->scaleInfo.scalable = FALSE;
    lpFT->scaleInfo.emMasterUnits = 0;
    /*** Tetra end ***/
  
    lpPFM = lpFT->softFontBuf;
    lpDL = 0L;
  
    while (!fontLoaded && (lpFT->fontID =
        NextWinSF(hWinSF,lpPFM,sizeof(lpFT->softFontBuf))) > -1)
    {
        /*  lpPFM points to file.pfm, advance lpDL to next name (download
        *  font) or end of string (permanently downloaded font).
        */
        for (lpDL=lpPFM; *lpDL && *lpDL != ','; ++lpDL)
            ;
  
        /*  Turn comma (if found) into NULL.
        */
        if (*lpDL == ',')
            *(lpDL++) = '\0';
  
        DBGsoftfont(("loadSoftFont(): id=%d, pfm=%ls, dl=%ls\n",
        lpFT->fontID, lpPFM, lpDL));
  
        /*  Open PFM file.
        */
        if ((hFile = _lopenp(lpPFM, OF_READ)) > 0)
        {
            /*  File successfully opened.  Read FONTINFO.
            */
  
            /* make sure seeks go right */
            lpFT->lPCMOffset=0L;
  
            /* grab the header stuff */
            err = _lread(hFile, lpBuf, sizeof(PFMSHORTHEADER));
  
            /* if no error, grab the PFM! */
            if (err==sizeof(PFMSHORTHEADER))
                fontLoaded=loadFontPFM(hFile,lpFontInfo,orient,lpFT,lpDL);
            else
                fontLoaded=FALSE;
  
            DBGsoftfont(("loadSoftFont(): %ls: fontLoaded=%s\n",
            lpPFM, fontLoaded ? "TRUE" : "FALSE"));
  
            _lclose (hFile);
        }
    #ifdef LOCAL_DEBUG
        else {
            DBGerr(("loadSoftFont(): *failed* to open %ls\n", lpPFM));
        }
    #endif
    }
  
    lpFT->offset = -1;
    lpFT->indPFMPath = -1;
    lpFT->indDLPath = -1;
    lpFT->indPFMName = -1;
    lpFT->indDLName = -1;
  
    /*  HACK for the Z cartridge -- no cartridge here, only
    *  soft fonts.  Ditto for typographic quotes.
    */
    lpFT->ZCART_hack = FALSE;
    lpFT->QUOTE_hack = FALSE;
  
    if (fontLoaded && loadUniqueName(lpPFM, lpFT, fnt_pfmname))
    {
        if (*lpDL && !loadUniqueName(lpDL, lpFT, fnt_dlname))
        {
            DBGerr(("loadSoftFont(): failed to load download file name\n"));
            lpLS->status = FM_STRINGALLOC_FAIL;
            fontLoaded = FALSE;
        }
    }
    else if (fontLoaded)
    {
        DBGerr(("loadSoftFont(): failed to load pfm file name\n"));
        lpLS->status = FM_STRINGALLOC_FAIL;
        fontLoaded = FALSE;
    }
  
#ifdef DEBUG_FUNCT
    DB(("Exiting loadSoftFont\n"));
#endif
    return (fontLoaded);
}
  
/*
*  initLoadCartridgeFont
*
*  initializes loading a font from a cartridge defined by a PCM file.
*
*  PCM file names appear in cartridgen entries in WIN.INI.
*
*/
LOCAL void initLoadCartridgeFont(lpLS, lpEnviron)
LPLFS lpLS;
LPPCLDEVMODE lpEnviron;
{
    int i,j;
#ifdef DEBUG_FUNCT
    DB(("Entering initLoadCartridge\n"));
#endif
  
    DBGentry(("initLoadCartridge(%lp,%lp)\n",lpLS,lpEnviron));
  
    if (lpLS->status)
        return;
  
    lpLS->currentCart=0;
    for (i=j=0; i<DEVMODE_MAXCART && i<lpEnviron->numCartridges; i++)
        lpLS->nextCFont[j++] = lpEnviron->cartIndex[i];
  
    lpLS->numCartridges=j;
  
    lpLS->currentCart=-1;   /* fh of PCM file */
  
    lpLS->lastRFont=lpLS->nextRFont=-1;     /* use for name indices */
  
    DBGcartridge(("number of 3rd party carts=%d\n",j));
  
#ifdef DEBUG_FUNCT
    DB(("Exiting initLoadCartridge\n"));
#endif
}
  
/*
*  loadCartridgeFont
*
*  loads a font from a cartridge PCM file
*/
  
LOCAL BOOL loadCartridgeFont(lpFontInfo, orient, lpLS, lpFT)
LPFONTINFO lpFontInfo;
short orient;
LPLFS lpLS;
LPFNT lpFT;
{
    long err;
    int hF,iT;
    long lSeek;
    char *pch, *pch2;
    char szPCMName[64];
    PCMHEADER pcmheader;
    BOOL fontLoaded=FALSE;
    LPSTR lpBuf=(LPSTR) lpFT->workBuf;
    static char szVar[sizeof(CART_LABEL)+6]=CART_LABEL;
    HANDLE hPCM;
    static long lMax;
  
#ifdef DEBUG_FUNCT
    DB(("Entering loadCartridgeFont\n"));
#endif
    DBGentry((
    "loadCartridgeFont(%lp,%d,%lp,%lp)\n",lpFontInfo,orient,lpLS,lpFT));
  
    if (lpLS->status)
        return FALSE;
  
    GetNextCartridge:
  
    /* do we have an open PCM file? */
  
    while (lpLS->currentCart==-1)
    {
        if (lpLS->numCartridges)
        {
            /* try opening next cartridge */
            /* get the cartindex */
            iT=lpLS->nextCFont[--lpLS->numCartridges];
  
            DBGcartridge(("loading file for cartindex %d\n",iT));
  
            if (iT<0)
                iT=-iT;
  
            if (hPCM = FindResource(hLibInst,(LPSTR)(DWORD)(WORD)iT,
                (LPSTR)(DWORD)PCMFILE))
            {
                /* use the driver as the PCM file!!!  Since we store the
                * offset of the PFM it's ok that the PCM header is not
                * at the beginning of the file...
                */
                GetModuleFileName(hLibInst,szPCMName,sizeof(szPCMName));
                hF = AccessResource(hLibInst,hPCM);
                if (hF < 0)
                    continue;
            }
            else
            {
                /* cartridge not in resources, get PCM filename from
                * WIN.INI and use that file
                */
                /* I do believe we must have cartindex > 0 since
                * 0 is used for "None", so do a semi-brain-dead conversion.
                */
                for (pch=szVar+sizeof(CART_LABEL)-1; iT; iT/=10,pch++)
                    *pch = (char)(iT%10) + '0';
  
                /* null terminate */
                *pch--=0;
  
                /* reverse the integer string if necessary (loop above does things
                * a little backwards...
                */
                for (pch2=szVar+sizeof(CART_LABEL)-1; pch2<pch; pch2++,pch--)
                {
                    (char)iT = *pch2;
                    *pch2 = *pch;
                    *pch = (char)iT;
                }
  
                // read the PCM filename from the font installer's WIN.INI setting
  
                GetProfileString(lpLS->appName,szVar,"",
                szPCMName,sizeof(szPCMName));
  
                DBGcartridge(("[%ls] %ls = %ls\n",(LPSTR)lpLS->appName,
                (LPSTR)szVar,(LPSTR)szPCMName));
  
                /* did we get anything? */
                if (!szPCMName[0])
                    continue;
  
                /* try opening the file.  If it didn't work, blow it off and try
                * for the next one.
                */
                hF = _lopenp(szPCMName, OF_READ);
                if (hF<=0)
                {
                    DBGerr(("failed to open %ls",(LPSTR)szPCMName));
                    continue;
                }
            }
  
        }
        else
        {
            DBGcartridge(("no more 3rd party cartridges\n"));
            return FALSE;
        }
  
        /* try reading the header */
        if (_lread(hF,(LPSTR)&pcmheader,sizeof(PCMHEADER))
            != sizeof(PCMHEADER)
            || pcmheader.pcmMagic != PCM_MAGIC      /* vfy its a PCM */
            || pcmheader.pcmVersion != PCM_VERSION)
        {
            DBGerr(("header read failure\n"));
            _lclose(hF);
            continue;
        }
  
        lMax = _llseek(hF,0L,1) - (long)sizeof(PCMHEADER)
        + pcmheader.pcmSize;
  
        DBGcartridge(("Cartridge: PFMList=%ld\n",pcmheader.pcmPFMList));
  
        /* seek to first PCM in file */
        _llseek(hF,pcmheader.pcmPFMList-(long)sizeof(PCMHEADER),1);
  
        lpLS->currentCart=hF;
  
    }
  
    /* get PCM file handle */
    hF=lpLS->currentCart;
  
    /* get the file offset of the beginning of this PFM */
    lSeek=lpFT->lPCMOffset=_llseek(hF,0L,1);
  
    DBGcartridge(("beginning of this PFM is %ld, ",lSeek));
  
    err=_lread(hF,lpBuf,sizeof(PFMSHORTHEADER));
  
    /* an error? close this cartridge and look for the next one... */
    if (err!=sizeof(PFMSHORTHEADER))
    {
        DBGerr(("failed to read PFM header\n"));
  
        _lclose(hF);
  
        /* remember to get a new file name */
        lpLS->nextRFont=lpLS->lastRFont=lpLS->currentCart=-1;
        goto GetNextCartridge;
    }
  
    /* remember where the end of the PFM is */
    lSeek+=((PFMSHORTHEADER FAR *)lpBuf)->dfSize;
  
    DBGcartridge(("beginning of next PFM is %ld\n",lSeek));
  
    /* loadFontPFM() _might_ set these hack flags, set defaults now */
  
    lpFT->ZCART_hack=lpFT->QUOTE_hack=FALSE;
  
    /*** Tetra begin ***/
    lpFT->scaleInfo.scalable = FALSE;
    lpFT->scaleInfo.emMasterUnits = 0;
    /*** Tetra end ***/
  
    /* load the PFM from the file */
  
    fontLoaded = loadFontPFM(hF,lpFontInfo,orient,lpFT,(LPSTR)0L);
  
    /* if we don't want this font, go see if there is another */
  
    if (fontLoaded)
    {
        lpFT->indDLPath=-1;
        lpFT->indDLName=-1;
        lpFT->offset=-1;
  
        if (lpLS->nextRFont==-1)
        {
            /* first font in a given PCM, need to add the name in szPCMName */
            /* initialize name indices... */
            lpFT->indPFMPath=-1;
            lpFT->indPFMName=-1;
  
            /* allocate space for the name of PCM file */
  
            if (loadUniqueName(szPCMName,lpFT,fnt_pfmname))
            {
                lpLS->nextRFont=lpFT->indPFMName;
                lpLS->lastRFont=lpFT->indPFMPath;
            }
            else
            {
                lpLS->status=FM_STRINGALLOC_FAIL;
                fontLoaded=FALSE;
            }
        }
        else
        {
            /* szPCMName does not contain a valid filename, so we'll use the
            * indices stored in lpLS
            */
            lpFT->indPFMName=lpLS->nextRFont;
            lpFT->indPFMPath=lpLS->lastRFont;
        }
    }
  
    if (lMax && lMax <= lSeek)
    {
        /* get a new file next time around
        */
        _lclose(hF);
        lpLS->nextRFont=lpLS->lastRFont=lpLS->currentCart=-1;
    }
    else
    {
        /* find the next PFM */
        _llseek(hF,lSeek,0);
    }
  
    if (!fontLoaded)
        goto GetNextCartridge;
  
    DBGentry(("loadCartridgeFont returning TRUE...\n"));
  
#ifdef DEBUG_FUNCT
    DB(("Exiting loadCartridgeFont\n"));
#endif
    return TRUE;
}
  
  
/*  initLoadResourceFont
*
*  Setup to load font information from the resource files.  These fonts
*  the the ROM and cartridge fonts.  We will load in a directory file
*  that contains only top-level FONTINFO for all the available fonts.
*/
LOCAL void initLoadResourceFont(lpLS, lpEnviron)
LPLFS lpLS;
LPPCLDEVMODE lpEnviron;
{
#ifdef DEBUG_FUNCT
    DB(("Entering initLoadResourceFont\n"));
#endif
  
    DBGentry(("initLoadResourceFont(%lp,%lp)\n", lpLS, lpEnviron));
  
    if (lpLS->status)
        return;
  
    lpLS->nextRFont = lpEnviron->romind;
    lpLS->lastRFont = lpEnviron->romind + lpEnviron->romcount - 1;
  
#if defined(CARTS_IN_RESOURCE)  /*--------------------------------------*/
  
    for (ind = 0;
        (ind < DEVMODE_MAXCART) && (ind < lpEnviron->numCartridges);
        ++ind)
    {
        lpLS->nextCFont[ind] = lpEnviron->cartind[ind];
        lpLS->lastCFont[ind] =
        lpEnviron->cartind[ind] + lpEnviron->cartcount[ind] - 1;
    }
    lpLS->numCartridges = ind;
    lpLS->currentCart = 0;
  
#endif  /*-- defined(CARTS_IN_RESOURCE) --------------------------------*/
#ifdef DEBUG_FUNCT
    DB(("Exiting initLoadResourceFont\n"));
#endif
}
  
  
/*  loadResourceFont
*
*  Load the next resource font.  If we've loaded them all, return false.
*/
LOCAL BOOL
loadResourceFont(LPFONTINFO lpFontInfo, HANDLE hModule, short orient,
LPLFS lpLS, LPFNT lpFT) {
  
    LPPFMHEADER lpPFM;
    BOOL loadit = FALSE;
    int thisFont = -1, len;
    HANDLE hResInfo, hResData;
    char sepchar, fontProfile[256];
    LPSTR lpFontProfile, lpFaceName,
  
    /*** Tetra begin ***/
    lpFPdup;
    BOOL pound = FALSE;
    /*** Tetra end ***/
  
#if defined(CARTS_IN_RESOURCE)  /*--------------------------------------*/
    BOOL isZCART1 = FALSE;
    BOOL isZCART1A = FALSE;
    BOOL isS2CART = FALSE;
#endif  /*-- defined(CARTS_IN_RESOURCE) --------------------------------*/
  
#ifdef DEBUG_FUNCT
    DB(("Entering loadResourceFont\n"));
#endif
    DBGentry(("loadResourceFont(%lp,%d,%d,%lp,%lp)\n",
    lpFontInfo, (HANDLE)hModule, orient, lpLS, lpFT));
  
    if (lpLS->status)
        return FALSE;
  
    lpFT->lPCMOffset=0L;        /* don't offset; not in PCM */
  
    do {
  
        /*  Determine next font to load.  Note that external resource fonts
        *  will be skipped since current = 1 and last = 0
        */
  
#if defined(CARTS_IN_RESOURCE)  /*--------------------------------------*/
  
        if (((i = lpLS->currentCart) < lpLS->numCartridges &&
            lpLS->nextCFont[i] <= lpLS->lastCFont[i]) ||
            ((i = ++lpLS->currentCart) < lpLS->numCartridges &&
        lpLS->nextCFont[i] <= lpLS->lastCFont[i])) {
  
            /*  Load cartridge font.
            */
            thisFont = lpLS->nextCFont[lpLS->currentCart];
  
            /*  HACK for the Z cartridge.  We have to detect the
            *  Z cartridge and set a special flag.  The fonts in
            *  the Z cartridge are offset 0.017 inch to the right.
            *  On output, we'll adjust for this.
            */
            if (thisFont >= 15 && thisFont <= 31)
                isZCART1 = TRUE;
            else
                isZCART1 = FALSE;
  
            if (thisFont >= 32 && thisFont <= 48)
                isZCART1A = TRUE;
            else
                isZCART1A = FALSE;
  
            /*  Another hack for typographic quotes.  We want to shift
            *  out to USASCII to get the good quotes when the font is
            *  in ECMA-94.  This effects the Z and S2 cartridges.
            */
            if (thisFont >= 134 && thisFont <= 138)
                isS2CART = TRUE;
            else
                isS2CART = FALSE;
  
            len = LoadString(hModule,
            CART_ESC_BASE+thisFont, fontProfile, sizeof(fontProfile));
            ++lpLS->nextCFont[lpLS->currentCart];
        }
        else
  
#endif  /*-- defined(CARTS_IN_RESOURCE) --------------------------------*/
  
            if (lpLS->nextRFont <= lpLS->lastRFont) {
  
                /*  Load memory resident font.
                */
                thisFont = lpLS->nextRFont;
                len = LoadString(hModule,
                ROM_ESC_BASE+thisFont, fontProfile, sizeof(fontProfile));
                ++lpLS->nextRFont;
  
            } else {
  
                /*  No more font resources to load.
                */
                return FALSE;
            }
  
        if (!len) {
            DBGerr(("loadResourceFont(): could *not* load profile string %d\n",
            thisFont));
            lpLS->status = FM_RESOURCE_FAIL;
            return FALSE;
        }
  
        /*  Extract the number of the resource from the font profile.
        */
        lpFontProfile = (LPSTR) fontProfile;
        sepchar = *lpFontProfile++;
  
        thisFont = GetInt((LPSTR FAR *)&lpFontProfile, sepchar);
        lpFT->offset = thisFont;
  
        if (*lpFontProfile != sepchar) {
            DBGerr((
            "loadResourceFont(): fontProfile string too short or out of sync\n"));
            lpLS->status = FM_RESOURCE_FAIL;
            return FALSE;
        }
  
        /*  Extract "portrait" or "landscape" from the font profile.
        */
  
        while ((*(++lpFontProfile) != sepchar) && (*lpFontProfile)) {
  
            if ((*lpFontProfile == 'p') || (*lpFontProfile == 'P')) {
                if (orient == DMORIENT_PORTRAIT)
                    loadit = TRUE;
            } else if ((*lpFontProfile == 'l') || (*lpFontProfile == 'L')) {
                if (orient == DMORIENT_LANDSCAPE)
                    loadit = TRUE;
            }
        }
  
        /*  Load font if orientation == dontcare.
        */
        if (orient == 0)
            loadit = TRUE;
  
        /*** Tetra begin ***/
        lpFT->scaleInfo.scalable = FALSE;
        lpFT->scaleInfo.emMasterUnits = 0;
        /*** Tetra end ***/
  
        /*  Advance lpFontProfile so it points to the escape string
        *  in the profile string, and extract the symbol set from
        *  the escape string.  If this escape string already exists
        *  in the fontSummary struct, then DO NOT load the font.
        */
        if (loadit) {
  
            if (*lpFontProfile++ != sepchar) {
  
                DBGerr(("loadResourceFont(): fontProfile string too short or out of sync\n"));
                lpLS->status = FM_RESOURCE_FAIL;
                return FALSE;
            }
  
            if (!(*lpFontProfile) || dupEscape(lpFT, lpFontProfile))
                loadit = FALSE;
  
            /*** Tetra begin ***/
            lpFPdup = lpFontProfile;
            while (!(pound = (*lpFPdup == '#')) && (*lpFPdup))
                *lpFPdup++;
  
            if (pound)
            {
                lpFT->scaleInfo.scalable = TRUE;
                lpFT->scaleInfo.emMasterUnits = 8782;
            }
  
            if (loadit)
                lpFT->symbolSet = ssFromEscape(lpFontProfile);
        }
  
    } while (!loadit);
  
  
    /*  Find the font resource.
    */
    if (!(hResInfo = FindResource(hModule, (LPSTR)(long)thisFont,
    (LPSTR)(long)MYFONT))) {
  
        DBGerr(("loadResourceFont(): Could not *find* resource %d\n",
        thisFont));
        lpLS->status = FM_RESOURCE_FAIL;
        return FALSE;
    }
  
    /*  Load (actually, only locate) font resource.
    */
    if (!(hResData = LoadResource(hModule, hResInfo))) {
  
        DBGerr(("loadResourceFont(): Could not *load* resource\n"));
        lpLS->status = FM_RESOURCE_FAIL;
        return FALSE;
    }
  
    /*  Lock (and load) font resource.
    */
    if (!(lpPFM = (LPPFMHEADER)LockResource(hResData))) {
  
        DBGerr(("loadResourceFont(): Could not *lock* resource\n"));
        FreeResource(hResData);
        lpLS->status = FM_RESOURCE_FAIL;
        return FALSE;
    }
  
    /*  Load into caller's copy.
    */
    lmemcpy((LPSTR)lpFontInfo, (LPSTR)&lpPFM->dfType, sizeof(FONTINFO));
  
    if (!lpPFM->dfFace) {
  
        DBGerr(("loadResourceFont(): font does not have a name\n"));
        lpLS->status = FM_STRINGALLOC_FAIL;
  
    } else {
  
        /*  Get font face name.
        */
        lpFaceName = (LPSTR)lpPFM + lpPFM->dfFace;
  
        /*  Load name of font and the escape sequence for invoking the font
        *  into font name table.
        */
        if (!loadUniqueName(lpFaceName, lpFT, fnt_fontname) ||
        !loadUniqueName(lpFontProfile, lpFT, fnt_escape)) {
  
            DBGerr(("loadResourceFont(): could *not* load font name or escape sequence\n"));
            lpLS->status = FM_STRINGALLOC_FAIL;
        }
    }
  
    /*  Free up the font resource.
    */
    GlobalUnlock(hResData);
    FreeResource(hResData);
  
    /*  ROM and cartridge fonts do not use soft fonts.
    */
    lpFT->indPFMPath = -1;
    lpFT->indDLPath = -1;
    lpFT->indPFMName = -1;
    lpFT->indDLName = -1;
    lpFT->fontID = -1;
    lpFT->memUsage = 0L;
  
#if defined(CARTS_IN_RESOURCE)  /*--------------------------------------*/
  
    /*  HACK for the Z cartridge -- the fonts are offset 0.017 inch
    *  to the right.  We detect the Z cartridge and, on output,
    *  will shift 0.017 inch to the left.
    */
    lpFT->ZCART_hack = isZCART1 || isZCART1A;
  
    /*  HACK for typgraphic quotes -- we want to use ECMA-94 for
    *  everything except the quotes, which we want to get from
    *  USASCII, set a flag indicating we should shift-out to the
    *  secondary font to get typographic quotes.
    */
    lpFT->QUOTE_hack = isZCART1A || isS2CART;
  
#else   /*-- defined(CARTS_IN_RESOURCE) --------------------------------*/
  
    lpFT->ZCART_hack = lpFT->QUOTE_hack = FALSE;
  
#endif  /*-- defined(CARTS_IN_RESOURCE) --------------------------------*/
  
#ifdef DEBUG_FUNCT
    DB(("Exiting loadResourceFont\n"));
#endif
    return (!lpLS->status);
}
  
  
/*  dupEscape
*
*  Traverse the existing fonts looking for an escape which matches
*  the passed-in escape string.  Return TRUE if a match is found.
*/
LOCAL BOOL dupEscape(lpFT, lpEscape)
LPFNT lpFT;
LPSTR lpEscape;
{
    LPSTR lpNMTBL;
    BOOL dupFound = FALSE;
    short ind, len;
  
#ifdef DEBUG_FUNCT
    DB(("Entering dupEscape\n"));
#endif
    DBGentry(("dupEscape(%lp,%lp)\n", lpFT, lpEscape));
  
    len = lpFT->numExistingNames;
  
    /*  Lock down name table.
    */
    if (!(lpNMTBL = GlobalLock(lpFT->hNMTBL)))
    {
        DBGerr(("dupEscape(): failed to lock name table\n"));
        return FALSE;
    }
  
    /*  Traverse the name table looking for a duplicate escape.  Note
    *  that this is not completely accurate -- if a font had a name
    *  identical to an escape string (probability of happening??) we
    *  would erroneously consider this a valid match.
    */
    for (ind = 0; ind < len; ++ind)
    {
        if (!lstrcmpi(lpEscape, &lpNMTBL[lpFT->indExistingName[ind]]))
        {
            dupFound = TRUE;
            break;
        }
    }
  
    GlobalUnlock (lpFT->hNMTBL);
    lpNMTBL = 0L;
  
#ifdef DEBUG_FUNCT
    DB(("Exiting dupEscape\n"));
#endif
    return (dupFound);
}
  
  
/*  loadUniqueName
*
*  Locate an existing copy of the name or load a new one.
*/
LOCAL BOOL loadUniqueName(lpName, lpFT, whoseName)
LPSTR lpName;
LPFNT lpFT;
FNTNameList whoseName;
{
    LPSTR lpNMTBL, s;
    short ind, len, indName;
    char savechar;
  
#ifdef DEBUG_FUNCT
    DB(("Entering loadUniqueName\n"));
#endif
    DBGentry(("loadUniqueName(%lp,%lp,%d): %ls\n",
    lpName, lpFT, whoseName, lpName));
  
    /*  Initialize index.  For file names (PFM and DL), strip off the
    *  path and call loadUniqueName() again to load the path name
    *  separately.
    */
    switch (whoseName)
    {
        default:
            return FALSE;
        case fnt_fontname: lpFT->indName = -1; break;
        case fnt_escape: lpFT->indEscape = -1; break;
        case fnt_pfmpath: lpFT->indPFMPath = -1; break;
        case fnt_dlpath: lpFT->indDLPath = -1; break;
  
        case fnt_pfmname:
            lpFT->indPFMPath = -1;
            lpFT->indPFMName = -1;
            goto strip_path;
        case fnt_dlname:
            lpFT->indDLPath = -1;
            lpFT->indDLName = -1;
            strip_path:
            /*  Strip the path off of the file name and store it
            *  separately -- do this because the path is normally
            *  the same, so why repeat it?
            */
            for (s = lpName + lstrlen(lpName);
                (s > lpName) && (s[-1] != ':') && (s[-1] != '\\'); --s)
                ;
  
            if (s > lpName)
            {
                savechar = *s;
                *s = '\0';
                if (!loadUniqueName(lpName, lpFT,
                    (whoseName == fnt_pfmname) ? fnt_pfmpath : fnt_dlpath))
                {
                    DBGerr(("loadUniqueName(): could not load path %ls\n",
                    lpName));
                    *s = savechar;
                    return FALSE;
                }
                *s = savechar;
                lpName = s;
            }
    }
  
    len = lpFT->numExistingNames;
  
    /*  Lock down name table.
    */
    if (!(lpNMTBL = GlobalLock(lpFT->hNMTBL)))
    {
        DBGerr(("loadUniqueName(): failed to lock name table\n"));
        return FALSE;
    }
  
    /*  Traverse the table looking for a duplicate name -- if it exists,
    *  then we will not add a new name.
    */
    for (ind = 0; ind < len; ++ind)
    {
        if (!lstrcmpi(lpName, &lpNMTBL[lpFT->indExistingName[ind]]))
        {
            indName = lpFT->indExistingName[ind];
            break;
        }
    }
  
    if (ind == len)
    {
        /*  Name does not yet exist in font name table.
        */
        if (len < FNTBL_MAXNM)
        {
            /*  There is room in the array of indices to names.
            */
            if ((ind = lstrlen(lpName) + 1) > lpFT->availMem)
            {
                /*  Not enough room in table for name, we have
                *  to allocate more space.
                */
                HANDLE tmp;
  
                /*  Unlock the table for realloc.
                */
                GlobalUnlock (lpFT->hNMTBL);
                lpNMTBL = 0L;
  
                /*  Limit checks.
                */
                if ((ind > lpFT->availMem + MEM_BLOCK) ||
                    (lpFT->tblSize > FNTBL_MAXLEN))
                {
                    DBGerr(("loadUniqueName(): name or table too large!\n"));
                    return FALSE;
                }
  
                if (tmp = GlobalReAlloc(lpFT->hNMTBL,
                    GlobalSize(lpFT->hNMTBL) + (DWORD)MEM_BLOCK,
                    GMEM_MOVEABLE))
                {
                    lpFT->hNMTBL = tmp;
                    lpFT->availMem += MEM_BLOCK;
                    DBGloadUniqueName(("loadUniqueName() %d allocated, %d free\n",
                    MEM_BLOCK, lpFT->availMem));
                }
                else
                {
                    DBGerr((
                    "loadUniqueName(): failed to ReAlloc name table\n"));
                    return FALSE;
                }
  
                if (!(lpNMTBL = GlobalLock(lpFT->hNMTBL)))
                {
                    DBGerr(("loadUniqueName(): failed to lock name table\n"));
                    return FALSE;
                }
            }
  
            /*  Add new name.
            */
            indName = lpFT->indExistingName[len] = lpFT->tblSize;
            lmemcpy((LPSTR) &lpNMTBL[indName], lpName, ind);
            lpFT->tblSize += ind;
            lpFT->availMem -= ind;
            ++lpFT->numExistingNames;
  
            /*  Unlock table.
            */
            GlobalUnlock (lpFT->hNMTBL);
            lpNMTBL = 0L;
        }
        else
        {
            /*  Not enough room in the array of indices to names.
            */
            GlobalUnlock (lpFT->hNMTBL);
            lpNMTBL = 0L;
            DBGerr(("loadUniqueName(): array of name ptrs overflow\n"));
            return FALSE;
        }
    }
    else
    {
        /*  We didn't add a new name, so just unlock the table.
        */
        GlobalUnlock (lpFT->hNMTBL);
        lpNMTBL = 0L;
    }
  
    switch (whoseName)
    {
        case fnt_fontname: lpFT->indName = indName; break;
        case fnt_escape: lpFT->indEscape = indName; break;
        case fnt_pfmpath: lpFT->indPFMPath = indName; break;
        case fnt_dlpath: lpFT->indDLPath = indName; break;
        case fnt_pfmname: lpFT->indPFMName = indName; break;
        case fnt_dlname: lpFT->indDLName = indName; break;
    }
  
#ifdef DEBUG_FUNCT
    DB(("Exiting loadUniqueName\n"));
#endif
    return TRUE;
}
  
  
/*  ssFromEscape
*
*  Parse the escape string (the string we send to the printer to invoke
*  the font) looking for the symbol set evocation.  If it exists, then
*  translate it and return our internal SYMBOLSET representation -- if
*  it does not exist, return Generic8.
*/
LOCAL SYMBOLSET ssFromEscape(lpEscape)
LPSTR lpEscape;
{
    SYMBOLSET ss;
    BOOL ssLocated = FALSE;
    LPSTR c = lpEscape;
    short num, letter;
#ifdef DEBUG_FUNCT
    DB(("Entering ssFromEscape\n"));
#endif
  
    DBGentry(("ssFromEscape(%lp): %ls\n", lpEscape, lpEscape));
  
    do  {
        /*  Advance to start of escape.
        */
        for (; *c && (*c != '\033'); ++c)
            ;
  
        /*  Open paren indicates possible start of
        *  symbol set definition.
        */
        if (*c && *(++c) && (*c == '('))
        {
            /*  Look for number and/or letter.
            */
            if (*c && *(++c) && (*c >= '0') && (*c <= '9'))
            {
                /*  First thing is a number, second should be
                *  a letter.
                */
                for (num = 0; *c && (*c >= '0') && (*c <= '9'); ++c)
                    num = (num * 10) + (*c - '0');
                if (*c && (*c >= 'A') && (*c <= 'Z'))
                    ssLocated = TRUE;
            }
            else
            {
                /*  The first thing was not a number (zero may
                *  be implied), look for a letter.
                */
                num = 0;
                if (*c && (*c >= 'A') && (*c <= 'Z'))
                    ssLocated = TRUE;
            }
            letter = *c;
        }
  
        if (!ssLocated && *c)
            ++c;
  
    } while (!ssLocated && *c);
  
  
    /*  Convert values and translate to character set.
    *  (formula from HP Technical Documentation)
    */
    if (ssLocated)
    {
        num = (num * 32) + letter - 64;
        switch (num)
        {
            case (8*32)+'U'-64:     /* Roman-8 */
                ss = epsymRoman8;
                break;
            case (8*32)+'K'-64:     /* Kana-8 */
                ss = epsymKana8;
                break;
            case (8*32)+'M'-64:     /* Math-8 */
                ss = epsymMath8;
                break;
            case (0*32)+'U'-64:     /* USASCII */
                ss = epsymUSASCII;
                break;
            case (0*32)+'B'-64:     /* Line Draw */
                ss = epsymLineDraw;
                break;
            case (0*32)+'A'-64:     /* Math Symbols */
                ss = epsymMathSymbols;
                break;
            case (1*32)+'U'-64:     /* US Legal */
                ss = epsymUSLegal;
                break;
            case (0*32)+'E'-64:     /* Roman Extension */
                ss = epsymRomanExt;
                break;
            case (0*32)+'D'-64:     /* ISO Denmark/Norway */
                ss = epsymISO_DenNor;
                break;
            case (1*32)+'E'-64:     /* ISO United Kingdon */
                ss = epsymISO_UK;
                break;
            case (0*32)+'F'-64:     /* ISO France */
                ss = epsymISO_France;
                break;
            case (0*32)+'G'-64:     /* ISO German */
                ss = epsymISO_German;
                break;
            case (0*32)+'I'-64:     /* ISO Italy */
                ss = epsymISO_Italy;
                break;
            case (0*32)+'S'-64:     /* ISO Sweden/Finland */
                ss = epsymISO_SwedFin;
                break;
            case (1*32)+'S'-64:     /* ISO Spain */
                ss = epsymISO_Spain;
                break;
            case (11*32)+'Q'-64:    /* ECMA-94 */
            case (0*32)+'N'-64: /* ECMA-94 */
                ss = epsymECMA94;
                break;
            default:
                ss = epsymGENERIC8;
                break;
        }
    }
    else
        ss = epsymGENERIC8;
  
#ifdef DEBUG_FUNCT
    DB(("Exiting ssFromEscape\n"));
#endif
    return (ss);
}
  
  
/*  expandLockFontSum
*
*  Add dSize bytes to the fontSummary struct.
*/
LOCAL LPFONTSUMMARYHDR expandLockFontSum(lpHFS, lpLS, dSize)
LPHANDLE lpHFS;
LPLFS lpLS;
DWORD dSize;
{
    LPFONTSUMMARYHDR lpFontSummary;
    HANDLE hFontSummary = *lpHFS;
    DWORD dAlloc;
  
#ifdef DEBUG_FUNCT
    DB(("Entering expandLockFontSum\n"));
#endif
    DBGentry(("expandLockFontSum(%lp,%lp,%ld): freemem=%ld, handle=%d\n",
    lpHFS, lpLS, dSize, lpLS->freemem, (WORD)hFontSummary));
  
    if (dSize <= lpLS->freemem)
    {
        /*  Enough space in the structure right now, we don't
        *  have to do a memalloc.
        */
        lpLS->freemem -= dSize;
    }
    else
    {
        /*  Allocate memory in blocks so we won't call GlobalReAlloc
        *  so much when we're loading lots of fonts.
        */
        dAlloc = MEM_BLOCK - lpLS->freemem;
  
        if (dAlloc < dSize)
            dAlloc = dSize;
  
        lpLS->freemem = 0;
  
        DBGexpandLockFS(("expandLockFontSum(): allocating %ld bytes\n",
        (DWORD) dAlloc));
  
        if (!(hFontSummary = GlobalReAlloc(hFontSummary,
            GlobalSize(hFontSummary) + dAlloc,
            GMEM_MOVEABLE | GMEM_LOWER | GMEM_DDESHARE)))
        {
            DBGerr(("expandLockFontSum(): could *not* realloc fontSummary\n"));
            return (0L);
        }
  
        lpLS->freemem = dAlloc - dSize;
    }
  
    /*  Assign new handle.
    */
    *lpHFS = hFontSummary;
  
    /*  Lock down the fontSummary struct.
    */
    if (!(lpFontSummary = (LPFONTSUMMARYHDR)GlobalLock(hFontSummary)))
    {
        DBGerr(("expandLockFontSum(): could *not* lock fontSummary\n"));
        return (0L);
    }
  
#ifdef DEBUG_FUNCT
    DB(("Exiting expandLockFontSum\n"));
#endif
    return (lpFontSummary);
}
  
  
#ifdef DIALOG_MESSAGES
/*  DlgLotsOfFonts
*
*  Stub dialog proc for displaying messages when we're loading lots
*  of soft fonts.
*/
int FAR PASCAL DlgLotsOfFonts(hWnd, wMsg, wParam, lParam)
HWND hWnd;
unsigned wMsg;
WORD wParam;
LONG lParam;
{
#ifdef DEBUG_FUNCT
    DB(("Entering DlgLotsOfFonts\n"));
#endif
    DBGentry(("DlgLotsOfFonts(%d,%d,%d,%ld)\n",
    (HWND)hWnd, (unsigned)wMsg, (WORD)wParam, lParam));
  
#ifdef DEBUG_FUNCT
    DB(("Entering DlgLotsOfFonts\n"));
#endif
    return FALSE;
}
  
  
/*  UpdateLotsOfFontsDlg
*
*  Update the dialog on the screen informing the user how many soft fonts
*  have been loaded.
*/
LOCAL void UpdateLotsOfFontsDlg(hWnd, lpFT, lpFont, count, totCount,
lpPoint, lpBold, lpItalic)
HWND hWnd;
LPFNT lpFT;
LPFONTINFO lpFont;
WORD count;
WORD totCount;
LPSTR lpPoint;
LPSTR lpBold;
LPSTR lpItalic;
{
    LPSTR lpName;
    WORD percent;
    char str[80];
  
#ifdef DEBUG_FUNCT
    DB(("Entering UpdateLotsOfFontsDlg\n"));
#endif
    DBGentry(("UpdateLotsOfFontsDlg(%d,%lp,%lp,%d,%ls,%ls,%ls)\n",
    (HWND)hWnd, lpFT, lpFont, count, lpPoint, lpBold, lpItalic));
    DBGentry(("                    %ls, %ls, %ls\n",
    lpPoint, lpBold, lpItalic));
  
    if ((percent = count * 100 / totCount) > 100)
        percent = 100;
  
    SetDlgItemInt(hWnd, LOTSFONT_PCNT, percent, FALSE);
  
    /*  Locate name from font name table.
    */
    if (!(lpName = GlobalLock(lpFT->hNMTBL)))
    {
        DBGerr(("UpdateLotsOfFontsDlg(): failed to lock name table\n"));
        return;
    }
    lpName = &lpName[lpFT->indName];
  
    if (lstrlen(lpName) < sizeof(str) - 1)
    {
        lstrcpy(str, lpName);
        lstrcat(str, (LPSTR)" ");
    }
  
    GlobalUnlock(lpFT->hNMTBL);
    lpName = 0L;
  
    if (lstrlen(str) < sizeof(str) - 10)
    {
        /* Avoid lstrcat'ing "0pt" to a scalable font name */
        if (lpFont->dfPixHeight)
        {
            if (itoa((((lpFont->dfPixHeight -
                lpFont->dfInternalLeading)*72+150)/VDPI), &str[lstrlen(str)]) &&
                (lstrlen(str) < sizeof(str) - lstrlen(lpPoint)))
                lstrcat(str, lpPoint);
        }
  
        if ((lpFont->dfWeight > FW_NORMAL) &&
            (lstrlen(str) < sizeof(str) - lstrlen(lpBold)))
        {
            lstrcat(str, lpBold);
        }
  
        if ((lpFont->dfItalic) &&
            (lstrlen(str) < sizeof(str) - lstrlen(lpItalic)))
        {
            lstrcat(str, lpItalic);
        }
  
        SetDlgItemText(hWnd, LOTSFONT_FONT, str);
    }
#ifdef DEBUG_FUNCT
    DB(("Exiting UpdateLotsOfFontsDlg\n"));
#endif
}
#endif
  
  
/***************************************************************************/
/**************************   Debug Procedures   ***************************/
  
#ifdef DEBUG
/*  DBGnameofPitch
*
*  Return name of pitch.
*/
LOCAL char *DBGnameofPitch(pitch)
unsigned pitch;
{
#ifdef DEBUG_FUNCT
    DB(("Entering DBGnameofPitch\n"));
#endif
    switch (pitch)
    {
        case 0:  return "default/fixed";
        case 1:  return "variable";
        default: return "unknown";
    }
#ifdef DEBUG_FUNCT
    DB(("Exiting DBGnameofPitch\n"));
#endif
}
  
  
/*  DBGnameofFamily
*
*  Return name of family.
*/
LOCAL char *DBGnameofFamily(family)
unsigned family;
{
#ifdef DEBUG_FUNCT
    DB(("Entering DBGNameofFamily\n"));
#endif
    switch (family)
    {
        case FF_DONTCARE:   return "dont care";
        case FF_ROMAN:      return "roman";
        case FF_SWISS:      return "swiss";
        case FF_MODERN:     return "modern";
        case FF_SCRIPT:     return "script";
        case FF_DECORATIVE: return "decorative";
        default:        return "unknown";
    }
#ifdef DEBUG_FUNCT
    DB(("Exiting DBGNameofFamily\n"));
#endif
}
  
  
/*  DBGnameofSymbolSet
*
*  Return name of symbol set.
*/
LOCAL char *DBGnameofSymbolSet(ss)
SYMBOLSET ss;
{
#ifdef DEBUG_FUNCT
    DB(("Entering DBGnameofSymbolSet\n"));
#endif
    switch (ss)
    {
        case epsymUserDefined:  return "UserDefined";
        case epsymRoman8:   return "Roman8";
        case epsymKana8:    return "Kana8";
        case epsymMath8:    return "Math8";
        case epsymUSASCII:  return "USASCII";
        case epsymLineDraw: return "LineDraw";
        case epsymMathSymbols:  return "MathSymbols";
        case epsymUSLegal:  return "USLegal";
        case epsymRomanExt: return "RomanExt";
        case epsymISO_DenNor:   return "ISO_DenNor";
        case epsymISO_UK:   return "ISO_UK";
        case epsymISO_France:   return "ISO_France";
        case epsymISO_German:   return "ISO_German";
        case epsymISO_Italy:    return "ISO_Italy";
        case epsymISO_SwedFin:  return "ISO_SwedFin";
        case epsymISO_Spain:    return "ISO_Spain";
        case epsymGENERIC7: return "GENERIC7";
        case epsymGENERIC8: return "GENERIC8";
        case epsymECMA94:   return "ECMA94";
        default:        return "unknown";
    }
#ifdef DEBUG_FUNCT
    DB(("Exiting DBGnameofSymbolSet\n"));
#endif
}
  
  
/*  DBGnameofWeight
*
*  Return name of weight.
*/
LOCAL char *DBGnameofWeight(weight)
short weight;
{
#ifdef DEBUG_FUNCT
    DB(("Entering DBGnameofWeight\n"));
#endif
    switch (weight)
    {
        case FW_DONTCARE:   return "dont care";
        case FW_THIN:       return "thin";
        case FW_EXTRALIGHT: return "extralight";
        case FW_LIGHT:      return "light";
        case FW_NORMAL:     return "normal";
        case FW_MEDIUM:     return "medium";
        case FW_SEMIBOLD:   return "semibold";
        case FW_BOLD:       return "bold";
        case FW_EXTRABOLD:  return "extrabold";
        case FW_HEAVY:      return "heavy";
        default:        return "unknown";
    }
#ifdef DEBUG_FUNCT
    DB(("Exiting DBGnameofWeight\n"));
#endif
}
  
  
/*  DBGdumpFSItem
*
*  Dump the contents of the font summary item.
*/
LOCAL void DBGdumpFSItem(lpFontSummary, ind)
LPFONTSUMMARYHDR lpFontSummary;
short ind;
{
    LPFONTSUMMARY lpSummary = &lpFontSummary->f[ind];
    LPSTR fontNameTable = (LPSTR) &lpFontSummary->f[lpFontSummary->len];
    LPSTR fontName;
  
#ifdef DEBUG_FUNCT
    DB(("Entering DBGdumpFSItem\n"));
#endif
    fontName = &fontNameTable[lpSummary->indName];
    DBMSG(("%ls %lp, ID=%d, ind(%d): nm=%d esc=%d pfm=%d,%d, dl=%d,%d\n",
    fontName, lpSummary, lpSummary->fontID, ind, lpSummary->indName,
    lpSummary->indEscape, lpSummary->indPFMPath, lpSummary->indPFMName,
    lpSummary->indDLPath, lpSummary->indDLName));
    DBMSG(("   hExtMetrics=%d, hWidthTable=%d, hPairKernTable=%d, hTrackKernTable=%d\n",
    lpSummary->hExtMetrics, lpSummary->hWidthTable,
    lpSummary->hPairKernTable, lpSummary->hTrackKernTable));
  
    /*** Tetra begin ***/
    DBMSG(("   ss=%s, offs=%d, indPrvSft=%d, indNxtSft=%d, LRU=%d, mUse=%ld,\n   Z=%d, Q=%d, S=%d, MU=%d\n",
    DBGnameofSymbolSet(lpSummary->symbolSet), lpSummary->offset,
    lpSummary->indPrevSoft, lpSummary->indNextSoft, lpSummary->LRUcount,
    lpSummary->memUsage, lpSummary->ZCART_hack, lpSummary->QUOTE_hack,
    lpSummary->scaleInfo.scalable, lpSummary->scaleInfo.emMasterUnits));
    /*** Tetra end ***/
  
#ifdef DBGmediumdumpfs
    DBMSG(("   dfPitchAndFamily = %d: %s, %s\n",
    lpSummary->dfPitchAndFamily,
    DBGnameofPitch(lpSummary->dfPitchAndFamily & 0x0f),
    DBGnameofFamily(lpSummary->dfPitchAndFamily & 0xf0)));
    DBMSG(("   dfPixHeight = %d\n", lpSummary->dfPixHeight));
    DBMSG(("   dfAvgWidth = %d\n", lpSummary->dfAvgWidth));
    DBMSG(("   dfWeight = %d: %s\n", lpSummary->dfWeight,
    DBGnameofWeight(lpSummary->dfWeight)));
    DBMSG(("   dfItalic = %d\n", (BYTE)lpSummary->dfItalic));
#ifdef DBGlongdumpfs
    DBMSG(("   dfType = %d\n", lpSummary->dfType));
    DBMSG(("   dfPoints = %d\n", lpSummary->dfPoints));
    DBMSG(("   dfVertRes = %d\n", lpSummary->dfVertRes));
    DBMSG(("   dfHorizRes = %d\n", lpSummary->dfHorizRes));
    DBMSG(("   dfAscent = %d\n", lpSummary->dfAscent));
    DBMSG(("   dfUnderline = %d\n", (BYTE)lpSummary->dfUnderline));
    DBMSG(("   dfStrikeOut = %d\n", (BYTE)lpSummary->dfStrikeOut));
    DBMSG(("   dfCharSet = %d\n", (BYTE)lpSummary->dfCharSet));
    DBMSG(("   dfPixWidth = %d\n", lpSummary->dfPixWidth));
    DBMSG(("   dfInternalLeading = %d\n", lpSummary->dfInternalLeading));
    DBMSG(("   dfExternalLeading = %d\n", lpSummary->dfExternalLeading));
    DBMSG(("   dfMaxWidth = %d\n", lpSummary->dfMaxWidth));
    DBMSG(("   dfFirstChar = %d: %c\n",
    (BYTE)lpSummary->dfFirstChar, lpSummary->dfFirstChar));
    DBMSG(("   dfLastChar = %d: %c\n",
    (BYTE)lpSummary->dfLastChar, lpSummary->dfLastChar));
    DBMSG(("   dfDefaultChar = %d: %c\n",
    (BYTE)lpSummary->dfDefaultChar, lpSummary->dfDefaultChar));
    DBMSG(("   dfBreakChar = %d: %c\n",
    (BYTE)lpSummary->dfBreakChar, lpSummary->dfBreakChar));
#endif
#endif
#ifdef DEBUG_FUNCT
    DB(("Exiting DBGdumpFSItem\n"));
#endif
}
  
  
/*  DBGdumpFontNameTable
*
*  Dump the contents of the font name table that resides at the end of
*  the font summary data structure.
*/
LOCAL void DBGdumpFontNameTable(lpFontSummary)
LPFONTSUMMARYHDR lpFontSummary;
{
    LPSTR fontNameTable = (LPSTR) &lpFontSummary->f[lpFontSummary->len];
    short len = lpFontSummary->softfonts - 1;
    short ind, count;
  
#ifdef DEBUG_FUNCT
    DB(("Entering DBGdumpFontName\n"));
#endif
    DBMSG(("   0: "));
  
    for (ind = 0; ind < len; ++ind)
        if (fontNameTable[ind] == '\0')
        {
            count = ind + 1;
            DBMSG(("\n%c%c%c%d: ",
            ((count / 1000 > 0) ? '\0' : ' '),
            ((count / 100 > 0) ? '\0' : ' '),
            ((count / 10 > 0) ? '\0' : ' '),
            count));
        }
        else
        {
            DBMSG(("%c", fontNameTable[ind]));
        }
    DBMSG(("\n"));
#ifdef DEBUG_FUNCT
    DB(("Exiting DBGdumpFontName\n"));
#endif
}
#endif
