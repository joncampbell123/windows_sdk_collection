/**[f******************************************************************
* $support.c -
*
* Copyright (C) 1990,1991 Hewlett-Packard Company.
*     All rights reserved.
*     Company confidential.
*  2 feb 92 rk Changed $ include files to _ include files
*
**f]*****************************************************************/
/*
 * $Header: 
 */

/*
 * $Log:
 */
  
//#define DEBUG
  
#include "_windows.h"
#include "_kludge.h"
#include "debug.h"
#include "_strings.h"
#include "_support.h"
#include "_cgifwin.h"
#include "_tmu.h"
#include "_sfidlg.h"
#include "_sfdir.h"
  
/****************************************************************************\
* Debug Definitions
\****************************************************************************/
  
#ifdef DEBUG
    #define DBGentry(msg)      /*DBMSG(msg)*/
    #define DBGquery(msg)      /*DBMSG(msg)*/
    #define DBGerr(msg)        /*DBMSG(msg)*/
    #define DBGtrace(msg)      /*DBMSG(msg)*/
    #define DBGconfig(msg)     /*DBMSG(msg)*/
#else
    #define DBGentry(msg)      /*null*/
    #define DBGquery(msg)      /*null*/
    #define DBGerr(msg)        /*null*/
    #define DBGtrace(msg)      /*null*/
    #define DBGconfig(msg)     /*null*/
#endif
  
/****************************************************************************\
* Local Definitions
\****************************************************************************/
  
#define MAX_PATHNAME_SIZE MAX_FILENAME_SIZE - 14
#define BUFFER_SIZE 10240
  
/****************************************************************************\
* Forward References for Local Procedures
\****************************************************************************/
  
VOID FAR PASCAL BlinkStatusLine(HWND, int, LPSTR);
BOOL FAR PASCAL QueryCopyFile(LPSTR, LPSTR, HWND, LPSTR);
BOOL FAR PASCAL unlink(LPSTR);
int FAR PASCAL MyMessageBox(HWND, WORD, LPSTR, WORD);
  
  
/****************************************************************************\
* Global Variables
\****************************************************************************/
  
extern HANDLE hLibInst;
extern char gIndexFile[MAX_FILENAME_SIZE];
BOOL gHQ3exist;
BOOL gIFexist;
BOOL gSSexist;
char gBulletIntellifont[] = {"Intellifont"};
char gSupportFiles[] = {"SupportFiles"};
char gBowStatus[] = {"ifwstatus"};
char gSymbolFile[MAX_FILENAME_SIZE];
char gNull[] = {""};
  
extern char gSupportFileDirectory[MAX_FILENAME_SIZE];
extern char gTypeDirectory[MAX_FILENAME_SIZE];
  
/****************************************************************************\
* Procedures
\****************************************************************************/
  
  
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*  Function:   ConfigBullet
*   Written:   06/22/90
*        By:   Robin Murray
*
*      Desc:   Get Bullet type directory, IF.FNT file, Default Source drive
*
*    Return:   None
*
*   Globals:   None
*
* Called By:   PFdlgFn
*      SFdlgFn
*
*  Calls To:
*
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
  
BOOL FAR PASCAL ConfigBullet(hDB, hMd, lpAppName, lpIndexFile, lpTypeDirectory,
lpSourcePath, lpSupportFileDirectory)
HWND hDB;
HANDLE hMd;
LPSTR lpAppName;
LPSTR lpIndexFile;
LPSTR lpTypeDirectory;
LPSTR lpSourcePath;
LPSTR lpSupportFileDirectory;
{
    char lpUpdateFile[MAX_FILENAME_SIZE];
    char gHomeDirectory[MAX_FILENAME_SIZE];
    LPSTR lpHomeDirectory;
    int hFile;
    HANDLE hModule;
    LPSTR p;
    char StatusLine[128];
    BOOL FirstTimeInstallation = FALSE;
    LPSTR lpSymbolFile = (LPSTR) gSymbolFile;
    char Message[128];
    int i;
    BOOL status;
  
    DBGentry(("ConfigureBullet in support\n"));
  
    DBGconfig(("ConfigBullet\n"));
    lpHomeDirectory = (LPSTR) gHomeDirectory;
    *lpHomeDirectory = '\0';
    GetModuleFileName(hLibInst, lpHomeDirectory, MAX_FILENAME_SIZE);
    DBGconfig(("Module File Name = %ls\n", lpHomeDirectory));
    p = lpHomeDirectory+lstrlen(lpHomeDirectory);
    while(p > lpHomeDirectory)
    {
        if (*p == '\\')
        {
            *p = '\0';
            break;
        }
        --p;
    }
    DBGconfig(("HomeDirectory = %ls\n", lpHomeDirectory));
  
  
    if (GetProfileString(               /*--------------------------------*/
        (LPSTR) gBulletIntellifont,      /* Application heading in WIN.INI */
        (LPSTR) gSupportFiles,           /* Key name for support files     */
        (LPSTR) gNull,               /* Default support file directory */
        (LPSTR) lpSupportFileDirectory,      /* Variable to store directory    */
        MAX_PATHNAME_SIZE) == 0)
  
        return FALSE;
  
  
    if (GetProfileInt((LPSTR)gBulletIntellifont, (LPSTR)gBowStatus, -1) == -1)
        return FALSE;
  
    DBGconfig(("Support File directory = %ls\n", lpSupportFileDirectory));
    DBGconfig(("Type Directory = %ls\n", lpTypeDirectory));
    DBGconfig(("WIN.INI heading = %ls\n", lpAppName));
  
    return TRUE;
  
}
  
  
/****************************************************************************\
*
*  name: QueryCopyFile
*
*  description:
*
*  return value:
*
*  globals:
*
*  called by:
*
*  history:
*
\****************************************************************************/
  
BOOL FAR PASCAL QueryCopyFile(lpSrcFile, lpDstFile, hDB, QueryMessage)
LPSTR lpSrcFile;
LPSTR lpDstFile;
HWND hDB;
LPSTR QueryMessage;
{
    int hSrcFile, hDstFile;
    int BytesRead, BytesWrote;
    HANDLE hBuffer;
    LPSTR lpBuffer;
  
    DBGquery(("QueryCopyFile in support [%ls] [%ls]\n", (LPSTR)lpSrcFile, (LPSTR)lpDstFile));
  
    if (lstrcmpi(lpSrcFile, lpDstFile) == 0)
        return(TRUE);
  
  
  
    if ((hSrcFile = _lopen(lpSrcFile, OF_READ)) == -1)
        return(FALSE);
  
  
    if ((hDstFile = _lcreat(lpDstFile, 0 /*normal read write*/)) == -1)
    {
        _lclose(hSrcFile);
        return(FALSE);
    }
  
  
    if ((hBuffer = GlobalAlloc(GMEM_MOVEABLE, (LONG) BUFFER_SIZE)) == NULL)
    {
        _lclose(hSrcFile);
        _lclose(hDstFile);
        DBGquery(("Could not allocate memory\n"));
        return(FALSE);
    }
  
  
    if ((lpBuffer = GlobalLock(hBuffer)) == (LPSTR) NULL)
    {
        GlobalFree(hBuffer);
        _lclose(hSrcFile);
        _lclose(hDstFile);
        DBGquery(("Could not lock memory\n"));
        return(FALSE);
    }
    /*--------------------------------------------------------------*\
    | it is possible that if cancel in the middle we will not detect it and not
    | completely write a file.  we shouldkeep count and then return status
    \*--------------------------------------------------------------*/
  
  
    do  {
        if ((BytesRead = _lread(hSrcFile, lpBuffer, BUFFER_SIZE)) > 0)
        {
            BytesWrote = _lwrite(hDstFile, lpBuffer, BytesRead);
            if (BytesWrote != BytesRead)
            {
                _lclose(hSrcFile);
                _lclose(hDstFile);
                unlink(lpDstFile);
                GlobalUnlock(hBuffer);
                GlobalFree(hBuffer);
                return(FALSE);
            }
        }
    }
    while (BytesRead == BUFFER_SIZE);
  
  
    _lclose(hSrcFile);
    _lclose(hDstFile);
    GlobalUnlock(hBuffer);
    GlobalFree(hBuffer);
    return(TRUE);
}
  
  
/****************************************************************************\
*
*  name: unlink
*
*  description:
*
*  return value:
*
*  globals:
*
*  called by:
*
*  history:
*
\****************************************************************************/
  
BOOL FAR PASCAL unlink(lpFileName)
LPSTR lpFileName;
{
    OFSTRUCT lpReOpenBuff;
  
    return(OpenFile(lpFileName, (LPOFSTRUCT) &lpReOpenBuff, OF_DELETE));
}
  
  
/************************************************************
*
*     BlinkStatusLine
*
************************************************************/
  
  
VOID FAR PASCAL BlinkStatusLine(hDB, idStaticText, string)
HWND hDB;
int idStaticText;
LPSTR string;
{
    DBGentry(("BlinkStatusLine in support\n"));
  
    SetDlgItemText(hDB, idStaticText, string);
}
  
