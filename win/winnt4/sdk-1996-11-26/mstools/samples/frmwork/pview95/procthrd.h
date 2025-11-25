// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993 - 1996  Microsoft Corporation.  All Rights Reserved.
//
// PURPOSE:
//    Contains declarations for process and thread manipulation functions.


//-------------------------------------------------------------------------
// Manifest constants



//-------------------------------------------------------------------------
// Type definitions


//-------------------------------------------------------------------------
// Function prototypes.

BOOL InitToolhelp32 (void);
BOOL RefreshThreadList (HWND hListView, DWORD dwOwnerPID);
BOOL RefreshProcessList (HWND hListView);
BOOL KillProcess (DWORD dwPID);

