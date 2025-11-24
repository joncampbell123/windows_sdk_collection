/*****************************************************************/ 
/**               Microsoft Windows 95                          **/
/**           Copyright (C) Microsoft Corp., 1991-1995          **/
/*****************************************************************/ 

/* MSPP.H
 *   
 * Header file for W4W print provider.  Contains prototypes for all the
 * required print provider functions for Chicago.
 *
 */      
 
#ifndef __mspp_h__
#define __mspp_h__

// Handy stuff taken from NPDEFS.H
//
#ifndef RC_INVOKED

#pragma warning(disable:4147)   // warning about ignoring __loadds on function
                                // ptr decls, of which there are 5 in windows.h
#pragma warning(disable:4118)   // warning about not accepting the intrinsic function pragma
                                // during a fast compile

// Macro to quiet compiler for an unused formal parameter.
#define UNUSED(x) ((void)(x))

// Macros to define process local storage:
#define PROCESS_LOCAL_BEGIN data_seg(".PrcLcl","INSTANCE")
#define PROCESS_LOCAL_END data_seg()

extern "C" {

#endif  // RC_INVOKED

#include <windows.h>
#include <winspool.h>
#include <netcons.h>

#ifndef RC_INVOKED
};
#endif

#include "pptypes.h"
#include "ppdebug.h"

//
// Global variables

// Environment string for this version of Windows
//
extern char PROVIDER_NAME[];
extern char ENVIRONMENT_STRING[];
extern char EMPTY_STRING[];
extern char BACKSLASH[];
#define EMPTY_STRING_SIZE 1

// Prototypes for functions in PPJOBS.C (Functions that manipulate print
// jobs)
//
BOOL WINAPI PPAddJob(HANDLE hPrinter,
                     DWORD Level,
                     LPBYTE pData,
                     DWORD dbBuf,
                     LPDWORD pcbNeeded) ;

BOOL WINAPI PPEnumJobs(HANDLE hPrinter,
                       DWORD FirstJob,
                       DWORD NoJobs,
                       DWORD Level,
                       LPBYTE pJob,
                       DWORD cbBuf,
                       LPDWORD pcbNeeded,
                       LPDWORD pcbReturned) ; 
              
BOOL WINAPI PPGetJob(HANDLE hPrinter,
                     DWORD JobId,
                     DWORD Level,
                     LPBYTE pJob,
                     DWORD cbBuf,
                     LPDWORD pcbNeeded) ; 
            
BOOL WINAPI PPSetJob(HANDLE hPrinter,
                     DWORD JobId,
                     DWORD Level,
                     LPBYTE pJob,
                     DWORD Command) ; 
            
BOOL WINAPI PPScheduleJob(HANDLE hPrinter,
                          DWORD JobId) ;                                    
                   
//
// Prototypes for functions in PPINFO.C (Functions that provide information
// on printers.)
//     
BOOL WINAPI PPEnumPrinters(DWORD Type,
                           LPTSTR Name,
                           DWORD Level,
                           LPBYTE pPrinterEnum,
                           DWORD cbBuf,
                           LPDWORD pcbNeeded,
                           LPDWORD pcbReturned) ;

BOOL WINAPI PPGetPrinter(HANDLE hPrinter,
                         DWORD Level,
                         LPBYTE pPrinter,
                         DWORD cbBuf,
                         LPDWORD pcbNeeded) ;
                  
BOOL WINAPI PPSetPrinter(HANDLE hPrinter,
                         DWORD Level,
                         LPBYTE pPrinter,
                         DWORD Command) ;                                    
                  
//
// Prototypes for functions in PPPRN.C (Functions that control the printer
// during the course of a single job.)
//                  

BOOL WINAPI PPAbortPrinter(HANDLE hPrinter) ;              

BOOL WINAPI PPReadPrinter(HANDLE hPrinter,
                          LPVOID pBuf,
                          DWORD cbBuf,
                          LPDWORD pNoBytesRead) ;
                   
BOOL WINAPI PPClosePrinter(HANDLE hPrinter) ;

BOOL WINAPI PPOpenPrinter(LPTSTR pPrinterName,
                          LPHANDLE phPrinter,
                          LPPRINTER_DEFAULTS pDefaults);
                                                 
                                                 
BOOL WINAPI PPEndDocPrinter(HANDLE hPrinter) ;

DWORD WINAPI PPStartDocPrinter(HANDLE hPrinter,
                              DWORD Level,
                              LPBYTE pBuffer);
                       
BOOL WINAPI PPEndPagePrinter(HANDLE hPrinter) ;                                                                        
                                        
BOOL WINAPI PPStartPagePrinter(HANDLE hPrinter); 

BOOL WINAPI PPWritePrinter(HANDLE hPrinter,
                           LPVOID pBuf,
                           DWORD cbBuf, 
                           LPDWORD pcWritten); 
                    
//
// Prototypes for functions in PPDRVR.C
//                                                          
BOOL WINAPI PPGetPrinterDriver(HANDLE hPrinter,
                               LPTSTR pEnvironment,
                               DWORD Level,
                               LPBYTE pDriverInfo,
                               DWORD cbBuf,
                               LPDWORD pcbNeeded) ;
                        
BOOL WINAPI PPGetPrinterDriverDirectory(LPTSTR pName,
                                        LPTSTR pEnvironment,
                                        DWORD Level,
                                        LPBYTE pDriverDirectory,
                                        DWORD cbBuf,
                                        LPDWORD pcbNeeded);
                                 
BOOL WINAPI PPGetPrintProcessorDirectory(LPTSTR pName,
                                         LPTSTR pEnvironment,
                                         DWORD Level,
                                         LPBYTE pPrintProcessorInfo,
                                         DWORD cbBuf,
                                         LPDWORD pcbNeeded);

//
// Prototypes for functions in PPPORTS.C
//
BOOL WINAPI PPEnumPorts(LPSTR  lpName,
                        DWORD  Level,
                        LPBYTE lpPorts,
                        DWORD  cbBuf,
                        LPDWORD pcbNeeded,
                        LPDWORD pcbReturned);

//
// Prototypes for functions in PPUTIL.C
//
void PPInitCritical();
void PPDeleteCritical();
void PPEnterCritical();
void PPLeaveCritical();

LPVOID PPAllocMem(DWORD dwCount) ;
BOOL PPValidateMem(LPVOID pMem,DWORD cb) ;
BOOL PPFreeMem(LPVOID pMem, DWORD  cb ) ;
LPSTR PPAllocString(LPSTR pStr) ;
BOOL PPFreeString(LPSTR pStr) ;
BOOL PPInitHeap();

int lstrsize(LPSTR string);
void nls_strcpyn(LPSTR dest,LPSTR src,DWORD n);
LPSTR nls_strchr(LPSTR lpString,TCHAR ch);
BOOL is_unc_name(LPSTR lpName);

BOOL ResolveDeviceName(LPSTR lpInName,LPSTR lpServer, LPSTR lpDevice);
void PPSetString(LPSTR *dest,LPSTR src,LPSTR *buf);
void PPCopyString(LPSTR dest,LPSTR src);
void PPCopyMem(LPSTR *dest,LPSTR src,DWORD cbSize,LPSTR *buf);

#define EndOfBuffer(buf,length) (((LPSTR) buf) + (length - 1))

#ifdef DEBUG

BOOL _pp_mem_leak_check();
#define PPMemLeakCheck() _pp_mem_leak_check();

#else

#define PPMemLeakCheck()

#endif

#endif   // __mspp_h__ 
