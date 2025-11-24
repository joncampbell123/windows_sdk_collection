// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993-1995  Microsoft Corporation.  All Rights Reserved.
//
//  MODULE:   stckinfo.c
//
//  PURPOSE:   Displays the "Edit Stock Information" dialog box
//
//  FUNCTIONS:
//    CmdEditStockInfo - Displays the "Edit Stock Information" dialog box
//    EditStockInfo    - Processes messages for "Edit Stock Information" 
//                       dialog box.
//    MsgESInit        - To initialize the dialogbox 
//    MsgESCommand     - Process WM_COMMAND message sent to the dialog
//    CmdESDone        - Free the dialog box and store the new data.
//
//  COMMENTS:
//
//

#include <windows.h>            // required for all Windows applications
#include <windowsx.h>
#include <commctrl.h>

#include "globals.h"            // prototypes specific to this application
#include "listview.h"
#include "resource.h"


LRESULT CALLBACK EditStockInfo(HWND, UINT, WPARAM, LPARAM);

LRESULT MsgESInit(HWND, UINT, WPARAM, LPARAM);
LRESULT MsgESCommand(HWND, UINT, WPARAM, LPARAM);
LRESULT CmdESDone(HWND, WORD, WORD, HWND);

// Dialog message table definition.
MSD rgmsdEditStockInfo[] =
{
    {WM_COMMAND,    MsgESCommand},
    {WM_INITDIALOG, MsgESInit}
};

MSDI msdiEditStockInfo =
{
    sizeof(rgmsdEditStockInfo) / sizeof(MSD),
    rgmsdEditStockInfo,
    edwpNone
};

// Dialog command table definition.
CMD rgcmdEditStockInfo[] =
{
    {IDOK,     CmdESDone},
    {IDCANCEL, CmdESDone}
};

CMDI cmdiEditStockInfo =
{
    sizeof(rgcmdEditStockInfo) / sizeof(CMD),
    rgcmdEditStockInfo,
    edwpNone
};

//
//  FUNCTION: CmdEditStockInfo(HWND, WORD, WORD, HWND)
//
//  PURPOSE: Displays the "Edit Stock Information" dialog box
//
//  PARAMETERS:
//    hwnd      - Window handle
//    wCommand  - IDM_EDITSTOCKINFO (unused)
//    wNotify   - Notification number (unused)
//    hwndCtrl  - NULL (unused)
//
//  RETURN VALUE:
//
//    Always returns 0 - Message handled
//
//  COMMENTS:
//    To process the IDM_EDITSTOCKINFO message, call DialogBox() to display the
//    dialog box.

LRESULT CmdEditStockInfo(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    DialogBox(hInst, MAKEINTRESOURCE(IDD_EDITSTOCKINFO), hwnd, (DLGPROC)EditStockInfo);
    return 0;
}


//
//  FUNCTION: EditStockInfo(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for "Edit Stock Information" dialog box.
//
//  PARAMETERS:
//    hdlg - window handle of the dialog box
//    wMessage - type of message
//    wparam - message-specific information
//    lparam - message-specific information
//
//  RETURN VALUE:
//    TRUE - message handled
//    FALSE - message not handled
//
//  COMMENTS:
//
//

LRESULT CALLBACK EditStockInfo(HWND hdlg, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    return DispMessage(&msdiEditStockInfo, hdlg, uMessage, wparam, lparam);
}


//
//  FUNCTION: MsgESInit(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: To initialize the controls in the dialog box with the 
//           stock information.
//
//  PARAMETERS:
//    hwnd - The window handing the message.
//    uMessage - The message number. (unused).
//    wparam - Message specific data (unused).
//    lparam - Message specific data (unused).
//
//  RETURN VALUE:
//    Always returns 0 - message handled.
//
//  COMMENTS:
//

LRESULT MsgESInit(HWND hdlg, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    HWND hwndLV = GetDlgItem(GetParent(hdlg), IDD_LISTVIEW);
    int iFocusItem = ListView_GetNextItem(hwndLV, -1, LVNI_ALL | LVNI_FOCUSED); 
    STOCKINFO *psi;
    LV_ITEM   lvi;

    if (iFocusItem != -1)
    {
        lvi.mask     = LVIF_PARAM;
        lvi.iItem    = iFocusItem;
        lvi.iSubItem = 0;
        
        ListView_GetItem(hwndLV, &lvi);
        psi = (STOCKINFO *)lvi.lParam;
        
        SetDlgItemText(hdlg, IDC_COMPANYNAME, psi->szCompany);
        SetDlgItemText(hdlg, IDC_SYMBOL,      psi->szSymbol);
        SetDlgItemText(hdlg, IDC_EXCHANGE,    psi->szExchange);
        
        SetDlgItemInt(hdlg, IDC_HIGH,         psi->iHigh,  FALSE);
        SetDlgItemInt(hdlg, IDC_LOW,          psi->iLow,   FALSE);
        SetDlgItemInt(hdlg, IDC_CLOSE,        psi->iClose, FALSE);
        
        SetDlgItemText(hdlg, IDC_NEWS,        psi->szNews);
        
        SendDlgItemMessage(hdlg, IDC_COMPANYNAME, EM_LIMITTEXT, IDC_MAXCOMPANYLEN-1,  0L);
        SendDlgItemMessage(hdlg, IDC_SYMBOL,      EM_LIMITTEXT, IDC_MAXSYMBOLLEN-1,   0L);
        SendDlgItemMessage(hdlg, IDC_EXCHANGE,    EM_LIMITTEXT, IDC_MAXEXCHANGELEN-1, 0L);
        SendDlgItemMessage(hdlg, IDC_HIGH,        EM_LIMITTEXT, IDC_MAXDOLLARLEN-1,   0L);
        SendDlgItemMessage(hdlg, IDC_LOW,         EM_LIMITTEXT, IDC_MAXDOLLARLEN-1,   0L);
        SendDlgItemMessage(hdlg, IDC_CLOSE,       EM_LIMITTEXT, IDC_MAXDOLLARLEN-1,   0L);
        SendDlgItemMessage(hdlg, IDC_NEWS,        EM_LIMITTEXT, IDC_MAXNEWSLEN-1,     0L);
        
        SetWindowLong(hdlg, DWL_USER, (LONG)psi);
    }
    else
    {
        MessageBox(hdlg, "Please select an item first", "LISTCTRL", MB_OK);
        EndDialog(hdlg, FALSE);          // Exit the dialog
        SetFocus(GetDlgItem(GetParent(hdlg), IDD_LISTVIEW));
    }

    return TRUE;
}

//
//  FUNCTION: MsgESCommand(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Process WM_COMMAND message sent to the dialog.
//
//  PARAMETERS:
//    hwnd - The window handing the message.
//    uMessage - The message number. (unused).
//    wparam - Message specific data (unused).
//    lparam - Message specific data (unused).
//
//  RETURN VALUE:
//    Always returns 0 - message handled.
//
//  COMMENTS:
//    Uses this DipsCommand function defined in wndproc.c combined
//    with the cmdiEditStockInfo structure defined in this file to handle
//    the command messages for the dialog box.
//

LRESULT MsgESCommand(HWND   hwnd, 
                     UINT   uMessage, 
                     WPARAM wparam, 
                     LPARAM lparam)
{
    return DispCommand(&cmdiEditStockInfo, hwnd, wparam, lparam);
}

//
//  FUNCTION: CmdESDone(HWND, WORD, HWND)
//
//  PURPOSE: Free the dialog box and grab the new stock data.
//
//  PARAMETERS:
//    hwnd - The window handling the command.
//    wCommand - The command to be handled (unused).
//    wNotify   - Notification number (unused)
//    hwndCtrl - NULL (unused).
//
//  RETURN VALUE:
//    Always returns TRUE.
//
//  COMMENTS:
//    Calls EndDialog to finish the dialog session.
//

LRESULT CmdESDone(HWND hdlg, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    HWND hwndLV=GetDlgItem(GetParent(hdlg), IDD_LISTVIEW);
    int iFocusItem = ListView_GetNextItem(hwndLV, -1, LVNI_ALL | LVNI_SELECTED); 
    BOOL bSuccess=0;
    RECT rc;

    if (wCommand==IDOK)
    {
        STOCKINFO *psi = (STOCKINFO *)GetWindowLong(hdlg, DWL_USER);

        GetDlgItemText(hdlg, IDC_COMPANYNAME, psi->szCompany, IDC_MAXCOMPANYLEN);
        GetDlgItemText(hdlg, IDC_SYMBOL, psi->szSymbol, IDC_MAXSYMBOLLEN);
        GetDlgItemText(hdlg, IDC_EXCHANGE, psi->szExchange, IDC_MAXEXCHANGELEN);

        psi->iHigh  = GetDlgItemInt(hdlg, IDC_HIGH,  &bSuccess, FALSE);
        psi->iLow   = GetDlgItemInt(hdlg, IDC_LOW,   &bSuccess, FALSE);
        psi->iClose = GetDlgItemInt(hdlg, IDC_CLOSE, &bSuccess, FALSE);

        GetDlgItemText(hdlg, IDC_NEWS, psi->szNews, IDC_MAXNEWSLEN);

        ListView_GetItemRect(hwndLV, iFocusItem, &rc, LVIR_BOUNDS);

        InvalidateRect(hwndLV, &rc, TRUE);
        UpdateWindow(hwndLV);
    }

    EndDialog(hdlg, wCommand==IDOK);          // Exit the dialog
    SetFocus(hwndLV);
    return TRUE;
}

