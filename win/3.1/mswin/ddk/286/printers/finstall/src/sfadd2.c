/**[f******************************************************************
* sfadd2.c -
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

/********************************   sfadd2.c   ******************************/
//
//  SFadd2:  Module for adding fonts.
//
// History
//
// 04 Sep 91    rk(HP)  Changed OpenFile compare in CopyFile                  
// 20 Aug 91    rk(HP)  Added external reference to hLibInst to be used in    
//              TFMtoPFM for LoadString of SF_INSTALLAF, SF_READING, and SF_WRITING
// 17 Jul 91    rk(HP)  Added local buffer in copyScrnFnt because of GP fault
//              when using lpBuf->buf as lpDefault in GetProfileString.
// 08 oct 89    peterbe No longer disable listboxes in AddFonts()
// 28 sep 89    peterbe Reversed test of handle returned by lzOpenFile() near
//          beginning of copyFile() -- to fix bug about copying
//          from floppy drives.
//          In AddFonts(), disable redraw while copying.  This
//          still needs a bit of work. Look at routines this calls!
// 24 sep 89    peterbe Added DBGcopy()
// 19 sep 89    peterbe Just added some DBG messages
// 18 sep 89    peterbe Send WM_FONTCHANGE in addition to WM_WININICHANGE when
//          screen fonts are copied.
// 07 aug 89    peterbe Changed all lstrcmp() to lstrcmpi().
// 01 aug 89    peterbe Changed DOS/WIN file calls for hsrcFile to
//          LZEXPAND calls.
//  7 jun 89    peterbe Corrected typo on last edit
//  2 jun 89    peterbe More cleanup
// 22 mar 89    peterbe Format cleanup.
//   1-26-89    jimmat  Adjustments for changes in resource file.
//   2-20-89    jimmat  Font Installer/Driver use same WIN.INI section (again)!
//
/***************************************************************************/
  
  
//#define DEBUG
  
  
#ifdef SFDEBUG
#define DEBUG
#endif
  
#include <ctype.h>
  
#include "nocrap.h"
#undef NOMSG
#undef NOOPENFILE
#undef NOMEMMGR
#undef NOWINMESSAGES
#undef NOCTLMGR
#undef NOSCROLL
#undef NOMB
#undef NOGDI
#include "windows.h"
#include "neededh.h"
#include "resource.h"
#include "strings.h"
#include "pfm.h"
#include "sfadd.h"
#include "sfdir.h"
#include "sflb.h"
#include "sfpfm.h"
#include "sfedit.h"
#include "sfinstal.h"
#define NOBLDDESCSTR
#include "sfutils.h"
#include "dlgutils.h"
#include "dosutils.h"
#include "expand.h"
#include "tfmread.h"
#include "_tmu.h"
#include "_sflib.h"
#include "glue.h"
  
  
/****************************************************************************\
*                     Debug Definitions
\****************************************************************************/
  
#ifdef DEBUG
#define DBGcopy(msg)       /* DBMSG(msg) */
#define DBGx(msg)          /* DBMSG(msg) */
#define DBGdlgfn(msg)      /* DBMSG(msg) */
#define DBGaddfonts(msg)   /* DBMSG(msg) */
#define DBGchkdu(msg)      /* DBMSG(msg) */
#else
#define DBGx(msg)          /* nothing */
#define DBGcopy(msg)        /* nothing */
#define DBGdlgfn(msg)       /* nothing */
#define DBGaddfonts(msg)   /* nothing */
#define DBGchkdu(msg)       /* nothing */
#endif
  
  
/*  Define this switch if you want the edit dialog to stay
*  up when lots of fonts need editing (see buildPFM).
*/
#undef KEEPITUP
  
#define LOCAL static
  
#define MINDISKFREE 4096
#define ALRTDISKFREE    1048576
  
  
/*  Flags used by copyFile and copyScrnFont.
*/
#define NOT_COPIED  0
#define NEWFILE_COPIED  1
#define SRC_EQ_DST  2
#define DUP_SCRFNT  3
  
  
/*  Structure used by AddFont().
*/
typedef struct {
    char buf[1024];     /* General buffer */
    char path[80];      /* Destination directory path */
    char pfmFile[80];       /* PFM file name */
    char dlFile[80];        /* Download file name */
    char appName[64];       /* Application name for win.ini */
    char sfstr[32];     /* SoftFontn= line from win.ini */
    char cartstr[32];       /* cartridgen= line from win.ini */
    char adding[32];        /* "Adding: " for status line */
    char bldpfm[32];        /* "Building PFM: " for status line */
    char point[32];     /* For description */
    char bold[32];      /* For description */
    char italic[32];        /* For description */
    char fntAppNm[32];      /* App name for fonts section in win.ini */
    int canReplace;     /* ==1 if can replace existing files */
    BOOL duWrned;       /* TRUE if user wrned re: low disk space */
    int fontCount;      /* Number of fonts in dst listbox */
    EDREC edrec;        /* Edit record for editing PFM face */
    OFSTRUCT ofstruct;      /* Open file struct */
    DIRDATA dirdata;        /* Directory data for _opend */
    PFMHEADER pfmHead;      /* PFM file header */
    WORD widths[256];       /* PFM file width table */
    PFMEXTENSION pfmExt;    /* PFM file extension (comes after widths) */
    char face[80];      /* PFM file font face */
    char device[80];        /* PFM file printer name */
    EXTTEXTMETRIC extText;  /* PFM file extended text metrics */
    DRIVERINFO drvInfo;     /* PFM file driver info struct */
} ADDREC;
typedef ADDREC FAR *LPADDREC;
  
  
  
// typedef struct
//  {                   /* -------------------------------------------- */
//  HANDLE hFiles;      /* Handle to memory containing all file names   */
//  WORD length;        /* Length of memory                             */
//  WORD NextEntry;     /* Next free space to store file name in memory */
//  int NumFiles;       /* Number of file names stored in block         */
//  } DIRECTORY, FAR * LPDIRECTORY;     /* -------------------------------------------- */
//
//typedef struct
//{
//char Name[64];
//BOOL IsDJFont;
//char Creator[16];
//int DisplayRes;
//char SymbolSet[5];
//char ScreenSizes[MAX_NUMBER_OF_SCREEN_FONTS+1];
//} SCRNFNTINFO, FAR * LPSCRNFNTINFO;
//
//#define MAXNAME 80
//
//typedef struct
//{
//    LONG id;              /* typeface id */
//    LONG complement;          /* character complement */
//    LONG space_req;           /* space required for the library */
//    BYTE typefaceName[51];              /* null terminated string */
//    BYTE nameOrDir[MAXNAME];
//    BYTE pad;     /* To make the if.dsc agree with the dos built version */
//} TYPEINFO;
//typedef TYPEINFO FAR * LPTYPEINFO;
//
//
//typedef struct
//{
//int   Type;
//WORD  Length;
//LPSTR next;
//LPSTR prev;
//int   Listbox;
//DWORD ListboxEntry;
//BOOL Selected;
//int  usage;
//SCRNFNTINFO ScrnFntInfo;
//char ScreenSizes[MAX_NUMBER_OF_SCREEN_FONTS+1];
//TYPEINFO TypeInfo;
//WORD name_off;
//WORD OffsetName;
//WORD OffsetPath;
//int ScreenType;
//} SFI_FONTLIBENTRY, far *LPSFI_FONTLIBENTRY;
  
/*  Forward references
*/
LOCAL BOOL TFMtoPFM(HWND, LPADDREC, LPSFDIRFILE, BOOL, BOOL);
LOCAL BOOL buildPFM(HWND, HANDLE, LPADDREC, int);
LOCAL int openUniqPFM(LPADDREC, int);
LOCAL WORD copyFile(HWND, HANDLE, LPADDREC, LPSTR, BOOL, int, int);
LOCAL WORD copyScrnFnt(HWND, HANDLE, LPADDREC, int);
LOCAL BOOL removeCopiedFile(LPSTR, int, BOOL, LPSTR, int);
LOCAL int isABdrive(LPSTR);
LOCAL BOOL checkAlertDU(
HWND, HANDLE, LPSTR, LPSTR, DWORD, BOOL FAR *, LPSTR, int);
  
/* external function definitions */
extern void FAR PASCAL tfmread(HANDLE, LPSTR);
extern void FAR PASCAL FreeTFM(HANDLE);
extern void FAR PASCAL wt_pfm(HANDLE, int, DWORD, LPSTR, LPSTR, BYTE, BOOL);
extern HANDLE FAR PASCAL get_cart_info(int, LPSTR, LPINT);
  
extern HANDLE gHG;      // Handle to Glue info
extern int gTotal;      // # of soft fonts and carts in glue file
extern int gGluein;     // file handle to glue.txt
extern HANDLE hLibInst; // global library instance handle
  
/**************************************************************************/
/****************************   Global Procs   ****************************/
  
/*  AddFonts
*/
HANDLE FAR PASCAL
AddFonts(
hDB, hMd, iddstLB, hdstSFlb, idsrcLB, hsrcSFlb, JustDoIt, lpModNm, lpPortNm, lpCount, lpFntLib, PCLV)
HWND hDB;
HANDLE hMd;
WORD iddstLB;
HANDLE hdstSFlb;
WORD idsrcLB;
HANDLE hsrcSFlb;
BOOL FAR *JustDoIt;        /* TRUE = call WriteSelections later */
LPSTR lpModNm;
LPSTR lpPortNm;
WORD FAR *lpCount;
LPSTR lpFntLib;
BOOL PCLV;          /* TRUE = PCL V device */
  
{
    MSG msg;
    LPSFLB lpsrcSFlb = 0L;
    LPSFLBENTRY sflb = 0L;
    LPSFLBENTRY j, k;
    LPSFDIRFILE lpSFfile = 0L;
    LPSFDIRSCRFNT lpSFscrn = 0L;
    LPSFDIRLOGDRV lpSFdrv = 0L;
    LPADDREC lpBuf = 0L;
    HANDLE hBuf = 0;
    LPDIRECTORY lpFontLibrary = (LPDIRECTORY)lpFntLib;
    LPSFI_FONTLIBENTRY lpLib;
    LPSTR lpAppNm;
    BOOL existDL;               /* TRUE if download file exists */
    BOOL needPFM;               /* TRUE if there's no PFM/PCM */
    BOOL scrnFnts;              /* TRUE if we have screen fonts */
    BOOL fIsPCM;                    /* TRUE if file is a PCM */
    BOOL NoBitmaps = TRUE;
    WORD priority, best_priority;
    WORD pos, prevPos;
    WORD dlrc;
    int curLOGdrv;
    int indLOGdrv;
    int indScrnFnt;
    int ind, i, lbnum;
    int id;
    int NumFiles = 0;
    LPSTR lpVF = NULL;
  
    DBGaddfonts(("AddFonts(%d,%d,%d,%d,%d,%d,%lp): %ls\n",
    (WORD)hDB, (WORD)hMd, iddstLB, hdstSFlb, idsrcLB, hsrcSFlb,
    lpPortNm, lpPortNm));
  
    *lpCount = 0;
  
    if (hsrcSFlb &&
        (hBuf=GlobalAlloc(GMEM_FIXED, (DWORD)sizeof(ADDREC))) &&
        (lpBuf=(LPADDREC)GlobalLock(hBuf)) &&
        LoadString(hMd, SF_SOFTFONT, lpBuf->sfstr, sizeof(lpBuf->sfstr)) &&
        LoadString(hMd, SF_CARTRIDGE, lpBuf->cartstr,sizeof(lpBuf->cartstr)) &&
        LoadString(hMd, SF_PFMDEVNM, lpBuf->device, sizeof(lpBuf->device)) &&
        LoadString(hMd, SFADD_ADDING, lpBuf->adding, sizeof(lpBuf->adding)) &&
        LoadString(hMd, SFADD_BLDPFM, lpBuf->bldpfm, sizeof(lpBuf->bldpfm)) &&
        LoadString(hMd, SF_POINT, lpBuf->point, sizeof(lpBuf->point)) &&
        LoadString(hMd, SF_BOLD, lpBuf->bold, sizeof(lpBuf->bold)) &&
        LoadString(hMd, SF_ITALIC, lpBuf->italic, sizeof(lpBuf->italic)) &&
        LoadString(hMd, SF_APPFNT, lpBuf->fntAppNm, sizeof(lpBuf->fntAppNm)) &&
        (lpsrcSFlb=(LPSFLB)GlobalLock(hsrcSFlb)))
    {
        lpAppNm = lpBuf->appName;
  
        /*  Build "[<driver>,<port>]" for accessing win.ini file.
        */
        MakeAppName(lpModNm,lpPortNm,lpAppNm,sizeof(lpBuf->appName));
  
        DBGx(("lpsrcSFlb->len = %d\n", lpsrcSFlb->len));
        DBGx(("lpsrcSFlb->free = %d\n", lpsrcSFlb->free));
        for (lbnum = 0; lbnum < lpsrcSFlb->free; lbnum++)
        {
  
            DBGx((" lbnum = %d, state = %lp\n", lbnum,
            (long)lpsrcSFlb->sflb[lbnum].state));
            if ((lpsrcSFlb->sflb[lbnum].state & SFLB_SEL) &&
                (!(lpsrcSFlb->sflb[lbnum].state & SFLB_FAIS)))
            {
                DBGx((" No Bitmaps == FALSE \n"));
                NoBitmaps = FALSE;
            }
        }
  
  
  
  
        if (NoBitmaps || (GetTargDir(hDB,hMd,lpBuf->path,sizeof(lpBuf->path),lpAppNm)))
        {
            /*  Remove highlight and gray listbox.
            */
            SendMessage(GetDlgItem(hDB,idsrcLB), WM_SETREDRAW, FALSE, 0L);
            SendMessage(GetDlgItem(hDB,iddstLB), WM_SETREDRAW, FALSE, 0L);
            SendDlgItemMessage(hDB, idsrcLB, LB_SETSEL, FALSE, (long)(-1));
  
            lpBuf->fontCount = 0;
  
            if (!hdstSFlb)
            {
                SendDlgItemMessage(hDB, iddstLB, LB_RESETCONTENT, 0, 0L);
            }
            else
            {
                LPSFLB lpdstSFlb;
  
                if (lpdstSFlb=(LPSFLB)GlobalLock(hdstSFlb))
                {
                    lpBuf->fontCount = lpdstSFlb->free;
  
                    if (lpdstSFlb->free == 0)
                    {
                        SendDlgItemMessage(hDB,
                        iddstLB, LB_RESETCONTENT, 0, 0L);
                    }
                    GlobalUnlock(hdstSFlb);
                }
            }
  
            ///EnableWindow(GetDlgItem(hDB, idsrcLB), FALSE);
            ///EnableWindow(GetDlgItem(hDB, iddstLB), FALSE);
  
            SendMessage(GetDlgItem(hDB,idsrcLB), WM_VSCROLL, SB_TOP, 0L);
            SendMessage(GetDlgItem(hDB,iddstLB), WM_VSCROLL, SB_TOP, 0L);
            ///SendMessage(GetDlgItem(hDB,idsrcLB), WM_SETREDRAW, TRUE, 0L);
            ///SendMessage(GetDlgItem(hDB,iddstLB), WM_SETREDRAW, TRUE, 0L);
            ///InvalidateRect(GetDlgItem(hDB,idsrcLB), (LPRECT)0L, FALSE);
            ///InvalidateRect(GetDlgItem(hDB,iddstLB), (LPRECT)0L, FALSE);
  
            SetDlgItemText(hDB, SF_STATUS, (LPSTR)"");
            EnableWindow(GetDlgItem(hDB, SF_MOVE), FALSE);
            EnableWindow(GetDlgItem(hDB, SF_COPY), FALSE);
            EnableWindow(GetDlgItem(hDB, SF_ERASE), FALSE);
            EnableWindow(GetDlgItem(hDB, SF_EDIT), FALSE);
            EnableWindow(GetDlgItem(hDB, SF_ADD_RIGHT), FALSE);
  
            /*  Change exit button to Cancel.
            */
  
            if (LoadString(hMd,SF_CNCLSTR,lpBuf->buf,sizeof(lpBuf->buf)))
            {
                SetDlgItemText(hDB, SF_EXIT, lpBuf->buf);
                gSF_FLAGS |= SF_NOABORT;
            }
  
  
            gSF_FLAGS |= SF_NOABORT;
  
            //  Change cursor to an hour glass.
            //  and disable redrawing of listboxes.
            SetCursor(LoadCursor(NULL,IDC_WAIT));
            SendMessage(GetDlgItem(hDB,idsrcLB), WM_SETREDRAW, FALSE, 0L);
            SendMessage(GetDlgItem(hDB,iddstLB), WM_SETREDRAW, FALSE, 0L);
  
            /*  canReplace is -1 if the user has not been asked, 0
            *  if the user said do not replace existing files with
            *  the same name, and 1 if the user said do replace
            *  existing files with the same name.
            */
            lpBuf->canReplace = -1;
            lpBuf->duWrned = FALSE;
  
            lmemset((LPSTR)&lpBuf->edrec, 0, sizeof(EDREC));
            scrnFnts = FALSE;
            prevPos = 0;
            pos = 0;
  
            /*  Loop through, loading fonts by logical drive.
            */
            while (gSF_FLAGS & SF_NOABORT)
            {
                /*  Pick up the logical drive with the best priority.
                */
                for (sflb=&lpsrcSFlb->sflb[ind=0], best_priority=0xFFFF;
                    ind < lpsrcSFlb->free; ++ind, ++sflb)
                {
                    if ((sflb->state & SFLB_SEL) &&
                        (lpSFfile=
                        (LPSFDIRFILE)lockSFdirEntry(0L,sflb->indSFfile)))
                    {
                        if ((indLOGdrv=lpSFfile->indLOGdrv) > -1)
                        {
                            if (lpSFdrv=
                                (LPSFDIRLOGDRV)lockSFdirEntry(0L,indLOGdrv))
                            {
                                /*  Pick up the priority value of
                                *  this drive (based upon order in
                                *  which logical drive was read from
                                *  sfinstal.dir file).
                                */
                                priority = lpSFdrv->priority;
                                unlockSFdirEntry(indLOGdrv);
                            }
                            else
                            {
                                /*  Error: failed to lock logical drive.
                                */
                                priority = 0xFFFF;
                                sflb->state & ~(SFLB_SEL);
                            }
                        }
                        else
                            priority = 0;
  
                        if (priority < best_priority)
                        {
                            best_priority = priority;
                            curLOGdrv = indLOGdrv;
                        }
  
                        unlockSFdirEntry(sflb->indSFfile);
                    }
                    else
                    {
                        /*  Error: failed to lock SF dir entry.
                        */
                        sflb->state & ~(SFLB_SEL);
                    }
                }
  
                DBGaddfonts(("AddFonts(): best_priority=%d, curLOGdrv=%d\n",
                best_priority, curLOGdrv));
  
                /*  If we're done, then break out of the loop.
                */
                if (best_priority == 0xFFFF)
                    break;
  
                /*  For each listbox item whose logical drive matches
                *  the one we just found.
                */
                for (sflb=&lpsrcSFlb->sflb[ind=0];
                    ind < lpsrcSFlb->free && (gSF_FLAGS & SF_NOABORT); )
                {
                    /*  If selected, attempt to move to the destination
                    *  listbox.
                    */
                    if ((sflb->state & SFLB_SEL) &&
                        (lpSFfile=
                        (LPSFDIRFILE)lockSFdirEntry(0L,sflb->indSFfile)))
                    {
                        /*  Pick up the logical drive and determine
                        *  whether or not we'll have to build the
                        *  PFM file.
                        */
                        indLOGdrv = lpSFfile->indLOGdrv;
                        indScrnFnt = lpSFfile->indScrnFnt;
                        existDL = lpSFfile->offsDLname;
                        needPFM = !lpSFfile->offsPFMname;
                        fIsPCM = lpSFfile->fIsPCM;
  
                        unlockSFdirEntry(sflb->indSFfile);
  
                        /*  Look only at fonts with a matching
                        *  logical drive.
                        */
                        if (indLOGdrv != curLOGdrv)
                        {
                            ++ind;
                            ++sflb;
                            continue;
                        }
  
                        /*  Unselect the entry.
                        */
                        sflb->state &= ~(SFLB_SEL);
  
                        /*  Test to see if we've loaded the maximum
                        *  allowable number of fonts.
                        */
                        if (lpBuf->fontCount == MAX_SFLB_FONTS)
                        {
                            /*  Too many fonts, abort.
                            */
                            MaxFontAlert(hDB, hMd);
                            gSF_FLAGS &= ~(SF_NOABORT);
                            ++ind;
                            ++sflb;
                            continue;
                        }
  
                        /*  Scroll the listbox to the entry
                        *  about to be copied.
                        */
                        if (pos > ind)
                        {
                            for (; pos > ind; --pos)
                                SendMessage(
                                GetDlgItem(hDB,idsrcLB),
                                WM_VSCROLL,SB_LINEUP,0L);
                        }
                        else
                        {
                            if (pos < 7)
                                pos = 7;
  
                            for (; pos < ind; ++pos)
                                SendMessage(
                                GetDlgItem(hDB,idsrcLB),
                                WM_VSCROLL,SB_LINEDOWN,0L);
                        }
  
                        /*  Change the cursor back to an hour glass because
                        *  the dialog messages turned it into an arrow.
                        */
                        SetCursor(LoadCursor(NULL,IDC_WAIT));
  
                        /*  Copy matching screen font.
                        */
                        if (indScrnFnt > -1)
                        {
                            if (copyScrnFnt(hDB,hMd,lpBuf,indScrnFnt))
                            {
                                scrnFnts = TRUE;
                            }
                            else if (!(gSF_FLAGS & SF_NOABORT))
                            {
                                ++ind;
                                ++sflb;
                                continue;
                            }
                        }
  
                        dlrc = NOT_COPIED;
  
                        /*  Move the downloadable font file to the
                        *  destination directory (note: buildPFM()
                        *  relies upon this call to copyFile to fill
                        *  in lpBuf->dlFile).
                        */
                        if (existDL &&
                            !(dlrc=copyFile(hDB,hMd,lpBuf,lpBuf->path,
                            FALSE,sflb->indSFfile,indLOGdrv)))
                        {
                            ++ind;
                            ++sflb;
                            continue;
                        }
  
                        /*  Copy PFM file.
                        */
                        if (needPFM)
                        {
                            DBGx(("AddFonts(): Build PFM file\n"));
  
                            if ((!existDL) && (!fIsPCM))  /* FAIS file */
                            {
                                /*  Append the SF dir entry to the destination
                                *  listbox struct.
                                */
  
                                /*  Install the FAIS or TYP files.
                                */
  
                                DBGcopy(("Its an FAIS file\n"));
  
                                if (lpVF = (LPSTR) GlobalLock(lpFontLibrary->hFiles))
                                {
  
                                    for (NumFiles = lpFontLibrary->NumFiles; NumFiles > 0; --NumFiles)
                                    {
                                        lpLib = (LPSFI_FONTLIBENTRY)lpVF;
  
                                        DBGcopy(("Numfiles in sfadd2 = %d,  ", NumFiles));
                                        DBGcopy(("usage = %d\n", lpLib->usage));
                                        DBGcopy(("lpLib->OffsetName = %ls.\n", (LPSTR)lpLib+lpLib->OffsetName));
                                        DBGcopy(("lpSFfile->s = %ls.\n", (LPSTR)lpSFfile->s));
                                        if ((lstrcmp((LPSTR)lpLib+lpLib->OffsetName, (LPSTR)lpSFfile->s) == 0)
                                            && lpLib->usage)
                                        {
                                            DBGcopy(("Inside the string comp result\n"));
                                            lpLib->ListboxEntry = (WORD)ind;
                                            lpLib->Selected = TRUE;
  
                                            /* Call WriteSelections later */
                                            *JustDoIt = TRUE;
  
                                            break;
                                        }
  
                                        lpVF+=lpLib->Length;
                                    }
                                    GlobalUnlock(lpFontLibrary->hFiles);
                                }
                                else
                                    DBGcopy(("Global Lock failed\n"));
  
                                ++ind;
                                ++sflb;
                                continue;
                            }
  
                            needPFM = TFMtoPFM(hDB,lpBuf,lpSFfile,fIsPCM,PCLV);
  
                            /*  Build the PFM file.
                            *
                            *  This call to buildPFM relies upon the first
                            *  call to copyFile to fill in lpBuf->dlFile.
                            */
                            if (needPFM && !buildPFM(hDB,hMd,lpBuf,sflb->indSFfile))
                            {
                                /*  Failed to copy PFM file, so remove
                                *  the copied DL file (if we created a
                                *  new file).
                                */
                                if (dlrc == NEWFILE_COPIED)
                                {
                                    removeCopiedFile(lpBuf->path,
                                    sflb->indSFfile,FALSE,lpBuf->buf,
                                    sizeof(lpBuf->buf));
                                }
                                ++ind;
                                ++sflb;
                                continue;
                            }
                        }
                        else
                        {
                            DBGx(("AddFonts(): Copy PFM file\n"));
                            /*  PFM file exists, just move it to the
                            *  destination directory.
                            */
                            if (!copyFile(hDB,hMd,lpBuf,lpBuf->path,TRUE,
                                sflb->indSFfile,indLOGdrv))
                            {
                                /*  Failed to copy PFM file, so remove
                                *  the copied DL file (if we created a
                                *  new file).
                                */
                                if (existDL && (dlrc = NEWFILE_COPIED))
                                {
                                    removeCopiedFile(lpBuf->path,
                                    sflb->indSFfile,FALSE,lpBuf->buf,
                                    sizeof(lpBuf->buf));
                                }
                                ++ind;
                                ++sflb;
                                continue;
                            }
                        }
  
  
                        /*  Check to see if entry already exists in the
                        *  destination listbox.
                        */
                        if ((i=dupSFlistbox(hdstSFlb, sflb->indSFfile,
                            lpBuf->buf, sizeof(lpBuf->buf))) > -1)
                        {
                            /*  Entry does exist in the destination listbox,
                            *  i contains the ind to the sflb entry.
                            *  Check to see if it is okay to replace it.
                            */
                            if (!CanReplace(hDB,hMd,&lpBuf->canReplace,
                                lpBuf->buf,sizeof(lpBuf->buf)))
                            {
                                /*  Abort if we're not supposed to replace
                                *  fonts by the same name.
                                */
                                ++ind;
                                ++sflb;
                                continue;
                            }
  
                            /*  Replace the existing listbox entry.
                            */
                            hdstSFlb = replaceSFlistbox(hDB, hdstSFlb,
                            iddstLB, i, &id, sflb->indSFfile,
                            (fIsPCM ? SFLB_PERM|SFLB_CART :
                            (existDL ? 0 : SFLB_PERM)),
                            lpBuf->buf, sizeof(lpBuf->buf));
                        }
                        else
                        {
                            /* get an all-new id */
                            if (fIsPCM)
                                id=-getUniqueID(hdstSFlb)-100;
                            else
                                id=getUniqueID(hdstSFlb);
  
                            /*  Append the SF dir entry to the destination
                            *  listbox struct.
                            */
                            hdstSFlb = addSFlistbox(hDB, hdstSFlb, iddstLB,
                            id, sflb->indSFfile,
                            (fIsPCM ? SFLB_PERM|SFLB_CART :
                            (existDL ? 0 : SFLB_PERM)),
                            lpBuf->buf, sizeof(lpBuf->buf), &prevPos);
                        }
  
                        /*  Both files successfully copied, change
                        *  the path names.
                        */
                        chngSFdirPath(0L,sflb->indSFfile,TRUE,lpBuf->path);
                        if (existDL)
                            chngSFdirPath(0L,sflb->indSFfile,FALSE,lpBuf->path);
  
                        /*  Remove the entry from the source listbox.
                        */
  
                        SendDlgItemMessage(hDB,
                        idsrcLB,LB_DELETESTRING,(WORD)ind,0L);
  
                        /*  Write the win.ini file entry.  Note that
                        *  this destroys the strings for the download
                        *  and PFM file names (it is okay to blindly
                        *  do lstrcat because the download file follows
                        *  the PFM file in lpBuf).
                        */
                        if (existDL)
                        {
                            lstrcat(lpBuf->pfmFile, (LPSTR)",");
                            lstrcat(lpBuf->pfmFile, lpBuf->dlFile);
                        }
                        if (fIsPCM)
                            lstrcpy(lpBuf->buf, lpBuf->cartstr);
                        else
                            lstrcpy(lpBuf->buf, lpBuf->sfstr);
  
                        /* append absolute value of ID */
                        itoa((id<0?-id:id), &lpBuf->buf[lstrlen(lpBuf->buf)]);
                        WriteProfileString(lpAppNm,lpBuf->buf,lpBuf->pfmFile);
  
                        /*  Increment count of added fonts.
                        */
                        ++(*lpCount);
                        ++lpBuf->fontCount;
  
                        /*  Shuffle the contents of the SFLB struct
                        *  back one item.
                        */
                        for (j=&sflb[0], k=&sflb[1], i=ind+1;
                            i < lpsrcSFlb->free; ++i, ++j, ++k)
                        {
                            *j = *k;
                        }
                        --lpsrcSFlb->free;
                    }
                    else
                    {
                        sflb->state &= ~(SFLB_SEL);
                        ++ind;
                        ++sflb;
                    }
  
                    /*  Process any messages to the installer's dialog box
                    *  so we can detect the cancel button.
                    */
                    while (PeekMessage(&msg, hDB, NULL, NULL, TRUE) &&
                        IsDialogMessage(hDB, &msg))
                        ;
                }
            }
  
            if (lpBuf->edrec.hEDwnd)
            {
                DestroyWindow(lpBuf->edrec.hEDwnd);
                lpBuf->edrec.hEDwnd = 0;
            }
  
            if (!(*JustDoIt))
            {
                /*  Enable listbox.
                */
                //SendMessage(GetDlgItem(hDB,idsrcLB), WM_SETREDRAW, FALSE, 0L);
                //SendMessage(GetDlgItem(hDB,iddstLB), WM_SETREDRAW, FALSE, 0L);
                EnableWindow(GetDlgItem(hDB,idsrcLB), TRUE);
                EnableWindow(GetDlgItem(hDB,iddstLB), TRUE);
                SendMessage(GetDlgItem(hDB,idsrcLB), WM_VSCROLL, SB_TOP, 0L);
                SendMessage(GetDlgItem(hDB,iddstLB), WM_VSCROLL, SB_TOP, 0L);
                SendMessage(GetDlgItem(hDB,idsrcLB), WM_SETREDRAW, TRUE, 0L);
                SendMessage(GetDlgItem(hDB,iddstLB), WM_SETREDRAW, TRUE, 0L);
                InvalidateRect(GetDlgItem(hDB,idsrcLB), (LPRECT)0L, TRUE);
                InvalidateRect(GetDlgItem(hDB,iddstLB), (LPRECT)0L, TRUE);
  
                EnableWindow(GetDlgItem(hDB, idsrcLB), TRUE);
            }
  
            /*  Restore exit button.
            */
            if (LoadString(hMd,SF_EXITSTR,lpBuf->buf,sizeof(lpBuf->buf)))
            {
                SetDlgItemText(hDB, SF_EXIT, lpBuf->buf);
                gSF_FLAGS &= ~(SF_NOABORT);
            }
  
            //  Restore arrow cursor.
            InvalidateRect(GetDlgItem(hDB,iddstLB), (LPRECT)0L, FALSE);
            SetCursor(LoadCursor(NULL,IDC_ARROW));
  
            SetDlgItemText(hDB, SF_STATUS, (LPSTR)"");
            EnableWindow(GetDlgItem(hDB, SF_ADD_RIGHT), TRUE);
  
            /*  Flag new fontSummary in win.ini file.
            */
            NewFS(hMd, lpAppNm);
  
            /*  If matching screen fonts were loaded up, then
            *  put up an alert describing this.  Broadcast a
            *  message to all other apps that the [fonts]
            *  section of the win.ini file has changed.
            */
            if (scrnFnts)
            {
                FARPROC lpDlgFunc;
  
                MyDialogBox(hMd,SFSCRNALERT, hDB, GenericWndProc);
  
                // Some apps expect one of these messages, others the other.
                // We'll send both.
                SendMessage(0xFFFF, WM_WININICHANGE, 0, (LONG)(LPSTR)"fonts");
                // 18 sep 89: send this also.
                SendMessage(0xFFFF, WM_FONTCHANGE, 0, 0L);
            }
        }
        else
        {
            /*  Remove highlight from source listbox if
            *  cancel from GetTargDir().
            */
            SendDlgItemMessage(hDB, idsrcLB, LB_SETSEL, FALSE, (long)(-1));
        }
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
  
    if (lpsrcSFlb)
    {
        GlobalUnlock(hsrcSFlb);
        lpsrcSFlb = 0L;
    }
  
    DBGx(("AddFonts(): return hdstSFlb = %d\n", hdstSFlb));
    return (hdstSFlb);
  
}   // AddFonts()
  
/**************************************************************************/
/*****************************   Local Procs   ****************************/
  
  
LOCAL BOOL TFMtoPFM(hDB, lpBuf, lpSFfile, fIsPCM, PCLV)
HWND hDB;
LPADDREC lpBuf;
LPSFDIRFILE lpSFfile;
BOOL fIsPCM;
BOOL PCLV;
{
    LPGLUEINFO G=0L;        /* Info from glue file */
    LPSYMINFO lpSymSet=0L;  /* Symbol set info from cart info */
    LPPCMHEADER lpPCM;      /* PCM header info */
    HANDLE  hP;     /* handle to PCM memory */
    HANDLE hC;          /* Handle to cartridge info */
    LPGCINFO C;     /* cartridge info */
    BYTE tfmin[MAX_FILEPATH+1];/* tfm file name */
    BYTE req_ss[4];     /* requested symset for PFM */
    BYTE lb_name[75];       /* name in listbox */
    LPSTR s;            /* index into string */
    int  cfnum;     /* # of fonts in cartridge */
    int i, l, m, n;
    int ptindex;        /* index into glue file point size array */
    int pfmfile;        /* Handle to PFM/PCM file */
    LONG PCMsize;       /* Size of PCM file */
    BOOL needPFM=TRUE;      /* True if we still need a PFM */
    HANDLE hT;          /* Handle to TFM */
  
    if ((gHG) && (G = (LPGLUEINFO)GlobalLock(gHG)) &&
        (hT = GlobalAlloc(GHND, (DWORD)sizeof(struct TFMType))))
    {
        DBGx(("AddFonts(): Just locked glueinfo\n"));
  
        for (i=0; i<gTotal; i++)
        {
            lstrcpy((LPSTR)lb_name, (LPSTR)G[i].giName);
  
            if (G[i].giType == 1)
            {
                lstrcat((LPSTR)lb_name, (LPSTR)G[i].giWeight);
                lstrcat((LPSTR)lb_name, (LPSTR)" (WN)");
                lstrcat((LPSTR)lb_name, (LPSTR)G[i].giSlant);
  
                s = G[i].giFFile + lstrlen(G[i].giFFile);
                while ((s>G[i].giFFile) && (s[-1]!=':') && (s[-1]!='\\') &&
                    (s[-1]!=' '))
                    s--;
            }
  
            /* if this glue entry matches selection */
            if ((lstrcmp((LPSTR)lb_name, (LPSTR)lpSFfile->s) == 0) &&
                ((G[i].giType != 1) ||
                (lstrcmp(s, (LPSTR)&lpSFfile->s[lpSFfile->offsDLname]) == 0)))
            {
                DBGx(("AddFonts(): Match: ind=%d\n", i));
  
//                (LPSTR)"Installing: AutoFont Support metrics");
                if (LoadString(hLibInst,SF_INSTALLAF,
                                lpBuf->buf,sizeof(lpBuf->buf)) )                         // rk 8/20/91
                      SetDlgItemText(hDB, SF_STATUS, lpBuf->buf);
  
                if (fIsPCM)
                {
                    DBGx(("AddFonts(): fIsPCM\n"));
  
                    hC = get_cart_info(gGluein, G[i].giName, (LPINT)&cfnum);
  
                    DBGx(("AddFonts(): cfnum=%d\n", cfnum));
  
                    lstrcpy(lpBuf->pfmFile, (LPSTR)"CRT");
  
                    /* Add crt #, up to 5 digits */
                    G[i].giFFile[5] = '\0';
                    lstrcat(lpBuf->pfmFile, G[i].giFFile);
                    lstrcat(lpBuf->pfmFile, (LPSTR)".PCM");
  
                    MergePath(lpBuf->path, lpBuf->pfmFile,
                    sizeof(lpBuf->pfmFile), TRUE);
  
                    DBGx(("AddFonts(): lpBuf->pfmFile: %ls\n", lpBuf->pfmFile));
                    DBGx(("AddFonts(): lpBuf->path: %ls\n", lpBuf->path));
  
                    if ((hC) && (C = (LPGCINFO)GlobalLock(hC)))
                    {
                        DBGx(("AddFonts(): Locked C\n"));
  
                        /* creat & open PCM file */
                        pfmfile=_lcreat(lpBuf->pfmFile,0);
  
                        /* alloc mem for PCM header */
                        if ((hP = GlobalAlloc(GMEM_MOVEABLE,
                            (DWORD)sizeof(PCMHEADER))) &&
                            (lpPCM = (LPPCMHEADER)GlobalLock(hP)))
                        {
  
                            DBGx(("AddFonts(): Opened PCM file and alloced mem\n"));
  
                            /* Fill out PCM header and write to file */
                            lpPCM->pcmMagic = PCM_MAGIC;
                            lpPCM->pcmVersion = PCM_VERSION;
                            lpPCM->pcmSize = 0L;    /* fill in later */
                            lpPCM->pcmTitle = (DWORD)sizeof(PCMHEADER);
                            lpPCM->pcmPFMList = (DWORD)(lpPCM->pcmTitle + lstrlen(G[i].giName) + 1);
  
                            _lwrite(pfmfile, (LPSTR)lpPCM, sizeof(PCMHEADER));
                            _lwrite(pfmfile, (LPSTR)G[i].giName, lstrlen(G[i].giName) + 1);
  
                            /* Free PCM header */
                            GlobalUnlock(hP);
                            GlobalFree(hP);
  
                            for (l = 0; l < cfnum; l++)
                            {
                                /* write a pfm for the windows char set
                                */
                                lstrcpy((LPSTR)tfmin, C[l].gcTfm);
  
                                DBGx(("AddFonts(): tfmin: %ls\n", (LPSTR)tfmin));
  
//                                lstrcpy(lpBuf->buf, (LPSTR)"Reading: ");
                                lpBuf->buf[0] = '\0';
                                LoadString(hLibInst,SF_READING,
                                           lpBuf->buf,sizeof(lpBuf->buf));               // rk 8/20/91
                                lstrcat(lpBuf->buf, (LPSTR)tfmin);
                                SetDlgItemText(hDB, SF_STATUS, lpBuf->buf);
  
                                tfmread(hT, (LPSTR)tfmin);
  
                                DBGx(("AddFonts(): Read TFM, now write PFM (in PCM)\n"));
  
//                                lstrcpy(lpBuf->buf, (LPSTR)"Writing: ");
                                
                                lpBuf->buf[0] = '\0';
                                LoadString(hLibInst,SF_WRITING,
                                           lpBuf->buf,sizeof(lpBuf->buf));               // rk 8/20/91
                                lstrcat(lpBuf->buf, (LPSTR)lpBuf->pfmFile);
                                SetDlgItemText(hDB, SF_STATUS, lpBuf->buf);
  
  
                                if (C[l].gcClass == 'O')    /* Scalable */
                                {
                                    lstrcpy((LPSTR)req_ss, (LPSTR)"9U");
  
                                    wt_pfm(hT, pfmfile, 0L, NULL,
                                    (LPSTR)req_ss, C[l].gcOrient, PCLV);
  
                                    DBGx(("AddFonts(): Wrote PFM (in PCM)\n"));
                                }
                                else            // need to make bitmapped pfms for carts
                                {
                                    if ((C[l].gchSym) && (lpSymSet =
                                        (SYMINFO far *)GlobalLock(C[l].gchSym)))
                                    {
  
                                        for(m = 0; m < (C[l].gcSSIndex); m++)
                                        {
                                            ptindex = 0;
  
                                            for (n = 0; n < (C[l].gcNumSizes); n++)
                                            {
                                                /* Skip spaces */
                                                while (isspace(C[l].gcPtSizes[ptindex]))
                                                    ptindex++;
  
                                                lstrcpy((LPSTR)req_ss,
                                                (LPSTR)lpSymSet[m].ssPCLcode);
  
                                                wt_pfm(hT, pfmfile, 0L,
                                                (LPSTR)&C[l].gcPtSizes[ptindex],
                                                (LPSTR)req_ss, C[l].gcOrient,
                                                PCLV);
  
                                                /* Skip pt size text */
                                                while ((C[l].gcPtSizes[ptindex]) &&
                                                    (!isspace(C[l].gcPtSizes[ptindex])))
                                                    ptindex++;
                                            }
                                        }
  
                                        GlobalUnlock(C[l].gchSym);
  
                                        DBGx(("AddFonts(): Wrote PFMs (in PCM)\n"));
                                    }
                                }
  
                                if (C[l].gchSym)
                                    GlobalFree(C[l].gchSym);
  
                            } /* for l */
  
                            PCMsize = _llseek(pfmfile, 0L, 2);
                            _llseek(pfmfile, 4L, 0);
                            _lwrite(pfmfile, (LPSTR)&PCMsize, 4);
                            _llseek(pfmfile, 0L, 2);
  
                        }   /* if alloc PCM ... */
  
                        GlobalUnlock(hC);
                        GlobalFree(hC);
                    }   /* if lock hC */
                }
                else    /* Not a cartridge */
                {
                    DBGx(("AddFonts(): !fIsPCM\n"));
  
                    /* Copy PCLEO file name (no path) */
                    s = G[i].giFFile + lstrlen(G[i].giFFile);
                    while ((s>G[i].giFFile) && (s[-1]!=':') && (s[-1]!='\\') &&
                        (s[-1]!=' '))
                        s--;
  
                    lstrcpy(lpBuf->pfmFile, s);
                    s = lpBuf->pfmFile + lstrlen(lpBuf->pfmFile);
                    *(--s) = 'M';
                    *(--s) = 'F';
                    *(--s) = 'P';
  
                    MergePath(lpBuf->path,lpBuf->pfmFile,
                    sizeof(lpBuf->pfmFile), TRUE);
  
                    /* creat & open PFM file */
                    pfmfile=_lcreat(lpBuf->pfmFile,0);
  
  
                    lstrcpy((LPSTR)tfmin, G[i].giTfm);
  
//                    lstrcpy(lpBuf->buf, (LPSTR)"Reading: ");
                    lpBuf->buf[0] = '\0';
                    LoadString(hLibInst,SF_READING,
                                  lpBuf->buf,sizeof(lpBuf->buf));               // rk 8/20/91
                    lstrcat(lpBuf->buf, (LPSTR)tfmin);
                    SetDlgItemText(hDB, SF_STATUS, lpBuf->buf);
  
                    tfmread(hT, (LPSTR)tfmin);
  
                    /* Add stroke weight to name now */
                    //          if (lstrcmp(G[i].giWeight, (LPSTR)" Bd"))
                    //          lstrcat(G[i].giName, G[i].giWeight);
  
//                    lstrcpy(lpBuf->buf, (LPSTR)"Writing: ");
                    lpBuf->buf[0] = '\0';
                    LoadString(hLibInst,SF_WRITING,
                                           lpBuf->buf,sizeof(lpBuf->buf));               // rk 8/20/91
                    lstrcat(lpBuf->buf, lpBuf->pfmFile);
                    SetDlgItemText(hDB, SF_STATUS, lpBuf->buf);
  
                    lstrcpy((LPSTR)req_ss, (LPSTR)"9U");
  
                    wt_pfm(hT, pfmfile, G[i].giMemusage,
                    NULL, (LPSTR)req_ss, 0, PCLV);
                }
  
                _lclose(pfmfile);
  
                /* Skip the DLtoPFM routine later */
                needPFM = FALSE;
  
                break;  /* We're done, bud */
            }   /* if ind == G[i].giInd */
        }       /* for */
  
        SetDlgItemText(hDB, SF_STATUS, (LPSTR)"");
  
        /* Free up TFM memory */
        FreeTFM(hT);
        GlobalFree(hT);
  
        GlobalUnlock(gHG);
    }
  
    return(needPFM);
}
  
  
/*  buildPFM
*/
LOCAL BOOL buildPFM(hDB, hMd, lpBuf, indSFfile)
HWND hDB;
HANDLE hMd;
LPADDREC lpBuf;
int indSFfile;
{
    LPEXTTEXTMETRIC lpExtText = (LPEXTTEXTMETRIC)&lpBuf->extText;
    LPPFMHEADER lpPFMhead = (LPPFMHEADER)&lpBuf->pfmHead;
    LPPFMEXTENSION lpPFMext = (LPPFMEXTENSION)&lpBuf->pfmExt;
    WORD FAR *lpWidths = lpBuf->widths;
    LPDRIVERINFO lpDrvInfo = (LPDRIVERINFO)&lpBuf->drvInfo;
    LPSTR lpFace = lpBuf->face;
    LPSTR lpDevice = lpBuf->device;
    BOOL success = FALSE;
    int hsrcFile;
    int hdstFile;
  
    DBGx(("buildPFM(): lzOpenFile(%ls)", (LPSTR) (lpBuf->dlFile) ));
    if ((hsrcFile=lzOpenFile(lpBuf->dlFile,&lpBuf->ofstruct,OF_READ)) != -1)
    {
        // DBGx(("buildPFM(): opened, handle = %d\n", hsrcFile));
  
        if (DLtoPFM(hsrcFile, hMd, FALSE, lpPFMhead, lpWidths,
            lpExtText, lpDrvInfo,
            lpFace, sizeof(lpBuf->face), lpBuf->buf, sizeof(lpBuf->buf)) &&
            (hdstFile=openUniqPFM(lpBuf,indSFfile)) > 0)
        {
            // DBGx(("buildPFM(): Return TRUE from DLtoPFM()\n"));
            /*  Update status dialog.
            */
            lstrcpy(lpBuf->buf, lpBuf->bldpfm);
            lstrcat(lpBuf->buf, lpBuf->pfmFile);
            // DBGx(("buildPFM(): lstrcpy(), lstrcat() OK.\n"));
            SetDlgItemText(hDB, SF_STATUS, lpBuf->buf);
            // DBGx(("buildPFM(): SetDlgItemText() OK.\n"));
  
            if (lpBuf->face[0] == '\0')
            {
                /*  No face name, prompt the user for one.
                */
                LPSTR s;
  
        #ifdef KEEPITUP
                /*  Leave the dialog up there -- if we encountered one
                *  bad font, they're probably all bad.
                */
                lpBuf->edrec.showNext = TRUE;
        #endif
  
                makeDesc((LPSTR)lpPFMhead, lpBuf->edrec.desc,
                sizeof(lpBuf->edrec.desc), lpBuf->point,
                lpBuf->bold, lpBuf->italic);
  
                /*  Pick up the name of the downloadable font file
                *  to display in the edit dialog.
                */
                lpBuf->edrec.dlname[0] = '\0';
                for (s=lpBuf->dlFile+lstrlen(lpBuf->dlFile);
                    s > lpBuf->dlFile && s[-1] != '\\' && s[-1] != ':'; --s)
                    ;
                lmemcpy(lpBuf->edrec.dlname, s, 13);
                lpBuf->edrec.dlname[12] = '\0';
  
        #ifdef KEEPITUP
                /*  If the edit dialog is already up, then just
                *  give it focus.
                */
                if (lpBuf->edrec.hEDwnd)
                {
                    SetFocus(lpBuf->edrec.hEDwnd);
                }
        #endif
  
                if (lpBuf->edrec.global || editPFM(hDB, hMd, &lpBuf->edrec))
                {
                    int k;
  
                    /*  Copy new name to the name that will be written
                    *  to the PFM file.
                    */
                    lmemcpy(lpFace,lpBuf->edrec.name,sizeof(lpBuf->face));
                    lpFace[sizeof(lpBuf->face)-1] = '\0';
  
                    lpPFMhead->dfPitchAndFamily |= (lpBuf->edrec.family & 0xF0);
  
                    /*  Append the description (like "12pt italic")
                    *  to the name and change the SF dir entry.
                    */
                    k = lstrlen(lpBuf->edrec.name);
                    lmemcpy(&lpBuf->edrec.name[k],
                    lpBuf->edrec.desc,sizeof(lpBuf->edrec.name)-k);
                    lpBuf->edrec.name[sizeof(lpBuf->edrec.name)-1] = '\0';
  
                    /*  Change the name in the SF dir.
                    */
                    chngSFdirDesc(0L,indSFfile,
                    lpBuf->edrec.name,lpBuf->buf,sizeof(lpBuf->buf));
  
                    /*  Restore to just the name.
                    */
                    lpBuf->edrec.name[k] = '\0';
  
            #ifdef KEEPITUP
                    if (!lpBuf->edrec.global)
                    {
                        /*  Clear the dialog and gray it, but don't pull it
                        *  down unless we encounter a font that does not
                        *  require editing.
                        */
                        SetDlgItemText(lpBuf->edrec.hEDwnd,SFED_FILE,(LPSTR)"");
                        SetDlgItemText(lpBuf->edrec.hEDwnd,SFED_DESC,(LPSTR)"");
                        SetDlgItemText(lpBuf->edrec.hEDwnd,SFED_NAME,(LPSTR)"");
                        CheckRadioButton(hDB,SFED_ROMAN,SFED_DONTCARE,0);
                        SetFocus(hDB);
                    }
            #endif
                }
                else
                {
            #ifdef KEEPITUP
                    /*  The user clicked on the cancel button.  Pull
                    *  down the dialog and make sure the face is null.
                    */
                    DestroyWindow(lpBuf->edrec.hEDwnd);
                    lpBuf->edrec.hEDwnd = 0;
                    lpBuf->face[0] = '\0';
            #endif
  
                    /*  The user pressed the cancel button.
                    */
                    if (lpBuf->edrec.stop)
                        gSF_FLAGS &= ~(SF_NOABORT);
                }
            }
        #ifdef KEEPITUP
            else if (lpBuf->edrec.hEDwnd)
            {
                /*  We encountered a font which does not require
                *  editing, but the edit dialog is still up, so
                *  pull it down.
                */
                DestroyWindow(lpBuf->edrec.hEDwnd);
                lpBuf->edrec.hEDwnd = 0;
            }
        #endif
  
            if (lpBuf->face[0] != '\0' &&
                writePFM(hdstFile, lpPFMhead, lpWidths, lpPFMext, lpExtText,
                lpDrvInfo, lpFace, lpDevice) == lpPFMhead->dfSize)
            {
                _lclose(hdstFile);
  
                /*  If this is a duplicate PFM file, then erase
                *  it and use the duplicate that already exists.
                *  The name will be changed to the duplicate
                *  PFM file.
                */
                useDupPFM(lpBuf->pfmFile,lpBuf->buf,sizeof(lpBuf->buf));
  
                /*  Add the PFM file to existing SFDIRFILE struct.
                */
                lstrcpy(lpBuf->buf, lpBuf->pfmFile);
  
                if (addSFdirFile(0L,indSFfile,TRUE,
                    lpBuf->buf,sizeof(lpBuf->buf)))
                {
                    success = TRUE;
                }
            }
            else
                _lclose(hdstFile);
  
            if (!success)
            {
                /*  File not successfully made, blow away the
                *  created PFM file.
                */
                DBGx(("buildPFM(): delete .PFM file\n"));
                OpenFile(lpBuf->pfmFile,&lpBuf->ofstruct,OF_DELETE);
            }
        }
  
        DBGx(("buildPFM(): lzClose(%d)", hsrcFile));
        lzClose(hsrcFile);
        DBGx((".. closed.\n"));
    }
    #ifdef DEBUG
    else
    {
        DBMSG(("buildPFM(): could not open %ls\n", lpBuf->dlFile));
    }
    #endif
  
    if (!success)
    {
        int k;
  
        if (LoadString(hMd,SFADD_NOADD,lpBuf->buf,sizeof(lpBuf->buf)) &&
            (k=lstrlen(lpBuf->buf)+1) &&
            LoadString(hMd,SFADD_NOGENPFM,&lpBuf->buf[k],sizeof(lpBuf->buf)-k))
        {
            if (MessageBox(hDB,&lpBuf->buf[k],lpBuf->buf,
                MB_OKCANCEL | MB_ICONEXCLAMATION) == IDCANCEL)
            {
                gSF_FLAGS &= ~(SF_NOABORT);
            }
        }
    }
  
    #ifdef DEBUG
    if (success)
        DBGx(("BuildPFM() .. success.\n"));
    else
        DBGx(("BuildPFM() .. failed.\n"));
    #endif
  
    return (success);
}   // BuildPfm()
  
/*  openUniqPFM
*/
LOCAL int openUniqPFM(lpBuf, indSFfile)
LPADDREC lpBuf;
int indSFfile;
{
    LPEXTTEXTMETRIC lpExtText = (LPEXTTEXTMETRIC)&lpBuf->extText;
    LPPFMHEADER lpPFMhead = (LPPFMHEADER)&lpBuf->pfmHead;
    LPSFDIRFILE lpSFfile;
    LPSTR lpPFM = lpBuf->pfmFile;
    long scaled;
    int hFile;
    int i, k;
  
    lpPFM[0] = '\0';
  
    /*  Set first two characters (first two characters from DL file).
    */
    if (lpSFfile=(LPSFDIRFILE)lockSFdirEntry(0L,indSFfile))
    {
        lmemcpy(lpPFM, (LPSTR)&lpSFfile->s[lpSFfile->offsDLname], 2);
        lpPFM[2] = '\0';
        unlockSFdirEntry(indSFfile);
    }
  
    while ((k=lstrlen(lpPFM)) < 2)
    {
        lpPFM[k] = 'x';
        lpPFM[k+1] = '\0';
    }
  
    /*  Set third character (P, L, or X for portrait, landscape,
    *  or both orienation).
    */
    if (lpExtText->emOrientation == 1)
    {
        lpPFM[2] = 'P';
    }
    else if (lpExtText->emOrientation == 2)
    {
        lpPFM[2] = 'L';
    }
    else
    {
        lpPFM[2] = 'X';
    }
  
    /*  Set fourth character (B, R, I, E for bold, roman, italic
    *  or enhanced bold/italic).
    */
    if (lpPFMhead->dfItalic && lpPFMhead->dfWeight > FW_NORMAL)
    {
        lpPFM[3] = 'E';
    }
    else if (lpPFMhead->dfWeight > FW_NORMAL)
    {
        lpPFM[3] = 'B';
    }
    else if (lpPFMhead->dfItalic)
    {
        lpPFM[3] = 'I';
    }
    else
    {
        lpPFM[3] = 'R';
    }
  
  
    /*  Place point size in character postions 5 through 7.
    */
    scaled = lpPFMhead->dfPixHeight - lpPFMhead->dfInternalLeading;
    scaled = ldiv((lmul(scaled,(long)72)+36),(long)300);
  
    itoa((int)scaled, &lpPFM[4]);
  
    /*  Make sure number is flush right in positions 5 to 7.
    */
    if ((k=lstrlen(lpPFM)) < 7)
    {
        for (i=7; i > 3; --i, --k)
        {
            if (k > 3)
                lpPFM[i] = lpPFM[k];
            else
                lpPFM[i] = '0';
        }
    }
  
    lpPFM[7] = '\0';
    lstrcat(lpPFM, (LPSTR)"0.PFM");
  
    MergePath(lpBuf->path, lpPFM, sizeof(lpBuf->pfmFile), TRUE);
    k = lstrlen(lpPFM) - 5;
  
    /*  Fiddle with the 8th character until dos_opend() cannot
    *  find an existing file with the same name.
    */
    for (i=0, hFile=0; hFile != DOS_NOFILES && i < 36; ++i)
    {
        if (i < 10)
        {
            lpPFM[k] = (BYTE)'0' + (BYTE)i;
        }
        else
        {
            lpPFM[k] = (BYTE)'A' + (BYTE)i - (BYTE)10;
        }
  
        hFile = dos_opend(&lpBuf->dirdata,lpPFM,0x07);
    }
  
    if (i < 36 && hFile == DOS_NOFILES)
    {
        hFile = OpenFile(lpPFM,&lpBuf->ofstruct,OF_WRITE | OF_CREATE);
    }
    else
        hFile = -1;
  
    return (hFile);
}   // openUniqPFM()
  
/*  copyFile
*
*  Copy a file from the source path to the target path.  Note that one
*  caller, copyScrnFnt, puts the path name in the face field, so if
*  copyFile screws around with the face, it will break copyScrnFnt.
*/
LOCAL WORD copyFile(hDB, hMd, lpBuf, lpPath, isPFM, indSFfile, indLOGdrv)
HWND hDB;
HANDLE hMd;
LPADDREC lpBuf;
LPSTR lpPath;
BOOL isPFM;
int indSFfile;
int indLOGdrv;
{
    LPSFDIRLOGDRV lpSFdrv = 0L;
    BYTE copied = NOT_COPIED;
    int hsrcFile = -1;
    int hdstFile = -1;
    int drive, j, k, n;
  
    DBGcopy(("copyFile(%d,%d,%lp,%lp,%d,%d,%d): %ls\n",
    hDB, hMd, lpBuf, lpPath, (WORD)isPFM, indSFfile, indLOGdrv, lpPath));
  
    lpBuf->buf[0] = '\0';
  
    if (makeSFdirFileNm(0L,indSFfile,isPFM,lpBuf->buf,sizeof(lpBuf->buf)))
    {
        j = lstrlen(lpBuf->buf) + 1;
  
        DBGx(("copyFile(): Attempting to open source with lzOpenFile():\n"));
        while ( ((hsrcFile=
            lzOpenFile(lpBuf->buf,&lpBuf->ofstruct,OF_READ)) <= 0) &&
            !existLBL(hMd,indLOGdrv,&lpBuf->buf[j],sizeof(lpBuf->buf)-j) &&
            (drive=isABdrive(lpBuf->buf)) )
        {
            DBGx(("copyFile(): Could not open '%ls'\n", lpBuf->buf));
            DBGx(("copyFile(): handle from lzOpenFile()=%d\n", hsrcFile));
            DBGx(("copyFile(): isABdrive() returned %d\n", drive));
  
            /*  Build a "switch disks" message to display -- note that
            *  we preserve the file name in lpBuf->buf and build both
            *  caption and message in lpBuf->buf.
            */
            LoadString(hMd,SFADD_CHNGDSK,&lpBuf->buf[j],sizeof(lpBuf->buf)-j);
            k = j + lstrlen(&lpBuf->buf[j]) + 1;
            LoadString(hMd,SFADD_PROMPT1,&lpBuf->buf[k],sizeof(lpBuf->buf)-k);
            n = k + lstrlen(&lpBuf->buf[k]);
            lpBuf->buf[n] = '\0';
  
            if (indLOGdrv > -1 &&
                (lpSFdrv=(LPSFDIRLOGDRV)lockSFdirEntry(0L,indLOGdrv)))
            {
                /*  Get the prompt for the logical drive.
                */
                if (lpSFdrv->offsDesc)
                {
                    lmemcpy(&lpBuf->buf[n],
                    &lpSFdrv->s[lpSFdrv->offsDesc],sizeof(lpBuf->buf)-n);
                }
                else if (lpSFdrv->offsLabel)
                {
                    lmemcpy(&lpBuf->buf[n],
                    &lpSFdrv->s[lpSFdrv->offsLabel],sizeof(lpBuf->buf)-n);
                }
                unlockSFdirEntry(indLOGdrv);
            }
  
            if (lpBuf->buf[n] == '\0')
            {
                /*  Failed to get the prompt from the logical drive,
                *  so use the name of the file.
                */
                n += LoadString(hMd, SFADD_DISKWITH,
                &lpBuf->buf[n], sizeof(lpBuf->buf)-n);
                lmemcpy(&lpBuf->buf[n], lpBuf->buf, sizeof(lpBuf->buf)-n);
            }
  
            lpBuf->buf[sizeof(lpBuf->buf)-1] = '\0';
            n = k + lstrlen(&lpBuf->buf[k]);
            LoadString(hMd, ((drive==1) ? SFADD_PROMPT2 : SFADD_PROMPT3),
            &lpBuf->buf[n],sizeof(lpBuf->buf)-n);
  
            if (MessageBox(hDB,&lpBuf->buf[k],&lpBuf->buf[j],
                MB_OKCANCEL | MB_ICONEXCLAMATION) == IDCANCEL)
            {
                gSF_FLAGS &= ~(SF_NOABORT);
                return FALSE;
            }
        }
    }
  
    if (hsrcFile != -1)
    {
        /*  Source file successfully opened.  Now replace the path with
        *  that of the target directory.
        */
        j = lstrlen(lpBuf->buf) + 1;
        lmemcpy(&lpBuf->buf[j], lpBuf->buf, sizeof(lpBuf->buf)-j);
        lpBuf->buf[sizeof(lpBuf->buf)-1] = '\0';
  
        MergePath(lpPath,&lpBuf->buf[j],sizeof(lpBuf->buf)-j,TRUE);
  
        /*  Save the name for updating the win.ini file.
        */
        if (isPFM)
        {
            lmemcpy(lpBuf->pfmFile,&lpBuf->buf[j],sizeof(lpBuf->pfmFile));
            lpBuf->pfmFile[sizeof(lpBuf->pfmFile)-1] = '\0';
        }
        else
        {
            lmemcpy(lpBuf->dlFile,&lpBuf->buf[j],sizeof(lpBuf->dlFile));
            lpBuf->dlFile[sizeof(lpBuf->dlFile)-1] = '\0';
        }
  
        k = j + lstrlen(&lpBuf->buf[j]) + 1;
  
        /*  Check for enough disk space to copy the file.  If there
        *  is not enough room, put up an alert and return FALSE.
        */
        DBGcopy(("copyFile(): About to call lzSeek(), checkAlertDU()\n"));
        if (!checkAlertDU(hDB,hMd,lpPath,lpBuf->buf,lzSeek(hsrcFile,0L,2),
            &lpBuf->duWrned,&lpBuf->buf[k],sizeof(lpBuf->buf)-k))
        {
            DBGcopy(("copyFile(): (1) lzClose(%d)\n", hsrcFile));
            lzClose(hsrcFile);
            DBGcopy((".. closed.\n"));
            gSF_FLAGS &= ~(SF_NOABORT);
            return FALSE;
        }
        DBGcopy(("copyFile(): did not call lzClose()\n"));
  
        if (lstrcmpi(lpBuf->buf, &lpBuf->buf[j]) == 0)
        {
            /*  dst = src, so just say the file was successfully
            *  copied without doing anything else.
            */
            copied = SRC_EQ_DST;
        }
        else
        {
            /*  Shift the destination file name to the top of
            *  lpBuf->buf for simplicity -- we do not need the
            *  name of the source file any more.
            */
            lstrcpy(lpBuf->buf, &lpBuf->buf[j]);
  
            if (dos_opend(&lpBuf->dirdata, lpBuf->buf, 0x01) == 0)
            {
                /*  Destination file already exists, check to see
                *  if it is okay to copy over the file.
                */
                j = lstrlen(lpBuf->buf) + 1;
  
                if (!CanReplace(hDB,hMd,&lpBuf->canReplace,
                    &lpBuf->buf[j],sizeof(lpBuf->buf)-j))
                {
                    /*  Abort if we're not supposed to replace fonts
                    *  by the same name.
                    */
                    DBGx(("copyFile(): (2) lzClose(%d)", hsrcFile));
                    lzClose(hsrcFile);
                    DBGx(("... closed.\n"));
                    return FALSE;
                }
            }
  
//            if ((hdstFile=OpenFile(lpBuf->buf,&lpBuf->ofstruct,
//                OF_CREATE | OF_WRITE)) > 0)
            if ((hdstFile=OpenFile(lpBuf->buf,&lpBuf->ofstruct,
                OF_CREATE | OF_WRITE)) >= 0)                                             // RK 04/09/91
            {
                /*  Destination file opened.  Update status line.
                */
                lstrcpy(lpBuf->buf, lpBuf->adding);
                lstrcat(lpBuf->buf, (isPFM ? lpBuf->pfmFile : lpBuf->dlFile));
                SetDlgItemText(hDB, SF_STATUS, lpBuf->buf);
  
                DBGx(("copyFile(): (2) lzSeek(%d, 0L, 0)", hsrcFile));
                lzSeek(hsrcFile, 0L, 0);
                DBGx((".. seek OK.\n"));
  
                do {
                    /*  Copy a block at a time to the new file.
                    */
                    //DBGx(( "copyFile(): lzRead(), _lwrite() in copy loop"));
                    if ((k=lzRead(hsrcFile,lpBuf->buf,sizeof(lpBuf->buf))) > 0)
                    {
                        _lwrite(hdstFile,lpBuf->buf,k);
                    }
  
                    DBGx((".. ok\n"));
  
                } while (k == sizeof(lpBuf->buf));
  
                /*  Successfully copied file.
                */
                copied = NEWFILE_COPIED;
  
                _lclose(hdstFile);
            }
            else
            {
                /*  We failed to open the destination (copy to) file.
                *  Report an err.  Note that both the caption and err
                *  message are built in lpBuf->buf.
                */
                j = lstrlen(lpBuf->buf) + 1;
                LoadString(
                hMd,SFADD_NOCOPY,&lpBuf->buf[j],sizeof(lpBuf->buf)-j);
                k = j + lstrlen(&lpBuf->buf[j]) + 1;
                LoadString(
                hMd,SFADD_NODEST,&lpBuf->buf[k],sizeof(lpBuf->buf)-k);
                n = k + lstrlen(&lpBuf->buf[k]);
                lmemcpy(&lpBuf->buf[n], lpBuf->buf, sizeof(lpBuf->buf)-n);
                lpBuf->buf[sizeof(lpBuf->buf)-1] = '\0';
  
                if (MessageBox(hDB,&lpBuf->buf[k],&lpBuf->buf[j],
                    MB_OKCANCEL | MB_ICONEXCLAMATION) == IDCANCEL)
                {
                    gSF_FLAGS &= ~(SF_NOABORT);
                }
            }
        }
  
        DBGx(("copyFile(): (3) lzClose(%d)", hsrcFile));
        lzClose(hsrcFile);
        DBGx((".. Ok.\n"));
    }
    else if (lpBuf->buf != '\0')
    {
        /*  We failed to open the source (copy from) file.  Report
        *  an err.  Note that both the caption and err message
        *  are built in lpBuf->buf.
        */
        j = lstrlen(lpBuf->buf) + 1;
        LoadString(hMd,SFADD_NOCOPY,&lpBuf->buf[j],sizeof(lpBuf->buf)-j);
        k = j + lstrlen(&lpBuf->buf[j]) + 1;
        LoadString(hMd,SFADD_NOFIND,&lpBuf->buf[k],sizeof(lpBuf->buf)-k);
        n = k + lstrlen(&lpBuf->buf[k]);
        lmemcpy(&lpBuf->buf[n], lpBuf->buf, sizeof(lpBuf->buf)-n);
        lpBuf->buf[sizeof(lpBuf->buf)-1] = '\0';
  
        if (MessageBox(hDB,&lpBuf->buf[k],&lpBuf->buf[j],
            MB_OKCANCEL | MB_ICONEXCLAMATION) == IDCANCEL)
        {
            gSF_FLAGS &= ~(SF_NOABORT);
        }
    }
  
    return (copied);
}   // copyFile()
  
/*  copyScrnFnt
*
*  Copy a screen font file if it has not already been copied.  Note
*  that we can use the normal copyFile routine because the SFDIRSCRFNT
*  struct looks just like a SFDIRFILE struct with a PFM file.
*  Added local buffer because of GP fault when using lpBuf->buf as lpDefault
*  in GetProfileString.
*/
LOCAL WORD copyScrnFnt(hDB, hMd, lpBuf, indScrnFnt)
HWND hDB;
HANDLE hMd;
LPADDREC lpBuf;
int indScrnFnt;
{
    LPSFDIRSCRFNT lpSFscrn;
    LPSTR s;
    BOOL copied = NOT_COPIED;
    WORD rc;
    int indLOGdrv;
    BYTE dkbuf[128];  // Added to prevent GP fault.

    if (lpSFscrn=(LPSFDIRSCRFNT)lockSFdirEntry(0L,indScrnFnt))
    {
        indLOGdrv = lpSFscrn->indLOGdrv;
  
//        lpBuf->buf[0] = '\0';
        dkbuf[0] = '\0';
  
        /*  Get the existing profile string from the win.ini file.
        */
//        GetProfileString(lpBuf->fntAppNm, lpSFscrn->s, lpBuf->buf,
//        lpBuf->buf, sizeof(lpBuf->buf));

        GetProfileString((LPSTR)lpBuf->fntAppNm, (LPSTR)lpSFscrn->s, 
            (LPSTR)dkbuf, (LPSTR)dkbuf, (int)sizeof(dkbuf));
  
//        if (lstrcmpi(lpBuf->buf,&lpSFscrn->s[lpSFscrn->offsFN]) == 0)
        if (lstrcmpi(dkbuf,&lpSFscrn->s[lpSFscrn->offsFN]) == 0)
        {
            /*  The entry already exists, assume it is the same
            *  file and do not copy.
            */
            unlockSFdirEntry(indScrnFnt);
            copied = DUP_SCRFNT;
        }
        else
        {
            /*  Unlock the entry now because we know that copyFile
            *  may cause shifts in the SF directory, so we want to
            *  leave it free to grow if necessry.
            */
            unlockSFdirEntry(indScrnFnt);
  
            /*  Get the full driver file name and strip off the name.
            *  We'll put the screen font file in the same directory
            *  as the driver.  Note that we use the face field in
            *  lpBuf because we know that it is not used by copyFile.
            */
            GetModuleFileName(hMd,lpBuf->face,sizeof(lpBuf->face));
  
            for (s=(LPSTR)lpBuf->face+lstrlen(lpBuf->face);
                s > (LPSTR)lpBuf->face && s[-1] != '\\' && s[-1] != ':'; --s)
                ;
            *s = '\0';
  
            /*  Copy the screen font and update the win.ini file.  Note
            *  that the screen font struct looks almost the same as the
            *  SFDIRFILE struct, and the screen font file data is in
            *  the exact same spots as the PFM file data.
            */
            if ((rc=copyFile(hDB,hMd,
                lpBuf,lpBuf->face,TRUE,indScrnFnt,indLOGdrv)) &&
                (lpSFscrn=(LPSFDIRSCRFNT)lockSFdirEntry(0L,indScrnFnt)))
            {
                WriteProfileString(lpBuf->fntAppNm,
                lpSFscrn->s,&lpSFscrn->s[lpSFscrn->offsFN]);
                unlockSFdirEntry(indScrnFnt);
  
                /*  Change the path of the screen font.
                */
                chngSFdirPath(0L, indScrnFnt, TRUE, lpBuf->face);
                copied = rc;
            }
        }
    }
   
    return (copied);
  
}   // copyScrnFnt()
  
/*  removeCopiedFile
*/
LOCAL BOOL removeCopiedFile(lpPath, indSFfile, rmvPFM, lpBuf, bufsz)
LPSTR lpPath;
int indSFfile;
BOOL rmvPFM;
LPSTR lpBuf;
int bufsz;
{
    BOOL success = FALSE;
  
    if (makeSFdirFileNm(0L,indSFfile,rmvPFM,lpBuf,bufsz) &&
        MergePath(lpPath,lpBuf,bufsz,TRUE))
    {
        DBMSG(("removeCopiedFile(): remove %ls", lpBuf));
  
        success = (dos_delete(lpBuf) == 0);
    }
    #ifdef DEBUG
    else { DBMSG(("removeCopiedFile(): ")); }
    #endif
  
    DBMSG(("...return %ls\n", (success ? (LPSTR)"TRUE" : (LPSTR)"FALSE")));
  
    return (success);
  
}   // removeCopiedFile()
  
/*  isABdrive
*
*  Determine if the file is on the A or B drive, return 1 if it
*  is A, return 2 if it is B, otherwise return 0.
*/
LOCAL int isABdrive(lpFile)
LPSTR lpFile;
{
  
    DBGx(("isABdrive(): (floppy?) file = %ls\n", lpFile));
  
    if (lpFile[1] == ':')
    {
        if (lpFile[0] == 'a' || lpFile[0] == 'A')
        {
            return (1);
        }
        else if (lpFile[0] == 'b' || lpFile[0] == 'B')
        {
            return (2);
        }
    }
  
    return (0);
  
}   // IsABdrive()
  
/*  checkAlertDU
*
*  Check for enough space to fit the passed-in file size.  Alert
*  if there is not enough room and return FALSE.  If there is
*  enough room but the disk space is getting low, throw up an alert
*  to wrn the user.
*
*  lpPath points to the target path, lpFileNm points to the original
*  (uncopied) file name.
*/
LOCAL BOOL checkAlertDU(hDB, hMd, lpPath, lpFileNm, filesize, lpDUwrned, lpBuf, bufsz)
HWND hDB;
HANDLE hMd;
LPSTR lpPath;
LPSTR lpFileNm;
DWORD filesize;
BOOL FAR *lpDUwrned;
LPSTR lpBuf;
int bufsz;
{
    LPDISKINFO lpDiskinfo;
    DWORD total;
    DWORD avail;
    int j, k;
    BYTE drive;
  
    DBGchkdu(("checkAlertDU(%d,%d,%lp,%lp,%ld,%lp,%d): %ls\n",
    hDB, hMd, lpPath, lpFileNm, filesize, lpBuf, bufsz, lpPath));
  
    /*  The drive letter should be the first thing in the path
    *  (followed by a colon).  If not, just return TRUE and hope
    *  there is enough room on the disk.
    */
    if (lpPath[1] != ':')
    {
        DBGchkdu(("checkAlertDU(): path does not contain drive letter!\n"));
        return TRUE;
    }
  
    drive = *lpPath;
  
    /*  Translate drive to 1=A, 26=Z.
    */
    if (drive >= 'A' && drive <= 'Z')
    {
        drive -= (BYTE)'A' - 1;
    }
    else if (drive >= 'a' && drive <= 'z')
    {
        drive -= (BYTE)'a' - 1;
    }
    else
        drive = 0;
  
    if (drive)
    {
        /*  Make sure the passed-in buffer is big enough.
        */
        if (sizeof(DISKINFO) > bufsz)
        {
            DBGchkdu(("checkAlertDU(): lpBuf is not big enough\n"));
            return TRUE;
        }
  
        lpDiskinfo = (LPDISKINFO)lpBuf;
  
        /*  Get the disk usage information.  If this fails, return
        *  success and hope there is enough disk space.
        */
        if (dos_gtfree(drive, lpDiskinfo))
        {
            DBGchkdu(("checkAlertDU(): dos_gtfree returned failure\n"));
            return TRUE;
        }
  
        /*  Calculate the disk usage in bytes.
        */
        avail = total =
        lmul((long)lpDiskinfo->b_sector,(long)lpDiskinfo->s_cluster);
        total = lmul(total,(long)lpDiskinfo->tt_clusters);
        avail = lmul(avail,(long)lpDiskinfo->av_clusters);
  
        /*  Advance filesize to include the minimum allowable
        *  free space after the file is copied.
        */
        filesize += MINDISKFREE;
  
        DBGchkdu(("checkAlertDU(): total=%ld, avail=%ld, filesize=%ld\n",
        total, avail, filesize));
  
        /*  Check for enough room to copy the file.
        */
        if (avail < filesize)
        {
            /*  Not enough room to copy the file, put up an alert
            *  and return FALSE.
            */
            LoadString(hMd, SFINSTAL_NM, lpBuf, bufsz);
            k = lstrlen(lpBuf) + 1;
            LoadString(hMd, SFADD_NOROOM, &lpBuf[k], bufsz-k);
            j = k + lstrlen(&lpBuf[k]);
            lmemcpy(&lpBuf[j], lpFileNm, bufsz-j);
            lpBuf[bufsz-1] = '\0';
  
            MessageBox(hDB, &lpBuf[k], lpBuf, MB_OK | MB_ICONHAND);
  
            return FALSE;
        }
        else if ((avail < (DWORD)ALRTDISKFREE) && !(*lpDUwrned))
        {
            /*  There is enough space to copy the file, but the
            *  free space is getting very low.  Put up an alert
            *  telling the user this is happening.
            */
            WORD percent = (WORD)ldiv(total-avail,ldiv(total,(long)100));
  
            if (percent < 50)
                return TRUE;
            else
            {
                *lpDUwrned = TRUE;
                return (alertDU(hDB,hMd,percent));
            }
        }
    }
  
    return TRUE;
  
}   // checkAlertDU()
