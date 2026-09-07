/***
*initcon.c - direct console I/O initialization and termination for Win32
*
*       Copyright (c) 1991-1994, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Defines __initcon() and __termcon() routines, and the corresponding
*       entries in the initialization and termination sections.
*
*       NOTE: The __initcon() and __termcon() routines are called indirectly
*       by the startup and termination code.
*
*******************************************************************************/

#include <cruntime.h>
#include <internal.h>
#include <oscalls.h>

void __cdecl __initcon(void);
void __cdecl __termcon(void);

#ifdef _MSC_VER

#pragma data_seg(".CRT$XIC")
static  _PVFV pinit = __initcon;

#pragma data_seg(".CRT$XPX")
static  _PVFV pterm = __termcon;

#pragma data_seg()

#endif  /* _MSC_VER */

/*
 * define console handles. these definitions cause this file to be linked
 * in if one of the direct console I/O functions is referenced.
 */
int _coninpfh = -1;     /* console input */
int _confh = -1;        /* console output */

/***
*void __initcon(void) - open handles for console I/O
*
*Purpose:
*       Opens handles for console input and output.
*
*Entry:
*       None.
*
*Exit:
*       No return value. If successful, handle values are copied into the
*       global variables _coninpfh and _confh.
*
*Exceptions:
*
*******************************************************************************/

void __cdecl __initcon (
        void
        )
{

#ifndef DLL_FOR_WIN32S

        _coninpfh = (int)CreateFile( "CONIN$",
                                     GENERIC_READ | GENERIC_WRITE,
                                     FILE_SHARE_READ | FILE_SHARE_WRITE,
                                     NULL,
                                     OPEN_EXISTING,
                                     0,
                                     NULL
                                    );

        _confh = (int)CreateFile( "CONOUT$",
                                  GENERIC_WRITE,
                                  FILE_SHARE_READ | FILE_SHARE_WRITE,
                                  NULL,
                                  OPEN_EXISTING,
                                  0,
                                  NULL
                                );

#endif  /* DLL_FOR_WIN32S */
}


/***
*void __termcon(void) - close console I/O handles
*
*Purpose:
*       Closes _coninpfh and _confh.
*
*Entry:
*       None.
*
*Exit:
*       No return value.
*
*Exceptions:
*
*******************************************************************************/

void __cdecl __termcon (
        void
        )
{
#ifndef DLL_FOR_WIN32S

        if ( _confh != -1 ) {
                CloseHandle( (HANDLE)_confh );
        }

        if ( _coninpfh != -1 ) {
                CloseHandle( (HANDLE)_coninpfh );
        }

#endif  /* DLL_FOR_WIN32S */
}
