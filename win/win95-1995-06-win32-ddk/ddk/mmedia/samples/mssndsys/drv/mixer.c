//---------------------------------------------------------------------------
//
//  Module:   mixer.c
//
//  Description:
//     Mixer support interfaces
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
#include <windowsx.h>

// stop compiler from griping about in-line

#pragma warning (disable:4704)

#define Not_VxD
#include <vmm.h>
#include <configmg.h>

#include <mmsystem.h>
#include <mmddk.h>
#include <mmreg.h>
#include <limits.h>
#include "mssndsys.h"
#include "driver.h"
#include "mixer.h"

#if 1       // protected mode shortcuts for these APIs (WIN16 only)
#define GlobalLock(h) MAKELP(h, 0)
#define GlobalUnlock(h) (0)
#define GlobalHandle(sel) ((HANDLE)(sel))
#endif

//--------------------------------------------------------------------------
//
//  MMRESULT MxdInit
//
//  Description:
//      Mixer device initialization routine.
//
//  Parameters:
//      PHARDWAREINSTANCE phwi
//         hardware instance
//
//  Return (MMRESULT):
//      MMSYSERROR_NOMEM if unable to allocate memory for mixer
//      device, otherwise MMSYSERR_NOERROR.
//
//
//--------------------------------------------------------------------------

MMRESULT FAR PASCAL MxdInit
(
    PHARDWAREINSTANCE   phwi
)
{
   UINT             uCurString;
   PMIXERINSTANCE   pmi ;

   union
   {
      AUXCAPS     AuxCaps ;
      WAVEOUTCAPS WoCaps ;
      WAVEINCAPS  WiCaps ;

   } CapStructs ;

   MDEVICECAPSEX  mdex ;

   DPF( 1, "MxdInit" ) ;

   // Initialize first (default) card information...

    if (NULL ==
          (pmi = (PMIXERINSTANCE) GlobalAllocPtr( GHND | GMEM_SHARE,
                                                  sizeof( MIXERINSTANCE ) )))
       return ( MMSYSERR_NOMEM ) ;

   phwi -> pmi = pmi ;

   //
   // Initialize the mixer lines and controls...
   //

   // --- sources

   _fmemcpy( pmi -> mxlSources, gmxlSources, sizeof( gmxlSources ) ) ;

   // --- destinations

   _fmemcpy( pmi -> mxlDests, gmxlDests, sizeof( gmxlDests ) ) ;

   // --- controls

   _fmemcpy( pmi -> mxc, gmxc, sizeof( gmxc ) ) ;

   //
   // LoadStrings from RC for all source names (long and short)
   //

   DPF( 1, "MixInit about to load strings for lines" ) ;

   for (uCurString=0; uCurString < MAXSOURCES; uCurString++)
   {
      LoadString( ghModule,
                  SR_STR_MIXER_SOURCE0_LONG + uCurString,
                  pmi -> mxlSources[ uCurString ].szName,
                  MIXER_LONG_NAME_CHARS);

      LoadString( ghModule,
                  SR_STR_MIXER_SOURCE0_SHORT + uCurString,
                  pmi -> mxlSources[ uCurString ].szShortName,
                  MIXER_SHORT_NAME_CHARS);
   }

   for (uCurString=0; uCurString < MAXDESTINATIONS; uCurString++)
   {
      LoadString( ghModule,
                  SR_STR_MIXER_DEST0_LONG + uCurString,
                  pmi -> mxlDests[ uCurString ].szName,
                  MIXER_LONG_NAME_CHARS);

      LoadString( ghModule,
                  SR_STR_MIXER_DEST0_SHORT + uCurString,
                  pmi -> mxlDests[ uCurString ].szShortName,
                  MIXER_SHORT_NAME_CHARS);
   }

   for (uCurString = 0; uCurString < MAXCONTROLS; uCurString++)
   {
      LoadString( ghModule,
                  SR_STR_MIXER_CONTROL0_LONG + uCurString,
                  pmi -> mxc[ uCurString ].szName,
                  MIXER_LONG_NAME_CHARS ) ;

      LoadString( ghModule,
                  SR_STR_MIXER_CONTROL0_SHORT + uCurString,
                  pmi -> mxc[ uCurString ].szShortName,
                  MIXER_SHORT_NAME_CHARS ) ;
   }

   //
   // control mapping
   //
   // The control map describes the controls, the # of
   // channels supported for the control and locations
   // (e.g. source and destination for the control).
   //
   // The control map lists the controls in control ID
   // order, with their destinations, relative source ID,
   // and the number of channels.
   //
   // The index into this array is the control ID.  The
   // array contains the destination number, the relative
   // source number and number of channels.  If a control
   // is at the destination, the source is UINT_MAX.
   //
   // If a control has the type MIXERCONTROL_CONTROLF_UNIFORM,
   // the channels element must be 1.
   //

   _fmemcpy( pmi -> auControlMap, gauControlMap, sizeof( gauControlMap ) ) ;

   //
   // source map
   //
   //
   // The source map describes the connections for each
   // destination.  This maps a relative source number at a
   // destination to an actual source number in the mixer
   // source list.
   //
   // For example, the line-out destination may have three
   // sources, such as wave out, line-in, and fm synth.  The
   // wave-in destination may have two sources: line-in and mic.
   //
   // The line-out relative source number for line-in would be 1
   // and the wave-in relative source number would be 0, but both
   // map to the same "physical" source (SOURCE_LINEIN).
   //
   // A source is not present on a line by defining SOURCE_INVALID
   // (or UINT_MAX) in the structure.  However, if a destination
   // has three connections (as line-out in the example above), the
   // source map must be "compacted" to include these three sources
   // in the first three source mappings of that destination.  It
   // is not valid to leave "holes" in the source map.  The source
   // map must be padded with SOURCE_INVALID (or UINT_MAX).
   //

   _fmemcpy( pmi -> auSourceMap, gauSourceMap, sizeof( gauSourceMap ) ) ;

   //
   // source control mapping
   //
   //
   // The source controls map describes the number of controls
   // for a source at a destination.  The format of the source
   // controls map is a UINT for each source at a destination.
   // The first index in the array is the destination number,
   // the second index is the relative source number.
   //

   _fmemcpy( pmi -> auSourceControlsMap, gauSourceControlsMap,
               sizeof( gauSourceControlsMap ) ) ;

   //
   // active/disconnect map
   //
   //
   // The active map contains flags that are OR'd into the
   // MIXERLINE fdwLine element.  This allows a source at
   // a destination to become active or disconnected, etc.
   //
   // For the most part, this map is NULL unless a line is
   // specifically activated by an action such as opening a
   // wave output (or input) or midi output device.
   //
   // Also, persistent connections (such as a line-in to a
   // line-out or speakers line should be marked as active
   // here..
   //
   // The format of the active map is a DWORD for each source
   // at a destination.  The first index in the array is
   // the destination number, the second index is the relative
   // source number (actual source number is defined by the
   // source map).
   //

   pmi -> adwActiveMap[ DEST_LINEOUT ][ 0 ] = MIXERLINE_LINEF_ACTIVE ;
   pmi -> mxlDests[ DEST_LINEOUT ].fdwLine = MIXERLINE_LINEF_ACTIVE ;

   //
   // Disconnect FM synthesis until we've initialized the mixer
   // pipe to MSOPL, if it never initializes then the MSOPL
   // driver is not responding or is not installed -- we won't have
   // FM synthesis available until MSOPL responds.
   //

   // disconnect from line out

   pmi -> adwActiveMap[ DEST_LINEOUT ][ 2 ] =
      MIXERLINE_LINEF_DISCONNECTED ;

   //
   // If this card has something other than a AD1848J, set new levels.
   //

   if (phwi -> wCODECClass != CODEC_J_CLASS)
   {
      pmi -> mxc[VOL_OUTAUX1].Metrics.cSteps = 32;
      pmi -> mxc[VOL_W_INAUX1].Metrics.cSteps = 32;
      pmi -> mxc[VOL_V_INAUX1].Metrics.cSteps = 32;
   }

   //
   // Get the device mapping from the appropriate places elsewhere
   // in the driver.
   //

   //
   // First do the wave-in stuff
   //

   DPF( 1, "Copying wavein devcaps" ) ;

   mdex.cbSize = sizeof( WAVEINCAPS ) ;
   mdex.pCaps = &CapStructs ;
   widGetDevCaps( phwi, &mdex ) ;

   //
   // Copy wave-in info to "WaveIn"
   //

   _fmemcpy( &pmi -> mxlDests[ DEST_WAVEIN ].Target.wMid,
               &CapStructs,
               sizeof( WORD ) * 2 + sizeof( VERSION ) + MAXPNAMELEN ) ;

   pmi -> mxlDests[ DEST_WAVEIN ].Target.dwType =
      MIXERLINE_TARGETTYPE_WAVEIN ;

   //
   // Copy wave-in info to "VoiceIn"
   //

   DPF( 1, "Copying voicein devcaps" ) ;

   _fmemcpy( &pmi -> mxlDests[ DEST_VOICEIN ].Target.wMid,
               &CapStructs,
               sizeof( WORD ) * 2 + sizeof( VERSION ) + MAXPNAMELEN ) ;


   pmi -> mxlDests[ DEST_VOICEIN ].Target.dwType =
      MIXERLINE_TARGETTYPE_WAVEIN ;

   //
   // Now do Waveout stuff.
   //

   DPF( 1, "Copying waveout devcaps" ) ;

   mdex.cbSize = sizeof( WAVEOUTCAPS ) ;
   mdex.pCaps = &CapStructs ;
   wodGetDevCaps( phwi, &mdex ) ;

   _fmemcpy( &pmi -> mxlSources[ SOURCE_WAVEOUT ].Target.wMid,
             &CapStructs,
             sizeof( WORD ) * 2 + sizeof( VERSION ) + MAXPNAMELEN ) ;

   pmi -> mxlSources[ SOURCE_WAVEOUT ].Target.dwType =
      MIXERLINE_TARGETTYPE_WAVEOUT ;

   //
   // Now do Aux stuff.
   //

   DPF( 1, "Copying aux devcaps" ) ;

   mdex.cbSize = sizeof( AUXCAPS ) ;
   mdex.pCaps = &CapStructs ;
   auxilGetDevCaps( phwi, &mdex ) ;

   _fmemcpy( &pmi -> mxlSources[ SOURCE_AUX1 ].Target.wMid,
               &CapStructs,
               sizeof( WORD ) * 2 + sizeof( VERSION ) + MAXPNAMELEN ) ;

   pmi -> mxlSources[ SOURCE_AUX1 ].Target.dwType =
      MIXERLINE_TARGETTYPE_AUX ;

   //
   //  succeed the load...
   //

   DPF( 2, "\r\nMixInit finished\r\n" ) ;

   return ( MMSYSERR_NOERROR ) ;

} // MxdInit()

//--------------------------------------------------------------------------
//
//  VOID MxdEnd
//
//  Description:
//      Frees the mixer object information...
//
//  Parameters:
//      PHARDWAREINSTANCE phwi
//         hardware instance
//
//  Return (VOID):
//      Nothing.
//
//--------------------------------------------------------------------------

VOID FAR PASCAL MxdEnd
(
    PHARDWAREINSTANCE   phwi
)
{
   GlobalFreePtr( phwi -> pmi ) ;
   // LocalFree( (HLOCAL) phwi -> pmi ) ;
   phwi -> pmi = NULL ;

} // MxdEnd()

//==========================================================================;
//==========================================================================;
//==========================================================================;

//--------------------------------------------------------------------------
//
//  VOID MxdNotifyLineChange
//
//  Description:
//      Sends a line change notification to all mixer clients.
//
//  Parameters:
//      DWORD dwLineId
//         line id for notification
//
//  Return (VOID):
//      Nothing.
//
//
//--------------------------------------------------------------------------

VOID NEAR PASCAL MxdNotifyLineChange
(
    PMIXERINSTANCE   pmi,
    DWORD            dwLineId
)
{
   PMXDCLIENT  pCur ;

   //
   // !!! WARNING DANGER WARNING DANGER WARNING DANGER !!!
   //
   // See notes regarding DriverCallback() usage below...
   //
   // !!! WARNING DANGER WARNING DANGER WARNING DANGER !!!
   //

   //
   // Walk the list for the mixer device and notify everyone...
   //

   pCur = pmi -> pMxdClients ;
   while (pCur)
   {
      DriverCallback( pCur -> dwCallback,
                      pCur -> fuCallback,
                      pCur -> hmx,
                      MM_MIXM_LINE_CHANGE,
                      pCur -> dwInstance,
                      dwLineId,
                      0L ) ;

      pCur = pCur -> pNext ;
   }

} // MxdNotifyLineChange()

//--------------------------------------------------------------------------
//
//  VOID MxdUpdateLine
//
//  Description:
//      Updates the active "bit" in the line structure and notifies
//      mixer apps about the change.  Because a mute control does
//      not know the active status of a line, the mute status is
//      stored as a seperate flag and AND'd with the line active
//      flag to determine the active state of the mixer line.
//
//  Parameters:
//      PHARDWAREINSTANCE phwi
//         pointer to hardware instance structure
//
//      UINT uDest
//         destination line
//
//      UINT uSource
//         source on uDest
//
//      BOOL fSet
//         set or reset?
//
//      UINT fuAction
//         action to take
//            MXDUPDATELINE_ACTIONF_LINESTATUS
//               line active status is changing
//
//            MXDUPDATELINE_ACTIONF_MUTESTATUS
//               mute status is changing
//
//  Return (VOID):
//      Nothing.
//
//
//--------------------------------------------------------------------------

VOID FAR PASCAL MxdUpdateLine
(
    PHARDWAREINSTANCE  phwi,
    UINT               uDest,
    UINT               uSource,
    BOOL               fSet,
    UINT               fuAction
)
{
   int              i ;
   DWORD            dwLineID, dwMuxControlID, fdwPrev, fdwCur ;
   UINT             uRelSource ;
   PMIXERINSTANCE   pmi;

   if (NULL == (pmi = phwi -> pmi))
   {
      DPF( 1, "what??? no mixer instance in phwi???" ) ;
      return ;
   }

   if (fuAction & MXDUPDATELINE_ACTIONF_SOURCE)
   {
      // compute relative source number for this destination

      for (i = 0; i < MAXSOURCES; i++)
         if (pmi -> auSourceMap[ uDest ][ i ] == uSource)
            break ;

      //
      // NOTE: Because this is an internal function, only validate
      //       when in debug - we better have a valid source number!!!
      //

      D(
         if ((i > MAXSOURCES) ||
            (pmi -> auSourceMap[ uDest ][ i ] == SOURCE_INVALID))
            DPF( 1,  "MxdUpdateLine: Source mapping failure!!!!" ) ;
      ) ;

      fdwPrev = pmi -> adwActiveMap[ uDest ][ i ] ;
      if (fdwPrev & IMIXERLINE_LINEF_MUTED)
         fdwPrev &= ~(IMIXERLINE_LINEF_MUTED | MIXERLINE_LINEF_ACTIVE) ;

      if (fuAction & MXDUPDATELINE_ACTIONF_MUTESTATUS)
      {
         if (fSet)
            pmi -> adwActiveMap[ uDest ][ i ] |= IMIXERLINE_LINEF_MUTED ;
         else
            pmi -> adwActiveMap[ uDest ][ i ] &= ~IMIXERLINE_LINEF_MUTED ;
      }
      else
      {
         if (fSet)
            pmi -> adwActiveMap[ uDest ][ i ] |= MIXERLINE_LINEF_ACTIVE ;
         else
            pmi -> adwActiveMap[ uDest ][ i ] &= ~MIXERLINE_LINEF_ACTIVE ;
      }

      fdwCur = pmi -> adwActiveMap[ uDest ][ i ] ;
      if (fdwCur & IMIXERLINE_LINEF_MUTED)
         fdwCur &= ~(IMIXERLINE_LINEF_MUTED | MIXERLINE_LINEF_ACTIVE) ;

      //
      // Only notify when a change is really made...
      //

      if (fdwPrev != fdwCur)
      {
         dwLineID = MAKELONG( uDest, i ) ;
         MxdNotifyLineChange( pmi, dwLineID ) ;
      }

      // Grab destination's previous flags...

      fdwPrev = pmi -> mxlDests[ uDest ].fdwLine ;
      if (fdwPrev & IMIXERLINE_LINEF_MUTED)
         fdwPrev &= ~(IMIXERLINE_LINEF_MUTED | MIXERLINE_LINEF_ACTIVE) ;

   }
   else
   {
      // Grab previous flags before we update the mute status...

      fdwPrev = pmi -> mxlDests[ uDest ].fdwLine ;
      if (fdwPrev & IMIXERLINE_LINEF_MUTED)
         fdwPrev &= ~(IMIXERLINE_LINEF_MUTED | MIXERLINE_LINEF_ACTIVE) ;

      // We're updating a destination's flags... note that
      // we dynamically compute the active status based on
      // the sources, however, we must update the mute status
      // as requested.

      if (fuAction & MXDUPDATELINE_ACTIONF_MUTESTATUS)
      {
         if (fSet)
            pmi -> mxlDests[ uDest ].fdwLine |= IMIXERLINE_LINEF_MUTED ;
         else
            pmi -> mxlDests[ uDest ].fdwLine &= ~IMIXERLINE_LINEF_MUTED ;
      }
   }

   // Clear before we OR in the source status

   pmi -> mxlDests[ uDest ].fdwLine &= ~MIXERLINE_LINEF_ACTIVE ;

   // If this destination is muted, we're done, otherwise
   // check the sources connected to this destination.

   if (0 == (pmi -> mxlDests[ uDest ].fdwLine & IMIXERLINE_LINEF_MUTED))
   {
      if (uDest != DEST_LINEOUT)
      {
         dwMuxControlID =
            (uDest == DEST_WAVEIN) ? MUX_WAVEIN : MUX_VOICEIN ;

         // NOTE:  The mux value corresponds to the relative source
         //        number at that destination.

         uRelSource = (UINT) pmi -> dwValue[ dwMuxControlID ][ 0 ] ;
         if (MIXERLINE_LINEF_ACTIVE ==
               (pmi -> adwActiveMap[ uDest ][ uRelSource ] &
                  (MIXERLINE_LINEF_ACTIVE | IMIXERLINE_LINEF_MUTED)))
            pmi -> mxlDests[ uDest ].fdwLine |= MIXERLINE_LINEF_ACTIVE ;
      }
      else
      {
         // Line out destination is active when a source on the
         // destination is active.

         for (uRelSource = 0;
              uRelSource < pmi -> mxlDests[ uDest ].cConnections;
              uRelSource++)
         {
            if (MIXERLINE_LINEF_ACTIVE ==
                  (pmi -> adwActiveMap[ uDest ][ uRelSource ] &
                     (MIXERLINE_LINEF_ACTIVE | IMIXERLINE_LINEF_MUTED)))
               pmi -> mxlDests[ uDest ].fdwLine |= MIXERLINE_LINEF_ACTIVE ;
         }
      }
   }

   fdwCur = pmi -> mxlDests[ uDest ].fdwLine ;
   if (fdwCur & IMIXERLINE_LINEF_MUTED)
      fdwCur &= ~(IMIXERLINE_LINEF_MUTED | MIXERLINE_LINEF_ACTIVE) ;

   if (fdwPrev != fdwCur)
   {
      dwLineID = MAKELONG( uDest, 0xFFFF ) ;
      MxdNotifyLineChange( pmi, dwLineID ) ;
   }

} // MxdUpdateLine()

//--------------------------------------------------------------------------
//
//  MMRESULT VerifyControl
//
//  Description:
//      Verify that the control ID is valid and the associated
//      control is enabled.
//
//  Parameters:
//      PMIXERINSTANCE pmi
//         ptr -> mixer instance structure
//
//      DWORD dwControlID
//         control ID to validate
//
//  Return (MMRESULT):
//      MIXERR_INVALCONTROL if not in range or disabled, otherwise
//      MMSYSERR_NOERROR.
//
//
//--------------------------------------------------------------------------

MMRESULT NEAR PASCAL VerifyControl
(
    PMIXERINSTANCE   pmi,
    DWORD            dwControlID
)
{
   LPMIXERCONTROL  pmxc ;

   //
   //  did the caller specify a control # greater that what we have?
   //  (zero-offset)
   //
   if (dwControlID >= MAXCONTROLS)
      return MIXERR_INVALCONTROL ;

   //
   // Is the requested control enabled?
   //

   pmxc = &pmi -> mxc[ dwControlID ] ;

   if (MIXERCONTROL_CONTROLF_DISABLED & pmxc->fdwControl)
      return (MIXERR_INVALCONTROL);

   return (MMSYSERR_NOERROR);

} // VerifyControl()

//--------------------------------------------------------------------------
//
//  MMRESULT VerifyControlValue
//
//  Description:
//      Verifies that the proposed values are within a range.
//
//  Parameters:
//      PMIXERINSTANCE pmi
//         ptr -> mixer instance structure
//
//      LPMIXERCONTROLDETAILS pmxcd
//
//  Return (MMRESULT):
//      MMSYSERR_NOERROR if no problem
//
//
//--------------------------------------------------------------------------

MMRESULT NEAR PASCAL VerifyControlValue
(
    PMIXERINSTANCE          pmi,
    LPMIXERCONTROLDETAILS   pmxcd
)
{
    DWORD            dwControlID;
    LPMIXERCONTROL   pmxc;
    UINT             cChannels;

    MMRESULT         mmr ;
    UINT             uCurChannel ;

    dwControlID = pmxcd->dwControlID;
    mmr = VerifyControl( pmi, dwControlID ) ;
    if (mmr)
    {
        DPF( 1, "VerifyControlValue:Bad dwControlID" ) ;
        return (mmr);
    }

    pmxc = &pmi->mxc[dwControlID];
    cChannels = (UINT) pmxcd-> cChannels ;

    // Must be 1 or the number of channels in the control map

    if ((cChannels != 1) &&
        (cChannels != pmi -> auControlMap[ dwControlID ][CM_CHANNELS]))
       return MIXERR_INVALVALUE ;

    //
    // Is the value that the caller provided legal?
    //

    // Accept all boolean values...

    if (MIXERCONTROL_CT_UNITS_BOOLEAN ==
          (pmxc -> dwControlType & MIXERCONTROL_CT_UNITS_MASK))
          return MMSYSERR_NOERROR ;

    //
    // Is this control signed?
    //
    if ((MIXERCONTROL_CT_UNITS_SIGNED == (pmxc->dwControlType & MIXERCONTROL_CT_UNITS_MASK)) ||
        (MIXERCONTROL_CT_UNITS_DECIBELS == (pmxc->dwControlType & MIXERCONTROL_CT_UNITS_MASK)))
    {
        //
        // Make sure the value for each channel is valid.  We hardcode
        // here that we are using "int" values for these controls and
        // that we have room for two values.
        //
        for (uCurChannel = 0; uCurChannel < cChannels; uCurChannel++)
        {
            LPMIXERCONTROLDETAILS_SIGNED    pmxcd_s;

            //
            // Is the new value too big?
            //
            // Hardcode these as being on SIGNED boundry
            //
            pmxcd_s = &((LPMIXERCONTROLDETAILS_SIGNED)pmxcd->paDetails)[uCurChannel];

            D(
                char  szDebug[ 80 ] ;

                wsprintf(szDebug, "Verify(s): ctl: %ld (%s), channel: %d, value: %ld",
                        dwControlID,
                        (LPSTR)pmxc->szShortName,
                        uCurChannel,
                        pmxcd_s->lValue);
                DPF( 1, szDebug);
            )


            if (pmxcd_s->lValue > pmxc->Bounds.lMaximum)
            {
                D(
                char  szDebug[ 80 ] ;

                wsprintf(szDebug, "VerifyControlValue:New lValue was larger than lMaximum (%d) for this control",
                            pmxc->Bounds.lMaximum);
                DPF( 1, szDebug);
                )
                return MIXERR_INVALVALUE ;
            }

            //
            // Is the new value too small?
            //
            // Hardcode these as being on SIGNED stride boundry
            //

            if (pmxcd_s->lValue < pmxc->Bounds.lMinimum)
            {
                D(
                   char  szDebug[ 80 ] ;

                   wsprintf( szDebug, "VerifyControlValue:New lValue was smaller than lMinimum (%d) for this control",
                               pmxc->Bounds.lMinimum);
                   DPF( 1, szDebug);
                )
                return MIXERR_INVALVALUE ;
            }
        }
    }
    else
    {
        //
        // Make sure the value for each channel is valid.  We hardcode
        // here that we are using UNSIGNED values for these controls and
        // that we have room for two values.
        //

        for (uCurChannel = 0; uCurChannel < cChannels; uCurChannel++)
        {
            LPMIXERCONTROLDETAILS_UNSIGNED  pmxcd_u;

            pmxcd_u = &((LPMIXERCONTROLDETAILS_UNSIGNED)pmxcd->paDetails)[uCurChannel];

            D(
               char  szDebug[ 80 ] ;

               wsprintf( szDebug, "Verify(u): ctl: %lu (%s), channel: %d, value: %lu",
                         dwControlID,
                         pmxc->szShortName,
                         uCurChannel,
                         pmxcd_u->dwValue);
               DPF( 1, szDebug);
            )

            //
            // Is the new value too big?
            //
            // Hardcode these as being on int stride boundry
            //

            if (pmxcd_u->dwValue > pmxc->Bounds.dwMaximum)
            {
               DPF( 1, "VerifyControlValue:New uValue was larger than dwMaximum for this control" ) ;
               return MIXERR_INVALVALUE ;
            }

            //
            // Is the new value too small?
            //
            // Hardcode these as being on int stride boundry
            //

            if (pmxcd_u->dwValue < pmxc->Bounds.dwMinimum)
            {
               DPF( 1, "VerifyControlValue:New uValue was smaller than dwMinimum for this control" ) ;
               return MIXERR_INVALVALUE ;
            }
        }
    }

    //
    // The new value is just right...
    //

    return MMSYSERR_NOERROR ;

} // VerifyControlValue()

//--------------------------------------------------------------------------
//
//  VOID MxdAddClient
//
//  Description:
//      Adds the client to the device's client list.
//
//  Parameters:
//      PMIXERINSTANCE pmi
//         ptr -> mixer instance structure
//
//      PMXDCLIENT pmxdc
//         ptr -> client instance information
//
//  Return (VOID):
//      Nothing.
//
//
//--------------------------------------------------------------------------

VOID MxdAddClient
(
    PMIXERINSTANCE      pmi,
    PMXDCLIENT          pmxdc
)
{
   DPF( 3, "MxdAddClient()" ) ;

   pmxdc -> pNext = pmi -> pMxdClients ;
   pmi -> pMxdClients = pmxdc ;

} // MxdAddClient()

//--------------------------------------------------------------------------
//
//  VOID MxdRemoveClient
//
//  Description:
//      Removes the client from the device's client list
//
//  Parameters:
//      PMXDCLIENT pmxdc
//         ptr -> client instance information
//
//  Return (VOID):
//      Nothing.
//
//
//--------------------------------------------------------------------------

VOID MxdRemoveClient
(
    PMXDCLIENT          pmxdc
)
{
   PMIXERINSTANCE   pmi ;
   PMXDCLIENT       FAR *ppCur ;

   DPF( 3, "MxdRemoveClient()" ) ;

   pmi = pmxdc -> phwi -> pmi ;

   for (ppCur = &pmi -> pMxdClients ;
        *ppCur != NULL;
        ppCur = &(*ppCur)->pNext)
   {
      if (*ppCur == pmxdc)
      {
         *ppCur = (*ppCur)-> pNext ;
         break ;
      }
   }

} // MxdRemoveClient()

//==========================================================================;
//==========================================================================;
//==========================================================================;

//--------------------------------------------------------------------------;
//
//  VOID mxdGetDevCaps
//
//  Description:
//      This function handles the MXDM_GETDEVCAPS message for mxdMessage.
//      This message is sent by the Mixer Manager when the caller calls
//      mixerGetDevCaps.
//
//--------------------------------------------------------------------------;

VOID FAR PASCAL mxdGetDevCaps
(
    PHARDWAREINSTANCE   phwi,
    MDEVICECAPSEX FAR   *lpCaps
)
{
   char      szTemp[ 64 ] ;
   char      szClip[ 64 ] ;
   MIXERCAPS mc ;

   mc.wMid = MID_MICROSOFT ;
   mc.wPid = PID_MIXER ;
   mc.vDriverVersion = DRV_VERSION ;
   mc.fdwSupport = NULL ; /// (currently all reserved)
   mc.cDestinations = MAXDESTINATIONS ;
   LoadString( ghModule, SR_STR_DRIVERMIXER, szTemp, sizeof( szTemp ) ) ;
   wsprintf( szClip, szTemp, phwi -> wIOAddressCODEC ) ;
   lstrcpyn( mc.szPname, szClip, MAXPNAMELEN-1);

   _fmemcpy( lpCaps -> pCaps, &mc,
             min( (UINT) lpCaps -> cbSize, sizeof( mc ) ) ) ;

} // end of mxdGetDevCaps()

//--------------------------------------------------------------------------;
//
//  DWORD MxdOpen
//
//  Description:
//      This function handles the MXDM_OPEN message for mxdMessage. This
//      message is sent when a client calls mixerOpen. A client calls
//      mixerOpen to get one or more of following:
//
//      1.  Locks the mixer device such that it will not be removed until
//          the client closes the device.
//
//      2.  Allows a client to receive notifications of changes in the
//          mixer device state.
//
//      A mixer driver should be written to allow multiple opens on a
//      single device (thus allowing more than one client to receive
//      notifications).
//
//      Note that the Mixer Manager may or may NOT coalesce opens and closes
//      on a mixer device (depends on the situation) from applications.
//      This should make NO difference to the mixer device (and there is no
//      way for the mixer device to know). But if you are debugging your
//      driver and notice that only one MXDM_OPEN message gets sent even
//      if more than one application open the device, you now know why.
//
//  Arguments:
//      LPDWORD pdwUser: Pointer to _mixer device instance data store_.
//      The value that the mixer device places in this location will be
//      returned to the device as the dwUser argument in mxdMessage.
//      This value can be anything that the mixer device chooses.
//
//      LPMIXEROPENDESC pmxod: Pointer to a MIXEROPENDESC structure that
//      contains callback and other information for the client.
//
//      DWORD fdwOpen: Flags passed to mixerOpen. See the list of valid
//      flags for mixerOpen to determine what this argument may be.
//
//  Return (DWORD):
//      Returns zero (MMSYSERR_NOERROR) if successfully opened. Otherwise
//      returns a non-zero error code.
//
//
//--------------------------------------------------------------------------;

DWORD NEAR PASCAL MxdOpen
(
    PHARDWAREINSTANCE   phwi,
    LPDWORD             pdwUser,
    LPMIXEROPENDESC     pmxod,
    DWORD               fdwOpen
)
{
    PMXDCLIENT       pmxdc ;

    DPF( 3, "MxdOpen()" ) ;

    //
    //  if we cannot allocate our instance structure, then we must fail
    //  the open request.
    //
    pmxdc = (PMXDCLIENT) LocalAlloc( LPTR, sizeof( MXDCLIENT ) ) ;

    if (NULL == pmxdc)
    {
        DPF( 1, "MxdOpen() cannot LocalAlloc memory for instance data!" ) ;
        return (MMSYSERR_NOMEM);
    }

    //
    //  fill in our instance structure... save a bunch of stuff for
    //  callbacks.
    //

    pmxdc -> fdwOpen      = fdwOpen ;

    pmxdc -> hmx          = pmxod->hmx ;
    pmxdc -> dwCallback   = pmxod->dwCallback ;
    pmxdc -> fuCallback   = (UINT)HIWORD(fdwOpen & CALLBACK_TYPEMASK) ;
    pmxdc -> dwInstance   = pmxod->dwInstance ;
    pmxdc -> pNext        = NULL ;
    pmxdc -> phwi         = phwi ;

    //
    //  add to the device's client list
    //

    MxdAddClient( phwi -> pmi, pmxdc ) ;

    //
    //  return our instance data pointer--this will be returned in the
    //  dwUser argument for all other mixer driver messages that are
    //  on behalf of an opened instance..
    //

    *pdwUser = (DWORD)(UINT)pmxdc ;

    //
    //
    //
    return ( MMSYSERR_NOERROR ) ;

} // MxdOpen()


//--------------------------------------------------------------------------;
//
//  DWORD MxdClose
//
//  Description:
//      This function is called to handle the MXDM_CLOSE from mxdMessage.
//      This message is generated by a client calling mixerClose on a
//      previously mixerOpen'd mixer device handle. This function will never
//      be called unless it is for previously _successful_ call to
//      mixerOpen.
//
//  Arguments:
//      PMIXERINSTANCE pmi
//         ptr -> mixer instance structure
//
//      PMXDCLIENT pmxdc
//         Pointer to mixer device instance data allocated
//         by MxdOpen. This argument is passed as the dwUser
//         argument to mxdMessage.
//
//  Return (DWORD):
//      Should always succeed. Returns zero (MMSYSERR_NOERROR).
//
//
//--------------------------------------------------------------------------;

DWORD NEAR PASCAL MxdClose
(
    PMXDCLIENT      pmxdc
)
{

   DPF( 3, "MxdClose()" ) ;

   //
   // Note: pmxdc will always be valid - we will not get a close
   //       from msmixmgr (or mmsystem) unless an open has
   //       succeeded.
   //

   MxdRemoveClient( pmxdc ) ;
   LocalFree( (HLOCAL) pmxdc ) ;

   //
   //  return success
   //

   return MMSYSERR_NOERROR ;

} // MxdClose()

//--------------------------------------------------------------------------;
//
//  DWORD MxdGetLineInfo
//
//  Description:
//      This function handles the MXDM_GETLINEINFO message for mxdMessage.
//      The message is sent in response to a client calling mixerGetLineInfo.
//
//      There are currently five different query types that the caller can
//      use to get information on a line (specified in fdwInfo):
//
//      1.  MIXER_GETLINEINFOF_DESTINATION: caller wants information on
//          the MIXERLINE.dwDestination line.
//
//      2.  MIXER_GETLINEINFOF_SOURCE: caller wants information on the
//          MIXERLINE.dwSource associated with MIXERLINE.dwDestination.
//
//      3.  MIXER_GETLINEINFOF_LINEID: caller wants information on the
//          the MIXERLINE.dwLineID line.
//
//      4.  MIXER_GETLINEINFOF_COMPONENTTYPE: caller wants information on
//          the _first_ MIXERLINE.dwComponentType.
//
//      5.  MIXER_GETLINEINFOF_TARGETTYPE: caller wants information on the
//          the MIXERLINE.Target.dwType (wMid, wPid, etc) line.
//
//      All mixer drivers must support these four queries. If a query
//      is sent that the mixer device does not know how to handle, then
//      MMSYSERR_NOTSUPPORTED must be returned.
//
//  Arguments:
//      PMXDCLIENT pmxdc
//         Pointer to mixer device instance data allocated
//         by MxdOpen (if the mixer device was opened!).
//
//      LPMIXERLINE pmxl
//         Pointer to caller's receiving buffer for the line information.
//         This pointer has been validated by the Mixer Manager to be at
//         least big enough for all input arguments and big enough to
//         hold MIXERLINE.cbStruct bytes of information.
//
//      DWORD fdwInfo
//         Flags passed from mixerGetLineInfo.
//
//  Return (DWORD):
//      Returns zero (MMSYSERR_NOERROR) if successfull. Otherwise
//      returns a non-zero error code.
//
//
//--------------------------------------------------------------------------;

DWORD NEAR PASCAL MxdGetLineInfo
(
    PMXDCLIENT          pmxdc,
    LPMIXERLINE         pmxl,
    DWORD               fdwInfo
)
{
    UINT                uDestination ;
    UINT                uSource ;
    UINT                uSourceLine ;
    LPMIXERLINE         pmxlToCopy ;
    UINT                cbmxl ;
    PMIXERINSTANCE      pmi ;

    if (pmxdc == NULL)
    {
       DPF( 1, "MxdGetLineInfo: pmxdc is NULL!\r\n" ) ;
       return MMSYSERR_INVALHANDLE ;
    }

    if (!pmxdc -> phwi -> fEnabled)
       return MMSYSERR_NOTENABLED ;

    pmi = pmxdc -> phwi -> pmi ;


    //  we will fill in the following elements dynamically:
    //
    //      dwDestination
    //      dwSource
    //      dwLineID
    //
    //  so calculate how much _extra_ information we need to copy.
    //

    cbmxl  = (UINT)min(pmxl->cbStruct, sizeof(MIXERLINE));
    pmxl->cbStruct = cbmxl;
    cbmxl -= FIELD_OFFSET(MIXERLINE, fdwLine);

    //
    //  determine what line to get the information for. a mixer driver
    //  MUST support the following four queries:
    //
    //      MIXER_GETLINEINFOF_DESTINATION
    //      MIXER_GETLINEINFOF_SOURCE
    //      MIXER_GETLINEINFOF_LINEID
    //      MIXER_GETLINEINFOF_COMPONENTTYPE
    //
    //
    //  others (no others are defined for V1.00 of MSMIXMGR) can optionally
    //  be supported. if this mixer driver does NOT support a query, then
    //  MMSYSERR_NOTSUPPORTED must be returned.
    //

    switch (fdwInfo & MIXER_GETLINEINFOF_QUERYMASK)
    {
        //
        //  MIXER_GETLINEINFOF_DESTINATION
        //
        //  this query specifies that the caller is interested in the
        //  line information for MIXERLINE.dwDestination. this index can
        //  range from 0 to MIXERCAPS.cDestinations - 1.
        //
        //  valid elements of MIXERLINE:
        //      cbStruct
        //      dwDestination
        //
        //  all other MIXERLINE elements are undefined.
        //

        case MIXER_GETLINEINFOF_DESTINATION:
            DPF( 3, "---MxdGetLineInfo: by destination" ) ;

mxd_Get_Line_Info_Destination:

            if (pmxl->dwDestination > (MAXDESTINATIONS - 1))
            {
                DPF( 1, "MxdGetLineInfo: caller specified an invalid destination." ) ;
                return (MIXERR_INVALLINE);
            }

            uDestination = (UINT)pmxl->dwDestination;


            //
            //  return the dwLineID
            //
            pmxl->dwLineID = MAKELONG(uDestination, 0xFFFF);

            pmxlToCopy = &pmi->mxlDests[uDestination];

            _fmemcpy( &pmxl -> fdwLine, &pmxlToCopy -> fdwLine, cbmxl ) ;

            // Fix up line flags...

            if (pmxl -> fdwLine & IMIXERLINE_LINEF_MUTED)
               pmxl -> fdwLine &=
                  ~(IMIXERLINE_LINEF_MUTED | MIXERLINE_LINEF_ACTIVE) ;

            return (MMSYSERR_NOERROR);


        //
        //  MIXER_GETLINEINFOF_SOURCE
        //
        //  this query specifies that the caller is interested in the
        //  line information for MIXERLINE.dwSource associated with
        //  MIXERLINE.dwDestination.
        //
        //  valid elements of MIXERLINE:
        //      cbStruct
        //      dwDestination
        //      dwSource
        //
        //  all other MIXERLINE elements are undefined.
        //

        case MIXER_GETLINEINFOF_SOURCE:
            DPF( 3, "---MxdGetLineInfo: by source" ) ;

mxd_Get_Line_Info_Source:

            if (pmxl->dwDestination > (MAXDESTINATIONS - 1))
            {
                DPF( 1, "MxdGetLineInfo: caller specified an invalid destination." ) ;
                return (MIXERR_INVALLINE);
            }

            if (pmxl->dwSource > (MAXSOURCES - 1))
            {
                DPF( 1, "MxdGetLineInfo: caller specified an invalid source." ) ;
                return (MIXERR_INVALLINE);
            }

            uDestination = (UINT)pmxl->dwDestination;
            uSource      = (UINT)pmxl->dwSource;

            uSourceLine  = pmi -> auSourceMap[uDestination][uSource];

            // Is there really a source here???
            if (SOURCE_INVALID == uSourceLine)
            {
                DPF( 1, "MxdGetLineInfo: The caller specified an invalid source at the dest." ) ;
                return (MIXERR_INVALLINE);
            }


            //
            //  return the dwLineID
            //
            pmxl->dwLineID = MAKELONG(uDestination, uSource);

            pmxlToCopy = &pmi->mxlSources[uSourceLine];
            _fmemcpy( &pmxl -> fdwLine, &pmxlToCopy -> fdwLine, cbmxl ) ;

            // OR in the connected/active flags from the active map

            pmxl -> fdwLine |=
               pmi -> adwActiveMap[ uDestination ][ uSource ] ;
            if (pmxl -> fdwLine & IMIXERLINE_LINEF_MUTED)
               pmxl -> fdwLine &=
                  ~(IMIXERLINE_LINEF_MUTED | MIXERLINE_LINEF_ACTIVE) ;

            // Now fill in the uControls field...

            pmxl->cControls =
               pmi -> auSourceControlsMap[uDestination][uSource];

            return (MMSYSERR_NOERROR);


        //
        //  MIXER_GETLINEINFOF_LINEID
        //
        //  this query specifies that the caller is interested in the
        //  line information for MIXERLINE.dwLineID. the dwLineID is
        //  completely mixer driver dependent, so this driver must validate
        //  the ID.
        //
        //  valid elements of MIXERLINE:
        //      cbStruct
        //      dwLineID
        //
        //  all other MIXERLINE elements are undefined.
        //

        case MIXER_GETLINEINFOF_LINEID:
            DPF( 3, "MxdGetLineInfo: by lineid" ) ;

            uDestination = LOWORD(pmxl->dwLineID);
            uSource      = HIWORD(pmxl->dwLineID);

            pmxl->dwDestination = uDestination;
            pmxl->dwSource      = uSource;
            if (0xFFFF == uSource)
            {
                goto mxd_Get_Line_Info_Destination;
            }

            goto mxd_Get_Line_Info_Source;



        //
        //  MIXER_GETLINEINFOF_COMPONENTTYPE
        //
        //  this query specifies that the caller is interested in the
        //  line information for MIXERLINE.dwComponentType
        //
        //  valid elements of MIXERLINE:
        //      cbStruct
        //      dwComponentType
        //
        //  all other MIXERLINE elements are undefined.
        //

        case MIXER_GETLINEINFOF_COMPONENTTYPE:
            DPF( 3, "MxdGetLineInfo: by componenttype" ) ;

            // Walk destinations first...

            for (uDestination = 0;
                 uDestination < MAXDESTINATIONS;
                 uDestination++)
                 if (pmxl->dwComponentType ==
                        pmi -> mxlDests[ uDestination ].dwComponentType)
                    break ;


            if (uDestination != MAXDESTINATIONS)
            {
               pmxl -> dwDestination = uDestination ;
               goto mxd_Get_Line_Info_Destination;
            }

            // Walk sources...

            for (uDestination = 0;
                 uDestination < MAXDESTINATIONS;
                 uDestination++)
            {
               // Get num sources from source map

               for (uSource = 0;
                    uSource < pmi -> mxlDests[ uDestination ].cConnections;
                    uSource++)
               {
                 if (pmxl -> dwComponentType ==
                        pmi -> mxlSources[ pmi -> auSourceMap[ uDestination ][ uSource ] ].dwComponentType)
                    break ;
               }
               if (uSource != pmi -> mxlDests[ uDestination ].cConnections)
               {
                  pmxl -> dwDestination = uDestination ;
                  pmxl -> dwSource = uSource ;
                  goto mxd_Get_Line_Info_Source ;
               }
            }

            DPF( 1,  "MxdGetLineInfo: COMPONENTTYPE not found" ) ;

            return ( MIXERR_INVALLINE ) ;


        //
        //  MIXER_GETLINEINFOF_TARGETTYPE
        //
        //  this query specifies that the caller is interested in the
        //  line information for MIXERLINE.Target.
        //
        //  valid elements of MIXERLINE:
        //      cbStruct
        //      Target.dwType
        //      Target.wMid
        //      Target.wPid
        //      Target.vDriverVersion
        //      Target.szPname
        //
        //  all other MIXERLINE elements are undefined.
        //

        case MIXER_GETLINEINFOF_TARGETTYPE:
            D(
               char  szDebug[ 80 ] ;

               wsprintf( szDebug, "wMid = %d", pmxl -> Target.wMid ) ;
               DPF( 3, szDebug ) ;
               wsprintf( szDebug, "wPid = %d", pmxl -> Target.wPid ) ;
               DPF( 3, szDebug ) ;

               wsprintf( szDebug, "vVersion = %x", pmxl -> Target.vDriverVersion ) ;
               DPF( 3, szDebug ) ;

            ) ;

            switch (pmxl->Target.dwType)
            {
                case MIXERLINE_TARGETTYPE_WAVEOUT:
                    if ( 	(pmi -> mxlSources[ SOURCE_WAVEOUT ].Target.wPid != pmxl->Target.wPid)
								|| (pmi -> mxlSources[ SOURCE_WAVEOUT ].Target.wMid != pmxl->Target.wMid)
								|| (pmi -> mxlSources[ SOURCE_WAVEOUT ].Target.vDriverVersion != pmxl->Target.vDriverVersion)
							)
						return (MIXERR_INVALLINE);
						break;

                case MIXERLINE_TARGETTYPE_WAVEIN:
                    if ( 	(pmi -> mxlDests[ DEST_WAVEIN ].Target.wPid != pmxl->Target.wPid)
								|| (pmi -> mxlDests[ DEST_WAVEIN ].Target.wMid != pmxl->Target.wMid)
								|| (pmi -> mxlDests[ DEST_WAVEIN ].Target.vDriverVersion != pmxl->Target.vDriverVersion)
							)
							  return (MIXERR_INVALLINE);
                    break;

                case MIXERLINE_TARGETTYPE_MIDIOUT:
                    if ( 	(pmi -> mxlSources[ SOURCE_MIDIOUT ].Target.wPid != pmxl->Target.wPid)
								|| (pmi -> mxlSources[ SOURCE_MIDIOUT ].Target.wMid != pmxl->Target.wMid)
								|| (pmi -> mxlSources[ SOURCE_MIDIOUT ].Target.vDriverVersion != pmxl->Target.vDriverVersion)
							)
                        return (MIXERR_INVALLINE);

                    break;

                case MIXERLINE_TARGETTYPE_MIDIIN:
                    return (MIXERR_INVALLINE);

                case MIXERLINE_TARGETTYPE_AUX:
                    if ( 	(pmi -> mxlSources[ SOURCE_AUX1 ].Target.wPid != pmxl->Target.wPid)
								|| (pmi -> mxlSources[ SOURCE_AUX1 ].Target.wMid != pmxl->Target.wMid)
								|| (pmi -> mxlSources[ SOURCE_AUX1 ].Target.vDriverVersion != pmxl->Target.vDriverVersion)
							  )
                        return (MIXERR_INVALLINE);
                    break;

                default:
                    return (MIXERR_INVALLINE);
            }

            DPF( 3, "MxdGetLineInfo: wPid successful" ) ;

            // Walk destinations first...

            DPF( 3, "MxdGetLineInfo - target: walking destinations..." ) ;

            for (uDestination = 0;
                 uDestination < MAXDESTINATIONS;
                 uDestination++)
                 if ((pmxl->Target.dwType ==
                        pmi -> mxlDests[ uDestination ].Target.dwType) &&
                     (0 ==
                        lstrcmpi( pmxl -> Target.szPname,
                                  pmi -> mxlDests[ uDestination ].Target.szPname )))
                    break ;

            if (uDestination != MAXDESTINATIONS)
            {
               pmxl -> dwDestination = uDestination ;
               goto mxd_Get_Line_Info_Destination;
            }

            // Walk sources...

            DPF( 4, "MxdGetLineInfo - target: walking sources..." ) ;

            for (uDestination = 0;
                 uDestination < MAXDESTINATIONS;
                 uDestination++)
            {
               // Get num sources from source map

               for (uSource = 0;
                    uSource < pmi -> mxlDests[ uDestination ].cConnections;
                    uSource++)
               {
                 if ((pmxl->Target.dwType ==
                        pmi -> mxlSources[ pmi -> auSourceMap[ uDestination ][ uSource ] ].Target.dwType) &&
                     (0 ==
                        lstrcmpi( pmxl -> Target.szPname,
                                  pmi -> mxlSources[ pmi -> auSourceMap[ uDestination ][ uSource ] ].Target.szPname )))
                    break ;
               }
               if (uSource != pmi -> mxlDests[ uDestination ].cConnections)
               {
                  pmxl -> dwDestination = uDestination ;
                  pmxl -> dwSource = uSource ;
                  goto mxd_Get_Line_Info_Source ;
               }
            }

            DPF( 1,  "MxdGetLineInfo: TARGETTYPE not found" ) ;

            return ( MIXERR_INVALLINE ) ;

        //
        //  if the query type is not something this driver understands, then
        //  return MMSYSERR_NOTSUPPORTED.
        //
        default:
            return (MMSYSERR_NOTSUPPORTED);
    }

} // MxdGetLineInfo()

//--------------------------------------------------------------------------;
//
//  DWORD MxdGetLineControls
//
//  Description:
//      This function handles the MXDM_GETLINECONTROLS message for
//      mxdMessage. The message is sent in response to a client calling
//      mixerGetLineControls.
//
//      There are currently three different query types that the caller can
//      use to get information on a mixer's controls (specified in
//      fdwControls):
//
//      1.  MIXER_GETLINECONTROLSF_ALL: caller wants all controls for
//          a specified line (dwLineID).
//
//      2.  MIXER_GETLINECONTROLSF_ONEBYID: caller wants one control
//          specified by dwControlID.
//
//      3.  MIXER_GETLINECONTROLSF_ONEBYTYPE: caller wants the FIRST control
//          on the specified line (dwLineID) of type dwControlType.
//
//      All mixer drivers must support these three queries. If a query
//      is sent that the mixer device does not know how to handle, then
//      MMSYSERR_NOTSUPPORTED must be returned.
//
//  Arguments:
//      PMXDCLIENT pmxdc: Pointer to mixer device instance data allocated
//      by MxdOpen (if the mixer device was opened!). This argument MAY be
//      NULL if the caller did not open the mixer device.
//
//      LPMIXERLINECONTROLS pmxlc: Pointer to line control information
//      header. This pointer has been validated by the Mixer Manager to
//      be at least sizeof(MIXERLINECONTROLS) in size (currently the only
//      valid size for the header).
//
//      Also note that cControls has been validated to be >= 1. cbmxctrl
//      is at least sizeof(MIXERCONTROL) and pmxctrl points to a memory
//      location big enough to hold (cControls * cbmxctrl) bytes.
//
//      DWORD fdwControls: Flags passed from mixerGetLineControls.
//
//  Return (DWORD):
//      Returns zero (MMSYSERR_NOERROR) if successfull. Otherwise
//      returns a non-zero error code.
//
//
//--------------------------------------------------------------------------;

DWORD NEAR PASCAL MxdGetLineControls
(
    PMXDCLIENT              pmxdc,
    LPMIXERLINECONTROLS     pmxlc,
    DWORD                   fdwControls
)
{
    PMIXERINSTANCE      pmi;
    MMRESULT            mmr;
    LPMIXERCONTROL      pmxctrl;
    UINT                uDestination;
    UINT                uSource;
    UINT                uCurControl;
    DWORD               uNumControlsToCopy;

    if (pmxdc == NULL)
    {
       DPF( 1, "MxdGetLineControls: pmxdc is NULL!\r\n" ) ;
       return MMSYSERR_INVALHANDLE ;
    }

    if (!pmxdc -> phwi -> fEnabled)
       return MMSYSERR_NOTENABLED ;

    pmi = pmxdc -> phwi -> pmi ;
    pmxctrl = pmxlc->pamxctrl ;

    //
    //  Determine for which control(s) to get the information.
    //  A mixer driver MUST support the following three queries:
    //
    //      MIXER_GETLINECONTROLSF_ALL
    //      MIXER_GETLINECONTROLSF_ONEBYID
    //      MIXER_GETLINECONTROLSF_ONEBYTYPE
    //
    //  Others (no others are defined for V1.00 of MSMIXMGR) can optionally
    //  be supported. If this mixer driver does NOT support a query, then
    //  MMSYSERR_NOTSUPPORTED must be returned.
    //

    switch (fdwControls & MIXER_GETLINECONTROLSF_QUERYMASK)
    {
        //
        //  MIXER_GETLINECONTROLSF_ALL
        //
        //  This query specifies that the caller is interested in ALL
        //  controls for a line.
        //
        //  Valid elements of MIXERLINECONTROLS:
        //      cbStruct
        //      dwLineID
        //      cControls
        //      cbmxctrl
        //      pamxctrl
        //
        //  All other MIXERLINECONTROLS elements are undefined.
        //

        case MIXER_GETLINECONTROLSF_ALL:
            DPF( 3, "MxdGetLineControls: all" ) ;
            uDestination = LOWORD(pmxlc->dwLineID);
            if (uDestination > (MAXDESTINATIONS - 1))
            {
                DPF( 1, "MxdGetLineControls: caller specified an invalid dwLineID." ) ;
                return (MIXERR_INVALLINE);
            }

            uSource = (UINT)(int)HIWORD(pmxlc->dwLineID);
            if (UINT_MAX == uSource)
            {
                uNumControlsToCopy = pmi->mxlDests[uDestination].cControls;
            }
            else
            {
                if (uSource > (MAXSOURCES - 1))
                {
                    DPF( 1, "MxdGetLineControls: caller specified an invalid dwLineID." ) ;
                    return (MIXERR_INVALLINE);
                }

                uNumControlsToCopy = pmi -> auSourceControlsMap[uDestination][uSource];
            }

            if (uNumControlsToCopy != pmxlc->cControls)
            {
                DPF( 1, "MxdGetLineControls: caller specified an invalid cControls." ) ;
                return (MMSYSERR_INVALPARAM);
            }

            //
            // Copy the requested controls
            //
            for (uCurControl = 0; uCurControl < MAXCONTROLS; uCurControl++)
            {
                //
                //  make sure we only copy the controls we want
                //
                if ((pmi -> auControlMap[uCurControl][0] == uDestination) &&
                    (pmi -> auControlMap[uCurControl][1] == uSource))
                {
                    _fmemcpy( pmxctrl, &pmi -> mxc[ uCurControl ],
                              (WORD) pmi -> mxc[ uCurControl ].cbStruct ) ;

                    //
                    //  jump to the next structure--the step size is based on
                    //  the caller's specified structure size..
                    //

                    ((LPBYTE)pmxctrl) += pmxlc->cbmxctrl;
                }
            }

            //
            //  tell the caller what is really valid in the structure
            //
            pmxlc->cbmxctrl = sizeof(MIXERCONTROL);

            return (MMSYSERR_NOERROR);


        //
        //  MIXER_GETLINECONTROLSF_ONEBYID
        //
        //  This query specifies that the caller is interested in ONE
        //  control specified by dwControlID.
        //
        //  valid elements of MIXERLINECONTROLS:
        //      cbStruct
        //      dwControlID
        //      cbmxctrl
        //      pamxctrl
        //
        //  all other MIXERLINECONTROLS elements are undefined.
        //

        case MIXER_GETLINECONTROLSF_ONEBYID:
            DPF( 3, "MxdGetLineControls: by id" ) ;

            //
            // Make sure the control ID they gave us is OK.
            //
            mmr = VerifyControl( pmi, pmxlc->dwControlID ) ;
            if (mmr)
            {
                DPF( 1, "MxdGetLineControls:ONEBYID - Invalid dwControlID." ) ;
                return (mmr);
            }

            //
            //
            //
            pmxlc->cbmxctrl = pmi->mxc[pmxlc->dwControlID].cbStruct;

            _fmemcpy( pmxctrl, &pmi -> mxc[ pmxlc -> dwControlID ],
                      (WORD) pmxlc -> cbmxctrl ) ;

            pmxlc -> dwLineID =
               MAKELONG( pmi -> auControlMap[ pmxlc -> dwControlID ][ 0 ],
                         pmi -> auControlMap[ pmxlc -> dwControlID ][ 1 ] ) ;

            return ( MMSYSERR_NOERROR ) ;

        //
        //  MIXER_GETLINECONTROLSF_ONEBYTYPE
        //
        //  This query specifies that the caller is interested in the
        //  FIRST control of type dwControlType on dwLineID.
        //
        //  Valid elements of MIXERLINECONTROLS:
        //      cbStruct
        //      dwLineID
        //      dwControlType
        //      cbmxctrl
        //      pamxctrl
        //
        //  all other MIXERLINECONTROLS elements are undefined.
        //

        case MIXER_GETLINECONTROLSF_ONEBYTYPE:
            DPF( 3, "MxdGetLineControls: by type" ) ;
            uDestination = LOWORD(pmxlc->dwLineID);
            if (uDestination > (MAXDESTINATIONS - 1))
            {
                DPF( 1, "MxdGetLineControls: caller specified an invalid dwLineID." ) ;
                return (MIXERR_INVALLINE);
            }

            uSource = (UINT)(int)HIWORD(pmxlc->dwLineID);

            if ((UINT_MAX != uSource) && (uSource > (MAXSOURCES - 1)))
            {
                DPF( 1, "MxdGetLineControls: caller specified an invalid dwLineID." ) ;
                return (MIXERR_INVALLINE);
            }

            for (uCurControl = 0; uCurControl < MAXCONTROLS; uCurControl++)
            {
                // Has to be on this source/dest pair

                if ((pmi -> auControlMap[uCurControl][0] == uDestination) &&
                    (pmi -> auControlMap[uCurControl][1] == uSource) &&
                    (pmi -> mxc[uCurControl].dwControlType == pmxlc -> dwControlType))
                    break ;
            }
            if (MAXCONTROLS == uCurControl)
               return (MIXERR_INVALCONTROL) ;


            //
            //
            //
            pmxlc->cbmxctrl = pmi->mxc[uCurControl].cbStruct ;

            _fmemcpy( (LPSTR) pmxctrl, (LPSTR) &pmi -> mxc[ uCurControl ],
                      (WORD) pmxlc -> cbmxctrl ) ;

            return ( MMSYSERR_NOERROR ) ;


        //
        //  If the query type is not something this driver understands,
        //  then return MMSYSERR_NOTSUPPORTED.
        //
        default:
            return (MMSYSERR_NOTSUPPORTED);
    }

} // MxdGetLineControls()

//--------------------------------------------------------------------------;
//
//  MMRESULT MxdGetControlDetails
//
//  Description:
//
//
//  Arguments:
//      PHARDWAREINSTANCE phwi
//
//      LPMIXERCONTROLDETAILS pmxcd
//
//      DWORD fdwDetails
//
//  Return (MMRESULT):
//
//
//--------------------------------------------------------------------------;

MMRESULT FAR PASCAL MxdGetControlDetails
(
    PHARDWAREINSTANCE       phwi,
    LPMIXERCONTROLDETAILS   pmxcd,
    DWORD                   fdwDetails
)
{
    PMIXERINSTANCE      pmi;
    MMRESULT            mmr;
    UINT                cChannels;
    DWORD               dwControlID;
    DWORD               dwControlType;

    pmi = phwi -> pmi ;

    if (!phwi -> fEnabled)
       return MMSYSERR_NOTENABLED ;

    //
    //  verify that the control id is not bogus..
    //

    dwControlID = pmxcd->dwControlID ;

    mmr = VerifyControl( pmi, dwControlID ) ;
    if (mmr)
    {
        DPF( 1, "GetControlDetails:There was something wrong with the dwControlID param" ) ;
        return (mmr);
    }

    cChannels     = pmi -> auControlMap[dwControlID][CM_CHANNELS];
    dwControlType = pmi->mxc[dwControlID].dwControlType;

    switch (MIXER_GETCONTROLDETAILSF_QUERYMASK & fdwDetails)
    {
        //
        // If a multichannel control is queried with cChannels == 1
        // this routine must return a combination of left/right
        // channels.
        //

        case MIXER_GETCONTROLDETAILSF_VALUE:
        {

            switch (dwControlType)
            {
               case MIXERCONTROL_CONTROLTYPE_MUX:
               {
                  UINT                           uCurItem ;
                  LPMIXERCONTROLDETAILS_BOOLEAN  pmxcd_f ;

                  DPF( 1,  "Retrieving mux value..." ) ;

                  pmxcd_f = (LPMIXERCONTROLDETAILS_BOOLEAN)pmxcd->paDetails ;

                  for (uCurItem = 0;
                       uCurItem < pmxcd -> cMultipleItems ;
                       uCurItem++)
                  {
                     pmxcd_f -> fValue =
                        (pmi -> dwValue[ dwControlID ][ 0 ] == uCurItem) ;

                     //
                     //  move to the next entry
                     //

                     ((LPBYTE)pmxcd_f) += pmxcd->cbDetails;
                  }
                  pmxcd->cbDetails  = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
                  pmxcd->cChannels  = 1 ;

                  return (MMSYSERR_NOERROR);
               }
               break ;

               case MIXERCONTROL_CONTROLTYPE_PEAKMETER:

                  //
                  //  In this implementation, we only have one
                  //  peak meter routine since we don't allow simultaneous
                  //  WAVEOUT and WAVEIN.
                  //
                  //  We call the MxdPeakMeter routine and have it write
                  //  directly into our current settings array.
                  //
                  //  NOTE! this meter should NEVER fail unless an invalid
                  //  control id was passed. the return values should be
                  //  zero if the line associated with the meter is not
                  //  active.
                  //

                  MxdPeakMeter( phwi, dwControlID,
                                &pmi->dwValue[dwControlID][0] ) ;

                  // fall-through...

               default:
               {
                  LPMIXERCONTROLDETAILS_SIGNED    pmxcd_s ;
                  LPMIXERCONTROLDETAILS_UNSIGNED  pmxcd_u ;
                  LPMIXERCONTROLDETAILS_BOOLEAN   pmxcd_f ;
                  UINT                            uCurChannel ;

                  //
                  //  use type signed for copying the values--we don't
                  //  interpret the information here and all three types
                  //  of details structures that this mixer driver uses are
                  //  the same size in bits...
                  //

                  if (cChannels == pmxcd -> cChannels)
                  {
                     pmxcd_s = (LPMIXERCONTROLDETAILS_SIGNED)pmxcd->paDetails;

                     for (uCurChannel = 0; uCurChannel < cChannels; uCurChannel++)
                     {
                        pmxcd_s->lValue = pmi->lValue[dwControlID][uCurChannel];

                        //
                        //  move to the next entry
                        //

                        ((LPBYTE)pmxcd_s) += pmxcd->cbDetails;
                     }
                  }
                  else
                  {
                     // Better be all or one... anything else is invalid

                     if (pmxcd -> cChannels != 1)
                        return ( MMSYSERR_INVALPARAM ) ;

                     //
                     // Create a composite value of multichannel settings.
                     //
                     // See the mixer specification for details on how
                     // this should be handled for various multichannel
                     // control types.
                     //

                     //
                     // NOTE!!!! This will NOT WORK for cChannels > 2.
                     // Because our lines are all stereo lines, the composite
                     // values are computed using left & right channels
                     // only and do not consider additional channels for
                     // the composite value.
                     //
                     //

                     switch (dwControlType &  MIXERCONTROL_CT_CLASS_MASK)
                     {
                        case MIXERCONTROL_CT_CLASS_SWITCH:
                        {
                           pmxcd_f =
                              (LPMIXERCONTROLDETAILS_BOOLEAN) pmxcd->paDetails;

                           pmxcd_f -> fValue =
                              ((pmi -> dwValue[ dwControlID ][ 0 ] +
                                pmi -> dwValue[ dwControlID ][ 1 ]) > 0) ;

                        }
                        break ;

                        case MIXERCONTROL_CT_CLASS_METER:
                        case MIXERCONTROL_CT_CLASS_NUMBER:
                        case MIXERCONTROL_CT_CLASS_TIME:
                        {
                           // Assumes signed values...

                           pmxcd_s =
                              (LPMIXERCONTROLDETAILS_SIGNED) pmxcd->paDetails;

                           pmxcd_s -> lValue =
                              (abs(pmi->lValue[dwControlID][0]) >
                               abs(pmi->lValue[dwControlID][1])) ?
                                  pmi->lValue[dwControlID][0] :
                                  pmi->lValue[dwControlID][1] ;
                        }
                        break ;

                        case MIXERCONTROL_CT_CLASS_SLIDER:
                        case MIXERCONTROL_CT_CLASS_FADER:
                        {
                           // Assumes unsigned values...

                           pmxcd_u =
                              (LPMIXERCONTROLDETAILS_UNSIGNED) pmxcd->paDetails;

                           pmxcd_u -> dwValue =
                              (pmi->dwValue[dwControlID][0] >
                               pmi->dwValue[dwControlID][1]) ?
                                  pmi->dwValue[dwControlID][0] :
                                  pmi->dwValue[dwControlID][1] ;
                        }
                        break ;
                     }
                  }

                  //
                  //  return actual sizes of things back to caller
                  //

                  pmxcd->cbDetails  = sizeof( MIXERCONTROLDETAILS_SIGNED ) ;
                  pmxcd->cMultipleItems = 0 ;

                  return (MMSYSERR_NOERROR) ;
               }
            }

        }


        case MIXER_GETCONTROLDETAILSF_LISTTEXT:
        {
            int                             i ;
            UINT                            uSource, uDest ;
            LPMIXERCONTROLDETAILS_LISTTEXT  pmxcd_lt;

            //
            //  verify control type is MUX (the only valid type for this
            //  mixer driver that has a listtext).
            //

            if (MIXERCONTROL_CONTROLTYPE_MUX != dwControlType)
            {
                DPF( 1, "GetControlDetails: query listtext and controltype not MUX?!?" ) ;
                return (MIXERR_INVALCONTROL);
            }

            pmxcd_lt = (LPMIXERCONTROLDETAILS_LISTTEXT) pmxcd->paDetails ;

            //
            //  copy mux settings
            //
            //      dwParam1 = dwLineID
            //      dwParam2 = dwComponentType
            //

            // NOTE: dwControlID has already been validated...

            D(
               char  szDebug[ 80 ] ;

               wsprintf( szDebug, "mult items = %d",
                         (int) pmxcd -> cMultipleItems ) ;
               DPF( 1,  szDebug ) ;
            ) ;

            for (i = 0; i < (int) pmxcd -> cMultipleItems; i++)
            {
               // Compute destination...

               uDest =
                  (dwControlID == MUX_WAVEIN) ? DEST_WAVEIN : DEST_VOICEIN ;

               pmxcd_lt -> dwParam1 = MAKELONG( uDest, i ) ;
               uSource = pmi -> auSourceMap[ uDest ][ i ] ;
               pmxcd_lt -> dwParam2 =
                  pmi -> mxlSources[ uSource ].dwComponentType ;

               // NOTE:  szName is "long name" - apps should use
               // the dwLineID to obtain the short name if needed.

               lstrcpy( pmxcd_lt -> szName,
                        pmi -> mxlSources[ uSource ].szName ) ;

               ((LPBYTE)pmxcd_lt) += pmxcd->cbDetails ;
            }

            //
            //  return actual sizes of things back to caller
            //

            pmxcd->cbDetails  = sizeof(MIXERCONTROLDETAILS_LISTTEXT) ;
            pmxcd->cChannels  = 0 ;
            pmxcd->cMultipleItems = i ;

            return (MMSYSERR_NOERROR) ;
        }

        //
        //  if the query type is not something this driver understands, then
        //  return MMSYSERR_NOTSUPPORTED.
        //

        default:
            return (MMSYSERR_NOTSUPPORTED);
    }

} // MxdGetControlDetails()

//--------------------------------------------------------------------------;
//
//  MMRESULT MxdSetControlDetails
//
//  Description:
//
//
//  Arguments:
//      PHARDWAREINSTANCE phwi
//
//      LPMIXERCONTROLDETAILS pmxcd
//
//      DWORD fdwDetails
//
//  Return (MMRESULT):
//
//
//--------------------------------------------------------------------------;

MMRESULT FAR PASCAL MxdSetControlDetails
(
   PHARDWAREINSTANCE       phwi,
   LPMIXERCONTROLDETAILS   pmxcd,
   DWORD                   fdwDetails
)
{
   PMXDCLIENT          pCur ;
   PMIXERINSTANCE      pmi;
   MMRESULT            mmr;
   UINT                cChannels;
   UINT                uCurChannel;
   UINT                uCurItem;
   DWORD               dwControlID;

   // For future expandability...

   if (MIXER_SETCONTROLDETAILSF_VALUE !=
         (fdwDetails & MIXER_SETCONTROLDETAILSF_QUERYMASK))
         return ( MMSYSERR_NOTSUPPORTED ) ;

   pmi = phwi -> pmi ;

   if (!phwi -> fEnabled)
      return MMSYSERR_NOTENABLED ;

   //
   //  verify that the control id is not bogus..
   //

   dwControlID = pmxcd->dwControlID ;

   mmr = VerifyControl( pmi, dwControlID ) ;
   if (mmr)
   {
      DPF( 1, "SetControlDetails:There was something wrong with the dwControlID param" ) ;
      return (mmr);
   }

   cChannels = pmi -> auControlMap[dwControlID][CM_CHANNELS] ;

   switch (pmi -> mxc[ dwControlID ].dwControlType)
   {
      case MIXERCONTROL_CONTROLTYPE_MUX:
      {
         UINT                           uDest, uActiveDest ;
         LPMIXERCONTROLDETAILS_BOOLEAN  pmxcd_f ;

         //
         //  NOTES:
         //
         //  If a control affects the line status (connected, active, etc.)
         //  the control notification must occur after line notification!
         //

         DPF( 1,  "Setting mux..." ) ;

         //
         // For SNDSYS, this is the only multiple item control!  This
         // code should be modified to handle any list of multiple
         // items.
         //

         //
         // Notify of disconnect... (always, won't hurt if it is
         // already disconnected).
         //

         uDest = (dwControlID == MUX_WAVEIN) ? DEST_WAVEIN : DEST_VOICEIN ;

         //
         // Figure out the "active" destination, if any.
         //

         uActiveDest = 0 ;
         if (phwi -> bIntUsed == INT_WAVEIN)
            uActiveDest =
               (phwi -> dwCurCODECOwner != phwi -> dwLowUser) ?
                  DEST_WAVEIN : DEST_VOICEIN ;

         // Determine actual source ID 'cause it's mapped to the
         // relative source ID by MxdUpdateLine().

         MxdUpdateLine( phwi,
                        uDest,
                        pmi -> auSourceMap[ uDest ][ pmi -> dwValue[ dwControlID ][ 0 ] ],
                        FALSE,
                        MXDUPDATELINE_ACTIONF_LINESTATUS |
                           MXDUPDATELINE_ACTIONF_SOURCE ) ;

         // Active mux channel is the first one in the list
         // found to be set.

         pmxcd_f = (LPMIXERCONTROLDETAILS_BOOLEAN) pmxcd -> paDetails ;

         pmi -> dwValue[ dwControlID ][ 0 ] = 0 ;

         for (uCurItem = 0;
              uCurItem < pmi -> mxc[ dwControlID ].cMultipleItems;
              uCurItem++)
         {
            if (pmxcd_f -> fValue)
            {
               pmi -> dwValue[ dwControlID ][ 0 ] = uCurItem ;

               D(
                  char  szDebug[ 80 ] ;

                  wsprintf( szDebug, "Mux setting: %d", uCurItem ) ;
                  DPF( 1,  szDebug ) ;
               ) ;

               //
               // Notify of connect... IFF uDest is the active
               // destination (e.g. wave-in and low-pri match)
               //

               uDest =
                  (dwControlID == MUX_WAVEIN) ? DEST_WAVEIN : DEST_VOICEIN ;

               if (uDest == uActiveDest)
               {
                  // Determine actual source ID 'cause it's mapped to the
                  // relative source ID by MxdUpdateLine().

                  MxdUpdateLine( phwi,
                                 uDest,
                                 pmi -> auSourceMap[ uDest ][ pmi -> dwValue[ dwControlID ][ 0 ] ],
                                 TRUE,
                                 MXDUPDATELINE_ACTIONF_LINESTATUS |
                                    MXDUPDATELINE_ACTIONF_SOURCE ) ;
               }

               break ;

            }

            //
            //  move to the next entry
            //

            ((LPBYTE)pmxcd_f) += pmxcd->cbDetails;
         }

         //
         //  call someone else to update the hardware
         //

         mmr = ControlFunctionTable[dwControlID]( phwi, pmxcd ) ;
         if (mmr)
         {
            DPF( 1, "MxdSetControlDetails: The call to set the hardware failed.") ;
            return (mmr) ;
         }
      }
      break ;

      default:
      {
         LPMIXERCONTROLDETAILS_SIGNED    pmxcd_s ;

         mmr = VerifyControlValue( pmi, pmxcd ) ;
         if (mmr)
         {
            DPF( 1,  "SetControlDetails:VerifyControlValue failed" ) ;
            return ( mmr ) ;
         }

         //
         //  just use type SIGNED since we're
         //  not gonna interpret the value here.
         //

         pmxcd_s = (LPMIXERCONTROLDETAILS_SIGNED)pmxcd->paDetails;
         for (uCurChannel = 0; uCurChannel < cChannels; uCurChannel++)
         {
            pmi->lValue[dwControlID][uCurChannel] = pmxcd_s->lValue;

            //
            //  move to the next entry
            //
            if (pmxcd -> cChannels != 1)
               ((LPBYTE)pmxcd_s) += pmxcd->cbDetails;
         }

         //
         //  call someone else to update the hardware
         //

         mmr = ControlFunctionTable[dwControlID]( phwi, pmxcd ) ;
         if (mmr)
         {
            DPF( 1, "MxdSetControlDetails: The call to set the hardware failed.") ;
            return (mmr) ;
         }
      }
      break ;

   }

   //
   //  !!! WARNING DANGER WARNING DANGER WARNING DANGER WARNING DANGER !!!
   //
   //  all notification callbacks for mixer drivers must NEVER occur at
   //  interrupt time. so, even though documenation states that the
   //  DriverCallback() function can be called at interrupt time, this
   //  is NOT true for mixer drivers. ALWAYS perform driver callbacks
   //  for control and line changes at non-interrupt time.
   //
   //  !!! WARNING DANGER WARNING DANGER WARNING DANGER WARNING DANGER !!!
   //

   //
   // Walk the list for the mixer device and notify everyone...
   //

   pCur = pmi -> pMxdClients ;
   while (pCur)
   {
      DriverCallback( pCur -> dwCallback,
                      pCur -> fuCallback,
                      pCur -> hmx,
                      MM_MIXM_CONTROL_CHANGE,
                      pCur -> dwInstance,
                      pmxcd -> dwControlID,
                      0L) ;

      pCur = pCur -> pNext ;
   }

   return MMSYSERR_NOERROR;

} // MxdSetControlDetails()

//--------------------------------------------------------------------------;
//
//  DWORD mxdMessage
//
//  Description:
//
//
//  Arguments:
//      UINT uMsg:
//
//      DWORD dwUser:
//
//      DWORD dwParam1:
//
//      DWORD dwParam2:
//
//  Return (DWORD):
//
//
//--------------------------------------------------------------------------;

DWORD _far _pascal _export _loadds mxdMessage
(
    UINT            uDevId,
    UINT            uMsg,
    DWORD           dwUser,
    DWORD           dwParam1,
    DWORD           dwParam2
)
{
   PMXDCLIENT          pmxdc ;
   DWORD               dw ;
   PHARDWAREINSTANCE   phwi ;

   // take care of init time messages...

   switch (uMsg)
   {
      case MXDM_INIT:
      {
         DPF( 1, "MXDM_INIT" ) ;

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
         DPF( 1, "MXDM_ENABLE" ) ;

         // dwParam2 == PnP DevNode

         return (EnableDevNode( dwParam2 )) ;

      }
      break ;

      case DRVM_DISABLE:
      {
         DPF( 1, "MXDM_DISABLE" ) ;

         // dwParam2 == PnP DevNode

         return (DisableDevNode( dwParam2 )) ;

      }
      break ;

      case DRVM_EXIT:
      {
         DPF( 1, "MXDM_EXIT" ) ;

         // dwParam2 == PnP DevNode

         return (RemoveDevNode( dwParam2 )) ;

      }
      break ;

      case MXDM_GETNUMDEVS:
      {
         DPF( 1, "MXDM_GETNUMDEVS" ) ;

         if (NULL == (phwi = DevNodeToHardwareInstance( dwParam1 )))
            return MAKELONG( 0, MMSYSERR_INVALPARAM ) ;

         if (dwParam1)
         {
            if (phwi -> fEnabled)
               return 1L ;
            else
               return 0L ;
         }
         else
            return 0L ;
      }
      break ;

      case MXDM_OPEN:
      {
         DWORD  dn ;

         DPF( 1, "MXDM_OPEN" ) ;

         if (uDevId > 1)
            return MMSYSERR_BADDEVICEID ;

         dn = ((LPMIXEROPENDESC) dwParam1) -> dnDevNode ;
         if (NULL == (phwi = DevNodeToHardwareInstance( dn )))
         {
            DPF( 1, "devnode not associated with hardware instance???" ) ;
            return MMSYSERR_BADDEVICEID ;
         }

         if (!phwi -> fEnabled)
            return MMSYSERR_NOTENABLED ;

         return (MxdOpen( phwi,
                          (LPDWORD) dwUser,
                          (LPMIXEROPENDESC) dwParam1,
                          dwParam2 )) ;
      }
      break ;

      case MXDM_GETDEVCAPS:
      {
         DPF( 1, "MXDM_GETDEVCAPS" ) ;

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

         mxdGetDevCaps( phwi, (MDEVICECAPSEX FAR *) dwParam1 ) ;
         return MMSYSERR_NOERROR ;
      }
      break ;

   }

   // Bad device ID???

   if (uDevId > 1)
      return MMSYSERR_BADDEVICEID ;

   pmxdc = (PMXDCLIENT) LOWORD( dwUser ) ;
   phwi = pmxdc -> phwi ;

   switch (uMsg)
   {
      case MXDM_CLOSE:
         return (MxdClose( pmxdc )) ;

      case MXDM_GETLINEINFO:
         dw = MxdGetLineInfo( pmxdc, (LPMIXERLINE)dwParam1, dwParam2);
         return (dw);

      case MXDM_GETLINECONTROLS:
         dw = MxdGetLineControls( pmxdc, (LPMIXERLINECONTROLS)dwParam1, dwParam2);
         return (dw);

      case MXDM_GETCONTROLDETAILS:
         dw = MxdGetControlDetails( phwi, (LPMIXERCONTROLDETAILS)dwParam1, dwParam2);
         return (dw);

      case MXDM_SETCONTROLDETAILS:
         dw = MxdSetControlDetails( phwi, (LPMIXERCONTROLDETAILS)dwParam1, dwParam2);
         return (dw);
   }

   return (MMSYSERR_NOTSUPPORTED);

} // mxdMessage()

//--------------------------------------------------------------------------
//
//  LRESULT mixerPipeProc
//
//  Description:
//
//
//  Parameters:
//      HPIPE hp
//
//      DWORD dwMsg
//
//      DWORD dwParam1
//
//      DWORD dwParam2
//
//  Return (LRESULT):
//
//--------------------------------------------------------------------------

LRESULT FAR PASCAL _loadds mixerPipeProc
(
    HPIPE           hp,
    DWORD           dwMsg,
    DWORD           dwParam1,
    DWORD           dwParam2
)
{
   PHARDWAREINSTANCE  phwi ;

   //
   // dwParam1 is PnP DevNode
   //

   if (NULL == (phwi = DevNodeToHardwareInstance( dwParam1 )))
      return PIPE_ERR_INVALIDPARAM ;

   switch( dwMsg )
   {
      case PIPE_MSG_OPEN:
         break ;

      case PIPE_MSG_INIT:
      {
         PMIXERINSTANCE  pmi ;
         MDEVICECAPSEX   mdex ;
         MIDIOUTCAPS     mc ;

         pmi = phwi -> pmi ;
         phwi -> fnmxdPipe = (FNPIPECALLBACK) dwParam2 ;

         mdex.cbSize = sizeof( MIDIOUTCAPS ) ;
         mdex.pCaps = &mc ;

         phwi -> fnmxdPipe( hp,
                            PIPE_MSG_CONTROL | MSOPL_CTL_GETDEVCAPS,
                            dwParam1,
                            (DWORD) (MDEVICECAPSEX FAR *) &mdex ) ;

         _fmemcpy( &pmi -> mxlSources[ SOURCE_MIDIOUT ].Target.wMid,
                   &mc,
                   sizeof( WORD ) * 2 + sizeof( VERSION ) + MAXPNAMELEN ) ;

         pmi -> mxlSources[ SOURCE_MIDIOUT ].Target.dwType =
            MIXERLINE_TARGETTYPE_MIDIOUT ;

         // connect to line out

         pmi -> adwActiveMap[ DEST_LINEOUT ][ 2 ] &=
            ~MIXERLINE_LINEF_DISCONNECTED ;
      }
      break ;

      case PIPE_MSG_NOTIFY | MSOPL_NFY_CONTROL_CHANGE:
      {
         PMXDCLIENT      pCur ;
         PMIXERINSTANCE  pmi ;

         pmi = phwi -> pmi ;

         pmi -> dwValue[ VOL_OUTMIDI ][ 0 ] =
            (DWORD) LOWORD( dwParam2 ) ;
         pmi -> dwValue[ VOL_OUTMIDI ][ 1 ] =
            (DWORD) LOWORD( dwParam2 ) ;

         //
         // Walk the list for the mixer device and notify everyone...
         //

         pCur = pmi -> pMxdClients ;
         while (pCur)
         {
            DriverCallback( pCur -> dwCallback,
                            pCur -> fuCallback,
                            pCur -> hmx,
                            MM_MIXM_CONTROL_CHANGE,
                            pCur -> dwInstance,
                            VOL_OUTMIDI,
                            0L ) ;

            pCur = pCur -> pNext ;
         }
      }
      break ;

      case PIPE_MSG_NOTIFY | MSOPL_NFY_LINE_CHANGE:
         MxdUpdateLine( phwi, DEST_LINEOUT, SOURCE_MIDIOUT, (BOOL) dwParam2,
                        MXDUPDATELINE_ACTIONF_LINESTATUS |
                           MXDUPDATELINE_ACTIONF_SOURCE ) ;
         break ;

      case PIPE_MSG_CLOSE:
         phwi -> hpmxd = NULL ;
         phwi -> fnmxdPipe = NULL ;
         break ;
   }

   return PIPE_ERR_NOERROR ;

} // mixerPipeProc()

//---------------------------------------------------------------------------
//  End of File: mixer.c
//---------------------------------------------------------------------------
