// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993-1995  Microsoft Corporation.  All Rights Reserved.
//
//  MODULE:   listview.c
//
//  PURPOSE:  Handles all listview manipulation.
//
//  FUNCTIONS:
//    CreateListView         - Creates a list view control in report view
//    ListView_InitColumns   - Initializes the list view columns
//    AddProcessItem         - Adds an item to the process list view
//    AddThreadItem          - Adds an item to the thread list view
//    ListView_OnGetDispInfo - Handles the WM_NOTIFY/LV_GETDISPINFO message
//    ListView_OnClick       - Handles the WM_NOTIFY/NM_CLICK message
//    ListView_OnDeleteItem  - Frees memory when an item is deleted
//    ListView_OnColumnClick - Sorts the list view based on the column clicked
//    ListView_CompareProc   - Comparison callback for sorting the list view
//    LV_Subclass            - Subclass procedure for the thread list view
                            
//  COMMENTS:
//

#include <windows.h>            // required for all Windows applications
#include <commctrl.h>
#include <string.h>
#include "globals.h"            // prototypes specific to this application
#include "resource.h"
#include "listview.h"
#include "procthrd.h"

FARPROC g_lpfnThreadListView;

//-------------------------------------------------------------------------
// Useful application-global variables defined here!
APPCOLUMNDEF acdProcess[] = 
{
    {IDS_PROCESS,       100},
    {IDS_PID,           75},
    {IDS_PRIORITY,      75},
    {IDS_THREADCNT,     90},
    {IDS_PROCESSTYPE,   75},
    {IDS_FULLPATH,      250}
};

APPCOLUMNDEF acdThread[] = 
{
    {IDS_THREAD,        100},
    {IDS_TPID,          100},
    {IDS_TPRIORITY,     100}
};
     

//
//  FUNCTION: CreateListView(HWND, DWORD, UINT, UINT, UINT)
//
//  PURPOSE:  Creates a list view control
//
//  PARAMETERS:
//    hwndParent - Handle of the parent window.
//    dwStyle    - Style flags for listview control
//    idString   - Identifier of the first string table resource entry for
//                 the new control's column titles
//    cColumns   - Number of columns to add            
//    idCtl      - Control identifier
//
//  RETURN VALUE:
//    The window handle of the newly created listview control if successful, 
//    or NULL if a new listview control could not be created.
//
//

HWND CreateListView (HWND hwndParent, DWORD dwStyle, APPCOLUMNDEF acd[], UINT cColumns, UINT idCtl)
{
    HWND hwndLV   = NULL;
    RECT rcClient = {0};

    // Make sure the common controls DLL is loaded.
    InitCommonControls();

    // Create the control so that it fills the client area of the parent 
    // window.  The size of the control will be updated in the parent's 
    // WM_SIZE message handler
    GetClientRect(hwndParent, &rcClient);

    hwndLV = CreateWindow(WC_LISTVIEW, 
                          "", 
                          dwStyle,
                          rcClient.left, 
                          rcClient.top,
                          rcClient.right, 
                          rcClient.bottom,
                          hwndParent, 
                          (HMENU)idCtl, 
                          hInst, 
                          NULL);
    if (hwndLV == NULL)
        return NULL;

    // Initialize the image lists and add columns
    if (!ListView_InitColumns(hwndLV, acd, cColumns)) 
    {
        DestroyWindow(hwndLV);
        return NULL;
    }
    return hwndLV;
}


//
//  FUNCTION: ListView_InitColumns(HWND, UINT, UINT)
//
//  PURPOSE:  Adds columns to the specified list view.
//
//  PARAMETERS:
//    hwndLV    - Handle to the listview control.
//    idString  - ID in the string table where the column names begin
//    cColumns  - Number of columns to add                                                
//
//  RETURN VALUE:
//    TRUE  - Success, all the columns were added          
//    FALSE - Failure, an error occured while adding one of the columns
//
//  COMMENTS:
//
//

BOOL WINAPI ListView_InitColumns (HWND hwndLV, APPCOLUMNDEF acd[], UINT cColumns)
{
    LV_COLUMN lvc          = {0};
    UINT      iCol         = 0;
    char      szTemp[64];

    // Initialize the LV_COLUMN struct
    lvc.mask    = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvc.fmt     = LVCFMT_LEFT;
    lvc.pszText = szTemp;             

    // Add the columns
    for (iCol=0; iCol < cColumns; iCol++)
    {
        lvc.iSubItem = iCol;
        lvc.cx = acd[iCol].cxColumn;
        LoadString(hInst, acd[iCol].idString, szTemp, sizeof(szTemp));
        if (ListView_InsertColumn(hwndLV, iCol, &lvc) == -1)
            return (FALSE);
    }
    return (TRUE);
}


//
//  FUNCTION: AddProcessItem(HWND, PINFO pi)
//
//  PURPOSE:  Add an item and subitems to the process list view.
//
//  PARAMETERS:
//    hwndLV  - Handle of the list view control
//    pi      - Application-defined MYITEM structure containing the text
//              of the item and its corresponding subitems
//
//  RETURN VALUE:
//    TRUE if the item is added successfully, FALSE otherwise.
//
//

BOOL AddProcessItem(HWND hwndLV, PINFO pi)
{
    LV_ITEM lvi      = {0};
    PPINFO  ppi      = {0};
    int     iSubItem = 0;

    lvi.mask      = LVIF_TEXT | LVIF_STATE | LVIF_PARAM;
    lvi.iItem     = 0;
    lvi.iSubItem  = 0;
    lvi.state     = 0;
    lvi.stateMask = 0;
    lvi.pszText   = LPSTR_TEXTCALLBACK;
    lvi.iImage    = 0;  

    // Because we're using the LPSTR_TEXTCALLBACK type for the strings,
    // we need to save the PINFO structure in the lParam of the item. So,
    // here we allocate the memory for the structure, and we free the
    // memory when we get the notification that the item is being deleted.
    ppi = (PPINFO)malloc(sizeof(PINFO));
    if (!ppi)
        return FALSE;

    CopyMemory(ppi, &pi, sizeof(PINFO));

    lvi.lParam = (LPARAM)ppi;

    if (ListView_InsertItem(hwndLV, &lvi) == -1)
        return FALSE;

    // set up all the sub items to use LBSTR_TEXTCALLBACK
    for (iSubItem = 1; iSubItem < CPROCESSCOLUMNS; iSubItem++)
        ListView_SetItemText(hwndLV, 0, iSubItem, LPSTR_TEXTCALLBACK);

    return TRUE;
}


//
//  FUNCTION: AddThreadItem(HWND, TINFO ti)
//
//  PURPOSE:  Add an item and subitems to the thread list view.
//
//  PARAMETERS:
//    hwndLV  - Handle of the list view control
//    ti      - Application-defined MYITEM structure containing the text
//              of the item and its corresponding subitems
//
//  RETURN VALUE:
//    TRUE if the item is added successfully, FALSE otherwise.
//
//

BOOL AddThreadItem(HWND hwndLV, TINFO ti)
{
    LV_ITEM lvi      = {0};
    PTINFO  pti      = {0};
    int     iSubItem = 0;

    lvi.mask      = LVIF_TEXT | LVIF_STATE | LVIF_PARAM;
    lvi.iItem     = 0;
    lvi.iSubItem  = 0;
    lvi.state     = 0;
    lvi.stateMask = 0;
    lvi.pszText   = LPSTR_TEXTCALLBACK;
    lvi.iImage    = 0;

    // Because we're using the LPSTR_TEXTCALLBACK type for the strings,
    // we need to save the PINFO structure in the lParam of the item. So,
    // here we allocate the memory for the structure, and we free the
    // memory when we get the notification that the item is being deleted.
    pti = (PTINFO)malloc(sizeof(TINFO));
    if (!pti)
        return FALSE;

    CopyMemory(pti, &ti, sizeof(TINFO));
    lvi.lParam = (LPARAM)pti;

    if (ListView_InsertItem(hwndLV, &lvi) == -1)
        return FALSE;

    // set up all the sub items to use LPSTR_TEXTCALLBACK as well
    for (iSubItem = 1; iSubItem < CTHREADCOLUMNS; iSubItem++)
        ListView_SetItemText(hwndLV, 0, iSubItem, LPSTR_TEXTCALLBACK);

    return TRUE;
}


//
//  FUNCTION: ListView_OnGetDispInfo(int, LV_DISPINFO *)
//
//  PURPOSE:  Handle the WM_NOTIFY/LV_GETDISPINFO notification by returning
//            the string for the item/subitem being asked for.
//
//  PARAMETERS:
//    idFrom - Identifier of the control this is being asked for
//    plvdi  - Specifies which data is being asked for 
//
//  RETURN VALUE:
//    Always returns 0.
//
//  COMMENTS:
//    The function will return the string in the plvdi->item.pszText field
//

LRESULT ListView_OnGetDispInfo(int idFrom, LV_DISPINFO *plvdi)
{
static  char szText[128];

    switch (idFrom)
    {
        case IDC_PROCESS:
        {
            PINFO *pi = (PPINFO)(plvdi->item.lParam);

            switch (plvdi->item.iSubItem)
            {
                case 0:                     // module name
                    plvdi->item.pszText = pi->szModName;
                    break;

                case 1:                     // process id
                    wsprintf(szText, "%X", pi->pid);
                    plvdi->item.pszText = szText;
                    break;

                case 2:                     // base priority

                    // If the priority is a known value, display a readable
                    // description. Otherwise display the actual numerical 
                    // value

                    switch (pi->dwPriorityClass)
                    {
                        case REALTIME_PRIORITY_CLASS:
                            wsprintf(szText, "%d  (%s)", 
                                     pi->pcPriClassBase, 
                                     "Real Time");
                            break;

                        case HIGH_PRIORITY_CLASS:
                            wsprintf(szText, "%d  (%s)", 
                                     pi->pcPriClassBase, 
                                     "High");
                            break;

                        case NORMAL_PRIORITY_CLASS:
                            wsprintf(szText, "%d  (%s)", 
                                     pi->pcPriClassBase, 
                                     "Normal");
                            break;

                        case IDLE_PRIORITY_CLASS:
                            wsprintf(szText, "%d  (%s)", 
                                     pi->pcPriClassBase, 
                                     "Idle");
                            break;

                        default:
                            wsprintf(szText, "%d", pi->pcPriClassBase);
                    }

                    plvdi->item.pszText = szText;
                    break;

                case 3:                     // thread count
                    wsprintf(szText, "%d", pi->cntThreads);
                    plvdi->item.pszText = szText;
                    break;

                case 4:
                    wsprintf(szText, "%d-Bit", pi->uAppType);
                    plvdi->item.pszText = szText;
                    break;

                case 5:
                    plvdi->item.pszText = pi->szFullPath;
                    break;

                default:
                    break;
            }
            break;
        } // case IDC_PROCESS

        case IDC_THREAD:
        {
            PTINFO pti = (PTINFO)(plvdi->item.lParam);

            switch (plvdi->item.iSubItem)
            {
                case 0:                     // thread ID
                    wsprintf(szText, "%X", pti->tid);
                    plvdi->item.pszText = szText;
                    break;

                case 1:                     // owning PID
                    wsprintf(szText, "%X", pti->pidOwner);
                    plvdi->item.pszText = szText;
                    break;

                case 2:                     // thread priority

                    // If the priority is a known value, display a readable
                    // description. Otherwise display the actual numerical 
                    // value
                    switch (pti->tpDeltaPri)
                    {
                        case THREAD_PRIORITY_TIME_CRITICAL:
                            wsprintf(szText, "%d  (%s)", 
                                     pti->tpDeltaPri + pti->tpBasePri, 
                                     "Time Critical");
                            break;

                        case THREAD_PRIORITY_HIGHEST:
                            wsprintf(szText, "%d  (%s)", 
                                     pti->tpDeltaPri + pti->tpBasePri, 
                                     "Highest");
                            break;

                        case THREAD_PRIORITY_ABOVE_NORMAL:
                            wsprintf(szText, "%d  (%s)", 
                                     pti->tpDeltaPri + pti->tpBasePri, 
                                     "Above Normal");
                            break;

                        case THREAD_PRIORITY_NORMAL:
                            wsprintf(szText, "%d  (%s)", 
                                     pti->tpDeltaPri + pti->tpBasePri, 
                                     "Normal");
                            break;

                        case THREAD_PRIORITY_BELOW_NORMAL:
                            wsprintf(szText, "%d  (%s)", 
                                     pti->tpDeltaPri + pti->tpBasePri, 
                                     "Below Normal");
                            break;

                        case THREAD_PRIORITY_LOWEST:
                            wsprintf(szText, "%d  (%s)", 
                                     pti->tpDeltaPri + pti->tpBasePri, 
                                     "Lowest");
                            break;

                        case THREAD_PRIORITY_IDLE:
                            wsprintf(szText, "%d  (%s)", 
                                     pti->tpDeltaPri + pti->tpBasePri, 
                                     "Idle");
                            break;

                        default:
                            wsprintf(szText, "%d", 
                                     pti->tpDeltaPri + pti->tpBasePri);
                    }
              
                    plvdi->item.pszText = szText;
                    break;

                default:
                    break;
            }
            break;
        } // case IDC_THREAD
    }
    return (0);
}


//
//  FUNCTION: ListView_OnClick(int, NM_LISTVIEW *)
//
//  PURPOSE:  Handle the WM_NOTIFY/NM_CLICK message by retrieving the 
//            item information for the process item clicked on and 
//            refresh the threads associated with the process item.
//
//  PARAMETERS:
//    idFrom  - Identifier of the control sending this notification
//    pnmlv   - Control specific information about the notification
//
//  RETURN VALUE:
//    TRUE  - Success
//    FALSE - Initialization failed
//
//  COMMENTS:
//
//

LRESULT ListView_OnClick(int idFrom, NM_LISTVIEW *pnmlv)
{
    LV_HITTESTINFO lvhti         = {0};
    LV_ITEM        lvi           = {0};
    int            iItemClicked  = 0;
    BOOL           fErr          = FALSE;
    POINT          point         = {0};

    // only handle this message if it is sent from the process list view
    if (idFrom != IDC_PROCESS)
        return (0);         

    // Find out where the cursor is
    GetCursorPos(&point);
    ScreenToClient(g_hwndProcess, &point);

    // Now do a hittest to find out which item this is
    lvhti.pt     = point;
    iItemClicked = ListView_HitTest(g_hwndProcess, &lvhti);

    // Sset up the LV_ITEM structure to get the process id
    lvi.iItem    = iItemClicked;
    lvi.iSubItem = 0;
    lvi.mask     = LVIF_PARAM;

    fErr = ListView_GetItem(g_hwndProcess, &lvi);
    if (TRUE == fErr)   // If we got the item...
        RefreshThreadList(g_hwndThread, ((PPINFO)(lvi.lParam))->pid);
    
    return (0);
}


//
//  FUNCTION: ListView_OnDeleteItem(int, NM_LISTVIEW *)
//
//  PURPOSE:  Handle the WM_NOTIFY/LV_DELETEITEM by freeing the structure
//            stored with the item.
//
//  PARAMETERS:
//    idFrom  - Identifier of the control sending the notification
//    pnmlv   - Control specific information about the notification, 
//              specifically which item is being freed.
//
//  RETURN VALUE:
//    Always returns 0  
//
//  COMMENTS:
//

LRESULT ListView_OnDeleteItem(int idFrom, NM_LISTVIEW *pnmlv)
{
    LV_ITEM lvi  = {0};
    BOOL    fErr = FALSE;

    // set up the LV_ITEM structure to get the pointer stored in the lParam
    lvi.iItem    = pnmlv->iItem;
    lvi.iSubItem = 0;
    lvi.mask     = LVIF_PARAM;

    // Get the LV_ITEM structure from the appropriate control 
    fErr = ListView_GetItem(pnmlv->hdr.hwndFrom, &lvi);
    if (TRUE == fErr)                   // If we got the item                                           
        free((VOID *)lvi.lParam);       // free the structure

    return (0);
}


//
//  FUNCTION: ListView_OnColumnClick(int, NM_LISTVIEW *)
//
//  PURPOSE:  Handle the WM_NOTIFY/LVN_COLUMNCLICK notification by sorting
//            the control based on the column clicked
//
//  PARAMETERS:
//    idFrom  - Identifier of the control sending the notification
//    pnmlv   - More information about the notification, specifically which
//              column was clicked on.
//
//  RETURN VALUE:
//    Always returns 0.
//
//  COMMENTS:
//

LRESULT ListView_OnColumnClick(int idFrom, NM_LISTVIEW *pnmlv)
{
    SORTINFO si = {0};

    si.idFrom   = idFrom;
    si.iSubItem = pnmlv->iSubItem;

    ListView_SortItems(pnmlv->hdr.hwndFrom, ListView_CompareProc,
                       (LPARAM)&si);
    return (0);
}


//
//  FUNCTION: ListView_CompareProc(LPARAM, LPARAM, LPARAM)
//
//  PURPOSE:  Callback used for column sorting in a listview
//
//  PARAMETERS:  
//    lParam1    - lParam of the first item being compared (from LV_ITEM)
//    lParam2    - lParam of the second item being compared (from LV_ITEM)
//    lParamSort - lParam passed in from the ListView_SortItems()
//
//  RETURN VALUE:
//    -1 - lParam1 should come before lParam2
//     0 - lParam1 and lParam2 are the same
//     1 - lParam1 should come after lParam2
//
//  COMMENTS:
//

int CALLBACK ListView_CompareProc(LPARAM lParam1, 
                                  LPARAM lParam2, 
                                  LPARAM lParamSort)
{
    PSORTINFO psi          = (PSORTINFO)lParamSort;
    int       iReturn      = 0;
    char      szTemp1[32];
    char      szTemp2[32];

    if (psi->idFrom == IDC_PROCESS)
    {
        PPINFO ppi1 = (PPINFO)lParam1;
        PPINFO ppi2 = (PPINFO)lParam2;

        switch (psi->iSubItem)
        {
            case 0:                         // module name
                iReturn = lstrcmpi(ppi1->szModName, ppi2->szModName);
                break;

            case 1:                         // process ID
                wsprintf(szTemp1, "%X", ppi1->pid);
                wsprintf(szTemp2, "%X", ppi2->pid);
                iReturn = lstrcmpi(szTemp1, szTemp2);
                break;

            case 2:                         // base priority
                if (ppi1->pcPriClassBase < ppi2->pcPriClassBase)    
                    iReturn = -1;
                else
                    if (ppi1->pcPriClassBase > ppi2->pcPriClassBase)
                        iReturn = 1;
                    else                    // same, so fall back on name
                        iReturn = lstrcmpi(ppi1->szModName, 
                                           ppi2->szModName);
                break;

            case 3:                          // thread count
                if (ppi1->cntThreads < ppi2->cntThreads)    
                    iReturn = -1;
                else
                    if (ppi1->cntThreads > ppi2->cntThreads)
                        iReturn = 1;
                    else                    // same, so fall back on name
                        iReturn = lstrcmpi(ppi1->szModName, 
                                           ppi2->szModName);
                break;
            
            case 4:                         // 16 or 32 bit
                if (ppi1->uAppType < ppi2->uAppType)
                    iReturn = -1;
                else
                    if (ppi1->uAppType == ppi2->uAppType)
                        iReturn = 0;
                    else
                        iReturn = 1;
                break;

            case 5:                         // full path name
                iReturn = lstrcmpi(ppi1->szFullPath, ppi2->szFullPath);
                if (iReturn == 0)
                    iReturn = lstrcmpi(ppi1->szModName, ppi2->szModName);
                break;                  

            default:
                iReturn = -1;
        }
    } // if(idFrom)
    else
    {
        PTINFO pti1 = (PTINFO)lParam1;
        PTINFO pti2 = (PTINFO)lParam2;

        switch (psi->iSubItem)
        {
            case 0:                         // thread id
                wsprintf(szTemp1, "%X", pti1->tid);
                wsprintf(szTemp2, "%X", pti2->tid);
                iReturn = lstrcmpi(szTemp1, szTemp2);
                break;

            case 1:                         // owning pid
                wsprintf(szTemp1, "%X", pti1->pidOwner);
                wsprintf(szTemp2, "%X", pti2->pidOwner);
                iReturn = lstrcmpi(szTemp1, szTemp2);
                break;

            case 2:                         // tpBasePri + tpDeltaPri
                if ((pti1->tpBasePri + pti1->tpDeltaPri) < 
                    (pti2->tpBasePri + pti2->tpDeltaPri))   

                    iReturn = -1;
                else
                    if ((pti1->tpBasePri + pti1->tpDeltaPri) > 
                        (pti2->tpBasePri + pti2->tpDeltaPri))

                        iReturn = 1;
                    else                    // same, so fall back on tid
                    {
                        wsprintf(szTemp1, "%X", pti1->tid);
                        wsprintf(szTemp2, "%X", pti2->tid);
                        iReturn = lstrcmpi(szTemp1, szTemp2);
                    }
                break;

            default:
                iReturn = -1;
        } 
    } // else

    return iReturn;
}
                

//
//  FUNCTION: ListView_GetSelectedPid(HWND, LPSTR, int)
//
//  PURPOSE:  Retrieve the PID for the selected item in the list view             
//
//  PARAMETERS:
//    
//    hwndLV     - handle to the list view control
//     lpszPath  - Buffer to contain full path of selected process's 
//                 executable file
//     cch       - Size in bytes of lpszPath
//
//  RETURN VALUE:
//    The PID of the selected item in the list view control
//
//  COMMENTS:
//

DWORD ListView_GetSelectedPid(HWND hwndLV, LPSTR lpszPath, int cch)
{
    int cbPathLen;
    int index;          // index of the selected item
    LV_ITEM lvi;

    // get the index of the selected item
    index = ListView_GetNextItem(hwndLV, -1, LVNI_ALL | LVNI_SELECTED);
    if (index == -1)
        return 0;
        
    // fill out the LV_ITEM struct so we can get the PINFO struct stored
    // in the lParam for the selected item
    lvi.iItem = index;
    lvi.iSubItem = 0;
    lvi.mask = LVIF_PARAM;
    ListView_GetItem(hwndLV, &lvi);

    cbPathLen = min (cch, sizeof(((PPINFO)(lvi.lParam))->szFullPath));
    CopyMemory(lpszPath, ((PPINFO)(lvi.lParam))->szFullPath, cbPathLen);

    // return the PID
    return (DWORD)((PPINFO)(lvi.lParam))->pid;
}



//
//  FUNCTION: LV_Subclass(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Subclass procedure so the thread list view can reject the focus
//
//  PARAMETERS:
//
//    hwnd      - Window handle  (Unused)
//    uMessage  - WM_SETFOCUS    
//    wparam    - handle of the window losing focus
//    lparam    - Extra data     (Unused)
//
//  RETURN VALUE:
//
//    return 0 if the message is WM_SETFOCUS, otherwise the message is passed
//    on to the default List view window procedure
//
//  COMMENTS:
//
//

LRESULT CALLBACK LV_Subclass(HWND hwnd, UINT uMsg, WPARAM wParam,
        LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_SETFOCUS:
            // set the focus back to the the control losing the focus
            SetFocus((HWND)wParam);
            return 0;
    }

    // if this is not the WM_SETFOCUS message, send it to the original
    // window procedure
    return CallWindowProc(g_lpfnThreadListView, hwnd, uMsg, wParam, 
            lParam);
}
