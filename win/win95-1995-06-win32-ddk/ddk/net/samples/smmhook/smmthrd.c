//THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
//ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
//THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
//
// Copyright  1993-1995  Microsoft Corporation.  All Rights Reserved.
//
//      MODULE:         smmthrd.c
//
//      PURPOSE:        The Session Management thread
//
//	PLATFORMS:	Windows 95
//
//      FUNCTIONS:
//              SMMSessThread() - the entry point to the Session Management thread
//              UseConnection() - transfers data across the connection
//
//	SPECIAL INSTRUCTIONS: N/A
//

#include "smmhook.h" // includes common header files and global declarations
#include <tapi.h>    // includes TAPI function prototypes and data types

typedef struct tagLineModemID {
    VARSTRING   varstring;
    HANDLE      hComm;
    char        szDeviceName[1];
}   LINEMODEMID, * PLINEMODEMID, * LPLINEMODEMID;

char    c_szModemClass[] = "comm/datamodem";

DWORD NEAR PASCAL UseConnection (HANDLE hConn);
BOOL  NEAR PASCAL TransferData(HANDLE hComm);

//
//
//   FUNCTION: SMMSessThread (LPAECB)
//
//   PURPOSE: the entry point for a Session Managment thread
//
//   COMMENTS:
//	
//      This thread carries out the session management before handing off
//      the control to the SMM module it hooks into. This function also
//      demenonstrates how the module hands off the control to the SMM module
//      it hooks into and how to terminate the session in a failure case.
// 

void WINAPI SMMSessThread (LPAECB lpAECB)
{
  HANDLE                    hConn;
  SESS_CONFIGURATION_INFO   sci;
  HANDLE                    hStop;
  HANDLE                    hThread;
  DWORD                     dwRet;

  // Copy the information
  //
  hConn   = lpAECB->hdr.hConn;
  sci     = lpAECB->sci;
  hStop   = lpAECB->hStop;
  hThread = lpAECB->hThread;

  //**************************************************************************
  // Start using the connection
  //**************************************************************************

  dwRet = UseConnection(hConn);

  //**************************************************************************
  // Stop using the connection
  //**************************************************************************

  // We might have been stopped already
  //
  if (WaitForSingleObject(hStop, 0) == WAIT_OBJECT_0)
  {
    // We were stopped, terminate ourselves
    //
    RnaTerminate (hConn, hThread);
  }
  else
  {
    // Now we can bypass to the real SMM
    //
    if (dwRet == ERROR_SUCCESS)
    {
      dwRet = (*gRnaFuncs.lpfnStart)(hConn, &sci);
    };

    // If the real SMM cannot be started, we need to fail the session
    //
    if (dwRet != ERROR_SUCCESS)
    {
      COMPLETE_INFO   ci;

      // Report the error condition to Dial-Up Networking
      //
      ci.dwSize   = sizeof(ci);
      ci.dwResult = dwRet;
      ci.idMsg    = 0;
      ci.fUnload  = FALSE;
      ci.hThread  = NULL;

      RnaComplete(hConn, &ci, NULL, 0);

      // Wait until Dial-Up Networking notifies us to stop
      //
      WaitForSingleObject(hStop, INFINITE);

      // Notifies Dial-Up Networking that we are ready to terminate
      //
      RnaTerminate (hConn, hThread);
    }
    else
    {
      // We may be stopped while we are starting, bypass the stop call
      //
      if (WaitForSingleObject(hStop, 0) == WAIT_OBJECT_0)
      {
        (*gRnaFuncs.lpfnStop)(hConn);
      };
    };
  };

  // We finish our hook, cleanup the resources
  //
  CleanupACB((LPACB_HEADER)lpAECB);

  CloseHandle(hStop);
  ExitThread(ERROR_SUCCESS);
}

//
//
//   FUNCTION: UseConnection (HANDLE)
//
//   PURPOSE: transfers data across the connection
//
//   COMMENTS:
//
//      This function demonstrates how to obtain the communication port from
//      the conenction.
// 

DWORD NEAR PASCAL UseConnection (HANDLE hConn)
{
  DEVICE_PORT_INFO  dpi;
  VARSTRING         varstring;
  LPLINEMODEMID     lplmid;
  HANDLE            hComm;
  DWORD             dwRet;

  // Request for the connection information
  //
  dpi.dwSize = sizeof(dpi);
  if ((dwRet = RnaGetDevicePort (hConn, &dpi)) != ERROR_SUCCESS)
    return dwRet;

  // Get the communication port from TAPI
  //
  varstring.dwTotalSize = sizeof(VARSTRING);
  if ((dwRet = lineGetID(dpi.hLine, dpi.dwAddressID, dpi.hCall,
                         LINECALLSELECT_LINE,
                         (LPVARSTRING)&varstring,
                         c_szModemClass)) != ERROR_SUCCESS)
    return dwRet;

  if ((lplmid = (LPLINEMODEMID)LocalAlloc(LMEM_FIXED,
                                          varstring.dwNeededSize)) == NULL)
    return ERROR_OUTOFMEMORY;

  lplmid->varstring.dwTotalSize = varstring.dwNeededSize;
  if ((dwRet = lineGetID(dpi.hLine, dpi.dwAddressID, dpi.hCall,
                         LINECALLSELECT_LINE,
                         (LPVARSTRING)lplmid,
                         c_szModemClass)) == ERROR_SUCCESS)
  {
    hComm = lplmid->hComm;

    if (hComm == NULL)
      dwRet = ERROR_INVALID_HANDLE;
  };
  LocalFree((HLOCAL)lplmid);

  if (dwRet == ERROR_SUCCESS)
  {
    // Use the communication port
    //
    dwRet = TransferData(hComm) ? ERROR_SUCCESS : ERROR_USER_DISCONNECTION;

    // Close the communication port
    //
    CloseHandle(hComm);
  };

  return dwRet;
}
