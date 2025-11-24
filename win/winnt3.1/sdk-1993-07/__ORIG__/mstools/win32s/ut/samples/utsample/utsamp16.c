/*++

Copyright (c) 1985-93, Microsoft Corporation

Module Name:

    utsamp16.c

Abstract:

    Win32s sample code of Universal Thunk (UT) -
    This is the main source file of utsamp16.DLL.

--*/
#ifndef APIENTRY
#define APIENTRY
#endif
#define W32SUT_16

#include <windows.h>
#include <w32sut.h>
#include "utsamp.h"

int FAR PASCAL WEP (int bSystemExit)
 {
  return (1);
 } // WEP()

int FAR PASCAL LibMain (HANDLE hLibInst, WORD wDataSeg,
                        WORD cbHeapSize, LPSTR lpszCmdLine)
 {
  return (1);
 } // LibMain()

DWORD FAR PASCAL
UTInit( UT16CBPROC lpfnUT16CallBack,
        LPVOID lpBuf )
 {
  return(1); // Return Success
 } // UTInit()


/*
 * Call the appropriate Win16 API based on the dwFunc parameter.  Extract any
 * necessary parameters from the lpBuf buffer.
 *
 * This function must be exported.
 *
 */

DWORD FAR PASCAL
UTProc( LPVOID lpBuf,
        DWORD dwFunc)
 {

  /*
   * Call appropriate 16 bit API based on the dwFunc.  Extract any necessary
   * parameters from lpBuf.
   *
   */

  switch (dwFunc)
   {

    case MYGETFREESPACE:

      return( GetFreeSpace( (UINT) 0 ) );

    case MYWNETGETUSER:
     {
       // Windows for Workgroups supports multiple networks.
       // to get WNetGetUser to function properly on that
       // NOS, we must call MNetSetNextTarget passing the
       // handle returned from MNetGetLastTarget

      HINSTANCE hWfwNet = 0;
      FARPROC   pMNetSetNextTarget = NULL;
      FARPROC	pMNetGetLastTarget = NULL;
      DWORD	Temp;

      SetErrorMode(SEM_NOOPENFILEERRORBOX);

      hWfwNet = LoadLibrary( "WFWNET.DRV" );

      SetErrorMode( 0 );

      if (hWfwNet > HINSTANCE_ERROR)
       {
        pMNetGetLastTarget = GetProcAddress( hWfwNet, "MNetGetLastTarget" );
        pMNetSetNextTarget = GetProcAddress( hWfwNet, "MNetSetNextTarget" );

        if ( pMNetGetLastTarget && pMNetSetNextTarget )
          (*pMNetSetNextTarget) ( (HANDLE)(*pMNetGetLastTarget) () );

       } // if (hWfwNet > HINSTANCE_ERROR)
      else
       hWfwNet = 0;

      Temp = WNetGetUser( (LPSTR)  ((LPDWORD)lpBuf) [0],
			  (LPWORD) ((LPDWORD)lpBuf) [1] );

      if (hWfwNet)
        FreeLibrary( hWfwNet );

      return( Temp );

    } // case MYWNETGETUSER:

   } // switch (dwFunc)

  return( (DWORD)-1L ); // We should never get here.

 } // UTProc()
