/**[f******************************************************************
* sfedit.c -
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

/*******************************   sfedit.c   ******************************/
/*
*  SFedit:  Module for editing PFM information.
*
*  04 Sep 91   RK(HP) Changed OpenFile comparison in readNameFamily
*  04 Sep 91   RK(HP) Changed OpenFile comparison in writeNameFamily
*  07 aug 89   peterbe Changed all lstrcmp() to lstrcmpi().
*  27 jul 89   peterbe Use NULL instead of NullStr so 'blank' line is deleted
*          from WIN.INI.
*  12 may 89   peterbe Blank in second character of string, after '*' or ' ',
*          in ShowFace().  Likewise, name string is shifted
*          over 2 in ChangeFontStuff().  This corresponds to
*          change in buildDescrString() in SFUTILS.
*          Changed ' ' to 0x20 everywhere!
*   1-13-89    jimmat  Reduced # of redundant strings by adding lclstr.h
*   1-26-89    jimmat  Adjustments due to changes in resource file.
*   2-20-89    jimmat  Font Installer/Driver use same WIN.INI section (again)!
*/
/***************************************************************************/
  
  
//#define DEBUG
  
#include "nocrap.h"
#undef NOCTLMGR
#undef NOWINMESSAGES
#undef NOSCROLL
#undef NOMEMMGR
#undef NOGDI
#undef NOOPENFILE
#undef NOSHOWWINDOW
#undef NOMSG
#undef NOPOINT
#undef NOMB
#include "windows.h"
#include "neededh.h"
#include "resource.h"
#include "strings.h"
#include "dlgutils.h"
#include "sfdir.h"
#include "sflb.h"
#define NOBLDDESCSTR
#include "sfutils.h"
#include "sfedit.h"
#include "pfm.h"
#include "lclstr.h"
  
  
/****************************************************************************\
*                     Debug Definitions
\****************************************************************************/
  
#ifdef DEBUG
    #define DBGedit(msg)        /*DBMSG(msg)*/
    #define DBGdlgfn(msg)      /*DBMSG(msg)*/
#else
    #define DBGedit(msg)       /*null*/
    #define DBGdlgfn(msg)   /*null*/
#endif
  
  
  
  
#define EDBUFSZ 256
#define LOCAL static
  
typedef struct {
    EDREC edrec;        /* Edit record for editPFM */
    char buf[EDBUFSZ];      /* Work buffer: atleast sizeof(PFMHEADER) */
    char editing[32];       /* "Editing: " for status line */
    char point[32];     /* For description */
    char bold[32];      /* For description */
    char italic[32];        /* For description */
    OFSTRUCT ofstruct;      /* Open file struct */
} TMPEDIT;
  
typedef TMPEDIT FAR *LPTMPEDIT;
  
  
/*  Forward references
*/
int FAR PASCAL EditDlgFn(HWND, unsigned, WORD, LONG);
LOCAL BOOL readNameFamily(LPTMPEDIT, int);
LOCAL BOOL writeNameFamily(LPTMPEDIT, int, BOOL);
LOCAL void ChangeFontStuff(HWND, HANDLE, LPTMPEDIT, LPSFLBENTRY, int, WORD, HANDLE);
LOCAL BOOL ShowFace(HWND, LPEDREC);
LOCAL BOOL checkID(HWND, HANDLE, LPEDREC);
LOCAL BOOL checkFace(HWND, HANDLE, LPEDREC);
LOCAL BOOL MoveProfileString(HANDLE, LPSTR, int, int, LPSTR, int);
  
  
LOCAL LPEDREC gLPED;
  
/**************************************************************************/
/****************************   Global Procs   ****************************/
  
/*  EditFonts
*/
BOOL FAR PASCAL
EditFonts(hDB, hMd, idLB, hSFlb, hoSFlb, lpModNm, lpPortNm, lpCount)
HWND hDB;
HANDLE hMd;
WORD idLB;
HANDLE hSFlb;
HANDLE hoSFlb;
LPSTR lpModNm;
LPSTR lpPortNm;
WORD FAR *lpCount;
{
    LPSFLB lpSFlb = 0L;
    LPSFLBENTRY sflb = 0L;
    LPSFLBENTRY j = 0L;
    LPSFDIRFILE lpSFfile = 0L;
    LPTMPEDIT lpBuf = 0L;
    LPEDREC lpEDrec = 0L;
    LPSTR lpAppNm = 0L;
    HANDLE hBuf = 0;
    BOOL success = FALSE;
    int ind;
    int i;
  
    *lpCount = 0;
  
    if (hSFlb && (lpSFlb=(LPSFLB)GlobalLock(hSFlb)) &&
        (hBuf=GlobalAlloc(GMEM_MOVEABLE,(DWORD)sizeof(TMPEDIT))) &&
        (lpBuf=(LPTMPEDIT)GlobalLock(hBuf)) &&
        LoadString(hMd,SFED_EDITING,lpBuf->editing,sizeof(lpBuf->editing)) &&
        LoadString(hMd, SF_POINT, lpBuf->point, sizeof(lpBuf->point)) &&
        LoadString(hMd, SF_BOLD, lpBuf->bold, sizeof(lpBuf->bold)) &&
        LoadString(hMd, SF_ITALIC, lpBuf->italic, sizeof(lpBuf->italic)))
    {
        lpEDrec = &lpBuf->edrec;
        lmemset((LPSTR)lpEDrec, 0, sizeof(EDREC));
        lpAppNm = lpBuf->edrec.appName;
  
        /*  Build "[<driver>,<port>]" for accessing win.ini file.
        */
        MakeAppName(lpModNm,lpPortNm,lpAppNm,sizeof(lpBuf->edrec.appName));
  
        /*  Gray listbox and remove highlight.
        */
        SendMessage(GetDlgItem(hDB,SF_LB_LEFT), WM_SETREDRAW, FALSE, 0L);
        SendMessage(GetDlgItem(hDB,SF_LB_RIGHT), WM_SETREDRAW, FALSE, 0L);
        SendDlgItemMessage(hDB, idLB, LB_SETSEL, FALSE, (long)(-1));
        EnableWindow(GetDlgItem(hDB,idLB), FALSE);
        SendMessage(GetDlgItem(hDB,idLB), WM_SETREDRAW, TRUE, 0L);
        InvalidateRect(GetDlgItem(hDB,idLB), (LPRECT)0L, FALSE);
  
        SetDlgItemText(hDB, SF_STATUS, (LPSTR)NullStr);
        EnableWindow(GetDlgItem(hDB, SF_MOVE), FALSE);
        EnableWindow(GetDlgItem(hDB, SF_COPY), FALSE);
        EnableWindow(GetDlgItem(hDB, SF_ERASE), FALSE);
        EnableWindow(GetDlgItem(hDB, SF_EDIT), FALSE);
  
        if (idLB == SF_LB_LEFT)
        {
            CheckRadioButton(hDB, SF_PERM_LEFT, SF_TEMP_LEFT, 0);
            EnableWindow(GetDlgItem(hDB, SF_PERM_LEFT), FALSE);
            EnableWindow(GetDlgItem(hDB, SF_TEMP_LEFT), FALSE);
        }
        else
        {
            CheckRadioButton(hDB, SF_PERM_RIGHT, SF_TEMP_RIGHT, 0);
            EnableWindow(GetDlgItem(hDB, SF_PERM_RIGHT), FALSE);
            EnableWindow(GetDlgItem(hDB, SF_TEMP_RIGHT), FALSE);
        }
  
        lpEDrec->hSFlb = hSFlb;
        lpEDrec->hMd = hMd;
        lpEDrec->normalEdit = TRUE;
  
  
        /*  For each listbox item.
        */
        for (ind=0, sflb=&lpSFlb->sflb[0];
            ind < lpSFlb->free && !lpEDrec->stop;
            (sflb->state &= ~(SFLB_SEL)),
            ++ind, ++sflb)
        {
            /*  If it is selected, then edit the PFM.
            */
            if ((sflb->state & SFLB_SEL) &&
                (lpSFfile=(LPSFDIRFILE)lockSFdirEntry(0L,sflb->indSFfile)))
            {
                /* 11-27-90 dtk
                * if it is a .typ file, pop a message
                * box for edit not allowed!
                */
                if (sflb->state & SFLB_FAIS)
                {
                    MessageBox(hDB,(LPSTR)"Scalable screen font files can not be edited!",
                    (LPSTR)NULL, MB_OK | MB_ICONEXCLAMATION);
                }
                else
                {
  
                    /*  Update the status line and pick up the name of
                    *  the download file.
                    */
                    if (lpSFfile->offsPFMname)
                    {
                        lstrcpy(lpBuf->buf, lpBuf->editing);
                        lstrcat(lpBuf->buf, &lpSFfile->s[lpSFfile->offsPFMname]);
                        SetDlgItemText(hDB, SF_STATUS, lpBuf->buf);
                        if (lpSFfile->offsDLname)
                        {
                            lmemcpy(lpEDrec->dlname,
                            &lpSFfile->s[lpSFfile->offsDLname],13);
                            lpEDrec->dlname[12] = '\0';
                        }
                        unlockSFdirEntry(sflb->indSFfile);
                    }
                    else
                    {
                        unlockSFdirEntry(sflb->indSFfile);
                        continue;
                    }
  
                    /*  If the global flag has been set, then all fonts
                    *  will receive the same change.
                    */
                    if (lpEDrec->global)
                    {
                        lpEDrec->ind = ind;
                        lpEDrec->id = sflb->id;
                        lpEDrec->state = sflb->state;
  
                        /*  Note: lpEDrec->desc is changed by writeNameFamily
                        *  because the point size and bold, italics do not
                        *  change for the font.
                        */
                        if (writeNameFamily(lpBuf,sflb->indSFfile,TRUE))
                        {
                            /*  Update SF dir and listboxes.
                            */
                            ChangeFontStuff(hDB,hMd,lpBuf,sflb,ind,idLB,hoSFlb);
  
                            /*  Increment count of edited fonts.
                            */
                            ++(*lpCount);
                        }
                        continue;
                    }
  
                    /*  Determine whether or not we should show 'next'
                    *  button on listbox.
                    */
                    for (i=ind+1, j=&sflb[1]; i < lpSFlb->free; ++i, ++j)
                    {
                        if (j->state & SFLB_SEL)
                            break;
                    }
                    lpEDrec->showNext = (BOOL)(i < lpSFlb->free);
  
                    if (!readNameFamily(lpBuf,sflb->indSFfile))
                        continue;
  
                    lpEDrec->ind = ind;
                    lpEDrec->id = sflb->id;
                    lpEDrec->state = sflb->state;
  
                    if (editPFM(hDB, hMd, lpEDrec))
                    {
                        if (writeNameFamily(lpBuf,sflb->indSFfile,FALSE))
                        {
                            /*  Update win.ini, SF dir and listboxes.
                            */
                            ChangeFontStuff(hDB,hMd,lpBuf,sflb,ind,idLB,hoSFlb);
                            success = TRUE;
  
                            /*  Increment count of edited fonts.
                            */
                            ++(*lpCount);
                        }
                        else
                        {
                            /*  Failed to write PFM changes.
                            */
                            if (LoadString(hMd,SFED_WRPFMMSG,
                                lpBuf->buf,sizeof(lpBuf->buf)) &&
                                (i = lstrlen(lpBuf->buf) + 1) &&
                                LoadString(hMd,SFED_WRPRMCAP,
                                &lpBuf->buf[i],sizeof(lpBuf->buf)-i))
                            {
                                MessageBox(hDB,&lpBuf->buf[i],lpBuf->buf,
                                MB_OK | MB_ICONEXCLAMATION);
                            }
  
                        }
  
                    }
                } /* else (soft font) */
  
            } /* if selected */
  
        } /* for */
  
        /*  No previous selection.
        */
        lpSFlb->prevsel = -1;
  
        /*  Enable listbox.
        */
        SendMessage(GetDlgItem(hDB,idLB), WM_SETREDRAW, FALSE, 0L);
        EnableWindow(GetDlgItem(hDB,idLB), TRUE);
        SendMessage(GetDlgItem(hDB,SF_LB_LEFT), WM_SETREDRAW, TRUE, 0L);
        SendMessage(GetDlgItem(hDB,SF_LB_RIGHT), WM_SETREDRAW, TRUE, 0L);
        InvalidateRect(GetDlgItem(hDB,SF_LB_LEFT), (LPRECT)0L, FALSE);
        InvalidateRect(GetDlgItem(hDB,SF_LB_RIGHT), (LPRECT)0L, FALSE);
        SetDlgItemText(hDB, SF_STATUS, (LPSTR)NullStr);
  
        /*  Flag new fontSummary in win.ini file.
        */
        NewFS(hMd, lpAppNm);
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
  
    return (success);
}
  
/*  editPFM
*/
BOOL FAR PASCAL editPFM(hDB, hMd, lpEDrec)
HWND hDB;
HANDLE hMd;
LPEDREC lpEDrec;
{
    MSG msg;
    int response = IDCANCEL;
  
    DBGedit(("editPFM(%d,%d,%lp)\n", hDB, hMd, lpEDrec));
  
    if (gLPED = lpEDrec)
    {
        /*  Translate family to dialog controls.
        */
        switch (lpEDrec->family & 0xF0)
        {
            case FF_ROMAN:
                lpEDrec->family = SFED_ROMAN;
                break;
            case FF_SWISS:
                lpEDrec->family = SFED_SWISS;
                break;
            case FF_MODERN:
                lpEDrec->family = SFED_MODERN;
                break;
            case FF_SCRIPT:
                lpEDrec->family = SFED_SCRIPT;
                break;
            case FF_DECORATIVE:
                lpEDrec->family = SFED_DECORATIVE;
                break;
            case FF_DONTCARE:
            default:
                if (lpEDrec->normalEdit)
                    lpEDrec->family = SFED_DONTCARE;
                else
                    lpEDrec->family = SFED_ROMAN;
                break;
        }
  
        lpEDrec->hMd = hMd;
  
        /*  Translate state to dialog controls.
        */
        if (lpEDrec->state & SFLB_PERM)
        {
            lpEDrec->state = SFED_PERM;
        }
        else
        {
            lpEDrec->state = SFED_TEMP;
        }
  
        /*  Pull up dialog.
        */
        if (lpEDrec->hEDwnd || lpEDrec->showNext)
        {
            if (!lpEDrec->hEDwnd)
            {
                /*  Pull up a modeless dialog box so we can edit
                *  more than one font without having to pull down
                *  the dialog between each font.
                */
                if (!(lpEDrec->hEDwnd = CreateDialog(hMd,
                    MAKEINTRESOURCE(lpEDrec->normalEdit ?
                    SFEDIT : SFUNKNOWN),
                    hDB, EditDlgFn)))
                {
                    DBGedit((
                    "editPFM(): *failed* to pull up modeless dialog\n"));
                    lpEDrec->stop = TRUE;
                    return FALSE;
                }
  
                ShowWindow(lpEDrec->hEDwnd, SHOW_OPENWINDOW);
                UpdateWindow(lpEDrec->hEDwnd);
            }
            else
            {
                /*  The dialog is already up, send an init message so
                *  it will display the next font.
                */
                SendMessage(lpEDrec->hEDwnd, WM_INITDIALOG, 0, 0L);
            }
  
            lpEDrec->response = 0;
  
            /*  Wait until the user clicks on the OK, Cancel,
            *  or Stop button.
            */
            while (lpEDrec->response != IDOK &&
                lpEDrec->response != IDNEXT &&
                lpEDrec->response != IDCANCEL)
            {
                if (PeekMessage(&msg,lpEDrec->hEDwnd,NULL,NULL,TRUE) &&
                    IsDialogMessage(lpEDrec->hEDwnd,&msg))
                {
                    UpdateWindow(lpEDrec->hEDwnd);
                }
            }
            response = lpEDrec->response;
  
            /*  The dialog was pulled down if this was the last font,
            *  the user clicked on the next button, or the user checked
            *  the "changes are global" button.
            */
            if (lpEDrec->stop || !lpEDrec->showNext || lpEDrec->global)
            {
                lpEDrec->hEDwnd = 0;
            }
        }
        else
        {
            /*  Just one font, so put up a modal dialog box.
            */
            response = MyDialogBox(hMd,
            lpEDrec->normalEdit ? SFEDIT : SFUNKNOWN,
            hDB, EditDlgFn);
        }
  
        /*  Convert dialog control to Windows' family value.
        */
        switch (lpEDrec->family)
        {
            case SFED_ROMAN:
                lpEDrec->family = FF_ROMAN;
                break;
            case SFED_SWISS:
                lpEDrec->family = FF_SWISS;
                break;
            case SFED_MODERN:
                lpEDrec->family = FF_MODERN;
                break;
            case SFED_SCRIPT:
                lpEDrec->family = FF_SCRIPT;
                break;
            case SFED_DECORATIVE:
                lpEDrec->family = FF_DECORATIVE;
                break;
            case SFED_DONTCARE:
            default:
                lpEDrec->family = FF_DONTCARE;
                break;
        }
  
        /*  Convert dialog control to state.
        */
        if (lpEDrec->state == SFED_PERM)
        {
            lpEDrec->state = SFLB_PERM;
        }
        else
        {
            lpEDrec->state = 0;
        }
    }
  
    return (response == IDOK);
}
  
/*  EditDlgFn
*/
int FAR PASCAL EditDlgFn(hDB, wMsg, wParam, lParam)
HWND hDB;
unsigned wMsg;
WORD wParam;
LONG lParam;
{
    switch (wMsg)
    {
        case WM_INITDIALOG:
            DBGdlgfn(("EditDlgFn(%d,%d,%d,%ld): WM_INITDIALOG\n",
            hDB, wMsg, wParam, lParam));
            CenterDlg(hDB);
  
            SetDlgItemText(hDB, SFED_FILE, gLPED->dlname);
            SetDlgItemText(hDB, SFED_DESC, gLPED->desc);
            SetDlgItemText(hDB, SFED_NAME, gLPED->name);
            CheckRadioButton(hDB, SFED_ROMAN, SFED_DONTCARE, gLPED->family);
            CheckDlgButton(hDB, SFED_GLOBAL, (gLPED->global=FALSE));
  
            if (gLPED->normalEdit)
            {
                if (!gLPED->showNext)
                {
                    EnableWindow(GetDlgItem(hDB,SFED_GLOBAL), FALSE);
                    ShowWindow(GetDlgItem(hDB,IDNEXT), HIDE_WINDOW);
                }
                SetDlgItemInt(hDB, SFED_ID, gLPED->id, FALSE);
                CheckRadioButton(hDB, SFED_PERM, SFED_TEMP, gLPED->state);
            }
            else
                ShowWindow(GetDlgItem(hDB,SFED_DONTCARE), HIDE_WINDOW);
  
            SetDlgItemText(hDB, SFED_NAME, gLPED->name); // already did this!
            PostMessage(GetDlgItem(hDB,SFED_NAME),EM_SETSEL,0,0x7FFF0000L);
  
            gLPED->response = 0;
            break;
  
        case WM_COMMAND:
        switch (wParam)
        {
            case SFED_NAME:
                DBGdlgfn(("EditDlgFn(%d,%d,%d,%ld): SFED_NAME\n",
                hDB, wMsg, wParam, lParam));
                ShowFace(hDB, gLPED);
                break;
  
            case SFED_ID:
                DBGdlgfn(("EditDlgFn(%d,%d,%d,%ld): SFED_ID\n",
                hDB, wMsg, wParam, lParam));
                gLPED->id = GetDlgItemInt(hDB,SFED_ID,0L,FALSE);
                break;
  
            case SFED_PERM:
            case SFED_TEMP:
                DBGdlgfn(("EditDlgFn(%d,%d,%d,%ld): PERM/TEMP\n",
                hDB, wMsg, wParam, lParam));
                CheckRadioButton(hDB, SFED_PERM, SFED_TEMP,
                gLPED->state = wParam);
                ShowFace(hDB, gLPED);
                break;
  
            case SFED_ROMAN:
            case SFED_SWISS:
            case SFED_MODERN:
            case SFED_SCRIPT:
            case SFED_DECORATIVE:
            case SFED_DONTCARE:
                DBGdlgfn(("EditDlgFn(%d,%d,%d,%ld): FAMILY\n",
                hDB, wMsg, wParam, lParam));
                CheckRadioButton(hDB, SFED_ROMAN, SFED_DONTCARE,
                gLPED->family = wParam);
                break;
  
            case SFED_GLOBAL:
                DBGdlgfn(("EditDlgFn(%d,%d,%d,%ld): SFED_GLOBAL\n",
                hDB, wMsg, wParam, lParam));
                CheckDlgButton(hDB, SFED_GLOBAL,
                (gLPED->global = !gLPED->global));
                break;
  
            case IDOK:
            case IDCANCEL:
            case IDNEXT:
        #ifdef DEBUG
                if (wParam == IDOK)
                { DBGdlgfn(("EditDlgFn(%d,%d,%d,%ld): IDOK\n",
                hDB, wMsg, wParam, lParam)); }
                else if (wParam == IDCANCEL)
                { DBGdlgfn(("EditDlgFn(%d,%d,%d,%ld): IDCANCEL\n",
                hDB, wMsg, wParam, lParam)); }
                else
                { DBGdlgfn(("EditDlgFn(%d,%d,%d,%ld): IDNEXT\n",
                hDB, wMsg, wParam, lParam)); }
        #endif
  
                gLPED->response = wParam;
  
                if (wParam == IDOK &&
                    (!checkID(hDB, gLPED->hMd, gLPED) ||
                    !checkFace(hDB, gLPED->hMd, gLPED)))
                {
                    gLPED->response = 0;
                    break;
                }
  
                if (gLPED->global &&
                    (wParam == IDCANCEL || wParam == IDNEXT))
                {
                    /*  Uncheck global changes button if the
                    *  user chose cancel or next.
                    */
                    CheckDlgButton(hDB, SFED_GLOBAL, FALSE);
                    gLPED->global = FALSE;
                }
  
                if (gLPED->stop=(wParam == IDCANCEL))
                {
                    /*  The user clicked on the cancel button.
                    */
                    EndDialog(hDB, wParam);
                }
                else if (!gLPED->showNext)
                {
                    /*  The next button is not an option,
                    *  so we must pull down the dialog.
                    */
                    EndDialog(hDB, wParam);
                }
                else if (gLPED->global)
                {
                    /*  The user clicked on the global changes
                    *  button, so pull down the dialog because
                    *  the same change will be applied to all
                    *  selected fonts.
                    */
                    EndDialog(hDB, wParam);
                }
                break;
        }
            break;
  
        default:
            return FALSE;
    }
  
    return TRUE;
}
  
/*  DupIdDlgFn
*/
int FAR PASCAL DupIdDlgFn(hDB, wMsg, wParam, lParam)
HWND hDB;
unsigned wMsg;
WORD wParam;
long lParam;
{
    switch (wMsg)
    {
        case WM_INITDIALOG:
            CenterDlg(hDB);
        {
            LPSFDIRFILE lpSFfile;
  
            if (lpSFfile=(LPSFDIRFILE)lockSFdirEntry(0L,gLPED->indSFfile))
            {
                SetDlgItemText(hDB, SFED_DUPIDNM, lpSFfile->s);
                unlockSFdirEntry(gLPED->indSFfile);
            }
        }
            break;
  
        case WM_COMMAND:
        switch (wParam)
        {
            case SFED_DUPIDNM:
                DBGdlgfn(("EditDlgFn(%d,%d,%d,%ld): SFED_DUPIDNM\n",
                hDB, wMsg, wParam, lParam));
                break;
  
            case IDOK:
            case IDCANCEL:
        #ifdef DEBUG
                if (wParam == IDOK)
                { DBGdlgfn(("EditDlgFn(%d,%d,%d,%ld): IDOK\n",
                hDB, wMsg, wParam, lParam)); }
                else
                { DBGdlgfn(("EditDlgFn(%d,%d,%d,%ld): IDCANCEL\n",
                hDB, wMsg, wParam, lParam)); }
        #endif
  
                EndDialog(hDB, wParam);
                break;
        }
            break;
  
        default:
            return FALSE;
    }
  
    return TRUE;
}
  
/**************************************************************************/
/*****************************   Local Procs   ****************************/
  
/*  readNameFamily
*
*  Read the face name and family from the PFM file.
*/
LOCAL BOOL readNameFamily(lpBuf, indSFfile)
LPTMPEDIT lpBuf;
int indSFfile;
{
    LPEDREC lpEDrec = &lpBuf->edrec;
    BOOL success = FALSE;
    int hFile;
    int ind;
  
    if (makeSFdirFileNm(0L,indSFfile,TRUE,lpBuf->buf,sizeof(lpBuf->buf)) &&
//        (hFile=OpenFile(lpBuf->buf,&lpBuf->ofstruct,OF_READ)) > 0)
        (hFile=OpenFile(lpBuf->buf,&lpBuf->ofstruct,OF_READ)) >= 0)                      // RK 09/04/91
    {
        if (_lread(hFile,lpBuf->buf,sizeof(PFMHEADER)) == sizeof(PFMHEADER))
        {
            LPPFMHEADER lpPFMhead = (LPPFMHEADER)lpBuf->buf;
            long scaled;
  
            /*  Pick up family.
            */
            lpEDrec->family = lpPFMhead->dfPitchAndFamily & 0xF0;
  
            /*  Make description string to append to the face name.
            */
            makeDesc((LPSTR)lpPFMhead, lpEDrec->desc, sizeof(lpEDrec->desc),
            lpBuf->point, lpBuf->bold, lpBuf->italic);
  
            if (_llseek(hFile,lpPFMhead->dfFace,0) > 0)
            {
                /*  Read the face name.
                */
                for (ind=0; ind < sizeof(lpEDrec->name) &&
                    _lread(hFile,&lpEDrec->name[ind],1) == 1 &&
                    lpEDrec->name[ind] != '\0'; ++ind)
                    ;
  
                if (ind > 0 && ind < sizeof(lpEDrec->name))
                {
                    success = TRUE;
                }
            }
        }
  
        _lclose(hFile);
    }
  
    return (success);
}
  
/*  writeNameFamily
*
*  Write the new face name and family to the PFM file.
*/
LOCAL BOOL writeNameFamily(lpBuf, indSFfile, chngDesc)
LPTMPEDIT lpBuf;
int indSFfile;
BOOL chngDesc;
{
    LPEDREC lpEDrec = &lpBuf->edrec;
    int hFile;
    int err = -1;
    int ind;
  
//    if (makeSFdirFileNm(0L,indSFfile,TRUE,lpBuf->buf,sizeof(lpBuf->buf)) &&
//        (hFile=OpenFile(lpBuf->buf,&lpBuf->ofstruct,OF_READWRITE)) > 0)
    if (makeSFdirFileNm(0L,indSFfile,TRUE,lpBuf->buf,sizeof(lpBuf->buf)) &&
        (hFile=OpenFile(lpBuf->buf,&lpBuf->ofstruct,OF_READWRITE)) >=
 0)
    {
        if (_lread(hFile,lpBuf->buf,sizeof(PFMHEADER)) == sizeof(PFMHEADER))
        {
            LPPFMHEADER lpPFMhead = (LPPFMHEADER)lpBuf->buf;
  
            err = 0;
  
            /*  Change the family value (preserve the pitch value).
            */
            lpEDrec->family |= lpPFMhead->dfPitchAndFamily & 0x0F;
            lpPFMhead->dfPitchAndFamily = (BYTE)lpEDrec->family;
  
            /*  The caller wants the description of the font (pt size,
            *  bold, italic), so read it from the PFM heaader.
            */
            if (chngDesc)
            {
                makeDesc((LPSTR)lpPFMhead, lpEDrec->desc, sizeof(lpEDrec->desc),
                lpBuf->point, lpBuf->bold, lpBuf->italic);
            }
  
            /*  Read/write the face name.
            */
            if (_llseek(hFile,lpPFMhead->dfFace,0) > 0)
            {
                LPSTR lpName = &lpBuf->buf[sizeof(PFMHEADER)];
                int namesz = sizeof(lpBuf->buf) - sizeof(PFMHEADER);
  
                /*  Read the face name.
                */
                for (ind=0; ind < namesz &&
                    _lread(hFile,&lpName[ind],1) == 1 &&
                    lpName[ind] != '\0'; ++ind)
                    ;
  
                if (ind > 0 && ind < namesz &&
                    lstrcmpi(lpEDrec->name,lpName) != 0)
                {
                    /*  We located and read the old name in the file,
                    *  it is not the same as the new name, so we'll
                    *  have to change it.
                    */
                    if (lpPFMhead->dfFace + ind == lpPFMhead->dfSize)
                    {
                        /*  Face name is already the last thing
                        *  in the file, so move to that position
                        *  and truncate the old name off.
                        */
                        _llseek(hFile,lpPFMhead->dfFace,0);
                        _lwrite(hFile,(LPSTR)NullStr,0);
                        lpPFMhead->dfSize +=
                        lstrlen(lpEDrec->name) - lstrlen(lpName);
                    }
                    else if (lstrlen(lpEDrec->name) < lstrlen(lpName))
                    {
                        /*  New face name is shorter than the old face
                        *  name, so we can just write over top of the
                        *  old name (note that the file size does not
                        *  change).
                        */
                        _llseek(hFile,lpPFMhead->dfFace,0);
                    }
                    else
                    {
                        /*  Add the face name to the end of the file,
                        *  we'll just leave the old name sitting in
                        *  the middle of the file.
                        */
                        _llseek(hFile,lpPFMhead->dfSize,0);
                        lpPFMhead->dfFace = lpPFMhead->dfSize;
                        lpPFMhead->dfSize += lstrlen(lpEDrec->name) + 1;
                    }
  
                    /*  Write the new name.
                    */
                    err = lstrlen(lpEDrec->name) + 1;
                    err -= _lwrite(hFile,lpEDrec->name,err);
                }
            }
  
            /*  Write the header (changed family, file size, and
            *  offset to face name).
            */
            _llseek(hFile, 0L, 0);
            err = _lwrite(hFile,(LPSTR)lpPFMhead,sizeof(PFMHEADER));
            err -= sizeof(PFMHEADER);
        }
  
        _lclose(hFile);
    }
  
    return (err == 0);
}
  
/*  ChangeFontStuff
*
*  Change the SF dir and the listbox entries.
*/
LOCAL void ChangeFontStuff(hDB, hMd, lpBuf, sflb, ind, idLB, hoSFlb)
HWND hDB;
HANDLE hMd;
LPTMPEDIT lpBuf;
LPSFLBENTRY sflb;
int ind;
WORD idLB;
HANDLE hoSFlb;
{
    LPSFLB lpoSFlb;
    LPEDREC lpEDrec = &lpBuf->edrec;
    LPSTR s;
    int len;
  
    //  Shift the name to the right two characters. // peterbe
    //
    len = lstrlen(lpEDrec->name) + 2;
    for (s=lpEDrec->name+len; s > lpEDrec->name; --s)
    {
        s[1] = s[-1];   // peterbe
    }
  
    /*  Add the rest of the description to the
    *  name (like "12pt italic").
    */
    lmemcpy(&lpEDrec->name[len], lpEDrec->desc, sizeof(lpEDrec->name)-len);
    lpEDrec->name[sizeof(lpEDrec->name)-1] = '\0';
  
    //  Place status ('*' or ' ' for permanent or temporary
    //  download) in the first character, and a ' ' in the second character.
    //  Follow this by a blank always.
    //
    lpEDrec->name[0] = (lpEDrec->state & SFLB_PERM) ? (char)'*' : (char)0x20;
    lpEDrec->name[1] = (char)0x20;
  
    //  Change description in SF directory. // peterbe
    //
    chngSFdirDesc(0L, sflb->indSFfile, &lpEDrec->name[2], lpBuf->buf,
    sizeof(lpBuf->buf));
  
    /*  If the id changed, move the entry in the win.ini file.
    */
    if ((lpEDrec->id != sflb->id) &&
        MoveProfileString(hMd, lpEDrec->appName, lpEDrec->id, sflb->id,
        lpBuf->buf, sizeof(lpBuf->buf)))
    {
        lpEDrec->state |= SFLB_NEWID;
    }
  
    /*  Update id and state (perm or temp).
    */
    sflb->id = lpEDrec->id;
    sflb->state = lpEDrec->state;
  
    /*  Update listbox.
    */
    SendMessage(GetDlgItem(hDB, idLB), WM_SETREDRAW, FALSE, 0L);
    SendDlgItemMessage(hDB, idLB, LB_DELETESTRING, (WORD)ind, 0L);
    SendDlgItemMessage(hDB, idLB, LB_INSERTSTRING, (WORD)ind,
    (long)(LPSTR)lpEDrec->name);
    SendMessage(GetDlgItem(hDB, idLB), WM_SETREDRAW, TRUE, 0L);
    InvalidateRect(GetDlgItem(hDB, idLB), (LPRECT)0L, FALSE);
  
    if (hoSFlb && (lpoSFlb=(LPSFLB)GlobalLock(hoSFlb)))
    {
        /*  Traverse other listbox looking for duplicate entry.
        *  If one exists, change that listbox too (note that we
        *  only change the name, we do not change the ID or the
        *  state).
        */
        int i;
        LPSFLBENTRY j;
  
        for (i=0, j=&lpoSFlb->sflb[0]; i < lpoSFlb->free; ++i, ++j)
        {
            if (j->indSFfile == sflb->indSFfile)
                break;
        }
  
        if (i < lpoSFlb->free)
        {
            /*  Change state to satisfy state of other listbox.
            */
            lpEDrec->name[0] = (j->state & SFLB_PERM) ?
            (char)'*' : (char)0x20;
  
            /*  Select other listbox.
            */
            idLB = (idLB == SF_LB_LEFT) ? SF_LB_RIGHT : SF_LB_LEFT;
  
            /*  Update other listbox (note that the other dialog is
            *  disabled for the whole display process, so we do not
            *  surround the calls with WM_SETREDRAW messages).
            */
            SendDlgItemMessage(hDB, idLB, LB_DELETESTRING, (WORD)i, 0L);
            SendDlgItemMessage(hDB, idLB, LB_INSERTSTRING, (WORD)i,
            (long)(LPSTR)lpEDrec->name);
        }
  
        GlobalUnlock(hoSFlb);
    }
  
    //  Restore the name.
  
    lpEDrec->name[len] = '\0';
    lstrcpy(lpEDrec->name, &lpEDrec->name[2]);  // peterbe
}
  
/*  ShowFace
*/
LOCAL BOOL ShowFace(hDB, lpEDrec)
HWND hDB;
LPEDREC lpEDrec;
{
    int k;
  
    /*  Pick up name out of edit field.
    */
    GetDlgItemText(hDB,SFED_NAME,&lpEDrec->name[2],sizeof(lpEDrec->name)-1);
  
    /*  Precede by '*' for permanent or ' ' for temporary download.
    */
    lpEDrec->name[0] = (lpEDrec->state == SFED_PERM) ? (char)'*' : (char)0x20;
  
    // second character is a blank
  
    lpEDrec->name[1] = (char)0x20;
  
    /*  Concatenate point size, bold, and italic.
    */
    k = lstrlen(lpEDrec->name);
    lmemcpy(&lpEDrec->name[k], lpEDrec->desc, sizeof(lpEDrec->name)-k);
    lpEDrec->name[sizeof(lpEDrec->name)-2] = '\0';
  
    /*  Update dialog display.
    */
    SetDlgItemText(hDB, SFED_DESC, lpEDrec->name);
  
    //  Revert to just the name in the name field.
    //  (remove "* " or "  ")
    lpEDrec->name[k] = '\0';
    lstrcpy(lpEDrec->name, &lpEDrec->name[2]);  // peterbe
  
    return (lpEDrec->name[0] != '\0');
}
  
/*  Verify that the ID is in a valid range.
*/
LOCAL BOOL checkID(hDB, hMd, lpEDrec)
HWND hDB;
HANDLE hMd;
LPEDREC lpEDrec;
{
    LPSFLB lpSFlb = 0L;
    LPSFLBENTRY sflb = 0L;
    FARPROC lpDlgFunc;
    BOOL valid = TRUE;
    int response;
    int oldid;
    int ind;
  
    if (lpEDrec->normalEdit)
    {
        if (lpEDrec->id < 1)
        {
            /*  Invalid ID value, report an err (note that we use
            *  the space at the end of the desc field as a buffer).
            */
            int k = lstrlen(lpEDrec->desc) + 1;
            LPSTR lpBuf = &lpEDrec->desc[k];
            int bufsz = sizeof(lpEDrec->desc) - k;
  
            if (LoadString(hMd,SFED_BADIDMSG,lpBuf,bufsz) &&
                (k = lstrlen(lpBuf) + 1) &&
                LoadString(hMd,SFED_BADIDCAP,&lpBuf[k],bufsz-k))
            {
                MessageBox(hDB,&lpBuf[k],lpBuf,MB_OK | MB_ICONEXCLAMATION);
            }
            return FALSE;
        }
  
        if (lpEDrec->hSFlb && (lpSFlb=(LPSFLB)GlobalLock(lpEDrec->hSFlb)))
        {
            for (ind=0, sflb=&lpSFlb->sflb[0]; ind < lpSFlb->free;
                ++ind, ++sflb)
            {
                if (ind != lpEDrec->ind && sflb->id == lpEDrec->id)
                    break;
            }
  
            if (ind < lpSFlb->free)
            {
                /*  Duplicate ID found.
                */
                valid = FALSE;
                lpEDrec->indSFfile = sflb->indSFfile;
                response = MyDialogBox(hMd,DUPSFID,hDB,DupIdDlgFn);
  
                if ((response == IDOK) &&
                    (sflb->id=getUniqueID(lpEDrec->hSFlb)) &&
                    MoveProfileString(lpEDrec->hMd,lpEDrec->appName,
                    sflb->id,lpEDrec->id,0L,0))
                {
                    sflb->state |= SFLB_NEWID;
                    valid = TRUE;
                }
            }
  
            GlobalUnlock(lpEDrec->hSFlb);
        }
    }
  
    return (valid);
}
  
/*  Verify that the face name exists.
*/
LOCAL BOOL checkFace(hDB, hMd, lpEDrec)
HWND hDB;
HANDLE hMd;
LPEDREC lpEDrec;
{
    if (lpEDrec->name[0] == '\0')
    {
        /*  No font name, report an err (nmote that we use the name
        *  field as a buffer).
        */
        LPSTR lpBuf = lpEDrec->name;
        int bufsz = sizeof(lpEDrec->name);
        int k;
  
        if (LoadString(hMd,SFED_BADNMMSG,lpBuf,bufsz) &&
            (k = lstrlen(lpBuf) + 1) &&
            LoadString(hMd,SFED_BADNMCAP,&lpBuf[k],bufsz-k))
        {
            MessageBox(hDB,&lpBuf[k],lpBuf,MB_OK | MB_ICONEXCLAMATION);
        }
        lpEDrec->name[0] = '\0';
  
        return FALSE;
    }
  
    return TRUE;
}
  
/*  MoveProfileString
*/
LOCAL BOOL MoveProfileString(hMd, lpAppNm, newID, oldID, lpBuf, bufsz)
HANDLE hMd;
LPSTR lpAppNm;
int newID;
int oldID;
LPSTR lpBuf;
int bufsz;
{
    LPSTR sfnew;
    LPSTR sfold;
    HANDLE hBuf = 0;
    BOOL success = FALSE;
    int sflen;
    int ind;
    int id;
    int k;
  
    /*  Sanity check.
    */
    if (newID < 1 || oldID < 1)
        return FALSE;
  
    /*  No change, simulate success.
    */
    if (newID == oldID)
        return TRUE;
  
    /*  If no work buffer was passed in, create one.
    */
    if (!lpBuf)
    {
        if (hBuf=GlobalAlloc(GMEM_MOVEABLE,(DWORD)(bufsz=EDBUFSZ)))
        {
            if (!(lpBuf=GlobalLock(hBuf)))
            {
                GlobalFree(hBuf);
                hBuf = 0;
                return FALSE;
            }
        }
        else
            return FALSE;
    }
  
    /*  Create two "SoftFontn=" strings: one for the new ID, and
    *  one for the old ID.  Leave lpBuf pointing to the remaining
    *  space after the two strings.
    */
    for (ind=0; ind < 2; ++ind)
    {
        switch (ind)
        {
            case 0:
                sfnew = lpBuf;
                id = newID;
                sflen = LoadString(hMd, SF_SOFTFONT, lpBuf, bufsz);
                break;
            case 1:
                if (sflen > bufsz)
                    goto backout;
                sfold = lpBuf;
                id = oldID;
                lmemcpy(sfold, sfnew, sflen);
                sfold[sflen] = '\0';
                break;
        }
  
        if ((k=lstrlen(lpBuf)) > bufsz - 4)
            goto backout;
  
        k += itoa(id, &lpBuf[k]) + 1;
        lpBuf += k;
        bufsz -= k;
    }
  
    DBMSG(("MoveProfileString(): sfnew=%ls, sfold=%ls\n", sfnew, sfold));
  
    *lpBuf = '\0';
  
    if (GetProfileString(lpAppNm, sfold, lpBuf, lpBuf, bufsz) &&
        (*lpBuf) &&
        WriteProfileString(lpAppNm, sfnew, lpBuf) &&
        //WriteProfileString(lpAppNm, sfold, (LPSTR)NullStr))
        WriteProfileString(lpAppNm, sfold, (LPSTR)NULL))
    {
        success = TRUE;
    }
  
    backout:
    if (hBuf)
    {
        GlobalUnlock(hBuf);
        lpBuf = 0L;
        GlobalFree(hBuf);
        hBuf = 0;
    }
  
    return (success);
}
