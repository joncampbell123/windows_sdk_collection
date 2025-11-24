// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993-1995  Microsoft Corporation.  All Rights Reserved.
//
//  MODULE:   toolbar.c
//
//  PURPOSE:  Handles all toolbar manipulation.
//
//  FUNCTIONS:
//    CreateAppToolbar -   Creates the toolbar control and adds buttons
//    Toolbar_OnNeedText - Provides tooltip text for WM_NOTIFY/TTN_NEEDTEXT
// 	  
//  COMMENTS:
//
#include <windows.h>
#include <commctrl.h>
#include "globals.h"
#include "resource.h"

#define NUM_BITMAPS 2

//
//  FUNCTION: CreateAppToolbar(HWND, WORD)
//
//  PURPOSE:  Creates the toolbar control and adds buttoms
//
//  PARAMETERS:
//    hwndParent - Handle of the parent window.
//    wID      	 - Control identifier
//
//  RETURN VALUE:
//    The window handle of the newly created toolbar control if successful, 
//    or NULL if a new toolbar control could not be created.
//
//

HWND CreateAppToolbar(HWND hwndParent, WORD wID)
{
	HWND 	 hwndToolbar;	// window handle for the toolbar
	TBBUTTON tbButtons[] = 	// array of buttons for the toolbar

	// the struct goes like this:
	// bitmap index, WM_COMMAND id, state, style, app data, string index
	{
		{ 0, 0, 			 		TBSTATE_ENABLED, TBSTYLE_SEP, 	 0, 0 },
		{ 0, ID_PROCESS_REFRESH, 	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
		{ 1, ID_PROCESS_KILL,  		TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 }
	};

	// Ensure that the common control DLL is loaded
	InitCommonControls();

	// Create a toolbar that is customizable and has the tooltips enabled
	hwndToolbar = CreateToolbarEx(hwndParent, WS_CHILD | TBSTYLE_TOOLTIPS, 
			wID, NUM_BITMAPS, hInst, IDB_TOOLBAR, tbButtons,
			sizeof(tbButtons) / sizeof(TBBUTTON), 0, 16, 16, 15, 
			sizeof(TBBUTTON));

	if (!hwndToolbar)
		return NULL;

	ShowWindow(hwndToolbar, SW_SHOW);
	return hwndToolbar;
}


//
//  FUNCTION: Toolbar_OnNeedText(int, LPTOOLTIPTEXT)
//
//  PURPOSE:  Supplies the string table id for the tooltip.
//
//  PARAMETERS:
//		
//		idFrom - Identifies the control sending the notification
//		lpttt  - Provides information about which button needs tooltip text
//
//  RETURN VALUE:
//
//    Always returns 0 - Message handled
//
//  COMMENTS:
//
//

LRESULT Toolbar_OnNeedText(int idFrom, LPTOOLTIPTEXT lpttt)
{
	switch (lpttt->hdr.idFrom)	
	{
		// return the string table resource id for the appropriate string
		case ID_PROCESS_REFRESH:
			lpttt->lpszText = MAKEINTRESOURCE(IDS_TIP_REFRESH);
			break;

		case ID_PROCESS_KILL:
			lpttt->lpszText = MAKEINTRESOURCE(IDS_TIP_KILL);
			break;
	}

	return 0;
}
