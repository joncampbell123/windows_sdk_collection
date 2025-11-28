/*==========================================================================
 *
 *  Copyright (C) 1995-1996 Microsoft Corporation. All Rights Reserved.
 *
 *  File:       wizard.h
 *  Content:    user input routines include file
 *
 *
 ***************************************************************************/
#define IDIRECTPLAY2_OR_GREATER
#include <commctrl.h>
#include <dplay.h>
#include "duel.h"

void    SetupFonts(HDC hDC);

void    CleanupFonts(void);

LONG    RegSet(LPCTSTR lptszName, CONST BYTE * lpData, DWORD dwSize);

LONG    RegSetA(LPCTSTR lptszName, CONST BYTE * lpData, DWORD dwSize);

LONG    RegGet(LPCTSTR lptszName, LPBYTE lpData, LPDWORD lpdwDataSize);

DWORD   WINAPI DoWizard(LPVOID pv);

BOOL    WINAPI  EnumSP(LPGUID lpGuid, LPTSTR lpDesc, DWORD dwJamorVersion,
                       DWORD dwMinorVersion, LPVOID lpv);
BOOL    CALLBACK DlgProcChooseProvider(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

BOOL    WINAPI  EnumSession(LPCDPSESSIONDESC2 lpDPSessionDesc, LPDWORD lpdwTimeOut, 
                           DWORD dwFlags,  LPVOID pContext);

BOOL    CALLBACK DlgProcGameSetup (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

BOOL    WINAPI EnumPlayer(DPID pidID, DWORD dwPlayerType, LPCDPNAME lpName,
    DWORD dwFlags, LPVOID lpContext);

BOOL    CALLBACK DlgProcJoinSession (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

BOOL    CALLBACK DlgProcHostSession (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

BOOL    InitTreeViewImageLists(HWND hwndTV);

HTREEITEM AddItemToTree(HWND hwndTV, LPTSTR lpszItem, DWORD dwData, int nLevel);

void    ReleaseSessionData(HWND hWndCtl);

void    ReleaseSPData(void);
