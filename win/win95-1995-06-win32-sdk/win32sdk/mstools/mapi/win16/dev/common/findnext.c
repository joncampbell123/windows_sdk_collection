/*
 *  FINDNEXT.C
 *  
 *  File I/O functions that also drag in time conversion code.
 *
 *  Copyright 1993-1995 Microsoft Corporation. All Rights Reserved.
 */

#pragma warning(disable:4001)   /* single line comments */
#pragma warning(disable:4054)   /* cast function pointer to data pointer */
#pragma warning(disable:4100)   /* unreferenced formal parameter */
#pragma warning(disable:4127)   /* conditional expression is constant */
#pragma warning(disable:4201)   /* nameless struct/union */
#pragma warning(disable:4204)   /* non-constant aggregate initializer */
#pragma warning(disable:4209)   /* benign typedef redefinition */
#pragma warning(disable:4214)   /* bit field types other than int */
#pragma warning(disable:4505)   /* unreferenced local function removed */
#pragma warning(disable:4514)   /* unreferenced inline function removed */
#pragma warning(disable:4702)   /* unreachable code */
#pragma warning(disable:4704)   /* inline assembler turns off global optimizer */
#pragma warning(disable:4705)   /* statement has no effect */
#pragma warning(disable:4706)   /* assignment within conditional expression */
#pragma warning(disable:4710)   /* function not expanded */
#pragma warning(disable:4115)   /* named type def in parens */

#include <windows.h>
#pragma warning(disable:4001)   /* single line comments */
#include <windowsx.h>
#include <mapiwin.h>
#include <mapidbg.h>
#include <compobj.h>
#include <mapicode.h>
#include <mapiperf.h>
#include <_mapiwin.h>

#include <memory.h>     /* for memcpy, memset */

#pragma SEGMENT(Common)

/* cribbed from DOS.H */

struct MyDOS_find_t {
    char reserved[21];
    char attrib;
    unsigned wr_time;
    unsigned wr_date;
    long size;
    char name[13];
    };

#pragma warning(disable: 4704)      /* asm disables global optimizations */

LPVOID  PvGetDTA(void);
void    SetDTA(LPVOID pv);

#ifdef WIN16

static FILETIME ftZero = { 0, 0 };

/*
 *  FindFirstFile
 *  
 *  Limitations
 *      dwReserved0, dwReserved1 and cAlternateFileName are
 *      not supported in the WIN32_FIND_DATA structure.
 */

#pragma warning(disable: 4769)

HANDLE __export WINAPI
FindFirstFile(LPCSTR lpFileName, LPWIN32_FIND_DATA lpFindFileData)
{
    FILETIME    ft;
    WORD        segPath;
    WORD        offPath;
    WORD        segDTA;
    WORD        offDTA;
    LPVOID      pvDTASave = NULL;
    char        szFile[MAX_PATH];

    HANDLE      hfind = NULL;
    struct MyDOS_find_t FAR *pfind = NULL;


    Assert(!IsBadStringPtr(lpFileName, MAX_PATH));
    Assert(!IsBadWritePtr(lpFindFileData, sizeof(WIN32_FIND_DATA)));

    AnsiToOem(lpFileName, szFile);
    memset(lpFindFileData, -1, sizeof(WIN32_FIND_DATA));

    if (!(hfind = GlobalAlloc(GMEM_FIXED, sizeof(struct MyDOS_find_t))) ||
        !(pfind = (struct MyDOS_find_t FAR *)GlobalLock(hfind)))
    {
        DebugTrace("FindFirstFile: no memory for DOS finddata\n");
        return INVALID_HANDLE_VALUE;
    }

    segPath = SELECTOROF(szFile);
    offPath = OFFSETOF(szFile);
    segDTA = SELECTOROF(pfind);
    offDTA = OFFSETOF(pfind);

    pvDTASave = PvGetDTA();
    Assert(pvDTASave);
    SetDTA(pfind);

    _asm
    {
        push ds
        mov cx, 10h                 ; include subdirectories
        mov dx, segPath             ; file name
        mov ds, dx
        mov dx, offPath
        mov ah, 4eh                 ; Find First File
        call DOS3Call
        pop ds

        jnc fff10
        jmp fail

fff10:
            ; else fall through

    }

    lpFindFileData->dwFileAttributes = pfind->attrib;
    OemToAnsi(pfind->name, lpFindFileData->cFileName);

    if (!DosDateTimeToFileTime ((WORD)pfind->wr_date, (WORD)pfind->wr_time, &ft)
     || !LocalFileTimeToFileTime (&ft, &lpFindFileData->ftLastAccessTime))
    {
        //  Time was bogus. This can happen, for instance, on Netware.
        //  Return zeroes but do not fail.
        lpFindFileData->ftLastAccessTime = ftZero;
        lpFindFileData->ftCreationTime = ftZero;
        lpFindFileData->ftLastWriteTime = ftZero;
    }
    else
    {
        lpFindFileData->ftCreationTime = lpFindFileData->ftLastAccessTime;
        lpFindFileData->ftLastWriteTime = lpFindFileData->ftLastAccessTime;
    }

    lpFindFileData->nFileSizeHigh = 0;
    lpFindFileData->nFileSizeLow = pfind->size;

    SetDTA(pvDTASave);
    GlobalUnlock(hfind);

    return hfind;

fail:
    SetDTA(pvDTASave);
    if (hfind)
    {
        if (pfind)
            GlobalUnlock(hfind);
        GlobalFree(hfind);
    }

#ifdef  DEBUG
    if (GetLastError() != ERROR_NO_MORE_FILES)
        DebugTraceDos(FindFirstFile);
#endif  
    return INVALID_HANDLE_VALUE;
}

/*
 *  FindNextFile
 *  
 *  Limitations
 *      dwReserved0, dwReserved1 and cAlternateFileName are
 *      not supported in the WIN32_FIND_DATA structure.
 */
BOOL __export WINAPI
FindNextFile(HANDLE hFindFile, LPWIN32_FIND_DATA lpFindFileData)
{
    FILETIME    ft;
    LPVOID      pvDTASave = NULL;
    struct MyDOS_find_t FAR *pfind = NULL;
    BOOL        f = FALSE;

    if (!GlobalSize(hFindFile) ||
        !(pfind = (struct MyDOS_find_t FAR *)GlobalLock(hFindFile)))
    {
        DebugTraceArg(FindNextFile, "Invalid find handle");
        return FALSE;
    }

    Assert(!IsBadWritePtr(lpFindFileData, sizeof(WIN32_FIND_DATA)));

    memset(lpFindFileData, -1, sizeof(WIN32_FIND_DATA));

    pvDTASave = PvGetDTA();
    Assert(pvDTASave);
    SetDTA(pfind);

    _asm
    {
        mov ah, 4fh                 ; Find Next File
        call DOS3Call
        jc fnfErr10

        mov f, TRUE                 ; signal success

fnfErr10:
    }

    if (!f)
    {
#ifdef  DEBUG
        if (GetLastError() != ERROR_NO_MORE_FILES)
            DebugTraceDos(FindNextFile);
#endif  
        goto fail;
    }

    lpFindFileData->dwFileAttributes = pfind->attrib;
    OemToAnsi(pfind->name, lpFindFileData->cFileName);

    if (!DosDateTimeToFileTime ((WORD)pfind->wr_date, (WORD)pfind->wr_time, &ft)
     || !LocalFileTimeToFileTime (&ft, &lpFindFileData->ftLastAccessTime))
    {
        //  Time was bogus. This can happen, for instance, on Netware.
        //  Return zeroes but do not fail.
        lpFindFileData->ftLastAccessTime = ftZero;
        lpFindFileData->ftCreationTime = ftZero;
        lpFindFileData->ftLastWriteTime = ftZero;
    }
    else
    {
        lpFindFileData->ftCreationTime = lpFindFileData->ftLastAccessTime;
        lpFindFileData->ftLastWriteTime = lpFindFileData->ftLastAccessTime;
    }

    lpFindFileData->nFileSizeHigh = 0;
    lpFindFileData->nFileSizeLow = pfind->size;

    SetDTA(pvDTASave);
    GlobalUnlock(hFindFile);

    return TRUE;

fail:
    SetDTA(pvDTASave);
    if (pfind)
        GlobalUnlock(hFindFile);
    return FALSE;
}

/*
 *  FindClose
 */
BOOL __export WINAPI
FindClose(HANDLE hFindFile)
{
    BOOL    f;

    if (hFindFile == INVALID_HANDLE_VALUE)
        return 0;

    f = (GlobalSize(hFindFile) != 0L);
    GlobalFree(hFindFile);
    return f;
}

#pragma warning(default: 4769)


/*
 *  GetFileTime
 *  
 *  Limitations
 *      Only last-modified time is supported; no create or access
 *      time. Fails if either of the latter is requested.
 */
BOOL __export WINAPI
GetFileTime(HANDLE hFile, FILETIME FAR *lpftCreation, FILETIME FAR *lpftLastAccess,
    FILETIME FAR *lpftLastWrite)
{
    FILETIME    ftLocal;
    WORD        wDate;
    WORD        wTime;

    Assert(!IsBadWritePtr(lpftLastWrite, sizeof(FILETIME)));

    if (lpftCreation || lpftLastAccess)
    {
        DebugTraceArg(GetFileTime, "lpftCreation and lpftLastAccess are unsupported");
        return FALSE;
    }

    _asm {
        mov bx, hFile
        mov ax, 5700h               ; Get File Date and Time
        call DOS3Call
        jc gftErr

        mov wDate, dx
        mov wTime, cx
    }

    if (!DosDateTimeToFileTime(wDate, wTime, &ftLocal))
        goto gftErr;
    return LocalFileTimeToFileTime (&ftLocal, lpftLastWrite);

gftErr:
    DebugTraceDos(GetFileTime);
    return FALSE;
}

#pragma warning(disable: 4035)      /* no return value */

LPVOID
PvGetDTA()
{
    _asm
    {
        mov ah, 2fh                 ; Get Disk Transfer Address
        call DOS3Call
                                    ; no failure report
        mov dx, es                  ; return as long
        mov ax, bx
    }
}

#pragma warning(default: 4035)      /* no return value */

void
SetDTA(LPVOID pvDTA)
{
    WORD    segDTA;
    WORD    offDTA;

    Assert(pvDTA);

    segDTA = SELECTOROF(pvDTA);
    offDTA = OFFSETOF(pvDTA);

    _asm {
        push ds
        mov dx, segDTA
        mov ds, dx
        mov dx, offDTA
        mov ah, 1ah                 ; Set Disk Transfer Address
        call DOS3Call
                                    ; no failure report
        pop ds
    }
}



#endif  /* WIN16 */
