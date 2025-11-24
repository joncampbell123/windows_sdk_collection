//---------------------------------------------------------------------------
//
//  Module:   midiin.c
//
//  Description:
//     MSMPU401 MIDI-in routines
//
//---------------------------------------------------------------------------
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1994 - 1995 Microsoft Corporation.	All Rights Reserved.
//
//---------------------------------------------------------------------------

#include <windows.h>
#include <mmsystem.h>
#include <mmddk.h>
#include <mmreg.h>

#include "msmpu401.h"

//--------------------------------------------------------------------------
//  
//  void midFreeQ
//  
//  Description:
//      Current this is only called after sending off any partially filled
//      buffers, so all buffers here are empty.  The timestamp value is 0
//      in this case.
//  
//  Parameters:
//      PMIDIINCLIENT pmic
//  
//  Return (void):
//      Nothing.
//
//--------------------------------------------------------------------------

void NEAR PASCAL midFreeQ
(
    PMIDIINCLIENT   pmic
)
{
    LPMIDIHDR   lpH, lpN;
    DWORD       dwTime;

    lpH = pmic -> lpmhQueue ;             // point to top of the queue
    pmic -> lpmhQueue = NULL ;            // mark the queue as empty
    pmic -> dwCurData = 0L ;

    dwTime = timeGetTime() - pmic -> dwRefTime;

    while (lpH)
    {
        lpN = lpH->lpNext;
        lpH->dwFlags |= MHDR_DONE;
        lpH->dwFlags &= ~MHDR_INQUEUE;
        lpH->dwBytesRecorded = 0;
        midiCallback( (PPORTALLOC) pmic, MIM_LONGDATA, (DWORD)lpH, dwTime ) ;
        lpH = lpN;
    }

} // midFreeQ()

//--------------------------------------------------------------------------
//  
//  DWORD midAddBuffer
//  
//  Description:
//      This function adds a buffer to the list of midi input buffers.
//  
//  Parameters:
//      PMIDIINCLIENT pmic
//  
//      LPMIDIHDR lpmh
//  
//  Return (DWORD):
//  
//--------------------------------------------------------------------------

#pragma optimize( "leg", off )

DWORD NEAR PASCAL midAddBuffer
(
    PMIDIINCLIENT   pmic,
    LPMIDIHDR       lpmh
)
{
   LPMIDIHDR   lpN;
   WORD        wFlags ;

   // check if it's been prepared
   if (!(lpmh->dwFlags & MHDR_PREPARED))
      return MIDIERR_UNPREPARED;

   // check if it's in our queue already
   if (lpmh->dwFlags & MHDR_INQUEUE)
      return MIDIERR_STILLPLAYING;

   // add the buffer to our queue
   lpmh->dwFlags |= MHDR_INQUEUE;
   lpmh->dwFlags &= ~MHDR_DONE;

   // sanity
   lpmh->dwBytesRecorded = 0;
   lpmh->lpNext = NULL;

   _asm
   {
      pushf
      pop   wFlags
      test  wFlags, 0x200
      jz    SHORT No_Disable
      cli

   No_Disable:
   }

   if ( lpN = pmic -> lpmhQueue )
   {
      while ( lpN->lpNext && (lpN = lpN->lpNext) )
            ;

      lpN->lpNext = lpmh;
   }
   else 
      pmic -> lpmhQueue = lpmh;

   _asm
   {
      test  wFlags, 0x200
      jz    SHORT No_Enable
      sti

   No_Enable:
   }

   // return success
   return ( 0L );

} // midAddBuffer()

#pragma optimize( "", on )

//--------------------------------------------------------------------------
//  
//  void midGetDevCaps
//  
//  Description:
//  
//  
//  Parameters:
//      PHARDWAREINSTANCE phwi
//  
//      MDEVICECAPSEX FAR* lpCaps
//  
//  Return (void):
//  
//--------------------------------------------------------------------------

void NEAR PASCAL midGetDevCaps
(
    PHARDWAREINSTANCE       phwi,
    MDEVICECAPSEX FAR*      lpCaps
)
{
    MIDIINCAPS mc;

    mc.wMid = MM_MICROSOFT;
    mc.wPid = MM_MPU401_MIDIIN;
    mc.vDriverVersion = DRIVER_VERSION;
    LoadString(ghModule, IDS_MPU401MIDIIN, mc.szPname, MAXPNAMELEN);
    mc.dwSupport = 0;
    _fmemcpy(lpCaps->pCaps, &mc, min(lpCaps->cbSize, sizeof(MIDIINCAPS)));

} // midGetDevCaps()

//--------------------------------------------------------------------------
//  
//  LRESULT midOpen
//  
//  Description:
//      This should be called when a midi file is opened.
//      It initializes some variables and client structure.
//  
//  Parameters:
//      PHARDWAREINSTANCE phwi
//         pointer to hardware instance structure
//
//      LPDWORD pdwUser
//         pointer to user return
//
//      LPMIDIOPENDESC pod
//         pointer to open description structure
//
//      DWORD dwFlags
//         options
//  
//  Return (LRESULT):
//      MMSYSERR_NOERROR if successful, else error
//  
//--------------------------------------------------------------------------

#pragma optimize( "leg", off )

LRESULT FAR PASCAL midOpen
(
    PHARDWAREINSTANCE   phwi,
    LPDWORD             pdwUser,
    LPMIDIOPENDESC      pod,
    DWORD               dwFlags
)
{
   PMIDIINCLIENT pmic ;
   WORD          wFlags ;

   DPF( 3, "midOpen" ) ;

   if (phwi -> bMidiInFlags & MIF_ALLOCATED)
      return MMSYSERR_ALLOCATED ;

   if (!AcquireMPU401( phwi ))
      return MMSYSERR_ALLOCATED ;

   pmic = (PMIDIINCLIENT) LocalAlloc( LPTR, sizeof( MIDIINCLIENT ) ) ;
   if (NULL == pmic)
   {   ReleaseMPU401( phwi ) ;
       return MMSYSERR_NOMEM ;
   }

   // save client information

   ((PPORTALLOC) pmic) -> dwCallback = pod -> dwCallback ;
   ((PPORTALLOC) pmic) -> dwInstance = pod -> dwInstance ;
   ((PPORTALLOC) pmic) -> hMidi      = pod -> hMidi ;
   ((PPORTALLOC) pmic) -> dwFlags    = dwFlags ;
   ((PPORTALLOC) pmic) -> phwi       = phwi ;

   // init queue stuff

   pmic -> dwCurData = 0 ;
   pmic -> lpmhQueue = 0 ;

   //  NOTE: we must init ref time in case someone adds longdata
   //  buffers after opening, then resets the midi stream without
   //  starting midi input.  Otherwise, midFreeQ would give 
   //  inconsistent timestamps...

   pmic -> dwRefTime = timeGetTime() ;

   phwi -> pmic = pmic ;

   *pdwUser = MAKELONG( (WORD) pmic, NULL ) ;

   phwi -> bMidiInFlags |= MIF_ALLOCATED ;

   if (phwi -> fnisrPipe)
   {
      _asm
      {
         pushf
         pop   wFlags
         cli
      }

      phwi -> fnisrPipe( phwi -> hpisr, 
                         PIPE_MSG_CONTROL |
                            MSMPU401_CTL_SET_ISR,
                         phwi -> dn,
                         (DWORD) MPU401InterruptHandler ) ;

      phwi -> fnisrPipe( phwi -> hpisr, 
                         PIPE_MSG_CONTROL |
                            MSMPU401_CTL_SET_INSTANCE,
                         phwi -> dn,
                         MAKELONG( (WORD) phwi, 0 ) ) ;
      _asm
      {
         mov   ax, wFlags
         test  ah, 2
         jz    SHORT No_Enable
         sti

      No_Enable:
      }
   }

   // notify client

   midiCallback( (PPORTALLOC) pmic, MIM_OPEN, 0L, 0L ) ;

   return MMSYSERR_NOERROR ;

} // midOpen()

//--------------------------------------------------------------------------
//  
//  VOID midClose
//  
//  Description:
//      Closes the MIDI-in device.
//  
//  Parameters:
//      PMIDIINCLIENT pmic
//         pointer to client structure
//  
//  Return (VOID):
//      Nothing.
//  
//--------------------------------------------------------------------------

VOID FAR PASCAL midClose
(
    PMIDIINCLIENT        pmic
)
{
   PHARDWAREINSTANCE  phwi ;
   WORD               wFlags ;

   DPF( 3, "midClose" ) ;

   phwi = pmic -> pa.phwi ;
   phwi -> pmic = NULL ;

   ReleaseMPU401( phwi ) ;

   phwi -> bMidiInFlags &= ~MIF_ALLOCATED ;

   //
   // notify client
   //

   midiCallback( (PPORTALLOC) pmic, MIM_CLOSE, 0L, 0L ) ;

   LocalFree( (HLOCAL) pmic ) ;

   if (phwi -> fnisrPipe)
   {
      _asm
      {
         pushf
         pop   wFlags
         cli
      }

      phwi -> fnisrPipe( phwi -> hpisr, 
                         PIPE_MSG_CONTROL | 
                            MSMPU401_CTL_SET_ISR,
                         phwi -> dn,
                         NULL ) ;
      _asm
      {
         mov   ax, wFlags
         test  ah, 2
         jz    SHORT No_Enable
         sti

      No_Enable:
      }
   }

} // midClose()

#pragma optimize( "", on )

//--------------------------------------------------------------------------
//  
//  LRESULT midMessage
//  
//  Description:
//     This function conforms to the standard MIDI input driver
//     message proc midMessage, which is documented in mmddk.h.
//  
//  Parameters:
//      UINT uDevId
//         device id
//
//      UINT msg
//         driver message
//  
//      DWORD dwUser
//         user info
//  
//      DWORD dwParam1
//         message specific
//  
//      DWORD dwParam2
//         message specific
//  
//  Return (LRESULT):
//      Error status
//  
//--------------------------------------------------------------------------

LRESULT FAR PASCAL _loadds midMessage
(
    UINT            uDevId,
    UINT            uMsg,
    DWORD           dwUser,
    DWORD           dwParam1,
    DWORD           dwParam2
)
{
   PMIDIINCLIENT       pmic ;
   PHARDWAREINSTANCE   phwi ;

   // take care of init time messages...

   switch (uMsg)
   {
      case MIDM_INIT:
      {
         DPF( 1, "MIDM_INIT" ) ;

         // dwParam2 == PnP DevNode

         return (AddDevNode( dwParam2 )) ;
      }
      break ;

      case DRVM_ENABLE:
      {
         UINT   uVxDId ;
         ULONG  cIds ;

         DPF( 3, "MIDM_ENABLE" ) ;

         // dwParam2 == PnP DevNode

         // Query the supporting VxD ID

         cIds = 1 ;

         if (midiInMessage( (HMIDIIN) dwParam1, DRV_QUERYDRIVERIDS,
                            (DWORD) (LPWORD) &uVxDId, 
                            (DWORD) (LPDWORD) &cIds ))
            return MMSYSERR_INVALPARAM ;

         return (EnableDevNode( dwParam2, uVxDId )) ;

      }
      break ;

      case DRVM_DISABLE:
      {
         DPF( 3, "MIDM_DISABLE" ) ;

         // dwParam2 == PnP DevNode

         return (DisableDevNode( dwParam2 )) ;

      }
      break ;

      case DRVM_EXIT:
      {
         DPF( 3, "MIDM_EXIT" ) ;

         // dwParam2 == PnP DevNode

         return (RemoveDevNode( dwParam2 )) ;

      }
      break ;
        
      case MIDM_GETNUMDEVS:
      {
         DPF( 1, "MIDM_GETNUMDEVS" ) ;

         if (NULL == (phwi = DevNodeToHardwareInstance( dwParam1 )))
            return MAKELONG( 0, MMSYSERR_INVALPARAM ) ;

         if (phwi -> fEnabled)
            return 1L ;
         else
            return 0L ;
      }
      break ;

      case MIDM_OPEN:
      {
         DWORD  dn ;

         DPF( 1, "MIDM_OPEN" ) ;

         if (uDevId > 1)
            return MMSYSERR_BADDEVICEID ;

         dn = ((LPMIDIOPENDESC) dwParam1) -> dnDevNode ;

         if (NULL == 
               (phwi = DevNodeToHardwareInstance( dn )))
         {
            DPF( 1, "devnode not associated with hardware instance???" ) ;
            return MMSYSERR_BADDEVICEID ;
         }

         if (!phwi -> fEnabled)
            return MMSYSERR_NOTENABLED ;

         return (midOpen( phwi, 
                          (LPDWORD) dwUser, 
                          (LPMIDIOPENDESC) dwParam1, 
                          dwParam2 )) ;
      }
      break ;

      case MIDM_GETDEVCAPS:

      {
         DPF( 1, "MIDM_GETDEVCAPS" ) ;

         if (uDevId > 1)
            return MMSYSERR_BADDEVICEID ;

         if (NULL == 
               (phwi = DevNodeToHardwareInstance( dwParam2 )))
         {
            DPF( 1, "devnode not associated with hardware instance???" ) ;
            return MMSYSERR_BADDEVICEID ;
         }

         if (!phwi -> fEnabled)
            return MMSYSERR_NOTENABLED ;

         midGetDevCaps( phwi, (MDEVICECAPSEX FAR *) dwParam1 ) ;

         return MMSYSERR_NOERROR ;
      }
      break ;

   }

   // Bad device ID???

   if (uDevId > 1)
      return MMSYSERR_BADDEVICEID ;

   pmic = (PMIDIINCLIENT) LOWORD( dwUser ) ;
   phwi = pmic -> pa.phwi ;

   switch ( uMsg )
   {
      case MIDM_CLOSE:
      {
         DPF( 1,"MIDM_CLOSE" ) ;

         if ( pmic -> lpmhQueue )
               return MIDIERR_STILLPLAYING;

         //
         // Just in case we started without adding buffers
         //

         midStop( pmic ) ;

         midClose( pmic ) ;

         //
         // we're not used any more
         //

         return MMSYSERR_NOERROR ;
      }

      case MIDM_ADDBUFFER:
      {
         DPF( 1, "MIDM_ADDBUFFER" ) ;

         // attempt to add the buffer...

         return midAddBuffer( pmic, (LPMIDIHDR)dwParam1 ) ;
      }

      case MIDM_START:
      {
         DPF( 1, "MIDM_START" ) ;

         if (phwi -> bMidiInFlags & MIF_STARTED)
            return 0L ;

         // initialize all the parsing status variables

         pmic -> fSysEx = 0 ;
         pmic -> bStatus = 0 ;
         pmic -> bBytesLeft = 0 ;
         pmic -> bBytePos = 0 ;
         pmic -> dwShortMsg = 0 ;
         pmic -> dwMsgTime = 0 ;
         pmic -> dwRefTime = 0 ;
         pmic -> dwCurData = 0 ;

         // get a new reference time...

         pmic -> dwRefTime = timeGetTime() ;

         midStart( pmic ) ;
         return 0L ;
      }

      case MIDM_STOP:
      {
         DPF( 1, "MIDM_STOP" ) ;
         midStop( pmic ) ;
         return 0L ;
      }

      case MIDM_RESET:
      {
         DPF( 1, "MIDM_RESET" ) ;

         // stop if it is started and release all buffers

         midStop( pmic ) ;
         midFreeQ( pmic ) ;
         return 0L ;
      }
   }

   return MMSYSERR_NOTSUPPORTED ;

} // midMessage()

//---------------------------------------------------------------------------
//  End of File: midiin.c
//---------------------------------------------------------------------------
