// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993-1995  Microsoft Corporation.  All Rights Reserved.
//
//  MODULE:   listview.c
//
//  PURPOSE:   Implement the functions specific to the listview control.
//
//  FUNCTIONS:
//    MsgNotify              - Processes the WM_NOTIFY message
//    ListViewCompareProc    - Callback routine for sorting listview items
//    InitListViewImageLists - Initialize the listview image lists
//    InitListViewColumns    - Specify the text for the column headings.
//    InitListViewItems      - Add the items to the listview control
//    CreateListView         - Create the listview control
//    SwitchView             - Change the visual view of the listview control
//
//  COMMENTS:
//

#include <windows.h>            // required for all Windows applications
#include <windowsx.h>
#include <commctrl.h>
#include "globals.h"            // prototypes specific to this application
#include "listview.h"           // prototypes and defines specific to the
                                // listview control
#include "resource.h"

// Module specific globals

STOCKINFO rgStockInfo[]=
{
    {"Banc One",     "ONE",      "NYSE",      33, 32, 33, "Yes"},
    {"Borland",      "BORL",     "NASDAQ",    14, 13, 13, "Yes"},
    {"Intel",        "INTC",     "NASDAQ",    65, 64, 64, "No"},
    {"Kodak",        "EK",       "NYSE",      45, 43, 44, "Yes"},
    {"Microsoft",    "MSFT",     "NASDAQ",    80, 78, 79, "Yes"},
};


//
//  FUNCTION: MsgNotify(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes the WM_NOTIFY message sent by the listview control
//
//  PARAMETERS:
//
//    hwnd      - Window handle  (Unused)
//    uMessage  - Message number (Unused)
//    wparam    - Extra data     (Unused)
//    lparam    - Extra data     (Unused)
//
//  RETURN VALUE:
//
//    Always returns 0 - Message handled
//
//  COMMENTS:
//
//

LRESULT MsgNotify(HWND   hwnd, 
                  UINT   uMessage, 
                  WPARAM wparam, 
                  LPARAM lparam)
{
    LPNM_LISTVIEW  pnm    = (LPNM_LISTVIEW)lparam;
    HWND           hwndLV = pnm->hdr.hwndFrom;
    UINT           state;
    DWORD          dwpos;
    LV_HITTESTINFO lvhti;
    int            iItemClicked;

    if (wparam != IDD_LISTVIEW)
       return 0L;

    switch (pnm->hdr.code)
    {
        case LVN_GETDISPINFO:
        {
            LV_DISPINFO *plvdi = (LV_DISPINFO *)lparam;
            STOCKINFO   *pi    = (STOCKINFO *)(plvdi->item.lParam);

            if (plvdi->item.mask & LVIF_TEXT)
            {
                switch (plvdi->item.iSubItem)
                {
                    case 0:
                        
                        plvdi->item.pszText = pi->szCompany;
                        break;

                    case 1:

                        plvdi->item.pszText = pi->szSymbol;
                        break;

                    case 2:

                        plvdi->item.pszText = pi->szExchange;
                        break;

                    case 3:

                        //Before sending the WM_NOTIFY/LVN_GETDISPINFO message,
                        //the control has allocated plvdi->item.cchTextMax 
                        //characters for the plvdi->item.pszText member.
                        //Since we will only be copying 3 characters (including
                        //the NULL terminator), we don't bother checking
                        //plvdi->item.cchTextMax since it's set to MAX_PATH in this
                        //version of Windows 95.
                        
                        wsprintf(plvdi->item.pszText, "%u", pi->iHigh);
                        break;

                    case 4:

                        wsprintf(plvdi->item.pszText, "%u", pi->iLow);
                        break;

                    case 5:

                        wsprintf(plvdi->item.pszText, "%u", pi->iClose);
                        break;

                    case 6:

                        plvdi->item.pszText = pi->szNews;
                        break;
                  
                    default:
                        break;
                }

                break;
            }

            break;
        }
        
        case LVN_ENDLABELEDIT:

            return TRUE;

        case LVN_SETDISPINFO:

        {
            LV_DISPINFO *plvdi = (LV_DISPINFO *)lparam;
            STOCKINFO   *pi    = (STOCKINFO *)(plvdi->item.lParam);

//We really should check the mask here, but Win95 does not set this bit before
//sending the LVN_SETDISPINFO notification.  The *only* time LVN_SETDISPINFO is
//sent is when the text has been modified, so you can assume the pszText member
//is valid.
//            if (plvdi->item.mask & LVIF_TEXT)

            lstrcpy(pi->szCompany, plvdi->item.pszText);
            PostMessage(hwnd, WM_COMMAND, IDM_EDITSTOCKINFO, 0L);
            break;
        }

        case LVN_COLUMNCLICK:

            ListView_SortItems(pnm->hdr.hwndFrom,
                               ListViewCompareProc,
                               (LPARAM)(pnm->iSubItem));
            break;

        case NM_CLICK:
        case NM_RCLICK:

            // This code does the check box stuff...

            // Find out where the cursor was
            dwpos = GetMessagePos();
            lvhti.pt.x = LOWORD(dwpos);
            lvhti.pt.y = HIWORD(dwpos);

            MapWindowPoints(HWND_DESKTOP, hwndLV, &lvhti.pt, 1);

            // Now do a hittest with this point.
            iItemClicked = ListView_HitTest(hwndLV, &lvhti);

            if (lvhti.flags & LVHT_ONITEMSTATEICON)
            {
                // Now lets get the state from the item and toggle it.
                state = ListView_GetItemState(hwndLV, 
                                              iItemClicked, 
                                              LVIS_STATEIMAGEMASK);

                state = (state == LVIS_GCNOCHECK) ? LVIS_GCCHECK : LVIS_GCNOCHECK;

                ListView_SetItemState(hwndLV,
                                      iItemClicked,
                                      state, 
                                      LVIS_STATEIMAGEMASK);
            }
            break;

        default:
            break;
    }

    return 0;
}


//
//  FUNCTION: ListViewCompareProc(LPARAM, LPARAM, LPARAM)
//
//  PURPOSE: Callback routine for sorting list
//
//  PARAMETERS:
//
//    lparam1    - Pointer to STOCKINFO structure
//    lparam2    - Pointer to STOCKINFO structure
//    lparamSort - Column being sorted.  Same value that was
//                 passed in the lparam of the ListView_SortItems()
//                 call.
//
//  RETURN VALUE:
//
//    Always returns 0 - Message handled
//
//  COMMENTS:
//    lparam1 and lparam2 are pointers to STOCKINFO structures.
//    These two parameters will always point to the same type of
//    structure that was specified in the lparam of the LV_ITEM
//    structure when the item was added.  See InitListViewItems()
//    below, where the lvi.lparam value is set.
//

int CALLBACK ListViewCompareProc(LPARAM lparam1, 
                                 LPARAM lparam2,
                                 LPARAM lparamSort)
{
    STOCKINFO *pRec1 = (STOCKINFO *)lparam1;
    STOCKINFO *pRec2 = (STOCKINFO *)lparam2;
    LPSTR     lpString1 = NULL, lpString2 = NULL;
    char      szTemp1[5];
    char      szTemp2[5];
    int       iResult = 1;

    if (pRec1 && pRec2)
    {
        switch (lparamSort)
        {
            case 0:
                lpString1 = pRec1->szCompany;
                lpString2 = pRec2->szCompany;
                break;             

            case 1:
                lpString1 = pRec1->szSymbol;
                lpString2 = pRec2->szSymbol;
                break;

            case 2:
                lpString1 = pRec1->szExchange;
                lpString2 = pRec2->szExchange;
                break;

            case 3:
                wsprintf(szTemp1, "%d", pRec1->iHigh);
                wsprintf(szTemp2, "%d", pRec2->iHigh);
                lpString1 = szTemp1;
                lpString2 = szTemp2;
                break;

            case 4:
                wsprintf(szTemp1, "%d", pRec1->iLow);
                wsprintf(szTemp2, "%d", pRec2->iLow);
                lpString1 = szTemp1;
                lpString2 = szTemp2;
                break;

            case 5:
                wsprintf(szTemp1, "%d", pRec1->iClose);
                wsprintf(szTemp2, "%d", pRec2->iClose);
                lpString1 = szTemp1;
                lpString2 = szTemp2;
                break;

            case 6:
                lpString1 = pRec1->szNews;
                lpString2 = pRec2->szNews;
                break;

            default:
                break;
        }

        iResult = lstrcmpi(lpString1, lpString2);

        //Now, if the strings are equal, compare the szCompany fields
        if (!iResult)
           iResult = lstrcmpi(pRec1->szCompany, pRec2->szCompany);
    }

    return iResult;
}


//
//  FUNCTION: InitListViewImageLists(HWND)
//
//  PURPOSE: Fill the image lists with the icons images that will
//           be used when displaying the items.
//
//  PARAMETERS:
//
//    HWND - Handle to the listview control
//
//  RETURN VALUE:
//
//    TRUE  - if image lists were successfully initialized
//    FALSE - if image lists were not successfully initialized
//
//  COMMENTS:
//

BOOL WINAPI InitListViewImageLists(HWND hwndLV)
{
    HICON hicon;
    UINT  i;
    HIMAGELIST himlSmall;
    HIMAGELIST himlLarge;
    HIMAGELIST himlState;

    himlSmall = ImageList_Create(16, 16, TRUE, IDC_NUMITEMS, 0); 
    himlLarge = ImageList_Create(32, 32, TRUE, IDC_NUMITEMS, 0); 
    himlState = ImageList_Create(16, 16, TRUE, 2, 0);

    ImageList_AddMasked (himlState, 
                         LoadBitmap (hInst,MAKEINTRESOURCE(IDB_CHECKSTATES)),
                         RGB (255,0,0));
    
    for (i = IDC_ONE; i <= IDC_MSFT; i++)
    {
        hicon = LoadIcon(hInst, MAKEINTRESOURCE(i));
        if (ImageList_AddIcon(himlSmall, hicon) == -1 ||
            ImageList_AddIcon(himlLarge, hicon) == -1)
            return FALSE;
    }

    ListView_SetImageList(hwndLV, himlLarge, LVSIL_NORMAL);
    ListView_SetImageList(hwndLV, himlSmall, LVSIL_SMALL);
    ListView_SetImageList(hwndLV, himlState, LVSIL_STATE);

    return TRUE;
}


//
//  FUNCTION: InitListViewColumns(HWND)
//
//  PURPOSE: Specify the text for the column headings.
//
//  PARAMETERS:
//
//    HWND - Handle to the listview control
//
//  RETURN VALUE:
//
//    TRUE  - if column headings were successfully initialized
//    FALSE - if column headings were not successfully initialized
//
//  COMMENTS:
//

BOOL WINAPI InitListViewColumns(HWND hwndLV)
{
    LV_COLUMN lvc;
    int       iCol;
    char      szText[IDC_MAXCOLUMNHDG];

    // Initialize the LV_COLUMN structure.

    lvc.mask    = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvc.fmt     = LVCFMT_LEFT;
    lvc.cx      = 95;
    lvc.pszText = szText; 

    // Add the columns.  NOTE: This code assumes that IDS_COMPANYCOLUMN
    // is the first string in the group, and that the other strings are
    // in numerical order following.  See LISTVIEW.H for the defines.

    for (iCol = 0; iCol < IDC_NUMCOLUMNS; iCol++)
    {
        lvc.iSubItem = iCol;
        LoadString(hInst, 
                   IDS_COMPANYCOLUMN + iCol,
                   szText,
                   sizeof(szText));
        if (ListView_InsertColumn(hwndLV, iCol, &lvc) == -1)
            return FALSE;
    }

//  Now, set the column widths based on the header.  I really want to put
//  this in the loop above, but for some reason it doesn't like it there.
 
    for (iCol = 1; iCol < IDC_NUMCOLUMNS; iCol++)
        ListView_SetColumnWidth(hwndLV, iCol, LVSCW_AUTOSIZE_USEHEADER);
    return TRUE;
}


//
//  FUNCTION: InitListViewItems(HWND)
//
//  PURPOSE: Add the items to the listview control
//
//  PARAMETERS:
//
//    HWND - Handle to the listview control
//
//  RETURN VALUE:
//
//    TRUE  - if items were successfully added
//    FALSE - if items were not successfully added
//
//  COMMENTS:
//

BOOL WINAPI InitListViewItems(HWND hwndLV)
{
    LV_ITEM lvi;
    int iItem;
    int iSubItem;
                                                 

    lvi.mask        = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
    lvi.state       = LVIS_GCNOCHECK;
    lvi.stateMask   = LVIS_STATEIMAGEMASK;

    for (iItem = 0; iItem < IDC_NUMITEMS; iItem++)
    {
        lvi.iItem       = iItem;
        lvi.iSubItem    = 0;
        lvi.pszText     = LPSTR_TEXTCALLBACK;
        lvi.cchTextMax  = IDC_MAXCOMPANYLEN;
        lvi.iImage      = iItem;
        lvi.lParam      = (LPARAM)&rgStockInfo[iItem];

        if (ListView_InsertItem(hwndLV, &lvi) == -1)
           return FALSE;

        for (iSubItem = 1; iSubItem < IDC_NUMCOLUMNS; iSubItem++)
        {
            ListView_SetItemText(hwndLV,
                                 iItem, 
                                 iSubItem, 
                                 LPSTR_TEXTCALLBACK);
        }
    }

    return TRUE;
}


//
//  FUNCTION: CreateListView(HWND)
//
//  PURPOSE: Create the listview control
//
//  PARAMETERS:
//
//    HWND - Handle to the parent window
//
//  RETURN VALUE:
//
//    hwndLV - Handle to listview control if successful
//    NULL   - if creation of listview control fails
//
//  COMMENTS:
//

HWND WINAPI CreateListView(HWND hwndParent)
{
    HWND hwndLV;
    RECT rc;

    // Force the common controls DLL to be loaded.

    InitCommonControls();

    // Create the control.

    GetClientRect(hwndParent, &rc);

    hwndLV = CreateWindow(WC_LISTVIEW, "",
                          WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_EDITLABELS |
                          LVS_REPORT | LVS_AUTOARRANGE | LVS_SHOWSELALWAYS,
                          0, 0, 
                          rc.right-rc.left, rc.bottom-rc.top,
                          hwndParent,
                          (HMENU)IDD_LISTVIEW, 
                          hInst, 
                          NULL);

    if (hwndLV == NULL)
        return NULL;

    // Initialize the image lists, add columns, and add some
    // items and subitems.

    if (!InitListViewImageLists(hwndLV) ||
        !InitListViewColumns(hwndLV)    ||
        !InitListViewItems(hwndLV))
    {
        DestroyWindow(hwndLV);
        return FALSE;
    }
    return hwndLV;
}


//
//  FUNCTION: SwitchView(HWND, DWORD)
//
//  PURPOSE: Change the visual view of the listview control
//
//  PARAMETERS:
//
//    HWND  - Handle to the listview control
//    DWORD - View style indicating new view
//
//  RETURN VALUE:
//
//    void
//
//  COMMENTS:
//

void SwitchView(HWND hwndLV, DWORD dwView)
{
    DWORD dwStyle = GetWindowLong(hwndLV, GWL_STYLE);

    if ((dwStyle & LVS_TYPEMASK) != dwView)
        SetWindowLong(hwndLV, GWL_STYLE, (dwStyle & ~LVS_TYPEMASK) | dwView);
}

