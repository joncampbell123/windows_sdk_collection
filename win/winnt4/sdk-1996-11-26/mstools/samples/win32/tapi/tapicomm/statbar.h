// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1995-1996  Microsoft Corporation.  All Rights Reserved.
//
// PURPOSE:
//    Contains declarations for the status bar module.
//

//-------------------------------------------------------------------------
#define IDM_STATUSBAR       501
#define IDM_TIMER           701
#define TIMER_TIMEOUT       1000


// System Menu string ID's
#define IDS_SYSMENU         900


// Global variables for status bar window
extern HWND hWndStatusbar;

// Function prototypes for status bar creation
BOOL CreateSBar(HWND);
void InitializeStatusBar(HWND);
void UpdateStatusBar(LPSTR, WORD, WORD);
