//---------------------------------------------------------------------------
//
//  Module:   pipe.c
//
//  Description:
//     "Named pipe" functions.  Allows two devices to communicate
//     using a common "pipe" name.  For example, MSSNDSYS and MSOPL
//     use this functionality to establish a communication path
//     between the mixer/driver(s) without having to rely on dynamic
//     links and load order dependancies.
//
//  Comments:
//     Note: Clients _EXPECT_ to have the associated devnode returned
//     in dwParam1.  You will break them if you change this assumption.
//     Thus, dwParam1 is not really dwParam1, it it the DevNode parameter.
//     However, to keep this code "generic", it will remain named
//     as dwParam1.
//
//---------------------------------------------------------------------------
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1995 Microsoft Corporation.  All Rights Reserved.
//
//---------------------------------------------------------------------------

#define  WANTVXDWRAPS

#include <basedef.h>
#include <vmm.h>
#include <vmmreg.h>
#include <vxdwraps.h>
#include <configmg.h>
#include <vmmreg.h>
#include <debug.h>
#include <vxdpipe.h>

#include "pipe.h"

#undef CURSEG               
#define  CURSEG()                   PCODE

#ifdef  DEBUG
#define DPF(x) Out_Debug_String(x)
#else
#define DPF(x)
#endif

#pragma VxD_PAGEABLE_CODE_SEG
#pragma VxD_PAGEABLE_DATA_SEG

//--------------------------------------------------------------------------
//  
//  VOID pipeCallProc
//  
//  Description:
//  
//  
//  Parameters:
//  
//  Return (DWORD):
//  
//--------------------------------------------------------------------------

DWORD pipeCallProc
(
    VOID PIPEFAR* pProc,
    HPIPE         hp,
    DWORD         dwMsg,
    DWORD         dwParam1,
    DWORD         dwParam2
)
{
   CRS    crs ;
   DWORD  dwRetVal, hVM ;
   PCRS   pcrs ;
   struct Client_Word_Reg_Struc *pcrsw ;

   DPF( "PIPE: calling client procedure\r\n" ) ;

   if (!pProc)
      return PIPE_ERR_INVALIDPROC ;

   vmmSaveClientState( &crs ) ;

   vmmBeginNestExec() ;

   hVM = vmmGetCurVMHandle() ;

   pcrs = (PCRS) (((PVMCB) hVM) -> CB_Client_Pointer) ;

   // push parameters for _pascal call

   vmmSimulatePush( HIWORD( hp ) ) ;
   vmmSimulatePush( LOWORD( hp ) ) ;
   vmmSimulatePush( HIWORD( dwMsg ) ) ;
   vmmSimulatePush( LOWORD( dwMsg ) ) ;
   vmmSimulatePush( HIWORD( dwParam1 ) ) ;
   vmmSimulatePush( LOWORD( dwParam1 ) ) ;
   vmmSimulatePush( HIWORD( dwParam2 ) ) ;
   vmmSimulatePush( LOWORD( dwParam2 ) ) ;

   vmmSimulateFarCall( HIWORD( pProc ), LOWORD( pProc ) ) ;
   vmmResumeExec() ;

   pcrsw = (struct Client_Word_Reg_Struc *) pcrs ;
   dwRetVal =
      ((DWORD) (pcrsw -> Client_DX) << 16)| (pcrsw -> Client_AX) ;

   vmmEndNestExec() ;

   vmmRestoreClientState( &crs ) ;

   return dwRetVal ;

} // pipeCallProc()

//--------------------------------------------------------------------------
//  
//  HPIPE pipeOpen
//  
//  Description:
//  
//  
//  Parameters:
//      DWORD dn
//  
//      PVMMLIST phlp
//  
//      PCHAR psz
//  
//      PPIPEOPENSTRUCT pos
//  
//  Return (HPIPE):
//  
//--------------------------------------------------------------------------

HPIPE pipeOpen
(
    DWORD               dn,
    PVMMLIST            phlp,
    PCHAR               psz,
    PPIPEOPENSTRUCT     pos
)
{
   VMMLIST    hlp ;
   PPIPENODE  ppn ;

   DPF( "PIPE: pipeOpen()\r\n" ) ;

   hlp = *phlp ;

   if (!hlp)
   {
      hlp = List_Create( LF_ALLOC_ERROR, sizeof( PIPENODE ) ) ;
      if (hlp == NULL)
         return NULL ;
      *phlp = hlp ;
   }

   // truncate pipe name if necessary

   if (strlen( psz ) > 8)
   {
     psz[ 8 ] = NULL ;
   }

   // walk list to see if pipe is already open

   for (ppn = (PPIPENODE) List_Get_First( hlp ); 
        ppn != NULL; 
        ppn = (PPIPENODE) List_Get_Next( hlp, ppn ))
   {
      if (0 == strcmp( psz, ppn -> szName ))
         break ;
   }

   if (ppn)
   {
      // old pipe, see if it's got room for the client

      if (ppn -> apClientProc[ 1 ])
         return NULL ;

      // ok, other side is open, notify both clients
      // that the pipe was opened.

      if (pipeCallProc( ppn -> apClientProc[ 0 ], 
                        (HPIPE) ppn, 
                        PIPE_MSG_OPEN, 
                        ppn -> dn, 
                        NULL ))
      {
         // client failed to open, zero out apClientProc and close

         ppn -> apClientProc[ 0 ] = NULL ;
         pipeClose( phlp, ppn ) ;
         return NULL ;
      }

      ppn -> apClientProc[ 1 ] = pos -> pClientProc ;
      if (pipeCallProc( ppn -> apClientProc[ 1 ], 
                        (HPIPE) ppn, 
                        PIPE_MSG_OPEN, 
                        ppn -> dn,
                        NULL ))
      {
         ppn -> apClientProc[ 1 ] = NULL ;
         pipeClose( phlp, ppn ) ;
         return NULL ;
      }

      // Ok, both pipes ok'd the open, now send init and
      // give 'em the other end's pipe proc.

      DPF( "PIPE: pipe opened, now initializing\r\n" ) ;

      pipeCallProc( ppn -> apClientProc[ 0 ],
                    (HPIPE) ppn, 
                    PIPE_MSG_INIT,
                    ppn -> dn,
                    (DWORD) ppn -> apClientProc[ 1 ] ) ;

      pipeCallProc( ppn -> apClientProc[ 1 ],
                    (HPIPE) ppn, 
                    PIPE_MSG_INIT, 
                    ppn -> dn,
                    (DWORD) ppn -> apClientProc[ 0 ] ) ;
   }
   else if (NULL == (ppn = (PPIPENODE) List_Allocate( hlp )))
      return NULL ;
   else
   {
      // new pipe

      ppn -> dn = dn ;
      ppn -> apClientProc[ 0 ] = pos -> pClientProc ;
      ppn -> apClientProc[ 1 ] = NULL ;
      strcpy( ppn -> szName, psz ) ;

      List_Attach_Tail( hlp, ppn ) ;

      // we'll notify when the other side opens
   }

   return (HPIPE) ppn ;

} // pipeOpen()

//--------------------------------------------------------------------------
//  
//  VOID pipeClose
//  
//  Description:
//  
//  
//  Parameters:
//      PVMMLIST phlp
//  
//      PPIPENODE ppn
//  
//  Return (VOID):
//  
//--------------------------------------------------------------------------


VOID pipeClose
(
    PVMMLIST        phlp,
    PPIPENODE       ppn
)
{

#pragma message( REMIND( "validate the pipe node" ) )

   DPF( "PIPE: pipeClose()\r\n" ) ;

   if (ppn)
   {
      // notify the clients of the pipe closure

      if (ppn -> apClientProc[ 0 ])
      {
         pipeCallProc( ppn -> apClientProc[ 0 ], 
                       (HPIPE) ppn, 
                       PIPE_MSG_CLOSE, 
                       ppn -> dn, 
                       NULL ) ;
         ppn -> apClientProc[ 0 ] = NULL ;
      }

      if (ppn -> apClientProc[ 1 ])
      {
         pipeCallProc( ppn -> apClientProc[ 1 ], 
                       (HPIPE) ppn, 
                       PIPE_MSG_CLOSE, 
                       ppn -> dn, 
                       NULL ) ;
         ppn -> apClientProc[ 1 ] = NULL ;
      }

   }

   if (*phlp)
   {
      List_Remove( *phlp, ppn ) ;
      List_Deallocate( *phlp, ppn ) ;
      if (NULL == List_Get_First( *phlp ))
         List_Destroy( *phlp ) ;
      *phlp = NULL ;
   }

} // pipeClose()

//---------------------------------------------------------------------------
//  End of File: pipe.c
//---------------------------------------------------------------------------

