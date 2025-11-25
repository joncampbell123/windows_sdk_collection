// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1995-1996  Microsoft Corporation.  All Rights Reserved.
//
//  MODULE: EditCtls.c
//
//  PURPOSE: Handles the UI Edit controls to get data from user and display
//    data recieved from the COMM port.
//
//  EXPORTED FUNCTIONS:  These functions are for use by other modules.
//    InitEditCtls              - Initialize the edit controls.
//    SizeEditCtls              - Size and resize the edit controls.
//    SetFocusEditCtls          - Set the focus to the correct edit control.
//    WriteToDisplayCtl         - Write a string to the Display edit control.
//    PostWriteToDisplayCtl     - Posts a message to the UI thread
//                                with data to write.
//
//  INTERNAL FUNCTIONS:  These functions are for this module only.
//    SubclassInputEditProc     - Subclass proc to catch the 'enter' keystroke.
//
// COMMENTS:
//   This sample uses exceptionally simple terminal emulation: none.
//   There are two edit controls involved.  One for the user to type the
//   outgoing strings (called the InputCtl) and another to display both
//   transmitted and received strings (called the DisplayCtl).  There is
//   absolutely *no* translation of incoming strings, and the only
//   modifications to outgoing strings is to append "/r/n" to it.
//
//   This sample only emulates line mode transmission.  The string is only
//   read from the InputCtl when the 'Enter' key is typed.
//

#include <windows.h>
#include <string.h>
#include "globals.h"
#include "EditCtls.h"
#include "CommCode.h"
#include "toolbar.h"
#include "statbar.h"

// Maximum size of Edit control buffers

#define MAXDISPLAYSIZE 0xF000  // Almost 64K
#define MAXINPUTSIZE 0x400      // 1K

#define PWM_WRITESTRING WM_USER + 741 // Arbitrary number

HWND hWndInputCtl;      // Edit control to get input from user
HWND hWndDisplayCtl;    // Edit control to display all output
HWND hWndParent;        // Parent window of edit controls

WNDPROC lpfnInputEdit;  // Storage for subclassed edit control function


// Prototype for the subclassed procedure.
LRESULT CALLBACK SubclassInputEditProc(
    HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


//
//  FUNCTION: InitEditCtls(HWND)
//
//  PURPOSE: Initialize the UI edit controls.
//
//  PARAMETERS:
//    hWndMain - The window that is to contain the edit controls.
//
//  RETURN VALUE:
//    TRUE if the edit controls are successfully initialized.
//
//  COMMENTS:
//    This function should only need to be called once, when
//    creating the main window.  The only reason it could fail
//    is if the edit controls couldn't be created.
//

BOOL InitEditCtls(HWND hWndMain)
{
    hWndParent = hWndMain;

    if (!hWndMain)
    {
        OutputDebugString("Invalid parent hWnd for EditCtls!\n");
        return FALSE;
    }

    hWndDisplayCtl = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "EDIT","",
        WS_CHILD | WS_HSCROLL | WS_VSCROLL | ES_MULTILINE
        | ES_OEMCONVERT | WS_VISIBLE | WS_BORDER,
        0, 0, 0, 0, hWndParent,
        NULL, hInst, NULL);

    hWndInputCtl = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "EDIT","",
        WS_CHILD | ES_AUTOHSCROLL | WS_VISIBLE | WS_BORDER,
        0, 0, 0, 0, hWndParent,
        NULL, hInst, NULL);

    if (!hWndDisplayCtl || !hWndInputCtl)
    {
        OutputDebugString("Unable to create EditCtls.\n");
        return FALSE;
    }

    SendMessage(hWndInputCtl, EM_LIMITTEXT, MAXINPUTSIZE, 0);
    SendMessage(hWndDisplayCtl, EM_LIMITTEXT, MAXDISPLAYSIZE, 0);

        // This is where we subclass the input edit control
        // so we can catch the 'enter' keystroke
    lpfnInputEdit = (WNDPROC)
        SetWindowLong(hWndInputCtl, GWL_WNDPROC, (long) SubclassInputEditProc);

    return TRUE;
}


//
//  FUNCTION: SizeEditCtls
//
//  PURPOSE: Sizes, resizes and positions the edit controls.
//
//  PARAMETERS:
//    none
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//    Anytime the parent window is resized, it needs to signal
//    the edit controls to also resize.  The size of the edit
//    controls assumes that the edit controls are going to 
//    contain the full client area of the parent, allowing for
//    the toolbar and statusbar.
//

void SizeEditCtls()
{
    RECT rectParent, rectTemp;
    LONG HeightToolbar, HeightStatusbar;
    LONG HeightDisplayCtl, HeightInputCtl;
    LONG WidthCtls;
    LONG yposTopInputCtl, yposTopDisplayCtl;
    HDC hdc;
    TEXTMETRIC tm;

    // Get the client area of the parent.
    if (!GetClientRect(hWndParent, &rectParent))
    {
        OutputDebugString("GetClientRect on EditCtls parent failed\n");
        return;
    }

    // leave a pixel on each side
    WidthCtls = rectParent.right - 2;
    
    // Allow for the toolbar at the top.
    GetWindowRect(hWndToolbar, &rectTemp);
    HeightToolbar = rectTemp.bottom - rectTemp.top;

    // Allow for the statusbar at the bottom.
    GetWindowRect(hWndStatusbar, &rectTemp);
    HeightStatusbar = rectTemp.bottom - rectTemp.top;

    // How high should the input control be?
    hdc = GetDC(hWndInputCtl);
    GetTextMetrics(hdc, &tm);
    ReleaseDC(hWndInputCtl, hdc);
    HeightInputCtl = tm.tmHeight + tm.tmExternalLeading + 6;
        // 6 == 3 extra pixels between text and edit control vertical borders.

    // Position Input control.
    yposTopInputCtl = rectParent.bottom - HeightStatusbar - HeightInputCtl - 1;

    // Position Display control.
    yposTopDisplayCtl = rectParent.top + HeightToolbar + 2;
    HeightDisplayCtl = yposTopInputCtl - yposTopDisplayCtl - 1;

    // Move them both.
    MoveWindow(
        hWndInputCtl,
        1, yposTopInputCtl,
        WidthCtls, HeightInputCtl,
        TRUE);

    MoveWindow(
        hWndDisplayCtl,
        1, yposTopDisplayCtl,
        WidthCtls, HeightDisplayCtl,
        TRUE);
}


//
//  FUNCTION: SetFocusEditCtls
//
//  PURPOSE: Sets the focus to the correct edit control.
//
//  PARAMETERS:
//    none
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//    Everytime the parent window gets focus, it needs to signal
//    the edit controls to set focus also.  This very simple
//    algorythm always sets focus to the Input control.
//

void SetFocusEditCtls()
{
    DefWindowProc(hWndParent, WM_SETFOCUS, (WPARAM) NULL, (LPARAM) NULL);

    if (GetFocus() == hWndParent)
        SetFocus(hWndInputCtl);

    return;
}


//
//  FUNCTION: SubclassInputEditProc
//
//  PURPOSE: Subclass proceedure of Input control.
//
//  PARAMETERS:
//    Standard WNDPROC parameters.
//
//  RETURN VALUE:
//    Standard WNDPROC return value.
//
//  COMMENTS:
//    We subclass the Input edit control so that we can catch the 'Enter'
//    keystroke (VK_RETURN).  This is how we know when the user wants to
//    transmit something to the COMM port.  This indeed means that only
//    line based terminal emulation is supported by this sample.  However,
//    it makes editing the outgoing text much easier.
//
//    WriteCommString is the call that writes a string to the COMM port.
//    However, the string that is written *must* be allocated with
//    LocalAlloc, and if WriteCommString succeeds, then it will also
//    LocalFree the string.  If WriteCommString fails, then it refused
//    the string and its our job to free it.
//

LRESULT CALLBACK SubclassInputEditProc(
    HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        case WM_CHAR:

            // Found a RETURN keystroke!
            if ((TCHAR) wParam == VK_RETURN)
            {
                
                LPSTR pszString;
                long lSizeofString;

                // Get the size of the string to send.
                lSizeofString = 
                    SendMessage(hWndInputCtl, WM_GETTEXTLENGTH, 0, 0);

                // Allocate enough space for the string and "\r\0"
                pszString = (LPSTR) LocalAlloc(LPTR, lSizeofString +2);

                // Get the string
                SendMessage(hWndInputCtl, WM_GETTEXT,
                    lSizeofString + 1, (LPARAM) pszString);

                // Terminate it.
                strcat(pszString, "\r");
                lSizeofString += 1;

                // If the write to Comm failed, its probably because
                // a connection isn't made yet.
                if (WriteCommString(pszString, lSizeofString))
                {
                    // This is what we'd do to local echo user input
                    // Unnecessary usually.
                    //WriteToOutputEdit(pszString, lSizeofString);
                    ;
                }
                else
                {
                    WriteToDisplayCtl("Not yet connected\r", 20);

                    // The comm code refused the string, so we must free it.
                    LocalFree(pszString);
                }

                // Clear the Input control.
                SendMessage(hWndInputCtl, WM_SETTEXT, 0, (LPARAM) "");

                // Don't let default processing handle the RETURN key.
                return 0;
            }

            break;

        case PWM_WRITESTRING:
            WriteToDisplayCtl((LPSTR) lParam, (DWORD) wParam);
            LocalFree((LPSTR) lParam);
            break;

        default:
            break;
    }
    return CallWindowProc(lpfnInputEdit, hWnd, message, wParam, lParam);
}


//
//  FUNCTION: WriteToDisplayCtl(LPSTR, DWORD)
//
//  PURPOSE: Writes a string to the Display control.
//
//  PARAMETERS:
//    lpNewString       - The string to display.
//    dwSizeofNewString - The length of the string.
//
//  RETURN VALUE:
//    none.
//
//  COMMENTS:
//

void WriteToDisplayCtl(LPSTR lpNewString, DWORD dwSizeofNewString)
{
    DWORD dwSizeofEdit;

    dwSizeofEdit = SendMessage(hWndDisplayCtl, WM_GETTEXTLENGTH, 0, 0);

    if ((dwSizeofEdit + dwSizeofNewString) > MAXDISPLAYSIZE)
    {
        // Suggestion for future additions:
        // Handle edit control overflow by deleting from top of edit control
    }

    // Actually place the string into the Display control.
    SendMessage(hWndDisplayCtl, EM_SETSEL, dwSizeofEdit, dwSizeofEdit);
    SendMessage(hWndDisplayCtl, EM_REPLACESEL, 0, (LPARAM) lpNewString);
    SendMessage(hWndDisplayCtl, EM_SETSEL, 
        dwSizeofEdit+dwSizeofNewString, dwSizeofEdit+dwSizeofNewString);
}


//
//  FUNCTION: PostWriteToDisplayCtl(LPSTR, DWORD)
//
//  PURPOSE: Writes a string to the Display control.
//
//  PARAMETERS:
//    lpNewString       - The string to display.
//    dwSizeofNewString - The length of the string.
//
//  RETURN VALUE:
//    none.
//
//  COMMENTS:
//
//    Its very important that when data is recieved from the comm port,
//    the thread reading the comm port doesn't have to wait for the
//    data to be interpreted.  Instead, it posts the data to the main UI
//    thread to be handled when it is convenient.  This is the API to
//    wrap up the PostMessage.
//
//    Note that the string posted must be LocalAlloc()d, and its the job
//    of the window recieving the message to LocalFree it.
//

BOOL PostWriteToDisplayCtl(LPSTR lpNewString, DWORD dwSizeofNewString)
{
    return PostMessage(hWndInputCtl, PWM_WRITESTRING, 
        (WPARAM)dwSizeofNewString, (LPARAM) lpNewString);
}
