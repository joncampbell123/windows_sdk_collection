//****************************************************************************
// File: glthread.c
//
// Purpose: Contains main message loop and application entry point.  Also 
//			contains the code to handle palettes properly in an OpenGL 
//          application and the main window callback procedure.
//
// Functions:
//    WinMain 		   - initializes everything and enters message loop
//    InitializeWindow - Initializes the window class of and creates the main 
//						 window
//    MainWndProc      - Main window procedure
//    CommandHandler   - Handle all WM_COMMAND messages
//
// Development Team:
//         Greg Binkerd - Windows Developer Support
//
// Written by Microsoft Windows Developer Support
// Copyright (c) 1995-1996 Microsoft Corporation. All rights reserved.
//****************************************************************************

#include "glthread.h"

CHAR szAppName[]="GLThreads";

HGLRC hRC;

HWND ghWnd;

extern HPALETTE ghpalOld, ghPalette;

/* forward declarations of helper functions in this module */
HWND   WINAPI InitializeWindow (HINSTANCE, int);
LONG   WINAPI CommandHandler (HWND, WPARAM, LPARAM);
LONG   WINAPI MainWndProc (HWND, UINT, WPARAM, LPARAM);


//***********************************************************************
// Function: WinMain
//
// Purpose: Called by Windows on app startup.  Initializes everything,
//          and enters a message loop.
//
// Parameters:
//    hInstance     == Handle to _this_ instance.
//    hPrevInstance == Handle to last instance of app.
//    lpCmdLine     == Command Line passed into app.
//    nCmdShow      == How app should come up (i.e. minimized/normal)
//
// Returns: Return value from PostQuitMessage.
//
// Comments:
//
// History:  Date       Author        Reason
//           6/08/94                  Created
//****************************************************************************

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, 
					LPSTR lpCmdLine, int nCmdShow)
{
    MSG        msg;
    HWND       hWnd;

    /* previous instances do not exist in Win32 */
    if (hPrevInstance)
        return 0;

    if (!(hWnd = InitializeWindow (hInstance, nCmdShow)))
        return FALSE;

    /* main window message loop */
    while (GetMessage (&msg, NULL, 0, 0))
    {
      	TranslateMessage (&msg);
      	DispatchMessage (&msg);
    }

    /* return success of application */
    return TRUE;
}


//****************************************************************************
// Function: InitializeWindow
//
// Purpose: Called by WinMain on first instance of app.  Registers
//          the window class.
//
// Parameters:
//    hInstance == Handle to _this_ instance.
//    nCmdShow  == Initial state of window
//
// Returns: Handle to newly created window if successful
//
// Comments:
//
// History:  Date       Author        Reason
//           3/15/95                  Created
//****************************************************************************

HWND WINAPI InitializeWindow (HINSTANCE hInstance, int nCmdShow)
    {
    WNDCLASS   wc;
    HWND       hWnd;

    /* Register the frame class */
    wc.style         = 0;
    wc.lpfnWndProc   = (WNDPROC)MainWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon (hInstance, szAppName);
    wc.hCursor       = LoadCursor (NULL,IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = szAppName;
    wc.lpszClassName = szAppName;

    if (!RegisterClass (&wc) )
        return FALSE;

    /* Create the frame */
    ghWnd = hWnd = CreateWindow (szAppName,
			"Generic OpenGL Sample",
			WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			NULL,
			NULL,
			hInstance,
			NULL);

    /* make sure window was created */
    if (!hWnd)
        return FALSE;

    /* show and update main window */
    ShowWindow (hWnd, nCmdShow);

    UpdateWindow (hWnd);

    return hWnd;
}


//****************************************************************************
// Function: MainWndProc
//
// Purpose: Message handler for main overlapped window.
//
// Parameters:
//    hWnd    == Handle to this window.
//    uMsg    == Message to process.
//    wParam  == WORD parameter -- depends on message
//    lParam  == LONG parameter -- depends on message
//
// Returns: Depends on message.
//
// Comments:
//
// History:  Date       Author        Reason
//           3/15/95                  Created
//****************************************************************************

LONG WINAPI MainWndProc (
    HWND    hWnd,
    UINT    uMsg,
    WPARAM  wParam,
    LPARAM  lParam)
{
    LONG    lRet = 1;

    switch (uMsg)
    {
    	case WM_SIZE:
	        resize(hWnd);
            break;

        // The WM_QUERYNEWPALETTE message informs a window that it is about to
        // receive input focus. In response, the window receiving focus should
        // realize its palette as a foreground palette and update its client
        // area. If the window realizes its palette, it should return TRUE;
        // otherwise, it should return FALSE.

        case WM_QUERYNEWPALETTE:
        {
            HDC     hDC;

            if(ghPalette)
            {
                hDC = GetDC(hWnd);

                // Select and realize the palette

                ghpalOld = SelectPalette(hDC, ghPalette, FALSE);
                RealizePalette(hDC);

                // Redraw the client area

                InvalidateRect(hWnd, NULL, TRUE);
                UpdateWindow(hWnd);

                if(ghpalOld)
                    SelectPalette(hDC, ghpalOld, FALSE);

                ReleaseDC(hWnd, hDC);

                return TRUE;
            }

            return FALSE;
        }

        // The WM_PALETTECHANGED message informs all windows that the window
        // with input focus has realized its logical palette, thereby changing 
        // the system palette. This message allows a window without input focus
        // that uses a color palette to realize its logical palettes and update
        // its client area.
        //
        // This message is sent to all windows, including the one that changed
        // the system palette and caused this message to be sent. The wParam of
        // this message contains the handle of the window that caused the system
        // palette to change. To avoid an infinite loop, care must be taken to
        // check that the wParam of this message does not match the window's
        // handle.

        case WM_PALETTECHANGED:
        {
            HDC         hDC; 

            // Before processing this message, make sure we
            // are indeed using a palette

            if (ghPalette)
            {
                // If this application did not change the palette, select
                // and realize this application's palette

                if (wParam != (WPARAM)hWnd)
                {
                    // Need the window's DC for SelectPalette/RealizePalette

                    hDC = GetDC(hWnd);

                    // Select and realize our palette

                    ghpalOld = SelectPalette(hDC, ghPalette, FALSE);
                    RealizePalette(hDC);

                    // WHen updating the colors for an inactive window,
                    // UpdateColors can be called because it is faster than
                    // redrawing the client area (even though the results are
                    // not as good)

                    UpdateColors(hDC);

                    // Clean up

                    if (ghpalOld)
                       SelectPalette(hDC, ghpalOld, FALSE);

                    ReleaseDC(hWnd, hDC);
                }
            }
            break;
        }


    	case WM_COMMAND:
            lRet = CommandHandler (hWnd, wParam, lParam);
            break;

   	    case WM_CLOSE:
            /* call destroy window to cleanup and go away */
            DestroyWindow (hWnd);
	        break;

   	    case WM_DESTROY:
            PostQuitMessage (0);
	        break;

    	default:
            /* pass all unhandled messages to DefWindowProc */
            lRet = DefWindowProc (hWnd, uMsg, wParam, lParam);
    	    break;
    }

    /* return 1 if handled message, 0 if not */
    return lRet;
}


//****************************************************************************
// Function: CommandHandler
//
// Purpose: Command handler for main overlapped window.
//
// Parameters:
//    hWnd    == Handle to this window.
//    wParam  == WORD parameter -- depends on message
//    lParam  == LONG parameter -- depends on message
//
// Returns: Depends on message.
//
// Comments:
//
// History:  Date       Author        Reason
//           3/15/95                  Created
//****************************************************************************

LONG WINAPI CommandHandler (
    HWND    hWnd,
    WPARAM  wParam,
    LPARAM  lParam)
{
    switch (LOWORD(wParam))
    {
    	case IDM_EXIT:
            /* exit application */
            PostMessage (hWnd, WM_CLOSE, 0, 0L);
        	break;

    	case ID_DRAWWAVES:
	    	draw_scene(hWnd);
  	    	break;
    	
    	default:
            return FALSE;
    }
    return TRUE;
}
