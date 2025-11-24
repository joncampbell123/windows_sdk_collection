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

extern TCHAR FAR *szWinPrint;

typedef struct _PRINTPROCESSORDATA {
    DWORD   signature;
    DWORD   cb;
    struct _PRINTPROCESSORDATA FAR *pNext;
    DWORD   fsStatus;
    HANDLE  semPaused;
    DWORD   uDatatype;
    HANDLE  hPrinter;
    LPTSTR  pPrinterName;

} PRINTPROCESSORDATA, *PPRINTPROCESSORDATA;

#define PRINTPROCESSORDATA_SIGNATURE    0x5051  /* 'QP' is the signature value */

/* Define flags for fsStatus field */

#define PRINTPROCESSOR_ABORTED      0x00000001
#define PRINTPROCESSOR_PAUSED       0x00000002
#define PRINTPROCESSOR_CLOSED       0x00000004

// Flags for banner type

#define BANNER_SIMPLE   1
#define BANNER_FULL     2
#define BANNER_CUSTOM   3


PPRINTPROCESSORDATA
ValidateHandle(
    HANDLE  hPrintProcessor
);

BOOL
WINAPI
InitializePrintProcessor(
    LPPRINTPROCESSOR    pPrintProcessor,
    DWORD           cbPrintProcessor
);

BOOL
WINAPI
WinprintEnumPrintProcessorDatatypes(
    LPTSTR  pName,
    LPTSTR  pPrintProcessorName,
    DWORD   Level,
    LPSTR   pDatatypes,
    DWORD   cbBuf,
    LPDWORD pcbNeeded,
    LPDWORD pcReturned
);

HANDLE
WINAPI
WinprintOpenPrintProcessor(
    LPTSTR   pPrinterName
);

BOOL
WINAPI
WinprintPrintDocumentOnPrintProcessor(
    HANDLE  hPrintProcessor,
    LPPRINTPROCESSORDOCUMENTDATA    lpDoc
);

BOOL
WINAPI
WinprintClosePrintProcessor(
    HANDLE  hPrintProcessor
);

BOOL
WINAPI
WinprintControlPrintProcessor(
    HANDLE  hPrintProcessor,
    DWORD   Command,
    DWORD   JobID,
    LPTSTR  pDatatype,
    LPTSTR  pSpoolFile
);

BOOL
WINAPI
WinprintInstallPrintProcessor(
    HWND    hWnd
);


BOOL
InsertBannerPage(
    HANDLE hPrinter,
    DWORD dwJobId,
    LPSTR pOutputFile,
    DWORD iBannerType,
    LPSTR pSepFile
);
