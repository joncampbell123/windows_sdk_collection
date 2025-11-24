//---------------------------------------------------------------------------
//
//  Module:   wavefix.c
//
//  Purpose:
//     Fixed wave routines.
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
#include "mssndsys.h"
#include "driver.h"

// Driver specific bit used to mark a wave header as being done with
// all DMA activity--so we can post the WOM_DONE message at the correct
// time.  See dmaLoadBuffer in wavefix.c for more information.

#define WHDR_REALLYDONE 0x80000000l // internal driver flag for wave headers

//------------------------------------------------------------------------
//  VOID FAR PASCAL waveCallBack( NPWAVEALLOC pWave, WORD msg, DWORD dw1 )
//
//  Description:
//     This calls DriverCallback for a WAVEHDR.
//
//  Parameters:
//     NPWAVEALLOC pWave
//        pointer to wave device
//
//     WORD msg
//        the message
//
//     DWORD dw1
//        DWORD parameter of DWORD, dw2 is always set to 0.
//
//  Return Value:
//     Nothing.
//
//
//------------------------------------------------------------------------

VOID FAR PASCAL waveCallBack
(
    NPWAVEALLOC     pWave,
    WORD            msg,
    DWORD           dw1
)
{
   // DPF( 1, "waveCallBack" ) ;

   // Invoke the callback function, if it exists.  dwFlags contains
   // wave driver specific flags in the LOWORD and generic driver
   // flags in the HIWORD.

   if (HIWORD(pWave->dwFlags))
      DriverCallback(
         pWave->dwCallback,                     // user's callback DWORD
         HIWORD(pWave->dwFlags),                // callback flags
         pWave->hWave,                          // handle to the wave device
         msg,                                   // the message
         pWave->dwInstance,                     // user's instance data
         dw1,                                   // first DWORD
         0L ) ;                                 // second DWORD

} // end of waveCallBack()

//------------------------------------------------------------------------
//  VOID wodBlockFinished
//
//  Description:
//     This function sets the done bit and invokes the callback
//     function if there is one.
//
//  Parameters:
//     LPWAVEHDR lpHdr
//        Far pointer to the header.
//
//  Return Value:
//     Nothing
//
//
//------------------------------------------------------------------------

VOID FAR PASCAL wodBlockFinished
(
    LPWAVEHDR       lpHdr
)
{
   NPWAVEALLOC  pWav ;

   DPF( 3, "blkfin" ) ;

   // Set the 'done' bit...

   lpHdr->dwFlags |= WHDR_DONE ;

   // We are giving the block back to the application.  The header
   // is no longer in our queue, so we reset the WHDR_INQUEUE bit.
   // Also, we clear our driver specific bit and cauterize the
   // lpNext pointer.

   lpHdr->dwFlags &= ~(WHDR_INQUEUE | WHDR_REALLYDONE) ;
   lpHdr -> lpNext = NULL ;

   pWav = (NPWAVEALLOC)(lpHdr->reserved) ;

   // Invoke the callback function..

   waveCallBack( pWav, WOM_DONE, (DWORD)lpHdr ) ;

} // end of wodBlockFinished()

//--------------------------------------------------------------------------
//  
//  VOID waveFillSilence
//  
//  Description:
//      Fills the given buffer with silence.
//  
//  Parameters:
//      DWORD dwFormat
//         currnet wave format
//
//      LPSTR lpBuf
//         buffer to fill with silence
//
//      WORD wSize
//         amount of silence
//
//      DWORD dwFlags
//         flags
//
//      DWORD dwParam
//         dword parameter
//  
//  Return (VOID):
//      Nothing.
//  
//--------------------------------------------------------------------------

VOID FAR PASCAL waveFillSilence
(
    DWORD           dwFormat,
    LPSTR           lpBuf,
    WORD            wSize,
    DWORD           dwFlags,
    DWORD           dwParam
)
{
   if (dwFlags & WFS_FLAG_WAVJAMMER)
   {
      // LOWORD( dwParam ) is SRAM data register

      switch (dwFormat)
      {
         case (FORMAT_MULAW):
         case (FORMAT_MULAW | WOFORMAT_STEREO):
            NMCMemFillSilent( wSize, 0x7f7f, LOWORD( dwParam ) ) ;
            break ;

         case (FORMAT_ALAW):
         case (FORMAT_ALAW | WOFORMAT_STEREO):
            NMCMemFillSilent( wSize, 0xd5d5, LOWORD( dwParam ) ) ;
            break ;

         case (FORMAT_8BIT):
         case (FORMAT_8BIT | WOFORMAT_STEREO):
            NMCMemFillSilent( wSize, 0x8080, LOWORD( dwParam ) ) ;
            break ;

         case (FORMAT_16BIT):
         case (FORMAT_16BIT | WOFORMAT_STEREO):
            NMCMemFillSilent( wSize, 0x0000, LOWORD( dwParam ) ) ;
            break ;

         default:
            NMCMemFillSilent( wSize, 0x8080, LOWORD( dwParam ) ) ;
            break ;
      }
   }
   else
   {
      switch (dwFormat)
      {
         case (FORMAT_MULAW):
         case (FORMAT_MULAW | WOFORMAT_STEREO):
            MemFillSilent( lpBuf, wSize, 0x7f7f ) ;
            break ;

         case (FORMAT_ALAW):
         case (FORMAT_ALAW | WOFORMAT_STEREO):
            MemFillSilent( lpBuf, wSize, 0xd5d5 ) ;
            break ;

         case (FORMAT_8BIT):
         case (FORMAT_8BIT | WOFORMAT_STEREO):
            MemFillSilent( lpBuf, wSize, 0x8080 ) ;
            break ;

         case (FORMAT_16BIT):
         case (FORMAT_16BIT | WOFORMAT_STEREO):
            MemFillSilent( lpBuf, wSize, 0x0000 ) ;
            break ;

         default:
            MemFillSilent( lpBuf, wSize, 0x8080 ) ;
            break ;

      }
   }

} // waveFillSilence()

//--------------------------------------------------------------------------
//  
//  VOID wodPostAllHeaders
//  
//  Description:
//     Finish all dead headers.
//  
//  Parameters:
//     PHARDWAREINSTANCE phwi
//        pointer to hardware instance structure
//
//  Return (VOID):
//      Nothing.
//  
//  
//--------------------------------------------------------------------------

VOID FAR PASCAL wodPostAllHeaders
(
    PHARDWAREINSTANCE       phwi
)
{
   LPWAVEHDR  lpNuke ;

   DPF( 2, "post ALL deadheads" ) ;

   // free the lpwodDeadHeads...

   while (lpNuke = phwi -> lpwodDeadHeads)
   {
      phwi -> lpwodDeadHeads = (phwi -> lpwodDeadHeads) -> lpNext ;
      wodBlockFinished( lpNuke ) ;
   }

} // wodPostAllHeaders()

//--------------------------------------------------------------------------
//  
//  VOID wodPostDoneHeaders
//  
//  Description:
//     Posts done headers from the lpDeadHead list.
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

VOID NEAR PASCAL wodPostDoneHeaders
(
    PHARDWAREINSTANCE       phwi
)
{
   LPWAVEHDR  lpNuke, lpPrev ;

   DPF( 2, "PostDeadHeaders" ) ;

   lpPrev = NULL ;
   for (lpNuke = phwi -> lpwodDeadHeads; lpNuke; lpNuke = lpNuke -> lpNext)
   {
      if (lpNuke -> dwFlags & WHDR_REALLYDONE)
      {
         if (lpPrev)
            lpPrev->lpNext = NULL ;
         else
            phwi -> lpwodDeadHeads = NULL ;

         // from lpNuke down, we need to wodBlockFinished()...

         while (lpPrev = lpNuke)
         {
            lpNuke = lpNuke -> lpNext ;
            wodBlockFinished( lpPrev ) ;
         }

         // break out of the for() loop
         break ;
      }

      lpNuke -> dwFlags |= WHDR_REALLYDONE ;
      lpPrev = lpNuke ;
   }

} // wodPostDoneHeaders()

//--------------------------------------------------------------------------
//  
//  WORD wodDMALoadBuffer
//  
//  Description:
//      Loads a DMA buffer from the data queue.
//  
//  Parameters:
//      PHARDWAREINSTANCE phwi
//         pointer to hardware instance structure
//
//      LPSTR lpBuffer
//         far pointer to the DMA buffer.
//  
//      WORD wBufSize
//         size of the buffer in bytes
//
//      BOOL fPostEm
//         TRUE if we post dead headers
//  
//  Return (WORD):
//      # bytes transferred, a value of zero indicates that there
//      was no more data in the output queue
//
//  Comments:
//      This routine is called once when DMA is started and then again at
//      interrupt time when a DMA block is complete.  It will in turn call a
//      callback funtion to the app if it empties a request queue block and a
//      callback function is defined.  Note that interrupts are disabled
//      throughout the process, however, this may change in the future.
//
//  
//--------------------------------------------------------------------------

WORD FAR PASCAL wodDMALoadBuffer
(
    PHARDWAREINSTANCE       phwi,
    LPSTR                   lpBuffer,
    WORD                    wBufSize,
    BOOL                    fPostEm
)
{
   WORD       wBytesTransferred ; // how many bytes transferred to
                                  //    DMA buffer
   WORD       wToGo ;             // min (buf space left, bytes
                                  //    left in data block)
   LPWAVEHDR  lpNuke ;            // wavehdr to free
   LPWAVEHDR  lpQ ;               // for traversing queue
   int        i ;

   if (fPostEm && phwi -> lpwodDeadHeads)
      wodPostDoneHeaders( phwi ) ;

   lpQ = phwi -> lpwodQueue ;
   wBytesTransferred = 0 ;

   if (phwi -> wFlags & SSI_FLAG_BUSTYPE_PCMCIA)
      WRITEPORT( 0x0a, 0x04 ) ;  // Clear HOST ptr.

   if (!lpQ)
      goto Fill_Silence ;

   while (wBytesTransferred < wBufSize)
   {
      // We break if we have no data left or we
      // complete the request.

      if (phwi -> hpwodCurData == NULL)
      {
         // First time in for this queue
         
         DPF( 3, "firstq" ) ;

         if (phwi -> fwodDMABusy)
            DPF( 3, "dmabusy" ) ;

         phwi -> hpwodCurData = lpQ -> lpData ;
         phwi -> dwwodCurCount = lpQ -> dwBufferLength ;

         // Check if this is the start of a loop
         
         if (lpQ -> dwFlags & WHDR_BEGINLOOP)
         {
            DPF( 4, "BeginLoop1" ) ;

            phwi -> lpwodLoopStart = lpQ ;
            phwi -> dwwodLoopCount = lpQ -> dwLoops ;
         }
      }

      // Break the loop?

      if (phwi -> fwodBreakLoop)
      {
         phwi -> dwwodLoopCount = 0L ; 
         phwi -> fwodBreakLoop = FALSE ;
      }

      // If we are looping and no more loops need executing, then
      // copy nothing... we still need to go through the motions
      // to keep everything updated correctly.

      if (phwi -> lpwodLoopStart && (phwi -> dwwodLoopCount == 0))
         phwi -> dwwodCurCount = 0 ;

      // Only write out wave data if we're not skipping
      // the current loop entry

      if (phwi -> dwwodCurCount)
      {
         // hpwodCurData points to some chunk we can grab

         wToGo = (WORD)min( phwi -> dwwodCurCount,
                            (DWORD)(wBufSize - wBytesTransferred) ) ;

         // This is obviously off by a half DMA buffer... but
         // it's currently the easiest solution for PCMCIA.

         HwPeakMeter( phwi, phwi -> hpwodCurData, wToGo, 
                      &phwi -> dwwodLeftPeak ) ;

         // fill the buffer

         if (phwi -> wFlags & SSI_FLAG_BUSTYPE_PCMCIA)
            phwi -> hpwodCurData = 
               NMCMemCopySrc( phwi -> hpwodCurData, wToGo, 
                              phwi -> wSRAMBase ) ;
         else
            phwi -> hpwodCurData = 
               MemCopySrc( lpBuffer, phwi -> hpwodCurData, wToGo ) ;

         lpBuffer += wToGo ;
         phwi -> dwwodCurCount -= wToGo ;
         wBytesTransferred += wToGo ;
         ((NPWAVEALLOC)LOWORD( lpQ -> reserved )) -> dwByteCount += wToGo ;
      }

      // see if that emptied the current buffer

      if (phwi -> dwwodCurCount == 0)
      {
         DPF( 4, "blockfin" ) ;

         if (lpQ -> dwFlags & WHDR_ENDLOOP)
         {
            DPF( 4, "EndOfLoop" ) ;

            if (phwi -> dwwodLoopCount == 0)
            {
               DPF( 4, "KillLoop" ) ;

               lpNuke = phwi -> lpwodLoopStart ;
               lpQ = lpQ -> lpNext ;

               while (lpNuke != lpQ)
               {
                  // Free up loop blocks

                  LPWAVEHDR  lpKillMe ;

                  // move the "almost done" blocks to death row

                  lpKillMe = lpNuke ;
                  lpNuke = lpNuke -> lpNext ;
                  lpKillMe -> lpNext = phwi -> lpwodDeadHeads ;
                  phwi -> lpwodDeadHeads = lpKillMe ;
               }
               phwi -> lpwodLoopStart = NULL ;
            }
            else
            {
               DPF( 3, "loop--" ) ;
               phwi -> dwwodLoopCount-- ;

               // back to the beginning of the loop...

               lpQ = phwi -> lpwodQueue = phwi -> lpwodLoopStart ;
            }
         }
         else
         {
            DPF( 4, "NotEndOfLoop" ) ;

            // Move the "almost done" block into the lpwodDeadHeads list

            lpNuke = lpQ ;
            lpQ = lpQ -> lpNext ;
            if (phwi -> lpwodLoopStart == NULL)
            {
               lpNuke -> lpNext = phwi -> lpwodDeadHeads ;
               phwi -> lpwodDeadHeads = lpNuke ;
            }
         }

         if (lpQ == NULL)
         {
            // End of the list

            DPF( 3, "endofq" ) ;
            phwi -> hpwodCurData = NULL ;
            phwi -> dwwodCurCount = 0L ;

            // Break from the while loop.
            break ;
         }
         else
         {
            DPF( 3, "NextItem" ) ;

            phwi -> hpwodCurData = lpQ -> lpData ;
            phwi -> dwwodCurCount = lpQ -> dwBufferLength ;
            if (phwi -> lpwodLoopStart == NULL)
            {
               // Check if this is the start of a loop

               if (lpQ -> dwFlags & WHDR_BEGINLOOP)
               {
                  DPF( 4, "BeginLoop" ) ;
                  phwi -> lpwodLoopStart = lpQ ;
                  phwi -> dwwodLoopCount = lpQ -> dwLoops ;
               }
            }
         }
      }
   }

   if (!(phwi -> lpwodQueue = lpQ) && phwi -> lpwodLoopStart)
      phwi -> lpwodQueue = phwi -> lpwodLoopStart ;

   if (wBytesTransferred == 0)
   {
      // We still have an empty buffer, free up the hostage
      // so that we can quit the next time through.

      if (!phwi -> fwodDMABusy)
      {
         DPF( 2, "zerolenfree" ) ;
         wodPostAllHeaders( phwi ) ;
      }
   }

   // Pad out with silence

Fill_Silence:

   if (i = wBufSize - wBytesTransferred)
   {
      phwi -> lpSilenceStart = lpBuffer ;
      phwi -> wSilenceSize = i ;
      if (phwi -> wFlags & SSI_FLAG_BUSTYPE_PCMCIA)
      {
         waveFillSilence( phwi -> dwWaveFormat, 
                          NULL,
                          phwi -> wSilenceSize, 
                          WFS_FLAG_WAVJAMMER, 
                          phwi -> wSRAMBase ) ;
      }
      else
         waveFillSilence( phwi -> dwWaveFormat, 
                          phwi -> lpSilenceStart, 
                          phwi -> wSilenceSize, 0, 0 ) ;
   }
   else
      phwi -> lpSilenceStart = NULL ;

   // Buffer filled

   return wBytesTransferred ;

} // wodDMALoadBuffer()

//------------------------------------------------------------------------
//  VOID FAR PASCAL widBlockFinished( LPWAVEHDR lpHdr )
//
//  Description:
//     Sets the done bit and invokes the callback function if
//     one is specified.
//
//  Parameters:
//     LPWAVEHDR lpHdr
//        pointer to wave header
//
//  Return Value:
//     Nothing.
//
//
//------------------------------------------------------------------------

VOID FAR PASCAL widBlockFinished
(
    LPWAVEHDR       lpHdr
)
{
   NPWAVEALLOC  pClient ;
 
   DPF( 1, "widBlockFinished" ) ;


   // if it's an empty block, set the 'done' bit and length field

   if (!(lpHdr->dwFlags & WHDR_DONE))
   {
      lpHdr->dwFlags |= WHDR_DONE ;
      lpHdr->dwFlags &= ~WHDR_INQUEUE ;
      lpHdr->dwBytesRecorded = 0 ;
   }

   pClient = (NPWAVEALLOC)(lpHdr->reserved) ;

   // call client's callback

   waveCallBack( pClient, WIM_DATA, (DWORD) lpHdr ) ;

} // end of widBlockFinished()

//--------------------------------------------------------------------------
//  
//  WORD widFillBuffer
//  
//  Description:
//      This routine is called once when DMA is started and then again
//      at interrupt time when a DMA block is complete.  It will call
//      a callback function of the client if it fills a request queue
//      block and a callback function is defined.  Note that interrupts
//      are disable throughout the process.
//  
//  Parameters:
//      PHARDWAREINSTANCE phwi
//         pointer to hardware instance structure
//
//      LPSTR lpBuffer
//         far pointer to the DMA buffer
//  
//      WORD wBufSize
//         size of the buffer in bytes.
//  
//  Return (WORD):
//      The return value is the number of bytes transferred.  A value
//      of zero indicates that there is no more data in the input queue.
//
//  
//--------------------------------------------------------------------------

WORD NEAR PASCAL widFillBuffer
(
    PHARDWAREINSTANCE       phwi,
    LPSTR                   lpBuffer,
    WORD                    wBufSize
)
{
   WORD      wBytesTransferred ; // how many bytes transferred to DMA buffer
   WORD      wToGo ;             // min (buf space left, bytes left
                                 //      in data block)
   LPWAVEHDR lpNext ;            // next WAVEHDR in queue
   WORD      wCount ;

   if (!phwi -> lpwidQueue)
      return 0 ; // no queue at all

   if (phwi -> wFlags & SSI_FLAG_BUSTYPE_PCMCIA)
      WRITEPORT( 0x0A, 0x04 ) ; // Clear HOST ptr.

   wBytesTransferred = 0 ;
   while (wBytesTransferred < wBufSize)
   {
      // break if we have no data left or we complete the request

      if (phwi -> hpwidCurData == NULL)
      {
         // first time in for this queue

         phwi -> hpwidCurData = phwi -> lpwidQueue -> lpData ;
         phwi -> dwwidCurCount = phwi -> lpwidQueue -> dwBufferLength ;
         phwi -> lpwidQueue -> dwBytesRecorded = 0 ;
      }

      // Fill the buffer, phwi -> hpwidCurData points to an
      // empty location in a buffer

      wToGo = (WORD) min( phwi -> dwwidCurCount,
                          (DWORD)(wBufSize - wBytesTransferred) ) ;

      if (wToGo)
      {
         wCount = wToGo ;

         if (phwi -> wFlags & SSI_FLAG_BUSTYPE_PCMCIA)
         {
            HPSTR hp ;

            ASCRITENTER ;

            hp =
               NMCMemCopyDest( phwi -> hpwidCurData, wCount, 
                               phwi -> wSRAMBase ) ;
            HwPeakMeter( phwi, phwi -> hpwidCurData, wToGo, 
                         &phwi -> dwwidLeftPeak ) ;
            phwi -> hpwidCurData = hp ;

            ASCRITLEAVE ;

         }
         else
         {
            HwPeakMeter( phwi, lpBuffer, wToGo, 
                        &phwi -> dwwidLeftPeak ) ;
            phwi -> hpwidCurData = 
               MemCopyDest( phwi -> hpwidCurData, lpBuffer, wCount ) ;
         }

         lpBuffer += wToGo ;
         phwi -> dwwidCurCount -= wToGo ;
         wBytesTransferred += wToGo ;
         phwi -> lpwidQueue -> dwBytesRecorded += wToGo ;
      }

      // see if that filled the current buffer

      if (phwi -> dwwidCurCount == 0)
      {
         DPF( 2, "loopfin" ) ;

         // move on to the next block in the queue

         lpNext = phwi -> lpwidQueue -> lpNext ;
         phwi -> lpwidQueue->dwFlags |= WHDR_DONE ;
         phwi -> lpwidQueue->dwFlags &= ~WHDR_INQUEUE ;

         // release the data block

         widBlockFinished( phwi -> lpwidQueue ) ;
         phwi -> lpwidQueue = lpNext ;

         if (phwi -> lpwidQueue == NULL)
         {
            // end of the list

            DPF( 3,"endofq") ;

            phwi -> hpwidCurData = NULL ;
            phwi -> dwwidCurCount = 0L ;
            break ; // from the while loop (return wBytesTransferred)
         }
         else
         {
            phwi -> hpwidCurData = phwi -> lpwidQueue -> lpData ;
            phwi -> dwwidCurCount = phwi -> lpwidQueue -> dwBufferLength ;
            phwi -> lpwidQueue->dwBytesRecorded = 0 ;
         }
      }
   }

   return ( wBytesTransferred ) ;

} // widFillBuffer()

//---------------------------------------------------------------------------
//  End of File: wavefix.c
//---------------------------------------------------------------------------
