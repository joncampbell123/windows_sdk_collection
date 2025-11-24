/*++

Copyright (c) 1985-92, Microsoft Corporation

Module Name:

    db32.c

Abstract:

    Win32s sample code of Universal Thunk (UT) -
    32bit code that provides the same services as DB.DLL by means
    of thunking to 16bit dll.
    This is the main source file of DB32.DLL

--*/


#define W32SUT_32

#include <windows.h>
#include <w32sut.h>
#include <db.h>

UT32PROC  pfnUTProc=NULL;
int       cProcessesAttached = 0;


BOOL
DllInit(HANDLE hInst, DWORD fdwReason, LPVOID lpReserved)
{

  if ( fdwReason == DLL_PROCESS_ATTACH ) {

    /*
     * Registration of UT need to be done only once for first
     * attaching process.
     */

    if ( cProcessesAttached++ ) {
        return(TRUE);
    }

    return UTRegister( hInst,        // DB32.DLL module handle
                       "DB16.DLL",   // name of 16bit thunk dll
                       NULL,         // no init routine
                       "UTProc",     // name of 16bit dispatch routine
                                     // exported from DB16.DLL
                       &pfnUTProc,   // Global variable to receive thunk address
                       NULL,         // no call back function
                       NULL          // no shared memroy
                     );
  } else if ( fdwReason == DLL_PROCESS_DETACH ) {

      if ( --cProcessesAttached == 0 ) {
          UTUnRegister( hInst );
      }
  }

}

/*
 * constants for dispatcher in 16bit side
 */

#define DB_SRV_GETVERSION   0
#define DB_SRV_SETTIME      1
#define DB_SRV_ADDUSER      2


int
DbGetVersion(void)
{

    /*
     * call the 16bit dispatch thru the 32bit thunk. no parameters
     * are passed.
     * returned value is the service result.
     */

    return( (int) (* pfnUTProc)(NULL, DB_SRV_GETVERSION, NULL) );

}

void
DbSetTime(LPDB_TIME pTime)
{

    /*
     * call the 16bit dispatch thru the 32bit thunk.
     * pass one pointer to a buffer which will be translated
     * to 16:16 address before passed to 16bit side.
     *
     */

    (* pfnUTProc)(pTime, DB_SRV_SETTIME, NULL);

}

short
DbAddUser(LPDB_NAME pName, DWORD Permission, LPDWORD pId)
{

    DWORD    Args[3];           // function has three arguments
    PVOID    TransList[4];      // Three pointers need translation:
                                //     pName
                                //     pName->str
                                //     pId
                                // plus null to indicate end of list
    char    *pSaveStr;
    int     Ret;

    /*
     * put arguments in buffer
     */

    Args[0] = (DWORD) pName;
    Args[1] = Permission;
    Args[2] = (DWORD) pId;

    /*
     * build translation list for all the flat pointers that need to
     * be translated to segmented form.
     */
    TransList[0] = & Args[0];
    TransList[1] = & pName->str;
    TransList[2] = & Args[2];
    TransList[3] = 0;

    /*
     * save the original pointer in the NAME struct so it can be restored
     * after the call to the thunk.
     * This is required only if the caller of the service expects the
     * structure to be left unmodified.
     */

    pSaveStr = pName->str;

    /*
     * call the 16bit dispatch thru the 32bit thunk.
     * pass arguments in buffer along with list of addresses
     * that need to be translated equivalent segmented format.
     *
     */

    Ret = (int) (* pfnUTProc)(Args, DB_SRV_ADDUSER, TransList);

    /*
     * restore flat pointer
     */
    pName->str = pSaveStr;

    return(Ret);
}


