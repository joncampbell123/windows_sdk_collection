/**[f******************************************************************
* realize.c -
*
* Copyright (C) 1988,1989 Aldus Corporation.
* Copyright (C) 1988-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.
* Company confidential.
*
**f]*****************************************************************/
  
/*
* $Header: realize.c,v 3.890 92/02/06 16:12:09 dtk FREEZE $
*/
  
/*
* $Log:	realize.c,v $
 * Revision 3.890  92/02/06  16:12:09  16:12:09  dtk (Doug Kaltenecker)
 * Win3.1 Freeze
 * 
 * Revision 3.874  92/02/06  15:53:59  15:53:59  dtk (Doug Kaltenecker)
 * dont realize tt at 75 or 150 dpi.
 * 
 * Revision 3.873  92/01/30  11:51:48  11:51:48  daniels (Susan Daniels)
 * Fix BUG #788:  thinlines in pastels not printing.  Changed all thinlines
 * and 1 dot lines that are not white, to black.  This is what Uni-driver 
 * does.
 * 
 * Revision 3.872  92/01/02  16:07:18  16:07:18  dtk (Doug Kaltenecker)
 * Changed some ifdef WIN31s to compile a 30 version
 * 
 * Revision 3.871  91/12/02  16:44:29  16:44:29  dtk (Doug Kaltenecker)
 * Changed the ifdef TTs to ifdef WIN31s.
 * 
 * Revision 3.871  91/11/22  13:19:15  13:19:15  dtk (Doug Kaltenecker)
 * Win 3.1 Post Beta 3 version.
 * 
 * Revision 3.870  91/11/08  11:43:52  11:43:52  dtk (Doug Kaltenecker)
 * 3.1 Release Candidate 1
 * 
 * Revision 3.865  91/11/01  13:51:52  13:51:52  dtk (Doug Kaltenecker)
 * Beta release for Windows 3.1
 * 
 * Revision 3.862  91/10/25  13:47:12  13:47:12  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.860  91/10/23  09:48:39  09:48:39  dtk (Doug Kaltenecker)
 * WinWird Release
 * 
 * Revision 3.852  91/10/09  14:59:40  14:59:40  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.850  91/10/04  16:49:52  16:49:52  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE
 * 
 * Revision 3.840  91/09/28  14:17:11  14:17:11  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE 3 FOR WINWORD
 * 
 * Revision 3.830  91/09/18  16:33:27  16:33:27  dtk (Doug Kaltenecker)
 * RELEASE
 * 
 * Revision 3.822  91/09/16  10:34:00  10:34:00  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE 2 FOR WIN30.
 * 
 * Revision 3.820  91/09/06  14:12:06  14:12:06  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE FOR WINWORD 2.0
 * 
 * Revision 3.813  91/09/04  11:45:32  11:45:32  dtk (Doug Kaltenecker)
 * Moved the local #define TT to build.h and included it.
 * 
 * Revision 3.812  91/08/22  14:32:02  14:32:02  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.808  91/08/22  13:24:30  13:24:30  dtk (Doug Kaltenecker)
 * When asked for a 0 height font, we now return a default of 1
 * 12 point if it's scalable.
 * 
 * Revision 3.807  91/08/08  10:31:19  10:31:19  dtk (Doug Kaltenecker)
 * PREBETA3
 * 
 * Revision 3.803  91/08/05  16:41:14  16:41:14  dtk (Doug Kaltenecker)
 * *** empty log message ***
 * 
 * Revision 3.802  91/07/22  12:54:16  12:54:16  oakeson (Ken Oakeson)
 * BETA
 * 
 * Revision 3.800  91/07/21  12:36:17  12:36:17  dtk (Doug Kaltenecker)
 * uncommented code for tt as reaster
 * 
 * Revision 3.799  91/07/02  11:51:43  11:51:43  daniels (Susan Daniels)
 * Beta
 * 
 * Revision 3.797  91/07/01  16:30:51  16:30:51  dtk (Doug Kaltenecker)
 * added support for TT as graphics
 * 
 * Revision 3.796  91/06/26  11:26:02  11:26:02  stevec (Steve Claiborne)
 * BETA
 * 
 * Revision 3.790  91/06/11  16:03:18  16:03:18  stevec (Steve Claiborne)
 * Freeze
 * 
 * Revision 3.786  91/06/11  15:44:15  15:44:15  dtk (Doug Kaltenecker)
 * Prettified files!
 * 
* Revision 3.785  91/05/22  14:56:57  14:56:57  stevec (Steve Claiborne)
* Beta version to MS
*
* Revision 3.781  91/05/22  13:54:23  13:54:23  oakeson (Ken Oakeson)
* Made GetRasterizerCaps work for 3.0 and 3.1
*
* Revision 3.780  91/05/15  15:57:06  15:57:06  stevec (Steve Claiborne)
* Beta
*
* Revision 3.776  91/05/14  13:26:26  13:26:26  oakeson (Ken Oakeson)
* Forced complete exit of EnumDFonts when callback function returns zero
*
* Revision 3.775  91/04/05  14:30:59  14:30:59  stevec (Steve Claiborne)
* Beta release to MS
*
* Revision 3.770  91/03/25  15:35:58  15:35:58  stevec (Steve Claiborne)
* maintance release
*
* Revision 3.760  91/03/12  07:52:42  07:52:42  stevec (Steve Claiborne)
* Maintance release
*
* Revision 3.755  91/03/03  07:46:03  07:46:03  stevec (Steve Claiborne)
* March 3 Freeze
*
* Revision 3.720  91/02/11  09:15:21  09:15:21  stevec (Steve Claiborne)
* Aldus version
*
* Revision 3.711  91/02/08  16:26:29  16:26:29  stevec (Steve Claiborne)
* Added debuging
*
* Revision 3.710  91/02/04  15:47:40  15:47:40  stevec (Steve Claiborne)
* Aldus freeze
*
* Revision 3.700  91/01/19  09:00:21  09:00:21  stevec (Steve Claiborne)
* Release
*
* Revision 3.685  91/01/14  15:43:16  15:43:16  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.681  91/01/14  10:17:32  10:17:32  stevec (Steve Claiborne)
* Updated the copy right stmt.
*
* Revision 3.680  91/01/10  16:16:39  16:16:39  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.671  90/12/20  12:02:17  12:02:17  stevec (Steve Claiborne)
* Put ifdef around all truetype stuff but left default truetype defined.
* This fixes bug #105.  SJC 12-20-90
*
* Revision 3.670  90/12/14  14:54:04  14:54:04  stevec (Steve Claiborne)
* freeze for 12-14-90 ver. 3.670
*
* Revision 3.665  90/12/10  15:35:48  15:35:48  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.661  90/12/10  10:04:48  10:04:48  stevec (Steve Claiborne)
* Removed some unreferenced local variables.  SJC
*
* Revision 3.660  90/12/07  14:50:17  14:50:17  stevec (Steve Claiborne)
* Freeze 12-7-90
*
* Revision 3.651  90/12/07  14:31:22  14:31:22  stevec (Steve Claiborne)
* Changed hand-tuned bitmap and thresholds of white and black.
*
* Revision 3.604  90/11/19  08:51:41  08:51:41  tshanno
n (Terry Shannon)
* Tuned gray scaling for HP Laserjets.  Also added lighten g
ray scale
* button.  Terry Shannon 11-19-90
*
* Revision 3.603  90/10/25
17:16:34  17:16:34  oakeson
(Ken Oakeson)
* Used GetVersion instead of #ifdef to separate truetype code
*
* Revision 3.602  90/08/24  11:37:55  11:37:55  daniels (Susan Daniels)
* message.txt
*
* Revision 3.601  90/08/14  15:35:21  15:35:21  oakeson (Ken Oakeson)
* Added TrueType support
*
*/
  
/***************************************************************************/
/******************************   realize.c   ******************************/
//
//  Font enumeration module.
//
//  rev:
//
// 27 Jan 92  SD     Map 1 dot lines to black.  BUG #788.
// 20 Jul 90  SJC      Removed PCL_* - don't need to lock segments.
// 20 Jul 90   SJC   Saved lpInObj in RealizeObj for later use in
//                   extendedtextout for print direction.
// 09 may 90   SD    Set dfFace = 0 in RealizeObject() to avoid walking
//                   off segment; MS sent fix.  BUGFIX
// 14 feb 90   VO    Changed some calls from ScaleWidth to ScaleVertical
// 03 feb 90    Ken O   Changed lmemcpy() to lstrncpy()
// 01 feb 90   VO    Added 11 point into the enumerated size list.
// 20 jan 90    Ken O   Removed non-Galaxy support
// 12 jan 90   VO    Added provisions for scalable fonts.
// 25 oct 89    peterbe sorted init. values by type to elim. annoying compiler
//          warning
// 10-12-89@12:32:59 (Thu) CRAIGC realize a default font for memory dc's
// 15 aug 89    peterbe Changed 'int .. EnumDFonts()'
// 07 aug 89    peterbe Changed lstrcmp() to lstrcmpi().
//  11-30-86    msd cleanup/fix checkString() and Translate()
//  11-16-86    msd integrated fontSummary structure into RealizObject
//  11-09-86    msd cleanup, document and add lots of debug stuff
//  11-07-86    msd compile for Wrning level 2
//  12-29-88    jimmat  Added some quick(er)-out checks to RealizeObject()
//          and reduced the number of locks/unlocks by xxxFace()
//          routines.
//   1-17-89    jimmat  Added PCL_* entry points to lock/unlock data seg.
//   1-19-89    jimmat  Return a fixed size for font structure--this way we
//          don't have to realize every font twice (once just
//                      to get the len of the facename, once to really do it).
//   2-24-89    jimmat  Converted the face table from a resource to a table
//                      resident in the code segment--no need to load/lock,etc.
//
  
//#define DEBUG
  
#include "generic.h"
#include "resource.h"
#define FONTMAN_ENABLE
#include "fontman.h"
#include "fonts.h"
#include "utils.h"
#include "paper.h"
#include "truetype.h"
#include "build.h"
  
/*  lockfont utility
*/
#include "lockfont.c"
  
/*  dfType through dfBreakChar in FONTINFO, for realizing memory DC fonts
*/
#define CBMEMFONT 0x21
int display_pbrush_size;
  
/*  Local debug structure.
*/
#ifdef DEBUG
#define LOCAL_DEBUG
#define DBGdumprealize
#endif
  
#ifdef LOCAL_DEBUG
/*  Debug switches: entry - message on entry to primary procs
*                  err   - message when an unexpected err occurs
*                  proc  - detailed messages specific to the proc
*/
#define DBGentry(msg)          DBMSG(msg)
#define DBGerr(msg)            DBMSG(msg)
#define DBGrealize(msg)        DBMSG(msg)
#define DBGfontselect(msg)     DBMSG(msg)
#define DBGinfotostruct(msg)   DBMSG(msg)
#define DBGextractfont(msg)    DBMSG(msg)
#define DBGenumfonts(msg)      DBMSG(msg)
#define DBGface(msg)           /*DBMSG(msg)*/
#else
#define DBGentry(msg)        /*null*/
#define DBGerr(msg)          /*null*/
#define DBGrealize(msg)      /*null*/
#define DBGfontselect(msg)   /*null*/
#define DBGinfotostruct(msg) /*null*/
#define DBGextractfont(msg)  /*null*/
#define DBGenumfonts(msg)    /*null*/
#define DBGface(msg)         /*null*/
#endif
  
  
/*  Macro
*/
#define tmPitchTOlfPitch(pitch) \
((pitch) ? (BYTE)VARIABLE_PITCH : (BYTE)FIXED_PITCH)
  
  
extern  HANDLE hLibInst;            /* driver's instance handle */
  
  
/*  Forward procedures.
*/
  
short FAR PASCAL RealizeObject(LPDEVICE,short,LPLOGFONT,LPFONTINFO,
LPTEXTXFORM);
int   FAR PASCAL EnumDFonts(LPDEVICE,LPSTR,FARPROC,long);
  
void InfoToStruct(LPFONTINFO, short, LPSTR);
void ExtractFontInfo(LPFONTINFO, LPFONTSUMMARY);
  
BOOL defaultFace(LPSTR);
BOOL aliasFace(LPLOGFONT, LPSTR);
  
  
  
char handmade_patterns [256] =
{ 0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00, // 32 black
    0x00,  0x02,  0x00,  0x00,  0x00,  0x20,  0x00,  0x00, // 31
    0x00,  0x40,  0x00,  0x04,  0x00,  0x40,  0x00,  0x04, // 30
    0x00,  0x8A,  0x00,  0x54,  0x00,  0x8A,  0x00,  0x54, // 29
    0x00,  0x6D,  0x00,  0x55,  0x00,  0xD6,  0x00,  0xAA, // 28
    0x00,  0x6D,  0x00,  0x77,  0x00,  0xD6,  0x00,  0xDD, // 27
    0x00,  0x6D,  0x00,  0xDD,  0x00,  0x6B,  0x00,  0xDD, // 26
    0x00,  0x77,  0x00,  0xDD,  0x00,  0x77,  0x00,  0xDD, // 25 (40 dot)
    0x33,  0x8C,  0x33,  0xC8,  0x33,  0x8C,  0x33,  0xC8, // 24 (36 dot)
    0x00,  0x6F,  0x00,  0xFF,  0x00,  0xF6,  0x00,  0xFF, // 23 (34 dot)
    0x00,  0xFF,  0x00,  0xFF,  0x00,  0xFF,  0x00,  0xFF, // 22 (32 dot)
    0x80,  0xFF,  0x00,  0xFF,  0x08,  0xFF,  0x00,  0xFF, // 21 (30 dot)
    0x88,  0xFF,  0x00,  0xFF,  0x22,  0xFF,  0x00,  0xFF, // 20 (28 dot)
    0x88,  0xFF,  0x10,  0xFF,  0x22,  0xFF,  0x01,  0xFF, // 19 (26 dot)
    0x55,  0x77,  0x55,  0xDD,  0x55,  0x77,  0x55,  0xDD, // 18 (24 dot)
    0x2A,  0xFF,  0xA2,  0xFF,  0x8A,  0xFF,  0xA8,  0xFF, // 17 (20 dot)
    0xCB,  0xF9,  0x2F,  0xE5,  0xBC,  0x9F,  0xF2,  0x5E, // 16 (22 dot)
    0x2A,  0xFF,  0xA2,  0xFF,  0x8A,  0xFF,  0xA8,  0xFF, // 15 (20 dot)
    0x2A,  0xFF,  0xAA,  0xFF,  0xA2,  0xFF,  0xAA,  0xFF, // 14 (18 dot)
    0xBB,  0x77,  0xDD,  0xEE,  0xBB,  0x77,  0xDD,  0xEE, // 13 (16 dot)
    0xAA,  0xFF,  0xAA,  0xFF,  0xAA,  0xFF,  0xAA,  0xFF, // 12 (16 dot)
    0xF3,  0x7E,  0xCB,  0xF9,  0x3F,  0xE7,  0x6C,  0x9F, // 11 (18 dot)
    0xF9,  0x3F,  0xE7,  0xFC,  0x9F,  0xF3,  0x7E,  0xCF, // 10 (16 dot)
    0xF9,  0x3F,  0xF7,  0xFC,  0x9F,  0xF3,  0x7F,  0xCF, // 9 (14 dot)
    0xEF,  0xFD,  0xAF,  0x77,  0xFE,  0xDF,  0xFA,  0x77, // 8 (12 dot)
    0xEF,  0xFD,  0xBF,  0xE7,  0xFE,  0xDF,  0xFB,  0x7E, // 7 (10 dot)
    0xEF,  0xFD,  0xBF,  0xF7,  0xFE,  0xDF,  0xFB,  0x7F, // 6 (8 dot)
    0xBF,  0xDF,  0xFB,  0xFD,  0xBF,  0xDF,  0xFB,  0xFD, // 5 (8 dot)
    0xF7,  0xFF,  0x7F,  0xEF,  0xFD,  0xFF,  0xDF,  0xFE, // 4 (6 dot)
    0xBF,  0xFF,  0xFB,  0xFF,  0xBF,  0xFF,  0xFB,  0xFF, // 3 (4 dot)
    0xFF,  0xEF,  0xDF,  0xFF,  0xFF,  0xFE,  0xFD,  0xFF, // 2 (4 dot)
0xFF,  0xFF,  0xDF,  0xFF,  0xFF,  0xFF,  0xFD,  0xFF};// 1 (2 dot)
  
void NEAR PASCAL BuildHalftoneBrush(BYTE grey, BYTE FAR *lpbits)
{
    int x, y, pattern_index;
    int bit_offset = 0;
#ifdef DEBUG_FUNCT
    DB(("Entering BuildHalftoneBrush\n"));
#endif
  
    if (global_grayscale) {
        for (y = 0; y < BRUSH_YSIZE; y++) {
  
            for (x = 0; x < BRUSH_XSIZE; x++) {
  
                if (grey < SCREEN[SCREENX(x)][SCREENY(y)]) {
                    // clear the bit (black)
                    *lpbits &= bit_index[bit_offset];
                } else {
                    // set the bit (white)
                    *lpbits |= ~bit_index[bit_offset];
                }
                bit_offset++;
                if (bit_offset == 8) {
                    bit_offset = 0;
                    lpbits++;
                }
            }
        }
  
    }
    else
    {
        if (grey == 0xFF) {   // The only valid white value.
            for (pattern_index = 0; pattern_index < 8; pattern_index++) {
                for (x = 0; x < (BRUSH_XSIZE / 8); x++) {
                    *lpbits = 0xFF;
                    for (y = 0; y < (BRUSH_YSIZE / 8); y++) {
                        *(lpbits + BRUSH_XSIZE)  = 0xFF;
                    }
                    lpbits++;
                }
            }
        }
        else {  // Do all other shades except for white.
            grey = (BYTE)((int)grey / 8);  // The range of gray is now 0 to 32.
            for (pattern_index = 0; pattern_index < 8; pattern_index++) {
                for (x = 0; x < (BRUSH_XSIZE / 8); x++) {
                    *lpbits = handmade_patterns [((int)grey * 8) + pattern_index];
                    for (y = 0; y < (BRUSH_YSIZE / 8); y++) {
                        *(lpbits + BRUSH_XSIZE)  = handmade_patterns
                        [((int)grey * 8) + pattern_index];
                    }
                    lpbits++;
                }
            }
        }
    }
#ifdef DEBUG_FUNCT
    DB(("Exiting BuildHalftoneBrush\n"));
#endif
}
/***********************************************************************
G E T  F O N T  W E I G H T
***********************************************************************/
  
unsigned long near pascal GetFontWeight(
LPFONTINFO lpSummary,
LPLOGFONT lpInObj,
LPTEXTXFORM lpTextXForm,
LPSTR dfFaceName,
unsigned long bestvalue,
BOOL scalfont)
{
    unsigned long value;
    short tmp, tmp2;
  
#ifdef DEBUG_FUNCT
    DB(("Entering GetFontWeight\n"));
#endif
    value = 0L;
  
    /*  character set
    */
    if (lpInObj->lfCharSet != lpSummary->dfCharSet)
    {
        value += 1 << CHARSET_WEIGHT;
        DBGfontselect(("   %d: charset %d to %d\n",
        (1 << CHARSET_WEIGHT), lpInObj->lfCharSet,
        lpSummary->dfCharSet));
        if (value > bestvalue)
            return value;
    }
  
    /* The facename check can take "some time" so check the pitch and
    family first, with the hope that these checks will rule out
    some fonts that would otherwise go through the facename process. */
  
    /*  pitch
    *
    *  no penalty if lfPitch or dfPitch == dontcare
    */
    if ((tmp = (lpInObj->lfPitchAndFamily & 0x03)) &&
        (tmp2 = tmPitchTOlfPitch(lpSummary->dfPitchAndFamily & 0x03)) &&
        !(tmp & tmp2))
    {
        value += 1 << PITCH_WEIGHT;
        DBGfontselect(("   %d: pitch %d to %d\n",
        (1 << PITCH_WEIGHT), (unsigned)tmp,
        (unsigned)tmPitchTOlfPitch(
        lpSummary->dfPitchAndFamily & 0x0F)));
        if (value > bestvalue)
            return value;
    }
  
    /*  family
    *
    *  no penalty if lfFamily or dfFamily == dontcare
    */
    if ((tmp = (lpInObj->lfPitchAndFamily & 0xF0)) &&
        (tmp2 = (lpSummary->dfPitchAndFamily & 0xF0)) &&
        (tmp != tmp2))
    {
        value += 1 << FAMILY_WEIGHT;
        DBGfontselect(("   %d: family %d to %d\n",
        (1 << FAMILY_WEIGHT), (unsigned)tmp,
        (unsigned)(lpSummary->dfPitchAndFamily & 0xF0)));
        if (value > bestvalue)
            return value;
    }
  
    /*  facename
    */
    DBGfontselect(("FONTSELECT: comparing '%ls' to '%ls'\n",
    (LPSTR)lpInObj->lfFaceName, dfFaceName));
  
    if ((lstrlen((LPSTR)lpInObj->lfFaceName) > 0) &&
        (lstrlen((LPSTR)dfFaceName) > 0))
    {
        /* We got valid face names, penalize if they do not match.
        */
        if (lstrcmpi((LPSTR)lpInObj->lfFaceName, dfFaceName))
        {
            if (aliasFace(lpInObj, dfFaceName))
            {
                value += 1;
                DBGfontselect(("   1: facename (alternate face)\n"));
            }
            else
            {
                value += 1 << FACENAME_WEIGHT;
                DBGfontselect(
                ("   %d: facename\n", (1 << FACENAME_WEIGHT)));
            }
        }
    }
    else
    {
        /* Penalize if we got zero string lengths, but do not penalize
        * if we are looking at a Courier font and the caller has
        * requested a fixed pitch font and no font family
        * (this ensures that the stock object will end up being Courier).
        */
        if (!lstrlen((LPSTR)dfFaceName) ||
            ((lpInObj->lfPitchAndFamily & 0xF3) !=
            (FIXED_PITCH | FF_DONTCARE)) ||
            !defaultFace(dfFaceName))
        {
            value += 1 << FACENAME_WEIGHT;
            DBGfontselect(
            ("   %d: NULL facename\n", (1 << FACENAME_WEIGHT)));
        }
    }
  
    if (value > bestvalue)
        return value;
  
  
    /*  height
    *
    * If zero height requested, default to 12 points (this guarantees
    * that we will select a 12 point font for the stock object).
    */
    if (!(tmp = lpInObj->lfHeight))
    {
        tmp = -50;
    }
  
    if (tmp < 0)
    {
        /*  The height we are asking for is negative, which means
        *  ignore internal leading.
        */
        tmp = -tmp + lpSummary->dfInternalLeading;
    }
  
    /*  Capture the difference in height.
    */
    /*** Tetra -- If font being checked is scal., then no penalty for height ***/
    if (scalfont)
        tmp = 0;
    else
        /*** Tetra end ***/
        tmp -= lpSummary->dfPixHeight;
  
    if (tmp > 0)
    {
        /*  The height we are asking for is greater than the
        *  height we have = small penalty.
        */
        value += tmp << HEIGHT_WEIGHT;
        DBGfontselect(("   %d: taller height %d to %d\n",
        (tmp << HEIGHT_WEIGHT), (tmp + lpSummary->dfPixHeight),
        lpSummary->dfPixHeight));
    }
    else
    {
        /*  The height we are asking for is less than the height
        *  we have = large penalty.
        */
        value += (-tmp) << LARGE_HEIGHT_WEIGHT;
        DBGfontselect(("   %d: shorter height %d to %d\n",
        ((-tmp) << LARGE_HEIGHT_WEIGHT),
        (tmp + lpSummary->dfPixHeight), lpSummary->dfPixHeight));
    }
  
    /*  width
    *
    *  no penalty if lfWidth or dfAvgWidth == dontcare
    */
    if ((tmp = lpInObj->lfWidth) &&
        (tmp2 = lpSummary->dfAvgWidth) &&
        (tmp -= tmp2) &&
        /*** Tetra begin -- If font is scalable, then no width penalty ***/
        (!(scalfont)))
        /*** Tetra end ***/
    {
        value += abs(tmp) << WIDTH_WEIGHT;
        DBGfontselect(("   %d: width %d to %d\n",
        (abs(tmp) << WIDTH_WEIGHT), (tmp + lpSummary->dfAvgWidth),
        lpSummary->dfAvgWidth));
    }
  
    /*  italic
    */
    if (lpInObj->lfItalic != lpSummary->dfItalic)
    {
        value += 1 << ITALIC_WEIGHT;
        DBGfontselect(("   %d: width %d to %d\n",
        (1 << ITALIC_WEIGHT), lpInObj->lfItalic,
        lpSummary->dfItalic));
    }
  
    /*  weight
    *
    *  no penalty if lfWeight or dfWeight == dontcare
    */
    if ((tmp = lpInObj->lfWeight) &&
        (tmp2 = lpSummary->dfWeight) &&
        (tmp -= tmp2))
    {
        value += abs(tmp) >> WEIGHT_WEIGHT;
        DBGfontselect(("   %d: weight %d to %d\n",
        (abs(tmp) >> WEIGHT_WEIGHT), (tmp + lpSummary->dfWeight),
        lpSummary->dfWeight));
    }
  
#ifdef DEBUG_FUNCT
    DB(("Exiting GetFontWeight\n"));
#endif
    return value;
}
  
/***********************************************************************
R E A L I Z E  O B J E C T
***********************************************************************/
  
short FAR PASCAL RealizeObject(
LPDEVICE lpDevice,
short Style,
LPLOGFONT lpInObj,
LPFONTINFO lpOutObj,
LPTEXTXFORM lpTextXForm)
{
    LPFONTSUMMARYHDR lpFontSummary;
    LPFONTSUMMARY lpSummary;
    LPSTR fontNameTable, dfFaceName, lpFace;
    unsigned long bestvalue, value;
    short ind, bestind, len;
    short res;
  
    /* TrueType variables */
    int fEngine = FALSE;
    static int gcbEngine;
    RASTCAPINFO RastCap;
  
#ifdef DEBUG_FUNCT
    DB(("Entering RealizeObject\n"));
#endif
    DBGentry(("RealizeObject(%lp,0x%x,%lp,%lp,%lp)\n",
    lpDevice, Style, lpInObj, lpOutObj, lpTextXForm));

#ifdef WIN31
  
    RastCap.nFlags = 0;
    if (GetVersion() >> 8)
        GetRasterizerCaps((LPRASTCAPINFO)&RastCap,(DWORD)sizeof(RASTCAPINFO));
#endif
  
    /*  We've been asked to delete the font, take no action but
    *  return success.
    */
    if (Style == -OBJ_FONT)
    {
        DBGrealize(("RealizeObject(): Style(%d) == -OBJ_FONT(%d)\n",
        Style, OBJ_FONT));
  
        /* For some wierd reason the font comes in as lpInObj.
        * Also check for Win 3.1
        */

#ifdef WIN31

        if ((RastCap.nFlags & CAPS_TTENABLED) &&
            (((LPFONTINFO)lpInObj)->dfType & 0x100))
            {
                /* its an engine font and not a graphics font
                 */
                if(!lpDevice->epTTRaster)
                    EngineDeleteFont((LPFONTINFO)lpInObj);
            }
#endif
  
        return (1);
    }
  
    // if it is not a font we pass it on to the brute.  if it is
    // a brush we store some of our own data with the pbrush
    // to do our halftone brush fills

    /* if it is a non-white pen, and it is hairline or width is 1,
     * make it black  BUG #788
     */
    if (Style == OBJ_PEN)
    {
        LOGPEN localpen = *(LPLOGPEN)lpInObj;
        if ((localpen.lopnWidth.xcoord == 0 || localpen.lopnWidth.xcoord == 1) &&
            (localpen.lopnColor != 0xFFFFFF))
            localpen.lopnColor = 0x00;
        res = dmRealizeObject(lpDevice->epType ? (LPDEVICE) & lpDevice->epBmpHdr : lpDevice,
              Style, (LPSTR)&localpen, (LPSTR)lpOutObj, (LPSTR)lpTextXForm);
        return (res);
    }
    else if (Style == OBJ_BRUSH)
    {
        res = dmRealizeObject(lpDevice->epType ? (LPDEVICE) &
        lpDevice->epBmpHdr : lpDevice, Style, (LPSTR)lpInObj,
        (LPSTR)lpOutObj, (LPSTR)lpTextXForm);
  
        if (Style == OBJ_BRUSH) {
  
            if (!lpOutObj) {
  
                display_pbrush_size = res;
  
                // tell GDI we need a bit more storage
  
                res += sizeof(PBRUSH);
            } else {
  
                // fill in the fields we will use
  
                LPPBRUSH lppbr;
                DWORD rgb;
                BYTE grey;
  
            #define lplbrush ((LOGBRUSH FAR *)lpInObj)
  
                lppbr = (LPPBRUSH)((LPSTR)lpOutObj + display_pbrush_size);
  
                if (lppbr->bMyBrush = (lplbrush->lbStyle == BS_SOLID)) {
  
                    rgb = lplbrush->lbColor;
  
                    grey = INTENSITY(GetRValue(rgb),
                    GetGValue(rgb),
                    GetBValue(rgb));
  
                    BuildHalftoneBrush(grey, (BYTE FAR *)lppbr->Pattern);
                }
            }
        }
        return res;
    }
    else if (Style != OBJ_FONT)
    {
        res = dmRealizeObject(lpDevice->epType ? (LPDEVICE) &
        lpDevice->epBmpHdr : lpDevice, Style, (LPSTR)lpInObj,
        (LPSTR)lpOutObj, (LPSTR)lpTextXForm);
        return (res);
    }

    if (!lpDevice->epType)
    {
        DBGerr(("RealizeObject(): !lpDevice->epType\n"));
  
        /* realizing a font for a memory DC.  Used to return 0, but that
        * causes GDI to try and give us a font.  So return a dumb one.
        */
  
        if (!lpOutObj)
            return CBMEMFONT;
  
        lpOutObj->dfFace = 0L;        /* 5/09/90  BUGFIX Offset to dfType*/
        lpOutObj->dfType = 0x80;        /* device */
        lpOutObj->dfPoints = 12;
        lpOutObj->dfVertRes = lpOutObj->dfHorizRes = 300;
  
        lpOutObj->dfWeight = 400;
        lpOutObj->dfPixWidth = 30;
        lpOutObj->dfPixHeight = 50;
        lpOutObj->dfPitchAndFamily = FF_MODERN;
        lpOutObj->dfAvgWidth = 30;
        lpOutObj->dfMaxWidth = 30;
        lpOutObj->dfFirstChar = 32;
        lpOutObj->dfLastChar = 255;
  
  
        lpOutObj->dfAscent =            // 0 int values
        lpOutObj->dfInternalLeading =
        lpOutObj->dfExternalLeading = 0;
  
        lpOutObj->dfDefaultChar =       // 0 char values
        lpOutObj->dfBreakChar =
        lpOutObj->dfItalic =
  
        lpOutObj->dfUnderline =
        lpOutObj->dfStrikeOut =
        lpOutObj->dfCharSet = (BYTE) 0;
  
        lpTextXForm->ftHeight = 50;
        lpTextXForm->ftWidth = 30;
  
        lpTextXForm->ftWeight = 400;
  
        lpTextXForm->ftOutPrecision = OUT_DEFAULT_PRECIS;
        lpTextXForm->ftClipPrecision = CLIP_DEFAULT_PRECIS;
  
        lpTextXForm->ftItalic =         // 0 byte values
        lpTextXForm->ftUnderline =
        lpTextXForm->ftStrikeOut = (BYTE) 0;
  
        lpTextXForm->ftEscapement =     // 0 word values
        lpTextXForm->ftOrientation =
  
        lpTextXForm->ftAccelerator =
        lpTextXForm->ftOverhang = 0;
  
        return CBMEMFONT;
    }
  
    /*  Cannot realize OEM character sets (vector fonts).
    */
    if (lpInObj->lfCharSet == OEM_CHARSET)
    {
        DBGerr(("RealizeObject(): OEM_CHARSET\n"));
        return (0);
    }
  
    /*  If the fontSummary structure is not there, cannot continue.
    */
    if (!(lpFontSummary = lockFontSummary(lpDevice)))
    {
        DBGerr(("RealizeObject(): could *not* lock fontSummary\n"));
        return (0);
    }
  
    /*  If there are no fonts in the fontSummary structure, then
    *  fail to realize every font.
    */
    if (!(len = lpFontSummary->len))
    {
        DBGerr(("RealizeObject(): no fonts in fontSummary, abort\n"));
        ind = 0;
        goto exit;
    }
  
    DBGrealize(("RealizeObject(): GetVersion: %d\n", GetVersion()));
  
    /*  If lpOutObj == 0 then the caller only wants the size of
    *  the struct.
    */
    if (!lpOutObj)
    {
        unlockFontSummary(lpDevice);
        ind = sizeof(PRDFONTINFO) + LF_FACESIZE + 1;

#ifdef WIN31

        if (lpDevice->epTTRaster)	// If we're NOT using TT as dev fonts.
        {
            gcbEngine = 0;
            return (ind);
        }

        if (RastCap.nFlags & CAPS_TTENABLED)
        {
            /* get the size of an engine font, plus our extra data.  Use the
             * maximum of the two
             */
            gcbEngine = EngineRealizeFont(lpInObj,lpTextXForm,NULL);

            if (gcbEngine)
            {
                if (gcbEngine + sizeof(TTFONTINFO) > ind)
                    ind = gcbEngine + sizeof(TTFONTINFO);
            }
        }
#endif
  
        return(ind);
    }
  
  
    bestvalue = 0xFFFFFFFFL;

  
#ifdef WIN31

    if (RastCap.nFlags & CAPS_TTENABLED)
    {
        lmemset((LPSTR)lpOutObj,0,gcbEngine); 

        /* If the font is rotated and dpi is 75 or 150, 
         * fail to realize a TT font. Fixes a bug in GDI where low
         * res TT printed as graphics screws up. - dtk 2/92
         */

        if ((gcbEngine) &&
            !((lpDevice->epScaleFac) &&
              (lpInObj->lfOrientation || lpInObj->lfEscapement)))
        {
            ind = EngineRealizeFont(lpInObj,lpTextXForm,lpOutObj);

            if(ind)
            {
                fEngine = TRUE;
                LVHWORD(lpOutObj->dfFace) = HWORD(lpOutObj);
                bestvalue = GetFontWeight(lpOutObj,lpInObj,lpTextXForm,
                (LPSTR)lpOutObj->dfFace,bestvalue,FALSE);
                bestind = 0x7FFF;
            }
        }
    }
#endif

    lpSummary = &lpFontSummary->f[0];
    fontNameTable = (LPSTR) &lpFontSummary->f[len];

    DBGrealize(("RealizeObject(): Searching for font, len=%d\n", len));

    /*  Traverse the list of fonts and locate the font with the
     *  lowest penalty value.
     */
    for (ind = 0; ind < len; ++ind, ++lpSummary)
    {
        dfFaceName = (LPSTR) &fontNameTable[lpSummary->indName];

        value = GetFontWeight((LPFONTINFO)&lpSummary->dfType,
        lpInObj,lpTextXForm,dfFaceName,bestvalue,
        lpSummary->scaleInfo.scalable);

        DBGfontselect(("   value=%ld, bestvalue=%ld\n",
        (unsigned long)value, (unsigned long)bestvalue));

        if (value <= bestvalue)
        {
            bestvalue = value;
            bestind = ind;

#ifdef WIN31

            if ((RastCap.nFlags & CAPS_TTENABLED) && (fEngine))
            {
                fEngine = FALSE;
                EngineDeleteFont(lpOutObj);
            }
#endif

        }
    }
  
#ifdef WIN31

    if ((RastCap.nFlags & CAPS_TTENABLED) && (fEngine))
        {
            lpOutObj->dfType |= TYPE_TRUETYPE;
            lpOutObj->dfBitsOffset = (DWORD)(LPSTR)lpOutObj + gcbEngine;
            ((LPTTFONTINFO)lpOutObj->dfBitsOffset)->idFont
            = ++lpDevice->epNextTTFont;
        }
        else
#endif
        {
  
            lpSummary = &lpFontSummary->f[bestind];
  
            DBGrealize(("RealizeObject(): font found at ind=%d, bestvalue=%ld\n",
            bestind, (unsigned long)bestvalue));
  
  
            /*  Get face name of selected font.
            */
            dfFaceName = (LPSTR) &fontNameTable[lpSummary->indName];
  
            /*  Init caller's struct.
            */
            ind = sizeof(PRDFONTINFO) + LF_FACESIZE + 1;
            lmemset((LPSTR)lpOutObj, 0, ind);
  
            /*  Get basic data from fontSummary.
            */
            ExtractFontInfo(lpOutObj, lpSummary);
            ((LPPRDFONTINFO)lpOutObj)->indFontSummary = bestind;
  
            /*** Tetra begin ***/
            /*** If font is scalable, then scale the PFM header info ***/
            ((LPPRDFONTINFO)lpOutObj)->scaleInfo.scalable
                = lpSummary->scaleInfo.scalable;
            ((LPPRDFONTINFO)lpOutObj)->scaleInfo.emMasterUnits
                = lpSummary->scaleInfo.emMasterUnits;

            if (lpSummary->scaleInfo.scalable)
            {
                /* if the height is zero then return a 12 pt font -dtk
                 */
                if (lpInObj->lfHeight == 0)
                {
                    lpOutObj->dfPoints = 12;
                    lpOutObj->dfAscent = 38;
                    lpOutObj->dfInternalLeading = 0;
                    lpOutObj->dfExternalLeading = 10;
                    lpOutObj->dfPixHeight = 50;
                    lpOutObj->dfPixWidth = 0;
                    lpOutObj->dfAvgWidth = 36;
                    lpOutObj->dfMaxWidth = 45;
                }
                else
                {
                    lpOutObj->dfPixWidth = lpInObj->lfWidth;

                    lpOutObj->dfPixHeight = (lpInObj->lfHeight < 0) ?
                                            -(lpInObj->lfHeight) :
                                            lpInObj->lfHeight;

                    lpOutObj->dfPoints = (short) labdivc ((long) lpOutObj->dfPixHeight,
                                        (long) 72,
                                        (long) lpOutObj->dfVertRes);

                    lpOutObj->dfAscent = ScaleVertical (lpOutObj->dfAscent,
                                        lpSummary->scaleInfo.emMasterUnits,
                                        lpOutObj->dfPixHeight);

                    lpOutObj->dfInternalLeading = ScaleVertical (lpOutObj->dfInternalLeading,
                                                lpSummary->scaleInfo.emMasterUnits,
                                                lpOutObj->dfPixHeight);

                    lpOutObj->dfExternalLeading = ScaleVertical (lpOutObj->dfExternalLeading,
                                                lpSummary->scaleInfo.emMasterUnits,
                                                lpOutObj->dfPixHeight);

                    lpOutObj->dfAvgWidth = ScaleWidth ((long) lpOutObj->dfAvgWidth,
                                        (long) lpSummary->scaleInfo.emMasterUnits,
                                        (long) lpOutObj->dfPixHeight,
                                        (long) lpOutObj->dfVertRes);

                    lpOutObj->dfMaxWidth = ScaleWidth ((long) lpOutObj->dfMaxWidth,
                                        (long) lpSummary->scaleInfo.emMasterUnits,
                                        (long) lpOutObj->dfPixHeight,
                                        (long) lpOutObj->dfVertRes);
                }
            }
            /*** Tetra end ***/
  
  
            /*  Load face name.
            */
            len = lstrlen(dfFaceName);
            if (len > LF_FACESIZE)
                len = LF_FACESIZE;
            lpFace = (LPSTR)lpOutObj + (lpOutObj->dfFace = sizeof(PRDFONTINFO));
            lmemcpy(lpFace,dfFaceName,len);     /* ending 0 set by lmemset() */
  
            /*  Chose symbol translation table.
            */
            ((LPPRDFONTINFO)lpOutObj)->symbolSet = lpSummary->symbolSet;
  
            /*  HACK for the Z cartridge -- the fonts on the Z cartridge
            *  are offset 0.017 inch to the right.  We have to detect that
            *  a font came from the Z cartridge and then adjust.
            */
            ((LPPRDFONTINFO)lpOutObj)->ZCART_hack = lpSummary->ZCART_hack;
  
            /*  HACK for typgraphic quotes -- for cartridges that use ECMA-94,
            *  we want to be able to switch out to USASCII to get better quotes.
            */
            ((LPPRDFONTINFO)lpOutObj)->QUOTE_hack = lpSummary->QUOTE_hack;
  
            /*  Set flag to indicate where the pfm file is, whether in
            *  resources or external PFM file.
            */
            ((LPPRDFONTINFO)lpOutObj)->isextpfm = (BOOL)(lpSummary->indPFMName > 0);
        } /* else ... (Not a TrueType font) */
  
    /*  Copy to textXform struct.
    */
    InfoToStruct(lpOutObj, HP_TEXTXFORM, (LPSTR)lpTextXForm);
    // Save the desired Orientation for later use in exttextout - sjc 7-20-90
    lpTextXForm->ftOrientation = lpInObj->lfOrientation;
  
    /*  Adjust textXform data.
    */
#ifdef SYM_BOLD
    if ((lpInObj->lfWeight >= FW_BOLD) && (lpOutObj->dfWeight <= FW_NORMAL))
    {
        /*  Simulated bold (always simulate FW_BOLD regardless of
        *  how much bolding was requested).
        */
        lpTextXForm->ftWeight = FW_BOLD;
  
        /*  Calculate overstrike amount based on point size.
        */
        lpTextXForm->ftOverhang = 1 + (lpOutObj->dfPixHeight -
        lpOutObj->dfInternalLeading) / TENPT_PIXHEIGHT;
  
        DBGrealize(
        ("RealizeObject(): simulated bold lfWeight=%d, ftOverhang=%d\n",
        lpInObj->lfWeight, lpTextXForm->ftOverhang));
    }
    else
    {
        /*  If not specifically bolding a font, then shut
        *  off the overhang stuff.
        */
        lpTextXForm->ftOverhang = 0;
    }
#endif
  
    lpTextXForm->ftUnderline = lpInObj->lfUnderline;
  
    /* TETRA -- removed non-Galaxy support */
    lpTextXForm->ftStrikeOut = lpInObj->lfStrikeOut;
  
    exit:
    unlockFontSummary(lpDevice);
    DBGrealize(("...end of RealizeObject, return %d\n", ind));
#ifdef DEBUG_FUNCT
    DB(("Exiting RealizeObject\n"));
#endif
    return (ind);
}
  
/***********************************************************************
E N U M  D  F O N T S
***********************************************************************/
  
/*  Enumerate the list of fonts available to the application.  This proc
*  keys itself on the value of lpFaceName:
*
*      1. if lpFaceName == NULL, then EnumDFonts lists the available
*         family names (Courier, Tms Rmn, Helv) and calls the callback
*         procedure with each family name.
*
*      2. if lpFacename == font family name, then EnumDFonts lists the
*         available faces within the family (Tms Rmn 10pt, Tms Rmn 12pt)
*         and calls the callback procedure with each font's logical and
*         textmetric information.
*
*  The application is expected to enumerate fonts in the following manner:
*
*      A. EnumDFonts is called with lpFaceName == NULL and a pointer to
*         callback function #1.  EnumDFonts calls callback function #1
*         with each font family name.
*
*      B. Callback function #1 calls EnumDFonts with lpFaceName == font
*         family name and a pointer to callback function #2.  EnumDFonts
*         calls callback function #2 with each fonts' logical and
*         textmetric information.
*
*      C. Callback function #2 stashes the font information into a data
*         structure the application uses during execution.
*/
  
//near PASCAL NullFaceName()
//{
//}
  
/*Tetra*/
/* List of pointsizes to enumerate for scalable fonts.  Point sizes are */
/* expressed in pixels:                                                 */
/*    25 (6pt), 33 (8pt), 46 (11pt), 50 (12pt), 58 (14pt), 75 (18pt),   */
/*     100 (24pt), 125 (30pt), 150 (36pt), 200 (48pt).                  */
/* A 10 point font is always returned so it is not in                   */
/*   this list.  These fonts are only listed when enumerating           */
/*   a specific typeface.                                               */
  
#define numPoints  10
short PointList[numPoints] = {25,33,46,50,58,75,100,125,150,200};
  
  
int far PASCAL EnumDFonts(lpDevice, lpFaceName, lpCallbackFunc, lpClientData)
LPDEVICE lpDevice;
LPSTR lpFaceName;
FARPROC lpCallbackFunc;
long lpClientData;
{
    LPFONTSUMMARYHDR lpFontSummary;
    LPFONTSUMMARY lpSummary;
    TEXTMETRIC TextMetric;
    FONTINFO fontInfo;
    LOGFONT LogFont;
    LPSTR fontNameTable, lpRefName;
    short status, ind, len, namelen, indPrevName = -1;
  
    RASTCAPINFO RastCap;
  
    /*** Tetra II Samna bugfix begin ***/
    typedef struct {
        short int dfAscent;
        short int dfInternalLeading;
        short int dfExternalLeading;
        short int dfAvgWidth;
        short int dfMaxWidth;
    } SCALESTORE;
  
    SCALESTORE scaleStore;
    /*** Tetra II Samna bugfix end ***/
  
  
    DBGenumfonts(("EnumDFonts(%lp,%lp,%lp,%lp)\n",
    lpDevice, lpFaceName, lpCallbackFunc, lpClientData));
  
    /*  Cannot enumerate for anything but a LaserJet DC.
    */
#ifdef DEBUG_FUNCT
    DB(("Entering EnumDFonts\n"));
#endif
    if (!lpDevice->epType)
    {
        DBGerr(("EnumDFonts(): !lpDevice->epType\n"));
        return (1);
    }
  
#ifdef WIN31

    RastCap.nFlags = 0;
    if (GetVersion() >> 8)
        GetRasterizerCaps((LPRASTCAPINFO)&RastCap,(DWORD)sizeof(RASTCAPINFO));
  

    if (RastCap.nFlags & CAPS_TTENABLED)
        {
            /* call the engine font enumerator
            */
            if ( ! lpDevice->epTTRaster )  
                if (!(status = EngineEnumerateFont(lpFaceName,lpCallbackFunc,
                    lpClientData)))
                {
                    DBGerr(("EnumDFonts(): EngineEnumerateFont failed\n"));
                    return (0);
                }
  
                DBGerr(("EnumDFonts(): EngineEnumerateFont == %d\n", status));
        }
#endif
  
    /*  If the fontSummary structure is not there, cannot continue.
    */
    if (!(lpFontSummary = lockFontSummary(lpDevice)))
    {
        DBGerr(("EnumDFonts(): could *not* lock fontSummary\n"));
        return (1);
    }
  
    /*  If a face name was passed in, get its length (used to indicate
    *  that facename exists).
    */
    if (lpFaceName && *lpFaceName)
    {
        DBGenumfonts(("EnumDFonts(): FaceName=%ls\n", lpFaceName));
        namelen = lstrlen (lpFaceName);
    }
    else
    {
        // lpFaceName was NULL or pointed to NULL string
        //NullFaceName();       // for debug
        namelen = 0;
    }
  
    lpSummary = &lpFontSummary->f[0];
    len = lpFontSummary->len;
    fontNameTable = (LPSTR) &lpFontSummary->f[len];
  
    for (status = 1, ind = 0; ind < len; ind++, lpSummary++)
    {
        lpRefName = (LPSTR) &fontNameTable[lpSummary->indName];
  
        /* a facename was specified
        */
        if ((namelen && !lstrcmpi(lpFaceName,lpRefName)) ||
  
            /* no face name specified
            */
            (!namelen && (lpSummary->indName != indPrevName)))
        {
            indPrevName = lpSummary->indName;
            lmemset((LPSTR)&fontInfo, 0, sizeof(FONTINFO));
            ExtractFontInfo((LPFONTINFO)&fontInfo, lpSummary);
  
            /*** If font is scalable, then scale the PFM header info ***/
            if (lpSummary->scaleInfo.scalable)
            {
                /* ???
                lpOutObj->dfPixWidth = lpInObj->lfWidth;
                */
                fontInfo.dfPixHeight = 42;       /* 10 point */
                fontInfo.dfPoints = (short)labdivc((long)fontInfo.dfPixHeight,
                (long) 72,
                (long) fontInfo.dfVertRes);
  
                /*** Samna bugfix begin ***/
                /*** Save original values to be used when scaling ***/
                scaleStore.dfAscent = fontInfo.dfAscent;
                scaleStore.dfInternalLeading = fontInfo.dfInternalLeading;
                scaleStore.dfExternalLeading = fontInfo.dfExternalLeading;
                scaleStore.dfAvgWidth = fontInfo.dfAvgWidth;
                scaleStore.dfMaxWidth = fontInfo.dfMaxWidth;
  
                /*** Changed calls from ScaleWidth to ScaleVertical ***/
                fontInfo.dfAscent = ScaleVertical (scaleStore.dfAscent,
                lpSummary->scaleInfo.emMasterUnits,
                fontInfo.dfPixHeight);
                fontInfo.dfInternalLeading =
                ScaleVertical (scaleStore.dfInternalLeading,
                lpSummary->scaleInfo.emMasterUnits,
                fontInfo.dfPixHeight);
                fontInfo.dfExternalLeading = ScaleVertical
                (scaleStore.dfExternalLeading,
                lpSummary->scaleInfo.emMasterUnits,
                fontInfo.dfPixHeight);
                fontInfo.dfAvgWidth = ScaleWidth ((long) scaleStore.dfAvgWidth,
                (long) lpSummary->scaleInfo.emMasterUnits,
                (long) fontInfo.dfPixHeight,
                (long) fontInfo.dfVertRes);
                fontInfo.dfMaxWidth = ScaleWidth ((long) scaleStore.dfMaxWidth,
                (long) lpSummary->scaleInfo.emMasterUnits,
                (long) fontInfo.dfPixHeight,
                (long) fontInfo.dfVertRes);
                DBGenumfonts(("EnumFonts: AvgWidth= %d, MaxWidth= %d\n",
                fontInfo.dfAvgWidth, fontInfo.dfMaxWidth));
                /*** Samna bugfix end ***/
            }
  
            InfoToStruct((LPFONTINFO)&fontInfo, HP_LOGFONT, (LPSTR) &LogFont);
            InfoToStruct(
            (LPFONTINFO)&fontInfo, HP_TEXTMETRIC, (LPSTR) &TextMetric);
            /* TETRA -- changed lmemcpy to lstrncpy -- KLO */
            lstrncpy(LogFont.lfFaceName, lpRefName,
            sizeof(LogFont.lfFaceName)-1);
  
        #ifdef LOCAL_DEBUG
            if (!namelen) {
                short dbg;
                DBGenumfonts(
                ("EnumDFonts(): no face specified, returning family "));
                for (dbg=0; (LogFont.lfFaceName[dbg] != '\0'); ++dbg) {
                DBGenumfonts(("%c", LogFont.lfFaceName[dbg])); }
                DBGenumfonts(("\n"));
            }
            else {
                short dbg;
                DBGenumfonts(
                ("EnumDFonts(): face requested=%ls\n", lpFaceName));
                DBGenumfonts(("              returning font within face "));
                for (dbg=0; (LogFont.lfFaceName[dbg] != '\0'); ++dbg) {
                DBGenumfonts(("%c", LogFont.lfFaceName[dbg])); }
                DBGenumfonts(("\n"));
            }
        #endif
  
            /*** Samna bugfix begin ***/
            /*** OR with 0 if scalable, RASTER_FONTTYPE if bitmap ***/
            DBGenumfonts(("EnumDFonts(): Calling callback function...\n"));
            if ((status = (*lpCallbackFunc)((LPSTR) &LogFont,
                (LPSTR) &TextMetric, (DEVICE_FONTTYPE |
                (lpSummary->scaleInfo.scalable ?
                0 :
                RASTER_FONTTYPE)
                ),
                lpClientData)) == 0)
            {
                DBGenumfonts((
                "EnumDFonts(): Callback function returned 0, breaking FOR\n"));
                break;
            }
            DBGenumfonts(("EnumDFonts(): Callback function returned %d\n", status));
            /*** Samna bugfix end ***/
  
  
            /* Send callback fn the rest of the info for display ptsizes for */
            /* scalable fonts.                                               */
  
            if (lpSummary->scaleInfo.scalable && namelen)
            {
                short count;
                for (count = 0; count < numPoints; count++)
                {
                    fontInfo.dfPixHeight = PointList[count];
                    DBGenumfonts(("EnumFonts: dfPixHeight= %d\n",
                    fontInfo.dfPixHeight));
  
                    DBGrealize(("Setting lpOutObj to return fonts with"));
                    DBGrealize(("dfVertRes = %d,    and emMasterUnits = %d\n",
                    fontInfo.dfVertRes,lpSummary->scaleInfo.emMasterUnits));
                    fontInfo.dfPoints = (short)labdivc((long)fontInfo.dfPixHeight,
                    (long)72,
                    (long)fontInfo.dfVertRes);
  
                    /*** Samna bugfix begin ***/
                    /*** Use original values to scale from ***/
  
                    /*** Changed calls from ScaleWidth to ScaleVertical ***/
                    fontInfo.dfAscent = ScaleVertical (scaleStore.dfAscent,
                    lpSummary->scaleInfo.emMasterUnits,
                    fontInfo.dfPixHeight);
                    fontInfo.dfInternalLeading = ScaleVertical (scaleStore.dfInternalLeading,
                    lpSummary->scaleInfo.emMasterUnits,
                    fontInfo.dfPixHeight);
                    fontInfo.dfExternalLeading = ScaleVertical (scaleStore.dfExternalLeading,
                    lpSummary->scaleInfo.emMasterUnits,
                    fontInfo.dfPixHeight);
  
                    DBGenumfonts(("EnumFonts: AvgWidth= %d, MaxWidth= %d\n",
                    fontInfo.dfAvgWidth, fontInfo.dfMaxWidth));
  
                    fontInfo.dfAvgWidth = ScaleWidth ((long) scaleStore.dfAvgWidth,
                    (long) lpSummary->scaleInfo.emMasterUnits,
                    (long) fontInfo.dfPixHeight,
                    (long) fontInfo.dfVertRes);
                    fontInfo.dfMaxWidth = ScaleWidth ((long) scaleStore.dfMaxWidth,
                    (long) lpSummary->scaleInfo.emMasterUnits,
                    (long) fontInfo.dfPixHeight,
                    (long) fontInfo.dfVertRes);
  
                    DBGenumfonts(("EnumFonts: AvgWidth= %d, MaxWidth= %d\n",
                    fontInfo.dfAvgWidth, fontInfo.dfMaxWidth));
  
                    InfoToStruct((LPFONTINFO)&fontInfo, HP_LOGFONT,
                    (LPSTR) &LogFont);
                    InfoToStruct((LPFONTINFO)&fontInfo, HP_TEXTMETRIC,
                    (LPSTR) &TextMetric);
                    lmemcpy(LogFont.lfFaceName, lpRefName,
                    sizeof(LogFont.lfFaceName)-1);
  
            #ifdef LOCAL_DEBUG
                    {
                        short dbg;
  
                        DBGenumfonts(("EnumDFonts(): face requested=%ls\n", lpFaceName));
                        DBGenumfonts(("              returning font within face "));
                        for (dbg=0; (LogFont.lfFaceName[dbg] != '\0'); ++dbg) {
                        DBGenumfonts(("%c", LogFont.lfFaceName[dbg])); }
                        DBGenumfonts((", %d pts", fontInfo.dfPoints));
                        DBGenumfonts(("\n"));
                    }
            #endif
  
                    DBGenumfonts(("EnumDFonts(): Calling callback function...\n"));
                    if ((status = (*lpCallbackFunc)((LPSTR) &LogFont,
                        (LPSTR) &TextMetric, DEVICE_FONTTYPE,
                        lpClientData)) == 0)
                    {
                        DBGenumfonts((
                        "EnumDFonts(): Callback function returned 0, breaking FOR\n"));
                        break;
                    }
                    DBGenumfonts(("EnumDFonts(): Callback function returned %d\n", status));
  
                }       /* for (count < numpoints)  */
            }       /* if (scalable && namelen) */
        }           /* if (...)                 */
  
        /* Did we break from the scalable for loop? If so, do it again */
        if (status == 0)
        {
            DBGenumfonts(("EnumDFonts(): Callback fn returned 0, breaking FOR\n"));
            break;
        }
  
    }           /* for (index < len)        */
  
    #ifdef LOCAL_DEBUG
    if (indPrevName == -1) {
        DBGenumfonts(("EnumDFonts(): Could *not* find facename, return %d\n",
        status));
    }
    #endif
  
    unlockFontSummary(lpDevice);
  
    DBGenumfonts(("...end of EnumDFonts, return %d\n", status));
#ifdef DEBUG_FUNCT
    DB(("Exiting EnumDFonts\n"));
#endif
    return status;
}
  
/***********************************************************************
I N F O  T O  S T R U C T
***********************************************************************/
  
void InfoToStruct(lpFont, style, info)
LPFONTINFO lpFont;
short style;
LPSTR info;
{
#ifdef DEBUG_FUNCT
    DB(("Entering InfoToStruct\n"));
#endif
    DBGinfotostruct(("InfoToStruct(%lp,%d,%lp)\n", lpFont, style, info));
  
    switch (style)
    {
        case HP_LOGFONT:
            ((LPLOGFONT)info)->lfHeight = lpFont->dfPixHeight;
            ((LPLOGFONT)info)->lfWidth = lpFont->dfAvgWidth;
            ((LPLOGFONT)info)->lfEscapement = 0;
            ((LPLOGFONT)info)->lfOrientation = 0;
            ((LPLOGFONT)info)->lfUnderline = lpFont->dfUnderline;
            ((LPLOGFONT)info)->lfStrikeOut = lpFont->dfStrikeOut;
            ((LPLOGFONT)info)->lfCharSet = lpFont->dfCharSet;
            ((LPLOGFONT)info)->lfOutPrecision = OUT_CHARACTER_PRECIS;
            ((LPLOGFONT)info)->lfClipPrecision = CLIP_CHARACTER_PRECIS;
            ((LPLOGFONT)info)->lfQuality = DEFAULT_QUALITY;
            ((LPLOGFONT)info)->lfPitchAndFamily =
            (lpFont->dfPitchAndFamily & 0xF0) +
            tmPitchTOlfPitch(lpFont->dfPitchAndFamily & 0x0F);
            ((LPLOGFONT)info)->lfItalic = lpFont->dfItalic;
            ((LPLOGFONT)info)->lfWeight = lpFont->dfWeight;
            break;
        case HP_TEXTXFORM:
            ((LPTEXTXFORM)info)->ftHeight = lpFont->dfPixHeight;
            ((LPTEXTXFORM)info)->ftWidth = lpFont->dfAvgWidth;
            ((LPTEXTXFORM)info)->ftEscapement = 0;
            ((LPTEXTXFORM)info)->ftOrientation = 0;
            ((LPTEXTXFORM)info)->ftWeight = lpFont->dfWeight;
            ((LPTEXTXFORM)info)->ftItalic = lpFont->dfItalic;
            ((LPTEXTXFORM)info)->ftOutPrecision = OUT_CHARACTER_PRECIS;
            ((LPTEXTXFORM)info)->ftClipPrecision = CLIP_CHARACTER_PRECIS;
#ifdef WIN31

            if (!(lpFont->dfType & TYPE_TRUETYPE))
                ((LPTEXTXFORM)info)->ftAccelerator = TC_OP_CHARACTER;
#else
            ((LPTEXTXFORM)info)->ftAccelerator = TC_OP_CHARACTER;

#endif
            ((LPTEXTXFORM)info)->ftOverhang = OVERHANG;
            break;
        case HP_TEXTMETRIC:
            ((LPTEXTMETRIC)info)->tmHeight = lpFont->dfPixHeight;
            ((LPTEXTMETRIC)info)->tmAscent = lpFont->dfAscent;
            ((LPTEXTMETRIC)info)->tmDescent = lpFont->dfPixHeight -
            lpFont->dfAscent;
            ((LPTEXTMETRIC)info)->tmInternalLeading =
            lpFont->dfInternalLeading;
            ((LPTEXTMETRIC)info)->tmExternalLeading =
            lpFont->dfExternalLeading;
            ((LPTEXTMETRIC)info)->tmAveCharWidth = lpFont->dfAvgWidth;
            ((LPTEXTMETRIC)info)->tmMaxCharWidth = lpFont->dfMaxWidth;
            ((LPTEXTMETRIC)info)->tmItalic = lpFont->dfItalic;
            ((LPTEXTMETRIC)info)->tmWeight = lpFont->dfWeight;
            ((LPTEXTMETRIC)info)->tmUnderlined = lpFont->dfUnderline;
            ((LPTEXTMETRIC)info)->tmStruckOut = lpFont->dfStrikeOut;
            ((LPTEXTMETRIC)info)->tmFirstChar = lpFont->dfFirstChar;
            ((LPTEXTMETRIC)info)->tmLastChar = lpFont->dfLastChar;
            ((LPTEXTMETRIC)info)->tmDefaultChar = lpFont->dfDefaultChar +
            lpFont->dfFirstChar;
            ((LPTEXTMETRIC)info)->tmBreakChar = lpFont->dfBreakChar +
            lpFont->dfFirstChar;
            ((LPTEXTMETRIC)info)->tmPitchAndFamily = lpFont->dfPitchAndFamily;
            ((LPTEXTMETRIC)info)->tmCharSet = lpFont->dfCharSet;
            ((LPTEXTMETRIC)info)->tmOverhang = OVERHANG;
            ((LPTEXTMETRIC)info)->tmDigitizedAspectX = VDPI;
            ((LPTEXTMETRIC)info)->tmDigitizedAspectY = HDPI;
    }
#ifdef DEBUG_FUNCT
    DB(("Exiting InfoToStruct\n"));
#endif
}
  
/***********************************************************************
E X T R A C T  F O N T  I N F O
***********************************************************************/
  
/*  Copy the metric information stored in the font summary structure
*  into a FONTINFO structure.
*/
  
void ExtractFontInfo(lpFont, lpSummary)
LPFONTINFO lpFont;
LPFONTSUMMARY lpSummary;
{
#ifdef DEBUG_FUNCT
    DB(("Entering ExtractFontInfo\n"));
#endif
    DBGextractfont(("ExtractFontInfo(%lp,%lp)\n", lpFont, lpSummary));
  
    lpFont->dfType              = lpSummary->dfType;
    lpFont->dfPoints            = lpSummary->dfPoints;
    lpFont->dfVertRes           = lpSummary->dfVertRes;
    lpFont->dfHorizRes          = lpSummary->dfHorizRes;
    lpFont->dfAscent            = lpSummary->dfAscent;
    lpFont->dfInternalLeading   = lpSummary->dfInternalLeading;
    lpFont->dfExternalLeading   = lpSummary->dfExternalLeading;
    lpFont->dfItalic            = lpSummary->dfItalic;
    lpFont->dfUnderline         = lpSummary->dfUnderline;
    lpFont->dfStrikeOut         = lpSummary->dfStrikeOut;
    lpFont->dfWeight            = lpSummary->dfWeight;
    lpFont->dfCharSet           = lpSummary->dfCharSet;
    lpFont->dfPixWidth          = lpSummary->dfPixWidth;
    lpFont->dfPixHeight         = lpSummary->dfPixHeight;
    lpFont->dfPitchAndFamily    = lpSummary->dfPitchAndFamily;
    lpFont->dfAvgWidth          = lpSummary->dfAvgWidth;
    lpFont->dfMaxWidth          = lpSummary->dfMaxWidth;
    lpFont->dfFirstChar         = lpSummary->dfFirstChar;
    lpFont->dfLastChar          = lpSummary->dfLastChar;
    lpFont->dfDefaultChar       = lpSummary->dfDefaultChar;
    lpFont->dfBreakChar         = lpSummary->dfBreakChar;
#ifdef DEBUG_FUNCT
    DB(("Exiting ExtractFontInfo\n"));
#endif
}
  
/***********************************************************************
A L I A S  F A C E
***********************************************************************/
  
BOOL
aliasFace(LPLOGFONT lpLogFont, LPSTR lpFace) {
  
    extern void FT_NumFaces(void);  /* slimy hack to reference data */
    extern void FT_FaceTable(void); /*  via far ptr in code segment */
  
    int one_in_set = -1, two_in_set = -1;
    LPSTR f, lpLogFace = lpLogFont->lfFaceName;
    WORD set, fam, numfaces, family = (lpLogFont->lfPitchAndFamily & 0xF0);
  
  
#ifdef DEBUG_FUNCT
    DB(("Entering aliasFace\n"));
#endif
    DBGface(("aliasFace(%lp,%lp): %ls, %ls\n",
    lpLogFont, lpFace, (LPSTR)lpLogFont->lfFaceName, lpFace));
  
    numfaces = *(f = (LPSTR)FT_NumFaces);
    f = (LPSTR)FT_FaceTable;
  
    while (numfaces--) {
  
        fam = *f++;
        set = *f++;
  
        DBGface(("aliasFace(): num=%d, set=%d, fam=%d, 1:%d, 2:%d, %ls\n",
        numfaces, set, fam, one_in_set, two_in_set, f));
  
        if (fam == family) {
  
            DBGface(("aliasFace(): families match\n"));
  
            if (lstrcmpi(f, lpLogFace) == 0) {
  
                DBGface(("                 ...first face matches\n"));
  
                if (two_in_set == (one_in_set = set)) {
  
                    DBGface(("                 ...both in the set\n"));
                    break;
                }
            }
  
            if (lstrcmpi(f, lpFace) == 0) {
  
                DBGface(("                 ...second face matches\n"));
  
                if (one_in_set == (two_in_set = set)) {
  
                    DBGface(("                 ...both in the set\n"));
                    break;
                }
            }
        }
  
        /*  Advance to next face.
        */
        while (*f++)
            ;
    }
  
#ifdef DEBUG_FUNCT
    DB(("Exiting aliasFace\n"));
#endif
    return (one_in_set > -1 && two_in_set == one_in_set);
}
  
/***********************************************************************
D E F A U L T  F A C E
***********************************************************************/
  
BOOL
defaultFace(LPSTR lpFace) {
  
    BOOL match;
    extern void FT_DefaultFace(void);       /* hack for data in code seg */
  
    DBGface(("defaultFace(%lp): %ls", lpFace, lpFace));
  
    match = (lstrcmpi((LPSTR)FT_DefaultFace, lpFace) == 0);
  
    DBGface(("  %ls\n",match ? (LPSTR)"*match*" : (LPSTR)"*failed*"));
  
    return (match);
}
  
