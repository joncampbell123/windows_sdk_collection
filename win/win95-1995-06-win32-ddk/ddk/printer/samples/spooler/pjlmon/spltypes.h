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

typedef HANDLE SEM;

typedef struct _INIENTRY {
    DWORD       signature;
    DWORD       cb;
    struct _INIENTRY FAR *pNext;
    DWORD       cRef;
    LPTSTR      pName;
} INIENTRY, FAR *PINIENTRY;

typedef struct _INIPORT {       /* ipo */
    DWORD   signature;
    DWORD   cb;
    struct  _INIPORT FAR *pNext;
    DWORD   cRef;

    LPTSTR  pName;
    DWORD   status;
    LPTSTR  pPrinterName;
    DWORD   JobId;

    HANDLE  hPort;
    HANDLE  hPrinter;
    HANDLE  WakeUp;
    HANDLE  DoneReading;

    DWORD   PrinterStatus;
    DWORD   TimeoutCount;
    DWORD   dwAvailableMemory;
    DWORD   dwInstalledMemory;

    MONITOR fn;

} INIPORT, FAR *PINIPORT;


#define IPO_SIGNATURE   0x4F50  /* 'PO' is the signature value */

#define PP_INSTARTDOC       0x00000001  // Inside StartDoc, sending data to the printer
#define PP_RUN_THREAD       0x00000002  // Tell the monitor thread to start running
#define PP_THREAD_RUNNING   0x00000004  // Tell the main thread that the monitor thread is running
#define PP_PRINTER_OFFLINE  0x00000008  // The printer is OFFLINE
#define PP_PJL_SENT         0x00000010  // PJL Command was sent to the printer
#define PP_SEND_PJL         0x00000020  // Set at StartDoc so that we initialize PJL
                                        // commands during the first write port
#define PP_PORT_OPEN        0x00000040  // Port is open, for read thread
#define PP_IS_PJL           0x00000080  // Port is PJL
#define PP_DONT_TRY_PJL     0x00000100  // Don't try again...
#define PP_RESETDEV         0x00000200  // Device has just been reset, job aborted
#define PP_HPLJ4L           0x00000400  // Special case for HP LaserJet 4L

// PP_PJL_SENT, PP_SEND_PJL, PP_IS_PJL, PP_PORT_OPEN are set/cleared on
//           per job basis.
// PP_DONT_TRY_PJL is set/cleared on per printer basis.

#define FindPort(psz)      (PINIPORT   )FindIniKey((PINIENTRY)pIniFirstPort, (LPTSTR)(psz))
