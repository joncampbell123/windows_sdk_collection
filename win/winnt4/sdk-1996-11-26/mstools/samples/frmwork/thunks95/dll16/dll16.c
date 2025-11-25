// -----------------------------------------------------------------
// File name:  DLL16.C
//
// This is the main DLL source file.  It contains LibMain, the DLL's
// entry point.
//
// Description of functions:
//
//    LibMain          -  This DLL's entry point.  Analogous to WinMain.
//    DLL16PascalCall   -  A function exported by this DLL that uses the
//                        Pascal calling convention
//    DLL16CCall        -  A function exported by this DLL that uses the
//                        C calling convention
//
//
//   Copyright (C) 1993 - 1996 Microsoft Corporation.  All rights reserved.
//
//   You have a royalty-free right to use, modify, reproduce and
//   distribute the Sample Files (and/or any modified version) in
//   any way you find useful, provided that you agree that
//   Microsoft has no warranty obligations or liability for any
//   Sample Application Files which are modified.
//
// -----------------------------------------------------------------


#include<windows.h>
#include "dll16.h"
#include "..\globals.h"

BOOL FAR PASCAL __export DllEntryPoint (DWORD dwReason,
                               WORD  hInst,
                               WORD  wDS,
                               WORD  wHeapSize,
                               DWORD dwReserved1,
                               WORD  wReserved2);

BOOL FAR PASCAL thk_ThunkConnect16(LPSTR pszDll16,
                                   LPSTR pszDll32,
                                   WORD  hInst,
                                   DWORD dwReason);

HANDLE ghDLLInst;


// -----------------------------------------------------------------
//
// Function: LibMain
//
// Purpose : This is the DLL's entry point.  It is analogous to WinMain
//           for applications.
//
// Params  : hInstance   ==  The handle to the DLL's instance.
//           wDataSeg    ==  Basically it is a pointer to the DLL's
//                           data segment.
//           wHeapSize   ==  Size of the DLL's heap in bytes.
//           lpszCmdLine ==  The command line passed to the DLL
//                           by Windows.  This is rarely used.
//
// Returns : 1 indicating DLL initialization is successful.
//
// Comments: LibMain is called by Windows.  Do not call it in your
//           application!
// -----------------------------------------------------------------

int FAR PASCAL LibMain (HANDLE hInstance,
                        WORD   wDataSeg,
                        WORD   wHeapSize,
                        LPSTR  lpszCmdLine)
{
    ghDLLInst = hInstance;

    if (wHeapSize != 0)   // If DLL data seg is MOVEABLE
        UnlockData (0);

    return (1);
}


//
//
// Function: DLL16Call
//
// Purpose : This is a simple function which accepts parameters
//           from the 32-bit side of the world and displays them
//           on the 16-bit side of the dialog.
//
// Params  : HWND - handle to the dialog box for displaying the
//           fact that we got here.
//
// Returns : Always returns TRUE.
//
// Comments: Not meant to do a whole lot except show that the values
//           in the structure have been successfully thunked down to
//           16-bits.
//
//
int FAR PASCAL __export DLL16Call(HWND   hDlg,
                                  char   bThunk,
                                  int    shortThunk,
                                  UINT   ushortThunk,
                                  long   lThunk,
                                  ULONG  ulThunk,
                                  LPSTR  lpstrThunk,
                                  LPVOID lpvoidThunk)
{
    char szUserMsg[100];

    wsprintf(szUserMsg,
             "\n\rIn DLL16Call: hDlg = %d szThunk = %s",
             hDlg,
             lpstrThunk);
    OutputDebugString(szUserMsg);

    // Set the hwnd static text
    wsprintf(szUserMsg, "%d", hDlg);
    SetWindowText(GetDlgItem(hDlg, IDD_HWND16), szUserMsg);

    // Set the character field
    wsprintf(szUserMsg, "%c", bThunk);
    SetWindowText(GetDlgItem(hDlg, IDD_CHAR16), szUserMsg);

    // Set the integer field
    wsprintf(szUserMsg, "%d", shortThunk);
    SetWindowText(GetDlgItem(hDlg, IDD_INT16), szUserMsg);

    // Set the usigned short field
    wsprintf(szUserMsg, "%d", ushortThunk);
    SetWindowText(GetDlgItem(hDlg, IDD_USHORT16), szUserMsg);

    // Set the long field
    wsprintf(szUserMsg, "%ld", lThunk);
    SetWindowText(GetDlgItem(hDlg, IDD_LONG16), szUserMsg);

    // Set the unsigned long field
    wsprintf(szUserMsg, "%ld", ulThunk);
    SetWindowText(GetDlgItem(hDlg, IDD_ULONG16), szUserMsg);

    // Set the lpstr field
    wsprintf(szUserMsg, "%s", lpstrThunk);
    SetWindowText(GetDlgItem(hDlg, IDD_LPSTR16), szUserMsg);

    // Set the lpvoid field with a pointer to the structure
    wsprintf(szUserMsg,
             "%04x:%04x",
             SELECTOROF(lpvoidThunk),
             OFFSETOF(lpvoidThunk));
    SetWindowText(GetDlgItem(hDlg, IDD_VOIDSTAR16), szUserMsg);

    return (TRUE);
}


BOOL FAR PASCAL __export DllEntryPoint (DWORD dwReason,
                               WORD  hInst,
                               WORD  wDS,
                               WORD  wHeapSize,
                               DWORD dwReserved1,
                               WORD  wReserved2)
{
    OutputDebugString("In 16bit DllEntryPoint: Calling thkThunkConnect16");
    if (!thk_ThunkConnect16("DLL16.DLL",
                            "DLL32.DLL",
                            hInst,
                            dwReason))
    {
        OutputDebugString("\n\rIn 16bit DllEntryPoint: thkThunkConnect16 ret FALSE");
        return FALSE;
    }

    OutputDebugString("\n\rIn 16bit DllEntryPoint: thkThunkConnect16 ret TRUE");
    return TRUE;
}




