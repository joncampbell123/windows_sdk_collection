/*
 *  MAPIWIN.C
 *
 *  Support for cross-platform development (WIN16 and WIN32) of MAPI
 *  components and service providers.
 *
 *  Most functions have some limitations relative to their WIN32
 *  counterparts. Such limitations are noted in the comments to the
 *  function. Where possible, if the caller requests unsupported
 *  functionality, the function fails.
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
#include <memory.h>     /*  for memcpy, memset */

#include <mapiwin.h>
#include <mapidbg.h>
#include <compobj.h>
#include <mapicode.h>
#include <mapiutil.h>
#include <_mapiu.h>
#include <mapiperf.h>

#include <_mapiwin.h>

#include <stdlib.h>

#ifdef DBCS
#ifdef DOS
#include <gapidos.h>
#else
#include <mapigapi.h>
#endif
#endif

#pragma SEGMENT(MAPI_Util)

#pragma warning(disable: 4704)      /* asm disables global optimizations */

#ifdef  WIN16

/*
 *  GetLastError
 *
 *  Limitations:
 *      It seems likely that some of the DOS and CRT functions used
 *      elsewhere in this module won't set the DOS extended error
 *      info, so this function won't be as reliable as it is on NT.
 *
 *      If a function in this module fails because an unsupported
 *      feature was requested, no error is set.
 */
DWORD __export WINAPI
GetLastError()
{
    WORD    w;

    _asm {
        push bp
        push ds
        push si
        push di

        mov ah, 59h         ; Get Extended Error
        call DOS3Call
                            ; no failure report
        mov w, ax

        pop di
        pop si
        pop ds
        pop bp
    }

    return (DWORD)w;
}

/*
 *  GetFileAttributes
 *
 *  Limitations:
 *      Won't work on Netware without FILESCAN rights.
 */
DWORD __export WINAPI
GetFileAttributes(LPCSTR lpFileName)
{
    WORD w;
    WORD x;
    char szFile[MAX_PATH];

    Assert(!IsBadStringPtr(lpFileName, MAX_PATH));
    AnsiToOem(lpFileName, szFile);

    w = SELECTOROF(szFile);
    x = OFFSETOF(szFile);

    _asm
    {
        push ds
        mov dx, w
        mov ds, dx
        mov dx, x
        mov ax, 4300h               ; Get File Attributes
        call DOS3Call
        pop ds                      ; doesn't affect flags
        jc gfaErr

        mov w, cx                   ; here are attributes
    }
    return (DWORD)w;

gfaErr:
    DebugTraceDos(GetFileAttributes);
    return (DWORD)-1;
}


/*
 *  GetFileSize
 */
DWORD __export WINAPI
GetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh)
{
    WORD    wLow;
    WORD    wHigh;

    if (lpFileSizeHigh)
    {
        Assert(!IsBadWritePtr(lpFileSizeHigh, sizeof(DWORD)));
        *lpFileSizeHigh = 0L;
    }

    _asm {
        mov bx, hFile
        xor dx, dx                  ; seek offset 0L
        xor cx, cx
        mov ax, 4201h               ; current position
                                    ; Move File Pointer
        call DOS3Call
        jc gfsErr

        push ax                     ; save current position
        push dx

        xor dx, dx                  ; seek offset 0L
        xor cx, cx
        mov ax, 4202h               ; end of file
                                    ; Move File Pointer
        call DOS3Call
        jc gfsErr1

        mov wLow, ax                ; save file size
        mov wHigh, dx

        pop cx
        pop dx                      ; restore original position
        mov ax, 4200h               ; beginning of file
                                    ; Move File Pointer
        call DOS3Call
        jc gfsErr
        jmp gfsDone

gfsErr1:
        pop ax                      ; clean stack
        pop ax
gfsErr:
        xor ax, ax
        dec ax
        mov wLow, ax                ; return error
        mov wHigh, ax

gfsDone:
    }

#ifdef  DEBUG
    if (wLow + wHigh == 0)
        DebugTraceDos(GetFileSize);
#endif
    return wLow | ((DWORD)wHigh << 16);
}

/*
 *  CreateFile
 *
 *  Limitations
 *      dwDesiredAccess is ignored.
 *      lpSecurityAttributes is not supported, fails if not NULL.
 *      dwCreationDisposition is partially supported:
 *          handles CREATE_ALWAYS, OPEN_ALWAYS, TRUNCATE_EXISTING,
 *              OPEN_EXISTING
 *          does not handle CREATE_NEW
 *      dwFlagsAndAttributes is ignored.
 *      hTemplateFile is not supported, fails if nonzero.
 */
HANDLE __export WINAPI
CreateFile(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
    LPVOID lpSecurityAttributes, DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
    int hf = -1;
    BYTE b = 0;
    WORD w;
    WORD segPath;
    WORD offPath;

    if (lpSecurityAttributes || hTemplateFile)
    {
        DebugTraceArg(CreateFile, "lpSecurityAttributes and hTemplateFile are unsupported");
        return INVALID_HANDLE_VALUE;
    }

    Assert(!IsBadStringPtr(lpFileName, MAX_PATH));

    segPath = SELECTOROF(lpFileName);
    offPath = OFFSETOF(lpFileName);

    /*  Map the MAPIWIN.H definitions to the underlying */
    /*  access modes */

    if ((dwDesiredAccess == 0) || (dwDesiredAccess == GENERIC_READ))
        b = 0x00;                                           /* OPEN_ACCESS_READONLY */
    else if (dwDesiredAccess == GENERIC_WRITE)
        b = 0x01;                                           /* OPEN_ACCESS_WRITEONLY */
    else if (dwDesiredAccess == GENERIC_READ|GENERIC_WRITE)
        b = 0x02;                                           /* OPEN_ACCESS_READWRITE */

    if (dwShareMode == 0)
        b |= 0x10;                                          /* OPEN_SHARE_DENYREADWRITE */
    else if (dwShareMode == FILE_SHARE_READ)
        b |= 0x20;                                          /* OPEN_SHARE_DENYWRITE */
    else if (dwShareMode == FILE_SHARE_WRITE)
        b |= 0x30;                                          /* OPEN_SHARE_DENYREAD */
    else if (dwShareMode == FILE_SHARE_READ|FILE_SHARE_WRITE)
    {
        /*  We use OPEN_SHARE_DENYNONE because an */
        /*  attempt to open the file in compatability mode fails if the file */
        /*  had been opened in any other sharing mode.  Also if the file */
        /*  has been opened in compatability mode attempting to open the  */
        /*  file in any of the sharing modes will fail. */
        
        b |= 0x40;                                          /* OPEN_SHARE_DENYNONE */
    }

    if (dwCreationDisposition == CREATE_ALWAYS)
        goto doCreate;

    _asm
    {
        push ds

        mov dx, segPath             ; file name
        mov ds, dx
        mov dx, offPath
        mov al, b                   ; access mode
        mov ah, 3dh                 ; Open File
        call DOS3Call
        pop ds
        jc cfErr10

        mov hf, ax                  ; file handle

cfErr10:
    }

    if (dwCreationDisposition == CREATE_NEW)
    {
        if (hf == HFILE_ERROR)
            goto doCreate;
        else
        {
            CloseHandle(hf);
            hf = HFILE_ERROR;
            goto ret;
        }
    }
    /*  If caller only wanted to open an existing file, give up. */
    /*  OPEN_EXISTING doesn't need any explicit code */
    else if (dwCreationDisposition == OPEN_ALWAYS)
    {
        if (hf == HFILE_ERROR)
            goto doCreate;
        else
            goto ret;
    }
    else if (dwCreationDisposition == OPEN_EXISTING)
    {
            goto ret;
    }

    /*  Set file size to 0 if requested. */
    if (hf != HFILE_ERROR && dwCreationDisposition == TRUNCATE_EXISTING)
    {
        _asm
        {
            mov bx, hf
            xor dx, dx              ; seek offset 0L
            xor cx, cx
            mov ax, 4200h           ; beginning of file
                                    ; Move File Pointer
            call DOS3Call
            jc cfErr20

            push ds

            mov bx, hf
            xor cx, cx              ; write 0 bytes
            mov dx, segPath
            mov ds, dx
            mov dx, offPath
            mov ah, 40h             ; Write File
            call DOS3Call
            pop ds
            jnc cf30

cfErr20:
            mov bx, hf
            mov ah, 3eh             ; Close File
            call DOS3Call
            mov hf, -1

cf30:
        }
    }
    goto ret;

    /*  Try to create a new file. */

doCreate:
    /*  Get the file attributes */
    if ((w = (WORD)dwFlagsAndAttributes) == FILE_ATTRIBUTE_NORMAL)
        w = 0;
    else if (w & FILE_ATTRIBUTE_NORMAL)
        w &= ~FILE_ATTRIBUTE_NORMAL;

    _asm
    {
        push ds

        mov dx, segPath             ; path name
        mov ds, dx
        mov dx, offPath
        mov cx, w                   ; file attributes
        mov ah, 3ch                 ; Create File
        call DOS3Call
        pop ds
        jc cfErr40

        mov hf, ax
        jmp cf50

cfErr40:
        mov hf, -1
cf50:
    }

    /*  When a file is created via 3Ch.  The file is */
    /*  "opened" with read/write share compatibility. */
    /*  If any other mode was requested, we have to */
    /*  close the file and re-open it. */
    
    if ((hf != -1) &&
        !(dwShareMode == (FILE_SHARE_WRITE | FILE_SHARE_READ)))
    {
        CloseHandle(hf);
        hf = -1;
        _asm
        {
            push ds
            mov dx, segPath             ; file name
            mov ds, dx
            mov dx, offPath
            mov al, b                   ; access mode
            mov ah, 3dh                 ; Open File
            call DOS3Call
            pop ds
            jc cfErr50
            mov hf, ax                  ; file handle
cfErr50:
        }
        if (hf == -1)
            DeleteFile (lpFileName);
    }

ret:
#ifdef  DEBUG
    if (hf == -1)
        DebugTraceDos(CreateFile);
#endif
    return (HANDLE)hf;
}

/* OpenFile() is the same in both Win16 and Win32. */

/*
 *  ReadFile
 *
 *  Limitations
 *      Count is limited to 64K.
 *      lpOverlapped is not supported, fails if not NULL.
 */
BOOL __export WINAPI
ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead,
    LPDWORD lpNumberOfBytesRead, LPVOID lpOverlapped)
{
    WORD    w;
    WORD    segBuf;
    WORD    offBuf;
    BOOL    f;
    BYTE __huge *pb;
    BYTE __far *pbT;

    if (lpOverlapped)
    {
        DebugTraceArg(ReadFile, "lpOverlapped is unsupported");
        return FALSE;
    }

    pb = (BYTE __huge *) lpBuffer;

#ifdef  DEBUG
    Assert(!IsBadHugeWritePtr(pb, nNumberOfBytesToRead));
    Assert(!IsBadWritePtr(lpNumberOfBytesRead, sizeof(DWORD)));

    if (nNumberOfBytesToRead > 0x10000)
        DebugTrace("ReadFile: huge write 0x%lx\n", nNumberOfBytesToRead);
#endif

    *lpNumberOfBytesRead = 0L;

    while (nNumberOfBytesToRead)
    {
        w = (WORD) min(nNumberOfBytesToRead, 0xffff);
        pbT = (BYTE __far *) pb;
        segBuf = SELECTOROF(pbT);
        offBuf = OFFSETOF(pbT);
        f = TRUE;

        _asm
        {
            push ds

            mov bx, hFile
            mov cx, w                   ; read count
            mov dx, segBuf              ; buffer address
            mov ds, dx
            mov dx, offBuf
            mov ah, 3fh                 ; Read File
            call DOS3Call
            pop ds
            jc rfErr10

            mov w, ax                   ; returned byte count
            jmp rf20

rfErr10:
            mov f, FALSE

rf20:
        }

        if (f)
        {
            *lpNumberOfBytesRead += (DWORD)w;
            if (w != (WORD) min(nNumberOfBytesToRead, 0xffff))
            {
                /* short read, assume we're done */
                break;
            }

            pb += w;
            nNumberOfBytesToRead -= w;
        }
        else
        {
#ifdef  DEBUG
            DebugTraceDos(ReadFile);
#endif
            break;
        }
    }

    return f;
}

/*
 *  WriteFile
 *
 *  Limitations
 *      lpOverlapped is not supported, fails if not NULL.
 */
BOOL __export WINAPI
WriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite,
    LPDWORD lpNumberOfBytesWritten, LPVOID lpOverlapped)
{
    WORD    w;
    WORD    segBuf;
    WORD    offBuf;
    BOOL    f;
    BYTE __huge *pb;
    BYTE far *pbT;

    if (lpOverlapped)
    {
        DebugTraceArg(WriteFile, "lpOverlapped is unsupported");
        return FALSE;
    }

    pb = (BYTE __huge *) lpBuffer;

#ifdef  DEBUG
    Assert(!IsBadHugeReadPtr(pb, nNumberOfBytesToWrite));
    Assert(!IsBadWritePtr(lpNumberOfBytesWritten, sizeof(DWORD)));

    if (nNumberOfBytesToWrite > 0x10000)
        DebugTrace("WriteFile: huge write 0x%lx\n", nNumberOfBytesToWrite);
#endif

    *lpNumberOfBytesWritten = 0L;

    while (nNumberOfBytesToWrite)
    {
        w = (WORD) min(nNumberOfBytesToWrite, 0xffff);
        pbT = (void far *) pb;
        segBuf = SELECTOROF(pbT);
        offBuf = OFFSETOF(pbT);
        f = TRUE;

        _asm
        {
            push ds

            mov bx, hFile
            mov cx, w                   ; byte count
            mov dx, segBuf              ; buffer address
            mov ds, dx
            mov dx, offBuf
            mov ah, 40h                 ; Write File
            call DOS3Call
            pop ds
            jc wfErr10

            mov w, ax                   ; returned byte count
            jmp wf20

wfErr10:
            mov f, FALSE

wf20:
        }

        if (f)
        {
            *lpNumberOfBytesWritten += (DWORD)w;
            if (w != (WORD) min(nNumberOfBytesToWrite, 0xffff))
            {
                /* We got a short write, bail. */
                break;
            }
            pb += w;
            nNumberOfBytesToWrite -= w;
        }
        else
        {
#ifdef  DEBUG
            DebugTraceDos(WriteFile);
#endif
            break;
        }
    }

    return f;
}

/*
 *  SetFilePointer
 *
 *  Limitations
 *      Distance is limited to 2GB (signed 32-bits). Fails if
 *      lpDistanceToMoveHigh is present and nonzero (unless it's the
 *      sign extension of a negative distance).
 */
DWORD __export WINAPI
SetFilePointer(HANDLE hFile, LONG lDistanceToMove, LONG FAR *lpDistanceToMoveHigh,
    DWORD dwMoveMethod)
{
    WORD    w;
    WORD    x;
    BYTE    b;

    if (lpDistanceToMoveHigh && *lpDistanceToMoveHigh)
    {
        if ((*lpDistanceToMoveHigh != 0xFFFFFFFF) ||
            !(lDistanceToMove & 0x80000000))
        {
            DebugTraceArg(SetFilePointer, "seeks > 4 gig are not supported");
            return 0xFFFFFFFF;
        }
    }

    if (dwMoveMethod > 2)
    {
        DebugTraceArg(SetFilePointer, "dwMoveMethod unknown");
        return 0xFFFFFFFF;
    }

    w = LOWORD(lDistanceToMove);
    x = HIWORD(lDistanceToMove);
    b = (BYTE)dwMoveMethod;

    _asm
    {
        mov bx, hFile
        mov dx, w                   ; seek offset
        mov cx, x
        mov al, b                   ; seek origin
        mov ah, 42h                 ; Move File Pointer
        call DOS3Call
        jc sfpErr10

        mov w, ax
        mov x, dx
        jmp sfp20

sfpErr10:
        xor ax, ax
        dec ax
        mov w, ax
        mov x, ax

sfp20:
    }

#ifdef  DEBUG
    if ((w | ((DWORD)x << 16)) ==  0xFFFFFFFF)
        DebugTraceDos(SetFilePointer);
#endif
    return w | ((DWORD)x << 16);
}

/*
 *  SetEndOfFile
 *
 *  Limitations
 *      Certain SMB file servers, particularly UNIX-based ones, don't
 *      recognize this DOS idiom as a rqeuest to set the end of
 *      file mark.
 */
BOOL __export WINAPI
SetEndOfFile(HANDLE hFile)
{
    BYTE bBuf;
    WORD w = SELECTOROF(&bBuf);
    WORD x = OFFSETOF(&bBuf);
    BOOL f = TRUE;

    _asm
    {
        push ds                     ;

        mov bx, hFile               ;
        mov dx, w                   ; buffer address
        mov ds, dx
        mov dx, x
        xor cx, cx                  ; byte count = 0: set EOF
        mov ah, 40h                 ; Write File
        call DOS3Call
        pop ds
        jnc seof20

        mov f, FALSE                ; report error

seof20:
    }

#ifdef  DEBUG
    if (!f)
        DebugTraceDos(SetEndOfFile);
#endif
    return f;
}

/*
 *  CloseHandle
 *
 *  Limitations
 *      Don't expect it to close anything other than file handles.
 */
BOOL __export WINAPI
CloseHandle(HANDLE hObject)
{
    BOOL    f = TRUE;

    _asm
    {
        mov bx, hObject             ; file handle
        mov ah, 3eh                 ; Close File
        call DOS3Call
        jnc ch10

        mov f, FALSE                ; failed
ch10:
    }

#ifdef  DEBUG
    if (!f)
        DebugTraceDos(CloseHandle);
#endif
    return f;
}

/* //$  REVIEW not sure whether GetDOSEnvironment returns OEM or ANSI */
/* //$  charset. Assuming OEM. */
STDAPI_(LPSTR)
SzGetEnv(LPSTR szName)
{
    LPSTR   sz;
    LPSTR   szDst;
    CHAR    rgch[128];

    Assert(!IsBadStringPtr(szName, 128));

    if (!(sz = GetDOSEnvironment()))
    {
        DebugTrace("SzGetEnv: GetDOSEnvironment() failed\n");
        return NULL;
    }

    while (*sz)
    {
        szDst = rgch;
#ifdef DBCS
        while (*sz && (!FGLeadByte(*sz) && *sz != '='))
        {
            if(FGLeadByte(*sz))
                *szDst++ = *sz++;
            *szDst++ = *sz++;
        }
#else
        while (*sz && *sz != '=')
            *szDst++ = *sz++;
#endif
        if (*sz == '=')
            *szDst++ = 0;
        OemToAnsi(rgch, rgch);      /* to match the argument */

        if (!lstrcmpi(szName, rgch))
            return sz + 1;

        sz += lstrlen(sz) + 1;
    }

    return NULL;
}

/*
 *  GetTempPath
 *
 *  Limitations
 *      Uses the environment, not anything intrinsic to the system.
 *      I mean, basically, we're guessing.
 *      Buffer length limited to 64K (yawn).
 */
DWORD __export WINAPI
GetTempPath(DWORD nBufferLength, LPSTR lpBuffer)
{
    char *sz;
    
    AssertSz( lpBuffer && !IsBadWritePtr( lpBuffer, (UINT)nBufferLength ),
            "lpBuffer fails address check" );

    if (sz = SzGetEnv("TEMP"))
        lstrcpy(lpBuffer, sz);
    else if (sz = SzGetEnv("TMP"))
        lstrcpy(lpBuffer, sz);
    else
        lstrcpy(lpBuffer, "C:\\");

    //  Guarantee that the path has a trailing backslash
    sz = SzFindLastCh(lpBuffer, '\\');
    if (!sz || *++sz != 0)              //  DBCS OK
        lstrcat(lpBuffer, "\\");

    OemToAnsi(lpBuffer, lpBuffer);
    return (DWORD)lstrlen(lpBuffer);
}


/*
 *  GetTempFileName32
 *
 *  Limitations:
 *      Not implemented.
 */
UINT __export WINAPI
GetTempFileName32 (LPCSTR lpPathName, LPCSTR lpPrefixString, UINT uUnique,
    LPSTR lpTempFileName)
{
    int ich;
    UINT nExt;
    UINT nStop = 0;
    OFSTRUCT ofs;
    char rgch[MAX_PATH];

    HANDLE hf = NULL;

    if (lpPathName == NULL)
        return FALSE;
        
    AssertSz( !IsBadStringPtr( lpPathName, MAX_PATH ),
            "lpPathName fails address check" );

    //  The prefix string is optionally NULL
    //
    AssertSz( !lpPrefixString || !IsBadStringPtr( lpPrefixString, MAX_PATH ),
            "lpPrefixName fails address check" );
        
    AssertSz( !IsBadWritePtr( lpTempFileName, MAX_PATH ),
            "lpTempFileName fails address check" );

    ich = lstrlen (lpPathName);

    if (!uUnique)
    {
        nExt = (UINT)((WORD)GetTickCount ());
        nStop = nExt - 1;
    }
    else
        nExt = uUnique;

    for (; nExt != nStop; nExt++)
    {
#ifdef DBCS
        if ((*SzGPrev(lpPathName, lpPathName + ich) != '\\') &&
            (*SzGPrev(lpPathName, lpPathName + ich) != ':' ))
#else
        if ((lpPathName[ich - 1] != '\\') && (lpPathName[ich - 1] != ':'))
#endif
            wsprintf (rgch, "%s\\%.3s%04X.TMP", lpPathName, lpPrefixString, nExt);
        else
            wsprintf (rgch, "%s%.3s%04X.TMP", lpPathName, lpPrefixString, nExt);

        hf = OpenFile (rgch, &ofs, OF_EXIST);
        if ((hf == HFILE_ERROR) || uUnique)
            break;
    }

    if (((hf == HFILE_ERROR) && (ofs.nErrCode == 0x0002)) || uUnique)
    {
        lstrcpy (lpTempFileName, rgch);
        return nExt;
    }

    return 0;
}


/*
 *  DeleteFile
 */
BOOL __export WINAPI
DeleteFile(LPCSTR lpFileName)
{
    char    szPath[MAX_PATH];
    WORD    segPath;
    WORD    offPath;
    BOOL    f = TRUE;

    Assert(!IsBadStringPtr(lpFileName, MAX_PATH));
    AnsiToOem(lpFileName, szPath);

    segPath = SELECTOROF(szPath);
    offPath = OFFSETOF(szPath);

    _asm
    {
        push ds

        mov dx, segPath             ; path name
        mov ds, dx
        mov dx, offPath
        mov ah, 41h                 ; Delete File
        call DOS3Call
        pop ds
        jnc df10

        mov f, FALSE                ; report error
df10:
    }

#ifdef  DEBUG
    if (!f)
        DebugTraceDos(DeleteFile);
#endif
    return f;
}

/*
 *  CreateDirectory
 *
 *  Limitations:
 *      lpSecurityAttribtues not supported, fails if not NULL.
 */
BOOL __export WINAPI
CreateDirectory(LPCSTR lpPathName, LPVOID lpSecurityAttributes)
{
    char    szPath[MAX_PATH];
    WORD    segPath = SELECTOROF(lpPathName);
    WORD    offPath = OFFSETOF(lpPathName);
    BOOL    f = TRUE;

    if (lpSecurityAttributes)
    {
        DebugTraceArg(CreateDirectory, "lpSecurityAttributes is unsupported");
        return FALSE;
    }

    Assert(!IsBadStringPtr(lpPathName, MAX_PATH));
    AnsiToOem(lpPathName, szPath);

    segPath = SELECTOROF(szPath);
    offPath = OFFSETOF(szPath);

    _asm
    {
        push ds

        mov dx, segPath             ; path name
        mov ds, dx
        mov dx, offPath
        mov ah, 39h                 ; Create Directory
        call DOS3Call
        pop ds
        jnc cd10

        mov f, FALSE                ; report error
cd10:
    }

#ifdef  DEBUG
    if (!f)
        DebugTraceDos(CreateDirectory);
#endif
    return f;
}

/*
 *  RemoveDirectory
 */
BOOL __export WINAPI
RemoveDirectory(LPCSTR lpPathName)
{
    char    szPath[MAX_PATH];
    WORD    segPath;
    WORD    offPath;
    BOOL    f = TRUE;

    Assert(!IsBadStringPtr(lpPathName, MAX_PATH));
    AnsiToOem(lpPathName, szPath);

    segPath = SELECTOROF(szPath);
    offPath = OFFSETOF(szPath);

    _asm
    {
        push ds

        mov dx, segPath             ; path name
        mov ds, dx
        mov dx, offPath
        mov ah, 3ah                 ; Remove Directory
        call DOS3Call
        pop ds
        jnc rd10

        mov f, FALSE                ; report error
rd10:
    }

#ifdef  DEBUG
    if (!f)
        DebugTraceDos(RemoveDirectory);
#endif
    return f;
}

/*
 *  GetFullPathName
 *
 *  input: lpFileName, a relative path
 *  output: lpBuffer contains the matching absolute path, including
 *  the drive; *lpFilePart points to the last node of the path in lpBuffer.
 *
 *  Returns the length of the string in lpBuffer. If the buffer is too small,
 *  returns the required size, and null string in lpBuffer.
 *
 *  Limitations
 *      Does not handle ".." path components in lpFileName.
 */
DWORD __export WINAPI
GetFullPathName(LPCSTR lpFileName, DWORD nBufferLength, LPSTR lpBuffer,
    LPSTR *lpFilePart)
{
    char        chDrive;
    char        szCwd[MAX_PATH];
    char        szSlash[2];
    char        szFile[MAX_PATH];
    WORD        w;
    LPSTR       sz = (LPSTR)szFile;
    LPSTR       szLast;
    WORD        segCwd;
    WORD        offCwd;

    if (IsBadWritePtr(lpBuffer, (UINT) nBufferLength))
    {
        DebugTraceArg(GetFullPathName, "lpBuffer fails address check");
        return 0L;
    }
    Assert(!IsBadStringPtr(lpFileName, MAX_PATH));
    AnsiToOem(lpFileName, szFile);

    *lpBuffer = 0;

    /* Get drive */
    if (lstrlen(sz) > 2 && sz[1] == ':')
    {
        chDrive = *sz;
        w = (WORD)(chDrive - 'A');
        sz += 2;
    }
    else
    {
        _asm
        {
            mov ah, 19h             ; Get Default Drive
            call DOS3Call
                                    ; no failure report
            mov w, ax
        }
        chDrive = (char)((w & 0xFF) + 'A');
    }

    /* Get directory */
    *szCwd = 0;
    szSlash[0] = szSlash[1] = 0;
    w = 0;                          /*  assume failure */
    if (*sz != '\\')
    {
        segCwd = SELECTOROF((LPSTR)&szCwd);
        offCwd = OFFSETOF((LPSTR)&szCwd);
        _asm
        {
            push ds
            push si

            mov si, segCwd          ; buffer for current directory
            mov ds, si
            mov si, offCwd
            mov dl, chDrive         ; drive ('A' - 'Z')
            sub dl, 41h             ; subtract off asciii 'A'
            inc dl                  ; zero means default drive, so add one

            mov ah, 47h             ; Get Current Directory
            call DOS3Call
            pop si
            pop ds
            jc gfpnErr10

            mov w, 1                ; signal success

gfpnErr10:
        }

        if (!w)
        {
            DebugTraceDos(GetFullPathName);
            goto ret;
        }

        if (lstrlen(szCwd) > 1)
            lstrcat(szCwd, "\\");
        if (*szCwd && *szCwd != '\\')
            szSlash[0] = '\\';
    }

    /* Build output path */
    w = (WORD)(3 + lstrlen(szCwd) + lstrlen(sz) + 1);
    if ((DWORD) w > nBufferLength)
        return w;

    wsprintf(lpBuffer, "%c:%s%s%s", chDrive, szSlash, szCwd, sz);
    OemToAnsi(lpBuffer, lpBuffer);

    /* point to filename component */
    szLast = lpBuffer + 2;
#ifdef DBCS
    for (sz = szLast; *sz; sz = SzGNext(sz))
        if (!FGLeadByte(*sz) && *sz == '\\')
            szLast = sz + 1;
#else
    for (sz = szLast; *sz; ++sz)
        if (*sz == '\\')
            szLast = sz + 1;
#endif

    *lpFilePart = szLast;

ret:
    return lstrlen(lpBuffer);
}

/*
 *  MoveFile
 *
 *  Limitations
 *      Won't move a directory. Will always work across drives
 *      //$ SPEED It always does a copy/delete.
 */
BOOL __export WINAPI
MoveFile(LPCSTR lpExistingFileName, LPCSTR lpNewFileName)
{
    Assert(!IsBadStringPtr(lpExistingFileName, MAX_PATH));
    Assert(!IsBadStringPtr(lpNewFileName, MAX_PATH));

    /*  Note: no OEM conversion, because the subroutines do it. */

    if (!CopyFile(lpExistingFileName, lpNewFileName, TRUE))
    {
        DebugTraceDos(MoveFile);
        return FALSE;
    }

    if (!DeleteFile(lpExistingFileName))
    {
        DebugTraceDos(MoveFile);
        (void)DeleteFile(lpNewFileName);
        return FALSE;
    }

    return TRUE;
}


/*
 *  CopyFile
 *
 *  Limitations
 */
BOOL __export WINAPI
CopyFile(LPCSTR szSrc, LPCSTR szDst, BOOL fFailIfExists)
{
    HANDLE  hfSrc = INVALID_HANDLE_VALUE;
    HANDLE  hfDst = INVALID_HANDLE_VALUE;
    BYTE    rgb[512];
    DWORD   dwRead;
    DWORD   dwWritten;
    OFSTRUCT of;
    BOOL    f = TRUE;

    Assert(!IsBadStringPtr(szSrc, MAX_PATH));
    Assert(!IsBadStringPtr(szDst, MAX_PATH));

    /*  Note: no OEM conversion, Open/CreateFile do that */

    if ((hfSrc = OpenFile(szSrc, &of, OF_READ)) == INVALID_HANDLE_VALUE ||
        (hfDst = CreateFile(szDst, GENERIC_READ | GENERIC_WRITE,
            0, NULL, fFailIfExists ? CREATE_NEW : CREATE_ALWAYS, 0, 0))
                == INVALID_HANDLE_VALUE)
        goto fail;
    for (;;)
    {
        if (!ReadFile(hfSrc, rgb, sizeof(rgb), &dwRead, NULL))
            goto fail;
        if (!dwRead)
            break;          /*  finished */

        if (!WriteFile(hfDst, rgb, dwRead, &dwWritten, NULL))
            goto fail;
    }

ret:
    if (hfSrc != INVALID_HANDLE_VALUE)
        CloseHandle(hfSrc);
    if (hfDst != INVALID_HANDLE_VALUE)
        CloseHandle(hfDst);
    return f;

fail:
    f = FALSE;
    DebugTraceDos(CopyFile);
    goto ret;
}

/*
 *  GetCurrentProcessId
 */
DWORD __export WINAPI
GetCurrentProcessId(void)
{
    return (DWORD)GetCurrentTask();
}

/*
 *  MulDiv32
 *
 *  Supports the MULDIV macro of mapiwin.h. Takes 32-bit arguments,
 *  unlike the native Win16 MulDiv.
 *
 *  Limitations
 *      Does not check for overflow on the multiply.
 */
long __export WINAPI
MulDiv32(long nMultiplicand, long nMultiplier, long nDivisor)
{
    FILETIME    ftProd;
    long        lQuot;
    long        lSign = 1;


    if (!nDivisor)
        return -1L;

    //  We want to use the math64.c stuff so make everything posistive
    //  and keep track of the sign of the result
    if (nDivisor < 0)
    {
        nDivisor = -nDivisor;
        lSign = -1;
    }

    if (!nMultiplier || !nMultiplicand)
        return 0L;

    //  Easy out for 16 bit case
    if (   (-((LONG) 0x8000) < nMultiplier && nMultiplier < (LONG) 0x7fff)
        && (-((LONG) 0x8000) < nMultiplicand && nMultiplicand < (LONG) 0x7fff))
    {
        return ((nMultiplier * nMultiplicand) / nDivisor);
    }

    //  We want to use the math64.c stuff so make everything posistive
    //  and keep track of the sign of the result
    if (nMultiplier < 0)
    {
        nMultiplier = -nMultiplier;
        lSign = -lSign;
    }

    if (nMultiplicand < 0)
    {
        nMultiplicand = -nMultiplicand;
        lSign = -lSign;
    }

    ftProd = FtMulDwDw( (DWORD) nMultiplicand, (DWORD) nMultiplier);

    lQuot = (long) DwDivFtDw( ftProd, (DWORD) nDivisor);

    if ((lQuot != -1) && (lSign == -1))
        lQuot = -lQuot;


    return lQuot;
}


/*
 *  FBadReadPtr
 *
 *  Functions like IsBadReadPtr, but returns FALSE if cbBytes is 0
 *  regardless of the value of lpvPtr.
 */
#undef  IsBadReadPtr
BOOL WINAPI
FBadReadPtr(const void FAR* lpvPtr, UINT cbBytes)
{
    return cbBytes > 0 && IsBadReadPtr(lpvPtr, cbBytes);
}
#define IsBadReadPtr    FBadReadPtr


/*
 *  Sleep
 *
 *  Limitations
 *      alt-TAB, alt-ESC for task switch not handled
 */

#if 1   /*  THIS IS THE WORKING CODE */

void __export WINAPI
Sleep(DWORD iMilisec)
{
    DWORD dwNow = GetCurrentTime();
    MSG msg;

    while ((GetCurrentTime() - dwNow) < iMilisec)
    {
        if (PeekMessage(&msg, NULL, WM_PAINT, WM_PAINT, PM_NOREMOVE))
        {
/*          if (msg.message == WM_QUIT || msg.message == WM_CLOSE) */
/*              return; */

            if (msg.message == WM_PAINT)
            {
                GetMessage(&msg, NULL, WM_PAINT, WM_PAINT);
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }
    return;
}

#else   /*  THIS IS THE OLD BROKEN CODE WITH "FOOTPRINTS" IN IT */

void __export WINAPI
Sleep(DWORD iMilisec)
{
    DWORD dwNow = GetCurrentTime();
    MSG msg;

    while ((GetCurrentTime() - dwNow) < iMilisec)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
        {
            if (msg.message == WM_QUIT || msg.message == WM_CLOSE)
{
DebugTrace("Sleep: caught 0x%x, exiting\n", msg.message);
                return;
}

            GetMessage(&msg, NULL, 0, 0);
            TranslateMessage(&msg);
            if (msg.message == WM_PAINT)
                DispatchMessage(&msg);
else
{
DebugTrace("Sleep: DISCARDING 0x%x !!!\n", msg.message);
}
        }
    }
    return;
}

#endif

#pragma warning(disable:4035)       /*  no return value */

/*
 *  DosGetConnection
 *  
 *  This is the non-WfW equivalent to WNetGetoOnnection.
 *  It asks the redirector to return the mapping for the drive
 *  indicated by szLocalPath.
 *  
 *  FOr internal use only - must change type to export it!
 */
UINT
DosGetConnection(LPSTR szLocal, LPSTR szRemote)
{
    WORD    segLocal = SELECTOROF(szLocal);
    WORD    offLocal = OFFSETOF(szLocal);
    WORD    segRemote = SELECTOROF(szRemote);
    WORD    offRemote = OFFSETOF(szRemote);

    _asm {
        push ds
        push es
        mov  ax, 5f02h      ; get assign-list entry
        mov  bl, 04h        ; disk device
        mov  ds, segLocal   ; local device name
        mov  si, offLocal
        mov  es, segRemote  ; remote name
        mov  di, offRemote

        call DOS3Call

        pop  es
        pop  ds
        jnc  dgc10          ; success
        mov  ax, WN_NOT_CONNECTED
        jmp  dgc20
dgc10:
        xor  ax,ax
dgc20:
    }
}

#pragma warning(default:4035)       /*  no return value */

#endif  /* WIN16 */

#pragma warning (default: 4704)
