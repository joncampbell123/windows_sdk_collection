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

#define USECOMM

#include <windows.h>
#include <winspool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsplp.h>
#include "spltypes.h"
#include "local.h"
#include "dialogs.h"
#include "parsepjl.h"


// ---------------------------------------------------------------------
// PROTO, CONSTANT, and GLOBAL
//
// ---------------------------------------------------------------------

int ProcessPJLString(PINIPORT, CHAR *, LPDWORD);
VOID ProcessParserError(DWORD);
VOID InterpreteTokens(PINIPORT, PTOKENPAIR, DWORD);
BOOL IsPJL(HANDLE);
BOOL WriteCommand(HANDLE, LPSTR, BOOL);
BOOL ReadCommand(HANDLE);

#define READTHREADTIMEOUT       5000
#define GETDEVICEID             17
#define MAX_DEVID               1024
#define READ_THREAD_ERROR_TIMEOUT       1800000 // 30 min
#define READ_THREAD_ABORT_TIMEOUT       60000   // 1 min
#define READ_THREAD_IDLE_TIMEOUT_4L     180000  // 3 min
#define READ_THREAD_IDLE_TIMEOUT_OTHER  900000  // 15 min

PINIPORT pIniFirstPort = NULL;
CRITICAL_SECTION pjlMonSection = {0,0,0,0,0,0};
DWORD ReadThreadErrorTimeout = READ_THREAD_ERROR_TIMEOUT;
DWORD ReadThreadAbortTimeout = READ_THREAD_ABORT_TIMEOUT;
DWORD ReadThreadIdleTimeout4L = READ_THREAD_IDLE_TIMEOUT_4L;
DWORD ReadThreadIdleTimeoutOther = READ_THREAD_IDLE_TIMEOUT_OTHER;
TCHAR szErrorTimeout[] = "ErrorTimeout";
TCHAR szAbortTimeout[] = "AbortTimeout";
TCHAR szIdleTimeout4L[] = "IdleTimeout4L";
TCHAR szIdleTimeoutOther[] = "IdleTimeoutOther";
LPSTR pRegistryPath = NULL;


// ---------------------------------------------------------------------
// LibMain
//
// ---------------------------------------------------------------------
BOOL
WINAPI
LibMain(
    HANDLE hModule,
    DWORD dwReason,
    LPVOID lpRes
)
{
    return TRUE;
}


// ---------------------------------------------------------------------
// GetTimeoutsFromRegistry
//
// Store timeouts in the registry so user or app can change it if they
// want to.
// 
// ---------------------------------------------------------------------
VOID
GetTimeoutsFromRegistry()
{
    HKEY hKey;

    // get the timeout values from registry
    // or initialize registry if these values don't exist

    if (pRegistryPath &&
        !RegOpenKeyEx(HKEY_LOCAL_MACHINE, pRegistryPath, 0, KEY_READ, &hKey))
    {
        DWORD Size = sizeof(DWORD);

        if (RegQueryValueEx(hKey, szErrorTimeout, NULL, NULL,
                                (LPBYTE)&ReadThreadErrorTimeout, &Size))
        {
            ReadThreadErrorTimeout = READ_THREAD_ERROR_TIMEOUT;
            RegSetValueEx(hKey, szErrorTimeout, 0, REG_DWORD,
                (LPBYTE)&ReadThreadErrorTimeout, sizeof(ReadThreadErrorTimeout));
        }

        Size = sizeof(DWORD);

        if (RegQueryValueEx(hKey, szAbortTimeout, NULL, NULL,
                                (LPBYTE)&ReadThreadAbortTimeout, &Size))
        {
            ReadThreadAbortTimeout = READ_THREAD_ABORT_TIMEOUT;
            RegSetValueEx(hKey, szAbortTimeout, 0, REG_DWORD,
                (LPBYTE)&ReadThreadAbortTimeout, sizeof(ReadThreadAbortTimeout));
        }

        Size = sizeof(DWORD);

        if (RegQueryValueEx(hKey, szIdleTimeout4L, NULL, NULL,
                                (LPBYTE)&ReadThreadIdleTimeout4L, &Size))
        {
            ReadThreadIdleTimeout4L = READ_THREAD_IDLE_TIMEOUT_4L;
            RegSetValueEx(hKey, szIdleTimeout4L, 0, REG_DWORD,
                (LPBYTE)&ReadThreadIdleTimeout4L, sizeof(ReadThreadIdleTimeout4L));
        }

        Size = sizeof(DWORD);

        if (RegQueryValueEx(hKey, szIdleTimeoutOther, NULL, NULL,
                                (LPBYTE)&ReadThreadIdleTimeoutOther, &Size))
        {
            ReadThreadIdleTimeoutOther = READ_THREAD_IDLE_TIMEOUT_OTHER;
            RegSetValueEx(hKey, szIdleTimeoutOther, 0, REG_DWORD,
                (LPBYTE)&ReadThreadIdleTimeoutOther, sizeof(ReadThreadIdleTimeoutOther));
        }

        RegCloseKey(hKey);
    }
}


// ---------------------------------------------------------------------
// UstatusThread
//
// This thread will continue to read unsolicited until it's asked to
// terminate, which will happen either
//  1) Receive EOJ confirmation from the printer.
//  2) Timeout waiting for EOJ confirmation.
//  3) The port is been closed.
//
// ---------------------------------------------------------------------
DWORD
WINAPI
UstatusThread(
    HANDLE hPort
)
{
    PINIPORT    pIniPort = (PINIPORT)((INIPORT *)hPort);
    DWORD dwTimeout;
    BOOL bGetTimeoutsFromRegistry = FALSE;

    pIniPort->status |= PP_THREAD_RUNNING;
    pIniPort->TimeoutCount = 0;

    while (1)
    {
        // check if we should exit

        if (!(pIniPort->status & PP_RUN_THREAD))
        {
            // somebody has clear the PP_RUN_THREAD bit
            // so we're told to end this thread

           EnterSplSem();

            if (pIniPort->status & PP_INSTARTDOC)
            {
                // there's an active job, can't end the thread

                pIniPort->status |= PP_RUN_THREAD;

               LeaveSplSem();
            }
            else
            {
                DBGMSG(DBG_INFO, ("PJLMon Read Thread for Port %s Closing Down.\n", pIniPort->pName));
                pIniPort->status &= ~PP_THREAD_RUNNING;

                // clear all the error status now since we won't be reading anymore

                SetPrinter(pIniPort->hPrinter, 0, (LPBYTE)0, PRINTER_CONTROL_SET_STATUS);
                ClosePrinter(pIniPort->hPrinter);
                pIniPort->hPrinter = NULL;

                if (pIniPort->status & PP_PORT_OPEN)
                {
                    pIniPort->status &= ~PP_PORT_OPEN;
                    (*pIniPort->fn.pfnEndDocPort)(pIniPort->hPort);
                }

                CloseHandle(pIniPort->WakeUp);
                CloseHandle(pIniPort->DoneReading);

                // wake up ClosePort if it's waiting

                pIniPort->WakeUp = NULL;

               LeaveSplSem();

                return 0;
            }
        }

        // check if the printer is bi-di

        if (pIniPort->status & PP_IS_PJL)
        {
            // try to read from printer is printer is PJL bi-di capable

            if (ReadCommand(hPort))
            {
                // commands have been read and processed
                // reset the idle timeout count

                pIniPort->TimeoutCount = 0;
            }
            else if (!(pIniPort->status & PP_INSTARTDOC))
            {
                if (!bGetTimeoutsFromRegistry)
                {
                    GetTimeoutsFromRegistry();
                    bGetTimeoutsFromRegistry++;
                }

                // nothing read from the printer and there's no active job
                // meaning current job has possibly been
                //  1) completely sent to the printer, or
                //  2) cancelled (aborted)

                if (pIniPort->status & PP_PRINTER_OFFLINE)
                {
                    // don't terminate the thread if the
                    // printer is in an error/offline state.
                    // use a super long timeout just in case the printer
                    // stop responding for some reason, like user turn off
                    // the printer to fix the error.

                    dwTimeout = ReadThreadErrorTimeout;
                }
                else if (pIniPort->status & PP_RESETDEV)
                {
                    // PP_RESETDEV is set if the very last print job
                    // has been aborted.
                    // set a timeout if the job is aborted, since we
                    // know that we're not going to get an EOJ from the
                    // printer.

                    dwTimeout = ReadThreadAbortTimeout;
                }
                else if (pIniPort->status & PP_HPLJ4L)
                {
                    // special case 4L because it doesn't return the EOJ reliably
                    // kill the read thread after timeout expires

                    dwTimeout = ReadThreadIdleTimeout4L;
                }
                else
                {
                    // waiting for EOJ or a very long timeout
                    // in case users reset the printer while there's an
                    // error then we'll never get an EOJ

                    dwTimeout = ReadThreadIdleTimeoutOther;
                }

                Sleep(2000);

                if (!pIniPort->TimeoutCount)
                {
                    // start the timeout count

                    pIniPort->TimeoutCount = GetTickCount();
                }
                else
                {
                    if ((GetTickCount() - pIniPort->TimeoutCount) > dwTimeout)
                    {
                        // thread's been idle for too long, it's time to exit

                        pIniPort->status &= ~PP_RUN_THREAD;
                    }
                }
            }

            // wait if there's a active job

            if (pIniPort->status & PP_INSTARTDOC)
            {
                // WakeUp will be signal when
                //  1) WritePort or WriteCommand fails
                //  2) EndDocPort
                //  3) ClosePort
                //  4) GetPrinterDataFromPort
                //  5) receive EOJ from printer

                WaitForSingleObject(pIniPort->WakeUp, INFINITE);
            }
        }
        else
        {
            // exit the thread if printer is not PJL bi-di capable

            Sleep(2000);
            pIniPort->status &= ~PP_RUN_THREAD;
        }
    }
}


// ---------------------------------------------------------------------
// CreateUstatusThread
//
// ---------------------------------------------------------------------
BOOL
CreateUstatusThread(
    PINIPORT pIniPort
)
{
    HANDLE  ThreadHandle;
    DWORD   ThreadId;

    DBGMSG(DBG_INFO, ("PJLMon Read Thread for Port %s Starting.\n", pIniPort->pName));

    pIniPort->status |= PP_RUN_THREAD;

    pIniPort->WakeUp = CreateEvent(NULL, FALSE, FALSE, NULL);
    pIniPort->DoneReading = CreateEvent(NULL, TRUE, TRUE, NULL); // manual-reset event, initially signal state

    ThreadHandle = CreateThread(NULL, 16*1024,
                             (LPTHREAD_START_ROUTINE)UstatusThread,
                             pIniPort,
                             0, &ThreadId);

    SetThreadPriority(ThreadHandle, THREAD_PRIORITY_LOWEST);

    CloseHandle(ThreadHandle);

    return TRUE;
}


// ---------------------------------------------------------------------
// CreatePortEntry
//
// This function needs to be called in side of monitor critical section.
// Caller has to enter critical section before calling.
//
// ---------------------------------------------------------------------
PINIPORT
CreatePortEntry(
    LPTSTR   pPortName,
    LPTSTR   pPrinterName
)
{
    DWORD       cb;
    PINIPORT    pIniPort, pPort;

    cb = sizeof(INIPORT) + wcslen(pPortName)*sizeof(TCHAR) + sizeof(TCHAR);

    pIniPort=AllocSplMem(cb);

    if(pIniPort)
    {
        pIniPort->pName = wcscpy((LPTSTR)(pIniPort+1), pPortName);
        pIniPort->cb = cb;
        pIniPort->pNext = 0;
        pIniPort->signature = IPO_SIGNATURE;
        pIniPort->pPrinterName = AllocSplStr(pPrinterName);
        OpenPrinter(pPrinterName, &pIniPort->hPrinter, NULL);

        if (pPort = pIniFirstPort)
        {
            while (pPort->pNext)
                pPort = pPort->pNext;

            pPort->pNext = pIniPort;
        }
        else
            pIniFirstPort = pIniPort;
    }

    return pIniPort;
}


// ---------------------------------------------------------------------
// DeletePortEntry
//
// This function needs to be called in side of monitor critical section.
// Caller has to enter critical section before calling.
//
// ---------------------------------------------------------------------
BOOL
DeletePortEntry(
    PINIPORT pIniPort
)
{
    if (pIniPort == pIniFirstPort)
        pIniFirstPort = pIniPort->pNext;
    else
    {
        PINIPORT    pPort;

        pPort = pIniFirstPort;
        while (pPort && pPort->pNext != pIniPort)
            pPort = pPort->pNext;

        if (pPort)
            pPort->pNext = pIniPort->pNext;
        else
            return FALSE;
    }

    if (pIniPort->hPrinter)
        ClosePrinter(pIniPort->hPrinter);

    FreeSplStr(pIniPort->pPrinterName);
    FreeSplMem(pIniPort, pIniPort->cb);

    return TRUE;
}


// ---------------------------------------------------------------------
// PJLMonOpenPortEx
//
// ---------------------------------------------------------------------
BOOL
WINAPI
PJLMonOpenPortEx(
    LPTSTR   pPortName,
    LPTSTR   pPrinterName,
    LPHANDLE pHandle,
    LPMONITOR pMonitor
)
{
    PINIPORT     pIniPort;

   EnterSplSem();

    if (pIniPort = FindPort(pPortName))
    {
       LeaveSplSem();
        SetLastError(ERROR_BUSY);
        return FALSE;
    }

    pIniPort = CreatePortEntry(pPortName, pPrinterName);

    // assume that printer is PJL bi-di capable to begin with

    pIniPort->status |= PP_IS_PJL;

    if (pIniPort && (*pMonitor->pfnOpenPort)(pPortName, &pIniPort->hPort))
    {
        *pHandle = pIniPort;

        memcpy((LPSTR)&pIniPort->fn, (LPSTR)pMonitor, sizeof(MONITOR));

       LeaveSplSem();
        return TRUE;
    }
    else
    {
        DBGMSG(DBG_ERROR, ("pjlMon!OpenPort %s : Failed\n", pPortName));

       LeaveSplSem();
        return FALSE;
    }
}


// ---------------------------------------------------------------------
// PJLMonStartDocPort
//
// Note:
// PP_PJL_SENT, PP_SEND_PJL, PP_IS_PJL, PP_PORT_OPEN, PP_LJ4L, PP_RESETDEV
//      are set/cleared on per job basis.
// PP_DONT_TRY_PJL is set/cleared on per printer basis.
//
// ---------------------------------------------------------------------
BOOL
WINAPI
PJLMonStartDocPort(
    HANDLE  hPort,
    LPTSTR  pPrinterName,
    DWORD   JobId,
    DWORD   Level,
    LPBYTE  pDocInfo
)
{

    PINIPORT    pIniPort = (PINIPORT)((INIPORT *)hPort);
    PDOC_INFO_2 pDocInfo2 = (PDOC_INFO_2)pDocInfo;

    // Serialize access to the port
    if (pIniPort->status & PP_INSTARTDOC)
    {
        SetLastError(ERROR_BUSY);
        return FALSE;
    }

    pIniPort->status |= PP_INSTARTDOC;

    if (pIniPort->status & PP_PORT_OPEN)
    {
        pIniPort->status &= ~PP_PORT_OPEN;
        (*pIniPort->fn.pfnEndDocPort)(pIniPort->hPort);

        // We want to wait a bit in case we're in the middle of a ReadCommand,
        // so that our next StartDocPort to the port monitor will succeed.

        WaitForSingleObject(pIniPort->DoneReading, READTHREADTIMEOUT);
    }

    // calling port monitor StartDoc

    if (!(*pIniPort->fn.pfnStartDocPort)(pIniPort->hPort, pPrinterName, JobId, Level, pDocInfo))
    {
        pIniPort->status &= ~PP_INSTARTDOC;
        return FALSE;
    }

    // check if we're printing to a different printer
    // or if the hPrinter has been closed

    if (lstrcmp(pIniPort->pPrinterName, pPrinterName))
    {
        // we're printing to a different printer now,
        // clear the PP_DONT_TRY_PJL bit which is set in IsPJL() if
        // the printer is not bi-di capable.

       EnterSplSem();

        FreeSplStr(pIniPort->pPrinterName);
        pIniPort->pPrinterName = AllocSplStr(pPrinterName);
        if (pIniPort->hPrinter)
            ClosePrinter(pIniPort->hPrinter);

        OpenPrinter(pPrinterName, &pIniPort->hPrinter, NULL);
        pIniPort->status &= ~PP_DONT_TRY_PJL;

       LeaveSplSem();
    }
    else if (!pIniPort->hPrinter)
        OpenPrinter(pPrinterName, &pIniPort->hPrinter, NULL);

    pIniPort->status |= PP_PORT_OPEN;

    // check if the printer supports PJL bi-di
    // if not, we'll by pass the language monitor

    if (IsPJL(hPort))
    {
        pIniPort->status |= PP_IS_PJL;

        if ((Level == 1) ||
            (pDocInfo2->dwMode == DI_CHANNEL) ||
            (pDocInfo2->dwMode == DI_CHANNEL_WRITE))
        {
            // set PP_SEND_PJL flag here so the first write of the job
            // will try to send PJL command to initiate the job control

            pIniPort->status |= PP_SEND_PJL;
        }

        if (!(pIniPort->status & PP_RUN_THREAD))
        {
            // only create the read thread if printer is PJL capable

            CreateUstatusThread(pIniPort);
        }
    }
    else
        pIniPort->status &= ~PP_IS_PJL;

    // clear PP_PJL_SENT and PP_RESETDEV flag for this new job

    pIniPort->status &= ~(PP_PJL_SENT | PP_RESETDEV);

    // reset the read thread thermination timeout

    pIniPort->TimeoutCount = 0;

    pIniPort->JobId = JobId;

    return TRUE;
}


// ---------------------------------------------------------------------
// PJLMonReadPort
//
// ---------------------------------------------------------------------
BOOL
WINAPI
PJLMonReadPort(
    HANDLE hPort,
    LPBYTE pBuffer,
    DWORD  cbBuf,
    LPDWORD pcbRead
)
{
    PINIPORT    pIniPort = (PINIPORT)((INIPORT *)hPort);

    if (!(pIniPort->status & PP_PORT_OPEN))
    {
        *pcbRead = 0;
        return FALSE;
    }

    return (*pIniPort->fn.pfnReadPort)(pIniPort->hPort, pBuffer, cbBuf, pcbRead);
}


// ---------------------------------------------------------------------
// PJLMonWritePort
//
// ---------------------------------------------------------------------
BOOL
WINAPI
PJLMonWritePort(
    HANDLE  hPort,
    LPBYTE  pBuffer,
    DWORD   cbBuf,
    LPDWORD pcbWritten
)
{
    PINIPORT    pIniPort = (PINIPORT)((INIPORT *)hPort);
    BOOL        ret;

    if (!(pIniPort->status & PP_PORT_OPEN))
    {
        *pcbWritten = 0;
        return FALSE;
    }

    // check if it's the fist write of the job

    if (pIniPort->status & PP_SEND_PJL)
    {
        // PP_SEND_PJL is set if it's the first write of the job

        char string[35];

        if (!(WriteCommand(hPort, "\033%-12345X@PJL \015\012", TRUE)))
        {
            // we failed to write this PJL command

            return FALSE;
        }

        // clear PP_SEND_PJL here if we have successfully send a PJL command.

        pIniPort->status &= ~PP_SEND_PJL;

        // set PP_PJL_SENT meaning that we have successfully sent a
        // PJL command to the printer, though it doesn't mean that
        // we will get a successfully read. PP_PJL_SENT gets cleared in
        // StartDocPort.

        pIniPort->status |= PP_PJL_SENT;

        wsprintf(string, TEXT("@PJL JOB NAME = \"MSJOB %d\"\015\012"), pIniPort->JobId);
        WriteCommand(hPort, string, TRUE);
        WriteCommand(hPort, "@PJL USTATUS JOB = ON \015\012@PJL USTATUS PAGE = OFF \015\012@PJL USTATUS DEVICE = ON \015\012@PJL USTATUS TIMED = 0 \015\012", TRUE);
    }

    // writing to port monitor

    ret = (*pIniPort->fn.pfnWritePort)(pIniPort->hPort, pBuffer, cbBuf, pcbWritten);

    if ((!*pcbWritten || pIniPort->PrinterStatus) &&
        (pIniPort->status & PP_PJL_SENT))
    {
        DBGMSG(DBG_ERROR, ("PJLMON!No data Written\n"));
        SetEvent(pIniPort->WakeUp);
    }

    return ret;
}


// ---------------------------------------------------------------------
// PJLMonEndDocPort
//
// ---------------------------------------------------------------------
BOOL
WINAPI
PJLMonEndDocPort(
   HANDLE   hPort
)
{
    PINIPORT    pIniPort = (PINIPORT)((INIPORT *)hPort);

    // check if we had sent PJL command, i.e. if the printer is bi-di

   EnterSplSem();

    if (pIniPort->status & PP_PJL_SENT)
    {
        // if the printer is bi-di, tell printer to let us know when the job
        // is don't in the printer and we'll really EndDoc then. this is so
        // that we can continue to monitor the job status until the job is
        // really done in case there's an error occurs.
        // but some cheap printers like 4L, doesn't handle this EOJ command
        // reliably, so we time out if printer doesn't tell us EOJ after a
        // while so that we don't end up having the port open forever in this
        // case.

        char    string[256];

        wsprintf(string, TEXT("\033%%-12345X@PJL EOJ NAME = \"MSJOB %d\"\015\012\033%%-12345X"), pIniPort->JobId);
        WriteCommand(hPort, string, TRUE);
    }
    else
    {
        // if the printer is not bi-di, EndDoc now.

        ClosePrinter(pIniPort->hPrinter);
        pIniPort->hPrinter = NULL;

        pIniPort->status &= ~PP_PORT_OPEN;
        (*pIniPort->fn.pfnEndDocPort)(pIniPort->hPort);
    }

    pIniPort->status &= ~PP_INSTARTDOC;

    // wake up the UStatus read thread if printer is bi-di

    if (pIniPort->status & PP_IS_PJL)
        SetEvent(pIniPort->WakeUp);

   LeaveSplSem();

    return TRUE;
}


// ---------------------------------------------------------------------
// PJLMonClosePort
//
// Terminate the UstatusThread.
//
// ---------------------------------------------------------------------
BOOL
WINAPI
PJLMonClosePort(
    HANDLE  hPort
)
{
    PINIPORT    pIniPort = (PINIPORT)((INIPORT *)hPort);

    if (pIniPort->status & PP_THREAD_RUNNING)
    {
        // this will kill the UStatus read thread

        pIniPort->status &= ~PP_RUN_THREAD;

        if (pIniPort->status & PP_PORT_OPEN)
        {
            pIniPort->status &= ~PP_PORT_OPEN;
            (*pIniPort->fn.pfnEndDocPort)(pIniPort->hPort);
        }

        SetEvent(pIniPort->WakeUp);

        // if UStatusThread is still running at this point,
        // wait utill it terminates, because we can't DeletePortEntry
        // until it terminates.

        while (pIniPort->WakeUp)
            Sleep(500);
    }

    (*pIniPort->fn.pfnClosePort)(pIniPort->hPort);

   EnterSplSem();
    DeletePortEntry(pIniPort);
   LeaveSplSem();

    return TRUE;
}


// ---------------------------------------------------------------------
// WriteCommand
//
// Return TRUE if the command has been successfully written.
// Return FALSE otherwise.
//
// ---------------------------------------------------------------------
BOOL
WriteCommand(
    HANDLE hPort,
    LPSTR cmd,
    BOOL bDirectToPort
)
{
    DWORD cbWrite, cbWritten;
    PINIPORT    pIniPort = (PINIPORT)((INIPORT *)hPort);

    if (!(pIniPort->status & PP_PORT_OPEN))
        return FALSE;

    cbWrite = wcslen(cmd);

    if (bDirectToPort)
        (*pIniPort->fn.pfnWritePort)(pIniPort->hPort, cmd, cbWrite, &cbWritten);
    else
        PJLMonWritePort(hPort, cmd, cbWrite, &cbWritten);

    if (!cbWritten)
    {
        DBGMSG(DBG_ERROR, ("PJLMON!No data Written\n"));
        SetEvent(pIniPort->WakeUp);
    }

    return cbWritten == cbWrite;
}


// ---------------------------------------------------------------------
// ReadCommand
//
// Return TRUE if it has read and process one or more commands.
// Return FALSE if it did not read and process any command.
//
// ---------------------------------------------------------------------
#define CBSTRING 1024

BOOL
ReadCommand(
    HANDLE hPort
)
{
    PINIPORT    pIniPort = (PINIPORT)((INIPORT *)hPort);
    DWORD   cbRead, cbToRead, cbProcessed, cbPrevious;
    char    string[CBSTRING];
    DWORD   status;

    // Fails if the port has been closed.

    if (!(pIniPort->status & PP_PORT_OPEN))
        return FALSE;

    cbPrevious = 0;

    ResetEvent(pIniPort->DoneReading);

    while (1)
    {
        cbToRead = CBSTRING - cbPrevious - 1;

        PJLMonReadPort(hPort, &string[cbPrevious], cbToRead, &cbRead);

        if (cbRead)
        {
            string[cbPrevious + cbRead] = '\0';
            status = ProcessPJLString(pIniPort, string, &cbProcessed);

            if (status == STATUS_END_OF_STRING)
            {
                if (cbPrevious + cbRead - cbProcessed == CBSTRING - 1)
                    DBGMSG(DBG_ERROR, ("PJLMON!STATUS_END_OF_STRING AND THE BUFFER IS TOO SMALL\n"));

                wcscpy(string, string+cbProcessed);
                cbPrevious = cbRead - cbProcessed;
            }

            ASSERT((cbProcessed < CBSTRING), ("cbProcessed is bad %d\n", cbProcessed));
        }
        else
        {
            ASSERT((!cbPrevious), ("STATUS_END_OF_STRING AND NOTHING MORE TO READ\n"));
        }

        if ((cbToRead == 0) || (cbRead != cbToRead))
            break;
    }

    SetEvent(pIniPort->DoneReading);

    return (BOOL)cbRead;
}


// ---------------------------------------------------------------------
// PJLMonGetPrinterDataFromPort
//
// Currently spooler needs to call StartDocPort before call this, since
// WriteCommand only works if StartDocPort has been called. We might
// want to change it so it'll automatically call StartDocPort if need to.
//
// This monitor function supports the following two functionalities,
//
//  1. Allow spooler or language monitor to call DeviceIoControl to get
//      information from the port driver vxd, i.e. ControlID != 0.
//      And only port monitor support this functionality, language monitor
//      doesn't, so language monitor just pass this kind of calls down to
//      port monitor.
//
//  2. Allow app or printer driver query language monitor for some device
//      information by specifying some key names that both parties understand,
//      i.e. ControlID == 0 && pValueName != 0. So when printer driver call
//      DrvGetPrinterData DDI, gdi will call spooler -> language monitor
//      to get specific device information, for example, UNIDRV does this
//      to get installed printer memory from PJL printers thru PJLMON.
//      Only language monitor support this functionality,
//      port monitor doesn't.
//
// ---------------------------------------------------------------------
BOOL
WINAPI
PJLMonGetPrinterDataFromPort(
    HANDLE   hPort,
    DWORD   ControlID,
    LPTSTR  pValueName,
    LPTSTR  lpInBuffer,
    DWORD   cbInBuffer,
    LPTSTR  lpOutBuffer,
    DWORD   cbOutBuffer,
    LPDWORD lpcbReturned
)
{
    PINIPORT    pIniPort = (PINIPORT)((INIPORT *)hPort);

    // we can only do this if we have called StartDocPort to the port monitor

    if (!(pIniPort->status & PP_PORT_OPEN))
        return FALSE;

    if (ControlID)
    {
        if (ControlID == RESETDEV)
        {
            // the current job is being aborted,
            // and spooler is trying to reset the device (printer).
            // the port driver and printer may or may not support this functionality
            // for example, lpt.vxd supports it, serial.vxd doesn't.
            // LaserJet 4L supports it, other 4 series don't.

            pIniPort->status |= PP_RESETDEV;

#ifdef LATER
            // because LJ4s don't support hardware soft reset,
            // we'll see if we can reset the printer this way.

            WriteCommand(hPort, "\033Z", TRUE);
#endif
        }

        if (!pIniPort->fn.pfnGetPrinterDataFromPort)
            return FALSE;

        return (*pIniPort->fn.pfnGetPrinterDataFromPort)
            (pIniPort->hPort, ControlID, pValueName, lpInBuffer, cbInBuffer,lpOutBuffer, cbOutBuffer, lpcbReturned);
    }

    // make sure the first write succeeds

    if ((pIniPort->status & PP_IS_PJL) &&
        WriteCommand(hPort, "\033%-12345X@PJL INFO CONFIG\015\012", FALSE))
    {
        // PJLMON currently only supports the following pValueName
        //  1. installed printer memory
        //  2. available printer memory

        if (!lstrcmpi(pValueName, PD_INSTALLED_MEMORY))
            pIniPort->dwInstalledMemory = 0;
        else if (!lstrcmpi(pValueName, PD_AVAILABLE_MEMORY))
            pIniPort->dwAvailableMemory = 0;

        pIniPort->status |= PP_PJL_SENT;

        ResetEvent(pIniPort->DoneReading);
        SetEvent(pIniPort->WakeUp);
        WaitForSingleObject(pIniPort->DoneReading, READTHREADTIMEOUT);

        WriteCommand(hPort, "@PJL INFO MEMORY\015\012@PJL INFO STATUS\015\012", FALSE);

        ResetEvent(pIniPort->DoneReading);
        SetEvent(pIniPort->WakeUp);
        WaitForSingleObject(pIniPort->DoneReading, READTHREADTIMEOUT);

        if (!lstrcmpi(pValueName, PD_INSTALLED_MEMORY))
        {
            *lpcbReturned = sizeof(DWORD);

            if (lpOutBuffer && (cbOutBuffer >= sizeof(DWORD)) &&
                pIniPort->dwInstalledMemory)
            {
                *((LPDWORD)lpOutBuffer) = pIniPort->dwInstalledMemory;

                return TRUE;
            }
        }
        else if (!lstrcmpi(pValueName, PD_AVAILABLE_MEMORY))
        {
            *lpcbReturned = sizeof(DWORD);

            if (lpOutBuffer && (cbOutBuffer >= sizeof(DWORD)) &&
                pIniPort->dwAvailableMemory)
            {
                *((LPDWORD)lpOutBuffer) = pIniPort->dwAvailableMemory;

                return TRUE;
            }
        }
    }

    return FALSE;
}


// ---------------------------------------------------------------------
// InitializeMonitorEx
//
// ---------------------------------------------------------------------
BOOL
WINAPI
InitializeMonitorEx(
    LPTSTR  pRegistryRoot,
    LPMONITOR lpMonitor
)
{
    InitializeCriticalSection(&pjlMonSection);

    if (pRegistryRoot && *pRegistryRoot)
        pRegistryPath = AllocSplStr(pRegistryRoot);

    lpMonitor->pfnOpenPortEx    = PJLMonOpenPortEx;
    lpMonitor->pfnStartDocPort  = PJLMonStartDocPort;
    lpMonitor->pfnWritePort     = PJLMonWritePort;
    lpMonitor->pfnReadPort      = PJLMonReadPort;
    lpMonitor->pfnEndDocPort    = PJLMonEndDocPort;
    lpMonitor->pfnClosePort     = PJLMonClosePort;
    lpMonitor->pfnGetPrinterDataFromPort = PJLMonGetPrinterDataFromPort;

    // language monitor doesn't support the following functions

    lpMonitor->pfnSetPortTimeOuts = NULL;
    lpMonitor->pfnOpenPort = NULL;
    lpMonitor->pfnAddPort = NULL;
    lpMonitor->pfnConfigurePort = NULL;
    lpMonitor->pfnDeletePort = NULL;
    lpMonitor->pfnEnumPorts = NULL;

    return TRUE;
}


// ---------------------------------------------------------------------
// ProcessPJLString
//
// ---------------------------------------------------------------------
#define NTOKEN  20

int
ProcessPJLString(
    PINIPORT pIniPort,
    LPTSTR pInString,
    LPDWORD lpcbProcessed
)
{
    TOKENPAIR tokenPairs[NTOKEN];
    DWORD nTokenParsedRet;
    LPSTR lpRet;
    DWORD status;

    // We process one PJL command at a time.

    lpRet = pInString;

#ifdef DEBUG
    OutputDebugString(pInString);
#endif

    for (*lpcbProcessed = 0; *pInString != 0; pInString = lpRet)
    {
        // hack to determine if printer is bi-di.  LJ 4 does not have p1284 device ID
        // so we do PCL memory query and see if it returns anything
        if (!(pIniPort->status & PP_IS_PJL) &&
            !mystrncmp(pInString, "PCL\015\012INFO MEMORY", 16))
            pIniPort->status |= PP_IS_PJL;

        status = GetPJLTokens(pInString, NTOKEN, tokenPairs, &nTokenParsedRet, &lpRet);

        if (status == STATUS_REACHED_END_OF_COMMAND_OK)
        {
            pIniPort->status |= PP_IS_PJL;
            InterpreteTokens(pIniPort, tokenPairs, nTokenParsedRet);
        }
        else
            ProcessParserError(status);

        // if a PJL command straddles between buffers

        if (status == STATUS_END_OF_STRING)
           break;

        *lpcbProcessed += lpRet - pInString;
    }

    return status;
}


// ---------------------------------------------------------------------
// InterpreteTokens
//
// ---------------------------------------------------------------------
VOID
InterpreteTokens(
    PINIPORT pIniPort,
    PTOKENPAIR tokenPairs,
    DWORD nTokenParsed
)
{
    DWORD i, OldStatus;
    PJLTOPRINTERSTATUS * pMap;

    OldStatus = pIniPort->PrinterStatus;

    for (i = 0; i < nTokenParsed; i++)
    {
        DBGMSG(DBG_INFO, ("pjlmon!Token=0x%x, Value=%d\n", tokenPairs[i].token, tokenPairs[i].value));

        switch(tokenPairs[i].token)
        {
        case TOKEN_INFO_STATUS_CODE:
        case TOKEN_USTATUS_DEVICE_CODE:
            for (pMap = PJLToStatus; pMap->pjl; pMap++)
            {
                if (pMap->pjl == tokenPairs[i].value)
                {
                    pIniPort->PrinterStatus = pMap->status;

                    // set printer offline in case of an error

                    if (tokenPairs[i].value > 40000)
                        pIniPort->status |= PP_PRINTER_OFFLINE;

                    break;
                }
            }

            // some printers use this to signal online/ready

            if ((tokenPairs[i].value == 10001) ||
                (tokenPairs[i].value == 10002) ||
                (tokenPairs[i].value == 11002))
                pIniPort->status &= ~PP_PRINTER_OFFLINE;

            // background or foreground paper out

            if ((tokenPairs[i].value > 11101 && tokenPairs[i].value < 12000) ||
                (tokenPairs[i].value > 41101 && tokenPairs[i].value < 42000))
            {
                pIniPort->PrinterStatus = PRINTER_STATUS_PAPER_OUT;
                pIniPort->status |= PP_PRINTER_OFFLINE;
            }

            if (tokenPairs[i].value > 40000)
                pIniPort->PrinterStatus |= PRINTER_STATUS_USER_INTERVENTION;

            break;

        case TOKEN_INFO_STATUS_ONLINE:
        case TOKEN_USTATUS_DEVICE_ONLINE:
            DBGMSG(DBG_INFO, ("PJLMON:ONLINE = %d\n", tokenPairs[i].value));

            if (tokenPairs[i].value)
            {
                pIniPort->PrinterStatus = 0;
                pIniPort->status &= ~PP_PRINTER_OFFLINE;
            }
            else
            {
                pIniPort->PrinterStatus |= PRINTER_STATUS_OFFLINE;
                pIniPort->status |= PP_PRINTER_OFFLINE;
            }
            break;

        case TOKEN_USTATUS_JOB_NAME_MSJOB:
            DBGMSG(DBG_INFO, ("PJLMON:EOJ received for job %d\n", tokenPairs[i].value));

            if (tokenPairs[i].value == pIniPort->JobId)
            {
                // this will terminate the thread

                pIniPort->status &= ~PP_RUN_THREAD;

                SetEvent(pIniPort->WakeUp);
            }
            break;

        case TOKEN_INFO_CONFIG_MEMORY:
        case TOKEN_INFO_CONFIG_MEMORY_SPACE:

            // IMPORTANT NOTE:
            // 
            // Use SetPrinterData to cache the information in printer's registry.
            // GDI's DrvGetPrinterData will check the printer's registry first,
            // and if cache data is available, it will use it and not call
            // GetPrinterData (which calls language monitor's
            // GetPrinterDataFromPort).

            SetPrinterData(pIniPort->hPrinter, (LPTSTR)PD_INSTALLED_MEMORY, REG_DWORD, (LPBYTE)&tokenPairs[i].value, sizeof(DWORD));
            pIniPort->dwInstalledMemory = tokenPairs[i].value;
            break;

        case TOKEN_INFO_MEMORY_TOTAL:

            // IMPORTANT NOTE:
            // 
            // Use SetPrinterData to cache the information in printer's registry.
            // GDI's DrvGetPrinterData will check the printer's registry first,
            // and if cache data is available, it will use it and not call
            // GetPrinterData (which calls language monitor's
            // GetPrinterDataFromPort).

            SetPrinterData(pIniPort->hPrinter, (LPTSTR)PD_AVAILABLE_MEMORY, REG_DWORD, (LPBYTE)&tokenPairs[i].value, sizeof(DWORD));
            pIniPort->dwAvailableMemory = tokenPairs[i].value;
            break;

        default:
            break;
        }
    }

    if (OldStatus != pIniPort->PrinterStatus)
    {
        SetPrinter(pIniPort->hPrinter, 0, (LPBYTE)pIniPort->PrinterStatus, PRINTER_CONTROL_SET_STATUS);
        DBGMSG(DBG_INFO, ("calling SetPrinter, with Status = %x\n", pIniPort->PrinterStatus));
    }
}


// ---------------------------------------------------------------------
// ProcessParserError
//
// ---------------------------------------------------------------------
VOID
ProcessParserError(
    DWORD status
)
{
#ifdef DEBUG
    LPTSTR pString;

    switch (status)
    {
    case STATUS_REACHED_END_OF_COMMAND_OK:
        pString = "STATUS_REACHED_END_OF_COMMAND_OK\n";
        break;

    case STATUS_CONTINUE:
        pString = "STATUS_CONTINUE\n";
        break;

    case STATUS_REACHED_FF:
        pString = "STATUS_REACHED_FF\n";
        break;

    case STATUS_END_OF_STRING:
        pString = "STATUS_END_OF_STRING\n";
        break;

    case STATUS_SYNTAX_ERROR:
        pString = "STATUS_SYNTAX_ERROR\n";
        break;

    case STATUS_ATPJL_NOT_FOUND:
        pString = "STATUS_ATPJL_NOT_FOUND\n";
        break;

    case STATUS_NOT_ENOUGH_ROOM_FOR_TOKENS:
        pString = "STATUS_NOT_ENOUGH_ROOM_FOR_TOKENS\n";
        break;

    default:
        pString = "INVALID STATUS RETURNED!!!!!!\n";
        break;
    };

    OutputDebugString(pString);
#endif
}


#define HPLJ4L                      "LaserJet 4L"
#define LEN_HPLJ4L                  11
#define MODEL                       "MODEL:"
#define MDL                         "MDL:"
#define COMMAND                     "COMMAND SET:"
#define CMD                         "CMD:"
#define COLON                       ':'
#define SEMICOLON                   ';'


// ---------------------------------------------------------------------
// FindP1284Key
//
// Return the pointer to the COMMAND string. NULL if it's not found.
//
// ---------------------------------------------------------------------
LPSTR
FindP1284Key
(
    PINIPORT    pIniPort,
    LPSTR   lpKey
)
{
    LPSTR   lpValue;                // Pointer to the Key's value
    WORD    wKeyLength;             // Length for the Key (for stringcmps)
    LPSTR   ret = NULL;

    // While there are still keys to look at.

    DBGMSG(DBG_INFO, ("PJLMon!DeviceId(%s)\n", lpKey));

    while (lpKey && *lpKey)
    {
        // Is there a terminating COLON character for the current key?

        if (!(lpValue = mystrchr(lpKey, COLON)) )
        {
            // N: OOPS, somthing wrong with the Device ID

            return ret;
        }

        // The actual start of the Key value is one past the COLON

        ++lpValue;

        // Compute the Key length for Comparison, including the COLON
        // which will serve as a terminator

        wKeyLength = (WORD)(lpValue - lpKey);

        // Compare the Key to the Know quantities.  To speed up the comparison
        // a Check is made on the first character first, to reduce the number
        // of strings to compare against.
        // If a match is found, the appropriate lpp parameter is set to the
        // key's value, and the terminating SEMICOLON is converted to a NULL
        // In all cases lpKey is advanced to the next key if there is one.

        switch (*lpKey)
        {
        case 'C':
            // Look for COMMAND SET or CMD

            if (!mystrncmp(lpKey, COMMAND, wKeyLength) ||
                !mystrncmp(lpKey, CMD, wKeyLength))
            {
                ret = lpValue;
            }
            break;

        case 'M':
            // Look for MODEL or MDL

            if (!mystrncmp(lpKey, MODEL, wKeyLength) ||
                !mystrncmp(lpKey, MDL, wKeyLength))
            {
                if (!mystrncmp(lpValue, HPLJ4L, LEN_HPLJ4L))
                {
                    // the model is LaserJet 4L

                    pIniPort->status |= PP_HPLJ4L;
                    DBGMSG(DBG_INFO, ("PJLMon!Printer is LaserJet 4L.\n"));
                }
            }
            break;
        }

        // Go to the next Key

        if (lpKey = mystrchr(lpValue, SEMICOLON))
        {
            *lpKey = '\0';
            ++lpKey;
        }
    }

    return ret;
}


// ---------------------------------------------------------------------
// IsPJL
//
// Return TRUE if printer supports PJL bi-di, FALSE otherwise.
// Set PP_DONT_TRY_PJL if we failed, so we don't try again, until the
// printer that we're printing to is changed in StartDocPort.
//
// ---------------------------------------------------------------------
BOOL
IsPJL(
    HANDLE hPort
)
{
    PINIPORT    pIniPort = (PINIPORT)((INIPORT *)hPort);
    char    szID[MAX_DEVID];
    DWORD   cbRet;
    LPSTR   lpCMD;

    // clear the LaserJet 4L special flag

    pIniPort->status &= ~PP_HPLJ4L;

    // for printers that supports P1284 plug and play like LJ 4L, DJ540.
    // we parse the COMMAND string and see if PJL is supported

    if (pIniPort->fn.pfnGetPrinterDataFromPort)
    {
        // Only try P1284 if port monitor supports DeviceIOCtl

        memset((LPBYTE)szID, 0, sizeof(szID));
        cbRet = 0;
        if ((*pIniPort->fn.pfnGetPrinterDataFromPort)
            (pIniPort->hPort, GETDEVICEID, NULL, NULL, 0, szID, sizeof(szID), &cbRet)
            && cbRet)
        {
            // succeeded the P1284 plug and play protocol

            if (lpCMD = FindP1284Key(pIniPort, szID))
            {
                // found the COMMAND string

                while (*lpCMD)
                {
                    // look for "PJL"

                    if ((*lpCMD == 'P') && (lpCMD[1] == 'J') && (lpCMD[2] == 'L'))
                    {
                        pIniPort->status &= ~PP_DONT_TRY_PJL;
                        return TRUE;
                    }

                    lpCMD++;
                }

                pIniPort->status |= PP_DONT_TRY_PJL;
                return FALSE;
            }
        }

        // fall thru to try PJL bi-di if we failed the P1284 communication
        // or P1284 didn't return a COMMAND string
    }

    // for printers that don't support P1284 plug and play, but support PJL
    // language command, like LJ 4 and 4M. we try to write/read PJL
    // command and see if it succeeds.
    // if we can't set the time outs we don't want to try to read, just fail.

    if (pIniPort->fn.pfnSetPortTimeOuts &&
        !(pIniPort->status & PP_DONT_TRY_PJL))
    {
        COMMTIMEOUTS CTO;

        memset((LPSTR)&CTO, 0, sizeof(CTO));
        CTO.ReadTotalTimeoutConstant = 5000,
        CTO.ReadIntervalTimeout = 200,
        (*pIniPort->fn.pfnSetPortTimeOuts)(pIniPort->hPort, &CTO, 0);

        // This <ESC>*s1M is a PCL5 command to determine the amount of memory
        // in a PCL5 printer, and if the printer is PCL5 and bi-di capable,
        // it will return "PCL\015\012INFO MEMORY".
        // See PJL Tech Ref Manual page 7-21.

        pIniPort->status &= ~PP_IS_PJL;

        if (!WriteCommand(hPort, "\033*s1M", TRUE))
            return FALSE;

        // ReadCommand->ProcessPJLString will set PP_IS_PJL
        // if we read any valid PJL command back from the printer

        if (!ReadCommand(hPort))
        {
            // We have jumped through the hoop to determin if this printer can
            // understand PJL.  It DOES NOT.  We are not going to try again.
            // until there is a printer change.

            pIniPort->status |= PP_DONT_TRY_PJL;
        }

        if (pIniPort->status & PP_IS_PJL)
            return TRUE;
    }

    return FALSE;
}
