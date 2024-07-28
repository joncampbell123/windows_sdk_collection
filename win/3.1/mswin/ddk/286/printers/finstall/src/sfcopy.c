/**[f******************************************************************
* sfcopy.c -
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

/*******************************   sfcopy.c   ******************************/
/*
*  SFcopy:  Module for copying files across ports.
*
*  25 oct 91   RK(HP) Commented out "Didn't redraw" and "Done copying" strings.
*              Bug 718
*  08 oct 89   peterbe Add hourglass cursor, keep redraw off during copy.
*          Don't call EnableWindow(.... FALSE) any more.
*  29 sep 89   peterbe Added comments.
*  07 aug 89   peterbe Changed all lstrcmp() to lstrcmpi().
*  27 jul 89   peterbe Use NULL as parameter when deleting font line.
*  04 may 89   peterbe Fixed listbox problem in .RC file!
*          Still need to fix redrawing of controls under
*          right listbox when SFCOPYFONT is dragged and
*          OK is clicked... FIX IS IN SFINSTAL.C!
*  02 may 89   peterbe Fixing listbox redraw problems at init.
*  01 may 89   peterbe Changed SFCPY_TARGPORT back to simple listbox.
*  22 mar 89   peterbe Initialized edit-control field of SFCPY_TARGPORT.
*  21 mar 89   peterbe Changed tabs to 8 spaces.
*          Making listbox SFCPY_TARGPORT a COMBOBOX control.
*   1-26-89    jimmat  Adjustments for changes in resource file.
*   2-20-89    jimmat  Font Installer/Driver use same WIN.INI section (again)!
*/
/***************************************************************************/
  
  
//#define DEBUG
  
#include "nocrap.h"
#undef NOCTLMGR
#undef NOWINMESSAGES
#undef NOMEMMGR
#undef NOMB
#undef NOSCROLL
#undef NOMSG
#undef NOPOINT
#include "windows.h"
#include "neededh.h"
#include "resource.h"
#include "sfcopy.h"
#include "strings.h"
#include "dlgutils.h"
#include "sfdir.h"
#include "sflb.h"
#define NOBLDDESCSTR
#include "sfutils.h"
#include "sfinstal.h"
  
  
/****************************************************************************\
* Debug Definitions
\****************************************************************************/
  
#ifdef DEBUG
   #define DBGdlgfn(msg)        /*DBMSG(msg)*/
#else
   #define DBGdlgfn(msg)        /*null*/
#endif
  
  
#define LOCAL static
  
  
#define MAX_PORTS 64
  
typedef struct {
    char buf[1024];     /* Buffer for list of port names */
    char AppPort[32];       /* Application name for win.ini */
    int ind[MAX_PORTS];     /* Array of indices to port names */
    int num;            /* Number of ports */
    int selected;       /* Index to selected port */
} PORTREC;
typedef PORTREC FAR *LPPORTREC;
  
  
typedef struct {
    char buf[256];      /* Work buffer */
    char appSrcName[64];    /* Source App name for win.ini */
    char appDstName[64];    /* Dest App name for win.ini */
    char key[48];       /* SoftFontn= line for win.ini */
    char sfstr[32];     /* SoftFontn= constant for win.ini */
    char cartstr[32];       /* CArtridgen= constant for win.ini */
    char moving[32];        /* "Moving: " for status line */
    int canReplace;     /* ==1 if can replace existing files */
    int fontCount;      /* Number of fonts in dst listbox */
} COPYREC;
typedef COPYREC FAR *LPCOPYREC;
  
  
// forward
int FAR PASCAL CopyDlgFn(HWND, unsigned, WORD, LONG);
LOCAL int SetUpPorts(LPPORTREC, LPSTR);
  
  
LOCAL LPPORTREC gLPP = 0L;
  
/**************************************************************************/
/****************************   Global Procs   ****************************/
  
  
/*  GetPort
*/
BOOL FAR PASCAL GetPort(hDB, hMd, lpRefPortNm, lpPortNm, portsz)
HWND hDB;
HANDLE hMd;
LPSTR lpRefPortNm;
LPSTR lpPortNm;
int portsz;
{
    FARPROC lpDlgFunc;
    HANDLE hPorts;
    int response = IDCANCEL;
  
    gLPP = 0L;
    lmemset(lpPortNm, 0, portsz);
  
    if ((hPorts=GlobalAlloc(GMEM_MOVEABLE, (DWORD)sizeof(PORTREC))) &&
        (gLPP=(LPPORTREC)GlobalLock(hPorts)) &&
        LoadString(hMd,SF_APPPORTS,gLPP->AppPort,sizeof(gLPP->AppPort)))
    {
        gLPP->selected = -1;
  
        if (SetUpPorts(gLPP,lpRefPortNm))
        {
            response = MyDialogBox(hMd, SFCOPYFONT, hDB, CopyDlgFn);
        }
        else
        {
            /*  No ports, report an err.
            */
            int k;
  
            if (LoadString(hMd,SFCPY_NPCAP,gLPP->buf,sizeof(gLPP->buf)) &&
                (k=lstrlen(gLPP->buf)+1) &&
                LoadString(hMd,SFCPY_NPMSG,&gLPP->buf[k],sizeof(gLPP->buf)-k))
            {
                MessageBox(hDB,
                &gLPP->buf[k],gLPP->buf,MB_OK | MB_ICONEXCLAMATION);
            }
        }
  
        if (gLPP->selected > -1 && response == IDOK)
        {
            lmemcpy(lpPortNm, &gLPP->buf[gLPP->selected], portsz);
            lpPortNm[portsz-1] ='\0';
        }
    }
  
    if (gLPP)
    {
        GlobalUnlock(hPorts);
        gLPP = 0L;
    }
  
    if (hPorts)
    {
        GlobalFree(hPorts);
        hPorts = 0;
    }
  
    return (response == IDOK);
}
  
/*  CopyFonts
*/
HANDLE FAR PASCAL CopyFonts(hDB, hMd, iddstLB, hdstSFlb, lpdstPortNm,
idsrcLB, hsrcSFlb, lpsrcPortNm, ersSrc, lpCount, lpModNm)
HWND hDB;
HANDLE hMd;
WORD iddstLB;
HANDLE hdstSFlb;
LPSTR lpdstPortNm;
WORD idsrcLB;
HANDLE hsrcSFlb;
LPSTR lpsrcPortNm;
BOOL ersSrc;
WORD FAR *lpCount;
LPSTR lpModNm;
{
    MSG msg;
    LPSFLB lpsrcSFlb = 0L;
    LPSFLBENTRY sflb = 0L;
    LPSFLBENTRY j, k;
    LPSFDIRFILE lpSFfile = 0L;
    LPCOPYREC lpBuf = 0L;
    HANDLE hBuf = 0;
    LPSTR lpsrcAppNm;
    LPSTR lpdstAppNm;
    BOOL existDL;
    WORD prevPos = 0;
    int ind, i;
    int id;
  
    *lpCount = 0;
  
    if (hsrcSFlb &&
        (hBuf=GlobalAlloc(GMEM_MOVEABLE, (DWORD)sizeof(COPYREC))) &&
        (lpBuf=(LPCOPYREC)GlobalLock(hBuf)) &&
        LoadString(hMd, SF_SOFTFONT, lpBuf->sfstr, sizeof(lpBuf->sfstr)) &&
        LoadString(hMd, SF_CARTRIDGE, lpBuf->cartstr,sizeof(lpBuf->cartstr)) &&
        LoadString(hMd, (ersSrc ? SFCPY_MOVING : SFCPY_CPYING),
        lpBuf->moving, sizeof(lpBuf->moving)) &&
        (lpsrcSFlb=(LPSFLB)GlobalLock(hsrcSFlb)))
    {
        lpsrcAppNm = lpBuf->appSrcName;
        lpdstAppNm = lpBuf->appDstName;
  
        /*  Build "[<driver>,<port>]" for accessing win.ini file.
        */
        MakeAppName(lpModNm,lpsrcPortNm,lpsrcAppNm,sizeof(lpBuf->appSrcName));
        MakeAppName(lpModNm,lpdstPortNm,lpdstAppNm,sizeof(lpBuf->appDstName));
  
        /*  canReplace is -1 if the user has not been asked, 0
        *  if the user said: DO NOT replace existing files with
        *  the same name, and 1 if the user said: DO replace
        *  existing files with the same name.
        */
        lpBuf->canReplace = -1;
        lpBuf->fontCount = 0;
  
        //  Disable redrawing in both listboxes.
        SendMessage(GetDlgItem(hDB,idsrcLB), WM_SETREDRAW, FALSE, 0L);
        SendMessage(GetDlgItem(hDB,iddstLB), WM_SETREDRAW, FALSE, 0L);
        SetCursor(LoadCursor(NULL,IDC_WAIT));
  
        // Deselect all items.
        SendDlgItemMessage(hDB, idsrcLB, LB_SETSEL, FALSE, (long)(-1));
  
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
                    SendDlgItemMessage(hDB, iddstLB, LB_RESETCONTENT, 0, 0L);
                }
                GlobalUnlock(hdstSFlb);
            }
        }
  
        ///EnableWindow(GetDlgItem(hDB, idsrcLB), FALSE);
        ///EnableWindow(GetDlgItem(hDB, iddstLB), FALSE);
  
        SendMessage(GetDlgItem(hDB,idsrcLB), WM_VSCROLL, SB_TOP, 0L);
        SendMessage(GetDlgItem(hDB,iddstLB), WM_VSCROLL, SB_TOP, 0L);
  
        // redraw the listboxes.
    #ifdef REDRAWWHILECOPYING   // xxx
        SendMessage(GetDlgItem(hDB,idsrcLB), WM_SETREDRAW, TRUE, 0L);
        SendMessage(GetDlgItem(hDB,iddstLB), WM_SETREDRAW, TRUE, 0L);
        InvalidateRect(GetDlgItem(hDB,idsrcLB), (LPRECT)0L, FALSE);
        InvalidateRect(GetDlgItem(hDB,iddstLB), (LPRECT)0L, FALSE);
    #endif
  
//        SetDlgItemText(hDB, SF_STATUS, (LPSTR)"Didn't redraw"); // xxx
  
        EnableWindow(GetDlgItem(hDB, SF_MOVE), FALSE);
        EnableWindow(GetDlgItem(hDB, SF_COPY), FALSE);
        EnableWindow(GetDlgItem(hDB, SF_ERASE), FALSE);
        EnableWindow(GetDlgItem(hDB, SF_EDIT), FALSE);
        CheckRadioButton(hDB, SF_PERM_LEFT, SF_TEMP_LEFT, 0);
        EnableWindow(GetDlgItem(hDB, SF_PERM_LEFT), FALSE);
        EnableWindow(GetDlgItem(hDB, SF_TEMP_LEFT), FALSE);
        CheckRadioButton(hDB, SF_PERM_RIGHT, SF_TEMP_RIGHT, 0);
        EnableWindow(GetDlgItem(hDB, SF_PERM_RIGHT), FALSE);
        EnableWindow(GetDlgItem(hDB, SF_TEMP_RIGHT), FALSE);
  
        /*  Change exit button to Cancel.
        */
        if (LoadString(hMd,SF_CNCLSTR,lpBuf->buf,sizeof(lpBuf->buf)))
        {
            SetDlgItemText(hDB, SF_EXIT, lpBuf->buf);
            gSF_FLAGS |= SF_NOABORT;
        }
  
        /*  For each listbox item.
        */
        for (ind=0, sflb=&lpsrcSFlb->sflb[0];
            ind < lpsrcSFlb->free && (gSF_FLAGS & SF_NOABORT); )
        {
            // make sure the cursor STAYS as an hourglass.
            SetCursor(LoadCursor(NULL,IDC_WAIT));
  
            /* We don't copy screen fonts twixt ports */
            if (sflb->state & SFLB_FAIS)
                sflb->state &= ~(SFLB_SEL);
  
            /*  If selected, attempt to copy/move to the destination
            *  listbox.
            */
            if ((sflb->state & SFLB_SEL) &&
                (lpSFfile=(LPSFDIRFILE)lockSFdirEntry(0L,sflb->indSFfile)))
            {
                lstrcpy(lpBuf->buf, lpBuf->moving);
                i = lstrlen(lpBuf->buf);
                lmemcpy(&lpBuf->buf[i], lpSFfile->s, sizeof(lpBuf->buf)-i);
                lpBuf->buf[sizeof(lpBuf->buf)-1] = '\0';
                SetDlgItemText(hDB, SF_STATUS, lpBuf->buf);
  
                existDL = lpSFfile->offsDLname;
  
                unlockSFdirEntry(sflb->indSFfile);
  
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
                    (sflb->id<0 ? SFLB_PERM|SFLB_CART :
                    (existDL ? 0 : SFLB_PERM)),
                    lpBuf->buf, sizeof(lpBuf->buf));
                }
                else
                {
                    int state;
  
                    /* find the ID and status of the entry */
                    if (sflb->id<0)
                    {
                        id = -getUniqueID(hdstSFlb)-100;
                        state = SFLB_PERM|SFLB_CART;
                    }
                    else
                    {
                        id = getUniqueID(hdstSFlb);
                        state = (existDL ? 0 : SFLB_PERM);
                    }
  
                    /*  Append the SF dir entry to the destination
                    *  listbox struct.
                    */
                    hdstSFlb = addSFlistbox(hDB, hdstSFlb, iddstLB,
                    id, sflb->indSFfile, state,
                    lpBuf->buf, sizeof(lpBuf->buf), &prevPos);
                }
  
                /*  Build the string to put in the win.ini for the
                *  destination printer.
                */
                if (!makeSFdirFileNm(
                    0L,sflb->indSFfile,TRUE,lpBuf->buf,sizeof(lpBuf->buf)))
                {
                    /*  Failed to make PFM file name, cannot continue.
                    */
                    ++ind;
                    ++sflb;
                    continue;
                }
  
                if (existDL)
                {
                    /*  Concatenate comma and download file name if there,
                    *  otherwise just terminate after the PFM name.
                    *  Assumes existDL == 0 for PCMs.
                    */
                    if ((i=lstrlen(lpBuf->buf)) < sizeof(lpBuf->buf)-2)
                    {
                        lstrcat(lpBuf->buf, (LPSTR)",");
                        ++i;
                    }
  
                    if (!makeSFdirFileNm(0L,
                        sflb->indSFfile,FALSE,&lpBuf->
                        buf[i],sizeof(lpBuf->buf)-i))
                    {
                        lpBuf->buf[--i] = '\0';
                    }
                }
  
                /*  Write destination profile string.
                */
                if (id<0)
                {
                    lstrcpy(lpBuf->key, lpBuf->cartstr);
                    itoa(-id, &lpBuf->key[lstrlen(lpBuf->key)]);
                }
                else
                {
                    lstrcpy(lpBuf->key, lpBuf->sfstr);
                    itoa(id, &lpBuf->key[lstrlen(lpBuf->key)]);
                }
                WriteProfileString(lpdstAppNm, lpBuf->key, lpBuf->buf);
  
                /*  Increment count of added fonts.
                */
                ++(*lpCount);
                ++lpBuf->fontCount;
  
                if (ersSrc)
                {
                    /*  We're "moving" fonts so remove the font from
                    *  the source listbox.
                    */
                    SendDlgItemMessage(
                    hDB,idsrcLB,LB_DELETESTRING,(WORD)ind,0L);
  
                    if (sflb->id < 0)
                    {
                        lstrcpy(lpBuf->key, lpBuf->cartstr);
                        itoa(-sflb->id, &lpBuf->key[lstrlen(lpBuf->key)]);
                    }
                    else
                    {
                        lstrcpy(lpBuf->key, lpBuf->sfstr);
                        itoa(sflb->id, &lpBuf->key[lstrlen(lpBuf->key)]);
                    }
                    //Remove the line from WIN.INI.
                    WriteProfileString(lpsrcAppNm, lpBuf->key, (LPSTR)NULL);
  
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
                    /*  We're "copying" fonts so add another owner to
                    *  to the listbox item.
                    */
                    addSFdirOwner(0L, sflb->indSFfile);
  
                    /*  Increment counts and scroll source listbox.
                    */
                    ++ind;
                    ++sflb;
                    SendMessage(
                    GetDlgItem(hDB,idsrcLB), WM_VSCROLL, SB_LINEDOWN, 0L);
                    // disable redraw
                    SendMessage(GetDlgItem(hDB,idsrcLB), WM_SETREDRAW, FALSE,
                    0L);
                }
            }
            else
            {
                ++ind;
                ++sflb;
  
                /*  Scroll source listbox.
                */
                SendMessage(
                GetDlgItem(hDB,idsrcLB), WM_VSCROLL, SB_LINEDOWN, 0L);
                // disable redraw
                SendMessage(GetDlgItem(hDB,idsrcLB), WM_SETREDRAW, FALSE, 0L);
            }
  
            /*  Process any messages to the installer's dialog box
            *  so we can detect the cancel button.
            */
            while (PeekMessage(&msg, hDB, NULL, NULL, TRUE) &&
                IsDialogMessage(hDB, &msg))
                ;
        }
  
        DBGdumpSFbuf(0L);
  
        /*  Enable listboxes.
        */
        SendMessage(GetDlgItem(hDB,idsrcLB), WM_SETREDRAW, FALSE, 0L);
        SendMessage(GetDlgItem(hDB,iddstLB), WM_SETREDRAW, FALSE, 0L);
  
        EnableWindow(GetDlgItem(hDB,idsrcLB), TRUE);
        EnableWindow(GetDlgItem(hDB,iddstLB), TRUE);
  
        SendMessage(GetDlgItem(hDB,idsrcLB), WM_VSCROLL, SB_TOP, 0L);
        SendMessage(GetDlgItem(hDB,iddstLB), WM_VSCROLL, SB_TOP, 0L);
  
        SendMessage(GetDlgItem(hDB,idsrcLB), WM_SETREDRAW, TRUE, 0L);
        SendMessage(GetDlgItem(hDB,iddstLB), WM_SETREDRAW, TRUE, 0L);
  
        InvalidateRect(GetDlgItem(hDB,idsrcLB), (LPRECT)0L, FALSE);
        InvalidateRect(GetDlgItem(hDB,iddstLB), (LPRECT)0L, FALSE);
  
        // restore pointer cursor
        SetCursor(LoadCursor(NULL,IDC_ARROW));
  
        /*  Restore exit button.
        */
        if (LoadString(hMd,SF_EXITSTR,lpBuf->buf,sizeof(lpBuf->buf)))
        {
            SetDlgItemText(hDB, SF_EXIT, lpBuf->buf);
            gSF_FLAGS &= ~(SF_NOABORT);
        }
  
//        SetDlgItemText(hDB, SF_STATUS, (LPSTR)"Done copying");  // xxx
  
        /*  Flag new fontSummary in win.ini file.
        */
        NewFS(hMd, lpdstAppNm);
  
        if (ersSrc)
            NewFS(hMd, lpsrcAppNm);
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
  
    return (hdstSFlb);
  
}   // CopyFonts()
  
/*  CopyDlgFn
*
* handles 'copy between ports ..." dialog.
*/
BOOL FAR PASCAL CopyDlgFn(hDB, wMsg, wParam, lParam)
HWND hDB;
unsigned wMsg;
WORD wParam;
LONG lParam;
{
    switch (wMsg)
    {
        case WM_INITDIALOG:
            DBGdlgfn(("CopyDlgFn(%d,%d,%d,%ld): WM_INITDIALOG\n",
            hDB, wMsg, wParam, lParam));
            CenterDlg(hDB);
        {
            int iNum;
            HWND hLB;           // handle of listbox
            RECT rectLB;        // bounding rectangle of listbox.
  
            hLB = GetDlgItem(hDB, SFCPY_TARGPORT);
  
            // clear the redraw flag
            SendMessage(hLB, WM_SETREDRAW, FALSE, 0L);
  
            // Fill in listbox with names of ports from which fonts
            // might be copied.
  
            for (iNum=0; iNum < gLPP->num; ++iNum)
            {
                // Add string to END of list.
  
                SendMessage(hLB, LB_INSERTSTRING,
                (WORD)(-1), (long)(LPSTR)&gLPP->buf[gLPP->ind[iNum]]);
            }
  
            // set the redraw flag
            SendMessage(hLB, WM_SETREDRAW, TRUE, 0L);
  
            // If there are any items in the listbox, select the first
            // one and enable the OK button.
            if (gLPP->num)  /* any selections? */
            {
                /* Select first item
                */
                SendDlgItemMessage(hDB, SFCPY_TARGPORT, LB_SETCURSEL,
                (WORD) 0, (long) 0);
  
                // enable OK button
                gLPP->selected = gLPP->ind[0];
                EnableWindow(GetDlgItem(hDB,IDOK),TRUE);
            }
            else
                // Disable OK button.  Won't ever happen, since we
                // won't get called, but...
                EnableWindow(GetDlgItem(hDB,IDOK),FALSE);
        }
  
            break;
  
        case WM_COMMAND:
        switch (wParam)
        {
            case SFCPY_TARGPORT:
                /* a message has come for the listbox.
                */
  
                DBGdlgfn(("CopyDlgFn(%d,%d,%d,%ld): SFCPY_TARGPORT\n",
                hDB, wMsg, wParam, lParam));
            {
                int iNum;
  
                if (HIWORD(lParam) == LBN_ERRSPACE)
                    EndDialog(hDB,-1);
  
                if ((iNum=(int)SendDlgItemMessage(hDB, SFCPY_TARGPORT,
                    LB_GETCURSEL, 0, 0L)) == LB_ERR)
                {
                    /* got an error, disable this control  */
                    gLPP->selected = -1;
                    EnableWindow(GetDlgItem(hDB,IDOK),FALSE);
                }
                else
                {
                    /* message is ok, save no. of selected message */
                    gLPP->selected = gLPP->ind[iNum];
                    EnableWindow(GetDlgItem(hDB,IDOK),TRUE);
                }
  
                if (HIWORD(lParam) == 2)
                {
                    DBGdlgfn((
                    "CopyDlgFn(%d,%d,%d,%ld): ...double-click: IDOK\n",
                    hDB, wMsg, wParam, lParam));
                    EndDialog(hDB, IDOK);
                }
            }
                break;
  
            case IDOK:
            case IDCANCEL:
        #ifdef DEBUG
                if (wParam == IDOK)
                { DBGdlgfn(
                    ("CopyDlgFn(%d,%d,%d,%ld): IDOK\n",
                hDB, wMsg, wParam, lParam)); }
                else
                { DBGdlgfn(
                    ("CopyDlgFn(%d,%d,%d,%ld): IDCANCEL\n",
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
}   // CopyDlgFn()
  
/**************************************************************************/
/*****************************   Local Procs   ****************************/
  
  
LOCAL int SetUpPorts(lpP, lpRefPortNm)
LPPORTREC lpP;
LPSTR lpRefPortNm;
{
    LPSTR p;
    int ind;
  
    lpP->num = 0;
    lpP->buf[0] = '\0';
  
    if (GetProfileString(lpP->AppPort,0L,lpP->buf,lpP->buf,sizeof(lpP->buf)) &&
        (lpP->buf[0] != '\0'))
    {
        /*  Roll through the list of key names setting up a pointer
        *  to each one.  Do not include the current port selection
        *  (left listbox) in the list.
        */
        for (ind=0, p=&lpP->buf[0]; *p; ++ind, ++p)
        {
            if (lstrcmpi(p,lpRefPortNm) != 0 && lpP->num < MAX_PORTS)
            {
                lpP->ind[lpP->num++] = ind;
            }
  
            for (; *p; ++ind, ++p)
                ;
        }
    }
  
    return (lpP->num);
}
