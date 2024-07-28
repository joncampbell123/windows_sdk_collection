/**[f******************************************************************
* sferase.c -
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

/******************************   sferase.c   ******************************/
/*
*  SFerase:  Module for erase soft font entries.
*
*  02 feb 91   rk Changed $ include files to _ include files.        
*  29 nov 89   peterbe Changed MB_ICONQUESTION to MB_ICONEXCLAMATION
*  08 oct 89   peterbe Use hourglass cursor during deletion, don't disable
*          listbox, but disable redraw.
*  27 mar 89   peterbe Use NULL instead of "" in WriteProfileString() to
*          remove a line.
*  09 mar 89   peterbe Physical tabs now 8 spaces in this source.
*   2-20-89    jimmat  Font Installer/Driver use same WIN.INI section (again)!
*   2-21-89    jimmat  Corrected problem with removing SoftFontn= entries.
*/
/***************************************************************************/
  
  
//#define DEBUG
  
#include "nocrap.h"
#undef NOMEMMGR
#undef NOMB
#undef NOWINMESSAGES
#undef NOCTLMGR
#undef NOSHOWWINDOW
#undef NOSCROLL
#undef NOMSG
#undef NOPOINT
#include "windows.h"
#include "neededh.h"
#include "sferase.h"
#include "sfdir.h"
#include "sflb.h"
#define NOBLDDESCSTR
#include "sfutils.h"
#include "strings.h"
#include "sfinstal.h"
#include "dlgutils.h"
  
#include "_cgifwin.h"
#include "_kludge.h"
#include "_tmu.h"
#include "_sflib.h"
  
  
/****************************************************************************\
* Debug Definitions
\****************************************************************************/
  
#ifdef DEBUG
    #define DBGrmvfont(msg)      /*DBMSG(msg)*/
    #define DBGrmvprofile(msg)   /*DBMSG(msg)*/
#else
    #define DBGrmvfont(msg)      /*null*/
    #define DBGrmvprofile(msg)   /*null*/
#endif
  
  
#define LOCAL static
  
  
typedef struct {
    char buf[256];      /* Work buffer: MUST BE FIRST */
    char cap[32];       /* Caption */
    char appName[64];       /* Application name for win.ini */
    char sfstr[32];     /* SoftFontn= line from win.ini */
    char cartstr[32];       /* Cartridgen= line from win.ini */
    char rmving[32];        /* "Removing: " for status line */
} SFREMOVE;
typedef SFREMOVE FAR *LPSFREMOVE;
  
  
LOCAL BOOL RemoveProfileString(LPSTR, LPSTR);
  
/**************************************************************************/
/****************************   Global Procs   ****************************/
  
  
/*  RemoveFonts
*/
BOOL FAR PASCAL RemoveFonts(hDB, hMd, idLB, hSFlb, lpModNm, lpPortNm, lpCount, lpSupportFileDirectory, lpFntLib)
HWND hDB;
HANDLE hMd;
WORD idLB;
HANDLE hSFlb;
LPSTR lpModNm;
LPSTR lpPortNm;
WORD FAR *lpCount;
LPSTR lpSupportFileDirectory;
LPSTR lpFntLib;
{
    MSG msg;
    LPSFLB lpSFlb = 0L;
    LPSFLBENTRY sflb = 0L;
    LPSFLBENTRY j, k;
    LPSFREMOVE lpBuf = 0L;
    HANDLE hBuf = 0;
    LPSTR lpAppNm = 0L;
    BOOL removed = FALSE;
    int response;
    int ind, i, fsnum, numdel;
    //dtk
    LPSTR lpVF = NULL;
    LPSFI_FONTLIBENTRY lpLib;
    int status = 0;
    LPDIRECTORY lpFontLibrary = (LPDIRECTORY)lpFntLib;
    int NumFiles;
    LPSFDIRFILE lpSFfile = 0L;
    BYTE fs_numstr[5];
  
    *lpCount = 0;
    numdel = 0;
  
    if (*lpPortNm && hSFlb && (lpSFlb=(LPSFLB)GlobalLock(hSFlb)) &&
        (hBuf=GlobalAlloc(GMEM_MOVEABLE, (DWORD)sizeof(SFREMOVE))) &&
        (lpBuf=(LPSFREMOVE)GlobalLock(hBuf)) &&
        LoadString(hMd, SF_SOFTFONT, lpBuf->sfstr, sizeof(lpBuf->sfstr)) &&
        LoadString(hMd, SF_CARTRIDGE, lpBuf->cartstr,sizeof(lpBuf->cartstr)) &&
        LoadString(hMd, SF_RMVING, lpBuf->rmving, sizeof(lpBuf->rmving)) &&
        LoadString(hMd, SF_REMVCAP, lpBuf->cap, sizeof(lpBuf->cap)) &&
        LoadString(hMd, SF_REMVMSG, lpBuf->buf, sizeof(lpBuf->buf)) &&
        ((response=MessageBox(hDB, lpBuf->buf, lpBuf->cap,
        MB_YESNOCANCEL | MB_ICONEXCLAMATION)) != IDCANCEL))
    {
        lpAppNm = lpBuf->appName;
  
        /*  Build "[<driver>,<port>]" for accessing win.ini file.
        */
        MakeAppName(lpModNm,lpPortNm,lpAppNm,sizeof(lpBuf->appName));
  
        /*  Gray listbox and remove highlight.
        */
        SendMessage(GetDlgItem(hDB,idLB), WM_SETREDRAW, FALSE, 0L);
        SendDlgItemMessage(hDB, idLB, LB_SETSEL, FALSE, (long)(-1));
  
        ///EnableWindow(GetDlgItem(hDB,idLB), FALSE);
  
        SendMessage(GetDlgItem(hDB,idLB), WM_VSCROLL, SB_TOP, 0L);
        ///SendMessage(GetDlgItem(hDB,idLB), WM_SETREDRAW, TRUE, 0L);
        ///InvalidateRect(GetDlgItem(hDB,idLB), (LPRECT)0L, FALSE);
  
        /*  Kill all the other controls.
        */
        SetDlgItemText(hDB, SF_STATUS, (LPSTR)"");
        EnableWindow(GetDlgItem(hDB, SF_MOVE), FALSE);
        EnableWindow(GetDlgItem(hDB, SF_COPY), FALSE);
        EnableWindow(GetDlgItem(hDB, SF_ERASE), FALSE);
        EnableWindow(GetDlgItem(hDB, SF_EDIT), FALSE);
        CheckRadioButton(hDB, SF_PERM_LEFT, SF_TEMP_LEFT, 0);
        EnableWindow(GetDlgItem(hDB, SF_PERM_LEFT), FALSE);
        EnableWindow(GetDlgItem(hDB, SF_TEMP_LEFT), FALSE);
        EnableWindow(GetDlgItem(hDB, SF_ADD_RIGHT), FALSE);
  
        /*  Change exit button to Cancel.
        */
        if (LoadString(hMd,SF_CNCLSTR,lpBuf->buf,sizeof(lpBuf->buf)))
        {
            SetDlgItemText(hDB, SF_EXIT, lpBuf->buf);
            gSF_FLAGS |= SF_NOABORT;
        }
  
        /*  For each listbox item.
        */
        for (ind=0, sflb=&lpSFlb->sflb[0];
            ind < lpSFlb->free && (gSF_FLAGS & SF_NOABORT); )
        {
            DBGrmvfont(("SFERASE: Inside the for loop\n"));
  
            // keep loading hourglass cursor
            SetCursor(LoadCursor(NULL,IDC_WAIT));
  
            /*  If it is selected, then remove its entry from the
            *  win.ini file.
            */
  
            if ((sflb->state & SFLB_SEL) &&
                (lpSFfile=(LPSFDIRFILE)lockSFdirEntry(0L,sflb->indSFfile)))
  
            {
  
                /* 11-27-90 dtk added to delete .typ files
                */
                if (sflb->state & SFLB_FAIS)
                {
                    DBGrmvfont(("SFERASE: It is a typ file\n"));
                    /* remove the entry from the support files
                    */
                    if (lpFontLibrary->hFiles == NULL)
                    {
                        DBGrmvfont(("SFERASE:  hFiles == NULL\n"));
                        GlobalUnlock(hSFlb);
                        GlobalUnlock(hBuf);
                        GlobalFree(hBuf);
                        return(FALSE);
                    }
  
                    DBGrmvfont(("SFERASE: after hFiles check\n"));
  
                    if ((lpVF = (LPSTR) GlobalLock(lpFontLibrary->hFiles)) == NULL)
                    {
                        DBGrmvfont(("SFERASE: Unable to lock memory\n"));
                        GlobalUnlock(hSFlb);
                        GlobalUnlock(hBuf);
                        GlobalFree(hBuf);
                        return(FALSE);
                    }
  
                    DBGrmvfont(("SFERASE: numfiles = %d\n",lpFontLibrary->NumFiles));
                    for (NumFiles = 0; NumFiles<lpFontLibrary->NumFiles; ++NumFiles)
                    {
                        lpLib = (LPSFI_FONTLIBENTRY)lpVF;
  
                        DBGrmvfont(("selected = %d\n", lpLib->Selected));
                        DBGrmvfont(("usage = %d\n", lpLib->usage));
                        DBGrmvfont(("lib->name = %ls\n", (LPSTR)lpLib+lpLib->OffsetName));
                        DBGrmvfont(("lpsfile->name = %ls\n", (LPSTR)lpSFfile->s));
  
                        if ((lstrcmp((LPSTR)lpLib+lpLib->OffsetName, (LPSTR)lpSFfile->s) == 0)
                            && lpLib->usage)
  
                        {
                            /*  Blow away the file if the user requested it.
                            */
  
                            DBGrmvfont(("SFERASE: inside 2nd selected if\n"));
  
                            if (response == IDYES)
                                status = FACEdelete((LPTYPEINFO) &lpLib->TypeInfo, lpSupportFileDirectory, TRUE);
  
                            else
                                status = FACEdelete((LPTYPEINFO) &lpLib->TypeInfo, lpSupportFileDirectory, FALSE);
  
                            if (!status)
                            {
                                ++numdel;
                                lpLib->Selected = 0;
                                lpLib->usage = 0;
                            }
                        }
                        lpVF += lpLib->Length;
                    }
                    GlobalUnlock(lpFontLibrary->hFiles);
  
  
                    /*  Remove the entry from the listbox.
                    */
                    SendDlgItemMessage(hDB,idLB,LB_DELETESTRING,(WORD)ind,0L);
  
                    /*  Remove the entry from the SF directory.
                    */
                    delSFdirEntry(0L, sflb->indSFfile);
  
                    /*  Shuffle the contents of the SFLB struct
                    *  back one item.
                    */
                    for (j=&sflb[0], k=&sflb[1], i=ind+1;
                        i < lpSFlb->free; ++i, ++j, ++k)
                    {
                        *j = *k;
                    }
                    --lpSFlb->free;
  
                    /*  Increment count of deleted fonts.
                    */
                    ++(*lpCount);
  
                    /*  Process any messages to the installer's dialog box
                    *  so we can detect the cancel button.
                    */
                    while (PeekMessage(&msg, hDB, NULL, NULL, TRUE) &&
                        IsDialogMessage(hDB, &msg))
                        ;
  
                    /* Bug # 171 -- this should fix it
                    */
                    unlockSFdirEntry(sflb->indSFfile);
  
  
                }
                else
                {
  
                    lstrcpy(lpBuf->buf, lpBuf->rmving);
                    i = lstrlen(lpBuf->buf) - 1;
                    SendDlgItemMessage(hDB, idLB, LB_GETTEXT, (WORD)ind,
                    (long)(LPSTR)&lpBuf->buf[i]);
                    lpBuf->buf[i] = ' ';
                    SetDlgItemText(hDB, SF_STATUS, lpBuf->buf);
  
                    /*  Remove "SoftFontn=" entry.
                    */
                    if (sflb->id<0)
                    {
                        lstrcpy(lpBuf->buf, lpBuf->cartstr);
                        itoa(-sflb->id, &lpBuf->buf[lstrlen(lpBuf->buf)]);
                    }
                    else
                    {
                        lstrcpy(lpBuf->buf, lpBuf->sfstr);
                        itoa(sflb->id, &lpBuf->buf[lstrlen(lpBuf->buf)]);
                    }
                    RemoveProfileString(lpAppNm, lpBuf->buf);
  
                    /*  Remove PFMfile=DLfile reference in win.ini file.
                    */
                    if (makeSFdirFileNm(0L, sflb->indSFfile, TRUE,
                        lpBuf->buf, sizeof(lpBuf->buf)))
                    {
                        RemoveProfileString(lpAppNm, lpBuf->buf);
                    }
  
                    /*  Blow away the files if the user requested it.
                    */
                    if (response == IDYES)
                    {
                        delSFdirFile(0L, sflb->indSFfile,
                        lpBuf->buf, sizeof(lpBuf->buf));
                    }
  
                    /*  Remove the entry from the listbox.
                    */
                    SendDlgItemMessage(hDB,idLB,LB_DELETESTRING,(WORD)ind,0L);
  
                    /*  Remove the entry from the SF directory.
                    */
                    delSFdirEntry(0L, sflb->indSFfile);
  
                    /*  Shuffle the contents of the SFLB struct
                    *  back one item.
                    */
                    for (j=&sflb[0], k=&sflb[1], i=ind+1;
                        i < lpSFlb->free; ++i, ++j, ++k)
                    {
                        *j = *k;
                    }
                    --lpSFlb->free;
  
                    /*  Increment count of deleted fonts.
                    */
                    ++(*lpCount);
  
                    /*  Process any messages to the installer's dialog box
                    *  so we can detect the cancel button.
                    */
                    while (PeekMessage(&msg, hDB, NULL, NULL, TRUE) &&
                        IsDialogMessage(hDB, &msg))
                        ;
  
                    /* Bug # 171 -- this should fix it
                    */
                    unlockSFdirEntry(sflb->indSFfile);
  
                }
            }
            else
            {
                ++ind;
                ++sflb;
  
                /*  Scroll listbox.
                */
                SendMessage(GetDlgItem(hDB,idLB), WM_VSCROLL, SB_LINEDOWN, 0L);
            }
        }
  
        DBGdumpSFbuf(0L);
  
        /*  No previous selection.
        */
        lpSFlb->prevsel = -1;
  
        /*  Enable listbox.
        */
  
        //SendMessage(GetDlgItem(hDB,idLB), WM_SETREDRAW, FALSE, 0L);
        //EnableWindow(GetDlgItem(hDB,idLB), TRUE);
  
        SendMessage(GetDlgItem(hDB,idLB), WM_VSCROLL, SB_TOP, 0L);
        SendMessage(GetDlgItem(hDB,idLB), WM_SETREDRAW, TRUE, 0L);
        InvalidateRect(GetDlgItem(hDB,idLB), (LPRECT)0L, FALSE);
  
        // arrow cursor again.
        SetCursor(LoadCursor(NULL,IDC_ARROW));
  
        /*  Restore exit button.
        */
        if (LoadString(hMd,SF_EXITSTR,lpBuf->buf,sizeof(lpBuf->buf)))
        {
            SetDlgItemText(hDB, SF_EXIT, lpBuf->buf);
            gSF_FLAGS &= ~(SF_NOABORT);
        }
  
        SetDlgItemText(hDB, SF_STATUS, (LPSTR)"");
        EnableWindow(GetDlgItem(hDB, SF_ADD_RIGHT), TRUE);
  
        /*  Flag new fontSummary in win.ini file.
        */
        NewFS(hMd, lpAppNm);
  
        /* Update the number of ifw fonts in win.ini
        */
        fsnum = GetProfileInt((LPSTR)lpAppNm,(LPSTR)"IfwFonts",0);
        fsnum -= numdel;
        itoa(fsnum, fs_numstr);
        WriteProfileString((LPSTR)lpAppNm,(LPSTR)"IfwFonts", (LPSTR)fs_numstr);
  
        removed = TRUE;
    }
  
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
  
    return (removed);
}
  
/**************************************************************************/
/*****************************   Local Procs   ****************************/
  
  
/*  RemoveProfileString
*
*  Remove a profile string only if it is there.
*/
LOCAL BOOL RemoveProfileString(lpAppName, lpKeyName)
LPSTR lpAppName;
LPSTR lpKeyName;
{
    char buf[4];
  
    DBGrmvprofile(("RemoveProfileString(%ls,%ls)\n",
    lpAppName, lpKeyName));
  
    buf[0] = '\0';
  
    if (GetProfileString(lpAppName, lpKeyName, buf, buf, sizeof(buf)) &&
        (buf[0] != '\0'))
    {
        //return (WriteProfileString(lpAppName, lpKeyName, (LPSTR)""));
        return (WriteProfileString(lpAppName, lpKeyName, (LPSTR)NULL));
    }
  
    return TRUE;
}
