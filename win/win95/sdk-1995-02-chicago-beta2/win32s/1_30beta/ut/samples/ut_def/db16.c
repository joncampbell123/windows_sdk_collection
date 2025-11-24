/*++

Copyright (c) 1985-92, Microsoft Corporation

Module Name:

    db16.c

Abstract:

    Win32s sample code of Universal Thunk (UT) -
    services dispatcher in 16bit side.
    This is the main source file of DB16.DLL.

--*/

#define W32SUT_16

#include <windows.h>
#include <w32sut.h>
#include "db.h"


/*
 * constants for dispatcher in 16bit side
 */

#define DB_SRV_GETVERSION   0
#define DB_SRV_SETTIME      1
#define DB_SRV_ADDUSER      2


/*
 * 16bit dispatcher function.
 * exported by DB16.DLL
 */

DWORD FAR PASCAL
UTProc(LPVOID lpBuf, DWORD dwFunc)
{

    /*
     * call 16bit DB services based on the function Id.
     */

    switch (dwFunc) {

    case DB_SRV_GETVERSION:
        return( (DWORD) DbGetVersion() );

    case DB_SRV_SETTIME:
        DbSetTime((LPDB_TIME) lpBuf);
        return(0);

    case DB_SRV_ADDUSER:
        return( (DWORD) DbAddUser( (LPDB_NAME) ((LPDWORD)lpBuf) [0] ,
                                   (DWORD)     ((LPDWORD)lpBuf) [1] ,
                                   (LPDWORD)   ((LPDWORD)lpBuf) [2]
                                 )
              );

    }

    return( 0 );
}



