typedef struct _PRINTPROCESSORDATA {
    DWORD   signature;
    DWORD   cb;
    struct _PRINTPROCESSORDATA *pNext;
    DWORD   fsStatus;
    HANDLE  semPaused;
    DWORD   uDatatype;
    HANDLE  hPrinter;
    LPWSTR  pPrinterName;
    LPWSTR  pDocument;
    LPWSTR  pDatatype;
    LPWSTR  pParameters;
    DWORD   JobId;
    HDC     hDC;
} PRINTPROCESSORDATA, *PPRINTPROCESSORDATA;

#define PRINTPROCESSORDATA_SIGNATURE    0x5051  /* 'QP' is the signature value */

/* Define flags for fsStatus field */

#define PRINTPROCESSOR_ABORTED      0x0001
#define PRINTPROCESSOR_PAUSED       0x0002
#define PRINTPROCESSOR_CLOSED       0x0004

#define PRINTPROCESSOR_RESERVED     0xFFF8

PPRINTPROCESSORDATA
ValidateHandle(
    HANDLE  hPrintProcessor
);

