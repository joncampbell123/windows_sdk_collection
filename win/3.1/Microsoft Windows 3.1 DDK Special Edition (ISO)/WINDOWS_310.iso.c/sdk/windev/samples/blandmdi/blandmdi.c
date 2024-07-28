/*
 *
 *  PROGRAM  : BlandMDI.c
 *
 *  PURPOSE  : Show example of a minimal MDI application
 *
 *  FUNCTIONS:
 *
 *      WinMain() - Calls the initialization function
 *                  and processes message loop
 *
 *      BlandFrameWndProc() - Window function for the "frame"
 *                  window, which controls the menu
 *                  and contains the MDI client window
 *
 *      BlandMDIChildWndProc()- Window function for the individual
 *                  child windows
 *
 *      CloseAllChildren - Destroys all MDI child windows.
 *
 *      CommandHandler() - Processes the "frame" window's
 *                  WM_COMMAND messages.
 *
 * Copyright 1991 Microsoft Corporation. All rights reserved.
 */


/*--------------------  #includes  -------------------------------*/

#include "blandmdi.h"


/*--------------------  global variables  -------------------------*/

// global variables used in this module or among more than one module

HANDLE hInst;                   // Program instance handle
HWND hwndFrame     = NULL;      // Handle to main window
HWND hwndMDIClient = NULL;      // Handle to MDI client
LONG styleDefault  = 0;         // Default style bits for child windows


/*---------------------  function prototypes  -----------------------*/

// Forward declarations of helper functions in this module

VOID NEAR PASCAL CommandHandler  (HWND,WORD);
VOID NEAR PASCAL CloseAllChildren(VOID);


/*---------------  BlandFrameWndProc()  -------------------------------*/
/*
 *
 *  FUNCTION   : BlandFrameWndProc (hwnd, msg, wParam, lParam )
 *
 *  PURPOSE    : The window function for the "frame" window, which controls
 *               the menu and encompasses all the MDI child windows. Does
 *               the major part of the message processing. Specifically, in
 *               response to:
 *
 *               WM_CREATE : Creates and displays the "frame".
 *
 *               WM_COMMAND: Passes control to a command-handling function.
 *
 *               WM_CLOSE  : Quits the app. 
 *
 *               WM_DESTROY: Destroys frame window and quits app.
 *
 *  NOTE: If cases are added for WM_MENUCHAR, WM_NEXTMENU, WM_SETFOCUS,
 *        and WM_SIZE, note that these messages should be passed on
 *        to DefFrameProc even if we handle them.  See the SDK Reference
 *        entry for DefFrameProc
 */


LONG FAR PASCAL BlandFrameWndProc ( hwnd, msg, wParam, lParam )

register HWND hwnd;
UINT	      msg;
register WPARAM wParam;
LPARAM		lParam;

{
    switch (msg)
        {
        case WM_CREATE:
            {
            CLIENTCREATESTRUCT ccs;

            // Find window menu where children will be listed 
            ccs.hWindowMenu  = GetSubMenu (GetMenu(hwnd),WINDOWMENU);
            ccs.idFirstChild = IDM_WINDOWCHILD;

            // Create the MDI client 
            hwndMDIClient = CreateWindow ("mdiclient",
                NULL,
                WS_CHILD | WS_CLIPCHILDREN,
                0,
                0,
                0,
                0,
                hwnd,
                0,
                hInst,
                (LPSTR)&ccs);
            ShowWindow (hwndMDIClient,SW_SHOW);
            break;
            }

        case WM_COMMAND:
            // Direct all menu selection or accelerator commands to 
            // the CommandHandler function
            CommandHandler (hwnd,wParam);
            break;

        case WM_CLOSE:
            DestroyWindow (hwnd);
            break;

        case WM_DESTROY:
            PostQuitMessage (0);
            break;

        default:
            // use DefFrameProc() instead of DefWindowProc(), since there
            // are things that have to be handled differently because of MDI
            
            return DefFrameProc (hwnd,hwndMDIClient,msg,wParam,lParam);
        }
    return 0;
}


/*--------------------BlandMDIChildWndProc  ----------------------*/
/*
 *
 *  FUNCTION: BlandMDIChildWndProc ( hwnd, msg, wParam, lParam )  
 *
 *  NOTE: If cases are added for WM_CHILDACTIVATE, WM_GETMINMAXINFO,
 *        WM_MENUCHAR, WM_MOVE, WM_NEXTMENU, WM_SETFOCUS, WM_SIZE,
 *        or WM_SYSCOMMAND, these messages should be passed on
 *        to DefMDIChildProc even if we handle them. See the SDK
 *        Reference entry for DefMDIChildProc
 *
 */


LONG FAR PASCAL BlandMDIChildWndProc ( hwnd, msg, wParam, lParam )

register HWND hwnd;
UINT	      msg;
register WPARAM wParam;
LPARAM		lParam;

{
    // Since this is a generic MDI app, the children don't handle
    // and messages in non-default ways
    return DefMDIChildProc (hwnd, msg, wParam, lParam);
}



/*-----------------  CommandHandler  -------------------------------*/
/*
 *
 *  FUNCTION   : CommandHandler ()
 *                    
 *  PURPOSE    : Processes all "frame" WM_COMMAND messages.       
 *                     
 */

VOID NEAR PASCAL CommandHandler ( hwnd, wParam )
register HWND hwnd;
register WORD wParam;

{
    switch (wParam)
        {
        case IDM_FILENEW:
            // Make an empty MDI child window 
            MakeNewChild (NULL);
            break;

        case IDM_FILEEXIT:
            // Close application 
            SendMessage (hwnd, WM_CLOSE, 0, 0L);
            break;

        case IDM_WINDOWTILE:
            // Tile MDI windows 
            SendMessage (hwndMDIClient, WM_MDITILE, 0, 0L);
            break;

        case IDM_WINDOWCASCADE:
            // Cascade MDI windows 
            SendMessage (hwndMDIClient, WM_MDICASCADE, 0, 0L);
            break;

        case IDM_WINDOWICONS:
            // Auto - arrange MDI icons 
            SendMessage (hwndMDIClient, WM_MDIICONARRANGE, 0, 0L);
            break;

        case IDM_WINDOWCLOSEALL:
            CloseAllChildren();
            break;

        default:
            // This is essential, since there are frame WM_COMMANDS generated
            // by the MDI system for activating child windows via the
            // window menu.
            DefFrameProc(hwnd, hwndMDIClient, WM_COMMAND, wParam, 0L);
        }
}

/*-------------------  CloseAllChildren  -----------------------------*/
/*
 *
 *  FUNCTION   : CloseAllChildren ()       
 *                 
 *  PURPOSE    : Destroys all MDI child windows.
 *
 */


VOID NEAR PASCAL CloseAllChildren (VOID)
{
    register HWND hwndT;

    // As long as the MDI client has a child, destroy it 
    while ( hwndT = GetWindow (hwndMDIClient, GW_CHILD))
        {
        // Skip the icon title windows 
        while (hwndT && GetWindow (hwndT, GW_OWNER))
            hwndT = GetWindow (hwndT, GW_HWNDNEXT);
        if (hwndT)
            SendMessage (hwndMDIClient, WM_MDIDESTROY, (WORD)hwndT, 0L);
        else
            break;
        }
}



/*-----------------------  WinMain  ----------------------------------*/
/*
 *
 *  FUNCTION   : WinMain(HANDLE, HANDLE, LPSTR, int) 
 *                     
 *  PURPOSE    : Creates the "frame" window, does some initialization and  
 *     enters the message loop.         
 *              
 */


int NEAR PASCAL WinMain(hInstance, hPrevInstance, lpszCmdLine, nCmdShow)

HANDLE hInstance;
HANDLE hPrevInstance;
LPSTR  lpszCmdLine;
int    nCmdShow;
{
    MSG msg;

    hInst = hInstance;

    // If this is the first instance of the app. register window classes 
    if (!hPrevInstance)
       if (!InitializeApplication ())
         return 0;

    // Create the frame and do other initialization
    if (!InitializeInstance (lpszCmdLine, nCmdShow))
       return 0;

    // Enter main message loop 
    while (GetMessage (&msg, NULL, 0, 0))
        // If a keyboard message is for the MDI, let the MDI client
        // take care of it.  Otherwise, just handle the message as usual
        if ( !TranslateMDISysAccel (hwndMDIClient, &msg))
            {
            TranslateMessage (&msg);
            DispatchMessage (&msg);
            }
    return 0;
}
