/*+==========================================================================
  File:      DLLSKEL.CPP

  Summary:   Implementation file for the general DLL skeleton
             that can be used as a point of departure for more complex
             OLE Win32 DLLs.  It is used as a skeletal base for
             the OLE Tutorial series of code samples.

             DLLSKEL.CPP contains the DllMain entry function for the DLL.
             As a simple skeleton, this DLL contains only two
             representative exported function calls: DllHelloBox and
             DllAboutBox.

             For a comprehensive tutorial code tour of DLLSKEL's
             contents and offerings see the accompanying DLLSKEL.TXT file.
             For more specific technical details on the internal workings
             see the comments dispersed throughout the DLLSKEL source code.

  Classes:   none.

  Functions: DllMain, DllHelloBox, DllAboutBox

  Origin:    8-3-95: atrent - Created via editor-inheritance and enhancement
               of DLLSKEL.C in the Win32 SDK FRMWORK code samples.

----------------------------------------------------------------------------
  This file is part of the Microsoft OLE Tutorial Code Samples.

  Copyright (C) Microsoft Corporation, 1996.  All rights reserved.

  This source code is intended only as a supplement to Microsoft
  Development Tools and/or on-line documentation.  See these other
  materials for detailed information regarding Microsoft code samples.

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
  PARTICULAR PURPOSE.
==========================================================================+*/

/*---------------------------------------------------------------------------
  We include WINDOWS.H for all Win32 applications.
  We include OLE2.H because we will be making calls to the OLE Libraries
    in future exploitation of this DLL skeleton.
  We include APPUTIL.H because we will be building this DLL using
    the convenient Virtual Window and Dialog classes and other
    utility functions in the APPUTIL Library (ie, APPUTIL.LIB).
  We include DLLSKELI.H because it has internal class declarations and
    resource ID definitions specific for this DLL.
  We include DLLSKEL.H because it has the necessary STDENTRY function
    prototypes.  The _DLLEXPORT_ #define is used to tell DLLSKEL.H to
    define the appropriate functions as exported from this DLL.
    Otherwise, DLLSKEL.H is included by users of this DLL who do not
    define _DLLEXPORT_ so that the appropriate functions are then
    declared as imports rather than defined as exports.
---------------------------------------------------------------------------*/
#include <windows.h>
#include <ole2.h>
#include <apputil.h>
#include "dllskeli.h"
#define _DLLEXPORT_
#include "dllskel.h"


// Global variable definitions. Some Initialized in DllMain() below.

// We define a global count variable and tell the compiler via a pragma
// to put that variable in a data section (called "Shared") that is
// shared by all loaded instances of this DLL.
#pragma data_seg("Shared")
// A place to store a global count of DllHelloBox calls.
int g_iHelloCount = 0;
#pragma data_seg()

// We tell the linker to make the Shared data section readable, writable,
// and shared.
#pragma comment(lib, "msvcrt " "-section:Shared,rws")

// We encapsulate some of the global instance data for clarity.  Here is a
// pointer to the DllData object.
CDllData* g_pDll;


/*F+F++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  Function: UnicodeOk

  Summary:  Checks if the platform will handle unicode versions of
            Win32 string API calls.

  Args:     void

  Returns:  BOOL
              TRUE if unicode support; FALSE if not.
------------------------------------------------------------------------F-F*/
BOOL UnicodeOk(void)
{
  BOOL bOk = TRUE;
  TCHAR szUserName[MAX_STRING_LENGTH];
  DWORD dwSize = MAX_STRING_LENGTH;

  if (!GetUserName(szUserName, &dwSize))
    bOk = ERROR_CALL_NOT_IMPLEMENTED == GetLastError() ? FALSE : TRUE;

  return bOk;
}


/*F+F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F
  Function: DllMain

  Summary:  Like WinMain is for an EXE application, this DllMain function
            is the main entry point for this DLL.  It is called when the
            DLL is loaded by a process, and when new threads are created
            by a process that has already loaded this DLL.  DllMain is also
            called when threads of a process that has loaded the DLL exit
            cleanly and when the process itself unloads the DLL.

            If you want to use C runtime libraries, keep this function
            named "DllMain" and you won't have to do anything special to
            initialize the runtime libraries.

            When fdwReason == DLL_PROCESS_ATTACH, the return value is used
            to determine if the DLL should remain loaded, or should be
            immediately unloaded depending upon whether the DLL could be
            initialized properly.  For all other values of fdwReason,
            the return value is ignored.

  Args:     HINSTANCE hDllInst,
              Instance handle of the DLL.
            DWORD fdwReason,
              Process attach/detach or thread attach/detach.
              Reason for calling.
            LPVOID lpvReserved)
              Reserved and not used.

  Returns:  BOOL,
              Return value is used only when fdwReason == DLL_PROCESS_ATTACH.
              TRUE  -  Used to signify that the DLL should remain loaded.
              FALSE -  Used to signify that the DLL should be
                immediately unloaded.
F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F-F*/
BOOL WINAPI DllMain(
              HINSTANCE hDllInst,
              DWORD fdwReason,
              LPVOID lpvReserved)
{
  BOOL bResult = TRUE;

  // Dispatch this call based on the reason it was called.
  switch (fdwReason)
  {
    case DLL_PROCESS_ATTACH:
      // The DLL is being loaded for the first time by a given process.
      // Perform per-process initialization here.  If the initialization
      // is successful, return TRUE; if unsuccessful, return FALSE.
      bResult = FALSE;
      if (UnicodeOk())
      {
        // Instantiate a DLL global data encapsulator class.
        g_pDll = new CDllData;
        if (NULL != g_pDll)
        {
          // Remember the DLL Instance handle.
          g_pDll->hDllInst = hDllInst;
          // Create a MsgBox object.
          g_pDll->pMsgBox = new CMsgBox;
          if (NULL != g_pDll->pMsgBox)
            bResult = TRUE;
        }
      }
      break;

    case DLL_PROCESS_DETACH:
      // The DLL is being unloaded by a given process.  Do any
      // per-process clean up here, such as undoing what was done in
      // DLL_PROCESS_ATTACH.  The return value is ignored.
      if (NULL != g_pDll)
      {
        DELETE_POINTER(g_pDll->pMsgBox);
        DELETE_POINTER(g_pDll);
      }
      break;

    case DLL_THREAD_ATTACH:
      // A thread is being created in a process that has already loaded
      // this DLL.  Perform any per-thread initialization here.  The
      // return value is ignored.
      break;

    case DLL_THREAD_DETACH:
      // A thread is exiting cleanly in a process that has already
      // loaded this DLL.  Perform any per-thread clean up here.  The
      // return value is ignored.
      break;

    default:
      break;
  }

  return (bResult);
}


/*F+F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F
  Function: DllHelloBox

  Summary:  One of the exported service functions of this DLL.  In this
            simple code sample, DllHelloBox uses a critical section to
            govern access to incrementing a global count of the number
            of times this function was called.  A message box is shown
            to display the instance handle of this DLL and the number
            of times this DllHelloBox has been called.  See DLLSKEL.TXT
            for more details.

  Args:     HWND hWnd)
              Handle of the window that is to be parent of message box.

  Returns:  BOOL,
              Always returns TRUE.
F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F-F*/
STDENTRY_(BOOL) DllHelloBox(
                  HWND hWnd)
{
  int iHelloCount;

  // Increment the global count of Hellos.
  InterlockedIncrement((LPLONG) &g_iHelloCount);
  iHelloCount = g_iHelloCount;

  // Now show the user a -Notice- message box and load the display strings
  // out of this DLL's resources.  Use the format string to show the
  // instance handle of this loaded DLL (in hex) and the hello count.
  g_pDll->pMsgBox->Init(g_pDll->hDllInst, hWnd);
  g_pDll->pMsgBox->NoteFmtID(
            IDS_HELLOCOUNTFMT,
            g_pDll->hDllInst,
            iHelloCount);

  return TRUE;
}


/*F+F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F
  Function: DllAboutBox

  Summary:  One of the exported service functions of this DLL.  In this
            simple code sample, DllAboutBox showcases use of the
            CAboutBox class (from the APPUTIL utility library).  It also
            illustrates how to implement this dialog using resources
            stored in this DLL itself.  See DLLSKEL.TXT for details on
            how this DLLSKEL.DLL is exploited by the DLLUSER.EXE
            application.

  Args:     HWND hWnd)
              Handle of window that is to be parent of the dialog window.

  Returns:  BOOL,
              Always returns TRUE.
F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F-F*/
STDENTRY_(BOOL) DllAboutBox(
                  HWND hWnd)
{
  // Define one of those nifty APPUTIL CAboutBox modal dialog objects.
  CAboutBox dlgAboutBox;

  // Show the standard About Box dialog for this DLL by telling the dialog
  // C++ object to show itself by invoking its ShowDialog method.
  // Pass it this DLL instance and the parent window handle.  Use a dialog
  // resource ID for the dialog stored in this DLL module's resources.
  dlgAboutBox.ShowDialog(
    g_pDll->hDllInst,
    MAKEINTRESOURCE(IDD_ABOUTBOX),
    hWnd);

  return TRUE;
}
