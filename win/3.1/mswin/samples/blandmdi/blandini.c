/*
 *
 *  MODULE   : BlandIni.c
 *
 *  PURPOSE  : Contains initialization code for BlandMDI.
 *
 *  FUNCTIONS:
 *
 *      InitializeApplication() - Sets up Class data structure
 *                                and registers window class.
 *
 *      InitializeInstance ()   - Does a per-instance initialization
 *                                of BlandMDI. Creates the "frame"
 *                                and MDI client.
 *
 *      MakeNewChild ()         - Creates a new MDI child window
 *
 * Copyright 1991 Microsoft Corporation. All rights reserved.
 */

/*------------------------  #includes  --------------------------------*/

#include "BlandMDI.h"


/*------------------------  global variables  -------------------------*/

char szFrame[] = "bland frame";   // Class name for "frame" window
char szChild[] = "bland child";   // Class name for MDI window


/*--------------------- InitializeApplication  -------------------------*/
/*
 *
 *  FUNCTION   : InitializeApplication ()
 *
 *  PURPOSE    : Sets up the class data structures and does a one-time
 *               initialization of the app by registering the window classes
 *
 *  RETURNS    : TRUE  - If RegisterClass() was successful for both classes.
 *               FALSE - otherwise.
 *
 */

BOOL FAR PASCAL InitializeApplication()
{
    WNDCLASS  wc;

    // Register the frame class 
    wc.style         = 0;
    wc.lpfnWndProc   = BlandFrameWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInst;
    wc.hIcon         = LoadIcon(hInst, IDBLANDFRAME);
    wc.hCursor       = LoadCursor(NULL,IDC_ARROW);
    wc.hbrBackground = COLOR_APPWORKSPACE+1;
    wc.lpszMenuName  = IDBLANDMENU;
    wc.lpszClassName = szFrame;

    if (RegisterClass (&wc))
      {
      // Register the MDI child class 
      wc.lpfnWndProc   = BlandMDIChildWndProc;
      wc.hIcon         = LoadIcon(hInst,IDBLANDCHILD);
      wc.lpszMenuName  = NULL;
      wc.cbWndExtra    = CBWNDEXTRA;
      wc.lpszClassName = szChild;

      if (RegisterClass(&wc))
        return TRUE;
      }

    return FALSE;
}


/*----------------------  InitializeInstance  --------------------------*/
/*
 *
 *  FUNCTION   : InitializeInstance ()
 *
 *  PURPOSE    : Performs a per-instance initialization of BlandMDI. It
 *               also creates the frame and one MDI child window.
 *
 *  RETURNS    : TRUE  - If initialization was successful.
 *               FALSE - otherwise.
 *
 */


BOOL FAR PASCAL InitializeInstance(LPSTR lpCmdLine, WORD nCmdShow)
{
    extern HWND   hwndMDIClient;
	   char   sz[80];

    // Get the base window title
    LoadString (hInst, IDS_APPNAME, sz, sizeof(sz));

    // Create the frame 
    // MDI Client window is created in frame's WM_CREATE case
    hwndFrame = CreateWindow (szFrame,
            sz,
            WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
            CW_USEDEFAULT,
            0,
            CW_USEDEFAULT,
            0,
            NULL,
            NULL,
            hInst,
            NULL);

    if (hwndFrame && hwndMDIClient)
        {
        // Display the frame window 
        ShowWindow (hwndFrame, nCmdShow);
        UpdateWindow (hwndFrame);

        // Make the first MDI child window 
        MakeNewChild ("Initial Window");
        return TRUE;
        }

    return FALSE;
}


/*----------------   MakeNewChild  -----------------------------*/
/*
 *
 *  FUNCTION   : MakeNewChild (lpName)
 *
 *  PURPOSE    : Creates a new MDI child window.
 *
 *  RETURNS    : HWND  - A handle to the new window.
 *
 */

HWND FAR PASCAL MakeNewChild(char *pName)
{
    HWND            hwnd;
    char            sz[160];
    MDICREATESTRUCT mcs;

    if (!pName)
        {
        // pName parameter is NULL -- load the "Untitled" string 
        // from STRINGTABLE
        LoadString (hInst, IDS_UNTITLED, sz, sizeof(sz));
        mcs.szTitle = (LPSTR)sz;
        }
    else
        {
        mcs.szTitle = (LPSTR)pName; /* Fully qualified pathname*/
        }

    mcs.szClass    = szChild;
    mcs.hOwner     = hInst;
    mcs.x = mcs.cx = CW_USEDEFAULT;  // Use the default size for the window
    mcs.y = mcs.cy = CW_USEDEFAULT;
    mcs.style      = styleDefault;   // Set the style DWORD of the window
                                     // to default
    // tell the MDI Client to create the child 
    hwnd = (WORD)SendMessage (hwndMDIClient,
                              WM_MDICREATE,
                              0,
                              (LONG)(LPMDICREATESTRUCT)&mcs);
    ShowWindow(hwnd, SW_SHOW);

    return hwnd;
}
