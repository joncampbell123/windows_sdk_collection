#include <windows.h>
#include <winspool.h>
#include <winsplp.h>

#include "winprint.h"

#include <excpt.h>
#include <string.h>

#define PRINTPROCESSOR_TYPE_RAW     0
#define PRINTPROCESSOR_TYPE_JOURNAL 1
#define PRINTPROCESSOR_TYPE_TEXT    2
#define PRINTPROCESSOR_TYPE_NUM     3

LPWSTR  Datatypes[]={L"RAW", L"NT JNL 1.000", L"TEXT", 0};

LPWSTR
AllocStr(
    LPWSTR  pString
)
{
    LPWSTR  pNewString;

    if (pString) {
        pNewString = LocalAlloc(LMEM_FIXED, (wcslen(pString)+1)*sizeof(WCHAR));
        wcscpy(pNewString, pString);
        return pNewString;
    }

    return NULL;
}

BOOL
EnumPrintProcessorDatatypes(
    LPWSTR  pName,
    LPWSTR  pPrintProcessorName,
    DWORD   Level,
    LPBYTE  pDatatypes,
    DWORD   cbBuf,
    LPDWORD pcbNeeded,
    LPDWORD pcReturned
)
{
    DATATYPES_INFO_1    *pInfo1 = (DATATYPES_INFO_1 *)pDatatypes;
    LPWSTR   *pMyDatatypes = Datatypes;
    DWORD   cbTotal=0;
    LPBYTE  pEnd;

    *pcReturned = 0;

    pEnd = (LPBYTE)pInfo1 + cbBuf;

    while (*pMyDatatypes) {

        cbTotal += wcslen(*pMyDatatypes)*sizeof(WCHAR) + sizeof(WCHAR) +
                   sizeof(DATATYPES_INFO_1);

        pMyDatatypes++;
    }

    *pcbNeeded = cbTotal;

    if (cbTotal <= cbBuf) {

        pMyDatatypes = Datatypes;

        while (*pMyDatatypes) {

            pEnd -= wcslen(*pMyDatatypes)*sizeof(WCHAR) + sizeof(WCHAR);
            wcscpy((LPWSTR)pEnd, *pMyDatatypes);
            pInfo1->pName = (LPWSTR)pEnd;
            pInfo1++;
            (*pcReturned)++;

            pMyDatatypes++;
        }

    } else {

        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return FALSE;
    }

    return TRUE;
}

HANDLE
OpenPrintProcessor(
    LPWSTR   pPrinterName,
    PPRINTPROCESSOROPENDATA pPrintProcessorOpenData
)
{
    PPRINTPROCESSORDATA pData;
    LPWSTR   *pMyDatatypes=Datatypes;
    DWORD   uDatatype=0;
    HANDLE  hPrinter=0;
    HDC     hDC;

    while (*pMyDatatypes && wcscmp(*pMyDatatypes,
                                    pPrintProcessorOpenData->pDatatype)) {
        pMyDatatypes++;
        uDatatype++;
    }

    switch (uDatatype) {

    case 0:
        if (!OpenPrinter(pPrinterName, &hPrinter, NULL))
            return FALSE;
        break;

    case 1:
        if (!(hDC = CreateDC(L"", pPrinterName, L"",
                             pPrintProcessorOpenData->pDevMode)))
            return FALSE;
        break;

    default:
        SetLastError(ERROR_INVALID_DATATYPE);
        return FALSE;
    }

    pData = (PPRINTPROCESSORDATA)LocalAlloc(LPTR, sizeof(PRINTPROCESSORDATA));

    pData->cb          = sizeof(PRINTPROCESSORDATA);
    pData->signature   = PRINTPROCESSORDATA_SIGNATURE;
    pData->JobId       = pPrintProcessorOpenData->JobId;
    pData->hPrinter    = hPrinter;
    pData->semPaused   = CreateEvent(NULL, FALSE, TRUE,NULL);
    pData->uDatatype   = uDatatype;
    pData->hDC         = hDC;

    pData->pPrinterName = AllocStr(pPrinterName);
    pData->pDatatype = AllocStr(pPrintProcessorOpenData->pDatatype);
    pData->pDocument = AllocStr(pPrintProcessorOpenData->pDocumentName);
    pData->pParameters = AllocStr(pPrintProcessorOpenData->pParameters);

    return (HANDLE)pData;
}

BOOL
PrintDocumentOnPrintProcessor(
    HANDLE  hPrintProcessor,
    LPWSTR   pDocumentName
)
{
    PPRINTPROCESSORDATA pData;
    DOC_INFO_1 DocInfo;
    DWORD   rc;
    DWORD   NoRead, NoWritten;
    HANDLE  hPrinter;
    BYTE    Buffer[4096];

    if (!(pData = ValidateHandle(hPrintProcessor))) {

        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    if (pData->uDatatype == 1) {

        try {
                                             //!!! BUG BUG BUG - LPCWSTR
            return GdiPlayJournal(pData->hDC, (LPCSTR)pDocumentName, 0, -1);

        } except (TRUE) {

            OutputDebugString(L"GdiPlayJournal gave an exception\n");

            return FALSE;
        }
    }

    if (!OpenPrinter(pDocumentName, &hPrinter, NULL)) {
        return FALSE;
    }

    DocInfo.pDocName = pData->pDocument;
    DocInfo.pOutputFile = 0;
    DocInfo.pDatatype = pData->pDatatype;

    if (!StartDocPrinter(pData->hPrinter, 1, (LPBYTE)&DocInfo)) {
        ClosePrinter(hPrinter);
        return FALSE;
    }

    while ((rc = ReadPrinter(hPrinter, Buffer, sizeof(Buffer), &NoRead)) &&
           NoRead) {

        if (pData->fsStatus & PRINTPROCESSOR_PAUSED)
            WaitForSingleObject(pData->semPaused, INFINITE);

        if (pData->fsStatus & PRINTPROCESSOR_ABORTED) {
            break;
        }

        WritePrinter(pData->hPrinter, Buffer, NoRead, &NoWritten);
    }

    EndDocPrinter(pData->hPrinter);

    ClosePrinter(hPrinter);

    return TRUE;
}

BOOL
ClosePrintProcessor(
    HANDLE  hPrintProcessor
)
{
    PPRINTPROCESSORDATA pData;

    pData = ValidateHandle(hPrintProcessor);

    if (!pData) {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    pData->signature = 0;

    /* Release any allocated resources */

    if (pData->hPrinter)
        ClosePrinter(pData->hPrinter);

    if (pData->hDC)
        DeleteDC(pData->hDC);

    CloseHandle(pData->semPaused);

    if (pData->pPrinterName)
        LocalFree(pData->pPrinterName);

    if (pData->pDatatype)
        LocalFree(pData->pDatatype);

    if (pData->pDocument)
        LocalFree(pData->pDocument);

    if (pData->pParameters)
        LocalFree(pData->pParameters);

    LocalFree(pData);

    return TRUE;
}

BOOL
ControlPrintProcessor(
    HANDLE  hPrintProcessor,
    DWORD   Command
)
{
    PPRINTPROCESSORDATA pData;

    if (pData = ValidateHandle(hPrintProcessor)) {

        switch (Command) {

        case JOB_CONTROL_PAUSE:

            ResetEvent(pData->semPaused);
            pData->fsStatus |= PRINTPROCESSOR_PAUSED;
            return TRUE;
            break;

        case JOB_CONTROL_CANCEL:

            pData->fsStatus |= PRINTPROCESSOR_ABORTED;

            if (pData->uDatatype == PRINTPROCESSOR_TYPE_JOURNAL)
                CancelDC(pData->hDC);

            /* fall through to release job if paused */

        case JOB_CONTROL_RESUME:

            if (pData->fsStatus & PRINTPROCESSOR_PAUSED) {

                SetEvent(pData->semPaused);
                pData->fsStatus &= ~PRINTPROCESSOR_PAUSED;
            }

            return TRUE;
            break;

        default:

            return FALSE;
            break;
        }

    }

    return FALSE;
}

BOOL
InstallPrintProcessor(
    HWND    hWnd
)
{
    MessageBox(hWnd, L"WinPrint", L"Print Processor Setup", MB_OK);

    return TRUE;
}

PPRINTPROCESSORDATA
ValidateHandle(
    HANDLE  hQProc
)
{
    PPRINTPROCESSORDATA pData = (PPRINTPROCESSORDATA)hQProc;

    if (pData && pData->signature == PRINTPROCESSORDATA_SIGNATURE)
        return( pData );

    return( NULL );
}
