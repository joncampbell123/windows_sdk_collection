/*****************************************************************/ 
/**               Microsoft Windows 95                          **/
/**           Copyright (C) Microsoft Corp., 1991-1995          **/
/*****************************************************************/ 

BOOL WINAPI stubOpenPrinter(LPTSTR pPrinterName,
                            LPHANDLE phPrinter,
                            LPPRINTER_DEFAULTS pDefault) ;

BOOL WINAPI stubSetJob(HANDLE hPrinter,
                       DWORD JobId,
                       DWORD Level,
                       LPBYTE pJob,
                       DWORD Command) ;

BOOL WINAPI stubGetJob(HANDLE hPrinter,
                       DWORD JobId,
                       DWORD Level,
                       LPBYTE pJob,
                       DWORD cbBuf,
                       LPDWORD  pcbNeeded) ;

BOOL WINAPI stubEnumJobs(HANDLE  hPrinter,
 DWORD FirstJob,
 DWORD NoJobs,
 DWORD Level,
 LPBYTE  pJob,
 DWORD cbBuf,
 LPDWORD pcbNeeded,
 LPDWORD pcReturned) ;

HANDLE WINAPI stubAddPrinter(LPTSTR  pName,
  DWORD Level,
  LPBYTE  pPrinter) ;

BOOL WINAPI stubDeletePrinter(HANDLE hPrinter) ;

BOOL WINAPI stubSetPrinter(HANDLE  hPrinter,
                           DWORD Level,
                           LPBYTE  pPrinter,
                           DWORD Command) ;

BOOL WINAPI stubGetPrinter(HANDLE  hPrinter,
                           DWORD Level,
                           LPBYTE  pPrinter,
                           DWORD cbBuf,
                           LPDWORD pcbNeeded) ;

BOOL WINAPI stubEnumPrinters(DWORD Flags,
                             LPTSTR  Name,
                             DWORD Level,
                             LPBYTE  pPrinterEnum,
                             DWORD cbBuf,
                             LPDWORD pcbNeeded,
                             LPDWORD pcReturned) ;

BOOL WINAPI stubAddPrinterDriver(LPTSTR  pName,
                                 DWORD Level,
                                 LPBYTE  pDriverInfo) ;

BOOL WINAPI stubEnumPrinterDrivers(LPTSTR  pName,
                                   LPTSTR  pEnvironment,
                                   DWORD Level,
                                   LPBYTE  pDriverInfo,
                                   DWORD cbBuf,
                                   LPDWORD pcbNeeded,
                                   LPDWORD pcReturned) ;

BOOL WINAPI stubGetPrinterDriver(HANDLE  hPrinter,
                                 LPTSTR  pEnvironment,
                                 DWORD Level,
                                 LPBYTE pDriverInfo,
                                 DWORD cbBuf,
                                 LPDWORD pcbNeeded) ;

BOOL WINAPI stubGetPrinterDriverDirectory(LPTSTR  pName,
                                          LPTSTR  pEnvironment,
                                          DWORD Level,
                                          LPBYTE  pDriverDirectory,
                                          DWORD cbBuf,
                                          LPDWORD pcbNeeded) ;

BOOL WINAPI stubDeletePrinterDriver(LPTSTR pName,
                                    LPTSTR pEnvironment,
                                    LPTSTR pDriverName) ;

BOOL WINAPI stubAddPrintProcessor(LPTSTR  pName,
                                  LPTSTR  pEnvironment,
                                  LPTSTR  pPathName,
                                  LPTSTR  pPrintProcessorName) ;

BOOL WINAPI stubEnumPrintProcessors(LPTSTR  pName,
                                    LPTSTR  pEnvironment,
                                    DWORD Level,
                                    LPBYTE  pPrintProcessorInfo,
                                    DWORD cbBuf,
                                    LPDWORD pcbNeeded,
                                    LPDWORD pcReturned) ;

BOOL WINAPI stubGetPrintProcessorDirectory(LPTSTR  pName,
                                           LPTSTR  pEnvironment,
                                           DWORD Level,
                                           LPBYTE  pPrintProcessorInfo,
                                           DWORD cbBuf,
                                           LPDWORD pcbNeeded) ;

BOOL WINAPI stubDeletePrintProcessor(LPTSTR  pName,
                                     LPTSTR  pEnvironment,
                                     LPTSTR  pPrintProcessorName) ;

BOOL WINAPI stubEnumPrintProcessorDatatypes(LPTSTR  pName,
                                            LPTSTR  pPrintProcessorName,
                                            DWORD Level,
                                            LPBYTE  pDataypes,
                                            DWORD cbBuf,
                                            LPDWORD pcbNeeded,
                                            LPDWORD pcReturned) ;

BOOL WINAPI stubStartPagePrinter(HANDLE  hPrinter) ;

BOOL WINAPI stubWritePrinter(HANDLE  hPrinter,
  LPVOID pBuf,
  DWORD cbBuf,
  LPDWORD pcWritten) ;

BOOL WINAPI stubEndPagePrinter(HANDLE hPrinter) ;

BOOL WINAPI stubAbortPrinter(HANDLE hPrinter) ;

BOOL WINAPI stubReadPrinter(HANDLE hPrinter,
 LPVOID  pBuf,
 DWORD cbBuf,
 LPDWORD pNoBytesRead) ;

BOOL WINAPI stubEndDocPrinter(HANDLE hPrinter) ;

BOOL WINAPI stubAddJob(HANDLE  hPrinter,
  DWORD Level,
  LPBYTE  pData,
  DWORD cbBuf,
  LPDWORD pcbNeeded) ;

BOOL WINAPI stubScheduleJob(HANDLE  hPrinter,
                            DWORD JobId) ;

DWORD WINAPI stubGetPrinterData(HANDLE hPrinter,
                         LPTSTR pValueName,
                         LPDWORD  pType,
                         LPBYTE pData,
                         DWORD nSize,
                         LPDWORD  pcbNeeded);

DWORD WINAPI stubSetPrinterData(HANDLE  hPrinter,
                         LPTSTR  pValueName,
                         DWORD Type,
                         LPBYTE  pData,
                         DWORD cbData);

DWORD WINAPI stubWaitForPrinterChange(HANDLE hPrinter, DWORD Flags);

BOOL WINAPI stubClosePrinter(HANDLE hPrinter) ;

BOOL WINAPI stubAddForm(HANDLE  hPrinter,
                        DWORD Level,
                        LPBYTE  pForm) ;

BOOL WINAPI stubDeleteForm(HANDLE  hPrinter,
LPTSTR  pFormName) ;

BOOL WINAPI stubGetForm(HANDLE  hPrinter,
                        LPTSTR  pFormName,
                        DWORD Level,
                        LPBYTE  pForm,
                        DWORD cbBuf,
                        LPDWORD pcbNeeded) ;

BOOL WINAPI stubSetForm(HANDLE  hPrinter,
  LPTSTR  pFormName,
  DWORD Level,
  LPBYTE  pForm) ;

BOOL WINAPI stubEnumForms(HANDLE  hPrinter,
  DWORD Level,
  LPBYTE  pForm,
  DWORD cbBuf,
  LPDWORD pcbNeeded,
  LPDWORD pcReturned) ;

BOOL WINAPI stubEnumMonitors(LPTSTR  pName,
  DWORD Level,
  LPBYTE  pMonitors,
  DWORD cbBuf,
  LPDWORD pcbNeeded,
  LPDWORD pcReturned) ;

BOOL WINAPI stubEnumPorts(LPTSTR  pName,
  DWORD Level,
  LPBYTE  pPorts,
  DWORD cbBuf,
  LPDWORD pcbNeeded,
  LPDWORD pcReturned) ;

BOOL WINAPI stubAddPort(LPTSTR  pName,
                        HWND hWnd,
                        LPTSTR  pMonitorName) ;

BOOL WINAPI stubConfigurePort(LPTSTR  pName,
                              HWND hWnd,
                              LPTSTR  pPortName) ;

BOOL WINAPI stubDeletePort(LPTSTR  pName,
                           HWND hWnd,
                           LPTSTR  pPortName) ;

HANDLE WINAPI stubCreatePrinterIC(HANDLE  hPrinter,
                           LPDEVMODEW pDevMode);

BOOL WINAPI stubPlayGdiScriptOnPrinterIC(HANDLE  hPrinterIC,
                                         LPBYTE  pIn,
                                         DWORD cIn,
                                         LPBYTE  pOut,
                                         DWORD cOut,
                                         DWORD ul) ;

BOOL WINAPI stubDeletePrinterIC(HANDLE  hPrinterIC) ;

BOOL WINAPI stubAddPrinterConnection(LPTSTR  pName) ;

BOOL WINAPI stubDeletePrinterConnection(LPTSTR pName) ;

DWORD WINAPI stubPrinterMessageBox(HANDLE  hPrinter,
  DWORD Error,
  HWND hWnd,
  LPTSTR  pText,
  LPTSTR  pCaption,
  DWORD dwType);

BOOL WINAPI stubAddMonitor(LPTSTR  pName,
                           DWORD Level,
                           LPBYTE  pMonitorInfo) ;

BOOL WINAPI stubDeleteMonitor(LPTSTR  pName,
                           LPTSTR  pEnvironment,
                           LPTSTR  pMonitorName);


