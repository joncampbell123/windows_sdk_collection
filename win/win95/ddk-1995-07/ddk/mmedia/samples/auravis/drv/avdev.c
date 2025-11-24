//////////////////////////////////////////////////////////////////////////
//      AVDEV.C                                                         //
//                                                                      //
//      This module contains the hardware specific routines for the     //
//      AuraVision video controller.                                    //
//                                                                      //
//      For the AuraVision video capture driver AVCAPT.DRV.             //
//                                                                      //
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
#include <dos.h>
#include "avcapt.h"             // General include.
#include "avdev.h"              // Device specific include.
#include "debug.h"
#include "mmdebug.h"

#include "avwin.h"

#define KEY_COLOR_INDEX         5
#define EVEN_VSYNC_INTERRUPT    0x01
#define ODD_VSYNC_INTERRUPT     0x02


char            gszDriverName[] = "avcapt.drv";
static HBRUSH   hOverlayBrush;      // Overlay brush used to paint window.
static RECT     rcLastScreen;       // Last screen coords of overlay window.
static BOOL     fOverlayEnabled;    // Is overlay showing?
static WORD     InterleaveV;
static WORD     QFactorFlag;
static WORD     oldB;
static WORD     oldC;


BOOL	bCaptureInProgress = FALSE;
HWND	LastHwnd = NULL;

WORD	OldX = 0;		//;;;;
WORD	OldY = 0;		//;;;;
WORD	OldW = 0;		//;;;;
WORD	OldH = 0;		//;;;;


WORD FAR PASCAL GetProfileHex(LPSTR szApp, LPSTR szEntry, WORD wDef);


int FAR PASCAL FreeFrameBufferSelector()
	{
	return 0;
	}

void FAR PASCAL HW_InitializeCapture(
	int     nWidth,
	int     nHeight,
	DWORD   dFrameRate,
	DWORD   dTickRate)
	{
	WORD    Value;
	WORD    wTimeScale;
	WORD	wInterleave;

	// Calculate proper capture width value.
	wInterleave = AV_GetParameter(AVINTERLEAVE);
   InterleaveV = wInterleave;
	nWidth = ((nWidth + wInterleave - 1) / wInterleave) - 1;

	if (InterleaveV <= 2) {

		if (bCaptureInProgress == FALSE) {
	 		QFactorFlag = AV_GetParameter(AVQFACTOR);
			oldB = AV_GetParameter(AVBRIGHTNESS);
			oldC = AV_GetParameter(AVCONTRAST);

			if (AV_GetParameter(AVQFACTOR) < 100) {
				AV_SetParameter(AVQFACTOR,100);
				AV_SetParameter(AVBRIGHTNESS,0);
				AV_SetParameter(AVCONTRAST,0);
				AV_UpdateVideo();
	 		}
		}
	}
	// Set capture status flag.
	bCaptureInProgress = TRUE;

	// Reserve the overlay for our use.
	// If another window had been using it, initialize the video.
	AV_SetDrawInstance(1, 1);			//;;;;
	LastHwnd = AV_GetLastWindow();			//;;;;
	if (IsWindow(LastHwnd))      //;;;;
		{					//;;;;
		InvalidateRect(LastHwnd, NULL, FALSE);	//;;;;
		AV_Initialize();				//;;;;
		AV_CreateWindow(OldX, OldY, OldW, OldH, 1);	//;;;;
		AV_SetDrawInstance(NULL, NULL);			//;;;;
		AV_SetDrawInstance(1, 1);			//;;;;
		}						//;;;;


	// Calculate proper time scale value.
	wTimeScale = (WORD)((dTickRate * 256) / dFrameRate);
	if (wTimeScale > 256)
		wTimeScale = 256;

	AV_WaitVGARetrace();		//;;;;
	HW_WriteRegister(0x30, 0xC8);   // Global reset on.
	HW_WriteRegister(0x80, 0x89);   // Enable capture, DiffYUV411.
	HW_WriteRegister(0x68, 0xFF);   // VIN multi buffer depth.
	HW_WriteRegister(0x69, 0x03);
	if (gwHeight < 256)
		HW_WriteRegister(0x73, 0x03);   // Set quad buffering.
	else
		HW_WriteRegister(0x73, 0x01);	// Set double buffering.
	Value = HW_ReadRegister(0x58);	// Check interpolation control.
	if ((Value & 0x03) == 0)	// If both original and interpolated,
		HW_WriteRegister(0x58, Value | 0x01); // change to original.
	HW_WriteRegister(0x38, 0x42);           // Update registers.
	HW_WriteRegister(0x30, 0x48);   // Global reset off, Owner 128K.
	Value = HW_ReadRegister(0x97);	// Make sure video is on.
	HW_WriteRegister(0x97, Value & 0x7F);
	HW_WriteRegister(0x97, Value | 0x80);
	Value = AV_GetParameter(AVCONTRAST);	// Fix contrast register
	HW_WriteRegister(0xC9, Value);		//   after global reset.

	HW_WriteRegister(0x6C, 0x04);		// Acquire even and odd.
	HW_WriteRegister(0x6D, wTimeScale);     // Set time scaling.
	HW_WriteRegister(0x38, 0x42);           // Update registers.

	HW_WriteRegister(0x81, 0x00);   // Set capture viewport address.
	HW_WriteRegister(0x82, 0x00);
	HW_WriteRegister(0x83, 0x00);

	HW_WriteRegister(0x84, nWidth & 0xFF);  // Set capture width.
	HW_WriteRegister(0x85, nWidth >> 8);
	HW_WriteRegister(0x86, (nHeight - 1) & 0xFF); // Set capture height.
	HW_WriteRegister(0x87, (nHeight - 1) >> 8);

	HW_WriteRegister(0x88, 0x04);   // Set capture pixel buffer low.
	HW_WriteRegister(0x89, 0x0E);   // Set capture pixel buffer high.
	HW_WriteRegister(0x8A, 0xFF);   // Set capture multi buffer depth.
	HW_WriteRegister(0x8B, 0x03);

	HW_WriteRegister(0x80, 0x99);   // Reset capture pipeline.
	HW_WriteRegister(0x80, 0x89);
	HW_ReadRegister(0x64);          // Clear status register.
	}


WORD FAR PASCAL HW_GetPadFlag(WORD wWidth)
	{
	WORD	wMode, wPadWidth, wInterleave, wChromaSize, wTrimWidth;

	// Get memory mode to determine interleave and chroma group size.
	wMode = HW_ReadRegister(0x18) & 0x07;
	if (wMode < 3)
		{
		wInterleave = wMode + 2;
		wChromaSize = 4;
		}
	else
		{
		wInterleave = wMode - 1;
		wChromaSize = 2;
		}

	// Pad width to nearest multiple of the interleave.
	wPadWidth = wInterleave * ((wWidth + wInterleave - 1) / wInterleave);

	// Then trim down to a multiple of the chroma group.
	wTrimWidth = wChromaSize * (wPadWidth / wChromaSize);

	// If this final width is greater than the original, we have to pad.
	if (wTrimWidth > wWidth)
		return 1;
	else
		return 0;
	}


void FAR PASCAL HW_StartCapture()
	{
	WORD	Timeout;
 
	HW_WriteRegister(0x39, 0x08);	// Switch input to ISA.
	HW_WriteRegister(0x38, 0xC2);	// Reset input pipeline and update.

	Timeout = 0xFFFF;		// Wait for input reset to clear.
	while ((HW_ReadRegister(0x38) & 0x80) && Timeout--);

	HW_ReadRegister(0x61);		// Clear interrupt status.
	HW_ReadRegister(0x64);		// Clear capture status.
	HW_WriteRegister(0x80, 0x99);	// Reset capture pipeline.
	HW_WriteRegister(0x80, 0xC9);	// Enable capture.
	HW_WriteRegister(0x39, 0x00);	// Switch input to decoder.
	HW_WriteRegister(0x38, 0x42);	// Enable update.

   if ((InterleaveV <= 2) && (gfVideoInStarted) ) {

	 //	value = HW_ReadRegister(0x97);	// Make sure video is on/off.
	 //	value = value & 0x3F;
	 //	HW_WriteRegister(0x97, value );
	 // 	HW_WriteRegister(0x97, value | 0x80);
	 	 	HW_WriteRegister(0xA4, 0x02);
	 	 	HW_WriteRegister(0xA5, 0x00);
	 	 	HW_WriteRegister(0xA6, 0x02);
	 	 	HW_WriteRegister(0xA7, 0x00);


	       // 10/7	MessageBox(NULL,"Low MEMORY BandWidth.Display OFF During Capture.","AVCAPT.DRV", MB_OK|MB_ICONINFORMATION|MB_NOFOCUS);


	}

	}

void FAR PASCAL HW_WaitForCaptureData()
	{
	WORD	Timeout;

	Timeout = 0xFFFF;
	while (!(HW_ReadRegister(0x64) & 0x10) && Timeout--);
	}


void FAR PASCAL HW_StopCapture()
	{
	HW_IRQDisable();                // Disable video interrupts.
	HW_WriteRegister(0x80, 0x99);	// Reset capture pipeline.
	HW_WriteRegister(0x80, 0x89);	// Disable capture.

	if (InterleaveV <= 2)  {
  //		value = HW_ReadRegister(0x97);	// Make sure video is on.
	//	HW_WriteRegister(0x97, value | 0x40);
	 //	HW_WriteRegister(0x97, value | 0x80);

		AV_SetParameter(AVQFACTOR,QFactorFlag);
		AV_SetParameter(AVBRIGHTNESS,oldB);
		AV_SetParameter(AVCONTRAST,oldC);
		
		AV_UpdateVideo();
	}

	}



void FAR PASCAL HW_DeinitializeCapture()
	{
	WORD    Value;

	AV_WaitVGARetrace();		//;;;;
	HW_WriteRegister(0x30, 0xC0);   // Global reset on.
	HW_WriteRegister(0x80, 0x99);	// Reset capture pipeline.
	HW_WriteRegister(0x80, 0x00);   // Disable capture.
	HW_WriteRegister(0x73, 0x00);   // Set single buffer.
	HW_WriteRegister(0x30, 0x48);   // Global reset off, Owner 128K.
	Value = HW_ReadRegister(0x97);	// Make sure video is on.
	HW_WriteRegister(0x97, Value & 0x7F);
	HW_WriteRegister(0x97, Value | 0x80);
	HW_WriteRegister(0x6D, 0x00);   // Reset time scaling.
	HW_WriteRegister(0x38, 0x42);	// Update registers.
	Value = AV_GetParameter(AVCONTRAST);	// Fix contrast register
	HW_WriteRegister(0xC9, Value);		//   after global reset.

	bCaptureInProgress = FALSE;

	// Release the overlay control when done with it.
	AV_SetDrawInstance(NULL, NULL);		//;;;;
	}


//////////////////////////////////////////////////////////////////////////
//      Low level read/write routines.                                  //
//////////////////////////////////////////////////////////////////////////

// Write a byte to a register

void FAR PASCAL HW_WriteRegister(int nIndex, int nValue)
	{
	_outp(wAVPort, nIndex);
	_outp(wAVPort + 1, nValue);
	}


// Read a byte from a register

int FAR PASCAL HW_ReadRegister(int nIndex)
	{
	_outp(wAVPort, nIndex);
	return _inp(wAVPort + 1);
	}


//////////////////////////////////////////////////////////////////////////
//      Initialization and Fini routines.                               //
//////////////////////////////////////////////////////////////////////////

// Initialization routine called only once at driver load time.
// Returns TRUE on success.

BOOL FAR PASCAL HW_Init()
	{
	WORD    wRet;

	wRet = AV_Initialize();
        AuxDebugEx (1, DEBUGLINE "HW_Init: Returned=%x\r\n", wRet);

	if (wRet)
		{
		wAVPort = AV_GetParameter(AVPORT);

		// Create the overlay brush.
		hOverlayBrush = HW_SetKeyColor();
		HW_OverlayEnable(FALSE);        // Initially disable overlay.
//;;;;		HW_Acquire(FALSE);              // Turn off acquisition so we can test memory.
		}
	return wRet;
	}


// Called during shutdown of the driver.

void FAR PASCAL HW_Fini()
	{
	if (hOverlayBrush)
		{
		DeleteObject (hOverlayBrush);
		hOverlayBrush = NULL;
		}

	AV_Exit();
	}


//////////////////////////////////////////////////////////////////////////
//      Ports and address routines.                                     //
//////////////////////////////////////////////////////////////////////////

// Set the Base I/O port used by the device

void FAR PASCAL HW_SetPortAddress(int nPort)
	{
	AV_SetPortAddress(nPort);
	wPCVideoAddress = AV_GetPortAddress();
	}


// Get the Base I/O port used by the device

int FAR PASCAL HW_GetPortAddress()
	{
	return AV_GetParameter(AVPORT);
	}


// Get Linear memory address of frame buffer
// Returns value in the range 1 to 15 (meg)

int FAR PASCAL HW_GetFrameAddress()
	{
	return AV_GetParameter(AVSELECTOR);
	}


//--------------------------------------------------------------------------
//  
//  int HW_SetFrameAddress
//  
//  Description:
//      Set up physical address of frame buffer
//  
//  Parameters:
//      WORD wSegment
//         segment of physical address
//           e.g. 0x000A = 0xA0000, 0x0C0 = 0x00C00000
//  
//      WORD wSelector
//         GDT selector allocated by AVVXP500.VXD
//  
//  Return (int):
//  
//  History:   Date       Author      Comment
//              7/31/94   BryanW      Wrote it.
//  
//--------------------------------------------------------------------------

int FAR PASCAL HW_SetFrameAddress
(
    WORD            wSegment,
    WORD            wSelector
)
{
   glpFrameBuffer = (LPWORD) MAKELONG( 0, wSelector ) ;   
   AuxDebugEx (3, DEBUGLINE "HW_SetFrameAddress: Segment=%X, Selector=%X\r\n", 
        wSegment, wSelector);
   return AV_SetVideoAddressEx( wSegment, wSelector ) ;

} // HW_SetFrameAddress()


//////////////////////////////////////////////////////////////////////////
//      Configuration routines.                                         //
//////////////////////////////////////////////////////////////////////////

// Load configuration from a file.
// Returns TRUE on success, FALSE on failure

BOOL FAR PASCAL HW_LoadConfiguration(LPSTR lpszFile)
	{
	AV_LoadConfiguration();
	return TRUE;
	}


// Save configuration to a file.
// Returns TRUE on success, FALSE on failure

BOOL FAR PASCAL HW_SaveConfiguration(LPSTR lpszFile)
	{
	AV_SaveConfiguration();
	return TRUE;
	}

// General configuration

void FAR PASCAL HW_ConfigureDialogBox()
	{
	AV_ConfigureVideo();
	}


// Set the video source
// nSource ranges from 0 to ...

void FAR PASCAL HW_SetVideoSource(int nSource)
	{
	AV_SetParameter(AVSOURCE, nSource);
	AV_UpdateVideo();
	}

int FAR PASCAL HW_GetVideoSource(void)
	{
	return AV_GetParameter(AVSOURCE);
	}


// Get the number of video input channels available.

int FAR PASCAL HW_GetVideoChannelCount(void)
	{
	return 3;
	}


// Set the video standard
// 0 = NTSC, 1 = PAL

void FAR PASCAL HW_SetVideoStandard(int nStandard)
	{
	switch(nStandard)
		{
		case 0:
		AV_SetInputFormat(CF_NTSC);
		break;

		case 1:
		AV_SetInputFormat(CF_PAL);
		break;
		}
	}


int FAR PASCAL HW_GetVideoStandard(void)
	{
	return (AV_GetParameter(AVINPUTFORMAT) == 0);
	}


// Return TRUE if the device can accept SVideo
BOOL FAR PASCAL HW_HasSVideo()
	{
	return FALSE;
	}


// Set the selected input channel format to:
// 0 = composite
// 1 = SVideo
// 2 = RGB (someday?)

BOOL FAR PASCAL HW_SetVideoCableFormat(int nInputMode)
	{
	return FALSE;           // Can't do SVideo
	}


// Set the Color control registers 
// nVal on input ranges from 0 to 0x3f

void FAR PASCAL HW_SetColor(int nReg, int nVal)
	{
	}


//////////////////////////////////////////////////////////////////////////
//      Rectangle setting routines.                                     //
//////////////////////////////////////////////////////////////////////////

// Create and Position the overlay window

void FAR PASCAL HW_SetDisplayRect(LPRECT lpRectangle)
	{
	RECT    rR = *lpRectangle;
	OffsetRect(&rR, rcLastScreen.left, rcLastScreen.top);
	AV_CreateWindow(rR.left, rR.top, WidthRect(rR), HeightRect(rR), 1);
	OldX = rR.left;			//;;;;
	OldY = rR.top;			//;;;;
	OldW = WidthRect(rR);		//;;;;
	OldH = HeightRect(rR);		//;;;;
	}


void FAR PASCAL HW_SetPanAndScroll(LPRECT lpRectangle)
	{
	RECT    rR = *lpRectangle;
	}


//////////////////////////////////////////////////////////////////////////
//      Overlay keying and Painting routines.                           //
//////////////////////////////////////////////////////////////////////////

// Set the overlay key color
// Returns a brush which is used to paint the key color

HBRUSH FAR PASCAL HW_SetKeyColor()
	{
	return CreateSolidBrush(RGB(255, 0, 255));
	}


// Turn overlay display on or off

void FAR PASCAL HW_OverlayEnable(BOOL fDisplay)
	{
//;;;;	fOverlayEnabled = fDisplay;
	if (fDisplay)
		{
		AV_EnableVideo();

		// If regaining the overlay, initialize the video mode.
		if (!fOverlayEnabled)					//;;;;
			{						//;;;;
			//if in	capture mode don't reset
			//March 3 94 by Hsian 
			if (!(AV_GetRegister(0x60) & 0x41)){ 
			   AV_Initialize();				//;;;;
 			   AV_CreateWindow(OldX, OldY, OldW, OldH, 1);	//;;;;
				}
			}						//;;;;

		// Reserve the overlay for our use with fake draw instance.
		AV_SetDrawInstance(1, 1);			//;;;;
		LastHwnd = AV_GetLastWindow();			//;;;;
		if (IsWindow(LastHwnd))				//;;;;
			InvalidateRect(LastHwnd, NULL, FALSE);	//;;;;
		AV_SetDrawInstance(NULL, NULL);			//;;;;
		}
	else
		{
		AV_DisableVideo();
		}
	fOverlayEnabled = fDisplay;				//;;;;
	}


// Update the overlay window rectangle and paint the key color

void FAR PASCAL HW_Update(HWND hWnd, HDC hDC)
	{
	RECT    rcScreen, rcClient;
	HBRUSH  hbrOld;

	// Calculate screen rectangle for overlay.
	rcScreen = grcDestExtOut;
	rcScreen.right = rcScreen.left + gwWidth -1;
	rcScreen.bottom = rcScreen.top + gwHeight -1;

	// Calculate client area to be painted with key color.
	rcClient = grcDestExtOut;
	ScreenToClient(hWnd, (LPPOINT)&rcClient);
	ScreenToClient(hWnd, (LPPOINT)&rcClient+1);

	// Create the video overlay.
	if (fOverlayEnabled && !bCaptureInProgress)	//;;;;
		{
		// If video is frozen from a playback, initialize everything.
		if (AV_GetRegister(0x39) & 0x08)
			AV_Initialize();
		AV_CreateWindow(rcScreen.left, rcScreen.top,
			WidthRect(rcScreen), HeightRect(rcScreen), 1);
		OldX = rcScreen.left;		//;;;;
		OldY = rcScreen.top;		//;;;;
		OldW = WidthRect(rcScreen);	//;;;;
		OldH = HeightRect(rcScreen);	//;;;;
		}

	// Paint the key color into grcDestExtOut (in screen coords).
	hbrOld = SelectObject(hDC, hOverlayBrush);
	PatBlt(hDC, rcClient.left, rcClient.top,
		rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top, PATCOPY);
	SelectObject(hDC, hbrOld);

	rcLastScreen = rcScreen;
	}


//////////////////////////////////////////////////////////////////////////
//      Image acquistion and grabbing routines.                         //
//////////////////////////////////////////////////////////////////////////

// Turn acquisition of images into frame buffer on or off
// Not interrupt callable, since it enters the client DLL.

void FAR PASCAL HW_Acquire(BOOL fAcquire)
	{
	if (fAcquire)				//;;;;
		AV_UnfreezeVideo();		//;;;;
	else					//;;;;
		AV_FreezeVideo();		//;;;;
	}


// This routine is interrupt callable, and bypasses
// the client DLL.

void FAR PASCAL HW_PrivateAcquire(BOOL fAcquire)
	{
	if (fAcquire)
		{
		// Deactiveate capture.
		HW_WriteRegister(0x80, 0x89);
		HW_ReadRegister(0x64);
		HW_ReadRegister(0x61);
		}
	else
		{
		// Activate capture, reset capture and inpup pipelines.
		HW_ReadRegister(0x61);
		HW_WriteRegister(0x80, 0x99);
		HW_ReadRegister(0x64);
		HW_WriteRegister(0x80, 0xC9);
		}
	}


//////////////////////////////////////////////////////////////////////////
//      Interrupts and Sync routines.                                   //
//////////////////////////////////////////////////////////////////////////

// Set the IRQ used by the device.
// Returns the IRQ actually set.

int FAR PASCAL HW_SetIRQUsed(int nIRQ)
	{
	AV_SetParameter(AVIRQLEVEL, nIRQ);
	return AV_GetParameter(AVIRQLEVEL);
	}


// Returns the IRQ used by the device

int FAR PASCAL HW_GetIRQUsed()
	{
	return AV_GetParameter(AVIRQLEVEL);
	}


// Enable the VxP-500 chip to create odd and even interrupts

void FAR PASCAL HW_IRQEnable(void)
	{
	HW_ReadRegister(0x61);          // Clear old interrupts.
	HW_WriteRegister(0x60, 0x41);   // Set interrupts on even fields
	}				//   and start of acquisition.


void FAR PASCAL HW_IRQDisable(void)
	{
	HW_WriteRegister(0x60, 0x00);   // Disable interrupts.
	}

// Clear interrupts, and return value of interrupt status.

void FAR PASCAL HW_IRQClear(void)
	{
	wIRQStatus |= (WORD)(HW_ReadRegister(0x61) & 0xFF);
	wCapStatus |= (WORD)(HW_ReadRegister(0x64) & 0xFF);
	}
