// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
//
// Copyright (C) 1995-1996  Microsoft Corporation.  All Rights Reserved.
//
//  MODULE: TapiComm.c
//
//  PURPOSE: Handles general routines for the TapiComm sample.
//
//  FUNCTIONS:
//    WndProc     - Processes messages for the main window.
//    MsgCommand  - Handle the WM_COMMAND messages for the main window.
//    MsgCreate   - Handle the WM_CREATE messages for the main window.
//    MsgSize     - Handles the WM_SIZE message by calling SendMessage() to
//                  pass the WM_SIZE message onto the status bar and tool bar
//                  controls. 
//    MsgDestroy  - Handles the WM_DESTROY message by calling 
//                  PostQuitMessage().
//    CmdExit     - Handles the file exit command by calling destory 
//                  window on the main window.
//
//  COMMENTS:
//    Message dispatch table -
//      For every message to be handled by the main window procedure
//      place the message number and handler function pointer in
//      rgmsd (the message dispatch table).  Place the prototype
//      for the function in globals.h and the definition of the
//      function in the appropriate module.
//    Command dispatch table -
//      For every command to be handled by the main window procedure
//      place the command number and handler function pointer in
//      rgcmd (the command dispatch table).  Place the prototype
//      for the function in globals.h and the definition of the
//      function in the appropriate module.
//    Globals.h Contains the definitions of the structures and dispatch.c
//      contains the functions that use these structures.
//



#include <windows.h>            // required for all Windows applications
#include <windowsx.h>
#include <commctrl.h>           // prototypes and defs for common controls
#include "globals.h"            // prototypes specific to this application
#include "statbar.h"            // prototypes specific to statbar.c
#include "toolbar.h"            // prototypes specific to toolbar.c
#include "EditCtls.h"
#include "TapiCode.h"
#include "resource.h"

LRESULT CmdCreateFile1(HWND hWnd, WORD wCommand, WORD wNotify, HWND hwndCtrl);
LRESULT CmdCreateFile2(HWND hWnd, WORD wCommand, WORD wNotify, HWND hwndCtrl);


// Main window message table definition.
MSD rgmsd[] =
{
    {WM_COMMAND,    MsgCommand   },
    {WM_MENUSELECT, MsgMenuSelect},
    {WM_SIZE,       MsgSize      },
    {WM_NOTIFY,     MsgNotify    },
    {WM_CLOSE,      MsgClose     },
    {WM_CREATE,     MsgCreate    },
    {WM_SETFOCUS,   MsgSetFocus  },
    //{WM_PAINT,      MsgPaint     },
    {WM_DESTROY,    MsgDestroy   }
};

MSDI msdiMain =
{
    sizeof(rgmsd) / sizeof(MSD),
    rgmsd,
    edwpWindow
};


// Main window command table definition.
CMD rgcmd[] =
{
    {IDM_MAKECALL,    CmdMakeCall},
    {IDM_HANGUPCALL,  CmdHangupCall},
    {IDM_EXIT,        CmdExit},

    {IDM_EDITUNDO,    CmdStub},
    {IDM_EDITCUT,     CmdStub},
    {IDM_EDITCOPY,    CmdStub},
    {IDM_EDITPASTE,   CmdStub},
    {IDM_EDITCLEAR,   CmdStub},

    {IDM_ABOUT,       CmdAbout},
};

CMDI cmdiMain =
{
    sizeof(rgcmd) / sizeof(CMD),
    rgcmd,
    edwpWindow
};


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  PARAMETERS:
//    hwnd     - window handle
//    uMessage - message number
//    wparam   - additional information (dependant on message number)
//    lparam   - additional information (dependant on message number)
//
//  RETURN VALUE:
//    The return value depends on the message number.  If the message
//    is implemented in the message dispatch table, the return value is
//    the value returned by the message handling function.  Otherwise,
//    the return value is the value returned by the default window procedure.
//
//  COMMENTS:
//    Call the DispMessage() function with the main window's message dispatch
//    information (msdiMain) and the message specific information.
//

LRESULT CALLBACK WndProc(HWND   hwnd, 
                         UINT   uMessage, 
                         WPARAM wparam, 
                         LPARAM lparam)
{
    return DispMessage(&msdiMain, hwnd, uMessage, wparam, lparam);
}


//
//  FUNCTION: MsgCommand(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Handle the WM_COMMAND messages for the main window.
//
//  PARAMETERS:
//    hwnd     - window handle
//    uMessage - WM_COMMAND (Unused)
//    GET_WM_COMMAND_ID(wparam, lparam)   - Command identifier
//    GET_WM_COMMAND_HWND(wparam, lparam) - Control handle
//
//  RETURN VALUE:
//    The return value depends on the message number.  If the message
//    is implemented in the message dispatch table, the return value is
//    the value returned by the message handling function.  Otherwise,
//    the return value is the value returned by the default window procedure.
//
//  COMMENTS:
//    Call the DispCommand() function with the main window's command dispatch
//    information (cmdiMain) and the command specific information.
//

LRESULT MsgCommand(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    return DispCommand(&cmdiMain, hwnd, wparam, lparam);
}


//
//  FUNCTION: MsgCreate(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Handle the WM_CREATE messages for the main window.
//           and call InitCommonControls() API to initialize the
//           common control library. 
//
//  PARAMETERS:
//    hwnd     - window handle
//
//  RETURN VALUE:
//    Return 0 if the StatusBar and ToolBar Windows could be created
//    successfully. Otherwise, returns -1 to abort the main window
//    creation.
//
//  COMMENTS:
//    Call the CreateTSBars function with the main window's window handle
//    information (msdiMain). 
//
//    Must also initialize TAPI.
//

LRESULT MsgCreate(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    InitCommonControls() ; // Initialize the common control library.

    if (InitEditCtls(hwnd) == FALSE)
        return -1;

    InitializeTAPI(hwnd);

    if(!(CreateTBar(hwnd) && CreateSBar(hwnd)))
        return -1;   // Tool and status bars were not created, so return -1.

    UpdateStatusBar("Ready to make a call.",1,0);

    EnableMakeCall(hwnd, TRUE);
    EnableHangupCall(hwnd, FALSE);
    return 0;
}


//
//  FUNCTION: MsgSize(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  This function resizes the toolbar and statusbar controls. 
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
//
//    When the window procdure that has the status and tool bar controls
//    receive the WM_SIZE message, it has to pass the message on to these 
//    controls so that these controls can adjust their size accordingly. 
//   
//    It also has to resize the edit controls that are the UI for TAPI.
//
//

LRESULT MsgSize(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam) 
{
    SendMessage(hWndStatusbar,  uMessage, wparam, lparam);
    SendMessage(hWndToolbar, uMessage, wparam, lparam);

    // Re-position the panes in the status bar
    InitializeStatusBar(hwnd);

    SizeEditCtls();

    return 0 ;
}


//
//  FUNCTION: MsgSetFocus(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  This function puts the focus where is should be.
//
//
//  PARAMETERS:
//
//    hwnd      - Window handle  (Used)
//    uMessage  - Identifies window that lost focus (Used)
//    wparam    - Extra data     (Used)
//    lparam    - Extra data     (Used)
//
//  RETURN VALUE:
//
//    Always returns 0 - Message handled
//
//  COMMENTS:
//
//    Just signal the edit controls to set the focus where it belongs.
//
//

LRESULT MsgSetFocus(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam) 
{
    SetFocusEditCtls();

    return 0 ;
}


//
//  FUNCTION: MsgDestroy(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Calls PostQuitMessage().
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

LRESULT MsgDestroy(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    PostQuitMessage(0);
    return 0;
}


//
//  FUNCTION: MsgClose(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Exits the application.
//
//  PARAMETERS:
//
//    hwnd      - The window.
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
//    Make sure TAPI is stopped before exiting.
//
//

LRESULT MsgClose(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    ShutdownTAPI();

    DestroyWindow(hwnd);
    return 0;
}


//
//  FUNCTION: MsgPaint(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Paints the client area of the window.
//
//  PARAMETERS:
//
//    hwnd      - The window.
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
//    Not sure what needs to be painted, maybe text
//    labeling the edit controls?
//
//

LRESULT MsgPaint(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    HDC hdc;
    PAINTSTRUCT ps;

    hdc = BeginPaint(hwnd, &ps);

    EndPaint(hwnd, &ps);
    return 0;
}


//
//  FUNCTION: CmdExit(HWND, WORD, WORD, HWND)
//
//  PURPOSE: Exit the application.
//
//  PARAMETERS:
//    hwnd     - The window.
//    wCommand - IDM_EXIT (unused)
//    wNotify  - Notification number (unused)
//    hwndCtrl - NULL (unused)
//
//  RETURN VALUE:
//    Always returns 0 - command handled.
//
//  COMMENTS:
//
//    Make sure TAPI is stopped before exiting.
//
//

LRESULT CmdExit(HWND hwnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    char szBuffer[50];   
    int  cbWritten = 0;

    ShutdownTAPI();

    cbWritten = LoadString(hInst, wCommand, szBuffer, sizeof(szBuffer)); 
    if(cbWritten == 0) 
        lstrcpy(szBuffer, "Unknown Command");

    UpdateStatusBar(szBuffer, 0, 0);
 
    DestroyWindow(hwnd);
    return 0;
}


//
//  FUNCTION: CmdHangupCall(HWND, WORD, WORD, HWND)
//
//  PURPOSE: Stops TAPI
//
//  PARAMETERS:
//    hwnd     - The window.
//    wCommand - IDM_HANGUPCALL (unused)
//    wNotify  - Notification number (unused)
//    hwndCtrl - NULL (unused)
//
//  RETURN VALUE:
//    Always returns 0 - command handled.
//
//  COMMENTS:
//
//    Tells TAPI to close any opened lines.
//
//


LRESULT CmdHangupCall(HWND hWnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    HangupCall();
    return 0;
}


//
//  FUNCTION: CmdMakeCall(HWND, WORD, WORD, HWND)
//
//  PURPOSE: Starts TAPI
//
//  PARAMETERS:
//    hwnd     - The window.
//    wCommand - IDM_MAKECALL (unused)
//    wNotify  - Notification number (unused)
//    hwndCtrl - NULL (unused)
//
//  RETURN VALUE:
//    Always returns 0 - command handled.
//
//  COMMENTS:
//
//    Starts TAPI by calling the Dialing Dialog box.
//    Code for this dialog is with the rest of the TAPI code.
//
//

LRESULT CmdMakeCall(HWND hWnd, WORD wCommand, WORD wNotify, HWND hwndCtrl)
{
    DialCall();
    return 0;
}
