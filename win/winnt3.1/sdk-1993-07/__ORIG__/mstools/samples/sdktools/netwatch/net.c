/*
 *  net.c
 *  
 *  Purpose:
 *      net functions
 *  
 *  Owner:
 *      MikeSart
 */
#define UNICODE 1

#include <windows.h>
#include <windowsx.h>
#include <string.h>
#include <time.h>
#include <lm.h>
#include "netwatch.h"
#include "rcids.h"

typedef struct
{
    DWORD   dwType;
    DWORD   dwData;
    TCHAR   rgsz[];
} LBDATA;

// in datetime.c
TCHAR       *PutTime(time_t tm_t, TCHAR *szStr);
TCHAR       *PutDate(time_t tm_t, TCHAR *szStr);
TCHAR       *PutCounterTime(DWORD dw, TCHAR *szStr);

// variables
TCHAR       *szFmtLB[LBITEM_NUMITEMS];

// returns TRUE - error
// if it fails initing, you should still call
// InitNetWatch(FALSE) to clean up
BOOL
InitNetWatch(BOOL fInit)
{
    DWORD   dwT;
    TCHAR   *szCommandLine;
    BOOL    fError = FALSE;

    if(fInit)
    {
        szAppName = AllocAndLoadString(IDS_APPNAME);
        if(!szAppName)
            fError = TRUE;

        // load formatting info for each listbox item
        for(dwT = 0; dwT < LBITEM_NUMITEMS; dwT++)
        {
            szFmtLB[dwT] = AllocAndLoadString(IDS_FMTLB + dwT);
            if(!szFmtLB[dwT])
                fError = TRUE;
        }

        if(szCommandLine = GetCommandLine())
        {
            // skip first arg (our exe name)
            while(*szCommandLine && (*szCommandLine != ' '))
                szCommandLine++;

            // skip spaces
            while(*szCommandLine && (*szCommandLine == ' '))
                szCommandLine++;

            if(*szCommandLine)
            {
                if(lstrlen(szCommandLine) > UNCLEN)
                    szCommandLine[UNCLEN] = '\0';
                if(!(szServerName = GlobalAllocPtr(GHND,
                    (lstrlen(szCommandLine) + 3) * sizeof(TCHAR))))
                        fError = TRUE;
                else
                {
                    if(szCommandLine[0] != '\\')
                    {
                        szServerName[0] = szServerName[1] = '\\';
                        lstrcpy(&szServerName[2], szCommandLine);
                    }
                    else
                    {
                        lstrcpy(szServerName, szCommandLine);
                    }
                }
            }
        }
        return fError;
    }

    GlobalFreeNullPtr(szAppName);
    GlobalFreeNullPtr(szServerName);
    szAppName = szServerName = NULL;

    for(dwT = 0; dwT < LBITEM_NUMITEMS; dwT++)
    {
        GlobalFreeNullPtr(szFmtLB[dwT]);
        szFmtLB[dwT] = NULL;
    }

    return fError;
}

LBDATA *
AllocExtraData1(DWORD dwType, DWORD dwData, TCHAR *sz1)
{
    LBDATA  *plbitem;
    DWORD   cb;

    if(!sz1)
        sz1 = szNil;

    cb = sizeof(LBDATA) + ((wcslen(sz1) + 1) * sizeof(TCHAR));
    plbitem = (LBDATA *)GlobalAllocPtr(GHND, cb);
    if(plbitem)
    {
        plbitem->dwType = dwType;
        plbitem->dwData = dwData;

        wcscpy(plbitem->rgsz, sz1);
    }

    return plbitem;
}

LBDATA *
AllocExtraDataUser(DWORD dwType, DWORD dwData, TCHAR *szNetName, 
    TCHAR *szUserName, TCHAR *szShareName, TCHAR *szShareRemark)
{
    LBDATA  *plbitem;
    DWORD   cb;
    DWORD   cch1, cch2, cch3;
    DWORD   cch4 = 0;
    TCHAR   *szT;

    cch1 = wcslen(szNetName) + 1;
    cch2 = wcslen(szUserName) + 1;
    cch3 = wcslen(szShareName) + 1;
    if(szShareRemark && szShareRemark[0])
        cch4 = wcslen(szShareRemark) + 4; // for 'sharename (comment)' style

    cb = sizeof(LBDATA) + ((cch1 + cch2 + cch3 + cch4 + 2) * sizeof(TCHAR));
    plbitem = (LBDATA *)GlobalAllocPtr(GHND, cb);
    if(plbitem)
    {
        plbitem->dwType = dwType;
        plbitem->dwData = dwData;

        // add \\ to front of netname
        plbitem->rgsz[0] = plbitem->rgsz[1] = '\\';
        szT = wcscpy(plbitem->rgsz + 2, szNetName);
        szT = wcscpy(szT + cch1, szUserName);
        szT = wcscpy(szT + cch2, szShareName);
        if(cch4)
        {
            wcscat(szT, TEXT(" ("));
            wcscat(szT, szShareRemark);
            wcscat(szT, TEXT(")"));
        }
    }

    return plbitem;
}

BOOL
AddLBItem(HWND hwndLB, DWORD *pdwIndex, TCHAR *pszItem, DWORD dwData)
{
    DWORD           dwT;
    static TCHAR    szCurrentItem[256];
    register DWORD  dwIndex = *pdwIndex;

    // if the current item matches, just set the data
    if((ListBox_GetText(hwndLB, dwIndex, szCurrentItem) != LB_ERR) &&
        (!wcscmp(szCurrentItem, pszItem)))
    {
        GlobalFreeNullPtr((LPVOID)ListBox_GetItemData(hwndLB, dwIndex));
        goto SetData;
    }

    // if we found an exact match later, del everything up to that
    dwT = ListBox_FindStringExact(hwndLB, dwIndex, pszItem);
    if((dwT != LB_ERR) && (dwT > dwIndex))
    {
        DWORD dw;

        for(dw = dwIndex; dw < dwT; dw++)
        {
            GlobalFreeNullPtr((LPVOID)ListBox_GetItemData(hwndLB, dwIndex));
            ListBox_DeleteString(hwndLB, dwIndex);
        }

        GlobalFreeNullPtr((LPVOID)ListBox_GetItemData(hwndLB, dwIndex));
        goto SetData;
    }

    // ok, it's a new string, insert it
    ListBox_InsertString(hwndLB, dwIndex, pszItem);

SetData:
    ListBox_SetItemData(hwndLB, dwIndex, dwData);
    (*pdwIndex)++;

    return TRUE;
}

NET_API_STATUS
FilesEnum(HWND hwndLB, DWORD *pdwIndex, TCHAR *sharename, 
    TCHAR *username, TCHAR *sharepath)
{
    FILE_INFO_3     *fi3 = NULL;
    NET_API_STATUS  nas;
    DWORD           dwentriesread;
    DWORD           dwtotalentries;

    nas = NetFileEnum(szServerName, sharepath, username, 3,
        (LPBYTE *)&fi3, MAX_PREFERRED_LENGTH, 
        &dwentriesread, &dwtotalentries, NULL);
    if(nas)
        goto err;

    while(dwentriesread)
    {
        UINT    nBitmap = 4;    // assume read bitmap

        --dwentriesread;

        if((fi3[dwentriesread].fi3_permissions & PERM_FILE_WRITE) ||
            (fi3[dwentriesread].fi3_permissions & PERM_FILE_CREATE))
                nBitmap = 3;

        wsprintf(szBuffer, szFmtLB[LBITEM_FILE], nBitmap,
            fi3[dwentriesread].fi3_pathname);

        AddLBItem(hwndLB, pdwIndex, szBuffer, (DWORD)AllocExtraData1(
            LBITEM_FILE, fi3[dwentriesread].fi3_id, username));
    }

err:
    NetApiBufferFree(fi3);
    if(nas)
        return nas | FILE_ENUM_ERROR;
    return nas;
}

NET_API_STATUS
UsersEnum(HWND hwndLB, DWORD *pdwIndex, TCHAR *sharename, TCHAR *sharepath, 
    TCHAR *shareremark, DWORD dwShareType)
{
    DWORD               dwentriesread;
    DWORD               dwtotalentries;
    CONNECTION_INFO_1   *ci1 = NULL;
    NET_API_STATUS      nas;

    nas = NetConnectionEnum(szServerName, sharename, 1, (LPBYTE *)&ci1,
        MAX_PREFERRED_LENGTH, &dwentriesread, &dwtotalentries,
        NULL);
    if(nas)
        goto err;

    dwNumUsers += dwentriesread;
    while(dwentriesread)
    {
        TCHAR *username;
        TCHAR *netname;

        --dwentriesread;

        username = ci1[dwentriesread].coni1_username;
        netname = ci1[dwentriesread].coni1_netname;
        if(!username || !username[0])
            username = szNil;
        else
            CharLower(username);
        if(!netname || !netname[0])
            netname = szNil;
        else
            CharUpper(netname);

        wsprintf(szBuffer, szFmtLB[LBITEM_USER], username, netname);

        AddLBItem(hwndLB, pdwIndex, szBuffer, (DWORD)AllocExtraDataUser(
            LBITEM_USER, dwShareType, netname, username, sharename, 
            shareremark));

        if(ci1[dwentriesread].coni1_num_opens &&
            unMenuFlags[IDM_SHOWFILES & 0xff] == MF_CHECKED && !nas)
                nas = FilesEnum(hwndLB, pdwIndex, sharename, username,
                    sharepath);
    }

err:
    NetApiBufferFree(ci1);
    return nas;
}

void
RefreshDisplay(HWND hwnd)
{
    HWND            hwndLB = GetWindow(hwnd, GW_CHILD);
    SHARE_INFO_2    *shi2 = NULL;
    DWORD           dwIndex = 0;
    DWORD           dwentriesread;
    DWORD           dwtotalentries;
    DWORD           dwT;
    NET_API_STATUS  nas;
    INT             n;

    PunchTimer(FALSE);
    SetCursor(LoadCursor(NULL, IDC_WAIT));
    n = ListBox_GetCurSel(hwndLB);
    SetWindowRedraw(hwndLB, FALSE);
    dwNumUsers = 0;

    nas = NetShareEnum(szServerName, 2, (LPBYTE *)&shi2, 
        MAX_PREFERRED_LENGTH, &dwentriesread, &dwtotalentries, NULL);
    if(nas)
        goto err;

    while(dwentriesread)
    {
        TCHAR   *sharename;
        TCHAR   *shareremark;
        int     nBitmap = 0;    // assume normal directory share

        --dwentriesread;
        sharename = shi2[dwentriesread].shi2_netname;
        shareremark = shi2[dwentriesread].shi2_remark;

        if(!sharename)
            continue;
        if(!shareremark)
            shareremark = szNil;

        // skip hidden shares
        if((unMenuFlags[IDM_SHOWHIDDEN & 0xff] == MF_UNCHECKED) &&
            (sharename[lstrlen(sharename) - 1] == TEXT('$')))
                continue;

        if(shi2[dwentriesread].shi2_type == STYPE_IPC)  // use IPC$ icon
            nBitmap = 8;
        if(shareremark[0])
        {
            wsprintf(szBuffer, szFmtLB[LBITEM_SHARE2],
                nBitmap, sharename, shareremark);
        }
        else
        {
            wsprintf(szBuffer, szFmtLB[LBITEM_SHARE], nBitmap, sharename);
        }

        if((unMenuFlags[IDM_SHOWINUSE & 0xff] != MF_CHECKED) ||
            shi2[dwentriesread].shi2_current_uses)
        {
            AddLBItem(hwndLB, &dwIndex, szBuffer,
                (DWORD)AllocExtraData1(LBITEM_SHARE, 0, sharename));
        }

        if(shi2[dwentriesread].shi2_current_uses)
        {
            if(!nas || (nas & FILE_ENUM_ERROR))
                nas = UsersEnum(hwndLB, &dwIndex, sharename,
                    shi2[dwentriesread].shi2_path, shareremark, 
                    shi2[dwentriesread].shi2_type);
        }
    }

err:
    if(nas)
    {
        AddErrorStringToLB(nas);
        dwIndex++;
    }

    // delete extra strings dangling at the end
    while((dwT = ListBox_GetItemData(hwndLB, dwIndex)) != LB_ERR)
    {
        GlobalFreeNullPtr((LPVOID)dwT);
        ListBox_DeleteString(hwndLB, dwIndex);
    }

    if((ListBox_GetCurSel(hwndLB) == LB_ERR) && 
        (ListBox_SetCurSel(hwndLB, n) == LB_ERR))
            ListBox_SetCurSel(hwndLB, 0);

    SetWindowRedraw(hwndLB, TRUE);
    SetCursor(LoadCursor(NULL, IDC_ARROW));
    UpdateWindowText(hwnd, FALSE);
    NetApiBufferFree(shi2);
    PunchTimer(TRUE);
}

NET_API_STATUS
DisplayShareDetails(HWND hwnd, LBDATA *plbdata, WORD wAction)
{
    PROPERTIES      props;
    NET_API_STATUS  nas = 0;
    SHARE_INFO_2    *shi2 = NULL;
    TCHAR           szMaxUses[11];
    TCHAR           szMaxCurrent[11];

    nas = NetShareGetInfo(szServerName, plbdata->rgsz, 2, (LPBYTE *)&shi2);
    if(!nas && shi2)
    {
        props.rgIDSStart = IDS_SHAREPROPS;
        if(shi2->shi2_type == STYPE_IPC)
            props.dwrgBmp = 0xfffffff8;   // bitmap #8 for IPC$ netname
        else
            props.dwrgBmp = 0xfffffff0;   // bitmap #0 for folder netname

        props.rgsz[0] = shi2->shi2_netname;
        props.rgsz[1] = shi2->shi2_path;
        props.rgsz[2] = shi2->shi2_remark;
        if(shi2->shi2_max_uses == SHI_USES_UNLIMITED)
        {
            props.rgsz[3] = szFromIDS2(IDS_NOLIMIT);
        }
        else
        {
            wsprintf(szMaxUses, szFmtNum, shi2->shi2_max_uses);
            props.rgsz[3] = szMaxUses;
        }
        wsprintf(szMaxCurrent, szFmtNum, shi2->shi2_current_uses);
        props.rgsz[4] = szMaxCurrent;
        props.rgsz[5] = NULL;

        if(wAction == VK_DELETE)
        {
            // if no one is connected, nuke it, else confirm with user
            if(!(shi2->shi2_current_uses) ||
                (MessageBox(hwnd, szFromIDS1(IDS_AREYOUSURE + LBITEM_SHARE), 
                szAppName, MB_ICONEXCLAMATION | MB_YESNO | MB_DEFBUTTON2) == 
                IDYES))
            {
                nas = NetShareDel(szServerName, plbdata->rgsz, 0);
            }
        }
        else
        {
            DialogBoxParam(ghInst, MAKEINTRESOURCE(DLG_PROPERTIES), 
                hwnd, PropDlgProc, (LPARAM)&props);
        }
    }

    NetApiBufferFree(shi2);
    return nas;
}

NET_API_STATUS
DisplayFileDetails(HWND hwnd, LBDATA *plbdata, WORD wAction)
{
    PROPERTIES      props;
    NET_API_STATUS  nas;
    FILE_INFO_3     *fi3 = NULL;
    TCHAR           szNumLocks[11];
    UINT            nBitmap = 4;    // assume read bitmap

    nas = NetFileGetInfo(szServerName, plbdata->dwData, 3, (LPBYTE *)&fi3);
    if(!nas && fi3)
    {
        props.rgIDSStart = IDS_FILEPROPS;

        if((fi3->fi3_permissions & PERM_FILE_WRITE) ||
            (fi3->fi3_permissions & PERM_FILE_CREATE))
                nBitmap = 3;
        props.dwrgBmp = 0xfffff072 | (nBitmap << 8);

        props.rgsz[0] = fi3->fi3_pathname;
        props.rgsz[1] = fi3->fi3_username;
        props.rgsz[2] = szFromIDS1(IDS_FILEPROPS + 10 + nBitmap);
        wsprintf(szNumLocks, szFmtNum, fi3->fi3_num_locks);
        props.rgsz[3] = szNumLocks;
        props.rgsz[4] = NULL;
        props.rgsz[5] = NULL;

        if(wAction == VK_DELETE)
        {
            wsprintf(szBuffer, szFromIDS1(IDS_AREYOUSURE + LBITEM_FILE),
                plbdata->rgsz, fi3->fi3_pathname);
            if(MessageBox(hwnd, szBuffer, szAppName,
                MB_ICONEXCLAMATION | MB_YESNO | MB_DEFBUTTON2) == IDYES)
                    nas = NetFileClose(szServerName, plbdata->dwData);
        }
        else
        {
            DialogBoxParam(ghInst, MAKEINTRESOURCE(DLG_PROPERTIES), 
                hwnd, PropDlgProc, (LPARAM)&props);
        }
    }
    NetApiBufferFree(fi3);
    return nas;
}

NET_API_STATUS
DisplayUserDetails(HWND hwnd, LBDATA *plbdata, WORD wAction)
{
    PROPERTIES      props;
    NET_API_STATUS  nas;
    time_t          tm_t;
    SESSION_INFO_2  *sesi2 = NULL;
    TCHAR           szNumOpens[11];
    TCHAR           szConnectTime[50];
    TCHAR           szIdleTime[50];
    TCHAR           *szNetName = plbdata->rgsz;
    TCHAR           *szUserName = szNetName + lstrlen(szNetName) + 1;
    TCHAR           *szShareName = szUserName + lstrlen(szUserName) + 1;

    nas = NetSessionGetInfo(szServerName, szNetName, szUserName,
        2, (LPBYTE *)&sesi2);

    if(!nas && sesi2)
    {
        props.rgIDSStart = IDS_USERPROPS;
        if(plbdata->dwData == STYPE_IPC)
            props.dwrgBmp = 0xfffff817; // IPC$ share
        else
            props.dwrgBmp = 0xfffff017; // Drive share

        if(sesi2->sesi2_user_flags & SESS_GUEST)
        {
            lstrcpy(szBuffer, szUserName);
            lstrcat(szBuffer, szFromIDS1(IDS_GUEST));
            props.rgsz[0] = szBuffer;
        }
        else
            props.rgsz[0] = szUserName;

        props.rgsz[1] = szNetName;
        props.rgsz[2] = szShareName;

        time(&tm_t);
        PutTime(tm_t - sesi2->sesi2_time, szIdleTime);
        PutDate(tm_t - sesi2->sesi2_time, szConnectTime);
        lstrcat(szConnectTime, TEXT(" "));
        lstrcat(szConnectTime, szIdleTime);
        props.rgsz[3] = szConnectTime;
        props.rgsz[4] = PutCounterTime(sesi2->sesi2_idle_time, szIdleTime);

        wsprintf(szNumOpens, szFmtNum, sesi2->sesi2_num_opens);
        props.rgsz[5] = szNumOpens;

        if(wAction == VK_DELETE)
        {
            wsprintf(szBuffer, szFromIDS1(IDS_AREYOUSURE + LBITEM_USER),
                szUserName);
            if(MessageBox(hwnd, szBuffer, szAppName,
                MB_ICONEXCLAMATION | MB_YESNO | MB_DEFBUTTON2) == IDYES)
            {
                nas = NetSessionDel(szServerName, plbdata->rgsz,
                    plbdata->rgsz + lstrlen(plbdata->rgsz) + 1);
            }
        }
        else
        {
            DialogBoxParam(ghInst, MAKEINTRESOURCE(DLG_PROPERTIES), 
                hwnd, PropDlgProc, (LPARAM)&props);
        }
    }

    if(!nas && !sesi2)
        nas = ERROR_NOT_CONNECTED;
    NetApiBufferFree(sesi2);
    return nas;
}

void
HandleMenu(HWND hwnd)
{
    HWND            hwndLB = GetWindow(hwnd, GW_CHILD);
    INT             nIndex;
    DWORD           dwData;

    nIndex = ListBox_GetCurSel(hwndLB);
    dwData = ListBox_GetItemData(hwndLB, nIndex);
    if((nIndex == LB_ERR) || (dwData == LB_ERR) || !dwData)
        goto err;

    switch(((LBDATA *)dwData)->dwType)
    {
        case LBITEM_FILE:
        case LBITEM_SHARE:
        case LBITEM_USER:
            ModifyMenu(ghMenu, IDM_DELETERESOURCE, MF_BYCOMMAND | MF_STRING,
                IDM_DELETERESOURCE, 
                szFromIDS1(IDM_DELETERESOURCE + ((LBDATA *)dwData)->dwType));
            break;

        default:
            goto err;
    }

    return;

err:
    EnableMenuItem(ghMenu, IDM_DELETERESOURCE, MF_BYCOMMAND | MF_GRAYED);
}

void
HandleWM_VKEY(HWND hwnd, WORD wAction)
{
    HWND            hwndLB = GetWindow(hwnd, GW_CHILD);
    INT             nIndex;
    DWORD           dwData;
    NET_API_STATUS  nas = 0;

    PunchTimer(FALSE);
    nIndex = ListBox_GetCurSel(hwndLB);
    dwData = ListBox_GetItemData(hwndLB, nIndex);
    if((nIndex == LB_ERR) || (dwData == LB_ERR) || !dwData)
        goto err;

    if((wAction == VK_RETURN) || (wAction == VK_DELETE))
    {
        switch(((LBDATA *)dwData)->dwType)
        {
            case LBITEM_FILE:
                nas = DisplayFileDetails(hwnd, (LBDATA *)dwData, wAction);
                break;

            case LBITEM_SHARE:
                nas = DisplayShareDetails(hwnd, (LBDATA *)dwData, wAction);
                break;

            case LBITEM_USER:
                nas = DisplayUserDetails(hwnd, (LBDATA *)dwData, wAction);
                break;
        }
    }

err:
    if(nas)
    {
        TCHAR   *szErrMessage;

        if(szErrMessage = GetSystemErrMessage(nas))
        {
            MessageBox(hwnd, szErrMessage, szAppName, MB_ICONEXCLAMATION);
            LocalFree((HLOCAL)szErrMessage);
        }
    }

    PunchTimer(TRUE);
    if(nas || (wAction == VK_DELETE))
        PostMessage(hwnd, WM_TIMER, 0, 0L);
}

void
AddErrorStringToLB(NET_API_STATUS nas)
{
    TCHAR *szErrMessage;

    if(nas & FILE_ENUM_ERROR)
        wsprintf(szBuffer, szFmtLB[LBITEM_DENIED], szFromIDS1(IDS_ERRENUMFILES));
    else
        wsprintf(szBuffer, szFmtLB[LBITEM_DENIED], szFromIDS1(IDS_ERRENUMUSERS));

    if(szErrMessage = GetSystemErrMessage(nas))
    {
        lstrcat(szBuffer, TEXT(": "));
        lstrcat(szBuffer, szErrMessage);
        LocalFree((HLOCAL)szErrMessage);
    }

    ListBox_InsertString(GetWindow(hwndMain, GW_CHILD), 0, szBuffer);
    ListBox_SetItemData(GetWindow(hwndMain, GW_CHILD), 0, 0);
}
