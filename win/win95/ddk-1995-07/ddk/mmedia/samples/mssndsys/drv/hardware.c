//---------------------------------------------------------------------------
//
//   HARDWARE.C
//
//---------------------------------------------------------------------------
//
//  Module: hardware.c
//
//  Purpose: CODEC interface routines
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

// rate table for crystals...

static BYTE BCODE rateSend[] =
{
   0x01, 0x0f,
   0x00, 0x0e,
   0x03, 0x02,
   0x05, 0x07,
   0x04, 0x06,
   0x0d, 0x09,
   0x0b, 0x0c
} ;

#define NUMRATES (sizeof(rateSend)/sizeof(rateSend[0]))

static WORD BCODE rates[] =
{
   (5510 + 6620) / 2, (6620 + 8000) / 2,
   (8000 + 9600) / 2, (9600 + 11025) / 2,
   (WORD)(( 11025 +  16000) / 2), (WORD)((16000L + 18900L) / 2),
   (WORD)((18900L + 22050L) / 2), (WORD)((22050L + 27420L) / 2),
   (WORD)((27420L + 32000L) / 2), (WORD)((32000L + 33075L) / 2),
   (WORD)((33075L + 37800L) / 2), (WORD)((37800L + 44100L) / 2),
   (WORD)((44100L + 48000L) / 2)
} ;

static WORD BCODE rate[] =
{
   5510, 6620, 8000, 9600, 11025, 16000, 18900, 22050,
   27420, 32000, 33075, 37800, 44100, 48000
} ;

//--------------------------------------------------------------------------
//
//  WORD HwNearestRate
//
//  Description:
//      Finds the nearest sampling rate supported by the board.
//
//  Parameters:
//      DWORD samPerSec
//
//  Return (WORD):
//      nearest sampling rate
//
//
//--------------------------------------------------------------------------

WORD FAR PASCAL HwNearestRate
(
    DWORD           samPerSec
)
{
   BYTE    i;

   // D1 ("HwNearestRate" ) ;

   for (i = 0; i < (NUMRATES - 1); i++)
      if (samPerSec < (DWORD)rates[i])
         return rate[i];

   return rate[NUMRATES - 1];

} // end of HwNearestRate()

//--------------------------------------------------------------------------
//
//  VOID HwSetFormat
//
//  Description:
//      Sets the sampling/playback format
//
//  Parameters:
//      PHARDWAREINSTANCE phwi
//         pointer to hardware instance structure
//
//      DWORD samPerSec
//         samples per second
//
//      BYTE channels
//         channels
//
//      BYTE format
//         current format
//
//      BYTE bDirection
//         direction (ADC or DAC)
//
//  Return (VOID):
//      Nothing.
//
//
//--------------------------------------------------------------------------

VOID FAR PASCAL HwSetFormat
(
    PHARDWAREINSTANCE       phwi,
    DWORD                   samPerSec,
    BYTE                    channels,
    BYTE                    format,
    BYTE                    bDirection
)
{
   DWORD   dwBytesPerSecond ;
   WORD    w ;
   BYTE    send, i ;

   DPF( 1, "HwSetFormat" ) ;

   if (!phwi -> fEnabled)
      return ;

   phwi -> wSamplesPerSec = (WORD) samPerSec ;

   // sampling rate...

   for (i = 0, send = rateSend[NUMRATES - 1]; i < (NUMRATES - 1); i++)
   {
      if (samPerSec < (DWORD)rates[i])
      {
         send = rateSend[i];
         break;
      }
   }

   // channels

   send |= ((BYTE) (channels - 1) << 4);

   // data format

   send |= (format << 5);

   //
   // Format change if necessary...
   //

   if ((WORD) send != phwi -> wOldFormat)
   {

      if (phwi -> wCODECClass == CODEC_J_CLASS)
         HwExtMute( phwi, TRUE ) ;
      HwEnterMCE( phwi, FALSE ) ;

      CODEC_RegWrite( phwi, REGISTER_DATAFORMAT, (BYTE) send ) ;

      HwLeaveMCE( phwi, FALSE ) ;

#ifdef AZTECH
      if (phwi -> wCODECClass == CODEC_KPLUS_CLASS)
      {
         HwEnterMCE( phwi, FALSE ) ;
         CODEC_RegWrite( phwi, REGISTER_CAP_DATAFORMAT, (BYTE) send ) ;
         HwLeaveMCE( phwi, FALSE ) ;
      }
#endif

      if (phwi -> wCODECClass == CODEC_J_CLASS)
         HwExtMute( phwi, FALSE ) ;

      phwi -> wOldFormat = (WORD) send ;
   }
   else
   {
      //
      // If we've underrun/overrun then the CODEC is in
      // a bad state... clear it up by going through MCE &
      // recalibrating.
      //

      BYTE  bStatus ;

      bStatus = READPORT( CODEC_STATUS ) ;

      if (0x1F == (bStatus & 0x1F))
      {
         if (phwi -> wCODECClass == CODEC_J_CLASS)
            HwExtMute( phwi, TRUE ) ;

         HwEnterMCE( phwi, FALSE ) ;

         CODEC_RegWrite( phwi, REGISTER_DATAFORMAT, (BYTE) phwi -> wOldFormat ) ;

         HwLeaveMCE( phwi, FALSE ) ;

#ifdef AZTECH
      if (phwi -> wCODECClass == CODEC_KPLUS_CLASS)
      {
         HwEnterMCE( phwi, FALSE ) ;
         CODEC_RegWrite( phwi, REGISTER_CAP_DATAFORMAT, send ) ;
         HwLeaveMCE( phwi, FALSE ) ;
      }
#endif
         if (phwi -> wCODECClass == CODEC_J_CLASS)
            HwExtMute( phwi, FALSE ) ;

      }
   }


   phwi -> wDMAHalfBufSize =
      (WORD)(((DWORD) phwi -> wDMABufferLenMinusOne + 1) / 2) ;

   phwi -> wBytesPerSample  =
      (BYTE)(channels * ((format == FORMAT_16BIT) ? 2 : 1)) ;
   dwBytesPerSecond =
      (DWORD) HwNearestRate( samPerSec ) * phwi -> wBytesPerSample  ;

   // go for 16 interrupts per second, and round
   // down to the nearest 4 bytes.

   w = (WORD) (dwBytesPerSecond / 16) & 0xfffc ;

   phwi -> wDMAHalfBufSize = min( w, phwi -> wDMAHalfBufSize ) ;

   // set DMA buffer info...

   switch (format)
   {
      case FORMAT_8BIT:
      case FORMAT_ALAW:
      case FORMAT_MULAW:
         phwi -> wCODECSamples = (channels == 1) ?
            ((phwi -> wDMAHalfBufSize) - 1) :
            ((phwi -> wDMAHalfBufSize >> 1) - 1);
         break;

      case FORMAT_16BIT:
         phwi -> wCODECSamples = (channels == 1) ?
            ((phwi -> wDMAHalfBufSize >> 1) - 1) :
            ((phwi -> wDMAHalfBufSize >> 2) - 1);
         break;
   }

} // HwSetFormat()

//--------------------------------------------------------------------------
//
//  BOOL MxdPeakMeter
//
//  Description:
//      processes control information for mixer peak meter... values
//      were computed and stored in the hardware instance when we filled
//      or retrieved the PCM data from the buffer.
//
//  Parameters:
//      PHARDWAREINSTANCE phwi
//
//      DWORD dwControlID
//
//      LPLONG palValue
//
//  Return (BOOL):
//
//
//--------------------------------------------------------------------------

BOOL FAR PASCAL MxdPeakMeter
(
    PHARDWAREINSTANCE   phwi,
    DWORD               dwControlID,
    LPLONG              palValue
)
{
    PMIXERINSTANCE   pmi ;

    //
    //
    //
    pmi = phwi -> pmi ;

    palValue[0] = palValue[ 1 ] = 0 ;

    if (!phwi -> bIntUsed || (!phwi -> fwodDMABusy && !phwi -> fwidDMABusy))
        return (TRUE);

    switch (dwControlID)
    {
        case VU_WAVEOUT:
            if (INT_WAVEOUT != phwi -> bIntUsed)
                return (TRUE);

            break;

        case VU_WAVEIN_AUX1:
        case VU_WAVEIN_MIC:
            if (INT_WAVEIN != phwi -> bIntUsed)
                return (TRUE);

            if (phwi -> dwCurCODECOwner == phwi -> dwLowUser)
                return (TRUE);

            if (dwControlID !=
                  pmi -> dwValue[ MUX_WAVEIN ][ 0 ] + VU_WAVEIN_AUX1)
               return ( TRUE ) ;


            break;

        case VU_VOICEIN_AUX1:
        case VU_VOICEIN_MIC:
            if (INT_WAVEIN != phwi -> bIntUsed)
                return (TRUE);

            if (phwi -> dwCurCODECOwner != phwi -> dwLowUser)
                return (TRUE);

            if (dwControlID !=
                  pmi -> dwValue[ MUX_VOICEIN ][ 0 ] + VU_VOICEIN_AUX1)
               return ( TRUE ) ;

            break;

        default:
            return (FALSE);
    }

    switch (phwi -> bIntUsed)
    {
       case INT_WAVEOUT:
          palValue[ 0 ] = phwi -> dwwodLeftPeak ;
          palValue[ 1 ] = phwi -> dwwodRightPeak ;
          break ;

       case INT_WAVEIN:
          palValue[ 0 ] = phwi -> dwwidLeftPeak ;
          palValue[ 1 ] = phwi -> dwwidRightPeak ;
          break ;
    }

    return TRUE ;

} // MxdPeakMeter()

//--------------------------------------------------------------------------
//
//  VOID HwBeginADCDMA
//
//  Description:
//      Begin filling ADC DMA.  Starts the wave input if not
//      already active.
//
//  Parameters:
//      PHARDWAREINSTANCE phwi
//         pointer to hardware instance structure
//
//  Return (VOID):
//
//
//--------------------------------------------------------------------------

VOID FAR PASCAL HwBeginADCDMA
(
    PHARDWAREINSTANCE        phwi
)
{
   BYTE  bInterface, bPrev ;
   WORD  wDMAElements ;

   DPF( 1, "HwBeginADCDMA" ) ;

   if (!phwi -> fEnabled)
      return ;

   // Don't start it if its already going

   if (phwi -> fwidStarted)
      return ;
   phwi -> fwidStarted = TRUE ;
   phwi -> fwidDMABusy = 1 ;

   // We're not ready to stop yet...

   phwi -> fDMADone = FALSE ;
   phwi -> wLastDMAPos = 0 ;

   // Clear any pending interrupts

   bPrev = (BYTE) (CODEC_RegRead( phwi, REGISTER_DSP ) & 0xC0) ;
   CODEC_RegWrite( phwi, REGISTER_DSP, (BYTE) (phwi -> bMute | bPrev) ) ;
   WRITEPORT( CODEC_STATUS, 0 ) ;

   if (phwi -> wFlags & SSI_FLAG_BUSTYPE_PCMCIA)
   {
      // Clear SND_CTRL_REG Host and CODEC ptrs.

      WRITEPORT( 0xA, 0x6 ) ;

      phwi -> wLastDMAPos = DMA_BUFFER_PONG ;

   }
   else
   {
      ASCRITENTER;

      // Make sure that the DMA is off

      WRITE_PORT_UCHAR( phwi -> portCaptureDMASMR,
                        phwi -> portCaptureDMADisable ) ;

      // Start the DMA

      WRITE_PORT_UCHAR( phwi -> portCaptureDMAClr, 0 ) ;
      WRITE_PORT_UCHAR( phwi -> portCaptureDMAMod,
                        phwi -> portCaptureDMARead ) ;
      WRITE_PORT_UCHAR( phwi -> portCaptureDMAAdr,
                        phwi -> wDMAPhysAddr ) ;
      WRITE_PORT_UCHAR( phwi -> portCaptureDMAAdr,
                        (phwi -> wDMAPhysAddr) >> 8 ) ;
      WRITE_PORT_UCHAR( phwi -> portCaptureDMAPReg,
                        phwi -> bDMAPhysPage ) ;

      wDMAElements = (WORD)(((DWORD) phwi -> wDMAHalfBufSize * 2) - 1) ;

      WRITE_PORT_UCHAR( phwi -> portCaptureDMACnt, wDMAElements ) ;
      WRITE_PORT_UCHAR( phwi -> portCaptureDMACnt, wDMAElements >> 8 ) ;
      WRITE_PORT_UCHAR( phwi -> portCaptureDMASMR,
                        phwi -> portCaptureDMAEnable ) ;

      ASCRITLEAVE;
   }

   if (phwi -> wCODECClass == CODEC_J_CLASS)
      HwExtMute( phwi, TRUE ) ;

   MixSetADCHardware( phwi, NULL ) ;

   HwEnterMCE( phwi, FALSE ) ;

   CODEC_RegWrite( phwi, REGISTER_LOWERBASE,
                   (BYTE) (phwi -> wCODECSamples) ) ;
   CODEC_RegWrite( phwi, REGISTER_UPPERBASE,
                   (BYTE) (phwi -> wCODECSamples >> 8) ) ;

#ifdef AZTECH
   if (phwi -> wCODECClass == CODEC_KPLUS_CLASS)
   {
      CODEC_RegWrite( phwi, REGISTER_CAP_LOWERBASE,
                      (BYTE) (phwi -> wCODECSamples) ) ;
      CODEC_RegWrite( phwi, REGISTER_CAP_UPPERBASE,
                      (BYTE) (phwi -> wCODECSamples >> 8) ) ;
   }
#endif
   // say that we want to record

   if ((CODEC_J_CLASS == phwi -> wCODECClass) &&
       (0 == (phwi -> wHardwareOptions & DAK_DUALDMA)))
   {
      bInterface =
         AD1848_CONFIG_PPIO | AD1848_CONFIG_CEN | AD1848_CONFIG_ACAL ;
   }
   else
      bInterface = AD1848_CONFIG_CEN | AD1848_CONFIG_ACAL ;

   if (0 == (phwi -> wHardwareOptions & DAK_DUALDMA))
      bInterface |= AD1848_CONFIG_SDC ;

   CODEC_RegWrite( phwi, REGISTER_INTERFACE, bInterface ) ;

   // start DMA

   bPrev = (BYTE) (CODEC_RegRead( phwi, REGISTER_DSP ) & 0xC0) ;
   CODEC_RegWrite( phwi, REGISTER_DSP, 
                   (BYTE)(phwi -> bMute | bPrev | (BYTE)(0x02)) ) ;

   HwLeaveMCE( phwi, FALSE ) ;

   if (phwi -> wCODECClass == CODEC_J_CLASS)
      HwExtMute( phwi, FALSE ) ;

} // HwBeginADCDMA()

//--------------------------------------------------------------------------
//
//  VOID HwEndADCDMA
//
//  Description:
//      Stops the ADC.
//
//  Parameters:
//      PHARDWAREINSTANCE phwi
//         pointer to hardware instance structure
//
//  Return (VOID):
//      Nothing.
//
//
//--------------------------------------------------------------------------

VOID FAR PASCAL HwEndADCDMA
(
    PHARDWAREINSTANCE       phwi
)
{
   DPF( 1, "HwEndADCDMA" ) ;

   if (!phwi -> fEnabled)
      return ;

   if (!phwi -> fwidStarted)
      return ;

   HwEndDSPAndDMA( phwi ) ;

   // Set the last done bit in the last block of the header...

   widSendPartBuffer( phwi ) ;

   phwi -> fwidStarted = FALSE ;

} // HwEndADCDMA()

//------------------------------------------------------------------------
//  LPWAVEHDR HwAddADCBlock
//
//  Description:
//     Adds a block to be filled by the ADC.
//
//  Parameters:
//     PHARDWAREINSTANCE phwi
//        pointer to hardware instance structure
//
//     LPWAVEHDR lpWIQ
//        pointer to wave-in header queue
//
//     LPWAVEHDR lpWIHdr
//        far pointer to wave input data header
//
//  Return Value:
//     Nothing.
//
//
//------------------------------------------------------------------------

LPWAVEHDR FAR PASCAL HwAddADCBlock
(
    PHARDWAREINSTANCE       phwi,
    LPWAVEHDR               lpWIQ,
    LPWAVEHDR               lpWIHdr
)
{
   LPWAVEHDR    temp ;

   DPF( 1, "HwAddADCBlock" ) ;

   if (!phwi -> fEnabled)
      return ( lpWIQ ) ;

   // Next pointer should be NULL.

   lpWIHdr -> lpNext = NULL ;

   // increment our "wid" in queue counter

   ((NPWAVEALLOC) (lpWIHdr -> reserved)) -> dwByteCount += 
      lpWIHdr -> dwBufferLength ;

   ASCRITENTER ;

   // See if there is a queue to add...

   if (!lpWIQ)
   {
      // There's nothing in the queue. Add it...

      lpWIQ = lpWIHdr ;

      phwi -> hpwidCurData = NULL ;

      ASCRITLEAVE ;

      return ( lpWIQ ) ;
   }

   // Walk to the end of the queue...

   for (temp = lpWIQ; temp -> lpNext != NULL; temp = temp -> lpNext) ;

   // At the end, add the new block.

   temp -> lpNext = lpWIHdr ;

   ASCRITLEAVE ;

   return( lpWIQ ) ;

} // end of HwAddADCBlock()

//--------------------------------------------------------------------------
//  
//  WORD HwPrimeDAC
//  
//  Description:
//  
//  
//  Parameters:
//      PHARDWAREINSTANCE phwi
//  
//  Return (WORD):
//      bytes copied to DMA buffer
//  
//--------------------------------------------------------------------------

WORD NEAR HwPrimeDAC
(
    PHARDWAREINSTANCE   phwi
)
{
   WORD  wCopied ;

   if (phwi -> wFlags & SSI_FLAG_BUSTYPE_PCMCIA)
   {
      // Clear SND_CTRL_REG Host and CODEC ptrs.

      WRITEPORT( 0xA, 0x6 ) ;
      phwi -> wLastDMAPos = DMA_BUFFER_DONE ;

      wCopied =
         wodDMALoadBuffer( phwi, phwi -> lpDMABuffer,
                           phwi -> wDMAHalfBufSize, TRUE ) ;

      if (wCopied >= phwi -> wDMAHalfBufSize)
      {
         // Flip page.

         WRITEPORT( 0xA, 0x7 );
         wCopied +=
            wodDMALoadBuffer( phwi, phwi -> lpDMABuffer,
                              phwi -> wDMAHalfBufSize, TRUE ) ;

         if (wCopied > phwi -> wDMAHalfBufSize)
            phwi -> wLastDMAPos = DMA_BUFFER_PONG ;
      }

   }
   else
   {
      //
      // two parts so the notification algorithm works correctly...
      // 

      wCopied =
         wodDMALoadBuffer( phwi, phwi -> lpDMABuffer,
                           phwi -> wDMAHalfBufSize, TRUE ) ;
      wCopied +=
         wodDMALoadBuffer( phwi, phwi -> lpDMABuffer + 
                              phwi -> wDMAHalfBufSize,
                           phwi -> wDMAHalfBufSize, TRUE ) ;
   }

   if (wCopied > phwi -> wDMAHalfBufSize)
      phwi -> fDMADone = FALSE ;

   return wCopied ;

} // HwPrimeDAC()

//--------------------------------------------------------------------------
//
//  VOID HwBeginDACDMA
//
//  Description:
//     1) Load the full DMA buffer
//     2) Start the DMA from the 1st half buffer.
//
//  Parameters:
//     PHARDWAREINSTANCE phwi
//        pointer to hardware instance structure
//
//  Return (VOID):
//     Nothing.
//
//--------------------------------------------------------------------------

VOID FAR PASCAL HwBeginDACDMA
(
    PHARDWAREINSTANCE       phwi
)
{
   BYTE    bInterface, bPrev ;
   WORD    wDMAElements ;

   DPF( 1, "HwBeginDACDMA" ) ;

   if (!phwi -> fEnabled)
      return ;

   // Make sure the CODEC's dead...

   bPrev = (BYTE) (CODEC_RegRead( phwi, REGISTER_DSP ) & 0xC0) ;
   CODEC_RegWrite( phwi, REGISTER_DSP, 
                   (BYTE)(phwi -> bMute | bPrev | (BYTE) 0x00) ) ;
   WRITEPORT( CODEC_STATUS, 0 ) ;

   // Load half the buffer and start playing...

   phwi -> fDMADone = TRUE ;
   phwi -> wLastDMAPos = 0 ;

   if (!HwPrimeDAC( phwi ) && !phwi -> fJustStart)
   {
      DPF( 2, "NoDataSent" ) ;
      return ;
   }

   // Ok, we've started...

   phwi -> fwodDMABusy = 1 ;

   if (0 == (phwi -> wFlags & SSI_FLAG_BUSTYPE_PCMCIA))
   {

      ASCRITENTER ;

      // Make sure that the DMA is off...

      WRITE_PORT_UCHAR( phwi -> portPlaybackDMASMR,
                        phwi -> portPlaybackDMADisable ) ;

      // Enable interrupts and start DMA

      WRITE_PORT_UCHAR( phwi -> portPlaybackDMAClr, 0 ) ;
      WRITE_PORT_UCHAR( phwi -> portPlaybackDMAMod,
                        phwi -> portPlaybackDMAWrite ) ;

      // Output the address

      WRITE_PORT_UCHAR( phwi -> portPlaybackDMAAdr,
                        phwi -> wDMAPhysAddr ) ;
      WRITE_PORT_UCHAR( phwi -> portPlaybackDMAAdr,
                        (phwi -> wDMAPhysAddr) >> 8 ) ;
      WRITE_PORT_UCHAR( phwi -> portPlaybackDMAPReg,
                        phwi -> bDMAPhysPage ) ;

      wDMAElements = (WORD)(((DWORD) phwi -> wDMAHalfBufSize * 2) - 1);

      WRITE_PORT_UCHAR( phwi -> portPlaybackDMACnt, wDMAElements ) ;
      WRITE_PORT_UCHAR( phwi -> portPlaybackDMACnt, wDMAElements >> 8 ) ;

      // Enable the DMA channel

      WRITE_PORT_UCHAR( phwi -> portPlaybackDMASMR,
                        phwi -> portPlaybackDMAEnable ) ;

      ASCRITLEAVE;
   }

   // Program the CODEC for the next chunk. Set up for the mode we want

   // Tell the codec's DMA how many samples

   CODEC_RegWrite( phwi, REGISTER_LOWERBASE,
                   (BYTE) (phwi -> wCODECSamples) ) ;
   CODEC_RegWrite( phwi, REGISTER_UPPERBASE,
                   (BYTE) (phwi -> wCODECSamples >> 8) ) ;

#ifdef AZTECH
   if (phwi -> wCODECClass == CODEC_KPLUS_CLASS)
   {
      CODEC_RegWrite( phwi, REGISTER_CAP_LOWERBASE,
                      (BYTE) (phwi -> wCODECSamples) ) ;
      CODEC_RegWrite( phwi, REGISTER_CAP_UPPERBASE,
                      (BYTE) (phwi -> wCODECSamples >> 8) ) ;
   }
#endif
   // Turn on the DAC outputs

   DPF( 1, "In HwBeginDACDMA" ) ;

   CODEC_RegWrite( phwi, REGISTER_LEFTOUTPUT, phwi -> bDACToCODECLeft ) ;
   CODEC_RegWrite( phwi, REGISTER_RIGHTOUTPUT, phwi -> bDACToCODECRight ) ;

   // If not paused the start the CODEC

   if (!phwi -> fwodPaused)
   {
      // program the CODEC

      bInterface = AD1848_CONFIG_PEN ;

      if (0 == (phwi -> wHardwareOptions & DAK_DUALDMA))
         bInterface |= AD1848_CONFIG_SDC ;

      CODEC_RegWrite( phwi, REGISTER_INTERFACE, bInterface ) ;

      // start interrupts

      CODEC_RegWrite( phwi, REGISTER_DSP,
                      (BYTE)(phwi -> bMute | bPrev | (BYTE) 0x02) ) ;
   }

} // HwBeginDACDMA()

//--------------------------------------------------------------------------
//
//  VOID HwResumeDAC
//
//  Description:
//      Resumes playing of DAC.
//
//  Parameters:
//     PHARDWAREINSTANCE phwi
//        pointer to hardware instance structure
//
//  Return (VOID):
//     Nothing.
//
//--------------------------------------------------------------------------

VOID FAR PASCAL HwResumeDAC
(
    PHARDWAREINSTANCE       phwi
)
{
   BYTE  bInterface, bPrev ;

   DPF( 1, "HwResumeDAC" ) ;

   if (!phwi -> fEnabled)
      return ;

   if (!phwi -> fwodPaused)
      return ;

   phwi -> fwodPaused = 0 ;

   if (!phwi -> fwodDMABusy)
   {
      // Weren't playing anything so start

      HwBeginDACDMA( phwi ) ;
      if (phwi -> wFlags & SSI_FLAG_BUSTYPE_PCMCIA) 
      {
         // time stamp the first buffer
         phwi->dwInterruptTimeStamp = timeGetTime();
      }
      return ;
   }

   // send to CODEC saying to continue

   bInterface = AD1848_CONFIG_PEN ;
   if (0 == (phwi -> wHardwareOptions & DAK_DUALDMA))
      bInterface |= AD1848_CONFIG_SDC ;

   CODEC_RegWrite( phwi, REGISTER_INTERFACE, bInterface ) ;

   // start interrupts

   bPrev = (BYTE) (CODEC_RegRead( phwi, REGISTER_DSP ) & 0xC0) ;
   CODEC_RegWrite( phwi, REGISTER_DSP, 
                   (BYTE)(phwi -> bMute | bPrev | (BYTE)0x02) ) ;

   // turn on the DAC outputs

   CODEC_RegWrite( phwi, REGISTER_LEFTOUTPUT, phwi -> bDACToCODECLeft ) ;
   CODEC_RegWrite( phwi, REGISTER_RIGHTOUTPUT, phwi -> bDACToCODECRight ) ;

} // HwResumeDAC()

//--------------------------------------------------------------------------
//
//  VOID HwPauseDAC
//
//  Description:
//      Pauses the DAC.
//
//  Parameters:
//     PHARDWAREINSTANCE phwi
//        pointer to hardware instance structure
//
//  Return (VOID):
//     Nothing.
//
//
//--------------------------------------------------------------------------

VOID FAR PASCAL HwPauseDAC
(
    PHARDWAREINSTANCE       phwi
)
{
   BYTE  bPrev ;

   DPF( 1, "HwPauseDAC" ) ;

   if (!phwi -> fEnabled)
      return ;

   if (phwi -> fwodPaused)
      return ;

   phwi -> fwodPaused = 1 ;

   if (!phwi -> fwodDMABusy)
      return ;

   // Turn off DAC outputs

   CODEC_RegWrite( phwi, REGISTER_LEFTOUTPUT, 0x3f ) ;
   CODEC_RegWrite( phwi, REGISTER_RIGHTOUTPUT, 0x3f ) ;

   // Tell the CODEC to pause

   CODEC_RegWrite( phwi, REGISTER_INTERFACE,
                   (BYTE)
                      ((phwi -> wHardwareOptions & DAK_DUALDMA) ?
                         0 : AD1848_CONFIG_SDC) ) ;

   bPrev = (BYTE) (CODEC_RegRead( phwi, REGISTER_DSP ) & 0xC0) ;
   CODEC_RegWrite( phwi, REGISTER_DSP, 
                   (BYTE)(phwi -> bMute | bPrev | (BYTE) 0x00) ) ;

} // HwPauseDAC()

//--------------------------------------------------------------------------
//
//  VOID HwStopDACDMA
//
//  Description:
//      Halts the DAC's DMA.
//
//  Parameters:
//     PHARDWAREINSTANCE phwi
//        pointer to hardware instance structure
//
//  Return (VOID):
//     Nothing.
//
//  History:   Date       Author      Comment
//--------------------------------------------------------------------------

VOID FAR PASCAL HwStopDACDMA
(
    PHARDWAREINSTANCE       phwi
)
{
   DPF( 1, "HwStopDACDMA" ) ;

   if (!phwi -> fEnabled)
      return ;

   HwEndDSPAndDMA( phwi ) ;

} // HwStopDACDMA()

//--------------------------------------------------------------------------
//
//  VOID HwAddDACBlock
//
//  Description:
//      When a block is written it is added to the queue for the physical
//      device (only one in this case). If the DSP is currently sending
//      data the routine returns. If the DSP is idel then the DMA process is
//      started and a request for a new block is made immediately. As each
//      block is finihed its callback function is invokes (if not NULL).
//      If the app doesn't supply a callback then it's reponsible for keeping
//      the queue full and monitoring all blocks for "done bit" set when
//      the CODEC is done with them. We assume the header and block are both
//      page locked when they get here.
//
//
//  Parameters:
//      PHARDWAREINSTANCE phwi
//
//      LPWAVEHDR lpHdr
//
//  Return (VOID):
//      Nothing.
//
//--------------------------------------------------------------------------

VOID HwAddDACBlock
(
    PHARDWAREINSTANCE       phwi,
    LPWAVEHDR               lpHdr
)
{
   LPWAVEHDR       temp;

   DPF( 1, "HwAddDACBlock" ) ;

   if (!phwi -> fEnabled)
      return ;

   lpHdr -> lpNext = 0 ;

   ASCRITENTER ;

   if (!phwi -> lpwodQueue)
   {
      phwi -> lpwodQueue = lpHdr ;

      if (phwi -> lpwodLoopStart)
      {
         for (temp = phwi -> lpwodLoopStart;
              temp -> lpNext != NULL;
              temp = temp->lpNext) ;
         temp -> lpNext = lpHdr ;
      }
   }
   else
   {
      for (temp = phwi -> lpwodQueue;
           temp->lpNext != NULL;
           temp = temp->lpNext) ;

      temp->lpNext = lpHdr ;
   }

   if (phwi -> fwodDMABusy)
   {
      if (phwi -> lpSilenceStart)
      {
         LPSTR lpBuf ;
         WORD  wFill ;

         lpBuf = phwi -> lpSilenceStart ;
         phwi -> lpSilenceStart = NULL ;
         wFill = phwi -> wSilenceSize ;
         phwi -> wSilenceSize = 0 ;

         wodDMALoadBuffer( phwi, lpBuf, wFill, TRUE ) ;
         if (phwi -> fDMADone)
         {
            DPF( 2,  "silencekick" ) ;
            if (phwi -> wFlags & SSI_FLAG_BUSTYPE_PCMCIA)
            {
               phwi -> wLastDMAPos = DMA_BUFFER_PING ;
               if ((LOWORD( lpBuf ) - LOWORD( phwi -> lpDMABuffer )) >=
                     phwi -> wDMAHalfBufSize)
                  phwi -> wLastDMAPos++ ;
            }

            phwi -> fDMADone = FALSE ;
         }
      }
   }
   else if (!phwi -> fwodPaused)
      HwBeginDACDMA( phwi ) ;

   ASCRITLEAVE ;
}

//---------------------------------------------------------------------------
//  End of File: hardware.c
//---------------------------------------------------------------------------
