/*++

Copyright (c) 1985-92, Microsoft Corporation

Module Name:

    UTSamp32.c

Abstract:

    Win32s sample code of Universal Thunk (UT) -
    This is the main source file of UTSamp32.DLL

--*/


#define W32SUT_32

#include <windows.h>
#include <w32sut.h>
#include "utsamp.h"

typedef BOOL (WINAPI * PUTREGISTER) ( HANDLE     hModule,
                                      LPCSTR     lpsz16BitDLL,
                                      LPCSTR     lpszInitName,
                                      LPCSTR     lpszProcName,
                                      UT32PROC * ppfn32Thunk,
                                      FARPROC    pfnUT32Callback,
                                      LPVOID     lpBuff
                                    );


typedef VOID (WINAPI * PUTUNREGISTER) (HANDLE hModule);

UT32PROC      pfnUTProc = NULL;
PUTREGISTER   pUTRegister = NULL;
PUTUNREGISTER pUTUnRegister = NULL;
int           cProcessesAttached = 0;
BOOL          fWin32s = FALSE;
HANDLE        hKernel32 = 0;


BOOL
APIENTRY
DllInit(HANDLE hInst, DWORD fdwReason, LPVOID lpReserved)
 {
  if ( fdwReason == DLL_PROCESS_ATTACH )
   {

    /*
     * Registration of UT need to be done only once for first
     * attaching process.  At that time set the fWin32s flag
     * to indicate if the DLL is executing under Win32s or not.
     * If we are running under WIn32s, we need to get the
     * address of the entrypoints for UTRegister & UTUnRegister
     *
     */

    if ( cProcessesAttached++ )
     {
      return(TRUE);         // Not the first initialization.
     }

    fWin32s = (BOOL) (GetVersion() & 0x80000000);

    if ( !fWin32s )
      return(TRUE);         // Not on Win32s - no further initialization needed

    hKernel32 = LoadLibrary( "Kernel32.Dll" ); // Get Handle to Kernel32.Dll

    pUTRegister = (PUTREGISTER) GetProcAddress( hKernel32, "UTRegister" );

    if ( !pUTRegister )
      return(FALSE);        // Error - On Win32s, can't find UTRegister

    pUTUnRegister = (PUTUNREGISTER) GetProcAddress( hKernel32, "UTUnRegister" );

    if ( !pUTUnRegister )
      return(FALSE);        // Error - On Win32s, can't find UTUnRegister

    return (*pUTRegister)( hInst,           // UTSamp32.DLL module handle
                           "UTSAMP16.DLL",  // name of 16bit thunk dll
                           NULL,            // no init routine
                           "UTProc",        // name of 16bit dispatch routine
                                            // exported from UTSamp16.DLL
                           &pfnUTProc,      // Global variable to receive thunk address
                           NULL,            // no callback function
                           NULL );          // no shared memroy

   }
  else
    if (   (fdwReason == DLL_PROCESS_DETACH)
        && (0 == --cProcessesAttached)
        && fWin32s )
     {
      (*pUTUnRegister) ( hInst );
      FreeLibrary( hKernel32 );
     }

 }


DWORD
APIENTRY
MyGetFreeSpace(void)
 {
  MEMORYSTATUS MemoryStatus;

  /*
   * Call GlobalMemoryStatus to get the amount of free physical memory.  Since
   * GlobalMemoryStatus does not have a return value, the way to check for
   * failure is to SetLastError to NO_ERROR, call GlobalMemoryStatus, and then
   * GetLastError to see if ERROR_CALL_NOT_IMPLEMENTED was returned.  If so,
   * and if Win32s is loaded, call the UT entrypoint passing MYGETFREESPACE
   * as the parameter.
   *
   */

  memset( &MemoryStatus, sizeof(MEMORYSTATUS), 0);
  SetLastError( NO_ERROR );
  GlobalMemoryStatus( &MemoryStatus );
  if (ERROR_CALL_NOT_IMPLEMENTED == GetLastError() && fWin32s)
    return( (* pfnUTProc)( NULL, MYGETFREESPACE, NULL ) );
  else
    return( MemoryStatus.dwAvailPhys );
 }

DWORD
APIENTRY
MyWNetGetUser(LPTSTR lpszLocalName, LPTSTR lpszUserName, LPDWORD lpcchBuffer)
 {

  DWORD Args[2];
  PVOID TransList[3];
  DWORD retval;

  /*
   * Since the Win16 WNetGetUser does not support the lpszLocalName,
   * this parameter is ignored by this function when called under Win32s.
   * It is accepted for compatibility with the Win32 API.
   *
   * The error return method for the this API changed between Win16 and Win32.
   * Win16 uses several different return codes to indicate different errors,
   * while Win32 uses only a few return codes to indicate error, one of which
   * indicates that WNetGetLastError should be called for more detailed error
   * information.
   *
   * This function just maps all Win16 error codes into the Win32 error
   * ERROR_NO_NETWORK.	A more complete solution would be to switch on the
   * various return codes possible from WNetGetUser (both Win16 & Win32) and
   * to set appropriate SetLastError values.
   *
   */


  if (ERROR_CALL_NOT_IMPLEMENTED ==
      (retval = WNetGetUser( lpszLocalName, lpszUserName, lpcchBuffer )))
   {

    /*
     * call the 16bit WNetGetUser via the UT entrypoint.
     * Since the parameters being passed include pointers that need to be
     * translated, we need to use the Translation List.
     *
     */

    Args[0] = (DWORD)lpszUserName; // Buffer to receive UserName
    Args[1] = (DWORD)lpcchBuffer;  // Pointer to a DWord containing the size
                                   // of this buffer on entry.  On return it will
                                   // have been replaced with the size of the
                                   // returned text.  Since the Win16 API only
                                   // changes the low 16 bits, we need to zero out
                                   // the high 16 bits before calling the API.

    if ((*lpcchBuffer) > 0xFFFF)   // The buffer could be 64K or larger, but
     {                             // since the API only looks at the lower
      *lpcchBuffer = 0xFFFF;       // 16 bits passed, we need to make sure the
     }                             // buffer is not too large.

    TransList[0] = & Args[0];      // Translate the two pointes passed in Args[]
    TransList[1] = & Args[1];      // from 0:32 to 16:16.
    TransList[2] =   NULL;         // End of list of pointers to translate

    retval = (* pfnUTProc)(Args, MYWNETGETUSER, TransList);

    if (retval)                    // WN_SUCCESS == NO_ERROR == 0.  All others
      retval = ERROR_NO_NETWORK;   // return codes are Win16 error codes that
                                   // we map to ERROR_NO_NETWORK.
   }

  return (retval);

 }
