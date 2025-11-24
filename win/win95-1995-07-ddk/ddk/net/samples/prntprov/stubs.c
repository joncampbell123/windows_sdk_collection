/*****************************************************************/ 
/**               Microsoft Windows 95                          **/
/**           Copyright (C) Microsoft Corp., 1991-1995          **/
/*****************************************************************/ 

/* STUBS.C
 *  
 * Stubs for unimplemented optional print provider functions
 */          

#include "mspp.h"

/*******************************************************************
  _stub_routine

  Common code for stubbed pp routines. Sets last error then 
  returns FALSE.
*******************************************************************/
BOOL _stub_routine() {

  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;

}

BOOL WINAPI stubOpenPrinter(LPTSTR pPrinterName,
                            LPHANDLE phPrinter,
                            LPPRINTER_DEFAULTS pDefault) {
  return _stub_routine();
}

BOOL WINAPI stubSetJob(HANDLE hPrinter,
                       DWORD JobId,
                       DWORD Level,
                       LPBYTE pJob,
                       DWORD Command) {

  return _stub_routine();
}

BOOL WINAPI stubGetJob(HANDLE hPrinter,
                       DWORD JobId,
                       DWORD Level,
                       LPBYTE pJob,
                       DWORD cbBuf,
                       LPDWORD  pcbNeeded) {

  return _stub_routine();
}

BOOL WINAPI stubEnumJobs(HANDLE  hPrinter,
 DWORD FirstJob,
 DWORD NoJobs,
 DWORD Level,
 LPBYTE  pJob,
 DWORD cbBuf,
 LPDWORD pcbNeeded,
 LPDWORD pcReturned) {

  return _stub_routine();
}

HANDLE WINAPI stubAddPrinter(LPTSTR  pName,
  DWORD Level,
  LPBYTE  pPrinter) {

  return (HANDLE) _stub_routine();
}

BOOL WINAPI stubDeletePrinter(HANDLE hPrinter) {

  return _stub_routine();
}

BOOL WINAPI stubSetPrinter(HANDLE  hPrinter,
                           DWORD Level,
                           LPBYTE  pPrinter,
                           DWORD Command) {

  return _stub_routine();
}

BOOL WINAPI stubGetPrinter(HANDLE  hPrinter,
                           DWORD Level,
                           LPBYTE  pPrinter,
                           DWORD cbBuf,
                           LPDWORD pcbNeeded) {

  return _stub_routine();
}

BOOL WINAPI stubEnumPrinters(DWORD Flags,
                             LPTSTR  Name,
                             DWORD Level,
                             LPBYTE  pPrinterEnum,
                             DWORD cbBuf,
                             LPDWORD pcbNeeded,
                             LPDWORD pcReturned) {

  return _stub_routine();
}

BOOL WINAPI stubAddPrinterDriver(LPTSTR  pName,
                                 DWORD Level,
                                 LPBYTE  pDriverInfo) {

  return _stub_routine();
}

BOOL WINAPI stubEnumPrinterDrivers(LPTSTR  pName,
                                   LPTSTR  pEnvironment,
                                   DWORD Level,
                                   LPBYTE  pDriverInfo,
                                   DWORD cbBuf,
                                   LPDWORD pcbNeeded,
                                   LPDWORD pcReturned) {

  return _stub_routine();
}

BOOL WINAPI stubGetPrinterDriver(HANDLE  hPrinter,
                                 LPTSTR  pEnvironment,
                                 DWORD Level,
                                 LPBYTE pDriverInfo,
                                 DWORD cbBuf,
                                 LPDWORD pcbNeeded) {

  return _stub_routine();
}

BOOL WINAPI stubGetPrinterDriverDirectory(LPTSTR  pName,
                                          LPTSTR  pEnvironment,
                                          DWORD Level,
                                          LPBYTE  pDriverDirectory,
                                          DWORD cbBuf,
                                          LPDWORD pcbNeeded) {

  return _stub_routine();
}

BOOL WINAPI stubDeletePrinterDriver(LPTSTR pName,
                                    LPTSTR pEnvironment,
                                    LPTSTR pDriverName) {

  return _stub_routine();
}

BOOL WINAPI stubAddPrintProcessor(LPTSTR  pName,
                                  LPTSTR  pEnvironment,
                                  LPTSTR  pPathName,
                                  LPTSTR  pPrintProcessorName) {

  return _stub_routine();
}

BOOL WINAPI stubEnumPrintProcessors(LPTSTR  pName,
                                    LPTSTR  pEnvironment,
                                    DWORD Level,
                                    LPBYTE  pPrintProcessorInfo,
                                    DWORD cbBuf,
                                    LPDWORD pcbNeeded,
                                    LPDWORD pcReturned) {

  return _stub_routine();
}

BOOL WINAPI stubGetPrintProcessorDirectory(LPTSTR  pName,
                                           LPTSTR  pEnvironment,
                                           DWORD Level,
                                           LPBYTE  pPrintProcessorInfo,
                                           DWORD cbBuf,
                                           LPDWORD pcbNeeded) {

  return _stub_routine();
}

BOOL WINAPI stubDeletePrintProcessor(LPTSTR  pName,
                                     LPTSTR  pEnvironment,
                                     LPTSTR  pPrintProcessorName) {

  return _stub_routine();
}

BOOL WINAPI stubEnumPrintProcessorDatatypes(LPTSTR  pName,
                                            LPTSTR  pPrintProcessorName,
                                            DWORD Level,
                                            LPBYTE  pDataypes,
                                            DWORD cbBuf,
                                            LPDWORD pcbNeeded,
                                            LPDWORD pcReturned) {

  return _stub_routine();
}

DWORD stubStartDocPrinter(HANDLE  hPrinter,
                          DWORD Level,
                          LPBYTE  pDocInfo) {

  return _stub_routine();
}

BOOL WINAPI stubStartPagePrinter(HANDLE  hPrinter) {

  return _stub_routine();
}

BOOL WINAPI stubWritePrinter(HANDLE  hPrinter,
  LPVOID pBuf,
  DWORD cbBuf,
  LPDWORD pcWritten) {

  return _stub_routine();
}

BOOL WINAPI stubEndPagePrinter(HANDLE hPrinter) {

  return _stub_routine();
}

BOOL WINAPI stubAbortPrinter(HANDLE hPrinter) {

  return _stub_routine();
}

BOOL WINAPI stubReadPrinter(HANDLE hPrinter,
 LPVOID  pBuf,
 DWORD cbBuf,
 LPDWORD pNoBytesRead) {

  return _stub_routine();
}

BOOL WINAPI stubEndDocPrinter(HANDLE hPrinter) {

  return _stub_routine();
}

BOOL WINAPI stubAddJob(HANDLE  hPrinter,
  DWORD Level,
  LPBYTE  pData,
  DWORD cbBuf,
  LPDWORD pcbNeeded) {

  return _stub_routine();
}

BOOL WINAPI stubScheduleJob(HANDLE  hPrinter,
                            DWORD JobId) {

  return _stub_routine();
}

DWORD WINAPI stubGetPrinterData(HANDLE hPrinter,
                         LPTSTR pValueName,
                         LPDWORD  pType,
                         LPBYTE pData,
                         DWORD nSize,
                         LPDWORD  pcbNeeded) {

  return _stub_routine();
}

DWORD WINAPI stubSetPrinterData(HANDLE  hPrinter,
                         LPTSTR  pValueName,
                         DWORD Type,
                         LPBYTE  pData,
                         DWORD cbData) {

  return _stub_routine();
}

DWORD WINAPI stubWaitForPrinterChange(HANDLE hPrinter, DWORD Flags) {

  return _stub_routine();
}

BOOL WINAPI stubClosePrinter(HANDLE hPrinter) {

  return _stub_routine();
}

BOOL WINAPI stubAddForm(HANDLE  hPrinter,
                        DWORD Level,
                        LPBYTE  pForm) {

  return _stub_routine();
}

BOOL WINAPI stubDeleteForm(HANDLE  hPrinter,
LPTSTR  pFormName) {

  return _stub_routine();
}

BOOL WINAPI stubGetForm(HANDLE  hPrinter,
                        LPTSTR  pFormName,
                        DWORD Level,
                        LPBYTE  pForm,
                        DWORD cbBuf,
                        LPDWORD pcbNeeded) {

  return _stub_routine();
}

BOOL WINAPI stubSetForm(HANDLE  hPrinter,
  LPTSTR  pFormName,
  DWORD Level,
  LPBYTE  pForm) {

  return _stub_routine();
}

BOOL WINAPI stubEnumForms(HANDLE  hPrinter,
  DWORD Level,
  LPBYTE  pForm,
  DWORD cbBuf,
  LPDWORD pcbNeeded,
  LPDWORD pcReturned) {

  return _stub_routine();
}

BOOL WINAPI stubEnumMonitors(LPTSTR  pName,
  DWORD Level,
  LPBYTE  pMonitors,
  DWORD cbBuf,
  LPDWORD pcbNeeded,
  LPDWORD pcReturned) {

  return _stub_routine();
}

BOOL WINAPI stubEnumPorts(LPTSTR  pName,
  DWORD Level,
  LPBYTE  pPorts,
  DWORD cbBuf,
  LPDWORD pcbNeeded,
  LPDWORD pcReturned) {

  return _stub_routine();
}

BOOL WINAPI stubAddPort(LPTSTR  pName,
                        HWND hWnd,
                        LPTSTR  pMonitorName) {

  return _stub_routine();
}

BOOL WINAPI stubConfigurePort(LPTSTR  pName,
                              HWND hWnd,
                              LPTSTR  pPortName) {

  return _stub_routine();
}

BOOL WINAPI stubDeletePort(LPTSTR  pName,
                           HWND hWnd,
                           LPTSTR  pPortName) {

  return _stub_routine();
}

HANDLE WINAPI stubCreatePrinterIC(HANDLE  hPrinter,
                           LPDEVMODEW pDevMode) {

  return (HANDLE) _stub_routine();
}

BOOL WINAPI stubPlayGdiScriptOnPrinterIC(HANDLE  hPrinterIC,
                                         LPBYTE  pIn,
                                         DWORD cIn,
                                         LPBYTE  pOut,
                                         DWORD cOut,
                                         DWORD ul) {

  return _stub_routine();
}

BOOL WINAPI stubDeletePrinterIC(HANDLE  hPrinterIC) {

  return _stub_routine();
}

BOOL WINAPI stubAddPrinterConnection(LPTSTR  pName) {

  return _stub_routine();
}

BOOL WINAPI stubDeletePrinterConnection(LPTSTR pName) {

  return _stub_routine();
}

DWORD WINAPI stubPrinterMessageBox(HANDLE  hPrinter,
  DWORD Error,
  HWND hWnd,
  LPTSTR  pText,
  LPTSTR  pCaption,
  DWORD dwType) {

  return _stub_routine();
}

BOOL WINAPI stubAddMonitor(LPTSTR  pName,
                           DWORD Level,
                           LPBYTE  pMonitorInfo) {

  return _stub_routine();
}

BOOL WINAPI stubDeleteMonitor(LPTSTR  pName,
                           LPTSTR  pEnvironment,
                           LPTSTR  pMonitorName) {

  return _stub_routine();
}






