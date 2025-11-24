/*
 * ICON.H
 *
 * Internal definitions, structures, and function prototypes for the
 * OLE 2.0 UI Change Icon dialog.
 *
 * Copyright (c)1992 Microsoft Corporation, All Right Reserved
 */


#ifndef _ICON_H_
#define _ICON_H_

#define CXICONPAD                   12
#define CYICONPAD                   4

//Internally used structure
typedef struct tagCHANGEICON
    {
    LPOLEUICHANGEICON   lpOCI;      //Original structure passed.

    /*
     * What we store extra in this structure besides the original caller's
     * pointer are those fields that we need to modify during the life of
     * the dialog but that we don't want to change in the original structure
     * until the user presses OK.
     */
    DWORD               dwFlags;
    HICON               hCurIcon;
    char                szLabel[OLEUI_CCHLABELMAX+1];
    char                szFile[OLEUI_CCHPATHMAX];
    UINT                iIcon;
    HICON               hDefIcon;
    char                szDefIconFile[OLEUI_CCHPATHMAX];
    UINT                iDefIcon;
    } CHANGEICON, *PCHANGEICON, FAR *LPCHANGEICON;


//Internal function prototypes
//ICON.C
BOOL CALLBACK EXPORT ChangeIconDialogProc(HWND, UINT, WPARAM, LPARAM);
BOOL            FChangeIconInit(HWND, WPARAM, LPARAM);
UINT            UFillIconList(HWND, UINT, LPSTR);
BOOL            FDrawListIcon(LPDRAWITEMSTRUCT);
void            UpdateResultIcon(LPCHANGEICON, HWND, UINT);


#endif //_ICON_H_
