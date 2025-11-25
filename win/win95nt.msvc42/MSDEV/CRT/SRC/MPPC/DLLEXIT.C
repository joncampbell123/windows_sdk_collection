/***
*dllmain.c - Dummy DllMain for user DLLs that have no notification handler
*
*       Copyright (c) 1993, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This object goes into LIBC.LIB and LIBCMT.LIB and MSVCRT.LIB for use
*       when linking a DLL with one of the three models of C run-time library.
*       If the user does not provide a DllMain notification routine, this
*       dummy handler will be linked in.  It always returns TRUE (success).
*
******************************************************************************/

#include <cruntime.h>
#include <macos\types.h>


/***
*DllExit - dummy version DLLs linked with all 3 C Run-Time Library models
*
*Purpose:
*       The routine DllExit is always called by _DllMainCrtExit.  If
*       the user does not provide a routine named DllExit, this one will
*       get linked in so that _DllMainCRTExit has something to call.
*
*Entry:
*
*Exit:
*
*Exceptions:
*
******************************************************************************/

void _DllExit()
{
        return;
}
