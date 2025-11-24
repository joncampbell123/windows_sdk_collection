/*****************************************************************/
/**               Microsoft Windows                             **/
/**           Copyright (C) Microsoft Corp., 1991-1995          **/
/*****************************************************************/ 

/* PPJOBS.C .
 *   
 * MS Net print provider functions that deal with print jobs.
 *
 */          

#include "mspp.h"

#include <time.h>
#include <string.h>

extern "C" {
#include "dosdef.h"
#include "dospmspl.h"
}

#include "ppprn.h"
#include "ppjobs.h"
#include "errxlat.h"

//
// Table for translating W4W/LanMan job status codes to Win32 
// job status codes.
//
JOBSTATUSXLAT status_map[] = {

  {PRJ_COMPLETE   ,JOB_STATUS_PRINTED},
  {PRJ_INTERV     ,JOB_STATUS_ERROR},
  {PRJ_ERROR      ,JOB_STATUS_ERROR},
  {PRJ_DESTOFFLINE,JOB_STATUS_OFFLINE},
  {PRJ_DESTPAUSED ,JOB_STATUS_PAUSED},
  {PRJ_NOTIFY     ,JOB_STATUS_ERROR},
  {PRJ_DESTNOPAPER,JOB_STATUS_PAPEROUT},
  {PRJ_DESTFORMCHG,JOB_STATUS_ERROR},
  {PRJ_DESTCRTCHG ,JOB_STATUS_ERROR},
  {PRJ_DESTPENCHG ,JOB_STATUS_ERROR},
  {PRJ_DELETED    ,JOB_STATUS_DELETING}

};

/*******************************************************************
  _I_map_job_status

  Maps a W4W/Lanman job status USHORT to a Win32 status DWORD code.

*******************************************************************/
DWORD _I_map_job_status(USHORT uIn) {
  short i,n;
  DWORD dwOut;

// Translate LanMan status codes in bits 0-1.
//
  switch (uIn & PRJ_QSTATUS) {

    case PRJ_QS_QUEUED:
      DBGMSG(DBG_LEV_VERBOSE,("  job status: PRJ_QS_QUEUED\n"));
      dwOut = 0;
      break;

    case PRJ_QS_PAUSED:
      DBGMSG(DBG_LEV_VERBOSE,("  job status: PRJ_QS_PAUSED\n"));
      dwOut = JOB_STATUS_PAUSED;
      break;

    case PRJ_QS_SPOOLING:
      DBGMSG(DBG_LEV_VERBOSE,("  job status: PRJ_QS_SPOOLING\n"));
      dwOut = JOB_STATUS_SPOOLING;
      break;

    case PRJ_QS_PRINTING:
      DBGMSG(DBG_LEV_VERBOSE,("  job status: PRJ_QS_PRINTING\n"));
      dwOut = JOB_STATUS_PRINTING;
      break;
  }

// translate bits 2-11 of the LanMan status, which hold extended
// status flags.
//
  uIn = uIn & PRJ_DEVSTATUS;

  n = sizeof(status_map) / sizeof(JOBSTATUSXLAT);

  for (i = 0; i < n; i++) {

    if (uIn & status_map[i].in) dwOut |= status_map[i].out;

  }

  return dwOut;

}

/*******************************************************************
  _I_net_enum_jobs

  Internal. Allocates a buffer and fills it with an array of 
  PRJINFO structures containing information on each printer
  job in a specified queue.

  Returns a pointer to the newly allocated buffer if successful.
  The first structure in the buffer is actually a PRQINFO structure
  describing the queue.  The address of the first PRJINFO structure
  is given by (lpBuffer + sizeof(PRQINFO)). The PRQINFO structure
  contains the number of jobs retrieved.

  The size of the returned memory block is placed in the DWORD
  pointed to by cbSize. 

  Returns NULL if unable to retrieve job information. Error 
  info is available through GetLastError.

  NOTE: Assumes that hPrinter is valid.

*******************************************************************/
LPBYTE _I_net_enum_jobs(HANDLE hPrinter,LPDWORD cbSize,DWORD Level) {

  PNETPRINTERQUEUE pPrinter;
  LPSTR pNetInfo;
  USHORT Needed;
  DWORD SplErr;

  pPrinter = PtrFromHandle(hPrinter);

//
// When you call DosPrintQGetInfo to ask how much data it has available,
// with a NULL buffer and a buffer size of zero, it responds only with'
// the space required for the queue description record (about 44 bytes). 
// It doesn't tell you how much space it needs for job descriptions.
//
// To work around this behavior (bug?) as well as to speed things up by
// reducing the number of network requests, we're going to allocate
// a buffer large enough so we stand a good chance of getting all our
// info from a single request.
//
  *cbSize = 1000;

  pNetInfo = (LPSTR) PPAllocMem(*cbSize);
  if (pNetInfo == NULL) return NULL;

  for ( ; ; ) {      // forever

    Needed = (USHORT) *cbSize;

    SplErr = MapError(DosPrintQGetInfo((LPBYTE) pPrinter->szServerName,
                                       (LPBYTE) pPrinter->szQueueName,
                                       (USHORT) Level,
                                       (LPBYTE) pNetInfo,
                                       Needed,
                                       &Needed));
//
// If this returns successfully, we've got the data we need
//
    if (SplErr == ERROR_SUCCESS) break;

    PPFreeMem(pNetInfo,*cbSize);

//
// If more memory was needed, allocate it and try again.
//
    if (SplErr == ERROR_INSUFFICIENT_BUFFER) {

      *cbSize = (DWORD) Needed;

      pNetInfo = (LPSTR) PPAllocMem(*cbSize);
      if (pNetInfo == NULL) return NULL;

    }
//
// Otherwise a network error occurred and it's time to abandon ship
//
    else {

      SetLastError(SplErr);
      return NULL;

    }
  }

  return (LPBYTE) pNetInfo;

}

/*******************************************************************
  _I_ej1_calc_size

  Internal.  Calculates the amount of space needed to store a 
  single JOB_INFO_1 structure constructed from the specified
  PRJINFO structure.


*******************************************************************/
DWORD _I_ej1_calc_size(PNETPRINTERQUEUE pQueue,PPRJINFO pJob) {
  DWORD cbSize;

//
// CODEWORK - we could provide more detailed status info to the
// user by converting the net's status code to a text string for
// the JOB_INFO_x structure ourselves, instead of simply mapping
// the status code.
//
  cbSize = sizeof(JOB_INFO_1) + 
           (lstrsize(pQueue->szQueueName)) +      // Printer name
           (lstrsize(pQueue->szServerName)) +     // Machine name
           (lstrsize(pJob->szUserName)) +         // User name
           (lstrsize((LPSTR) pJob->pszComment)) +         // Document name
           (lstrsize((LPSTR) pJob->szDataType)) +         // Data type
           (lstrsize((LPSTR) pJob->pszStatus));           // Status string

  return cbSize;

}

/*******************************************************************
  _I_ej2_calc_size

  Internal.  Calculates the amount of space needed to store a 
  single JOB_INFO_2 structure constructed from the specified
  PRJINFO structure.


*******************************************************************/
DWORD _I_ej2_calc_size(PNETPRINTERQUEUE pQueue,PPRJINFO pJob) {
  DWORD cbSize;

//
// CODEWORK - we could provide more detailed status info to the
// user by converting the net's status code to a text string for
// the JOB_INFO_x structure ourselves, instead of simply mapping
// the status code.
//
  cbSize = sizeof(JOB_INFO_2) +
           (lstrsize((LPSTR) pQueue->szQueueName)) +      // Printer name
           (lstrsize((LPSTR) pQueue->szServerName)) +     // Machine name
           (lstrsize((LPSTR) pJob->szUserName)) +         // User name
           (lstrsize((LPSTR) pJob->pszComment)) +         // Document name
           (lstrsize((LPSTR) pJob->szNotifyName)) +       // Notify name
           (lstrsize((LPSTR) pJob->szDataType)) +         // Data type
           (EMPTY_STRING_SIZE) +                       // Print processor
           (lstrsize((LPSTR) pJob->pszParms)) +           // Parameters
           (EMPTY_STRING_SIZE) +                       // Driver Name
           (lstrsize((LPSTR) pJob->pszStatus));           // Status string

  return cbSize;

}

/*******************************************************************
  _I_convert_job_time

  Converts a job submittal time in Lan Manager's DWORD seconds 
  since Jan 1, 1970 format to a Win 32 SYSTEMTIME record.

*******************************************************************/
void _I_convert_job_time(DWORD dwTime,PSYSTEMTIME pSubmitted) {
  struct tm *jobtime;
  FILETIME ft;

// gmtime uses static data , so it's not reentrant. 
//
  PPEnterCritical();
  jobtime = gmtime((const long *) &dwTime);
  PPLeaveCritical();

  if (jobtime == NULL) return;

  pSubmitted->wYear         = (WORD) 1900 + jobtime->tm_year;
  pSubmitted->wMonth        = (WORD) 1 + jobtime->tm_mon;
  pSubmitted->wDayOfWeek    = (WORD) jobtime->tm_wday;
  pSubmitted->wDay          = (WORD) jobtime->tm_mday;
  pSubmitted->wHour         = (WORD) jobtime->tm_hour;
  pSubmitted->wMinute       = (WORD) jobtime->tm_min;
  pSubmitted->wSecond       = (WORD) jobtime->tm_sec;
  pSubmitted->wMilliseconds = 0;

// Convert local time from server to UTC.
//
  SystemTimeToFileTime(pSubmitted,&ft);
  LocalFileTimeToFileTime(&ft,&ft);      // convert local to UTC
  FileTimeToSystemTime(&ft,pSubmitted);
}

/*******************************************************************
  _I_ej1_build_record

  Internal. Reformats the data in a PRJINFO structure to fill
  a JOB_INFO_1 structure.  Takes a pointer to the start of
  string space for this structure and returns a pointer to
  the last available byte in the data buffer.

*******************************************************************/
LPSTR _I_ej1_build_record(PNETPRINTERQUEUE pQueue,
                          PPRJINFO pNetJob,
                          PJOB_INFO_1 pJobInfo,
                          LPSTR lpString) {

  pJobInfo->JobId = (DWORD) pNetJob->uJobId;

  PPSetString(&pJobInfo->pPrinterName,pQueue->szQueueName,&lpString);
  PPSetString(&pJobInfo->pMachineName,pQueue->szServerName,&lpString);
  PPSetString(&pJobInfo->pUserName,(LPSTR) pNetJob->szUserName,&lpString);
  PPSetString(&pJobInfo->pDocument,(LPSTR) pNetJob->pszComment,&lpString);
  PPSetString(&pJobInfo->pDatatype,(LPSTR) pNetJob->szDataType,&lpString);
  PPSetString(&pJobInfo->pStatus,(LPSTR) pNetJob->pszStatus,&lpString);  

  pJobInfo->Status = _I_map_job_status(pNetJob->fsStatus);
  pJobInfo->Priority = 0;
  pJobInfo->Position = (DWORD) pNetJob->uPosition;
  pJobInfo->TotalPages = 0;
  pJobInfo->PagesPrinted = 0;

  _I_convert_job_time(pNetJob->ulSubmitted,&pJobInfo->Submitted);

  return lpString;

}

/*******************************************************************
  _I_ej2_build_record

  Internal. Reformats the data in a PRJINFO structure to fill
  a JOB_INFO_2 structure.  Takes a pointer to the start of
  string space for this structure and returns a pointer to
  the last available byte in the buffer.

*******************************************************************/
LPSTR _I_ej2_build_record(PNETPRINTERQUEUE pPrinter,
                          PPRQINFO pNetQueue,
                          PPRJINFO pNetJob,
                          PJOB_INFO_2 pJobInfo,
                          LPSTR lpString) {

  pJobInfo->JobId = (DWORD) pNetJob->uJobId;

  PPSetString(&pJobInfo->pPrinterName,pPrinter->szQueueName,&lpString);
  PPSetString(&pJobInfo->pMachineName,pPrinter->szServerName,&lpString);
  PPSetString(&pJobInfo->pUserName,(LPSTR) pNetJob->szUserName,&lpString);
  PPSetString(&pJobInfo->pDocument,(LPSTR) pNetJob->pszComment,&lpString);
  PPSetString(&pJobInfo->pNotifyName,(LPSTR) pNetJob->szNotifyName,&lpString);
  PPSetString(&pJobInfo->pDatatype,(LPSTR) pNetJob->szDataType,&lpString);
  PPSetString(&pJobInfo->pPrintProcessor,EMPTY_STRING,&lpString);   
  PPSetString(&pJobInfo->pParameters,(LPSTR) pNetJob->pszParms,&lpString);
  PPSetString(&pJobInfo->pDriverName,EMPTY_STRING,&lpString);
  PPSetString(&pJobInfo->pStatus,(LPSTR) pNetJob->pszStatus,&lpString);

  pJobInfo->pDevMode = NULL;
  pJobInfo->pSecurityDescriptor = NULL;
  pJobInfo->Status = _I_map_job_status(pNetJob->fsStatus);
  pJobInfo->Priority = 0;
  pJobInfo->Position = (DWORD) pNetJob->uPosition;
  pJobInfo->StartTime = (DWORD) pNetQueue->uStartTime;
  pJobInfo->UntilTime = (DWORD) pNetQueue->uUntilTime;
  pJobInfo->TotalPages = 0;
  pJobInfo->Size = pNetJob->ulSize;
  pJobInfo->Time = 0;
  pJobInfo->PagesPrinted = 0;

  _I_convert_job_time(pNetJob->ulSubmitted,&pJobInfo->Submitted);

  return lpString;

}

/*******************************************************************
  _I_enum_jobs_level1

  Internal. Given a PRQINFO structure, followed by an array of
  PRJINFO structures as returned by DosPrintQGetInfo, fills a
  user supplied buffer with an array of JOB_INFO_1 structures.

  Returns TRUE if successful, FALSE otherwise.  If successful,
  returns the number of bytes of the supplied buffer actually
  used in the DWORD pointed to by pcbNeeded and the number of
  job records in the DWORD pointed to by pcbResulted.
  
  If the buffer isn't big enough to hold all the data, pcbNeeded 
  will be set to the amount of buffer space needed and pcbResulted
  will be set to the number of complete records actually copied.

  Extended error information is available through GetLastError().

*******************************************************************/
BOOL _I_enum_jobs_level1(PNETPRINTERQUEUE pPrinter,
                         LPBYTE pInfo,
                         DWORD FirstJob,
                         DWORD NumJobs,
                         LPBYTE pBuffer,
                         DWORD cbBuffer,
                         LPDWORD pcbNeeded,
                         LPDWORD pcbResulted) {

  PPRQINFO pNetQueue;
  PPRJINFO pNetJob;
  PJOB_INFO_1 pJobList;
  LPSTR    lpString;
  DWORD i;

  *pcbNeeded   = 0;
  *pcbResulted = 0;
  pNetQueue = (PPRQINFO) pInfo;

//
// If there are no jobs in the queue or if the users has requested that
// enumeration start after the last job in the queue, we're done.
//
  if (pNetQueue->cJobs <= FirstJob) {

    return TRUE;

  }

//
// Get a pointer to the first job that the caller wants enumerated.
//
  pNetJob = (PPRJINFO) (pInfo + sizeof(PRQINFO)
                        + (FirstJob * sizeof(PRJINFO)));

//
// Determine the amount of memory we'll need to fill the caller's
// request.  
//
  NumJobs = min(NumJobs, pNetQueue->cJobs - FirstJob);

  for (i = 0; i < NumJobs; i++,pNetJob++) {

    *pcbNeeded += _I_ej1_calc_size(pPrinter,pNetJob);

    if (*pcbNeeded <= cbBuffer) {

      (*pcbResulted)++;

    }
  }

//
// Now create an array of job information structures.
// String data is placed at the end of the buffer.
//
  pNetJob  = (PPRJINFO) (pInfo + sizeof(PRQINFO)
                         + (FirstJob * sizeof(PRJINFO)));

  pJobList = (PJOB_INFO_1) pBuffer;

  lpString = EndOfBuffer(pBuffer,cbBuffer);

  for (i = 0; i < *pcbResulted; i++,pNetJob++,pJobList++) {

    lpString = _I_ej1_build_record(pPrinter,pNetJob,pJobList,lpString);

  }

  if (*pcbResulted < NumJobs) {

    SetLastError(ERROR_INSUFFICIENT_BUFFER);
    return FALSE;

  }
  else {

    return TRUE;

  }

}

/*******************************************************************
  _I_enum_jobs_level2

  Internal. Given a PRQINFO structure, followed by an array of
  PRJINFO structures as returned by DosPrintQGetInfo, fills a
  user supplied buffer with an array of JOB_INFO_2 structures.

  Returns TRUE if successful, FALSE otherwise.  If successful,
  returns the number of bytes of the supplied buffer actually
  used in the DWORD pointed to by pcbNeeded and the number of
  job records in the DWORD pointed to by pcbResulted.
  
  If the buffer isn't big enough to hold all the data, pcbNeeded 
  will be set to the amount of buffer space needed and pcbResulted
  will be set to the number of complete records actually copied.

  Extended error information is available through GetLastError().

*******************************************************************/
BOOL _I_enum_jobs_level2(PNETPRINTERQUEUE pPrinter,
                         LPBYTE pInfo,
                         DWORD FirstJob,
                         DWORD NumJobs,
                         LPBYTE pBuffer,
                         DWORD  cbBuffer,
                         LPDWORD pcbNeeded,
                         LPDWORD pcbResulted) {

  PPRQINFO pNetQueue;
  PPRJINFO pNetJob;
  PJOB_INFO_2 pJobList;
  LPSTR    lpString;
  DWORD i;

  *pcbNeeded   = 0;
  *pcbResulted = 0;
  pNetQueue = (PPRQINFO) pInfo;

//
// If there are no jobs in the queue or if the users has requested that
// enumeration start after the last job in the queue, we're done.
//
  if (pNetQueue->cJobs <= FirstJob) {

    return TRUE;

  }

//
// Get a pointer to the first job that the caller wants enumerated.
//
  pNetJob = (PPRJINFO) (pInfo + sizeof(PRQINFO)
                        + (FirstJob * sizeof(PRJINFO)));

//
// Determine the amount of memory we'll need to fill the caller's
// request.  
//
  NumJobs = min(NumJobs, pNetQueue->cJobs - FirstJob);

  for (i = 0; i < NumJobs; i++,pNetJob++) {

    *pcbNeeded += _I_ej2_calc_size(pPrinter,pNetJob);

    if (*pcbNeeded <= cbBuffer) {

      (*pcbResulted)++;

    }
  }

//
// Now create an array of job information structures.
// String data is placed at the end of the data buffer.
//
  pNetJob  = (PPRJINFO) (pInfo + sizeof(PRQINFO)
                         + (FirstJob * sizeof(PRJINFO)));

  pJobList = (PJOB_INFO_2) pBuffer;

  lpString = EndOfBuffer(pBuffer,cbBuffer);

  for (i = 0; i < *pcbResulted; i++,pNetJob++,pJobList++) {

    lpString = _I_ej2_build_record(pPrinter,
                                   pNetQueue,
                                   pNetJob,
                                   pJobList,
                                   lpString);

  }

  if (*pcbResulted < NumJobs) {

    SetLastError(ERROR_INSUFFICIENT_BUFFER);
    return FALSE;

  }
  else {

    return TRUE;

  }
}
   
//////////////////////////////////////////////////////////////////////////////
// PPAddJob
//
// Fills a user specified buffer with a fully qualified pathname to a file 
// that can be used to store a print job. Returns TRUE if successful, FALSE
// if an error occurred.
//
// BUGBUG: Since this function does not map well into the SMB printing model,
// it is not implemented. It simply validates its parameters as best it
// can and returns failure.
//
//////////////////////////////////////////////////////////////////////////////
BOOL WINAPI PPAddJob(HANDLE hPrinter,
                     DWORD Level,
                     LPBYTE pData,
                     DWORD dbBuf,
                     LPDWORD pcbNeeded) {

  DBGMSG(DBG_LEV_VERBOSE,("MSPP.PPAddJob(%d)\n",hPrinter));

  if (ValidatePrinterHandle(hPrinter)) {

    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);

  }

  return FALSE;

}            

//////////////////////////////////////////////////////////////////////////////
// _delete_job
//
// Internal. Deletes a print job. hPrinter is assumed to be valid.
//
//////////////////////////////////////////////////////////////////////////////
BOOL _delete_job(HANDLE hPrinter, DWORD JobId) {

  PNETPRINTERQUEUE pPrinter;
  DWORD SplErr;

  DBGMSG(DBG_LEV_VERBOSE,("MSPP: delete job # %ld\n",JobId));

  pPrinter = PtrFromHandle(hPrinter);

  SplErr = MapError(DosPrintJobDel((LPBYTE) pPrinter->szServerName,
                                   (USHORT) JobId));

  if (SplErr != NO_ERROR) {

    DBGMSG(DBG_LEV_ERROR,("  delete job failed. Code: %ld\n",SplErr));

    SetLastError(SplErr);
    return FALSE;

  }
  else {

    return TRUE;

  }
}            

//////////////////////////////////////////////////////////////////////////////
// _pause_job
//
// Internal. Pauses a print job. hPrinter is assumed to be valid.
//
//////////////////////////////////////////////////////////////////////////////
BOOL _pause_job(HANDLE hPrinter, DWORD JobId) {

  PNETPRINTERQUEUE pPrinter;
  DWORD SplErr;

  DBGMSG(DBG_LEV_VERBOSE,("MSPP: pause job # %ld\n",JobId));

  pPrinter = PtrFromHandle(hPrinter);

  SplErr = MapError(DosPrintJobPause((LPBYTE) pPrinter->szServerName,
                                   (USHORT) JobId));

  if (SplErr != NO_ERROR) {

    DBGMSG(DBG_LEV_ERROR,("  pause job failed. Code: %ld\n",SplErr));

    SetLastError(SplErr);
    return FALSE;

  }
  else {

    return TRUE;

  }
}

//////////////////////////////////////////////////////////////////////////////
// _resume_job
//
// Internal. Causes a paused print job to resume. hPrinter is assumed to 
// be valid.
//
//////////////////////////////////////////////////////////////////////////////
BOOL _resume_job(HANDLE hPrinter, DWORD JobId) {

  PNETPRINTERQUEUE pPrinter;
  DWORD SplErr;

  DBGMSG(DBG_LEV_VERBOSE,("MSPP: resume job # %ld\n",JobId));

  pPrinter = PtrFromHandle(hPrinter);

  SplErr = MapError(DosPrintJobContinue((LPBYTE) pPrinter->szServerName,
                                   (USHORT) JobId));

  if (SplErr != NO_ERROR) {

    DBGMSG(DBG_LEV_ERROR,("  resume job failed. Code: %ld\n",SplErr));

    SetLastError(SplErr);
    return FALSE;

  }
  else {

    return TRUE;

  }
}

//////////////////////////////////////////////////////////////////////////////
// PPEnumJobs
//
// Retrieves information about a specified set of print jobs for a specified
// printer.  Returns TRUE if successful, FALSE if an error occurred.
//
//////////////////////////////////////////////////////////////////////////////   
BOOL WINAPI PPEnumJobs(HANDLE hPrinter,
                       DWORD FirstJob,
                       DWORD NoJobs,
                       DWORD Level,
                       LPBYTE pJob,
                       DWORD cbBuf,
                       LPDWORD pcbNeeded,
                       LPDWORD pcbResulted) {


  BOOL bResult;
  DWORD JobListSize;
  LPBYTE lpJobList;                       

  DBGMSG(DBG_LEV_VERBOSE,("MSPP.PPEnumJobs(%d)\n",hPrinter));

//
// Make sure we have a valid printer handle
//
  if (!ValidatePrinterHandle(hPrinter)) return FALSE;

//
// Attempt to get a list of jobs from the network print spooler
//
  lpJobList = _I_net_enum_jobs(hPrinter,&JobListSize,2);
  if (lpJobList == NULL) return FALSE;

//
// Format the job information to the requested information level.
//
  switch (Level) {

    case 1:

      bResult = _I_enum_jobs_level1(PtrFromHandle(hPrinter),
                                    lpJobList,
                                    FirstJob,
                                    NoJobs,
                                    pJob,
                                    cbBuf,
                                    pcbNeeded,
                                    pcbResulted);
      break;


    case 2:

      bResult = _I_enum_jobs_level2(PtrFromHandle(hPrinter),
                                    lpJobList,
                                    FirstJob,
                                    NoJobs,
                                    pJob,
                                    cbBuf,
                                    pcbNeeded,
                                    pcbResulted);
      break;

    default:

      DBGMSG(DBG_LEV_WARN,("MSPP.PPEnumJobs: Invalid Level.\n"));

      SetLastError(ERROR_INVALID_LEVEL);
      bResult = FALSE;
      break;
  }

//
// Free the memory we used for our network job list.
//
  PPFreeMem(lpJobList,JobListSize);

  return bResult;
              
} 

/*******************************************************************
  _I_net_get_job

  Uses DosPrintJobGetInfo to retrieve information about a print
  job from the network.  If successful, returns a pointer to a
  newly allocated (with PPAllocMem) block of memory containing
  the job information.  Returns NULL and calls SetLastError if
  unable to access the job.

*******************************************************************/
LPBYTE _I_net_get_job(LPSTR lpServer,
                      DWORD JobId,
                      DWORD Level,
                      LPDWORD pcbSize) {

  LPBYTE lpBuffer;
  USHORT Needed;
  DWORD SplErr;

//
// Find out how much memory we're going to need for this.
//
// TBD - we're going to need support for Job Info level 3 to
// get all the info Win32 wants.
//
  SplErr = MapError(DosPrintJobGetInfo((LPBYTE) lpServer,
                                       (USHORT) JobId,
                                       (USHORT) Level,
                                       NULL,
                                       0,
                                       &Needed));

  if (SplErr != ERROR_INSUFFICIENT_BUFFER) {

    DBGMSG(DBG_LEV_ERROR,
      ("MSPP.net_get_job: DosPrintJobGetInfo failed. Code %d\n",SplErr));

    SetLastError(SplErr);
    return NULL;

  }

//
// Attempt to allocate an appropriately sized chunk'o'memory
//
  *pcbSize = (DWORD) Needed;
  lpBuffer = (LPBYTE) PPAllocMem(*pcbSize);
  if (lpBuffer == NULL) return NULL;

//
// Retrieve the job information from the network
//
  SplErr = MapError(DosPrintJobGetInfo((LPBYTE) lpServer,
                                       (USHORT) JobId,
                                       (USHORT) Level,
                                       lpBuffer,
                                       Needed,
                                       &Needed));

  if (SplErr != NO_ERROR) {

    PPFreeMem(lpBuffer,*pcbSize);
    SetLastError(SplErr);
    return NULL;

  }

  return lpBuffer;

}

//////////////////////////////////////////////////////////////////////////////
// PPGetJob
//
// Retrieves information about a print job on a specified printer. Returns
// TRUE if successful, FALSE if an error occurred.
//
//////////////////////////////////////////////////////////////////////////////
BOOL WINAPI PPGetJob(HANDLE hPrinter,
                     DWORD JobId,
                     DWORD Level,
                     LPBYTE pJob,
                     DWORD cbBuf,
                     LPDWORD pcbNeeded) {

  BOOL bResult;
  PNETPRINTERQUEUE pPrinter;
  DWORD  NetJobSize;
  DWORD  NetQSize;
  DWORD  dwScratch;
  LPBYTE lpNetJob;
  LPBYTE lpQueue;

  DBGMSG(DBG_LEV_VERBOSE,("MSPP.PPGetJob(%d)\n",hPrinter));

//
// Make sure we're looking at a valid printer handle
//
  if (!ValidatePrinterHandle(hPrinter)) return FALSE;

  pPrinter = PtrFromHandle(hPrinter);

  lpNetJob = _I_net_get_job(pPrinter->szServerName,
                            JobId,
                            1,
                            &NetJobSize);

  if (lpNetJob == NULL) return FALSE;


  switch (Level) {

    case 1:

      *pcbNeeded = _I_ej1_calc_size(pPrinter,(PPRJINFO) lpNetJob);

      if (*pcbNeeded <= cbBuf) {

        _I_ej1_build_record(pPrinter,
                            (PPRJINFO) lpNetJob,
                            (PJOB_INFO_1) pJob,
                            EndOfBuffer(pJob,cbBuf));

        bResult = TRUE;
      }
      else {

        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        bResult = FALSE;

      }

      break;

    case 2:

//
// We need printer queue start and stop times for this info level, so
// we've got to get some information on the printer queue
//
      if (!PPGetPrinter(hPrinter,2,(LPBYTE) &NetQSize,0,&NetQSize)) break;

      lpQueue = (LPBYTE) PPAllocMem(NetQSize);
      if (lpQueue == NULL) break;

      if (!PPGetPrinter(hPrinter,2,lpQueue,NetQSize,&dwScratch)) break;

//
// If the user gave us enough buffer space, construct our job record
//
      *pcbNeeded = _I_ej2_calc_size(pPrinter,(PPRJINFO) lpNetJob);

      if (*pcbNeeded <= cbBuf) {

        _I_ej2_build_record(pPrinter,
                            (PPRQINFO) lpQueue,
                            (PPRJINFO) lpNetJob,
                            (PJOB_INFO_2) pJob,
                            EndOfBuffer(pJob,cbBuf));

        bResult = TRUE;
      }
      else {

        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        bResult = FALSE;

      }

      PPFreeMem(lpQueue,NetQSize);

      break;

    default:

      DBGMSG(DBG_LEV_WARN,("MSPP.PPGetJob: Invalid Level.\n"));

      SetLastError(ERROR_INVALID_LEVEL);
      bResult = FALSE;
      break;
  }

  PPFreeMem(lpNetJob,NetJobSize);

  return bResult;
            
}                         

/*******************************************************************
  _set_job_level1

  Attempts to change print job settings to those specified by the
  supplied PJOB_INFO_1 structure.  Returns TRUE if successful,
  FALSE otherwise. Extended error information available from
  GetLastError().

*******************************************************************/
BOOL _set_job_level1(PNETPRINTERQUEUE pPrinter,
                     DWORD dwJobId,
                     PJOB_INFO_1 pJob) {
  DWORD Result;
  USHORT len;

//
// Set comment
//
  if (lstrsize(pJob->pDocument)) {

    len = (USHORT) max(lstrsize(pJob->pDocument),49); // LM MAXCOMMENTSIZE+1

    Result = 
      MapError(DosPrintJobSetInfo((LPBYTE) pPrinter->szServerName,
                                  (USHORT) dwJobId,
                                  1,
                                  (LPBYTE) pJob->pDocument,
                                  len,
                                  PRJ_COMMENT_PARMNUM));

    if (Result != ERROR_SUCCESS) {
      DBGMSG(DBG_LEV_VERBOSE,
        ("MSPP.JobSetInfo(comment) failed. Code %ld\n",Result));
      return FALSE;
    }
  }

//
// Set Position
//
  if (pJob->Position == 0) return TRUE;

  Result = MapError(DosPrintJobSetInfo((LPBYTE) pPrinter->szServerName,
                                      (USHORT) dwJobId,
                                      1,
                                      (LPBYTE) &pJob->Position,
                                      sizeof(USHORT),
                                      PRJ_POSITION_PARMNUM));
  if (Result != ERROR_SUCCESS) {
    DBGMSG(DBG_LEV_VERBOSE,
      ("MSPP.JobSetInfo(position) failed. Code %ld\n",Result));
    return FALSE;
  }

  return TRUE;

}

/*******************************************************************
  _set_job_level2

  Attempts to change print job settings to those specified by the
  supplied PJOB_INFO_2 structure.  Returns TRUE if successful,
  FALSE otherwise. Extended error information available from
  GetLastError().

*******************************************************************/
BOOL _set_job_level2(PNETPRINTERQUEUE pPrinter,
                     DWORD dwJobId,
                     PJOB_INFO_2 pJob) {
  DWORD Result;
  USHORT len;


//
// Set comment
//
  if (lstrsize(pJob->pDocument)) {

    len = (USHORT) max(lstrsize(pJob->pDocument),49); // LM MAXCOMMENTSIZE+1

    Result = 
      MapError(DosPrintJobSetInfo((LPBYTE) pPrinter->szServerName,
                                  (USHORT) dwJobId,
                                  1,
                                  (LPBYTE) pJob->pDocument,
                                  len,
                                  PRJ_COMMENT_PARMNUM));

    if (Result != NO_ERROR) goto sj2_exception;

  }

//
// Set data type
//
  if (lstrsize(pJob->pDatatype)) {

    Result = MapError(DosPrintJobSetInfo((LPBYTE) pPrinter->szServerName,
                                        (USHORT) dwJobId,
                                        3,
                                        (LPBYTE) pJob->pDatatype,
                                        (USHORT) sizeof(LPBYTE),
                                        PRJ_DATATYPE_PARMNUM));

    if (Result != NO_ERROR) goto sj2_exception;
  }

//
// Set notify name
//
  if (lstrsize(pJob->pNotifyName)) {

    Result = MapError(DosPrintJobSetInfo((LPBYTE) pPrinter->szServerName,
                                        (USHORT) dwJobId,
                                        3,
                                        (LPBYTE) pJob->pNotifyName,
                                        (USHORT) lstrsize(pJob->pNotifyName),
                                        PRJ_NOTIFYNAME_PARMNUM));

    if (Result != NO_ERROR) goto sj2_exception;
  }

//
// Set parameters
//
  if (lstrsize(pJob->pParameters)) {

    Result = MapError(DosPrintJobSetInfo((LPBYTE) pPrinter->szServerName,
                                        (USHORT) dwJobId,
                                        3,
                                        (LPBYTE) pJob->pParameters,
                                        (USHORT) lstrsize(pJob->pParameters),
                                        PRJ_PARMS_PARMNUM));

    if (Result != NO_ERROR) goto sj2_exception;
  }

//
// Set priority
//
  if (pJob->Priority > 0) {

    Result = MapError(DosPrintJobSetInfo((LPBYTE) pPrinter->szServerName,
                                        (USHORT) dwJobId,
                                        3,
                                        (LPBYTE) (PUSHORT) &pJob->Priority,
                                        sizeof(USHORT),
                                        PRJ_PRIORITY_PARMNUM));

    if (Result != NO_ERROR) goto sj2_exception;
  }

//
// Set position
//

  Result = MapError(DosPrintJobSetInfo((LPBYTE) pPrinter->szServerName,
                                      (USHORT) dwJobId,
                                      3,
                                      (LPBYTE) (PUSHORT) &pJob->Position,
                                      sizeof(USHORT),
                                      PRJ_POSITION_PARMNUM));

  if (Result != NO_ERROR) goto sj2_exception;

//
// All done, no errors.
//
  return TRUE;

//
// This is where we land if one of our DosPrintJobSetInfo calls
// fails.
//
sj2_exception:
  DBGMSG(DBG_LEV_ERROR,("  _set_job_level2 failed.  Code %ld\n",Result));

  SetLastError(Result);
  return FALSE;

}

//////////////////////////////////////////////////////////////////////////////
// PPSetJob
//
// Sets information for and issues commands to a print job.  Returns
// TRUE if successful, FALSE if an error occurred.
//
//////////////////////////////////////////////////////////////////////////////
BOOL WINAPI PPSetJob(HANDLE hPrinter,
                     DWORD JobId,
                     DWORD Level,
                     LPBYTE pJob,
                     DWORD Command) {

  BOOL bResult;
  PNETPRINTERQUEUE pPrinter;

  DBGMSG(DBG_LEV_VERBOSE,
        ("MSPP.PPSetJob(%d,JobID: %ld,Level: %ld)\n",hPrinter,JobId,Level));

//
// Make sure we've got a valid printer handle
//
  if (!ValidatePrinterHandle(hPrinter)) return FALSE;

  pPrinter = PtrFromHandle(hPrinter);

//
// Set job parameters
//
  switch(Level) {

    case 0:           // Info level 0 means we do not set parameters
      bResult = TRUE;
      break;

    case 1:
      bResult = _set_job_level1(pPrinter,JobId,(PJOB_INFO_1) pJob);
      break;

    case 2:
      bResult = _set_job_level2(pPrinter,JobId,(PJOB_INFO_2) pJob);
      break;

    default:
      DBGMSG(DBG_LEV_WARN,("MSPP.PPSetJob: Invalid Level.\n"));

      SetLastError(ERROR_INVALID_LEVEL);
      bResult = FALSE;
      break;
  }

//
// If parameters were successfully set, perform specified
// job control command.
//
  if (bResult) {

    switch (Command) {

      case 0:              // No command
        bResult = TRUE;    
        break;

      case JOB_CONTROL_CANCEL:
        bResult = _delete_job(hPrinter,JobId);
        break;

      case JOB_CONTROL_PAUSE:
        bResult = _pause_job(hPrinter,JobId);
        break;

      case JOB_CONTROL_RESTART:
        bResult = _resume_job(hPrinter,JobId);
        break;

      case JOB_CONTROL_RESUME:
        bResult = _resume_job(hPrinter,JobId);
        break;

    }
  }

  return bResult;
            
}      

//////////////////////////////////////////////////////////////////////////////
// PPScheduleJob
//
// Informs the spool subsystem that a job that was previously added with
// AddJob can now be scheduled to print.  Returns TRUE if successful, FALSE if
// an error occurred.
//
// BUGBUG: Since this function does not map well into the SMB printing model,
// it is not implemented. It simply validates its parameters as best it
// can and returns failure.
//
//////////////////////////////////////////////////////////////////////////////                          
BOOL WINAPI PPScheduleJob(HANDLE hPrinter,
                          DWORD JobId) {

  DBGMSG(DBG_LEV_VERBOSE,("MSPP.PPScheduleJob(%d)\n",hPrinter));

  if (ValidatePrinterHandle(hPrinter)) {

    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);

  }

  return FALSE;

}                 
