// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1995  Microsoft Corporation.  All Rights Reserved.
//
//  MODULE: statbar.c
//
//  PURPOSE: Handles general routines for the TapiComm sample.
//
//  FUNCTIONS:
//    MsgTimer      - Handles the WM_TIMER messages to set the time on
//                    the status bar.
//    MsgMouseMove  - Handles the WM_MOUSEMOVE to display the cursor position.
//    MsgMenuSelect - Handle the WM_MENUSELECT message. This message will
//                    enable the status bar control to update when the user
//                    moves across menu items on the main window.
//    InitializeStatusBar - Sets the pane positions in the statusbar control
//    CreateSBar    - Calls CreateStatusWindow() to create the status bar
//    UpdateStatusBar - Updates the statusbar control with appropriate text
//

#include <windows.h>            // required for all Windows applications
#include <windowsx.h>
#include <commctrl.h>           // prototypes and defs for common controls
#include "globals.h"            // prototypes specific to this application
#include "statbar.h"            // prototypes specific to statbar.c


// Global Variables for the status bar control.

HWND  hWndStatusbar;

//  **TODO**  Add entries to the string table in TapiComm.rc for each menu
//            command.  MsgMenuSelect (below) loads these strings to display
//            information in the status bar.  MsgMenuSelect assumes that the
//            string ID is the same as the command ID and that a string
//            exists for every command.
//
// The following array contains resource string ID's for popup menus
// in the main application menu.  This array is used by MsgMenuSelect
// to display information in the status bar.
//
//  **TODO**  Add entries to this array for each popup menu in the same
//            positions as they appear in the main menu.  Remember to define
//            the ID's in globals.h and add the strings to TapiComm.rc.

UINT idPopup[] =
{
    IDS_FILEMENU,
    IDS_EDITMENU,
    IDS_HELPMENU,
};



//
//  FUNCTION: MsgMenuSelect(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Upadates menu selections on the staus bar.
//
//
//  PARAMETERS:
//
//    hwnd      - Window handle  (Used)
//    uMessage  - Message number (Used)
//    wparam    - Extra data     (Used)
//    lparam    - Extra data     (Used)
//
//  RETURN VALUE:
//
//    Always returns 0 - Message handled
//
//  COMMENTS:
//    This message is sent when the user selects menu items by
//    by pulling down  a popup menu move the mouse around to highlite
//    different menu items.
//
//

#define MENUHACK 1              // Menu Select message has a bug..so workaround

LRESULT MsgMenuSelect(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    static char szBuffer[128];
    UINT   nStringID = 0;
    UINT   fuFlags = GET_WM_MENUSELECT_FLAGS(wparam, lparam) & 0xffff;
    UINT   uCmd    = GET_WM_MENUSELECT_CMD(wparam, lparam);
    HMENU  hMenu   = GET_WM_MENUSELECT_HMENU(wparam, lparam);


    szBuffer[0] = 0;                            // First reset the buffer


    if (fuFlags == 0xffff && hMenu == NULL)     // Menu has been closed
        nStringID = IDS_DESCRIPTION;

    else if (fuFlags & MFT_SEPARATOR)           // Ignore separators
        nStringID = 0;

    else if (fuFlags & MF_POPUP)                // Popup menu
    {
#ifdef MENUHACK                 // This code shouldn't be necessary...
        UINT i;
        for (i = 0; i < sizeof(idPopup)/sizeof(idPopup[0]); i++)
        {
            if(GetSubMenu(hMenu, i) == (HMENU)uCmd)
            {
                uCmd = i;
                break;
            }
        }
#endif
        if (fuFlags & MF_SYSMENU)               // System menu
            nStringID = IDS_SYSMENU;

        else
            // Get string ID for popup menu from idPopup array.
            nStringID = ((uCmd < sizeof(idPopup)/sizeof(idPopup[0])) ?
                            idPopup[uCmd] : 0);
    }  // for MF_POPUP

    else                                        // Must be a command item
        nStringID = uCmd;                       // String ID == Command ID

    // Load the string if we have an ID
    if (0 != nStringID)
        LoadString(hInst, nStringID, szBuffer, sizeof(szBuffer));

    // Finally... send the string to the status bar
    UpdateStatusBar(szBuffer, 0, 0);

    return 0;
}


//
//  FUNCTION: InitializeStatusBar(HWND)
//
//  PURPOSE:  Initialize statusbar control with time and mouse positions.
//
//
//  PARAMETERS:
//
//  hwndParent - Window handle of the status bar's parent
//
//
//  RETURN VALUE:  NONE
//
//
//  COMMENTS:
//
//   This function initializes the time  and mouse positions sections of
//   the statubar window. The Date for the time section is obtained by
//   calling SetTimer API. When the timer messages start comming in,
//   GetSytemTime() to fill the time section.
//   The WPARAM of SB_SETTEXT is divided into 2 parameters. The LOWORD
//   determines which section/part the text goes into, and the HIWORD
//   tells how the bar is drawn (popin or popout).
//

void InitializeStatusBar(HWND hwndParent)
{
    const cSpaceInBetween = 8;
    int   ptArray[2];   // Array defining the number of parts/sections
    SIZE  size = {0,0};         // the Status bar will display.
    RECT  rect;
    HDC   hDC;

   /*
    * Fill in the ptArray...
    */

    hDC = GetDC(hwndParent);
    GetClientRect(hwndParent, &rect);
    ReleaseDC(hwndParent, hDC);

    ptArray[0] = (rect.right) / 2;
    ptArray[1] = rect.right;

    SendMessage(hWndStatusbar,
                SB_SETPARTS,
                sizeof(ptArray)/sizeof(ptArray[0]),
                (LPARAM)(LPINT)ptArray);

    UpdateStatusBar(SZDESCRIPTION, 0, 0);
}


//
//  FUNCTION: CreateSBar(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Calls CreateStatusWindow() to create the status bar
//
//
//  PARAMETERS:
//
//  hwndParent - Window handle of the status bar's parent
//
//  RETURN VALUE:
//
//  If both controls were created successfully Return TRUE,
//  else returns FALSE.
//
//  COMMENTS:
//
//

BOOL CreateSBar(HWND hwndParent)
{
    hWndStatusbar = CreateStatusWindow(WS_CHILD | WS_VISIBLE | WS_BORDER,
                                       SZDESCRIPTION,
                                       hwndParent,
                                       IDM_STATUSBAR);
    if(hWndStatusbar)
    {
        InitializeStatusBar(hwndParent);
        return TRUE;
    }

    return FALSE;
}


//
//  FUNCTION: UpdateStatusBar(HWND)
//
//  PURPOSE:  Updates the statusbar control with appropriate text
//
//
//  PARAMETERS:
//
//  lpszStatusString - text to be displayed
//  partNumber       - which part of the status bar to display text in
//  displayFlags     - display flags
//
//
//  RETURN VALUE: NONE
//
//
//  COMMENTS:
//      None
//
//

void UpdateStatusBar(LPSTR lpszStatusString, WORD partNumber, WORD displayFlags)
{
    SendMessage(hWndStatusbar,
                SB_SETTEXT,
                partNumber | displayFlags,
                (LPARAM)lpszStatusString);
}
