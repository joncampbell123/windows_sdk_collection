// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993 - 1996  Microsoft Corporation.  All Rights Reserved.
//
//  MODULE:   misc.c
//
//  PURPOSE:  Contains all helper functions "global" to the application.
//
//  FUNCTIONS:
//    CenterWindow            - Center one window over another.
//    AllocThreadData         - Allocates a thread data block.
//    FreeThreadData          - Deallocates a thread data block.
//    FillListBox             - Fills a listbox with strings and item data
//    GetCurChildWindowThread - Returns handle of current MDI child window's
//                                worker thread.
//    SetWatchdogTimeout      - Sets a watchdog timer to reduce high priority
//                                threads' priorities back to normal
//    Watchdog                - The watchdog timer that reduces thread
//                                priorities
//
//  COMMENTS:
//
//


#include <windows.h>            // required for all Windows applications
#include <windowsx.h>
#include "globals.h"            // prototypes specific to this application


// Local function prototypes
DWORD WINAPI Watchdog (LPVOID lpvThread);

//
//  FUNCTION: CenterWindow(HWND, HWND)
//
//  PURPOSE:  Center one window over another.
//
//  PARAMETERS:
//    hwndChild  - The handle of the window to be centered.
//    hwndParent - The handle of the window to center on.
//
//  RETURN VALUE:
//
//    TRUE  - Success
//    FALSE - Failure
//
//  COMMENTS:
//
//    Dialog boxes take on the screen position that they were designed
//    at, which is not always appropriate. Centering the dialog over a
//    particular window usually results in a better position.
//

BOOL CenterWindow(HWND hwndChild, HWND hwndParent)
{
    RECT rcChild, rcParent;
    int  cxChild, cyChild, cxParent, cyParent;
    int  cxScreen, cyScreen, xNew, yNew;
    HDC  hdc;

    // Get the Height and Width of the child window
    GetWindowRect(hwndChild, &rcChild);
    cxChild = rcChild.right - rcChild.left;
    cyChild = rcChild.bottom - rcChild.top;

    // Get the Height and Width of the parent window
    GetWindowRect(hwndParent, &rcParent);
    cxParent = rcParent.right - rcParent.left;
    cyParent = rcParent.bottom - rcParent.top;

    // Get the display limits
    hdc = GetDC(hwndChild);
    cxScreen = GetDeviceCaps(hdc, HORZRES);
    cyScreen = GetDeviceCaps(hdc, VERTRES);
    ReleaseDC(hwndChild, hdc);

    // Calculate new X position, then adjust for screen
    xNew = rcParent.left + ((cxParent - cxChild) / 2);
    if (xNew < 0)
    {
        xNew = 0;
    }
    else if ((xNew + cxChild) > cxScreen)
    {
        xNew = cxScreen - cxChild;
    }

    // Calculate new Y position, then adjust for screen
    yNew = rcParent.top  + ((cyParent - cyChild) / 2);
    if (yNew < 0)
    {
        yNew = 0;
    }
    else if ((yNew + cyChild) > cyScreen)
    {
        yNew = cyScreen - cyChild;
    }

    // Set it, and return
    return SetWindowPos(hwndChild,
                        NULL,
                        xNew, yNew,
                        0, 0,
                        SWP_NOSIZE | SWP_NOZORDER);
}


//
//  FUNCTION: AllocThreadData(void)
//
//  PURPOSE: To allocate a data block that will be shared between a child
//           window procedure and its corresponding thread.
//
//  PARAMETERS: None.
//
//
//  RETURN VALUE:
//    If successful, AllocThreadData returns a pointer to the allocated
//    data.
//    If AllocThreadData couldn't allocate the data, it returns NULL.
//
//  COMMENTS:
//
//

PTHREAD_DATA AllocThreadData (void)
{
    return HeapAlloc (GetProcessHeap(),
                      HEAP_ZERO_MEMORY,
                      sizeof (THREAD_DATA)
                     );
}


//
//  FUNCTION: FreeThreadData(PTHREAD_DATA)
//
//  PURPOSE:  Frees the data block shared by a child window procedure
//            and its corresponding thread.
//
//  PARAMETERS:
//    pData    -  Pointer to the THREAD_DATA block to be freed.
//
//
//  RETURN VALUE:
//    TRUE  - Succeeded in freeing the data block.
//    FALSE - Failed to free the data block.
//
//  COMMENTS:
//
//

BOOL FreeThreadData (PTHREAD_DATA pData)
{
    return HeapFree (GetProcessHeap(), 0, pData);
}


//
//  FUNCTION: FillListBox(HWND, int, LPLISTBOXDATA, int)
//
//  PURPOSE:  To fill a listbox with strings and item data.
//
//  PARAMETERS:
//    hdlg     -  The dialog box handle.
//    int      -  Child ID of the listbox.
//    lpData   -  Pointer to the list of strings and item data to put
//                into the listbox.
//    cItems   -  Number of items to put into the listbox--usually the
//                number of items in the list pointed to by lpData.
//
//
//  RETURN VALUE:
//    TRUE  - Succeeded in filling the listbox.
//    FALSE - Failed to fill listbox with all strings and item data in
//            lpData.
//
//  COMMENTS:
//
//

BOOL FillListBox (HWND          hdlg,
                  int           idListBox,
                  LPLISTBOXDATA lpData,
                  int           cItems)
{
    LONG lIndex, lResult;
    int  i;

    for (i = 0; i < cItems; i++)
    {
        // Put the string into the listbox.  If an error occurs, clear the
        // listbox's contents and exit.
        lIndex = SendDlgItemMessage(hdlg,
                                    idListBox,
                                    LB_ADDSTRING,
                                    0,
                                    (LPARAM)lpData[i].szString);

        if (lIndex == LB_ERR || lIndex == LB_ERRSPACE)
        {
            SendDlgItemMessage(hdlg, idListBox, LB_RESETCONTENT, 0, 0);
            return FALSE;
        }


        // Put the string's item data into the listbox.  If an error occurs,
        // clear the listbox's contents and exit.
        lResult = SendDlgItemMessage(hdlg,
                                     idListBox,
                                     LB_SETITEMDATA,
                                     (WPARAM)lIndex,
                                     (LPARAM)lpData[i].dwData);

        if (lResult == LB_ERR)
        {
            SendDlgItemMessage(hdlg, idListBox, LB_RESETCONTENT, 0, 0);
            return FALSE;
        }
    }
    return TRUE;
}


//
//  FUNCTION: GetCurChildWindowThread(void)
//
//  PURPOSE:  To retrieve the handle of the worker thread that works on the
//            currently active MDI child window.
//
//  PARAMETERS:
//    None
//
//  RETURN VALUE:
//    If successful, returns the thread handle that works on the active MDI
//    child window.
//    If unsuccessful, returns NULL.
//
//  COMMENTS:
//
//

HANDLE GetCurChildWindowThread (void)
{
    PTHREAD_DATA lpThreadData = NULL;
    HWND         hwndActive   = NULL;
    HANDLE       hThread      = NULL;


    // Get the currently active MDI child window.  If there is no active
    // child window, return NULL.
    // Get its thread data, and then get the thread handle out of the
    // thread data.

    hwndActive = (HWND)SendMessage(ghwndMDIClient, WM_MDIGETACTIVE, 0, 0);

    if (!IsWindow(hwndActive))
        return NULL;

    lpThreadData = (PTHREAD_DATA)GetWindowLong(hwndActive, GWL_USERDATA);

    // Should aways access data shared between two threads in a synchronized
    // way.
    EnterCriticalSection(&(lpThreadData -> csCritSec));
    hThread = lpThreadData -> hThread;
    LeaveCriticalSection(&(lpThreadData -> csCritSec));

    return hThread;
}


//
//  FUNCTION: SetWatchdogTimeout(HANDLE)
//
//  PURPOSE:  To start the watchdog that prevents a high priority thread from
//    hogging the system forever
//
//  PARAMETERS:
//    HANDLE hThreadToWatch -- Handle of the thread to reduce priority of
//
//  RETURN VALUE:
//    None
//
//  COMMENTS:  Creates a watchdog thread that runs with time critical priority.
//    For the most part, the watchdog thread sleeps.  When the time for it
//    awaken has elapsed, it will preempt the thread it is supposed to watch
//    and reduce its priority.
//
//

void SetWatchdogTimeout (HANDLE hThreadToWatch)
{
   HANDLE hWatchdogThread;
   DWORD  tid;

   hWatchdogThread = CreateThread (NULL, 0, Watchdog, hThreadToWatch,
                                   0, &tid);
   if (hWatchdogThread)
   {
      SetThreadPriority (hWatchdogThread, THREAD_PRIORITY_TIME_CRITICAL);
      CloseHandle (hWatchdogThread);
   }
}


//
//  FUNCTION: Watchdog (LPVOID)
//
//  PURPOSE:  Prevent a high priority thread from hogging the system forever
//
//  PARAMETERS:
//    LPVOID lpvThread -- Handle of the thread to reduce priority of
//
//  RETURN VALUE:
//    Always returns 1.
//
//  COMMENTS:  Runs with time critical priority. For the most part, the
//    watchdog thread sleeps.  When the time for it awaken has elapsed, it
//    will preempt the thread it is supposed to watch and reduce its priority.
//
//

DWORD WINAPI Watchdog (LPVOID lpvThread)
{
   Sleep (20000);  // sleep for 20 sec
   SetThreadPriority ((HANDLE)lpvThread, THREAD_PRIORITY_NORMAL);
   return 1;
}
