//////////////////////////////////////////////////////////////////////////
//	VMSG.C								//
//									//
//	This module contains the video message processing routines.	//
//									//
//	For the AuraVision video capture driver AVCAPT.DRV.		//
//									//
//////////////////////////////////////////////////////////////////////////
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
#include <msvideo.h>
#include <msviddrv.h>
#include "avcapt.h"
#include "config.h"
#include "debug.h"
#include "avvxp500.h"
#include "mmdebug.h"

#define WIDTHBYTES(i)     ((unsigned)((i+31)&(~31))/8)


static BOOL             fDeviceInitialized = FALSE;
extern LPVIDEOHDR	lpVHdrFirst;

PCHANNEL NEAR PASCAL VideoOpen(LPVIDEO_OPEN_PARMS lpOpenParms)
	{
	PCHANNEL	pChannel;
	LPDEVICE_INIT	lpDI = &devInit;
	LPDWORD		lpdwError = &lpOpenParms->dwError;
	DWORD		dwFlags = lpOpenParms-> dwFlags;

        // We're passed a DevNode in the VIDEO_OPEN_PARMS, which we'll pass
        // down to the VxD.  This is really only needed when multiple 
        // instances of identical hardware are installed, and we want to 
        // let the application select which instance to use.
        // The VxD must then compare the DevNode passed in with its internal
        // list of DevNodes and use the corresponding hardware.

        lpDI->dnDevNode = lpOpenParms->dnDevNode;

	//
	//  if this is the very first open then init the hardware.
	//
        AuxDebugEx (2, DEBUGLINE "VideoOpen, RefCount = %d\r\n", gwDriverUsage);

	*lpdwError = DV_ERR_OK;

	/* Only initalize on first call... */
	if (!fDeviceInitialized) {

            AuxDebugEx (4, DEBUGLINE "VideoOpen, trying BasePort=%X, IRQ=%d, BaseMemory=%X\r\n",
			lpDI->wIOBase, lpDI->bInterrupt, lpDI->wSegment);

	   if (!(AcquireVXP500( gpVxDEntry, &devInit )))
		{
	 	D1("AcquireFailed");
		*lpdwError = DV_ERR_NOTDETECTED;
		}

	    // Perform hardware initialization
	    if (! HardwareInit(lpDI))
		{
		*lpdwError = DV_ERR_NOTDETECTED;
		return NULL;
		}

	    ConfigGetSettings();	// Get global hue, sat, channel, zoom.


	    // Can we access the frame buffer?  ;;; Chicago is always Prot.Mode
//		if ((*lpdwError = InitCheckMem()) == DV_ERR_OK) 
//			{
	    if (ConfigInit(lpDI)) {

		if (TransInit())
			{
			*lpdwError = DV_ERR_NOMEM;
			}
		else
			fDeviceInitialized = TRUE;
	    }
	    else
		*lpdwError = DV_ERR_NOMEM;

	    if (*lpdwError != DV_ERR_OK)
		{
		TransFini();
		HardwareFini();
		return NULL;
		}

	}	// End if this is the first open.

	// get instance memory
	pChannel = (PCHANNEL)LocalAlloc (LPTR, sizeof(CHANNEL));
	if (pChannel == NULL)
		return (PCHANNEL) NULL;

	//
	//  make sure the channel is not already in use
	//
	switch (dwFlags & 
		(VIDEO_EXTERNALIN | VIDEO_EXTERNALOUT | VIDEO_IN | VIDEO_OUT))
		{
		case VIDEO_EXTERNALIN:
		if(gwCaptureUsage >= MAX_CAPTURE_CHANNELS)
			goto error;
		gwCaptureUsage++;
		break;

		case VIDEO_EXTERNALOUT:
		if( gwDisplayUsage >= MAX_DISPLAY_CHANNELS)
			goto error;
		gwDisplayUsage++;
		break;

		case VIDEO_IN:
		if( gwVideoInUsage >= MAX_IN_CHANNELS)
			goto error;
		gwVideoInUsage++;
		break;

		case VIDEO_OUT:
		if( gwVideoOutUsage >= MAX_OUT_CHANNELS)
			goto error;
		gwVideoOutUsage++;
		break;

		default:
		goto error;
	}

	//
	// Now that the hardware is allocated init our instance struct.
	//
	pChannel->fccType = OPEN_TYPE_VCAP;
	pChannel->dwOpenType = (dwFlags
		& (VIDEO_EXTERNALIN|VIDEO_EXTERNALOUT|VIDEO_IN|VIDEO_OUT));
	pChannel->dwOpenFlags = dwFlags;
	pChannel->lpVHdr = NULL;
	pChannel->dwError = 0L;

	gwDriverUsage++;
	return pChannel;

	error:
	if (pChannel)
		LocalFree((HLOCAL)pChannel);

	*lpdwError = DV_ERR_ALLOCATED;
	return NULL;
	}


DWORD NEAR PASCAL VideoClose(PCHANNEL pChannel)
	{
	// Decrement the channel open counters
	switch (pChannel-> dwOpenType)
		{
		case VIDEO_EXTERNALIN:
		gwCaptureUsage--;
		break;

		case VIDEO_EXTERNALOUT:
		gwDisplayUsage--;
		break;

		case VIDEO_IN:
		#ifdef USE_PROFILER
		ProfFinish();
		#endif
		// If started, or buffers in the queue,
		// don't let the close happen
		if (gfVideoInStarted || lpVHdrFirst)
			return DV_ERR_STILLPLAYING;
		gwVideoInUsage--;
		break;

		case VIDEO_OUT:
		gwVideoOutUsage--;
		break;

		default:
		break;
		}

	gwDriverUsage--;	// Decrement overall driver useage count.

	if (gwDriverUsage == 0)
		{
		HardwareFini();			// Shut down the device.
		TransFini();			// Free translation tables.
	        ReleaseVXP500( gpVxDEntry, &devInit );
		fDeviceInitialized = FALSE;
		}

	// Free the instance data.
	LocalFree((HLOCAL)pChannel);
	return DV_ERR_OK;
	}


/****************************************************************************
  Show channel specific configuration dialogs
****************************************************************************/
DWORD NEAR PASCAL VideoDialog(
	DWORD	dwOpenType,
	HWND	hWndParent,
	DWORD	dwFlags)
	{
	switch (dwOpenType)
		{
		case VIDEO_EXTERNALIN:
		if (dwFlags & VIDEO_DLG_QUERY)
			return DV_ERR_OK;	// Support the dialog.
		DialogBox(ghModule, MAKEINTRESOURCE(DLG_VIDEOSOURCE),
			(HWND)hWndParent, VideoSourceDlgProc);
		break;

		case VIDEO_IN:
		if (dwFlags & VIDEO_DLG_QUERY)
			return DV_ERR_OK;	// Support the dialog.
		DialogBox(ghModule, MAKEINTRESOURCE(DLG_VIDEOFORMAT),
			(HWND)hWndParent, VideoFormatDlgProc);
		break;

		case VIDEO_OUT:
		return DV_ERR_NOTSUPPORTED;

		case VIDEO_EXTERNALOUT:
		if (dwFlags & VIDEO_DLG_QUERY)
			return DV_ERR_OK;	   // Support the dialog
		DialogBox(ghModule, MAKEINTRESOURCE(DLG_VIDEODISPLAY),
			(HWND)hWndParent, VideoDisplayDlgProc);
		break;

		default:
		return DV_ERR_NOTSUPPORTED;
		}
	return DV_ERR_OK;
	}


/****************************************************************************
  Paint the key color
****************************************************************************/
DWORD NEAR PASCAL VideoUpdate(PCHANNEL pChannel, HWND hWnd, HDC hDC)
	{
	HW_Update(hWnd, hDC);
	return DV_ERR_OK;
	}


/****************************************************************************
  handles DVM_GET_CHANNEL_CAPS message
****************************************************************************/
DWORD NEAR PASCAL VideoChannelCaps(
	PCHANNEL	pChannel,
	LPCHANNEL_CAPS	lpCaps,
	DWORD		dwSize)
	{
	lpCaps-> dwFlags = 0L;

	switch (pChannel->dwOpenType)
		{
		case VIDEO_EXTERNALIN:
		// For this device, scaling happens during digitization
		// into the frame buffer.
		lpCaps-> dwFlags = VCAPS_CAN_SCALE;
		lpCaps-> dwSrcRectXMod = 1;	 // Src undefined at present.
		lpCaps-> dwSrcRectYMod = 1;
		lpCaps-> dwSrcRectWidthMod = 1;
		lpCaps-> dwSrcRectHeightMod = 1;
		lpCaps-> dwDstRectXMod = 4;
		lpCaps-> dwDstRectYMod = 2;
		lpCaps-> dwDstRectWidthMod = 40;
		lpCaps-> dwDstRectHeightMod = 30;
		break;

		case VIDEO_IN:
		lpCaps-> dwFlags = 0;	   // No scaling or clipping.
		lpCaps-> dwSrcRectXMod = 4;
		lpCaps-> dwSrcRectYMod = 2;
		lpCaps-> dwSrcRectWidthMod = 40;
		lpCaps-> dwSrcRectHeightMod = 30;
		lpCaps-> dwDstRectXMod = 4;
		lpCaps-> dwDstRectYMod = 2;
		lpCaps-> dwDstRectWidthMod = 40;
		lpCaps-> dwDstRectHeightMod = 30;
		break;

		case VIDEO_OUT:
		return DV_ERR_NOTSUPPORTED;  
		break;

		case VIDEO_EXTERNALOUT:
		// Overlay cannot scale. Positions on 4 X, 2 Y boundaries.
		lpCaps-> dwFlags = VCAPS_OVERLAY;
		lpCaps-> dwSrcRectXMod = 4;
		lpCaps-> dwSrcRectYMod = 2;
		lpCaps-> dwSrcRectWidthMod = 40;
		lpCaps-> dwSrcRectHeightMod = 30;
		lpCaps-> dwDstRectXMod = 4;
		lpCaps-> dwDstRectYMod = 2;
		lpCaps-> dwDstRectWidthMod = 40;
		lpCaps-> dwDstRectHeightMod = 30;
		break;

		default:
		return DV_ERR_NOTSUPPORTED;
		}
	return DV_ERR_OK;
	}


/****************************************************************************
  handles DVM_SRC_RECT and DVM_DST_RECT messages
****************************************************************************/
DWORD NEAR PASCAL VideoRectangles(
	PCHANNEL	pChannel,
	BOOL		fSrc,
	LPRECT		lpRect,
	DWORD		dwFlags)
	{
	static RECT	rcMaxRect = {0, 0, 640, 480};

	if (lpRect == NULL)
		return DV_ERR_PARAM1;

	// Note: many of the uses of the rectangle functions are not actually
	// implemented by the sample driver, (or by Vidcap), but are included 
	// here for future compatibility.

	switch (pChannel->dwOpenType)
		{
		case VIDEO_EXTERNALIN:
		if (!fSrc)
			{
			switch (dwFlags)
				{
				case VIDEO_CONFIGURE_SET:
				case VIDEO_CONFIGURE_SET | VIDEO_CONFIGURE_CURRENT:
				// Where in the frame buffer should the incoming
				// video be digitized?
				// For this driver, we only digitize to 0,0

				if ((lpRect->left == 0) && (lpRect->top == 0))
					{
					// We should really do some setup here, but for
					// the moment, all dimensions are really controlled
					// by the DVM_SET_FORMAT message.
					grcDestExtIn = *lpRect;
					return DV_ERR_OK;
					}
				break;

			case VIDEO_CONFIGURE_GET | VIDEO_CONFIGURE_CURRENT:
			*lpRect = grcDestExtIn;
			return DV_ERR_OK;
   
			case VIDEO_CONFIGURE_GET | VIDEO_CONFIGURE_MAX:
			*lpRect = rcMaxRect;
			return DV_ERR_OK;

			default:
			break;
			}
		}
	return DV_ERR_NOTSUPPORTED;  
	break;

	case VIDEO_IN:
	if (fSrc)
		{
		switch (dwFlags)
			{
			case VIDEO_CONFIGURE_SET:
			case VIDEO_CONFIGURE_SET | VIDEO_CONFIGURE_CURRENT:
			// Where in the frame buffer should we take
			// the image from?
			if ((lpRect->right - lpRect->left == (int)gwWidth) &&
				(lpRect->bottom - lpRect->top == (int)gwHeight))
				{
				grcSourceIn = *lpRect;
				return DV_ERR_OK;
				}
			break;

			case VIDEO_CONFIGURE_GET | VIDEO_CONFIGURE_CURRENT:
			*lpRect = grcSourceIn;
			return DV_ERR_OK;
				   
			case VIDEO_CONFIGURE_GET | VIDEO_CONFIGURE_MAX:
			*lpRect = rcMaxRect;
			return DV_ERR_OK;

			default:
			break;
			}
		}
	return DV_ERR_NOTSUPPORTED;  
	break;

	case VIDEO_OUT:
	return DV_ERR_NOTSUPPORTED;  
	break;

	case VIDEO_EXTERNALOUT:
	if (fSrc)
		{
		if (dwFlags & VIDEO_CONFIGURE_SET)
			{
			// What part of the frame buffer should the 
			// overlay display ?
			// These are "Windows style" rects,
			// ie. 0,0 to 160,120 specifies a 160x120 rect.
			return SetExtOutSourceRect(lpRect);
			}
		else
			return DV_ERR_NOTSUPPORTED;  
		}
	else
		{
		if (dwFlags & VIDEO_CONFIGURE_SET)
			{
			// Screen coordinates where the overlay should
			// appear.  These are "Windows style" rects,
			// ie. 0,0 to 160,120 specifies a 160x120 rect.
			return SetExtOutDestRect(lpRect);
			}
		else
			return DV_ERR_NOTSUPPORTED;  
		}
		break;

		default:
		return DV_ERR_NOTSUPPORTED;
		}
	return DV_ERR_NOTSUPPORTED;
	}


/****************************************************************************
  handles ConfigureStorage message
		lParam1 is lpszKeyFile
		lParam2 is dwFlags
****************************************************************************/
DWORD NEAR PASCAL VideoConfigureStorageMessage(
	PCHANNEL	pChannel,
	UINT		msg,
	LONG		lParam1,
	LONG		lParam2)
	{
	if (lParam2 & VIDEO_CONFIGURE_GET)
		HW_LoadConfiguration((LPSTR)lParam1);
	else if (lParam2 & VIDEO_CONFIGURE_SET)
		HW_SaveConfiguration((LPSTR)lParam1);
	else
		return DV_ERR_FLAGS;

	return DV_ERR_OK;
	}


/****************************************************************************

  handles Configure messages for video
		lParam1 is dwFlags
		lParam2 is LPVIDEOCONFIGPARMS

****************************************************************************/
DWORD NEAR PASCAL VideoConfigureMessage(
	PCHANNEL	pChannel,
	UINT		msg,
	LONG		lParam1,
	LONG		lParam2)
	{
	LPVIDEOCONFIGPARMS	lpcp;
	LPDWORD	lpdwReturn;	// Return parameter from configure.
	LPVOID	lpData1;	// Pointer to data1.
	DWORD	dwSize1;	// size of data buffer1.
	LPVOID	lpData2;	// Pointer to data2.
	DWORD	dwSize2;	// size of data buffer2.
	DWORD	dwFlags;

	if (pChannel-> dwOpenType != VIDEO_IN)
		return DV_ERR_NOTSUPPORTED;

	dwFlags = lParam1;

	lpcp = (LPVIDEOCONFIGPARMS)lParam2;
	lpdwReturn = lpcp->lpdwReturn;
	lpData1 = lpcp->lpData1;	 
	dwSize1 = lpcp->dwSize1;	 
	lpData2 = lpcp->lpData2;	 
	dwSize2 = lpcp->dwSize2;	 

	switch (msg)
		{
		case DVM_PALETTE:
		switch (dwFlags)
			{
			case (VIDEO_CONFIGURE_QUERY | VIDEO_CONFIGURE_SET):
			case (VIDEO_CONFIGURE_QUERY | VIDEO_CONFIGURE_GET):
			return DV_ERR_OK;

			case VIDEO_CONFIGURE_QUERYSIZE:
			case (VIDEO_CONFIGURE_QUERYSIZE | VIDEO_CONFIGURE_GET):
			*lpdwReturn = sizeof(LOGPALETTE)
				+ (palCurrent.palNumEntries-1)
				* sizeof(PALETTEENTRY);
			break;

			case VIDEO_CONFIGURE_SET:
			case (VIDEO_CONFIGURE_SET | VIDEO_CONFIGURE_CURRENT):
			if (!lpData1)	   // points to a LOGPALETTE structure.
				return DV_ERR_PARAM1;
			return (SetDestPalette ( (LPLOGPALETTE) lpData1, 
				(LPBYTE) NULL));
			break;

			case VIDEO_CONFIGURE_GET:
			case (VIDEO_CONFIGURE_GET | VIDEO_CONFIGURE_CURRENT):
			return (GetDestPalette ( (LPLOGPALETTE) lpData1, 
				(WORD) dwSize1));
			break;

			default:
			return DV_ERR_NOTSUPPORTED;
			}
		return DV_ERR_OK;   

		case DVM_PALETTERGB555:  
		switch (dwFlags)
			{
			case (VIDEO_CONFIGURE_QUERY | VIDEO_CONFIGURE_SET):
				return DV_ERR_OK;  // Only set command is supported.

			case VIDEO_CONFIGURE_SET:
			case (VIDEO_CONFIGURE_SET | VIDEO_CONFIGURE_CURRENT):
			if (!lpData1)	   // Points to a LOGPALETTE structure.
				return DV_ERR_PARAM1;
			if (!lpData2)	   // Points to a 32k byte RGB555 translate table.
				return DV_ERR_PARAM2;
			if (dwSize2 != 0x8000)
				return DV_ERR_PARAM2;
			return (SetDestPalette ((LPLOGPALETTE)lpData1, 
				(LPBYTE) lpData2));
			break;

			default:
			return DV_ERR_NOTSUPPORTED;
			}
		return DV_ERR_OK;

		case DVM_FORMAT:
		switch (dwFlags)
			{
			case (VIDEO_CONFIGURE_QUERY | VIDEO_CONFIGURE_SET):
			case (VIDEO_CONFIGURE_QUERY | VIDEO_CONFIGURE_GET):
				return DV_ERR_OK;  // command is supported

			case VIDEO_CONFIGURE_QUERYSIZE:
			case (VIDEO_CONFIGURE_QUERYSIZE | VIDEO_CONFIGURE_GET):
				*lpdwReturn = sizeof(BITMAPINFOHEADER);
			break;

			case VIDEO_CONFIGURE_SET:
			case (VIDEO_CONFIGURE_SET | VIDEO_CONFIGURE_CURRENT):
			return (SetDestFormat ((LPBITMAPINFOHEADER) lpData1, 
				(WORD) dwSize1));
			break;

			case VIDEO_CONFIGURE_GET:
			case (VIDEO_CONFIGURE_GET | VIDEO_CONFIGURE_CURRENT):
			return (GetDestFormat ((LPBITMAPINFOHEADER) lpData1, 
				(WORD) dwSize1));
			break;

			default:
			return DV_ERR_NOTSUPPORTED;
			}

		return DV_ERR_OK;

		default:		// Not a msg that we understand.
		return DV_ERR_NOTSUPPORTED;
		}
	return DV_ERR_NOTSUPPORTED;
	}


/****************************************************************************

  handles Stream messages for video 

****************************************************************************/
DWORD NEAR PASCAL VideoStreamMessage(
	PCHANNEL	pChannel,
	UINT		msg,
	LONG		lParam1,
	LONG		lParam2)
	{
	DWORD	dwOpenType = pChannel-> dwOpenType;

	if (dwOpenType == VIDEO_EXTERNALIN)	// Capture channel.
		{
		switch (msg)
			{
			case DVM_STREAM_INIT:
			HW_Acquire (TRUE);
			break;

			case DVM_STREAM_FINI:
			HW_Acquire (FALSE);
			break;

			default:
			return DV_ERR_NOTSUPPORTED;
			}
		return DV_ERR_OK;
		}

	else if (dwOpenType == VIDEO_EXTERNALOUT)	// Overlay channel.
		{
		switch (msg)
			{
			case DVM_STREAM_INIT:
			HW_OverlayEnable (TRUE);
			break;

			case DVM_STREAM_FINI:
			HW_OverlayEnable (FALSE);
			break;

			default:
			return DV_ERR_NOTSUPPORTED;
			}
		return DV_ERR_OK;
		}

	else switch (msg)		// Input channel.
		{
		//
		//  lParam1	 -   LPVIDEO_STREAM_INIT_PARMS
		//
		//  lParam2	 -   sizeof (LPVIDEO_STREAM_INIT_PARMS)
		//
		case DVM_STREAM_INIT:		   
		return InStreamOpen((LPVIDEO_STREAM_INIT_PARMS)lParam1);

		case DVM_STREAM_FINI:
		return InStreamClose();

		case DVM_STREAM_GETERROR:
		return InStreamError((LPDWORD) lParam1, (LPDWORD) lParam2);

		case DVM_STREAM_GETPOSITION:
		return InStreamGetPos((LPMMTIME) lParam1, (DWORD) lParam2);

		case DVM_STREAM_ADDBUFFER:
		return InStreamAddBuffer((LPVIDEOHDR)lParam1);

		case DVM_STREAM_PREPAREHEADER:  // Handled by MSVideo
		return DV_ERR_NOTSUPPORTED;

		case DVM_STREAM_UNPREPAREHEADER: // Handled by MSVideo
		return DV_ERR_NOTSUPPORTED;

		case DVM_STREAM_RESET:		  
		return InStreamReset();

		case DVM_STREAM_START:		  
		return InStreamStart();

		case DVM_STREAM_STOP:		   
		return InStreamStop();

		default:
		return DV_ERR_NOTSUPPORTED;
		}
	}


/****************************************************************************

  Main message handler

****************************************************************************/
DWORD NEAR PASCAL VideoProcessMessage(
	PCHANNEL	pChannel,
	UINT		msg,
	LONG		lParam1,
	LONG		lParam2)
	{
	DWORD	dwOpenType = pChannel-> dwOpenType;

	switch (msg)
		{
		case DVM_GETERRORTEXT: // lParam1 = LPVIDEO_GETERRORTEXT_PARMS
		if (LoadString(ghModule, 
			(WORD)  ((LPVIDEO_GETERRORTEXT_PARMS) lParam1) ->dwError,
			(LPSTR) ((LPVIDEO_GETERRORTEXT_PARMS) lParam1) ->lpText,
			(int)   ((LPVIDEO_GETERRORTEXT_PARMS) lParam1) ->dwLength))
			return DV_ERR_OK;
		else
			return DV_ERR_PARAM1;
		break;			

		//
		//  lParam1	 -   hWndParent
		//
		//  lParam2	 -   flags
		//
		case DVM_DIALOG: // lParam1 = hWndParent, lParam2 = dwFlags
			return (VideoDialog (dwOpenType, (HWND) lParam1, (DWORD) lParam2));
		break;

		case DVM_PALETTE:
		case DVM_FORMAT:
		case DVM_PALETTERGB555:
		return VideoConfigureMessage(pChannel, msg, lParam1, lParam2);

		case DVM_SRC_RECT:
		case DVM_DST_RECT:
		return VideoRectangles (pChannel, (msg == DVM_SRC_RECT) /* fSource */,
			(LPRECT) lParam1, (DWORD) lParam2);

		case DVM_UPDATE:
		if (dwOpenType != VIDEO_EXTERNALOUT)
			return DV_ERR_NOTSUPPORTED;
		return VideoUpdate (pChannel, (HWND) lParam1, (HDC) lParam2);

		case DVM_CONFIGURESTORAGE:
		return VideoConfigureStorageMessage(pChannel, msg, lParam1, lParam2);

		case DVM_FRAME:
		if (dwOpenType != VIDEO_IN)
			return DV_ERR_NOTSUPPORTED;
		return (CaptureFrame((LPVIDEOHDR)lParam1));

		case DVM_GET_CHANNEL_CAPS:
			return VideoChannelCaps (pChannel, (LPCHANNEL_CAPS) lParam1,  (DWORD)lParam2); 

		default:
		if (msg >= DVM_STREAM_MSG_START && msg <= DVM_STREAM_MSG_END)
			return VideoStreamMessage(pChannel, msg, lParam1,
				lParam2);
		else
			return DV_ERR_NOTSUPPORTED;
		}
	}
