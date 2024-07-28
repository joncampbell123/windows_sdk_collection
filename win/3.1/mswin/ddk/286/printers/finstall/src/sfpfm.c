
/**[f******************************************************************
* sfpfm.c -
*
* Copyright (C) 1988,1989 Aldus Corporation
* Copyright (C) 1989-1990 Microsoft Corporation.
* Copyright (C) 1990,1991 Hewlett-Packard Company.
*     All rights reserved.
*     Company confidential.
*
**f]*****************************************************************/
  
/*
 * $Header: 
 */

/*
 * $Log:
 */

/********************************   sfpfm.c   ******************************/
/*
* / SFpfm:  utilities for reading downloadable font files and generating
*          Printer Font Metrics (PFM) files.
*/
//*******************************************************************
//
// History
//  27 nov 91   RK(HP)      In djcharinfo cast lpCHdata->lpTrans[ind] to a BYTE
//  11 oct 91   RK(HP)      In ljcharinfo cast lpCHdata->lpTrans[ind] to a BYTE
//
//  04 Sept91   RK(hp)      Modified OpenFile comparisons in useDupPFM
//                          (hsrcFile and hcmpFile)
//  25 sep 89   peterbe     Changes marked '25sep89' from
//                  Steve Dentel @ hp.
//
//  20 sep 89   peterbe     Just added some DBGx messages
//
//  01 aug 89   peterbe     Adding calls to LZEXPAND.MOD
//                  for reading 'download' font files.
//
//  17 jul 89   peterbe     checked in multiple changes for
//                  DeskJet support.
/***************************************************************************/
//
// PLEASE EDIT THIS FILE USING THE FOLLOWING GUIDELINES:
//  Tabs = EIGHT spaces
//  max no. of columns = 80
//  If a statement is > 80 columns, PLEASE continue on NEXT line.
//
/***************************************************************************/
  
  
//#define DEBUG
  
extern int gPrintClass;
#include "nocrap.h"
#undef NOGDI
#undef NOMEMMGR
#undef NOOPENFILE
#include "windows.h"
#include "neededh.h"
#include "pfm.h"
#include "sfpfm.h"
#include "transtbl.h"
#include "strings.h"
#include "deskjet.h"
#include "expand.h"
  
  
WIDTH_TABLE dj[256];  /* must stick around for each call to djcharinfo */
  
  
/****************************************************************************\
* Debug Definitions
\****************************************************************************/
  
#ifdef DEBUG
    #define DBGpfm(msg) /*DBMSG(msg)*/
    #define DBGx(msg)   /*DBMSG(msg)*/
#else
    #define DBGpfm(msg) /*nulla*/
    #define DBGx(msg)           /*nulla*/
#endif
  
  
  
  
#ifdef DEBUG
#undef DUMP_PFM
#endif
  
  
#define LOCAL static
  
#define MAX_INVALIDCHARS 32
  
#define swab(x) ((((x) << 8) & 0xFF00) | (((x) >> 8) & 0xFF))
  
#include "desc.h"
  
LOCAL BYTE readnum(int, int FAR *);
LOCAL BOOL fontdes(HANDLE, LPPFMHEADER, LPEXTTEXTMETRIC, LPDRIVERINFO,
LPSTR, int, LPFONTDES);
LOCAL void djfontdes(HANDLE, LPPFMHEADER, LPEXTTEXTMETRIC, LPDRIVERINFO,
LPSTR, int, LPDJFONTDES);
LOCAL void ljfontdes(HANDLE, LPPFMHEADER, LPEXTTEXTMETRIC, LPDRIVERINFO,
LPSTR, int, LPLJFONTDES);
LOCAL BOOL charinfo(LPEXTTEXTMETRIC, WORD FAR *, BYTE, LPCHARDES, LPCHDATA);
LOCAL BOOL ljcharinfo(LPEXTTEXTMETRIC, WORD FAR *, BYTE, LPCHARDES, LPCHDATA);
LOCAL BOOL djcharinfo(LPEXTTEXTMETRIC, WORD FAR *, BYTE, LPCHARDES, LPCHDATA);
LOCAL WORD TranslateWeight(int);
LOCAL BYTE TranslateCharSet(HANDLE, LPFONTDES, LPDRIVERINFO, LPSTR, int);
LOCAL BYTE TranslateFamily(HANDLE, BYTE, LPSTR, int);
  
#ifdef DUMP_PFM
#define DBGdumpPFM(a,b,c,d) dumpPFM(a,b,c,d)
#define DBGdumpCHdata(a) dumpCHdata(a)
#define DBGdumpWidths(a) dumpWidths(a)
LOCAL void dumpPFM(LPPFMHEADER, LPPFMEXTENSION, LPEXTTEXTMETRIC, LPDRIVERINFO);
LOCAL void dumpCHdata(LPCHDATA);
LOCAL void dumpWidths(WORD FAR *);
#else
#define DBGdumpPFM(a,b,c,d) /*null*/
#define DBGdumpCHdata(a) /*null*/
#define DBGdumpWidths(a) /*null*/
#endif
  
/**************************************************************************/
/****************************   Global Procs   ****************************/
  
/*  DLtoPFM
*
*  Fill in the structures which are passed in -- not all structures
*  have to be passed in, just the ones containing information the
*  caller needs.  In order to write a PFM file, all structures must
*  be provided.
*/
BOOL FAR PASCAL DLtoPFM(hDlFile, hMd, shortScan, lpPFMhead, lpWidths, lpExtText,
lpDrvInfo, lpFace, facesz, lpBuf, bufsz)
int hDlFile;
HANDLE hMd;
BOOL shortScan;
LPPFMHEADER lpPFMhead;
WORD FAR *lpWidths;
LPEXTTEXTMETRIC lpExtText;
LPDRIVERINFO lpDrvInfo;
LPSTR lpFace;
int facesz;
LPSTR lpBuf;
int bufsz;
{
    CHDATA chdata;
    BOOL realPFM = FALSE;
    int num;
    char esc, tmpc, ch, curchar;
  
    curchar = 0;    // 25sep89
  
    DBGpfm(("DLtoPFM(%d,%d,%lp,%lp,%lp,%lp,%d,%lp,%d)\n",
    hDlFile, shortScan, lpPFMhead, lpWidths, lpExtText, lpFace,
    facesz, lpBuf, bufsz));
  
    /*  Explicitly set these to zero up front.
    */
    chdata.hTrans = 0;
    chdata.lpTrans = 0L;
  
    DBGpfm(("DLtoPFM(): many lzRead(%d) calls:\n", hDlFile));
    while (lzRead(hDlFile, (LPSTR)&esc, 1) > 0)
    {
        if ((esc == '\033') &&
            (lzRead(hDlFile,(LPSTR)&ch,1) > 0) &&
            (ch == ')' || ch == '(') &&
            (lzRead(hDlFile,(LPSTR)&tmpc,1) > 0) &&
            (tmpc == 's') &&
            (tmpc = readnum(hDlFile, &num)) &&
            (tmpc == 'W'))
        {
            /*  Escape for defining a font header or a character.
            */
            lzRead(hDlFile, lpBuf, (num > bufsz ? bufsz : num));
  
            if (ch == ')')
            {
                /*  Font header
                *
                *  Initialize all passed-in structs.
                */
                if (lpPFMhead)
                    lmemset((LPSTR)lpPFMhead, 0, sizeof(PFMHEADER));
                if (lpWidths)
                    lmemset((LPSTR)lpWidths, 0, 256*2);
                if (lpExtText)
                    lmemset((LPSTR)lpExtText, 0, sizeof(EXTTEXTMETRIC));
                if (lpDrvInfo)
                    lmemset((LPSTR)lpDrvInfo, 0, sizeof(DRIVERINFO));
                if (lpFace)
                    lmemset(lpFace, 0, facesz);
  
                lmemset((LPSTR)&chdata, 0, sizeof(CHDATA));
                chdata.firstchar = 255;
                chdata.hMd = hMd;
  
                /*  Read font header.  If fontdes returns false, then this
                *  particular softfont is not the appropriate gPrintClass.
                */
                if (!fontdes(hMd,lpPFMhead,lpExtText,
                    lpDrvInfo,lpFace,facesz,(LPFONTDES)lpBuf))
                {
                    DBGx(("DLtoPFM(): realPFM = FALSE\n"));
                    break;
                }
                realPFM = TRUE;
                DBGx(("DLtoPFM(): realPFM = TRUE\n"));
  
                /*  Jam the width of the space just in case there
                *  is not a space in the font file.
                */
                if (lpWidths)
                    lpWidths[32] = swab(((LPGENERIC_FONTDES)lpBuf)->pitch) / 4;
  
                /*  Pick up symbol set.
                */
                if (lpDrvInfo)
                    chdata.symbolSet = lpDrvInfo->xtbl.symbolSet;
  
                /*  Finish if we just want the header information.
                */
                if (shortScan)
                    break;
            }
            else
            {
                /*  Character definition
                *
                *  Break if we did not read a font header before the
                *  character definition.
                */
                if (!realPFM)
                {
                    DBGpfm((
                    "DLtoPFM(): encountered character before header, fail\n"));
                    break;
                }
  
                if (!charinfo(lpExtText,lpWidths,curchar,
                    (LPCHARDES)lpBuf,&chdata))
                    return FALSE;
            }
  
            /*  Read the rest of the buf -- this should never happen.
            */
            for (num -= bufsz; num > 0; num -= bufsz)
            {
                lzRead(hDlFile, lpBuf, (num > bufsz ? bufsz : num));
            }
        }
        else if ((esc == '\033') &&
            (ch == '*') &&
            (lzRead(hDlFile,(LPSTR)&tmpc,1) > 0) &&
            (tmpc == 'c') &&
            (tmpc = readnum(hDlFile, &num)) &&
            (tmpc == 'E'))
        {
            /*  Escape for identifying a character code (precedes the
            *  definition of a character).
            */
            curchar = (BYTE)((num > 255) ? 0 : num);
        }
        else if (!realPFM)
        {
            DBGpfm(("DLtoPFM(): invalid text, return FALSE\n"));
            return FALSE;
        }
    }
  
    /*  Unlock and free translation table.
    */
    if (chdata.lpTrans)
    {
        GlobalUnlock(chdata.hTrans);
        chdata.lpTrans = 0L;
    }
  
    /*  Pick up the rest of the information after scanning the
    *  characters.
    */
    if (realPFM && !shortScan && lpPFMhead)
    {
        /*  Force the first char to be no higher than the space.
        */
        if (chdata.firstchar > 32)
            chdata.firstchar = 32;
  
        lpPFMhead->dfPitchAndFamily |= chdata.pitch;
        lpPFMhead->dfPixWidth = ((chdata.pitch) ? 0 : lpWidths[32]);
        if (chdata.count > 0)
            lpPFMhead->dfAvgWidth = (WORD)ldiv(chdata.totalWidth,chdata.count);
        lpPFMhead->dfMaxWidth = chdata.maxWidth;
        lpPFMhead->dfFirstChar = chdata.firstchar;
        lpPFMhead->dfLastChar = chdata.lastchar;
        lpPFMhead->dfDefaultChar = (BYTE)(HP_DF_CH - chdata.firstchar);
        lpPFMhead->dfBreakChar = (BYTE)(32 - chdata.firstchar);
  
        if (lpDrvInfo)
        {
            lpDrvInfo->epMemUsage =
            FontMEM(1,chdata.totalWidth,(long)lpPFMhead->dfPixHeight);
        }
  
        if (lpExtText)
        {
            lpExtText->emSuperScript =
            lpExtText->emCapHeight - lpExtText->emXHeight;
            switch (gPrintClass)
            {
                case CLASS_DESKJET:
                case CLASS_DESKJET_PLUS:
                    lpExtText->emSubScript = 1 +
                    ((LPDJFONTDES)lpBuf)->bottom_underline_pos;
                    lpExtText->emDoubleUpperUnderlineOffset =
                    ((LPDJFONTDES)lpBuf)->top_underline_pos;
                    lpExtText->emDoubleLowerUnderlineOffset =
                    ((LPDJFONTDES)lpBuf)->bottom_underline_pos;
                    break;
                case CLASS_LSRJETIII:
                case CLASS_PAINTJET:
                case CLASS_LASERJET:
                    lpExtText->emSubScript =
                    -lpExtText->emLowerCaseDescent;
                    lpExtText->emDoubleUpperUnderlineOffset =
                    lpExtText->emUnderlineOffset +
                    lpExtText->emUnderlineWidth * 2;
                    lpExtText->emDoubleLowerUnderlineOffset =
                    lpExtText->emUnderlineOffset;
                    break;
            }
            lpExtText->emDoubleUpperUnderlineOffset =
            lpExtText->emUnderlineOffset + lpExtText->emUnderlineWidth * 2;
            lpExtText->emDoubleLowerUnderlineOffset =
            lpExtText->emUnderlineOffset;
            lpExtText->emDoubleUpperUnderlineWidth =
            lpExtText->emUnderlineWidth;
            lpExtText->emDoubleLowerUnderlineWidth =
            lpExtText->emUnderlineWidth;
        }
  
        DBGdumpCHdata(&chdata);
    }
  
    DBGx(("DLtoPFM(): return %ls.\n",
    (realPFM)? (LPSTR)"TRUE" : (LPSTR)"FALSE"));
  
    return (realPFM);
  
}   // DLtoPFM()
  
/*  writePFM
*
*  Write a PFM file (this proc is NOT like DLtoPFM, ALL structures
*  must be provided!
*/
long FAR PASCAL writePFM(hPfmFile, lpPFMhead, lpWidths, lpPFMext,
lpExtText, lpDrvInfo, lpFace, lpDevice)
int hPfmFile;
LPPFMHEADER lpPFMhead;
WORD FAR *lpWidths;
LPPFMEXTENSION lpPFMext;
LPEXTTEXTMETRIC lpExtText;
LPDRIVERINFO lpDrvInfo;
LPSTR lpFace;
LPSTR lpDevice;
{
    int wsize;
    long w;
  
    DBGpfm(("writePFM(%d,%lp,%lp,%lp,%lp,%lp,%lp,%lp): %ls\n",
    hPfmFile, lpPFMhead, lpWidths, lpPFMext, lpExtText,
    lpDrvInfo, lpFace, lpDevice, lpFace));
  
    /*  Size of width table (zero if fixed pitch).
    */
    if (lpPFMhead->dfPitchAndFamily & 0x01)
        wsize = (lpPFMhead->dfLastChar - lpPFMhead->dfFirstChar + 2) * 2;
    else
        wsize = 0;
  
    /*  Set size and pointers to all the structures.
    */
    lpPFMhead->dfFace = sizeof(PFMHEADER) - 2 + wsize + sizeof(PFMEXTENSION);
    lpPFMhead->dfDevice = lpPFMhead->dfFace + lstrlen(lpFace) + 1;
  
    lpPFMext->dfSizeFields = sizeof(PFMEXTENSION);
    lpPFMext->dfExtMetricsOffset = lpPFMhead->dfDevice + lstrlen(lpDevice) + 1;
    lpPFMext->dfExtentTable = 0L;
    lpPFMext->dfOriginTable = 0L;
    lpPFMext->dfPairKernTable = 0L;
    lpPFMext->dfTrackKernTable = 0L;
    lpPFMext->dfDriverInfo = lpPFMext->dfExtMetricsOffset +
    sizeof(EXTTEXTMETRIC);
    lpPFMext->dfReserved = 0L;
  
    lpExtText->emSize = sizeof(EXTTEXTMETRIC);
    lpDrvInfo->epSize = sizeof(DRIVERINFO);
  
    lpPFMhead->dfSize = lpPFMext->dfDriverInfo + sizeof(DRIVERINFO);
  
    DBGdumpPFM(lpPFMhead, lpPFMext, lpExtText, lpDrvInfo);
    DBGdumpWidths(lpWidths);
  
    /*  Write PFM file.
    */
    w = _lwrite(hPfmFile, (LPSTR)lpPFMhead, sizeof(PFMHEADER)-2);
    if (wsize)
        w += _lwrite(hPfmFile, (LPSTR)&lpWidths[lpPFMhead->dfFirstChar], wsize);
    w += _lwrite(hPfmFile, (LPSTR)lpPFMext, sizeof(PFMEXTENSION));
    w += _lwrite(hPfmFile, lpFace, lstrlen(lpFace)+1);
    w += _lwrite(hPfmFile, lpDevice, lstrlen(lpDevice)+1);
    w += _lwrite(hPfmFile, (LPSTR)lpExtText, sizeof(EXTTEXTMETRIC));
    w += _lwrite(hPfmFile, (LPSTR)lpDrvInfo, sizeof(DRIVERINFO));
  
    return (w);
}
  
/*  useDupPFM
*
*  Search for PFM files sharing a similar file name and see if their
*  contents are the same.  If they are, then erase that passed in file
*  and change its name to the duplicate PFM file.
*
*  This proc assumes the name of the PFM file was made by openUniqPFM()
*  in sfadd2.
*/
void FAR PASCAL useDupPFM(lpPFMnm, lpBuf, bufsz)
LPSTR lpPFMnm;
LPSTR lpBuf;
int bufsz;
{
    LPSZPFMHEAD lpSZhead;
    LPSTR lpTwiddle;
    LPSTR b1, b2, b3, b4;
    DWORD srcSize;
    BOOL dup;
    char savech;
    int hsrcFile;
    int hcmpFile;
    int blen;
    int j, k;
  
    DBMSG(("useDupPFM(%lp,%lp,%d): %ls\n", lpPFMnm, lpBuf, bufsz, lpPFMnm));
  
    /*  Step back to the eighth character in the file name,
    *  this is the character that was changed from 0-9, then A-Z,
    *  until a unique PFM name was found.
    */
    for (lpTwiddle=lpPFMnm+lstrlen(lpPFMnm)-1;
        lpTwiddle > lpPFMnm && lpTwiddle[1] != '.'; --lpTwiddle)
        ;
  
    if ((lpTwiddle > lpPFMnm) &&
        (bufsz > sizeof(OFSTRUCT)) &&
        (bufsz > sizeof(SZPFMHEAD)) &&
        ((*lpTwiddle >= '0' && *lpTwiddle <= '9') ||
        (*lpTwiddle >= 'A' && *lpTwiddle <= 'Z') ||
        (*lpTwiddle >= 'a' && *lpTwiddle <= 'z')) &&
        (*lpTwiddle > '0'))
    {
        /*  Other files exist that share similar file names,
        *  test to see if their contents are the same.
        */
        savech = *lpTwiddle;
  
        /*  Make sure file name is upper case.
        */
        if (savech >= 'a' && savech <= 'z')
        {
            AnsiUpper(lpPFMnm);
            savech = *lpTwiddle;
        }
  
        /*  Open source file.
        */
//      if ((hsrcFile=OpenFile(lpPFMnm,(LPOFSTRUCT)lpBuf,OF_READ)) > 0)
        if ((hsrcFile=OpenFile(lpPFMnm,(LPOFSTRUCT)lpBuf,OF_READ)) >=0)                  // RK 09/04/91
        {
            _lread(hsrcFile, lpBuf, sizeof(SZPFMHEAD));
            lpSZhead = (LPSZPFMHEAD)lpBuf;
            srcSize = lpSZhead->dfSize;
  
            DBMSG(("useDupPFM(): looking for dup to %ls, size=%ld\n",
            lpPFMnm, srcSize));
  
            blen = bufsz / 2;
            b1 = lpBuf;
            b2 = &lpBuf[blen];
            dup = FALSE;
  
            do {
                /*  Change the file name.
                */
                if (*lpTwiddle == 'A')
                {
                    *lpTwiddle = '9';
                }
                else
                {
                    --(*lpTwiddle);
                }
  
                /*  Open compare file.
                */
//              if ((hcmpFile=OpenFile(lpPFMnm,(LPOFSTRUCT)lpBuf,OF_READ)) > 0)
                if ((hcmpFile=OpenFile(lpPFMnm,(LPOFSTRUCT)lpBuf,OF_READ)) >= 0)         // RK 09/04/91
                {
                    _lread(hcmpFile, lpBuf, sizeof(SZPFMHEAD));
                    lpSZhead = (LPSZPFMHEAD)lpBuf;
  
                    DBMSG(("useDupPFM(): ...compare to %ls, size=%ld\n",
                    lpPFMnm, lpSZhead->dfSize));
  
                    if (lpSZhead->dfSize == srcSize)
                    {
                        /*  Files are the same size, do a byte-for-byte
                        *  compare of the two files.
                        */
                        _llseek(hsrcFile, 0L, 0);
                        _llseek(hcmpFile, 0L, 0);
  
                        do {
                            j = 0;
                            k = _lread(hsrcFile, b1, blen);
  
                            if (_lread(hcmpFile, b2, blen) == k)
                            {
                                for (b3=b1,b4=b2; j < k && *b3 == *b4;
                                    ++j, ++b3, ++b4)
                                    ;
                            }
                        } while (j == blen);
  
                        dup = (j == k);
                    }
  
                    _lclose(hcmpFile);
                }
  
            } while (*lpTwiddle > '0' && !dup);
  
            _lclose(hsrcFile);
  
            if (dup)
            {
                /*  Duplicate found, erase the passed-in file and
                *  change the name to that of the duplicate.
                */
                char ch;
  
                DBMSG(("useDupPFM(): duplicate PFM found in %ls\n", lpPFMnm));
                ch = *lpTwiddle;
                *lpTwiddle = savech;
  
                OpenFile(lpPFMnm, (LPOFSTRUCT)lpBuf, OF_DELETE);
  
                *lpTwiddle = ch;
            }
            else
            {
                DBMSG(("useDupPFM(): duplicate PFM not found\n"));
                *lpTwiddle = savech;
            }
        }
    #ifdef DEBUG
        else { DBMSG(("useDupPFM(): could not open %ls\n", lpPFMnm)); }
    #endif
    }
}
  
/**************************************************************************/
/*****************************   Local Procs   ****************************/
  
  
//  readnum()
//
//  read a decimal integer from a download font file.
//  The file has been opened with lzOpenFile().
//
LOCAL BYTE readnum(hDlFile, lpNum)
int hDlFile;
int FAR *lpNum;
{
    BYTE ch;
  
    *lpNum = 0;
  
    while (lzRead(hDlFile,(LPSTR)&ch,1) > 0 && ch >= '0' && ch <= '9')
    {
        *lpNum *= 10;
        *lpNum += (int)ch - (int)'0';
    }
  
    if (*lpNum == 0)
        ch = '\0';
  
    return (ch);
}
  
/*  fontdes.  Return true if the soft font matches gPrintClass.  return false
*  if not.
*/
LOCAL BOOL fontdes(hMd, lpPFMhead, lpExtText, lpDrvInfo, lpFace, facesz,
lpFontDes)
HANDLE hMd;
LPPFMHEADER lpPFMhead;
LPEXTTEXTMETRIC lpExtText;
LPDRIVERINFO lpDrvInfo;
LPSTR lpFace;
int facesz;
LPFONTDES lpFontDes;
{
    switch (gPrintClass)
    {
        case CLASS_LSRJETIII:
        case CLASS_PAINTJET:
        case CLASS_LASERJET:
            if (((LPLJFONTDES)lpFontDes)->zero1 == LASERJET_FONT)
            {
                ljfontdes(hMd, lpPFMhead, lpExtText, lpDrvInfo,
                lpFace, facesz, (LPLJFONTDES)lpFontDes);
                DBMSG(("fontdes(): exit\n", lpFace));
                return( TRUE );
            }
            else
            {
                return( FALSE );
            }
  
        case CLASS_DESKJET:
            if (((LPDJFONTDES)lpFontDes)->header_format == DESKJET_FONT)
            {
                djfontdes(hMd, lpPFMhead, lpExtText, lpDrvInfo,
                lpFace, facesz, (LPDJFONTDES)lpFontDes);
                return( TRUE );
            }
            else
            {
                return( FALSE );
            }
  
        case CLASS_DESKJET_PLUS:
            if ((((LPDJFONTDES)lpFontDes)->header_format == DESKJET_FONT) ||
                (((LPDJFONTDES)lpFontDes)->header_format == DESKJET_PLUS_FONT))
            {
                djfontdes(hMd, lpPFMhead, lpExtText, lpDrvInfo,
                lpFace, facesz, (LPDJFONTDES)lpFontDes);
                return( TRUE );
            }
            else
            {
                return( FALSE );
            }
    }
}
  
// djfontdes(): fontdes for the DeskJet class of printer.
  
LOCAL void djfontdes(hMd, lpPFMhead, lpExtText, lpDrvInfo, lpFace, facesz,
lpFontDes)
HANDLE hMd;
LPPFMHEADER lpPFMhead;
LPEXTTEXTMETRIC lpExtText;
LPDRIVERINFO lpDrvInfo;
LPSTR lpFace;
int facesz;
LPDJFONTDES lpFontDes;
{
    char a[32];
    int k;
  
    if (lpDrvInfo)
    {
        lpDrvInfo->epSize = sizeof(DRIVERINFO);
        lpDrvInfo->epVersion = DRIVERINFO_VERSION;
        lpDrvInfo->epEscape = 0L;
        lpDrvInfo->xtbl.offset = 0L;
        lpDrvInfo->xtbl.len = 0;
        lpDrvInfo->xtbl.firstchar = 0;
        lpDrvInfo->xtbl.lastchar = 0;
  
        /*  Calculated later.
        */
        lpDrvInfo->epMemUsage = 0L;
        lpDrvInfo->xtbl.symbolSet = epsymUserDefined;
    }
  
    if (lpPFMhead)
    {
        lpPFMhead->dfVersion = 256;
        lpPFMhead->dfSize = 0L;
  
        lmemcpy(lpPFMhead->dfCopyright,
        lpFontDes->copyright, sizeof(lpPFMhead->dfCopyright));
        lpPFMhead->dfCopyright[sizeof(lpPFMhead->dfCopyright)-1] = '\0';
  
        lpPFMhead->dfType = 128;
        lpPFMhead->dfPoints = swab(lpFontDes->height) * 72 / 1200;
        lpPFMhead->dfVertRes = 300;
        lpPFMhead->dfHorizRes = 300;
        lpPFMhead->dfPixHeight = swab(lpFontDes->line_spacing) / 4;
        if ((75 * lpPFMhead->dfPixHeight) % 100 > 33)
            lpPFMhead->dfAscent = ((75 * lpPFMhead->dfPixHeight) / 100) + 1;
        else
            lpPFMhead->dfAscent = (75 * lpPFMhead->dfPixHeight) / 100;
        lpPFMhead->dfInternalLeading =
        lpPFMhead->dfPixHeight - lpPFMhead->dfAscent;
        if ((lpPFMhead->dfInternalLeading % 3) >= 1)
            lpPFMhead->dfInternalLeading = (lpPFMhead->dfInternalLeading / 3)
            * 2 + (lpPFMhead->dfInternalLeading % 3);
        else
            lpPFMhead->dfInternalLeading =
            (lpPFMhead->dfInternalLeading / 3) * 2;
  
        // (bad line removed 20 jul 89 -- peterbe
  
        lpPFMhead->dfExternalLeading = lpPFMhead->dfInternalLeading / 2;
        lpPFMhead->dfItalic = (BYTE)((lpFontDes->style == 1) ? 1 : 0);
        lpPFMhead->dfUnderline = 0;
        lpPFMhead->dfStrikeOut = 0;
        lpPFMhead->dfWeight = TranslateWeight(lpFontDes->stroke_weight);
        lpPFMhead->dfCharSet =
        TranslateCharSet(hMd,
        (LPFONTDES)lpFontDes, lpDrvInfo, a, sizeof(a));
        lpPFMhead->dfPitchAndFamily =
        TranslateFamily(hMd, lpFontDes->typeface, lpFace, facesz);
  
        /*  This next group of variables are set after reading the
        *  characters -- note that the 'pitch' part of dfPitchAndFamily
        *  is set later.
        */
        lpPFMhead->dfPixWidth = 0;
        lpPFMhead->dfAvgWidth = 0;
        lpPFMhead->dfMaxWidth = 0;
        lpPFMhead->dfFirstChar = 0;
        lpPFMhead->dfLastChar = 0;
        lpPFMhead->dfDefaultChar = 0;
        lpPFMhead->dfBreakChar = 0;
  
        /*  dfDevice and dfFace are set when the file is written.
        */
        lpPFMhead->dfWidthBytes = 0L;
        lpPFMhead->dfDevice = 0L;
        lpPFMhead->dfFace = 0L;
        lpPFMhead->dfBitsPointer = 0L;
        lpPFMhead->dfBitsOffset = 0L;
        lpPFMhead->dfCharOffset[0] = 0;
  
        DBGx(("djfontdes(): (1) lpFace=%ls\n", lpFace));
    }
    else if (lpFace)
    {
        TranslateCharSet(hMd, (LPFONTDES)lpFontDes, lpDrvInfo, a, sizeof(a));
        TranslateFamily(hMd, lpFontDes->typeface, lpFace, facesz);
        DBGx(("djfontdes(): (2) lpFace=%ls\n", lpFace));
    }
  
    /*  Concatenate added string to face name.
    */
    if (lpFace && (k=lstrlen(lpFace)) && (k < facesz - lstrlen(a)))
    {
        lstrcat(lpFace, a);
    }
  
    if (lpExtText)
    {
        /*  Some of the formulas used herein are compliments of the
        *  FONT INTERCHANGE STANDARD, v1.0, available from Xerox.
        *  The rest, I got from super-driver memo, or I made them up.
        */
        lpExtText->emSize = 0L;
        lpExtText->emPointSize = swab(lpFontDes->line_spacing) / 4;
        lpExtText->emPointSize *= 24;
        lpExtText->emPointSize /= 5;
        lpExtText->emOrientation = (lpFontDes->orientation) ? 2 : 1;
        lpExtText->emMasterHeight = lpPFMhead->dfPixHeight;
        lpExtText->emMinScale = lpExtText->emMasterHeight;
        lpExtText->emMaxScale = lpExtText->emMasterHeight;
        lpExtText->emMasterUnits = lpExtText->emMasterHeight;
        lpExtText->emCapHeight =
        lpPFMhead->dfAscent - lpPFMhead->dfInternalLeading;
        lpExtText->emSlant = 0;
        lpExtText->emSuperScriptSize = lpPFMhead->dfPixHeight;
        lpExtText->emSubScriptSize = lpPFMhead->dfPixHeight;
        lpExtText->emKernPairs = 0;
        lpExtText->emKernTracks = 0;
  
        /*  x-height
        *  x-height is stored in the font header in quarter dots
        */
  
        lpExtText->emXHeight = swab(lpFontDes->xHeight) / 4;
  
        /*  lower-case ascent
        *  The ascender height can be calculated by taking 58% of the nominal
        *  line spacing.  nominal line spacing is also stored in the font
        *  header in quarter dots.
        */
  
        lpExtText->emLowerCaseAscent =
        (58 * swab(lpFontDes->line_spacing) / 4) / 100;
  
        /*  lower-case descent
        *  The descender height can be calculated by taking 18% of the nominal
        *  line spacing.
        */
  
        lpExtText->emLowerCaseDescent =
        (18 * swab(lpFontDes->line_spacing) / 4) / 100;
  
        /*  underline
        */
  
        lpExtText->emUnderlineOffset = lpFontDes->underline_pos;
        lpExtText->emUnderlineWidth = lpFontDes->underline_height;
  
        /*  strike-through
        *  Use half the x-height for positioning.  Use the single underline
        *  character for the height.
        */
  
        lpExtText->emStrikeOutOffset = swab(lpFontDes->xHeight) / 8;
        lpExtText->emStrikeOutWidth = lpFontDes->underline_height;
  
        /*  These items computed after reading characters.
        */
        lpExtText->emSuperScript = 0;
        lpExtText->emSubScript = 0;
        lpExtText->emDoubleUpperUnderlineOffset = 0;
        lpExtText->emDoubleLowerUnderlineOffset = 0;
        lpExtText->emDoubleUpperUnderlineWidth = 0;
        lpExtText->emDoubleLowerUnderlineWidth = 0;
    }
  
} // djfontdes()
  
// ljfontdes(): fontdes for the LaserJet class of printer.
  
LOCAL void ljfontdes(hMd, lpPFMhead, lpExtText, lpDrvInfo, lpFace,
facesz, lpFontDes)
HANDLE hMd;
LPPFMHEADER lpPFMhead;
LPEXTTEXTMETRIC lpExtText;
LPDRIVERINFO lpDrvInfo;
LPSTR lpFace;
int facesz;
LPLJFONTDES lpFontDes;
{
    char a[32];
    int k;
  
    if (lpDrvInfo)
    {
        lpDrvInfo->epSize = sizeof(DRIVERINFO);
        lpDrvInfo->epVersion = DRIVERINFO_VERSION;
        lpDrvInfo->epEscape = 0L;
        lpDrvInfo->xtbl.offset = 0L;
        lpDrvInfo->xtbl.len = 0;
        lpDrvInfo->xtbl.firstchar = 0;
        lpDrvInfo->xtbl.lastchar = 0;
  
        /*  Calculated later.
        */
        lpDrvInfo->epMemUsage = 0L;
        lpDrvInfo->xtbl.symbolSet = epsymUserDefined;
    }
  
    if (lpPFMhead)
    {
        lpPFMhead->dfVersion = 256;
        lpPFMhead->dfSize = 0L;
  
        lmemcpy(lpPFMhead->dfCopyright,
        lpFontDes->copyright, sizeof(lpPFMhead->dfCopyright));
        lpPFMhead->dfCopyright[sizeof(lpPFMhead->dfCopyright)-1] = '\0';
  
        lpPFMhead->dfType = 128;
        lpPFMhead->dfPoints = swab(lpFontDes->height) * 72 / 1200;
        lpPFMhead->dfVertRes = 300;
        lpPFMhead->dfHorizRes = 300;
        lpPFMhead->dfAscent = swab(lpFontDes->baseline);
        lpPFMhead->dfInternalLeading =
        swab(lpFontDes->cell_height) - ((swab(lpFontDes->height) + 2) / 4);
        lpPFMhead->dfExternalLeading =
        (swab(lpFontDes->height) / 4) - swab(lpFontDes->baseline);
        lpPFMhead->dfItalic = (BYTE)((lpFontDes->style == 1) ? 1 : 0);
        lpPFMhead->dfUnderline = 0;
        lpPFMhead->dfStrikeOut = 0;
        lpPFMhead->dfWeight = TranslateWeight(lpFontDes->stroke_weight);
        lpPFMhead->dfCharSet =
        TranslateCharSet(hMd, (LPFONTDES)lpFontDes, lpDrvInfo, a, sizeof(a));
        lpPFMhead->dfPixHeight = swab(lpFontDes->cell_height);
        lpPFMhead->dfPitchAndFamily =
        TranslateFamily(hMd, lpFontDes->typeface, lpFace, facesz);
  
        /*  This next group of variables are set after reading the
        *  characters -- note that the 'pitch' part of dfPitchAndFamily
        *  is set later.
        */
        lpPFMhead->dfPixWidth = 0;
        lpPFMhead->dfAvgWidth = 0;
        lpPFMhead->dfMaxWidth = 0;
        lpPFMhead->dfFirstChar = 0;
        lpPFMhead->dfLastChar = 0;
        lpPFMhead->dfDefaultChar = 0;
        lpPFMhead->dfBreakChar = 0;
  
        /*  dfDevice and dfFace are set when the file is written.
        */
        lpPFMhead->dfWidthBytes = 0L;
        lpPFMhead->dfDevice = 0L;
        lpPFMhead->dfFace = 0L;
        lpPFMhead->dfBitsPointer = 0L;
        lpPFMhead->dfBitsOffset = 0L;
        lpPFMhead->dfCharOffset[0] = 0;
  
        DBGx(("ljfontdes(): (1) lpFace=%ls\n", lpFace));
    }
    else if (lpFace)
    {
        TranslateCharSet(hMd, (LPFONTDES)lpFontDes, lpDrvInfo, a, sizeof(a));
        TranslateFamily(hMd, lpFontDes->typeface, lpFace, facesz);
        DBGx(("ljfontdes(): (2) lpFace=%ls\n", lpFace));
    }
  
    /*  Concatenate added string to face name.
    */
  
    //DBGx(("ljfontdes(): about to lstrlen\n", lpFace));
  
    if (lpFace && (k=lstrlen(lpFace)) && (k < facesz - lstrlen(a)))
    {
        //DBGx(("ljfontdes(): about to lstrcat\n", lpFace));
        lstrcat(lpFace, a);
    }
  
    if (lpExtText)
    {
        DBGx(("fontdes(): lpExtText=TRUE\n", lpFace));
        /*  Some of the formulas used herein are compliments of the
        *  FONT INTERCHANGE STANDARD, v1.0, available from Xerox.
        *  The rest, I got from super-driver memo, or I made them up.
        */
        lpExtText->emSize = 0L;
        lpExtText->emPointSize = swab(lpFontDes->cell_height) * 1440 / 300;
        lpExtText->emOrientation = (lpFontDes->orientation) ? 2 : 1;
        lpExtText->emMasterHeight = swab(lpFontDes->cell_height);
        lpExtText->emMinScale = lpExtText->emMasterHeight;
        lpExtText->emMaxScale = lpExtText->emMasterHeight;
        lpExtText->emMasterUnits = lpExtText->emMasterHeight;
        lpExtText->emCapHeight = swab(lpFontDes->baseline) -
        (swab(lpFontDes->cell_height) - ((swab(lpFontDes->height) + 2) / 4));
        lpExtText->emSlant = 0;
        lpExtText->emSuperScriptSize = swab(lpFontDes->cell_height);
        lpExtText->emSubScriptSize = swab(lpFontDes->cell_height);
        lpExtText->emKernPairs = 0;
        lpExtText->emKernTracks = 0;
  
        /*  These items filled in while reading characters.
        */
  
        lpExtText->emXHeight = 0;
        lpExtText->emLowerCaseAscent = 0;
        lpExtText->emLowerCaseDescent = 0;
        lpExtText->emUnderlineOffset = 0;
        lpExtText->emUnderlineWidth = 0;
        lpExtText->emStrikeOutOffset = 0;
        lpExtText->emStrikeOutWidth = 0;
  
        /*  These items computed after reading characters.
        */
        lpExtText->emSuperScript = 0;
        lpExtText->emSubScript = 0;
        lpExtText->emDoubleUpperUnderlineOffset = 0;
        lpExtText->emDoubleLowerUnderlineOffset = 0;
        lpExtText->emDoubleUpperUnderlineWidth = 0;
        lpExtText->emDoubleLowerUnderlineWidth = 0;
    }
  
    //DBGx(("ljfontdes(): exit\n", lpFace));
  
}   // ljfontdes()
  
/*  charinfo
*/
LOCAL BOOL charinfo(lpExtText, lpWidths, ch, lpCharDes, lpCHdata)
LPEXTTEXTMETRIC lpExtText;
WORD FAR *lpWidths;
BYTE ch;
LPCHARDES lpCharDes;
LPCHDATA lpCHdata;
{
    switch (gPrintClass)
    {
        case CLASS_LSRJETIII:
        case CLASS_PAINTJET:
        case CLASS_LASERJET:
            return(ljcharinfo(lpExtText, lpWidths, ch, lpCharDes,
            lpCHdata));
  
        case CLASS_DESKJET:
        case CLASS_DESKJET_PLUS:
            return(djcharinfo(lpExtText, lpWidths, ch, lpCharDes,
            lpCHdata));
    }
}
  
LOCAL BOOL djcharinfo(lpExtText, lpWidths, ch, lpCharDes, lpCHdata)
LPEXTTEXTMETRIC lpExtText;
WORD FAR *lpWidths;
BYTE ch;
LPCHARDES lpCharDes;
LPCHDATA lpCHdata;
{
    WORD theWidth = 0;
    WORD ind, last;
    BYTE pos;
    int c, firstchar, lastchar;
  
    if (lpCharDes->dj.norm_width == 255)
        theWidth = 0;
    else
        theWidth = (lpCharDes->dj.left_ps_pad +
        lpCharDes->dj.norm_width +
        lpCharDes->dj.right_ps_pad) / 2;
  
  
    if (++lpCHdata->count == 1)
        lpCHdata->prevWidth = theWidth;
  
    lpCHdata->totalWidth += theWidth;
  
    if (theWidth > lpCHdata->maxWidth)
        lpCHdata->maxWidth = theWidth;
  
    if (theWidth != lpCHdata->prevWidth)
        lpCHdata->pitch = 1;
  
    lpCHdata->prevWidth = theWidth;
  
    /*  If curChar < TRANS_MIN, then it translates directly into an
    *  ANSI character, so drop that width into the table.
    */
    if (ch < TRANS_MIN)
    {
        lpWidths[ch] = theWidth;
  
        if (ch < lpCHdata->firstchar)
            lpCHdata->firstchar = ch;
  
        if (ch > lpCHdata->lastchar)
            lpCHdata->lastchar = ch;
    }
  
  
    /*  Inverse translation:  use the same tables the laserjet driver uses
    *  to translate from Windows Ansi to USASCII or Roman 8
    *
    *  Traverse the translation table and drop the width of the character
    *  into any slot (character or overstrike character) where curChar
    *  exists.  In the case where both a normal and overstrike character
    *  exist (look at the table in trans.dat, the first column is normal
    *  characters, the second column is overstrike), then we drop in the
    *  width of the widest character.
    */
    last = (0xff - TRANS_MIN) * 2;
  
    /*  Lock down character translation table. (25sep89)
    */
    if (!lpCHdata->hTrans)
    {
        if (!lpCHdata->hMd)
        {
            DBGpfm(("charinfo(): no module handle, fail to get trans table\n"));
            return FALSE;
        }
  
        if (!(lpCHdata->hTrans=GetTransTable(lpCHdata->hMd,
            &lpCHdata->lpTrans, lpCHdata->symbolSet)))
        {
            DBGpfm(("charinfo(): *failed* to get trans table\n"));
            return FALSE;
        }
    }
  
    if (lpCHdata->symbolSet != epsymDESKJET8)
    {
        for (ind = 0; ind <= last; ++ind)
        {
//            if (ch == lpCHdata->lpTrans[ind])
            if (ch == (BYTE)lpCHdata->lpTrans[ind])                                      //RK(HP) 11/27/91
            {
                pos = (BYTE)(ind / 2 + TRANS_MIN);
  
                if (pos < lpCHdata->firstchar)
                    lpCHdata->firstchar = pos;
  
                if (pos > lpCHdata->lastchar)
                    lpCHdata->lastchar = pos;
#ifdef NODEF
                if (lpWidths)
                {
                    if (lpWidths[pos] == 0)
                        lpWidths[pos] = theWidth;
                    else if (!(ind % 2))
                        lpWidths[pos] = theWidth;
                }
#else
                if (lpWidths[pos] == 0)
                    lpWidths[pos] = theWidth;
                else if (theWidth > lpWidths[pos])
                    lpWidths[pos] = theWidth;
#endif
            }
        }
    }
    else
    {
        if (ch < 255)
        {
            dj[ch].left = lpCharDes->dj.left_ps_pad;
            dj[ch].width = lpCharDes->dj.norm_width;
            dj[ch].right = lpCharDes->dj.right_ps_pad;
            dj[ch].total = (lpCharDes->dj.left_ps_pad +
            lpCharDes->dj.norm_width +
            lpCharDes->dj.right_ps_pad) / 2;
        }
        else
        {
            dj[ch].left = lpCharDes->dj.left_ps_pad;
            dj[ch].width = lpCharDes->dj.norm_width;
            dj[ch].right = lpCharDes->dj.right_ps_pad;
            dj[ch].total = 0;
  
            lpCHdata->firstchar = 0;
            lpCHdata->lastchar = 255;
  
            for (ind = 0; ind <= 255; ++ind)
            {
                // 25sep89
                pos = (BYTE) (lpCHdata->lpTrans[ind*2]);
  
                lpWidths[ind] = ((dj[pos].width) >= (dj[pos+1].width)) ?
                dj[pos].total :
                dj[pos+1].total;
            }
        }
    }
    return( TRUE );
}
  
LOCAL BOOL ljcharinfo(lpExtText, lpWidths, ch, lpCharDes, lpCHdata)
LPEXTTEXTMETRIC lpExtText;
WORD FAR *lpWidths;
BYTE ch;
LPCHARDES lpCharDes;
LPCHDATA lpCHdata;
{
    WORD theWidth = 0;
    WORD ind, last;
    BYTE pos;
    int c;
  
  
    theWidth = swab(lpCharDes->lj.delta_x) / 4;
  
    if (++lpCHdata->count == 1)
        lpCHdata->prevWidth = theWidth;
  
    lpCHdata->totalWidth += theWidth;
  
    if (theWidth > lpCHdata->maxWidth)
        lpCHdata->maxWidth = theWidth;
  
    if (theWidth != lpCHdata->prevWidth)
        lpCHdata->pitch = 1;
  
    lpCHdata->prevWidth = theWidth;
  
    /*  If curChar < TRANS_MIN, then it translates directly into an
    *  ANSI character, so drop that width into the table.
    */
    if (ch < TRANS_MIN)
    {
        if (lpWidths)
            lpWidths[ch] = theWidth;
  
        if (ch < lpCHdata->firstchar)
            lpCHdata->firstchar = ch;
  
        if (ch > lpCHdata->lastchar)
            lpCHdata->lastchar = ch;
    }
  
    /*  Get all the detailed information that we need to fill in the
    *  extended text metrics struct.
    */
    if (lpExtText)
    {
        switch (ch)
        {
            case (BYTE)'x': /* x-height */
                if (lpCharDes->lj.orientation)
                    /* landscape */
                    lpExtText->emXHeight = swab(lpCharDes->lj.left_offset);
                else
                    /* portrait */
                    lpExtText->emXHeight = swab(lpCharDes->lj.top_offset);
                break;
  
            case (BYTE)'d': /* lower-case ascent */
                if (lpCharDes->lj.orientation)
                    lpExtText->emLowerCaseAscent =
                    swab(lpCharDes->lj.left_offset);
                else
                    lpExtText->emLowerCaseAscent =
                    swab(lpCharDes->lj.top_offset);
                break;
  
            case (BYTE)'p': /* lower-case descent */
                if (lpCharDes->lj.orientation)
                    lpExtText->emLowerCaseDescent =
                    swab(lpCharDes->lj.char_width) -
                    swab(lpCharDes->lj.left_offset);
                else
                    lpExtText->emLowerCaseDescent =
                    swab(lpCharDes->lj.char_height) -
                    swab(lpCharDes->lj.top_offset);
                break;
  
            case (BYTE)'_': /* underline */
                if (lpCharDes->lj.orientation)
                {
                    lpExtText->emUnderlineWidth =
                    swab(lpCharDes->lj.char_width);
                    lpExtText->emUnderlineOffset =
                    swab(lpCharDes->lj.left_offset);
                }
                else
                {
                    lpExtText->emUnderlineWidth =
                    swab(lpCharDes->lj.char_height);
                    lpExtText->emUnderlineOffset =
                    swab(lpCharDes->lj.top_offset);
                }
                lpExtText->emDoubleUpperUnderlineOffset =
                lpExtText->emUnderlineOffset +
                lpExtText->emUnderlineWidth * 2;
                lpExtText->emDoubleLowerUnderlineOffset =
                lpExtText->emUnderlineOffset;
                lpExtText->emDoubleUpperUnderlineWidth =
                lpExtText->emUnderlineWidth;
                lpExtText->emDoubleLowerUnderlineWidth =
                lpExtText->emDoubleUpperUnderlineWidth;
                break;
  
            case (BYTE)'-': /* strike-through */
                if (lpCharDes->lj.orientation)
                {
                    lpExtText->emStrikeOutWidth =
                    swab(lpCharDes->lj.char_width);
                    lpExtText->emStrikeOutOffset =
                    swab(lpCharDes->lj.left_offset) -
                    lpExtText->emStrikeOutWidth;
                }
                else
                {
                    lpExtText->emStrikeOutWidth =
                    swab(lpCharDes->lj.char_height);
                    lpExtText->emStrikeOutOffset =
                    swab(lpCharDes->lj.top_offset) -
                    lpExtText->emStrikeOutWidth;
                }
                break;
        }
    }
  
    /*  Inverse translation:  use the same tables the laserjet driver uses
    *  to translate from Windows Ansi to USASCII or Roman 8
    *
    *  Traverse the translation table and drop the width of the character
    *  into any slot (character or overstrike character) where curChar
    *  exists.  In the case where both a normal and overstrike character
    *  exist (look at the table in trans.dat, the first column is normal
    *  characters, the second column is overstrike), then we drop in the
    *  width of the widest character.
    */
    last = (0xff - TRANS_MIN) * 2;
  
    /*  Lock down character translation table.
    */
    if (!lpCHdata->hTrans)
    {
        if (!lpCHdata->hMd)
        {
            DBGpfm(("charinfo(): no module handle, fail to get trans table\n"));
            return FALSE;
        }
  
        if (!(lpCHdata->hTrans=GetTransTable(lpCHdata->hMd,
            &lpCHdata->lpTrans, lpCHdata->symbolSet)))
        {
            DBGpfm(("charinfo(): *failed* to get trans table\n"));
            return FALSE;
        }
    }
  
    for (ind = 0; ind <= last; ++ind)
    {
//        if (ch == lpCHdata->lpTrans[ind])
        if (ch == (BYTE)lpCHdata->lpTrans[ind])                                          //RK(HP) 10/11/91
        {
            pos = (BYTE)(ind / 2 + TRANS_MIN);
  
            if (pos < lpCHdata->firstchar)
                lpCHdata->firstchar = pos;
  
            if (pos > lpCHdata->lastchar)
                lpCHdata->lastchar = pos;
  
            if (lpWidths)
            {
                if (lpWidths[pos] == 0)
                    lpWidths[pos] = theWidth;
                else if (!(ind % 2))
                    lpWidths[pos] = theWidth;
            }
        }
    }
  
    return TRUE;
}
  
/*  TranslateWeight
*
*  Translate HP weight value to Windows weight value.
*/
LOCAL WORD TranslateWeight(weight)
int weight;
{
    WORD dfWeight = 0;
  
    switch (weight)
    {
        case -7: dfWeight = FW_THIN;        break;
        case -6: dfWeight = FW_THIN;        break;
        case -5: dfWeight = FW_EXTRALIGHT;  break;
        case -4: dfWeight = FW_EXTRALIGHT;  break;
        case -3: dfWeight = FW_LIGHT;       break;
        case -2: dfWeight = FW_LIGHT;       break;
        case -1: dfWeight = FW_NORMAL;      break;
        case  0: dfWeight = FW_NORMAL;      break;
        case  1: dfWeight = FW_MEDIUM;      break;
        case  2: dfWeight = FW_SEMIBOLD;    break;
        case  3: dfWeight = FW_BOLD;        break;
        case  4: dfWeight = FW_BOLD;        break;
        case  5: dfWeight = FW_EXTRABOLD;   break;
        case  6: dfWeight = FW_EXTRABOLD;   break;
        case  7: dfWeight = FW_HEAVY;       break;
        default: dfWeight = FW_DONTCARE;    break;
    }
  
    return (dfWeight);
}
  
/*  TranslateCharSet
*/
LOCAL BYTE TranslateCharSet(hMd, lpFontDes, lpDrvInfo, lpAdd, addsz)
HANDLE hMd;
LPFONTDES lpFontDes;
LPDRIVERINFO lpDrvInfo;
LPSTR lpAdd;
int addsz;
{
    SYMBOLSET symbolSet;
    BYTE dfCharSet;
  
    *lpAdd = '\0';          /* Just in case LoadString fails */
  
    switch (swab(((LPGENERIC_FONTDES)lpFontDes)->symbol_set))
    {
        case (8*32)+'U'-64: /* Roman-8 */
            dfCharSet = ANSI_CHARSET;
            symbolSet = epsymRoman8;
            *lpAdd = '\0';
            break;
  
        case (0*32)+'U'-64: /* USASCII */
            dfCharSet = ANSI_CHARSET;
            symbolSet = epsymUSASCII;
            *lpAdd = '\0';
            break;
  
        case (11*32)+'Q'-64:        /* ECMA-94 */
        case (0*32)+'N'-64: /* ECMA-94 */
            dfCharSet = ANSI_CHARSET;
            symbolSet = epsymECMA94;
            *lpAdd = '\0';
            break;
  
        case (8*32)+'M'-64: /* Math-8 */
            dfCharSet = MATH8_CHARSET;
            symbolSet = epsymGENERIC8;
            LoadString(hMd, SF_CHSET_MATH8, lpAdd, addsz);
            break;
  
        case (15*32)+'U'-64:    /* PI Font */
            dfCharSet = PIFONT_CHARSET;
            symbolSet = epsymGENERIC8;
            LoadString(hMd, SF_CHSET_PIFONT, lpAdd, addsz);
            break;
  
        case (0*32)+'B'-64: /* LineDraw */
            dfCharSet = LINEDRAW_CHARSET;
            symbolSet = epsymGENERIC7;
            LoadString(hMd, SF_CHSET_LINEDRAW, lpAdd, addsz);
            break;
  
        case (4*32)+'Q'-64: /* PC Line */
            dfCharSet = PCLINE_CHARSET;
            symbolSet = epsymGENERIC7;
            LoadString(hMd, SF_CHSET_PCLINE, lpAdd, addsz);
            break;
  
        case (1*32)+'U'-64: /* US Legal */
            dfCharSet = USLEGAL_CHARSET;
            symbolSet = epsymGENERIC7;
            LoadString(hMd, SF_CHSET_USLEGAL, lpAdd, addsz);
            break;
  
        case 529:   /* DeskJet8 */
            dfCharSet = ECMA94_CHARSET;
            symbolSet = epsymDESKJET8;
            *lpAdd = '\0';
            break;
  
        default:
            dfCharSet = ANSI_CHARSET;
            if (((LPGENERIC_FONTDES)lpFontDes)->font_type)
                symbolSet = epsymGENERIC8;
            else
                symbolSet = epsymGENERIC7;
            *lpAdd = '\0';
            break;
    }
    lpAdd[addsz-1] = '\0';
  
    if (lpDrvInfo)
        lpDrvInfo->xtbl.symbolSet = symbolSet;
  
    return (dfCharSet);
}
  
/*  TranslateFamily
*/
LOCAL BYTE TranslateFamily(hMd, typeface, lpFace, facesz)
HANDLE hMd;
BYTE typeface;
LPSTR lpFace;
int facesz;
{
    register BYTE dfPitchAndFamily = 0;
  
    *lpFace = '\0';
  
    DBMSG(("TranslateFamily(): typeface=%d", (int)typeface));
  
    /*  Load the face name string from the resources.  The format
    *  of the string is:
    *
    *      <family-code>,<face-Name>
    *
    *  Pick up the family code and then shift the face name to
    *  the left over top of the family code.
    */
    if (LoadString(hMd, SF_FACE_OFFSET+typeface, lpFace, facesz))
    {
        DBMSG((", string=%ls\n", lpFace));
  
        switch (lpFace[0])
        {
            case 'm':
            case 'M':
                dfPitchAndFamily |= FF_MODERN;
                break;
  
            case 'w':
            case 'W':
                dfPitchAndFamily |= FF_SWISS;
                break;
  
            case 'r':
            case 'R':
                dfPitchAndFamily |= FF_ROMAN;
                break;
  
            case 's':
            case 'S':
                dfPitchAndFamily |= FF_SCRIPT;
                break;
  
            case 'd':
            case 'D':
                dfPitchAndFamily |= FF_DECORATIVE;
                break;
  
            case '0':
            default:
                dfPitchAndFamily |= FF_DONTCARE;
                break;
        }
  
        if (lstrlen(lpFace) > 2)
            lstrcpy(lpFace, &lpFace[2]);
        else
            *lpFace = '\0';
    }
    else
    {
        DBMSG((" (failed to get face name -- using default)\n"));
  
        dfPitchAndFamily |= FF_DONTCARE;
        *lpFace = '\0';
    }
  
    return (dfPitchAndFamily);
}
  
#ifdef DUMP_PFM
/*  dumpCHdata
*
*  Dump character data.
*/
void dumpCHdata(lpCHdata)
LPCHDATA lpCHdata;
{
    DBMSG(("hTrans=%d\n", lpCHdata->hTrans));
    DBMSG(("lpTrans=%lp\n", lpCHdata->lpTrans));
    DBMSG(("hMd=%d\n", lpCHdata->hMd));
    DBMSG(("symbolSet=%d\n", (WORD)lpCHdata->symbolSet));
    DBMSG(("count=%ld\n", lpCHdata->count));
    DBMSG(("prevWidth=%d\n", lpCHdata->prevWidth));
    DBMSG(("totalWidth=%d\n", lpCHdata->totalWidth));
    DBMSG(("maxWidth=%d\n", lpCHdata->maxWidth));
    DBMSG(("pitch=%d\n", (WORD)lpCHdata->pitch));
    DBMSG(("firstchar=%d\n", (WORD)lpCHdata->firstchar));
    DBMSG(("lastchar=%d\n", (WORD)lpCHdata->lastchar));
}
  
/*  dumpPFM
*
*  Display contents of .pfm file.
*/
void dumpPFM(lpPFMhead, lpPFMext, lpExtText, lpDrvInfo)
LPPFMHEADER lpPFMhead;
LPPFMEXTENSION lpPFMext;
LPEXTTEXTMETRIC lpExtText;
LPDRIVERINFO lpDrvInfo;
{
    DBMSG(("memUsage=%ld\n", lpDrvInfo->epMemUsage));
  
    DBMSG(("dfVersion = %d\n", lpPFMhead->dfVersion));
    DBMSG(("dfSize = %ld\n", lpPFMhead->dfSize));
    DBMSG(("dfCopyright = %ls\n", (LPSTR)lpPFMhead->dfCopyright));
    DBMSG(("dfType = %d\n", lpPFMhead->dfType));
    DBMSG(("dfPoints = %d\n", lpPFMhead->dfPoints));
    DBMSG(("dfVertRes = %d\n", lpPFMhead->dfVertRes));
    DBMSG(("dfHorizRes = %d\n", lpPFMhead->dfHorizRes));
    DBMSG(("dfAscent = %d\n", lpPFMhead->dfAscent));
    DBMSG(("dfInternalLeading = %d\n", lpPFMhead->dfInternalLeading));
    DBMSG(("dfExternalLeading = %d\n", lpPFMhead->dfExternalLeading));
    DBMSG(("dfItalic = %d\n", (WORD)lpPFMhead->dfItalic));
    DBMSG(("dfUnderline = %d\n", (WORD)lpPFMhead->dfUnderline));
    DBMSG(("dfStrikeOut = %d\n", (WORD)lpPFMhead->dfStrikeOut));
    DBMSG(("dfWeight = %d\n", lpPFMhead->dfWeight));
    DBMSG(("dfCharSet = %d\n", (WORD)lpPFMhead->dfCharSet));
    DBMSG(("dfPixWidth = %d\n", lpPFMhead->dfPixWidth));
    DBMSG(("dfPixHeight = %d\n", lpPFMhead->dfPixHeight));
    DBMSG(("dfPitchAndFamily = %d\n", (WORD)lpPFMhead->dfPitchAndFamily));
    DBMSG(("dfAvgWidth = %d\n", lpPFMhead->dfAvgWidth));
    DBMSG(("dfMaxWidth = %d\n", lpPFMhead->dfMaxWidth));
    DBMSG(("dfFirstChar = %d\n", (WORD)lpPFMhead->dfFirstChar));
    DBMSG(("dfLastChar = %d\n", (WORD)lpPFMhead->dfLastChar));
    DBMSG(("dfDefaultChar = %d\n", (WORD)lpPFMhead->dfDefaultChar));
    DBMSG(("dfBreakChar = %d\n", (WORD)lpPFMhead->dfBreakChar));
    DBMSG(("dfWidthBytes = %d\n", lpPFMhead->dfWidthBytes));
    DBMSG(("dfDevice = %ld\n", lpPFMhead->dfDevice));
    DBMSG(("dfFace = %ld\n", lpPFMhead->dfFace));
    DBMSG(("dfBitsPointer = %ld\n", lpPFMhead->dfBitsPointer));
    DBMSG(("dfBitsOffset = %ld\n", lpPFMhead->dfBitsOffset));
    DBMSG(("dfCharOffset[0] = %d\n", lpPFMhead->dfCharOffset[0]));
  
    DBMSG(("dfSizeFields = %d\n", lpPFMext->dfSizeFields));
    DBMSG(("dfExtMetricsOffset = %ld\n", lpPFMext->dfExtMetricsOffset));
    DBMSG(("dfExtentTable = %ld\n", lpPFMext->dfExtentTable));
    DBMSG(("dfOriginTable = %ld\n", lpPFMext->dfOriginTable));
    DBMSG(("dfPairKernTable = %ld\n", lpPFMext->dfPairKernTable));
    DBMSG(("dfTrackKernTable = %ld\n", lpPFMext->dfTrackKernTable));
    DBMSG(("dfDriverInfo = %ld\n", lpPFMext->dfDriverInfo));
    DBMSG(("dfReserved = %ld\n", lpPFMext->dfReserved));
  
    DBMSG(("emSize = %d\n", lpExtText->emSize));
    DBMSG(("emPointSize = %d\n", lpExtText->emPointSize));
    DBMSG(("emOrientation = %d\n", lpExtText->emOrientation));
    DBMSG(("emMasterHeight = %d\n", lpExtText->emMasterHeight));
    DBMSG(("emMinScale = %d\n", lpExtText->emMinScale));
    DBMSG(("emMaxScale = %d\n", lpExtText->emMaxScale));
    DBMSG(("emMasterUnits = %d\n", lpExtText->emMasterUnits));
    DBMSG(("emCapHeight = %d\n", lpExtText->emCapHeight));
    DBMSG(("emXHeight = %d\n", lpExtText->emXHeight));
    DBMSG(("emLowerCaseAscent = %d\n", lpExtText->emLowerCaseAscent));
    DBMSG(("emLowerCaseDescent = %d\n", lpExtText->emLowerCaseDescent));
    DBMSG(("emSlant = %d\n", lpExtText->emSlant));
    DBMSG(("emSuperScript = %d\n", lpExtText->emSuperScript));
    DBMSG(("emSubScript = %d\n", lpExtText->emSubScript));
    DBMSG(("emSuperScriptSize = %d\n", lpExtText->emSuperScriptSize));
    DBMSG(("emSubScriptSize = %d\n", lpExtText->emSubScriptSize));
    DBMSG(("emUnderlineOffset = %d\n", lpExtText->emUnderlineOffset));
    DBMSG(("emUnderlineWidth = %d\n", lpExtText->emUnderlineWidth));
    DBMSG(( "emDoubleUpperUnderlineOffset = %d\n",
    lpExtText->emDoubleUpperUnderlineOffset));
    DBMSG(("emDoubleLowerUnderlineOffset = %d\n",
    lpExtText->emDoubleLowerUnderlineOffset));
    DBMSG(("emDoubleUpperUnderlineWidth = %d\n",
    lpExtText->emDoubleUpperUnderlineWidth));
    DBMSG(("emDoubleLowerUnderlineWidth = %d\n",
    lpExtText->emDoubleLowerUnderlineWidth));
    DBMSG(("emStrikeOutOffset = %d\n", lpExtText->emStrikeOutOffset));
    DBMSG(("emStrikeOutWidth = %d\n", lpExtText->emStrikeOutWidth));
    DBMSG(("emKernPairs = %d\n", lpExtText->emKernPairs));
    DBMSG(("emKernTracks = %d\n", lpExtText->emKernTracks));
  
    DBMSG(("epSize = %d\n", lpDrvInfo->epSize));
    DBMSG(("epVersion = %d\n", lpDrvInfo->epVersion));
    DBMSG(("epMemUsage = %ld\n", lpDrvInfo->epMemUsage));
    DBMSG(("epEscape = %ld\n", lpDrvInfo->epEscape));
    DBMSG(("xtbl.symbolSet = %d\n", lpDrvInfo->xtbl.symbolSet));
    DBMSG(("xtbl.offset = %ld\n", lpDrvInfo->xtbl.offset));
    DBMSG(("xtbl.len = %d\n", lpDrvInfo->xtbl.len));
    DBMSG(("xtbl.firstchar = %d\n", (WORD)lpDrvInfo->xtbl.firstchar));
    DBMSG(("xtbl.lastchar = %d\n", (WORD)lpDrvInfo->xtbl.lastchar));
}
  
/*  dumpWidths
*
*  Dump width table.
*/
LOCAL void dumpWidths(lpWidths)
WORD FAR *lpWidths;
{
    BYTE c;
    WORD n;
    int ind;
  
    for (ind=0; ind < 255; ++ind)
    {
        if ((ind % 10) == 0)
        {
            DBMSG(("\n%c%c%d-%c%c%d: ",
            ((ind > 99) ? '\0' : '0'),
            ((ind > 9) ? '\0' : '0'),
            ind,
            (((ind+9) > 99) ? '\0' : '0'),
            (((ind+9) > 9) ? '\0' : '0'),
            (ind+9) ));
        }
  
        n = (WORD)lpWidths[ind];
        c = (ind < 32 || ind > 127) ? (BYTE)'.' : (BYTE)ind;
  
        DBMSG((" %c%c%d/%c",
        ((n > 99) ? '\0' : ' '),
        ((n > 9) ? '\0' : ' '),
        n, c));
    }
  
    DBMSG(("\n\n"));
}
  
#endif
