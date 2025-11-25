// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993-1996  Microsoft Corporation.  All Rights Reserved.
//
//  MODULE:   VListVw.c
//
//  PURPOSE:   Implement the windows procedure for the main application
//    windows.  Also implement the generic message and command dispatchers.
//
//  FUNCTIONS:
//    WndProc      - Processes messages for the main window.
//    MsgCreate    - Handle the WM_CREATE message 
//    MsgCommand   - Handle the WM_COMMAND messages for the main window.
//    MsgSysColorChange - Pass the WM_SYSCOLORCHANGE message to the 
//                        listview control
//    MsgDestroy   - Handles the WM_DESTROY message by calling 
//                   PostQuitMessage().
//	  MsgHScroll   - Handles the horizontal trackbar notifications.
//    MsgVScroll   - Handles the vertical scrollbar notifications.
//    
//
//  COMMENTS:
//

#include <windows.h>            // required for all Windows applications
#include <windowsx.h>
#include "commctrl.h"
#include "globals.h"            // prototypes specific to this application
#include "Trackbar.h"
#include "resource.h"

#define ITEM_COUNT   100000

// Main window message table definition.
MSD g_rgmsd[] =
{
   {WM_CREATE,         MsgCreate        },
   {WM_COMMAND,        MsgCommand       },
   {WM_SYSCOLORCHANGE, MsgSysColorChange},
   {WM_DESTROY,        MsgDestroy       },
   {WM_HSCROLL,		   MsgHScroll		},
   {WM_VSCROLL,		   MsgVScroll		}
};


MSDI g_msdiMain =
{
   sizeof(g_rgmsd) / sizeof(MSD),
   g_rgmsd,
   edwpWindow
};


// Main window command table definition.
CMD g_rgcmd[] =
{
   {IDM_EXIT,          CmdExit},
   {IDM_ABOUT,         CmdAbout}
};

CMDI g_cmdiMain =
{
   sizeof(g_rgcmd) / sizeof(CMD),
   g_rgcmd,
   edwpWindow
};


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  PARAMETERS:
//    hWnd     - window handle
//    uMessage - message number
//    wParam   - additional information (dependant on message number)
//    lParam   - additional information (dependant on message number)
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

LRESULT CALLBACK WndProc(HWND   hWnd, 
                         UINT   uMessage, 
                         WPARAM wParam, 
                         LPARAM lParam)
{
   return DispMessage(&g_msdiMain, hWnd, uMessage, wParam, lParam);
}


//
//  FUNCTION: MsgCommand(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Handle the WM_COMMAND messages for the main window.
//
//  PARAMETERS:
//    hWnd     - window handle
//    uMessage - WM_COMMAND (Unused)
//    GET_WM_COMMAND_ID(wParam, lParam)   - Command identifier
//    GET_WM_COMMAND_HWND(wParam, lParam) - Control handle
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

LRESULT MsgCommand(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
   return DispCommand(&g_cmdiMain, hWnd, wParam, lParam);
}


//
//  FUNCTION: MsgCreate(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Handle the WM_CREATE messages for the main window.
//           Calls CreateTrackbar() to create two trackbar controls
//			 and CreateLabel to create the static text buddy windows
//			 for the trackbars.
//
//  PARAMETERS:
//    hWnd     - window handle
//
//  RETURN VALUE:
//    Always return 0;
//    
//
//  COMMENTS:
//    
//    
//

LRESULT MsgCreate(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
   RECT rc;
 
   // Create the trackbar controls
   g_hWndTrackbarH = CreateTrackbar(hWnd, 
									 WS_VISIBLE | 
									 WS_CHILD | 
									 TBS_HORZ |
									 TBS_ENABLESELRANGE |
									 TBS_AUTOTICKS |
									 TBS_BOTTOM,
									 TB_MIN, 
									 TB_MAX);


   g_hWndTrackbarV = CreateTrackbar(hWnd, 
									 WS_VISIBLE | 
									 WS_CHILD | 
									 TBS_VERT |
									 TBS_AUTOTICKS |
									 TBS_BOTTOM,
									 TB_MIN, 
									 TB_MAX);
	// Create the static text labels
   	g_hWndLabelH = CreateLabel(hWnd, "0");
	g_hWndLabelV = CreateLabel(hWnd, "0");


	if ((NULL == g_hWndTrackbarV) ||
		(NULL == g_hWndTrackbarH) ||
		(NULL == g_hWndLabelH)	  ||
		(NULL == g_hWndLabelV))
		MessageBox(hWnd, "Unable To Create All Windows", NULL, MB_OK);

	// Some code to position the controls in the window.
	GetClientRect(hWnd, &rc);
	MoveWindow(g_hWndTrackbarH, rc.left + 30, rc.bottom - 60, 
			  (rc.right - rc.left) - 60, 30, FALSE);

	MoveWindow(g_hWndTrackbarV, (rc.right - rc.left) / 2, rc.top + 10,
			   30, 200, FALSE);


	// Now that everything is properly positioned we can show the main window.
	ShowWindow(hWnd, SW_NORMAL);


	return 0;    
}



//
//  FUNCTION: MsgSysColorChange(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Passed the WM_SYSCOLORCHANGE message to the trackbar
//			 controls.  Since the static controls are buddy windows
//			 they get the background color of the trackbar controls
//			 so we don't pass this on.
//
//  PARAMETERS:
//
//    hWnd      - Window handle  
//    uMessage  - Message number 
//    wParam    - Extra data     
//    lParam    - Extra data     
//
//  RETURN VALUE:
//
//    Call DefWindowProc() and return value from DWP
//
//  COMMENTS:
//    
//    For clarity's sake, I do not attempt to handle this message
//    for the main window.
//
//

LRESULT MsgSysColorChange(HWND hWnd,
                          UINT uMessage,
                          WPARAM wParam,
                          LPARAM lParam)
{
   HDC hDC;

   hDC = GetDC(hWnd);
   SetBkColor(hDC, GetSysColor(COLOR_3DFACE));
   ReleaseDC(hWnd, hDC);


   PostMessage(   g_hWndTrackbarH, 
                  WM_SYSCOLORCHANGE, 
                  wParam, 
                  lParam);

   PostMessage(   g_hWndTrackbarV, 
                  WM_SYSCOLORCHANGE, 
                  wParam, 
                  lParam);
   
   return DefWindowProc(hWnd, uMessage, wParam, lParam);

}


//
//  FUNCTION: MsgDestroy(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Calls PostQuitMessage().
//
//  PARAMETERS:
//
//    hWnd      - Window handle  (Unused)
//    uMessage  - Message number (Unused)
//    wParam    - Extra data     (Unused)
//    lParam    - Extra data     (Unused)
//
//  RETURN VALUE:
//
//    Always returns 0 - Message handled
//
//  COMMENTS:
//
//

LRESULT MsgDestroy(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
   PostQuitMessage(0);
   return 0;
}

//
//  FUNCTION: MsgHScroll(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Recives trackbar notifications for the
//           horizontal trackbar.
//
//  PARAMETERS:
//
//    hWnd      - Window handle  (Unused)
//    uMessage  - Message number (Unused)
//    wParam    - Extra data     (Unused)
//    lParam    - Extra data     (Unused)
//
//  RETURN VALUE:
//
//    Always returns 0 - Message handled
//
//  COMMENTS:
//
//

LRESULT MsgHScroll(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	static cTrackPos = 0;
	TCHAR szBuf[MAX_CHARS + 1];

	switch (LOWORD(wParam)){
		
		case TB_PAGEDOWN:
			if ((cTrackPos += PAGE_SIZE) > TB_MAX){
				cTrackPos = TB_MAX;
			}
			break;

		case TB_PAGEUP:
			if ((cTrackPos -= PAGE_SIZE) < TB_MIN){
				cTrackPos = TB_MIN;
			}
			break;

		default:
			cTrackPos = SendMessage(g_hWndTrackbarH, TBM_GETPOS, 0, 0);
			break;

	}
	
	SendMessage(g_hWndTrackbarH, TBM_SETSEL, TRUE, MAKELONG(0, cTrackPos));
	SetWindowText(g_hWndLabelH, (LPTSTR)itoa(cTrackPos, szBuf, 10));
	
    return 0;
}


//
//  FUNCTION: MsgVScroll(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Recives trackbar notifications for the
//			 vertical scrollbar.
//
//  PARAMETERS:
//
//    hWnd      - Window handle  (Unused)
//    uMessage  - Message number (Unused)
//    wParam    - Extra data     (Unused)
//    lParam    - Extra data     (Unused)
//
//  RETURN VALUE:
//
//    Always returns 0 - Message handled
//
//  COMMENTS:
//
//

LRESULT MsgVScroll(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	static cTrackPos = 0;
	TCHAR szBuf[MAX_CHARS + 1];

	switch (LOWORD(wParam)){
		
		case TB_PAGEDOWN:
			if ((cTrackPos += PAGE_SIZE) > TB_MAX){
				cTrackPos = TB_MAX;
			}
			break;

		case TB_PAGEUP:
			if ((cTrackPos -= PAGE_SIZE) < TB_MIN){
				cTrackPos = TB_MIN;
			}
			break;

		default:
			cTrackPos = SendMessage(g_hWndTrackbarV, TBM_GETPOS, 0, 0);
			break;

	}
	
	SetWindowText(g_hWndLabelV, (LPTSTR)itoa(cTrackPos, szBuf, 10));
    return 0;
}



//
//  FUNCTION: CmdExit(HWND, WORD, WORD, HWND)
//
//  PURPOSE: Exit the application.
//
//  PARAMETERS:
//    hWnd     - The window.
//    wCommand - IDM_EXIT (unused)
//    wNotify  - Notification number (unused)
//    hWndCtrl - NULL (unused)
//
//  RETURN VALUE:
//    Always returns 0 - command handled.
//
//  COMMENTS:
//
//

LRESULT CmdExit(HWND hWnd, WORD wCommand, WORD wNotify, HWND hWndCtrl)
{
    DestroyWindow(hWnd);
    return 0;
}

//
//  FUNCTION: CreateTrackbar(HWND)
//
//  PURPOSE: Create the trackbar control
//
//  PARAMETERS:
//
//    HWND - Handle to the parent window
//
//  RETURN VALUE:
//
//    hwndTrackbar - Handle to trackbar control if successful
//    NULL   - if creation of trackbar control fails
//
//  COMMENTS:
//

HWND WINAPI CreateTrackbar(HWND hWndParent,
						   DWORD dwStyle,
                           int nMin, 
                           int nMax)
{
   HWND hWndTrackbar;

   // Force the common controls to be registered.
   InitCommonControls();

   // Create the control.
   hWndTrackbar = CreateWindowEx( 0,
								  TRACKBAR_CLASS, 
								  NULL,
								  dwStyle,
								  CW_USEDEFAULT, 
								  CW_USEDEFAULT, 
								  TB_WIDTH, 
								  TB_HEIGHT,
								  hWndParent,
								  NULL, 
								  g_hInst, 
								  NULL);

   if (hWndTrackbar == NULL)
      return NULL;

    SendMessage(hWndTrackbar, TBM_SETRANGE, 
                (WPARAM) TRUE,                   // redraw flag 
                (LPARAM) MAKELONG(nMin, nMax));  // min. & max. positions 
    
    SendMessage(hWndTrackbar, TBM_SETPAGESIZE, 
                0, (LPARAM) PAGE_SIZE);                  // new page size 
 
    SetFocus(hWndTrackbar); 
 
    return hWndTrackbar; 
}

//
//  FUNCTION: CreateLabel(HWND)
//
//  PURPOSE: Create the trackbar control
//
//  PARAMETERS:
//
//    HWND - Handle to the parent window
//
//  RETURN VALUE:
//
//    hwndLabel - Handle to trackbar control if successful
//    NULL   - if creation of trackbar control fails
//
//  COMMENTS:
//
HWND CreateLabel(HWND hwndParent, 
                 LPTSTR szText)
{
    HWND hwndLabel;
    HDC hDC;
    TEXTMETRIC tm;
    DWORD dwStyle =     WS_CHILD |
						WS_VISIBLE |
                        SS_LEFT;
    
    hDC = GetDC(hwndParent);
    GetTextMetrics(hDC, &tm);
	ReleaseDC(hwndParent, hDC);
    hDC = NULL;


    hwndLabel = CreateWindowEx( 0,
                                "STATIC",
                                szText,
                                dwStyle,
                                0,
                                100,
                                tm.tmAveCharWidth * MAX_CHARS,
                                tm.tmHeight + tm.tmExternalLeading,
                                hwndParent, 
                                NULL,
                                g_hInst,
                                NULL);

    return hwndLabel;
}






							  
