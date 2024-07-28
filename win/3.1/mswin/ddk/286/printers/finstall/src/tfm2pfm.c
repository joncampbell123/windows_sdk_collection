/**[f******************************************************************
* tfm2pfm.c -
*
* Copyright (C) 1989,1990,1991 Hewlett-Packard Company.
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

/*
 * 31 Jul 90   rk(HP) Changed wt_char_metrics for loop and wt_pfm to use
 *                    lastchar for default symbol set. In wt_pfm, added ssindex which
 *                    is the symbol set index, which is used to determine the 
 *                    default lastchar. Added symbolLength to record number of
 *                    symbols in the symbol set. Changed default lastchar to 
 *                    FRSTCHAR + symbolLength -1. If lastchar is greater than
 *                    (FRSTCHAR + symbolLength -1), then set it to that value. 
 *                     Changed wr_header_struct so that character width table
 *                     has (lastchar-FRSTCHAR +2) CHARWDTHs (to include the sentinel)
 *                     less the last word of PFMHEADER, which is part of the chr
 *                     Modified wt_char_metrics to calculate length as (lastchar-FRSTCHAR+1).
 *                     Gloabl alloc length + 1 and loop o to less than length.
 *                     -lwrite only length+1
 */

/*************************** tfm2pfm.c **************************************
*
*   Summary:  This program will read the GLUE.TXT file to get Windows font
*             info, call the TFM reader, convert the appropriate data and
*             write a PFM or PCM file.
*
*  History:
*
*            2-Feb-90  dtk  Original.
*
****************************************************************************/
  
  
//#define DEBUG
  
#include <ctype.h>
  
/* local includes */
#include "printer.h"
#include "windows2.h"
#include "tfmread.h"
#include "tfmdefs.h"
//#include "tfmstruc.h"
#include "glue.h"
#include "neededh.h"
  
#define LOCAL static
  
/****************************************************************************\
*                     Debug Definitions
\****************************************************************************/
  
/*  Local debug stuff.
*/
#ifdef DEBUG
    #define LOCAL_DEBUG
#endif
  
#ifdef LOCAL_DEBUG
    #define DBGerr(msg) /* DBMSG(msg) */
    #define DBGwinsf(msg)   /* DBMSG(msg) */
    #define DBGt2p(msg) /* DBMSG(msg) */
    #define DBGentry(msg)   /* DBMSG(msg) */
#else
    #define DBGerr(msg) /*null*/
    #define DBGwinsf(msg)   /*null*/
    #define DBGt2p(msg) /*null*/
    #define DBGentry(msg)   /*null*/
#endif
  
typedef int far *LPINT;
  
typedef struct
{
    WORD width;
    /*      WORD offset; */
  
} CHARWDTH;
typedef CHARWDTH far * LPCHARWDTH;
  
/* function definitions */
extern BOOL FAR PASCAL    MoveFamilyName(LPSTR, LPSTR, WORD);
  
void FAR PASCAL wt_pfm(HANDLE, int, DWORD, LPSTR, LPSTR, BYTE, BOOL);
  
void LOCAL wt_driver_struc(struct TFMType far *, int, long, DWORD, int);
  
#ifdef KERN
void LOCAL wt_kernpr_tbl(struct TFMType far *, int);
#endif
  
void LOCAL wt_real_pfmext(int, long, long, BYTE, WORD);
long LOCAL wt_font_name(int, LPSTR, int, BYTE);
void LOCAL wt_escape_str(struct TFMType far *, int, LPSTR, LPSTR, BOOL);
void LOCAL wt_driver_name(int);
void LOCAL wt_header_struc(HANDLE, int, BYTE, LPSTR);
BYTE LOCAL get_face(int, int);
void LOCAL wt_char_metrics(struct TFMType far *, int, BYTE, LPSTR);
void LOCAL wt_extext_struc(struct TFMType far *, int, short, BYTE);
WORD LOCAL get_weight(int);
//void LOCAL get_width_name(BYTE, LPSTR);
void LOCAL mk_pitch(struct TFMType far *, LPSTR, WORD);
/* void wt_trackrn_tbl(struct TFMType far *, int); */
  
int gPtSize;            /* Point size for current PFM */
  
  
/****************************************************************************
  
Routine Title: wt_pfm
  
control module to print the first part of the PFM file
  
  
****************************************************************************/
  
void FAR PASCAL wt_pfm(HANDLE hT, int outfile, DWORD MemUsage,
LPSTR lpPtSizes, LPSTR lpSS, BYTE Orient, BOOL PCLV)
  
  
{
    struct TFMType far *TFM;
    long pfm_start, file_size, fontnm_pos;
    LPSYMINFO lpSym;
    int i, ssnum;
    int pos;
    int ssval;           /* Symbol set value convert to a #    */
    BYTE lastchar;       /* Last character in symbol set       */
    BOOL decimal=FALSE;      /* TRUE when we hit a decimal point   */
    BOOL ssfound;
    short twip=0;        /* 20ths of a pt for ext text metrics */
    LPPFMEXTENSION pfmext;   /* Used to write dummy PFM */
    HANDLE hP;           /* Used to write dummy PFM */
    int ssindex=0;       /* index to symbol set- used to determine default lastchar  */  //rk 7/31/91
    int symbolLength;
  
    DBGentry(("wt_pfm: enter\n"));
  
    /* We might be in a PCM, so record file position of PFM */
    pfm_start = _llseek(outfile, 0L, 1);
  
    DBGt2p(("wt_pfm: pfm_start = %d\n", pfm_start));
  
    /* Get point size */
    gPtSize = 0;
    pos = 0;
    if (lpPtSizes)
    {
        while (lpPtSizes[pos] && (!isspace(lpPtSizes[pos])))
        {
            if (lpPtSizes[pos] == '.')  /* It's a fraction from here on out */
                decimal = TRUE;
            else if (decimal)       /* Round the pt size and get twips  */
            {
                twip = (lpPtSizes[pos++] - '0') * 2; /* Accuracy: 1/10th pt */
  
                if (twip >= 10)         /* Round point size */
                    gPtSize++;
  
                /* Make twip accurate to 1/20th point */
                if (lpPtSizes[pos] && (!isspace(lpPtSizes[pos])))
                    if ((lpPtSizes[pos] - '0') >= 5)
                        twip++;
  
                break;
            }
            else
                gPtSize = gPtSize * 10 + lpPtSizes[pos] - '0';
  
            pos++;
        }
    }
  
    if ((hT) && (TFM = (struct TFMType far *)GlobalLock(hT)))
    {
        ssnum = TFM->typeface->general.numberSymbolSets;
  
        for(i=0 ; i<ssnum; i++)
        {
            /* Check for correct symset or last symset, if correct isn't found */
            if (ssfound =
                (lstrcmp(TFM->typeface->symbol.symbolSetDirectory[i].selectionName,
                lpSS) == 0))
                {
                    ssindex = i;                 
                    break;
                }
        }
  
        if (!ssfound)       /* No Windows ANSI; try Zapf Dingbats */
        {
            lstrcpy(lpSS, (LPSTR)"10L");
  
            for(i=0 ; i<ssnum; i++)
            {
                /* Check for correct symset or last symset */
                if (ssfound =
                    (lstrcmp((LPSTR)TFM->typeface->symbol.symbolSetDirectory[i].selectionName,
                    lpSS) == 0))
                    {
                       ssindex = i;                 
                       break;
                    }
            }
        }
  
        if (!ssfound)       /* Just take the 1st symset */
            {
            lstrcpy(lpSS, (LPSTR)TFM->typeface->symbol.symbolSetDirectory[0].selectionName);
            ssindex = 0;
            }

        /* Determine symbolLength */
        symbolLength = TFM->typeface->symbol.symbolSetDirectory[ssindex].symbolLength;

        GlobalUnlock(hT);
    }
    else
        return;
  
    /* Get symbol set related info */
    ssval = lpSS[0] - '0';
    if (lstrlen(lpSS) == 3)      /* Two digit number */
    {
        ssval = (ssval * 10 + lpSS[1] - '0') * 32 + lpSS[2] - 64;
    }
    else
        ssval = ssval * 32 + lpSS[1] - 64;
  
    switch (ssval)
    {
        case DT:
        case M8:
        case MS:
        case PC:
        case PD:
        case PM:
        case R8:
            lastchar = 254;
            break;
        case LG:
        case PI:
        case US:
            lastchar = 127;
            break;
        case PB:
            lastchar = 248;
            break;
        case TS:
            lastchar = 251;
            break;
        case D1:
        case D2:
        case D3:
        case DS:
        case DV:
        case E1:
        case VI:
        case VM:
        case VU:
        case WN:
//        default:
//            lastchar = 255;
            lastchar = 255;
        default:
            lastchar = (FRSTCHAR + symbolLength -1);                                     // rk 07/31/91
    }
    /* set lastchar to be lessor of lastchar and length */
            if (lastchar > (FRSTCHAR + symbolLength -1))                                 // rk 07/31/91
                  lastchar = (FRSTCHAR + symbolLength -1);                                                // rk 07/31/91


    wt_header_struc(hT, outfile, lastchar, lpSS);
    DBGt2p(("wt_pfm: wrote header struct\n"));
  
    /* allocate zero-inited memory for PFMEXTENSION pointer */
    if ((hP = GlobalAlloc(GHND, (DWORD)sizeof(PFMEXTENSION))) &&
        (pfmext = (LPPFMEXTENSION)GlobalLock(hP)))
    {
        _lwrite(outfile, (LPSTR)pfmext, sizeof(PFMEXTENSION));
  
        GlobalUnlock(hP);
        GlobalFree(hP);
    }
    DBGt2p(("wt_pfm: wrote dummy pfm extension\n"));
  
    wt_driver_name(outfile);
    DBGt2p(("wt_pfm: wrote driver name\n"));
  
    if (TFM = (struct TFMType far *)GlobalLock(hT))
    {
        fontnm_pos = wt_font_name(outfile, (LPSTR)TFM->typeface->general.typeface,
        ssval, TFM->typeface->typefaceMetrics.appearanceWidth);
        DBGt2p(("wt_pfm: wrote font name\n"));
  
        wt_real_pfmext(outfile, pfm_start, fontnm_pos, lastchar,
        TFM->typeface->typefaceMetrics.spacing);
        DBGt2p(("wt_pfm: wrote pfm extension\n"));
  
        wt_extext_struc(TFM, outfile, twip, Orient);
        DBGt2p(("wt_pfm: wrote extended text metrics\n"));
  
        wt_driver_struc(TFM, outfile, pfm_start, MemUsage, ssval);
        DBGt2p(("wt_pfm: wrote driver info\n"));
  
#ifdef KERN
        /* No kern table for fixed pitch */
        if (!(TFM->typeface->typefaceMetrics.spacing))
        {
            wt_kernpr_tbl(TFM, outfile);
            DBGt2p(("wt_pfm: wrote kern table\n"));
        }
#endif
  
        /*  wt_trackrn_tbl(TFM, outfile); Nobody uses track kerning... */
  
        wt_escape_str(TFM, outfile, lpPtSizes, lpSS, PCLV);
        DBGt2p(("wt_pfm: wrote driver info\n"));
  
        GlobalUnlock(hT);
    }
  
    /* Fill in correct value for pfmh->dfSize */
    file_size = _llseek(outfile, 0L, 1) - pfm_start;
    DBGt2p(("wt_pfm: file_size = %d\n", file_size));
  
    _llseek(outfile, pfm_start + 2L, 0);
    _lwrite(outfile, (LPSTR)&file_size, sizeof(long));
    _llseek(outfile, 0L, 2);
  
    DBGentry(("wt_pfm: exit\n"));
}
  
  
  
  
/****************************************************************************
  
Routine Title: wt_header_struc
  
****************************************************************************/
  
void LOCAL wt_header_struc(HANDLE hT, int outfile, BYTE lastchar, LPSTR lpSS)
  
{
    int         i,j, pandf;
    short       CharTable;
    LPPFMHEADER pfmh;
    HANDLE   hP = 0;
    struct TFMType far *TFM;
  
    DBGentry(("wt_header_struc: enter\n"));
  
    /* allocate memory for PFMHEADER pointer */
    if (!(TFM = (struct TFMType far *)GlobalLock(hT)))
        return;
  
    if ((hP = GlobalAlloc(GMEM_MOVEABLE, (DWORD)sizeof(PFMHEADER))) &&
        (pfmh = (LPPFMHEADER)GlobalLock(hP)))
    {
        DBGt2p(("wt_header_struc: alloced memory\n"));
  
        pfmh->dfVersion = 0x100;
  
        DBGt2p(("wt_header_struc: pfmh->dfVersion = %d\n", pfmh->dfVersion));
  
        /* write dummy file size
        *  change later when size is known
        */
        pfmh->dfSize = 0L;
  
        DBGt2p(("wt_header_struc: TFM copyright: %ls\n",
        (LPSTR)TFM->typeface->general.copyright));
  
        lstrcpyn(pfmh->dfCopyright, (LPSTR)TFM->typeface->general.copyright, 60);
        // pfmh->dfCopyright[60] = '\0';
  
        DBGt2p(("wt_header_struc: pfmh->dfCopyright: %ls\n", pfmh->dfCopyright));
  
        pfmh->dfType = PFMTYPE;
  
        /* Should we adjust value if points per inch != 72 ? */
        pfmh->dfPoints = (gPtSize) ? gPtSize : (WORD)ldiv(
        TFM->typeface->typefaceMetrics.nominalPointSizeN,
        TFM->typeface->typefaceMetrics.nominalPointSizeD);
  
        DBGt2p(("wt_header_struc: pfmh->dfPoints = %d\n", pfmh->dfPoints));
  
        pfmh->dfVertRes = DEV_RES;
  
        pfmh->dfHorizRes = DEV_RES;
  
        // pfmh->dfAscent = BASELINE * (PTPIN/HPPTPIN);
        pfmh->dfAscent = fdiv5(
        (DWORD)TFM->typeface->typefaceMetrics.capheight,
        TFM->typeface->typefaceMetrics.pointD,
        1L,
        TFM->typeface->typefaceMetrics.pointN,
        (DWORD)HPPTPIN);
        if (gPtSize)
            pfmh->dfAscent = duconvert(TFM, gPtSize, pfmh->dfAscent);
  
        /* can't get this field from tfm file
        * sdk fnt spec. says its ok to zero out
        */
        pfmh->dfInternalLeading = 0;
  
        pfmh->dfExternalLeading =
        TFM->typeface->typefaceMetrics.recommendedLineSpacing / 6;
        if (gPtSize)
            pfmh->dfExternalLeading = duconvert(TFM, gPtSize,
            pfmh->dfExternalLeading);
  
        /* read the slant from the TFM structure
        * and check to see if non 0 -- italic
        */
  
        /* slant = duconvert(TFM, gPtSize,TFM->typeface->typefaceMetrics.slant); */
  
        /* pfmh->dfItalic = (slant) ? 0 : 1; */
        pfmh->dfItalic = (TFM->typeface->typefaceMetrics.slant) ? (BYTE)1 : (BYTE)0;
  
        pfmh->dfUnderline = 0;
  
        pfmh->dfStrikeOut = 0;
  
        pfmh->dfWeight = get_weight(TFM->typeface->typefaceMetrics.strokeWeight);
  
        pfmh->dfCharSet = 0; /* = get_charset(class); Use WN for now */
  
        pfmh->dfPixWidth = TFM->typeface->typefaceMetrics.spacing;
        if (gPtSize)
            pfmh->dfPixWidth = duconvert(TFM, gPtSize, pfmh->dfPixWidth);
  
//      pfmh->dfPixHeight = ((TFM->typeface->typefaceMetrics.nominalPointSize *
//      0x12C) / 72);
  
        pfmh->dfPixHeight = (gPtSize) ? ((gPtSize * 300) / 72) : 0;
  
        pandf = TFM->typeface->typefaceMetrics.serifStyle;
  
        pfmh->dfPitchAndFamily = get_face(pandf, TFM->typeface->typefaceMetrics.spacing);
  
        pfmh->dfAvgWidth = (WORD)ldiv(TFM->typeface->typefaceMetrics.averageWidthN,
        TFM->typeface->typefaceMetrics.averageWidthD);
        if (gPtSize)
            pfmh->dfAvgWidth = duconvert(TFM, gPtSize, pfmh->dfAvgWidth);
  
        DBGt2p(("wt_header_struc: pfmh->dfAvgWidth = %d\n", pfmh->dfAvgWidth));
  
        pfmh->dfMaxWidth = TFM->typeface->typefaceMetrics.maximumWidth;
        if (gPtSize)
            pfmh->dfMaxWidth = duconvert(TFM, gPtSize, pfmh->dfMaxWidth);
  
        pfmh->dfFirstChar = FRSTCHAR;
  
        pfmh->dfLastChar = lastchar;
  
        pfmh->dfDefaultChar = (BYTE)(BREAKCHAR - pfmh->dfFirstChar);
  
        pfmh->dfBreakChar = (BYTE)(32 - pfmh->dfFirstChar);
  
        pfmh->dfWidthBytes = 0;
  
        /* Remember, the last WORD of PFMHEADER struct is part of chr wdth table */
        if (TFM->typeface->typefaceMetrics.spacing)
            pfmh->dfDevice = sizeof(PFMHEADER) - sizeof(WORD) + sizeof(PFMEXTENSION);
        else
            {
            pfmh->dfDevice = sizeof(PFMHEADER) + sizeof(PFMEXTENSION) +
            (DWORD)((lastchar - FRSTCHAR + 2) * sizeof(CHARWDTH) - sizeof(WORD));
            }

        pfmh->dfFace = pfmh->dfDevice + DRVNAMESIZE;
  
        pfmh->dfBitsPointer = 0L;
  
        pfmh->dfBitsOffset = 0L;
  
  
        /* write pfmh */
  
        DBGt2p(("wt_header_struc: About to write pfm header\n"));
  
        _lwrite(outfile, (LPSTR)pfmh, sizeof(PFMHEADER)-sizeof(pfmh->dfCharOffset));
  
        DBGt2p(("wt_header_struc: Wrote pfm header, now write unlock & free.\n"));
  
        if (!(pfmh->dfPixWidth))
        {
            wt_char_metrics(TFM, outfile, lastchar, lpSS);
            DBGt2p(("wt_header_struc: wrote char extent table\n"));
        }
  
        GlobalUnlock(hP);
    }
  
    GlobalUnlock(hT);
    if (hP)
        GlobalFree(hP);
  
    DBGentry(("wt_header_struc: exit\n"));
}
  
  
/**************************************************************************
  
Routine Title: wt_char_metrics
  
General Info: This routine writes the extent table for scalable fonts,
rather than a width table for bitmap fonts.
  
***************************************************************************/
  
/* CHARWDTH *get_char_metrics(struct TFMType *TFM, int out, int class) */
  
void LOCAL wt_char_metrics(struct TFMType far *TFM, int out, BYTE lastchar,
LPSTR lpSS)
  
{
    register int j;
    int         i,ssnum, length, index;
    BOOL ssfound;
    LPCHARWDTH CW;
    HANDLE   hC;
  
  
    DBGentry(("wt_char_metrics: enter\n"));
  
    ssnum = TFM->typeface->general.numberSymbolSets;
  
    DBGt2p(("wt_char_metrics: ssnum = %d\n", ssnum));
  
    for(i=0 ; i<ssnum; i++)
    {
        /* Check for correct symset */
        ssfound=(lstrcmp(TFM->typeface->symbol.symbolSetDirectory[i].selectionName,
        lpSS) == 0);
  
        if (ssfound)
        {
            DBGt2p(("wt_char_metrics: ssfound, i = %d\n", i));
  
//            length = TFM->typeface->symbol.symbolSetDirectory[i].symbolLength;
            length = (lastchar -  FRSTCHAR + 1 );
//            if (length > lastchar)
//                length = lastchar;
  
            DBGt2p(("wt_char_metrics:          length = %d\n", length));
  
            /* GHND memory: zero-init'ed & moveable */
            /* Use length + 1 to get empty width field at end of table for sentinel */
//            if (hC = GlobalAlloc(GHND,(DWORD)((length + 2) * sizeof(CHARWDTH))))
            if (hC = GlobalAlloc(GHND,(DWORD)((length + 1) * sizeof(CHARWDTH))))
            {
                if (!(CW = (LPCHARWDTH)GlobalLock(hC)))
                {
                    GlobalFree(hC);
                    return;
                }
            }
            else
                return;
  
//            if (!((hC = GlobalAlloc(GHND,
//                (DWORD)((length + 2) * sizeof(CHARWDTH)))) &&
//                (CW = (LPCHARWDTH)GlobalLock(hC))))
//                return;
  
            DBGt2p(("wt_char_metrics:          alloced mem\n"));
  
//            for(j=0; j<=length; j++)
            for(j=0; j<length; j++)
            {
                index = TFM->typeface->symbol.symbolSetDirectory[i].symbolIndex[j];
  
                //DBGt2p(("wt_char_metrics:          index = %d\n", index));
  
                if (index == -1)
                    CW[j].width = 0;
                else
                {
                    CW[j].width =
                    TFM->typeface->characterMetrics.character[index].horizontalEscapement;
  
                    if (gPtSize)
                        CW[j].width = duconvert(TFM, gPtSize, CW[j].width);
                }
  
                //DBGt2p(("wt_char_metrics:          CW[%d].width = %d\n", j, CW[j].width));
            }
  
            break;    /* Don't check any more symbol sets */
        }
    }
  
//    _lwrite(out, (LPSTR)CW, ((length + 2) * sizeof(CHARWDTH)));
    _lwrite(out, (LPSTR)CW, ((length + 1) * sizeof(CHARWDTH)));
  
    GlobalUnlock(hC);
    GlobalFree(hC);
  
    DBGentry(("wt_char_metrics: exit\n"));
}
  
  
/**************************************************************************
  
Routine Title: wt_real_pfmext
  
***************************************************************************/
  
void LOCAL wt_real_pfmext(int out, long pfm_start, long fontnm_pos,
BYTE lastchar, WORD spacing)
  
  
{
    LPPFMEXTENSION pfmext; /* pointer to PFM extension structure */
    HANDLE hP;
  
    /* allocate memory for PFMEXTENSION pointer */
    if ((hP = GlobalAlloc(GMEM_MOVEABLE, (DWORD)sizeof(PFMEXTENSION))) &&
        (pfmext = (LPPFMEXTENSION)GlobalLock(hP)))
    {
        /* Remember, the last WORD of PFMHEADER is part of char width table. */
        /* The dfCharOffset field of the PFMHEADER must be a WORD            */
        if (spacing)
            _llseek(out, (pfm_start +
            (LONG)(sizeof(PFMHEADER) - sizeof(WORD))), 0);
        else
            _llseek(out, (pfm_start + (LONG)sizeof(PFMHEADER) +
            (LONG)((lastchar - FRSTCHAR + 1) * sizeof(CHARWDTH))), 0);
  
        pfmext->dfSizeFields = sizeof(PFMEXTENSION);
  
        pfmext->dfExtMetricsOffset = fontnm_pos - pfm_start;
  
        pfmext->dfExtentTable = (gPtSize || spacing) ? 0 :
        sizeof(PFMHEADER) - sizeof(WORD);
  
        pfmext->dfOriginTable = 0L; /* Always zero for printer fonts */
  
        /* Can't determine dfPairKernTable until dfDriverInfo is defined */
  
        pfmext->dfTrackKernTable = 0L;  /* No support for track kerning now */
  
        pfmext->dfDriverInfo = pfmext->dfExtMetricsOffset
        + (LONG)sizeof(EXTTEXTMETRIC);
  
        pfmext->dfReserved = 0L;
  
#ifdef KERN
        pfmext->dfPairKernTable = pfmext->dfDriverInfo +
        (LONG)sizeof(DRIVERINFO);
#else
        pfmext->dfPairKernTable = 0L;
#endif
  
        _lwrite(out, (LPSTR)pfmext, sizeof(PFMEXTENSION));
  
        GlobalUnlock(hP);
  
        /* Jump back to the end of the file */
        _llseek(out, 0L, 2);
    }
  
    if (hP)
        GlobalFree(hP);
  
}
  
  
  
/**************************************************************************
  
Routine Title: wt_extext_struc
  
***************************************************************************/
  
void LOCAL wt_extext_struc(struct TFMType far *TFM, int out, short twip,
BYTE Orient)
  
  
{
    DWORD ptsizeN, ptsizeD, ptunitsN, ptunitsD;
  
    LPEXTTEXTMETRIC extmet; /* pointer to PFM extension structure */
    HANDLE hE;
  
  
    /* allocate memory for EXTTEXTMETRIC pointer */
  
    if ((hE = GlobalAlloc(GMEM_MOVEABLE, (DWORD)sizeof(EXTTEXTMETRIC))) &&
        (extmet = (LPEXTTEXTMETRIC)GlobalLock(hE)))
    {
        extmet->etmSize = sizeof(EXTTEXTMETRIC);
  
        extmet->etmOrientation = Orient;
  
  
        if (gPtSize)
        {
            extmet->etmPointSize = gPtSize * 20 + twip;
  
            extmet->etmMasterHeight = (gPtSize * DEV_RES) / 72;
  
            extmet->etmMinScale = extmet->etmMaxScale =
            extmet->etmMasterUnits = extmet->etmMasterHeight;
        }
        else
        {
            ptunitsN = TFM->typeface->typefaceMetrics.pointN;
            ptunitsD = TFM->typeface->typefaceMetrics.pointD;
            ptsizeN = TFM->typeface->typefaceMetrics.nominalPointSizeN;
            ptsizeD = TFM->typeface->typefaceMetrics.nominalPointSizeD;
  
            /* Do we want to adjust this if ptunits != 72.0 ? */
            extmet->etmPointSize = fdiv5(ptsizeN, 20L, 1L, ptsizeD, 1L);
  
            //      extmet->etmMasterHeight = ptsize / ptunits * DEV_RES;
            extmet->etmMasterHeight = fdiv5(ptsizeN,
            ptunitsN,
            (long)DEV_RES,
            ptunitsD,
            ptsizeD);
  
            //      extmet->etmMinScale = MIN_SIZE / ptunits * DEV_RES;
            extmet->etmMinScale = fdiv5(ptunitsN,
            (long)MIN_SIZE,
            (long)DEV_RES,
            ptunitsD,
            1L);
  
            //      extmet->etmMaxScale = MAX_SIZE / ptunits * DEV_RES;
            extmet->etmMaxScale = fdiv5(ptunitsN,
            (long)MAX_SIZE,
            (long)DEV_RES,
            ptunitsD,
            1L);
  
            extmet->etmMasterUnits =
            (short)ldiv(TFM->typeface->typefaceMetrics.designUnitsN,
            TFM->typeface->typefaceMetrics.designUnitsD);
        }
  
        extmet->etmCapHeight = TFM->typeface->typefaceMetrics.capheight;
  
        extmet->etmXHeight = TFM->typeface->typefaceMetrics.xHeight;
  
        extmet->etmLowerCaseAscent = TFM->typeface->typefaceMetrics.lowercaseAscent;
  
        extmet->etmLowerCaseDescent = TFM->typeface->typefaceMetrics.lowercaseDescent;
  
        extmet->etmSlant = TFM->typeface->typefaceMetrics.slant;
  
        extmet->etmSuperScript = (extmet->etmCapHeight - extmet->etmXHeight);
  
        extmet->etmSubScript = - extmet->etmLowerCaseDescent;
  
        extmet->etmSuperScriptSize =
        extmet->etmSubScriptSize = extmet->etmMasterUnits;
  
        extmet->etmUnderlineOffset = TFM->typeface->typefaceMetrics.underscoreDescent;
  
        extmet->etmUnderlineWidth = TFM->typeface->typefaceMetrics.underscoreThickness;
  
        if (gPtSize)    /* Convert bitmap font data to 300dpi dots */
        {
            extmet->etmCapHeight = duconvert(TFM, gPtSize,
            extmet->etmCapHeight);
  
            extmet->etmXHeight = duconvert(TFM, gPtSize, extmet->etmXHeight);
  
            extmet->etmLowerCaseAscent = duconvert(TFM, gPtSize,
            extmet->etmLowerCaseAscent);
  
            extmet->etmLowerCaseDescent = duconvert(TFM, gPtSize,
            extmet->etmLowerCaseDescent);
  
            extmet->etmSlant = duconvert(TFM, gPtSize, extmet->etmSlant);
  
            extmet->etmUnderlineOffset = duconvert(TFM, gPtSize,
            extmet->etmUnderlineOffset);
  
            extmet->etmUnderlineWidth = duconvert(TFM, gPtSize,
            extmet->etmUnderlineWidth);
        }
  
        extmet->etmSuperScript = (extmet->etmCapHeight - extmet->etmXHeight);
  
        extmet->etmSubScript = - extmet->etmLowerCaseDescent;
  
        extmet->etmSuperScriptSize =
        extmet->etmSubScriptSize = extmet->etmMasterUnits;
  
        extmet->etmDoubleUpperUnderlineOffset = extmet->etmUnderlineOffset;
  
        extmet->etmDoubleLowerUnderlineOffset =
        (extmet->etmDoubleUpperUnderlineOffset +
        (2 * extmet->etmUnderlineWidth));
  
        extmet->etmDoubleUpperUnderlineWidth =
        extmet->etmDoubleLowerUnderlineWidth = extmet->etmUnderlineWidth;
  
        extmet->etmStrikeOutOffset = extmet->etmXHeight / 2;
  
        extmet->etmStrikeOutWidth = extmet->etmXHeight / 4;
  
#ifdef KERN
        extmet->etmKernPairs = TFM->typeface->kerning.realPairs;
#else
        extmet->etmKernPairs = 0;
#endif
        extmet->etmKernTracks = 0; /* TFM->typeface->kerning.numberTracks; */
  
  
        _lwrite(out, (LPSTR)extmet, sizeof(EXTTEXTMETRIC));
  
        GlobalUnlock(hE);
    }
  
    if (hE)
        GlobalFree(hE);
  
}
  
  
  
/***************************************************************************
  
Routine Title: get_weight
  
***************************************************************************/
  
void LOCAL mk_pitch(struct TFMType far *TFM, LPSTR lpStr, WORD spacing)
  
{
    LONG N, D;      /* Numerator & Denominator */
    int pitch_int;  /* Truncated pitch value * 100  */
  
    N = lmul(lmul(TFM->typeface->typefaceMetrics.pointD,
    TFM->typeface->typefaceMetrics.nominalPointSizeD),
    TFM->typeface->typefaceMetrics.designUnitsN);
  
    D = lmul(lmul(lmul(TFM->typeface->typefaceMetrics.pointN,
    TFM->typeface->typefaceMetrics.nominalPointSizeN),
    TFM->typeface->typefaceMetrics.designUnitsD),
    (LONG)spacing);
  
    pitch_int = (int)ldiv(lmul(N, 100L), D);
  
    /* Round up if the remainder's big enough */
    if (lmod(N, D) > ldiv(D, 2L))
        pitch_int++;
  
    itoa((pitch_int / 100), lpStr);
  
    lstrcat(lpStr, (LPSTR)".");
  
    itoa((pitch_int - (pitch_int / 100) * 100),(LPSTR)&lpStr[lstrlen(lpStr)]);
}
  
/***************************************************************************
  
Routine Title: get_weight
  
***************************************************************************/
  
WORD LOCAL get_weight(int tfmweight)
  
  
{
  
    if (tfmweight < 18)
        return(FW_THIN-50);
    else if (tfmweight < 35)
        return(FW_THIN);
    else if (tfmweight < 52)
        return(FW_EXTRALIGHT-50);
    else if (tfmweight < 69)
        return(FW_EXTRALIGHT);
    else if (tfmweight < 86)
        return(FW_LIGHT-50);
    else if (tfmweight < 103)
        return(FW_LIGHT);
    else if (tfmweight < 120)
        return(FW_LIGHT+50);
    else if (tfmweight < 137)
        return(FW_NORMAL);
    else if (tfmweight < 154)
        return(FW_MEDIUM);
    else if (tfmweight < 171)
        return(FW_SEMIBOLD);
    else if (tfmweight < 188)
        return(FW_BOLD);
    else if (tfmweight < 205)
        return(FW_BOLD+50);
    else if (tfmweight < 222)
        return(FW_EXTRABOLD);
    else if (tfmweight < 239)
        return(FW_EXTRABOLD+50);
    else if (tfmweight < 256)
        return(FW_HEAVY);
    else
        return(FW_DONTCARE);
  
}
  
  
  
/****************************************************************************
  
Routine Title: get_face
  
****************************************************************************/
  
BYTE LOCAL get_face(int serifstyle, int fixpitch)
  
{
    /* This does not take in to account the FF_DECORATIVE case */
    /* because this info can not be found in a TFM file   */
  
    if (fixpitch)                                   /* Modern (fixed pitch) */
        return(FF_MODERN);
    else if ((serifstyle > 0 && (serifstyle < 37)) ||
        ((serifstyle > 150) && (serifstyle < 170)))  /* San Serif */
        return(FF_SWISS);
    else if (serifstyle < 150)                           /* Serif   */
        return(FF_ROMAN);
    else if (serifstyle < 255)                           /* Script */
        return(FF_SCRIPT);
    else
        return(FF_DONTCARE);                              /* Don't Care */
  
}
  
  
  
/**************************************************************************
  
Routine Title: wt_driver_struc
  
***************************************************************************/
  
void LOCAL wt_driver_struc(struct TFMType far *TFM, int fp, long pfm_start,
DWORD MemUsage, int ssval)
{
    LPDRIVERINFO lpdrvinfo;
    HANDLE hD;
  
    if ((hD = GlobalAlloc(GMEM_MOVEABLE, (DWORD)sizeof(DRIVERINFO))) &&
        (lpdrvinfo = (LPDRIVERINFO)GlobalLock(hD)))
    {
        lpdrvinfo->epSize = sizeof(DRIVERINFO);
        lpdrvinfo->epVersion = DRIVERINFO_VERSION;
        lpdrvinfo->epMemUsage =  MemUsage;
  
#ifdef KERN
        lpdrvinfo->epEscape = _llseek(fp, 0L, 1) + (LONG)sizeof(DRIVERINFO) +
        (LONG)(TFM->typeface->kerning.realPairs * sizeof(KERNPAIR)) -
        pfm_start;
#else
        lpdrvinfo->epEscape = _llseek(fp, 0L, 1) + (LONG)sizeof(DRIVERINFO) -
        pfm_start;
#endif
  
        switch (ssval)
        {
            case M8:
                lpdrvinfo->xtbl.symbolSet = epsymMath8;
                break;
            case MS:
                lpdrvinfo->xtbl.symbolSet = epsymMathSymbols;
                break;
            case LG:
                lpdrvinfo->xtbl.symbolSet = epsymUSLegal;
                break;
            case PI:
            case US:
                lpdrvinfo->xtbl.symbolSet = epsymGENERIC7;
                break;
            default:
                lpdrvinfo->xtbl.symbolSet = epsymGENERIC8;
        }
  
        lpdrvinfo->xtbl.offset = 0L;
        lpdrvinfo->xtbl.len = 0;
        lpdrvinfo->xtbl.firstchar = 0;
        lpdrvinfo->xtbl.lastchar = 0;
  
        _lwrite(fp, (LPSTR)lpdrvinfo, sizeof(DRIVERINFO));
  
        GlobalUnlock(hD);
    }
  
    if (hD)
        GlobalFree(hD);
}
  
  
  
/**************************************************************************
  
Routine Title: wt_kernpr_tbl
  
***************************************************************************/
#ifdef KERN
void LOCAL wt_kernpr_tbl(struct TFMType far *TFM, int fp)
{
    int i;
    kern_pairs_type Kern;
    HANDLE hK;
    WORD real_pairs = 0;
  
    Kern.npairs = TFM->typeface->kerning.realPairs;
  
    if ((hK = GlobalAlloc(GMEM_MOVEABLE,
        (DWORD)(Kern.npairs * sizeof(KERNPAIR)))) &&
        (Kern.kpptr = (LPKERNPAIR)GlobalLock(hK)))
    {
        for (i=0; i<TFM->typeface->kerning.numberPairs; i++)
        {
            if (TFM->typeface->kerning.kernPairs[i].kernValue)
            {
                Kern.kpptr[real_pairs].kpPair.each[1] =
                (BYTE)(TFM->typeface->kerning.kernPairs[i].firstCharIndex);
                Kern.kpptr[real_pairs].kpPair.each[2] =
                (BYTE)(TFM->typeface->kerning.kernPairs[i].secondCharIndex);
  
                /* This should be in font units */
                Kern.kpptr[real_pairs].kpKernAmount =
                TFM->typeface->kerning.kernPairs[i].kernValue;
  
                real_pairs++;
            }
        }
  
        _lwrite(fp, (LPSTR)Kern.kpptr, (real_pairs * sizeof(KERNPAIR)));
  
        GlobalUnlock(hK);
    }
  
    if (hK)
        GlobalFree(hK);
}
#endif
  
  
/**************************************************************************
  
Routine Title: wt_escape_str
  
***************************************************************************/
  
void LOCAL wt_escape_str(struct TFMType far *TFM, int fp, LPSTR lpPtSizes,
LPSTR lpSS, BOOL PCLV)
{
    BYTE lpStr[32];
    int i = 0;
    WORD Width;
    int len;        /* length of one point size string */
  
  
    /* Add symbol set */
    lstrcpy((LPSTR)lpStr, (LPSTR)"\x1B(");
    lstrcat((LPSTR)lpStr, lpSS);
    lstrcat((LPSTR)lpStr, (LPSTR)"\x1B(s");
  
    /* Determine spacing (fixed/prop) */
    if (TFM->typeface->typefaceMetrics.spacing)
        lstrcat((LPSTR)lpStr, (LPSTR)"0p");
    else
        lstrcat((LPSTR)lpStr, (LPSTR)"1p");
    _lwrite(fp, (LPSTR)lpStr, lstrlen((LPSTR)lpStr));
  
    /* Determine style word */
    i = (WORD)(TFM->typeface->typefaceMetrics.typeStruct / 8);
  
    i = i << 5;
    Width = TFM->typeface->typefaceMetrics.appearanceWidth;
  
    if (Width < 21)
        Width = 4;
    else if (Width < 48)
        Width = 3;
    else if (Width < 75)
        Width = 2;
    else if (Width < 129)
        Width = 1;
    else if (Width < 156)
        Width = 0;
    else if (Width < 210)
        Width = 6;
    else
        Width = 7;
  
    i |= (Width << 2);
  
    if ((WORD)(TFM->typeface->typefaceMetrics.slant))
        i++;
  
    itoa(i, (LPSTR)lpStr);
    lstrcat((LPSTR)lpStr, (LPSTR)"s");
  
    _lwrite(fp, (LPSTR)lpStr, lstrlen((LPSTR)lpStr));
  
    /* Determine stroke weight */
    i = (TFM->typeface->typefaceMetrics.strokeWeight / 17) - 7;
    itoa(i, (LPSTR)lpStr);
    lstrcat((LPSTR)lpStr, (LPSTR)"b");
  
    _lwrite(fp, (LPSTR)lpStr, lstrlen((LPSTR)lpStr));
  
    lstrcpy((LPSTR)lpStr,
    (LPSTR)TFM->typeface->general.typefaceSelectionString);
  
    if (!PCLV)
    {
        i = myatoi((LPSTR)lpStr) % 0x0100;  /* mod 256 */
        itoa(i, (LPSTR)lpStr);
    }
  
    if (lpPtSizes)
    {
        lstrcat((LPSTR)lpStr, (LPSTR)"T\x1B(s");
  
        len = 0;
        while (lpPtSizes[len] && (!isspace(lpPtSizes[len])))
            len++;
  
        i = lstrlen((LPSTR)lpStr);
  
        lstrcpyn((LPSTR)&lpStr[i], (LPSTR)lpPtSizes, len);
  
        lpStr[i + len] = '\0';
  
        if (TFM->typeface->typefaceMetrics.spacing)
        {
            lstrcat((LPSTR)lpStr,(LPSTR)"v");
  
            /* compute pitch */
            mk_pitch(TFM, (LPSTR)&lpStr[lstrlen((LPSTR)lpStr)],
            TFM->typeface->typefaceMetrics.spacing);
  
            lstrcat((LPSTR)lpStr,(LPSTR)"H");
        }
        else
            lstrcat((LPSTR)lpStr,(LPSTR)"V");
    }
    else if (TFM->typeface->typefaceMetrics.spacing)
        lstrcat((LPSTR)lpStr, (LPSTR)"T\x1B(s#PITCHH");
    else
        lstrcat((LPSTR)lpStr, (LPSTR)"T\x1B(s#HEIGHTV");
  
    _lwrite(fp, (LPSTR)lpStr, (lstrlen((LPSTR)lpStr) + 1));
}
  
  
  
/**************************************************************************
  
Routine Title: wt_font_name
  
***************************************************************************/
  
long LOCAL wt_font_name(int fp, LPSTR lpName, int ssval, BYTE Width)
{
    BYTE temp_name[50]; /* name revised a la Type Director */
    BYTE full_name[50]; /* name with symset appended */
    BYTE usr_ss[6];     /* user-friendly symset name extension */
    int pos;            /* position in name array */
  
    lstrcpy((LPSTR)temp_name, lpName);
  
    MoveFamilyName((LPSTR)full_name, (LPSTR)temp_name, sizeof(full_name));
  
  
    //  lstrcpy((LPSTR)full_name, lpName);
  
    //  get_width_name(Width, (LPSTR)&full_name[lstrlen((LPSTR)full_name)]);
  
    usr_ss[0] = ' ';
    usr_ss[1] = '(';
    usr_ss[4] = ')';
    usr_ss[5] = '\0';
  
    switch (ssval)
    {
        case D1:
            usr_ss[2] = 'D';
            usr_ss[3] = '1';
            break;
        case D2:
            usr_ss[2] = 'D';
            usr_ss[3] = '2';
            break;
        case D3:
            usr_ss[2] = 'D';
            usr_ss[3] = '3';
            break;
        case DS:
            usr_ss[2] = 'D';
            usr_ss[3] = 'S';
            break;
        case DT:
            usr_ss[2] = 'D';
            usr_ss[3] = 'T';
            break;
        case DV:
            usr_ss[2] = 'D';
            usr_ss[3] = 'V';
            break;
        case E1:
            usr_ss[2] = 'E';
            usr_ss[3] = '1';
            break;
        case LG:
            usr_ss[2] = 'L';
            usr_ss[3] = 'G';
            break;
        case M8:
            usr_ss[2] = 'M';
            usr_ss[3] = '8';
            break;
        case MS:
            usr_ss[2] = 'M';
            usr_ss[3] = 'S';
            break;
        case PB:
            usr_ss[2] = 'P';
            usr_ss[3] = 'B';
            break;
        case PC:
            usr_ss[2] = 'P';
            usr_ss[3] = 'C';
            break;
        case PD:
            usr_ss[2] = 'P';
            usr_ss[3] = 'D';
            break;
        case PI:
            usr_ss[2] = 'P';
            usr_ss[3] = 'I';
            break;
        case PM:
            usr_ss[2] = 'P';
            usr_ss[3] = 'M';
            break;
        case R8:
            usr_ss[2] = 'R';
            usr_ss[3] = '8';
            break;
        case TS:
            usr_ss[2] = 'T';
            usr_ss[3] = 'S';
            break;
        case US:
            usr_ss[2] = 'U';
            usr_ss[3] = 'S';
            break;
        case VI:
            usr_ss[2] = 'V';
            usr_ss[3] = 'I';
            break;
        case VM:
            usr_ss[2] = 'V';
            usr_ss[3] = 'M';
            break;
        case VU:
            usr_ss[2] = 'V';
            usr_ss[3] = 'U';
            break;
        case WN:
            usr_ss[2] = 'W';
            usr_ss[3] = 'N';
            break;
        default:
            usr_ss[2] = 'U';
            usr_ss[3] = 'D';
    }
  
    lstrcat((LPSTR)full_name, (LPSTR)usr_ss);
  
    _lwrite(fp, (LPSTR)full_name, lstrlen((LPSTR)full_name) + 1);
  
    return(_llseek(fp, 0L, 1));
}
  
  
  
/**************************************************************************
  
Routine Title: wt_trackrn_tbl
  
***************************************************************************/
  
/*
void wt_trackrn_tbl(struct TFMType *TFM, int fp)
{
  
}
*/
  
  
/**************************************************************************
  
Routine Title: wt_driver_name
  
***************************************************************************/
  
void LOCAL wt_driver_name(int fp)
{
//  LPSTR lpString = "PCL5 / HP LaserJet III";
//  _lwrite(fp, (LPSTR)String, DRVNAMESIZE);
//  BYTE String[DRVNAMESIZE]; */
//  
//  lstrcpy(String, (LPSTR)"PCL5 / HP LaserJet III"); */
//  _lwrite(fp, String, lstrlen(String) + 1); */
  
    _lwrite(fp, (LPSTR)"PCL5 / HP LaserJet III", DRVNAMESIZE);
  
//  LPSTR lpString;
//  HANDLE hStr;
//
//  hStr = GlobalAlloc(GMEM_MOVEABLE, (DWORD)DRVNAMESIZE);
//  lpString = (LPSTR)GlobalLock(hStr);
//
//  lstrcpy(lpString, (LPSTR)"PCL5 / HP LaserJet III");
//  _lwrite(fp, lpString, lstrlen((LPSTR)lpString) + 1);
//
//  GlobalUnlock(hStr);
//  GlobalFree(hStr);
}
  
  
/******************************************************************
Function: get_charset
Modified: October 11, 1989 (DAL)
Summary: Get character set
Inputs: x
Outputs: Character set
******************************************************************/
//WORD get_charset (x)
//
//int x;
//
//{
//    if (x & MATH_SET)
//      return MATH8_CHARSET;
//
//    if (x & PI_SET)
//      return PIFONT_CHARSET;
//
//    if (x & LINE_SET)
//      return LINEDRAW_CHARSET;
//
//    if (x & PC_LINE_SET)
//      return PCLINE_CHARSET;
//
//    if (x & TAXLINE_SET)
//      return TAXLINE_CHARSET;
//
//  return (0);
//}
  
/***************************************************************************
  
Routine Title:  get_width_name
  
Summary:  This routine generates a name for the specified width
  
Inputs:  Width - value for appearance width in the glue file.
  
Outputs:  pointer to a char str for the name of the width value.
  
  
****************************************************************************/
//void LOCAL get_width_name(BYTE Width, LPSTR lpString)
//  
//{
//  if (Width < 21)
//      lstrcpy(lpString, (LPSTR)" UCmp");
//  else if (Width < 48)
//      lstrcpy(lpString, (LPSTR)" XCmp");
//  else if (Width < 75)
//      lstrcpy(lpString, (LPSTR)" XCd");
//  else if (Width < 129)
//      lstrcpy(lpString, (LPSTR)" Cd");
//  else if (Width < 156)
//      return;
//  else if (Width < 210)
//      lstrcpy(lpString, (LPSTR)" Ext");
//  else
//      lstrcpy(lpString, (LPSTR)" XExt");
//}
//
