/*****************************************************************/ 
/**               Microsoft Windows 95                          **/
/**           Copyright (C) Microsoft Corp., 1991-1995          **/
/*****************************************************************/ 

/* PPINFO.C .
 *  
 * MS Net print provider functions that get and set printer information.
 *
 */          


#include "mspp.h"

extern "C" {
#include "dosdef.h"
#include "dospmspl.h"
};

#include "ppprn.h"
#include "ppinfo.h"
#include "errxlat.h"

#define I_DFLT_BUFSIZE 2400
#define I_MAX_BUFSIZE  32768 

// MINOR HACK
//
// The SaneTime macro is needed validate printer Start and Until times,
// because Samba, a Unix-based SMB server product returns invalid times
// in response to DosPrintQGetInfo.  This, in turn, causes AddPrinter 
// to choke when installing a Samba printer, thus annoying users everywhere.  SaneTime 
//
#define SaneTime(t) ((t < 1440) ? (DWORD) t : 0)

#define offsetof(type, identifier) (DWORD)(&(((type)0)->identifier))

// Table of printer parameters for use with DosPrintQSetInfo
// 
USHORT PrintQParmTable[] = {
  PRQ_PRIORITY_PARMNUM,
  PRQ_STARTTIME_PARMNUM,
  PRQ_UNTILTIME_PARMNUM,
  PRQ_SEPARATOR_PARMNUM,
  PRQ_PARMS_PARMNUM,
  PRQ_COMMENT_PARMNUM
};

// Table of offsets for use with DosPrintQSetInfo
// 
DWORD PrintQOffsetTable[] = {
  offsetof(PPRQINFO,uPriority),
  offsetof(PPRQINFO,uStartTime),
  offsetof(PPRQINFO,uUntilTime),
  offsetof(PPRQINFO,pszSepFile),
  offsetof(PPRQINFO,pszParms),
  offsetof(PPRQINFO,pszComment)
};

// Table of element sizes for use with DosPrintQSetInfo
//
#define INDEX_PSZSEPFILE 3
#define INDEX_PSZPARMS   4
#define INDEX_PSZCOMMENT 5
USHORT PrintQSizeTable[] = {
  sizeof(USHORT),  // uPriority
  sizeof(USHORT),  // uStartTime
  sizeof(USHORT),  // uUntilTime
  sizeof(int),     // placeholder for pszSepFile
  sizeof(int),     // placeholder for pszParms
  sizeof(int)      // placeholder for pszComment
};

/*******************************************************************
  _I_calc_prq3_size

  Returns the size of the block of memory needed for a PRQINFO3
  structure constructed from the specified PRQINFO structure.

*******************************************************************/
DWORD _I_calc_prq3_size(PPRQINFO pOld) {
  DWORD cbSize;

  cbSize = sizeof(PRQINFO3) + 
           lstrsize(pOld->szName) +
           lstrsize((LPSTR) pOld->pszSepFile) +
           lstrsize((LPSTR) pOld->pszPrProc) +
           lstrsize((LPSTR) pOld->pszDestinations) +
           lstrsize((LPSTR) pOld->pszParms) +
           lstrsize((LPSTR) pOld->pszComment) +
           EMPTY_STRING_SIZE; // driver name

  return cbSize;
}

/*******************************************************************
  _I_build_prq3

  Internal.  Constructs a PRQINFO3 structure from a supplied
  PRQINFO structure.  We do this so that higher level functions
  will only have to work with PRQINFO3 structures, whether or
  not the server provides them directly.

  The lpString parameter is a pointer to the last available byte
  in the output buffer - the area that will be used for string
  storage.  

  Returns a pointer to the last available byte in the output
  buffer.

*******************************************************************/
LPSTR _I_build_prq3(PPRQINFO pOld,PPRQINFO3 pNew,LPSTR lpString) {

  PPSetString((LPSTR *) &pNew->pszName,pOld->szName,&lpString);

  pNew->uPriority  = pOld->uPriority;
  pNew->uStartTime = pOld->uStartTime;
  pNew->uUntilTime = pOld->uUntilTime;

  PPSetString((LPSTR *) &pNew->pszSepFile,(LPSTR) pOld->pszSepFile,&lpString);
  PPSetString((LPSTR *) &pNew->pszPrProc,(LPSTR) pOld->pszPrProc,&lpString);
  PPSetString((LPSTR *) &pNew->pszParms,(LPSTR) pOld->pszParms,&lpString);
  PPSetString((LPSTR *) &pNew->pszComment,(LPSTR) pOld->pszComment,&lpString);
  
  pNew->fsStatus = pOld->fsStatus;
  pNew->cJobs    = pOld->cJobs;

  PPSetString((LPSTR *) &pNew->pszPrinters,(LPSTR) pOld->pszDestinations,&lpString);
  PPSetString((LPSTR *) &pNew->pszDriverName,EMPTY_STRING,&lpString);

  pNew->pDriverData = NULL;

  return lpString;

}


/*******************************************************************
  _I_map_prq_to_prq3

  Places info from a PRQINFO structure into a PRQINFO3 structure.
  This makes it easier for us to support down level servers.  All
  of our internal stuff uses the PRQINFO3 structure.  If we can 
  get one directly from the server, fine.  If not, we get a PRQINFO
  structure instead and use this function to map it to a PRQINFO3.

  Returns a pointer to the newly allocated PRQINFO3 structure if
  successful, FALSE otherwise.

*******************************************************************/
PPRQINFO3 _I_map_prq_to_prq3(PPRQINFO pOld,DWORD cbSize,LPDWORD pcbNeeded) {
  PPRQINFO3 pNew;

//
// Figure out how much memory we're going to need for this and
// attempt to allocate it.
//
  pNew = (PPRQINFO3) PPAllocMem(cbSize);
  if (pNew == NULL) return NULL;

//
// Move data from the old structure to the new
//
  *pcbNeeded = _I_calc_prq3_size(pOld);
  _I_build_prq3(pOld,pNew,EndOfBuffer(pNew,*pcbNeeded));

  return pNew;

}

/*******************************************************************
  _I_map_enum_prq_to_prq3

  Internal. Maps a whole array of PRQINFO structures into PRQINFO3
  structures.  

  Returns a pointer to the newly allocated PRQINFO3 structures if
  successful, FALSE otherwise.

*******************************************************************/
PPRQINFO3 _I_map_enum_prq_to_prq3(PPRQINFO pOld,
                                  DWORD cbNumRecs,
                                  LPDWORD cbSize) {
  PPRQINFO3 pNew,pNewTemp;
  PPRQINFO  pOldTemp;
  DWORD i;
  LPSTR lpString;

//
// Calculate the amount of memory we'll need.
//
  pOldTemp = pOld;
  cbSize = 0;

  for (i = 0; i < cbNumRecs; i++,pOldTemp++) {

    *cbSize += _I_calc_prq3_size(pOldTemp);

  }

  pNew =(PPRQINFO3) PPAllocMem(*cbSize);
  if (pNew == NULL) return NULL;

//
// Construct the array of PRQINFO3 records
//
  pOldTemp = pOld;
  pNewTemp = pNew;
  lpString = EndOfBuffer(pNew,*cbSize);

  for (i = 0; i < cbNumRecs; i++,pOldTemp++,pNewTemp++) {

    lpString = _I_build_prq3(pOld,pNew,lpString);

  }

  return pNew;

}

/*******************************************************************
  _I_net_enum_queues

  Internal. Allocates a buffer and fills it with an array of 
  PRQINFO structures containing information on each printer 
  queue available on a server. 
  
  Returns a pointer to the newly allocated buffer if successful,
  NULL otherwise. Places the size of the allocated buffer in the 
  DWORD pointed to by cbSize and the number of queue entries read
  in the DWORD pointed to by EntryCount.

*******************************************************************/
PPRQINFO3 _I_net_enum_queues(LPSTR szServer, 
                           LPDWORD EntryCount,
                           LPDWORD cbSize) {
  LPSTR pNetInfo,pOld;
  char  szOemName[MAX_PATH];
  DWORD SplErr;
  DWORD EntriesAvail,RemapSize;
  USHORT Level;

  if (!ResolveDeviceName(szServer,szOemName,NULL)) return FALSE;

//
// To keep the number of times we have to try this to a minimum,
// we'll start with a reasonable minimum buffer size, make our net
// call and see if it works.  If it does, we're well off.  If not,
// we increase the buffer size and try again.
//
  *cbSize = I_DFLT_BUFSIZE;
  *EntryCount = 0;
  EntriesAvail = 0;
  Level = 3;

  pNetInfo = (LPSTR) PPAllocMem(*cbSize);
  if (pNetInfo == NULL) return NULL;


  for ( ; ; ) {                         // forever

//
// Ask the W4W server to tell us about its printer queues. First we'll
// try this at level 3. If that fails with an "Invalid level" error 
// code, we'll try again at level 1 and remap the result into a level
// 3 style buffer.
//
    SplErr = MapError(DosPrintQEnum((LPBYTE) szOemName,
                                    Level,
                                    (LPBYTE) pNetInfo,
                                    (UINT) *cbSize,
                                    (PUSHORT) EntryCount,
                                    (PUSHORT) &EntriesAvail));
//
// If DosPrintQEnum returns successfully, we're done.  Break out of
// our forever loop.
//
    if (SplErr == NO_ERROR) {

      break;

    }

//
// If we failed with "Invalid level", try again with a lower level and
// the same memory size.
//
    else if ((SplErr == ERROR_INVALID_LEVEL) && (Level == 3)) {

        Level = 1;

    }

//
// If the server says it needs more room, we'll try again with a bigger
// memory block.
//
    else if (SplErr == ERROR_INSUFFICIENT_BUFFER) {

      PPFreeMem(pNetInfo,*cbSize);

      *cbSize += I_DFLT_BUFSIZE;

      if (*cbSize >= I_MAX_BUFSIZE) {

        DBGMSG(DBG_LEV_ERROR,
              ("MSPP.net_enum_queues: Sanity check! Print Q data > 32K.\n"));

        SetLastError(ERROR_INVALID_DATA);
        return NULL;
      }

      pNetInfo = (LPSTR) PPAllocMem(*cbSize);
      if (pNetInfo == NULL) return NULL;

    }
    
//
// Otherwise, we have some sort of network error.
//
    else {

      PPFreeMem(pNetInfo,*cbSize);

      DBGMSG(DBG_LEV_ERROR,
             ("MSPP.net_enum_queues: DosPrintQEnum Failed. Code %d\n",SplErr));

      SetLastError(SplErr);
      return NULL;

    }
  }          // loop forever


//
// When we arrive at this point, we've got something from the network.
// Check the current info level to see if we need to remap it upward.
//

  if (Level != 3) {

    pOld = pNetInfo;

    pNetInfo =(LPSTR) _I_map_enum_prq_to_prq3((PPRQINFO) pOld,
                                              *EntryCount,
                                              &RemapSize);

    PPFreeMem(pOld,*cbSize);

    if (pNetInfo == NULL) {

      DBGMSG(DBG_LEV_ERROR,
             ("MSPP.net_enum_queues: remap to lev 3 failed\n"));
      return NULL;

    }

    *cbSize = RemapSize;

  }

  return (PPRQINFO3) pNetInfo;
}


/*******************************************************************
  _I_enum_printers_level1

  Internal.  Given an array of printer information structures as
  returned by DosPrintQEnum, fills a user supplied buffer with
  an array of PRINTER_INFO_1 structures.  

  Returns TRUE if successful, FALSE otherwise.  If successful,
  returns the number of bytes of the supplied buffer actually
  used in the DWORD pointed to by pcbNeeded and the number of
  queue records in the DWORD pointed to by pcbReturned.
  
  If the buffer isn't big enough to hold all the data, pcbNeeded 
  will be set to the amount of buffer space needed and pcbReturned
  will be set to the number of complete records actually copied.

  Extended error information is available through GetLastError().

*******************************************************************/
BOOL _I_enum_printers_level1(PPRQINFO3 pNetInfo,
                             LPBYTE pBuffer,
                             DWORD NumQueues,
                             DWORD cbBufSize,
                             LPDWORD pcbNeeded,
                             LPDWORD pcbReturned) {

  LPSTR lpString;
  PPRQINFO3 pNetTemp;
  PPRINTER_INFO_1 pPrinter;
  DWORD i;

//
// Calculate how much memory we'll need, and how many records we can
// fit into the buffer space the user gave us.
//
  *pcbNeeded   = 0;
  *pcbReturned = 0;
  pNetTemp = pNetInfo;


  for (i = 0; i < NumQueues; i++,pNetTemp++ ) {

    *pcbNeeded += _I_gp1_calc_size(pNetTemp);

    if (*pcbNeeded <= cbBufSize) {

      (*pcbReturned)++;

    }
  }

//
// Create an array of filled PRINTER_INFO_1 structures, placing string
// data at the end of the buffer.
//
  lpString = EndOfBuffer(pBuffer,cbBufSize);
  pNetTemp = pNetInfo;
  pPrinter = (PPRINTER_INFO_1) pBuffer;

  for (i = 0; i < *pcbReturned; i++,pNetTemp++,pPrinter++) {

    lpString = _I_gp1_build_record(pNetTemp,pPrinter,lpString);

  }

  if (*pcbReturned < NumQueues) {

    SetLastError(ERROR_INSUFFICIENT_BUFFER);
    return FALSE;
  }
  else {

    return TRUE;

  }
}

/*******************************************************************
  _I_enum_printers_level2

  Internal.  Given an array of printer information structures as
  returned by DosPrintQEnum, fills a user supplied buffer with
  an array of PRINTER_INFO_2 structures.  

  Returns TRUE if successful, FALSE otherwise.  If successful,
  returns the number of bytes of the supplied buffer actually
  used in the DWORD pointed to by pcbNeeded and the number of
  queue records in the DWORD pointed to by pcbReturned.
  
  If the buffer isn't big enough to hold all the data, pcbNeeded 
  will be set to the amount of buffer space needed and pcbReturned
  will be set to the number of complete records actually copied.

  Extended error information is available through GetLastError().

*******************************************************************/
BOOL _I_enum_printers_level2(LPSTR lpServerName,
                             PPRQINFO3 pNetInfo,
                             LPBYTE pBuffer,
                             DWORD NumQueues,
                             DWORD cbBufSize,
                             LPDWORD pcbNeeded,
                             LPDWORD pcbReturned) {

  LPSTR lpString;
  PPRQINFO3 pNetTemp;
  PPRINTER_INFO_2 pPrinter;
  DWORD i;

  *pcbNeeded   = 0;
  *pcbReturned = 0;
  pNetTemp = pNetInfo;

//
// Calculate how much memory we'll need, and how many records we can
// fit into the buffer space the user gave us.
//
  for (i = 0; i < NumQueues; i++,pNetTemp++ ) {

    *pcbNeeded += _I_gp2_calc_size(lpServerName,pNetTemp,NULL);

    if (*pcbNeeded <= cbBufSize) {

      (*pcbReturned)++;

    }
  }

//
// We have a large enough buffer.  Create an array of filled
// PRINTER_INFO_2 structures, placing string data at the
// end of the buffer.
//
  lpString = EndOfBuffer(pBuffer,cbBufSize);
  pNetTemp = pNetInfo;
  pPrinter = (PPRINTER_INFO_2) pBuffer;

  for (i = 0; i < *pcbReturned; i++,pNetTemp++,pPrinter++) {

    lpString = _I_gp2_build_record(lpServerName,
                                   pNetTemp,
                                   pPrinter,
                                   lpString,
                                   NULL);

  }

  if (*pcbReturned < NumQueues) {

    SetLastError(ERROR_INSUFFICIENT_BUFFER);
    return FALSE;
  }
  else {

    return TRUE;

  }
}

/*******************************************************************
  _describe_provider

  Fills a PRINTER_INFO_1 structure with information about the
  print provider.  Returns TRUE if successful, FALSE otherwise. 
  Extended error information available from GetLastError() on return.

*******************************************************************/
BOOL _describe_provider(LPPRINTER_INFO_1 pInfo,
                       DWORD cbSize,
                       LPDWORD pcbNeeded,
                       LPDWORD pcbReturned) {
  LPSTR lpString;

  *pcbNeeded = sizeof(PRINTER_INFO_1) +
               lstrsize(PROVIDER_NAME);
  *pcbReturned = 0;

  if (*pcbNeeded > cbSize) {
    SetLastError(ERROR_INSUFFICIENT_BUFFER);
    return FALSE;
  }

  pInfo->Flags = 0;
  pInfo->pDescription = NULL;
  pInfo->pComment = NULL;

  lpString = EndOfBuffer(pInfo,cbSize);
  PPSetString(&pInfo->pName,PROVIDER_NAME,&lpString);

  *pcbReturned = 1;
  return TRUE;

}
 
//////////////////////////////////////////////////////////////////////////////
// PPEnumPrinters
//
// Enumerates the available printers. Returns TRUE if successful, FALSE
// if an error occurred. NOTE: Supports level 2 PRINTER_INFO to provide Point
// and Print functionality.
//
//////////////////////////////////////////////////////////////////////////////
BOOL WINAPI  PPEnumPrinters(DWORD Type,
                           LPTSTR Name,
                           DWORD Level,
                           LPBYTE pPrinterEnum,
                           DWORD cbBuf,
                           LPDWORD pcbNeeded,
                           LPDWORD pcbReturned) {

PPRQINFO3 pNetInfo;
DWORD NumQueues;
DWORD cbNetInfoSize;

BOOL bReturn;

  DBGMSG(DBG_LEV_VERBOSE,("MSPP.PPEnumPrinters\n"));

//
// Throw back enumeration requests that we don't handle. The routing
// layer above us is specifically looking for the ERROR_INVALID_NAME.
//
  if (!(Type & (PRINTER_ENUM_REMOTE | PRINTER_ENUM_NAME))) {

    DBGMSG(DBG_LEV_VERBOSE,("  Enum request not for us.\n"));

    SetLastError(ERROR_INVALID_NAME);
    return FALSE;
  }

// 
// If the request is one that we handle, but the caller didn't supply
// a printer name, we'll fail. Again, to keep things going as the
// router expects, we must return an ERROR_INVALID_NAME condition.
//
  if (lstrsize(Name) == 0) {

    DBGMSG(DBG_LEV_WARN,("  Enum request w/NULL server name.\n"));

    if ((Level == 1) && (Type & PRINTER_ENUM_NAME)) {

      return _describe_provider((LPPRINTER_INFO_1) pPrinterEnum,
                                 cbBuf,
                                 pcbNeeded,
                                 pcbReturned);
    } 
    else if (Type & PRINTER_ENUM_REMOTE) {
//
// In this case, the user has asked us to enumerate the printers in
// our domain. Until we decide what we really want to do, we'll s
// simply return success with an empty enumeration.
// 
      *pcbNeeded   = 0;
      *pcbReturned = 0;
      
      return TRUE;
    }
    else {
      SetLastError(ERROR_INVALID_NAME);
      return FALSE;
    }
  }

  DBGMSG(DBG_LEV_VERBOSE,
        ("  Type: %ld\n  Name: %s\n  Level: %ld\n",Type,Name,Level));

// 
// One last bozoid special case to test for -- did they ask us to
// enumerate ourselves (the provider) by name?
//
  if ((Level == 1) && (Type & PRINTER_ENUM_NAME)) {

    if (lstrcmp(Name,PROVIDER_NAME) == 0) {

      return _describe_provider((LPPRINTER_INFO_1) pPrinterEnum,
                                 cbBuf,
                                 pcbNeeded,
                                 pcbReturned);
    }
  }

//
// Ask the server to tell us about its print queues.
//

  pNetInfo = _I_net_enum_queues(Name,&NumQueues,&cbNetInfoSize);

  if (pNetInfo == NULL) {
    return FALSE;
  }

  switch (Level) {

    case 1:
      bReturn = _I_enum_printers_level1(pNetInfo,
                                        pPrinterEnum,
                                        NumQueues,
                                        cbBuf,
                                        pcbNeeded,
                                        pcbReturned);
      break;

    case 2:
      bReturn = _I_enum_printers_level2(Name,
                                        pNetInfo,
                                        pPrinterEnum,
                                        NumQueues,
                                        cbBuf,
                                        pcbNeeded,
                                        pcbReturned);
      break;

    default:

      DBGMSG(DBG_LEV_WARN,("MSPP.PPEnumPrinters: Invalid Level.\n"));

      SetLastError(ERROR_INVALID_LEVEL);
      bReturn = FALSE;
      break;

  }

  PPFreeMem(pNetInfo,cbNetInfoSize);

  return bReturn;                  
                  
} 

/*******************************************************************
  _I_get_devmode_size

  Retrieves the number of bytes used by the DEVMODE structure for 
  this remote printer.  Returns TRUE if successful, FALSE otherwise.

*******************************************************************/
BOOL _I_get_devmode_size(LPSTR szServer,
                         LPSTR szQueue,
                         LPDWORD pcbSize) {

  USHORT nNeeded;
  DWORD result;

  result =  MapError(DosPrintQGetInfo((LPBYTE) szServer,
                                      (LPBYTE) szQueue,
                                      51,
                                      (LPBYTE) NULL,
                                      (USHORT) 0,
                                      &nNeeded));

  if (result != ERROR_INSUFFICIENT_BUFFER) {
    DBGMSG(DBG_LEV_VERBOSE,("_I_get_devmode_size failed. Code: %d\n",result));
    *pcbSize = 0;
    SetLastError(result);
    return FALSE;
  }

  *pcbSize = (DWORD) nNeeded;

  DBGMSG(DBG_LEV_VERBOSE,("_I_get_devmode_size OK. Size: %d bytes\n",*pcbSize));

  return TRUE;
}

/*******************************************************************
  _I_get_devmode


*******************************************************************/
BOOL  _I_get_devmode(LPSTR  szServer,
                     LPSTR  szQueue,
                     LPBYTE lpBuffer,
                     DWORD  cbBufSize) {

  DWORD result;
  USHORT nNeeded;
  
  result =  MapError(DosPrintQGetInfo((LPBYTE) szServer,
                                       (LPBYTE) szQueue,
                                       51,
                                       lpBuffer,
                                       (USHORT) cbBufSize,
                                       &nNeeded));

  if (result == NO_ERROR) {
    DBGMSG(DBG_LEV_VERBOSE,("_I_get_devmode OK.\n"));
    return TRUE;
  }

  DBGMSG(DBG_LEV_VERBOSE,("_I_get_devmode failed. Code: %d\n",result));
  SetLastError(result);
  return FALSE;
}

/*******************************************************************
  _I_net_get_info

  Internal. Retrieves a PRQINFO3 struct filled with DosPrintQGetInfo
  information.  Places the size of the structure in the
  DWORD pointed to by cbSize.  

  Returns a pointer to a newly allocated (with PPAllocMem) PRQINFO
  structure if successful, NULL otherwise. Extended error information
  available through GetLastError().

  NOTE: For internal use only -- assumes that hPrinter has already
  been checked with ValidatePrinterHandle.

*******************************************************************/
PPRQINFO3 _I_net_get_info(HANDLE hPrinter,LPDWORD cbSize,LPDEVMODE *ppDevmode) {
  PNETPRINTERQUEUE pInfo;
  LPBYTE pQueue,pOld;
  USHORT Level;
  DWORD SplErr;
  DWORD cbNeeded;
  DWORD cbDevmode;

  pInfo = PtrFromHandle(hPrinter);

  DBGMSG(DBG_LEV_VERBOSE,
         ("MSPP.net_get_info(%s,%s)\n",
          (LPSTR) pInfo->szServerName,
          (LPSTR) pInfo->szQueueName));

//
// Allocate a reasonably large block of memory so that we can do
// this as quickly as possible.  Note that DEVMODE structures for
// PostScript devices can easily exceed 2K.
//
  *cbSize = I_DFLT_BUFSIZE;
  cbNeeded = 0;

  _I_get_devmode_size(pInfo->szServerName,pInfo->szQueueName,&cbDevmode);

  Level = 3; 

  for (; ; ) {  // until we get all the data or run out of RAM

    pQueue = (LPBYTE) PPAllocMem(*cbSize);
    if (pQueue == NULL) return NULL;


    SplErr = MapError(DosPrintQGetInfo((LPBYTE) pInfo->szServerName,
                                       (LPBYTE) pInfo->szQueueName,
                                       Level,
                                       pQueue,
                                       (USHORT) (*cbSize - cbDevmode),
                                       (PUSHORT) &cbNeeded));
//
// If successful, continue
//
    if (SplErr == NO_ERROR) {

      DBGMSG(DBG_LEV_VERBOSE,
             ("  DosPrintQGetInfo OK. Bytes Needed: %ld\n",cbNeeded));
      break;

    }
//
// If the buffer was too small to hold all the data, resize it
// if possible and try again
//
    else if (SplErr == ERROR_INSUFFICIENT_BUFFER) {

      PPFreeMem(pQueue,*cbSize);
      *cbSize *= 2;

      if (*cbSize > I_MAX_BUFSIZE) {
        DBGMSG(DBG_LEV_ERROR,("  DosPrintQGetInfo data too large.\n"));
        return NULL;
      }
    }

//
// If the server couldn't handle a level 3 request, we'll make our net 
// request at level 2  -- the only level that some downlevel servers,
// like the WGC 2.0 server can handle.
//
    else if ((Level == 3) && (SplErr == ERROR_INVALID_LEVEL)) {

       DBGMSG(DBG_LEV_VERBOSE,("  Retrying DosPrintQGetInfo @ level 2\n"));
       Level = 2;

    }
//
// On any other network error, report back to the user and fail.
// 
    else {
      DBGMSG(DBG_LEV_ERROR,("  DosPrintQGetInfo failed. Code %d\n",SplErr));
    
      PPFreeMem(pQueue,*cbSize);
      SetLastError(SplErr);
      return NULL;
    }
  }

//
// Map the PRQINFO structure that we got back from DosPrintQGetInfo level2
// to a PRQINFO3 structure that the rest of the provider can digest more
// easily.
//
  if (Level == 2) {

    pOld = pQueue;
    pQueue = (LPBYTE) _I_map_prq_to_prq3((PPRQINFO) pOld,*cbSize,&cbNeeded);

    PPFreeMem(pOld,*cbSize);

    if (pQueue == NULL) {

      DBGMSG(DBG_LEV_ERROR,
             ("MSPP.net_get_info: remap to lev 3 failed\n"));

      return NULL;
    }
  }

// If the DEVMODE structure is larger than 0 bytes, indicating that we
// got one, we'll attempt to copy it into our memory block, after the
// last byte of the printer data.  In this case, we'll return
// a pointer to the beginning of the DEVMODE in the variable ppDevmode
//
// If we already know that we can't get a DEVMODE, or if our attempt to
// actually fetch the thing fails, we return NULL in this pointer.
//
  if (cbDevmode > 0) {
    *ppDevmode = (LPDEVMODE) (((LPBYTE) pQueue) + cbNeeded);
    if (!_I_get_devmode(pInfo->szServerName,
                        pInfo->szQueueName,
                        (LPBYTE) *ppDevmode,
                        cbDevmode))
      *ppDevmode = NULL;
  }
  else {
    *ppDevmode = NULL;
  }

  DBGMSG(DBG_LEV_VERBOSE,("MSPP.net_get_info for %s\\%s succeeded.\n",
                          (LPSTR) pInfo->szServerName,
                          (LPSTR) pInfo->szQueueName));

  return (PPRQINFO3) pQueue;
}

/*******************************************************************
  _I_gpl1_calc_size

  Internal.  Calculates the amount of buffer space needed to
  store a single level 1 GetPrinter() result buffer.

*******************************************************************/
DWORD _I_gp1_calc_size(PPRQINFO3 pInfo) {
  DWORD cbSize;

//
// Figure out how many bytes of buffer space we're going to need. 
//
  cbSize = sizeof(PRINTER_INFO_1) + 
           EMPTY_STRING_SIZE +            // description
           lstrsize((LPSTR) pInfo->pszName) +    // name
           lstrsize((LPSTR) pInfo->pszComment);  // comment

  return cbSize;
}

/*******************************************************************
  _I_gp1_build_record

  Internal. Given a pointer to a source PRQINFO structure, a
  destination PRINTER_INFO_1 structure and a pointer to the 
  start of string storage space, fills the PRINTER_INFO_1 structure.

  Called by GetPrinter and EnumPrinters, level 1.  Returns an
  LPSTR pointer to the last available data byte remaining in the
  buffer.

*******************************************************************/
LPSTR _I_gp1_build_record(PPRQINFO3 pInfo,
                         PPRINTER_INFO_1 pPrinter,
                         LPSTR lpString) {

//
// Flags
//
  pPrinter->Flags = PRINTER_ENUM_CONTAINER;

//
// Description
//
// BUGBUG - The Win32 structure has a field for "description" that has
// no equivalent in the network printer info structure.  What do we
// really want here? 
//

  PPSetString(&pPrinter->pDescription,EMPTY_STRING,&lpString);
  PPSetString(&pPrinter->pName,(LPSTR) pInfo->pszName,&lpString);
  PPSetString(&pPrinter->pComment,(LPSTR) pInfo->pszComment,&lpString);

  return lpString;

}

/*******************************************************************
  _I_get_printer_level1

  Internal. Given pInfo, a block of information on a printer returned by a
  DosPrintQGetInfo call, fill a PRINTER_INFO_1 structure.  Returns
  TRUE if successful, FALSE otherwise.  Places the number of bytes
  of pBuffer used in the DWORD pointed to by pcbNeeded.

*******************************************************************/
BOOL _I_get_printer_level1(PPRQINFO3 pInfo,
                           LPBYTE pBuffer,
                           DWORD cbBuf,
                           LPDWORD pcbNeeded) {
//
// Figure out how many bytes of buffer space we're going to need. 
// Set error code and return failure if we don't have enough.
//
  *pcbNeeded = _I_gp1_calc_size(pInfo);

  if (*pcbNeeded > cbBuf) {

    DBGMSG(DBG_LEV_VERBOSE,("  print info:insufficient buffer\n"));
    SetLastError(ERROR_INSUFFICIENT_BUFFER);
    return FALSE;

  }
      
//
// Copy the required data from the DosPrint structure to the Win 32
// structure.
//
  _I_gp1_build_record(pInfo,
                      (PPRINTER_INFO_1) pBuffer,
                      EndOfBuffer(pBuffer,cbBuf));

  return TRUE;

}

/*******************************************************************
  _I_gp2_calc_size

  Internal.  Calculates the amount of buffer space needed to
  store a single level 1 GetPrinter() result buffer.

*******************************************************************/
DWORD _I_gp2_calc_size(LPSTR lpServerName,
                       PPRQINFO3 pNetInfo,
                       LPDEVMODE pDevmode) {
  DWORD cbSize;

//
// Figure out how many bytes of buffer space we're going to need. 
//
// CODEWORK - several of these fields are filled with "" placeholders.
// We probably need to put real information  in some of them.
//
  cbSize = sizeof(PRINTER_INFO_2) + 
           lstrsize(lpServerName) +               // Server Name
           EMPTY_STRING_SIZE +                    // Printer Name
           lstrsize((LPSTR) pNetInfo->pszName) +          // Share Name
           EMPTY_STRING_SIZE +                    // Port Name
           lstrsize((LPSTR) pNetInfo->pszDriverName) +    // Driver Name
           lstrsize((LPSTR) pNetInfo->pszComment) +       // Comment
           EMPTY_STRING_SIZE +                    // Location
           sizeof(DEVMODE) +                      // Device mode
           lstrsize((LPSTR) pNetInfo->pszSepFile) +       // Separator file
           lstrsize((LPSTR) pNetInfo->pszPrProc) +        // Print processor
           EMPTY_STRING_SIZE +                    // Data type
           lstrsize((LPSTR) pNetInfo->pszParms);          // Parameters

   if (pDevmode != NULL) 
     cbSize += pDevmode->dmSize + pDevmode->dmDriverExtra;

  return cbSize;

}

/*******************************************************************
  _I_gp2_build_record

  Internal. Given a server name, a pointer to a source PRQINFO3
  structure, a destination PRINTER_INFO_1 structure and a pointer to 
  the start of string storage space, fills the PRINTER_INFO_2 structure.

  Called by GetPrinter and EnumPrinters, level 2.  Returns an
  LPSTR pointer to the last available data byte in the data buffer.

*******************************************************************/
LPSTR _I_gp2_build_record(LPSTR lpServerName,
                         PPRQINFO3 pNetInfo,
                         PPRINTER_INFO_2 pPrinter,
                         LPSTR lpString,
                         LPDEVMODE pDevmode) {

//
// Copy the required data to the Win 32 PRINTER_INFO_2 structure.
//
// CODEWORK - several of these fields are filled with "" placeholders.
// We probably need to put real information  in some of them.
// 

  PPSetString(&pPrinter->pServerName,lpServerName,&lpString);
  PPSetString(&pPrinter->pPrinterName,EMPTY_STRING,&lpString);
  PPSetString(&pPrinter->pShareName,(LPSTR) pNetInfo->pszName,&lpString);
  PPSetString(&pPrinter->pPortName,EMPTY_STRING,&lpString);
  PPSetString(&pPrinter->pDriverName,(LPSTR) pNetInfo->pszDriverName,&lpString);
  PPSetString(&pPrinter->pComment,(LPSTR) pNetInfo->pszComment,&lpString);
  PPSetString(&pPrinter->pLocation,EMPTY_STRING,&lpString);

  PPSetString(&pPrinter->pSepFile,(LPSTR) pNetInfo->pszSepFile,&lpString);  
  PPSetString(&pPrinter->pPrintProcessor,(LPSTR) pNetInfo->pszPrProc,&lpString);
  PPSetString(&pPrinter->pDatatype,EMPTY_STRING,&lpString);
  PPSetString(&pPrinter->pParameters,(LPSTR) pNetInfo->pszParms,&lpString);

  pPrinter->pSecurityDescriptor = NULL;
  pPrinter->Attributes      = PRINTER_ATTRIBUTE_SHARED;
  pPrinter->Priority        = (DWORD) pNetInfo->uPriority;
  pPrinter->DefaultPriority = pPrinter->Priority;
  pPrinter->StartTime       = SaneTime(pNetInfo->uStartTime);
  pPrinter->UntilTime       = SaneTime(pNetInfo->uUntilTime);
  pPrinter->Status          = (DWORD) pNetInfo->fsStatus;
  pPrinter->cJobs           = (DWORD) pNetInfo->cJobs;
  pPrinter->AveragePPM      = 0;

  if (pDevmode != NULL) {

    PPCopyMem((LPSTR *) &pPrinter->pDevMode,
             (LPSTR) pDevmode,
             pDevmode->dmSize + pDevmode->dmDriverExtra,
             &lpString);
  }
  else 
    pPrinter->pDevMode = NULL;

  return lpString;
}

/*******************************************************************
  _I_get_printer_level2

  Internal. Given pInfo, a block of information on a printer returned by a
  DosPrintQGetInfo call, fill a PRINTER_INFO_2 structure.  Returns
  TRUE if successful, FALSE otherwise. Places the number of bytes of 
  pBuffer used in the DWRD pointed to by pcbNeeded.

*******************************************************************/
BOOL _I_get_printer_level2(PNETPRINTERQUEUE pHandleInfo,
                           PPRQINFO3 pNetInfo,
                           LPDEVMODE pDevmode,
                           LPBYTE pBuffer,
                           DWORD cbBuf,
                           LPDWORD pcbNeeded) {

//
// Figure out how many bytes of buffer space we're going to need. 
// Set error code and return failure if we don't have enough.
//
  *pcbNeeded = _I_gp2_calc_size(pHandleInfo->szServerName,
                                pNetInfo,
                                pDevmode);

  if (*pcbNeeded > cbBuf) {

    DBGMSG(DBG_LEV_VERBOSE,("  print info:insufficient buffer\n"));
    SetLastError(ERROR_INSUFFICIENT_BUFFER);
    return FALSE;

  }

//
// Copy the required data to the Win 32 PRINTER_INFO_2 structure.
//
  _I_gp2_build_record(pHandleInfo->szServerName,
                      pNetInfo,
                      (PPRINTER_INFO_2) pBuffer,
                      EndOfBuffer(pBuffer,cbBuf),
                      pDevmode);

  return TRUE;

}

//////////////////////////////////////////////////////////////////////////////
// PPGetPrinter
// 
// Retrieves information about a printer. Returns TRUE if successful, FALSE
// if an error occurred. NOTE: Supports level 2 Printer_INFO to provide
// Point and Print functionality for Chicago.
//
//////////////////////////////////////////////////////////////////////////////
BOOL WINAPI  PPGetPrinter(HANDLE hPrinter,
                         DWORD Level,
                         LPBYTE pPrinter,
                         DWORD cbBuf,
                         LPDWORD pcbNeeded) {

  PNETPRINTERQUEUE pInfo;
  PPRQINFO3 pQueueInfo;
  DWORD cbNetSize;
  BOOL bResult;
  LPDEVMODE pDevmode;

  DBGMSG(DBG_LEV_VERBOSE,
        ("MSPP.PPGetPrinter(%d,%ld)\n",hPrinter,Level));

//
// Make sure we have a valid printer handle
//
  if (!ValidatePrinterHandle(hPrinter)) return FALSE;

  if (IsServerHandle(hPrinter)) return FALSE;

  pInfo = PtrFromHandle(hPrinter);

//
// Ask the server for information about the network printer.
//
  pQueueInfo = _I_net_get_info(hPrinter,&cbNetSize,&pDevmode);
  if (pQueueInfo == NULL) return FALSE;

#ifdef DEBUG

  DBGMSG(DBG_LEV_VERBOSE,
        ("  pDevmode: %x\n",pDevmode));
  
  if (pDevmode != NULL) {
    DBGMSG(DBG_LEV_VERBOSE,("  size: %d\n",pDevmode->dmSize+pDevmode->dmDriverExtra));
  }

#endif

//
// Reformat the data according to the requested info Level
//
  switch (Level) {

    case 1:
    
      bResult = _I_get_printer_level1(pQueueInfo,
                                      pPrinter,
                                      cbBuf,
                                      pcbNeeded);
      break;

    case 2:

      bResult = _I_get_printer_level2(pInfo,
                                      pQueueInfo,
                                      pDevmode,
                                      pPrinter,
                                      cbBuf,
                                      pcbNeeded);
      break;

    default:

      DBGMSG(DBG_LEV_WARN,("MSPP.PPGetPrinter: Invalid Level.\n"));

      SetLastError(ERROR_INVALID_LEVEL);
      bResult = FALSE;
      break;

  }

  DBGMSG(DBG_LEV_VERBOSE,("MSPP.PPGetPrinter returning %d\n",bResult));


  PPFreeMem(pQueueInfo,cbNetSize);
  return bResult;

}    

/*******************************************************************
  _I_map_pinfo1_to_prqinfo              

  Internal.  Fills a newly allocated PRQINFO structure with the
  job information from a user supplied PRINTER_INFO_1 structure. The
  number of bytes in the PRQINFO structure are placed in the
  DWORD indicated by pcbSize.  Returns NULL if unable to allocate
  enough space for the PRQINFO structure.

*******************************************************************/
LPBYTE _I_map_pinfo1_to_prqinfo(PPRINTER_INFO_1 pInfo,LPDWORD pcbSize) {
  PPRQINFO pPrq;
  LPSTR lpString;

//
// Figure out how much memory we're going to need
//
  *pcbSize = sizeof(PRQINFO) +
             lstrsize(pInfo->pComment);

  pPrq = (PPRQINFO) PPAllocMem(*pcbSize);
  if (pPrq == NULL) return NULL;

  lpString = EndOfBuffer(pPrq,*pcbSize);

//
// Shuffle the data from one record to the other.
//
  PPSetString((LPSTR *) &pPrq->pszComment,pInfo->pComment,&lpString);

  return (LPBYTE) pPrq;

}

/*******************************************************************
  _I_map_pinfo2_to_prqinfo              

  Internal.  Fills a newly allocated PRQINFO structure with the
  job information from a user supplied PRINTER_INFO_2 structure. The
  number of bytes in the PRQINFO structure are placed in the
  DWORD indicated by pcbSize.  Returns NULL if unable to allocate
  enough space for the PRQINFO structure.

*******************************************************************/
LPBYTE _I_map_pinfo2_to_prqinfo(PPRINTER_INFO_2 pInfo,LPDWORD pcbSize) {
  PPRQINFO pPrq;
  LPSTR lpString;

//
// Figure out how much memory we're going to need
//
  *pcbSize = sizeof(PRQINFO) +
             lstrsize(pInfo->pComment) + 
             lstrsize(pInfo->pSepFile) +
             lstrsize(pInfo->pPrintProcessor);


  pPrq = (PPRQINFO) PPAllocMem(*pcbSize);
  if (pPrq == NULL) return NULL;

  lpString = EndOfBuffer(pPrq,*pcbSize);

//
// Shuffle the data from one record to the other.
//
  pPrq->uPriority  = (USHORT) pInfo->Priority;
  pPrq->uStartTime = (USHORT) pInfo->StartTime;
  pPrq->uUntilTime = (USHORT) pInfo->UntilTime;

  PPSetString((LPSTR *) &pPrq->pszSepFile,pInfo->pSepFile,&lpString);
  PPSetString((LPSTR *) &pPrq->pszPrProc,pInfo->pPrintProcessor,&lpString);
  PPSetString((LPSTR *) &pPrq->pszParms,pInfo->pParameters,&lpString);

  PPSetString((LPSTR *) &pPrq->pszComment,pInfo->pComment,&lpString);

  return (LPBYTE) pPrq;

}

/*******************************************************************
  _I_set_printer_level1

  Internal.

  Given a completed PRQINFO structure, make the necessary DosPrintQ
  calls to do a level 1 Win32 SetPrinter.  Returns TRUE if successful,
  FALSE otherwise. Extended error info available through GetLastError.

  Assumes that the buffer pointed to by pNetQ was allocated by
  PPAllocMem() to NetQSize bytes.  Frees the buffer before returning.

*******************************************************************/
BOOL _I_do_set_printer_level1(LPSTR lpServer,
                              LPSTR lpQueue,
                              LPBYTE pNetQ,
                              DWORD NetQSize) {
  DWORD SplErr;

  if (pNetQ == NULL) return FALSE;

  SplErr = MapError(DosPrintQSetInfo((LPBYTE) lpServer,
                                     (LPBYTE) lpQueue,
                                     1,
                                     pNetQ,
                                     (USHORT) NetQSize,
                                     PRQ_COMMENT_PARMNUM));

  PPFreeMem(pNetQ,NetQSize);

  if (SplErr == NO_ERROR) return TRUE;

  DBGMSG(DBG_LEV_VERBOSE,("  MSPP:QSetInfo lev 1 failed. Code %d\n",SplErr));
  SetLastError(SplErr);
  return FALSE;

}

/*******************************************************************
  _I_set_printer_level2

  Internal.

  Given a completed PRQINFO structure, make the necessary DosPrintQ
  calls to do a level 2 Win32 SetPrinter.  Returns TRUE if successful,
  FALSE otherwise. Extended error info available through GetLastError.

  Assumes that the buffer pointed to by pNetQ was allocated by
  PPAllocMem() to NetQSize bytes.  Frees the buffer before returning.

*******************************************************************/
BOOL _I_do_set_printer_level2(LPSTR lpServer,
                              LPSTR lpQueue,
                              LPBYTE pNetQ,
                              DWORD NetQSize) {
  DWORD SplErr;
  BOOL bResult;
  int i,cParms;
  PPRQINFO pPrq;

  if (pNetQ == NULL) return FALSE;

// Calculate lengths of input strings for our table
//
  pPrq = (PPRQINFO) pNetQ;


  PrintQSizeTable[INDEX_PSZSEPFILE] = lstrsize((LPSTR) pPrq->pszSepFile);  
  PrintQSizeTable[INDEX_PSZPARMS]   = lstrsize((LPSTR) pPrq->pszParms);  
  PrintQSizeTable[INDEX_PSZCOMMENT] = lstrsize((LPSTR) pPrq->pszComment);  

// Set all the stuff we can. Since the PRQ_xxx_PARMNUM values are
// enumeration values rather than bit flags, they must be set one at 
// a time, using one net call per parameter. Kinda ugly...
//
  bResult = TRUE;
  cParms = sizeof(PrintQParmTable)/sizeof(PrintQParmTable[0]);

  for (i = 0; i < cParms; i++) {

    SplErr = MapError(DosPrintQSetInfo((LPBYTE) lpServer,
                                        (LPBYTE) lpQueue,
                                        1,
                                        pNetQ + PrintQOffsetTable[i],
                                        (USHORT) PrintQSizeTable[i],
                                        PrintQParmTable[i]));

    if (SplErr != NO_ERROR) {
      DBGMSG(DBG_LEV_VERBOSE,("  MSPP:QSetInfo failed. Index %d, Code %d\n",
                              i,SplErr));
      bResult = FALSE;
      SetLastError(SplErr);
    }
  }

  PPFreeMem(pNetQ,NetQSize);

  return bResult;
}

//////////////////////////////////////////////////////////////////////////////
// PPSetPrinter
// 
// Sets information about and issues commands to a printer. Returns TRUE if
// successful, FALSE if an error occurred. 
//
// Note: Supports info levels 0, 1 and 2.
//
//////////////////////////////////////////////////////////////////////////////
BOOL WINAPI  PPSetPrinter(HANDLE hPrinter,
                         DWORD Level,
                         LPBYTE pPrinter,
                         DWORD Command) {

  PNETPRINTERQUEUE pInfo;
  LPBYTE pNetQ;
  DWORD NetQSize;
  DWORD SplErr;
  BOOL bResult;

  DBGMSG(DBG_LEV_VERBOSE,("MSPP.PPSetPrinter(%d)\n",hPrinter));

//
// Make sure we have a good printer handle.
//
  if (!ValidatePrinterHandle(hPrinter)) return FALSE;

  if (IsServerHandle(hPrinter)) return FALSE;

  pInfo = PtrFromHandle(hPrinter);

//
// The Level argument controls how we set printer parameters. If Level is 0,
// pPrinter is required to be NULL. Otherwise, we map the Win32 
// structures into appropriate DosPrint structures and call the
// net API.
//
// NOTE: under this implementation, it is possible to
// issue a printer command and change printer parameters in the
// same call to SetPrinter.
//
  switch (Level) {

    case 0:                            
                                       
      bResult = (pPrinter == NULL);
      break;

    case 1:   

      pNetQ = _I_map_pinfo1_to_prqinfo((PPRINTER_INFO_1) pPrinter,
                                       &NetQSize);

      bResult = _I_do_set_printer_level1(pInfo->szServerName,
                                         pInfo->szQueueName,
                                         pNetQ,
                                         NetQSize);
      break;

    case 2:

      bResult = FALSE;
      pNetQ = _I_map_pinfo2_to_prqinfo((PPRINTER_INFO_2) pPrinter,
                                       &NetQSize);

      bResult = _I_do_set_printer_level2(pInfo->szServerName,
                                         pInfo->szQueueName,
                                         pNetQ,
                                         NetQSize);
      break;

    default:

      bResult = FALSE;
      SetLastError(ERROR_INVALID_LEVEL);
      break;

  }

//
// If we successfully set parameters, or if the user didn't ask us to
// set any parameters, issue any printer control commands specified.
//
  if (bResult) {

    switch (Command) {

      case 0:                           // no command
        break;

      case PRINTER_CONTROL_PAUSE:
        DBGMSG(DBG_LEV_VERBOSE,("Command: Pause print queue\n"));

        SplErr = MapError(DosPrintQPause((LPBYTE) pInfo->szServerName,
                                         (LPBYTE) pInfo->szQueueName)) ;

        if (SplErr != NO_ERROR) {
          DBGMSG(DBG_LEV_VERBOSE,("DosPrintQPause failed. Code: %d\n",SplErr));

          SetLastError(SplErr);  
          bResult = FALSE;

        }

        break;

      case PRINTER_CONTROL_PURGE:
        DBGMSG(DBG_LEV_VERBOSE,("Command: Purge print queue\n"));

        SplErr = MapError(DosPrintQPurge((LPBYTE) pInfo->szServerName,
                                         (LPBYTE) pInfo->szQueueName));

        if (SplErr != NO_ERROR) {

          DBGMSG(DBG_LEV_VERBOSE,("DosPrintQPurge failed. Code: %d\n",SplErr));
          SetLastError(SplErr);  
          bResult = FALSE;

        }

        break;

      case PRINTER_CONTROL_RESUME:
        DBGMSG(DBG_LEV_VERBOSE,("Command: Resume print queue\n"));

        SplErr = MapError(DosPrintQContinue((LPBYTE) pInfo->szServerName,
                                            (LPBYTE) pInfo->szQueueName));

        if (SplErr != NO_ERROR) {

          DBGMSG(DBG_LEV_VERBOSE,("DosPrintQPause failed. Code: %d\n",SplErr));
          SetLastError(SplErr);  
          bResult = FALSE;

        }

        break;

      default:

        bResult = FALSE;
        SetLastError(ERROR_INVALID_PARAMETER);
        break;

    }
  }

  return bResult;
}                                    
