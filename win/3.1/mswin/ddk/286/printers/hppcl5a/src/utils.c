/**[f******************************************************************
* utils.c -
*
* Copyright (C) 1988,1989 Aldus Corporation.
* Copyright (C) 1988-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.
* Company confidential.
*
**f]*****************************************************************/
  
/***************************************************************************/
/********************************   UTILS   ********************************/
/*
* 02 dec 91   SD      Bug #740: xmoveto now accepts negative x offsets.
* 14 feb 90   VO      Added support routines for downloadable scalables
* 20 jan 90    Ken O   Removed non-Galaxy support
* 12 jan 90   VO      Added support routines for scaling
*   1-13-89    jimmat  Reduced # of redundant strings by adding lclstr.h
*   1-25-89    jimmat  Use hLibInst instead of calling GetModuleHandle()-
*          lclstr.h no longer required by this file.
*   1-30-89    jimmat  Changed MakeAppName to allow names for other app's
*          to be built (like the Font Installer).
*/
  
/*
* $Header: utils.c,v 3.890 92/02/06 16:12:32 dtk FREEZE $
*/
  
/*
* $Log:	utils.c,v $
 * Revision 3.890  92/02/06  16:12:32  16:12:32  dtk (Doug Kaltenecker)
 * Win3.1 Freeze
 * 
 * Revision 3.871  91/12/03  21:03:40  21:03:40  daniels (Susan Daniels)
 * Fix bug #740:  text was appearing to the right of its true location
 * and hence looked like it wasn't getting clipped properly.  Changed
 * lmoveto so that it can handle negative x-offsets.
 * 
 * Revision 3.870  91/11/08  11:44:17  11:44:17  dtk (Doug Kaltenecker)
 * 3.1 Release Candidate 1
 * 
 * Revision 3.865  91/11/01  13:52:15  13:52:15  dtk (Doug Kaltenecker)
 * Beta release for Windows 3.1
 * 
 * Revision 3.862  91/10/25  13:47:35  13:47:35  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.860  91/10/23  09:49:03  09:49:03  dtk (Doug Kaltenecker)
 * WinWird Release
 * 
 * Revision 3.852  91/10/09  15:00:03  15:00:03  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.850  91/10/04  16:50:17  16:50:17  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE
 * 
 * Revision 3.840  91/09/28  14:17:33  14:17:33  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE 3 FOR WINWORD
 * 
 * Revision 3.830  91/09/18  16:33:51  16:33:51  dtk (Doug Kaltenecker)
 * RELEASE
 * 
 * Revision 3.822  91/09/16  10:34:35  10:34:35  dtk (Doug Kaltenecker)
 * WINWORD RELEASE CANDIDATE 2 FOR WIN30.
 * 
 * Revision 3.820  91/09/06  14:12:30  14:12:30  dtk (Doug Kaltenecker)
 * RELEASE CANDIDATE FOR WINWORD 2.0
 * 
 * Revision 3.812  91/08/22  14:32:26  14:32:26  dtk (Doug Kaltenecker)
 * BETA
 * 
 * Revision 3.807  91/08/08  10:31:42  10:31:42  dtk (Doug Kaltenecker)
 * PREBETA3
 * 
 * Revision 3.802  91/07/22  12:54:45  12:54:45  oakeson (Ken Oakeson)
 * BETA
 * 
 * Revision 3.799  91/07/02  11:52:07  11:52:07  daniels (Susan Daniels)
 * Beta
 * 
 * Revision 3.796  91/06/26  11:26:27  11:26:27  stevec (Steve Claiborne)
 * BETA
 * 
 * Revision 3.790  91/06/11  16:03:43  16:03:43  stevec (Steve Claiborne)
 * Freeze
 * 
 * Revision 3.786  91/06/11  15:44:48  15:44:48  dtk (Doug Kaltenecker)
 * Prettified files!
 * 
* Revision 3.785  91/05/22  14:57:20  14:57:20  stevec (Steve Claiborne)
* Beta version to MS
*
* Revision 3.780  91/05/15  15:57:28  15:57:28  stevec (Steve Claiborne)
* Beta
*
* Revision 3.775  91/04/05  14:31:21  14:31:21  stevec (Steve Claiborne)
* Beta release to MS
*
* Revision 3.770  91/03/25  15:36:20  15:36:20  stevec (Steve Claiborne)
* maintance release
*
* Revision 3.760  91/03/12  07:53:07  07:53:07  stevec (Steve Claiborne)
* Maintance release
*
* Revision 3.756  91/03/05  15:29:14  15:29:14  oakeson (Ken Oakeson)
* Use copy of portname to truncate in MakeAppName
*
* Revision 3.755  91/03/03  07:46:26  07:46:26  stevec (Steve Claiborne)
* March 3 Freeze
*
* Revision 3.721  91/02/25  17:03:26  17:03:26  oakeson (Ken Oakeson)
* Changed lpAppNm to lpPortNm in MakeAppName for loop.  We want to strip off
* the end of stuff like "LPT1.PRN", not "HPPCL5A"
*
* Revision 3.720  91/02/11  09:15:43  09:15:43  stevec (Steve Claiborne)
* Aldus version
*
* Revision 3.711  91/02/08  16:26:51  16:26:51  stevec (Steve Claiborne)
* Added debuging
*
* Revision 3.710  91/02/04  15:47:59  15:47:59  stevec (Steve Claiborne)
* Aldus freeze
*
* Revision 3.701  91/02/04  12:37:34  12:37:34  oakeson (Ken Oakeson)
* Incr str variable when looping in MyWrite
*
* Revision 3.700  91/01/19  09:00:40  09:00:40  stevec (Steve Claiborne)
* Release
*
* Revision 3.685  91/01/14  15:43:36  15:43:36  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.681  91/01/14  10:17:53  10:17:53  stevec (Steve Claiborne)
* Updated the copy right stmt.
*
* Revision 3.680  91/01/10  16:16:58  16:16:58  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.671  90/12/19  13:14:21  13:14:21  oakeson (Ken Oakeson)
*
* Use font's masterunits for scaling, instead of hardcoded 8782
*
* Revision 3.670  90/12/14  14:54:25  14:54:25  stevec (Steve Claiborne)
* freeze for 12-14-90 ver. 3.670
*
* Revision 3.665  90/12/10  15:36:08  15:36:08  stevec (Steve Claiborne)
* Freeze
*
* Revision 3.660  90/12/07  14:50:36  14:50:36  stevec (Steve Claiborne)
* Freeze 12-7-90
*
* Revision 3.650  90/11/30  08:12:30  08:12:30  stevec (Steve Claiborne)
* Freeze 3.650, 11-30-90
*
* Revision 3.601  90/08/24  11:38:03  11:38:03  daniels (Susan Daniels)
* message.txt
*
* Revision 3.600  90/08/03  11:10:04  11:10:04  stevec (Steve Claiborne)
* This is the Aug. 3 release ver. 3.600
*
* Revision 3.550  90/07/27  11:31:20  11:31:20  root ()
* Experimental freeze 3.55
*
* Revision 3.540  90/07/25  12:34:54  12:34:54  stevec (Steve Claiborne)
* Experimental freeze 3.54
*
* Revision 3.520  90/06/13  16:52:52  16:52:52  root ()
* 5_2_release
*
*
*    Rev 1.1   20 Feb 1990 15:29:26   vordaz
* Support for downloadables.
*/
  
//#define DEBUG
  
#include "generic.h"
#include "resource.h"
#include "utils.h"
  
  
static int reverse(LPSTR);
  
  
#ifdef DEBUG
#define DBGtrace(msg) DBMSG(msg)
#define DBGerr(msg) DBMSG(msg)
#else
#define DBGtrace(msg) /* null */
#define DBGerr(msg) /* null */
#endif
  
  
#define topofpath(s,lpFileName) \
for (s = lpFileName + lstrlen(lpFileName); \
    (s > lpFileName) && (s[-1] != ':') && (s[-1] != '\\'); --s)
  
    /*  labdivc
    *
    *  Long (A*B)/C:  this is an auxilary routine used to access the
    *  long arithmetic library functions as a FAR procedure.
    */
    long FAR PASCAL labdivc(lval1, lval2, lval3)
    long lval1;
long lval2;
long lval3;
{
    return((lval1 * lval2) / lval3);
}
  
  
/*  lmul
*
*  Long multiply:  this an auxilary routine used to access the
*  long arithmetic library functions as a FAR procedure.
*/
long FAR PASCAL lmul(lval1, lval2)
long lval1;
long lval2;
{
    return(lval1 * lval2);
}
  
  
/*  ldiv
*
*  Long divide:  this an auxilary routine used to access the
*  long arithmetic library functions as a FAR procedure.
*/
long FAR PASCAL ldiv(lval1, lval2)
long lval1;
long lval2;
{
    return(lval1/lval2);
}
  
  
/*  FontMEM
*
*  Calculate the  number of bytes per font.
*/
long FAR PASCAL FontMEM(numchars, width, height)
int numchars;
long width;
long height;
{
    return((long)numchars * (((width + 7) >> 3) * height + 63));
}
  
/*  itoa
*
*  Convert integer to ascii text.
*/
int FAR PASCAL itoa(n, s)
int n;
LPSTR s;
{
    int i, sign;
  
    if ((sign = n) < 0)
        n = -n;
    i = 0;
    do          /* generate digits in reverse order */
    {
        s[i++] = (char)(n % 10 + '0');
    } while (n /= 10);
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
    return i;
}
  
  
/*  atoi
*
*  Convert ascii to integer.
*/
int FAR PASCAL atoi(s)
LPSTR s;
{
    short n, i;
  
    for (i = n = 0;; n *= 10)
    {
        n += s[i] - '0';
        if (!s[++i])
            break;
    }
    return n;
}
  
/*  myWriteSpool
*
*  Dump the contents of the epSpool buffer into the output channel.
*/
int FAR PASCAL myWriteSpool(lpDevice)
register LPDEVICE lpDevice;
{
    short n;
  
    DBGtrace(("IN myWriteSpool\n"));
  
    if (!lpDevice->epPtr || lpDevice->epDoc != TRUE)
        return 0;
    if ((n = WriteSpool(lpDevice->epJob, (LPSTR)lpDevice->epSpool,
        lpDevice->epPtr)) != lpDevice->epPtr)
        lpDevice->epDoc = n;
  
    lpDevice->epPtr = 0;
    return n;
}
  
/*  myWrite
*
*  Copy the output string into our spool buffer, if the string overflows
*  the spool buffer, then dump it first.
*/
int FAR PASCAL myWrite(lpDevice, str, n)
LPDEVICE lpDevice;
LPSTR str;                  /*string of character to write*/
short n;                    /*length of string*/
{
    LPSTR p;
    register short i;
    register short m;
  
#ifdef DEBUG_FUNCT
    DB(("Entering myWrite\n"));
#endif
    #ifdef DBGEscapes
    if (*str == '\033' && n < 20)
    {
        DBGtrace(("IN myWrite "));
        #ifdef DEBUG
        debugStr(str, n);
        #endif
    }
    else {
        DBGtrace(("myWrite(%lp,%lp,%d)\n", lpDevice, str, n));
    }
    #endif
  
    if (lpDevice->epDoc != TRUE)
        return ERROR;
  
    p = &lpDevice->epSpool[i = lpDevice->epPtr]; /*end of current spool buf*/
  
    do {
        if ((i += m = n) >= SPOOL_SIZE)
        {
            if (myWriteSpool(lpDevice) < 0) /*write out spool buffer*/
                return ERROR;
  
            p = lpDevice->epSpool;
  
            i = m = (n > SPOOL_SIZE) ? SPOOL_SIZE : n;
        }
  
        lpDevice->epPtr = i;                /*update spool count*/
        lmemcpy(p, str, (WORD)m);           /*add new characters*/
  
        n -= m;
        str += m;
  
    } while (n > 0);
  
    return SUCCESS;
}
  
/*  MakeEscape
*
*  Build an escape string.
*/
int FAR PASCAL MakeEscape(lpEsc, start1, start2, end, n)
lpESC lpEsc;
char start1, start2, end;
short n;
{
    lpEsc->esc = '\033';
    lpEsc->start1 = start1;
    lpEsc->start2 = start2;
    n = itoa(n, lpEsc->num);
    lpEsc->num[n] = end;
    lpEsc->num[n + 1] = '\0';
    return n + 4;
}
  
  
/*  xmoveto
*
*  x value is in 300 dpi.  If x < 0, the current x is stored in
*  epCurx, and 0 is sent in the dot move absolute escape sequence.
*/
int FAR PASCAL xmoveto(lpDevice, x)
LPDEVICE lpDevice;
int x;                              /* Bug #740: x can be negative now. */
{
    ESCtype escape;
    int err = SUCCESS;
    DBGtrace(("IN xmoveto,x=%d\n", x));
  
    if (lpDevice->epCurx != x)
    {
        lpDevice->epCurx = x;
        if (x < 0)
            x = 0;
        /* TETRA -- removed non-Galaxy support */
        err = myWrite(lpDevice, (LPSTR) &escape,
        MakeEscape(&escape, DOT_HCP, x));
    }
  
    return (err);
}
  
/*  ymoveto
*
*  y value is in 300dpi 
*/
int FAR PASCAL ymoveto(lpDevice, y)
LPDEVICE lpDevice;
WORD y;
{
    ESCtype escape;
    int err = SUCCESS;
    DBGtrace(("IN ymoveto,y=%d\n", y));
  
    if (lpDevice->epCury != y)
    {
        lpDevice->epCury = y;
  
        /* TETRA -- removed non-Galaxy support */
        err=myWrite(lpDevice,(LPSTR) &escape,
        MakeEscape((lpESC)&escape,DOT_VCP,y));
    }
  
    return (err);
}
  
/*** Tetra II begin ***/
/*  ScaleVertical
*
*  Scale with rounding a vertical value from design units to pixels.
*/
short FAR PASCAL ScaleVertical(RawHeight, MasterUnits, PixHeight)
short RawHeight,
MasterUnits,
PixHeight;
{
    long height,
    height10;
  
    height = labdivc ((long) RawHeight,
    lmul ((long) PixHeight, (long) 10),
    (long) MasterUnits);
  
    height10 = (height % (long)10);
    height /= (long)10;
  
    if ((height10 >= (long)5) || (height10 <= (long)-5))
        if (height10 > 0)
            height++;
        else
            height--;
  
    return ((short) height);
}
/*** Tetra II end ***/
  
/*** Tetra II begin ***/
/*  MakeSizeEscape
*
*  Build and escape string to set the current font size.  Returns
*  the length of the built string.
*/
int FAR PASCAL MakeSizeEscape(lpEsc, pitch, pixels, resolution, rawWidth,
emMasterUnits)
lpESC lpEsc;
BYTE pitch;
short pixels, resolution, rawWidth, emMasterUnits;
{
    long size_100;
    short n, pixWidth;
    char end;
  
    lpEsc->esc = '\033';
    lpEsc->start1 = '(';
    lpEsc->start2 = 's';
  
    if (pitch & (BYTE)0x1)
    {
        size_100 = CalcPtSize((long) pixels, (long) resolution);
        end = 'V';
    }
    else
    {
        pixWidth = ScaleWidth((long) rawWidth, (long) emMasterUnits,
        (long) pixels, (long) resolution);
        size_100 = labdivc((long) resolution, (long) 100, (long) pixWidth);
        end = 'H';
    }
  
    n = MakeEscSize(size_100, (LPSTR)lpEsc->num);
  
    lpEsc->num[n] = end;
    lpEsc->num[n+1] = '\0';
  
    return (n + 4);
}
/*** Tetra II end ***/
  
/*** Tetra begin ***/
/*  lstrind
*
*  Attempt to find a character in a string.  If the character is not
*  found, then return -1, else return the index of the character in
*  the string.
*/
int FAR PASCAL lstrind(string, c)
LPSTR string;
int c;
{
    LPSTR string2;
    int i = 0;
  
    string2 = string;
  
    while ((*string2) && (*string2 != (char) c))
    {
        *string2++;
        i++;
    }
  
    if (*string2 != (char) c)
        i = -1;
  
    return i;
}
/*** Tetra end ***/
  
/*** Tetra begin ***/
/*  hplstrcpyn
*
*  Copies n characters from one string to another.  If the end of the
*  source string has been reached before n characters have been copied,
*  then the destination string is padded with nulls.  Returns the
*  number of characters used from the source string.
*/
int FAR PASCAL hplstrcpyn(string1, string2, n)
LPSTR string1;
LPSTR string2;
int n;
{
    LPSTR s1,
    s2;
    int i = 0;
  
    s1 = string1;
    s2 = string2;
  
    while ((*s2) && (n > 0))
    {
        *s1++ = *s2++;
        i++;
        n--;
    }
  
    while (n > 0)
    {
        *s1++ = '\0';
        n--;
    }
  
    return i;
}
/*** Tetra end ***/
  
/*** Tetra begin ***/
/*  MakeEscSize
*
*  Converts a long integer value to an ascii string with a decimal
*  point two digits to the left.  Returns the length of the string.
*/
int FAR PASCAL MakeEscSize(lsize, string1)
long lsize;
LPSTR string1;
{
    char ipart[20],
    fpart[20];
    int l;
    long n;
  
    n = (long) lsize / (long) 100;
    l = itoa ((int) n, (LPSTR) ipart);
  
    n = (long) lsize % (long) 100;
    l += itoa ((int) n, (LPSTR) fpart);
  
    lstrcat ((LPSTR) ipart, (LPSTR) ".");
    l++;
    lstrcat ((LPSTR) ipart, (LPSTR) fpart);
    lstrcpy (string1, (LPSTR) ipart);
  
    return l;
}
/*** Tetra end ***/
  
/*** Tetra begin ***/
/*  CalcPtSize
*
*  Calculates a pointsize to the nearest quarter point from the
*  pixel height of the font.  Returns the calculated pointsize times 100.
*/
long FAR PASCAL CalcPtSize(PixHeight, VertRes)
long PixHeight;
long VertRes;
{
    long size,
    quarter;
  
    size = labdivc (PixHeight,
    lmul ((long) 72, (long) 100),
    VertRes);
  
    if ((quarter = ((size % (long)100) % (long)25)) > (long)12)
        size += ((long)25 - quarter);
    else
        size -= quarter;
  
    return (size);
}
/*** Tetra end ***/
  
/*** Tetra begin ***/
/*  ScaleWidth
*
*  Returns a rounded width in dots which is calculated from the pixel
*  height and the raw width of the character.  The raw width of the
*  character is in units of emMasterUnits.
*/
short FAR PASCAL ScaleWidth(RawWidth, MasterUnits, PixHeight, VertRes)
long RawWidth;
long MasterUnits;
long PixHeight;
long VertRes;
{
    long width,
    centipix;
  
  
    width = (((RawWidth * (long)15000) / (MasterUnits / 2)) *
    CalcPtSize(PixHeight, VertRes)) /
    (long)7231;
  
  
    centipix = (width % (long)100);
    width /= (long) 100;
  
    DBGtrace(("ScaleWidth: RawWidth= %ld, PixHeight= %ld, VertRes=%ld, centipix= %ld, w= %ld\n",
    RawWidth, PixHeight, VertRes, centipix, width));
  
  
    if (centipix >= (long)50)
        width++;
  
    return ((short) width);
}
/*** Tetra end ***/
  
/*  _lopenp
*
*  Attempt to open a file at lpFileName.  If that fails, then try to
*  append the file name to the same path the driver is on and attempt
*  to open the file again.
*/
int FAR PASCAL _lopenp(lpFileName, IOflag)
LPSTR lpFileName;
WORD IOflag;
{
    OFSTRUCT ofStruct;
    int hFile;
    extern HANDLE hLibInst;
  
    if ((hFile = OpenFile(lpFileName, (LPOFSTRUCT)&ofStruct, IOflag)) > 0)
        return (hFile);
    else
    {
        char modFileName[128];
        LPSTR s, t;
  
        if (!GetModuleFileName(hLibInst,(LPSTR)modFileName,sizeof(modFileName)))
        {
            DBGerr(("_lopenp(): could not get module file name\n"));
            return (0);
        }
  
        /*  Merge the name of the file with the path of the
        *  driver executable file.
        */
        topofpath(s, ((LPSTR)modFileName));
        topofpath(t, lpFileName);
  
        *s = '\0';
  
        /*  Failure if name is too long.
        */
        if ((lstrlen(t) + lstrlen(modFileName)) > sizeof(modFileName))
        {
            DBGerr(("_lopenp(): path name too long, abort\n"));
            return (0);
        }
  
        /*  Merge path + name.
        */
        lstrcpy(s, t);
  
        DBGerr(("_lopenp(): could not open %ls, trying %ls\n",
        lpFileName, (LPSTR)modFileName));
  
        /*  Open file.
        */
        return (OpenFile((LPSTR)modFileName, (LPOFSTRUCT)&ofStruct, IOflag));
    }
}
  
/*  MakeAppName
*
*  Build the application name that heads up the section for the printer
*  driver in the win.ini file.
*/
void FAR PASCAL
MakeAppName(lpModNm, lpPortNm, lpAppNm, nmsz)
LPSTR lpModNm;
LPSTR lpPortNm;
LPSTR lpAppNm;
int nmsz;
{
    LPSTR s;
    int ModNmLen;
  
#if defined(THE_WAY_IT_WAS)     /*----------------------------------*/
    /*  If a module handle is passed in, get the file name for that module
    *  --otherwise assume lpAppNm already has the file name.
    */
  
    if (hMd) {
        GetModuleFileName(hMd, lpAppNm, nmsz);
  
        /*  Strip off leading path name.
        */
        for (s = lpAppNm + lstrlen(lpAppNm);
            (s > lpAppNm) && (s[-1] != ':') && (s[-1] != '\\'); --s)
            ;
  
        /*  Shift the name back over the path.
        */
        lstrcpy(lpAppNm, s);
    }
#else                   /*----------------------------------*/
    lstrcpy(lpAppNm,lpModNm);
#endif                  /*----------------------------------*/
  
  
    ModNmLen = lstrlen(lpModNm);
  
    if (lstrlen(lpPortNm) + ModNmLen + 1 < nmsz)
    {
        lstrcat(lpAppNm, (LPSTR)",");
        lstrcat(lpAppNm, lpPortNm);
  
        /*  Strip off the extension to the file name.
        */
        for (s = lpAppNm + ModNmLen; *s && (*s != '.');  ++s)
            ;
        *s = '\0';
  
        /*  Remove colon from end of port name if there is one.
        */
        s = lpAppNm + lstrlen(lpAppNm);
        if (*(--s) == ':')
            *s = '\0';
    }
}
  
/***************************************************************************/
/********************************   Local   ********************************/
  
  
/*  reverse() -
s1 points to the end of the string and moves toward the beginning
s points to the beginning and moves to the end until s >= s1
*/
static int reverse(s)
LPSTR s;
{
    register char c, far *s1;
  
    s1 = s;
    while (*s1)
        s1++;
    for (--s1; s < s1; s++, s1--)
    {
        c = *s;
        *s = *s1;
        *s1 = c;
    }
    return (0);
}

