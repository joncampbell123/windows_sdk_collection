/********************************************************************\
*  Module: UTSamp32.c                                                *
*                                                                    *
*  Comments: Source for utsamp32.dll                                 *
*                                                                    *                                                                    *
*  Functions:                                                        *
*                                                                    *
*  DllMain()          - DLL entry point                              *
*  MyGetFreeSpace()   - Get the amount of free physical memory       *
*  MyWNetGetUser()    - Get the default user name                    *
*  My32CallBack()     - Dispatch function for callbacks              *
*                                                                    *
*                                                                    *
\********************************************************************/

/*********************  Header Files  *********************/

#define W32SUT_32

#include <windows.h>
#include "w32sut.h"
#include "utsamp.h"

/*********************   Prototypes   *********************/

DWORD APIENTRY My32Callback (LPVOID, DWORD );
void APIENTRY MyFunc1( void );
void APIENTRY MyFunc2( void );

/*********************   Type Defs    *********************/

typedef BOOL (WINAPI * PUTREGISTER) ( HANDLE     hModule,
				      LPCSTR     lpsz16BitDLL,
				      LPCSTR     lpszInitName,
				      LPCSTR     lpszProcName,
				      UT32PROC * ppfn32Thunk,
				      FARPROC    pfnUT32Callback,
				      LPVOID     lpBuff
				    );


typedef VOID (WINAPI * PUTUNREGISTER) (HANDLE hModule);

typedef DWORD (APIENTRY * PUT32CBPROC) (LPVOID lpBuff, DWORD dwUserDefined );

/*********************     Globals    *********************/

UT32PROC      pfnUTProc = NULL;
PUTREGISTER   pUTRegister = NULL;
PUTUNREGISTER pUTUnRegister = NULL;
PUT32CBPROC   pfnUT32CBProc = &My32Callback;
int           cProcessesAttached = 0;
BOOL          fWin32s = FALSE;
HANDLE        hKernel32 = 0;

/********************************************************************\
* Function: BOOL APIENTRY DllMain(HANDLE, DWORD, LPVOID)             *
*                                                                    *
*  Purpose: DLL entry point                                          *
*                                                                    *
* Comments: Detect whether or not we are running under Win32s. If    *
*           we are running under Win32s, get the address of          *
*           UTRegister() and UTUnRegister(). Call UTRegister() to    *
*           set up the thunk when this DLL is loaded. Call           *
*           UTUnRegister() to destroy the thunk when this DLL is     *
*           unloaded.                                                *
*                                                                    *
*                                                                    *
\********************************************************************/

BOOL APIENTRY DllMain(HANDLE hInst, DWORD fdwReason, LPVOID lpReserved)
{
   DWORD dwVersion;

   if ( fdwReason == DLL_PROCESS_ATTACH )
   {

    /*
     * Registration of UT need to be done only once for first
     * attaching process.  At that time set the fWin32s flag
     * to indicate if the DLL is executing under Win32s or not.
     */

      if( cProcessesAttached++ )
      {
         return(TRUE);         // Not the first initialization.
      }

      // Find out if we're running on Win32s
      dwVersion = GetVersion();
      fWin32s = (BOOL) (!(dwVersion < 0x80000000))
                        && (LOBYTE(LOWORD(dwVersion)) < 4);

      if( !fWin32s )
         return(TRUE);         // Not on Win32s - no further initialization needed

      hKernel32 = LoadLibrary( "Kernel32.Dll" ); // Get Handle to Kernel32.Dll

      pUTRegister = (PUTREGISTER) GetProcAddress( hKernel32, "UTRegister" );

      if( !pUTRegister )
         return(FALSE);        // Error - On Win32s, but can't find UTRegister

      pUTUnRegister = (PUTUNREGISTER) GetProcAddress( hKernel32, "UTUnRegister" );

      if( !pUTUnRegister )
         return(FALSE);        // Error - On Win32s, but can't find UTUnRegister

      return (*pUTRegister)( hInst,           // UTSamp32.DLL module handle
  			     "UTSAMP16.DLL",  // name of 16-bit thunk dll
			     "UTInit",	      // name of init routine
			     "UTProc",        // name of 16-bit dispatch routine (in utsamp16)
			     &pfnUTProc,      // Global variable to receive thunk address
			     pfnUT32CBProc,   // callback function
			     NULL );          // no shared memroy

   }
   if( (fdwReason == DLL_PROCESS_DETACH) && (0 == --cProcessesAttached) && fWin32s )
   {
      (*pUTUnRegister)( hInst );
      FreeLibrary( hKernel32 );
   }
} // DllMain()

/********************************************************************\
* Function: DWORD APIENTRY MyGetFreeSpace(void)                      *
*                                                                    *
*  Purpose: Get the amount of free physical memory.                  *
*                                                                    *
* Comments: Call GlobalMemoryStatus(). If this is Win32s, this       *
*           returns ERROR_CALL_NOT_IMPLEMENTED. In this case, use    *
*           the UT entrypoint, passing it MYGETFREESPACE.            *
*                                                                    *
*                                                                    *
\********************************************************************/

DWORD APIENTRY MyGetFreeSpace(void)
{
   MEMORYSTATUS MemoryStatus;

  /*
   * GlobalMemoryStatus() does not have a return value. Check for failure
   * by calling SetLastError() with NO_ERROR, call GlobalMemoryStatus(),
   * then call GetLastError() to see if ERROR_CALL_NOT_IMPLEMENTED was
   * returned.
   */

   memset( &MemoryStatus, sizeof(MEMORYSTATUS), 0);
   SetLastError( NO_ERROR );
   GlobalMemoryStatus( &MemoryStatus );

   if( ERROR_CALL_NOT_IMPLEMENTED == GetLastError() && fWin32s )
      return( (* pfnUTProc)( NULL, MYGETFREESPACE, NULL ) );
   else
      return( MemoryStatus.dwAvailPhys );
} // MyGetFreeSpace()

/********************************************************************\
* Function: DWORD APIENTRY MyWNetGetUser(LPTSTR, LPTSTR, LPDWORD)    *
*                                                                    *
*  Purpose: Get the default user name.                               *
*                                                                    *
* Comments: Call WNetGetUser(). If this Win32s, this returns         *
*           ERROR_CALL_NOT_IMPLEMENTED. In this case, use the UT     *
*           entrypoint, passing it MYWNETGETUSER.                    *
*                                                                    *
*           Win16 WNetGetUser() does not support lpszLocalName, so   *
*           the 1st parameter is ignored when this function is       *
*           called under Win32s.                                     *
*                                                                    *
*                                                                    *
\********************************************************************/

DWORD APIENTRY MyWNetGetUser(LPTSTR lpszLocalName, LPTSTR lpszUserName, LPDWORD lpcchBuffer)
{
   DWORD Args[2];
   PVOID TransList[3];
   DWORD retval;

  /*
   * Win16 uses several different return codes to indicate different errors,
   * while Win32 uses only a few return codes to indicate error, one of which
   * indicates that WNetGetLastError() should be called for more detailed error
   * information.
   *
   * This function just maps all Win16 error codes into the Win32 error
   * ERROR_NO_NETWORK.  A more complete solution would be to switch on the
   * various return codes possible from WNetGetUser (both Win16 & Win32) and
   * to set appropriate SetLastError values.
   *
   */


   if( ERROR_CALL_NOT_IMPLEMENTED ==
       (retval = WNetGetUser( lpszLocalName, lpszUserName, lpcchBuffer )))
   {

    /*
     * The parameters being passed include pointers that need to be
     * translated, using the Translation List.
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
      {                              // since the API only looks at the lower
         *lpcchBuffer = 0xFFFF;      // 16 bits passed, we need to make sure the
      }                              // buffer is not too large.

      TransList[0] = & Args[0];      // Translate the two pointes passed in Args[]
      TransList[1] = & Args[1];      // from 0:32 to 16:16.
      TransList[2] = NULL;           // End of list of pointers to translate

      retval = (* pfnUTProc)(Args, MYWNETGETUSER, TransList);

      if( retval)                    // WN_SUCCESS == NO_ERROR == 0.  All others
         retval = ERROR_NO_NETWORK;  // return codes are Win16 error codes that
				     // we map to ERROR_NO_NETWORK.
   }

   return (retval);
} //MyWNetGetUser()

/********************************************************************\
* Function: void APIENTRY GenerateCallback(DWORD)                    *
*                                                                    *
*  Purpose: Dummy routine which causes 16-bit side to make a         *
*           callback.                                                *
*                                                                    *
* Comments: This wouldn't be needed in an application - this is just *
*           a simple demo which was requested by customers after     *
*           using the original UTSample.                             *
*                                                                    *
*                                                                    *
\********************************************************************/

void APIENTRY GenerateCallback( DWORD dwFunc )
{
   if( fWin32s ) {
      if( dwFunc == 1 )
         (* pfnUTProc)( &dwFunc, MYCALLBACK1, NULL );
      else
         (* pfnUTProc)( &dwFunc, MYCALLBACK2, NULL );
   }
   else {
      if( dwFunc == 1 )
         MyFunc1();
      else
         MyFunc2();
   }
} // GenerateCallback()

/********************************************************************\
* Function: DWORD APIENTRY My32Callback(LPVOID, DWORD)               *
*                                                                    *
*  Purpose: Dispatch function for callbacks                          *
*                                                                    *
* Comments: Simple code which is part of showing how callbacks from  *
*           the 16-bit code to the 32-bit code that loaded it works. *
*                                                                    *
*                                                                    *
\********************************************************************/

DWORD APIENTRY My32Callback( LPVOID lpBuff, DWORD dwFunc )
{
   switch( dwFunc )
   {
      case MYFUNC1:
	 MyFunc1();
	 break;
      case MYFUNC2:
	 MyFunc2();
	 break;
      default:
	 return( 0 );
   }
   return( 1 );
} // My32Callback()

/********************************************************************\
* Function: void APIENTRY MyFunc1(void)                              *
*                                                                    *
*  Purpose: To be called from 16-bit side via Reverse UT             *
*                                                                    *
* Comments: Does nothing at this point                               *
*                                                                    *
*                                                                    *
\********************************************************************/

void APIENTRY MyFunc1( )
{
   MessageBox( NULL, "Inside MyCallback1", "UTSamp32", MB_OK );
} // MyCallback1()

/********************************************************************\
* Function: void APIENTRY MyFunc2(void)                              *
*                                                                    *
*  Purpose: To be called from 16-bit side via Reverse UT             *
*                                                                    *
* Comments: Does nothing at this point                               *
*                                                                    *
*                                                                    *
\********************************************************************/

void APIENTRY MyFunc2( )
{
   MessageBox( NULL, "Inside MyCallback2", "UTSamp32", MB_OK );
} // MyCallback2()
