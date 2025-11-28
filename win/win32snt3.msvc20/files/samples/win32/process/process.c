/******************************************************************************\
*       This is a part of the Microsoft Source Code Samples. 
*       Copyright (C) 1993 Microsoft Corporation.
*       All rights reserved. 
*       This source code is only intended as a supplement to 
*       Microsoft Development Tools and/or WinHelp documentation.
*       See these sources for detailed information regarding the 
*       Microsoft samples programs.
\******************************************************************************/

/**************************************************************************\
*  process.c -- sample program demonstrating the CreateProcess and
*   TerminateProcess() functions.
*
*  In this sample the main window is a dialog box.  There is no need to
*   register a new window class or create a new window.  Instead just call
*   DialogBox() and use the template defined in the .RC file.
*
\**************************************************************************/

#include <windows.h>
#include <stdio.h>
#include <commdlg.h>
#include "process.h"

/* declare a global instance handle because it is needed for the open
 *  file common dialog box later on.
 */
HANDLE hInst;

/**************************************************************************\
*
*  function:  WinMain()
*
*  input parameters:  c.f. generic sample
*
\**************************************************************************/
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
    int ret;

    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );
    UNREFERENCED_PARAMETER( nCmdShow);

    hInst = hInstance;

    ret = DialogBox (hInstance, "processDlg", NULL, (DLGPROC)MainDlgProc);
    return ret;
}


/**************************************************************************\
*
*  function:  MainDlgProc()
*
*  input parameters:  standard window procedure parameters.
*
\**************************************************************************/
LRESULT CALLBACK MainDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch (message) {

    /********************************************************************\
    * WM_INITDIALOG
    *
    * Establish the correct tab stops for both of the list boxes.
    *  Fill in the header list box.
    \********************************************************************/
    case WM_INITDIALOG: {
      DWORD  tabs[4];

      tabs[0] =           (WORD)(sizeof("00000000 ") *4);
      tabs[1] = tabs[0] + (WORD)(sizeof("00000000 ") *4);
      tabs[2] = tabs[1] + (WORD)(sizeof("dwProcessID ") *4);
      tabs[3] = tabs[2] + (WORD)(sizeof("dwThreadID ") *4);

      SendDlgItemMessage (hwnd, DID_LISTBOX, LB_SETTABSTOPS, 4, (LONG) tabs);
      SendDlgItemMessage (hwnd, DID_HEADER,  LB_SETTABSTOPS, 4, (LONG) tabs);

      SendDlgItemMessage (hwnd, DID_HEADER,  LB_ADDSTRING,   0, (LONG)
            "hProcess \thThread \tdwProcessID \tdwThreadID \tImage file");

    } return TRUE;

    /********************************************************************\
    * WM_SYSCOMMAND
    *
    * ignore all syscommand messages, except for SC_CLOSE.
    *  on this one, call EndDialog().
    \********************************************************************/
    case WM_SYSCOMMAND:
      if (wParam == SC_CLOSE) {
        EndDialog (hwnd, TRUE);
        return TRUE;
      } else
        return FALSE;
    break;


    /********************************************************************\
    * WM_COMMAND
    *
    * When the different buttons are hit, clear the list box, disable
    *  updating to it, call the function which will fill it, reenable
    *  updating, and then force a repaint.
    *
    \********************************************************************/
    case WM_COMMAND:

      /* if the list box sends back messages, return.  */
      if (LOWORD(wParam)==DID_LISTBOX) return TRUE;

      /* switch on the control ID of the button that is pressed. */
      switch (LOWORD(wParam)) {
        case DID_CREATE :    doCreate (hwnd);   break;
        case DID_TERMINATE : doTerminate(hwnd); break;
      } /* end switch (LOWORD()) */

      return TRUE;
    break; /* end WM_COMMAND */


    default: return FALSE;
  } /* end switch(message) */
  return 0;
}


/**************************************************************************\
*
*  function:  doCreate()
*
*  input parameters: hwnd - window handle for the dialog.
*  global parameters: hInst (for the OPENFILENAME struct.)
*
* Call GetOpenFileName() in order to get the name of some EXE file,
*  then call CreatProcess() and put the results in the list box.
*
\**************************************************************************/
VOID doCreate (HWND hwnd)
{
    OPENFILENAME         of;
    char buffer [MAXCHARS];
    char bufferLB [MAXCHARS];
    STARTUPINFO          sui;
    PROCESS_INFORMATION  pi;
    DWORD ret;

    buffer[0] = 0;

    /* set up the OPENFILE structure,
     *  then use the appropriate common dialog
     */
    of.lStructSize       = sizeof (OPENFILENAME);
    of.hwndOwner         = NULL;
    of.hInstance         = hInst;
    of.lpstrFilter       = "Executables\000 *.EXE\000\000";
    of.lpstrCustomFilter = NULL;
    of.nMaxCustFilter    = 0;
    of.nFilterIndex      = 0;
    of.lpstrFile         = buffer;
    of.nMaxFile          = MAXCHARS;
    of.lpstrFileTitle    = NULL;
    of.nMaxFileTitle     = 0;
    of.lpstrInitialDir   = NULL;
    of.lpstrTitle        = NULL;
    of.Flags             = OFN_HIDEREADONLY;
    of.nFileOffset       = 0;
    of.nFileExtension    = 0;
    of.lpstrDefExt       = NULL;
    of.lCustData         = 0;
    of.lpfnHook          = NULL;
    of.lpTemplateName    = NULL;
    if (!GetOpenFileName (&of)) return;


    /* set up the STARTUPINFO structure,
     *  then call CreateProcess to try and start the new exe.
     */
    sui.cb               = sizeof (STARTUPINFO);
    sui.lpReserved       = 0;
    sui.lpDesktop        = NULL;
    sui.lpTitle          = NULL;
    sui.dwX              = 0;
    sui.dwY              = 0;
    sui.dwXSize          = 0;
    sui.dwYSize          = 0;
    sui.dwXCountChars    = 0;
    sui.dwYCountChars    = 0;
    sui.dwFillAttribute  = 0;
    sui.dwFlags          = 0;
    sui.wShowWindow      = 0;
    sui.cbReserved2      = 0;
    sui.lpReserved2      = 0;

    /**********************************************/
    /**********************************************/
    ret = CreateProcess (buffer, NULL, NULL, NULL,
                   FALSE, DETACHED_PROCESS,
                   NULL, NULL, &sui, &pi );
    /**********************************************/
    /**********************************************/


    if (ret == TRUE){
      /* Put the data about the new process in the list box. */
      wsprintf (bufferLB, "%08lx \t%08lx \t%d  \t%d \t%s",
                  (LONG) pi.hProcess,    (LONG) pi.hThread,
                  (DWORD)pi.dwProcessId, (DWORD)pi.dwThreadId, buffer);
      SendDlgItemMessage (hwnd, DID_LISTBOX, LB_ADDSTRING, 0, (LONG) bufferLB);
    } else {
      /* report failure to the user. */
      ret = GetLastError ();
      wsprintf (buffer, "GetLastError = 0x%lx", (int)ret);
      MessageBox (hwnd, buffer, "Process", MB_ICONSTOP | MB_OK);
    }

    return;
}


/**************************************************************************\
*
*  function:  doTerminate()
*
*  input parameters: hwnd - window handle for the dialog.
*
* Determine which process is selected in the list box.  Then parse the
*  process handle out of that string and call TerminateProcess() with it.
*


Warning:

"TerminateProcess is used to cause all of the threads within a process
to terminate.  While TerminateProcess will cause all threads within a
process to terminate, and will cause an application to exit, it does
not notify DLLs that the process is attached to that the process is
terminating.  TerminateProcess is used to unconditionally cause a
process to exit. It should only be used in extreme circumstances.
The state of global data maintained by DLLs may be compromised if
TerminateProcess is used rather that ExitProcess."

\**************************************************************************/

VOID doTerminate(HWND hwnd)
{
    char buffer [MAXCHARS];
    int  sel;
    DWORD ret;
    HANDLE hProcess;

    /* determine which item is selected in the list box, and get the text */
    sel= SendDlgItemMessage (hwnd, DID_LISTBOX, LB_GETCURSEL, 0, 0);
    if (sel == LB_ERR) {
      MessageBox (hwnd, "No listbox item is selected.",
                  "Process", MB_ICONSTOP | MB_OK);
      return;
    }
    SendDlgItemMessage (hwnd, DID_LISTBOX, LB_GETTEXT, sel, (LONG)buffer);

    /* pick the process handle out of the string. */
    sscanf (buffer, " %lx", &hProcess);

    /**********************************************/
    /**********************************************/
    ret = TerminateProcess (hProcess, 0);
    /**********************************************/
    /**********************************************/

    if (ret == TRUE){
      SendDlgItemMessage (hwnd, DID_LISTBOX, LB_DELETESTRING, sel, 0);
    } else {
      ret = GetLastError ();
      wsprintf (buffer, "0x%lx", (int)ret);
      MessageBox (hwnd, buffer, "Process", MB_ICONSTOP | MB_OK);
    }

  return ;
}
