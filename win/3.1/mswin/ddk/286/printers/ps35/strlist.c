/**[f******************0.************************************************
 * strlist.c - 
 *
 * Copyright (C) 1988 Aldus Corporation.  All rights reserved.
 * Copyright (C) 1989 Microsoft Corporation.
 * Company confidential.
 *
 **f]*****************************************************************/

/*********************************************************************
 * STRLIST.C
 *
 * 90-08-14	kensy   Created module
 *   
 *********************************************************************/

#include "pscript.h"
#include "strlist.h"
#include "etm.h"
#include "truetype.h"
#include "psdata.h"

LONG NEAR PASCAL FindStringIndex(HANDLE hStrList, LPSTR lpStr);

#define WS_STRLIST (WS_POPUP | LBS_NOREDRAW | LBS_SORT)
//#define WS_STRLIST (WS_POPUP | DS_MODALFRAME | WS_VISIBLE | LBS_STANDARD)

HANDLE FAR PASCAL CreateStringList(void)
{
    HWND hwnd;

    hwnd = CreateWindow("LISTBOX", szNull, WS_STRLIST, 0, 0, 0, 0, NULL, NULL, 
                        ghInst, NULL);

#if 0
    if (hwnd) {
        char s[40];

        wsprintf(s, "%x", hwnd);
        SetWindowText(hwnd, s);

        ShowWindow(hwnd, SW_SHOWNORMAL);
    }
#endif

    return hwnd;
}


void FAR PASCAL DeleteStringList(HANDLE hStrList)
{
    EmptyStringList(hStrList);
    DestroyWindow(hStrList);
}

void FAR PASCAL EmptyStringList(HANDLE hStrList)
{
    int nItems, i;
    DWORD dwItem;
    LPDF lpdfBase;

    nItems = (int) SendMessage(hStrList, LB_GETCOUNT, 0, 0L);
    
    for (i = 0; i < nItems; ++i) {
        dwItem = SendMessage(hStrList, LB_GETITEMDATA, i, 0L);
        if (LOWORD(dwItem))  {
            lpdfBase = *((LPDF *)LocalLock(LOWORD(dwItem)));
            if (lpdfBase) {
                EngineDeleteFont(lpdfBase);
                GlobalFree(LOWORD(GlobalHandle(HIWORD((DWORD)lpdfBase))));
            }
            LocalUnlock(LOWORD(dwItem));
            LocalFree(LOWORD(dwItem));
        }
    }

    SendMessage(hStrList, LB_RESETCONTENT, 0, 0L);
}


BOOL FAR PASCAL AddString(HANDLE hStrList, LPSTR lpStr, DWORD dwData)
{
    int i;

    if (IsStringInList(hStrList, lpStr))
        return TRUE;

    if ((i = (int)SendMessage(hStrList, LB_ADDSTRING, 0, (DWORD) lpStr)) < 0)
        return FALSE;

    SendMessage(hStrList, LB_SETITEMDATA, i, dwData);

    return TRUE;
}


BOOL FAR PASCAL DeleteString(HANDLE hStrList, LPSTR lpStr)
{
    int iItem;
    DWORD dwItem;

    iItem = (int)FindStringIndex(hStrList, lpStr);
    if (iItem < 0)
        return FALSE;

    dwItem = SendMessage(hStrList, LB_GETITEMDATA, iItem, 0L);
    if (LOWORD(dwItem))
        LocalFree(LOWORD(dwItem));

    return SendMessage(hStrList, LB_DELETESTRING, iItem, 0L) >= 0;
}


BOOL FAR PASCAL EnumStrings(HANDLE hStrList, LPSTR lpStr, int nMaxLen)
{
    static HANDLE hCurStrList;
    static int iCurItem, nItems;

    if (hStrList) {
        hCurStrList = hStrList;
        iCurItem = 0;
        nItems = (int) SendMessage(hStrList, LB_GETCOUNT, 0, 0L);
    }

    if (iCurItem >= nItems)
        return FALSE;

    return SendMessage(hCurStrList, LB_GETTEXT, iCurItem++, (DWORD) lpStr) >= 0;
}


BOOL FAR PASCAL IsStringInList(HANDLE hStrList, LPSTR lpStr)
{
    return FindStringIndex(hStrList, lpStr) >= 0;
}


DWORD FAR PASCAL GetStringData(HANDLE hStrList, LPSTR lpStr)
{
    int i;

    i = (int)FindStringIndex(hStrList, lpStr);

    if (i < 0)
        return -1L;

    return SendMessage(hStrList, LB_GETITEMDATA, i, 0L);
}


DWORD FAR PASCAL SetStringData(HANDLE hStrList, LPSTR lpStr, DWORD dwNewData)
{
    int i;

    i = (int)FindStringIndex(hStrList, lpStr);

    if (i < 0)
        return -1L;

    return SendMessage(hStrList, LB_SETITEMDATA, i, dwNewData);
}


LONG NEAR PASCAL FindStringIndex(HANDLE hStrList, LPSTR lpStr)
{
    char szBuf[128];
    int i;

    i = (int)SendMessage(hStrList, LB_FINDSTRING, -1, (DWORD)lpStr);

    /* if prefix not found then string doesn't exist */
    if (i < 0)
        return -1L;

    /* get the entry and compare to the string we're looking for */
    SendMessage(hStrList, LB_GETTEXT, i, (DWORD)((LPSTR)szBuf));
    return lstrcmpi(lpStr, szBuf) ? -1L : (DWORD)i;
}

