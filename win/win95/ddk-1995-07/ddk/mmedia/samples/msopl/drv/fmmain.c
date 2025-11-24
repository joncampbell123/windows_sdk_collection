//---------------------------------------------------------------------------
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1993 - 1995  Microsoft Corporation.  All Rights Reserved.
//
//---------------------------------------------------------------------------
//
//  Module: fmmain.c
//
//  Description:
//     Main MIDI interface routines
//
//
//---------------------------------------------------------------------------


#include <windows.h>
#include <mmsystem.h>
#include <mmddk.h>
#include <mmreg.h>

#include "msopl.h"
#include "resource.h"

static WORD gcPatchLock = 0 ;

//--------------------------------------------------------------------------
//  
//  void modGetDevCaps
//  
//  Description:
//  
//  
//  Parameters:
//      PHARDWAREINSTANCE phwi
//
//      MDEVICECAPSEX FAR *lpCaps
//  
//  Return (void):
//  
//--------------------------------------------------------------------------

void FAR PASCAL modGetDevCaps
(
    PHARDWAREINSTANCE   phwi,
    MDEVICECAPSEX FAR   *lpCaps
)
{
   MIDIOUTCAPS mc ;

   mc.wMid = MID_MICROSOFT ;
   mc.wPid = PID_SYNTH ;
   mc.vDriverVersion = DRV_VERSION ;
   mc.wTechnology = MOD_FMSYNTH ;

   if (phwi -> wHardwareOptions & MSOPL_HWOPTIONSF_OPL3DETECTED)
   {
      mc.wVoices = 18 ;
      mc.wNotes = 18 ;
      mc.dwSupport = MIDICAPS_VOLUME | MIDICAPS_LRVOLUME ;
   }
   else
   {
      mc.wVoices = 9 ;
      mc.wNotes = 9 ;
      mc.dwSupport = MIDICAPS_VOLUME ;
   }
   mc.wChannelMask = 0xffff ;
   LoadString(ghModule, IDS_DRIVER_SYNTHNAME, mc.szPname, MAXPNAMELEN ) ;

   _fmemcpy( lpCaps -> pCaps, &mc, 
             min( (UINT) lpCaps -> cbSize, sizeof( mc ) ) ) ;

} // modGetDevCaps()

//------------------------------------------------------------------------
//  VOID fmQuiet
//
//  Description:
//     This turns everything on the FM synth off to make it quiet.
//
//  Parameters:
//      PHARDWAREINSTANCE phwi
//         pointer to hardware instance structure
//  
//  Return Value:
//     Nothing.
//
//------------------------------------------------------------------------

VOID FAR PASCAL fmQuiet
(
    PHARDWAREINSTANCE   phwi
)
{
   WORD    i;

   // Reset the FM chip.

   fmSend( phwi, AD_MASK, 0x60 ) ;
   fmSend( phwi, AD_NTS, 0x00 ) ;

   if (phwi -> wHardwareOptions & MSOPL_HWOPTIONSF_OPL3DETECTED)
   {
      fmSend( phwi, AD_NEW, 0x01 ) ;
      fmSend( phwi, AD_CONNECTION, 0x00 ) ;
   }

   // Turn off drums, use high vibrato/modulation

   fmSend( phwi, AD_DRUM, 0xc0 ) ;

   // Turn off all oscillators

   for (i = 0; i < 0x15; i++)
   {
      fmSend( phwi, AD_LEVEL + i, 0x3f ) ;

      if (phwi -> wHardwareOptions & MSOPL_HWOPTIONSF_OPL3DETECTED)
         fmSend( phwi, AD_LEVEL2 + i, 0x3f ) ;
   }

   // Turn off all voices

   for (i = 0; i < 0x08; i++)
   {
      fmSend( phwi, AD_BLOCK + i, 0x00 ) ;

      if (phwi -> wHardwareOptions & MSOPL_HWOPTIONSF_OPL3DETECTED)
         fmSend( phwi, AD_BLOCK2 + i, 0x00 ) ;
   }

   // Turn off the pitch bend

   for (i = 0; i < NUMCHANNELS; i++)
      phwi -> auBend[ i ] = 0 ;

} // end of fmQuiet()

//--------------------------------------------------------------------------
//  
//  BOOL fmPatchPrepare
//  
//  Description:
//      Fixes the patches block in memory.
//  
//  Parameters:
//      None.
//  
//  Return (BOOL):
//  
//--------------------------------------------------------------------------

BOOL FAR PASCAL fmPatchPrepare()
{
    WORD    i ;

    if (gcPatchLock)
       return TRUE ;

    //
    //  the patches need to be accessable at interrupt time, so we must
    //  page lock the patch memory block. but before page locking, we try
    //  to move the memory low in the global heap.. note that in standard
    //  mode, globalpagelock simply fixes the memory so it won't move--
    //  and this is good!
    //

    GlobalWire((HGLOBAL)SELECTOROF(glpPatch));
    i = GlobalPageLock((HGLOBAL)SELECTOROF(glpPatch));
    if (i == 0)
    {
        DPF( 1, "fmPatchPrepare: could not pagelock patch memory!! very bad!!" ) ;

        GlobalUnWire((HGLOBAL)SELECTOROF(glpPatch));
        return FALSE ;
    }

    gcPatchLock++ ;

    return TRUE ;

} // fmPatchPrepare()

//--------------------------------------------------------------------------
//  
//  BOOL fmPatchUnprepare
//  
//  Description:
//     Un-"fixes" patch memory.
//  
//  Parameters:
//      None.
//  
//  Return (BOOL):
//  
//--------------------------------------------------------------------------

BOOL FAR PASCAL fmPatchUnprepare()
{
    WORD    i;

    if (--gcPatchLock)
       return TRUE ;

    //
    //  unpagelock and unwire the patch memory--no longer need to access
    //  it at interrupt time. this keeps kernel happy.
    //

    if (glpPatch)
    {
        i = GlobalPageUnlock((HGLOBAL)SELECTOROF(glpPatch));
        if (i == 0)
        {
            GlobalUnWire((HGLOBAL)SELECTOROF(glpPatch));
        }
    }

    return TRUE ;

} // fmPatchUnprepare()

//--------------------------------------------------------------------------
//  
//  LRESULT fmOpen
//  
//  Description:
//      This should be called when a midi file is opened.
//      It initializes some variables and locks the patch memory.
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

LRESULT FAR PASCAL fmOpen
(
    PHARDWAREINSTANCE   phwi,
    LPDWORD             pdwUser,
    LPMIDIOPENDESC      pod,
    DWORD               dwFlags
)
{
   WORD    i;
   PFMALLOC pClient ;

   DPF( 3, "fmOpen" ) ;

   if (phwi -> fInUse)
      return MMSYSERR_ALLOCATED ;

   if (!AcquireFM( phwi ))
      return MMSYSERR_ALLOCATED ;

   if (!glpPatch || !fmPatchPrepare())
   {
      DPF( 1, "glpPatch NULL or unable to prepare patches." ) ;
      ReleaseFM( phwi ) ;
      return MMSYSERR_NOMEM ;
   }

   // call for silence

   fmQuiet( phwi ) ;

   // Clear all of the voice slots to empty

   for (i = 0; i < NUM2VOICES; i++)
      phwi -> aVoiceSlots[ i ].dwTime = 0 ;

   // Start attenuations at -3 dB, which is 90 MIDI level

   for (i = 0; i < NUMCHANNELS; i++)
   {
      phwi -> abChanAttens[ i ] = 4 ;
      phwi -> abStereoMasks[ i ] = 0xff ;
   }

   pClient = (PFMALLOC) LocalAlloc( LPTR, sizeof( FMALLOC ) ) ;
   if (NULL == pClient)
   {   ReleaseFM( phwi ) ;
       return MMSYSERR_NOMEM ;
   }

   // save client information

   pClient -> dwCallback = pod -> dwCallback ;
   pClient -> dwInstance = pod -> dwInstance ;
   pClient -> hMidi      = pod -> hMidi ;
   pClient -> dwFlags    = dwFlags ;
   pClient -> phwi       = phwi ;

   *pdwUser = MAKELONG( (WORD) pClient, NULL ) ;

   // notify client

   fmCallback( pClient, MOM_OPEN, 0L, 0L ) ;

   // we're in use...

   phwi -> fInUse = TRUE ;

   if (phwi -> fnmxdPipe)
      phwi -> fnmxdPipe( phwi -> hpmxd, 
                         PIPE_MSG_NOTIFY | 
                            MSOPL_NFY_LINE_CHANGE,
                         phwi -> dn,
                         TRUE ) ;

   return MMSYSERR_NOERROR ;

} // fmOpen()

//--------------------------------------------------------------------------
//  
//  VOID fmClose
//  
//  Description:
//      Kills the playing voices and unlocks the patch memory.
//  
//  Parameters:
//      PFMALLOC pClient
//         pointer to FM client
//  
//  Return (VOID):
//      Nothing.
//  
//--------------------------------------------------------------------------

VOID FAR PASCAL fmClose
(
    PFMALLOC        pClient
)
{
   PHARDWAREINSTANCE phwi ;
   WORD              i ;

   DPF( 3, "fmClose" ) ;

   phwi = pClient -> phwi ;

   // make sure all notes are turned off

   for (i = 0; i < NUM2VOICES; i++)
      fmNoteOff( phwi, 
                 phwi -> aVoiceSlots[ i ].bPatch, 
                 phwi -> aVoiceSlots[ i ].bNote, 
                 phwi -> aVoiceSlots[ i ].bChannel ) ;

   ReleaseFM( phwi ) ;

   fmPatchUnprepare() ;

   //
   // notify client
   //

   fmCallback( pClient, MOM_CLOSE, 0L, 0L ) ;

   LocalFree( (HLOCAL) pClient ) ;

   phwi -> fInUse = FALSE ;

   if (phwi -> fnmxdPipe)
      phwi -> fnmxdPipe( phwi -> hpmxd, 
                         PIPE_MSG_NOTIFY | 
                            MSOPL_NFY_LINE_CHANGE,
                         phwi -> dn,
                         FALSE ) ;

} // fmClose()

//--------------------------------------------------------------------------
//  
//  VOID fmReset
//  
//  Description:
//      Resets FM hardware.
//  
//  Parameters:
//      PHARDWAREINSTANCE phwi
//         pointer to hardware instance structure
//
//  Return (VOID):
//      Nothing.
// 
//--------------------------------------------------------------------------

VOID FAR PASCAL fmReset
(
    PHARDWAREINSTANCE   phwi
)
{
   WORD  i ;

   DPF( 3, "fmReset" ) ;

   // Make sure all notes are turned off

   for (i = 0; i < NUM2VOICES; i++)
      fmNoteOff( phwi, 
                 phwi -> aVoiceSlots[ i ].bPatch, 
                 phwi -> aVoiceSlots[ i ].bNote, 
                 phwi -> aVoiceSlots[i].bChannel ) ;

   // Call for silence

   fmQuiet( phwi ) ;

   // Clear all of the voice slots to empty

   for (i = 0; i < NUM2VOICES; i++)
      phwi -> aVoiceSlots[ i ].dwTime = 0 ;

   // Start attenuations at -3 dB, which is 90 MIDI level

   for (i = 0; i < NUMCHANNELS; i++)
   {
      phwi -> abChanAttens[ i ] = 4 ;
      phwi -> abStereoMasks[ i ] = 0xff ;
   }

} // fmReset()

//--------------------------------------------------------------------------
//  
//  DWORD mixerPipeProc
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
//  Return (DWORD):
//  
//--------------------------------------------------------------------------

DWORD FAR PASCAL _loadds mixerPipeProc
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
         return PIPE_ERR_NOERROR ;

      case PIPE_MSG_INIT:
         phwi -> fnmxdPipe = (FNPIPECALLBACK) dwParam2 ;
         return PIPE_ERR_NOERROR ;

      case (PIPE_MSG_CONTROL | MSOPL_CTL_DISABLE_SOFTWARE_VOLUME):
         phwi -> fSoftwareVolumeEnabled = FALSE ;
         return PIPE_ERR_NOERROR ;

      case (PIPE_MSG_CONTROL | MSOPL_CTL_GETDEVCAPS):
         modGetDevCaps( phwi, (MDEVICECAPSEX FAR *) dwParam2 ) ;
         return PIPE_ERR_NOERROR ;

      case (PIPE_MSG_CONTROL | MSOPL_CTL_SET_MASTER_VOLUME):
         if (phwi -> fSoftwareVolumeEnabled)
         {
            phwi -> dwMasterVolume = dwParam2 ;
            fmNewVolume( phwi, 0xFF ) ;
         }

         return PIPE_ERR_NOERROR ;

      case (PIPE_MSG_CONTROL | MSOPL_CTL_SET_SYNTH_VOLUME):
         if (phwi -> fSoftwareVolumeEnabled)
         {
            phwi -> dwSynthVolume = dwParam2 ;
            fmNewVolume( phwi, 0xFF ) ;
         }

         return PIPE_ERR_NOERROR ;

      case (PIPE_MSG_CONTROL | MSOPL_CTL_GET_SYNTH_VOLUME):
         if (phwi -> fSoftwareVolumeEnabled)
            *((LPDWORD) dwParam2) = phwi -> dwSynthVolume ;

         return PIPE_ERR_NOERROR ;

      case PIPE_MSG_CLOSE:
         phwi -> hpmxd = NULL ;
         phwi -> fnmxdPipe = NULL ;
         break ;
   }

   return PIPE_ERR_NOERROR ;

} // mixerPipeProc()

//---------------------------------------------------------------------------
//  End of File: fmmain.c
//---------------------------------------------------------------------------
