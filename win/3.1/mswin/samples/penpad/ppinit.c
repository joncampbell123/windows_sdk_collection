/***************************************************************************
 *									   *
 *  MODULE	: PpInit.c						   *
 *									   *
 *  PURPOSE	: Contains initialization code for penpad.		   *
 *									   *
 *  FUNCTIONS	: InitializeApplication() - Sets up Class data structure   *
 *					    and registers window class.    *
 *									   *
 *		  InitializeInstance ()   - Does a per-instance initial-   *
 *					    ization of penpad. Creates   *
 *					    the "frame" and MDI client.    *
 *									   *
 ***************************************************************************/
#include "penpad.h"

char szFrame[] = "ppframe";   /* Class name for "frame" window */
char szChild[] = "ppchild";   /* Class name for MDI window     */

/****************************************************************************
 *									    *
 *  FUNCTION   : InitializeApplication ()				    *
 *									    *
 *  PURPOSE    : Sets up the class data structures and does a one-time	    *
 *		 initialization of the app by registering the window classes*
 *									    *
 *  RETURNS    : TRUE  - If RegisterClass() was successful for both classes.*
 *		 FALSE - otherwise.					    *
 *									    *
 ****************************************************************************/

BOOL FAR PASCAL InitializeApplication()
{
    WNDCLASS	wc;

    /* Register the frame class */
    wc.style	     = 0;
    wc.lpfnWndProc   = PPFrameWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInst;
    wc.hIcon	     = LoadIcon(hInst,IDPENPAD);
    wc.hCursor	     = LoadCursor(NULL,IDC_ARROW);
    wc.hbrBackground = COLOR_APPWORKSPACE+1;
    wc.lpszMenuName  = IDPENPAD;
    wc.lpszClassName = szFrame;

    if (!RegisterClass (&wc) )
	return FALSE;

    /* Register the MDI child class */
    wc.lpfnWndProc   = PPMDIChildWndProc;
    wc.hIcon	     = LoadIcon(hInst,IDNOTE);
    wc.lpszMenuName  = NULL;
    wc.cbWndExtra    = CBWNDEXTRA;
    wc.lpszClassName = szChild;

    if (!RegisterClass(&wc))
	return FALSE;

    return TRUE;

}

/****************************************************************************
 *									    *
 *  FUNCTION   : InitializeInstance ()					    *
 *									    *
 *  PURPOSE    : Performs a per-instance initialization of MultiPad. It     *
 *		 also creates the frame and an MDI window.		    *
 *									    *
 *  RETURNS    : TRUE  - If initialization was successful.		    *
 *		 FALSE - otherwise.					    *
 *									    *
 ****************************************************************************/
BOOL FAR PASCAL InitializeInstance(LPSTR lpCmdLine, WORD nCmdShow)
{
    extern HWND  hwndMDIClient;
    char	 sz[80], *pCmdLine;

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

    /* Load main menu accelerators */
    if (!(hAccel = LoadAccelerators (hInst, IDPENPAD)))
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
        pCmdLine = (char *) LocalAlloc(LPTR, lstrlen(lpCmdLine) + 1);
        if (pCmdLine)
           lstrcpy(pCmdLine, lpCmdLine);
    }

    /* Add the first MDI window */
    AddFile (pCmdLine);

    /* if we allocated a buffer then free it */
    if (pCmdLine)
        LocalFree((LOCALHANDLE) pCmdLine);

    /* Default to minimized windows after the first. */
    styleDefault = 0L;

    return TRUE;

}
