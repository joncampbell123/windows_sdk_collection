/**[f******************************************************************
* utils.c -
*
* Copyright (C) 1988,1989 Aldus Corporation
* Copyright (C) 1988-1990 Microsoft Corporation.
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

/********************************   UTILS   ********************************/
/*
*   9-04-91    rk (HP) In _lopenp checked if OpenFile return was >=0 not just >0
*   1-13-89    jimmat  Reduced # of redundant strings by adding lclstr.h
*   1-25-89    jimmat  Use hLibInst instead of calling GetModuleHandle()-
*                      lclstr.h no longer required by this file.
*   1-26-89    jimmat  Modified for use by FINSTALL--unused code removed.
*   2-20-89    jimmat  Font Installer/Driver use same WIN.INI section (again)!
*/
/***************************************************************************/
  
  
//#define DEBUG
  
#include "printer.h"
#include "resource.h"
#include "utils.h"
#include "debug.h"
#include "ctype.h"
  
  
/****************************************************************************\
* Debug Definitions
\****************************************************************************/
  
#ifdef DEBUG
       #define  DBGutil(msg)      /*DBMSG(msg)*/
#else
       #define  DBGutil(msg)      /*null*/
#endif
  
  
static int reverse(LPSTR);
  
  
#define topofpath(s,lpFileName) \
for (s = lpFileName + lstrlen(lpFileName); \
    (s > lpFileName) && (s[-1] != ':') && (s[-1] != '\\'); --s)
  
  
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
  
  
/*  lmod
*
*  Long mod:  this an auxilary routine used to access the
*  long arithmetic library functions as a FAR procedure.
*/
long FAR PASCAL lmod(lval1, lval2)
long lval1;
long lval2;
{
    return(lval1%lval2);
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
    do      /* generate digits in reverse order */
    {
        s[i++] = (char)(n % 10 + '0');
    } while (n /= 10);
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
    return i;
}
  
  
/*********************************************************
*
*    myatoi:
*              Convert ascii to integer.
*
*********************************************************/
int FAR PASCAL myatoi(s)
LPSTR s;
{
    short n = 0, i = 0, sign = 1;
    DBGutil(("            inside myatoi in UTILS.C\n"));
  
    while((s[i] == ' ') || (s[i] == '\t'))
    {
        ++i;
        if (!s[i])
            return n * sign;
    }
    if (s[i] == '-')
    {
        sign = -1;
        ++i;
    }
    else if (s[i] == '+')
        ++i;
  
    while (isdigit(s[i]))
    {
        n *= 10;
        n += s[i] - '0';
        ++i;
    }
    return n * sign;
}
  
  
  
/*  ltoa
*
*  Convert long integer to ascii text.
*/
long FAR PASCAL ltoa(n, s)
long n;
LPSTR s;
{
    long i, sign;
  
    if ((sign = n) < 0)
        n = -n;
    i = 0;
    do      /* generate digits in reverse order */
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
  
    DBGutil(("            inside atoi in UTILS.C\n"));
  
    for (i = n = 0;; n *= 10)
    {
        n += s[i] - '0';
        if (!s[++i])
            break;
    }
    return n;
}
  
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
  
//    if ((hFile = OpenFile(lpFileName, (LPOFSTRUCT)&ofStruct, IOflag)) > 0)
    if ((hFile = OpenFile(lpFileName, (LPOFSTRUCT)&ofStruct, IOflag)) >= 0)
        return (hFile);
    else
    {
        char modFileName[128];
        LPSTR s, t;
  
        if (!GetModuleFileName(hLibInst,(LPSTR)modFileName,sizeof(modFileName)))
        {
            DBGutil(("_lopenp(): could not get module file name\n"));
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
            DBGutil(("_lopenp(): path name too long, abort\n"));
            return (0);
        }
  
        /*  Merge path + name.
        */
        lstrcpy(s, t);
  
        DBGutil(("_lopenp(): could not open %ls, trying %ls\n",
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
  
    lstrcpy(lpAppNm,lpModNm);
  
    /*  Strip off any extension to the file name.
    */
    for (s = lpAppNm; *s && (*s != '.');  ++s)
        ;
    *s = '\0';
  
    if (lstrlen(lpPortNm) + lstrlen(lpAppNm) + 1 < nmsz)
    {
        lstrcat(lpAppNm, (LPSTR)",");
        lstrcat(lpAppNm, lpPortNm);
  
        /*  Remove colon from end of port name if there is one.
        */
        s = lpAppNm + lstrlen(lpAppNm);
        if (*(--s) == ':')
            *s = '\0';
    }
}
  
  
/*** Tetra begin ***/
/*  lstrcpyn
*
*  Copies n characters from one string to another.  If the end of the
*  source string has been reached before n characters have been copied,
*  then the destination string is padded with nulls.  Returns the
*  number of characters used from the source string.
*/
int FAR PASCAL lstrcpyn(string1, string2, n)
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
  
  
/***************************************************************************
  
Routine Title: duconvert
  
**   Converts TFM design units to PFM dots  **
  
****************************************************************************/
  
WORD FAR PASCAL duconvert(struct TFMType far *TFM, int ptsize, short duval)
  
{
    //   int dotval;
    //   double nptsize, point, desUnits;
    //
    //
    //
    //   /* Values needed to convert design units */
    //
    //   nptsize = TFM->typeface->typefaceMetrics.nominalPointSize;
    //   point = TFM->typeface->typefaceMetrics.point;
    //   desUnits = TFM->typeface->typefaceMetrics.designUnits;
    //
    //   dotval = (int)((point * nptsize * (duval / desUnits)) * DEV_RES);
    //   return (dotval);
  
    double desunits;
    double point;
  
    desunits = (double)TFM->typeface->typefaceMetrics.designUnitsN /
    (double)TFM->typeface->typefaceMetrics.designUnitsD;
    point = (double)TFM->typeface->typefaceMetrics.pointN /
    (double)TFM->typeface->typefaceMetrics.pointD;
  
    return((WORD)(((double)((LONG)duval * (LONG)ptsize * 300L)
    * point / desunits)));
  
    //     return((WORD)ddiv(
    //  dmul(
    //       dmul(
    //        dmul(
    //             dmul(
    //              (LONG)duval,
    //              TFM->typeface->typefaceMetrics.designUnitsD),
    //             (LONG)gPtSize),
    //        TFM->typeface->typefaceMetrics.pointN),
    //       (LONG)DEV_RES),
    //  dmul(
    //       TFM->typeface->typefaceMetrics.designUnitsN,
    //       TFM->typeface->typefaceMetrics.pointD)
    //  ));
}
  
/***************************************************************************
  
Routine Title: fdiv5
  
Uses floating point ops to retain significant digits while multiplying
three longs in the numerator and two longs in the denominator.  Returns
result of fraction as a WORD
  
****************************************************************************/
  
WORD FAR PASCAL fdiv5(long n1, long n2, long n3, long d1, long d2)
  
{
    return((WORD)(((double)n1 * (double)n2 * (double)n3) /
    ((double)d1 * (double)d2)));
}
/*** Tetra end ***/
  
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
