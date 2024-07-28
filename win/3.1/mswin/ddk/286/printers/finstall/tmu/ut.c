/**[f******************************************************************
* ut.c -
*
* Copyright (C) 1987,1988,1989,1990 Compugraphic Corporation.
* Copyright (C) 1989,1990,1991 Hewlett-Packard Company.
*     All rights reserved.
*     Company confidential.
*
*  3 Feb 92 Changed $ include files to _ include files
**f]*****************************************************************/

/*
 * $Header: 
 */

/*
 * $Log:
 */
  
//#define DEBUG
  
#include    <windows.h>
#ifdef NODEF
#include    <stdio.h>
#include    <fcntl.h>
#include    <io.h>
#include    <string.h>
#endif
#include    <sys/types.h>
#include    <sys/stat.h>
#include    <ctype.h>
#include    "debug.h"
#include    "_cgifwin.h"
#include    "_ut.h"
  
  
/****************************************************************************\
*                          Debug Definitions
\****************************************************************************/
  
  
#ifdef DEBUG
    #define DBGutil(msg)           /*DBMSG(msg)*/
#else
    #define DBGutil(msg)           /*null*/
#endif
  
  
/****************************************************************************\
*
\****************************************************************************/
  
  
/*------------------*/
/*   UTfileSize     */
/*------------------*/
/*  Return the size in bytes of a file. Returns 0 if file doesn't
*  exist or an error.
*/
GLOBAL ULONG
UTfileSize(pathName)
LPSTR pathName;
{
    int f;
    struct stat st;
  
    if ((f = _lopen((LPSTR)pathName, 0 /*READ*/)) == -1)
        return (ULONG)0;
    if(myfstat(f, (LPSTAT)&st))
    {
        _lclose(f);
        return (ULONG)0;
    }
    _lclose(f);
    return st.st_size;
}
  
  
  
  
/*----------------------*/
/*     UTgetString      */
/*----------------------*/
/* Copy a string and make sure it's null terminated */
GLOBAL VOID
UTgetString(out_strng, in_strng, num)
LPSTR in_strng, out_strng;
WORD num;
{
    WORD i;
  
    lstrncpy(out_strng, in_strng,num);
    for(i=num-1; i>=0; i--)
    {
        if(isgraph(out_strng[i]))
        {
            out_strng[i+1] = '\0';
            return;
        }
    }
}
  
int FAR PASCAL myfstat(hFile, lpStat)
int hFile;
LPSTAT  lpStat ;
{
    long lStartPosition;
    long lNewPosition;
    int status = FALSE;
  
    if ((lStartPosition = _llseek(hFile, 0L, 1)) == -1)
        status = TRUE;
    if ((lpStat->st_size = _llseek(hFile, 0L, 2)) == -1)
        status = TRUE;
    if ((lNewPosition = _llseek(hFile, lStartPosition, 0)) == -1)
        status = TRUE;
    if (lNewPosition != lStartPosition)
        status = TRUE;
    return status;
}
  
int FAR PASCAL lstrncmp(str1, str2, n)
LPSTR str1, str2;
int n;
{
    while (n)
    {
        if (*str1 < *str2)
            return(-1);
        if (*str1 > *str2)
            return(1);
        ++str1; ++str2; --n;
    }
    return(0);
}
  
int FAR PASCAL lstrncmpi(str1, str2, n)
LPSTR str1, str2;
int n;
{
    char ch1, ch2;
  
    while (n)
    {
        ch1 = *str1;
        ch2 = *str2;
        if (IsCharUpper(ch1))
            ch1 = ch1+32;
        if (IsCharUpper(ch2))
            ch2 = ch2+32;
        if (ch1 < ch2)
            return(-1);
        if (ch1 > ch2)
            return(1);
        ++str1; ++str2; --n;
    }
    return(0);
}
  
int FAR PASCAL lmemcmp(str1, str2, n)
LPSTR str1, str2;
int n;
{
    while (n)
    {
        if (*str1 != *str2)
            return(1);
        ++str1; ++str2; --n;
    }
    return(0);
}
  
LPSTR FAR PASCAL lstrncpy(str1, str2, n)
LPSTR str1, str2;
int n;
{
    while (n)
    {
        *str1 = *str2;
        if (*str2 == '\0')
            break;
        ++str1; ++str2; --n;
    }
    return(str1);
}
  
LPSTR FAR PASCAL lstrncat(str1, str2, n)
LPSTR str1, str2;
int n;
{
    int l = lstrlen(str1);
    LPSTR p;
  
    p = str1+l;
    while (l < n-1)
    {
        *p = *str2;
        if (*str2 == '\0')
            break;
        ++p; ++str2; ++l;
    }
    *p = '\0';
    return(str1);
}
  
/*  atol
*
*  Convert ascii to long integer.
*/
long FAR PASCAL atol(s)
LPSTR s;
{
    long n, i;
  
    for (i = n = 0;; n *= 10)
    {
        n += s[i] - '0';
        if (!s[++i])
            break;
    }
    return n;
}
#ifdef NODEF
/*  atoi
*
*  Convert ascii to integer.
*/
int FAR PASCAL atoi(s)
LPSTR s;
{
    short n, i;
    DBGutil(("   inside atoi in UT.C"));
  
    for (i = n = 0;; n *= 10)
    {
        n += s[i] - '0';
        if (!s[++i])
            break;
    }
    return n;
}
#endif
  
VOID FAR PASCAL MyFreeMem(hBuf)
HANDLE FAR *hBuf;
{
  
    while ((GlobalFlags(*hBuf) & GMEM_LOCKCOUNT) > 0)
        GlobalUnlock(*hBuf);
  
    if(*hBuf)
    {
        GlobalFree(*hBuf);
        *hBuf = 0;
    }
}
  
  
LONG FAR PASCAL myfilelength(hFile)
int hFile;
{
    long lStartPosition;
    long lNewPosition;
    int status = 0;
    LONG size;
  
    if (((lStartPosition = _llseek(hFile, 0L, 1 /*from current*/)) == -1)||
        ((size = _llseek(hFile, 0L, 2 /*from end*/)) == -1)||
        ((lNewPosition = _llseek(hFile, lStartPosition, 0 /*from beginning*/)) == -1)||
        (lNewPosition != lStartPosition))
        return((LONG)-1);
  
    return (size);
}
  
  
/****************************************************************************\
*  New routines to windowize loader
\****************************************************************************/
  
VOID FAR PASCAL MyRewind(hFile)
int hFile;
{
    _llseek(hFile, 0L, LSEEK_SET);
}
  
LONG FAR PASCAL myftell(hFile)
int hFile;
{
    return(_llseek(hFile, 0L, LSEEK_CUR));
}
  
int FAR PASCAL myaccess(name, mode)
LPSTR name;
int mode;
{
    int hFile;
  
    if ((hFile =_lopen(name, OF_READ)) == -1)
        return(-1);
    _lclose(hFile);
    return(0);
}
  
