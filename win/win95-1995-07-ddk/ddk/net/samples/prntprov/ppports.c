/*****************************************************************/
/**               Microsoft Windows for Workgroups              **/
/**           Copyright (C) Microsoft Corp., 1991-1995          **/
/*****************************************************************/ 

/* PPPORTS.C
 *  
 * MS Net print provider. Enumerates network printer "ports" -- printer
 * shares on servers to which the user is connected.
 *
 * PPEnumPorts doesn't really do anything -- it's a placeholder for
 * diagnostics, and can eventually be placed in STUBS.C
 *
 */          

#include "mspp.h"


/*******************************************************************
  PPEnumPorts

  Enumerates the ports available for printing on a specified server.
  Returns TRUE if successful, FALSE otherwise.  Extended error info
  is available through GetLastError().

*******************************************************************/
BOOL WINAPI PPEnumPorts(LPSTR  lpName,
                        DWORD  Level,
                        LPBYTE lpPorts,
                        DWORD  cbBuf,
                        LPDWORD pcbNeeded,
                        LPDWORD pcbReturned) {
//
// This function is not supported and is included just to keep
// SPOOLSS.DLL from GP Faulting.
//
  DBGMSG(DBG_LEV_VERBOSE,("MSPP.PPEnumPorts\n"));

  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}
