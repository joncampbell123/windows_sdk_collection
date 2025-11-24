//---------------------------------------------------------------------------
//
//  Module:   wave.c
//
//  Purpose:
//     Wave device interface functions.
//
//---------------------------------------------------------------------------
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

//
// global variables
//

WORD  gwUniqueID = 1;

//------------------------------------------------------------------------
//  VOID FAR PASCAL waveBadBoardError( VOID )
//
//  Description:
//     This displays an error notifying the user of a bad
//     board (happens only once).
//
//  Parameters:
//     Nothing.
//
//  Return Value:
//     Nothing.
//
//------------------------------------------------------------------------

VOID FAR PASCAL waveBadBoardError()
{
   static  bTimes = 0 ;

   // Check to see if we've already displayed it.
   if (bTimes)
      return ;

   bTimes = 1 ;
   AlertBox( NULL, SR_ALERT_BAD ) ;

} // end of waveBadBoardError()

//------------------------------------------------------------------------
//  DWORD waveGetPos
//
//  Description:
//     Get the stream position in samples.
//
//  Parameters:
//     DWORD dwUser
//        client information
//
//     LPBYTE lpInfo
//        pointer to an MMTIME structure
//
//     WORD wSize
//        size of the MMTIME structure
//
//  Return Value:
//
//------------------------------------------------------------------------

DWORD NEAR PASCAL waveGetPos
(
    DWORD               dwUser,
    LPMMTIME            lpmmt,
    DWORD               dwSize,
    DWORD               fdwOptions
)
{
   NPWAVEALLOC         pClient ;
   DWORD               dwBytes ;
   DWORD               dwFormat ;
   PHARDWAREINSTANCE   phwi ;

   // Check for the current owner...

   pClient = (NPWAVEALLOC)LOWORD( dwUser ) ;
   phwi = pClient -> phwi ;

   if (dwUser != phwi -> dwCurCODECOwner)
   {
      // Not the current owner, so must be the
      // low-pri client???

      if ((dwUser != phwi -> dwLowUser) || (!phwi -> fLowPriStateSaved))
      {
         DPF( 1, "waveGetPos: Not low-pri client or state not saved!" ) ;
         return ( MMSYSERR_INVALPARAM ) ;
      }
   }

   if (dwSize < sizeof( MMTIME ))
      return MMSYSERR_INVALPARAM ;

   if (dwUser == phwi -> dwCurCODECOwner)
   {
      if (!phwi -> bIntUsed)
         return MMSYSERR_ERROR ;

      dwBytes = HwGetCurPos( phwi, NULL, fdwOptions ) ;
   }
   else
   {
      PLOWPRISAVE  plps ;

      DPF( 3, "position check from inactive low-pri client" ) ;

      plps = &phwi -> lps ;

      // Verify that we have the right user...

      if (plps -> dwUser != phwi -> dwLowUser)
      {
         DPF( 1, "waveGetPos: low-pri client dwUser mismatch!!!" ) ;
         return MMSYSERR_INVALPARAM ;
      }

      dwFormat = plps -> dwWaveFormat ;
      dwBytes =
         pClient -> dwHalfDMABuffers * (DWORD) phwi -> wDMAHalfBufSize ;
   }

   if (dwBytes > pClient -> dwByteCount)
      dwBytes = pClient -> dwByteCount ;

   // Write this to the buffer as appropriate.

   if (lpmmt-> wType == TIME_BYTES)
      lpmmt -> u.cb = dwBytes ;
   else
   {
      lpmmt -> wType = TIME_SAMPLES ;
      lpmmt -> u.sample = dwBytes / phwi -> wBytesPerSample ;
   }

   return ( MMSYSERR_NOERROR ) ;

} // end of waveGetPos()

//------------------------------------------------------------------------
//  VOID widFreeQ
//
//  Description:
//     Free all WID buffers.
//
//  Parameters:
//     LPWAVEHDR lpwidQueue
//        pointer to wave-in queue
//
//  Return Value:
//     Nothing.
//
//------------------------------------------------------------------------

static VOID NEAR PASCAL widFreeQ
(
    LPWAVEHDR       lpwidQueue
)
{
   LPWAVEHDR  lpN ;

   DPF( 3, "widFreeQ" ) ;

   while (lpwidQueue)
   {
      lpN = lpwidQueue -> lpNext ;
      widBlockFinished( lpwidQueue ) ;
      lpwidQueue = lpN ;
   }

} // end of widFreeQ()

//--------------------------------------------------------------------------
//
//  void widSendPartBuffer
//
//  Description:
//      This function is called from HwEndADCDMA().  Looks at the
//      buffer at the head of the queue and if it contains data,
//      marks it is done and sends it back to the client.
//
//  Parameters:
//      PHARDWAREINSTANCE phwi
//         pointer to hardware instance structure
//
//  Return (void):
//      Nothing.
//
//--------------------------------------------------------------------------

void FAR PASCAL widSendPartBuffer
(
    PHARDWAREINSTANCE       phwi
)
{
   LPWAVEHDR lpH;

   DPF( 3, "widSendPartBuffer" ) ;

   // NOTE: Unlike midi input, we don't have to check if the
   //       current buffer has data - it is only called if wave input is
   //       started, which means it has data (whereas midi input being
   //       started doesn't necessarily mean data has been received)

   ASCRITENTER;

   if (phwi -> lpwidQueue)
   {
      lpH = phwi -> lpwidQueue;
      phwi -> lpwidQueue = phwi -> lpwidQueue->lpNext;
      lpH->dwFlags |= WHDR_DONE;
      lpH->dwFlags &= ~WHDR_INQUEUE;
      phwi -> dwwidCurCount = 0L;
      phwi -> hpwidCurData = NULL;
   }
   else
      lpH = NULL ;

   ASCRITLEAVE ;

   if (lpH)
      widBlockFinished( lpH ) ;

} // widSendPartBuffer()

//------------------------------------------------------------------------
//  VOID widGetDevCaps
//
//  Description:
//     Get the device capabilities.
//
//  Parameters:
//     PHARDWAREINSTANCE phwi
//        pointer to hardware instance
//
//     MDEVICECAPSEX FAR *lpCaps
//        far pointer to a MDEVICECAPSEX structure to receive
//        the information
//
//  Return Value:
//     Nothing.
//
//------------------------------------------------------------------------


//
// READTHIS_READTHIS_READTHIS_READTHIS_READTHIS_READTHIS_READTHIS
//
// Note: This routine is called from MxdInit.  Check there before you
//       change anything here.
//

VOID FAR PASCAL widGetDevCaps
(
    PHARDWAREINSTANCE   phwi,
    MDEVICECAPSEX FAR   *lpCaps
)
{
   char       szTemp[ 64 ] ;
   char       szClip[ 64 ] ;
   WAVEINCAPS wc;

   wc.wMid = MID_MICROSOFT ;
   wc.wPid = PID_WAVEIN ;
   wc.vDriverVersion = DRV_VERSION ;
   wc.dwFormats = WAVE_FORMAT_1M08 | WAVE_FORMAT_1S08 |
                  WAVE_FORMAT_1M16 | WAVE_FORMAT_1S16 |
                  WAVE_FORMAT_2M08 | WAVE_FORMAT_2S08 |
                  WAVE_FORMAT_2M16 | WAVE_FORMAT_2S16 |
                  WAVE_FORMAT_4M08 | WAVE_FORMAT_4S08 |
                  WAVE_FORMAT_4M16 | WAVE_FORMAT_4S16 ;
   wc.wChannels = 2 ;
   LoadString( ghModule, SR_STR_DRIVERWAVEIN, szTemp, sizeof( szTemp ) ) ;
   wsprintf( szClip, szTemp, phwi -> wIOAddressCODEC ) ;
   lstrcpyn( wc.szPname, szClip, MAXPNAMELEN-1);
   _fmemcpy( lpCaps -> pCaps, &wc,
             min( (UINT) lpCaps -> cbSize, sizeof( wc ) ) ) ;

} // end of widGetDevCaps()

//------------------------------------------------------------------------
//  BOOL widLowPriSaveState
//
//  Description:
//     Saves the current wid state.  This is used to restore
//     the hardware when the active wid closes.
//
//  Parameters:
//     PHARDWAREINSTANCE phwi
//        pointer to hardware instance structure
//
//  Return Value:
//     Nothing.
//
//------------------------------------------------------------------------

BOOL NEAR PASCAL widLowPriSaveState
(
    PHARDWAREINSTANCE       phwi
)
{
   PLOWPRISAVE       plps ;
   PMIXERINSTANCE    pmi ;

   DPF( 3, "widLowPriSaveState()" ) ;

   // The state shouldn't have already been saved,
   // if so, return failure.

   if (phwi -> fLowPriStateSaved)
      return ( FALSE ) ;

   pmi = phwi -> pmi ;
   plps = &phwi -> lps ;

   // Save the "running" state.

   plps -> fwidStarted = phwi -> fwidStarted ;

   // Stop the ADC...

   HwEndADCDMA( phwi ) ;

   plps -> dwUser = phwi -> dwLowUser ;
   plps -> dwWaveFormat = phwi -> dwWaveFormat ;
   plps -> lpwidQueue = phwi -> lpwidQueue ;
   plps -> wDMAHalfBufSize = phwi -> wDMAHalfBufSize ;
   phwi -> fLowPriStateSaved = TRUE ;

   MxdUpdateLine( phwi, DEST_VOICEIN,
                  pmi -> auSourceMap[ DEST_VOICEIN ][ pmi -> dwValue[ MUX_VOICEIN ][ 0 ] ],
                  FALSE,
                  MXDUPDATELINE_ACTIONF_LINESTATUS |
                     MXDUPDATELINE_ACTIONF_SOURCE ) ;

   phwi -> lpwidQueue = NULL ;

   widHardwareRelease( phwi ) ;

   return ( TRUE ) ;

} // end of widLowPriSaveState()

//------------------------------------------------------------------------
//  BOOL widLowPriRestoreState
//
//  Description:
//     Restores the wid state.  This should be called when a
//     contending device (wod or wid) has closed.
//
//  Parameters:
//     PHARDWAREINSTANCE phwi
//        pointer to hardware instance structure
//
//  Return Value:
//     Nothing.
//
//------------------------------------------------------------------------

BOOL NEAR PASCAL widLowPriRestoreState
(
    PHARDWAREINSTANCE       phwi
)
{
   NPWAVEALLOC       pClient ;   // pointer to client information structure
   PLOWPRISAVE       plps ;
   PMIXERINSTANCE    pmi ;

   pmi = phwi -> pmi ;

   if (!phwi -> fLowPriStateSaved)
      return ( FALSE ) ;

   // Verify that this is the correct user!

   plps = &phwi -> lps ;
   if (plps -> dwUser != phwi -> dwLowUser)
      return ( FALSE ) ;

   // Attempt to 'acquire' the wave input hardware

   if (widHardwareAcquire( phwi ))
   {
      DPF( 3, "wave input hardware is not available!" ) ;
      return ( FALSE ) ;
   }

   // The low-pri client is now the owner of the CODEC.

   phwi -> dwCurCODECOwner = phwi -> dwLowUser ;

   // Reset the queue...

   phwi -> lpwidQueue = plps -> lpwidQueue ;

   // set the sample rate and set the clock to 0 indicating
   // that it hasn't been initialized yet

   phwi -> dwWaveFormat = plps -> dwWaveFormat ;
   pClient = (NPWAVEALLOC)LOWORD( plps -> dwUser ) ;
   HwSetFormat( phwi,
                (WORD)(pClient->pcmwf.wf.nSamplesPerSec),
                (BYTE)((phwi -> dwWaveFormat & WOFORMAT_STEREO) ? 2 : 1),
                (BYTE)(phwi -> dwWaveFormat & ~WOFORMAT_STEREO),
                DIRECTION_ADC ) ;

   // Restart ADC transfers if we we're running before...

   if (plps -> fwidStarted)
      HwBeginADCDMA( phwi ) ;

   phwi -> fLowPriStateSaved = FALSE ;

   MxdUpdateLine( phwi, DEST_VOICEIN,
                  pmi -> auSourceMap[ DEST_VOICEIN ][ pmi -> dwValue[ MUX_VOICEIN ][ 0 ] ],
                  TRUE,
                  MXDUPDATELINE_ACTIONF_LINESTATUS |
                     MXDUPDATELINE_ACTIONF_SOURCE ) ;

   return ( TRUE ) ;

} // end of widLowPriRestoreState()

//------------------------------------------------------------------------
//  DWORD widOpen
//
//  Description:
//     Handles the WIDM_OPEN message.
//
//  Parameters:
//     PHARDWAREINSTANCE phwi
//        pointer to hardware instance structure
//
//     DWORD dwUser
//        pointer to return handle
//
//     LPWAVEOPENDESC lpwid
//        contains a pointer to a WAVEOPENDESC
//
//     DWORD dwOpenFlags
//        LOWORD contains wave driver specific flags
//        HIWORD contains generic driver flags
//
//  Return Value:
//     DWORD
//        MMSYS_NOERROR if no error occurred otherwise
//          specific error code for problem
//
//------------------------------------------------------------------------

DWORD NEAR PASCAL widOpen
(
    PHARDWAREINSTANCE   phwi,
    DWORD               dwUser,
    LPWAVEOPENDESC      lpwid,
    DWORD               dwOpenFlags
)
{
   const WAVEFORMATEX FAR *lpFmt ;   // pointer to passed format
   NPWAVEALLOC            pClient;   // pointer to client information
                                     //    structure
   BYTE                   bOK ;      // TRUE if the format is OK.

   DWORD                  dwActualRate, dwError ;
   PMIXERINSTANCE         pmi ;

   // Assume everything's valid

   bOK = TRUE ;

   // Make sure we can handle the format

   lpFmt = (WAVEFORMATEX FAR *)lpwid -> lpFormat ;

   switch (lpFmt -> wFormatTag)
   {
      case WAVE_FORMAT_MULAW:
         DPF( 3, "u-Law requested..." ) ;
         if (((LPWAVEFORMATEX) lpFmt) -> wBitsPerSample != 8)
         {
            DPF( 1, "Only 8-bit companded format on this CODEC!" ) ;
            bOK = FALSE ;
         }
         break ;

      case WAVE_FORMAT_ALAW:
         DPF( 3, "A-Law requested..." ) ;
         if (((LPWAVEFORMATEX) lpFmt) -> wBitsPerSample != 8)
         {
            DPF( 1, "Only 8-bit companded format on this CODEC!" ) ;
            bOK = FALSE ;
         }
         break ;

      case WAVE_FORMAT_PCM:
         if (!( (((LPPCMWAVEFORMAT)lpFmt)->wBitsPerSample == 8) ||
            (((LPPCMWAVEFORMAT)lpFmt)->wBitsPerSample == 16) ))
                  bOK = FALSE;
         break ;

      default:

         // unsupported format

         bOK = FALSE ;

   }

   if (!((lpFmt->nChannels == 1) || (lpFmt->nChannels == 2)))
      bOK = FALSE ;

   // Only accept sampling rates which are close within a
   // 5 percent error...

   pmi = phwi -> pmi ;

   if (phwi -> fAcceptCloseRates)
      dwError = (lpFmt -> nSamplesPerSec) / 40L ;
   else
      dwError = 0 ;

   dwActualRate = HwNearestRate( lpFmt -> nSamplesPerSec ) ;

   // This fixes the bug that allowed any sampling rate to pass.. (|| not &&)

   if ((lpFmt -> nSamplesPerSec < (dwActualRate - dwError)) ||
       (lpFmt -> nSamplesPerSec > (dwActualRate + dwError)))
      bOK = FALSE ;

   if (!bOK)
      return ( WAVERR_BADFORMAT ) ;

   // did they just want format information?

   if (dwOpenFlags & WAVE_FORMAT_QUERY)
      return MMSYSERR_NOERROR ;

   // Is the board functioning properly?

   if (phwi -> fBadBoard)
   {
      waveBadBoardError() ;
      return MMSYSERR_ALLOCATED ;
   }

   // If a low-priority open is using the wave input
   // device, then close it up.

   if ((phwi -> bIntUsed == INT_WAVEIN) && phwi -> dwLowUser)
   {
      if (!widLowPriSaveState( phwi ))
      {
         DPF( 1, "wave input hardware not available... state already saved?" ) ;
         return MMSYSERR_ALLOCATED ;
      }
   }

   // attempt to 'acquire' the Wave input hardware

   if (widHardwareAcquire( phwi ))
   {
      DPF( 1, "wave input hardware is not available!" ) ;
      return MMSYSERR_ALLOCATED;
   }

   // Remember the format

   switch (lpFmt->wFormatTag)
   {
      case WAVE_FORMAT_MULAW:
         phwi -> dwWaveFormat = FORMAT_MULAW ;
         break ;

      case WAVE_FORMAT_ALAW:
         phwi -> dwWaveFormat = FORMAT_ALAW ;
         break ;

      case WAVE_FORMAT_PCM:
         if (((LPPCMWAVEFORMAT)lpFmt)->wBitsPerSample == 8)
            phwi -> dwWaveFormat = FORMAT_8BIT ;
         else
            phwi -> dwWaveFormat = FORMAT_16BIT;
         break ;
   }
   if (lpFmt->nChannels == 2)
      phwi -> dwWaveFormat |= WOFORMAT_STEREO;

   // allocate my per-client structure

   pClient = (NPWAVEALLOC) LocalAlloc( LPTR, sizeof( WAVEALLOC ) ) ;
   if (pClient == NULL)
   {
      widHardwareRelease( phwi ) ;
      return MMSYSERR_NOMEM ;
   }

   phwi -> dwLastPosition = 0L ;
   phwi -> lpwidQueue = NULL ;

   // and fill it with info

   pClient -> dwCallback       = lpwid -> dwCallback ;
   pClient -> dwInstance       = lpwid -> dwInstance ;
   pClient -> hWave            = lpwid -> hWave ;
   pClient -> dwFlags          = dwOpenFlags ;
   pClient -> dwByteCount      = 0L ;
   pClient -> dwHalfDMABuffers = 0L ;
   pClient -> pcmwf            = *((LPPCMWAVEFORMAT)lpFmt) ;
   pClient -> phwi             = phwi ;

   // Set the sample rate and set the clock to 0 indicating
   // that it hasn't been initialized yet.

   HwSetFormat( phwi, (WORD)(pClient->pcmwf.wf.nSamplesPerSec) ,
                (BYTE)((phwi -> dwWaveFormat & WOFORMAT_STEREO) ? 2 : 1),
                (BYTE)(phwi -> dwWaveFormat & ~WOFORMAT_STEREO),
                DIRECTION_ADC ) ;

   // give the client the driver id

   *((LPDWORD)dwUser) = phwi -> dwCurCODECOwner =
      MAKELONG( pClient, gwUniqueID++ ) ;

   // We now own the CODEC...

   // Notify mixer that line has activated.  Note that MxdUpdateLine()
   // takes a "real source id", so grab from the source map.

   MxdUpdateLine( phwi,
                  DEST_WAVEIN,
                  pmi -> auSourceMap[ DEST_WAVEIN ][ pmi -> dwValue[ MUX_WAVEIN ][ 0 ] ],
                  TRUE,
                  MXDUPDATELINE_ACTIONF_LINESTATUS |
                     MXDUPDATELINE_ACTIONF_SOURCE ) ;

   // call client's callback

   waveCallBack( pClient, WIM_OPEN, 0L ) ;

   return ( MMSYSERR_NOERROR ) ;

} // end of widOpen()

//------------------------------------------------------------------------
//  DWORD NEAR PASCAL widStart( DWORD dwUser )
//
//  Description:
//     Starts the wave input device.
//
//  Parameters:
//     DWORD dwUser
//        client information
//
//  Return Value:
//     DWORD
//        MMSYSERR_INVALPARAM
//           If invalid low-pri user (mismatch).
//
//        MMSYSERR_NOERROR
//           Everything's A-OK.
//
//------------------------------------------------------------------------

DWORD NEAR PASCAL widStart
(
    DWORD           dwUser
)
{
   PHARDWAREINSTANCE   phwi ;
   NPWAVEALLOC         pClient ;

   DPF( 3, "WIDM_START" ) ;

   pClient = (NPWAVEALLOC)LOWORD( dwUser ) ;
   phwi = pClient -> phwi ;

   // Check for the current owner...

   if (dwUser != phwi -> dwCurCODECOwner)
   {
      // Not the current owner, so must be the
      // low-pri client???

      if ((dwUser != phwi -> dwLowUser) || (!phwi -> fLowPriStateSaved))
      {
         DPF( 1, "widStart: Not low-pri client or state not saved!" ) ;
         return ( MMSYSERR_INVALPARAM ) ;
      }
   }

   // If current user, then start the hardware,
   // otherwise, just change the state.

   if (dwUser == phwi -> dwCurCODECOwner)
       HwBeginADCDMA( phwi ) ;
   else
   {
      PLOWPRISAVE  plps ;

      plps = &phwi -> lps ;

      // Verify that we have the right client...

      if (plps -> dwUser != phwi -> dwLowUser)
      {
         DPF( 1, "widStart: low-pri client dwUser mismatch!!!" ) ;
         return ( MMSYSERR_INVALPARAM ) ;
      }

      // Change the saved state

      plps -> fwidStarted = TRUE ;
   }

   return ( MMSYSERR_NOERROR ) ;

} // end of widStart()

//------------------------------------------------------------------------
//  DWORD NEAR PASCAL widStop( DWORD dwUser )
//
//  Description:
//     Stops the wave input device.
//
//  Parameters:
//     DWORD dwUser
//        client information
//
//  Return Value:
//     DWORD
//        MMSYSERR_INVALPARAM
//           If invalid low-pri user (mismatch).
//
//        MMSYSERR_NOERROR
//           Everything's A-OK.
//
//------------------------------------------------------------------------

DWORD NEAR PASCAL widStop
(
    DWORD           dwUser
)
{
   PHARDWAREINSTANCE   phwi ;
   NPWAVEALLOC         pClient ;

   DPF( 3, "WIDM_STOP" ) ;

   pClient = (NPWAVEALLOC)LOWORD( dwUser ) ;
   phwi = pClient -> phwi ;

   // Check for the current owner...

   if (dwUser != phwi -> dwCurCODECOwner)
   {
      // Not the current owner, so must be the
      // low-pri client???

      if ((dwUser != phwi -> dwLowUser) || (!phwi -> fLowPriStateSaved))
      {
         DPF( 1, "widStop: Not low-pri client or state not saved!" ) ;
         return ( MMSYSERR_INVALPARAM ) ;
      }
   }

   // If current user, then stop the hardware,
   // otherwise, just change the state.

   if (dwUser == phwi -> dwCurCODECOwner)
      HwEndADCDMA( phwi ) ;
   else
   {
      PLOWPRISAVE  plps ;

      plps = &phwi -> lps ;

      // Verify that we have the right client...

      if (plps -> dwUser != phwi -> dwLowUser)
      {
         DPF( 1, "widStop: low-pri client dwUser mismatch!!!" ) ;
         return ( MMSYSERR_INVALPARAM ) ;
      }

      // Change the saved state

      plps -> fwidStarted = FALSE ;
   }

   return ( MMSYSERR_NOERROR ) ;

} // end of widStop()

//------------------------------------------------------------------------
//  DWORD widAddBuffer
//
//  Description:
//     Add a buffer to the input queue.
//
//  Parameters:
//     LPWAVEHDR lpWIHdr
//        pointer to header to add
//
//     DWORD dwUser
//        client specific id
//
//  Return Value:
//
//------------------------------------------------------------------------

DWORD NEAR PASCAL widAddBuffer
(
    DWORD           dwUser,
    LPWAVEHDR       lpWIHdr
)
{
   PHARDWAREINSTANCE   phwi ;
   NPWAVEALLOC         pClient ;

   DPF( 3, "WIDM_ADDBUFFER" ) ;

   pClient = (NPWAVEALLOC)LOWORD( dwUser ) ;
   phwi = pClient -> phwi ;

   // Check for the current owner...

   if (dwUser != phwi -> dwCurCODECOwner)
   {
      // Not the current owner, so must be the
      // low-pri client???

      if ((dwUser != phwi -> dwLowUser) || (!phwi -> fLowPriStateSaved))
      {
         DPF( 1, "widAddBuffer: Not low-pri client or state not saved!" ) ;
         return ( MMSYSERR_INVALPARAM ) ;
      }
   }

   // check if it's been prepared

   if (!(lpWIHdr -> dwFlags & WHDR_PREPARED))
      return WAVERR_UNPREPARED ;

   // store the pointer to my WAVEALLOC structure in the wavehdr

   lpWIHdr -> reserved = (DWORD)(LPSTR) pClient;

   // add the buffer to our queue

   lpWIHdr -> dwFlags |= WHDR_INQUEUE ;
   lpWIHdr -> dwFlags &= ~WHDR_DONE ;
   lpWIHdr -> dwBytesRecorded = 0 ;

   // Special case for low-pri clients.

   if (dwUser == phwi -> dwCurCODECOwner)
   {
      ASCRITENTER ;
      phwi -> lpwidQueue =
         HwAddADCBlock( phwi, phwi -> lpwidQueue, lpWIHdr ) ;
      ASCRITLEAVE ;
   }
   else
   {
      PLOWPRISAVE  plps ;

      DPF( 3, "Adding buffer to inactive low-pri client." ) ;

      plps = &phwi -> lps ;
      plps -> lpwidQueue =
         HwAddADCBlock( phwi, plps -> lpwidQueue, lpWIHdr ) ;
   }

   return ( MMSYSERR_NOERROR ) ;

} // end of widAddBuffer()

//------------------------------------------------------------------------
//  DWORD NEAR PASCAL widReset( DWORD dwUser )
//
//  Description:
//     This resets a wave-in file.
//
//  Parameters:
//     DWORD dwUser
//        dwUser information
//
//  Return Value:
//     DWORD
//        error value
//
//------------------------------------------------------------------------

DWORD NEAR PASCAL widReset
(
    DWORD           dwUser
)
{
   PHARDWAREINSTANCE   phwi ;
   NPWAVEALLOC         pClient ;

   DPF( 3, "WIDM_RESET" ) ;

   pClient = (NPWAVEALLOC)LOWORD( dwUser ) ;
   phwi = pClient -> phwi ;

   // Check for the current owner...

   if (dwUser != phwi -> dwCurCODECOwner)
   {
      // Not the current owner, so must be the low-pri client???

      if ((dwUser != phwi -> dwLowUser) || (!phwi -> fLowPriStateSaved))
      {
         DPF( 1, "widReset: Not low-pri client or state not saved!" ) ;
         return ( MMSYSERR_INVALPARAM ) ;
      }
   }

   if (dwUser == phwi -> dwCurCODECOwner)
   {
      LPWAVEHDR lpTemp ;

      // stop if it is started and release all buffers

      HwEndADCDMA( phwi ) ;

      ASCRITENTER ;

      lpTemp = phwi -> lpwidQueue ;
      phwi -> lpwidQueue = NULL ;

      ASCRITLEAVE ;

      widFreeQ( lpTemp ) ;
   }
   else
   {
      PLOWPRISAVE  plps ;

      DPF( 3, "resetting inactive low-pri client" ) ;

      plps = &phwi -> lps ;

      // Verify that we have the right user...

      if (plps -> dwUser != phwi -> dwLowUser)
      {
         DPF( 1, "widReset: low-pri client dwUser mismatch!!!" ) ;
         return ( MMSYSERR_INVALPARAM ) ;
      }
      widFreeQ( plps -> lpwidQueue ) ;
      plps -> lpwidQueue = NULL ;
   }

   // reset byte count

   pClient -> dwByteCount = 0L ;
   pClient -> dwHalfDMABuffers = 0L ;
   phwi -> dwLastPosition = 0L ;

   return ( MMSYSERR_NOERROR ) ;

} // end of widReset()

//------------------------------------------------------------------------
//  DWORD widLowPriCloseInactive
//
//  Description:
//     Closes an inactive low-priority client.
//
//  Parameters:
//     PHARDWAREINSTANCE phwi
//        pointer to hardware instance structure
//
//  Return Value:
//     DWORD
//        MMSYSERR_INVALPARAM
//           If invalid low-pri user (mismatch).
//
//        WAVERR_STILLPLAYING
//           Buffers still in queue.
//
//        MMSYSERR_NOERROR
//           Everything's A-OK.
//
//------------------------------------------------------------------------

DWORD NEAR PASCAL widLowPriCloseInactive
(
    PHARDWAREINSTANCE       phwi
)
{
   NPWAVEALLOC   pClient ;
   PLOWPRISAVE  plps ;

   DPF( 3, "widLowPriCloseInactive()" ) ;

   plps = &phwi -> lps ;

   // Verify that we have the right user...

   if (plps -> dwUser != phwi -> dwLowUser)
   {
      DPF( 1, "widLowPriCloseInactive: low-pri client dwUser mismatch!!!" ) ;
      return ( MMSYSERR_INVALPARAM ) ;
   }

   // This should have been cleaned up when a widReset() came through...

   if (plps -> lpwidQueue)
      return ( WAVERR_STILLPLAYING ) ;

   // No low-pri client anymore...

   phwi -> dwLowUser = NULL ;
   phwi -> fLowPriStateSaved = FALSE ;

   // Notify client that it is closing.

   pClient = (NPWAVEALLOC) LOWORD( plps -> dwUser ) ;
   waveCallBack( pClient, WIM_CLOSE, 0L ) ;

   // free the allocated memory

   LocalFree( (LOCALHANDLE) pClient ) ;

   return ( MMSYSERR_NOERROR ) ;

} // end of widLowPriCloseInactive()

//--------------------------------------------------------------------------
//  
//  BOOL widSuspend
//  
//  Description:
//      Suspends the waveout device.
//  
//  Parameters:
//      PHARDWAREINSTANCE phwi
//  
//  Return (BOOL):
//  
//--------------------------------------------------------------------------

BOOL FAR PASCAL widSuspend
(
    PHARDWAREINSTANCE   phwi
)
{
   //
   // release the hardware if we own it
   //

   if (phwi -> bwidFlags & WIF_ALLOCATED)
   {
      if (phwi -> fwidStarted)
      {
         HwEndADCDMA( phwi ) ;
         phwi -> bwidFlags |= WIF_RESTART ;
      }
      widHardwareRelease( phwi ) ;
      phwi -> bwidFlags |= WIF_SUSPENDED ;
   }

   return TRUE ;

} // widSuspend()

//--------------------------------------------------------------------------
//  
//  BOOL widReactivate
//  
//  Description:
//     This function resumes a suspended waveout device.
//  
//  Parameters:
//      PHARDWAREINSTANCE phwi
//  
//  Return (BOOL):
//  
//--------------------------------------------------------------------------

BOOL FAR PASCAL widReactivate
(
    PHARDWAREINSTANCE   phwi
)
{
   if (phwi ->  bwidFlags & WIF_SUSPENDED)
   {
      if (!widHardwareAcquire( phwi ))
      {
          if (phwi -> bwidFlags & WIF_RESTART)
          {
             HwSetFormat( phwi, 
                          phwi -> wSamplesPerSec,
                          (BYTE)((phwi -> dwWaveFormat & WOFORMAT_STEREO) ? 2 : 1),
                          (BYTE)(phwi -> dwWaveFormat & ~WOFORMAT_STEREO),
                          DIRECTION_ADC ) ;
             HwBeginADCDMA( phwi ) ;
          }
          phwi -> bwidFlags &= ~(WIF_RESTART | WIF_SUSPENDED) ;
      }
      else
         return FALSE ;
   }

   return TRUE ;

} // widReactivate()

//------------------------------------------------------------------------
//  DWORD NEAR PASCAL widClose( DWORD dwUser )
//
//  Description:
//     Closes a wave-in device.
//
//  Parameters:
//     DWORD dwUser
//        user info, handle to the client
//
//  Return Value:
//     DWORD
//        error value
//
//------------------------------------------------------------------------

DWORD NEAR PASCAL widClose
(
    DWORD           dwUser
)
{
   PHARDWAREINSTANCE   phwi ;
   NPWAVEALLOC         pClient ;
   PMIXERINSTANCE      pmi ;

   DPF( 3, "WIDM_CLOSE" ) ;

   pClient = (NPWAVEALLOC)LOWORD( dwUser ) ;
   phwi = pClient -> phwi ;
   pmi = phwi -> pmi ;

   if (dwUser == phwi -> dwCurCODECOwner)
   {
      if (phwi -> lpwidQueue)
         return ( WAVERR_STILLPLAYING ) ;

      // just in case they started input without adding buffers...

      HwEndADCDMA( phwi ) ;

      widHardwareRelease( phwi ) ;

      // No CODEC owner, currently.

      phwi -> dwCurCODECOwner = 0 ;

      // Low-priority client closing...

      if (phwi -> dwLowUser == dwUser)
      {
#ifdef DEBUG
         // debug check...

         if (phwi -> fLowPriStateSaved)
            DPF( 1, "fStateSaved is TRUE!!!! when low-pri client closing." ) ;
#endif
         phwi -> fLowPriStateSaved = FALSE ;
         phwi -> dwLowUser = NULL ;

         MxdUpdateLine( phwi, DEST_VOICEIN,
                        pmi -> auSourceMap[ DEST_VOICEIN ][ pmi -> dwValue[ MUX_VOICEIN ][ 0 ] ],
                        FALSE,
                        MXDUPDATELINE_ACTIONF_LINESTATUS |
                           MXDUPDATELINE_ACTIONF_SOURCE ) ;
      }
      else
         MxdUpdateLine( phwi, DEST_WAVEIN,
                        pmi -> auSourceMap[ DEST_WAVEIN ][ pmi -> dwValue[ MUX_WAVEIN ][ 0 ] ],
                        FALSE,
                        MXDUPDATELINE_ACTIONF_LINESTATUS |
                           MXDUPDATELINE_ACTIONF_SOURCE ) ;

      // Notify client that it is closing.

      pClient = (NPWAVEALLOC) LOWORD( dwUser ) ;
      waveCallBack( pClient, WIM_CLOSE, 0L ) ;

      // free the allocated memory

      LocalFree( (LOCALHANDLE) pClient ) ;

      // If a low-pri client is remaining,
      // give it a kick start.

      if (phwi -> fLowPriStateSaved)
         widLowPriRestoreState( phwi ) ;

      return ( MMSYSERR_NOERROR ) ;
   }
   else if (dwUser == phwi -> dwLowUser)
   {
      // Client is not the current user, but the low-pri
      // client.  We better have a state saved otherwise
      // we shouldn't be here!!

      if (!phwi -> fLowPriStateSaved)
      {
         DPF( 1, "widClose(): dwUser == low-pri client, STATE NOT SAVED!!!!!" ) ;
         return ( MMSYSERR_INVALPARAM ) ;
      }

      // Ok... we're releasing an inactive low-pri client,
      // perform the necessary clean up and get out.

      return ( widLowPriCloseInactive( phwi ) ) ;
   }

   // If we're here, we've got an invalid handle.

   return ( MMSYSERR_INVALPARAM ) ;

} // end of widClose()

//------------------------------------------------------------------------
//  DWORD FAR PASCAL _loadds widMessage( WORD id, WORD msg,
//                                       DWORD dwUser, DWORD dwParam1,
//                                       DWORD dwParam2 )
//
//  Description:
//     This function conforms to the standard wave input driver message
//     procedure (widMessage), which is documented in mmddk.h.
//
//  Parameters:
//     WORD uDevId
//     WORD msg
//     DWORD dwUser
//     DWORD dwParam1
//     DWORD dwParam2
//
//  Return Value:
//     DWORD
//        message specific
//
//------------------------------------------------------------------------

DWORD FAR PASCAL _loadds widMessage
(
    UINT            uDevId,
    WORD            msg,
    DWORD           dwUser,
    DWORD           dwParam1,
    DWORD           dwParam2
)
{
   NPWAVEALLOC         pClient ;
   PHARDWAREINSTANCE   phwi ;

   //
   // Take care of init time messages...
   //

   switch (msg)
   {
      case WIDM_INIT:
      {
         DPF( 3, "WIDM_INIT" ) ;

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
         DPF( 3, "WIDM_ENABLE" ) ;

         // dwParam2 == PnP DevNode

         return (EnableDevNode( dwParam2 )) ;

      }
      break ;

      case DRVM_DISABLE:
      {
         DPF( 3, "WIDM_DISABLE" ) ;

         // dwParam2 == PnP DevNode

         return (DisableDevNode( dwParam2 )) ;

      }
      break ;

      case DRVM_EXIT:
      {
         DPF( 3, "WIDM_EXIT" ) ;

         // dwParam2 == PnP DevNode

         return (RemoveDevNode( dwParam2 )) ;

      }
      break ;

      case WIDM_GETNUMDEVS:
      {
         DPF( 3, "WIDM_GETNUMDEVS" ) ;

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

      case WIDM_OPEN:
      {
         DWORD  dn ;

         DPF( 3, "WIDM_OPEN" ) ;

         if (uDevId > 1)
            return MMSYSERR_BADDEVICEID ;

         dn = ((LPWAVEOPENDESC) dwParam1) -> dnDevNode ;

         // map devnode to hardware instance structure...

         if (NULL == (phwi = DevNodeToHardwareInstance( dn )))
         {
            DPF( 1, "devnode not associated with hardware instance???" ) ;
            return MMSYSERR_BADDEVICEID ;
         }

         if ((!phwi -> fEnabled) || (phwi -> bwidFlags & WIF_SUSPENDED))
            return MMSYSERR_NOTENABLED ;

         return (widOpen( phwi,
                          dwUser,
                          (LPWAVEOPENDESC) dwParam1,
                          dwParam2 ) ) ;
      }
      break ;

      case WIDM_GETDEVCAPS:
      {
         DPF( 3, "WIDM_GETDEVCAPS" ) ;

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

         widGetDevCaps( phwi, (MDEVICECAPSEX FAR *) dwParam1 ) ;
         return MMSYSERR_NOERROR ;
      }
      break ;

   }

   // Bad device ID???

   if (uDevId > 1)
      return MMSYSERR_BADDEVICEID ;

   pClient = (NPWAVEALLOC)LOWORD( dwUser ) ;
   phwi = pClient -> phwi ;
   if ((!phwi -> fEnabled) || (phwi -> bwidFlags & WIF_SUSPENDED))
      return MMSYSERR_NOTENABLED ;

   switch (msg)
   {
      case WIDM_CLOSE:
         return( widClose( dwUser ) ) ;

      case WIDM_ADDBUFFER:
         return( widAddBuffer( dwUser, (LPWAVEHDR) dwParam1 ) ) ;

      case WIDM_START:
         return( widStart( dwUser ) ) ;

      case WIDM_STOP:
         return( widStop( dwUser ) ) ;

      case WIDM_RESET:
         return( widReset( dwUser ) ) ;

      case WIDM_GETPOS:
         return waveGetPos( dwUser, (LPMMTIME)dwParam1,
                            dwParam2, GCP_OPTIONF_ADC );

      case WIDM_LOWPRIORITY:
      {
         PMIXERINSTANCE pmi ;

         pmi = phwi -> pmi ;

         // Make sure we're the owner, otherwise return allocated...

         if (dwUser != phwi -> dwCurCODECOwner)
            return ( MMSYSERR_ALLOCATED ) ;

         //
         // Remember low pri ID.  If already a low-pri
         // user, then bail.
         //

         if (phwi -> dwLowUser)
            return ( MMSYSERR_ALLOCATED ) ;
         else
         {
            DPF( 3, "Low priority selected" ) ;

            MxdUpdateLine( phwi, DEST_WAVEIN,
                           pmi -> auSourceMap[ DEST_WAVEIN ][ pmi -> dwValue[ MUX_WAVEIN ][ 0 ] ],
                           FALSE,
                           MXDUPDATELINE_ACTIONF_LINESTATUS |
                              MXDUPDATELINE_ACTIONF_SOURCE ) ;
            phwi -> dwLowUser = dwUser ;

            MxdUpdateLine( phwi, DEST_VOICEIN,
                           pmi -> auSourceMap[ DEST_VOICEIN ][ pmi -> dwValue[ MUX_VOICEIN ][ 0 ] ],
                           TRUE,
                           MXDUPDATELINE_ACTIONF_LINESTATUS |
                              MXDUPDATELINE_ACTIONF_SOURCE ) ;

            return ( MMSYSERR_NOERROR ) ;
         }
      }
   }

   //
   // If we got here, we don't know what they're talking about...
   //
   return ( MMSYSERR_NOTSUPPORTED ) ;

} // end of widMessage()

//--------------------------------------------------------------------------
//
//  static wodFreeQ
//
//  Description:
//      Frees all buffers.
//
//  Parameters:
//      PHARDWAREINSTANCE phwi
//         pointer to hardware instance structure
//
//  Return (VOID):
//      Nothing.
//
//--------------------------------------------------------------------------

static void NEAR PASCAL wodFreeQ
(
    PHARDWAREINSTANCE       phwi
)
{
   LPWAVEHDR lpH, lpN;

   // first free the dead heads

   wodPostAllHeaders( phwi ) ;

   if (phwi -> lpwodLoopStart)
      lpH = phwi -> lpwodLoopStart ;
   else
      lpH = phwi -> lpwodQueue ;  // point to top of the queue

   phwi -> lpwodQueue = NULL ;               // mark the queue as empty
   phwi -> lpwodLoopStart = NULL ;
   phwi -> hpwodCurData = NULL ;
   phwi -> dwwodCurCount = 0L ;
   phwi -> dwwodLoopCount = 0L ;
   phwi -> fwodDMABusy = 0 ;

   while (lpH)
   {
      lpN = lpH -> lpNext ;
      wodBlockFinished( lpH ) ;
      lpH = lpN ;
   }

} // wodFreeQ()

//------------------------------------------------------------------------
//  VOID wodGetDevCaps
//
//  Description:
//     Get the device capabilities.
//
//  Parameters:
//     PHARDWAREINSTANCE phwi
//
//     LPBYTE lpCaps
//        pointer to an MDEVICECAPSES structure
//
//  Return Value:
//     Nothing.
//
//------------------------------------------------------------------------

//
// READTHIS_READTHIS_READTHIS_READTHIS_READTHIS_READTHIS_READTHIS
//
// Note: This routine is called from MixerInit.  Check there before you
//       change anything here.
//

VOID FAR PASCAL wodGetDevCaps
(
    PHARDWAREINSTANCE   phwi,
    MDEVICECAPSEX FAR   *lpCaps
)
{
   char        szTemp[ 64 ] ;
   char        szClip[ 64 ] ;
   WAVEOUTCAPS wc;

   wc.wMid = MID_MICROSOFT ;
   wc.wPid = PID_WAVEOUT ;
   wc.vDriverVersion = DRV_VERSION ;
   wc.dwFormats = WAVE_FORMAT_1M08 | WAVE_FORMAT_1S08 |
                  WAVE_FORMAT_1M16 | WAVE_FORMAT_1S16 |
                  WAVE_FORMAT_2M08 | WAVE_FORMAT_2S08 |
                  WAVE_FORMAT_2M16 | WAVE_FORMAT_2S16 |
                  WAVE_FORMAT_4M08 | WAVE_FORMAT_4S08 |
                  WAVE_FORMAT_4M16 | WAVE_FORMAT_4S16 ;
   wc.wChannels = 2 ;
   wc.dwSupport = WAVECAPS_VOLUME | 
                  WAVECAPS_LRVOLUME |
                  WAVECAPS_SAMPLEACCURATE ;

   LoadString( ghModule, SR_STR_DRIVERWAVEOUT, szTemp, sizeof( szTemp ) ) ;
   wsprintf( szClip, szTemp, phwi -> wIOAddressCODEC ) ;
   lstrcpyn( wc.szPname, szClip, MAXPNAMELEN-1);
   _fmemcpy( lpCaps -> pCaps, &wc,
             min( (UINT) lpCaps -> cbSize, sizeof( wc ) ) ) ;

} // end of wodGetDevCaps()

//------------------------------------------------------------------------
//  LRESULT wodOpen
//
//  Description:
//
//
//  Parameters:
//      PHARDWAREINSTANCE phwi
//         pointer to hardware instance structure
//
//     DWORD dwUser
//
//     LPWAVEOPENDESC lpwod
//
//     DWORD dwOpenFlags
//
//  Return Value:
//
//------------------------------------------------------------------------

LRESULT NEAR PASCAL wodOpen
(
    PHARDWAREINSTANCE   phwi,
    DWORD               dwUser,
    LPWAVEOPENDESC      lpwod,
    DWORD               dwOpenFlags
)
{
   const WAVEFORMATEX FAR  *lpFmt ;           // pointer to passed format
   NPWAVEALLOC             pClient ;      // pointer to client
                                             // information structure
   LPPCMWAVEFORMAT         lpPCMFmt ;
   BYTE                    bOK ;
   DWORD                   dwActualRate, dwError ;

   //
   //  dwOpenFlags contains wave driver specific flags in the LOWORD
   //  and generic driver flags in the HIWORD
   //

   lpFmt = (WAVEFORMATEX FAR *) lpwod -> lpFormat;
   lpPCMFmt = (LPPCMWAVEFORMAT) lpFmt ;

   bOK = TRUE ;

   switch (lpFmt -> wFormatTag)
   {
      case WAVE_FORMAT_MULAW:
         DPF( 3, "u-Law requested..." ) ;
         if (((LPWAVEFORMATEX) lpFmt) -> wBitsPerSample != 8)
         {
            DPF( 1, "Only 8-bit companded format on this CODEC!" ) ;
            bOK = FALSE ;
         }
         break ;

      case WAVE_FORMAT_ALAW:
         DPF( 3, "A-Law requested..." ) ;
         if (((LPWAVEFORMATEX) lpFmt) -> wBitsPerSample != 8)
         {
            DPF( 1, "Only 8-bit companded format on this CODEC!" ) ;
            bOK = FALSE ;
         }
         break ;

      case WAVE_FORMAT_PCM:
         if (!( (((LPPCMWAVEFORMAT)lpFmt)->wBitsPerSample == 8) ||
            (((LPPCMWAVEFORMAT)lpFmt)->wBitsPerSample == 16) ))
         {
            DPF( 1, "Only 8-bit or 16-bit PCM supported." ) ;
            bOK = FALSE;
         }
         break ;

      default:
         // unsupported format

         DPF( 1, "Bad format" ) ;
         bOK = FALSE ;
   }

   if (!((lpFmt->nChannels == 1) || (lpFmt->nChannels == 2)))
   {
      DPF( 1, "Bad channels" ) ;
      bOK = FALSE;
   }

D(
   char szDebug[ 81 ] ;

   wsprintf( szDebug, "rate requested: %lu\r\n", lpFmt -> nSamplesPerSec ) ;
   DPF( 3, szDebug ) ;
) ;

   // Only accept sampling rates which are close within a
   // 5 percent error...

   if (phwi -> fAcceptCloseRates)
      dwError = (lpFmt -> nSamplesPerSec) / 40L ;
   else
      dwError = 0 ;

   dwActualRate = HwNearestRate( lpFmt -> nSamplesPerSec ) ;

   // This fixes the bug that allowed any sampling rate to pass.. (|| not &&)

   if ((lpFmt -> nSamplesPerSec < (dwActualRate - dwError)) ||
       (lpFmt -> nSamplesPerSec > (dwActualRate + dwError)))
      bOK = FALSE ;

   if (!bOK)
      return WAVERR_BADFORMAT;

   // did they just want format information?

   if (dwOpenFlags & WAVE_FORMAT_QUERY)
      return 0L ;

   // is the board functioning properly?

   if (phwi -> fBadBoard)
   {
      waveBadBoardError() ;
      return MMSYSERR_ALLOCATED ;
   }

   // If a low-priority open is using the wave input
   // device, then close it up.

   if ((phwi -> bIntUsed == INT_WAVEIN) && phwi -> dwLowUser)
   {
      if (!widLowPriSaveState( phwi ))
      {
         DPF( 1, "wave input hardware not available... state already saved?" ) ;
         return MMSYSERR_ALLOCATED ;
      }
   }

   // attempt to 'acquire' the wave output hardware...

   if (wodHardwareAcquire( phwi ))
   {
      DPF( 1, "wave output hardware is not available!" ) ;
      return MMSYSERR_ALLOCATED ;
   }

   // remember the format

   switch (lpFmt->wFormatTag)
   {
      case WAVE_FORMAT_MULAW:
         phwi -> dwWaveFormat = FORMAT_MULAW ;
         break ;

      case WAVE_FORMAT_ALAW:
         phwi -> dwWaveFormat = FORMAT_ALAW ;
         break ;

      case WAVE_FORMAT_PCM:
         if (((LPPCMWAVEFORMAT)lpFmt)->wBitsPerSample == 8)
            phwi -> dwWaveFormat = FORMAT_8BIT ;
         else
            phwi -> dwWaveFormat = FORMAT_16BIT ;
         break;

   }

   if (lpFmt -> nChannels == 2)
      phwi -> dwWaveFormat |= WOFORMAT_STEREO ;

   // allocate my per-client structure

   pClient = (NPWAVEALLOC) LocalAlloc( LPTR, sizeof( WAVEALLOC ) ) ;
   if (pClient == NULL)
   {
      wodHardwareRelease( phwi ) ;

      // Kick start the low-pri if we turned it off...

      if (phwi -> fLowPriStateSaved)
         widLowPriRestoreState( phwi ) ;

      return MMSYSERR_NOMEM;
   }

    // and fill it with info

    pClient -> dwCallback       = lpwod -> dwCallback ;
    pClient -> dwInstance       = lpwod -> dwInstance ;
    pClient -> hWave            = lpwod -> hWave ;
    pClient -> dwFlags          = dwOpenFlags ;
    pClient -> dwByteCount      = 0L ;
    pClient -> dwHalfDMABuffers = 0L ;
    pClient -> pcmwf            = *((LPPCMWAVEFORMAT)lpFmt) ;
    pClient -> phwi             = phwi ;

    // clear out some of the status variables

    phwi -> dwLastPosition = 0L ;
    phwi -> lpwodQueue = NULL ;
    phwi -> lpwodLoopStart = NULL ;
    phwi -> lpwodDeadHeads = NULL ;
    phwi -> hpwodCurData = NULL ;
    phwi -> dwwodCurCount = 0L ;
    phwi -> dwwodLoopCount = 0L ;
    phwi -> fwodDMABusy = 0 ;

    // give the client my driver dw

    *((LPDWORD)dwUser)= phwi -> dwCurCODECOwner =
      MAKELONG(pClient, gwUniqueID++);

    // Set the sample rate & format

    HwSetFormat( phwi,
                 lpFmt->nSamplesPerSec,
                 (BYTE)((phwi -> dwWaveFormat & WOFORMAT_STEREO) ? 2 : 1),
                 (BYTE)(phwi -> dwWaveFormat & ~WOFORMAT_STEREO),
                 DIRECTION_DAC ) ;

    // update mixer line information

    MxdUpdateLine( phwi, DEST_LINEOUT, SOURCE_WAVEOUT,
                   TRUE,
                   MXDUPDATELINE_ACTIONF_LINESTATUS |
                      MXDUPDATELINE_ACTIONF_SOURCE ) ;

    // send client his OPEN callback message

    waveCallBack( pClient, WOM_OPEN, 0L ) ;

    // are we running on a pcmcia sound card?

    if (phwi -> wFlags & SSI_FLAG_BUSTYPE_PCMCIA) 
    {
        // yep.. Start a time for waveOutGetPos

        phwi -> dwInterruptTimeStamp = 0 ;
    }

    return 0L ;

} // end of wodOpen()

//--------------------------------------------------------------------------
//
//  DWORD wodClose
//
//  Description:
//      Closes the wave-out device.
//
//  Parameters:
//      DWORD dwUser
//
//  Return (DWORD):
//
//--------------------------------------------------------------------------

DWORD NEAR PASCAL wodClose
(
    DWORD           dwUser
)
{
   PHARDWAREINSTANCE   phwi ;
   NPWAVEALLOC         pClient ;

   DPF( 3, "WODM_CLOSE" ) ;

   pClient = (NPWAVEALLOC) LOWORD( dwUser ) ;
   phwi = pClient -> phwi ;

   if (phwi -> lpwodQueue || phwi -> lpwodLoopStart)
   {
      DPF( 1, "Can't CLOSE!!!" ) ;
      return WAVERR_STILLPLAYING;
   }

   DPF( 3, "can close" ) ;

   // turn off DMA

   HwEndDSPAndDMA( phwi ) ;

   // update mixer line information

   MxdUpdateLine( phwi, DEST_LINEOUT, SOURCE_WAVEOUT,
                  FALSE,
                  MXDUPDATELINE_ACTIONF_LINESTATUS |
                     MXDUPDATELINE_ACTIONF_SOURCE ) ;

   // call client's callback

   waveCallBack( pClient, WOM_CLOSE, 0L ) ;

   // free the allocated memory
   LocalFree( (LOCALHANDLE)pClient ) ;

   wodHardwareRelease( phwi ) ;

   // Re-open the wid if lowpri state was saved...

   if (phwi -> fLowPriStateSaved)
      widLowPriRestoreState( phwi ) ;

   return 0L;

} // wodClose()

//--------------------------------------------------------------------------
//  
//  BOOL wodSuspend
//  
//  Description:
//      Suspends the waveout device.
//  
//  Parameters:
//      PHARDWAREINSTANCE phwi
//  
//  Return (BOOL):
//  
//--------------------------------------------------------------------------

BOOL FAR PASCAL wodSuspend
(
    PHARDWAREINSTANCE   phwi
)
{
   //
   // release the hardware if we own it
   //

   if (phwi -> bwodFlags & WOF_ALLOCATED)
   {
      if (!phwi -> fwodPaused)
      {
         HwStopDACDMA( phwi ) ;
         phwi -> bwodFlags |= WOF_RESTART ;
      }
      wodHardwareRelease( phwi ) ;
      phwi -> bwodFlags |= WOF_SUSPENDED ;
   }

   return TRUE ;

} // wodSuspend()

//--------------------------------------------------------------------------
//  
//  BOOL wodReactivate
//  
//  Description:
//     This function resumes a suspended waveout device.
//  
//  Parameters:
//      PHARDWAREINSTANCE phwi
//  
//  Return (BOOL):
//  
//--------------------------------------------------------------------------

BOOL FAR PASCAL wodReactivate
(
    PHARDWAREINSTANCE   phwi
)
{
   if (phwi ->  bwodFlags & WOF_SUSPENDED)
   {
      if (!wodHardwareAcquire( phwi ))
      {
          if (phwi -> bwodFlags & WOF_RESTART)
          {
             HwSetFormat( phwi, 
                          phwi -> wSamplesPerSec,
                          (BYTE)((phwi -> dwWaveFormat & WOFORMAT_STEREO) ? 2 : 1),
                          (BYTE)(phwi -> dwWaveFormat & ~WOFORMAT_STEREO),
                          DIRECTION_DAC ) ;
             HwBeginDACDMA( phwi ) ;
          }
          phwi -> bwodFlags &= ~(WOF_RESTART | WOF_SUSPENDED) ;
      }
      else
         return FALSE ;
   }
   return TRUE ;

} // wodReactivate()

//--------------------------------------------------------------------------
//
//  DWORD wodMessage
//
//  Description:
//      This function conforms to the standard wave output driver
//      message procedure (wodMessage), which is documented in mmddk.d.
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
//
//--------------------------------------------------------------------------

DWORD FAR PASCAL _loadds wodMessage
(
    UINT            uDevId,
    WORD            msg,
    DWORD           dwUser,
    DWORD           dwParam1,
    DWORD           dwParam2
)
{
   NPWAVEALLOC         pClient ;
   PHARDWAREINSTANCE   phwi ;

   // take care of init time messages...

   switch (msg)
   {
      case WODM_INIT:
      {
         DPF( 3, "WODM_INIT" ) ;

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
         DPF( 3, "WODM_ENABLE" ) ;

         // dwParam2 == PnP DevNode

         return (EnableDevNode( dwParam2 )) ;

      }
      break ;

      case DRVM_DISABLE:
      {
         DPF( 3, "WODM_DISABLE" ) ;

         // dwParam2 == PnP DevNode

         return (DisableDevNode( dwParam2 )) ;

      }
      break ;

      case DRVM_EXIT:
      {
         DPF( 3, "WODM_EXIT" ) ;

         // dwParam2 == PnP DevNode

         return (RemoveDevNode( dwParam2 )) ;

      }
      break ;

      case WODM_GETNUMDEVS:
      {
         DPF( 3, "WODM_GETNUMDEVS" ) ;

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

      case WODM_OPEN:
      {
         DWORD  dn ;

         DPF( 3, "WODM_OPEN" ) ;

         if (uDevId > 1)
            return MMSYSERR_BADDEVICEID ;

         dn = ((LPWAVEOPENDESC) dwParam1) -> dnDevNode ;

         if (NULL ==
               (phwi = DevNodeToHardwareInstance( dn )))
         {
            DPF( 1, "devnode not associated with hardware instance???" ) ;
            return MMSYSERR_BADDEVICEID ;
         }

         if ((!phwi -> fEnabled) || (phwi -> bwodFlags & WOF_SUSPENDED))
            return MMSYSERR_NOTENABLED ;

         return (wodOpen( phwi,
                          dwUser,
                          (LPWAVEOPENDESC) dwParam1,
                          dwParam2 )) ;
      }
      break ;

      case WODM_GETDEVCAPS:
      {
         DPF( 3, "WODM_GETDEVCAPS" ) ;

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

         wodGetDevCaps( phwi, (MDEVICECAPSEX FAR *) dwParam1 ) ;
         return MMSYSERR_NOERROR ;
      }
      break ;

      case WODM_SETVOLUME:
      {
         MIXERCONTROLDETAILS          mcd ;
         MIXERCONTROLDETAILS_UNSIGNED mcd_u[ 2 ] ;

         DPF( 3, "WODM_SETVOLUME" ) ;

         if (NULL ==
               (phwi = DevNodeToHardwareInstance( dwParam2 )))
         {
            DPF( 1, "devnode not associated with hardware instance???" ) ;
            return MMSYSERR_BADDEVICEID ;
         }

         // Set up control details structure

         mcd.dwControlID = VOL_OUTDAC ;
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

      case WODM_GETVOLUME:
      {
         PMIXERINSTANCE pmi ;

         if (NULL ==
               (phwi = DevNodeToHardwareInstance( dwParam2 )))
         {
            DPF( 1, "devnode not associated with hardware instance???" ) ;
            return MMSYSERR_BADDEVICEID ;
         }

         pmi = phwi -> pmi ;

         *((DWORD FAR *) dwParam1) =
            MAKELONG( (WORD) pmi -> dwValue[ VOL_OUTDAC ][ 0 ],
                        (WORD) pmi -> dwValue[ VOL_OUTDAC ][ 1 ] ) ;
         return MMSYSERR_NOERROR ;
      }

   }

   // Bad device ID???

   if (uDevId > 1)
      return MMSYSERR_BADDEVICEID ;

   pClient = (NPWAVEALLOC)LOWORD( dwUser ) ;
   phwi = pClient -> phwi ;

   if ((!phwi -> fEnabled) || (phwi -> bwodFlags & WOF_SUSPENDED))
      return MMSYSERR_NOTENABLED ;

   switch (msg)
   {
      case WODM_CLOSE:
         return ( wodClose( dwUser ) ) ;

      case WODM_WRITE:
      {
         DPF( 3, "WODM_WRITE" ) ;

         // Make sure we're the owner, otherwise return invalid param

         if (dwUser != phwi -> dwCurCODECOwner)
            return MMSYSERR_INVALPARAM ;

         // check if it's been prepared

         if (!(((LPWAVEHDR)dwParam1) -> dwFlags & WHDR_PREPARED))
            return WAVERR_UNPREPARED ;

         // store the pointer to my WAVEALLOC structure in the wavehdr

         ((LPWAVEHDR)dwParam1) -> reserved = (DWORD)(LPSTR)pClient ;

         // add the buffer to our queue

         ((LPWAVEHDR)dwParam1) -> dwFlags |= WHDR_INQUEUE ;
         ((LPWAVEHDR)dwParam1) -> dwFlags &= ~WHDR_DONE ;

         HwAddDACBlock( phwi, (LPWAVEHDR) dwParam1 ) ;

         return 0L ;
      }

      case WODM_PAUSE:
      {
         DPF( 3, "WODM_PAUSE" ) ;

         // Make sure we're the owner, otherwise return invalid param

         if (dwUser != phwi -> dwCurCODECOwner)
            return MMSYSERR_INVALPARAM ;

         // are we running on a pcmcia sound card?

         if (phwi -> wFlags & SSI_FLAG_BUSTYPE_PCMCIA)
         {
            phwi -> dwPauseTimeDelta = 
               (DWORD)timeGetTime() - phwi -> dwInterruptTimeStamp ;
         }
         HwPauseDAC( phwi );
         return 0L ;
      }

      case WODM_RESTART:
      {
         DPF( 3, "WODM_RESTART" ) ;

         // Make sure we're the owner, otherwise return invalid param

         if (dwUser != phwi -> dwCurCODECOwner)
            return MMSYSERR_INVALPARAM ;

         // are we running on a pcmcia sound card?

         if (phwi -> wFlags & SSI_FLAG_BUSTYPE_PCMCIA) 
         {          
            phwi -> dwPauseTimeDelta = 0 ;
            phwi -> dwInterruptTimeStamp = 
               (DWORD) timeGetTime() - phwi -> dwPauseTimeDelta ;
         }
         HwResumeDAC( phwi ) ;
         return 0L ;
      }

      case WODM_RESET:
      {
         DPF( 3, "WODM_RESET" ) ;

         // Make sure we're the owner, otherwise return invalid param

         if (dwUser != phwi -> dwCurCODECOwner)
            return MMSYSERR_INVALPARAM ;

         // Stop DAC DMA

         HwStopDACDMA( phwi ) ;

         // free the buffers

         wodFreeQ( phwi ) ;

         // Make sure we've reset flags...

         phwi -> fwodBreakLoop = 0 ;
         phwi -> fwodPaused = 0 ;
         phwi -> fwodSkipThisLoop = 0 ;
         phwi -> lpwodLoopStart = NULL ;
         phwi -> dwLastPosition = 0L ;

         // reset byte count

         pClient = (NPWAVEALLOC)LOWORD( dwUser ) ;
         pClient -> dwByteCount = 0L;
         pClient -> dwHalfDMABuffers = 0L ;

         return 0L ;
      }

      case WODM_BREAKLOOP:
      {
         DPF( 3, "WODM_BREAKLOOP" ) ;

         // Make sure we're the owner, otherwise return invalid param

         if (dwUser != phwi -> dwCurCODECOwner)
            return MMSYSERR_INVALPARAM ;

         if (phwi -> lpwodQueue)
             phwi -> fwodBreakLoop = 1 ;
         return 0L ;
      }

      case WODM_GETPOS:
      {
         DPF( 3, "WODM_GETPOS" ) ;

         if (!dwUser)
            return MMSYSERR_INVALPARAM;

         return waveGetPos( dwUser, (LPMMTIME)dwParam1,
                            dwParam2, GCP_OPTIONF_DAC ) ;
      }

      default:
         return MMSYSERR_NOTSUPPORTED ;
   }

   // should never get here...

   return MMSYSERR_NOTSUPPORTED ;

} // wodMessage()

//---------------------------------------------------------------------------
//  End of File: wave.c
//---------------------------------------------------------------------------

