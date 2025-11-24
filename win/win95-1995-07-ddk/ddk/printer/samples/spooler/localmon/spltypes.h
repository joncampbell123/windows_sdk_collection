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

#ifdef DBG
#define MYDEBUG printf
#else
#define MYDEBUG //
#endif

typedef HANDLE SEM;

typedef struct _KEYDATA {
    DWORD   cb;
    DWORD   cTokens;
    LPTSTR  pTokens[1];
} KEYDATA, FAR *PKEYDATA;

typedef struct _INIENTRY {
    DWORD       signature;
    DWORD       cb;
    struct _INIENTRY FAR *pNext;
    DWORD       cRef;
    LPTSTR      pName;
} INIENTRY, FAR *PINIENTRY;

typedef struct _INIPRINTPROC {             /* iqp */
    DWORD       signature;
    DWORD       cb;
    struct _INIPRINTPROC FAR *pNext;
    DWORD       cRef;
    LPTSTR      pName;
    LPTSTR      pDLLName;
    DWORD       cbDatatypes;
    DWORD       cDatatypes;
    LPTSTR      pDatatypes;
    HANDLE      hLibrary;
    PRINTPROCESSOR  fn;
} INIPRINTPROC, FAR *PINIPRINTPROC;

                            

typedef struct _INIDRIVER {            /* id */
    DWORD       signature;
    DWORD       cb;
    struct _INIDRIVER FAR *pNext;
    DWORD       cRef;
    LPTSTR      pName;
    LPTSTR      pDriverFile;
    LPTSTR      pConfigFile;
    LPTSTR      pDataFile;
    DWORD       cbDependentFiles;
    LPTSTR      pDependentFiles; // c:\drivers\PSPCRIPTUI.HLP\0c:\drivers\PSTEST.TXT\0\0
    LPTSTR      pMonitorName;   // if 0, default to LocalMon
    LPTSTR      pDefaultDataType;
    LPTSTR      pHelpFile;
    DWORD       cVersion;
} INIDRIVER, FAR *PINIDRIVER;


typedef struct _INIPRINTER {    /* ip */
    DWORD       signature;
    DWORD       cb;
    struct _INIPRINTER FAR *pNext;  // chain all the printers together
    DWORD       cRef;

    LPTSTR      pName;
    struct _INIPRINTER FAR *pSamePort;  // printers connected to the same
                                    // physical port Chicago - 9/20/93
    LPTSTR      pShareName;
    struct _INIPORT FAR *pIniPort;  // Chicago - added 9/17/93

    struct _INIMONITOR FAR *pIniLangMonitor;        // Chicago - added 9/17/93
    PINIPRINTPROC pIniPrintProc;
    LPTSTR      pDatatype;
    LPTSTR      pParameters;

    LPTSTR      pComment;
    PINIDRIVER  pIniDriver;
    DWORD       cbDevMode;
    LPDEVMODE   pDevMode;

    DWORD       Priority;       /* queue priority (lowest:1 - highest:9) */
    DWORD       DefaultPriority;
    DWORD       StartTime;      /* print daily after time: from 00:00 in min */
    DWORD       UntilTime;      /* print daily until time: from 00:00 in min */
                                /* uUntil is always bigger than uAfter */
    LPTSTR      pSepFile;     /* full path to separator file, null = def */
    DWORD       Status;         /* QMPAUSE/ERROR/PENDING */
    LPTSTR      pLocation;
    DWORD       Attributes;

    DWORD       cJobs;
    DWORD       AveragePPM;
    DWORD       LastTry;         /* Time stamp of the last try */
    struct _INIJOB FAR *pIniFirstJob;

    DWORD       dwInternal;
    DWORD       dnsTimeout;     /* in milliseconds */
    DWORD       txTimeout;      /* in milliseconds */
} INIPRINTER, FAR *PINIPRINTER;

typedef struct _INIMONITOR {       /* imo */
    DWORD   signature;
    DWORD   cb;
    struct  _INIMONITOR FAR *pNext;
    DWORD   cRef;
    LPTSTR  pName;
    LPTSTR  pMonitorDll;
    HANDLE  hMonitorModule;
    HANDLE  hMonitorPort;
    MONITOR fn;
} INIMONITOR, FAR *PINIMONITOR;


typedef struct _INIPORT {       /* ipo */
    DWORD       signature;
    DWORD       cb;
    struct _INIPORT FAR *pNext;
    DWORD       cRef;

    LPTSTR      pName;
    PINIPRINTER pIniPrinterCurrent;    // The printer we are using to print
    PINIMONITOR pIniMonitorCurrent;    // Monitor with OpenPort
    HANDLE  hProc;          /* Handle to Queue Processor */

    DWORD   Status;              // see PORT_ manifests
    HANDLE  Semaphore;           // Port Thread will sleep on this
    struct  _INIJOB FAR *pIniJob;     // Current Job
    HANDLE  Ready;

    HANDLE  hPort;
    PINIPRINTER pIniPrinter;    // linked list of printers on this port
    DWORD   cPrinters;          // number of printers connected to this port
    DWORD   cMonitors;          // number of monitors that support this port

    DWORD   cMaxMonitors;       // number of entries allocated for rgpMonitors
    PINIMONITOR far *ppIniMonitor;// pointer to array of monitors
                                // connected to to this port
    PINIDRIVER  pIniDriverLoaded;   // the driver loaded by port thread (if any)
    HANDLE      hDriverLoaded;
} INIPORT, FAR *PINIPORT;

#define IPO_SIGNATURE   0x4F50  /* 'PO' is the signature value */

#define PP_PAUSED         0x00000001
#define PP_WAITING        0x00000002
#define PP_RUNTHREAD      0x00000004  /* port thread should be running */
#define PP_THREADRUNNING  0x00000008  /* port thread are running */
#define PP_RESTART        0x00000010
#define PP_MONITOR        0x00000020  // There is a Monitor handling this
#define PP_NETSTARTDOC  0x00000040  // We had done a startdoc on the network handle
#define PP_NET          0x00000080  // Net Port

#define IFO_SIGNATURE   0x4650  /* 'FO' is the signature value */

#define FORM_USERDEFINED  0x0000
