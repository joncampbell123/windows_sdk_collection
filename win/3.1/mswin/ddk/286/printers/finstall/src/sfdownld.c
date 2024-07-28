/**[f******************************************************************
* sfdownld.c -
*
* Copyright (C) 1988,1989 Aldus Corporation
* Copyright (C) 1988-1990 Microsoft Corporation.
* Copyright (C) 1990,1991 Hewlett-Packard Company.
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

/******************************   sfdownld.c   *****************************/
/*
*  SFdownld:  Module for building download batch files.
*
* History
*
*  04 sep 91   rk (HP) In editAutoexec removed OF_READ from OpenFile
*              It was failing in Win 3.1 and causing OpenFile with OF_CREATE
*              which truncated existing AUTOEXEC.BAT file. Very bad.
*              Modified OpenFile comparison and lpBuf->hDLfile comparison
*              in InitDLFont.
*              Modified OpenFile comparison in lpOfStruct.
*              Modified OpenFile comparison in textToString.
*              Modified OpenFile comparison in CanMakeYN.   
*              Modified OpenFile comparison in BuildSFINSTAL_DIR  
*              Modified OpenFile comparison in sortdownload
*  29 nov 89   peterbe Changed MB_ICONQUESTION to MB_ICONEXCLAMATION
*
*  09 nov 89   peterbe Changed SFINSTAL.DIR to FINSTALL.DIR in comments.
*          While we can READ directories of various names, the
*          filename we use for output is FINSTALL.DIR now (the
*          name in the resource SFADD_DEFDIRF.
*
*  20 jul 89   peterbe Checked in changes by steved (HP).
*   7-07-89    steved  Sorted download file by size for DeskJet.
*   1-13-89    jimmat  Reduced # of redundant strings by adding lclstr.h
*   1-26-89    jimmat  Adjustments for changes in resource file.
*   2-20-89    jimmat  Font Installer/Driver use same WIN.INI section (again)!
*/
//*******************************************************************
//
// PLEASE EDIT THIS FILE USING THE FOLLOWING GUIDELINES:
//  Tabs = EIGHT spaces
//  max no. of columns = 80
//  If a statement is > 80 columns, PLEASE continue on NEXT line.
//
//*******************************************************************
  
  
//#define DEBUG
  
extern int gPrintClass;
#include "nocrap.h"
#undef NOOPENFILE
#undef NOMEMMGR
#undef NOCTLMGR
#undef NOWINMESSAGES
#undef NOMB
#undef NOHDC
#undef NOGDI
#include "windows.h"
#include "neededh.h"
#include "resource.h"
#include "strings.h"
#include "sfdownld.h"
#include "sfdir.h"
#include "sflb.h"
#include "sfutils.h"
#include "dlgutils.h"
#include "dosutils.h"
#include "lclstr.h"
#include "pfm.h"
#include "deskjet.h"
#include "desc.h"
  
/****************************************************************************\
*                     Debug Definitions
\****************************************************************************/
  
#ifdef DEBUG
    #define DBGdl(msg)       /*DBMSG(msg)*/
    #define DBGdlgfn(msg)   /*DBMSG(msg)*/
    #define DBGfindstr(msg) /*DBMSG(msg)*/
    #define DBGynprmpt(msg) /*DBMSG(msg)*/
    #define DBGdirfile(msg) /*DBMSG(msg)*/
#else
    #define DBGdl(msg)       /*null*/
    #define DBGdlgfn(msg)   /*null*/
    #define DBGfindstr(msg) /*null*/
    #define DBGynprmpt(msg) /*null*/
    #define DBGdirfile(msg) /*null*/
#endif
  
  
  
  
  
#ifdef DEBUG
#undef DBG_FINDSTR
#endif
  
  
#define LOCAL static
  
#define swab(x) ((((x) << 8) & 0xFF00) | (((x) >> 8) & 0xFF))
  
  
/* From printer.h
*/
HANDLE FAR  PASCAL OpenJob(LPSTR, LPSTR, HANDLE);
short FAR  PASCAL StartSpoolPage(HANDLE);
short FAR  PASCAL EndSpoolPage(HANDLE);
short FAR  PASCAL WriteSpool(HANDLE, LPSTR, short);
short FAR  PASCAL CloseJob(HANDLE);
short FAR  PASCAL DeleteJob(HANDLE, short);
  
  
typedef enum {
    dl_echo = 0,
    dl_delay,
    dl_formfeed,
    dl_copy,
    dl_bcopy,
    dl_erase
} DLACTION;
  
#define SFDIRFNMSZ  80
#define SFDLBUFSZ   512
  
typedef struct {
    char buf[SFDLBUFSZ];
    char cmd[128];
    char remcmd[128];           /* should be same size as cmd */
    char tmp[8];
    char dlfnm[80];
    char tmpfile[80];
    char dirfnm[SFDIRFNMSZ];
    char ynfnm[80];
    char appNm[64];
    char sfstr[32];
    char sfnum[32];
    char dlmsg[32];
    char echo[32];
    char copy[32];
    char erase[32];
    char binary[16];
    char alpha[16];
    char tmp1[32];
    char tmp2[32];
    char tmp3[32];
    char header[64];
    char autoex[64];
    char style[32];
    OFSTRUCT ofstruct;
    int hDLfile;
    HANDLE jobNum;
    HDC jobDC;
    int hTmpFile;
    BOOL spooling;
    BOOL fileSpooled;
} DLREC;
typedef DLREC FAR*LPDLREC;
  
  
int FAR PASCAL DLdlgFn(HWND, unsigned, WORD, LONG);
int FAR PASCAL DIRdlgFn(HWND, unsigned, WORD, LONG);
LOCAL BOOL UpdateWinIni(HANDLE, BOOL, LPDLREC, LPSFLBENTRY);
LOCAL void editAutoexec(HWND, HANDLE, LPDLREC, LPSFLB, LPSTR);
LOCAL long findString(int, LPDLREC, BOOL FAR *);
LOCAL void initDLfont(HANDLE, LPDLREC, LPSFLB, LPSTR);
LOCAL void DLfont(HWND, HANDLE, LPDLREC, LPSFLBENTRY, LPSTR);
LOCAL void endDLfont(HANDLE, LPDLREC, LPSTR);
LOCAL void fileToSpooler(HANDLE, int, LPOFSTRUCT, LPSTR, int);
LOCAL void fileToBatch(LPDLREC, int, LPSTR);
LOCAL void flushDelayed(LPDLREC, LPSTR, LPSTR);
LOCAL void textToStream(LPDLREC, DLACTION, LPSTR, LPSTR, BOOL);
LOCAL BOOL canDownload(int);
LOCAL BOOL getDLbatchNm(HANDLE, LPSFLB, LPDLREC, LPSTR);
LOCAL BOOL makeDLbatchNm(HANDLE, LPSTR, LPSTR, int);
LOCAL BOOL canMakeYN(HANDLE, LPDLREC);
LOCAL void writeYNprompt(HANDLE, LPDLREC, LPSTR);
LOCAL BOOL makeDIRfnm(HANDLE, LPDLREC);
LOCAL void BuildSFINSTAL_DIR (HWND, HANDLE, LPDLREC, LPSFLB, LPSTR);
LOCAL BOOL mylmemcmp(LPSTR, LPSTR, int);
LOCAL void sortdownload(LPSFLB);
LOCAL BYTE readnum(int, int FAR *);
  
extern BOOL gBowinstalled;
  
LOCAL WORD gDLstyle = 0;
LOCAL LPSTR gLPbuf = 0L;
LOCAL HANDLE gHMd;
  
/**************************************************************************/
/****************************   Global Procs   ****************************/
  
  
/*  EndPort
*/
BOOL FAR PASCAL
EndPort(hDB, hMd, hSFlb, lpModNm, lpPortNm, bldDirFile)
HWND hDB;
HANDLE hMd;
HANDLE hSFlb;
LPSTR lpModNm;
LPSTR lpPortNm;
BOOL bldDirFile;
{
    LPSFLBENTRY sflb = 0L;
    LPSFLB lpSFlb = 0L;
    LPDLREC lpBuf = 0L;
    HANDLE hBuf = 0;
    BOOL permFont = FALSE;
    BOOL success = TRUE;
    int ind, ifwnum, sfeqnum;
  
    DBGdl(("EndPort(%d,%d,%d,%lp): %ls\n",
    hDB, hMd, hSFlb, lpPortNm, lpPortNm));
  
    gHMd = hMd;
  
    /*  Set default download style.
    */
    gDLstyle = SFDL_NOW | SFDL_STARTUP;
  
    if (hSFlb && (lpSFlb=(LPSFLB)GlobalLock(hSFlb)) &&
        (hBuf=GlobalAlloc(GMEM_MOVEABLE,(DWORD)sizeof(DLREC))) &&
        (lpBuf=(LPDLREC)GlobalLock(hBuf)) &&
        LoadString(hMd,SF_SOFTFONT,lpBuf->sfstr,sizeof(lpBuf->sfstr)) &&
        LoadString(hMd,SF_SOFTFONTS,lpBuf->sfnum,sizeof(lpBuf->sfnum)) &&
        LoadString(hMd,SF_DOSECHO,lpBuf->echo,sizeof(lpBuf->echo)) &&
        LoadString(hMd,SF_DOSCOPY,lpBuf->copy,sizeof(lpBuf->copy)) &&
        LoadString(hMd,SF_DOSERASE,lpBuf->erase,sizeof(lpBuf->erase)) &&
        LoadString(hMd,SF_DOSBINARY,lpBuf->binary,sizeof(lpBuf->binary)) &&
        LoadString(hMd,SF_DOSALPHA,lpBuf->alpha,sizeof(lpBuf->alpha)) &&
        LoadString(hMd,SF_DOSCOMMAND,lpBuf->cmd,sizeof(lpBuf->cmd)) &&
        LoadString(hMd,SF_DOSREM,lpBuf->remcmd,sizeof(lpBuf->remcmd)) &&
        LoadString(hMd,SFDL_DLMSG,lpBuf->dlmsg,sizeof(lpBuf->dlmsg)) &&
        LoadString(hMd,SFDL_TMP1FILNM,lpBuf->tmp1,sizeof(lpBuf->tmp1)) &&
        LoadString(hMd,SFDL_TMP2FILNM,lpBuf->tmp2,sizeof(lpBuf->tmp2)) &&
        LoadString(hMd,SFDL_TMP3FILNM,lpBuf->tmp3,sizeof(lpBuf->tmp3)) &&
        LoadString(hMd,SFDL_HEADER,lpBuf->header,sizeof(lpBuf->header)) &&
        LoadString(hMd,SFDL_AUTOEXEC,lpBuf->autoex,sizeof(lpBuf->autoex)) &&
        LoadString(hMd,SFDL_STYLE,lpBuf->style,sizeof(lpBuf->style)))
    {
        /*  Build "[<driver>,<port>]" for accessing win.ini file.
        */
        MakeAppName(lpModNm,lpPortNm,lpBuf->appNm,sizeof(lpBuf->appNm));
  
        SetDlgItemText(hDB, SF_STATUS, (LPSTR)NullStr);
  
        /*  First look to see if there are any permanent fonts.
        */
        for (ind=0, sflb=&lpSFlb->sflb[0]; ind < lpSFlb->free;
            ++ind, ++sflb)
        {
            if ((sflb->state&SFLB_PERM) && !(sflb->state&SFLB_CART))
            {
                DBGdl(("EndPort(): ...first perm font at ind %d\n", ind));
                break;
            }
        }
  
        /*  Prompt for the user to select the download styles if there
        *  is atleast one downloadable font.
        */
        if (ind < lpSFlb->free)
        {
            /*  Read the download style from the win.ini file.
            */
            gDLstyle = GetProfileInt(lpBuf->appNm,lpBuf->style,gDLstyle);
  
            gLPbuf = lpPortNm;
  
            if (MyDialogBox(hMd,SFDLQUERY,hDB,DLdlgFn) ==
                IDCANCEL)
            {
                success = FALSE;
                goto backout;
            }
  
            /*  Write the download style to the win.ini file.
            */
            itoa(gDLstyle, lpBuf->buf);
            WriteProfileString(lpBuf->appNm,lpBuf->style,lpBuf->buf);
        }
        else
            gDLstyle = 0;
  
        /*  Look to see if any fonts have changed status.
        */
        for (ind=0, sflb=&lpSFlb->sflb[0]; ind < lpSFlb->free;
            ++ind, ++sflb)
        {
            if ((((sflb->state & SFLB_PERM) == 0) !=
                ((sflb->native & SFLB_PERM) == 0)) ||
                ((sflb->state & SFLB_PERM) && (sflb->state & SFLB_NEWID)))
            {
                DBGdl(("EndPort(): ...first changed font at ind %d\n", ind));
                break;
            }
        }
  
        if (!getDLbatchNm(hMd,lpSFlb,lpBuf,lpPortNm))
        {
            DBGdl(("EndPort(): *failed* to get dl batch file name.\n"));
            goto backout;
        }
  
        permFont = FALSE;
  
        /*  Hourglass cursor
        */
        SetCursor(LoadCursor(NULL,IDC_WAIT));
  
        if (ind < lpSFlb->free || (gDLstyle & SFDL_NOW))
        {
            /*  Atleast one font has changed status.
            */
            if (ind < lpSFlb->free)
            {
                /*  Fonts have changed status, this requires
                *  changing the fontSummary.
                */
                NewFS(hMd, lpBuf->appNm);
            }
  
            //  If we are dealing with a DeskJet Family printer we will sort
            //  the DL files by file size in descending order.  This will
            //  result in downloading the largest soft font files first.
            //  This is necessary to avoid fragmentation of the RAM
            //  cartridge(s) in the printer.
  
            if ((gPrintClass == CLASS_DESKJET) ||
                (gPrintClass == CLASS_DESKJET_PLUS)
                )
            {
                DBGdl(
                ("EndPort(): calling sortdownload, gPrintClass %d\n",
                gPrintClass));
                sortdownload(lpSFlb);
            }
  
            for (ind=0, sflb=&lpSFlb->sflb[0]; ind < lpSFlb->free;
                ++ind, ++sflb)
            {
                if (sflb->state & SFLB_PERM)
                {
                    if (canDownload(sflb->indSFfile))
                    {
                        if (!permFont)
                        {
                            initDLfont(hMd,lpBuf,lpSFlb,lpPortNm);
                            permFont = TRUE;
                        }
  
                        UpdateWinIni(hMd,FALSE,lpBuf,sflb);
                        DLfont(hDB,hMd,lpBuf,sflb,lpPortNm);
                    }
                }
                else if (sflb->native & SFLB_PERM)
                {
                    UpdateWinIni(hMd,TRUE,lpBuf,sflb);
                }
            }
  
            if (permFont)
            {
                endDLfont(hMd,lpBuf,lpPortNm);
            }
            else
            {
                /*  If we failed to download any fonts then shut
                *  off any requests for download -- this guarantees
                *  proper update of the autoexec.bat file.
                */
                gDLstyle = 0;
            }
        }
  
        SetDlgItemText(hDB, SF_STATUS, (LPSTR)NullStr);
  
        /*  Update autoexec.bat file.
        */
        editAutoexec(hDB,hMd,lpBuf,lpSFlb,lpPortNm);
  
        /*  Write the "softfonts=" entry to the win.ini file,
        Remebering to subtract the number of installed screen fonts.
        */
  
        if (gBowinstalled)
        {
            ifwnum = GetProfileInt(lpBuf->appNm,(LPSTR)"IfwFonts",0);
            sfeqnum = lpSFlb->free;
            sfeqnum -= ifwnum;
            itoa(sfeqnum, lpBuf->buf);
        }
        else
            itoa(lpSFlb->free, lpBuf->buf);
  
        WriteProfileString(lpBuf->appNm,lpBuf->sfnum,lpBuf->buf);
  
        /* restore arrow cursor
        */
        SetCursor(LoadCursor(NULL,IDC_ARROW));
  
        /*  Build a FINSTALL.DIR file if the user held down the
        *  CTRL-SHIFT keys.
        */
        if (bldDirFile && makeDIRfnm(hMd,lpBuf))
        {
            gLPbuf = (LPSTR)lpBuf;
  
            if (MyDialogBox(hMd,SFDLDIRFNM,hDB,DIRdlgFn) == IDCANCEL)
            {
                success = FALSE;
                goto backout;
            }
  
            BuildSFINSTAL_DIR(hDB,hMd,lpBuf,lpSFlb,lpPortNm);
        }
  
        /*  Remove each of the SF directory items.
        */
        for (ind=0, sflb=&lpSFlb->sflb[0]; ind < lpSFlb->free;
            ++ind, ++sflb)
        {
            delSFdirEntry(0L, sflb->indSFfile);
        }
        lpSFlb->free = 0;
    }
  
    backout:
  
    if (lpSFlb)
    {
        GlobalUnlock(hSFlb);
        lpSFlb = 0L;
    }
  
    if (lpBuf)
    {
        GlobalUnlock(hBuf);
        lpBuf = 0L;
    }
  
    if (hBuf)
    {
        GlobalFree(hBuf);
        hBuf = 0;
    }
  
    return (success);
}
  
/*  DLdlgFn
*/
int FAR PASCAL DLdlgFn(hDB, wMsg, wParam, lParam)
HWND hDB;
unsigned wMsg;
WORD wParam;
LONG lParam;
{
    switch (wMsg)
    {
        case WM_INITDIALOG:
            DBGdlgfn(("DLdlgFn(%d,%d,%d,%ld): WM_INITDIALOG\n",
            hDB, wMsg, wParam, lParam));
            CheckDlgButton(hDB,SFDL_NOW,(BOOL)(gDLstyle & SFDL_NOW));
            CheckDlgButton(hDB,SFDL_STARTUP,(BOOL)(gDLstyle & SFDL_STARTUP));
            SetDlgItemText(hDB,SFDL_PORT,gLPbuf);
            CenterDlg(hDB);
            break;
  
        case WM_COMMAND:
        switch (wParam)
        {
            case SFDL_NOW:
            case SFDL_STARTUP:
            #ifdef DEBUG
                if (wParam & SFDL_NOW)
                { DBGdlgfn(("DLdlgFn(%d,%d,%d,%ld): SFDL_NOW\n",
                hDB, wMsg, wParam, lParam)); }
                else
                { DBGdlgfn(("DLdlgFn(%d,%d,%d,%ld): SFDL_STARTUP\n",
                hDB, wMsg, wParam, lParam)); }
            #endif
  
                if (gDLstyle & wParam)
                {
                    CheckDlgButton(hDB,wParam,FALSE);
                    gDLstyle &= ~(wParam);
                }
                else
                {
                    CheckDlgButton(hDB,wParam,TRUE);
                    gDLstyle |= wParam;
                }
                break;
  
            case IDHELP:
                DBGdlgfn(("DLdlgFn(%d,%d,%d,%ld): IDHELP\n",
                hDB, wMsg, wParam, lParam));
                MyDialogBox(gHMd,SFDLHELP,hDB,GenericWndProc);
                break;
  
            case IDOK:
            case IDCANCEL:
            #ifdef DEBUG
                if (wParam == IDOK)
                { DBGdlgfn(("DLdlgFn(%d,%d,%d,%ld): IDOK\n",
                hDB, wMsg, wParam, lParam)); }
                else
                { DBGdlgfn(("DLdlgFn(%d,%d,%d,%ld): IDCANCEL\n",
                hDB, wMsg, wParam, lParam)); }
            #endif
                EndDialog(hDB, wParam);
                break;
  
            default:
                return FALSE;
        }
            break;
  
        default:
            return FALSE;
    }
  
    return TRUE;
}
  
/*  DIRdlgFn
*/
int FAR PASCAL DIRdlgFn(hDB, wMsg, wParam, lParam)
HWND hDB;
unsigned wMsg;
WORD wParam;
LONG lParam;
{
    switch (wMsg)
    {
        case WM_INITDIALOG:
            DBGdlgfn(("DIRdlgFn(%d,%d,%d,%ld): WM_INITDIALOG\n",
            hDB, wMsg, wParam, lParam));
            SetDlgItemText(hDB,SFDL_DIRNAME,((LPDLREC)gLPbuf)->dirfnm);
            CenterDlg(hDB);
            break;
  
        case WM_COMMAND:
        switch (wParam)
        {
            case SFDL_DIRNAME:
                DBGdlgfn(("DIRdlgFn(%d,%d,%d,%ld): SFDL_DIRNAME\n",
                hDB, wMsg, wParam, lParam));
                break;
  
            case IDOK:
            case IDCANCEL:
                if (wParam == IDOK)
                {
                    LPSTR s, t;
                    int k;
  
                    DBGdlgfn(("DIRdlgFn(%d,%d,%d,%ld): IDOK\n",
                    hDB, wMsg, wParam, lParam));
                    s = ((LPDLREC)gLPbuf)->dirfnm;
                    GetDlgItemText(hDB,SFDL_DIRNAME,s,SFDIRFNMSZ);
  
                    t = ((LPDLREC)gLPbuf)->buf;
                    if (dos_opend((LPDIRDATA)t,s,0x01) == 0 &&
                        LoadString(gHMd,SFINSTAL_NM,t,SFDLBUFSZ) &&
                        (s+=k=lstrlen(s=t)+1) &&
                        LoadString(gHMd,SFDL_DIREXIST,s,SFDLBUFSZ-k) &&
                        MessageBox(hDB,s,t,
                        MB_YESNO | MB_ICONEXCLAMATION) ==
                        IDNO)
                    {
                        break;
                    }
                }
                else
                {
                    DBGdlgfn(("DIRdlgFn(%d,%d,%d,%ld): IDCANCEL\n",
                    hDB, wMsg, wParam, lParam));
                    *gLPbuf = '\0';
                }
                EndDialog(hDB, wParam);
                break;
  
            default:
                return FALSE;
        }
            break;
  
        default:
            return FALSE;
    }
  
    return TRUE;
}
  
/**************************************************************************/
/*****************************   Local Procs   ****************************/
  
  
/*  UpdateWinIni
*
*  Write entry to win.ini file, could be temporary or permanent
*  download, based upon the value of tmpDL.
*/
LOCAL BOOL UpdateWinIni(hMd, tmpDL, lpBuf, sflb)
HANDLE hMd;
BOOL tmpDL;
LPDLREC lpBuf;
LPSFLBENTRY sflb;
{
    int j, k, n;
  
    DBGdl(("UpdateWinIni(%d,%d,%lp,%lp)\n", hMd, tmpDL, lpBuf, sflb));
  
    if (lstrlen(lpBuf->sfstr) + 8 > sizeof(lpBuf->buf))
        return FALSE;
    lstrcpy(lpBuf->buf, lpBuf->sfstr);
    itoa(sflb->id, &lpBuf->buf[lstrlen(lpBuf->buf)]);
    k = lstrlen(lpBuf->buf) + 1;
  
    j = tmpDL ? 0 : k;
  
    if (makeSFdirFileNm(0L, sflb->indSFfile, TRUE, &lpBuf->buf[k],
        sizeof(lpBuf->buf)-k))
    {
        n = k + lstrlen(&lpBuf->buf[k]);
  
        if (n + 2 > sizeof(lpBuf->buf))
            return FALSE;
  
        if (tmpDL)
            lpBuf->buf[n++] = ',';
        else
            k = ++n;
  
        if (makeSFdirFileNm(0L, sflb->indSFfile, FALSE, &lpBuf->buf[n],
            sizeof(lpBuf->buf)-n))
        {
            /*  Write profile:  if restoring temporary status, then
            *  write "SoftFontn=<PFM file>,<DL file>" or if permanent
            *  status write "<PFM file>=<DL file>."
            */
            WriteProfileString((LPSTR)lpBuf->appNm,&lpBuf->buf[j],
            &lpBuf->buf[k]);
  
            if (!tmpDL)
            {
                /*  If permanent, make sure we change the soft font
                *  entry to "SoftFontn=<PFM file>."
                */
                WriteProfileString(lpBuf->appNm,lpBuf->buf,&lpBuf->buf[j]);
            }
  
            return TRUE;
        }
    }
  
    return FALSE;
}
  
/*  editAutoexec
*/
LOCAL void editAutoexec(hDB, hMd, lpBuf, lpSFlb, lpPortNm)
HWND hDB;
HANDLE hMd;
LPDLREC lpBuf;
LPSFLB lpSFlb;
LPSTR lpPortNm;
{
    long pos;
    BOOL isDeleted;
    int hFile;
    int ind, len;
  
    lmemset((LPSTR)&lpBuf->ofstruct, 0, sizeof(OFSTRUCT));
  
//    if (OpenFile(lpBuf->autoex,&lpBuf->ofstruct,OF_READ | OF_EXIST) > 0)
    if (OpenFile(lpBuf->autoex,&lpBuf->ofstruct,OF_EXIST) >= 0)                          // RK 09/04/91
    {
        hFile = OpenFile(lpBuf->autoex,&lpBuf->ofstruct,OF_READWRITE);
    }
    else
    {
        hFile = OpenFile(lpBuf->autoex,&lpBuf->ofstruct,
        OF_READWRITE | OF_CREATE);
    }
  
//    if (hFile > 0)
    if (hFile >= 0)                                                                      // RK 09/04/91
    {
        if ((len=lstrlen(lpBuf->cmd))+(ind=lstrlen(lpBuf->remcmd))+
            lstrlen(lpBuf->dlfnm)+2 < sizeof(lpBuf->cmd))
        {
            /*  Pad both strings with a space.
            */
            lstrcat(lpBuf->cmd, (LPSTR)" ");
            lstrcat(lpBuf->remcmd, (LPSTR)" ");
            ++ind, ++len;
  
            /*  Complete the command string "command \c batch-file name."
            */
            lstrcat(lpBuf->cmd, lpBuf->dlfnm);
  
            /*  Merge the remark and the command string to create
            *  the deleted string "rem        batch-file name."
            */
            for (; ind < len; ++ind)
                lpBuf->remcmd[ind] = ' ';
  
            len = lstrlen(lpBuf->cmd);
  
            for (; ind <= len; ++ind)
                lpBuf->remcmd[ind] = lpBuf->cmd[ind];
  
        #ifdef DEBUG
            if (lstrlen(lpBuf->remcmd) != len)
            {
                DBMSG((
                "***COMMAND string and REMARK string are not the same length!\n"));
                DBMSG(("*** %d: %ls\n", len, (LPSTR)lpBuf->cmd));
                DBMSG((
                "*** %d: %ls\n", lstrlen(lpBuf->remcmd), (LPSTR)lpBuf->remcmd));
            }
        #endif
  
            /*  Search for the string (either deleted or undeleted)
            *  in the autoexec.bat file.
            */
            if ((pos=findString(hFile,lpBuf,&isDeleted)) > -1)
            {
                /*  The string already exists in the batch file.
                */
                if ((gDLstyle & SFDL_STARTUP) && isDeleted)
                {
                    /*  The line exists but is deleted, overwrite it with
                    *  the undeleted string.
                    */
                    _llseek(hFile, pos, 0);
                    _lwrite(hFile, lpBuf->cmd, len);
                }
                else if (!(gDLstyle & SFDL_STARTUP) && !isDeleted)
                {
                    /*  The string exists but needs to be deleted, overwrite
                    *  it with the deleted string.
                    */
                    _llseek(hFile, pos, 0);
                    _lwrite(hFile, lpBuf->remcmd, len);
                }
            }
            else if (gDLstyle & SFDL_STARTUP)
            {
                BYTE ch;
  
                /*  The command string could not be found but should
                *  be added.
                *
                *  The string will be added to the end of the file.
                *  Step backward to the file until the first alphanumeric
                *  character is encountered, then insert the string(s).
                *  We do this because some editors (ahem, like Windows
                *  notepad) dump strange characters like ETX at the end
                *  of the file justforthehellofit and cause anything
                *  appended to the file to be ignored when DOS executes
                *  the file.
                */
                pos = 0L;
                do {
                    if (_llseek(hFile, --pos, 2) > 0)
                        _lread(hFile, &ch, 1);
                    else
                    {
                        ch = ' ';
                        _llseek(hFile, 0L, 2);
                    }
                } while (ch < (BYTE)' ');
  
                if (LoadString(hMd,SFDL_AEMSG,lpBuf->buf,sizeof(lpBuf->buf)))
                {
                    _lwrite(hFile, (LPSTR)CrLf, 2);
                    _lwrite(hFile, lpBuf->buf, lstrlen(lpBuf->buf));
                }
  
                _lwrite(hFile, (LPSTR)CrLf, 2);
                _lwrite(hFile, lpBuf->cmd, lstrlen(lpBuf->cmd));
                _lwrite(hFile, (LPSTR)CrLf, 2);
            }
        }
  
        _lclose(hFile);
    }
    else if (gDLstyle & SFDL_STARTUP)
    {
        int k;
  
        if (LoadString(hMd,SFDL_NOAUTOCAP,lpBuf->buf,sizeof(lpBuf->buf)) &&
            (k=lstrlen(lpBuf->buf)+1) &&
            LoadString(hMd,SFDL_NOAUTOMSG,&lpBuf->buf[k],sizeof(lpBuf->buf)-k))
        {
            MessageBox(hDB,&lpBuf->buf[k],lpBuf->buf,
            MB_OK | MB_ICONEXCLAMATION);
        }
    }
}
  
/*  findString
*/
LOCAL long findString(hFile, lpBuf, lpIsDeleted)
int hFile;
LPDLREC lpBuf;
BOOL FAR *lpIsDeleted;
{
    LPSTR last, s;
    LPSTR lpCmd = lpBuf->cmd;
    LPSTR lpRem = lpBuf->remcmd;
    long pos = 0L;
    int len = lstrlen(lpCmd);
    int ind = 0;
    int bufsz;
    int readsz;
    int leftover;
  
    DBGdl(("findString(%d,%lp,%lp): %ls\n",
    hFile, lpBuf, lpIsDeleted, lpCmd));
    DBGdl(("                                   %ls\n", lpRem));
  
    *lpIsDeleted = FALSE;
  
    do {
        bufsz = sizeof(lpBuf->buf) - ind;
        readsz = _lread(hFile, &lpBuf->buf[ind], bufsz);
        last = &lpBuf->buf[ind] + readsz;
        leftover = ind;
        ind = 0;
        s = lpBuf->buf;
  
        while (s < last && (readsz + leftover - ind) >= len)
        {
        #ifdef DBG_FINDSTR
            {
                LPSTR j;
                int i;
                DBGfindstr(("scanning:  "));
                for (j=s,i=0; i < len; ++j, ++i)
                {
                    DBGfindstr(("%c", (char)*j));
                }
                DBGfindstr(("\n"));
            }
        #endif
  
            if (mylmemcmp(s,lpCmd,len))
            {
                DBGdl(("findString(): found at %ld\n", pos));
                return (pos);
            }
  
            if (mylmemcmp(s,lpRem,len))
            {
                DBGdl(("findString(): found <deleted> at %ld\n", pos));
                *lpIsDeleted = TRUE;
                return (pos);
            }
  
            for (; s < last && *s != '\r' && *s != '\n'; ++s, ++ind, ++pos)
                ;
            for (; s < last && (*s == '\r' || *s == '\n'); ++s, ++ind, ++pos)
                ;
        }
  
        lmemcpy(lpBuf->buf, s, ind=readsz+leftover-ind);
  
    } while (readsz == bufsz);
  
    DBGdl(("findString(): string not found, return -1\n"));
  
    return (-1);
}
  
/*  initDLfont
*/
LOCAL void initDLfont(hMd, lpBuf, lpSFlb, lpPortNm)
HANDLE hMd;
LPDLREC lpBuf;
LPSFLB lpSFlb;
LPSTR lpPortNm;
{
    DBGdl(("initDLfont(%d,%lp,%lp,%lp): %ls\n",
    hMd, lpBuf, lpSFlb, lpPortNm, lpPortNm));
  
    if (gDLstyle & SFDL_NOW)
    {
        /*  Open spool stream.
        */
        lpBuf->buf[0] = '\0';
        LoadString(hMd,SFDL_SPOOLNM,lpBuf->buf,sizeof(lpBuf->buf));
  
        lpBuf->jobDC = GetDC((HWND)NULL);
        if (!(lpBuf->jobNum = OpenJob(lpPortNm,lpBuf->buf,lpBuf->jobDC)))
        {
            if (lpBuf->jobDC)
                ReleaseDC((HWND)NULL, lpBuf->jobDC);
        }
        lpBuf->hTmpFile = -1;
        lpBuf->tmpfile[0] = '\0';
        lpBuf->spooling = FALSE;
        lpBuf->fileSpooled = FALSE;
    }
    else
        lpBuf->jobNum = 0;
  
    if (gDLstyle & SFDL_STARTUP)
    {
        /*  Open batch file stream.
        */
        lmemset((LPSTR)&lpBuf->ofstruct,0,sizeof(OFSTRUCT));
  
        lpBuf->hDLfile =
        OpenFile(lpBuf->dlfnm,&lpBuf->ofstruct,OF_WRITE | OF_CREATE);
  
        /*  Write header to download batch file.
        */
//        if (lpBuf->hDLfile > 0)
        if (lpBuf->hDLfile >= 0)                                                         // RK 09/04/91
        {
            if (LoadString(hMd,SFDL_BATCHHEAD,lpBuf->buf,sizeof(lpBuf->buf)))
                _lwrite(lpBuf->hDLfile,lpBuf->buf,lstrlen(lpBuf->buf));
  
            /*  Write the invocation of the ynprompt function.
            */
            if (canMakeYN(hMd,lpBuf))
                writeYNprompt(hMd,lpBuf,lpPortNm);
        }
    }
    else
        lpBuf->hDLfile = -1;
  
    /*  Reset the printer and begin building the banner file
    *  (downloaded after all the fonts are down).
    */
    textToStream(lpBuf,dl_echo,(LPSTR)"\033E\033*c0F",lpPortNm,FALSE);
    textToStream(lpBuf,dl_delay,lpBuf->header,lpBuf->tmp3,FALSE);
}
  
/*  DLfont
*/
LOCAL void DLfont(hDB, hMd, lpBuf, sflb, lpPortNm)
HWND hDB;
HANDLE hMd;
LPDLREC lpBuf;
LPSFLBENTRY sflb;
LPSTR lpPortNm;
{
    LPSFDIRFILE lpSFfile;
    int ind;
  
    DBGdl(("DLfont(%d,%lp,%lp,%lp): %ls\n",
    hMd, lpBuf, sflb, lpPortNm, lpPortNm));
  
    /*  First update dialog status line.
    */
    if (lpSFfile=(LPSFDIRFILE)lockSFdirEntry(0L,sflb->indSFfile))
    {
        lmemcpy(lpBuf->buf,lpBuf->dlmsg,sizeof(lpBuf->buf));
        lpBuf->buf[sizeof(lpBuf->buf)-1] = '\0';
        ind = lstrlen(lpBuf->buf);
        buildDescStr(&lpBuf->buf[ind],sizeof(lpBuf->buf)-ind,
        TRUE,sflb,lpSFfile);
        SetDlgItemText(hDB, SF_STATUS, lpBuf->buf);
        unlockSFdirEntry(sflb->indSFfile);
    }
  
    /*  Font ID.
    */
    lpBuf->buf[0] = '\033';
    lpBuf->buf[1] = '*';
    lpBuf->buf[2] = 'c';
    ind = 3 + itoa(sflb->id, &lpBuf->buf[3]);
    lpBuf->buf[ind] = 'D';
    lpBuf->buf[ind+1] = '\0';
  
    textToStream(lpBuf,dl_echo,lpBuf->buf,lpBuf->tmp1,FALSE);
  
    if (lpBuf->jobNum > 0 && lpBuf->spooling)
    {
        fileToSpooler(lpBuf->jobNum,sflb->indSFfile,&lpBuf->ofstruct,
        &lpBuf->buf[ind],sizeof(lpBuf->buf)-ind);
        lpBuf->fileSpooled = TRUE;
    }
  
    /*  Identify font as permanent download.
    */
    lpBuf->buf[ind] = 'd';
    lpBuf->buf[ind+1] = '5';
    lpBuf->buf[ind+2] = 'F';
    lpBuf->buf[ind+3] = '\0';
  
    textToStream(lpBuf,dl_echo,lpBuf->buf,lpBuf->tmp2,FALSE);
  
    /*  Display font name (this happens after all fonts have been
    *  downloaded).
    */
    lpBuf->buf[1] = '\033';
    lpBuf->buf[2] = '(';
    lpBuf->buf[ind] = 'X';
    lpBuf->buf[ind+1] = '\0';
  
    if (lpSFfile=(LPSFDIRFILE)lockSFdirEntry(0L,sflb->indSFfile))
    {
        /*  Append name to escape sequence sitting in lpBuf->buf.
        */
        buildDescStr(&lpBuf->buf[ind+1],sizeof(lpBuf->buf)-ind-1,
        TRUE,sflb,lpSFfile);
        unlockSFdirEntry(sflb->indSFfile);
    }
  
    textToStream(lpBuf,dl_delay,&lpBuf->buf[1],lpBuf->tmp3,TRUE);
  
//    if (lpBuf->hDLfile > 0)
    if (lpBuf->hDLfile >= 0)                                                             // RK 09/04/91
    {
        fileToBatch(lpBuf,sflb->indSFfile,lpPortNm);
    }
}
  
/*  endDLfont
*/
LOCAL void endDLfont(hMd, lpBuf, lpPortNm)
HANDLE hMd;
LPDLREC lpBuf;
LPSTR lpPortNm;
{
    DBGdl(("endDLfont(%d,%lp,%lp): %ls\n", hMd, lpBuf, lpPortNm, lpPortNm));
  
    /*  Send out the delayed file (contains a banner) and erase
    *  any temporary files.
    */
    flushDelayed(lpBuf,lpBuf->tmp3,lpPortNm);
    textToStream(lpBuf,dl_erase,lpBuf->tmp1,0L,0);
    textToStream(lpBuf,dl_erase,lpBuf->tmp2,0L,0);
    textToStream(lpBuf,dl_erase,lpBuf->tmp3,0L,0);
  
    if (lpBuf->hDLfile > 0)
    {
        /*  Write the label jumped to if ynprompt returns "no" response.
        */
        if (LoadString(hMd,SFDL_YNLABEL,lpBuf->buf,sizeof(lpBuf->buf)))
        {
            _lwrite(lpBuf->hDLfile,lpBuf->buf,lstrlen(lpBuf->buf));
            DBGynprmpt(("endDLfont(): %ls\n", (LPSTR)lpBuf->buf));
        }
  
        _lclose(lpBuf->hDLfile);
        lpBuf->hDLfile = -1;
    }
  
    if (lpBuf->jobNum > 0)
    {
        CloseJob(lpBuf->jobNum);
        lpBuf->jobNum = 0;
  
        if (lpBuf->jobDC)
        {
            ReleaseDC((HWND)NULL, lpBuf->jobDC);
            lpBuf->jobDC = NULL;
        }
    }
}
  
/*  fileToSpooler
*
*  Copy the file to the spooler.
*/
LOCAL void fileToSpooler(jobNum, indSFfile, lpOfStruct, lpBuf, bufsz)
HANDLE jobNum;
int indSFfile;
LPOFSTRUCT lpOfStruct;
LPSTR lpBuf;
int bufsz;
{
    LPSFDIRFILE lpSFfile;
    int hFile;
    int readsz;
    int writesz;
  
    DBGdl(("fileToSpooler(%d,%d,%lp,%lp,%d)\n",
    jobNum, indSFfile, lpOfStruct, lpBuf, bufsz));
  
    if (indSFfile > -1 &&
        makeSFdirFileNm(0L,indSFfile,FALSE,lpBuf,bufsz) &&
//        (hFile=OpenFile(lpBuf,lpOfStruct,OF_READ)) > 0)
        (hFile=OpenFile(lpBuf,lpOfStruct,OF_READ)) >= 0)                                 // RK 09/04/91
    {
        do {
            readsz = _lread(hFile,lpBuf,bufsz);
            writesz = WriteSpool(jobNum,lpBuf,readsz);
  
        } while (writesz == bufsz);
  
        _lclose(hFile);
    }
}
  
/*  fileToBatch
*
*  Place the copy command in the batch file.
*/
LOCAL void fileToBatch(lpBuf, indSFfile, lpPortNm)
LPDLREC lpBuf;
int indSFfile;
LPSTR lpPortNm;
{
    int ind;
  
    DBGdl(("fileToBatch(%lp,%d,%lp)\n", lpBuf, indSFfile, lpPortNm));
  
    if ((ind=lstrlen(lpBuf->tmp1)+1) < sizeof(lpBuf->buf))
    {
        lstrcpy(lpBuf->buf, lpBuf->tmp1);
        lstrcat(lpBuf->buf, (LPSTR)"+");
  
        if (makeSFdirFileNm(0L,indSFfile,FALSE,&lpBuf->buf[ind],
            sizeof(lpBuf->buf)-ind) &&
            lstrlen(lpBuf->buf)+lstrlen(lpBuf->binary)+lstrlen(lpBuf->tmp2)+
            lstrlen(lpBuf->alpha)+1 < sizeof(lpBuf->buf))
        {
            lstrcat(lpBuf->buf, lpBuf->binary);
            lstrcat(lpBuf->buf, (LPSTR)"+");
            lstrcat(lpBuf->buf, lpBuf->tmp2);
            lstrcat(lpBuf->buf, lpBuf->alpha);
  
            textToStream(lpBuf,dl_bcopy,lpBuf->buf,lpPortNm,FALSE);
        }
    }
}
  
/*  flushDelayed
*/
LOCAL void flushDelayed(lpBuf, lpStrmNm, lpPortNm)
LPDLREC lpBuf;
LPSTR lpStrmNm;
LPSTR lpPortNm;
{
    DBGdl(("flushDelayed(%lp,%lp,%lp): %lp\n",
    lpBuf, lpStrmNm, lpPortNm, lpStrmNm));
  
    if (lpBuf->hDLfile > 0)
    {
        textToStream(lpBuf,dl_formfeed,(LPSTR)"\014",lpBuf->tmp3,TRUE);
        textToStream(lpBuf,dl_copy,lpStrmNm,lpPortNm,FALSE);
    }
  
    if (lpBuf->jobNum > 0 && lpBuf->hTmpFile > 0)
    {
        int readsz;
        int writesz;
  
        if (!lpBuf->spooling &&
            !(lpBuf->spooling=StartSpoolPage(lpBuf->jobNum)))
        {
            DBGdl(("flushDelayed(): *failed* to StartSpoolPage\n"));
            return;
        }
  
        _llseek(lpBuf->hTmpFile, 0L, 0);
  
        do {
            readsz = _lread(lpBuf->hTmpFile,lpBuf->buf,sizeof(lpBuf->buf));
            writesz = WriteSpool(lpBuf->jobNum,lpBuf->buf,readsz);
  
        } while (writesz == sizeof(lpBuf->buf));
  
        WriteSpool(lpBuf->jobNum,(LPSTR)"\014",1);
  
        EndSpoolPage(lpBuf->jobNum);
        lpBuf->spooling = FALSE;
  
        _lclose(lpBuf->hTmpFile);
        OpenFile(lpBuf->tmpfile,&lpBuf->ofstruct,OF_DELETE);
    }
}
  
/*  textToStream
*
*  Write text to the output channel(s).
*/
LOCAL void textToStream(lpBuf, action, lpData, lpStrmNm, concat)
LPDLREC lpBuf;
DLACTION action;
LPSTR lpData;
LPSTR lpStrmNm;
BOOL concat;
{
    DBGdl(("textToStream(%lp,%d,%lp,%lp,%d)\n",
    lpBuf, action, lpData, lpStrmNm, concat, lpData));
  
    if (lpBuf->hDLfile > 0)
    {
        /*  Text to batch file.
        */
        LPSTR lpAction;
  
        lpBuf->tmp[0] = ' ';
        lpBuf->tmp[1] = '\0';
  
        switch (action)
        {
            case dl_echo:
            case dl_delay:
            case dl_formfeed:
                lpAction = lpBuf->echo;
                lpBuf->tmp[1] = '>';
                lpBuf->tmp[2] = concat ? (BYTE)'>' : (BYTE)' ';
                lpBuf->tmp[3] = concat ? (BYTE)' ' : (BYTE)'\0';
                lpBuf->tmp[4] = '\0';
                break;
            case dl_copy:
            case dl_bcopy:
                lpAction = lpBuf->copy;
                break;
            case dl_erase:
                lpAction = lpBuf->erase;
                break;
            default:
                return;
        }
  
        if ((_lwrite(lpBuf->hDLfile,lpAction,lstrlen(lpAction)) <= 0) ||
            (_lwrite(lpBuf->hDLfile,(LPSTR)" ",1) <= 0) ||
            (_lwrite(lpBuf->hDLfile,lpData,lstrlen(lpData)) <= 0))
        {
            DBGdl(("writeStream(): *failed* to write to file\n"));
            _lclose(lpBuf->hDLfile);
            lpBuf->hDLfile = -1;
        }
  
        if (action != dl_erase)
        {
            _lwrite(lpBuf->hDLfile,lpBuf->tmp,lstrlen(lpBuf->tmp));
            _lwrite(lpBuf->hDLfile,lpStrmNm,lstrlen(lpStrmNm));
        }
  
        if (action == dl_bcopy)
        {
            _lwrite(lpBuf->hDLfile,lpBuf->binary,lstrlen(lpBuf->binary));
        }
  
        _lwrite(lpBuf->hDLfile,(LPSTR)CrLf,2);
    }
  
    if (lpBuf->jobNum > 0)
    {
        /*  Text to spooler.
        */
        switch (action)
        {
            case dl_echo:
                DBGdl(("textToStream(): spooling=%ls, fileSpooled=%ls\n",
                ((lpBuf->spooling) ? (LPSTR)"TRUE" : (LPSTR)"FALSE"),
                ((lpBuf->fileSpooled) ? (LPSTR)"TRUE" : (LPSTR)"FALSE")
                ));
  
                if (!lpBuf->spooling &&
                    !(lpBuf->spooling=StartSpoolPage(lpBuf->jobNum)))
                {
                    DBGdl(("textToStream(): *failed* to StartSpoolPage\n"));
                    break;
                }
                WriteSpool(lpBuf->jobNum,lpData,lstrlen(lpData));
  
                if (lpBuf->fileSpooled)
                {
                    EndSpoolPage(lpBuf->jobNum);
                    lpBuf->spooling = FALSE;
                    lpBuf->fileSpooled = FALSE;
                }
                break;
  
            case dl_delay:
                if (lpBuf->hTmpFile <= 0)
                {
                    if (GetTempFileName(0,(LPSTR)"PCL",0,lpBuf->tmpfile))
                    {
                        DBGdl(("textToStream(): temporary file is %ls\n",
                        lpBuf->tmpfile));
  
                        lpBuf->hTmpFile = OpenFile(lpBuf->tmpfile,
                        &lpBuf->ofstruct, OF_READWRITE);
                    }
                }
  
//                if (lpBuf->hTmpFile > 0)
                if (lpBuf->hTmpFile >= 0)                                                // RK 09/04/91
                {
                    _lwrite(lpBuf->hTmpFile,lpData,lstrlen(lpData));
                    _lwrite(lpBuf->hTmpFile,(LPSTR)CrLf,2);
                }
                break;
  
            case dl_copy:
            case dl_bcopy:
            case dl_erase:
            case dl_formfeed:
            default:
                return;
        }
    }
}
  
/*  canDownload
*
*  Return TRUE if the name of the downloadable font file exists so
*  the font can be downloaded.
*/
LOCAL BOOL canDownload(indSFfile)
int indSFfile;
{
    LPSFDIRFILE lpSFfile = 0L;
    BOOL cando = FALSE;
  
    if (lpSFfile=(LPSFDIRFILE)lockSFdirEntry(0L,indSFfile))
    {
        cando = (BOOL)lpSFfile->offsDLname;
  
        unlockSFdirEntry(indSFfile);
    }
  
    DBGdl(("canDownload(%d): %ls download\n",
    indSFfile, ((cando) ? (LPSTR)"can" : (LPSTR)"*cannot*")));
  
    return (cando);
}
  
/*  getDLbatchNm
*
*  Pick up the name of the download batch file.
*/
LOCAL BOOL getDLbatchNm(hMd, lpSFlb, lpBuf, lpPortNm)
HANDLE hMd;
LPSFLB lpSFlb;
LPDLREC lpBuf;
LPSTR lpPortNm;
{
    LPSFLBENTRY sflb = 0L;
    LPSFDIRFILE lpSFfile = 0L;
    LPSFDIRSTRNG lpSFpath = 0L;
    int ind;
  
    DBGdl(("getDLbatchNm(%d,%lp,%lp,%lp): %ls\n",
    hMd, lpSFlb, lpBuf, lpPortNm, lpPortNm));
  
    /*  Attempt to pull the full name of the download batch file
    *  from the win.ini file.
    */
    LoadString(hMd,SFDL_DLKEYNM,lpBuf->buf,sizeof(lpBuf->buf));
    lpBuf->dlfnm[0] = '\0';
    GetProfileString(lpBuf->appNm,lpBuf->buf,lpBuf->dlfnm,lpBuf->dlfnm,
    sizeof(lpBuf->dlfnm));
  
    /*  File name is in win.ini file.
    */
    if (lpBuf->dlfnm[0] != '\0')
    {
        DBGdl(("getDLbatchNm(): from win.ini = %ls\n", (LPSTR)lpBuf->dlfnm));
        return TRUE;
    }
  
    /*  Attempt to pull the name of the soft font directory from
    *  the win.ini file.
    */
    LoadString(hMd,SFADD_DIRKEYNM,lpBuf->buf,sizeof(lpBuf->buf));
    lpBuf->dlfnm[0] = '\0';
    GetProfileString(lpBuf->appNm,lpBuf->buf,lpBuf->dlfnm,lpBuf->dlfnm,
    sizeof(lpBuf->dlfnm));
  
    /*  If the directory is not there, then use the path of one of the
    *  download files.
    */
    if (lpBuf->dlfnm[0] == '\0')
    {
        for (ind=0, sflb=&lpSFlb->sflb[0]; ind < lpSFlb->free; ++ind, ++sflb)
        {
            if ((lpSFfile=(LPSFDIRFILE)lockSFdirEntry(0L,sflb->indSFfile)) &&
                (lpSFfile->indDLpath > -1) &&
                (lpSFpath=(LPSFDIRSTRNG)lockSFdirEntry(0L,lpSFfile->indDLpath)))
            {
                lmemcpy(lpBuf->dlfnm,lpSFpath->s,sizeof(lpBuf->dlfnm));
                lpBuf->dlfnm[sizeof(lpBuf->dlfnm)-1] = '\0';
                unlockSFdirEntry(lpSFfile->indDLpath);
                lpSFpath = 0L;
                unlockSFdirEntry(sflb->indSFfile);
                lpSFfile = 0L;
                break;
            }
  
            if (lpSFpath)
            {
                unlockSFdirEntry(lpSFfile->indDLpath);
                lpSFpath = 0L;
            }
  
            if (lpSFfile)
            {
                unlockSFdirEntry(sflb->indSFfile);
                lpSFfile = 0L;
            }
        }
    #ifdef DEBUG
        if (ind < lpSFlb->free)
        {
            DBGdl(("getDLbatchNm(): dir name from ind %d = %ls\n",
            ind, (LPSTR)lpBuf->dlfnm));
        }
        else
        {
            DBGdl(("getDLbatchNm(): could *not* find a dir name\n"));
        }
    #endif
    }
    #ifdef DEBUG
    else
    {
        DBGdl(("getDLbatchNm(): dir name from win.ini = %ls\n",
        (LPSTR)lpBuf->dlfnm));
    }
    #endif
  
    /*  Append the name of the file to the path and write it to
    *  the win.ini file.
    */
    if (lpBuf->dlfnm[0] != '\0')
    {
        ind = lstrlen(lpBuf->dlfnm);
  
        if (lpBuf->dlfnm[ind-1] != '\\')
        {
            lpBuf->dlfnm[ind++] = '\\';
            lpBuf->dlfnm[ind] = '\0';
        }
  
        if (makeDLbatchNm(hMd,lpPortNm,&lpBuf->dlfnm[ind],
            sizeof(lpBuf->dlfnm)-ind) &&
            LoadString(hMd,SFDL_DLKEYNM,lpBuf->buf,sizeof(lpBuf->buf)))
        {
            WriteProfileString(lpBuf->appNm,lpBuf->buf,lpBuf->dlfnm);
  
            DBGdl(("getDLbatchNm(): ...name is %ls\n", lpBuf->dlfnm));
            return TRUE;
        }
    }
  
    return FALSE;
}
  
/*  makeDLbatchNm
*/
LOCAL BOOL makeDLbatchNm(hMd, lpPortNm, lpName, namesz)
HANDLE hMd;
LPSTR lpPortNm;
LPSTR lpName;
int namesz;
{
    LPSTR s;
  
    if (namesz < 13)
    {
        DBGdl(("makeDLbatchNm(%d,%lp,%lp,%d): filename not big enough\n",
        hMd, lpPortNm, lpName, namesz));
        return FALSE;
    }
  
    lstrcpy(lpName, "SF");
  
    /*  Concat the port name to the file prefix.
    */
    if (!lpPortNm || !lstrlen(lpPortNm))
    {
        lstrcat(lpName, "NONE");
    }
    else
    {
        s = lpPortNm + lstrlen(lpPortNm);
  
        /*  First get the filename part of the port (i.e., strip
        *  off any path names.
        */
        if (s[-1] == ':')
            --s;
        for (; s > lpPortNm && s[-1] != ':' && s[-1] != '\\'; --s)
            ;
        lmemcpy(&lpName[lstrlen(lpName)], s, 8);
        lpName[8] = '\0';
  
        /*  Truncate the name at any invalid file-name characters.
        */
        for (s=lpName+lstrlen(lpName)-1; s > lpName; --s)
        {
            if (*s == ':' || *s == '\\' || *s == '.')
            {
                *s = '\0';
            }
        }
    }
    lpName[8] = '\0';
  
    if (!lstrlen(lpName))
    {
        DBGdl(("makeDLbatchNm(): screwed up building file name\n"));
        lstrcpy(lpName, "SFNONE");
    }
  
    /*  Add extension to file name.
    */
    lstrcat(lpName, ".BAT");
  
    DBGdl(("makeDLbatchNm(%d,%lp,%lp,%d): %ls\n",
    hMd, lpPortNm, lpName, namesz, lpName));
  
    return TRUE;
}
  
/*  canMakeYN
*
*  Check for the presence of the ynprompt DOS program.  If it is not
*  there, then try to read it from the resources and write it to the
*  same directory containing the dl batch file.
*/
LOCAL BOOL canMakeYN(hMd, lpBuf)
HANDLE hMd;
LPDLREC lpBuf;
{
    HANDLE hResInfo;
    LPSTR s;
    BOOL success = FALSE;
    int hResFile;
    int hFile;
    int k;
  
    lpBuf->ynfnm[0] = '\0';
  
    /*  Get the path from the name of the batch file.
    */
    for (s=&lpBuf->dlfnm[k=lstrlen(lpBuf->dlfnm)];
        k > 0 && s[-1] != '\\' && s[-1] != ':'; --s, --k)
        ;
  
    if (k > 0 && k < sizeof(lpBuf->ynfnm))
        lmemcpy(lpBuf->ynfnm, lpBuf->dlfnm, k);
    else
        k = 0;
  
    /*  Append name of ynprompt program to path.
    */
    LoadString(hMd,SFDL_YNFILENM,&lpBuf->ynfnm[k],sizeof(lpBuf->ynfnm)-k);
  
    DBGynprmpt(("canMakeYN(%lp): ynprompt=%ls\n", lpBuf, (LPSTR)lpBuf->ynfnm));
  
    /*  Test for the existence of the ynprompt program.  If it
    *  is not there, then read it from the resources and write
    *  it out.
    */
    if (dos_opend((LPDIRDATA)lpBuf->buf,lpBuf->ynfnm,0x01) == DOS_NOFILES)
    {
        DBGynprmpt(("canMakeYN(): ynprompt file does not exist\n"));
  
        if ((hResInfo=FindResource(hMd,(LPSTR)YNPROMPT,(LPSTR)EXECUTABLE)) &&
            (k=SizeofResource(hMd,hResInfo)) > 0 &&
            (hResFile=AccessResource(hMd,hResInfo)) >= 0)
        {
            DBGynprmpt(("canMakeYN(): hResFile=%d, size=%d\n", hResFile, k));
  
            lmemset(lpBuf->buf, 0, sizeof(OFSTRUCT));
  
//            if ((hFile=OpenFile(lpBuf->ynfnm,(LPOFSTRUCT)lpBuf->buf,
//                OF_CREATE | OF_WRITE)) > 0)
            if ((hFile=OpenFile(lpBuf->ynfnm,(LPOFSTRUCT)lpBuf->buf,
                OF_CREATE | OF_WRITE)) >= 0)                                             // RK 09/04/91
            {
                int size=k;
  
                do {
                    if ((k=sizeof(lpBuf->buf)) > size)
                        k = size;
  
                    k = _lread(hResFile, lpBuf->buf, k);
                    _lwrite(hFile, lpBuf->buf, k);
                    size -= k;
  
                } while (k == sizeof(lpBuf->buf));
  
                _lclose(hFile);
  
                if (size == 0)
                    success = TRUE;
            }
  
            _lclose(hResFile);
        }
    }
    else
    {
        DBGynprmpt(("canMakeYN(): ynprompt file already exists.\n"));
        success = TRUE;
    }
  
    return (success);
}
  
/*  writeYNprompt
*
*  Write the yes/no prompt to the batch file.  This prompt is a separate
*  DOS program that asks the user to confirm that fonts should be down-
*  loaded.  If the user responds "no," the program returns a DOS errlevel
*  of one (1).  A batch file command is written which jumps over everything
*  in the file if this hapens.
*/
LOCAL void writeYNprompt(hMd, lpBuf, lpPortNm)
HANDLE hMd;
LPDLREC lpBuf;
LPSTR lpPortNm;
{
    /*  Build the string invoking the ynprompt program.  It accepts
    *  one argument:  the name of the port.
    */
    if (lpBuf->hDLfile > 0 && lstrlen(lpBuf->buf)+lstrlen(lpBuf->ynfnm)+
        lstrlen(lpPortNm)+3 < sizeof(lpBuf->buf))
    {
        lstrcpy(lpBuf->buf, lpBuf->ynfnm);
        lstrcat(lpBuf->buf, (LPSTR)" ");
        lstrcat(lpBuf->buf, lpPortNm);
        lstrcat(lpBuf->buf, (LPSTR)CrLf);
        _lwrite(lpBuf->hDLfile,lpBuf->buf,lstrlen(lpBuf->buf));
  
        DBGynprmpt(("writeYNprompt(): %ls", (LPSTR)lpBuf->buf));
  
        /*  Write the check for err level after ynprompt.
        */
        if (LoadString(hMd,SFDL_YNTEST,lpBuf->buf,sizeof(lpBuf->buf)))
        {
            _lwrite(lpBuf->hDLfile,lpBuf->buf,lstrlen(lpBuf->buf));
            DBGynprmpt(("writeYNprompt(): %ls", (LPSTR)lpBuf->buf));
        }
    }
    #ifdef DEBUG
    else
    { DBGynprmpt(("writeYNprompt(%lp): hDLfile=%d, cannot make string\n",
    lpBuf, lpBuf->hDLfile)); }
    #endif
}
  
/*  makeDIRfnm
*
*  Make the FINSTALL.DIR file name.
*/
LOCAL BOOL makeDIRfnm(hMd, lpBuf)
HANDLE hMd;
LPDLREC lpBuf;
{
    LPSTR s;
    int k;
  
    lpBuf->dirfnm[0] = '\0';
  
    /*  Get the path from the name of the batch file.
    */
    for (s=&lpBuf->dlfnm[k=lstrlen(lpBuf->dlfnm)];
        k > 0 && s[-1] != '\\' && s[-1] != ':'; --s, --k)
        ;
  
    if (k > 0 && k < sizeof(lpBuf->dirfnm))
        lmemcpy(lpBuf->dirfnm, lpBuf->dlfnm, k);
    else
        return FALSE;
  
    /*  Append name of FINSTALL.DIR to path.
    */
    LoadString(hMd,SFADD_DEFDIRF,&lpBuf->dirfnm[k],sizeof(lpBuf->dirfnm)-k);
  
    DBGdirfile(("makeDIRfnm(%d,%lp): %ls\n", hMd, lpBuf, (LPSTR)lpBuf->dirfnm));
  
    return TRUE;
}
  
/*  BuildSFINSTAL_DIR
*
*  Traverse the sflb structure building a FINSTALL.DIR file.  The easiest
*  way to figure out what this proc does is to look up documentation on
*  the format of the FINSTALL.DIR file (try PageMaker Developers' notes).
*/
LOCAL void BuildSFINSTAL_DIR (hDB, hMd, lpBuf, lpSFlb, lpPortNm)
HWND hDB;
HANDLE hMd;
LPDLREC lpBuf;
LPSFLB lpSFlb;
LPSTR lpPortNm;
{
    LPSFDIRFILE lpSFfile;
    LPSFLBENTRY sflb;
    register LPSTR s;
    LPSTR t;
    int hFile;
    int ind;
    register int k;
    int n;
    int fCarts=FALSE;
  
    DBGdirfile(("BuildSFINSTAL_DIR(%d,%d,%lp,%lp,%lp)\n",
    hMd, hDB, lpBuf, lpSFlb, lpPortNm));
  
    lmemset((LPSTR)&lpBuf->ofstruct, 0, sizeof(OFSTRUCT));
  
    /*  Open FINSTALL.DIR file.
    */
//    if (lpBuf->dirfnm[0] != '\0' &&
//       (hFile=OpenFile(lpBuf->dirfnm,&lpBuf->ofstruct,
//        OF_WRITE | OF_CREATE)) > 0)
    if (lpBuf->dirfnm[0] != '\0' &&
        (hFile=OpenFile(lpBuf->dirfnm,&lpBuf->ofstruct,
        OF_WRITE | OF_CREATE)) >= 0)                                                     // RK 09/04/91
    {
        s = lpBuf->buf;
        k = sizeof(lpBuf->buf) - 1;
  
        /*  Write:  FAMILY "PCL / HP LaserJet on <port>" {
        */
        *s = '\0';
        LoadString(hMd, SFDL_FAMILY, s, k);
        k -= lstrlen(s);
  
        if ((n=lstrlen(lpPortNm)) < k)
        {
            lstrcat(s, lpPortNm);
            k -= n;
        }
        else
            k = 0;
  
        if (k > 5)
        {
            lstrcat(s, (LPSTR)"\" {\r\n");
            k -= 5;
        }
        else
            k = 0;
  
        if (k > 0)
        {
            _lwrite(hFile, s, sizeof(lpBuf->buf)-1-k);
            DBGdirfile(("%ls", s));
        }
        #ifdef DEBUG
        else
        { DBGdirfile(
        ("BuildSFINSTAL_DIR(): *failed* to make FAMILY line")); }
        #endif
  
  
        /*  For each font in the listbox.
        */
        for (ind=0, sflb=&lpSFlb->sflb[0]; ind < lpSFlb->free;
            ++ind, ++sflb)
        {
  
            /* suppress cartridge output but remember we saw 'em */
            if (sflb->id<0)
            {
                fCarts=TRUE;
                continue;
            }
  
            /*  Lock down the sfdir entry.
            */
            if (lpSFfile=(LPSFDIRFILE)lockSFdirEntry(0L,sflb->indSFfile))
            {
                s = lpBuf->buf;
                k = sizeof(lpBuf->buf) - 1;
  
                /*  Write:  "<description-string>" =<space>
                */
                if (k > 3)
                {
                    lstrcpy(s, (LPSTR)"  \"");
                    k -= 3;
                }
                else
                    k = 0;
  
                if ((n=lstrlen(t=lpSFfile->s)) < k)
                {
                    lstrcat(s, t);
                    k -= n;
                }
                else
                    k = 0;
  
                if (k > 4)
                {
                    lstrcat(s, (LPSTR)"\" = ");
                    k -= 4;
                }
                else
                    k = 0;
  
                /*  Write:  <orientation>,<space>
                */
                if (k > 4) switch (lpSFfile->orient)
                {
                    case 0:
                        lstrcat(s, (LPSTR)"PL, ");
                        k -= 4;
                        break;
                    case 1:
                        lstrcat(s, (LPSTR)"P, ");
                        k -= 3;
                        break;
                    case 2:
                        lstrcat(s, (LPSTR)"L, ");
                        k -= 3;
                        break;
                    default:
                        k = 0;
                        break;
                }
                else
                    k = 0;
  
                /*  Write:  <dl-file>
                */
                if (lpSFfile->offsDLname)
                {
                    if ((n=lstrlen(t=&lpSFfile->s[lpSFfile->offsDLname])) < k)
                    {
                        lstrcat(s, t);
                        k -= n;
                    }
                    else
                        k = 0;
                }
                else if (!lpSFfile->offsPFMname)
                    k = 0;
  
                /*  Write:  , <pfm-file>
                */
                if (lpSFfile->offsPFMname)
                {
                    if (k > 2)
                    {
                        lstrcat(s, (LPSTR)CommaStr);    /* ", " */
                        k -= 2;
                    }
                    else
                        k = 0;
  
                    if ((n=lstrlen(t=&lpSFfile->s[lpSFfile->offsPFMname])) < k)
                    {
                        lstrcat(s, t);
                        k -= n;
                    }
                    else
                        k = 0;
                }
  
                /*  Write:  <eoline>
                */
                if (k > 2)
                {
                    lstrcat(s, (LPSTR)CrLf);
                    k -= 2;
                }
                else
                    k = 0;
  
                /*  If everything happened successfully, dump the
                *  line to file.
                */
                if (k > 0)
                {
                    _lwrite(hFile, s, sizeof(lpBuf->buf)-1-k);
                    DBGdirfile(("%ls", s));
                }
                #ifdef DEBUG
                else
                { DBGdirfile(
                    ("BuildSFINSTAL_DIR(): *failed* to make line at %d",
                sflb->indSFfile)); }
                #endif
  
                unlockSFdirEntry(sflb->indSFfile);
            }
        }
  
        /*  Write:  }
        */
        _lwrite(hFile, (LPSTR)"}\r\n", 3);
        DBGdirfile(("}\r\n"));
  
        /* we've got the PFM's, now blow out any cartridges we might have */
        if (fCarts)
        {
            s = lpBuf->buf;
            k = sizeof(lpBuf->buf)-1;
            *s = 0;
  
            k -= LoadString(hMd, SFDL_CARTRIDGE, s, k);
  
            if (k>0)
            {
                _lwrite(hFile,s,sizeof(lpBuf->buf)-1-k);
                DBGdirfile(("%ls",s));
            }
  
            /* loop through all the listbox entries */
            for (ind=0, sflb=lpSFlb->sflb; ind<lpSFlb->free; ++ind,++sflb)
            {
  
                /* we already wrote soft fonts */
                if (sflb->id>0)
                    continue;
  
                if (lpSFfile=(LPSFDIRFILE)lockSFdirEntry(0L,sflb->indSFfile))
                {
                    s = lpBuf->buf;
                    k = sizeof(lpBuf->buf) - 1;
  
                    /* write "cartridgename" = pcmfile.pcm */
                    if (k>3)
                    {
                        lstrcpy(s, (LPSTR)"  \"");
                        k-=3;
                    }
                    else
                        k=0;
  
                    if ((n=lstrlen(t=lpSFfile->s))<k)
                    {
                        lstrcat(s,t);
                        k -= n;
                    }
                    else
                        k = 0;
  
                    if (k>4)
                    {
                        lstrcat(s, "\" = ");
                        k-=4;
                    }
                    else
                        k=0;
  
                    if ((n=lstrlen(t=lpSFfile->s+lpSFfile->offsPFMname))<k)
                    {
                        lstrcat(s, t);
                        k-=n;
                    }
                    else
                        k=0;
  
                    if (k>2)
                    {
                        lstrcat(s, CrLf);
                        k-=2;
                    }
                    else
                        k=0;
  
                    if (k>0)
                    {
                        _lwrite(hFile, s, sizeof(lpBuf->buf)-1-k);
                        DBGdirfile(("%ls",s));
                    }
  
                    unlockSFdirEntry(sflb->indSFfile);
  
                }
            }
            _lwrite(hFile,"}\r\n",3);
            DBGdirfile(("}\r\n"));
        }
        _lclose (hFile);
    }
    else
    {
        /*  Failed to create the file, report an err message.
        */
        if (LoadString(hMd,SFINSTAL_NM,lpBuf->buf,sizeof(lpBuf->buf)) &&
            (s+=k=lstrlen(s=lpBuf->buf)+1) &&
            LoadString(hMd,SFDL_NOSFDMSG,s,sizeof(lpBuf->buf)-k))
        {
            if ((n=lstrlen(lpBuf->dirfnm)) < sizeof(lpBuf->buf)-k-lstrlen(s))
            {
                if (n > 0)
                    lstrcat(s, lpBuf->dirfnm);
                else
                    lstrcat(s, (LPSTR)"*");
            }
  
            MessageBox(hDB, s, lpBuf->buf, MB_OK | MB_ICONEXCLAMATION);
        }
    }
}
  
/*  mylmemcmp
*/
LOCAL BOOL mylmemcmp(a, b, len)
LPSTR a;
LPSTR b;
int len;
{
    while (len-- > 0)
    {
        if (*a++ != *b++)
            return FALSE;
    }
  
    return TRUE;
}
  
/*  sortdownload.  Take the SFLB structure passed in and sort its chain of
*  SFLBENTRY entries by fileSize in descending order.  This is necessary
*  to avoid fragmentation of the RAM cartridge(s) in the DeskJet Family
*  of printers when the DL (soft font) files are downloaded to the printer.
*/
LOCAL void sortdownload(lpSFlb)
LPSFLB lpSFlb;
{
    LPSFLBENTRY  sflb = 0L;
    SFLBENTRY    tempsflb;
    OFSTRUCT     ofStruct;
    int          hFile = -1;
    int          ind, num, j;
    char         buf[SFDLBUFSZ];
    char         fmthdr[SFDLBUFSZ];
    char         esc, ch, tmpc;
  
    /*  If the file is a DL file, get its size from the format header.
    */
    for (ind=0, sflb=&lpSFlb->sflb[0]; ind < lpSFlb->free;
        ++ind, ++sflb)
//        if ((canDownload(sflb->indSFfile)) &&
//            (makeSFdirFileNm(0L, sflb->indSFfile, FALSE, buf, SFDLBUFSZ)) &&
//            ((hFile=OpenFile(buf, &ofStruct, OF_READ)) > 0)
//            )
        if ((canDownload(sflb->indSFfile)) &&
            (makeSFdirFileNm(0L, sflb->indSFfile, FALSE, buf, SFDLBUFSZ)) &&
            ((hFile=OpenFile(buf, &ofStruct, OF_READ)) >= 0)                             // RK 09/04/91
            )
        {
            /*  Get the font_data_size from the header.
            */
            if ((_lread(hFile, (LPSTR)&esc, 1) > 0) &&
                (esc == '\033') &&
                (_lread(hFile, (LPSTR)&ch, 1) > 0) &&
                (ch == ')') &&
                (_lread(hFile, (LPSTR)&tmpc, 1) > 0) &&
                (tmpc == 's') &&
                (tmpc = readnum(hFile, &num)) &&
                (tmpc == 'W')
                )
            {
                _lread(hFile, fmthdr,
                (num > SFDLBUFSZ) ? SFDLBUFSZ : num);
                sflb->fileSize =
                swab(((LPDJFONTDES)fmthdr)->font_data_size);
  
                //  font_data_size for DeskJet PLUS fonts are stored
                //  as kbytes, whereas DeskJet fonts are stored as
                //  bytes.  Divide DeskJet fonts by 1000 so both are
                // in kbytes.
  
                if (((LPDJFONTDES)fmthdr)->header_format ==
                    DESKJET_FONT)
                    sflb->fileSize /= 1000;
                DBGdl(
                ("sortdownload(): %ls fileSize %d\n",
                (LPSTR)buf, sflb->fileSize));
            }
            _lclose(hFile);
        }
    /*  Sort the structure by fileSize in descending order.
    */
    for (ind=0; ind < lpSFlb->free-1; ++ind)
        for (j=0; j < lpSFlb->free-1-ind; ++j)
            if (lpSFlb->sflb[j].fileSize < lpSFlb->sflb[j+1].fileSize)
            {
                tempsflb.native    = lpSFlb->sflb[j].native;
                tempsflb.state     = lpSFlb->sflb[j].state;
                tempsflb.id        = lpSFlb->sflb[j].id;
                tempsflb.indSFfile = lpSFlb->sflb[j].indSFfile;
                tempsflb.fileSize  = lpSFlb->sflb[j].fileSize;
                lpSFlb->sflb[j].native    = lpSFlb->sflb[j+1].native;
                lpSFlb->sflb[j].state     = lpSFlb->sflb[j+1].state;
                lpSFlb->sflb[j].id        = lpSFlb->sflb[j+1].id;
                lpSFlb->sflb[j].indSFfile = lpSFlb->sflb[j+1].indSFfile;
                lpSFlb->sflb[j].fileSize  = lpSFlb->sflb[j+1].fileSize;
                lpSFlb->sflb[j+1].native    = tempsflb.native;
                lpSFlb->sflb[j+1].state     = tempsflb.state;
                lpSFlb->sflb[j+1].id        = tempsflb.id;
                lpSFlb->sflb[j+1].indSFfile = tempsflb.indSFfile;
                lpSFlb->sflb[j+1].fileSize  = tempsflb.fileSize;
            }
} //  End sortdownload.
  
/*  readnum
*/
LOCAL BYTE readnum(hFile, lpNum)
int hFile;
int FAR *lpNum;
{
    BYTE ch;
  
    *lpNum = 0;
  
    while (_lread(hFile,(LPSTR)&ch,1) > 0 && ch >= '0' && ch <= '9')
    {
        *lpNum *= 10;
        *lpNum += (int)ch - (int)'0';
    }
  
    if (*lpNum == 0)
        ch = '\0';
  
    return (ch);
}
