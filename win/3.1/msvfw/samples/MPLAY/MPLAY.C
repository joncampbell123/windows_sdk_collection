/**************************************************************************
 * MPLAY.C - Movie Player App using MCIWnd window class
 *
 *************************************************************************/
/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1992, 1993  Microsoft Corporation.  All Rights Reserved.
 * 
 **************************************************************************/

#include <windows.h>
#include <vfw.h>

static  char    szAppName[]="MPlay";

static  HWND    hwndApp;

int PASCAL WinMain(HANDLE hInst, HANDLE hPrev, LPSTR szCmdLine, WORD sw)
{
    MSG     msg;
    WORD    wVer;

    /* first let's make sure we are running on 1.1 */
    wVer = HIWORD(VideoForWindowsVersion());
    if (wVer < 0x010a){
	    /* oops, we are too old, blow out of here */
	    MessageBeep(MB_ICONHAND);
	    MessageBox(NULL, "Video for Windows version is too old",
		       "Movie Player Error", MB_OK|MB_ICONSTOP);
	    return FALSE;
    }

    /* create the window */
    hwndApp = MCIWndCreate(NULL, hInst,
//          MCIWNDF_NOAUTOSIZEWINDOW    |
//          MCIWNDF_NOPLAYBAR           |
//          MCIWNDF_NOAUTOSIZEMOVIE     |
//          MCIWNDF_NOMENU              |
            MCIWNDF_SHOWNAME            |
//          MCIWNDF_SHOWPOS             |
            MCIWNDF_SHOWMODE            |
//          MCIWNDF_RECORD              |
//          MCIWNDF_NOERRORDLG          |
            WS_OVERLAPPEDWINDOW         |
            WS_VISIBLE,
            NULL);

    if (hwndApp == NULL)
        return -1;

    SetWindowText(hwndApp, szAppName);

    if (szCmdLine && *szCmdLine)
    {
        MCIWndOpen(hwndApp, szCmdLine, 0);
        MCIWndPlay(hwndApp);
    }

    /*
     * Polling messages from event queue
     */
    while (GetMessage(&msg,NULL,0,0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (!IsWindow(hwndApp))
            PostQuitMessage(0);
    }

    return msg.wParam;
}
