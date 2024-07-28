/**[f******************************************************************
* sflb.c -
*
* Copyright (C) 1988,1989 Aldus Corporation
* Copyright (C) 1989-1990 Microsoft Corporation.
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

/*********************************   sflb.c   ******************************/
/*
*  SFlb:  Soft Font listbox utilities
*/
/***************************************************************************/
// history
//
// 29 sep 89   peterbe     in ReplaceSFListBox(), don't do WM_SETREDRAW
//             after update -- wait for calling function
//             to do this.
// 20 sep 89   peterbe     Just added some DBG* messages.
// 07 aug 89   peterbe     Changed all lstrcmp() to lstrcmpi().
// 17 jul 89   peterbe     Check in steved's change.
// 07 jul 89   steved(hp)  Initialized fileSize in SFLBENTRY structure.
// 24 mar 89   peterbe     (1) Call AddOwnerString() now for LB_ADDSTRING
//             and LB_INSERTSTRING messages to main list
//             boxes.
//             (2) Take this out.  buildDescrString() already
//             did the right thing.
/***************************************************************************/
  
  
//#define DEBUG
  
#include "nocrap.h"
#undef NOMEMMGR
#undef NOCTLMGR
#undef NOWINMESSAGES
#undef NOSCROLL
#include "windows.h"
#include "neededh.h"
#include "sfdir.h"
#include "sflb.h"
#include "sfutils.h"
  
  
/****************************************************************************\
* Debug Definitions
\****************************************************************************/
  
#ifdef DEBUG
    #define DBGaddlb(msg)      /*DBMSG(msg)*/
    #define DBGtest(msg)       /*DBMSG(msg)*/
#else
    #define DBGaddlb(msg)      /*null*/
    #define DBGtest(msg)       /*null*/
#endif
  
  
  
  
  
#define LOCAL static
  
#define SFLB_INITLEN    100
#define SFLB_STEPLEN    50
#define SFLB_INITSZ (sizeof(SFLB) + ((SFLB_INITLEN - 1) * sizeof(SFLBENTRY)))
#define SFLB_STEPSZ (SFLB_STEPLEN * sizeof(SFLBENTRY))
  
/**************************************************************************/
/****************************   Global Procs   ****************************/
  
  
/*  dupSFlistbox
*
*  Search for a duplicate SF dir entry.
*/
int FAR PASCAL dupSFlistbox(hSFlb, sfind, lpBuf, bufsz)
HANDLE hSFlb;
int sfind;
LPSTR lpBuf;
int bufsz;
{
    LPSFLB lpSFlb;
    LPSFLBENTRY sflb;
    LPSFDIRFILE lpSFfile;
    LPSTR lpDLnm;
    LPSTR lpPFMnm;
    int ind = -1;
  
    DBGaddlb(("dupSFlistbox(%d,%d,%lp,%d)\n", hSFlb, sfind, lpBuf, bufsz));
  
    if ((sfind > -1) && hSFlb && (lpSFlb=(LPSFLB)GlobalLock(hSFlb)))
    {
        /*  Pick up the name of the download and PFM files
        *  for the SF dir entry at sfind.
        */
        if (lpSFfile=(LPSFDIRFILE)lockSFdirEntry(0L,sfind))
        {
            if (lpSFfile->offsDLname > 0)
            {
                lpDLnm = lpSFfile->s+lpSFfile->offsDLname;
                ind = lstrlen(lpDLnm)+1;
                if (ind>bufsz)
                    ind=bufsz;
                lmemcpy(lpBuf,lpDLnm,ind);
                lpBuf[bufsz-1]= 0;
                lpDLnm = lpBuf;
                lpBuf += ind;
                bufsz -= ind;
            }
            else
                lpDLnm = 0L;
  
            if (lpSFfile->offsPFMname > 0)
            {
                lpPFMnm = lpSFfile->s + lpSFfile->offsPFMname;
                ind = lstrlen(lpPFMnm)+1;
                if (ind>bufsz)
                    ind=bufsz;
                lmemcpy(lpBuf,lpPFMnm,ind);
                lpBuf[bufsz-1] = 0;
                lpPFMnm = lpBuf;
                lpBuf += ind;
                bufsz -= ind;
            }
            else
                lpPFMnm = 0L;
  
            unlockSFdirEntry(sfind);
        }
        else
        {
            lpDLnm = 0L;
            lpPFMnm = 0L;
        }
  
        /*  Traverse the listbox struct looking for an entry
        *  which matches the entry at sfind.
        */
        for (ind=0, sflb=&lpSFlb->sflb[0]; ind < lpSFlb->free;
            ++ind, ++sflb)
        {
            if (sflb->indSFfile == sfind)
            {
                /*  SF dir entry already in listbox
                */
                DBGaddlb(("dupSFlb(): dup indSFfile=%d at %d\n", sfind, ind));
                break;
            }
            else if ((lpDLnm || lpPFMnm) &&
                (lpSFfile=(LPSFDIRFILE)lockSFdirEntry(0L,sflb->indSFfile)))
            {
                if (lpDLnm && lpSFfile->offsDLname &&
                    !lstrcmpi(lpDLnm,&lpSFfile->s[lpSFfile->offsDLname]))
                {
                    DBGaddlb(("dupSFlb(): dup DL name=%ls at %d\n",
                    lpDLnm, ind));
                    unlockSFdirEntry(sflb->indSFfile);
                    break;
                }
  
                if (lpPFMnm && lpSFfile->offsPFMname &&
                    !lstrcmpi(lpPFMnm,&lpSFfile->s[lpSFfile->offsPFMname]))
                {
                    DBGaddlb(("dupSFlb(): dup PFM name=%ls at %d\n",
                    lpPFMnm, ind));
                    unlockSFdirEntry(sflb->indSFfile);
                    break;
                }
  
                unlockSFdirEntry(sflb->indSFfile);
            }
        }
  
        if (ind == lpSFlb->free)
        {
            /*  Duplicate not found, turn into -1
            *  to indicate this.
            */
            ind = -1;
        }
  
        GlobalUnlock(hSFlb);
        lpSFlb = 0L;
    }
  
    return (ind);
  
}   // dupSFlistbox()
  
/*  addSFlistbox
*/
HANDLE FAR PASCAL addSFlistbox(hDB, hSFlb, idLB, fontID, sfind, state,
lpDesc, desclen, lpPrevPos)
HWND hDB;
HANDLE hSFlb;
WORD idLB;
int fontID;
int sfind;
WORD state;
LPSTR lpDesc;
int desclen;
WORD FAR *lpPrevPos;
{
    LPSFDIRFILE lpSFfile = 0L;
    LPSFLBENTRY sflb = 0L;
    LPSFLB lpSFlb = 0L;
    BOOL initList = FALSE;
    SFLBENTRY new;
    WORD prevPos;
    int pos, ind;
  
    DBGaddlb(("addSFlistbox(%d,%d,%d,%d,%d,%d,%lp,%d)\n",
    hDB, hSFlb, idLB, fontID, sfind, state, lpDesc, desclen));
  
    /*  If we did not get a handle, then it must be a new struct.
    */
    if (!hSFlb)
    {
        initList = TRUE;
  
        if (!(hSFlb = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT,
            (DWORD)SFLB_INITSZ)))
        {
            DBGaddlb(("addSFlistbox(): failed to alloc listbox array\n"));
            goto backout0;
        }
    }
  
    /*  Lock it down.
    */
    if (!(lpSFlb = (LPSFLB)GlobalLock(hSFlb)))
    {
        DBGaddlb(("addSFlistbox(): failed to lock listbox array\n"));
        goto backout1;
    }
  
    /*  Initialize if a new struct.
    */
    if (initList)
    {
        DBGaddlb(("addSFlistbox(): **new** listbox list\n"));
        lpSFlb->len = SFLB_INITLEN;
        lpSFlb->free = 0;
        lpSFlb->prevsel = -1;
    }
  
    /*  If we've filled what space we have so far, then allocate
    *  more memory so we may extend the structure.
    */
    if (lpSFlb->free == lpSFlb->len)
    {
        HANDLE tmp;
        DWORD dsize = GlobalSize(hSFlb) + SFLB_STEPSZ;
  
  
        while ((GlobalFlags(hSFlb) & GMEM_LOCKCOUNT) > 0)
            GlobalUnlock(hSFlb);
  
  
        if (!(tmp = GlobalReAlloc(hSFlb, dsize, GMEM_MOVEABLE)))
        {
            DBGaddlb(("addSFlistbox(): failed to realloc struct\n"));
            goto backout1;
        }
  
        if (!(lpSFlb = (LPSFLB)GlobalLock(hSFlb=tmp)))
        {
            DBGaddlb(("addSFlistbox(): failed to lock realloc'd struct\n"));
            goto backout1;
        }
  
        lpSFlb->len += SFLB_STEPLEN;
    }
  
    /*  Lock down the sfdir entry for the soft font file and initialize
    *  the listbox entry, generate the description string, and place
    *  the string in the listbox.
    */
    if (lpSFfile = (LPSFDIRFILE)lockSFdirEntry(0L, sfind))
    {
        new.native = new.state = state;
        new.id = fontID;
        new.indSFfile = sfind;
        new.fileSize = 0;
  
        buildDescStr(lpDesc, desclen, TRUE, &new, lpSFfile);
  
        DBGtest(("Name returned from buildDescStr = %ls\n\n",(LPSTR)lpDesc));
  
        unlockSFdirEntry(sfind);
  
        /*  Update the listbox.
        */
        if (hDB && idLB)
        {
            // Add string to listbox.
            pos = (int)SendDlgItemMessage(hDB,idLB,LB_ADDSTRING,0,
            (long)lpDesc);
  
            if (pos < 0)
                pos = lpSFlb->free;
  
            if (lpPrevPos)
            {
                if ((prevPos=*lpPrevPos) < 0)
                    prevPos = 0;
                else if (prevPos > lpSFlb->free)
                    prevPos = lpSFlb->free;
  
                if (prevPos > pos)
                {
                    for (ind=prevPos-pos; ind > 0; --ind)
                        SendMessage(GetDlgItem(hDB,idLB),WM_VSCROLL,SB_LINEUP,
                        0L);
                }
                else
                {
                    if (prevPos < 7)
                        prevPos = 7;
  
                    for (ind=pos-prevPos; ind > 0; --ind)
                        SendMessage(GetDlgItem(hDB,idLB),WM_VSCROLL,SB_LINEDOWN,
                        0L);
                }
                *lpPrevPos = pos;
            }
            /*          else
            *              {
            *              This is too jumpy.  So lets not do it for now.
            *
            *              SendMessage(GetDlgItem(hDB,idLB),WM_VSCROLL,SB_BOTTOM,0L);
            *              }
            */
        }
        else
        {
            pos = lpSFlb->free;
        }
  
        /*  Insert the new SFLB entry in the structure at the same spot
        *  as the item in the listbox.
        */
        for (sflb=&lpSFlb->sflb[ind=lpSFlb->free]; ind > pos; --ind, --sflb)
        {
            *sflb = sflb[-1];
        }
        *sflb = new;
  
        ++lpSFlb->free;
    }
    else
    {
        DBGaddlb(("addSFlistbox(): failed to lock SF dir entry\n"));
        goto backout2;
    }
  
    GlobalUnlock(hSFlb);
    lpSFlb = 0L;
  
    return (hSFlb);
  
    backout2:
    GlobalUnlock(hSFlb);
    lpSFlb = 0L;
    backout1:
    GlobalFree(hSFlb);
    hSFlb = 0;
    backout0:
    return (0);
  
}   // addSFlistBox()
  
/*  replaceSFlistbox
*/
HANDLE FAR PASCAL replaceSFlistbox(hDB, hSFlb, idLB, indSFlb, lpID,
sfind, state, lpDesc, desclen)
HWND hDB;
HANDLE hSFlb;
WORD idLB;
int indSFlb;
int FAR *lpID;
int sfind;
WORD state;
LPSTR lpDesc;
int desclen;
{
    LPSFDIRFILE lpSFfile = 0L;
    LPSFLBENTRY sflb = 0L;
    LPSFLB lpSFlb = 0L;
  
    DBGaddlb(("replaceSFlistbox(%d,%d,%d,%d,%lp,%d,%d,%lp,%d)\n",
    hDB, hSFlb, idLB, indSFlb, lpID, sfind, state, lpDesc, desclen));
  
    if (hSFlb && (lpSFlb=(LPSFLB)GlobalLock(hSFlb)) &&
        (indSFlb < lpSFlb->free))
    {
        sflb = &lpSFlb->sflb[indSFlb];
  
        if (lpSFfile = (LPSFDIRFILE)lockSFdirEntry(0L, sfind))
        {
            /*  Blow away the SF dir entry currently occupying
            *  this listbox entry.
            */
            delSFdirEntry(0L, sflb->indSFfile);
  
            /*  Replace with the new SF dir entry.
            */
            sflb->native = sflb->state = state;
            *lpID = sflb->id;
            sflb->indSFfile = sfind;
  
            buildDescStr(lpDesc, desclen, TRUE, sflb, lpSFfile);
  
            unlockSFdirEntry(sfind);
  
            /*  Update the listbox.
            */
            if (hDB && idLB)
            {
                // I guess it's OK to disable redraw here
                SendMessage(GetDlgItem(hDB,idLB),WM_SETREDRAW,FALSE,0L);
                SendDlgItemMessage(hDB,idLB,LB_DELETESTRING,(WORD)indSFlb,0L);
  
                SendDlgItemMessage(hDB,idLB,LB_INSERTSTRING,(WORD)indSFlb,
                (long)lpDesc);
  
                SendMessage(GetDlgItem(hDB,idLB),WM_VSCROLL,SB_BOTTOM,0L);
  
                // Don't re-enable redraw yet.
                //SendMessage(GetDlgItem(hDB,idLB),WM_SETREDRAW,TRUE,0L);
                //InvalidateRect(GetDlgItem(hDB,idLB),(LPRECT)0L,FALSE);
            }
        }
  
    }
  
  
    if (lpSFlb)
    {
        GlobalUnlock(hSFlb);
        lpSFlb = 0L;
    }
  
    return (hSFlb);
  
}   // replaceSFlistbox()
