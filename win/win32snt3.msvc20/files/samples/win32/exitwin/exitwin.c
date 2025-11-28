
/******************************************************************************\
*       This is a part of the Microsoft Source Code Samples. 
*       Copyright (C) 1993 Microsoft Corporation.
*       All rights reserved. 
*       This source code is only intended as a supplement to 
*       Microsoft Development Tools and/or WinHelp documentation.
*       See these sources for detailed information regarding the 
*       Microsoft samples programs.
\******************************************************************************/


/*  To enable NT's Reboot feature enable the below
 *  manifest constants. By default the program will
 *  log the user off only.
 */

//#define REBOOT

#define WIN32S  0x80000000l   // no manifest constance yet???

#define STRICT
#include "windows.h"                /* required for all Windows applications */

/****************************************************************************

    FUNCTION: WinMain(HANDLE, HANDLE, LPSTR, int)

    PURPOSE: calls initialization function, processes message loop

    COMMENTS:

        Windows recognizes this function by name as the initial entry point
        for the program.  This function calls the application initialization
        routine, if no other instance of the program is running, and always
        calls the instance initialization routine.  It then executes a message
        retrieval and dispatch loop that is the top-level control structure
        for the remainder of execution.  The loop is terminated when a WM_QUIT
        message is received, at which time this function exits the application
        instance by returning the value passed by PostQuitMessage().

        If this function must abort before entering the message loop, it
        returns the conventional value NULL.

****************************************************************************/

int PASCAL WinMain(HINSTANCE hInstance,      /* current instance  */
                   HINSTANCE hPrevInstance,  /* previous instance */
                   LPSTR     lpCmdLine,      /* command line      */
                   int       nCmdShow)       /* show-window type (open/icon) */
{
        HANDLE hToken;
        TOKEN_PRIVILEGES tkp;
        char szBuf[100];

        int x = MessageBox(NULL, "This application closes all running "
                "applications and logs you off of the system.\n"
                "Do you wish to continue?", "Exit Windows", MB_YESNO);

        if (x == IDNO) return(0);


        // Get a token for this process.
    if (!(GetVersion() & WIN32S)) {
                OutputDebugString("Setting token");
        // Running on NT so need to change privileges
        if (!OpenProcessToken(GetCurrentProcess(),
                        TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
            wsprintf(szBuf, "OpenProcessToken Error #%d", GetLastError ());
            MessageBox(NULL, szBuf, NULL, MB_OK);
        }

        // Get the LUID for shutdown privilege
        LookupPrivilegeValue(NULL, TEXT("SeShutdownPrivilege"), 
                &tkp.Privileges[0].Luid);
        tkp.PrivilegeCount = 1;  // one privilege to set
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

        // Get shutdown privilege for this process.
        if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0)) {
                wsprintf(szBuf, "AdjustTokenPrivileges Error #%d", GetLastError ());
            MessageBox(NULL, szBuf, NULL, MB_OK);
        }
        }

#ifdef REBOOT
        // Shut down the system, and reboot the system.
    if (!ExitWindowsEx( EWX_REBOOT, 0 )) {
                wsprintf(szBuf, "Error ExitWindows Error #%d", GetLastError ());
        MessageBox(NULL, szBuf, NULL, MB_OK);
    }
#else
        // Shut down the system, and force all applications closed.
    if (!ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0)) {
        wsprintf(szBuf, "Error ExitWindows Error #%d", GetLastError ());
        MessageBox(NULL, szBuf, NULL, MB_OK);
    }
#endif
}


