/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/

/*++

Module Name:

    localmon.h

Abstract:

    Header file for Local Monitor

--*/


// ---------------------------------------------------------------------
// EXTERN VARIABLES
// ---------------------------------------------------------------------
extern  TCHAR FAR *szLocalPort;


// ---------------------------------------------------------------------
// MACRO
// ---------------------------------------------------------------------
#define MSG_ERROR   MB_OK | MB_ICONSTOP
#define MSG_YESNO   MB_YESNO | MB_ICONQUESTION
#define MSG_INFORMATION     MB_OK | MB_ICONINFORMATION

#define TIMEOUT_MIN         1
#define TIMEOUT_MAX       999
#define TIMEOUT_STRING_MAX  3
#define WITHINRANGE( val, lo, hi )  ( ( val <= hi ) && ( val >= lo ) )




typedef struct _INIMONPORT {       /* ipo */
    DWORD       signature;
    DWORD       cb;
    struct _INIMONPORT FAR *pNext;
    DWORD       cRef;
    LPTSTR      pName;

    LPTSTR  pDeviceDesc;
    DWORD   Status;              // see PORT_ manifests
    HANDLE  hFile;
    LPTSTR  pRemoteName;
    HANDLE  hPrinter;
    DWORD   JobId;
    DWORD   txTimeout;          // in milliseconds
    DWORD   dnsTimeout;         // in milliseconds
    DWORD dwRetry;

#ifdef USE_OVERLAPPED_IO
    OVERLAPPED ovWrite;
    OVERLAPPED ovEvent;
    DWORD dwEventMask;
    HANDLE ahEvents[2];
    DWORD cbLastBuffer;
    LPBYTE pLastBuffer;
    LPBYTE pOutBuffer;
    DWORD dwLastCommError;
#endif

} INIMONPORT, FAR *PINIMONPORT;

#define IPO_SIGNATURE   0x4F50  /* 'PO' is the signature value */

#define PP_INSTARTDOC     0x00000001 // To serialize access to the port
#define PP_LPT            0x00000002 //
#define PP_COM            0x00000004 //
#define PP_FILE           0x00000008 //
#define PP_FIRSTWRITE     0x00000010 //

#define PP_HARDWARE       0x00000020 //
#define PP_REMOTE         0x00000040 //
#define PP_CURRENT        0x00000080 //


// ---------------------------------------------------------------------
// FUNCTION PROTOTYPE
// ---------------------------------------------------------------------
BOOL APIENTRY
PortNameDlg(
   HWND   hwnd,
   WORD   msg,
   WPARAM wparam,
   LPARAM lparam
);

BOOL APIENTRY
PrintToFileDlg(
   HWND   hwnd,
   WORD   msg,
   WPARAM wparam,
   LPARAM lparam
);

BOOL APIENTRY
ConfigureLPTPortDlg(
   HWND   hwnd,
   WORD   msg,
   WPARAM wparam,
   LPARAM lparam
);

VOID
EnterMonSem(
   VOID
);

VOID
LeaveMonSem(
   VOID
);


BOOL
WINAPI
LocalmonInitializeMonitorEx(
    LPTSTR  pRegPath,
    LPMONITOR pMonitor
);

BOOL
WINAPI
LocalmonEnumPorts(
    LPTSTR   pName,
    DWORD   Level,
    LPBYTE  pPorts,
    DWORD   cbBuf,
    LPDWORD pcbNeeded,
    LPDWORD pcReturned
);

BOOL
WINAPI
LocalmonOpenPort(
    LPTSTR   pName,
    LPHANDLE pHandle
);

BOOL
WINAPI
LocalmonStartDocPort(
    HANDLE  hPort,
    LPTSTR  pPrinterName,
    DWORD   JobId,
    DWORD   Level,
    LPBYTE  pDocInfo
);

BOOL
WINAPI
LocalmonReadPort(
    HANDLE hPort,
    LPBYTE pBuffer,
    DWORD  cbBuf,
    LPDWORD pcbRead
);

BOOL
WINAPI
LocalmonWritePort(
    HANDLE  hPort,
    LPBYTE  pBuffer,
    DWORD   cbBuf,
    LPDWORD pcbWritten
);

BOOL
WINAPI
LocalmonEndDocPort(
   HANDLE   hPort
);

BOOL
WINAPI
LocalmonClosePort(
    HANDLE  hPort
);

BOOL
WINAPI
LocalmonDeletePort(
    LPTSTR   pName,
    HWND    hWnd,
    LPTSTR   pPortName
);

BOOL
WINAPI
LocalmonAddPort(
    LPTSTR   pName,
    HWND    hWnd,
    LPTSTR   pMonitorName
);

BOOL
WINAPI
LocalmonConfigurePort(
    LPTSTR   pName,
    HWND  hWnd,
    LPTSTR pPortName
);

BOOL
WINAPI
LocalmonGetPrinterDataFromPort
(
    HANDLE  hPort,
    DWORD   ControlID,
    LPTSTR  pValueName,
    LPTSTR  lpInBuffer,
    DWORD   cbInBuffer,
    LPTSTR  lpOutBuffer,
    DWORD   cbOutBuffer,
    LPDWORD lpcbReturned
);

BOOL
WINAPI
LocalmonSetPortTimeOuts
(
    HANDLE  hPort,
    LPCOMMTIMEOUTS lpCTO,
    DWORD   reserved    // must be set to 0
);
