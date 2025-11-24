//THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
//ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
//THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
//
// Copyright  1993-1995  Microsoft Corporation.  All Rights Reserved.
//
//      MODULE:         smmhook.c
//
//      PURPOSE:        Module initialization and Session Management Module
//                      Service Provider Interface entry points
//
//	PLATFORMS:	Windows 95
//
//      FUNCTIONS:
//              _ProcessAttach() - initializes the module per process
//              _ProcessDetach() - cleans up the module when a process terminates
//              LibMain() - module's main entry point
//              RnaSessInitialize() - initializes Session Management Module
//              SMMSessStart() - starts managing Dial-Up Networking session
//              SMMSessStop() - terminates Dial-Up Networking session management
//
//	SPECIAL INSTRUCTIONS: N/A
//

#include "smmhook.h" // includes common header files and global declarations

HANDLE              ghInstance   = NULL;

HANDLE              ghPPPSMM     = NULL;
SESSINITIALIZEPROC  gpfnSessInit = NULL;

RNA_FUNCS           gRnaFuncs    = {0, NULL, NULL};

char                c_szPPPType[]    = "PPP";
char                c_szPPPSMM[]     = "rasapi32.dll";
char                c_szSessInit[]   = "RnaSessInitialize";
char                c_szDeviceClass[]= "COM";

//
//
//   FUNCTION: _ProcessAttach (HINSTANCE)
//
//   PURPOSE: initializes the module when a process is attached
//
//   COMMENTS:
//	
//      This function loads the SMM module that it hooks into and acquires
//      the module's initialization entry point.
// 

BOOL _ProcessAttach(HINSTANCE hDll)
{
  // Hook the real SMM
  //
  if ((ghPPPSMM = LoadLibrary(c_szPPPSMM)) != NULL)
  {
    if ((gpfnSessInit = (SESSINITIALIZEPROC)GetProcAddress(ghPPPSMM,
                                                           c_szSessInit))
      != NULL)
    {
      // Initialize the control block list
      //
      InitACBList();
      ghInstance = hDll;
      return TRUE;
    };

    FreeLibrary(ghPPPSMM);
    ghPPPSMM = NULL;
  };

  return FALSE;
}

//
//
//   FUNCTION: _ProcessDetach (HINSTANCE)
//
//   PURPOSE: cleans up the module when a process is detached.
//
//   COMMENTS:
//	
//      This function unloads the SMM module it hooks into.
// 

BOOL _ProcessDetach(HINSTANCE hDll)
{
  // Have we initialized successfully?
  //
  if (ghInstance != NULL)
  {
    DeInitACBList();

    gRnaFuncs.dwSize    = 0;
    gRnaFuncs.lpfnStart = NULL;
    gRnaFuncs.lpfnStop  = NULL;

    FreeLibrary(ghPPPSMM);
    ghPPPSMM = NULL;

    ghInstance = NULL;
  };

  return TRUE;
}

//
//
//   FUNCTION: LibMain (HINSTANCE, DWORD, LPVOID)
//
//   PURPOSE: the module's main entry point.
//
// 

BOOL APIENTRY LibMain(HANDLE hDll, DWORD dwReason,  LPVOID lpReserved)
{
  switch(dwReason)
  {
    case DLL_PROCESS_ATTACH:
      _ProcessAttach(hDll);
      break;

    case DLL_PROCESS_DETACH:
      _ProcessDetach(hDll);
      break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
      default:
      break;
  }

  return TRUE;
}

//
//
//   FUNCTION: RnaSessInitialize (LPSTR, LPRNA_FUNCS)
//
//   PURPOSE: initializes the Session Management information
//
//   COMMENTS:
//	
//      This function acquires the entry points of the SMM module's that it
//      hooks into. Dial-Up Networking calls this function for the module to
//      prepare for a session.
// 

DWORD WINAPI RnaSessInitialize (LPSTR lpszType, LPRNA_FUNCS lpRnaFuncs)
{
  RNA_FUNCS RnaFuncs;
  DWORD     dwRet;

  // Call the hooked SMM init entry
  //
  if ((dwRet = (*gpfnSessInit)(c_szPPPType, &RnaFuncs)) == ERROR_SUCCESS)
  {
    // If we do not have other entry points, get them
    //
    if (gRnaFuncs.dwSize == 0)
    {
      gRnaFuncs = RnaFuncs;
    };

    // Return the hooked entry points
    //
    lpRnaFuncs->lpfnStart = &SMMSessStart;
    lpRnaFuncs->lpfnStop  = &SMMSessStop;
  };

  return dwRet;
}

//
//
//   FUNCTION: RnaSessGetErrorString (UINT, LPSTR, DWORD)
//
//   PURPOSE: Gets error string
//
//   COMMENTS:
//	
//      This function gets an error string from the overlaid SMM module
// 

DWORD WINAPI RnaSessGetErrorString (UINT ids, LPSTR lpszError, DWORD cb)
{
  SESSERRORPROC lpfn;
  DWORD dwRet;

  // If the overlaid SMM has the function, use it
  //
  if (lpfn = (SESSERRORPROC)GetProcAddress(ghPPPSMM,
                                           SESS_GETERROR_FUNC))
  {
    dwRet = (*lpfn)(ids, lpszError, cb);
  }
  else
  {
    // The overlaid SMM does not have the function, try load the string directly
    //
    dwRet = LoadString(ghPPPSMM, ids, lpszError, cb);
  };

  return dwRet;
}

//
//
//   FUNCTION: SMMSessStart (HANDLE, LPSESS_CONFIGURATION_INFO)
//
//   PURPOSE: starts managing the Dial-Up Networking session
//
// 

DWORD WINAPI SMMSessStart (HANDLE hConn, LPSESS_CONFIGURATION_INFO lpSCI)
{
  LPAECB            lpAECB;
  DEVICE_PORT_INFO  dpi;
  DWORD             dwRet;

  // Make sure we can handle the call
  //
  if (gRnaFuncs.lpfnStart == NULL)
    return ERROR_INVALID_SMM;

  // Make sure we can handle this version
  //
  if (!lpSCI || (lpSCI->dwSize != sizeof(SESS_CONFIGURATION_INFO)))
    return ERROR_BUFFER_INVALID;

  // Request for the connection information
  //
  dpi.dwSize = sizeof(dpi);
  if ((dwRet = RnaGetDevicePort (hConn, &dpi)) != ERROR_SUCCESS)
    return dwRet;

  // Bypass imediately if
  //    1) it is for a responder, or
  //    2) we do not handle this device class
  //
  if ((lpSCI->stSessType == SESSTYPE_RESPONDER) ||
      (lstrcmpi(dpi.szDeviceClass, c_szDeviceClass)))
  {
    return ((*gRnaFuncs.lpfnStart)(hConn, lpSCI));
  };

  // Search a AECB for the port (if not found create one)
  //
  if (!(lpAECB = (LPAECB)FindACBFromConn(hConn, sizeof(AECB))))
      return ERROR_INVALID_HANDLE;

  // Initialize the structure information
  //
  lpAECB->sci = *lpSCI;
  if ((lpAECB->hStop = CreateEvent (NULL, FALSE, FALSE, NULL)) != NULL)
  {
    // Start the configuration thread
    //
    if ((lpAECB->hThread = CreateThread (NULL, 0,
                                         (LPTHREAD_START_ROUTINE) SMMSessThread,
                                         lpAECB, 0, &lpAECB->idThread))
        != NULL)
    {
      dwRet = ERROR_SUCCESS;
    }
    else
    {
      dwRet= GetLastError();
    };
  }
  else
  {
    dwRet= GetLastError();
  };

  // If not successful, clean up
  //
  if (dwRet != ERROR_SUCCESS)
  {
    if (lpAECB->hStop)
    {
      CloseHandle(lpAECB->hStop);
    };
    CleanupACB((LPACB_HEADER)lpAECB);
  };

  return dwRet;
}

//
//
//   FUNCTION: SMMSessStop (HANDLE)
//
//   PURPOSE: stop managing the Dial-Up Networking session
//
// 

DWORD WINAPI SMMSessStop (HANDLE hConn)
{
  LPAECB    lpAECB;

  // Search a AECB for the port.
  // If not found, we can bypass to the real SMM.
  //
  if (!(lpAECB = (LPAECB)FindACBFromConn(hConn, 0)))
  {
    return ((*gRnaFuncs.lpfnStop)(hConn));
  };

  // Notify the configuration thread
  //
  SetEvent(lpAECB->hStop);
  CloseThreadWindows(lpAECB->idThread);

  return ERROR_SUCCESS;
}
