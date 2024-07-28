/****************************************************************************
 *
 *   cap.c
 * 
 *   Main video capture module. Main capture ISR.
 *
 *   Microsoft Video for Windows Sample Capture Driver
 *   Chips & Technologies 9001 based frame grabbers.
 *
 ***************************************************************************/
/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1992, 1993  Microsoft Corporation.  All Rights Reserved.
 * 
 **************************************************************************/

        
#include <windows.h>
#include <mmsystem.h>
#include <msvideo.h>
#include <msviddrv.h>
#include <stdlib.h>
#include "ct.h"
#include "debug.h"

#ifndef DriverCallback
BOOL WINAPI DriverCallback(DWORD dwCallback, UINT uFlags,
    HANDLE hDevice, UINT uMessage, DWORD dwUser, DWORD dwParam1, DWORD dwParam2);
#endif

// The following are constants which define, in microseconds,
// the time between vertical retrace interrupts
// for either frames or fields.

#if FRAME_INTERRUPT
#define NTSCMICROSECPERTICK       33367L // = 1/59.94   = 33 mS per Frame
#define  PALMICROSECPERTICK       40000L // = 1/50.00   = 40 mS per Frame
#else
#define NTSCMICROSECPERTICK       16683L // 1/(2*59.94) = 16 mS per Field
#define  PALMICROSECPERTICK       20000L // 1/(2*50.00) = 20 mS per Field
#endif

//
//  we use the reserved fields of a VIDEOHDR for linked list pointers
//
#define NEXT(p)     (LPVIDEOHDR)(p->dwReserved[0])

/* Local to this module */
LPVIDEOHDR	lpVHdrFirst;
LPVIDEOHDR      lpVHdrLast;
BOOL		fVideoOpen = FALSE;
DWORD		dwTimeStart;
DWORD		dwTimeStream;
DWORD		dwMicroSecPerFrame;
DWORD		dwMicroSecPerTick; // Accounts for 59.95 video frames per sec.
DWORD		dwNextFrameNum;
DWORD		dwLastVideoClock;
DWORD           dwFramesSkipped = 0;
VIDEO_STREAM_INIT_PARMS  CaptureStreamParms;

/* 
 * Return the number of frames skipped to date.  This count will be
 *      inaccurate if the frame rate requested is over 15fps.
 * This function implements the DVM_STREAM_GETERROR message.
 */
DWORD FAR PASCAL InStreamError(LPDWORD lpdwErrorType, LPDWORD lpdwFramesSkipped)
{
    if (lpdwErrorType)
        *lpdwErrorType = (dwFramesSkipped ? DV_ERR_NO_BUFFERS : DV_ERR_OK);
    if (lpdwFramesSkipped)
        *lpdwFramesSkipped = dwFramesSkipped;
    dwFramesSkipped = 0;
    return( DV_ERR_OK );
}

/* 
 * Return the current stream time from the start of capture based
 *      on the number of vsync interrupts.
 * This function implements the DVM_STREAM_GETPOSITION message.
 */
DWORD FAR PASCAL InStreamGetPos( LPMMTIME lpMMTime, DWORD dwSize)
{
    if (dwSize != sizeof (MMTIME)) 
        return DV_ERR_NOTSUPPORTED;
    lpMMTime->wType = TIME_MS;
    lpMMTime->u.ms = dwTimeStream;
    return( DV_ERR_OK );
}


/****************************************************************************
 *  videoCallback()  This calls DriverCallback for the input stream
 *
 *  msg		 The message to send.
 *
 *  dw1		Message-dependent parameter.
 *
 * There is no return value.
 ***************************************************************************/

void FAR PASCAL videoCallback(WORD msg, DWORD dw1)
{
    // invoke the callback function, if it exists.  dwFlags contains driver-
    // specific flags in the LOWORD and generic driver flags in the HIWORD

    if (CaptureStreamParms.dwCallback)
        DriverCallback (CaptureStreamParms.dwCallback,    // client's callback DWORD
                 HIWORD(CaptureStreamParms.dwFlags),  // callback flags
                 (HANDLE) CaptureStreamParms.hVideo,         // handle to the device
                 msg,                     // the message
                 CaptureStreamParms.dwCallbackInst,  // client's instance data
                 dw1,                     // first DWORD
                 0);                      // second DWORD not used
}


/* 
 * Capture a frame
 * This function implements the DVM_FRAME message.
 */
WORD FAR PASCAL CaptureFrame(LPVIDEOHDR lpVHdr)
{
    int j;
    unsigned char huge *hpc;
    
#ifdef USE_PROFILER
    ProfStart ();
#endif

    if (lpVHdr->dwBufferLength < biDest.biSizeImage)
        return DV_ERR_SIZEFIELD; //!!!???

    CT_GrabFrame ();            // Get a new frame
    
    /* either grab a 8, 16, or 24 bit DIB, or YUV */
    switch( gwDestFormat )
    {
        case IMAGE_FORMAT_PAL8:
            RectCopyBytes(fpCopyBuffer, gwWidth * 2, 
                glpFrameBuffer, gwWidthBytes,
                0, 0, gwWidth * 2, gwHeight);
            CT_Acquire (TRUE);
            mapUnpackedYUVto8(lpVHdr->lpData,fpCopyBuffer,fpTrans16to8,
                   gwWidth, gwHeight, gwWidth*2);
            break;

        case IMAGE_FORMAT_RGB16:
            RectCopyBytes(fpCopyBuffer, gwWidth * 2, 
                glpFrameBuffer, gwWidthBytes,
                0, 0, gwWidth * 2, gwHeight);
            CT_Acquire (TRUE);
            mapUnpackedYUVtoRGB16(lpVHdr->lpData,fpCopyBuffer,fpYUVtoRGB16,
                   gwWidth, gwHeight, gwWidth*2);
            break;

        case IMAGE_FORMAT_RGB24:
            RectCopyBytes(fpCopyBuffer, gwWidth * 2, 
                glpFrameBuffer, gwWidthBytes,
                0, 0, gwWidth * 2, gwHeight);
            CT_Acquire (TRUE);
            for (j = 0; j < (int) gwHeight; j++) {
                hpc = (unsigned char huge *) fpCopyBuffer + 
                        ((LONG) gwWidth * 2 * j);
                CT_YUV2RGBNoInterp (hpc, (BYTE huge *) lpVHdr->lpData + 
                        ((LONG) gwWidthBytesDest * (gwHeight - (j + 1))), 
                        gwWidth, 24);
            }
            break;

        case IMAGE_FORMAT_YUV411UNPACKED:
           RectCopyBytes(lpVHdr->lpData, gwWidth * 2, 
               glpFrameBuffer, gwWidthBytes,
               0, 0, gwWidth * 2, gwHeight);
           CT_Acquire (TRUE);
           break;

        default:
            return DV_ERR_BADFORMAT;
    }

    lpVHdr->dwBytesUsed = biDest.biSizeImage;
    lpVHdr->dwTimeCaptured = timeGetTime ();
    lpVHdr->dwFlags |= VHDR_KEYFRAME;

#ifdef USE_PROFILER
    ProfStop ();
#endif
    
    return DV_ERR_OK;
}

/*
 *
 * Initalize video driver for input.  
 * This function implements the DVM_STREAM_INIT message.
 *
 */
WORD FAR PASCAL InStreamOpen( LPVIDEO_STREAM_INIT_PARMS lpStreamInitParms )
{
    if (fVideoOpen)
        return DV_ERR_NONSPECIFIC;

    CaptureStreamParms = *lpStreamInitParms;

    /* Page lock the data since it will be used at interrupt time */
    GlobalPageLock(HIWORD(fpTrans16to8));
    GlobalPageLock(HIWORD(fpCopyBuffer));

    if (gwDestFormat == IMAGE_FORMAT_RGB16) 
        GlobalPageLock(HIWORD(fpYUVtoRGB16));

    fVideoOpen = TRUE;
	
    dwNextFrameNum = 0L;
    dwTimeStream = 0L;
    dwFramesSkipped = 0L;

    lpVHdrFirst = NULL;
    lpVHdrLast = NULL;

    CT_Acquire (TRUE);
    
    dwMicroSecPerFrame = lpStreamInitParms-> dwMicroSecPerFrame;

    dwMicroSecPerTick = gfEurope ? PALMICROSECPERTICK :
                                  NTSCMICROSECPERTICK;

    videoCallback(MM_DRVM_OPEN, 0L); // Notify app we're open via callback

    IRQEnable ();

    return DV_ERR_OK;
}

/*
 *
 * Fini video driver input
 * This function implements the DVM_STREAM_FINI message.
 *
 */
WORD FAR PASCAL InStreamClose( void )
{
    if( !fVideoOpen )
        return DV_ERR_NONSPECIFIC;

    if (lpVHdrFirst)
        return DV_ERR_STILLPLAYING;

    InStreamStop();

    fVideoOpen = FALSE;
    
    GlobalPageUnlock (HIWORD (fpCopyBuffer));
    GlobalPageUnlock (HIWORD (fpTrans16to8));
    if (gwDestFormat == IMAGE_FORMAT_RGB16) 
        GlobalPageUnlock(HIWORD(fpYUVtoRGB16));

    lpVHdrFirst = NULL;
    lpVHdrLast = NULL; 

    IRQDisable ();
    videoCallback(MM_DRVM_CLOSE, 0L); // Notify app we're closed via callback

    return DV_ERR_OK;
}


LPVIDEOHDR  NEAR PASCAL DeQueHeader()
{
    LPVIDEOHDR lpVHdr;

    if (lpVHdr = lpVHdrFirst) {
        lpVHdr->dwFlags &= ~VHDR_INQUEUE;

        _asm cli

        lpVHdrFirst = NEXT(lpVHdr);

        if (lpVHdrFirst == NULL)
            lpVHdrLast = NULL;

        _asm sti
    }

    return lpVHdr;
}

void NEAR PASCAL QueHeader(LPVIDEOHDR lpVHdr)
{
    lpVHdr->dwFlags &= ~VHDR_DONE;
    lpVHdr->dwFlags |= VHDR_INQUEUE;

    NEXT(lpVHdr) = NULL;

    _asm cli

    if (lpVHdrLast)
        NEXT(lpVHdrLast) = lpVHdr;
    else
        lpVHdrFirst = lpVHdr;

    lpVHdrLast = lpVHdr;

    _asm sti
}

/*
 *
 * Add a buffer to the input queue
 * This function implements the DVM_STREAM_ADDBUFFER message.
 *
 */
WORD FAR PASCAL InStreamAddBuffer( LPVIDEOHDR lpVHdr )
{
    if( !fVideoOpen )
        return DV_ERR_NONSPECIFIC;

    /* return error if no node passed */
    if (!lpVHdr)
        return DV_ERR_NONSPECIFIC;

    /* return error if buffer has not been prepared */
    if (!(lpVHdr->dwFlags & VHDR_PREPARED))
        return DV_ERR_UNPREPARED;

    /* return error if buffer is already in the queue */
    if (lpVHdr->dwFlags & VHDR_INQUEUE)
        return DV_ERR_NONSPECIFIC;

    if (lpVHdr->dwBufferLength < biDest.biSizeImage)
        return DV_ERR_NONSPECIFIC;

    /* set back to basic flag */
    lpVHdr->dwFlags &= ~VHDR_DONE;
    lpVHdr->dwBytesUsed = 0;

    QueHeader(lpVHdr);

    return DV_ERR_OK;
}

/*
 *
 * Start the recording - all buffers should be prepared
 * This function implements the DVM_STREAM_START message.
 *
 */
WORD FAR PASCAL InStreamStart( void )
{
    if( !fVideoOpen )
        return DV_ERR_NONSPECIFIC;

    CT_Acquire (TRUE);
    CT_IRQClear ();     // Initially clear the interrupt

    dwNextFrameNum = 0L;
    dwTimeStart = timeGetTime();
    dwVideoClock = 0;
    dwLastVideoClock = 0;
    gfVideoInStarted = TRUE;

    return DV_ERR_OK;
}

/*
 *
 * Stop the recording - Finish last buffer if needed 
 * This function implements the DVM_STREAM_STOP message.
 *
 */
WORD FAR PASCAL InStreamStop( void )
{
    if( !fVideoOpen )
        return DV_ERR_NONSPECIFIC;

    gfVideoInStarted = FALSE;
    return DV_ERR_OK;
}

/*
 *
 * Reset the buffers so that they may be unprepared and freed
 * This function implements the DVM_STREAM_RESET message.
 *
 */
WORD FAR PASCAL InStreamReset( void )
{
    LPVIDEOHDR	lpVHdr;

    if( !fVideoOpen )
        return DV_ERR_NONSPECIFIC;

    InStreamStop();

    while (lpVHdr = DeQueHeader ()) {

        lpVHdr->dwFlags |= VHDR_DONE;

        videoCallback(MM_DRVM_DATA, (DWORD) lpVHdr);
    }

    lpVHdrFirst = NULL;
    lpVHdrLast = NULL;

    return DV_ERR_OK;
}

/*
 *
 *   ISR
 *       
 *   Actually map the memory over to the next buffer and mark it done
 *      at interrupt time.
 */
void NEAR PASCAL InStreamISR( void )
{
    LPVIDEOHDR  lpVHdr;
    int         j;
    unsigned char huge * hpc;

    if (!gfVideoInStarted)
        return;

#if FRAME_INTERRUPT
    ;
#else
    if (dwVideoClock & 1L)      // Skip odd fields
        return;
#endif

    // make sure enough interrupts have happened since last xfer
    if (dwVideoClock < dwLastVideoClock + 2)
        return;

    dwTimeStream = muldiv32 (dwVideoClock, dwMicroSecPerTick, 1000);

    // is it time to get the next frame?
    if (dwTimeStream < muldiv32(dwNextFrameNum, dwMicroSecPerFrame, 1000))
        return;

    lpVHdr = DeQueHeader();     // get next header.

    if (lpVHdr == NULL) {       // No buffers available
	dwFramesSkipped++;
        videoCallback(MM_DRVM_ERROR, dwFramesSkipped); 
        return;
    }

    switch (gwDestFormat) {
        case IMAGE_FORMAT_PAL8:
        {
           CT_PrivateAcquire (FALSE);         // Cannot access mem while acquiring
           mapUnpackedYUVto8(lpVHdr->lpData,glpFrameBuffer,fpTrans16to8,
                   gwWidth, gwHeight, gwWidthBytes);
           CT_PrivateAcquire (TRUE);
           dwLastVideoClock = dwVideoClock;
        }   
        break;

        case IMAGE_FORMAT_RGB16:
           CT_PrivateAcquire (FALSE);         // Cannot access mem while acquiring
           mapUnpackedYUVtoRGB16(lpVHdr->lpData,glpFrameBuffer, fpYUVtoRGB16,
                   gwWidth, gwHeight, gwWidthBytes);
           CT_PrivateAcquire (TRUE);
           dwLastVideoClock = dwVideoClock;
           break;

        case IMAGE_FORMAT_RGB24:
           CT_PrivateAcquire (FALSE);         // Cannot access mem while acquiring
           RectCopyBytes(fpCopyBuffer, gwWidth * 2, 
               glpFrameBuffer, gwWidthBytes,
               0, 0, gwWidth * 2, gwHeight);
           CT_PrivateAcquire (TRUE);
           dwLastVideoClock = dwVideoClock;
           for (j = 0; j < (int) gwHeight; j++) {
               hpc = (unsigned char huge *) fpCopyBuffer + 
                       ((LONG) gwWidth * 2 * j);
               CT_YUV2RGBNoInterp (hpc, (BYTE huge *) lpVHdr->lpData + 
                       ((LONG) gwWidthBytesDest * (gwHeight - (j + 1))), 
                       gwWidth, 24);
           }
           break;

        case IMAGE_FORMAT_YUV411UNPACKED:
           CT_PrivateAcquire (FALSE);         // Cannot access mem while acquiring
           RectCopyBytes(lpVHdr->lpData, gwWidth * 2, 
               glpFrameBuffer, gwWidthBytes,
               0, 0, gwWidth * 2, gwHeight);
           CT_PrivateAcquire (TRUE);
           dwLastVideoClock = dwVideoClock;
           break;

    }
    
    lpVHdr->dwFlags       |= (VHDR_DONE | VHDR_KEYFRAME);
    lpVHdr->dwBytesUsed    = biDest.biSizeImage;
    lpVHdr->dwTimeCaptured = dwTimeStream;

    videoCallback(MM_DRVM_DATA, (DWORD) lpVHdr); // Notify app data is available

    dwNextFrameNum++;
}
