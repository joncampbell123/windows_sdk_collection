//---------------------------------------------------------------------------
//
//  Module: auxil.c
//
//  Purpose:
//     Aux portion of SNDSYS.DRV.
//
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
// PURPOSE.
//
// Copyright (c) 1991 - 1995  Microsoft Corporation.  All Rights Reserved.
//
//---------------------------------------------------------------------------

#include <windows.h>

// stop compiler from griping about in-line

#pragma warning (disable:4704)

#define Not_VxD
#include <vmm.h>
#include <configmg.h>

#include <mmsystem.h>
#include <mmddk.h>
#include <mmreg.h>
#include "mssndsys.h"
#include "driver.h"

#include "mixer.h"

//--------------------------------------------------------------------------
//  
//  VOID auxilGetDevCaps
//  
//  Description:
//      Gets the aux capabilities.
//  
//  Parameters:
//  
//--------------------------------------------------------------------------

//
// READTHIS_READTHIS_READTHIS_READTHIS_READTHIS_READTHIS_READTHIS
//
// Note: This routine is called from MixerInit.  Check there before you
//       change anything here.
//

VOID FAR PASCAL auxilGetDevCaps
(
    PHARDWAREINSTANCE   phwi,
    MDEVICECAPSEX FAR   *lpCaps
)
{
  char     szTemp[ 64 ] ;
  char     szClip[ 64 ] ;
  AUXCAPS  auxCaps;

  // fill in the aux caps

  auxCaps.wMid = MID_MICROSOFT;
  auxCaps.wPid = PID_AUX;
  auxCaps.vDriverVersion = DRV_VERSION;
  auxCaps.wTechnology = AUXCAPS_AUXIN;
  auxCaps.dwSupport = AUXCAPS_LRVOLUME | AUXCAPS_VOLUME;

  LoadString( ghModule, SR_STR_DRIVERAUX, szTemp, sizeof( szTemp ) ) ;
  wsprintf( szClip, szTemp, phwi -> wIOAddressCODEC ) ;
  lstrcpyn( auxCaps.szPname, szClip, MAXPNAMELEN-1);

  _fmemcpy( lpCaps -> pCaps, &auxCaps, 
            min( (UINT) lpCaps -> cbSize, sizeof( auxCaps ) ) ) ;

} // auxilGetDevCaps()

//--------------------------------------------------------------------------
//  
//  DWORD auxMessage
//  
//  Description:
//      This function conforms to the standard auxilary driver
//      message procedure.
//  
//  Parameters:
//      UINT uDevId
//
//      WORD msg
//  
//      DWORD dwUser
//  
//      DWORD dwParam1
//  
//      DWORD dwParam2
//  
//  Return (DWORD):
//      Message specific
//
//  
//--------------------------------------------------------------------------

DWORD FAR PASCAL _loadds auxMessage
(
    UINT            uDevId,
    WORD            msg,
    DWORD           dwUser,
    DWORD           dwParam1,
    DWORD           dwParam2
)
{
   PHARDWAREINSTANCE  phwi ;

   // take care of init time messages...

   switch (msg)
   {
      case AUXM_INIT:
      {
         DPF( 1, "AUXDM_INIT" ) ;

         if (gwGlobalStatus)
         {
            DisplayConfigErrors() ;
            return 0L ;
         }
         else
         {
            // dwParam2 == PnP DevNode

            return (AddDevNode( dwParam2 )) ;
         }
      }
      break ;

      case DRVM_ENABLE:
      {
         DPF( 3, "AUXM_ENABLE" ) ;

         // dwParam2 == PnP DevNode

         return (EnableDevNode( dwParam2 )) ;
      }
      break ;

      case DRVM_DISABLE:
      {
         DPF( 3, "AUXM_DISABLE" ) ;

         // dwParam2 == PnP DevNode

         return (DisableDevNode( dwParam2 )) ;
      }
      break ;
        
      case DRVM_EXIT:
      {
         DPF( 3, "AUXM_EXIT" ) ;

         // dwParam2 == PnP DevNode

         return (RemoveDevNode( dwParam2 )) ;

      }
      break ;
        
      case AUXDM_GETNUMDEVS:
      {
         DPF( 1, "AUXDM_GETNUMDEVS" ) ;

         if (NULL == (phwi = DevNodeToHardwareInstance( dwParam1 )))
            return MAKELONG( 0, MMSYSERR_INVALPARAM ) ;

         if (phwi -> fEnabled)
            return 1L ;
         else
            return 0L ;
      }
      break ;

      case AUXDM_GETDEVCAPS:
      {
         DPF( 1, "AUXDM_GETDEVCAPS" ) ;

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

         auxilGetDevCaps( phwi, (MDEVICECAPSEX FAR *) dwParam1 ) ;
         return MMSYSERR_NOERROR ;
      }
      break ;

   }

   // Bad device ID???

   if (uDevId > 1)
      return MMSYSERR_BADDEVICEID ;

   switch (msg)
   {
      case AUXDM_GETVOLUME:
      {
         PHARDWAREINSTANCE  phwi ;
         PMIXERINSTANCE     pmi ;
   
         if (NULL == (phwi = DevNodeToHardwareInstance( dwParam2 )))
            return MMSYSERR_BADDEVICEID ;

         pmi = phwi -> pmi ;

         *((DWORD FAR *) dwParam1) =
            MAKELONG( (WORD) pmi -> dwValue[ VOL_OUTAUX1 ][ 0 ],
                        (WORD) pmi -> dwValue[ VOL_OUTAUX1 ][ 1 ] ) ;
         return MMSYSERR_NOERROR ;
      }

      case AUXDM_SETVOLUME:
      {
         PHARDWAREINSTANCE            phwi ;
         MIXERCONTROLDETAILS          mcd ;
         MIXERCONTROLDETAILS_UNSIGNED mcd_u[ 2 ] ;

         DPF( 2, "AUXDM_SETVOLUME" ) ;

         if (NULL == (phwi = DevNodeToHardwareInstance( dwParam2 )))
            return MMSYSERR_BADDEVICEID ;

         // Set up control details structure

         mcd.dwControlID = VOL_OUTAUX1 ;
         mcd.cbStruct = sizeof( MIXERCONTROLDETAILS ) ;
         mcd.cChannels = 2 ;
         mcd.cMultipleItems = 0 ;
         mcd.cbDetails  = sizeof( MIXERCONTROLDETAILS_UNSIGNED ) ;
         mcd.paDetails = &mcd_u ;

         mcd_u[ 0 ].dwValue = LOWORD( dwParam1 ) ;
         mcd_u[ 1 ].dwValue = HIWORD( dwParam1 ) ;

         MxdSetControlDetails( phwi, &mcd, 0 ) ;

         return MMSYSERR_NOERROR ;
      }

   }

   return MMSYSERR_NOTSUPPORTED;

} // auxMessage()

//---------------------------------------------------------------------------
//  End of File: auxil.c
//---------------------------------------------------------------------------
