/**[f******************************************************************
* fontman.c -
*
* Copyright (C) 1988,1989 Aldus Corporation.
* Copyright (C) 1988-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.  Company confidential.
*
**f]*****************************************************************/
  
/*
* $Header: fontman.c,v 3.890 92/02/06 16:11:56 dtk FREEZE $
*/
  
/*
* $Log:	fontman.c,v $
 * Revision 3.890  92/02/06  16:11:56  16:11:56  dtk (Doug Kaltenecker)
 * Win3.1 Freeze
 * 
 * Revision 3.871  91/12/18  13:52:11  13:52:11  daniels (Susan Daniels)
 * Change parameter to GetEnvironment for Win31 -- use device not port.
 * 
 * Revision 3.870  91/11/08  11:43:40  11:43:40  dtk (Doug Kaltenecker)
 * 3.1 Release Candidate 1
 * 
 * Revision 3.865  91/11/01  13:51:40  13:51:40  dtk (Doug Kaltenecker)
 * Beta release for Windows 3.1
 * 
 * Revision 3.862  91/10/25  13:47:00  13:47:00  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.860  91/10/23  09:48:26  09:48:26  dtk (Doug Kaltenecker)
 * WinWird Release
 * 
 * Revision 3.852  91/10/09  14:59:28  14:59:28  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.850  91/10/04  16:49:40  16:49:40  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE
 * 
 * Revision 3.841  91/10/04  11:05:27  11:05:27  daniels (Susan Daniels)
 * Bug #687:  If version string is > VERSION_LEN, truncate and write to
 * the font summary file instead of just writing 0's.  This causes automatic
 * font summary file updates to occur when the version changes.
 * 
 * Revision 3.840  91/09/28  14:16:59  14:16:59  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE 3 FOR WINWORD
 * 
 * Revision 3.830  91/09/18  16:33:14  16:33:14  dtk (Doug Kaltenecker)
 * RELEASE
 * 
 * Revision 3.822  91/09/16  10:33:42  10:33:42  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE 2 FOR WIN30.
 * 
 * Revision 3.820  91/09/06  14:11:54  14:11:54  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE FOR WINWORD 2.0
 * 
 * Revision 3.813  91/09/03  17:51:42  17:51:42  dtk (Doug Kaltenecker)
 * Added lots of code for chkecking of fontsummary file location
 * for the Grumand network bug.
 * 
 * Revision 3.812  91/08/22  14:31:50  14:31:50  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.807  91/08/08  10:31:07  10:31:07  dtk (Doug Kaltenecker)
 * PREBETA3
 * 
 * Revision 3.802  91/07/22  12:54:02  12:54:02  oakeson (Ken Oakeson)
 * BETA
 * 
 * Revision 3.799  91/07/02  11:51:29  11:51:29  daniels (Susan Daniels)
 * Beta
 * 
 * Revision 3.796  91/06/26  11:25:49  11:25:49  stevec (Steve Claiborne)
 * BETA
 * 
 * Revision 3.790  91/06/11  16:03:05  16:03:05  stevec (Steve Claiborne)
 * Freeze
 * 
 * Revision 3.786  91/06/11  15:43:52  15:43:52  dtk (Doug Kaltenecker)
 * Prettified files!
 * 
* Revision 3.785  91/05/22  14:56:44  14:56:44  stevec (Steve Claiborne)
* Beta version to MS
*
* Revision 3.780  91/05/15  15:56:53  15:56:53  stevec (Steve Claiborne)
* Beta
*
* Revision 3.775  91/04/05  14:30:46  14:30:46  stevec (Steve Claiborne)
* Beta release to MS
*
* Revision 3.770  91/03/25  15:35:46  15:35:46  stevec (Steve Claiborne)
* maintance release
*
* Revision 3.760  91/03/12  07:52:29  07:52:29  stevec (Steve Claiborne)
* Maintance release
*
* Revision 3.755  91/03/04  11:42:15  11:42:15  stevec (Steve Claiborne)
* March 3 freeze
*
* Revision 3.720  91/02/11  09:15:09  09:15:09  stevec (Steve Claiborne)
* Aldus version
*
* Revision 3.711  91/02/08  16:25:37  16:25:37  stevec (Steve Claiborne)
* Added debuging
*
* Revision 3.710  91/02/04  15:47:28  15:47:28  stevec (Steve Claiborne)
* Aldus freeze
*
* Revision 3.700  91/01/19  09:00:09  09:00:09  stevec (Steve Claiborne)
* Release
*
* Revision 3.685  91/01/14  15:43:04  15:43:04  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.681  91/01/14  10:17:21  10:17:21  stevec (Steve Claiborne)
* Updated the copy right stmt.
*
* Revision 3.680  91/01/10  16:16:27  16:16:27  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.670  90/12/14  14:53:52  14:53:52  stevec (Steve Claiborne)
* freeze for 12-14-90 ver. 3.670
*
* Revision 3.665  90/12/10  15:35:36  15:35:36  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.660  90/12/07  14:50:05  14:50:05  stevec (Steve Claiborne)
* Freeze 12-7-90
*
* Revision 3.650  90/11/30  08:12:00  08:12:00  stevec (Steve Claiborne)
* Freeze 3.650, 11-30-90
*
* Revision 3.604  90/11/26  15:32:18  15:32:18  stevec (Steve Claiborne)
* Turned debugging off
*
* Revision 3.603  90/11/14  15:59:53  15:59:53  stevec (Steve Claiborne)
* Fixed bug #80 - a sharing bug.  Moved the font summary file from the
* windows/system directory to the windows dir.   SJC
*
* Revision 3.602  90/10/25  17:13:52  17:13:52  oakeson (Ken Oakeson)
* Deallocated hCharDL (character download) structure
*
*/
  
/***************************************************************************/
/******************************   fontman.c   ******************************/
//
//  Fontman: This module sets up the font summary data used by realize
//  and enumfonts to choose a font.
//
// 18 dec 91    Make parameter to GetEnvironment dependent on Win31.  Win31
//              uses device and win30 uses port.
// 03 oct 91    SD  BUG #687:  if version is longer than 16 chars, truncate
//                      for file header instead of just writing 0's.
// 03 feb 90    KLO Changed lmemcpy() to lstrncpy().
// 07 aug 89    peterbe Changed lstrcmp() to lstrcmpi().
//   1-13-89    jimmat  Reduced # of redundant strings by adding lclstr.h
//   1-30-89    jimmat  Changes to support separate font installer DLL.
//   2-07-89    jimmat  Driver Initialization changes.
//   2-20-89    jimmat  Driver/Font Installer use same WIN.INI section (again)
//
  
//#define DEBUG
  
#include "generic.h"
#include "resource.h"
#define FONTMAN_ENABLE
#define FONTMAN_DISABLE
#include "fontman.h"
#include "fonts.h"
#include "strings.h"
// #include "version.h"
#include "fontpriv.h"
#include "environ.h"
#include "utils.h"
#include "lclstr.h"
  
  
/*  Utilities
*/
#include "lockfont.c"
  
  
/*  Local debug structure (mediumdumpfs must be
*  enabled to get longdumpfs).
*/
#ifdef DEBUG
    #define LOCAL_DEBUG
    #undef DBGdumpfontsummary
#endif
  
#ifdef LOCAL_DEBUG
    #define DBGentry(msg) DBMSG(msg)
    #define DBGerr(msg) DBMSG(msg)
    #define DBGgetfontsum(msg) DBMSG(msg)
    #define DBGfntsumexists(msg) DBMSG(msg)
    #define DBGgetfile(msg) DBMSG(msg)
    #define DBGputfile(msg) DBMSG(msg)
    #define DBGfreefnt(msg) DBMSG(msg)
#else
    #define DBGentry(msg) /*null*/
    #define DBGerr(msg) /*null*/
    #define DBGgetfontsum(msg) /*null*/
    #define DBGfntsumexists(msg) /*null*/
    #define DBGgetfile(msg) /*null*/
    #define DBGputfile(msg) /*null*/
    #define DBGfreefnt(msg) /*null*/
#endif
  
  
typedef struct {
    char version[VERSION_LEN];      /* version of PCL driver */
    char port[PORT_LEN];        /* full name of port */
    WORD fsvers;            /* fontSummary version number */
    WORD envSize;           /* size of PCLDEVMODE struct */
    WORD softfonts;         /* number of softfonts in win.ini */
    WORD numEntries;            /* number of fontSums in file */
    DWORD offsetFirstEntry;     /* offset to directory */
} FSUMFILEHEADER;
  
typedef struct {
    PCLDEVMODE environ;         /* devmode for this fontSummary */
    WORD sizeofFontSum;         /* len of this fontSummary struct */
} FSUMFILEENTRY;
  
  
/*  Forward local procs.
*/
LOCAL HANDLE FontSumExists(LPPCLDEVMODE);
LOCAL HANDLE getFileFontSummary(LPPCLDEVMODE, LPSTR, HANDLE, LPHANDLE);
LOCAL void putFileFontSummary(HANDLE, LPPCLDEVMODE, LPSTR, HANDLE);
LOCAL BOOL SameEnvironments(LPPCLDEVMODE, LPPCLDEVMODE);
LOCAL BOOL mylmemcmp(LPSTR, LPSTR, short);
LOCAL void bldFSumFileName(LPSTR, LPSTR, HANDLE);
LOCAL void _lshuffle(int, DWORD, DWORD, WORD);
WORD FAR PASCAL GetWindowsDirectory(LPSTR,int);
WORD FAR PASCAL GetSystemDirectory(LPSTR,int);
  
  
/*  gHFontSummary is used to share the same fontSummary struct
*  across different instances -- this is safe because the
*  fontSummary struct is used for read-only purposes.
*/
LOCAL HANDLE gHFontSummary = 0;
  
/***************************************************************************/
/**************************   Global Procedures   **************************/
  
  
/*  GetFontSummary
*
*  Lock down the fontSummary in memory if it exists, or see if it exists
*  in a file.  Otherwise, build a new structure.
*/
HANDLE far PASCAL
GetFontSummary(LPSTR lpPortNm, LPSTR lpDeviceNm, LPPCLDEVMODE lpEnviron,
HANDLE hModule) {
  
    PCLDEVMODE environ;
    HANDLE hLS, hFontSummary;
#ifdef DEBUG_FUNCT
    DB(("Entering GetFontSummary\n"));
#endif
  
    DBGentry(("GetFontSummary(%lp,%lp,%lp,%d): %ls, %ls\n",
    lpPortNm, lpDeviceNm, lpEnviron, (HANDLE)hModule,
    lpDeviceNm, lpPortNm));
  
    lmemset((LPSTR)&environ, 0, sizeof(PCLDEVMODE));
  
    /*  Get the environment (pcldevmode) structure.
    */
    if (lpEnviron)
    {
  
        /*  Environment passed in.
        */
        DBGgetfontsum(("GetFontSummary(): environment exists\n"));
        lmemcpy((LPSTR)&environ, (LPSTR)lpEnviron, sizeof(PCLDEVMODE));
  
    }
    else
    {
        /*  Environment not passed in, try to get it from Windows.
        */
        lstrcpy((LPSTR)&environ, lpDeviceNm);
#if defined(WIN31)
        if (!GetEnvironment(lpDeviceNm, (LPSTR)&environ, sizeof(PCLDEVMODE)))
#else
        if (!GetEnvironment(lpPortNm, (LPSTR)&environ, sizeof(PCLDEVMODE)))
#endif
        {
  
            /*  Environment not available, attempt to read it from the
            *  win.ini file or, if that fails, set the default values.
            */
            DBGgetfontsum(("GetFontSummary(): getting default environment\n"));
            MakeEnvironment(&environ, lpDeviceNm, lpPortNm, NULL);
        }
#ifdef LOCAL_DEBUG
        else
        {
            DBGgetfontsum(("GetFontSummary(): got environment from Windows\n"));
        }
#endif
    }
  
    /*  Get/build the fontSummary structure:
    *
    *  1. Attempt to share it with existing identical IC, or
    *  2. Read it from a file (previously created), or
    *  3. Build it from scratch.
    */
    if (!(hFontSummary = FontSumExists(&environ)) &&
    !(hFontSummary = getFileFontSummary(&environ,lpPortNm,hModule,&hLS))) {
  
        /*  Build from scratch -- this may take a while.  Note that after
        *  the call to buildFontSummary(), hLS is invalid.
        */
        if (hFontSummary = buildFontSummary(&environ,hLS,lpPortNm,hModule))
            putFileFontSummary(hFontSummary, &environ, lpPortNm, hModule);
    }
  
#ifdef DEBUG_FUNCT
    DB(("Exiting GetFontSummary\n"));
#endif
    return (gHFontSummary = hFontSummary);
}
  
/*  FreeFontSummary
*
*  If this is the only DC using the font summary information, then
*  erase the fontSummary struct.
*/
HANDLE far PASCAL FreeFontSummary (lpDevice)
LPDEVICE lpDevice;
{
    LPFONTSUMMARYHDR lpFontSummary = 0L;
    short numOpenDC;
    BOOL deleteit = FALSE;
#ifdef DEBUG_FUNCT
    DB(("Exiting FreeFontSummary\n"));
#endif
  
    DBGentry(("FreeFontSummary(%lp): %d\n",
    lpDevice, (HANDLE)lpDevice->epHFntSum));
  
    /*  Lock down fontSummary struct if it exists.
    */
    if (lpFontSummary = lockFontSummary(lpDevice))
    {
        DBGfreefnt(("FreeFontSummary(): fontSummary %d locked\n",
        (HANDLE)lpDevice->epHFntSum));
  
        /*  Decrement count of Display/Information Contexts sharing
        *  this structure -- if this is the last one, then mark the
        *  struct for delete.
        */
        if ((numOpenDC = --lpFontSummary->numOpenDC) <= 0)
        {
            LPFONTSUMMARY lpSummary;
            short ind, len;
  
            for (lpSummary=&lpFontSummary->f[ind=0], len=lpFontSummary->len;
                ind < len; ++ind, ++lpSummary)
            {
                /*  Free the handle to the width table if it exists.
                */
                if (lpSummary->hWidthTable)
                {
                    GlobalFree(lpSummary->hWidthTable);
                    lpSummary->hWidthTable = 0;
                }
  
                /*  Free the handle to the char download info if it exists.
                */
                if (lpSummary->hCharDL)
                {
                    GlobalFree(lpSummary->hCharDL);
                    lpSummary->hCharDL = 0;
                }
            }
  
            /*  Set flag to delete fontSummary after
            *  it is unlocked.
            */
            deleteit = TRUE;
        }
  
        unlockFontSummary(lpDevice);
        lpFontSummary = 0L;
  
        if (deleteit)
        {
            DBGfreefnt(("FreeFontSummary(): fontSummary deleted\n"));
            GlobalFree (lpDevice->epHFntSum);
  
            if (lpDevice->epHFntSum == gHFontSummary)
                /* Zero global fontSummary handle */
                gHFontSummary = 0;
  
            lpDevice->epHFntSum = 0;
        }
    #ifdef LOCAL_DEBUG
        else {
            DBGfreefnt((
            "FreeFontSummary(): fontSummary not deleted, %d DC using it\n",
            numOpenDC));
        }
    #endif
    }
  
    DBGfreefnt(("...end of FreeFontSummary, return 0\n"));
#ifdef DEBUG_FUNCT
    DB(("Exiting FreeFontSummary\n"));
#endif
    return (0);
}
  
/***************************************************************************/
/**************************   Local Procedures   ***************************/
  
  
/*  FontSumExists
*
*  Look for the global fontSummary struct.  If it exists, lock it down and
*  verify that it was built using environment parameters that match those
*  of the environment passed in by the caller.  If the environments match,
*  use the existing fontSummary.  If they don't, return failure.
*/
LOCAL HANDLE
FontSumExists(LPPCLDEVMODE lpEnviron) {
  
    LPPCLDEVMODE lpCmp;
    HANDLE hFontSummary = 0;
    LPFONTSUMMARYHDR lpFontSummary;
  
#ifdef DEBUG_FUNCT
    DB(("Entering FontSumExists\n"));
#endif
    DBGentry(("FontSumExists(%lp)\n", lpEnviron));
  
    if (gHFontSummary) {
  
        DBGfntsumexists(("FontSumExists(): gHFontSummary=%d exists\n",
        (HANDLE)gHFontSummary));
  
        if (lpFontSummary = (LPFONTSUMMARYHDR)GlobalLock(gHFontSummary)) {
  
            DBGfntsumexists(("FontSumExists(): gHFontSummary locked\n"));
  
            lpCmp = &lpFontSummary->environ;
  
            if (SameEnvironments(lpCmp, lpEnviron)) {
  
                /*  Environments are the same (i.e., font-related
                *  information is the same).  We'll use this struct.
                */
                DBGfntsumexists((
                "FontSumExists(): environments are the same\n"));
                hFontSummary = gHFontSummary;
                ++lpFontSummary->numOpenDC;
                lpFontSummary->newFS = FALSE;
            }
#ifdef LOCAL_DEBUG
            else {
                DBGfntsumexists((
                "FontSumExists(): environments are *not* the same\n"));
            }
#endif
  
            GlobalUnlock (gHFontSummary);
            lpFontSummary = 0L;
        }
    }
  
#ifdef DEBUG_FUNCT
    DB(("Exiting FontSumExists\n"));
#endif
    return (hFontSummary);
}
  
/*  getFileFontSummary
*
*  Attempt to read the fontSummary structure from a file.
*/
LOCAL HANDLE getFileFontSummary(lpEnviron, lpPortNm, hModule, lpHLS)
LPPCLDEVMODE lpEnviron;
LPSTR lpPortNm;
HANDLE hModule;
LPHANDLE lpHLS;
{
    FSUMFILEHEADER fsumFileHeader;
    FSUMFILEENTRY fsumFileEntry;
    HANDLE hFontSummary = 0;
    LPSTR lpFontSummary;
    LPLFS lpLS;
    DWORD seek;
    BOOL same;
    WORD fsvers;
    char fsumFileName[FSUM_FNMLEN];
    char version[VERSION_LEN];
    char buf[WRKBUF_LEN];
    int hFile = -1, err, count;
  
#ifdef DEBUG_FUNCT
    DB(("Entering getFileFontSummary\n"));
#endif
    DBGentry(("getFileFontSummary(%lp,%lp,%d,%lp): %ls, %ls\n",
    lpEnviron, lpPortNm, (HANDLE)hModule, lpHLS, (LPSTR)lpEnviron, lpPortNm));
  
    /*  Allocate LoadFontState struct.  We allocate it here (even though
    *  loadFontEntries() uses it) to have a place to store the list of
    *  soft font key names from the win.ini file.
    */
    if (!(*lpHLS = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT,
        (DWORD)sizeof(LOADFONTSTATE))))
    {
        DBGerr(("getFileFontSummary(): Could *not* alloc LoadFontState\n"));
        goto backout0;
    }
    if (!(lpLS = (LPLFS)GlobalLock(*lpHLS)))
    {
        DBGerr(("getFileFontSummary(): Could *not* lock LoadFontState\n"));
        goto backout1;
    }
  
    /*  Make application name (like "HPPCL,LPT1") for reading the
    *  win.ini entry.
    */
    MakeAppName(ModuleNameStr,lpPortNm,lpLS->appName,sizeof(lpLS->appName));
  
    /*  Get the fontSummary file version number from the win.ini
    *  file (or default to zero).
    */
    if (LoadString(hModule, WININI_FSVERS, buf, sizeof(buf)))
        fsvers = GetProfileInt(lpLS->appName, buf, 0);
    else
        fsvers = 0;
  
    /*  Get the number of soft fonts from win.ini (or default to zero).
    */
    if (LoadString(hModule, SF_SOFTFONTS, buf, sizeof(buf)))
        lpLS->softfonts = GetProfileInt(lpLS->appName, buf, 0);
    else
        lpLS->softfonts = 0;
  
    DBGgetfile(("getFileFontSummary(): appName=%ls, fsvers=%d, softfonts=%d\n",
    (LPSTR)lpLS->appName, fsvers, lpLS->softfonts));
  
    /*  Prepare to open fontSummary file.
    */
    lmemset(fsumFileName, 0, sizeof(fsumFileName));
  
    if (lstrlen(VNumStr) >= sizeof(version))
    {
        lstrncpy(version,VNumStr,VERSION_LEN);         /* Bug #687 */
    }
    else
        lstrcpy(version, VNumStr);
  
    /*  Get the name of the fontSummary file from the win.ini file,
    *  open the file, read its header, and verify that its okay
    *  to use the file.
    */
    if (LoadString(hModule, FSUM_NAME, buf, sizeof(buf)) &&
        GetProfileString(lpLS->appName, buf, fsumFileName,
        fsumFileName, sizeof(fsumFileName)) &&
        lstrlen(fsumFileName) &&
        ((hFile = _lopenp(fsumFileName, OF_READ)) > 0) &&
        (err = _lread(hFile, (LPSTR) &fsumFileHeader,
        sizeof(FSUMFILEHEADER))) &&
        (err == sizeof(FSUMFILEHEADER)) &&
        (lstrcmpi(version, fsumFileHeader.version) == 0) &&
        (fsvers == fsumFileHeader.fsvers) &&
        (lstrcmpi(lpPortNm, fsumFileHeader.port) == 0) &&
        (fsumFileHeader.envSize == sizeof(PCLDEVMODE)) &&
        (fsumFileHeader.softfonts == lpLS->softfonts))
    {
        DBGgetfile(("getFileFontSummary(): %ls=%ls\n",
        (LPSTR)buf, (LPSTR)fsumFileName));
        DBGgetfile(
        ("                      hFile=%d, err=%d, version=%ls, fsvers=%d, port=%ls\n",
        hFile, err, (LPSTR)version, fsvers, lpPortNm));
        DBGgetfile(
        ("                      envSize=%d, numEntries=%d, softfonts=%d\n",
        fsumFileHeader.envSize, fsumFileHeader.numEntries,
        fsumFileHeader.softfonts));
  
        /*  Search for a matching environment.
        */
        for (count = 0, same = FALSE, seek = fsumFileHeader.offsetFirstEntry;
            (count < fsumFileHeader.numEntries) &&
            (_llseek(hFile, seek, 0) == seek) &&
            (err =
            _lread(hFile, (LPSTR) &fsumFileEntry, sizeof(FSUMFILEENTRY))) &&
            (err == sizeof(FSUMFILEENTRY)) &&
            !(same = SameEnvironments(lpEnviron,(LPPCLDEVMODE)&fsumFileEntry));
            ++count, seek += sizeof(FSUMFILEENTRY)+fsumFileEntry.sizeofFontSum)
            ;
  
        if (same)
        {
            /*  A matching environment was found, we can use the
            *  fontSummary stored in the file.
            */
            DBGgetfile(
            ("getFileFontSummary(): environment %d matches\n", count));
  
            /*  Allocate the fontSummary struct.
            *
            *  Make the structure fixed because this works best
            *  with LIM4.  The structure will go as low in memory
            *  as possible, so it won't clutter up the swap area
            *  and it will be shared across multiple instances
            *  of the driver.
            */
            if (!(hFontSummary =
                GlobalAlloc(GMEM_FIXED | GMEM_LOWER | GMEM_DDESHARE,
                (DWORD)fsumFileEntry.sizeofFontSum)))
            {
                DBGerr(
                ("getFileFontSummary(): Could *not* alloc fontSummary\n"));
                goto backout3;
            }
  
            /*  Lock down fontSummary struct.
            */
            if (!(lpFontSummary = GlobalLock(hFontSummary)))
            {
                DBGerr(
                ("getFileFontSummary(): Could *not* lock fontSummary\n"));
                goto backout4;
            }
  
            if ((err =
                _lread(hFile, lpFontSummary, fsumFileEntry.sizeofFontSum)) &&
                (err == fsumFileEntry.sizeofFontSum))
            {
                DBGgetfile(
                ("getFileFontSummary(): fontSummary successfully read\n"));
  
                /*  Debug stuff: dump the whole damn thing.
                */
        #ifdef DBGdumpfontsummary
                DBGdumpFontSummary((LPFONTSUMMARYHDR)lpFontSummary, -1);
        #endif
  
                GlobalUnlock(hFontSummary);
                lpFontSummary = 0L;
  
                if (count > 0)
                {
                    _lclose(hFile);
                    hFile = 0;
                    if ((hFile = _lopenp(fsumFileName, OF_READWRITE)) > 0)
                    {
                        /*  Shuffle the directory item of the fontSummary
                        *  struct we just read to the top of the file.
                        *  This way we can use an easy LRU algorithm for
                        *  deleting items when the file grows too big.
                        */
                        _lshuffle(hFile, fsumFileHeader.offsetFirstEntry, seek,
                        sizeof(FSUMFILEENTRY)+fsumFileEntry.sizeofFontSum);
                    }
                }
                goto backout3;
            }
        #ifdef LOCAL_DEBUG
            else
            {
                DBGgetfile(
                ("getFileFontSummary(): *failed* to read fontSummary\n"));
            }
        #endif
        }
        else
        {
            DBGgetfile(
            ("getFileFontSummary(): matching environment *not* found\n"));
            goto backout3;
        }
    }
    else
    {
        DBGgetfile(
        ("getFileFontSummary(): failed to open/recognize fontSummary file\n"));
        goto backout3;
    }
  
    /*  Exit from the top if we allocated the fontSummary struct
    *  but failed to read it from the file.
    */
    GlobalUnlock(hFontSummary);
    lpFontSummary = 0L;
    backout4:
    GlobalFree(hFontSummary);
    hFontSummary = 0;
    backout3:
    if (hFile > 0)
        _lclose (hFile);
    hFile = -1;
    GlobalUnlock(*lpHLS);
    if (!hFontSummary)
        goto backout0;
    backout1:
    GlobalFree(*lpHLS);
    *lpHLS = 0;
    backout0:
#ifdef DEBUG_FUNCT
    DB(("Exiting getFileFontSummary\n"));
#endif
    return (hFontSummary);
}
  
/*  putFileFontSummary
*
*  Write the newly-created fontSummary structure to a file.
*/
LOCAL void putFileFontSummary(hFontSummary, lpEnviron, lpPortNm, hModule)
HANDLE hFontSummary;
LPPCLDEVMODE lpEnviron;
LPSTR lpPortNm;
HANDLE hModule;
{
    FSUMFILEHEADER fsumFileHeader;
    FSUMFILEENTRY fsumFileEntry;
    LPFONTSUMMARYHDR lpFontSummary;
    WORD fsvers;
    char appName[APPNM_LEN];
    char fsumFileName[FSUM_FNMLEN];
    char SystemDir[FSUM_FNMLEN];
    char tmpName[FSUM_FNMLEN];
    char prefix[5];
    char fsumName[24];
    char buf[WRKBUF_LEN];
    char version[VERSION_LEN];
    int hFile = -1, err, ind, count;
    int NewName = FALSE;
    int sdlen;
    DWORD maxMem, seek;
    LPSTR s;
  
#ifdef DEBUG_FUNCT
    DB(("Entering putFileFontSummary\n"));
#endif
    DBGentry(("putFileFontSummary(%d,%lp,%lp,%d): %ls, %ls\n",
    (HANDLE)hFontSummary, lpEnviron, lpPortNm,
    (HANDLE)hModule, (LPSTR)lpEnviron, lpPortNm));
  
    /*  Lock down the fontSummary struct.
    */
    if (!(lpFontSummary = (LPFONTSUMMARYHDR)GlobalLock(hFontSummary)))
    {
        DBGerr(("putFileFontSummary(): could *not* lock fontSummary struct\n"));
        return;
    }
  
    /*  Make application name (like "HPPCL,LPT1") for reading the
    *  win.ini entry.
    */
    MakeAppName(ModuleNameStr,lpPortNm,appName,sizeof(appName));
  
    /*  Get the fontSummary file version number from the win.ini
    *  file (or default to zero).
    */
    if (LoadString(hModule, WININI_FSVERS, buf, sizeof(buf)))
        fsvers = GetProfileInt(appName, buf, 0);
    else
        fsvers = 0;
  
    /*  Determine the maximum amount of memory this file may use.
    */
    if (LoadString(hModule, FSUM_MEMLIMIT, buf, sizeof(buf)))
    {
        if ((maxMem=(DWORD)GetProfileInt(appName,buf,MAXFILE_MEM)) <= 0)
        {
            DBGputfile(
            ("putFileFontSummary(): user requests zero size file\n"));
            goto backout0;
        }
  
        if (maxMem > MAXFILE_MAXMEM)
            maxMem = MAXFILE_MAXMEM;
    }
    else
        maxMem = MAXFILE_MEM;
  
    maxMem = lmul(maxMem, (long)1024);
  
    DBGputfile(
    ("putFileFontSummary(): file limited to approximately %ld bytes\n", maxMem));
  
    /*  Initialize the various buffers to 0.
    */
    lmemset(fsumFileName, 0, sizeof(fsumFileName));
    lmemset((LPSTR) &fsumFileHeader, 0, sizeof(FSUMFILEHEADER));
  
    if (lstrlen(VNumStr) >= sizeof(version))
    {
        lstrncpy(version,VNumStr, VERSION_LEN);        /* Bug #687 */
    }
    else
        lstrcpy(version, VNumStr);
  
    /*  Get the name of the fontSummary file from the win.ini file.
     */
    if (!LoadString(hModule, FSUM_NAME, fsumName, sizeof(fsumName)) ||
        !GetProfileString(appName, fsumName, fsumFileName,
        fsumFileName, sizeof(fsumFileName)))
    {
        /*  Failed to get file name, construct a new name.
         */
        bldFSumFileName(fsumFileName, lpPortNm, hModule);
        NewName = TRUE;
  
        DBGputfile(("putFileFontSummary(): built file name %ls\n",
        (LPSTR)fsumFileName));
    }
    
    /* Now we want to make sure the name is FS5___.PCL, and 
     * not FS___.PCL from the v3.42 driver AND that the directory
     * is not \(windows)\system. - dtk
     */
    if (!NewName)
    {
        /* Make sure the name is all upper case for
         * the following string comparisons
         */
        AnsiUpper((LPSTR)fsumFileName);

        /* strip off the file name from the full path
         */
        for(s = (LPSTR)fsumFileName + lstrlen((LPSTR)fsumFileName);
            (s > (LPSTR)fsumFileName) && (s[-1] != '\\') && (s[-1] != ':');
            --s);

        /* copy only the firs three characters...
         * looks like lstrncpy copies n-1 chars then puts a null
         */
        lstrncpy((LPSTR) tmpName, s, 4);

        /* get the file prefix FS5
         */
        if (!LoadString(hModule, FSUM_FILEPREFIX, prefix, 5))
            lstrcpy(prefix, "FS5");

        /* Compare the characters against the file prefix.
         * If they don't match, it's old, so rebuild the font
         * sum name.
         */

        if(lstrcmp(tmpName, prefix) != 0)
            bldFSumFileName(fsumFileName, lpPortNm, hModule);

        else

        /* Now check to see if the path name is the \system directory.
         *
         * (1)  If it is, re-write it to the \windows directory. 
         *      (If someone wants to write it to the system directory, 
         *      tough #$%&!. We have to draw the line somewhere to ensure 
         *      the majority of networked users work.)
         *
         * (2)  If it's not,then the user has entered a new path to write 
         *      the fsum into, so use this path instead. 
         *      (Probably a network application)
         * - dtk
         */
        {
            /* get the system directory and calc its length
             */
            GetSystemDirectory(SystemDir, FSUM_FNMLEN);
            sdlen = lstrlen(SystemDir);

            /* copy only the amount of chars as the system 
             * directory for comparison.
             */
            lstrncpy((LPSTR)tmpName, fsumFileName, sdlen+1);

            /* if they compare, its in the system directory, so re-build
             */
            if(lstrcmp(tmpName, SystemDir) == 0)
                bldFSumFileName(fsumFileName, lpPortNm, hModule);
        }

    } /* if !NewName */

    DBGputfile(("putFileFontSummary(): %ls=%ls\n",
    (LPSTR)fsumName, (LPSTR)fsumFileName));
  
    /*  Check to see if the fontSummary file already exists.
     */
    if ((hFile = _lopenp(fsumFileName, OF_READ)) > 0)
    {
        DBGputfile(("putFileFontSummary(): %ls open for read, hFile=%d\n",
        (LPSTR)fsumFileName, hFile));
  
        if ((err = _lread(hFile, (LPSTR) &fsumFileHeader,
            sizeof(FSUMFILEHEADER))) && (err == sizeof(FSUMFILEHEADER)))
        {
            /*  Successfully read header.
            */
            _lclose (hFile);
            hFile = 0;
            DBGputfile(
            ("                      ...%ls closed\n", (LPSTR)fsumFileName));
  
            if ((lstrcmpi(version, fsumFileHeader.version) == 0) &&
                (fsvers == fsumFileHeader.fsvers) &&
                (lstrcmpi(lpPortNm, fsumFileHeader.port) == 0) &&
                (fsumFileHeader.envSize == sizeof(PCLDEVMODE)) &&
                (fsumFileHeader.softfonts == lpFontSummary->softfonts))
            {
                /*  We can use this file, so reopen it read/write.
                */
                DBGputfile(
                ("putFileFontSummary(): %ls is a valid fontSummary file\n",
                (LPSTR)fsumFileName));
                hFile = _lopenp(fsumFileName, OF_READWRITE);
  
        #ifdef LOCAL_DEBUG
                if (hFile > 0) {
                    DBGputfile(
                    ("                      ...%ls reopened readwrite, hFile=%d\n",
                    (LPSTR)fsumFileName, hFile));
                } else {
                    DBGputfile(
                    ("                      ...failed to reopen %ls readwrite, hFile=%d\n",
                    (LPSTR)fsumFileName, hFile));
                }
        #endif
            }
        #ifdef LOCAL_DEBUG
            else
            {
                DBGputfile(("putFileFontSummary(): %ls header does not match\n",
                (LPSTR)fsumFileName));
            }
        #endif
        }
        else
        {
            DBGputfile(("putFileFontSummary(): could not read %ls\n",
            (LPSTR)fsumFileName));
            _lclose (hFile);
            hFile = 0;
        }
    }
  
    /*  If we failed to open an existing file, create a new one.
     */
    if (hFile <= 0)
    {
        lmemset((LPSTR) &fsumFileHeader, 0, sizeof(FSUMFILEHEADER));
  
        if ((hFile = _lopenp(fsumFileName, OF_CREATE | OF_READWRITE)) > 0)
        {
            DBGputfile(
            ("putFileFontSummary(): %ls created for readwrite, hFile=%d\n",
            (LPSTR)fsumFileName, hFile));
  
            /*  Fill in header.
            */
            if (lstrlen(VNumStr) < sizeof(fsumFileHeader.version))
                lstrcpy(fsumFileHeader.version, VNumStr);
            else                      /* Bug #687 */
                lstrncpy(fsumFileHeader.version, VNumStr, VERSION_LEN);
            if (lstrlen(lpPortNm) < sizeof(fsumFileHeader.port))
                lstrcpy(fsumFileHeader.port, lpPortNm);
            fsumFileHeader.fsvers = fsvers;
            fsumFileHeader.envSize = sizeof(PCLDEVMODE);
            fsumFileHeader.softfonts = lpFontSummary->softfonts;
  
            fsumFileHeader.offsetFirstEntry +=
            _lwrite(hFile, (LPSTR) &fsumFileHeader, sizeof(FSUMFILEHEADER));
  
            /*  Write a little message at the top of the file for
            *  curious users who open it.
            */
            for (ind = 0; (ind < 9) && LoadString(hModule,
                FSUM_MESSAGE+ind, buf, sizeof(buf)); ++ind)
            {
                fsumFileHeader.offsetFirstEntry +=
                _lwrite(hFile, buf, lstrlen(buf));
            }
        }
        else
        {
            DBGerr(("putFileFontSummary(): %ls could not be created\n",
            (LPSTR)fsumFileName));
            goto backout0;
        }
    }
  
    DBGputfile(
    ("putFileFontSummary(): version=%ls, port=%ls, envSize=%d, softfonts=%d\n",
    (LPSTR) fsumFileHeader.version, (LPSTR) fsumFileHeader.port,
    fsumFileHeader.envSize, fsumFileHeader.softfonts));
    DBGputfile(
    ("                      fsvers=%d, numEntries=%d, offsetFirstEntry=%ld\n",
    fsumFileHeader.fsvers, fsumFileHeader.numEntries,
    fsumFileHeader.offsetFirstEntry));
  
    if (GlobalSize(hFontSummary) > maxMem)
        maxMem = 0L;
    else
        maxMem -= GlobalSize(hFontSummary);
  
    DBGputfile(("putFileFontSummary(): adjusted maxMem=%ld bytes\n", maxMem));
  
    /*  Search to the end of the file or to the size limit.
    */
    for (count = 0, seek = fsumFileHeader.offsetFirstEntry;
        (_llseek(hFile, seek, 0) == seek) &&
        (count < fsumFileHeader.numEntries) &&
        (err = _lread(hFile, (LPSTR) &fsumFileEntry, sizeof(FSUMFILEENTRY))) &&
        (err == sizeof(FSUMFILEENTRY)) &&
        (seek +sizeof(FSUMFILEENTRY) +fsumFileEntry.sizeofFontSum < maxMem);
        ++count, seek += sizeof(FSUMFILEENTRY)+fsumFileEntry.sizeofFontSum)
        ;
  
    #ifdef LOCAL_DEBUG
    if (count < fsumFileHeader.numEntries) {
        DBGputfile(("putFileFontSummary(): %d entries will be deleted\n",
        (fsumFileHeader.numEntries - count)));
    }
    #endif
  
    DBGputfile(
    ("putFileFontSummary(): writing new fontSummary at %d\n", count));
  
    /*  Write out the new fontSummary struct and truncate the file.
    */
    lmemcpy((LPSTR) &fsumFileEntry.environ, (LPSTR) lpEnviron,
    sizeof(PCLDEVMODE));
  
    fsumFileEntry.sizeofFontSum = (WORD)GlobalSize(hFontSummary);
    _llseek(hFile, seek, 0);
    _lwrite(hFile, (LPSTR) &fsumFileEntry, sizeof(FSUMFILEENTRY));
    _lwrite(hFile, (LPSTR)lpFontSummary, fsumFileEntry.sizeofFontSum);
    _lwrite(hFile, (LPSTR)NullStr, 0); /* truncate file */
  
    /*  Rewrite header to set the count correctly.
    */
    fsumFileHeader.numEntries = count + 1;
    _llseek(hFile, 0L, 0);
    _lwrite(hFile, (LPSTR) &fsumFileHeader, sizeof(FSUMFILEHEADER));
  
    if (count > 0)
    {
        /*  Move the new entry to to top of the file -- fontSummary
        *  entries are deleted via LRU (least-recently-used) method.
        */
        DBGputfile(("putFileFontSummary(): shuffling new fontSummary to 0\n"));
  
        _lshuffle(hFile, fsumFileHeader.offsetFirstEntry, seek,
        sizeof(FSUMFILEENTRY)+fsumFileEntry.sizeofFontSum);
    }
  
    /*  Write the file name to the win.ini file.
    */
    if (lstrlen(appName) && lstrlen(fsumName) && lstrlen(fsumFileName))
        WriteProfileString(appName, fsumName, fsumFileName);
  
    /*  Close up shop.
    */
    _lclose(hFile);
    backout0:
    GlobalUnlock(hFontSummary);
#ifdef DEBUG_FUNCT
    DB(("Exiting putFileFontSummary\n"));
#endif
}
  
/*  SameEnvironments
*
*  Compare those fields of the PCLDEVMODE struct which could affect font
*  information -- return TRUE if they are identical.
*/
LOCAL BOOL
SameEnvironments(LPPCLDEVMODE lpEnvA, LPPCLDEVMODE lpEnvB) {
  
    BOOL same;
  
#ifdef DEBUG_FUNCT
    DB(("Entering SameEnvironment\n"));
#endif
    DBGentry(("SameEnvironments(%lp,%lp): same=", lpEnvA, lpEnvB));
  
    same = (!lstrcmpi((LPSTR)lpEnvA->dm.dmDeviceName,
    (LPSTR)lpEnvB->dm.dmDeviceName) &&
    (lpEnvA->dm.dmOrientation == lpEnvB->dm.dmOrientation) &&
    (lpEnvA->prtIndex == lpEnvB->prtIndex) &&
    (lpEnvA->romind == lpEnvB->romind) &&
    (lpEnvA->romcount == lpEnvB->romcount) &&
    (lpEnvA->numCartridges == lpEnvB->numCartridges) &&
    (mylmemcmp((LPSTR)lpEnvA->cartIndex,
    (LPSTR)lpEnvB->cartIndex, DEVMODE_MAXCART*2)) &&
    (mylmemcmp((LPSTR)lpEnvA->cartind,
    (LPSTR)lpEnvB->cartind, DEVMODE_MAXCART*2)) &&
    (mylmemcmp((LPSTR)lpEnvA->cartcount,
    (LPSTR)lpEnvB->cartcount, DEVMODE_MAXCART*2)) &&
    (lpEnvA->prtCaps == lpEnvB->prtCaps) &&
    (lpEnvA->options == lpEnvB->options) &&
    (lpEnvA->fsvers == lpEnvB->fsvers));
  
    #ifdef LOCAL_DEBUG
    if (same) {
        DBGentry(("TRUE\n"));
    } else {
        DBGentry(("FALSE\n"));
    }
    #endif
  
#ifdef DEBUG_FUNCT
    DB(("Exiting SameEnvironment\n"));
#endif
    return (same);
}
  
/*  mylmemcmp
*
*  Memcmp function.
*/
LOCAL BOOL mylmemcmp(a, b, len)
LPSTR a;
LPSTR b;
short len;
{
#ifdef DEBUG_FUNCT
    DB(("Entering mylmemcmp\n"));
#endif
    /*  DBGentry(("mylmemcmp(%lp,%lp,%d)\n", a, b, len));
    */
  
    while (len-- > 0)
    {
        if (*a++ != *b++)
            return FALSE;
    }
  
#ifdef DEBUG_FUNCT
    DB(("Exiting mylmemcmp\n"));
#endif
    return TRUE;
}
  
/*  bldFSumFileName
*
*  Build a file name for the file used to store the fontSummary struct.
*  The file name consists of prefix+port+extension.
*/
LOCAL void bldFSumFileName(lpFSumFileName, lpPortNm, hModule)
LPSTR lpFSumFileName;
LPSTR lpPortNm;
HANDLE hModule;
{
    LPSTR s;
    char tempfile[32];
  
#ifdef DEBUG_FUNCT
    DB(("Entering bldFSumFileName\n"));
#endif
    /*  Load the file prefix from the resource file.
    */
    if (!LoadString(hModule, FSUM_FILEPREFIX, tempfile, 5))
    {
        DBGerr(("bldFSumFileName(): could not load file prefix\n"));
        lstrcpy(tempfile, "FS5");
    }
  
    /*  Concat the port name to the file prefix.
    */
    if (!lpPortNm || !lstrlen(lpPortNm))
    {
        lstrcat(tempfile, "NONE");
    }
    else
    {
        s = lpPortNm + lstrlen(lpPortNm);
  
        /*  First get the filename part of the port (i.e., strip
        *  off any path names.
        */
        if (s[-1] == ':')
            --s;
        for (; s > lpPortNm && s[-1] != ':' && s[-1] != '\\'; --s)
            ;
        /* TETRA -- changed lmemcpy to lstrncpy -- KLO 
         */
        lstrncpy((LPSTR) &tempfile[lstrlen(tempfile)], s, 8);
        tempfile[8] = '\0';
  
        /*  Truncate the name at any invalid file-name characters.
        */
        for (s = (LPSTR)tempfile + lstrlen(tempfile) - 1;
            s > (LPSTR)tempfile; --s)
        {
            if (*s == ':' || *s == '\\' || *s == '.')
                *s = '\0';
        }
    }
    tempfile[8] = '\0';
  
    if (!lstrlen(tempfile))
    {
        DBGerr(("bldFSumFileName(): screwed up building file name\n"));
        lstrcpy(tempfile, "FSNONE");
    }
  
    /*  Add extension to file name.
    */
    lstrcat(tempfile, ".");
    if (!LoadString(hModule, FSUM_FILEEXTENSION,
        (LPSTR) &tempfile[lstrlen(tempfile)], 4))
    {
        DBGerr(("bldFSumFileName(): could not load file extension\n"));
        lstrcpy(tempfile, PclStr);
    }
  
    /* Use the same path the driver file is in, NOT!
     */
//  if (GetModuleFileName(hModule, lpFSumFileName, FSUM_FNMLEN))

    /* The following line was changed to call GetWindowsDirectory instead 
     * of GetModuleFileName to fix bug #80.  It was in response to a 
     * sharing violationerror over networks with a read-only system dir - SJC 
     */
    if (GetWindowsDirectory(lpFSumFileName, FSUM_FNMLEN)) 
    {
        /* Commented out for bug #80
         */
//      for (s = lpFSumFileName + lstrlen(lpFSumFileName);
//      (s > lpFSumFileName) && (s[-1] != ':') && (s[-1] != '\\');
//      --s)
//      ; 

        /* Added for bug #80 - Add '\' to end of lpFSumFileName
         */
        lstrcat(lpFSumFileName, "\\\0");

        /* Position s at end of lpFSumFileName
         */
        for(s = lpFSumFileName; *s; s++);
  
        if (lstrlen(tempfile) < FSUM_FNMLEN - lstrlen(lpFSumFileName))
            lstrcpy(s, tempfile);
        else
            lstrcpy(lpFSumFileName, tempfile);
    }
    else
    {
        lstrcpy(lpFSumFileName, tempfile);
    }

    DBMSG (("Exiting bldFSumFileName, lpFSumFileName=%ls\n",lpFSumFileName));
#ifdef DEBUG_FUNCT
    DB(("Exiting bldFSumFileName\n"));
#endif
}
  
/*  _lshuffle
*
*  Shuffle sizeofBuf bytes from sourcepos to destpos in the file
*  pointed to by hFile.
*/
LOCAL void _lshuffle(hFile, destpos, sourcepos, bufSize)
int hFile;
DWORD destpos;
DWORD sourcepos;
WORD bufSize;
{
    HANDLE hBuf;
    LPSTR lpSourceBuf, lpTransBuf;
    DWORD filepos, prevpos;
    WORD sizeofSourceBuf, sizeofTransBuf, spaceRemaining;
#ifdef DEBUG_FUNCT
    DB(("Entering _lshuffle\n"));
#endif
  
    DBGentry(("_lshuffle(%d,%ld,%ld,%d)\n",
    hFile, destpos, sourcepos, bufSize));
  
    if (destpos >= sourcepos)
    {
        DBGerr(("_lshuffle(): destpos >= sourcepos, *no* shuffle\n"));
        return;
    }
  
    if (!(hBuf = GlobalAlloc(GMEM_MOVEABLE, (DWORD)(2*MAX_BUFSIZE))))
    {
        DBGerr(("_lshuffle(): Could *not* alloc buffer\n"));
        return;
    }
  
    if (!(lpSourceBuf = GlobalLock(hBuf)))
    {
        DBGerr(("_lshuffle(): Could *not* lock buffer\n"));
        GlobalFree(hBuf);
        return;
    }
  
    lpTransBuf = lpSourceBuf + MAX_BUFSIZE;
  
    do {
        /*  Set up size of buffer.
        */
        if (bufSize > MAX_BUFSIZE)
        {
            sizeofSourceBuf = sizeofTransBuf = MAX_BUFSIZE;
            bufSize -= MAX_BUFSIZE;
        }
        else
        {
            sizeofSourceBuf = sizeofTransBuf = bufSize;
            bufSize = 0;
        }
  
        /*  Read the source buffer.
        */
        _llseek(hFile, prevpos=filepos=sourcepos, 0);
        _lread(hFile, lpSourceBuf, sizeofSourceBuf);
  
        /*  Shift all the bytes at the destination to the source.
        */
        do {
            if ((filepos - destpos) < (DWORD)sizeofTransBuf)
            {
                spaceRemaining = (WORD)(filepos - destpos);
                prevpos += (DWORD)(sizeofTransBuf - spaceRemaining);
                sizeofTransBuf = spaceRemaining;
            }
            filepos -= sizeofTransBuf;
  
            _llseek(hFile, filepos, 0);
            _lread(hFile, lpTransBuf, sizeofTransBuf);
            _llseek(hFile, prevpos, 0);
            _lwrite(hFile, lpTransBuf, sizeofTransBuf);
  
        } while ((prevpos=filepos) > destpos);
  
        /*  Write the source buffer at the destination.
        */
        _llseek(hFile, destpos, 0);
        _lwrite(hFile, lpSourceBuf, sizeofSourceBuf);
  
        /*  Update positions in case we loop.
        */
        destpos += sizeofSourceBuf;
        sourcepos += sizeofSourceBuf;
  
    } while (bufSize > 0);
  
    GlobalUnlock(hBuf);
    GlobalFree(hBuf);
#ifdef DEBUG_FUNCT
    DB(("Exiting _lshuffle\n"));
#endif
}
