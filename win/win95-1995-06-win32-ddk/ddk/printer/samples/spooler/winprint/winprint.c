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

#define TIMING

#include <windows.h>
#include <winspool.h>
#include <winsplp.h>

#include "local.h"
#include "winprint.h"

TCHAR FAR *szWinPrint = TEXT("WinPrint");
LPTSTR  Datatypes[]={"RAW", "EMF", 0};
LPTSTR  pSimple = NULL;
LPTSTR  pFull = NULL;


HDC WINAPI gdiPlaySpoolStream(LPSTR, LPSTR, LPSTR, DWORD, LPDWORD, HDC);

#define PRINTPROCESSOR_TYPE_RAW     0
#define PRINTPROCESSOR_TYPE_EMF     1
#define PRINTPROCESSOR_TYPE_NUM     2

#ifdef TIMING
HWND hWndBench = 0;
#endif


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
BOOL
WINAPI
InitializePrintProcessor (
    LPPRINTPROCESSOR    pPrintProcessor,
    DWORD           cbPrintProcessor
)
{
    char szBuf[MAX_PATH];

    pPrintProcessor->fpEnumDatatypes = WinprintEnumPrintProcessorDatatypes;
    pPrintProcessor->fpOpenPrintProcessor = WinprintOpenPrintProcessor;
    pPrintProcessor->fpPrintDocument = WinprintPrintDocumentOnPrintProcessor;
    pPrintProcessor->fpClosePrintProcessor = WinprintClosePrintProcessor;
    pPrintProcessor->fpControlPrintProcessor = WinprintControlPrintProcessor;

    if (LoadString(hInst, IDS_BANNERSIMPLE, szBuf, sizeof(szBuf)))
        pSimple = AllocSplStr(szBuf);

    if (LoadString(hInst, IDS_BANNERFULL, szBuf, sizeof(szBuf)))
        pFull = AllocSplStr(szBuf);

    return TRUE;
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
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
)
{
    DATATYPES_INFO_1    FAR *pInfo1 = (DATATYPES_INFO_1 FAR *)pDatatypes;
    LPTSTR   FAR *pMyDatatypes = Datatypes;
    DWORD   cbTotal=0;
    LPBYTE  pEnd;

    *pcReturned = 0;

    pEnd = (LPBYTE)pInfo1 + cbBuf;

    while (*pMyDatatypes) {

        cbTotal += wcslen(*pMyDatatypes)*sizeof(TCHAR) + sizeof(TCHAR) +
                   sizeof(DATATYPES_INFO_1);

        pMyDatatypes++;
    }

    *pcbNeeded = cbTotal;

    if (cbTotal <= cbBuf) {

        pMyDatatypes = Datatypes;

        while (*pMyDatatypes) {

            pEnd -= wcslen(*pMyDatatypes)*sizeof(TCHAR) + sizeof(TCHAR);
            wcscpy((LPTSTR)pEnd, *pMyDatatypes);
            pInfo1->pName = (LPTSTR)pEnd;
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


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
HANDLE
WINAPI
WinprintOpenPrintProcessor(
    LPTSTR   pPrinterName
)
{
    PPRINTPROCESSORDATA pData;
    HANDLE  hPrinter=0;
    HDC     hDC=0;

    if (!OpenPrinter(pPrinterName, &hPrinter, NULL))
        return FALSE;

    pData = (PPRINTPROCESSORDATA)AllocSplMem(sizeof(PRINTPROCESSORDATA));

    pData->cb          = sizeof(PRINTPROCESSORDATA);
    pData->signature   = PRINTPROCESSORDATA_SIGNATURE;
    pData->hPrinter    = hPrinter;
    pData->semPaused   = CreateEvent(NULL, FALSE, TRUE,NULL);
    pData->pPrinterName = AllocSplStr(pPrinterName);

    return (HANDLE)pData;
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
UINT ValidateDatatype(LPTSTR pDatatype)
{
    LPTSTR   FAR *pMyDatatypes=Datatypes;
    DWORD   uDatatype=0;

    while (*pMyDatatypes && wcscmp(*pMyDatatypes, pDatatype))
    {
        pMyDatatypes++;
        uDatatype++;
    }
    return uDatatype;
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
DWORD
ValidateBannerType(LPTSTR lpBanner)
{
    if (lpBanner && *lpBanner)
    {
        if (!wcscmp(lpBanner, pFull))
            return BANNER_FULL;
        if (!wcscmp(lpBanner, pSimple))
            return BANNER_SIMPLE;

#ifdef TIMING
        if (!wcscmp(lpBanner, "Bench"))
        {
            hWndBench = FindWindow("BNCH32PRT", "PRTWIN");

            if (hWndBench)
                SendMessage(hWndBench, WM_USER + 801, 0, 0);

            return 0;
        }
        else
            hWndBench = 0;
#endif

        return BANNER_CUSTOM;
    }

    return 0;
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
BOOL
WINAPI
WinprintPrintDocumentOnPrintProcessor(
    HANDLE  hPrintProcessor,
    LPPRINTPROCESSORDOCUMENTDATA    lpDoc
)
{
    PPRINTPROCESSORDATA pData;
    DOC_INFO_2 DocInfo;
    DWORD   LastError = 0;
    DWORD   NoRead, NoWritten;
    DWORD   iBannerType;
    BYTE    Buffer[4096];
    HANDLE  hPrinterRead;
    HDC hDC = NULL;
    LPBYTE pReadBuf;
    DWORD cbReadBuf;
    DWORD NoLeftOver = 0;
    BOOL ret = TRUE;

    if (!(pData = ValidateHandle(hPrintProcessor)))
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    pData->uDatatype = ValidateDatatype(lpDoc->pDatatype);

    iBannerType = ValidateBannerType(lpDoc->pSepFile);

    if (iBannerType)
        InsertBannerPage(pData->hPrinter, lpDoc->JobId, lpDoc->pOutputFile, iBannerType, lpDoc->pSepFile);

    DocInfo.pDocName = lpDoc->pDocumentName;
    DocInfo.pOutputFile = lpDoc->pOutputFile;   // the spool file
    DocInfo.pDatatype = lpDoc->pDatatype;
    DocInfo.JobId = lpDoc->JobId;

    // open the same printer for reading the spool file.
    if (!OpenPrinter(pData->pPrinterName, &hPrinterRead, NULL))
        return FALSE;

    // This makes ReadPrinter() read the spool file for us.
    DocInfo.dwMode = DI_READ_SPOOL_JOB;
    if (!StartDocPrinter(hPrinterRead, 2, (LPBYTE)&DocInfo))
    {
        LastError = GetLastError();
        ret = FALSE;
        goto Exit_2;
    }

    if (pData->uDatatype == PRINTPROCESSOR_TYPE_RAW)
    {
        // Start direct write to port
        DocInfo.dwMode = DI_CHANNEL_WRITE;
        if (!StartDocPrinter(pData->hPrinter, 2, (LPBYTE)&DocInfo))
        {
            // SetJob(pData->hPrinter, lpDoc->JobId, 0, NULL, JOB_CONTROL_CANCEL);
            LastError = GetLastError();
            ret = FALSE;
            goto Exit_1;
        }
    }

    pReadBuf = (LPBYTE)Buffer;
    cbReadBuf = sizeof(Buffer);


    // Here ReadPrinter() is used to actually read 4K of the spool file.
    // This data is then sent either to the printer directly if RAW,
    // or to GDI if EMF. In the later case, the metafile data is played back
    // on the printer DC and then sent to the printer (all this is done by gdiPlaySpoolStream()).
    while (((ReadPrinter(hPrinterRead, pReadBuf, cbReadBuf, &NoRead)) &&
        NoRead) || NoLeftOver)
    {
        // gdiPlaySpoolStream now plays one page at a time.
        // So playing back EMF gets opportunity to pause on every page.

        if (pData->fsStatus & PRINTPROCESSOR_PAUSED)
            WaitForSingleObject(pData->semPaused, INFINITE);

        if (pData->fsStatus & PRINTPROCESSOR_ABORTED)
        {
            // we can not just break for EMF
            // we need to clean up the DC and etc.

            if (pData->uDatatype == PRINTPROCESSOR_TYPE_EMF)
                gdiPlaySpoolStream(NULL, NULL, lpDoc->pSpoolFileName, 0, 0, hDC);

            break;
        }

        // check if RAW or EMF, and send it to the right place accordingly.
        if (pData->uDatatype == PRINTPROCESSOR_TYPE_RAW)
            WritePrinter(pData->hPrinter, Buffer, NoRead, &NoWritten);
        else
        {
            NoRead += NoLeftOver;
            NoWritten = NoRead;

            SetLastError(ERROR_SUCCESS);

            // hDC is NULL the first time we get here
            // This is where the metafile is played back.
            hDC = gdiPlaySpoolStream(
                pData->pPrinterName, lpDoc->pOutputFile,
                Buffer, lpDoc->JobId, &NoRead, hDC);

            // Upon return, NoRead is the number of bytes that are processed
            // in the previous buffer. And it must be no greater than NoWritten.

            if (hDC && (NoWritten >= NoRead))
            {
                NoLeftOver = NoWritten - NoRead;

                // there may be an incomplete sp block at the end that
                // wasn't processed and we need to carry over

                if (NoLeftOver)
                    CopyMemory(Buffer, Buffer + NoRead, NoLeftOver);

                pReadBuf = Buffer + NoLeftOver;
                cbReadBuf = sizeof(Buffer) - NoLeftOver;
            }
            else
            {
                // we failed
                // delete the ~EMF????.TMP ???
                // or do we want to leave the EMF around
                // and allow user to retry ???

                LastError = GetLastError();

                DBGMSG(DBG_ERROR, ("WinprintPrintDoc: gdiPlaySpoolStream failed %d\n", LastError));

                // prompt user for retry/cancel/ok ?

                gdiPlaySpoolStream(NULL, NULL, lpDoc->pSpoolFileName, 0, 0, 0);
                ret = FALSE;

                break;
            }
        }
    }

    if (pData->uDatatype == PRINTPROCESSOR_TYPE_RAW)
        EndDocPrinter(pData->hPrinter);
Exit_1:
    EndDocPrinter(hPrinterRead);
Exit_2:
    ClosePrinter(hPrinterRead);

    if (LastError)
        SetLastError(LastError);

#ifdef TIMING
    if (hWndBench)
        SendMessage(hWndBench, WM_USER + 802, 0, 0);
#endif

    return ret;
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
BOOL
WINAPI
WinprintClosePrintProcessor(
    HANDLE  hPrintProcessor
)
{
    PPRINTPROCESSORDATA pData;

    pData = ValidateHandle(hPrintProcessor);

    if (!pData)
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    pData->signature = 0;

    /* Release any allocated resources */

    if (pData->hPrinter)
        ClosePrinter(pData->hPrinter);

    CloseHandle(pData->semPaused);

    if (pData->pPrinterName)
        FreeSplStr(pData->pPrinterName);

    FreeSplMem(pData, pData->cb);

    return TRUE;
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
BOOL
WINAPI
WinprintControlPrintProcessor(
    HANDLE  hPrintProcessor,
    DWORD   Command,
    DWORD   JobID,
    LPTSTR  pDatatype,
    LPTSTR  pSpoolFile
)
{
    PPRINTPROCESSORDATA pData;
    PRINTPROCESSORDATA Data;

    if (hPrintProcessor)
        pData = ValidateHandle(hPrintProcessor);
    else
    {
        if (Command != JOB_CONTROL_CANCEL)
            return FALSE;

        Data.uDatatype = ValidateDatatype(pDatatype);
        if (Data.uDatatype >= 0)
            pData = &Data;
        else
            pData = 0;
    }

    if (pData)
    {
        switch (Command)
        {
        case JOB_CONTROL_PAUSE:

            ResetEvent(pData->semPaused);
            pData->fsStatus |= PRINTPROCESSOR_PAUSED;
            return TRUE;

        case JOB_CONTROL_CANCEL:

            if (!hPrintProcessor)
            {
                // we're deleting a job that hasn't started printing

                if (pData->uDatatype == PRINTPROCESSOR_TYPE_EMF)
                {
                    return (BOOL)gdiPlaySpoolStream(NULL, NULL, pSpoolFile, 0, 0, 0);
                }

                return TRUE;
            }

            pData->fsStatus |= PRINTPROCESSOR_ABORTED;

            /* fall through to release job if paused */

        case JOB_CONTROL_RESUME:

            if (pData->fsStatus & PRINTPROCESSOR_PAUSED)
            {
                pData->fsStatus &= ~PRINTPROCESSOR_PAUSED;
                SetEvent(pData->semPaused);
            }

            return TRUE;

        default:

            break;
        }

    }

    return FALSE;
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
BOOL
WINAPI
WinprintInstallPrintProcessor(
    HWND    hWnd
)
{
    return TRUE;
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
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
