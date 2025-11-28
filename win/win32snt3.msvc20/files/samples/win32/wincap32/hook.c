//***********************************************************************
//
// hook.c
//
// Source file for Keyboard Hook function and support procedures.  Defines
// the following exported functions:
//  
// InstallHook()       - Installs/Removes keyboard hook for app's hotkeys
// KeyboardHook()      - Keyboard Hook Procedure
//
// This file should go into a DLL.
//
// This file, although not officially containing DIB functions, is part 
// of the DIBAPI.DLL because:
//   1) System-wide Hook Procedures in 3.1 need to be in a DLL, and 
//   2) I didn't want the user to have 2 DLLs to worry about for WINCAP. 
//
//
// Development Team: Mark Bader
//                   Tony Claflin
//
// File change history:
// Date      Author              Reason
// 11/11/93  Hung H. Nguyen      Ported to Win32 & added strict type checking
//
// Written by Microsoft Product Support Services, Developer Support.
// Copyright (c) 1991 Microsoft Corporation. All rights reserved.
//***********************************************************************

#define     STRICT      // enable strict type checking

#include <windows.h>
#include "wincap.h"

// Globals for this module

extern HWND     ghWndMain;      // Handle to main window -- used to post msgs

static HHOOK    hHook;          // A handle to our installed hook
static BOOL     bHookInstalled; // TRUE if hook has been installed


// External variables

extern HANDLE ghDLLInst;   // Handle to the DLL's instance.  Set in LibMain.

// Local function.  Although this function is exported from our DLL (Windows
// needs to call it directly), no other app needs to call this, so we can
// just prototype it here. 

LRESULT CALLBACK KeyboardHook (int nCode, WORD wParam, DWORD lParam );

//**********************************************************************
// InstallHook()
//
// Installs/Removes Filter function for the WH_KEYBOARD hook.
//
// Parameters:
// HWND hWnd      Handle to main window to receive posted messages.  See
//                KeyboardHook() for more info on how it works.
// 
// BOOL bCode     TRUE to hook, FALSE to unhook
//
// Returns:
// 1 if successful, 0 if not.
//
// History:   Date      Author         Reason
//            03/04/92  Mark Bader     Created from HOOKS 3.1 SDK app
//
//**********************************************************************

int APIENTRY InstallHook (HWND hWnd, BOOL bCode )
{

    int nReturn = 1;

    ghWndMain = hWnd;  // Store app's window handle in DLL global variable

    // Make sure that we are installing/removing in the proper order

    if (bCode == bHookInstalled)
        return 0;

    if (bCode)
    {
        hHook = (HHOOK)SetWindowsHookEx(WH_KEYBOARD,(HOOKPROC)KeyboardHook,
            ghDLLInst, 0);

        if (!hHook)
            return 0;

        bHookInstalled = TRUE;
    }
    else
    {
        nReturn = UnhookWindowsHookEx(hHook);
        bHookInstalled = FALSE;
    }
    return nReturn;
}




//**********************************************************************
//
// KeyboardHook()
//
// This is the Keyboard Hook function which windows will call every
// time it gets a keyboard message.  In this function, we check to
// see if the key pressed was Ctrl+Alt+F[8,9,10], and if it is, we post
// the proper message to our main window which will do the right
// thing.  
//
// Note that the window handle that we post from was set by a call to
// InstallHook() above.
//
// Parameters/return value:
//
// Standard 3.1 KeyboardProc.  See docs for "KeyboardProc".
//
// History:   Date      Author         Reason
//            9/15/91   Tony Claflin   Created
//            12/11/91  Mark Bader     Added additional hotkey Ctrl-Shift-F10
//            03/04/92  Mark Bader     Moved into a DLL for Win 3.1
//
//*********************************************************************

LRESULT CALLBACK KeyboardHook (int nCode, WORD wParam, DWORD lParam )
{

    if ( nCode >= 0 )
    {

        // Check to see if it's a key we're looking for

        if (GetKeyState(VK_SHIFT) < 0 && GetKeyState(VK_CONTROL) < 0)
        {
            switch (wParam)
            {

//      MENUITEM    "&Active Window \aCtrl+Shift+F9",  IDM_ACTIVEWINDOW
//      MENUITEM    "&Desktop\aCtrl+Shift+F10",       IDM_DESKTOP

                case VK_F9:
                    if (HIWORD(lParam) & 0x8000)
                        PostMessage(ghWndMain, WM_COMMAND, IDM_ACTIVEWINDOW, 0L);
                    return 1;

                case VK_F10:

                    if (HIWORD(lParam) & 0x8000)
                        PostMessage(ghWndMain, WM_COMMAND, IDM_DESKTOP, 0L);
                    return 1;
            }
        }
   }

   //
   // If we haven't returned 1 by the time we get here, then we
   // need to pass on the message to CallNextHookEx.
   //
   return (int)CallNextHookEx(hHook, nCode, wParam, lParam);
}
