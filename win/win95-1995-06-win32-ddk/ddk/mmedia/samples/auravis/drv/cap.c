//////////////////////////////////////////////////////////////////////////
//      CAP.C                                                           //
//                                                                      //
//      This module contains the main video capture routines.           //
//                                                                      //
//      For the AuraVision video capture driver AVCAPT.DRV.             //
//                                                                      //
//      Copyright (c) 1992-1993 AuraVision Corp.   All Rights Reserved. //
//      Copyright (c) 1992 Microsoft Corporation.  All Rights Reserved. //
//////////////////////////////////////////////////////////////////////////
       
#include <windows.h>
#include <mmsystem.h>
#include <msvideo.h>
#include <msviddrv.h>
#include <stdlib.h>
#include "avcapt.h"
#include "debug.h"
#include "mmdebug.h"

#ifndef DriverCallback
	BOOL WINAPI DriverCallback(DWORD dwCallback, UINT uFlags,
		HANDLE hDevice, UINT uMessage, DWORD dwUser, DWORD dwParam1,
		DWORD dwParam2);
#endif

// The following are constants which define, in microseconds,
// the time between vertical retrace interrupts
// for either frames or fields.

#if FRAME_INTERRUPT
	#define NTSCMICROSECPERTICK     33367L  // = 1/59.94 = 33 mS per Frame
	#define  PALMICROSECPERTICK     40000L  // = 1/50.00 = 40 mS per Frame
#else
	#define NTSCMICROSECPERTICK     16683L  // 1/(2*59.94) = 16 mS per Field
	#define  PALMICROSECPERTICK     20000L  // 1/(2*50.00) = 20 mS per Field
#endif


//
//  we use the reserved fields of a VIDEOHDR for linked list pointers
//

#define NEXT(p)         (LPVIDEOHDR)(p->dwReserved[0])


// Local to this module.

LPVIDEOHDR      lpVHdrFirst;            // Linked list of VIDEOHDRs
LPVIDEOHDR      lpVHdrLast;
BOOL            fVideoOpen = FALSE;     // TRUE if streaming initialized
DWORD           dwTimeStart;            // timeGetTime() at start of capture
DWORD           dwTimeStream;           // Current stream time in milliseconds
DWORD           dwTimeFrameStart;       // time when this frame capture began
DWORD           dwMicroSecPerFrame;     // User requested frame delta
DWORD           dwMicroSecPerTick;      // VSync interrupt frequency
DWORD           dwNextFrameNum;         // Count of frames actually captured
DWORD           dwFramesSkipped = 0;    // Count of frames skipped

VIDEO_STREAM_INIT_PARMS CaptureStreamParms;


//////////////////////////////////////////////////////////////////////////
//      InStreamError(lpdwErrorType, lpdwFramesSkipped)                 //
//                                                                      //
//      Returns the number of frames skipped to date.  This count will  //
//      be inaccurate if the frame rate requested is over 15fps.        //
//                                                                      //
//      This doesn't affect ultimate file synchronization, since        //
//      AVICap uses the time stamps returned on each frame.             //
//                                                                      //
//      This function implements the DVM_STREAM_GETERROR message.       //
//////////////////////////////////////////////////////////////////////////

DWORD FAR PASCAL InStreamError(LPDWORD lpdwErrorType, LPDWORD lpdwFramesSkipped)
	{
	if (lpdwErrorType)
		*lpdwErrorType = (dwFramesSkipped ? DV_ERR_NO_BUFFERS : DV_ERR_OK);
	if (lpdwFramesSkipped)
		*lpdwFramesSkipped = dwFramesSkipped;
	dwFramesSkipped = 0;
	return DV_ERR_OK;
	}

//////////////////////////////////////////////////////////////////////////
//      InStreamGetPos(lpMMTime, dwSize)                                //
//                                                                      //
//      Returns the current stream time from the start of capture based //
//      on the number of vsync interrupts.                              //
//                                                                      //
//      This function implements the DVM_STREAM_GETPOSITION message.    //
//////////////////////////////////////////////////////////////////////////

DWORD FAR PASCAL InStreamGetPos(LPMMTIME lpMMTime, DWORD dwSize)
	{
	if (dwSize != sizeof (MMTIME)) 
		return DV_ERR_NOTSUPPORTED;
	lpMMTime->wType = TIME_MS;
	lpMMTime->u.ms = dwTimeStream;
	return DV_ERR_OK;
	}


//////////////////////////////////////////////////////////////////////////
//      videoCallback(msg, dw1)                                         //
//                                                                      //
//      Calls DriverCallback for the input stream.                      //
//                                                                      //
//      msg     The message to send.                                    //
//      dw1     Message-dependent parameter.                            //
//                                                                      //
//      There is no return value.                                       //
//////////////////////////////////////////////////////////////////////////

void FAR PASCAL videoCallback(WORD msg, DWORD dw1)
	{
	// invoke the callback function, if it exists.  dwFlags contains driver-
	// specific flags in the LOWORD and generic driver flags in the HIWORD

	if (CaptureStreamParms.dwCallback)
		DriverCallback (CaptureStreamParms.dwCallback,  // Client's callback DWORD.
			HIWORD(CaptureStreamParms.dwFlags),     // Callback flags.
			(HANDLE)CaptureStreamParms.hVideo,      // Handle to the device.
			msg,                                    // The message.
			CaptureStreamParms.dwCallbackInst,      // Client's instance data.
			dw1,                                    // First DWORD.
			0);                                     // Second DWORD not used.
	}


//////////////////////////////////////////////////////////////////////////
//      CaptureFrame(lpVHdr)                                            //
//                                                                      //
//      Captures a single frame.                                        //
//      This function implements the DVM_FRAME message.                 //
//////////////////////////////////////////////////////////////////////////

WORD FAR PASCAL CaptureFrame(LPVIDEOHDR lpVHdr)
	{

        // We can't capture single frames when in the process of streaming
        // capture, so ignore the request

	if (gfVideoInStarted)
		return DV_ERR_NONSPECIFIC;

	#ifdef USE_PROFILER
		ProfStart();
	#endif

        AuxDebugEx (4, DEBUGLINE "Start CapFrame, glpFrameB=%lX, ... ",
                glpFrameBuffer);

	if (lpVHdr->dwBufferLength < biDest.biSizeImage)
		return DV_ERR_SIZEFIELD;

	HW_InitializeCapture(gwWidth, gwHeight, 1, 1);
	gwPadFlag = HW_GetPadFlag(gwWidth);
	HW_StartCapture();
	HW_WaitForCaptureData();
    
	// Either grab a 8, 16, or 24 bit DIB, or YUV.

	switch (gwDestFormat)
		{
		case IMAGE_FORMAT_PAL8:
                mapYUV411D4toYUV411D4(fpCopyBuffer, glpFrameBuffer, NULL,
                        gwWidth, gwHeight, wAVPort, gwPadFlag);
                mapYUV411D4toYUV422(fpCopyBuffer2, fpCopyBuffer, NULL,
                        gwWidth, gwHeight, wAVPort);
                mapYUV422toPAL8(lpVHdr->lpData, fpCopyBuffer2,
                        fpTrans16to8, gwWidth, gwHeight, gwWidth * 2);
		break;

		case IMAGE_FORMAT_RGB16:
                mapYUV411D4toYUV411D4(fpCopyBuffer, glpFrameBuffer, NULL,
                        gwWidth, gwHeight, wAVPort, gwPadFlag);
                mapYUV411D4toYUV422(fpCopyBuffer2, fpCopyBuffer, NULL,
                        gwWidth, gwHeight, wAVPort);
                mapYUV422toRGB16(lpVHdr->lpData, fpCopyBuffer2,
                        fpYUVtoRGB16, gwWidth, gwHeight, gwWidth * 2);
		break;

		case IMAGE_FORMAT_RGB24:
                mapYUV411D4toYUV411D4(fpCopyBuffer, glpFrameBuffer, NULL,
                        gwWidth, gwHeight, wAVPort, gwPadFlag);
                mapYUV411D4toYUV422(fpCopyBuffer2, fpCopyBuffer, NULL,
                        gwWidth, gwHeight, wAVPort);
                mapYUV422toRGB24(lpVHdr->lpData, fpCopyBuffer2,
                        fpYUVtoRGB16, gwWidth, gwHeight, gwWidth * 2);
		break;

		case IMAGE_FORMAT_YUV411COMPRESSED:
		mapYUV411D4toYUV411D4(lpVHdr->lpData,
			glpFrameBuffer, NULL,
			gwWidth, gwHeight, wAVPort, gwPadFlag);
		break;

		default:
		return DV_ERR_BADFORMAT;
		}

	HW_StopCapture();
	HW_DeinitializeCapture();

	lpVHdr->dwBytesUsed = biDest.biSizeImage;
	lpVHdr->dwTimeCaptured = timeGetTime();
	lpVHdr->dwFlags |= VHDR_KEYFRAME;

	#ifdef USE_PROFILER
		ProfStop ();
	#endif

        AuxDebugEx (4, DEBUGLINE "Done.\r\n");

	return DV_ERR_OK;
	}

//////////////////////////////////////////////////////////////////////////
//      InStreamOpen(lpStreamInitParms)                                 //
//                                                                      //
//      Initalizes video driver for input.                              //
//                                                                      //
//      This function implements the DVM_STREAM_INIT message.           //
//////////////////////////////////////////////////////////////////////////

WORD FAR PASCAL InStreamOpen(LPVIDEO_STREAM_INIT_PARMS lpStreamInitParms)
	{
	D1("InStreamOpen");

	if (fVideoOpen)
		return DV_ERR_NONSPECIFIC;

	CaptureStreamParms = *lpStreamInitParms;

	// Page lock the data since it will be used at interrupt time.

	GlobalSmartPageLock(HIWORD(fpTrans16to8));
	GlobalSmartPageLock(HIWORD(fpCopyBuffer));
	GlobalSmartPageLock(HIWORD(fpCopyBuffer2));

	if (gwDestFormat == IMAGE_FORMAT_RGB16) 
		GlobalSmartPageLock(HIWORD(fpYUVtoRGB16));

	fVideoOpen = TRUE;

	dwNextFrameNum = 0L;
	dwTimeStream = 0L;
	dwFramesSkipped = 0L;

	lpVHdrFirst = NULL;
	lpVHdrLast = NULL;

	dwMicroSecPerFrame = lpStreamInitParms->dwMicroSecPerFrame;

	dwMicroSecPerTick = gfEurope ? PALMICROSECPERTICK :
				  NTSCMICROSECPERTICK;

	HW_InitializeCapture(gwWidth, gwHeight, dwMicroSecPerFrame,
		dwMicroSecPerTick);
	gwPadFlag = HW_GetPadFlag(gwWidth);

	videoCallback(MM_DRVM_OPEN, 0L);        // Notify app we're open.

	IRQEnable();

	return DV_ERR_OK;
	}

//////////////////////////////////////////////////////////////////////////
//      InStreamClose()                                                 //
//                                                                      //
//      Fini video driver input.                                        //
//                                                                      //
//      This function implements the DVM_STREAM_FINI message.           //
//////////////////////////////////////////////////////////////////////////

WORD FAR PASCAL InStreamClose(void)
	{
	if (!fVideoOpen)
		return DV_ERR_NONSPECIFIC;

	if (lpVHdrFirst)
		return DV_ERR_STILLPLAYING;

	InStreamStop();

	fVideoOpen = FALSE;

	GlobalSmartPageUnlock(HIWORD(fpCopyBuffer2));
	GlobalSmartPageUnlock(HIWORD(fpCopyBuffer));
	GlobalSmartPageUnlock(HIWORD(fpTrans16to8));
	if (gwDestFormat == IMAGE_FORMAT_RGB16) 
		GlobalSmartPageUnlock(HIWORD(fpYUVtoRGB16));

	lpVHdrFirst = NULL;
	lpVHdrLast = NULL; 

	IRQDisable();

	HW_DeinitializeCapture();

	videoCallback(MM_DRVM_CLOSE, 0L);       // Notify app we're closed

	return DV_ERR_OK;
	}


//////////////////////////////////////////////////////////////////////////
//      DeQueHeader()                                                   //
//////////////////////////////////////////////////////////////////////////

#pragma optimize( "leg", off ) 

LPVIDEOHDR  NEAR PASCAL DeQueHeader()
	{
	LPVIDEOHDR      lpVHdr;

	if (lpVHdr = lpVHdrFirst)
		{
		lpVHdr->dwFlags &= ~VHDR_INQUEUE;

		_asm cli
		lpVHdrFirst = NEXT(lpVHdr);
		if (lpVHdrFirst == NULL)
			lpVHdrLast = NULL;
		_asm sti
		}

		return lpVHdr;
	}

#pragma optimize( "", on )


//////////////////////////////////////////////////////////////////////////
//      QueHeader(lpVHdr)                                               //
//////////////////////////////////////////////////////////////////////////

#pragma optimize( "leg", off )

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

#pragma optimize( "", on )


//////////////////////////////////////////////////////////////////////////
//      InStreamAddBuffer                                               //
//                                                                      //
//      Add a buffer to the input queue.                                //
//                                                                      //
//      This function implements the DVM_STREAM_ADDBUFFER message.      //
//////////////////////////////////////////////////////////////////////////

WORD FAR PASCAL InStreamAddBuffer(LPVIDEOHDR lpVHdr)
	{
	if(!fVideoOpen)
		return DV_ERR_NONSPECIFIC;

	// Return error if no node passed.
	if (!lpVHdr)
		return DV_ERR_NONSPECIFIC;

	// Return error if buffer has not been prepared.
	if (!(lpVHdr->dwFlags & VHDR_PREPARED))
		return DV_ERR_UNPREPARED;

	// Return error if buffer is already in the queue.
	if (lpVHdr->dwFlags & VHDR_INQUEUE)
		return DV_ERR_NONSPECIFIC;

	if (lpVHdr->dwBufferLength < biDest.biSizeImage)
		return DV_ERR_NONSPECIFIC;

	// Set back to basic flag.
	lpVHdr->dwFlags &= ~VHDR_DONE;
	lpVHdr->dwBytesUsed = 0;

	QueHeader(lpVHdr);

	return DV_ERR_OK;
	}

//////////////////////////////////////////////////////////////////////////
//      InStreamStart()                                                 //
//                                                                      //
//      Start the recording - all buffers should be prepared.           //
//                                                                      //
//      This function implements the DVM_STREAM_START message.          //
//////////////////////////////////////////////////////////////////////////

WORD FAR PASCAL InStreamStart(void)
	{
	if (!fVideoOpen)
		return DV_ERR_NONSPECIFIC;

	// Set up some global variables.
	dwNextFrameNum = 0L;
	dwTimeStart = timeGetTime();
	dwVideoClock = 0;

	HW_IRQClear();			// Make sure interrupt is clear.

	gfVideoInStarted = TRUE;	// Signal start of capture.
	HW_StartCapture();		// Initiate capture sequence.
	return DV_ERR_OK;
	}


//////////////////////////////////////////////////////////////////////////
//      InStreamStop()                                                  //
//                                                                      //
//      Stop the recording - Finish last buffer if needed.              //
//                                                                      //
//      This function implements the DVM_STREAM_STOP message.           //
//////////////////////////////////////////////////////////////////////////

WORD FAR PASCAL InStreamStop(void)
	{
	if (!fVideoOpen)
		return DV_ERR_NONSPECIFIC;

	gfVideoInStarted = FALSE;
	HW_StopCapture();
	return DV_ERR_OK;
	}


//////////////////////////////////////////////////////////////////////////
//      InStreamReset()                                                 //
//                                                                      //
//      Reset the buffers so that they may be unprepared and freed.     //
//                                                                      //
//      This function implements the DVM_STREAM_RESET message.          //
//////////////////////////////////////////////////////////////////////////

WORD FAR PASCAL InStreamReset(void)
	{
	LPVIDEOHDR      lpVHdr;

	if (!fVideoOpen)
		return DV_ERR_NONSPECIFIC;

	InStreamStop();

	while (lpVHdr = DeQueHeader())
		{
		lpVHdr->dwFlags |= VHDR_DONE;
		videoCallback(MM_DRVM_DATA, (DWORD) lpVHdr);
		}

	lpVHdrFirst = NULL;
	lpVHdrLast = NULL;

	return DV_ERR_OK;
	}


//////////////////////////////////////////////////////////////////////////
//      InStreamISR()                                                   //
//                                                                      //
//      Main interrupt service routine  (vectored from isr.asm)         //
//          -determine if it is time to capture the next frame          //
//          -copy the data from the frame buffer                        //
//          -convert to the requested format                            //
//                                                                      //
//      All AVI synchronization depends on the dwTimeStamp value        //
//           expressed in milliseconds you return with the VIDEOHDR.    //
//      To calculate the current time, use muldiv32, which keeps        //
//           intermediate results as 64bit values.                      //
//                                                                      //
//           dwTimeStream = dwVideoClock * dwMicroSecPerTick / 1000.    //
//                                                                      //
//      To confirm that your time stamps are correct, add the following //
//           to win.ini:                                                //
//                                                                      //
//           [avicap32]                                                 //
//           ClipboardLogging=1                                         //
//                                                                      //
//      This will log the expected and actual timestamps to the         //
//           clipboard when the capture operation completes.  Logging   //
//           has a minimal impact on capture performance.               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

void NEAR PASCAL InStreamISR(void)
        {
        LPVIDEOHDR              lpVHdr;

        if (!gfVideoInStarted)
                return;

	// Calculate the current stream time based on the vertical sync counter.
	// dwVideoClock is the counter of VSyncs received
	// since the stream was started.
        dwTimeStream = muldiv32(dwVideoClock, dwMicroSecPerTick, 1000);

        // Is it time to get the next frame?
        // If not, just return and check again at the next VSync tick.
        if (dwTimeStream < muldiv32(dwNextFrameNum, dwMicroSecPerFrame, 1000))
           return;

 	// Is a frame being acquired?  If so, save a time stamp for it.
 	if (HW_CheckAcquire())
 		{
		// Remember when we started acquiring this particular frame
		if (dwTimeFrameStart == 0)
		    dwTimeFrameStart = dwTimeStream;
 		}
	
        // Is a frame ready to capture?  If so, capture it.  If not, exit.
        if (!HW_CheckCapture())
                return;

	if (dwTimeFrameStart == 0)
	    dwTimeFrameStart = dwTimeStream;
	
	// Get capture buffer header.  If none available, capture and
	// discard frame, and return a frame dropped message.
        lpVHdr = DeQueHeader();
        if (lpVHdr == NULL)
                {
                mapYUV411D4toYUV411D4(fpCopyBuffer, glpFrameBuffer, NULL,
                        gwWidth, gwHeight, wAVPort, gwPadFlag);
		dwFramesSkipped++;
		// Note:  dwFramesSkipped is NOT used for synchronization
		// purposes.  dwTimeCaptured is 
		videoCallback(MM_DRVM_ERROR, dwFramesSkipped);
		dwTimeFrameStart = 0;
                return;
                }

	// If buffer is available, capture into it based on format.
        switch (gwDestFormat)
                {
                case IMAGE_FORMAT_PAL8:
                mapYUV411D4toYUV411D4(fpCopyBuffer, glpFrameBuffer, NULL,
                        gwWidth, gwHeight, wAVPort, gwPadFlag);
                mapYUV411D4toYUV422(fpCopyBuffer2, fpCopyBuffer, NULL,
                        gwWidth, gwHeight, wAVPort);
                mapYUV422toPAL8(lpVHdr->lpData, fpCopyBuffer2,
                        fpTrans16to8, gwWidth, gwHeight, gwWidth * 2);
                break;

                case IMAGE_FORMAT_RGB16:
                mapYUV411D4toYUV411D4(fpCopyBuffer, glpFrameBuffer, NULL,
                        gwWidth, gwHeight, wAVPort, gwPadFlag);
                mapYUV411D4toYUV422(fpCopyBuffer2, fpCopyBuffer, NULL,
                        gwWidth, gwHeight, wAVPort);
                mapYUV422toRGB16(lpVHdr->lpData, fpCopyBuffer2,
                        fpYUVtoRGB16, gwWidth, gwHeight, gwWidth * 2);
                break;

                case IMAGE_FORMAT_RGB24:
                mapYUV411D4toYUV411D4(fpCopyBuffer, glpFrameBuffer, NULL,
                        gwWidth, gwHeight, wAVPort, gwPadFlag);
                mapYUV411D4toYUV422(fpCopyBuffer2, fpCopyBuffer, NULL,
                        gwWidth, gwHeight, wAVPort);
                mapYUV422toRGB24(lpVHdr->lpData, fpCopyBuffer2,
                        fpYUVtoRGB16, gwWidth, gwHeight, gwWidth * 2);
                break;

                case IMAGE_FORMAT_YUV411COMPRESSED:
                mapYUV411D4toYUV411D4(lpVHdr->lpData, glpFrameBuffer, NULL,
                        gwWidth, gwHeight, wAVPort, gwPadFlag);
                break;
                }

	// Set dwTimeCaptured (expressed in milliseconds)
	// relative to the start of the stream.
        lpVHdr->dwTimeCaptured = dwTimeFrameStart;

	// Set bitmap size field in header,
	// and mark it as a Keyframe and done.
        lpVHdr->dwBytesUsed    = biDest.biSizeImage;
        lpVHdr->dwFlags       |= (VHDR_DONE | VHDR_KEYFRAME);

	// Notify capture application that data is available.
        videoCallback(MM_DRVM_DATA, (DWORD) lpVHdr);

	// Reset the start of frame capture time stamp
	dwTimeFrameStart = 0;

	// Update number of frames captured.
        dwNextFrameNum++;
        }
