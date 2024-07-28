/**[f******************************************************************
* sfutils2.c -
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

/******************************   sfutils2.c   *****************************/
/*
*  SFUtils2:  Soft Font Dialog utilities
*
*  29 nov 89   peterbe Changed MB_ICONQUESTION to MB_ICONEXCLAMATION
*  04 nov 89   peterbe Changed MAKEINTRESOURCE() macro to (int) cast in
*          two MyDialogBox() calls. (eliminates confusingf
*          compiler warning)
*   1-26-89    jimmat  Adjustments do to resource file changes.
*   1-30-89    jimmat  Remove access to WIN.INI for fsvers variable--it may
*          be kept by the printer driver, not the font installer.
*   1-31-89    jimmat  getUniqueID shouldn't be returning 0 for the first id.
*/
/***************************************************************************/
  
//#define DEBUG
  
#include "nocrap.h"
#undef NOMEMMGR
#undef NOGDI
#undef NOCTLMGR
#undef NOWINMESSAGES
#undef NOMB
#include "windows.h"
#include "neededh.h"
#include "resource.h"
#include "dlgutils.h"
#include "sflb.h"
#define NOBLDDESCSTR
#include "sfutils.h"
#include "sfinstal.h"
#include "strings.h"
#include "pfm.h"
  
#define LOCAL static
  
  
// forward
int FAR PASCAL duDlgFn(HWND, unsigned, WORD, long);
  
LOCAL gDUpcnt = 0;
  
/**************************************************************************/
/****************************   Global Procs   ****************************/
  
  
/*  getUniqueID
*
*  Scan the listbox entries and determine a unique id number.
*/
int FAR PASCAL getUniqueID(hSFlb)
HANDLE hSFlb;
{
    LPSFLB lpSFlb;
    LPSFLBENTRY sflb;
    BOOL dupID;
    int ind;
    int id = -1;
    int ch;
  
    if (!hSFlb)
        return (1);
  
    if (lpSFlb=(LPSFLB)GlobalLock(hSFlb))
    {
        id = 1;
  
        do {
            dupID = FALSE;
  
            for (ind=0, sflb=&lpSFlb->sflb[0];
                ind < lpSFlb->free; ++ind, ++sflb)
            {
                if (id == sflb->id || id == -sflb->id-100 )
                {
                    dupID = TRUE;
                    ++id;
                }
            }
        } while (dupID);
  
        GlobalUnlock(hSFlb);
    }
  
    return (id);
}
  
/*  NewFS
*
*  Increment the version number in the win.ini file to indicate
*  that the fontSummary has changed.
*/
void FAR PASCAL NewFS(hMd, lpAppNm)
HANDLE hMd;
LPSTR lpAppNm;
{
    if (++gFSvers > 7200)
        gFSvers = 1;
  
    gSF_FLAGS |= SF_CHANGES;
}
  
  
/*  trimLBcaption
*
*  Make sure the listbox caption does not exceed the width of
*  the listbox.
*/
LPSTR FAR PASCAL trimLBcaption(lpCaption)
LPSTR lpCaption;
{
    if (lstrlen(lpCaption) > SFLB_CAPTIONWID)
    {
        lpCaption[SFLB_CAPTIONWID-2] = '.';
        lpCaption[SFLB_CAPTIONWID-1] = '.';
        lpCaption[SFLB_CAPTIONWID] = '\0';
    }
  
    return (lpCaption);
}
  
/*  makeDesc
*/
void FAR PASCAL makeDesc(lpBuf, lpDesc, descsz, lpPt, lpBold, lpItalic)
LPSTR lpBuf;
LPSTR lpDesc;
int descsz;
LPSTR lpPt;
LPSTR lpBold;
LPSTR lpItalic;
{
    LPPFMHEADER lpPFMhead = (LPPFMHEADER)lpBuf;
    long scaled;
    int n;
  
    lpDesc[0] = '\0';
  
    if (lpPFMhead->dfPixHeight)
    {
        /*  Put in point size. */
        if (descsz > 8)
        {
            /*  Put in point size (in points) flush right inside a
            *  field of three characters.
            */
            scaled = lpPFMhead->dfPixHeight - lpPFMhead->dfInternalLeading;
            scaled = ldiv((lmul(scaled,(long)72)+150),(long)300);
            lpDesc[0] = ' ';
            if ((n=itoa((int)scaled,&lpDesc[1])) < 3)
            {
                lpDesc[4] = '\0';
                lpDesc[3] = lpDesc[n];
                lpDesc[2] = (n == 2) ? lpDesc[1] : ' ';
                lpDesc[1] = ' ';
            }
        }
  
        /*  Concatenate "point." */
        if (lstrlen(lpDesc) < descsz - lstrlen(lpPt))
        {
            lstrcat(lpDesc, lpPt);
        }
    }
  
    /*  Concatenate "bold" if bold font.
    */
    if ((lpPFMhead->dfWeight > FW_NORMAL) &&
        (lstrlen(lpDesc) < descsz - lstrlen(lpBold)))
    {
        lstrcat(lpDesc, lpBold);
    }
  
    /*  Concatenate "italic" if italic font.
    */
    if (lpPFMhead->dfItalic &&
        (lstrlen(lpDesc) < descsz - lstrlen(lpItalic)))
    {
        lstrcat(lpDesc, lpItalic);
    }
}
  
/*  CanReplace
*
*  This proc is called during add and copy of fonts.
*
*  Query the user to see if it is okay to replace duplicate entries.
*  *lpCanReplace is -1 if we've never asked the user, 0 if the user
*  said no, and 1 if the user said yes.
*/
BOOL FAR PASCAL CanReplace(hDB, hMd, lpCanReplace, lpBuf, bufsz)
HWND hDB;
HANDLE hMd;
int FAR *lpCanReplace;
LPSTR lpBuf;
int bufsz;
{
    int k;
  
    if (*lpCanReplace < 0)
    {
        LoadString(hMd, SFINSTAL_NM, lpBuf, bufsz);
        k = lstrlen(lpBuf) + 1;
        LoadString(hMd, SFADD_REPDUP, &lpBuf[k], bufsz-k);
  
        if (MessageBox(hDB, &lpBuf[k], lpBuf,
            MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
        {
            *lpCanReplace = 1;
        }
        else
        {
            *lpCanReplace = 0;
        }
    }
  
    return (*lpCanReplace == 1);
}
  
/*  PermFontAlert
*/
void FAR PASCAL PermFontAlert(hDB, hMd)
HWND hDB;
HANDLE hMd;
{
    int response;
  
    response = MyDialogBox(hMd,SFPERMALERT,hDB,GenericWndProc);
  
    if (response == IDHELP)
    {
        MyDialogBox(hMd,SFPERMHELP, hDB, GenericWndProc);
    }
}
  
/*  MaxFontAlert
*/
void FAR PASCAL MaxFontAlert(hDB, hMd)
HWND hDB;
HANDLE hMd;
{
    MyDialogBox(hMd,(int)(SFMAXFALERT), hDB, GenericWndProc);
}
  
  
/*  MaxDirFAlert
*/
void FAR PASCAL MaxDirFAlert(hDB, hMd)
HWND hDB;
HANDLE hMd;
{
    MyDialogBox(hMd,(int)(MAXDIRF), hDB, GenericWndProc);
}
  
  
/*  alertDU
*/
BOOL FAR PASCAL alertDU(hDB, hMd, DUpcnt)
HWND hDB;
HANDLE hMd;
WORD DUpcnt;
{
    int response;
  
    gDUpcnt = DUpcnt;
  
    response = MyDialogBox(hMd,(int)(SFDUALERT), hDB, duDlgFn);
  
    return (response == IDOK);
}
  
/*  duDlgFn
*/
int FAR PASCAL duDlgFn(hDB, wMsg, wParam, lParam)
HWND hDB;
unsigned wMsg;
WORD wParam;
long lParam;
{
    switch (wMsg)
    {
        case WM_INITDIALOG:
            CenterDlg(hDB);
            SetDlgItemInt(hDB, SFDU_PCNT, gDUpcnt, FALSE);
            break;
  
        case WM_COMMAND:
        switch (wParam)
        {
            case IDOK:
            case IDCANCEL:
                EndDialog(hDB, wParam);
                break;
        }
            break;
  
        default:
            return FALSE;
    }
  
    return TRUE;
}
