/*
 *  utils.c
 *  
 *  Purpose:
 *      generic utils
 *  
 *  Owner:
 *      MikeSart
 */
#define UNICODE 1

#include <windows.h>
#include <windowsx.h>
#include <lm.h>
#include "netwatch.h"
#include "rcids.h"

void
RestoreWindowPosition(HWND hwnd)
{
    WINDOWPLACEMENT     wndpl;
    HKEY                hkey;
    DWORD               cb;
    DWORD               dwTopMost = 0;
    DWORD               fHideTitle;

    wndpl.showCmd = SW_SHOWNORMAL;
    if(!RegOpenKeyEx(HKEY_CURRENT_USER, szFromIDS1(IDS_KEY), 0, KEY_READ, &hkey))
    {
        cb = sizeof(fHideTitle);
        if(!RegQueryValueEx(hkey, szFromIDS1(IDS_HIDETITLE), 0, 0,
            (LPBYTE)&fHideTitle, &cb) && fHideTitle)
                ShowTitle(hwnd, SW_HIDE);

        cb = sizeof(wndpl);
        if(!RegQueryValueEx(hkey, szFromIDS1(IDS_PLACEMENT), 0, 0,
            (LPBYTE)&wndpl, &cb))
        {
            if(wndpl.length == sizeof(wndpl))
                SetWindowPlacement(hwnd, &wndpl);
            else
                wndpl.showCmd = SW_SHOWNORMAL;
        }

        cb = COUNT_CHECKMENUS * sizeof(unMenuFlags[0]);
        if(!RegQueryValueEx(hkey, szFromIDS1(IDS_MENUFLAGS), 0, 0,
            (LPBYTE)unMenuFlags, &cb))
                for(cb = 0; cb < COUNT_CHECKMENUS; cb++)
                {
                    CheckMenuItem(ghMenu, CHECKMENUSTART + cb,
                        MF_BYCOMMAND | unMenuFlags[cb]);
                }

        cb = sizeof(dwTimerInterval);
        RegQueryValueEx(hkey, szFromIDS1(IDS_TIMERINTERVAL), 0, 0,
            (LPBYTE)&dwTimerInterval, &cb);

        if(!RegQueryValueEx(hkey, szFromIDS1(IDS_TOPMOST), 0, 0,
            (LPBYTE)&dwTopMost, &cb) && cb && dwTopMost)
        {
            SetWindowPos(hwnd, HWND_TOPMOST, 0 ,0 ,0 ,0,
                SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            CheckMenuItem(ghMenu, IDM_TOPMOST,
                    MF_BYCOMMAND | MF_CHECKED);
        }
    }

    ShowWindow(hwnd, wndpl.showCmd);

    if(dwTimerInterval < 1000)
        dwTimerInterval = 1000;
}

void
SaveWindowPosition(HWND hwnd)
{
    WINDOWPLACEMENT wndpl;
    HKEY            hkey;
    DWORD           dwTopMost = 0;
    DWORD           fHideTitle;

    wndpl.length = sizeof(wndpl);
    GetWindowPlacement(hwnd, &wndpl);
    // for some reason, GetWindowPlacement resets the length part??
    wndpl.length = sizeof(wndpl);

    if(!RegCreateKeyEx(HKEY_CURRENT_USER, szFromIDS1(IDS_KEY), 0, szFromIDS1(IDS_KEY), 0,
        KEY_READ | KEY_WRITE, 0, &hkey, NULL))
    {
        RegSetValueEx(hkey, szFromIDS1(IDS_PLACEMENT), 0, REG_BINARY,
            (LPBYTE)&wndpl, sizeof(wndpl));

        RegSetValueEx(hkey, szFromIDS1(IDS_MENUFLAGS), 0, REG_BINARY,
            (LPBYTE)unMenuFlags, COUNT_CHECKMENUS * sizeof(unMenuFlags[0]));

        RegSetValueEx(hkey, szFromIDS1(IDS_TIMERINTERVAL), 0, REG_DWORD,
            (LPBYTE)&dwTimerInterval, sizeof(dwTimerInterval));

        fHideTitle = GetMenu(hwnd) ? 0 : 1;
        RegSetValueEx(hkey, szFromIDS1(IDS_HIDETITLE), 0, REG_DWORD,
            (LPBYTE)&fHideTitle, sizeof(fHideTitle));

        if(GetMenuState(ghMenu, IDM_TOPMOST, MF_BYCOMMAND) & MF_CHECKED)
            dwTopMost = 1;
        RegSetValueEx(hkey, szFromIDS1(IDS_TOPMOST), 0, REG_DWORD,
            (LPBYTE)&dwTopMost, sizeof(dwTopMost));
    }
}

// gets the string and allocs a block with
// just enough room for it.
TCHAR *
AllocAndLoadString(UINT unID)
{
    TCHAR   *sz;
    TCHAR   *szT = szFromIDS1(unID);

    sz = GlobalAllocPtr(GHND, (lstrlen(szT) + 1) * sizeof(TCHAR));
    if(sz)
        lstrcpy(sz, szT);

    return sz;
}    

TCHAR *
szFromIDS1(UINT unID)
{
    static TCHAR szBuf1[MAX_STRINGTABLE_LEN + 1];

    if(!LoadString(ghInst, unID, szBuf1, sizeof(szBuf1) / sizeof(TCHAR)))
        szBuf1[0] = '\0';
    return szBuf1;
}

TCHAR *
szFromIDS2(UINT unID)
{
    static TCHAR szBuf2[MAX_STRINGTABLE_LEN + 1];

    if(!LoadString(ghInst, unID, szBuf2, sizeof(szBuf2) / sizeof(TCHAR)))
        szBuf2[0] = '\0';
    return szBuf2;
}

TCHAR *
GetSystemErrMessage(DWORD dwError)
{
    LPTSTR  szErrMessage = NULL;
    HMODULE hLibrary;

    if(FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, dwError, 0/*LANG_USER_DEFAULT*/, (LPTSTR)&szErrMessage, 0, NULL))
    {
        szErrMessage[lstrlen(szErrMessage) - 2] = '\0'; // remove /r/n
    }
    else if(hLibrary = LoadLibrary(MESSAGE_FILENAME))
    {
        if(FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE,
            (LPVOID)hLibrary, dwError, 0, (LPTSTR)&szErrMessage, 0, NULL))
        {
            szErrMessage[lstrlen(szErrMessage) - 2] = '\0'; // remove /r/n
        }
        FreeLibrary(hLibrary);
    }

    return szErrMessage;
}

// TRUE: turn on timer
// FALSE: turn it off
void
PunchTimer(BOOL fNewState)
{
    static BOOL fCurrentState = FALSE;

    if(fCurrentState == fNewState)
        return;

    if(fNewState)
        SetTimer(hwndMain, TIMERID, dwTimerInterval, NULL);
    else
        KillTimer(hwndMain, TIMERID);

    fCurrentState = fNewState;
}

void
UpdateWindowText(HWND hwnd, BOOL fForceUpdate)
{
    static DWORD    dwNumUsersCache;

    if(fForceUpdate || dwNumUsersCache != dwNumUsers)
    {
        if(!szServerName)
            lstrcpy(szBuffer, szAppName);
        else if(dwNumUsers == 1)
            wsprintf(szBuffer, szFromIDS1(IDS_WINDOWTEXT1),
                szAppName, dwNumUsers, szServerName);
        else
            wsprintf(szBuffer, szFromIDS1(IDS_WINDOWTEXT),
                szAppName, dwNumUsers, szServerName);

        if(IsIconic(hwnd))
            InvalidateRect(hwnd, NULL, TRUE);

        SetWindowText(hwnd, szBuffer);

        dwNumUsersCache = dwNumUsers;
    }
}

void
ShowTitle(HWND hwnd, int nCmdShow)
{
    static HMENU    hMenu = NULL;
    DWORD           dwStyle;

    dwStyle = GetWindowLong(hwnd, GWL_STYLE);
    if(hMenu)
    {
        dwStyle = WS_TILEDWINDOW | dwStyle;
        SetMenu(hwnd, hMenu);
        hMenu = NULL;
    }
    else
    {
        dwStyle &= ~(WS_DLGFRAME | WS_SYSMENU |
            WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

        hMenu = GetMenu(hwnd);
        SetMenu(hwnd, NULL);
    }

    SetWindowLong(hwnd, GWL_STYLE, dwStyle);
    SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE |
        SWP_NOZORDER | SWP_FRAMECHANGED);

    ShowWindow(hwnd, nCmdShow);
}

// takes whatever is in szServerName and puts it
// in the caption - returns nas if it can't find szNewServerName
// assumes szNewServerName has \\ in front of it
NET_API_STATUS
SetWindowTextAndServerName(HWND hwnd, LPTSTR szNewServerName)
{
    SERVER_INFO_100     *si100 = NULL;
    SHARE_INFO_2        *shi2 = NULL;
    LPTSTR              szT = NULL;
    NET_API_STATUS      nas;
    DWORD               dwDummy;

    SetCursor(LoadCursor(NULL, IDC_WAIT));

    nas = NetServerGetInfo(szNewServerName, 100, (LPBYTE *)&si100);
    if(nas || !si100 || !si100[0].sv100_name)
        goto err;

    // make room for the \\ that NetServerGetInfo doesn't add
    szT = GlobalAllocPtr(GHND, 
        (lstrlen(si100[0].sv100_name) + 3) * sizeof(TCHAR));
    if(!szT)
        goto err;

    szT[0] = szT[1] = '\\';
    lstrcpy(&szT[2], si100[0].sv100_name);

    nas = NetShareEnum(szT, 2, (LPBYTE *)&shi2, 
        MAX_PREFERRED_LENGTH, &dwDummy, &dwDummy, NULL);
    if(nas)
        goto err;

    GlobalFreeNullPtr(szServerName);
    szServerName = szT;

    UpdateWindowText(hwndMain, TRUE);
    PostMessage(hwndMain, WM_TIMER, 0, 0L);

err:
    SetCursor(LoadCursor(NULL, IDC_ARROW));

    if(nas)
    {
        TCHAR   *szErrMessage;

        if(nas == ERROR_BAD_NETPATH)
        {
            wsprintf(szBuffer, szFromIDS1(IDS_COMPNOTFOUND), szNewServerName);
            MessageBox(hwnd, szBuffer, szAppName, MB_ICONEXCLAMATION);
        }
        else if(szErrMessage = GetSystemErrMessage(nas))
        {
            MessageBox(hwnd, szErrMessage, szAppName, MB_ICONEXCLAMATION);
            LocalFree((HLOCAL)szErrMessage);
        }
        else
            MessageBox(hwnd, szFromIDS1(IDS_ERRMEMORY), szAppName, MB_ICONEXCLAMATION);
    }

    NetApiBufferFree(si100);
    NetApiBufferFree(shi2);
    return nas;
}
