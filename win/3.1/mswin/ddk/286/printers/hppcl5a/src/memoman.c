/**[f******************************************************************
* memoman.c -
*
* Copyright (C) 1988,1989 Aldus Corporation.
* Copyright (C) 1989-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.
* Company confidential.
*
**f]*****************************************************************/
  
/***************************************************************************/
/*******************************   memoman.c   *******************************/
/*
*  Printer Memory Manager Routines
*
*  rev:
* 01-09-91 SJC  Corrected a problem that occured when printing with fonts
*               that have a 26 byte header -- bug #110.
*
* 02-08-90 VO       Added font size escape sequence to the i.d. string
*  12-05-86    msd     changed some == to = for proper execution; added
*                          push/pop cursor position in CopySoft()
*  11-29-86    skk     module creation
*/
  
//#define DEBUG
  
#include "generic.h"
#include "resource.h"
#define FONTMAN_UTILS
#include "fontman.h"
#include "utils.h"
#define SEG_PHYSICAL
#include "memoman.h"
#include "truetype.h"
#include "build.h"
  
  
/*
* $Header: memoman.c,v 3.890 92/02/06 16:12:15 dtk FREEZE $
*/
  
/*
* $Log:	memoman.c,v $
 * Revision 3.890  92/02/06  16:12:15  16:12:15  dtk (Doug Kaltenecker)
 * Win3.1 Freeze
 * 
 * Revision 3.872  92/01/10  11:28:59  11:28:59  dtk (Doug Kaltenecker)
 * Fixed the 255 char download problem.
 * 
 * Revision 3.871  91/12/02  16:43:56  16:43:56  dtk (Doug Kaltenecker)
 * Changed the ifdef TT build variables to ifdef WIN31.
 * 
 * Revision 3.871  91/11/22  13:19:21  13:19:21  dtk (Doug Kaltenecker)
 * Win 3.1 Post Beta 3 version.
 * 
 * Revision 3.870  91/11/08  11:43:58  11:43:58  dtk (Doug Kaltenecker)
 * 3.1 Release Candidate 1
 * 
 * Revision 3.865  91/11/01  13:51:58  13:51:58  dtk (Doug Kaltenecker)
 * Beta release for Windows 3.1
 * 
 * Revision 3.862  91/10/25  13:47:18  13:47:18  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.860  91/10/23  09:48:45  09:48:45  dtk (Doug Kaltenecker)
 * WinWird Release
 * 
 * Revision 3.852  91/10/09  14:59:46  14:59:46  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.850  91/10/04  16:49:59  16:49:59  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE
 * 
 * Revision 3.840  91/09/28  14:17:17  14:17:17  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE 3 FOR WINWORD
 * 
 * Revision 3.830  91/09/18  16:33:33  16:33:33  dtk (Doug Kaltenecker)
 * RELEASE
 * 
 * Revision 3.822  91/09/16  10:34:08  10:34:08  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE 2 FOR WIN30.
 * 
 * Revision 3.820  91/09/06  14:12:12  14:12:12  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE FOR WINWORD 2.0
 * 
 * Revision 3.813  91/09/04  11:44:08  11:44:08  dtk (Doug Kaltenecker)
 * Put #ifdef TT around the tt stuff in unloadsofts and included build.h.
 * 
 * Revision 3.812  91/08/22  14:32:08  14:32:08  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.807  91/08/08  10:31:24  10:31:24  dtk (Doug Kaltenecker)
 * PREBETA3
 * 
 * Revision 3.804  91/08/06  09:17:16  09:17:16  dtk (Doug Kaltenecker)
 * unlocked the ttfsum in unloadsofts
 * 
 * 
 * Revision 3.803  91/08/02  15:46:05  15:46:05  dtk (Doug Kaltenecker)
 * Changed definition of UnloadSofts to far pascal so it can be called 
 * from with in truetype.c.
 * 
 * Revision 3.802  91/07/22  12:54:24  12:54:24  oakeson (Ken Oakeson)
 * BETA
 * 
 * Revision 3.799  91/07/02  11:51:49  11:51:49  daniels (Susan Daniels)
 * Beta
 * 
 * Revision 3.797  91/07/01  16:29:34  16:29:34  dtk (Doug Kaltenecker)
 * Added support for TT memory mgmt
 * 
 * Revision 3.790  91/06/11  16:03:24  16:03:24  stevec (Steve Claiborne)
 * Freeze
 * 
 * Revision 3.786  91/06/11  15:44:24  15:44:24  dtk (Doug Kaltenecker)
 * Prettified files!
 * 
* Revision 3.785  91/05/22  14:57:02  14:57:02  stevec (Steve Claiborne)
* Beta version to MS
*
* Revision 3.780  91/05/15  15:57:11  15:57:11  stevec (Steve Claiborne)
* Beta          
*
* Revision 3.775  91/04/05  14:31:04  14:31:04  stevec (Steve Claiborne)
* Beta release to MS
*
* Revision 3.770  91/03/25  15:36:04  15:36:04  stevec (Steve Claiborne)
* maintance release
*
* Revision 3.760  91/03/12  07:52:48  07:52:48  stevec (Steve Claiborne)
* Maintance release
*
* Revision 3.755  91/03/03  07:46:08  07:46:08  stevec (Steve Claiborne)
* March 3 Freeze
*
* Revision 3.722  91/03/01  13:42:46  13:42:46  stevec (Steve Claiborne)
* Modified code to handle memory allocation and locking more sanely.
*
* Revision 3.721  91/02/26  12:08:51  12:08:51  oakeson (Ken Oakeson)
* Added check for last_code == 0
*
* Revision 3.720  91/02/11  09:15:26  09:15:26  stevec (Steve Claiborne)
* Aldus version
*
* Revision 3.712  91/02/11  00:43:02  00:43:02  oakeson (Ken Oakeson)
* Allowed SETCHARSET chars with fixed pitch fonts.
*
* Revision 3.711  91/02/08  16:25:59  16:25:59  stevec (Steve Claiborne)
* Added debuging
*
* Revision 3.710  91/02/04  15:47:45  15:47:45  stevec (Steve Claiborne)
* Aldus freeze
*
* Revision 3.701  91/02/04  12:36:20  12:36:20  oakeson (Ken Oakeson)
* Added secondary font select string code
*
* Revision 3.700  91/01/19  09:00:25  09:00:25  stevec (Steve Claiborne)
* Release
*
* Revision 3.685  91/01/14  15:43:20  15:43:20  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.684  91/01/14  10:17:37  10:17:37  stevec (Steve Claiborne)
* Updated the copy right stmt.
*
* Revision 3.683  91/01/14  01:23:05  01:23:05  oakeson (Ken Oakeson)
* Misc additional comments and error handling per earlier code inspection
*
* Revision 3.682  91/01/11  14:04:09  14:04:09  oakeson (Ken Oakeson)
* Corrected computation for size of last char in font file.  Forced
* BlowChunks to return FALSE but still download char when not enough data
* is read.
*
* Revision 3.681  91/01/11  10:32:26  10:32:26  stevec (Steve Claiborne)
* Changed an lseek from 38 bytes to 36 to componsate for an earlier
* read of two bytes.   SJC
*
* Revision 3.680  91/01/10  16:16:43  16:16:43  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.672  91/01/09  16:10:06  16:10:06  stevec (Steve Claiborne)
* Fixed bug #110 -- CopySoft didn't handle fonts that had only a 26
* byte Font Descriptor Size header and therefor, caused a UAE - SJC
*
* Revision 3.671  90/12/19  13:15:12  13:15:12  oakeson (Ken Oakeson)
* Added emMasterUnits to MakeSizeEscape
*
* Revision 3.670  90/12/14  14:54:10  14:54:10  stevec (Steve Claiborne)
* freeze for 12-14-90 ver. 3.670
*
* Revision 3.665  90/12/10  15:35:53  15:35:53  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.661  90/12/10  10:04:38  10:04:38  stevec (Steve Claiborne)
* Removed some unreferenced local variables.  SJC
*
* Revision 3.660  90/12/07  14:50:22  14:50:22  stevec (Steve Claiborne)
* Freeze 12-7-90
*
* Revision 3.650  90/11/30  08:12:16  08:12:16  stevec (Steve Claiborne)
* Freeze 3.650, 11-30-90
*
* Revision 3.605  90/10/24  17:45:22  17:45:22  oakeson (Ken Oakeson)
* Added comments for code review
*
* Revision 3.604  90/09/12  17:25:20  17:25:20  oakeson (Ken Oakeson)
* Reduced number of _lread calls
*
* Revision 3.603  90/09/06  14:59:38  14:59:38  oakeson (Ken Oakeson)
* Changed char downloading to allow for char_codes > 255, as well as
* Class 4 Intellifont characters
*
* Revision 3.602  90/08/29  17:43:28  17:43:28  oakeson (Ken Oakeson)
* Download characters on demand (as needed basis)
*
* Revision 3.601  90/08/24  11:37:43  11:37:43  daniels ()
* message.txt
*
* Revision 3.600  90/08/03  11:09:51  11:09:51  stevec (Steve Claiborne)
* This is the Aug. 3 release ver. 3.600
*
* Revision 3.550  90/07/27  11:31:05  11:31:05  root ()
* Experimental freeze 3.55
*
* Revision 3.540  90/07/25  12:34:35  12:34:35  stevec (Steve Claiborne)
* Experimental freeze 3.54
*
* Revision 3.520  90/06/13  16:52:35  16:52:35  root ()
* 5_2_release
*
*
*    Rev 1.1   20 Feb 1990 15:31:30   vordaz
* Support for downloadables.
*/
  
/*  utilities
*/
#include "lockfont.c"
#include "makefsnm.c"
  
  
/*  Local debug structure.
*/
#ifdef DEBUG
    #define LOCAL_DEBUG
#endif
  
#ifdef LOCAL_DEBUG
    #define DBGTrace(msg) DBMSG(msg)
    #define DBGErr(msg) DBMSG(msg)
    #define DBGUpdateLRU(msg) /*DBMSG(msg)*/
    #define DBGOnDemand(msg)  DBMSG(msg)
#else
    #define DBGErr(msg)  /*null*/
    #define DBGTrace(msg) /*null*/
    #define DBGUpdateLRU(msg) /*null*/
    #define DBGOnDemand(msg)  /*null*/
#endif
  
/*declaration of static routines*/
static BOOL CopySoft(LPDEVICE, LPSTR, LPFONTSUMMARY, LPSTR, short);
static BOOL BlowChunks(LPDEVICE, int, long);
static void DL_Class4(LPDEVICE, int, LPWORD, LPWORD, LPLONG, LPSHORT, WORD);
static BYTE readnum(int, int FAR *);


BOOL FAR PASCAL UnloadSofts(LPDEVICE,LPFONTSUMMARYHDR,long);


#ifdef EMPTY_FN
static void AddSoft(LPDEVICE, short);
static void DeleteSoft(LPDEVICE, short);
#endif
  
/*** Tetra II begin ***/
/*** Added pixel height to arg list for size escape sequence build. ***/

BOOL FAR PASCAL DownLoadSoft(lpDevice, fontInd, PixHeight, lpString, count)
LPDEVICE lpDevice;
short fontInd;
short PixHeight;
LPSTR lpString;     /* str of chars to download, if necessary */
short count;        /* # of chars in lpString */
/*** Tetra II end ***/
{
    BOOL retvalue=TRUE;
    int err=SUCCESS;
    ESCtype escape;
    char DLfilename[128];
    LPFONTSUMMARYHDR lpFontSummary;
    LPFONTSUMMARY lpSummary;
    short fontID;
    long newmem;
  
#ifdef DEBUG_FUNCT
    DB(("Entering DownLoadSoft\n"));
#endif
  
    DBGTrace(("In DownLoadSoft, fontInd=%d, epPgSoftNum=%d, epMaxMgSoft=%d, epFreeMem=%d\n",
    fontInd, lpDevice->epPgSoftNum, lpDevice->epMaxPgSoft, lpDevice->epFreeMem));
    if (lpDevice->epPgSoftNum >= lpDevice->epMaxPgSoft)
    {
        DBGTrace(("more than max softs already on page\n"));
        retvalue=FALSE;
    }
    else if (!(lpFontSummary = lockFontSummary(lpDevice)))
    {
        /*if fontsummary can't be accessed then exit*/
        DBGTrace(("DownLoadSoft: can't access fontsummary table\n"));
        retvalue=FALSE;
    }
    else 
    {
        lpSummary = &lpFontSummary->f[fontInd];
  
        if (lpDevice->epFreeMem < ((newmem=lpSummary->memUsage)+MINMEM)
            || (lpDevice->epTotSoftNum >= lpDevice->epMaxSoft))
        {
            if (!(UnloadSofts(lpDevice,lpFontSummary,newmem)))
                retvalue=FALSE;
        }
  
        if (retvalue && !MakeFontSumFNm (lpFontSummary, lpSummary,
            DLfilename, sizeof(DLfilename), FALSE))
            retvalue = FALSE;
  
        if (retvalue)
        {
            /*** Tetra II begin ***/
            /*** Added some vars to use with size string build ***/
  
            char desig_str[80];
            LPSTR lpDesigStr = (LPSTR) desig_str;
            LPSTR temp;
            short n = 0;
  
            /*** Tetra II end ***/
  
            /*send escape to set the font ID*/
            fontID=lpSummary->fontID;
            err=myWrite(lpDevice, (LPSTR)&escape,
            MakeEscape((lpESC)&escape,SET_FONT,fontID));
  
            /*download the font*/
            if (CopySoft(lpDevice, DLfilename, lpSummary,
                lpString, count))
            {
                /**/
                lpDevice->epFreeMem-=newmem;
                DBGTrace(("free mem is %ld\n",lpDevice->epFreeMem));
                UpdateSoftInfo(lpDevice,lpFontSummary,fontInd);
                lpDevice->epTotSoftNum++;
  
#ifdef EMPTY_FN
                /*add to soft font list*/
                AddSoft(lpDevice, fontInd);
#endif
  
                /*** Tetra II begin ***/
                /*** If font is scal, then build font size select sequence ***/
                /*** to go with the i.d. sequence                          ***/
                if (lpSummary->scaleInfo.scalable)
                {
                    n = MakeSizeEscape((lpESC)&escape,
                    lpSummary->dfPitchAndFamily,
                    PixHeight,
                    ((lpSummary->dfPitchAndFamily & 0x1) ?
                    lpSummary->dfVertRes :
                    lpSummary->dfHorizRes),
                    lpSummary->dfAvgWidth,
                    lpSummary->scaleInfo.emMasterUnits);
                    lmemcpy(lpDesigStr, (LPSTR)&escape, n);
                    lpDesigStr = &lpDesigStr[n];
                }
  
                n += MakeEscape((lpESC)&escape, DES_FONT, fontID);
                lmemcpy(lpDesigStr, (LPSTR)&escape, n);
                lpDesigStr[n] = '\0';
  
                lpDesigStr = (LPSTR)desig_str;
  
                /*send escape to select the font*/
                err = myWrite(lpDevice, lpDesigStr, n);
  
                /* It's gotta be a soft font, so use CG Times   */
                /* (with DeskTop symbol set) if we're doing the */
                /* "Publishing Translation"             */
  
                if (lpDevice->epPubTrans)
                {
                    temp = lpDesigStr;
  
                    /* Select secondary font */
                    while (*temp)
                    {
                        if (*temp == '(')
                            *temp = ')';
  
                        temp++;
                    }   /* while */
  
                    myWrite(lpDevice, lpDesigStr,
                    lstrlen(lpDesigStr));
  
                    /* Select CG Times (Desktop)              */
                    /* but keep bold/italic from primary font */
                    myWrite(lpDevice, DT_CGT_SECOND);
  
                    /* If fixed pitch and pixheight is defined */
                    if ((!(lpSummary->dfPitchAndFamily & 0x1)) &&
                        (PixHeight))
                    {
                        /*** CG Times is variable-pitch ***/
                        n = MakeSizeEscape((lpESC)&escape, 0x1,
                        PixHeight,
                        lpSummary->dfVertRes,
                        lpSummary->dfAvgWidth,
                        lpSummary->scaleInfo.emMasterUnits);
  
                        /* Set height for SECONDARY font */
                        escape.start1 = ')';
  
                        lmemcpy(lpDesigStr, (LPSTR)&escape, n);
                        lpDesigStr[n] = '\0';
  
                        myWrite(lpDevice, lpDesigStr,
                        lstrlen(lpDesigStr));
                    }
  
  
                }
  
                /*** Tetra II end ***/
            }
            else retvalue=FALSE;
        }
        unlockFontSummary(lpDevice);
    }
  
#ifdef DEBUG_FUNCT
    DB(("Exiting DownLoadSoft\n"));
#endif
    return retvalue;
}
  
  
void FAR PASCAL UpdateSoftInfo(lpDevice,lpFontSummary,fontind)
LPDEVICE    lpDevice;
LPFONTSUMMARYHDR lpFontSummary;
short   fontind;  /*index of current font*/
/*Assumes that FontSummary is locked. LRU counts for temporary soft
fonts are updated (the LRU count for the current
font is 0 and the LRU count of all other fonts is incremented) The
onPage flag is also set for the font and lpDevice->epPgFontNum is
incremented*/
{
    short  ind,len;
    LPFONTSUMMARY lpSummary;
  
#ifdef DEBUG_FUNCT
    DB(("Entering UpdateSoftInfo\n"));
#endif
  
    lpSummary=&lpFontSummary->f[0];
    len = lpFontSummary->len;
    for (ind=0; ind < len; ++ind, ++lpSummary) {
        if (ind==fontind) {
            if (lpSummary->indDLName != -1)
                lpSummary->LRUcount = 0;
            if (!(lpSummary->onPage)) {
                lpSummary->onPage = TRUE;
                lpDevice->epPgSoftNum++;
            }
        }
        else
            if ((lpSummary->indDLName != -1) && (lpSummary->LRUcount != -1))
                lpSummary->LRUcount++;
    }
  
#ifdef DEBUG_FUNCT
    DB(("Exiting UpdateSoftInfo\n"));
#endif
  
}
  
/*
* CopySoft  --  copies the font header and appropriate characters from the
*               specified soft font download file to the printer
*/
  
static BOOL CopySoft(lpDevice, DLfilename, lpSummary, lpString, count)
LPDEVICE lpDevice;
LPSTR   DLfilename;
LPFONTSUMMARY lpSummary;
LPSTR   lpString;
short count;
{
    BYTE ch;        /* temp char buffer */
    BYTE str[3];        /* buffer for DL file esc strings */
    register short i;
    int fh,         /* file handle */
    esc_int,        /* int from ASCII escape string */
    prev = 0;       /* previous char in DL file */
    long pos,
    char_size = 0; /* size of DL char */
    LPCHARDL lpchardl;  /* pointer to char download struct */
    LPWORD CharDwn;     /* bit field of downloaded chars */
    LPWORD Class4;      /* bit field of Class IV chars */
    LPLONG CharOffsets; /* offset of each char from begin of file */
    LPSHORT NextChar;   /* next char (from current) in DL file */
    WORD last_code=0;   /* Last code for font -- may be greater */
    /*     than # of characters for font    */
    WORD FntDSize;          /* Font Descriptor Size                 */
    BOOL cont;      /* TRUE if next data MIGHT be continuation */
  
#ifdef DEBUG_FUNCT
    DB(("Entering CopySoft\n"));
#endif
  
    DBGTrace(("IN CopySoft, DLfilename=%ls\n",DLfilename));
  
    if ((fh = _lopenp(DLfilename, OF_READ)) > 0)
    {
        DBGTrace(("file opened\n"));
  
        /* save current cursor position */
        myWrite(lpDevice,PUSH_POSITION);
  
        /* Tetra -- send down header if necessary, then send chars. */
        /*          Header escape sequence is Esc)s#W               */
        if (!lpSummary->hCharDL)
        {
            /* Minimal error handling -- skip any garbage at beginning */
            while ((_lread(fh, (LPSTR)str, 1) > 0) && (str[0] != '\033'));
  
            /* Make sure we got an escape, instead of the EOF */
            if ((str[0] == '\033') &&
                (_lread(fh, (LPSTR)str, 2) > 0) && (str[0] == ')') &&
                (str[1] == 's') &&
                (readnum(fh, (int FAR *)&esc_int) == 'W'))
            {
                /* compute header size and send it */
  
                /* Add size from escape to current file position */
                esc_int += (int)_llseek(fh, 0L, 1);
  
                _lread(fh, (LPSTR)&FntDSize, 2);
                FntDSize = WHACKWORD(FntDSize);
  
                DBGOnDemand(("FntDSize=%d\n", FntDSize));
                DBGOnDemand(("esc_int=%d\n", esc_int));
  
                if (FntDSize < 40)
                    // If Font Descriptor Size == 26, there's no last_code
                    // in header.  We select a reasonable value of 255.
                    // If this is an incorrect val, then the _lread later
                    // on will catch the problem.
                    last_code=255;
                else
                {
                    /* Nab last code value.  If we read it successfully, */
                    /* then "take the correctly formatted number & hash  */
                    /* it into nonsense (ie, swap bytes)."  Macro in     */
                    /* truetype.h (for Motorola <--> Intel formatting).  */
  
                    if ((_llseek(fh, 36L, 1) != -1) &&
                        (_lread(fh, (LPSTR)&last_code, 2) > 0))
                        last_code = WHACKWORD(last_code);
                    else
                        last_code = BACKUP_LC;
                }
  
                /* Check if last_code = 0 (some IFV fonts) */
                if (!last_code)
                    last_code = BACKUP_LC;
  
                DBGOnDemand(("last_code=%d\n",last_code));
  
                DBGTrace(("%d header bytes\n", esc_int));
                DBGTrace(("Font last code = %d\n", last_code));
  
                /* Send the header down */
                if ((_llseek(fh, 0L, 0) == -1) ||
                    (!BlowChunks(lpDevice, fh, (long)esc_int)))
                    return FALSE;
  
                /* Allocate the memory for download record and lock it */
                DBGOnDemand(("******************************\n"));
                DBGOnDemand(("About to call GlobalAlloc\n"));
                DBGOnDemand(("size=%d\n",(DWORD)(sizeof(WORD)+
                2 * ((last_code + 7) / 8) +
                last_code * (sizeof(long) + sizeof(short)))));
  
                /* The alloc size is enough for the last_code value, plus
                * two bit fields, a long array, and a short array for
                * X characters, where X = last_code
                */
                lpSummary->hCharDL = GlobalAlloc(GHND | GMEM_DDESHARE,
                (DWORD)(sizeof(WORD) +
                2 * ((last_code + 7) / 8) +
                last_code * (sizeof(long) + sizeof(short))));
  
                DBGTrace(("Just alloced %d bytes\n", (int)(sizeof(WORD) +
                2 * ((last_code + 7) / 8) +
                last_code * (sizeof(long) + sizeof(short)))));
  
                if ((lpSummary->hCharDL) &&
                    (lpchardl = (LPCHARDL)GlobalLock(lpSummary->hCharDL)))
                {
                    DBGTrace(("lpchardl = %lp\n", lpchardl));
  
                    /* Get last char code (may be > symset size) */
                    lpchardl->LastCode = last_code;
  
                    /* Get pointers to bit fields, etc. */
                    CharDwn = lpchardl->CharDown;
                    Class4 = (LPWORD)((LPSTR)CharDwn +
                    ((int)(last_code + 7) / 8));
                    CharOffsets = (LPLONG)((LPSTR)Class4 +
                    ((int)(last_code + 7) / 8));
                    NextChar = (LPSHORT)((LPSTR)CharOffsets +
                    ((int)last_code * sizeof(LONG)));
  
                    DBGTrace(("CharDwn = %lp\n", (LPSTR)CharDwn));
                    DBGTrace(("Class4 = %lp\n", (LPSTR)Class4));
                    DBGTrace(("CharOffsets = %lp\n", (LPSTR)CharOffsets));
                    DBGTrace(("NextChar = %lp\n", (LPSTR)NextChar));
  
                    /* Now get file offsets for each character */
                    while ((_lread(fh, (LPSTR)str, 3) > 0) && (str[0] == '\033'))
                    {
                        if (str[1] == '*')
                        {
                            /* It's a char ID escape sequence, so save */
                            /* the position.                           */
  
                            pos = _llseek(fh, 0L, 1) - 3;
                            if ((str[2] == 'c') &&
                                (readnum(fh, &esc_int) == 'E'))
                            {
                                /* NOTE: If a future version of PCL allows
                                * any additional commands after the char
                                * ID escape sequence but prior to the
                                * downloading of the character, we might
                                * need to check for a lowercase "E".
                                */
  
                                /* Let previous char know what's next */
                                NextChar[prev] = (short)esc_int;
  
                                /* Record file position */
                                CharOffsets[esc_int] = pos;
  
                                prev = esc_int;
                                DBGTrace(("DL char code = %d\n", esc_int));
                            }
  
                            cont = FALSE;  /* New char, not continuation */
                        }
                        else if ((str[1] == '(') &&
                            (str[2] == 's') &&
                            (readnum(fh, (int FAR *)&esc_int) == 'W'))
                            /* It's character data.    */
                            /*  Jump to the end of it. */
                        {
                            if ((!cont) &&
                                (lpSummary->scaleInfo.scalable))
                            {
                                /* Check if it's Class IV data */
                                if ((_llseek(fh, 3L, 1) != -1) &&
                                    (_lread(fh, (LPSTR)str, 1) > 0) &&
                                    (str[0] == 4))
                                {
                                    /* Mark as a compound contour */
                                    SETTRUE(Class4, prev);
                                    DBGTrace(("Class IV char\n"));
                                }
  
                                _llseek(fh,(long)(esc_int - 4),1);
                            }
                            else
                                _llseek(fh, (long)esc_int, 1);
  
                            /* Next data could be continuation */
                            cont = TRUE;
  
                            DBGTrace(("%d char bytes\n", esc_int));
                        }
#ifdef DEBUG
                        else
                            DBGTrace(("Failed conditionals in while loop.\n"));
#endif
                    }   /* while... */
  
                    //          DBGTrace(("str[0] (after while) = %c, %d, ", str[0], (int)str[0]));
  
                    /* "Flag" to denote the last character in the file */
                    NextChar[prev] = (short)prev;
                }
                else    /* Lock failed */
                {
                    if (lpSummary->hCharDL)
                    {
                        GlobalFree(lpSummary->hCharDL);
                        lpSummary->hCharDL = 0;
                    }
  
                    return FALSE;
                }
            }
            else    /* No font header escape string */
                return FALSE;
        }
        else    /* Font header already down -- just init pointers */
        {
            if (lpchardl = (LPCHARDL)GlobalLock(lpSummary->hCharDL))
            {
                last_code = lpchardl->LastCode;
                CharDwn = lpchardl->CharDown;
                Class4 = (LPWORD)((LPSTR)CharDwn +
                ((int)(last_code + 7) / 8));
                CharOffsets = (LPLONG)((LPSTR)Class4 +
                ((int)(last_code + 7) / 8));
                NextChar = (LPSHORT)((LPSTR)CharOffsets +
                ((int)last_code * sizeof(LONG)));
            }
            else
                return FALSE;
        }
  
        /* Now send down chars, if necessary */
        for (i=0; i<count; i++)
        {
            ch = lpString[i];
  
            DBGTrace(("lpString[i] (char code) = %c, %d, \n", ch, (int)ch));
  
            /* If character's not downloaded already... */
            if ((last_code >= ch) && (!ISTRUE(CharDwn, ch)))
            {
                SETTRUE(CharDwn, ch);
  
                /* Make sure the char exists */
                if (CharOffsets[ch])
                {
                    /* We treat Class IV differently */
                    if (ISTRUE(Class4, ch))
                        DL_Class4(lpDevice, fh, CharDwn, Class4, CharOffsets,
                        NextChar, (WORD)ch);
  
                    /* Check if it's the last char and compute the size */
                    if (NextChar[ch] == (short)ch)
                        char_size = _llseek(fh, 0L, 2) - CharOffsets[ch];
                    else
                        char_size = CharOffsets[NextChar[ch]]-CharOffsets[ch];
  
                    _llseek(fh, CharOffsets[ch], 0);
  
                    DBGTrace(("CopySoft(): "));
  
                    /* Send that baby out the door */
                    BlowChunks(lpDevice, fh, char_size);
                }
  
                DBGTrace(("!"));
            }
  
            DBGTrace(("ISTRUE\n"));
        }   /* for loop */
  
        GlobalUnlock(lpSummary->hCharDL);
        _lclose(fh);
  
        /* restore cursor position */
        myWrite(lpDevice,POP_POSITION);
  
#ifdef DEBUG_FUNCT
        DB(("Exiting CopySoft\n"));
#endif
  
        return TRUE;
    }
    else    /* could not open file */
        return FALSE;
}
  
/*  BlowChunks
*
*  reads the specified amount of data from the file and sends it to the
*  printer in CHUNK_SIZE chunks
*/
  
  
static BOOL BlowChunks(lpDevice, fh, count)
LPDEVICE lpDevice;
int fh;             /* file handle         */
long count;         /* number of bytes to send */
{
    char fbuf[CHUNK_SIZE];
    int read_size;          /* # of bytes to read at once */
    long i;
    BOOL status = TRUE;
  
#ifdef DEBUG_FUNCT
    DB(("Entering BlowChunks\n"));
#endif
  
    DBGTrace(("BlowChunks(%lp, %d, %d)\n", lpDevice, fh, count));
  
    for (i=0; ((i < count) && status); i+=CHUNK_SIZE)
    {
        /* Might not have a full CHUNK_SIZE buffer */
        if ((count - i) < CHUNK_SIZE)
            read_size = (int)(count - i);
        else
            read_size = CHUNK_SIZE;
  
        /* Read the chunk and send it on its merry way */
        status = (_lread(fh, (LPSTR) fbuf, read_size) != -1);
  
        myWrite(lpDevice, (LPSTR)fbuf, (short)read_size);
    }   /* for loop */
  
    myWriteSpool(lpDevice);
  
#ifdef DEBUG_FUNCT
    DB(("Exiting BlowChunks\n"));
#endif
  
    return status;
}
  
  
#ifdef EMPTY_FN
  
static void AddSoft(lpDevice, fontInd)
LPDEVICE lpDevice;
short fontInd;
/*add soft font to soft font list*/
{
  
}
  
static void DeleteSoft(lpDevice,fontInd)
LPDEVICE lpDevice;
short fontInd;
/*delete soft font from soft font list*/
{
  
}
  
#endif
  
BOOL FAR PASCAL UnloadSofts(lpDevice,lpFontSummary,memNeeded)
LPDEVICE lpDevice;
LPFONTSUMMARYHDR lpFontSummary;
long memNeeded;
/*note:assumes FONTSUMMARY is locked*/
/*based on LRU count, memUsage & onPage, decide which downloaded fonts to boot*/
{
    int err;                    /* error status */
    short fontInd;              /* font index */
    ESCtype escape;             /* printer escape */
    short  ind,len;
    LPTTFSUM lpTTFSum;          /* TT font sum */
    LPFONTSUMMARY lpSummary;    /* font summary */
    short lastLRU=-1;           /* last least-recently-used value */
  
    DBGTrace(("IN UnloadSofts, memneeded=%ld\n",memNeeded));

#ifdef WIN31

    /* lock down the TT font sum struct
     */
    lpTTFSum = (LPTTFSUM)GlobalLock(lpDevice->epTTFSum);

    /* for all the TT fonts, if it ain't on the page
     * nuke it and set the usage to 0
     */
    for(ind=0; ind < (lpDevice->epNextTTFont - TTBASE); ind ++)
    {
        if((lpTTFSum[ind].TTUsage != lpDevice->epPageCount) &&
          (lpTTFSum[ind].TTUsage != 0))
        {
            err=myWrite(lpDevice, (LPSTR)&escape,
            MakeEscape((lpESC)&escape,SET_FONT,ind+TTBASE+1));
            err=myWrite(lpDevice,DEL_FONT);
            lpDevice->epFreeMem += lpTTFSum[ind].TTMem;
            lpTTFSum[ind].TTUsage = 0;
        }
    }

    /* unlock the TT font sum struct
     */
    if (lpTTFSum)
        GlobalUnlock(lpDevice->epTTFSum);
#endif


    lpSummary=&lpFontSummary->f[0];
    len = lpFontSummary->len;
    for (ind=0; ind < len; ++ind, ++lpSummary) 
    {
        if (!lpSummary->onPage && (lpSummary->memUsage>=memNeeded) &&
        (lpSummary->LRUcount>lastLRU)) {
            fontInd=ind;
            lastLRU=lpSummary->LRUcount;
        }
    }
    if (lastLRU==-1)
        return FALSE;
  
#ifdef EMPTY_FN
    DeleteSoft(lpDevice,fontInd);
#endif
  
    /* send escapes to delete the font
     */
    lpFontSummary->f[fontInd].LRUcount=-1;
    err=myWrite(lpDevice, (LPSTR)&escape,
    MakeEscape((lpESC)&escape,DES_FONT,lpFontSummary->f[fontInd].fontID));
    err=myWrite(lpDevice,DEL_FONT);
    lpDevice->epFreeMem += lpFontSummary->f[fontInd].memUsage;
    lpDevice->epTotSoftNum--;
    lpDevice->epPgSoftNum--;
  
#ifdef DEBUG_FUNCT
    DB(("Exiting UnloadSofts\n"));
#endif

    return TRUE;
}
  
/*  DL_Class4()
*
*  Download a Class 4 character and it's dependent characters
*/
  
static void DL_Class4(lpDevice,hFile,CharDwn,Class4,CharOffsets,NextChar,ch)
LPDEVICE lpDevice;
int hFile;
LPWORD CharDwn;     /* Bitfield -- Is each char downloaded? */
LPWORD Class4;      /* Bitfield -- Is the char Class IV data */
LPLONG CharOffsets;    /* File offset array */
LPSHORT NextChar;       /* Next char in download file array */
WORD ch;            /* The Class IV character who started this */
{
    BYTE parts; /* parts: # of components */
    WORD char_code; /* Character code from component list */
    long char_size, /* Size of character data */
    pos;      /* position in file */
  
#ifdef DEBUG_FUNCT
    DB(("Entering DL_Class4\n"));
#endif
  
    DBGTrace(("DL_Class4(), lpDevice:    %lp\n", lpDevice));
    DBGTrace(("DL_Class4(), hFile:       %d\n", hFile));
    DBGTrace(("DL_Class4(), CharDwn:     %lp\n", CharDwn));
    DBGTrace(("DL_Class4(), Class4:      %lp\n", Class4));
    DBGTrace(("DL_Class4(), CharOffsets: %lp\n", CharOffsets));
    DBGTrace(("DL_Class4(), NextChar:    %lp\n", NextChar));
    DBGTrace(("DL_Class4(), ch:          %c\n\n", ch));
  
    /* Skip escape sequences before binary data */
    _llseek(hFile, (CharOffsets[ch] + 9L), 0);
    while ((_lread(hFile, (LPSTR)&parts, 1) > 0) && (parts != 'W'));
    _llseek(hFile, 6L, 1);
  
    /* Find out how many parts compose this character */
    if (_lread(hFile, (LPSTR)&parts, 1) > 0)
    {
        DBGTrace(("DL_Class4(), parts:       %d\n", (int)parts));
  
        /* Update and save current file position */
        pos = _llseek(hFile, 1L, 1);
  
        while (parts--)
        {
            /* Read the character code for one part */
            if (_lread(hFile, (LPSTR)&char_code, 2) > 0)
            {
                char_code = WHACKWORD(char_code);
  
#ifdef DEBUG
                /* Hey folks, I want to know EVERYTHING! */
                DBGTrace(("DL_Class4(), char_code:   %d\n\n", char_code));
  
                {
                    LPWORD j;
  
                    DBGTrace(("DL_Class4(), CharDwn bit field (right side)\n"));
                    for (j = CharDwn; j < Class4; j++)
                        DBGTrace(("DL_Class4(), CharDwn:   %lp\n", (0L | (*j))));
                    DBGTrace(("DL_Class4()\n"));
                }
#endif
  
                /* Check if the part is downloaded */
                if (!ISTRUE(CharDwn, char_code))
                {
                    SETTRUE(CharDwn, char_code);
                    DBGTrace(("DL_Class4(): CharOffsets[%d] = %lp\n", char_code,
                    CharOffsets[char_code]));
  
                    /* If the part exists... */
                    if (CharOffsets[char_code])
                    {
                        /* Recursively download Class IV parts */
                        if (ISTRUE(Class4, char_code))
                            DL_Class4(lpDevice, hFile, CharDwn, Class4,
                            CharOffsets, NextChar, char_code);
  
                        /* Check for last char and compute the size */
                        if (NextChar[char_code] == char_code)
                            char_size = _llseek(hFile, 0L, 2) -
                            CharOffsets[char_code];
                        else
                            char_size = CharOffsets[NextChar[char_code]] -
                            CharOffsets[char_code];
  
                        DBGTrace(("DL_Class4(): "));
  
                        /* Bombs away -- download character to printer */
                        if (_llseek(hFile, CharOffsets[char_code], 0) != -1)
                            BlowChunks(lpDevice, hFile, char_size);
                    }   /* if CharOffsets */
                }       /* if part is not downloaded (!ISTRUE) */
            }       /* if lread... */
  
            /* Restore the file position and move to next component */
            pos += 6L;
            _llseek(hFile, pos, 0);
  
        }   /* while */
    }   /* if lread */
  
#ifdef DEBUG_FUNCT
    DB(("Exiting DL_Class4\n"));
#endif
  
}
  
/*  readnum()
*
*  read a decimal integer from a font download file.
*  Return the character that followed the integer.
*
*  I yanked this code from the font installer, sly dog that I am...
*/
  
static BYTE readnum(hFile, lpNum)
int hFile;
int FAR *lpNum;
{
    BYTE ch;
  
#ifdef DEBUG_FUNCT
    DB(("Entering readnum\n"));
#endif
  
    *lpNum = 0;
  
    while (_lread(hFile,(LPSTR)&ch,1) > 0 && ch >= '0' && ch <= '9')
    {
        *lpNum *= 10;
        *lpNum += (int)ch - (int)'0';
    }
  
    /* We don't currently check if readnum() returns FALSE
    if (*lpNum == 0)
    ch = '\0';
    */
  
#ifdef DEBUG_FUNCT
    DB(("Exiting readnum\n"));
#endif
  
    return (ch);
}
