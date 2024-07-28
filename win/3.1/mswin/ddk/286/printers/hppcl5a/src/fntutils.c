/**[f******************************************************************
* fntutils.c -
*
* Copyright (C) 1988,1989 Aldus Corporation.  All rights reserved.
* Copyright (C) 1989-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* Company confidential.
*
**f]*****************************************************************/
  
/***************************************************************************/
/*******************************   FntUtils.c   ****************************/
/*
*  FntUtils:  Utilities shared by the soft font installer and fontbld.
*/
  
// History
//  03 feb 90   KLO         Changed lmemcpy() to lstrncpy()
//  09 aug 89   peterbe     Removed infinite in NextWinSF() and
//                  NextWinCart() to fix problem of
//                  finding cartridges after many soft
//                  fonts.
//  07 aug 89   peterbe     Changed lstrcmp() to lstrcmpi().
//  01 aug 89   peterbe     Removed declarations of _llseek() etc.
//  16 may 89   peterbe     added () in #define WINSF_HEADLEN ...
//  27 apr 89   peterbe     Tabs = 8, other formatting.
  
#include "nocrap.h"
#undef NOMEMMGR
#include "windows.h"
#include "fntutils.h"
#include "neededh.h"
#include "pfm.h"
#include "resource.h"
  
extern HANDLE hLibInst;
  
  
/*  Local debug stuff.
*/
#ifdef DEBUG
    #define LOCAL_DEBUG
#endif
  
#ifdef LOCAL_DEBUG
    #define DBGerr(msg) DBMSG(msg)
    #define DBGwinsf(msg) DBMSG(msg)
#else
    #define DBGerr(msg) /*null*/
    #define DBGwinsf(msg) /*null*/
#endif
  
  
#define LOCAL static
  
#define SF_LABEL    "SoftFont"
#define CART_LABEL  "Cartridge"
#define FIRST_KEYLEN    2048
#define STEP_KEYLEN 1024
#define MIN_FREESPACE   20
#define WINSF_HEADLEN   (64+4+4)    /* length of header in bytes */
#define WINCART_HEADLEN WINSF_HEADLEN
  
typedef struct {
    /* NOTE: if you change the header, change WINSF_HEADLEN */
    char appName[64];       /* Application name in win.ini */
    DWORD offsKeyName;      /* offset to key name */
    DWORD len;          /* length in bytes of key names buffer */
    char keyNames[1];       /* buffer of key names */
} WINSF, WINCART;
  
typedef WINSF FAR * LPWINSF;
typedef WINCART FAR * LPWINCART;
  
/*  Forward local procs
*/
LOCAL int countFreeSpace(LPSTR, DWORD);
LOCAL BOOL softFontLabel(LPSTR);
LOCAL BOOL CartridgeLabel(LPSTR);
LOCAL int getID(LPSTR);
  
/**************************************************************************/
/****************************   Global Procs   ****************************/
  
  
/*  InitWinSF
*
*  Create and initialize the structure which contains all the key names
*  listed in the win.ini file.
*
*  Since there is no simple way to figure out how many key names are
*  listed in the win.ini, the following approach is used:
*
*      1. Randomly allocate a block of memory and ask for a list
*         of key names.
*
*      2. If the list comes close to the end of the block of memory,
*         assume Windows had to truncate the list, so re-allocate
*         a bigger block of memory.
*
*      3. Repeat steps 1 and 2 until a complete list is read from
*         the win.ini file.
*
*  If the proc fails to get a complete list, it dumps whatever it did get
*  and returns failure.
*/
HANDLE FAR PASCAL InitWinSF(lpAppName)
LPSTR lpAppName;
{
    LPSTR lpKey = 0L;
    HANDLE hWinSF = 0;
    LPWINSF lpWinSF = 0L;
    DWORD size = FIRST_KEYLEN;
    int free = 0;
    int loop = 0;
#ifdef DEBUG_FUNCT
    DB(("Entering InitWinSF\n"));
#endif
  
    DBGwinsf(("InitWinSF(%lp): %ls\n", lpAppName, lpAppName));
  
    /*  First allocate a block for the list of key names.
    */
    hWinSF = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, size);
  
    do {
        /*  Lock down the key names or fail.
        */
        if (!hWinSF || !(lpWinSF = (LPWINSF)GlobalLock(hWinSF)))
            break;
  
        /*  Initialize the header.
        */
        /* TETRA -- changed lmemcpy to lstrncpy -- KLO */
        lstrncpy(lpWinSF->appName, lpAppName, sizeof(lpWinSF->appName)-1);
        lpWinSF->offsKeyName = 0L;
  
        /*  Pick up the size of the buffer for key names.
        */
        lpWinSF->len = size - WINSF_HEADLEN;
        lpKey = lpWinSF->keyNames;
  
        /*  Get the list of key names and count how much space
        *  we have left over.
        */
        GetProfileString(lpAppName, (LPSTR)0L, lpKey, lpKey, (int)lpWinSF->len);
        free = countFreeSpace(lpKey, lpWinSF->len);
  
        /*  Unlock the key names.
        */
        GlobalUnlock(hWinSF);
        lpWinSF = 0L;
        lpKey = 0L;
  
        /*  If it does not look like there was enough space at the end
        *  of the list, then allocate a larger block and loop again.
        */
        if (free < MIN_FREESPACE)
        {
            GlobalFree(hWinSF);
            size += STEP_KEYLEN;
            hWinSF = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, size);
            DBGwinsf(
            ("...InitWinSF(): re-allocate memory block to %ld bytes\n", size));
        }
    } while (free < MIN_FREESPACE && ++loop < 16);
  
    /*  If we failed before we got the whole list, then free up the
    *  struct and return zero (0), otherwise return the handle to
    *  the allocated struct.
    */
    if (free < MIN_FREESPACE && hWinSF)
    {
        GlobalFree(hWinSF);
        hWinSF = 0;
    }
  
    DBGwinsf(("...end of InitWinSF(), return %d\n", (WORD)hWinSF));
  
#ifdef DEBUG_FUNCT
    DB(("Exiting InitWinSF\n"));
#endif
    return (hWinSF);
}
  
/*  NextWinSF
*
*  Advance to the next key name in the win.ini file, make sure no
*  duplicates are read, the entry is non-null, and it contains a
*  valid font id.
*/
int FAR PASCAL NextWinSF(hWinSF, lpBuf, size)
HANDLE hWinSF;
LPSTR lpBuf;
WORD size;
{
    register LPSTR lpKey = 0L;
    LPWINSF lpWinSF = 0L;
    LPSTR lpPrevKey = 0L;
    //int infinite = 0;
    int id = -1;
#ifdef DEBUG_FUNCT
    DB(("Entering NextWinSF\n"));
#endif
  
    lmemset(lpBuf, 0, size);
  
    DBGwinsf(("NextWinSF(%d,%lp,%d)\n", (WORD)hWinSF, lpBuf, size));
  
    if (!hWinSF || !(lpWinSF = (LPWINSF)GlobalLock(hWinSF)))
        return (-1);
  
    lpKey = lpWinSF->keyNames + lpWinSF->offsKeyName;
  
    do {
        if (!(*lpKey))
            break;
  
        DBGwinsf(("NextWinSF(): examining %ls\n", lpKey));
  
        /*  Traverse all key names up to this one looking for any
        *  duplicates.
        */
        for (lpPrevKey = lpWinSF->keyNames; lpPrevKey < lpKey; )
        {
            if (lstrcmpi(lpPrevKey, lpKey) == 0)
                break;
  
            for (; *lpPrevKey; ++lpPrevKey)
                ;
            ++lpPrevKey;
        }
  
        if (lpPrevKey < lpKey)
        {
            DBGerr(
            ("...NextWinSF(): duplicate key name %ls, skipping\n", lpPrevKey));
            goto dup_keyname;
        }
  
        /*  If a legitimate soft font label, read it from the win.ini and
        *  copy to the caller's buffer.  Pick off the id and update our
        *  lpKeyName pointer for the next time we get called.
        */
        if (*lpKey && softFontLabel(lpKey))
        {
            *lpBuf = '\0';
  
            GetProfileString(lpWinSF->appName, lpKey, lpBuf, lpBuf, size);
  
            if (*lpBuf)
                id = getID(lpKey);
        }
  
        dup_keyname:
        /*  Advance pointer to the next key name in the list.
        */
        for (; *lpKey; ++lpKey)
            ;
        ++lpKey;
  
        lpWinSF->offsKeyName = lpKey - (LPSTR)lpWinSF->keyNames;
  
    } while (*lpKey && (id < 0) /* && ++infinite < 64*/);
  
    GlobalUnlock(hWinSF);
    lpWinSF = 0L;
  
#ifdef DEBUG_FUNCT
    DB(("Exiting NextWinSF\n"));
#endif
    return (id);
}
  
/*  EndWinSF
*
*  Done with the list of key names, free up our struct.
*/
int FAR PASCAL EndWinSF(hWinSF)
HANDLE hWinSF;
{
#ifdef DEBUG_FUNCT
    DB(("Entering EndWinSF\n"));
#endif
    DBGwinsf(("EndWinSF(%d)\n", (WORD)hWinSF));
  
    if (!hWinSF)
        return (-1);
  
#ifdef DEBUG_FUNCT
    DB(("Exiting EndWinSF\n"));
#endif
    return (GlobalFree(hWinSF));
}
  
/*  NextWinCart
*
*  Advance to the next key name in the win.ini file, make sure no
*  duplicates are read, the entry is non-null, and it contains a
*  valid cartridge id.
*/
int FAR PASCAL NextWinCart(hWinCart, lpBuf, size)
HANDLE hWinCart;
LPSTR lpBuf;
WORD size;
{
    register LPSTR lpKey = 0L;
    LPWINCART lpWinCart = 0L;
    LPSTR lpPrevKey = 0L;
    //int infinite = 0;
    int id = -1;
  
#ifdef DEBUG_FUNCT
    DB(("Entering NextWinCart\n"));
#endif
    lmemset(lpBuf, 0, size);
  
    DBGwinsf(("NextWinCart(%d,%lp,%d)\n", (WORD)hWinCart, lpBuf, size));
  
    if (!hWinCart || !(lpWinCart = (LPWINCART)GlobalLock(hWinCart)))
        return (-1);
  
    lpKey = lpWinCart->keyNames + lpWinCart->offsKeyName;
  
    do {
        if (!(*lpKey))
            break;
  
        DBGwinsf(("NextWinCart(): examining %ls\n", lpKey));
  
        /*  Traverse all key names up to this one looking for any
        *  duplicates.
        */
        for (lpPrevKey = lpWinCart->keyNames; lpPrevKey < lpKey; )
        {
            if (lstrcmpi(lpPrevKey, lpKey) == 0)
                break;
  
            for (; *lpPrevKey; ++lpPrevKey)
                ;
            ++lpPrevKey;
        }
  
        if (lpPrevKey < lpKey)
        {
            DBGerr(
            ("...NextWinCart(): duplicate key name %ls, skipping\n", lpPrevKey));
            goto dup_keyname;
        }
  
        /*  If a legitimate cartridge label, read it from the win.ini and
        *  copy to the caller's buffer.  Pick off the id and update our
        *  lpKeyName pointer for the next time we get called.
        */
        if (*lpKey && CartridgeLabel(lpKey))
        {
            *lpBuf = '\0';
  
            GetProfileString(lpWinCart->appName, lpKey, lpBuf, lpBuf, size);
  
            if (*lpBuf)
                id = getID(lpKey);
        }
  
        dup_keyname:
        /*  Advance pointer to the next key name in the list.
        */
        for (; *lpKey; ++lpKey)
            ;
        ++lpKey;
  
        lpWinCart->offsKeyName = lpKey - (LPSTR)lpWinCart->keyNames;
  
    } while (*lpKey && (id < 0) /*&& ++infinite < 64*/);
  
    GlobalUnlock(hWinCart);
    lpWinCart = 0L;
  
    DBMSG(("NextWinCart: Cartridge%d=%ls\n",id,lpBuf));
  
#ifdef DEBUG_FUNCT
    DB(("Exiting NextWinCart\n"));
#endif
    return (id);
}
  
/*
*  GetCartName
*
*  Reads a cartridge name from a PCM file.  Returns 0 if the
*  PCM cannot be found or is invalid.
*/
int FAR PASCAL GetCartName(LPSTR szPCM, LPSTR lpch, WORD cch)
{
    int hFile;
    PCMHEADER pcmheader;
    int cchRead=0;
  
#ifdef DEBUG_FUNCT
    DB(("Entering GetCartName\n"));
#endif
    if (HIWORD(szPCM))
    {
        hFile=_lopen(szPCM,0);
    }
    else
    {
        /* if the hiword (selector) is zero, the PCM is in the HPPCL
        * driver's resources.
        */
  
        hFile = FindResource(hLibInst,szPCM,MAKEINTRESOURCE(PCMFILE));
        if (!hFile)
            return 0;
  
        hFile = AccessResource(hLibInst,hFile);
    }
  
    if (hFile<0)
        return 0;
  
    if (_lread(hFile,(LPSTR)&pcmheader,sizeof(PCMHEADER))!=sizeof(PCMHEADER)
        || pcmheader.pcmMagic != PCM_MAGIC
        || pcmheader.pcmVersion != PCM_VERSION)
    {
        _lclose(hFile);
        return 0;
    }
  
    _llseek(hFile, pcmheader.pcmTitle-sizeof(PCMHEADER), 1);
  
    while (cch--)
    {
        if (_lread(hFile,lpch,1)!=1)
        {
            _lclose(hFile);
            return 0;
        }
        else
            cchRead++;
        if (!*lpch++)
            break;
    }
  
    _lclose(hFile);
  
#ifdef DEBUG_FUNCT
    DB(("Exiting GetCartName\n"));
#endif
    return cchRead;
  
}
  
/*  CountKeyNames
*
*  Count the number of key names in the key name list.  Each key
*  name is ended with a null and the last key word in the list is
*  ended by two nulls (see Windows' GetProfileString()).
*/
WORD FAR PASCAL CountKeyNames(hWinCart)
HANDLE hWinCart;
{
    register LPSTR lpKey = 0L;
    register WORD count = 0;
    LPWINCART lpWinCart = 0L;
#ifdef DEBUG_FUNCT
    DB(("Entering CountKeyNames\n"));
#endif
  
    if (!hWinCart || !(lpWinCart = (LPWINCART)GlobalLock(hWinCart)))
        return (0);
    lpKey = lpWinCart->keyNames;
  
    for (count=0; *lpKey; ++count)
    {
        for (; *lpKey; ++lpKey)
            ;
        ++lpKey;
    }
  
    GlobalUnlock(hWinCart);
    lpWinCart = 0L;
  
    DBGwinsf(("CountKeyNames(%d), return %d\n", (WORD)hWinCart, count));
  
#ifdef DEBUG_FUNCT
    DB(("Exiting CountKeyNames\n"));
#endif
    return (count);
}
  
/**************************************************************************/
/*****************************   Local Procs   ****************************/
  
  
/*  countFreeSpace
*
*  Count the number of NULL bytes at the end of a string.
*/
LOCAL int countFreeSpace(lpKey, size)
LPSTR lpKey;
DWORD size;
{
    LPSTR lpEnd = lpKey + size;
    int count = 0;
  
#ifdef DEBUG_FUNCT
    DB(("Entering countFreeSpace\n"));
#endif
    while (--lpEnd > lpKey && !(*lpEnd))
        ++count;
    count -= 2;
  
    DBGwinsf(("countFreeSpace(%lp,%ld), return %d\n",
    lpKey, (DWORD)size, (WORD)count));
  
#ifdef DEBUG_FUNCT
    DB(("Exiting countFreeSpace\n"));
#endif
    return (count);
}
  
/*  CartridgeLabel
*
*  Return TRUE if the passed string is a soft font label.
*/
LOCAL BOOL CartridgeLabel(lpName)
LPSTR lpName;
{
    char name[sizeof(CART_LABEL)];
    char label[sizeof(CART_LABEL)];
    short ind;
  
#ifdef DEBUG_FUNCT
    DB(("Entering CartridgeLabel\n"));
#endif
    DBGwinsf(("CartridgeLabel(%ls)\n", lpName));
  
    if (!(*lpName))
        return FALSE;
  
    /*  Copy name and label into work buffers.
    */
    lstrcpy((LPSTR)label, CART_LABEL);
    /* TETRA -- changed lmemcpy to lstrncpy -- KLO */
    lstrncpy((LPSTR)name, lpName, sizeof(CART_LABEL) - 1);
    name[sizeof(CART_LABEL) - 1] = '\0';
  
    /*  Knock digits off the name.
    */
    for (ind = lstrlen((LPSTR)name);
        ind > 0 && name[ind-1] >= '0' && name[ind-1] <= '9';
        name[--ind] = '\0')
        ;
  
    /*  Compare label to name.
    */
    if (ind > 0)
    {
        label[ind] = '\0';
  
        AnsiUpper((LPSTR)name);
        AnsiUpper((LPSTR)label);
  
        if (lstrcmpi((LPSTR)name, (LPSTR)label))
            return FALSE;
        else
            return TRUE;
    }
  
#ifdef DEBUG_FUNCT
    DB(("Exiting CartridgeLabel\n"));
#endif
    return FALSE;
}
  
/*  softFontLabel
*
*  Return TRUE if the passed string is a soft font label.
*/
LOCAL BOOL softFontLabel(lpName)
LPSTR lpName;
{
    char name[sizeof(SF_LABEL)];
    char label[sizeof(SF_LABEL)];
    short ind;
  
#ifdef DEBUG_FUNCT
    DB(("Entering softFontLabel\n"));
#endif
    DBGwinsf(("softFontLabel(%ls)\n", lpName));
  
    if (!(*lpName))
        return FALSE;
  
    /*  Copy name and label into work buffers.
    */
    lstrcpy((LPSTR)label, SF_LABEL);
    /* TETRA -- changed lmemcpy to lstrncpy -- KLO */
    lstrncpy((LPSTR)name, lpName, sizeof(SF_LABEL) - 1);
    name[sizeof(SF_LABEL) - 1] = '\0';
  
    /*  Knock digits off the name.
    */
    for (ind = lstrlen((LPSTR)name);
        ind > 0 && name[ind-1] >= '0' && name[ind-1] <= '9';
        name[--ind] = '\0')
        ;
  
    /*  Compare label to name.
    */
    if (ind > 0)
    {
        label[ind] = '\0';
  
        AnsiUpper((LPSTR)name);
        AnsiUpper((LPSTR)label);
  
        if (lstrcmpi((LPSTR)name, (LPSTR)label))
            return FALSE;
        else
            return TRUE;
    }
  
#ifdef DEBUG_FUNCT
    DB(("Exiting softFontLabel\n"));
#endif
    return FALSE;
}
  
/*  getID
*
*  Peel the ID number off of the "SoftFontn" name.
*/
LOCAL int getID(lpKey)
LPSTR lpKey;
{
    register LPSTR ch = 0L;
    register int id = -1;
#ifdef DEBUG_FUNCT
    DB(("Entering getID\n"));
#endif
  
    /*  Traverse to first number in key name.
    */
    for (ch = lpKey; *ch && (*ch < '0' || *ch > '9'); ++ch)
        ;
  
    if (*ch)
    {
        /*  Extract ID number from key name.
        */
        for (id = 0; *ch && (*ch >= '0') && (*ch <= '9'); ++ch)
            id = (id * 10) + (*ch - '0');
    }
  
    DBGwinsf(("getID(%ls), return %d\n", lpKey, id));
  
#ifdef DEBUG_FUNCT
    DB(("Exiting getID\n"));
#endif
    return (id);
}
