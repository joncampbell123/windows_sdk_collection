// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993-1995  Microsoft Corporation.  All Rights Reserved.
//
//  MODULE:   exports.c
//
//  PURPOSE:   Contains the DLL's exported functions.
//
//  FUNCTIONS:
//    DLL32Function -  Calls the 16 bit side of the thunk
//
//  COMMENTS:
//    This is a simple demonstration of the structure of the thunking
//    mechanism.  It is not designed to cover all test cases of thunks
//    but rather to provide a framework for adding a thunking layer
//    to a 32 to 16 bit application.
//

#include <windows.h>
#include "dllglob.h"

// Prototype for function in 16-bit dll.
int PASCAL DLL16Call(HWND      hDlg,
                     UCHAR     bThunk,
                     SHORT     nThunk,
                     USHORT    usThunk,
                     LONG      lThunk,
                     ULONG     uLong,
                     LPSTR     lpstrThunk,
                     LPVOID    lpvoidThunk);

//
//  FUNCTION: DLL32Call(HWND hDlg)
//
//  PURPOSE:
//    To display a message passed in by an application.
//
//  PARAMETERS:
//    hWnd       -  Handle of the window that is to be parent of message box
//    lpszMsg    -  Character string containing a message to display.
//
//  RETURN VALUE:
//    Always returns TRUE
//

DLLEXPORT BOOL WINAPI DLL32Call(HWND hDlg)
{
    static char szThunkMsg[] = "32to16";
    char    szUserMsg[20];
    char    bThunk;
    SHORT   shortThunk;
    USHORT  ushortThunk;
    LONG    lThunk;
    ULONG   ulThunk;
    LPSTR   lpstrThunk;
    LPVOID  lpvoidThunk;

    //
    // Set variables and output them to the 32-bit side of the
    // thunk dialog.  Then call into the 16-bit side fo the thunk
    // so they can be displayed on the 16-bit side of the thunk
    // dialog.
    //

    // Set the hwnd static text
    wsprintf(szUserMsg, "%d", hDlg);
    SetWindowText(GetDlgItem(hDlg, IDD_HWND32), szUserMsg);

    // Set the character field
    bThunk = 'T';
    wsprintf(szUserMsg, "%c", bThunk);
    SetWindowText(GetDlgItem(hDlg, IDD_CHAR32), szUserMsg);

    // Set the short field
    shortThunk = -1;
    wsprintf(szUserMsg, "%d", shortThunk);
    SetWindowText(GetDlgItem(hDlg, IDD_INT32), szUserMsg);

    // Set the usigned short field
    ushortThunk = 1;
    wsprintf(szUserMsg, "%d", ushortThunk);
    SetWindowText(GetDlgItem(hDlg, IDD_USHORT32), szUserMsg);

    // Set the long field
    lThunk = 0xFFFFFFFF;
    wsprintf(szUserMsg, "%ld", lThunk);
    SetWindowText(GetDlgItem(hDlg, IDD_LONG32), szUserMsg);

    // Set the unsigned long field
    ulThunk = 0xFFFFFFFF;
    wsprintf(szUserMsg, "%ld", ulThunk);
    SetWindowText(GetDlgItem(hDlg, IDD_ULONG32), szUserMsg);

    // Set the lpstr field
    lpstrThunk = szThunkMsg;
    wsprintf(szUserMsg, "%s", lpstrThunk);
    SetWindowText(GetDlgItem(hDlg, IDD_LPSTR32), szUserMsg);

    // Set the lpvoid field with a pointer to the structure
    lpvoidThunk = (LPVOID) lpstrThunk;
    wsprintf(szUserMsg, "%08lx", lpvoidThunk);
    SetWindowText(GetDlgItem(hDlg, IDD_LPVOID32), szUserMsg);

    // Call into the 16-bit dll.
    DLL16Call(hDlg,
              bThunk,
              shortThunk,
              ushortThunk,
              lThunk,
              ulThunk,
              lpstrThunk,
              lpvoidThunk);
            
    return TRUE;
}

