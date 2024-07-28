/**[f******************************************************************
* fontutil.c -
*
* Copyright (C) 1988,1989 Aldus Corporation.
* Copyright (C) 1988-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.  Company confidential.
*
**f]*****************************************************************/
  
/***************************************************************************/
/*****************************   fontutil.c   ******************************/
/*
*  Fontutil: This module contains utilities for accessing the fontSummary
*  structure.
*
*  rev:
*
*  02-07-90    VO      Added size sequence to font i.d. select
*  11-29-86    skk     made LoadFontString build escape string for
*                          downloadable fonts
*  11-26-86    msd     switched to getting escape sequences from the RC
*  11-23-86    msd     added Load/UnloadWidthTable
*  11-22-86    msd     module creation
*
*   1-13-89    jimmat  Reduced # of redundant strings by adding lclstr.h
*   1-25-89    jimmat  Use global hLibInst instead of GetModuleName() -
*          lclstr.h no longer required by this file.
*/
  
/*
* $Header: fontutil.c,v 3.890 92/02/06 16:11:49 dtk FREEZE $
*/
  
/*
* $Log:	fontutil.c,v $
 * Revision 3.890  92/02/06  16:11:49  16:11:49  dtk (Doug Kaltenecker)
 * Win3.1 Freeze
 * 
 * Revision 3.872  92/01/10  11:27:25  11:27:25  dtk (Doug Kaltenecker)
 * Fixed the Roman-8 char width remapping stuff.
 * 
 * Revision 3.871  91/12/02  16:43:52  16:43:52  dtk (Doug Kaltenecker)
 * Changed the ifdef TT build variables to ifdef WIN31.
 * 
 * Revision 3.871  91/11/22  13:18:54  13:18:54  dtk (Doug Kaltenecker)
 * Win 3.1 Post Beta 3 version.
 * 
 * Revision 3.870  91/11/08  11:43:34  11:43:34  dtk (Doug Kaltenecker)
 * 3.1 Release Candidate 1
 * 
 * Revision 3.865  91/11/01  13:51:33  13:51:33  dtk (Doug Kaltenecker)
 * Beta release for Windows 3.1
 * 
 * Revision 3.862  91/10/25  13:46:53  13:46:53  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.860  91/10/23  09:48:19  09:48:19  dtk (Doug Kaltenecker)
 * WinWird Release
 * 
 * Revision 3.852  91/10/09  14:59:22  14:59:22  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.850  91/10/04  16:49:33  16:49:33  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE
 * 
 * Revision 3.840  91/09/28  14:16:53  14:16:53  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE 3 FOR WINWORD
 * 
 * Revision 3.830  91/09/18  16:33:08  16:33:08  dtk (Doug Kaltenecker)
 * RELEASE
 * 
 * Revision 3.822  91/09/16  10:33:33  10:33:33  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE 2 FOR WIN30.
 * 
 * Revision 3.820  91/09/06  14:11:47  14:11:47  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE FOR WINWORD 2.0
 * 
 * Revision 3.813  91/09/04  11:43:21  11:43:21  dtk (Doug Kaltenecker)
 * Changed the local #ifdef TT Moved the local #ifdef TT to build.h and included it.
 * 
 * Revision 3.812  91/08/22  14:31:44  14:31:44  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.807  91/08/08  10:31:01  10:31:01  dtk (Doug Kaltenecker)
 * PREBETA3
 * 
 * Revision 3.802  91/07/22  12:53:55  12:53:55  oakeson (Ken Oakeson)
 * BETA
 * 
 * Revision 3.799  91/07/02  11:51:23  11:51:23  daniels (Susan Daniels)
 * Beta
 * 
 * Revision 3.796  91/06/26  11:25:42  11:25:42  stevec (Steve Claiborne)
 * BETA
 * 
 * Revision 3.790  91/06/11  16:02:57  16:02:57  stevec (Steve Claiborne)
 * Freeze
 * 
 * Revision 3.786  91/06/11  15:43:42  15:43:42  dtk (Doug Kaltenecker)
 * Prettified files!
 * 
* Revision 3.785  91/05/22  14:56:38  14:56:38  stevec (Steve Claiborne)
* Beta version to MS
*
* Revision 3.781  91/05/22  13:46:50  13:46:50  oakeson (Ken Oakeson)
* Added GetRasterizerCaps functionality
*
* Revision 3.780  91/05/15  15:56:47  15:56:47  stevec (Steve Claiborne)
* Beta
*
* Revision 3.775  91/04/05  14:30:40  14:30:40  stevec (Steve Claiborne)
* Beta release to MS
*
* Revision 3.770  91/03/25  15:35:40  15:35:40  stevec (Steve Claiborne)
* maintance release
*
* Revision 3.760  91/03/12  07:52:21  07:52:21  stevec (Steve Claiborne)
* Maintance release
*
* Revision 3.755  91/03/03  07:45:44  07:45:44  stevec (Steve Claiborne)
* March 3 Freeze
*
* Revision 3.720  91/02/11  09:15:03  09:15:03  stevec (Steve Claiborne)
* Aldus version
*
* Revision 3.711  91/02/08  16:25:43  16:25:43  stevec (Steve Claiborne)
* Added debuging
*
* Revision 3.710  91/02/04  15:47:21  15:47:21  stevec (Steve Claiborne)
* Aldus freeze
*
* Revision 3.700  91/01/19  09:00:03  09:00:03  stevec (Steve Claiborne)
* Release
*
* Revision 3.685  91/01/14  15:42:57  15:42:57  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.682  91/01/14  10:17:14  10:17:14  stevec (Steve Claiborne)
* Updated the copy right stmt.
*
* Revision 3.681  91/01/13  20:52:23  20:52:23  oakeson (Ken Oakeson)
* Added emMasterUnits to MakeSizeEscape fn
*
* Revision 3.680  91/01/10  16:16:20  16:16:20  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.672  90/12/20  12:03:20  12:03:20  stevec (Steve Claiborne)
* Put ifdef around all truetype stuff but left default truetype defined.
* This fixes bug #105.  SJC 12-20-90
*
* Revision 3.670  90/12/14  14:53:46  14:53:46  stevec (Steve Claiborne)
* freeze for 12-14-90 ver. 3.670
*
* Revision 3.665  90/12/10  15:35:29  15:35:29  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.660  90/12/07  14:49:59  14:49:59  stevec (Steve Claiborne)
* Freeze 12-7-90
*
* Revision 3.650  90/11/30  08:11:54  08:11:54  stevec (Steve Claiborne)
* Freeze 3.650, 11-30-90
*
* Revision 3.603  90/10/25  17:14:26  17:14:26  oakeson (Ken Oakeson)
* Used GetVersion instead of #ifdef around truetype code
*
* Revision 3.602  90/08/24  11:37:36  11:37:36  daniels (Susan Daniels)
* message.txt
*
* Revision 3.601  90/08/14  15:26:31  15:26:31  oakeson (Ken Oakeson)
* Added TrueType support
*
* Revision 3.600  90/08/03  11:09:24  11:09:24  stevec (Steve Claiborne)
* This is the Aug. 3 release ver. 3.600
*
* Revision 3.550  90/07/27  11:30:28  11:30:28  root ()
* Experimental freeze 3.55
*
* Revision 3.540  90/07/25  12:33:59  12:33:59  stevec (Steve Claiborne)
* Experimental freeze 3.54
*
* Revision 3.521  90/07/21  11:36:41  11:36:41  oakeson ()
* Added code to unscramble Roman-8 widths
*
* Revision 3.520  90/06/13  16:52:01  16:52:01  root ()
* 5_2_release
*
*
*    Rev 1.1   20 Feb 1990 15:31:04   vordaz
* Support for downloadables.
*/
  
/*** Tetra begin ***/
//#define DEBUG
#define DPOINT1 63
/*** Tetra end ***/
  
#include "generic.h"
#include "resource.h"
#define FONTMAN_UTILS
#define FONTMAN_ENABLE
#include "fontman.h"
#include "fonts.h"
#include "utils.h"
#define SEG_PHYSICAL
#include "memoman.h"
#include "truetype.h"
#include "build.h"
  
/* Tetra -- Roman-8 unscrambler */
#include "fix_r8.h"
  
/*  lockfont utility
*/
#include "lockfont.c"
#include "makefsnm.c"
#include "loadfile.c"
  
#define LOCAL static
  
/*  Local debug structure.
*/
#ifdef DEBUG
#define LOCAL_DEBUG
#define DBGdumpwidths
/*
#define DBGnewidthtable
*/
#endif
  
#ifdef LOCAL_DEBUG
#define DBGentry(msg) /*DBMSG(msg)*/
#define DBGerr(msg) /*DBMSG(msg)*/
#define DBGloadfontstring(msg) /*DBMSG(msg)*/
#define DBGloadwidthtable(msg) /*DBMSG(msg)*/
#define DBGscramble(msg) DBMSG(msg)
#else
#define DBGentry(msg) /*null*/
#define DBGerr(msg) /*null*/
#define DBGloadfontstring(msg) /*null*/
#define DBGloadwidthtable(msg) /*null*/
#define DBGscramble(msg) /*null*/
#endif
  
  
LOCAL LPSTR loadWidths(LPFONTSUMMARYHDR, LPFONTSUMMARY);
  
  
/*  LoadFontString
*
*  Retrieve the font string from the font string table at the end of
*  the fontSummary struct.
*/
/*** Tetra II begin ***/
/*** Added a short for the pixel height ***/
BOOL far PASCAL LoadFontString(lpDevice, lpDest, len, sType, fontInd, PixHeight)
LPDEVICE lpDevice;
LPSTR lpDest;
short len;
stringType sType;
short fontInd;
short PixHeight;
/*** Tetra II end ***/
{
    register LPFONTSUMMARYHDR lpFontSummary;
    LPSTR lpString;
    BOOL status = TRUE;
    short ind = -1, n;
    ESCtype escape;
#ifdef DEBUG_FUNCT
    DB(("Entering LoadFontString\n"));
#endif
  
    DBGentry(("LoadFontString(%lp,%d,%d,%d,%d)\n",
    lpDevice, lpDest, len, sType, fontInd));
  
    if (lpFontSummary = lockFontSummary(lpDevice))
    {
        if ((fontInd >= 0) && (fontInd < lpFontSummary->len))
        switch (sType)
        {
            case fontescape:
                ind = lpFontSummary->f[fontInd].indEscape;
                break;
            case fontpfmfile:
                ind = lpFontSummary->f[fontInd].indPFMName;
                break;
            case fontdlfile:
                ind = lpFontSummary->f[fontInd].indDLName;
                break;
            case fontname:
            default:
                ind = lpFontSummary->f[fontInd].indName;
                break;
        }
  
        if (ind > -1)
        {
            /*  Locate string table at end of fontSummary struct.
            */
            lpString = (LPSTR) & lpFontSummary->f[lpFontSummary->len];
  
            /*  Locate string inside of stringtable.
            */
            lpString = &lpString[ind];
            DBGloadfontstring(("LoadFontString(): string %lp, %ls\n",
            lpString, (lpString) ? lpString : (LPSTR)"{ null }"));
  
            /*  Copy into the caller's string.
            */
            lmemcpy(lpDest, lpString, len);
            lpDest[len-1] ='\0';
        }
        else if (sType == fontescape)
        {
            ind=lpFontSummary->f[fontInd].indDLName;
  
            /*  Handle downloadable fonts.
            */
            if ((ind==-1) || (lpFontSummary->f[fontInd].LRUcount!=-1))
            {
                /*** Tetra II begin ***/
                LPFONTSUMMARY lpSummary;
                /*** Tetra II end ***/
  
                /*  Font is downloaded so generate escape using ID.
                */
                /*** Tetra II begin ***/
                lpSummary = &lpFontSummary->f[fontInd];
                if (lpSummary->scaleInfo.scalable)
                {
                    n = MakeSizeEscape((lpESC)&escape, lpSummary->dfPitchAndFamily,
                    PixHeight, ((lpSummary->dfPitchAndFamily & 0x1) ?
                    lpSummary->dfVertRes :
                    lpSummary->dfHorizRes),
                    lpSummary->dfAvgWidth,
                    lpSummary->scaleInfo.emMasterUnits);
                    if ((n + 1) > len)
                        n = len - 1;
                    lmemcpy(lpDest, (LPSTR)&escape, n);
                    lpDest = &lpDest[n];
                }
                /*** Tetra II end ***/
                n = MakeEscape((lpESC)&escape, DES_FONT,
                lpFontSummary->f[fontInd].fontID);
                if ((n + 1) > len)
                    n = len - 1;
                lmemcpy(lpDest, (LPSTR)&escape, n);
                lpDest[n] = '\0';
                UpdateSoftInfo(lpDevice, lpFontSummary, fontInd);
                ind = 0;
            }
            else
                ind = -1;
        }
  
        /*  String not loaded.
        */
        if (ind <= -1)
        {
            DBGloadfontstring(("LoadFontString(): string does *not* exist\n"));
            lpDest[0] = '\0';
            status = FALSE;
        }
  
        /*  Free fontSummary struct.
        */
        unlockFontSummary(lpDevice);
    }
    else
    {
        DBGerr(("LoadFontString(): could *not* lock hFontSummary\n"));
        lpDest[0] = '\0';
        status = FALSE;
    }
  
    DBGloadfontstring(("LoadFontString(): return %s\n",
    status ? "SUCCESS" : "FAILURE"));
  
#ifdef DEBUG_FUNCT
    DB(("Exiting LoadFontString\n"));
#endif
    return (status);
}
  
  
/*  LoadWidthTable
*
*  Load the character extents table corresponding to the fontSummary
*  font at the passed in index.
*/
LPSTR far PASCAL LoadWidthTable(lpDevice, lpFont)
LPDEVICE lpDevice;
LPFONTINFO lpFont;
{
    LPFONTSUMMARY lpSummary;
    LPFONTSUMMARYHDR lpFontSummary;
    LPSTR lpWidthTable = 0L;
    BOOL status = TRUE;
    short fontInd;
  
#ifdef WIN31

    RASTCAPINFO RastCap;
  
    RastCap.nFlags = 0;
    if (GetVersion() >> 8)
        GetRasterizerCaps((LPRASTCAPINFO)&RastCap,(DWORD)sizeof(RASTCAPINFO));
  
    if (RastCap.nFlags & CAPS_TTENABLED)
        {
            LPTTFONTINFO lpttfi;
  
            if (lpFont->dfType & TYPE_TRUETYPE)
            {
                /* its a truetype font
                * the widths are in the font structure
                */
  
                lpttfi = (LPTTFONTINFO)lpFont->dfBitsOffset;
                if (!(lpFont->dfType & TYPE_HAVEWIDTHS))
                {
                    /* if they aren't initialized, get 'em from GDI/Engine
                    */
                    if (!EngineGetCharWidth(lpFont,lpFont->dfFirstChar,
                        lpFont->dfLastChar,lpttfi->rgwWidths))
                    {
                        /* didn't get em, fail (and don't set bit)
                        */
                        return NULL;
                    }
  
                    lpFont->dfType |= TYPE_HAVEWIDTHS;
                }
  
                return (LPSTR)lpttfi->rgwWidths;
            }
        }
#endif
  
    fontInd = ((LPPRDFONTINFO)lpFont)->indFontSummary;
  
    DBGloadwidthtable(("LoadWidthTable(%lp,%d)\n", lpDevice, fontInd));
  
    if (lpFontSummary = lockFontSummary(lpDevice))
    {
        /*  FontSummary successfully locked, make sure fontInd is in a
        *  valid range.
        */
        if ((fontInd >= 0) && (fontInd < lpFontSummary->len))
        {
            /*  fontInd is valid, get pointer to fontSummary info and
            *  attempt to lock down the widthTable if it has already once
            *  been loaded.
            */
            lpSummary = &lpFontSummary->f[fontInd];
  
            if (!lpSummary->hWidthTable ||
                !(lpWidthTable = GlobalLock(lpSummary->hWidthTable)))
            {
                if (!(lpWidthTable = loadWidths(lpFontSummary, lpSummary)))
                {
                    DBGerr(("LoadWidthTable(): could *not* load width table\n"));
                    lpSummary->hWidthTable = 0;
                    unlockFontSummary(lpDevice);
                }
                #ifdef DBGnewidthtable
                else {
                    DBMSG(("LoadWidthTable(): NEW width table created at fontInd %d\n", fontInd));
                }
                #endif
            }
            #ifdef LOCAL_DEBUG
            else {
                DBGloadwidthtable(("LoadWithTable(): width table already exists, successfully locked\n"));
            }
            #endif
        }
        else
        {
            DBGerr(("LoadWidthTable(): received invalid fontInd (%d)\n",
            fontInd));
            unlockFontSummary(lpDevice);
        }
    }
    #ifdef LOCAL_DEBUG
    else {
        DBGerr(("LoadWidthTable(): could *not* lock hFontSummary\n"));
    }
    #endif
  
    DBGloadwidthtable(("...end of LoadWidthTable, return %lp\n",
    lpWidthTable));
  
#ifdef DEBUG_FUNCT
    DB(("Exiting LoadWidthTable\n"));
#endif
    return (lpWidthTable);
}
  
  
void far PASCAL UnloadWidthTable(lpDevice, fontInd)
LPDEVICE lpDevice;
short fontInd;
{
    LPFONTSUMMARY lpSummary;
    LPFONTSUMMARYHDR lpFontSummary;
  
#ifdef DEBUG_FUNCT
    DB(("Entering UnloadWidthTable\n"));
#endif
    DBGloadwidthtable(("UnloadWidthTable(%lp,%d)\n", lpDevice, fontInd));
  
    /*  Lock down (again) the fontSummary struct.  LoadWidthTable already
    *  locked it, we call lock again to pick up the pointer to the struct.
    */
    if (lpFontSummary = lockFontSummary(lpDevice))
    {
        /*  FontSummary successfully locked, make sure fontInd is in a
        *  valid range.
        */
        if ((fontInd >= 0) && (fontInd < lpFontSummary->len))
        {
            /*  fontInd is valid, get pointer to fontSummary info and
            *  if the handle to the width table exists, unlock the
            *  width table
            */
            lpSummary = &lpFontSummary->f[fontInd];
  
            if (lpSummary->hWidthTable)
            {
                DBGloadwidthtable((
                "UnloadWidthTable(): unlocking width table\n"));
                GlobalUnlock(lpSummary->hWidthTable);
            }
        }
        #ifdef LOCAL_DEBUG
        else {
            DBGerr(("UnloadWidthTable(): received invalid fontInd (%d)\n",
            fontInd));
        }
        #endif
    }
    #ifdef LOCAL_DEBUG
    else {
        DBGerr(("UnloadWidthTable(): could *not* lock hFontSummary\n"));
    }
    #endif
  
    unlockFontSummary(lpDevice);
#ifdef DEBUG_FUNCT
    DB(("Exiting UnloadWidthTable\n"));
#endif
}
  
  
/***************************************************************************/
/**************************   Local Procedures   ***************************/
  
  
/*  loadWidths
*
*  Load the width table from the resource or soft font info.
*/
LOCAL LPSTR loadWidths(lpFontSummary, lpSummary)
LPFONTSUMMARYHDR lpFontSummary;
LPFONTSUMMARY lpSummary;
{
    LPSTR lpWidthTable = 0L;
    unsigned sizeWidthTable;
    extern HANDLE hLibInst;
  
    /* Tetra -- additional vars used to unscramble Roman-8 widths */
    short far * Scrambled = 0L; /* ptr to copy of scrambled widths */
    short far * Unscrambled;        /* ptr to widths after translation */
    HANDLE hScrambled;          /* handle to scrambled segment     */
    BYTE tbl_ind, fix_ind;      /* indices for width tbl & fix tbl */
    BYTE num_chars;         /* number of widths in widthtable  */
  
#ifdef DEBUG_FUNCT
    DB(("Entering loadWidths\n"));
#endif
    DBGentry(("loadWidths(%lp,%lp)\n", lpFontSummary, lpSummary));
  
    /*** Tetra begin ***/
    DBGentry(("loadWidths: offset= %d\n", lpSummary->offset));
    /*** Tetra end ***/
  
    /*  Calc size of width table.
    */
    sizeWidthTable =
    (lpSummary->dfLastChar - lpSummary->dfFirstChar + 2) * 2;
  
    /*  Free the handle to the width table if it exists.
    */
    if (lpSummary->hWidthTable)
    {
        GlobalFree(lpSummary->hWidthTable);
        lpSummary->hWidthTable = 0;
    }
  
    /*  Attempt to allocate the width table.
    */
    if (!(lpSummary->hWidthTable =
        GlobalAlloc(GMEM_MOVEABLE | GMEM_LOWER | GMEM_DDESHARE | GMEM_DISCARDABLE,
        (DWORD)sizeWidthTable)))
    {
        DBGerr(("loadWidths(): Could *not* alloc width table\n"));
        goto backout2;
    }
  
    /*  Attempt to lock down the width table.
    */
    if (!(lpWidthTable = GlobalLock(lpSummary->hWidthTable)))
    {
        DBGerr(("loadWidths(): Could *not* lock width table\n"));
        goto backout1;
    }
  
    if (lpSummary->indPFMName > -1)
    {
        /*  The fontSummary font came from a .pfm file.
        */
        if (!loadStructFromFile(lpFontSummary, lpSummary,
            lpWidthTable, FNTLD_WIDTHS))
        {
            goto backout0;
        }
    }
    else
    {
        /*  The fontSummary font came from a resource (ROM or cartridge
        *  font), load and lock the resource file.
        */
        LPPFMHEADER lpPFM;
        HANDLE hResData, hResInfo;
        /*** Tetra begin ***/
        LPSTR lpSource,
        lpPFMExt;
        PFMEXTENSION PFMExt;
        /*** Tetra end ***/
  
  
        /*  Find the font resource.
        */
        if (!(hResInfo = FindResource(hLibInst,
            (LPSTR)(long)(lpSummary->offset), (LPSTR)(long)MYFONT)))
        {
            DBGerr(("loadWidths(): Could not *find* resource\n"));
            goto backout0;
        }
  
        /*  Load (actually, only locate) font resource.
        */
        if (!(hResData = LoadResource(hLibInst, hResInfo)))
        {
            DBGerr(("loadWidths(): Could not *load* resource\n"));
            goto backout0;
        }
  
        /*  Lock (and load) font resource.
        */
        if (!(lpPFM = (LPPFMHEADER)LockResource(hResData)))
        {
            DBGerr(("loadWidths(): Could not *lock* resource\n"));
            FreeResource(hResData);
            goto backout0;
        }
  
        /*  Copy width table from resource file.
        */
  
        /*** Tetra begin ***/
        if (lpSummary->scaleInfo.scalable)
        {
            lpPFMExt = ((LPSTR)lpPFM->dfCharOffset + sizeWidthTable);
            lmemcpy ((LPSTR)&PFMExt, lpPFMExt, sizeof(PFMEXTENSION));
            lpSource = (LPSTR)lpPFM + PFMExt.dfExtentTable;
        }
        else
            lpSource = (LPSTR)lpPFM->dfCharOffset;
  
        lmemcpy(lpWidthTable, lpSource, sizeWidthTable);
  
#ifdef DEBUG
        {
            char Face[20];
            LPSTR lpFace, lpFaceArr;
            short far *w;
  
            lpFaceArr = (LPSTR)Face;
            lpFace = (LPSTR)lpPFM + lpPFM->dfFace;
            lmemcpy(lpFaceArr, lpFace, (WORD)20);
            DBGentry(("loadWidths(): FaceName= %ls , dfFace= %ld, dfAscent= %d, dfEL= %d\n",
            lpFaceArr, lpPFM->dfFace, lpPFM->dfAscent,
            lpPFM->dfExternalLeading));
            DBGentry(("loadWidths(): lmemcpy: dfCharOffset= %d, lpCO= %lp, dfET= %ld,\n   lpExt= %lp\n",
            lpPFM->dfCharOffset, (LPSTR)lpPFM->dfCharOffset, PFMExt.dfExtentTable, lpPFMExt));
            w = (short far *)lpSource;
            DBGentry(("loadWidths(): lpstrw= %lp, w= %lp, *w= %d, szPFMHdr= %d\n",
            lpSource, w, *w, sizeof(PFMHEADER)-2));
        }
#endif
        /*** Tetra end ***/
  
        /*  Free up resource.
        */
        GlobalUnlock(hResData);
        FreeResource(hResData);
    }


#if 0
  
    /* If it's Roman-8, the widths have been scrambled (even 
     * in the PFMs!).  Fix the widths that haven't been lost 
     * in the original scrambling. - klo                          
     *
     * We put Roman-8 re-mapping back in, so take this stuff 
     * back out so the scrapbled widths will be used. - dtk 1-92
     */
  
  
    if (lpSummary->symbolSet == epsymRoman8)
    {
  
        DBGscramble(("Roman-8.  About to alloc.\n"));
  
        if ((hScrambled = GlobalAlloc(GMEM_MOVEABLE, (DWORD)sizeWidthTable)) &&
            ((LPSTR)Scrambled = GlobalLock(hScrambled)))
        {
  
            DBGscramble(("Alloced.  About to lmemcpy.\n"));
  
            lmemcpy((LPSTR)Scrambled, lpWidthTable, sizeWidthTable);
  
            DBGscramble(("lmemcpy'ed.  About to swap widths.\n"));
  
            Unscrambled = (short far *)lpWidthTable;
            tbl_ind = (BYTE)0xa0 - lpSummary->dfFirstChar;
            fix_ind = -1;   /* After 1st incr, it'll be zero */
            num_chars = lpSummary->dfLastChar - lpSummary->dfFirstChar;
  
            DBGscramble(("tbl_ind = %d\n", tbl_ind));
  
            do
            {
                tbl_ind++;
                fix_ind++;
  
                DBGscramble(("Fix_R8[%d] = %d,",(int)fix_ind,Fix_R8[fix_ind]));
  
                Unscrambled[tbl_ind] =
                Scrambled[Fix_R8[fix_ind] - lpSummary->dfFirstChar];
  
                DBGscramble((" lpWidthTable[%d] = %d,", (int)tbl_ind,
                Unscrambled[tbl_ind]));
  
                DBGscramble((" Scrambled[%d] = %d\n", (int)tbl_ind,
                Scrambled[tbl_ind]));
            }
            while (tbl_ind != num_chars);
  
            DBGscramble(("Swapped widths.  About to Unlock and Free.\n"));
  
            GlobalUnlock(hScrambled);
            GlobalFree(hScrambled);
        }
    }

#endif
  
    DBGscramble(("All done with unscrambling.\n"));
  
#ifdef DBGdumpwidths
    {
        short ind, dpoint, pixht10, pixht;
        BYTE ch, last;
        short far * DBGwidth = (short far *)lpWidthTable;
        last = lpSummary->dfLastChar - lpSummary->dfFirstChar;
  
        DBMSG(("loadWidths(): dump of width table, size=%ld\n",
        GlobalSize(lpSummary->hWidthTable)));
        if (lpSummary->scaleInfo.scalable)
        {
            for (dpoint = DPOINT1; dpoint < 64; dpoint++)
            {
                DBMSG(("\n------------------------------------------------------------------\n"));
                DBMSG(("loadWidths(): Scaled widths for %d point font.\n", dpoint*2));
                pixht =
                (((pixht10 = (short) labdivc (lmul ((long)dpoint, (long)20),
                (long) 300,
                (long) 72)) % 10) < 5) ?
                (pixht10 / 10) :
                ((pixht10 / 10) + 1);
  
                for (ind = 1, ch = 0; ch < last; ++ch, ++ind)
                {
                    if ((ind % 10) == 0)
                        DBMSG(("\n"));
                    DBMSG(("%c%d  ", (char)(ch + lpSummary->dfFirstChar),
                    ScaleWidth((long) DBGwidth[ch],
                    (long) lpSummary->scaleInfo.emMasterUnits,
                    (long) pixht,
                    (long) lpSummary->dfVertRes)));
                }
                DBMSG(("\f"));
            }
        }
        else
        {
            for (ind = 1, ch = 0; ch < last; ++ch, ++ind)
            {
                if ((ind % 10) == 0)
                    DBMSG(("\n"));
                DBMSG(("%c%d  ", (char)(ch + lpSummary->dfFirstChar), DBGwidth[ch]));
            }
        }
        DBMSG(("\n"));
    }
#endif
    /* Tetra end */
  
    /*  Normal return.
    */
    goto done;
  
    backout0:
    /*  Error return.
    */
    GlobalUnlock(lpSummary->hWidthTable);
  
    backout1:
    GlobalFree(lpSummary->hWidthTable);
  
    backout2:
    lpSummary->hWidthTable = 0L;
    lpWidthTable = 0L;
  
    done:
#ifdef DEBUG_FUNCT
    DB(("Exiting loadWidths\n"));
#endif
    return (lpWidthTable);
}
