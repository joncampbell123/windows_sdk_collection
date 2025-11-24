/********************************************************************\
*  Module: UTSamp16.c                                                *
*                                                                    *
*  Comments: Source for utsamp16.dll                                 *
*                                                                    *                                                                    *
*  Functions:                                                        *
*                                                                    *
*  LibMain()  - DLL entry point                                      *
*  UTInit()   - Universal Thunk initialization procedure             *
*  UTProc()   - Dispatch routine                                     *
*  WEP()      - Windows exit procedure                               *
*                                                                    *
*                                                                    *
\********************************************************************/

/*********************  Header Files  *********************/

#ifndef APIENTRY
#define APIENTRY
#endif
#define W32SUT_16

#include <windows.h>
#include "w32sut.h"
#include "utsamp.h"

/*********************    Globals     *********************/

UT16CBPROC glpfnUT16CallBack;

/********************************************************************\
* Function: LRESULT CALLBACK LibMain(HANDLE, WORD, WORD, LPSTR)      *
*                                                                    *
*  Purpose: DLL entry point                                          *
*                                                                    *
* Comments: No special processing required                           *
*                                                                    *
*                                                                    *
\********************************************************************/

int FAR PASCAL LibMain( HANDLE hLibInst, WORD wDataSeg,
			WORD cbHeapSize, LPSTR lpszCmdLine)
{
   return (1);
} // LibMain()

/********************************************************************\
* Function: DWORD FAR PASCAL UTInit(UT16CBPROC, LPVOID)              *
*                                                                   f *
*  Purpose: Universal Thunk initialization procedure                 *
*                                                                    *
* Comments: Store callback procedure address in global variable      *
*                                                                    *
*                                                                    *
\********************************************************************/

DWORD FAR PASCAL UTInit( UT16CBPROC lpfnUT16CallBack, LPVOID lpBuf )
{
   glpfnUT16CallBack = lpfnUT16CallBack;
   return(1);   // Return Success
} // UTInit()


/********************************************************************\
* Function: DWORD FAR PASCAL UTProc(LPVOID, DWORD)                   *
*                                                                    *
*  Purpose: Dispatch routine called by 32-bit UT DLL                 *
*                                                                    *
* Comments: Call the appropriate Win16 API based on the dwFunc       *
*           parameter. Extract any necessary parameters from the     *
*           lpBuf buffer. This function must be exported.            *
*                                                                    *
*                                                                    *
\********************************************************************/

DWORD FAR PASCAL UTProc( LPVOID lpBuf, DWORD dwFunc)
{
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

         static HANDLE hNetDrv;

         HINSTANCE hWfwNet = 0;
         FARPROC   pMNetNetworkEnum   = NULL;
         FARPROC   pMNetSetNextTarget = NULL;
         WORD      wRetEnum;
         DWORD     Temp;

         SetErrorMode( SEM_NOOPENFILEERRORBOX );

         hWfwNet = LoadLibrary( "WFWNET.DRV" );

         SetErrorMode( 0 );

         if( hWfwNet <= HINSTANCE_ERROR )
         {  // Not WFW
            hWfwNet = 0;
            Temp = WNetGetUser( (LPSTR) ((LPDWORD)lpBuf)[0],
                                (LPWORD) ((LPDWORD)lpBuf)[1] );
         }
         else
         {  // WFW running
            pMNetNetworkEnum = GetProcAddress( hWfwNet, "MNetNetworkEnum" );
            pMNetSetNextTarget = GetProcAddress( hWfwNet, "MNetSetNextTarget" );

            if( pMNetNetworkEnum && pMNetSetNextTarget )
            {
               hNetDrv = 0;
               wRetEnum = (WORD) ((*pMNetNetworkEnum)((HANDLE FAR *)&hNetDrv));

               while( wRetEnum == WN_SUCCESS )
               {
                  (*pMNetSetNextTarget)( hNetDrv );   // activate network
                  Temp = WNetGetUser( (LPSTR) ((LPDWORD)lpBuf)[0],
                                      (LPWORD)((LPDWORD)lpBuf)[1] );

                  if( Temp == WN_SUCCESS )
                     break;  // terminate on success or end of enumeration
                  // otherwise try the next one for WNetGetUser support
                  wRetEnum = (WORD) ((*pMNetNetworkEnum)((HANDLE FAR *) &hNetDrv));
               }
               if( Temp != WN_SUCCESS )
                  *((LPSTR)((LPDWORD)lpBuf)[0]) = 0;
            }
            else // MNetXXX functions not found
               Temp = WNetGetUser( (LPSTR) ((LPDWORD)lpBuf)[0],
                                   (LPWORD) ((LPDWORD)lpBuf)[1] );
         }

         if (hWfwNet)
	    FreeLibrary( hWfwNet );

         return( Temp );

      } // case MYWNETGETUSER:

      case MYCALLBACK1: // example calling back to a 32-bit function
         (*glpfnUT16CallBack)( NULL, MYFUNC1, NULL );
         break;

      case MYCALLBACK2: // example calling back to a 32-bit function
         (*glpfnUT16CallBack)( NULL, MYFUNC2, NULL );
         break;
   } // switch (dwFunc)

   return( (DWORD)-1L ); // We should never get here.
} // UTProc()

/********************************************************************\
* Function: int FAR PASCAL WEP(int)                                  *
*                                                                    *
*  Purpose: Windows exit procedure                                   *
*                                                                    *
* Comments: No special processing required.                          *
*                                                                    *
*                                                                    *
\********************************************************************/

int FAR PASCAL WEP( int bSystemExit )
{
   return (1);
} // WEP()
