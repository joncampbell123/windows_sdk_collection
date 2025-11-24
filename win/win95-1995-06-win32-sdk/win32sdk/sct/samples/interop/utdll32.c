/********************************************************************\
*                                                                    *
*  File:      utdll32.c                                              *
*                                                                    *
*  Comments:  32-bit side of the Universal Thunk                     *
*                                                                    *
*  Functions: DllMain                                                *
*             InitThunk                                              *
*             UTCBProc                                               *
*                                                                    *
\********************************************************************/

/*********************  Header Files **********************/

#define W32SUT_32

#include <windows.h>
#include "w32sut.h"
#include "myut.h"

/*********************  Globals    ************************/

HWND     ghWnd;
HANDLE   ghDLL;
UT32PROC pfnUTProc = NULL;

/*********************  Prototypes ************************/

DWORD WINAPI UTCBProc( LPVOID, DWORD );

void WINAPI MyPrint( LPSTR, LPSTR, DWORD );

/********************************************************************\
* Function: BOOL WINAPI DllMain( HANDLE, DWORD, LPVOID )             *
*                                                                    *
*  Purpose: DLL entry point                                          *
*                                                                    *
* Comments: Standard template                                        *
*           Saves instance handle in a global variable               *
*                                                                    *
*                                                                    *
\********************************************************************/

BOOL WINAPI DllMain( HANDLE hDLL, DWORD dwReason, LPVOID lpReserved )
{
   static int cAttached = 0;

   switch (dwReason)
   {
      case DLL_PROCESS_ATTACH:
         if( !cAttached )
            ghDLL = hDLL;
         cAttached++;
         break;

      case DLL_THREAD_ATTACH:
         break;

      case DLL_THREAD_DETACH:
         break;

      case DLL_PROCESS_DETACH:
         --cAttached;
         if( !cAttached ) {
            UTUnRegister( hDLL );
         }
         break;
   }

   return TRUE;
}

/********************************************************************\
* Function: void WINAPI InitThunk( HWND )                            *
*                                                                    *
*  Purpose: Initialize the Universal Thunk                           *
*                                                                    *
* Comments: Saves the window handle passed by the Win32 stub app     *
*           and calls UTRegister() to establish the thunk and load   *
*           dll16.dll.                                               *
*                                                                    *
*                                                                    *
\********************************************************************/

void WINAPI InitThunk( HWND hWnd )
{
   ghWnd = hWnd;

   if( !UTRegister( ghDLL,
      "dll16.dll",
      "UTInit",
      "UTProc",
      &pfnUTProc,
      UTCBProc,
      NULL )
     )
   MessageBox( NULL, "UTRegister() failed", "UTDLL32", MB_OK );
}

/********************************************************************\
* Function: DWORD WINAPI UTCBProc( LPVOID, DWORD )                   *
*                                                                    *
*  Purpose: Dispatch routine                                         *
*                                                                    *
* Comments: Switches on dwFunc to determine which dll32 routine is   *
*           being called and calls into that DLL (dll32.dll). Note   *
*           that the two strings required by MyPrint() were put into *
*           a translation list in dll16.c, now they are passed as  *
*           separate arguments.                                      *
*                                                                    *
*                                                                    *
\********************************************************************/

DWORD WINAPI UTCBProc( LPVOID lpBuf, DWORD dwFunc )
{
   switch( dwFunc )
   {
      case MYPRINT:
         MyPrint( (LPSTR) ((LPDWORD)lpBuf)[0], (LPSTR) ((LPDWORD)lpBuf)[1],
            ((LPDWORD)lpBuf)[2] );
         break;

   /*
      Return window handle so that app16.exe can force stub32.exe
      to terminate via SendMessage( ... WM_CLOSE ... )
   */

      case GETHWND:
         return (DWORD) ghWnd;
         break;

      default:
         MessageBox( NULL, "Unsupported routine called", "utdll32", MB_OK );
   }
   return TRUE;
}
