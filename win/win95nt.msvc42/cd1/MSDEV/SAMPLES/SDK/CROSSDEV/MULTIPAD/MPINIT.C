
/******************************************************************************\
*       This is a part of the Microsoft Source Code Samples.
*       Copyright (C) 1993-1995 Microsoft Corporation.
*       All rights reserved.
*       This source code is only intended as a supplement to
*       Microsoft Development Tools and/or WinHelp documentation.
*       See these sources for detailed information regarding the
*       Microsoft samples programs.
\******************************************************************************/


/***************************************************************************
 *                                                                         *
 *  MODULE      : MpInit.c                                                 *
 *                                                                         *
 *  PURPOSE     : Contains initialization code for MultiPad.               *
 *                                                                         *
 *  FUNCTIONS   : InitializeApplication() - Sets up Class data structure   *
 *                                          and registers window class.    *
 *                                                                         *
 *                InitializeInstance ()   - Does a per-instance initial-   *
 *                                          ization of MultiPad. Creates   *
 *                                          the "frame" and MDI client.    *
 *                                                                         *
 ***************************************************************************/
#include "stdwin.h"
#include "multipad.h"

CHAR szFrame[] = "mpframe";   /* Class name for "frame" window */
CHAR szChild[] = "mpchild";   /* Class name for MDI window     */

/****************************************************************************
 *                                                                          *
 *  FUNCTION   : InitializeApplication ()                                   *
 *                                                                          *
 *  PURPOSE    : Sets up the class data structures and does a one-time      *
 *               initialization of the app by registering the window classes*
 *                                                                          *
 *  RETURNS    : TRUE  - If RegisterClass() was successful for both classes.*
 *               FALSE - otherwise.                                         *
 *                                                                          *
 ****************************************************************************/
BOOL InitializeApplication()
{
    WNDCLASS    wc;

    /* Register the frame class */
    wc.style         = 0;
    wc.lpfnWndProc   = (WNDPROC) MPFrameWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 4;
    wc.hInstance    = hInst;
    wc.hIcon         = LoadIcon(hInst,MAKEINTRESOURCE(ID_MULTIPAD_ICON));
    wc.hCursor       = LoadCursor(NULL,IDC_ARROW);
    wc.hbrBackground = (HBRUSH) (COLOR_APPWORKSPACE+1);
    wc.lpszMenuName  = IDMULTIPAD;
    wc.lpszClassName = szFrame;

    if (!RegisterClass (&wc) )
	return FALSE;

    /* Register the MDI child class */
    wc.lpfnWndProc   = (WNDPROC) MPMDIChildWndProc;
    wc.hIcon         = LoadIcon(hInst, MAKEINTRESOURCE(IDNOTE));
    wc.lpszMenuName  = NULL;
    wc.cbWndExtra    = CBWNDEXTRA;
    wc.lpszClassName = szChild;

    if (!RegisterClass(&wc))
	return FALSE;

    return TRUE;

}

/****************************************************************************
 *                                                                          *
 *  FUNCTION   : InitializeInstance ()                                      *
 *                                                                          *
 *  PURPOSE    : Performs a per-instance initialization of MultiPad. It     *
 *               also creates the frame and an MDI window.                  *
 *                                                                          *
 *  RETURNS    : TRUE  - If initialization was successful.                  *
 *               FALSE - otherwise.                                         *
 *                                                                          *
 ****************************************************************************/
BOOL InitializeInstance(LPSTR lpCmdLine, INT nCmdShow)
{
    extern HWND  hwndMDIClient;
    CHAR         sz[80], *pCmdLine;
    HDC          hdc;
    HMENU        hmenu;

    /* Get the base window title */
    LoadString (hInst, IDS_APPNAME, sz, sizeof(sz));

    /* Create the frame */
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

    if ((!hwndFrame) || (!hwndMDIClient))
	return FALSE;

#ifdef _MAC
	// Change About to About Multipad on Apple Menu
		{
			HMENU hmenuSys;

			hmenuSys = GetSystemMenu(hwndFrame, FALSE);
			ModifyMenu(hmenuSys, 0, MF_BYPOSITION, IDM_HELPABOUT, "About Multipad...");
	}
#endif

    /* Load main menu accelerators */
    if (!(hAccel = LoadAccelerators (hInst, IDMULTIPAD)))
	return FALSE;

    /* Display the frame window */
    ShowWindow (hwndFrame, nCmdShow);
    UpdateWindow (hwndFrame);

    /* If the command line string is empty, nullify the pointer to it
    ** else copy command line into our data segment
    */
    if ( lpCmdLine && !(*lpCmdLine))
	     pCmdLine = NULL;
    else {
	pCmdLine = (CHAR *) LocalAlloc(LPTR, lstrlen(lpCmdLine) + 1);
	if (pCmdLine)
	   lstrcpy(pCmdLine, lpCmdLine);
    }

    /* Add the first MDI window */
    AddFile (pCmdLine, pCmdLine);

    /* if we allocated a buffer then free it */
    if (pCmdLine)
	LocalFree((LOCALHANDLE) pCmdLine);

    /* Default to minimized windows after the first. */
    styleDefault = 0L;

    return TRUE;
	UNREFERENCED_PARAMETER(hmenu);
	UNREFERENCED_PARAMETER(hdc);

}
