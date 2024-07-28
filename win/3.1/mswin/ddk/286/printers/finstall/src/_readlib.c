/**[f******************************************************************
* $readlib.c -
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

// 21 aug 91   rk(HP)  Changed gPortNm from 32 to 128
  
//#define DEBUG
  
#include <string.h>
#include "_windows.h"
#include "_cgifwin.h"
#include "_tmu.h"
#include "_readlib.h"
#include "_sflib.h"
#include "_ut.h"
#include "_sfpfm.h"
#include "_kludge.h"
  
#include "debug.h"
#include "sfdir.h"
#include "sflb.h"
#include "sfutils.h"
#include "strings.h"
#include "utils.h"
  
/****************************************************************************\
* Debug Definitions
\****************************************************************************/
  
  
  
#ifdef DEBUG
    #define DBGerr(msg)     /*DBMSG(msg)*/
    #define DBGripf(msg)    /*DBMSG(msg)*/
    #define DBGtest(msg)    /*DBMSG(msg)*/
#else
    #define DBGerr(msg)     /*null*/
    #define DBGripf(msg)    /*null*/
    #define DBGtest(msg)    /*null*/
#endif
  
  
/****************************************************************************\
* Local Definitions
\****************************************************************************/
  
#define FALSE 0
#define TRUE 1
  
  
/****************************************************************************\
* Globals
\****************************************************************************/
  
extern char gSupportFileDirectory[MAX_FILENAME_SIZE];
extern char gIndexFile[MAX_FILENAME_SIZE];
extern DIRECTORY gInstalledLibrary;
  
// dtk 10-30-90
  
/*  Temporary structure used by FillListBox() to collect and
*  modify information.
*/
typedef struct {
    char   appName[64];     /* Application name for win.ini */
    char   drv[64];     /* String for "Driver on " */
    char   point[32];       /* String for point size */
    char   bold[32];        /* String for bold */
    char   italic[32];      /* String for italic */
    char   ind[2];          /* Space holder for SFDIRSTRNG entry */
    char   file[256];       /* PFM and DL file names */
    SFDIRFILE SFfile;       /* SFDIRFILE struct */
    char   s[256];          /* Buffer for strings at end of SFDIRFILE */
    char   buf[256];        /* General work buffer */
} FILL_LB_TEMP;
typedef FILL_LB_TEMP FAR *LPFILL_LB_TEMP;
  
void appendperiod(HWND);
  
extern char gModNm[32];
//extern char gPortNm[32];
extern char gPortNm[128];
extern char gAppName[sizeof(gModNm)+sizeof(gPortNm)];
  
// end dtk
  
  
/****************************************************************************\
* Forward References
\****************************************************************************/
  
BOOL FAR PASCAL SameFilename(LPSTR, LPSTR);
LPSTR FAR PASCAL LastPartOfFilename(LPSTR);
  
/****************************************************************************\
*
\****************************************************************************/
  
/****************************************************************************\
*
*  name: ReadInstalledPrinterFonts
*
*  description: Read the if.fnt file and update the scalable fonts
*           list box
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
  
HANDLE FAR PASCAL ReadInstalledPrinterFonts(hDB, ListBoxID, lpBuf2, hSFlb, NumInstalledFonts)
  
HWND   hDB;
int    ListBoxID;
LPSTR  lpBuf2;
HANDLE hSFlb;
WORD FAR *NumInstalledFonts;
{
    int pcount;
    int len, blen, slen;
    int nSoftFonts = 0;
    LPSTR top;
    WORD status, i;
    static HANDLE hTypeList = (HANDLE)0;
    LPTYPEINFO lpTypeInfo;
    LPTYPEINFO lpTypeList;
    LPSTR lpBulletPath, p, s;
  
    /* dtk  10-30-90
    */
    HANDLE hRet=hSFlb;
    LPFILL_LB_TEMP lpBuf = (LPFILL_LB_TEMP)lpBuf2;
    LPSFDIRFILE lpSFfile;
    int tmp;
    char nameBuf[128];
    BYTE fs_numstr[5];
  
    lmemset((LPSTR)nameBuf, 0, sizeof(nameBuf));
  
    DBGerr(("Begin readinstalledprinterfons\n"));
  
    *NumInstalledFonts = 0;
  
    lpBulletPath = (LPSTR)gSupportFileDirectory;
    DBGerr(("Support file dir = %ls\n",lpBulletPath));
  
    pcount = 0;
    status = FACElist((LPUWORD) &pcount, lpTypeList, lpBulletPath);   /* determine mem needed */
    if (status)
    {
        DBGerr(("FACElist error status %d\n", status));
        return (hRet);
    }
  
    if ((hTypeList = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT,
        (LONG)(pcount * sizeof(TYPEINFO))))==NULL)
    {
        DBGerr(("Could not allocate memory\n"));
        return (hRet);
    }
  
    if ((lpTypeList = (LPTYPEINFO)GlobalLock(hTypeList)) == (LPTYPEINFO)NULL)
    {
        GlobalFree(hTypeList);
        hTypeList = 0;
        DBGerr(("Could not lockmemory\n"));
        return (hRet);
    }
  
    status = FACElist((LPUWORD) &pcount, lpTypeList, lpBulletPath); /* get face names */
    if (status)
    {
        GlobalUnlock(hTypeList);
        GlobalFree(hTypeList);
        hTypeList = 0;
        lpTypeList = 0;
        DBGerr(("FACElist error status %d\n", status));
        return (hRet);
    }
  
  
  
    for(lpTypeInfo = lpTypeList, i = 0;   i < pcount; ++i, ++lpTypeInfo)
    {
        DBGripf(("Found face <%ls>\n", (LPSTR) lpTypeInfo->typefaceName));
  
        if (!SameFilename((LPSTR) lpTypeInfo->nameOrDir, (LPSTR) "HQ3UPDT.TYQ") &&
            !SameFilename((LPSTR) lpTypeInfo->nameOrDir, (LPSTR) "PLUGIN.TYQ" ) &&
            !SameFilename((LPSTR) lpTypeInfo->nameOrDir, (LPSTR) "SCREEN.TYS" ))
        {
  
  
            // dtk 10-30-90
  
            lpSFfile = (LPSFDIRFILE) &lpBuf->SFfile;
            lpSFfile->fIsPCM=FALSE;
            lpSFfile->orient=0;
            lpSFfile->indDLpath = -1;
            lpSFfile->offsDLname=0;
  
            lstrcpy(lpSFfile->s, lpTypeInfo->typefaceName);
            tmp = (lstrlen(lpSFfile->s) + sizeof(SFDIRFILE));
  
            DBGtest(("Name being put into lpSFfile = [%ls]\n",(LPSTR)lpSFfile->s));
  
            lpSFfile->indLOGdrv = -1;
            lpSFfile->indScrnFnt = -1;
            lpSFfile->indPFMpath = -1;
            lpSFfile->offsPFMname = 0;
  
  
            /*  Add the structure to the soft font directory list.*/
  
            if ((tmp=addSFdirEntry(0L,(LPSTR)lpSFfile,SF_FILE,tmp)) > -1)
            {
                DBGtest(("tmp =%d\n",tmp));
                hRet = addSFlistbox(hDB, hRet, ListBoxID, -1, tmp,
                SFLB_FAIS, nameBuf, sizeof(nameBuf), 0L);
            }
  
            else
            {
                GlobalUnlock(hTypeList);
                GlobalFree(hTypeList);
                return(0);
            }
  
  
            // end of additions
  
  
            if (AddDirEntry((LPDIRECTORY) &gInstalledLibrary,
                (LPSTR) lpTypeInfo->nameOrDir,
                (LPSTR) lpTypeInfo->typefaceName,
                (LPSTR) NULL,
                (LPSTR) NULL,
                LIB_ENTRY_TYPE,
                ListBoxID,
                (LPTYPEINFO) lpTypeInfo,
                (LPSCRNFNTINFO) NULL))
  
                ++ *NumInstalledFonts;
            else
            {
                GlobalUnlock(hTypeList);
                GlobalFree(hTypeList);
                return(0);
            }
  
  
            // Append a period every so many fonts to status line.
  
            if (!(nSoftFonts++ % 10))
                appendperiod(hDB);
  
        }
  
    } /* for */
  
    itoa(*NumInstalledFonts, fs_numstr);
    WriteProfileString((LPSTR)gAppName,(LPSTR)"IfwFonts", (LPSTR)fs_numstr);
  
  
    if(hTypeList != NULL)
    {
        GlobalUnlock(hTypeList);
        GlobalFree(hTypeList);
        hTypeList = 0;
        lpTypeList = 0;
    }
  
    return (hRet);
}
/****************************************************************************\
*
\****************************************************************************/
  
LPSTR FAR PASCAL LastPartOfFilename(lpFilename)
LPSTR lpFilename;
{
    LPSTR lpF;
  
    lpF = lpFilename + lstrlen(lpFilename) - 1;
    while ((lpF > lpFilename) && (*(lpF-1) != '\\') && (*(lpF-1) != ':'))
    {
        --lpF;
    }
    return lpF;
}
  
  
  
BOOL FAR PASCAL SameFilename(lpFilename1, lpFilename2)
LPSTR lpFilename1, lpFilename2;
{
    LPSTR lpF1, lpF2;
  
    lpF1 = LastPartOfFilename(lpFilename1);
    lpF2 = LastPartOfFilename(lpFilename2);
    if (lstrcmpi(lpF1, lpF2)==0)
        return TRUE;
    return FALSE;
}
  
  
/** added from sfutils.c dtk 11-1-90 **/
  
// Append a dot to the status line.
  
void appendperiod(hDB)
HWND hDB;
{
    char buff[80];
    int len;
  
    if (len = GetDlgItemText(hDB, SF_STATUS, (LPSTR)buff, sizeof(buff)))
        if (len < sizeof(buff) - 2);
        {
            lstrcat((LPSTR)buff, (LPSTR)".");
            SetDlgItemText(hDB, SF_STATUS, (LPSTR) buff);
        }
}
  
