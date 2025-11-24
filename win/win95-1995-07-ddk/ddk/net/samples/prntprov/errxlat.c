/*****************************************************************/ 
/**               Microsoft Windows 95                          **/
/**           Copyright (C) Microsoft Corp., 1991-1995          **/
/*****************************************************************/ 
#include "mspp.h"
#include <neterr.h>
#include <winerror.h>

//
// Stray definitions from NETCONS.H.  Including the whole thing causes
// the compiler to spew a lengthy list of warnings about macro redefinitions
//
#define USHORT unsigned short

#include "errxlat.h"
#include "ppdebug.h"

//
// LANMan/W4W error code ==> Win32 error code translation table. Errors
// not in this table are translated as "ERROR_CAN_NOT_COMPLETE".
//
static ERRORXLAT error_table[] = {

  {NERR_BufTooSmall     ,ERROR_INSUFFICIENT_BUFFER},
  {ERROR_BAD_NETPATH,    ERROR_INVALID_NAME},
  {ERROR_BAD_NET_NAME,   ERROR_INVALID_NAME},
  {ERROR_BAD_NET_RESP,   ERROR_INVALID_LEVEL},
  {ERROR_NETWORK_BUSY,   ERROR_INVALID_NAME},
  {ERROR_MORE_DATA,      ERROR_INSUFFICIENT_BUFFER},
  {NERR_NetNotStarted   ,ERROR_NO_NETWORK},
  {NERR_InvalidComputer ,ERROR_INVALID_NAME},
  {NERR_SpoolerNotLoaded,ERROR_DEV_NOT_EXIST},
  {NERR_WkstaNotStarted ,ERROR_NO_NETWORK},
  {NERR_QNotFound       ,ERROR_INVALID_NAME},
  {NERR_SpoolNoMemory   ,ERROR_NOT_ENOUGH_SERVER_MEMORY},
  {NERR_DataTypeInvalid ,ERROR_INVALID_DATATYPE},
  {NERR_BadDev          ,ERROR_BAD_DEVICE},
  {NERR_QInvalidState   ,ERROR_INVALID_PRINTER_COMMAND},
  {NERR_JobNotFound     ,ERROR_FILE_NOT_FOUND},
  {NERR_DriverNotFound  ,ERROR_UNKNOWN_PRINTER_DRIVER},
  {NERR_ProcNotFound    ,ERROR_UNKNOWN_PRINTPROCESSOR}

};

/*******************************************************************
  MapError

  Converts a W4W/LANMan Nerr_xxxx return code to a Win32 DWORD 
  error code. 

*******************************************************************/
DWORD MapError(UINT NetErr) {
  int i,n;

  if (NetErr != 0) {

    DBGMSG(DBG_LEV_VERBOSE,("  MSPP.MapError(LanMan Code %d)\n",NetErr));

  }

  n = sizeof(error_table) / sizeof(ERRORXLAT);

  for (i = 0; i < n; i++) {

     if (NetErr == error_table[i].in) {

       return error_table[i].out;

     }
  }

//
// Handle errors that aren't specifically mapped and are the same 
// in LANMan and Win32. This allows us to remap certain codes into
// the ones expected by the print router in SPOOLSS.DLL
//
  if (NetErr < NERR_BASE) 
    return (DWORD) NetErr;

//
// Unmapped error. Since we don't know exactly what happened, we'll 
// return a non-specific code.
//
  return ERROR_CAN_NOT_COMPLETE;
    
}
