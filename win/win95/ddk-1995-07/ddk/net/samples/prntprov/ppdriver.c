/*****************************************************************/ 
/**               Microsoft Windows 95                          **/
/**           Copyright (C) Microsoft Corp., 1991-1995          **/
/*****************************************************************/ 

/* PPDRIVER.C
 *  
 * MS Net print provider. Functions for retrieving printer driver
 * info from SMB print servers. Used mostly by Point and Print
 *
 */          


#include "mspp.h"
#include "ppprn.h"
#include "errxlat.h"

#include <base.h>
#include <buffer.h>
#include <npdefs.h>

extern "C" {
#include "dosdef.h"
#include "dospmspl.h"
};

#include <shares.h>

// Lifted from PMSPL.H in \common\h
//
typedef struct _PRQINFO50 { 
    USHORT   uVersion;    
    PSZ      pszModelName;
    PSZ      pszDriverName;
    PSZ      pszDataFileName;
    PSZ      pszMonitorName;
    PSZ      pszDriverPath;
    USHORT   cDependentNames;
} PRQINFO50;
typedef PRQINFO50 FAR *PPRQINFO50;

typedef struct _PRQINFO52 { 
    USHORT   uVersion;    
    PSZ      pszModelName;
    PSZ      pszDriverName;
    PSZ      pszDataFileName;
    PSZ      pszMonitorName;
    PSZ      pszDriverPath;
    PSZ      pszDefaultDataType;
    PSZ      pszHelpFile;
    PSZ      pszConfigFile;
    USHORT   cDependentNames;
} PRQINFO52;
typedef PRQINFO52 FAR *PPRQINFO52;


/*******************************************************************
  _is_msnet_print_queue

  Internal. Returns TRUE if a share of the specified name and type
  exists on the current network.

*******************************************************************/
BOOL _is_msnet_share(LPSTR szServer,LPSTR szResource,USHORT uType) {
  DWORD  dwResult;
  USHORT BlkSize,Needed;
  struct share_info_1 far *pSI;
  BOOL bResult;


  dwResult = MapError(NetShareGetInfo((LPSTR) szServer,
                                      (LPSTR) szResource,
                                      1,
                                      NULL,
                                      0,
                                      &BlkSize));

  if (dwResult != ERROR_INSUFFICIENT_BUFFER) return FALSE;

  pSI = (struct share_info_1 far *) PPAllocMem((DWORD) BlkSize);

  dwResult = MapError(NetShareGetInfo((LPSTR) szServer,
                                      (LPSTR) szResource,
                                      1,
                                      (LPSTR) pSI,
                                      BlkSize,
                                      &Needed));

  bResult = (dwResult == ERROR_SUCCESS);

  if (bResult) {

    bResult = (pSI->shi1_type == uType);

  }

  PPFreeMem(pSI,(DWORD) BlkSize);

  return bResult;
}

/*******************************************************************
  _I_get_driver_info


*******************************************************************/
PPRQINFO52 _I_get_driver_info(LPSTR szServer,
                              LPSTR szQueue,
                              LPDWORD pcbSize) {
  LPSTR pInfo;
  USHORT nNeeded;
  DWORD result;

  *pcbSize = 1024;

  while (TRUE) {

    pInfo = (LPSTR) PPAllocMem(*pcbSize);
    if (pInfo == NULL) return NULL;
  
    result =  MapError(DosPrintQGetInfo((LPBYTE) szServer,
                                        (LPBYTE) szQueue,
                                        52,
                                        (LPBYTE) pInfo,
                                        (USHORT) *pcbSize,
                                        &nNeeded));

    if (result == NO_ERROR) {
      break;
    }
    else if (result == ERROR_INSUFFICIENT_BUFFER) {
      PPFreeMem(pInfo,*pcbSize);
      *pcbSize *= 2;
    }
    else {
      DBGMSG(DBG_LEV_VERBOSE,
             ("_I_get_driver_info failed.  Code: %d\n",result));
      SetLastError(result);
      PPFreeMem(pInfo,*pcbSize);
      return NULL;
    }
  }

  return (PPRQINFO52) pInfo;
}

/*******************************************************************
  _I_get_driver_level1

  Internal. Fills a DRIVER_INFO_1 structure with information about
  the printer driver taken from a PPRQINFO52 structure. 
  
  Returns TRUE if successful, FALSE otherwise. 
  
*******************************************************************/
BOOL _I_get_driver_level1(PPRQINFO52  pPrinter,
                          PDRIVER_INFO_1 pDriver,
                          DWORD cbBuf,
                          LPDWORD pcbNeeded) {

  LPSTR lpString;

//
// Make sure we're going to have enough memory to do this
//
  *pcbNeeded = sizeof(DRIVER_INFO_1) + 
               lstrsize(pPrinter->pszModelName);

  if (*pcbNeeded > cbBuf) {

    SetLastError(ERROR_INSUFFICIENT_BUFFER);
    return FALSE;

  }

  lpString = EndOfBuffer(pDriver,cbBuf);

  PPSetString(&pDriver->pName,pPrinter->pszModelName,&lpString);
  
  return TRUE;

}

/*******************************************************************
  _I_get_driver_level2

  Internal. Fills a DRIVER_INFO_1 structure with information about
  the printer driver taken from a PPRQINFO52 structure.
  
  Returns TRUE if successful, FALSE otherwise. 
  
*******************************************************************/
BOOL _I_get_driver_level2(PNETPRINTERQUEUE pQueue,
                          PPRQINFO52 pPrinter,
                          PDRIVER_INFO_2 pDriver,
                          DWORD cbBuf,
                          LPDWORD pcbNeeded) {

  LPSTR lpString;
  DWORD cbPathLen;
  BOOL bResult;

//
// Make sure we're going to have enough memory to do this
//
  *pcbNeeded = sizeof(DRIVER_INFO_2) + 
               lstrsize(pPrinter->pszModelName) +  // printer model
               lstrsize(pPrinter->pszDriverName) + // driver name
               lstrsize(ENVIRONMENT_STRING) +      // environment
               lstrsize(pPrinter->pszDataFileName)+// data file
               lstrsize(pPrinter->pszConfigFile);    // config file

  if (*pcbNeeded > cbBuf) {

    SetLastError(ERROR_INSUFFICIENT_BUFFER);
    return FALSE;

  }

//
// Fill driver info structure. 
//
  lpString = EndOfBuffer(pDriver,cbBuf);

  pDriver->cVersion = (DWORD) pPrinter->uVersion;

  PPSetString(&pDriver->pName,pPrinter->pszModelName,&lpString);
  PPSetString(&pDriver->pDriverPath,pPrinter->pszDriverName,&lpString);
  PPSetString(&pDriver->pEnvironment,ENVIRONMENT_STRING,&lpString);
  PPSetString(&pDriver->pDataFile,pPrinter->pszDataFileName,&lpString);
  PPSetString(&pDriver->pConfigFile,pPrinter->pszConfigFile,&lpString);
  
  return TRUE;
}

/*******************************************************************
  _I_get_driver_level3

  Internal. Fills a DRIVER_INFO_3 structure with information about
  the printer driver taken from a PPRQINFO52 structure.
  
  Returns TRUE if successful, FALSE otherwise. 
  
*******************************************************************/
BOOL _I_get_driver_level3(PNETPRINTERQUEUE pQueue,
                          PPRQINFO52 pPrinter,
                          PDRIVER_INFO_3 pDriver,
                          DWORD cbBuf,
                          LPDWORD pcbNeeded) {

  LPSTR lpString;
  LPSTR *pDepFile;
  DWORD cbPathLen;
  DWORD cbDepend;
  int i;
  BOOL bResult;

//
// Make sure we're going to have enough memory to do this
//
  *pcbNeeded = sizeof(DRIVER_INFO_3) + 
               lstrsize(pPrinter->pszModelName) +  // printer model
               lstrsize(ENVIRONMENT_STRING) +      // environment
               lstrsize(pPrinter->pszDriverName) + // driver name
               lstrsize(pPrinter->pszDataFileName)+// data file
               lstrsize(pPrinter->pszConfigFile) + // config file
               lstrsize(pPrinter->pszMonitorName) +
               lstrsize(pPrinter->pszDefaultDataType) + //def data type 
               lstrsize(pPrinter->pszHelpFile);


  DBGMSG(DBG_LEV_VERBOSE,("  driver info: %d bytes\n",*pcbNeeded));

// Determine the size of the dependent file block
//
  pDepFile = (LPSTR *) (pPrinter + 1);
  cbDepend = 1;

  for (i = 0; i < pPrinter->cDependentNames; i++,pDepFile++) {
    cbDepend += lstrsize(*pDepFile);
  }
    
  DBGMSG(DBG_LEV_VERBOSE,("  dependent info: %d bytes\n",cbDepend));

  *pcbNeeded += cbDepend + 1;

  DBGMSG(DBG_LEV_VERBOSE,("  total info: %d bytes\n",*pcbNeeded));

  if (*pcbNeeded > cbBuf) {

    SetLastError(ERROR_INSUFFICIENT_BUFFER);
    return FALSE;

  }

//
// Fill driver info structure. 
//
  lpString = EndOfBuffer(pDriver,cbBuf);

  pDriver->cVersion = (DWORD) pPrinter->uVersion;

  PPSetString(&pDriver->pName,pPrinter->pszModelName,&lpString);
  PPSetString(&pDriver->pDriverPath,pPrinter->pszDriverName,&lpString);
  PPSetString(&pDriver->pEnvironment,ENVIRONMENT_STRING,&lpString);
  PPSetString(&pDriver->pDataFile,pPrinter->pszDataFileName,&lpString);
  PPSetString(&pDriver->pConfigFile,pPrinter->pszConfigFile,&lpString);
  PPSetString(&pDriver->pMonitorName,pPrinter->pszMonitorName,&lpString);
  PPSetString(&pDriver->pDefaultDataType,pPrinter->pszDefaultDataType,&lpString);
  PPSetString(&pDriver->pHelpFile,pPrinter->pszHelpFile,&lpString);

  if (pPrinter->cDependentNames > 0) {
    lpString -= cbDepend;
    pDriver->pDependentFiles = lpString;

    pDepFile = (LPSTR *) (pPrinter + 1);

    for (i = 0; i < pPrinter->cDependentNames; i++,pDepFile++) {
      lstrcpy(lpString,*pDepFile);
      lpString += lstrsize(*pDepFile);
    }

    *lpString = 0;
  }
  else {
    pDriver->pDependentFiles = NULL;
  }

#ifdef DEBUG
  if (pDriver->pDefaultDataType != NULL) { 
    DBGMSG(DBG_LEV_VERBOSE,("  pDefaultDataType is: %s\n",pDriver->pDefaultDataType));
  }
#endif
  
  return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// PPGetPrinterDriver
// 
// Retrieves information about a specified printer driver. 
//
// Returns TRUE if successful, FALSE otherwise.
//
////////////////////////////////////////////////////////////////////////////// 
BOOL WINAPI PPGetPrinterDriver(HANDLE hPrinter,
                               LPTSTR pEnvironment,
                               DWORD Level,
                               LPBYTE pDriverInfo,
                               DWORD cbBuf,
                               LPDWORD pcbNeeded) {

  BOOL bResult;
  PPRQINFO52 pPrinterInfo;
  PNETPRINTERQUEUE pPrinter;
  DWORD cbPSize;
  DWORD cbPNeeded;

  DBGMSG(DBG_LEV_VERBOSE,("MSPP.PPGetPrinterDriver(%d)\n",hPrinter));

//
// Validate parameters. First, make sure we have a valid printer handle
//
  if (!ValidatePrinterHandle(hPrinter)) return FALSE;

// Make sure the request is for our environment. The routing layer 
// very politely replaces NULL environment pointers from the caller with
// a pointer to our environment string, so we know we're always going
// to get something.  The question is, is it the *exact* environment
// we're handling?
//
  if (lstrcmp(pEnvironment,ENVIRONMENT_STRING) != 0) {

    SetLastError(ERROR_INVALID_NAME);
    return FALSE;

  }

  if (((pDriverInfo == NULL) && (cbBuf != 0)) ||
      (pcbNeeded == NULL)) {

    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;

  }

//
// Find out how much memory we're going to need to get level 2
// information on this printer.  Allocate sufficient memory
// and retrieve the information.
//
  pPrinter = PtrFromHandle(hPrinter);

  if (pPrinter->bSupportsWin95) {
    pPrinterInfo = _I_get_driver_info(pPrinter->szServerName,
                                      pPrinter->szQueueName,
                                      &cbPSize);
    if (pPrinterInfo == NULL)
      pPrinter->bSupportsWin95 = FALSE;
  }
  else {
    pPrinterInfo = NULL;
  }

  if (pPrinterInfo == NULL) {
    SetLastError(ERROR_INVALID_NAME);
    return FALSE;
  }

//
// Format driver information to the requested level
//
  switch (Level) {

    case 1:

      bResult = _I_get_driver_level1(pPrinterInfo,
                                     (PDRIVER_INFO_1) pDriverInfo,
                                     cbBuf,
                                     pcbNeeded);
      break;

    case 2:

      bResult = _I_get_driver_level2(PtrFromHandle(hPrinter),
                                     pPrinterInfo,
                                     (PDRIVER_INFO_2)  pDriverInfo,
                                     cbBuf,
                                     pcbNeeded);
      break;                         

    case 3:
      bResult = _I_get_driver_level3(PtrFromHandle(hPrinter),
                                     pPrinterInfo,
                                     (PDRIVER_INFO_3)  pDriverInfo,
                                     cbBuf,
                                     pcbNeeded);
      break;
    default:

      SetLastError(ERROR_INVALID_LEVEL);
      bResult = FALSE;
      break;
  }

  PPFreeMem((LPVOID) pPrinterInfo,cbPSize);

  return bResult;
}                        

//////////////////////////////////////////////////////////////////////////////
// PPGetPrinterDriverDirectory
// 
// Retrieves the fully qualified pathname of the directory where the printer
// driver resides. 
//
// Returns TRUE if successful, FALSE otherwise.
//
////////////////////////////////////////////////////////////////////////////// 
BOOL  WINAPI PPGetPrinterDriverDirectory(LPTSTR pName,
                                         LPTSTR pEnvironment,
                                         DWORD Level,
                                         LPBYTE pDriverDirectory,
                                         DWORD cbBuf,
                                         LPDWORD pcbNeeded) {

  BUFFER szServer(128);
  BUFFER szQueue(128);
  PPRQINFO52 pPrinter;
  DWORD cbPrinter;

  DBGMSG(DBG_LEV_VERBOSE,("MSPP.PPGetPrinterDriverDirectory(%s)\n",
                           pName));

//
// Validate our parameters as best we can
//

// Make sure the request is for our environment. The routing layer 
// very politely replaces NULL environment pointers from the caller with
// a pointer to our environment string, so we know we're always going
// to get something.  The question is, is it the *exact* environment
// we're handling?
//
  if (lstrcmp(pEnvironment,ENVIRONMENT_STRING) != 0) {

    SetLastError(ERROR_INVALID_NAME);
    return FALSE;

  }

  if ((pName == NULL) || 

// Disabled level check
//
//      (Level != 1) || 
      ((pDriverDirectory == NULL) && (cbBuf != 0)) ||
      (pcbNeeded == NULL)) {

    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;

  }

//
// Make sure that the incoming server name at least looks like
// one that could be ours. Names without backslashes and names
// that are too long will bomb out here.
//
  if (!ResolveDeviceName(pName,
                         (LPSTR) szServer.QueryPtr(),
                         (LPSTR) szQueue.QueryPtr())) {
    SetLastError(ERROR_INVALID_NAME);
    return FALSE;
  }

// Attempt to retrieve point and print information from the machine
//
  pPrinter = _I_get_driver_info((LPSTR) szServer.QueryPtr(),
                                (LPSTR) szQueue.QueryPtr(),
                                &cbPrinter);
  if (pPrinter == NULL) {
    SetLastError(ERROR_INVALID_NAME);
    return FALSE;
  }

//
// Figure out how much memory we're going to need for this
//
  *pcbNeeded = lstrsize(pPrinter->pszDriverPath);

  if (cbBuf < *pcbNeeded) {

    SetLastError(ERROR_INSUFFICIENT_BUFFER);
    return FALSE;

  }

  if(pPrinter->pszDriverPath != NULL)
    lstrcpy((LPSTR) pDriverDirectory,(LPSTR) pPrinter->pszDriverPath);
  else
    lstrcpy((LPSTR) pDriverDirectory,EMPTY_STRING);

  return TRUE;
                             
}                                                         

//////////////////////////////////////////////////////////////////////////////
// PPGetPrintProcessorDirectory
// 
// Retrieves the fully qualified pathname of the directory where the printer
// driver resides.
//
// NOTE - assumes that print processors reside in the same directory as
// printer drivers.  Is this always going to be true?
//
// Returns TRUE if successful, FALSE otherwise.
//
////////////////////////////////////////////////////////////////////////////// 
BOOL WINAPI PPGetPrintProcessorDirectory(LPTSTR pName,
                                         LPTSTR pEnvironment,
                                         DWORD Level,
                                         LPBYTE pPrintProcessorInfo,
                                         DWORD cbBuf,
                                         LPDWORD pcbNeeded) {

  DBGMSG(DBG_LEV_VERBOSE,("MSPP.PPGetPrintProcessorDirectory(%s)\n",
                           pName));

// For now, under Chicago, printer drivers and print processors
// are stored in the same directory on the server.
//
  return PPGetPrinterDriverDirectory(pName,
                                     pEnvironment,
                                     Level,
                                     pPrintProcessorInfo,
                                     cbBuf,
                                     pcbNeeded);
}                                                         
                                 

                      
