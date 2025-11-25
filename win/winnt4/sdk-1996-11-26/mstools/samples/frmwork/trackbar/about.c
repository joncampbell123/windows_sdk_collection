// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993-1996  Microsoft Corporation.  All Rights Reserved.
//
//  MODULE:   about.c
//
//  PURPOSE:   Displays the "About" dialog box
//
//  FUNCTIONS:
//    CmdAbout        - Displays the "About" dialog box
//    About           - Processes messages for "About" dialog box.
//    MsgAboutInit    - To initialize the about box with version info
//                      from resources.
//    MsgAboutCommand - Process WM_COMMAND message sent to the about box.
//    CmdAboutDone    - Free the about box and related data.
//
//  COMMENTS:
//
//

#include <windows.h>            // required for all Windows applications
#include <windowsx.h>

#include "globals.h"            // prototypes specific to this application
#include "resource.h"


LRESULT CALLBACK About(HWND, UINT, WPARAM, LPARAM);

LRESULT MsgAboutInit(HWND, UINT, WPARAM, LPARAM);
LRESULT MsgAboutCommand(HWND, UINT, WPARAM, LPARAM);
LRESULT CmdAboutDone(HWND, WORD, WORD, HWND);

// About dialog message table definition.
MSD g_rgmsdAbout[] =
{
    {WM_COMMAND,    MsgAboutCommand},
    {WM_INITDIALOG, MsgAboutInit}
};

MSDI g_msdiAbout =
{
    sizeof(g_rgmsdAbout) / sizeof(MSD),
    g_rgmsdAbout,
    edwpNone
};

// About dialog command table definition.
CMD g_rgcmdAbout[] =
{
    {IDOK,     CmdAboutDone},
    {IDCANCEL, CmdAboutDone}
};

CMDI g_cmdiAbout =
{
    sizeof(g_rgcmdAbout) / sizeof(CMD),
    g_rgcmdAbout,
    edwpNone
};

//
//  FUNCTION: CmdAbout(HWND, WORD, WORD, HWND)
//
//  PURPOSE: Displays the "About" dialog box
//
//  PARAMETERS:
//    hwnd      - Window handle
//    wCommand  - IDM_ABOUT (unused)
//    wNotify   - Notification number (unused)
//    hwndCtrl  - NULL (unused)
//
//  RETURN VALUE:
//
//    Always returns 0 - Message handled
//
//  COMMENTS:
//    To process the IDM_ABOUT message, call DialogBox() to display the
//    about dialog box.

LRESULT CmdAbout(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUT), hwnd, (DLGPROC)About);
    return 0;
}


//
//  FUNCTION: About(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for "About" dialog box.
//
//  PARAMETERS:
//    hdlg - window handle of the dialog box
//    wMessage - type of message
//    wParam - message-specific information
//    lParam - message-specific information
//
//  RETURN VALUE:
//    TRUE - message handled
//    FALSE - message not handled
//
//  COMMENTS:
//
//     Display version information from the version section of the
//     application resource.
//
//     Wait for user to click on "Ok" button, then close the dialog box.
//

LRESULT CALLBACK About(HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
    return DispMessage(&g_msdiAbout, hDlg, uMessage, wParam, lParam);
}


//
//  FUNCTION: MsgAboutInit(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: To initialize the about box with version info from resources.
//
//  PARAMETERS:
//    hwnd - The window handing the message.
//    uMessage - The message number. (unused).
//    wParam - Message specific data (unused).
//    lParam - Message specific data (unused).
//
//  RETURN VALUE:
//    Always returns 0 - message handled.
//
//  COMMENTS:
//    Uses the version apis to retrieve version information for
//    each of the static text boxes in the about box.
//

LRESULT MsgAboutInit(HWND hdlg, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
    char  szFullPath[256];
    DWORD dwVerHnd;
    DWORD dwVerInfoSize;

    // Center the dialog over the application window
    CenterWindow(hdlg, GetWindow(hdlg, GW_OWNER));

    // Get version information from the application
    GetModuleFileName(g_hInst, szFullPath, sizeof(szFullPath));
    dwVerInfoSize = GetFileVersionInfoSize(szFullPath, &dwVerHnd);
    if (dwVerInfoSize)
    {
        // If we were able to get the information, process it:
        HANDLE  hMem;
        LPVOID  lpvMem;
        char    szGetName[256];
        int     cchRoot;
        int     i;

        hMem = GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
        lpvMem = GlobalLock(hMem);
        GetFileVersionInfo(szFullPath, dwVerHnd, dwVerInfoSize, lpvMem);

		if (LANG_JAPANESE == PRIMARYLANGID(GetUserDefaultLangID()))
			lstrcpy(szGetName, "\\StringFileInfo\\041104E4\\");
		else
			lstrcpy(szGetName, "\\StringFileInfo\\040904E4\\");
        cchRoot = lstrlen(szGetName);

        // Walk through the dialog items that we want to replace:
        for (i = IDD_VERFIRST; i <= IDD_VERLAST; i++)
        {
            BOOL  fRet;
            UINT  cchVer = 0;
            LPSTR lszVer = NULL;
            char  szResult[256];

            GetDlgItemText(hdlg, i, szResult, sizeof(szResult));
            lstrcpy(&szGetName[cchRoot], szResult);
            fRet = VerQueryValue(lpvMem, szGetName, &lszVer, &cchVer);

            if (fRet && cchVer && lszVer)
            {
                // Replace dialog item text with version info
                lstrcpy(szResult, lszVer);
                SetDlgItemText(hdlg, i, szResult);
            }
        }
        GlobalUnlock(hMem);
        GlobalFree(hMem);
    }
    return TRUE;
}

//
//  FUNCTION: MsgAboutCommand(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Process WM_COMMAND message sent to the about box.
//
//  PARAMETERS:
//    hwnd - The window handing the message.
//    uMessage - The message number. (unused).
//    wParam - Message specific data (unused).
//    lParam - Message specific data (unused).
//
//  RETURN VALUE:
//    Always returns 0 - message handled.
//
//  COMMENTS:
//    Uses this DipsCommand function defined in wndproc.c combined
//    with the cmdiAbout structure defined in this file to handle
//    the command messages for the about dialog box.
//

LRESULT MsgAboutCommand(HWND   hwnd,
                        UINT   uMessage,
                        WPARAM wParam,
                        LPARAM lParam)
{
    return DispCommand(&g_cmdiAbout, hwnd, wParam, lParam);
}

//
//  FUNCTION: CmdAboutDone(HWND, WORD, HWND)
//
//  PURPOSE: Free the about box and related data.
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

LRESULT CmdAboutDone(HWND hdlg, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    EndDialog(hdlg, TRUE);          // Exit the dialog
    return TRUE;
}

