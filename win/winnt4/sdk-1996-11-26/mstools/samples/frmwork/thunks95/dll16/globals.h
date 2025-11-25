// -----------------------------------------------------------------
// File name:  GLOBALS.H
//
// This header file contains global constants, and global variable
// declarations.
//
// Copyright (C) 1991 - 1996 Microsoft Corporation.  All rights reserved.
// -----------------------------------------------------------------


// Menu defines

#define IDM_ABOUT           500
#define IDM_EXIT            501
#define IDM_THUNK           502

// Control id's in thunk dialog
#define ID_THUNK            104
#define IDC_INTEGER16       105
#define IDC_INTEGER32       106
#define IDC_POINTER16       107
#define IDC_POINTER32       108


// String Table Defines

#define IDS_PROGNAME       1
#define IDS_MAINMENUNAME   2
#define IDS_MAINCLASSNAME  3

// Global variables declared here
extern HANDLE ghInst;                     // Handle to this instance
extern HWND   ghWnd;                      // Handle to main window
