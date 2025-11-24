/*****************************************************************/ 
/**               Microsoft Windows 95                          **/
/**           Copyright (C) Microsoft Corp., 1991-1995          **/
/*****************************************************************/ 

/* PPPRN.C
 *  
 * MS Net print provider. Functions for dealing with opening/closing and
 * reading/writing printers.
 *
 */          

#include "mspp.h"
#include <netlib.h>

extern "C" {
#include "dosdef.h"
#include "dospmspl.h"
};

#include "ppprn.h"
#include "errxlat.h"

// Exported Kernel function to create and attach a Win32 handle to
// an existing DOS file handle.  This lets us use K32's Read and
// Write functions.
//
extern "C" {
  extern WORD Win32HandleToDosFileHandle(HANDLE);
}

// Net name caching stuff 
//
#define FAILURE_CACHE_TIMEOUT 5000   
#define SUCCESS_CACHE_TIMEOUT 10000
static char  __last_failure[MAX_PATH];
static DWORD __failure_clock;
static char  __last_success[MAX_PATH];
static DWORD __success_clock;

void UpdateLastFailure(LPSTR szName) {
  DBGMSG(DBG_LEV_VERBOSE,("  MSPP::failure cache: %s\n",szName));
  lstrcpy(__last_failure,szName);
  __failure_clock = GetTickCount();
}

BOOL NameAlreadyFailed(LPSTR szName) {
  if ((GetTickCount() - __failure_clock) > FAILURE_CACHE_TIMEOUT) {
    UpdateLastFailure(EMPTY_STRING);
    return FALSE;
  }
  return (lstrcmp(__last_failure,szName) == 0);
}

void UpdateLastSuccess(LPSTR szName) {
  DBGMSG(DBG_LEV_VERBOSE,("  MSPP::success cache: %s\n",szName));
  lstrcpy(__last_success,szName);
  __success_clock = GetTickCount();
}

BOOL NameAlreadySucceeded(LPSTR szName) {
  if ((GetTickCount() - __success_clock) > SUCCESS_CACHE_TIMEOUT) {
    UpdateLastSuccess(EMPTY_STRING);
    return FALSE;
  }

  return (lstrcmp(__last_success,szName) == 0);
}

/*******************************************************************
  _make_connection

  Attempts to establish a "deviceless" connection fr authentication.
  Uses WNetAddConnection3 so that the user will be prompted for
  name and password if necessary.

  Returns TRUE if successful, FALSE otherwise.

*******************************************************************/
BOOL _make_connection(LPSTR lpServer,LPSTR lpQueue) {
  char szPath[PATHLEN];
  NETRESOURCE NetRes;
  DWORD dwResult;

//
// Build a complete UNC path to our network printer
//
  lstrcpy(szPath,lpServer);
  lstrcat(szPath,BACKSLASH);
  lstrcat(szPath,lpQueue);

//
// Construct a NETRESOURCE structure for our deviceless connection
//
  NetRes.lpRemoteName = szPath;
  NetRes.lpLocalName  = NULL;
  NetRes.lpProvider   = NULL;
  NetRes.dwType       = RESOURCETYPE_ANY;

//
// Until we either succeed, get cancelled or fail beyond
// redemption.
//
  while (TRUE) {

    dwResult = WNetAddConnection3(NULL,
                                  &NetRes,
                                  NULL,
                                  NULL,
                                  CONNECT_INTERACTIVE | CONNECT_CURRENT_MEDIA);

    if (dwResult != WN_ACCESS_DENIED) break;

  }

  return (dwResult == WN_SUCCESS);
}

/*******************************************************************
  _break_connection

  Breaks a deviceless connection created with _make_connection
  Returns TRUE if successful, FALSE otherwise.

*******************************************************************/
BOOL _break_connection(LPSTR lpServer, LPSTR lpQueue) {
  char szPath[PATHLEN];
  DWORD dwResult;

//
// Build a complete UNC path to our network printer
//
  lstrcpy(szPath,lpServer);
  lstrcat(szPath,BACKSLASH);
  lstrcat(szPath,lpQueue);

//
// Break the connection
//
  dwResult = WNetCancelConnection2(szPath,0,TRUE);

  return (dwResult == WN_SUCCESS);
}

/*******************************************************************
 _try_open

 Attempts to open the specified file, which is presumably a UNC path
 to a network queue.  Returns TRUE and places the returned file
 handle in lpHandle if successful.  Returns FALSE otherwise.

*******************************************************************/
BOOL _try_open(LPSTR lpServer, LPSTR lpQueue,LPHANDLE lpHandle) {
  char szPath[PATHLEN];
  HANDLE h;

//
// Build a complete UNC path to our network printer
//
  lstrcpy(szPath,lpServer);
  lstrcat(szPath,BACKSLASH);
  lstrcat(szPath,lpQueue);

//
// Attempt to open the file.
//
  h = CreateFile(szPath,
                 GENERIC_WRITE,
                 FILE_SHARE_WRITE,
                 NULL,
                 OPEN_EXISTING,
                 FILE_ATTRIBUTE_NORMAL,
                 NULL);

  if (h == INVALID_HANDLE_VALUE) return FALSE;

  *lpHandle = h;

  return TRUE;

}

/*******************************************************************
  ValidatePrinterHandle

  Returns TRUE if the specified handle is a valid, open network
  printer handle, FALSE otherwise.
*******************************************************************/
BOOL ValidatePrinterHandle(HANDLE pPrinter) {
  PNETPRINTERQUEUE pInfo;
  BOOL result;

  pInfo = PtrFromHandle(pPrinter);

  result = (pInfo->signature == NPQ_SIGNATURE);

  if (!result) {

    SetLastError(ERROR_INVALID_HANDLE);

    DBGMSG(DBG_LEV_ERROR,
          ("MSPP.ValidatePrinterHandle - Invalid handle: %d\n",pPrinter));
    return FALSE;

  }

  return TRUE;
}

BOOL IsServerHandle(HANDLE hPrinter) {
  PNETPRINTERQUEUE pInfo;
  BOOL result;

  pInfo = PtrFromHandle(hPrinter);
  result = (*pInfo->szQueueName == 0) ? TRUE : FALSE;

  if (result) 
    SetLastError(ERROR_INVALID_HANDLE);

  return result;
}

/*******************************************************************
  PrintFileIsOpen

  Internal. Returns TRUE if the file associated with the specified 
  printer handle is open, FALSE otherwise. 

  Assumes that the printer handle has already been validated.
  
  If the file is open, calls like PPReadPrinter and PPWritePrinter 
  will succeed. Otherwise, they will fail.
*******************************************************************/
BOOL PrintFileIsOpen(HANDLE hPrinter) {
  PNETPRINTERQUEUE pInfo;


  pInfo = PtrFromHandle(hPrinter);

  return (pInfo->hFile != INVALID_HANDLE_VALUE);

}

/*******************************************************************
  ClosePrintFile

  Internal.  Attempts to close an open spool file associated with
  the specified printer handle.  Returns TRUE if successful, FALSE
  otherwise.

*******************************************************************/
BOOL ClosePrintFile(HANDLE hPrinter) {
  PNETPRINTERQUEUE pInfo;
  BOOL bResult;

//
// Make sure we have a valid printer handle
//
  if (!ValidatePrinterHandle(hPrinter)) return FALSE;

//
// If the print file isn't open, then we have nothing to do
//
  if (!PrintFileIsOpen(hPrinter)) return TRUE;

  pInfo = PtrFromHandle(hPrinter);

  bResult = CloseHandle(pInfo->hFile);

  pInfo->hFile = INVALID_HANDLE_VALUE;

  return bResult;
}

/*******************************************************************
  OpenPrintFile

  Internal. Attempts to open a spool file on the network printer
  specified by hPrinter.  Returns TRUE if successful, FALSE otherwise.

*******************************************************************/
BOOL OpenPrintFile(HANDLE hPrinter) {
  PNETPRINTERQUEUE pInfo;

//
// Make sure we have a valid printer handle
//
  if (!ValidatePrinterHandle(hPrinter)) return FALSE;

//
// If the file is already currently open and we're calling StartDoc
// again, something bad happened -- like, the server crashed in the
// middle of printing a document.  To clean up, we need to close
// the previous print file.
//
  if (PrintFileIsOpen(hPrinter)) {
    ClosePrintFile(hPrinter);
  }

  pInfo = PtrFromHandle(hPrinter);

//
// Attempt to open a spool file. If we can't open the file because of an
// access failure, try to establish a validated connection to the device,
// then retry the open.
//
  pInfo->hFile = INVALID_HANDLE_VALUE;

  while (TRUE) {
    
    if (_try_open(pInfo->szServerName,
                  pInfo->szQueueName,
                  &pInfo->hFile)) break;

    if (pInfo->bConnected) {
      _break_connection(pInfo->szServerName,pInfo->szQueueName);
      pInfo->bConnected = FALSE;
    }

    switch (GetLastError()) {

      case ERROR_INVALID_PASSWORD:
      case ERROR_BAD_USERNAME:
      case ERROR_ACCESS_DENIED:

        if (!_make_connection(pInfo->szServerName,pInfo->szQueueName))
          return FALSE;

        pInfo->bConnected = TRUE;

        break;

      default:
        DBGMSG(DBG_LEV_ERROR,("  OpenPrintFile failed. Code %ld\n",GetLastError()));
        return FALSE;
    }
  }

  return TRUE;

}

/*******************************************************************
  CreatePrinterHandle

  Given information about a net print queue returned by 
  DosPrintQGetInfo, create a data structure to keep some
  needed information around, and return a pointer to it
  to the caller as a "printer handle."

  For now, all we're keeping around is the server and
  device name of the printer

  Returns a non-zero handle value if successful, NULL if
  unable to create a handle.

*******************************************************************/
HANDLE CreatePrinterHandle(LPSTR lpServerName,
                           LPSTR lpDeviceName,
                           BOOL  bConnected,
                           LPPRINTER_DEFAULTS pDefaults) {

  PNETPRINTERQUEUE pPrinterInfo;

//
// Allocate memory for a new printer information structure
//
  pPrinterInfo = (PNETPRINTERQUEUE) PPAllocMem(sizeof(NETPRINTERQUEUE));

  if (pPrinterInfo == NULL) {

   SetLastError(ERROR_NOT_ENOUGH_MEMORY);
   return NULL;

  }

  pPrinterInfo->signature = NPQ_SIGNATURE;
  lstrcpy(pPrinterInfo->szServerName,lpServerName);
  lstrcpy(pPrinterInfo->szQueueName,lpDeviceName);

  pPrinterInfo->hFile = INVALID_HANDLE_VALUE;
  pPrinterInfo->bConnected     = bConnected;
  pPrinterInfo->bSupportsWin95 = TRUE;

//
// If the user requested an access level, save it.  Otherwise default
// to PRINTER_ACCESS_USE.  We don't do anything with this value for now
// but we may need it later.
//
// CODEWORK - need to think about security a little more. Do we need any
// local security or does the server take care of everything for us?
//
//  if (pDefaults != NULL) {
//
//    pPrinterInfo->AccessGranted = pDefaults->AccessGranted;
//
//  } 
//  else {
//
//    pPrinterInfo->AccessGranted = PRINTER_ACCESS_USE;
//
//  }

  return (HANDLE) pPrinterInfo;

}

/*******************************************************************
  DestroyPrinterHandle

  Frees all resources used by a printer handle object created by
  CreatePrinterHandle.  Returns TRUE if successful, FALSE otherwise.

*******************************************************************/
BOOL DestroyPrinterHandle(HANDLE hPrinter) {

  return PPFreeMem(PtrFromHandle(hPrinter),sizeof(NETPRINTERQUEUE));

}
 
//////////////////////////////////////////////////////////////////////////////
// PPAbortPrinter
//
// Deletes a printer's spool file if the printer is configured for spooling.
// Returns TRUE if successful, FALSE if an error occurs.
//
////////////////////////////////////////////////////////////////////////////// 
BOOL WINAPI PPAbortPrinter(HANDLE hPrinter) {
  PNETPRINTERQUEUE pInfo;
  PRIDINFO IdInfo;
  DWORD SplErr;
  DWORD Result;

   DBGMSG(DBG_LEV_VERBOSE,("MSPP.PPAbortPrinter(%d)\n",hPrinter));

//
// Make sure we have a valid printer handle
//
  if (!ValidatePrinterHandle(hPrinter)) return FALSE;

//
// Make sure that the spool file associated with this handle is
// actually open.
//
  if (!PrintFileIsOpen(hPrinter)) return FALSE;

//
// To kill the current document, we must
// (1) pause the current job
// (2) close its file handle
// (2) delete the job.
//
  pInfo = PtrFromHandle(hPrinter);
  
  SplErr = MapError(DosPrintJobGetId((HFILE) pInfo->hFile,
                                     &IdInfo,
                                     sizeof(PRIDINFO)));

  if (SplErr == ERROR_SUCCESS) {
    PPSetJob(hPrinter,(DWORD) IdInfo.uJobId,1,NULL,JOB_CONTROL_PAUSE);
  }

  Result = ClosePrintFile(hPrinter);

  if (SplErr == ERROR_SUCCESS) {
    PPSetJob(hPrinter,(DWORD) IdInfo.uJobId,1,NULL,JOB_CONTROL_CANCEL);
  }

  return Result;
} 

//////////////////////////////////////////////////////////////////////////////
// PPReadPrinter
//
// Reads data back from a bi-directional print device.Returns TRUE if 
// successful, FALSE if the device is not bi-directional or if another
// error occurs.
//
// NOTE: For now, PPReadPrinter does nothing but validate its parameters
// and fail with the last error code set to ERROR_CALL_NOT_IMPLEMENTED.
//
////////////////////////////////////////////////////////////////////////////// 
BOOL WINAPI PPReadPrinter(HANDLE hPrinter,
                          LPVOID pBuf,
                          DWORD cbBuf,
                          LPDWORD pNoBytesRead) {

  BOOL bResult;

  DBGMSG(DBG_LEV_VERBOSE,("MSPP.PPReadPrinter(%d)\n",hPrinter));

  bResult = FALSE;

  if (ValidatePrinterHandle(hPrinter) && PrintFileIsOpen(hPrinter)) {

    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);

  }

  return bResult; 
                   
} 

////////////////////////////////////////////////////////////////////////////
// PPClosePrinter
//
// Closes a prnter that was previously opened with PPOpenPrinter. Returns TRUE
// if successful, FALSE if an error occurred.
//
//////////////////////////////////////////////////////////////////////////// 
BOOL WINAPI PPClosePrinter(HANDLE hPrinter) {
  PNETPRINTERQUEUE pPrt;
  BOOL bResult;

  DBGMSG(DBG_LEV_VERBOSE,("MSPP.PPClosePrinter(%d)\n",hPrinter));

  bResult = FALSE;

//
// To close a printer handle we must:
//
// (1) make sure it's a valid, open printer handle
// (2) close its associated job file handle
// (3) cancel any associated network connections
// (4) free the memory used to store the printer information structure.
//
  if (ValidatePrinterHandle(hPrinter) && ClosePrintFile(hPrinter)) {

    pPrt = PtrFromHandle(hPrinter);

    if (pPrt->bConnected)
      _break_connection(pPrt->szServerName,pPrt->szQueueName);

    bResult = DestroyPrinterHandle(hPrinter);

  }

  return bResult;

}

/*******************************************************************
  _I_open_server

  Supports a somewhat strange behavior of OpenPrinter. Allows the
  user to retrieve a printer handle to a server name.
 
*******************************************************************/
BOOL _I_open_server(LPSTR szServer, LPHANDLE phPrinter) {
  DWORD dwResult;
  USHORT usAvail;
  USHORT usTotal;

  DBGMSG(DBG_LEV_VERBOSE,("  OpenPrinter called w/server name alone.\n"));

// Try to find out if the server exists on this network
//
  dwResult = MapError(DosPrintQEnum((LPBYTE)szServer,0,NULL,0,&usAvail,&usTotal));

  if (ERROR_INSUFFICIENT_BUFFER != dwResult) {
    SetLastError(ERROR_INVALID_NAME);
    return FALSE;
  }

  *phPrinter = CreatePrinterHandle(szServer,
                                   EMPTY_STRING,
                                   FALSE,
                                   NULL);

  return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// PPOpenPrinter
//
// Obtains a handle for the specified printer (queue).  Returns TRUE if 
// successful, FALSE if an error occurred.
//
////////////////////////////////////////////////////////////////////////////// 
BOOL WINAPI PPOpenPrinter(LPTSTR pPrinterName,
                          LPHANDLE phPrinter,
                          LPPRINTER_DEFAULTS pDefaults) {

  char szServerName[MAX_PATH];
  char szPrinterName[MAX_PATH];
  int nBufSize;
  BOOL bConnected;
  DWORD result;

  DBGMSG(DBG_LEV_VERBOSE,("MSPP.PPOpenPrinter(%s)\n",pPrinterName));

//
// Do whatever we must to convert the printer name from whatever format
// it's in, friendly, local, or relative, to a valid UNC path to a server.
// If we can't convert, then this name must not be ours.
//
  if (!ResolveDeviceName(pPrinterName,szServerName,szPrinterName)) {

    SetLastError(ERROR_INVALID_NAME);
    return FALSE;

  }

// If the user did not specify a queue name, attempt to get a handle
// to the server. 
//
  if (lstrlen(szPrinterName) == 0) 
    return _I_open_server(szServerName,phPrinter);

//
// Now that we have a valid UNC path to something, we need to see if
// that object actually exists on our network. 
//
  bConnected = FALSE;

  if (NameAlreadyFailed(pPrinterName)) {
    SetLastError(ERROR_INVALID_NAME);
    return FALSE;
  }

  while (TRUE) {

    if (NameAlreadySucceeded(pPrinterName))
      break;

    result =  MapError(DosPrintQGetInfo((LPBYTE) szServerName,
                                        (LPBYTE) szPrinterName,
                                        2,
                                        NULL,
                                        0,
                                        (PUSHORT) &nBufSize));

    if ((result == ERROR_SUCCESS) || 
        (result == ERROR_INSUFFICIENT_BUFFER)) {

      UpdateLastSuccess(pPrinterName);   
      break;

    }

//
// If we didn't succeed because of validation failure, prompt for username
// and password and create a temporary connection to the device (remembering
// to disconnect it when PPClosePrinter is called.)  If we establish a 
// connection, retry the QGetInfo query.
//
// On other errors, we just give up and return failure.
//
    if (bConnected) 
      _break_connection(szServerName,szPrinterName);

    switch (result) {

      case ERROR_INVALID_PASSWORD:
      case ERROR_BAD_USERNAME:
      case ERROR_ACCESS_DENIED:

        if (!_make_connection(szServerName,szPrinterName)) return FALSE;

        bConnected = TRUE;

        break;

      default:
        DBGMSG(DBG_LEV_ERROR,("  PPOpenPrinter: DosPrintQGetInfo failed.\n"));

        UpdateLastFailure(pPrinterName);

        SetLastError(result);
        return FALSE;
    }
  }

//
// We have a valid queue name. Now we'll attempt to get a handle
// for it. 
//
  *phPrinter = CreatePrinterHandle(szServerName,
                                   szPrinterName,
                                   bConnected,
                                   pDefaults);

//
// If we have a problem, CreatePrinterHandle calls SetLastError, so
// we can just return.
//
  if (*phPrinter == NULL) {

    DBGMSG(DBG_LEV_ERROR,("  PPOpenPrinter failed to create handle.s\n"));

    return FALSE;
  }

  DBGMSG(DBG_LEV_VERBOSE,
         ("  PPOpenPrinter succeeded. Handle: %d\n",*phPrinter));

  return TRUE;
}  

//////////////////////////////////////////////////////////////////////////////
// PPEndDocPrinter
//
// Ends a print job on the specified printer.  Retrns TRUE if successful,
// FALSE otherwise.
//
////////////////////////////////////////////////////////////////////////////// 
BOOL WINAPI PPEndDocPrinter(HANDLE hPrinter) {

  DBGMSG(DBG_LEV_VERBOSE,("MSPP.PPEndDocPrinter(%d)\n",hPrinter));

  if (ValidatePrinterHandle(hPrinter)) {

    return ClosePrintFile(hPrinter);

  }

  return FALSE;

}

//////////////////////////////////////////////////////////////////////////////
// PPStartDocPrinter
//
// Ends a print job on the specified printer.  Returns a print job ID if
// successful, an invalid job handle otherwise.
//
////////////////////////////////////////////////////////////////////////////// 
DWORD WINAPI PPStartDocPrinter(HANDLE hPrinter,
                               DWORD Level,
                               LPBYTE pBuffer) {
  PDOC_INFO_1 pDocInfo;
  DWORD dwJobId;
  PNETPRINTERQUEUE pPrinter;
  PRIDINFO IdInfo;
  JOB_INFO_1 JobInfo;
  DWORD SplErr;

  DBGMSG(DBG_LEV_VERBOSE,("MSPP.PPStartDocPrinter(%d)\n",hPrinter));

//
// Make sure we have a valid printer handle
//
  dwJobId = 0; // Invalid job ID

  if (!ValidatePrinterHandle(hPrinter)) return dwJobId;

  memsetf(&JobInfo,0,sizeof(JOB_INFO_1));

//
// Start a document on the specified printer.
//
// NOTE: The Chicago GDI uses StartDocPrinter level 2 for local printing.
// We don't need the extra info provided by the level 2 structure, so
// we'll just handle levels 1 and 2 the same way.
//
  switch (Level) {

    case 1:
    case 2:

      if (ClosePrintFile(hPrinter)) {

//
// Attempt to open a print job file
//
        if (OpenPrintFile(hPrinter)) {
//
// Get the job ID and set the job name field and datatype
//
          pDocInfo = (PDOC_INFO_1) pBuffer;
          pPrinter = PtrFromHandle(hPrinter);
  
          SplErr = MapError(DosPrintJobGetId(Win32HandleToDosFileHandle(pPrinter->hFile),
                                             &IdInfo,
                                             sizeof(PRIDINFO)));

          if (SplErr == NO_ERROR) {

            dwJobId = (DWORD) IdInfo.uJobId;
            JobInfo.pDocument  = pDocInfo->pDocName;
            JobInfo.pDatatype  = pDocInfo->pDatatype;
       
            PPSetJob(hPrinter,dwJobId,1,(LPBYTE) &JobInfo,0);

          }
          else {

            DBGMSG(DBG_LEV_ERROR,
                   ("  Error: Unable to get Job ID. Code: %ld\n",SplErr));

            SetLastError(SplErr);
            dwJobId = 0xdeadbeef;       
          }
        }
      }

      break;

    default:

      DBGMSG(DBG_LEV_WARN,("MSPP.PPStartDocPrinter: Invalid Level.\n"));

      SetLastError(ERROR_INVALID_LEVEL);
      break;
  }

  return dwJobId;
                        
}                        
  
//////////////////////////////////////////////////////////////////////////////
// PPEndPagePrinter
//
// Informs the printer that the data sent with WritePrinter since the last
// BeginPage function constitutes a page. Returns TRUE if successful,
// FALSE otherwise.
//
////////////////////////////////////////////////////////////////////////////// 
BOOL WINAPI PPEndPagePrinter(HANDLE hPrinter) {
  BOOL bResult;

  DBGMSG(DBG_LEV_VERBOSE,("MSPP.PPEndPagePrinter(%d)\n",hPrinter));

  bResult = FALSE;

  if (ValidatePrinterHandle(hPrinter) && PrintFileIsOpen(hPrinter)) {

    bResult = TRUE;

  }

  return bResult;

}

//////////////////////////////////////////////////////////////////////////////
// PPStartPagePrinter
//
// Informs the spool subsystem that a page is about to be started on this
// printer.  Returns TRUE if successful, FALSE if an error occurred.
//
////////////////////////////////////////////////////////////////////////////// 
BOOL WINAPI PPStartPagePrinter(HANDLE hPrinter) {
  BOOL bResult;

  DBGMSG(DBG_LEV_VERBOSE,("MSPP.PPStartPagePrinter(%d)\n",hPrinter));

  bResult = FALSE;

  if (ValidatePrinterHandle(hPrinter) && PrintFileIsOpen(hPrinter)) {

    bResult = TRUE;

  }

  return bResult;

}

//////////////////////////////////////////////////////////////////////////////
// PPWritePrinter
//
// Sendsthe data pointed to by pBuf to the specified printer. Returns TRUE
// if successful, FALSE if an error occurred.
//
////////////////////////////////////////////////////////////////////////////// 
BOOL WINAPI PPWritePrinter(HANDLE hPrinter,
                           LPVOID pBuf,
                           DWORD cbBuf, 
                           LPDWORD pcbWritten) {

  PNETPRINTERQUEUE pInfo;
  BOOL bResult;

  DBGMSG(DBG_LEV_VERBOSE,("MSPP.PPWritePrinter(handle: %d,size: %ld)\n",
         hPrinter,cbBuf));

//
// Make sure that we have a good printer handle and that the associated
// spool file is open, then do the write.
//
  bResult = FALSE;

  if (ValidatePrinterHandle(hPrinter) && PrintFileIsOpen(hPrinter)) {

    pInfo = PtrFromHandle(hPrinter);

    bResult = WriteFile(pInfo->hFile,pBuf,cbBuf,pcbWritten,NULL);

  }

  DBGMSG(DBG_LEV_VERBOSE,("MSPP.PPWritePrinter result is %d\n",bResult));
                    
  return bResult;
                    
}                    
                  
