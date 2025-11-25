/******************************Module*Header*******************************\
* Module Name: debug.c
*
* debug helper routines
*
* Copyright (c) 1992-1993 Microsoft Corporation
*
\**************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "driver.h"

#include <ntsdexts.h>

#include "lines.h"


#if DBG

#define LOG_SIZE_IN_BYTES 4000

typedef struct _LOGGER {
    ULONG ulEnd;
    ULONG ulCurrent;
    CHAR  achBuf[LOG_SIZE_IN_BYTES];
} DBGLOG;

#define GetAddress(dst, src)\
try {\
    dst = (VOID*) lpGetExpressionRoutine(src);\
} except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ?\
            EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {\
    lpOutputRoutine("NTSD: Access violation on \"%s\", switch to server context\n", src);\
    return;\
}

DBGLOG glog = {0, 0};           // If you muck with this, fix 'dumplog' too
ULONG  DebugLevel = 0;
ULONG  LogLevel = 1;

#endif // DBG

/*****************************************************************************
 *
 *   Routine Description:
 *
 *      This function is variable-argument, level-sensitive debug print
 *      routine.
 *      If the specified debug level for the print statement is lower or equal
 *      to the current debug level, the message will be printed.
 *
 *   Arguments:
 *
 *      DebugPrintLevel - Specifies at which debugging level the string should
 *          be printed
 *
 *      DebugMessage - Variable argument ascii c string
 *
 *   Return Value:
 *
 *      None.
 *
 ***************************************************************************/

VOID
DebugPrint(
    ULONG DebugPrintLevel,
    PCHAR DebugMessage,
    ...
    )


{

#if DBG

    va_list ap;

    va_start(ap, DebugMessage);

    if (DebugPrintLevel <= DebugLevel) {

        char buffer[128];

        vsprintf(buffer, DebugMessage, ap);

        OutputDebugStringA(buffer);
    }

    va_end(ap);

#endif // DBG

} // DebugPrint()


/*****************************************************************************
 *
 *   Routine Description:
 *
 *      This function is variable-argument, level-sensitive debug log
 *      routine.
 *      If the specified debug level for the log statement is lower or equal
 *      to the current debug level, the message will be logged.
 *
 *   Arguments:
 *
 *      DebugLogLevel - Specifies at which debugging level the string should
 *          be logged
 *
 *      DebugMessage - Variable argument ascii c string
 *
 *   Return Value:
 *
 *      None.
 *
 ***************************************************************************/

VOID
DebugLog(
    ULONG DebugLogLevel,
    PCHAR DebugMessage,
    ...
    )


{

#if DBG

    va_list ap;

    va_start(ap, DebugMessage);

    if (DebugLogLevel <= LogLevel) {

        char buffer[128];
        int  length;

        length = vsprintf(buffer, DebugMessage, ap);

        length++;           // Don't forget '\0' terminator!

        // Wrap around to the beginning of the log if not enough room for
        // string:

        if (glog.ulCurrent + length >= LOG_SIZE_IN_BYTES) {
            glog.ulEnd     = glog.ulCurrent;
            glog.ulCurrent = 0;
        }

        memcpy(&glog.achBuf[glog.ulCurrent], buffer, length);
        glog.ulCurrent += length;
    }

    va_end(ap);

#endif // DBG

} // DebugLog()


/*****************************************************************************
 *
 *   Routine Description:
 *
 *       This function is called as an NTSD extension to dump a LineState
 *
 *   Arguments:
 *
 *       hCurrentProcess - Supplies a handle to the current process (at the
 *           time the extension was called).
 *
 *       hCurrentThread - Supplies a handle to the current thread (at the
 *           time the extension was called).
 *
 *       CurrentPc - Supplies the current pc at the time the extension is
 *           called.
 *
 *       lpExtensionApis - Supplies the address of the functions callable
 *           by this extension.
 *
 *       lpArgumentString - the float to display
 *
 *   Return Value:
 *
 *       None.
 *
 ***************************************************************************/
VOID dumplog(
    HANDLE hCurrentProcess,
    HANDLE hCurrentThread,
    DWORD dwCurrentPc,
    PNTSD_EXTENSION_APIS lpExtensionApis,
    LPSTR lpArgumentString)
{

#if DBG

    PNTSD_OUTPUT_ROUTINE lpOutputRoutine;
    PNTSD_GET_EXPRESSION lpGetExpressionRoutine;
    PNTSD_GET_SYMBOL     lpGetSymbolRoutine;

    ULONG       cFrom;
    ULONG       cTo;
    ULONG       cCurrent;
    DBGLOG*     plogOriginal;
    DBGLOG*     plog;
    ULONG       ulCurrent;
    ULONG       ulEnd;
    CHAR*       pchEnd;
    CHAR*       pch;

    UNREFERENCED_PARAMETER(hCurrentThread);
    UNREFERENCED_PARAMETER(dwCurrentPc);

    lpOutputRoutine        = lpExtensionApis->lpOutputRoutine;
    lpGetExpressionRoutine = lpExtensionApis->lpGetExpressionRoutine;
    lpGetSymbolRoutine     = lpExtensionApis->lpGetSymbolRoutine;

    lpOutputRoutine("!s3.dumplog [<from#> [<to#>]]\n\n");

    // Evaluate the argument string to get the address of
    // the Line Structure

    cTo   = 1;              // Defaults
    cFrom = 20;

    pch = strpbrk(lpArgumentString, "0123456789");
    if (pch != NULL)        // Use defaults if no args given
    {
        cFrom = atoi(pch);
        pch = strchr(pch, ' ');
        if (pch != NULL)
        {
            pch = strpbrk(pch, "0123456789");
            if (pch != NULL)
                cTo = atoi(pch);
        }
    }

    // Do some parameter validation, then read the log into the
    // debugger process's address space:

    if (cTo >= cFrom)
        cTo = cFrom;

    if (cTo < 1)
    {
        cTo   = 1;
        cFrom = 1;
    }

    GetAddress(plogOriginal, "glog");

    if (!ReadProcessMemory(hCurrentProcess,
                          (LPVOID) &(plogOriginal->ulCurrent),
                          &ulCurrent,
                          sizeof(ulCurrent),
                          NULL))
        return;

    if (!ReadProcessMemory(hCurrentProcess,
                          (LPVOID) &(plogOriginal->ulEnd),
                          &ulEnd,
                          sizeof(ulEnd),
                          NULL))
        return;

    if (ulCurrent == 0 && ulEnd == 0)
    {
        lpOutputRoutine("Log empty\n\n");
        return;
    }

    plog = (DBGLOG*) LocalAlloc(0, sizeof(DBGLOG) + 1);

    if (plog == NULL) {
        lpOutputRoutine("Couldn't allocate temporary buffer!\n");
        return;
    }

    if (!ReadProcessMemory(hCurrentProcess,
                          (LPVOID) &(plogOriginal->achBuf[0]),
                          &plog->achBuf[1],
                          LOG_SIZE_IN_BYTES,
                          NULL))
        return;

    // Mark the first byte in the buffer as being a zero, because
    // we're going to search backwards through the buffer for zeroes,
    // and we'll want to stop when we get to the beginning:

    plog->achBuf[0] = 0;
    ulCurrent++;
    ulEnd++;

    // Find the start string by going backwards through the buffer
    // and counting strings until the count becomes equal to 'cFrom':

    cCurrent = 0;
    pch      = &plog->achBuf[ulCurrent - 1];
    pchEnd   = &plog->achBuf[0];

    while (TRUE)
    {
        if (*(--pch) == 0)
        {
            cCurrent++;
            if (--cFrom == 0)
                break;

            if (pch == &plog->achBuf[ulCurrent - 1])
                break;         // We're back to where we started!
        }

        // Make sure we wrap the end of the buffer:

        if (pch <= pchEnd)
        {
            if (ulCurrent >= ulEnd)
                break;

            pch = &plog->achBuf[ulEnd - 1];
        }
    }

    // pch is pointing to zero byte before our start string:

    pch++;

    // Output the strings:

    pchEnd = &plog->achBuf[max(ulEnd, ulCurrent)];

    while (cCurrent >= cTo)
    {
        lpOutputRoutine("-%li: %s", cCurrent, pch);
        pch += strlen(pch) + 1;
        cCurrent--;

        // Make sure we wrap when we get to the end of the buffer:

        if (pch >= pchEnd)
            pch = &plog->achBuf[1];     // First char in buffer is a NULL
    }

    lpOutputRoutine("\n");
    LocalFree(plog);

#endif // DBG

    return;
}

/*****************************************************************************
 *
 *   Routine Description:
 *
 *       This function is called as an NTSD extension to dump
 *       the CRTC registers of an S3 chip
 *
 *   Arguments:
 *
 *       hCurrentProcess - Supplies a handle to the current process (at the
 *           time the extension was called).
 *
 *       hCurrentThread - Supplies a handle to the current thread (at the
 *           time the extension was called).
 *
 *       CurrentPc - Supplies the current pc at the time the extension is
 *           called.
 *
 *       lpExtensionApis - Supplies the address of the functions callable
 *           by this extension.
 *
 *       lpArgumentString - the float to display
 *
 *   Return Value:
 *
 *       None.
 *
 ***************************************************************************/
VOID dcrtc(
    HANDLE hCurrentProcess,
    HANDLE hCurrentThread,
    DWORD dwCurrentPc,
    PNTSD_EXTENSION_APIS lpExtensionApis,
    LPSTR lpArgumentString)
{

#if DBG

    PNTSD_OUTPUT_ROUTINE lpOutputRoutine;
    PNTSD_GET_EXPRESSION lpGetExpressionRoutine;
    PNTSD_GET_SYMBOL     lpGetSymbolRoutine;
    CHAR                 Symbol[64];
    DWORD                Displacement;
    BOOL                 b;

    BYTE    szBuff[256];
    INT     i;
    BYTE    ajCrtc[0x65];
    DWORD   dwAddr;

    UNREFERENCED_PARAMETER(hCurrentThread);
    UNREFERENCED_PARAMETER(dwCurrentPc);

    lpOutputRoutine        = lpExtensionApis->lpOutputRoutine;
    lpGetExpressionRoutine = lpExtensionApis->lpGetExpressionRoutine;
    lpGetSymbolRoutine     = lpExtensionApis->lpGetSymbolRoutine;

    //
    // Evaluate the argument string to get the address of
    // the Line Structure
    //

    dwAddr = (lpGetExpressionRoutine)(lpArgumentString);
    if (!dwAddr) {
        return;
    }

    //
    // Get the symbolic name
    //

    (lpGetSymbolRoutine)((LPVOID)dwAddr, Symbol, &Displacement);

    //
    // Read from the debuggees address space into our own.
    //

    b = ReadProcessMemory(hCurrentProcess,
                          (LPVOID)dwAddr,
                          ajCrtc,
                          sizeof(ajCrtc),
                          NULL);

    if (!b) {
        return;
    }

    for (i = 0; i < 0x65; i++)
    {
        sprintf(szBuff, "%4.4x: %2.2x\n", i, ajCrtc[i]);
        (lpOutputRoutine)(szBuff);
    }


#endif // DBG

    return;
}





/*****************************************************************************
 *
 *   Routine Description:
 *
 *       This function is called as an NTSD extension to dump a LineState
 *
 *   Arguments:
 *
 *       hCurrentProcess - Supplies a handle to the current process (at the
 *           time the extension was called).
 *
 *       hCurrentThread - Supplies a handle to the current thread (at the
 *           time the extension was called).
 *
 *       CurrentPc - Supplies the current pc at the time the extension is
 *           called.
 *
 *       lpExtensionApis - Supplies the address of the functions callable
 *           by this extension.
 *
 *       lpArgumentString - the float to display
 *
 *   Return Value:
 *
 *       None.
 *
 ***************************************************************************/
VOID dls(
    HANDLE hCurrentProcess,
    HANDLE hCurrentThread,
    DWORD dwCurrentPc,
    PNTSD_EXTENSION_APIS lpExtensionApis,
    LPSTR lpArgumentString)
{

#if DBG

    PNTSD_OUTPUT_ROUTINE lpOutputRoutine;
    PNTSD_GET_EXPRESSION lpGetExpressionRoutine;
    PNTSD_GET_SYMBOL     lpGetSymbolRoutine;
    CHAR                 Symbol[64];
    DWORD                Displacement;
    BOOL                 b;

    DWORD       dwAddr;
    LINESTATE   ls;
    BYTE        szBuff[256];

    UNREFERENCED_PARAMETER(hCurrentThread);
    UNREFERENCED_PARAMETER(dwCurrentPc);

    lpOutputRoutine        = lpExtensionApis->lpOutputRoutine;
    lpGetExpressionRoutine = lpExtensionApis->lpGetExpressionRoutine;
    lpGetSymbolRoutine     = lpExtensionApis->lpGetSymbolRoutine;

    //
    // Evaluate the argument string to get the address of
    // the Line Structure
    //

    dwAddr = (lpGetExpressionRoutine)(lpArgumentString);
    if (!dwAddr) {
        return;
    }

    //
    // Get the symbolic name
    //

    (lpGetSymbolRoutine)((LPVOID)dwAddr, Symbol, &Displacement);

    //
    // Read from the debuggees address space into our own.
    //

    b = ReadProcessMemory(hCurrentProcess,
                          (LPVOID)dwAddr,
                          &ls,
                          sizeof(ls),
                          NULL);

    if (!b) {
        return;
    }


    sprintf(szBuff, "LineState: %8.8x\n", dwAddr);
    (lpOutputRoutine)(szBuff);

    sprintf(szBuff, "\tjAnd            : %2.2x\n", ls.jAnd);
    (lpOutputRoutine)(szBuff);

    sprintf(szBuff, "\tjXor            : %2.2x\n", ls.jXor);
    (lpOutputRoutine)(szBuff);

    sprintf(szBuff, "\tpspStart        : %4.4x\n", ls.pspStart);
    (lpOutputRoutine)(szBuff);

    sprintf(szBuff, "\tpspEnd          : %4.4x\n", ls.pspEnd);
    (lpOutputRoutine)(szBuff);

    sprintf(szBuff, "\tpsp             : %4.4x\n", ls.psp);
    (lpOutputRoutine)(szBuff);

    sprintf(szBuff, "\tspRemaining     : %4.4x\n", ls.spRemaining);
    (lpOutputRoutine)(szBuff);

    sprintf(szBuff, "\tspTotal         : %4.4x\n", ls.spTotal);
    (lpOutputRoutine)(szBuff);

    sprintf(szBuff, "\tspTotal2        : %4.4x\n", ls.spTotal2);
    (lpOutputRoutine)(szBuff);

    sprintf(szBuff, "\tspNext          : %4.4x\n", ls.spNext);
    (lpOutputRoutine)(szBuff);

    sprintf(szBuff, "\tspComplex       : %4.4x\n", ls.spComplex);
    (lpOutputRoutine)(szBuff);

    sprintf(szBuff, "\taspRtoL         : %4.4x\n", ls.aspRtoL);
    (lpOutputRoutine)(szBuff);

    sprintf(szBuff, "\taspLtoR         : %4.4x\n", ls.aspLtoR);
    (lpOutputRoutine)(szBuff);

    sprintf(szBuff, "\tulStyleMask     : %4.4x\n", ls.ulStyleMask);
    (lpOutputRoutine)(szBuff);

    sprintf(szBuff, "\txyDensity       : %4.4x\n", ls.xyDensity);
    (lpOutputRoutine)(szBuff);

    sprintf(szBuff, "\tcStyle          : %4.4x\n", ls.cStyle);
    (lpOutputRoutine)(szBuff);

    sprintf(szBuff, "\tulStyleMaskLtoR : %4.4x\n", ls.ulStyleMaskLtoR);
    (lpOutputRoutine)(szBuff);

    sprintf(szBuff, "\tulStyleMaskRtoL : %4.4x\n", ls.ulStyleMaskRtoL);
    (lpOutputRoutine)(szBuff);

    sprintf(szBuff, "\tulStartMask     : %4.4x\n", ls.ulStartMask);
    (lpOutputRoutine)(szBuff);

#endif // DBG

    return;
}

/*****************************************************************************
 *
 *   Routine Description:
 *
 *       This function is called as an NTSD extension to dump a dword
 *       (It's real function is as a prototype for other NT extensions.)
 *
 *   Arguments:
 *
 *       hCurrentProcess - Supplies a handle to the current process (at the
 *           time the extension was called).
 *
 *       hCurrentThread - Supplies a handle to the current thread (at the
 *           time the extension was called).
 *
 *       CurrentPc - Supplies the current pc at the time the extension is
 *           called.
 *
 *       lpExtensionApis - Supplies the address of the functions callable
 *           by this extension.
 *
 *       lpArgumentString - the float to display
 *
 *   Return Value:
 *
 *       None.
 *
 ***************************************************************************/
VOID dd(
    HANDLE hCurrentProcess,
    HANDLE hCurrentThread,
    DWORD dwCurrentPc,
    PNTSD_EXTENSION_APIS lpExtensionApis,
    LPSTR lpArgumentString)
{

#if DBG

    PNTSD_OUTPUT_ROUTINE lpOutputRoutine;
    PNTSD_GET_EXPRESSION lpGetExpressionRoutine;
    PNTSD_GET_SYMBOL     lpGetSymbolRoutine;
    CHAR                 Symbol[64];
    DWORD                Displacement;
    BOOL                 b;

    DWORD   dwAddr;
    DWORD   val;
    BYTE    szBuff[256];

    UNREFERENCED_PARAMETER(hCurrentThread);
    UNREFERENCED_PARAMETER(dwCurrentPc);

    lpOutputRoutine        = lpExtensionApis->lpOutputRoutine;
    lpGetExpressionRoutine = lpExtensionApis->lpGetExpressionRoutine;
    lpGetSymbolRoutine     = lpExtensionApis->lpGetSymbolRoutine;

    //
    // Evaluate the argument string to get the address of
    // the Line Structure
    //

    dwAddr = (lpGetExpressionRoutine)(lpArgumentString);
    if (!dwAddr) {
        return;
    }

    //
    // Get the symbolic name
    //

    (lpGetSymbolRoutine)((LPVOID)dwAddr, Symbol, &Displacement);

    //
    // Read from the debuggees address space into our own.
    //

    b = ReadProcessMemory(hCurrentProcess,
                          (LPVOID)dwAddr,
                          &val,
                          sizeof(val),
                          NULL);

    if (!b) {
        return;
    }

    sprintf(szBuff, "%8.8x: %8.8x\n", dwAddr, val);
    (lpOutputRoutine)(szBuff);
#endif // DBG

    return;
}

