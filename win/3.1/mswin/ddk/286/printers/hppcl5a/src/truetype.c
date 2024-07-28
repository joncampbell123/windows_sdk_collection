/**[f******************************************************************
* truetype.c -
*
*  Utility functions for TrueType support
*
* Copyright (C) 1990 Microsoft Corporation.
* Copyright (C) 1990, 1991 Hewlett-Packard Company.
* All rights reserved.
* Company confidential.
*
**f]*****************************************************************/
  
/*
* $Header: truetype.c,v 3.890 92/02/06 16:12:28 dtk FREEZE $
*/
  
/*
* $Log:	truetype.c,v $
 * Revision 3.890  92/02/06  16:12:28  16:12:28  dtk (Doug Kaltenecker)
 * Win3.1 Freeze
 * 
 * Revision 3.871  91/12/02  16:46:28  16:46:28  dtk (Doug Kaltenecker)
 * Changed the ifdef TTs to WIN31s.
 * 
 * Revision 3.871  91/11/22  13:19:37  13:19:37  dtk (Doug Kaltenecker)
 * Win 3.1 Post Beta 3 version.
 * 
 * Revision 3.870  91/11/08  11:44:13  11:44:13  dtk (Doug Kaltenecker)
 * 3.1 Release Candidate 1
 * 
 * Revision 3.865  91/11/01  13:52:11  13:52:11  dtk (Doug Kaltenecker)
 * Beta release for Windows 3.1
 * 
 * Revision 3.862  91/10/25  13:47:31  13:47:31  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.860  91/10/23  09:48:59  09:48:59  dtk (Doug Kaltenecker)
 * WinWird Release
 * 
 * Revision 3.852  91/10/09  14:59:59  14:59:59  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.850  91/10/04  16:50:14  16:50:14  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE
 * 
 * Revision 3.840  91/09/28  14:17:30  14:17:30  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE 3 FOR WINWORD
 * 
 * Revision 3.830  91/09/18  16:33:46  16:33:46  dtk (Doug Kaltenecker)
 * RELEASE
 * 
 * Revision 3.822  91/09/16  10:34:30  10:34:30  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE 2 FOR WIN30.
 * 
 * Revision 3.820  91/09/06  14:12:26  14:12:26  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE FOR WINWORD 2.0
 * 
 * Revision 3.814  91/09/04  14:03:32  14:03:32  dtk (Doug Kaltenecker)
 * Uncommented myWriteEscape because it is still used - oops.
 * 
 * Revision 3.813  91/09/04  11:46:02  11:46:02  dtk (Doug Kaltenecker)
 * Moved the local #define TT to build.h and included it.
 * 
 * Revision 3.812  91/08/22  14:32:21  14:32:21  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.807  91/08/08  10:31:38  10:31:38  dtk (Doug Kaltenecker)
 * PREBETA3
 * 
 * Revision 3.804  91/08/06  09:17:48  09:17:48  dtk (Doug Kaltenecker)
 * changed allocation for ttfsum from currentTTfont to nextTTfont.
 * 
 * Revision 3.803  91/08/02  15:50:27  15:50:27  dtk (Doug Kaltenecker)
 * Added call to Unload softs when we realize we could run out of memory
 * with the next font. Done by claculating FudgeFact.
 * 
 * Revision 3.802  91/07/22  12:54:41  12:54:41  oakeson (Ken Oakeson)
 * BETA
 * 
 * Revision 3.800  91/07/21  12:37:20  12:37:20  dtk (Doug Kaltenecker)
 * changed tt symbolset and typeface numbers
 * 
 * Revision 3.799  91/07/02  16:51:55  16:51:55  daniels (Susan Daniels)
 * Saving Ken's fix for GP fault.  Freezing for windows 3.1 beta also.
 * 
 * Revision 3.800  91/07/02  16:43:34  16:43:34  oakeson (Ken Oakeson)
 * Fixed mem allocing problem with TT mem management
 * 
 * Revision 3.799  91/07/02  11:52:03  11:52:03  daniels (Susan Daniels)
 * Beta
 * 
 * Revision 3.798  91/07/02  10:40:40  10:40:40  dtk (Doug Kaltenecker)
 * Screwed up and checked in the wrong file last time
 * 
 * Revision 3.790  91/06/11  16:03:39  16:03:39  stevec (Steve Claiborne)
 * Freeze
 * 
 * Revision 3.787  91/06/11  15:44:43  15:44:43  dtk (Doug Kaltenecker)
 * Prettified files!
 * 
* Revision 3.786  91/06/11  10:08:59  10:08:59  oakeson (Ken Oakeson)
* Removed sign bit from a few vars and forced an engine set context call
* every time SelectTTFont is called.
*
* Revision 3.785  91/05/22  14:57:15  14:57:15  stevec (Steve Claiborne)
* Beta version to MS
*
* Revision 3.781  91/05/22  13:48:16  13:48:16  oakeson (Ken Oakeson)
* Fixed dwSize overflow
*
* Revision 3.780  91/05/15  15:57:25  15:57:25  stevec (Steve Claiborne)
* Beta
*
* Revision 3.776  91/05/09  10:54:25  10:54:25  oakeson (Ken Oakeson)
* Set correct HMI and Last Char values in font header.
* Avoid downloading space char and char.s with zero width
*
* Revision 3.775  91/04/05  14:31:18  14:31:18  stevec (Steve Claiborne)
* Beta release to MS
*
* Revision 3.771  91/04/05  14:13:22  14:13:22  daniels (Susan Daniels)
* Has Ken's fix for the landscape truetype font bug.
*
* Revision 3.760  91/03/12  07:53:03  07:53:03  stevec (Steve Claiborne)
* Maintance release
*
* Revision 3.755  91/03/03  07:46:22  07:46:22  stevec (Steve Claiborne)
* March 3 Freeze
*
* Revision 3.721  91/03/01  13:42:56  13:42:56  stevec (Steve Claiborne)
* Modified code to handle memory allocation and locking more sanely.
*
* Revision 3.720  91/02/11  09:15:40  09:15:40  stevec (Steve Claiborne)
* Aldus version
*
* Revision 3.711  91/02/08  16:26:48  16:26:48  stevec (Steve Claiborne)
* Added debuging
*
* Revision 3.710  91/02/04  15:47:56  15:47:56  stevec (Steve Claiborne)
* Aldus freeze
*
* Revision 3.700  91/01/19  09:00:37  09:00:37  stevec (Steve Claiborne)
* Release
*
* Revision 3.685  91/01/14  15:43:32  15:43:32  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.681  91/01/14  10:17:49  10:17:49  stevec (Steve Claiborne)
* Updated the copy right stmt.
*
* Revision 3.680  91/01/10  16:16:54  16:16:54  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.671  90/12/20  12:03:16  12:03:16  stevec (Steve Claiborne)
* Put ifdef around all truetype stuff but left default truetype defined.
* This fixes bug #105.  SJC 12-20-90
*
* Revision 3.670  90/12/14  14:54:21  14:54:21  stevec (Steve Claiborne)
* freeze for 12-14-90 ver. 3.670
*
* Revision 3.665  90/12/10  15:36:04  15:36:04  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.661  90/12/10  10:04:52  10:04:52  stevec (Steve Claiborne)
* Removed some unreferenced local variables.  SJC
*
* Revision 3.660  90/12/07  14:50:33  14:50:33  stevec (Steve Claiborne)
* Freeze 12-7-90
*
* Revision 3.650  90/11/30  08:12:27  08:12:27  stevec (Steve Claiborne)
* Freeze 3.650, 11-30-90
*
* Revision 1.3  90/10/25  17:17:42  17:17:42  oakeson (Ken Oakeson)
* Removed #ifdef TT_FONTKIT
*
* Revision 1.2  90/08/24  11:38:02  11:38:02  daniels (Susan Daniels)
* message.txt
*
* Revision 1.1  90/08/14  15:36:15  15:36:15  oakeson (Ken Oakeson)
* Initial revision
*
*/
  
//#define DEBUG
//#define DEBUG_FUNCT
  
#include "generic.h"
#include "truetype.h"
#include "resource.h"
#include "fontman.h"
#include "utils.h"
#include "build.h"


/*  utilities
*/
#include "lockfont.c"

extern WORD __AHINCR;
#define ahincr  ((WORD)(&__AHINCR))
  
#ifdef DEBUG
#define DBGtt(msg) DBMSG(msg)
#else
#define DBGtt(msg) /* DBMSG(msg) */
#endif
extern WORD FAR PASCAL wvsprintf(LPSTR,LPSTR,LPINT);
extern BOOL FAR PASCAL UnloadSofts(LPDEVICE,LPFONTSUMMARYHDR,long);


/**************************************************************************
***************************************************************************/

void NEAR PASCAL myWriteEscape(LPDEVICE lpDevice, LPSTR lpsz, WORD n)
{
    char sz[32];
    int cb;

        cb = wvsprintf(sz,lpsz,&n);
        myWrite(lpDevice,sz,cb);
}


/**************************************************************************
***************************************************************************/

void FAR PASCAL SelectTTFont(LPDEVICE lpDevice, LPFONTINFO lpFont)
{
    LPTTFONTINFO lpttfi;
    static char escInit[] = {0x1B,')','s','6','4','W'};
    PCLHEADER ph;
    LPTTFSUM lpTTFSum = 0;
    DWORD dwCurntFSsz;
    WORD ttid, FudgeFact;
    int retvalue=TRUE;
    LPFONTSUMMARYHDR lpFontSummary;
    ESCtype escape;             /* printer escape */
  
#ifdef DEBUG_FUNCT
    DBMSG(("Entering SelectTTFont\n"));
#endif

    lpttfi = (LPTTFONTINFO)lpFont->dfBitsOffset;

    /* remember to set context if we download a glyph
    */
    lpFont->dfType |= TYPE_SETCONTEXT;
  
//  added to allow for memory tracking of TT fonts - dtk

    dwCurntFSsz = lmul((long)sizeof(TTFSUM), (long)(lpDevice->epNextTTFont - TTBASE));

    /* allocate and lock down the TT font sum header struct
     * if it's already allocated and too small, reallocate it. 
     */
    if (!lpDevice->epTTFSum)
        lpDevice->epTTFSum = GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,
		(dwCurntFSsz > (DWORD)DEFTTSZ) ? dwCurntFSsz: (DWORD)DEFTTSZ);
    else
        if ((DWORD)DEFTTSZ < dwCurntFSsz) 
            GlobalReAlloc(lpDevice->epTTFSum,dwCurntFSsz,GMEM_MOVEABLE|GMEM_ZEROINIT);
            
    /* lock down the TT font sum struct
     */
    lpTTFSum = (LPTTFSUM)GlobalLock(lpDevice->epTTFSum);

    /* Check if font was deleted or already selected
    */
    ttid = lpttfi->idFont - TTBASE - 1;

    if (lpTTFSum[ttid].TTUsage == 0)
        lpFont->dfType &= ~TYPE_HEADERDOWN;
    else if (lpDevice->epCurTTFont == lpttfi->idFont)
        {
        /* This is slightly more robust -- it covers the case where the
         * current font has been deleted for some reason
         */
        if (lpTTFSum)
            GlobalUnlock(lpDevice->epTTFSum);

        return;
        }
  
    lpDevice->epCurTTFont = lpttfi->idFont;
    lpDevice->epECtl = -1;

    if (lpTTFSum)
        lpTTFSum[ttid].TTUsage = lpDevice->epPageCount;

// dtk  

  
    if (!(lpFont->dfType & TYPE_HEADERDOWN))
    {
        /* initialize the flags to no downloaded characters
        */
        lmemset((LPSTR)lpttfi->rgfGlyphDown,0,sizeof(lpttfi->rgfGlyphDown));
        lmemset((LPSTR)&ph,0,sizeof(ph));
  
        /* initialize the font header
        */
        ph.wHeaderSize = WHACKWORD(64);     // 64 byte structure
        ph.bFontType = 2;           // any char
        ph.wBaseLine = WHACKWORD(lpFont->dfAscent);
        ph.wCellWidth = WHACKWORD(lpFont->dfMaxWidth);
        ph.wCellHeight = WHACKWORD(lpFont->dfPixHeight);
        //  ph.bOrientation = (lpDevice->epType == DEV_LAND);
        ph.bSpacing = 1;
        ph.wSymbolSet = WHACKWORD(309);     // ??? Jean, wtf?
        //  ph.wHMI = WHACKWORD(lpttfi->rgwWidths[lpFont->dfDefaultChar] * 4);
        ph.wHMI = WHACKWORD(lpttfi->rgwWidths[32 - lpFont->dfFirstChar] * 4);
        ph.wHeight = WHACKWORD(lpFont->dfPoints * 4);
        ph.wHeightX = WHACKWORD(lpFont->dfPoints * 3);
        ph.bTypeFace = 254; /* changed from 78 to new TT number - dtk 7-17-90 */
        ph.wLastChar = WHACKWORD(lpFont->dfLastChar);
        lstrncpy(ph.cName,(LPSTR)lpFont->dfFace,15);
  
        /* check to see if there is enough memory for the 
         * header and some characters.
         */
        FudgeFact = ((lpFont->dfPixHeight * lpFont->dfPixHeight) << 5);
        if (lpDevice->epFreeMem < FudgeFact)
        {
          /* if fontsummary can't be accessed, dont call
           * UnloadSofts.
           */
          if (!(lpFontSummary = lockFontSummary(lpDevice)))
              retvalue=FALSE;

           if (retvalue) 
              UnloadSofts(lpDevice,lpFontSummary,FudgeFact);
        }

        /* send the header down under the font's ID
         */
        lpDevice->epFreeMem -= TTHDRSZ;

        /* change to use MakeEscape not MyWriteEscape for consistency
         */
//      myWriteEscape(lpDevice,"\x1b*c%dD",lpttfi->idFont);

        myWrite(lpDevice, 
                (LPSTR)&escape,
                MakeEscape((lpESC)&escape,SET_FONT,lpttfi->idFont));

        myWrite(lpDevice,escInit,sizeof(escInit));
        myWrite(lpDevice,(LPSTR)&ph,64);

        lpFont->dfType |= TYPE_HEADERDOWN;
    }

    myWrite(lpDevice, 
            (LPSTR)&escape,
             MakeEscape((lpESC)&escape,SET_FONT,lpttfi->idFont));

    /* change to use MakeEscape not MyWriteEscape for consistency
     */
//  myWriteEscape(lpDevice,"\x1b(%dX",lpttfi->idFont);

    myWrite(lpDevice, 
            (LPSTR)&escape,
            MakeEscape((lpESC)&escape,DES_FONT,lpttfi->idFont));


    if (lpTTFSum)
        GlobalUnlock(lpDevice->epTTFSum);

#ifdef DEBUG_FUNCT
    DBMSG(("Exiting SelectTTFont\n"));
#endif
}


/**************************************************************************
***************************************************************************/


void FAR PASCAL DownloadCharacter(
LPDEVICE lpDevice,
LPFONTINFO lpFont,
BYTE chr)
{
    DWORD dwSize, dwActualSize;
    WORD width,ttid;
    BITMAPMETRICS bmm;
    CHARHEADER ch;
    WORD widthTT;
    register int i,j;
    LPDIBITS lp1, lp2, lpBitmap;
    LPTTFONTINFO lpttfi;
    LPTTFSUM lpTTFSum;
    int retvalue=TRUE;
    LPFONTSUMMARYHDR lpFontSummary;
    ESCtype escape;             /* printer escape */

#ifdef DEBUG_FUNCT
    DBMSG(("Entering DownloadCharacter\n"));
#endif
  

    if (lpFont->dfType & TYPE_HEADERDOWN)

    if ((chr == (BYTE)32) || (!(lpFont->dfType & TYPE_HEADERDOWN)))
        return;
  
    lpttfi = (LPTTFONTINFO)lpFont->dfBitsOffset;
  
    /* compute width in bytes (rounded to next dword)
    */
    width = (4*(lpFont->dfMaxWidth + lpFont->dfMaxWidth/2 + 4*8 - 1)) / 32;
    dwSize = lmul((long)width, (long)(lpFont->dfPixHeight));

    if (!lpDevice->epFontBmpMem)
    {
        lpDevice->epFontBmpMem = GlobalAlloc(GMEM_MOVEABLE, dwSize);
        if (!lpDevice->epFontBmpMem)
            return;
        goto getbmp;
    }
  
    GlobalReAlloc(lpDevice->epFontBmpMem,0L,GMEM_MODIFY|GMEM_MOVEABLE);
    dwActualSize = GlobalSize(lpDevice->epFontBmpMem);
  
    if (dwActualSize < dwSize || dwActualSize >= lmul(dwSize,2L))
    {
        HANDLE hT;
  
        /* will also handle discarded case since size == 0
        */
        if (!(hT=GlobalReAlloc(lpDevice->epFontBmpMem,dwSize,GMEM_MOVEABLE)))
            return;
        lpDevice->epFontBmpMem = hT;
    }
    else
        dwSize = dwActualSize;
  
    getbmp:
  
    /* we know we have enough mem
    */
#ifdef WIN31
    EngineSetFontContext(lpFont, 0);
    //      lpDevice->epType == DEV_PORT ? ROT_PORT : ROT_LAND);
#endif
  
    if(!(lpBitmap = GlobalLock(lpDevice->epFontBmpMem)))
    {
        GlobalReAlloc(lpDevice->epFontBmpMem,0L,GMEM_MODIFY|GMEM_DISCARDABLE);
        return;
    }

/*  added to allow for memory tracking of TT fonts - dtk
 */

    /* lock down the TT font sum struct
     */
    lpTTFSum = (LPTTFSUM)GlobalLock(lpDevice->epTTFSum);

    ttid = lpttfi->idFont - TTBASE - 1;

    /* PERFORM THE MAGICK
     */
#ifdef WIN31
    EngineGetGlyphBmp(lpDevice->ephDC,lpFont,chr,FD_QUERY_CHARIMAGE,lpBitmap,dwSize,&bmm);
#endif
  
    lmemset((LPSTR)&ch,0,sizeof(ch));
  
    ch.bFormat = 4;     // LJ II Tech Ref 10-16
    ch.bContinuation = 0;
    ch.bDescriptorSize = 14;
    ch.bClass = 1;
    //  ch.bOrientation = (lpDevice->epType == DEV_LAND);
    ch.wLeftOffset = WHACKWORD(FROMFIXED(bmm.pfxOrigin.x));
    ch.wTopOffset = WHACKWORD(FROMFIXED(bmm.pfxOrigin.y));
    ch.wWidth = WHACKWORD(LWORD(bmm.sizlExtent.x));
    ch.wHeight = WHACKWORD(LWORD(bmm.sizlExtent.y));
  
    /* use the width in the width table.  This is because if the width in
    * the width table is off (due to linear scaling vs. hinted rasterizing),
    * we want to increment by the width in order to justify correctly.
    * otherwise, it will be off.  If the widths are correct it'll be the
    * same anyway.
    */
    ch.wDeltaX = WHACKWORD(4 * lpttfi->rgwWidths[chr - lpFont->dfFirstChar]);
  
    /* There is a slight difference between the HP download format of a
    * bitmap and the TrueType bitmap: HP scanlines are byte-aligned and
    * TrueType scanlines are dword-aligned.  Thus there are 0-3 extra
    * bytes at the end of each scanline.
    * Also, there are a couple of size-related problems.  The HP printer
    * cannot accept more than 32K in a single download, and at 64k
    * the bitmap will cross a segment boundary, possibly (probably) in
    * the middle of a scanline.  <<sigh>>
    */
  
    width = (LWORD(bmm.sizlExtent.x) + 7) >> 3;
    widthTT = ((LWORD(bmm.sizlExtent.x) + 31) >> 3) & ~0x0003;
  
    if (width != widthTT)
    {
        lp1 = lp2 = lpBitmap;
        for (j = 0; j < LWORD(bmm.sizlExtent.y); j++)
        {
            for (i = 0; i < width; i++)
            {
                *lp1++ = *lp2++;
            }
            lp2 += widthTT - width;
        }
    }
  
    dwSize = lmul((DWORD)LWORD(bmm.sizlExtent.y), (DWORD)width);
  
    widthTT = 16;
  
    /* If we have bitmap data, select the character code 
     */
    if (dwSize)
        myWriteEscape(lpDevice,"\x1b*c%dE",chr);
  
    while (dwSize)
    {
        /* must be a divisor of 64K less than 32k, so...
         */
        if (dwSize > 16384)
            width = 16384;
        else
            width = (WORD)dwSize;
        dwSize -= width;

        myWriteEscape(lpDevice,"\x1b(s%dW",width+widthTT);
        myWrite(lpDevice,(LPSTR)&ch,widthTT);
        myWrite(lpDevice,lpBitmap,width);
        ch.bContinuation = 1;

        /* add the memory used to the tt fontsum and
         * subtract it from the printer's free mem.
         */
        if (lpTTFSum)
        {
            lpDevice->epFreeMem -= widthTT + width;
            lpTTFSum[ttid].TTMem += widthTT + width;
        }
        widthTT = 2;
        lpBitmap += width;
    }

    if (lpTTFSum)
        GlobalUnlock(lpDevice->epTTFSum);
    GlobalUnlock(lpDevice->epFontBmpMem);
    GlobalReAlloc(lpDevice->epFontBmpMem,0L,GMEM_MODIFY|GMEM_DISCARDABLE);
#ifdef DEBUG_FUNCT
    DBMSG(("Exiting DownloadCharacter\n"));
#endif
}



