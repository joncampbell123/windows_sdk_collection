//---------------------------------------------------------------------------
//
//   HARDFIX.C
//
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//
//  Module: hardfix.c
//
//  Purpose:
//     hardware interface for WSS (fixed routines)
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

#define	Not_VxD
#include <vmm.h>
#include <configmg.h>

#include <mmsystem.h>
#include <mmddk.h>
#include <mmreg.h>
#include <dos.h>
#include "mssndsys.h"
#include "driver.h"

//-----------------------
//
// Manifest constants
//
//-----------------------

#define TIMEDELAY  (15)

//-----------------------
//
// Global variables
//
//-----------------------

// Make sure we don't enter crit when already in one...

WORD  gwCritLevel = 0 ;

// Temporary values of registers

static BYTE gbReg[REGISTER_RIGHTOUTPUT + 1] ;

//--------------------------------------------------------------------------;
//
//  BOOL HwPeakMeter
//
//  Description:
//      Computes the values for a peak meter using the DMA buffer data.
//
//      NOTE! this function does NOT return Volume Units (VU)! nor could
//      it. so don't try to use these values for a Volume Indicator (VI)
//      instrument. it is a peak meter, nothing more.
//
//  Parameters:
//      PHARDWAREINSTANCE phwi
//         pointer to hardware instance structure
//
//      LPLONG palValue:
//
//  Return (BOOL):
//
//--------------------------------------------------------------------------;

BOOL FAR PASCAL HwPeakMeter
(
    PHARDWAREINSTANCE       phwi,
    LPBYTE                  lpBuffer,
    DWORD                   cbBuffer,
    LPLONG                  palValue
)
{
    LPBYTE           pbSamples;
    short            FAR *pnSamples;
    UINT             cSamples;
    int              nMaxL, nMinL, nMaxR, nMinR;
    int              nSample;

    BYTE             bMaxL, bMinL, bMaxR, bMinR;
    BYTE             bSample;

    pbSamples = lpBuffer ;

    bMaxL = bMaxR = 0x80;
    bMinL = bMinR = 0x80;

    pnSamples = (short FAR *)pbSamples;
    nMaxL = nMaxR = 0;
    nMinL = nMinR = 0;

    //
    //
    //
    switch (phwi -> dwWaveFormat)
    {
        case (FORMAT_8BIT):
            for (cSamples = (UINT) cbBuffer; cSamples; cSamples--)
            {
                bSample = *(pbSamples++);
                if (bSample > bMaxL)
                    bMaxL = bSample;
                else if (bSample < bMinL)
                    bMinL = bSample;
            };

            nMaxR = nMaxL = (int)(char)(bMaxL ^ (BYTE)0x80) << 8;
            nMinR = nMinL = (int)(char)(bMinL ^ (BYTE)0x80) << 8;
            break;

        case (FORMAT_8BIT | WOFORMAT_STEREO):
            for (cSamples = (UINT) (cbBuffer / 2); cSamples; cSamples--)
            {
                bSample = *(pbSamples++);
                if (bSample > bMaxL)
                    bMaxL = bSample;
                else if (bSample < bMinL)
                    bMinL = bSample;

                bSample = *(pbSamples++);
                if (bSample > bMaxR)
                    bMaxR = bSample;
                else if (bSample < bMinR)
                    bMinR = bSample;
            };

            nMaxL = (int)(char)(bMaxL ^ (BYTE)0x80) << 8;
            nMinL = (int)(char)(bMinL ^ (BYTE)0x80) << 8;

            nMaxR = (int)(char)(bMaxR ^ (BYTE)0x80) << 8;
            nMinR = (int)(char)(bMinR ^ (BYTE)0x80) << 8;
            break;


        case (FORMAT_16BIT):
            for (cSamples = (UINT) (cbBuffer / 2); cSamples; cSamples--)
            {
                nSample = *(pnSamples++);
                if (nSample > nMaxL)
                    nMaxL = nSample;
                else if (nSample < nMinL)
                    nMinL = nSample;
            }

            nMaxR = nMaxL;
            nMinR = nMinL;
            break;

        case (FORMAT_16BIT | WOFORMAT_STEREO):
            for (cSamples = (UINT) (cbBuffer / 4); cSamples; cSamples--)
            {
                nSample = *(pnSamples++);
                if (nSample > nMaxL)
                    nMaxL = nSample;
                else if (nSample < nMinL)
                    nMinL = nSample;

                nSample = *(pnSamples++);
                if (nSample > nMaxR)
                    nMaxR = nSample;
                else if (nSample < nMinR)
                    nMinR = nSample;
            }
            break;


        default:
            return (FALSE);
    }

    //
    //
    //
    palValue[0] = (abs(nMinL) > nMaxL) ? nMinL : nMaxL;
    palValue[1] = (abs(nMinR) > nMaxR) ? nMinR : nMaxR;

    return (TRUE);

} // HwPeakMeter()

//--------------------------------------------------------------------------
//
//  WORD HwGetCurrentDMAPosition
//
//  Description:
//      Gets the current DMA position from the DMA counter.
//
//  Parameters:
//      PHARDWAREINSTANCE phwi
//         pointer to hardware instance structure
//
//  Return (WORD):
//      0-64k based on actual DMA counter
//
//--------------------------------------------------------------------------

WORD FAR PASCAL HwGetCurrentDMAPosition
(
    PHARDWAREINSTANCE       phwi
)
{
   WORD   wCount = 0xFFFF ;

   if (!phwi -> bIntUsed || (!phwi -> fwodDMABusy && !phwi -> fwidDMABusy))
      return 0 ;

   wCount =
      MSSNDSYS_Get_DMA_Count( phwi -> dn,
                              (phwi -> bIntUsed == INT_WAVEIN) ?
                                 sfSS_GDC_ADC_Count : sfSS_GDC_DAC_Count ) ;
   D(
      char  szDebug[ 80 ] ;

      wsprintf( szDebug, "DMA counter pos: %x", wCount ) ;
      DPF( 1, szDebug ) ;
   ) ;

   return ( wCount ) ;

} // HwGetCurrentDMAPosition()

//--------------------------------------------------------------------------
//
//  VOID HwTimeDelay
//
//  Description:
//     Waits for the specified amount of time in ms.
//
//  Parameters:
//      WORD wTime
//         time to delay
//
//  Return (VOID):
//      Nothing
//
//
//--------------------------------------------------------------------------

VOID NEAR PASCAL HwTimeDelay
(
    WORD            wTime
)
{
   DWORD  dwEnd ;

   // Figure out at which time can restart.

   dwEnd = timeGetTime() + wTime ;

   while (timeGetTime() < dwEnd) ;

} // HwTimeDelay()

//--------------------------------------------------------------------------
//
//  BOOL CODEC_WaitForReady
//
//  Description:
//      Wait for CODEC to finish initializing.  This function will
//      timeout after XX miliseconds if the hardware is not functioning
//      properly.
//
//  Parameters:
//      PHARDWAREINSTANCE phwi
//         hardware instance to wait on...
//
//  Return (BOOL):
//      TRUE if successful, FALSE otherwise
//
//--------------------------------------------------------------------------

BOOL FAR PASCAL CODEC_WaitForReady
(
    PHARDWAREINSTANCE    phwi
)
{
    WORD   port ;
    DWORD  dwCount = 0x18000 ;

    if (phwi -> fBadBoard)
      return FALSE ;

    port = phwi -> wIOAddressCODEC + CODEC_ADDRESS ;

    do
    {
       if (0 == (READ_PORT_UCHAR( port ) & 0x80))
          return TRUE ;
    }
    while (dwCount--) ;

    DPF( 1, "CODEC_WaitForReady: TIMEOUT!!!" ) ;
    phwi -> fBadBoard = TRUE ;
    return FALSE ;

} // CODEC_WaitForReady()

//--------------------------------------------------------------------------
//
//  WORD CODEC_RegRead
//
//  Description:
//
//
//  Parameters:
//      PHARDWAREINSTANCE phwi
//
//      BYTE bReg
//
//  Return (WORD):
//
//--------------------------------------------------------------------------

WORD FAR PASCAL CODEC_RegRead
(
    PHARDWAREINSTANCE   phwi,
    BYTE                bReg
)
{
   WORD wRetVal = 0xFF00 ;

   if (!CODEC_WaitForReady( phwi ))
      return wRetVal ;

   WRITE_PORT_UCHAR( phwi -> wIOAddressCODEC + CODEC_ADDRESS,
                     phwi -> bModeChange | bReg ) ;

   wRetVal =
      (WORD) READ_PORT_UCHAR( phwi -> wIOAddressCODEC + CODEC_DATA ) ;

   return wRetVal ;

} // CODEC_RegRead()

//--------------------------------------------------------------------------
//
//  BOOL CODEC_RegWrite
//
//  Description:
//
//
//  Parameters:
//      PHARDWAREINSTANCE phwi
//
//      BYTE bReg
//
//      BYTE bValue
//
//  Return (BOOL):
//
//--------------------------------------------------------------------------

BOOL FAR PASCAL CODEC_RegWrite
(
    PHARDWAREINSTANCE   phwi,
    BYTE                bReg,
    BYTE                bValue
)
{
   if (!CODEC_WaitForReady( phwi ))
      return FALSE ;

   WRITE_PORT_UCHAR( phwi -> wIOAddressCODEC + CODEC_ADDRESS,
                     phwi -> bModeChange | bReg ) ;

   WRITE_PORT_UCHAR( phwi -> wIOAddressCODEC + CODEC_DATA,
                     bValue ) ;

   return TRUE ;

} // CODEC_RegWrite()

//--------------------------------------------------------------------------
//
//  VOID HwExtMute
//
//  Description:
//      This turns the external mute on/off, with the required
//      delays.
//
//  Parameters:
//      PHARDWAREINSTANCE phwi
//         pointer to hardware instance structure
//
//      WORD wOn
//         TRUE if on...
//
//  Return (VOID):
//
//
//--------------------------------------------------------------------------

VOID FAR PASCAL HwExtMute
(
    PHARDWAREINSTANCE       phwi,
    WORD                    wOn
)
{
   BYTE  bPrev ;

   if (phwi -> wHardwareOptions & DAK_COMPAQBA)
   {
      bPrev = READ_PORT_UCHAR( phwi -> wIOAddressAGA + 3 ) ;

      if (!wOn)
      {
         // Wait 10 ms before turning off the external mute - avoid
         // click with mute circuitry.

         HwTimeDelay( TIMEDELAY ) ;
      }
      WRITE_PORT_UCHAR( phwi -> wIOAddressAGA + 3,
                        (bPrev & 0xF0) | ((wOn) ? 0x0A : 0x08) ) ;
      HwTimeDelay( TIMEDELAY / 2 ) ;
   }
   else
   {
      bPrev = (BYTE) (CODEC_RegRead( phwi, REGISTER_DSP ) & ~(0xc0) ) ;

      if (wOn)
      {
         phwi -> bMute |= (BYTE) ((0x40));
         CODEC_RegWrite( phwi, REGISTER_DSP, (BYTE)(bPrev | phwi -> bMute) ) ;
      }
      else
      {
         // Wait 10 ms before turning off the external mute - avoid
         // click with mute circuitry.

         HwTimeDelay( TIMEDELAY ) ;
         phwi -> bMute &= (BYTE) (~(0x40)) ;
         CODEC_RegWrite( phwi, REGISTER_DSP, (BYTE)(bPrev | phwi -> bMute) ) ;
      }
   }

} // HwExtMute()

//--------------------------------------------------------------------------
//
//  VOID HwEnterMCE
//
//  Description:
//      Enters mode-change-enable state.  Mutes the CODEC and
//      then tells it to enter MCE.  HwLeaveMCE() must follow
//      as this function mutes all output (on the J-class).
//
//  Parameters:
//      PHARDWAREINSTANCE phwi
//         pointer to hardware instance structure
//
//      BOOL fAuto
//         set to true if auto-cal
//
//  Return (VOID):
//      Nothing.
//
//
//--------------------------------------------------------------------------

VOID FAR PASCAL HwEnterMCE
(
    PHARDWAREINSTANCE       phwi,
    BOOL                    fAuto
)
{
   BYTE    i;
   BYTE    bTemp;

   // Only perform muting when J-Class is used
   // or when autocalibrating

   if ((fAuto) || (phwi -> wCODECClass == CODEC_J_CLASS))
   {
      // Remember the old volume registers and then
      // mute each one of them...

      for (i = REGISTER_LEFTAUX1; i <= REGISTER_RIGHTOUTPUT; i++)
      {
              gbReg[i] = bTemp = (BYTE) CODEC_RegRead( phwi, i ) ;
              CODEC_RegWrite( phwi, i, (BYTE)(bTemp | 0x80) ) ;
      }

      // Make sure the record gain is not too high 'cause if
      // it is strange clipping problems result.

      for (i = REGISTER_LEFTINPUT; i <= REGISTER_RIGHTINPUT; i++)
      {
              gbReg[i] = bTemp = (BYTE) CODEC_RegRead( phwi, i ) ;
         if ((bTemp & 0x0f) > 13)
                      bTemp = (bTemp & (BYTE)0xf0) | (BYTE)13 ;
         CODEC_RegWrite( phwi, i, bTemp ) ;
           }
   }

   // Turn on MCE

   phwi -> bModeChange = AD1848_MODE_MCE ;

   // Make sure that we're not initializing

   CODEC_WaitForReady( phwi ) ;
   WRITEPORT( CODEC_ADDRESS, (BYTE) phwi -> bModeChange ) ;

} // HwEnterMCE()

//--------------------------------------------------------------------------
//
//  VOID HwLeaveMCE
//
//  Description:
//      Leaves MCE.  Tells the CODEC to leave MCE, waits for
//      calibration to stop and then un-mutes.
//
//  Parameters:
//      PHARDWAREINSTANCE phwi
//         pointer to hardware instance structure
//
//      BOOL fAuto
//         set to TRUE if auto-cal
//
//  Return (VOID):
//      Nothing
//
//
//--------------------------------------------------------------------------

VOID FAR PASCAL HwLeaveMCE
(
    PHARDWAREINSTANCE       phwi,
    BOOL                    fAuto
)
{
   DWORD  dwTime ;
   BYTE   i, bAuto ;

   phwi -> bModeChange &= ~AD1848_MODE_MCE ;

   // Make sure we're not initializing

   CODEC_WaitForReady( phwi ) ;

   // See if we're going to autocalibrate

   WRITEPORT( CODEC_ADDRESS,
               (BYTE) (phwi -> bModeChange | REGISTER_INTERFACE) ) ;
   bAuto = (BYTE) READPORT( CODEC_DATA ) ;

   // If we're going to autocalibrate then wait for it

   if (bAuto & 0x08)
   {
      WRITEPORT( CODEC_ADDRESS,
                  (BYTE) (phwi -> bModeChange | REGISTER_TEST) ) ;

      // NOTE: Hardware dependant timing loop

      // Wait for autocalibration to start and then stop.

      dwTime =100 ;
      while (( (~READPORT( CODEC_DATA )) & 0x20) && (dwTime--)) ;
      dwTime = phwi -> dwWaitLoop ;
      while (( (READPORT( CODEC_DATA )) & 0x20) && (dwTime--)) ;
   }

   // Delay for clicks

   HwTimeDelay( TIMEDELAY ) ;

   // Only perform un-muting when J-Class is used

   if ((fAuto) || (phwi -> wCODECClass == CODEC_J_CLASS))
   {
      // Restore the old volume registers...

      for (i = REGISTER_LEFTINPUT; i <= REGISTER_RIGHTOUTPUT; i++)
         CODEC_RegWrite( phwi, i, gbReg[i] ) ;
   }

} // HwLeaveMCE()

//--------------------------------------------------------------------------
//
//  BOOL HwDMAWaitForTxComplete
//
//  Description:
//      Waits for DMA controller to TC.
//
//  Parameters:
//      PHARDWAREINSTANCE phwi
//         pointer to hardware instance structure
//
//      BYTE bDMAChannel
//         DMA channel to watch
//
//  Return (BOOL):
//      TRUE if completed.
//
//
//--------------------------------------------------------------------------

BOOL NEAR PASCAL HwDMAWaitForTxComplete
(
    PHARDWAREINSTANCE       phwi,
    BYTE                    bDMAChannel
)
{
   DWORD   dwTime;
   BYTE    bDMARequestedMask;

   // NOTE: Another hardware dependant timing loop...

   bDMARequestedMask = (BYTE)((BYTE)0x10 << (bDMAChannel & (BYTE)0x03)) ;

   dwTime = phwi -> dwWaitLoop ;
   while ((READ_PORT_UCHAR(DMA8STAT) &  bDMARequestedMask) && (dwTime--)) ;

   if (!phwi -> fBadBoard && (dwTime == (DWORD) -1))
   {
      // We have a hardware error.  Note this and
      // make sure that our wait loops are set to 0 to
      // avoid delays in the future.

      phwi -> fBadBoard = TRUE ;
      phwi -> dwWaitLoop = 0 ;
      return ( FALSE ) ;
   }

   return ( TRUE ) ;

} // HwDMAWaitForTxComplete()

//--------------------------------------------------------------------------
//  
//  DWORD HwGetCurPos
//  
//  Description:
//  
//  
//  Parameters:
//      PHARDWAREINSTANCE phwi
//  
//      PWORD pwCurPos
//      
//      DWORD fdwOptions
//  
//  Return (DWORD):
//  
//  
//--------------------------------------------------------------------------

DWORD FAR PASCAL HwGetCurPos
(
    PHARDWAREINSTANCE   phwi,
    LPWORD              pwCurPos,
    DWORD               fdwOptions
)
{
   WORD         wCurrent ;
   DWORD        dwBuffers, dwFormat, dwBytes, dwDeltaTime ;
   NPWAVEALLOC  pClient ;

   dwFormat = phwi -> dwWaveFormat ;
   pClient = (NPWAVEALLOC)LOWORD( phwi -> dwCurCODECOwner ) ;

   if (((fdwOptions & GCP_OPTIONF_DAC) && phwi -> fwodDMABusy) ||
       ((fdwOptions & GCP_OPTIONF_ADC) && phwi -> fwidDMABusy))
   {

      if (phwi -> wFlags & SSI_FLAG_BUSTYPE_PCMCIA)
      {
         do
         {
            dwBuffers = pClient -> dwHalfDMABuffers ;

            // guess at the number of samples we have played (time is in milliseconds)

            if (phwi->fwodPaused)
               wCurrent = (WORD) ((phwi -> dwPauseTimeDelta * (DWORD) phwi -> wSamplesPerSec) / 1000L) ;
            else
            {
               dwDeltaTime = timeGetTime() - phwi -> dwInterruptTimeStamp ;
               wCurrent = (WORD) ((dwDeltaTime * (DWORD) phwi->wSamplesPerSec) / 1000L) ;
            }

            dwBytes = dwBuffers * phwi -> wDMAHalfBufSize + wCurrent ;
         }
         while (dwBuffers != pClient -> dwHalfDMABuffers) ;

         if (dwBytes < (dwBuffers * (DWORD) phwi->wDMAHalfBufSize))
            dwBytes += ((DWORD) phwi -> wDMAHalfBufSize << 1) ;

         if (dwBytes < phwi -> dwLastPosition)
            dwBytes = phwi -> dwLastPosition ;
         else
            phwi -> dwLastPosition = dwBytes ;

         if (pwCurPos)
            *pwCurPos = wCurrent ;
      }
      else
      {
         //
         // Current position calculation:
         //
         // +-------------+-------------+
         // |     A       |      B      |
         // +-------------+-------------+
         //
         // When DMA is active, interrupts fire at end of A and at the end
         // B,  but we've  only completed a full buffer at the end of B.
         // We know the current position in the full buffer by reading
         // the DMA counter.
         //
         // Current position is (interrupts @ B * buffer size) +
         // current DMA position.
         //

         //
         // The WHILE loop tests:
         //  (dwBuffers != pClient->dwHalfBuffers) :
         //    This is TRUE if an interrupt is serviced during the body of
         //    the loop.
         //  (dwBytes < (dwBuffers * (DWORD)phwi->wDMABufHalfSize)) :
         //    This is TRUE if there is still an interrupt pending.  The
         //    DMA pointer is somewhere in A but the interrupt from the end
         //    of B has not yet been serviced.
         //

         do
         {
            dwBuffers = pClient -> dwHalfDMABuffers ;
            wCurrent = HwGetCurrentDMAPosition( phwi ) + 1 ;
            wCurrent = phwi -> wDMAHalfBufSize * 2 - wCurrent ;

            dwBytes = (dwBuffers >> 1) * ((DWORD) phwi -> wDMAHalfBufSize << 1) ;
            dwBytes += wCurrent ;
         }
         while (dwBuffers != pClient -> dwHalfDMABuffers) ;

         if (dwBytes < (dwBuffers * (DWORD)phwi->wDMAHalfBufSize))
            dwBytes += ((DWORD) phwi -> wDMAHalfBufSize << 1) ;

         if (dwBytes < phwi -> dwLastPosition)
            dwBytes = phwi -> dwLastPosition ;
         else
            phwi -> dwLastPosition = dwBytes ;

         if (pwCurPos)
            *pwCurPos = wCurrent ;
      }
   }
   else
   {
      dwBytes =
         pClient -> dwHalfDMABuffers * (DWORD) phwi -> wDMAHalfBufSize ;

      if (dwBytes < phwi -> dwLastPosition)
         dwBytes = phwi -> dwLastPosition ;
      else
         phwi -> dwLastPosition = dwBytes ;

      if (pwCurPos)
         *pwCurPos = 0 ;
   }

   return dwBytes ;

} // HwGetCurPos()

//------------------------------------------------------------------------
//  VOID HwEndDSPAndDMA
//
//  Description:
//     HwEndDSPAndDMA - Shuts off the DSP and the DMA.
//
//  Parameters:
//      PHARDWAREINSTANCE phwi
//         pointer to hardware instance structure
//
//  Return Value (VOID):
//     Nothing.
//
//
//------------------------------------------------------------------------

VOID FAR PASCAL HwEndDSPAndDMA
(
    PHARDWAREINSTANCE       phwi
)
{
   BYTE  bAuto, bPrev ;

   DPF( 1, "HwEndDSPAndDMA" ) ;

   // Make sure we have something running

   if (!phwi -> fEnabled)
      return ;

   // If DMA is not busy, then we have already shut off

   if ((!phwi -> fwidDMABusy) && (!phwi -> fwodDMABusy))
      return ;

   // Enter MCE if coming out of a record

   bAuto = (BYTE) (phwi -> bIntUsed == INT_WAVEIN) ;

   // Only mute on J class CODECs

   if ((phwi -> wCODECClass == CODEC_J_CLASS) && (bAuto))
      HwExtMute( phwi, TRUE ) ;

   // Turn off the DAC outputs and minimize ADC gain

   CODEC_RegWrite( phwi, REGISTER_LEFTOUTPUT, 0x3f ) ;
   CODEC_RegWrite( phwi, REGISTER_RIGHTOUTPUT, 0x3f ) ;
   CODEC_RegWrite( phwi, REGISTER_LEFTINPUT, 0x00 ) ;
   CODEC_RegWrite( phwi, REGISTER_RIGHTINPUT, 0x00 ) ;

   if (bAuto)
      HwEnterMCE( phwi, TRUE ) ;

   // Tell the DSP to shut off

   // First, disable the interrupts

   bPrev = (BYTE) (CODEC_RegRead( phwi, REGISTER_DSP ) & 0xC0) ;
   CODEC_RegWrite( phwi, REGISTER_DSP, 
                   (BYTE) (phwi -> bMute | bPrev) ) ;

   // Clear any pending IRQs
   WRITEPORT( CODEC_STATUS, 0x00 ) ;

   // Kill DMA
   CODEC_RegWrite( phwi, REGISTER_INTERFACE, 0x00 ) ;

   if (bAuto)
      HwLeaveMCE( phwi, TRUE ) ;

   // Wait for the DMA to stop and then shut if off

   switch (phwi -> bIntUsed)
   {
      case INT_WAVEOUT:
         if (0 == (phwi -> wFlags & SSI_FLAG_BUSTYPE_PCMCIA))
         {
            HwDMAWaitForTxComplete( phwi, phwi -> bPlaybackDMA ) ;
            WRITE_PORT_UCHAR( phwi -> portPlaybackDMASMR,
                              phwi -> portPlaybackDMADisable ) ;
         }

         // If 11 kHz playback then enter/leave MCE to reduce the noise
         // (J-class CODECs ONLY!)

         if ((phwi -> wCODECClass == CODEC_J_CLASS) && phwi -> fDo11kHzMCE &&
             (phwi -> wSamplesPerSec <= 17000))
         {
            HwExtMute( phwi, TRUE ) ;
            HwEnterMCE( phwi, bAuto ) ;
	    CODEC_RegWrite( phwi, REGISTER_DATAFORMAT, 0x0C ) ;
	    phwi -> wOldFormat = 0x0C ;
            HwLeaveMCE( phwi, bAuto ) ;
            HwExtMute( phwi, FALSE ) ;
         }
         break ;

      case INT_WAVEIN:
         if (0 == (phwi -> wFlags & SSI_FLAG_BUSTYPE_PCMCIA))
         {
            HwDMAWaitForTxComplete( phwi, phwi -> bCaptureDMA ) ;
            WRITE_PORT_UCHAR( phwi -> portCaptureDMASMR,
                              phwi -> portCaptureDMADisable );
         }
         break;
   }

   if ((phwi -> wCODECClass == CODEC_J_CLASS) && (bAuto))
      HwExtMute( phwi, FALSE ) ;

   phwi -> fwodDMABusy = 0 ;
   phwi -> fwidDMABusy = 0 ;

} // end of HwEndDSPAndDMA()

//--------------------------------------------------------------------------
//
//  VOID wodInterrupt
//
//  Description:
//      Services a wave-out interrupt from the CODEC.
//
//  Parameters:
//      PHARDWAREINSTANCE phwi
//         pointer to hardware instance structure
//
//  Return (VOID):
//      None.
//
//
//--------------------------------------------------------------------------

VOID FAR PASCAL wodInterrupt
(
    PHARDWAREINSTANCE       phwi
)
{
   BYTE         bAddress ;
   WORD         cbFill, wCurPos ;
   NPWAVEALLOC  pClient ;

   // Make sure that we are ready for this interrupt.

   if (!phwi -> fwodDMABusy)
      return ;

   // If we set lpSilenceStart, it means we're about to start DMA'ing
   // an incompletely filled buffer.  Reset lpSilenceStart to 0 so
   // that new data arriving now has to wait until the next time
   // around.

   phwi -> lpSilenceStart = NULL ;

   // Safety precaution...

   bAddress = (BYTE) READPORT( CODEC_ADDRESS ) ;

   DPF( 2, "o" ) ;

   //
   // This fiddling around with last DMA position resolves the problems
   // with drift between the sample counter on the AD1848 and the
   // DMA counter.
   //

   pClient = (NPWAVEALLOC)LOWORD( phwi -> dwCurCODECOwner ) ;
   pClient -> dwHalfDMABuffers++ ;

   // Are we running on a pcmcia sound card?

   if (phwi->wFlags & SSI_FLAG_BUSTYPE_PCMCIA)
   {
      // get a time stamp

      phwi -> dwInterruptTimeStamp = timeGetTime() ;
   }

   if (phwi -> fDMADone)
   {
      // DMA is all finished, tidy up

      DPF( 2, "NoNextBuf" ) ;

      HwEndDSPAndDMA( phwi ) ;
      wodPostAllHeaders( phwi ) ;

      return ;
   }

   if (phwi -> wFlags & SSI_FLAG_BUSTYPE_PCMCIA)
   {
      WORD  wOffset ;

      phwi -> wLastDMAPos ^= (DMA_BUFFER_PING ^ DMA_BUFFER_PONG) ;
      wOffset =
         (phwi -> wLastDMAPos == DMA_BUFFER_PONG) ? phwi -> wDMAHalfBufSize : 0 ;
      if (!wodDMALoadBuffer( phwi, phwi -> lpDMABuffer + wOffset,
                             phwi -> wDMAHalfBufSize, TRUE ))
         phwi -> wLastDMAPos = DMA_BUFFER_DONE ;
   }
   else
   {
      wCurPos = HwGetCurrentDMAPosition( phwi ) + 1 ;
      wCurPos = phwi -> wDMAHalfBufSize * 2 - wCurPos ;
      wCurPos &= ~(phwi -> wBytesPerSample - 1) ;

      // If current position is greater than last position, straight
      // fill else we've gotta wrap.

      if (wCurPos > phwi -> wLastDMAPos)
      {
         if (0 != (cbFill = wCurPos - phwi -> wLastDMAPos))
         {
            if (!wodDMALoadBuffer( phwi,
                                   phwi -> lpDMABuffer + phwi -> wLastDMAPos,
                                   cbFill, TRUE ))
               phwi -> fDMADone = 
                  (((pClient -> dwHalfDMABuffers + 1) * 
                     (DWORD) phwi -> wDMAHalfBufSize) >= 
                         pClient -> dwByteCount) ;
         }
      }
      else
      {
         cbFill = (phwi -> wDMAHalfBufSize * 2) - phwi -> wLastDMAPos ;

         if (cbFill)
         {
            if (!wodDMALoadBuffer( phwi,
                                   phwi -> lpDMABuffer + phwi -> wLastDMAPos,
                                   cbFill, TRUE ))
            {
               // also need to fill the remainder on the wrap...

               waveFillSilence( phwi -> dwWaveFormat,
                                phwi -> lpDMABuffer, wCurPos, 0, 0 ) ;
               phwi -> fDMADone = 
                  (((pClient -> dwHalfDMABuffers + 1) * 
                     (DWORD) phwi -> wDMAHalfBufSize) >= 
                         pClient -> dwByteCount) ;
            }
            else
               wodDMALoadBuffer( phwi, phwi -> lpDMABuffer, wCurPos, FALSE ) ;
         }
      }
      phwi -> wLastDMAPos = wCurPos ;
   }

   WRITEPORT( CODEC_ADDRESS, bAddress ) ;

} // wodInterrupt()

//------------------------------------------------------------------------
//  VOID FAR PASCAL widInterrupt( VOID )
//
//  Description:
//     Services and interrupt from the CODEC.
//
//  Parameters:
//      PHARDWAREINSTANCE phwi
//         pointer to hardware instance structure
//
//  Return Value:
//     None.
//
//
//------------------------------------------------------------------------

VOID FAR PASCAL widInterrupt
(
    PHARDWAREINSTANCE       phwi
)
{
   BYTE   bAddress ;
   WORD   cbFill, wCurPos ;

   // Remember the register of the CODEC. This is a safeguard
   // in case we get an interrupt while setting volume levels.

   bAddress = (BYTE) READPORT( CODEC_ADDRESS ) ;

   DPF( 2, "i" ) ;

   //
   // Bump the "half DMA buffers" completed counter
   //

   ((NPWAVEALLOC)LOWORD( phwi -> dwCurCODECOwner )) -> dwHalfDMABuffers++ ;

   // Which buffer to read from?

   if (phwi -> wFlags & SSI_FLAG_BUSTYPE_PCMCIA)
   {
      WORD  wOffset ;

      phwi -> wLastDMAPos ^= (DMA_BUFFER_PING ^ DMA_BUFFER_PONG) ;
      wOffset =
         (phwi -> wLastDMAPos == DMA_BUFFER_PONG) ? phwi -> wDMAHalfBufSize : 0 ;

      widFillBuffer( phwi, phwi -> lpDMABuffer + wOffset,
                     phwi -> wDMAHalfBufSize ) ;
   }
   else
   {
      // Which buffer to read from?

      wCurPos = HwGetCurrentDMAPosition( phwi ) ;
      if (wCurPos == 0xFFFF)
         wCurPos = 0 ;

      wCurPos = phwi -> wDMAHalfBufSize * 2 - wCurPos ;
      wCurPos &= ~(phwi -> wBytesPerSample - 1) ;

      // If current position is greater than last position, straight
      // fill else we've gotta wrap.

      if (wCurPos > phwi -> wLastDMAPos)
      {
         cbFill = wCurPos - phwi -> wLastDMAPos ;
         if (cbFill)
            widFillBuffer( phwi, phwi -> lpDMABuffer + phwi -> wLastDMAPos,
                           cbFill ) ;
      }
      else
      {
         cbFill = (phwi -> wDMAHalfBufSize * 2) - phwi -> wLastDMAPos ;

         if (cbFill)
            widFillBuffer( phwi, phwi -> lpDMABuffer + phwi -> wLastDMAPos,
                           cbFill ) ;
         if (wCurPos)
            widFillBuffer( phwi, phwi -> lpDMABuffer, wCurPos )  ;
      }
      phwi -> wLastDMAPos = wCurPos ;
   }

   // Write out the old address.

   WRITEPORT( CODEC_ADDRESS, bAddress ) ;

} // end of widInterrupt()
