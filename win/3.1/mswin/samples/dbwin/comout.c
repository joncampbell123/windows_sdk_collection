//
// COM debug output routines
//
// WARNING - PLEASE NOTE
//
// The techniques employed in this file for performing COM output are
// appropriate for debugging purposes only.
//
// The COM file is opened using DOS in the context of the DBWIN.EXE
// application, but may be written to in the context of any application
// that calls OutputDebugString() or causes some other ToolHelp notification.
//
// This is accomplished by switching the DOS PSP (which defines the DOS
// open-file context) of the currently running task to that of DBWIN.EXE,
// performing the output, then switching back.
//
// The ComOut() routine must NOT be called while inside of DOS or during
// interrupt processing.
//
#include "dbwindlp.h"

#pragma optimize("gle", off)        // suppress compiler warnings

// DOS INT 21 AH function codes
#define DOS_SETPSP  50h
#define DOS_GETPSP  51h

HFILE hfCom = -1;
HANDLE pspOpen = NULL;

BOOL ComQuery(int port)
{
    if (ComOpen(port))
    {
        ComClose();
        return TRUE;
    }
    return FALSE;
}

BOOL ComOpen(int port)
{
    // Return FALSE if port is already open.
    //
    if (hfCom != -1)
        return FALSE;

    // Save a copy of caller's PSP
    //
    _asm
    {
        mov     ah,DOS_GETPSP   ; pspOpen = GetPSP();
        int     21h
        mov     pspOpen,bx
    }

    hfCom =  _lopen(port == 2 ? "COM2" : "COM1", READ_WRITE);

    return hfCom != -1;
}

BOOL ComClose(void)
{
    BOOL fSuccess;

    _asm
    {
        mov     bx,pspOpen      ; SetPSP(pspOpen);
        mov     ah,DOS_SETPSP
        int     21h
    }

    fSuccess = (_lclose(hfCom) == NULL);

    hfCom = -1;

    return fSuccess;
}

BOOL ComOut(LPCSTR lpsz)
{
    HANDLE pspSave;
    UINT cch = lstrlen(lpsz);
    UINT cchWritten;

    if (hfCom == -1)
        return FALSE;

    _asm
    {
        mov     ah,DOS_GETPSP   ; pspSave = GetPSP();
        int     21h
        mov     pspSave,bx

        mov     bx,pspOpen      ; SetPSP(pspOpen);
        mov     ah,DOS_SETPSP
        int     21h
    }

    cchWritten = _lwrite(hfCom, lpsz, cch);

    _asm
    {
        mov     bx,pspSave      ; SetPSP(pspSave);
        mov     ah,DOS_SETPSP
        int     21h
    }

    return cch != cchWritten;
}

int ComIn(void)
{
    char ch;
    HANDLE pspSave;

    if (hfCom == -1)
        return 0;

    _asm
    {
        mov     ah,DOS_GETPSP   ; pspSave = GetPSP();
        int     21h
        mov     pspSave,bx

        mov     bx,pspOpen      ; SetPSP(pspOpen);
        mov     ah,DOS_SETPSP
        int     21h
    }

    // Read a single character.
    //
    while (_lread(hfCom, &ch, 1) == 0)
        ;

    _asm
    {
        mov     bx,pspSave      ; SetPSP(pspSave);
        mov     ah,DOS_SETPSP
        int     21h
    }

    return ch;
}

#pragma optimize("", on)
