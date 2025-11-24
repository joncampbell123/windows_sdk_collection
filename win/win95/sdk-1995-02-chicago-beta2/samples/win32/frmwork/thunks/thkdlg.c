// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993-1995  Microsoft Corporation.  All Rights Reserved.
//
//  MODULE:   thkdlg.c
//
//  PURPOSE:   Displays the "Thunk" dialog box
//
//  FUNCTIONS:
//    CmdThunk        - Displays the "Thunk" dialog box
//    Thunk           - Processes messages for "Thunk" dialog box.
//    MsgThunkInit    - To initialize the about box with version info
//                      from resources.
//    MsgThunkCommand - Process WM_COMMAND message sent to the about box.
//    CmdThunkDone    - Free the about box and related data.
//
//  COMMENTS:
//
//

#include <windows.h>            // required for all Windows applications
#include <windowsx.h>
#include "globals.h"            // prototypes specific to this application
#include "dll32.h"


LRESULT CALLBACK ThunkDlg(HWND, UINT, WPARAM, LPARAM);
LRESULT MsgThunkInit(HWND, UINT, WPARAM, LPARAM);
LRESULT MsgThunkCommand(HWND, UINT, WPARAM, LPARAM);
LRESULT CmdThunkDone(HWND, WORD, WORD, HWND);
LRESULT CmdThunk(HWND hdlg, WORD wCommand, WORD wNotify, HWND hwndCtrl);

// Thunk dialog message table definition.
MSD rgmsdThunk[] =
{
    {WM_COMMAND,    MsgThunkCommand},
    {WM_INITDIALOG, MsgThunkInit}
};

MSDI msdiThunk =
{
    sizeof(rgmsdThunk) / sizeof(MSD),
    rgmsdThunk,
    edwpNone
};

// Thunk dialog command table definition.
CMD rgcmdThunk[] =
{
    {IDOK,     CmdThunk},       // OK to thunk
    {IDCANCEL, CmdThunkDone}
};

CMDI cmdiThunk =
{
    sizeof(rgcmdThunk) / sizeof(CMD),
    rgcmdThunk,
    edwpNone
};


//
//  FUNCTION: CmdThunkDlg(HWND, WORD, WORD, HWND)
//
//  PURPOSE: Displays the "Thunk" dialog box
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

LRESULT CmdThunkDlg(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    DialogBox(hInst, "ThunkDlg", hwnd, (DLGPROC)ThunkDlg);
    return 0;
}


//
//  FUNCTION: ThunkDlg(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for "Thunk" dialog box.
//
//  PARAMETERS:
//    hdlg     - window handle of the dialog box
//    wMessage - type of message
//    wparam   - message-specific information
//    lparam   - message-specific information
//
//  RETURN VALUE:
//    TRUE  - message handled
//    FALSE - message not handled
//
//  COMMENTS:
//
//     Display version information from the version section of the
//     application resource.
//
//     Wait for user to click on "Ok" button, then close the dialog box.
//

LRESULT CALLBACK ThunkDlg(HWND hdlg, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    return DispMessage(&msdiThunk, hdlg, uMessage, wparam, lparam);
}


//
//  FUNCTION: MsgThunkInit(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: To initialize the about box with version info from resources.
//
//  PARAMETERS:
//    hwnd     - The window handing the message.
//    uMessage - The message number. (unused).
//    wparam   - Message specific data (unused).
//    lparam   - Message specific data (unused).
//
//  RETURN VALUE:
//    Always returns 0 - message handled.
//
//  COMMENTS:
//    Uses the version apis to retrieve version information for
//    each of the static text boxes in the about box.
//

LRESULT MsgThunkInit(HWND hdlg, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    return TRUE;
}

//
//  FUNCTION: MsgThunkCommand(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Process WM_COMMAND message sent to the about box.
//
//  PARAMETERS:
//    hwnd     - The window handing the message.
//    uMessage - The message number. (unused).
//    wparam   - Message specific data (unused).
//    lparam   - Message specific data (unused).
//
//  RETURN VALUE:
//    Always returns 0 - message handled.
//
//  COMMENTS:
//    Uses this DipsCommand function defined in wndproc.c combined
//    with the cmdiThunk structure defined in this file to handle
//    the command messages for the about dialog box.
//

LRESULT MsgThunkCommand(HWND   hwnd,
                        UINT   uMessage,
                        WPARAM wparam,
                        LPARAM lparam)
{
    return DispCommand(&cmdiThunk, hwnd, wparam, lparam);
}

//
//  FUNCTION: CmdThunkDone(HWND, WORD, HWND)
//
//  PURPOSE: Free the about box and related data.
//
//  PARAMETERS:
//    hwnd     - The window handling the command.
//    wCommand - The command to be handled (unused).
//    hwndCtrl - NULL (unused).
//
//  RETURN VALUE:
//    Always returns TRUE.
//
//  COMMENTS:
//    Calls EndDialog to finish the dialog session.
//

LRESULT CmdThunkDone(HWND hdlg, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    EndDialog(hdlg, TRUE);          // Exit the dialog
    return TRUE;
}


//
//  FUNCTION: CmdThunk(HWND, WORD, HWND)
//
//  PURPOSE: Call the 32-bit dll which thunks to the 16-bit dll.
//
//  PARAMETERS:
//    hwnd     - The window handling the command.
//    wCommand - The command to be handled (unused).
//    hwndCtrl - NULL (unused).
//
//  RETURN VALUE:
//    Always returns TRUE.
//
//  COMMENTS:
//    Calls EndDialog to finish the dialog session.
//

LRESULT CmdThunk(HWND hdlg, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    DLL32Call(hdlg);
    return TRUE;
}
