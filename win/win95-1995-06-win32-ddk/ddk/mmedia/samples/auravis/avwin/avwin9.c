//////////////////////////////////////////////////////////////////////////
//   Module:    AVWIN9.C                                                //
//   Target:    AVWIN.DLL                                               //
//                                                                      //
//   Summary:   This module contains additional AuraVision library      //
//              functions for AVWIN.DLL.				//
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


//////////////////////////////////////////
//  Includes                            //
//////////////////////////////////////////
#include <windows.h>
#include "avwin.h"
#include "avwinrc.h"
#include "global.h"


//////////////////////////////////////////////////////////////////////////
//      AV_DrawWindow(ScreenXDst, ScreenYDst, DrawWDst, DrawHDst,	//
//		DrawWSrc, DrawHSrc, BitmapWSrc, BitmapHSrc, DrawType)   //
//                                                                      //
//      This function creates a video window for playback of AVI files. //
//                                                                      //
//      Params: ScreenXDst	Coordinates of destination window.      //
//		ScreenYDst						//
//		DrawWDst	Destination window width.		//
//		DrawHDst	Destination window height.		//
//		DrawWSrc	Source rectangle width.			//
//		DrawHSrc	Source rectangle height.		//
//		BitmapWSrc	Full bitmap width.			//
//		BitmapHSrc	Full bitmap height.			//
//		DrawType	Type of drawing to be done.		//
//                                                                      //
//      Return: none.                                                   //
//////////////////////////////////////////////////////////////////////////
void _export FAR PASCAL AV_DrawWindow(WORD ScreenXDst, WORD ScreenYDst,
	WORD DrawWDst, WORD DrawHDst, WORD DrawWSrc, WORD DrawHSrc,
	WORD BitmapWSrc, WORD BitmapHSrc, WORD DrawType)
	{
	WORD	X, Y, W, H;
	BOOL	Flag;
	int     nXStart, nYStart;
	int     nXEnd, nYEnd;
	int     nWidth, nHeight;
	WORD    Value, nMemorySize;
	WORD    nVideoWidth, nVideoHeight;
	WORD    nZoomWidth, nZoomHeight;
	WORD	DrawPadSrc, DrawMode, DrawInterleave;
	WORD	DrawVerticalInterpolation;
	WORD	DrawGroup, DrawPadDst, nHScale, nVScale;
	WORD	DrawBuffers;

	// Initialize some things.
	X = ScreenXDst;
	Y = ScreenYDst;
	W = DrawWDst;
	H = DrawHDst;
	Flag = 1;

	// Save window coordinates for later.
	OldX = X;
	OldY = Y;
	OldW = W;
	OldH = H;
	OldFlag = Flag;
	OldDrawWSrc = DrawWSrc;		//;;;;
	OldDrawHSrc = DrawHSrc;		//;;;;
	OldBitmapWSrc = BitmapWSrc;	//;;;;
	OldBitmapHSrc = BitmapHSrc;	//;;;;
	OldDrawType = DrawType;		//;;;;
	bUpdateType = 1;		//;;;; Update type is DRAW.

	// Get memory size parameter, and determine memory mode based on it.
	nMemorySize = Parameters[AVMEMORYSIZE];
	switch (nMemorySize)
		{
		case 1024:
//;;;;		DrawMode = 3;           // Memory mode 3 for 16-bit playback.
//;;;;		DrawInterleave = 2;     // Interleave 2.
//;;;;		DrawGroup = 4;          // Interleave/chroma group size.
		DrawMode = 1;           //;;;;
		DrawInterleave = 3;     //;;;;
		DrawGroup = 6;          //;;;;
		break;

		case 1536:
		DrawMode = 4;           // Memory mode 4 for 16-bit playback.
		DrawInterleave = 3;     // Interleave 3.
		DrawGroup = 6;          // Interleave/chroma group size.
		break;

		default:                // Interleave 2, 6 DRAM.
		DrawMode = 0;           // Memory mode 0 for 12-bit.
		DrawInterleave = 2;     // Interleave 2.
		DrawGroup = 4;          // Interleave/chroma group size.
		break;
		}

	// Calculate draw offset to pad lines to multiple of draw group.
	DrawPadSrc = DrawGroup * ((BitmapWSrc + DrawGroup - 1) / DrawGroup);

	// Decide whether we can use vertical interpolation for zooming.
	if (((DrawType == 6) || (DrawType == 106)) && (DrawInterleave > 2))
		DrawVerticalInterpolation = 1;
	else
		DrawVerticalInterpolation = 0;

	// Calculate video size.
	nVideoWidth = DrawPadSrc;
	nVideoHeight = BitmapHSrc;

	// Adjust width and height, if neccessary.
	// Keep old width and height for zoom purposes.
	if( (X + W) > ScreenWidth ) W = ScreenWidth - X + 1;
	if( W < 1 ) W = 1;
	if( H < 1 ) H = 1;
	nZoomWidth = W;
	nZoomHeight = H;
	if( W > nVideoWidth )  W = nVideoWidth - 4;
	if( H > nVideoHeight ) H = nVideoHeight - 1;

	// Set color key register.
	// Use given palette entry value if 256-color mode.
	// Look up attribute code if 16-color planar mode.
	if( nPlanes == 4 )
		Value = AVColorKeyTable[Parameters[AVCOLORKEY] & 15];
	else
		Value = Parameters[AVCOLORKEY];
	AV_SetRegister( KEY_COLOR, Value );
	AV_SetRegister(KEY_COLOR_BYTE_1, Value >> 8);

	// Set viewport position and size.
	nWidth = ((W + DrawInterleave - 1) / DrawInterleave) + 2;
	nHeight = H + 2;

	nXStart = X + Parameters[AVXPOSITION];
	if( nXStart < 0 ) nXStart = 2;

	nYStart = Y + Parameters[AVYPOSITION];
	if( nYStart < 0 ) nYStart = 2;

	if( Parameters[AVINTERLACEOUTPUT] ) nYStart |= 1;

	AV_SetRegister(DISPLAY_VIEWPORT_ORIGIN_TOP_A, nYStart);
	AV_SetRegister(DISPLAY_VIEWPORT_ORIGIN_TOP_B, nYStart >> 8);
	AV_SetRegister(DISPLAY_VIEWPORT_ORIGIN_LEFT_A, nXStart);
	AV_SetRegister(DISPLAY_VIEWPORT_ORIGIN_LEFT_B, nXStart >> 8);

	// Set display window on VGA screen.
	nXEnd = X + nZoomWidth - 1 + Parameters[AVXPOSITION];
	if( nXEnd > 0x3FF ) nXEnd = 0x3FF;
	nYEnd = Y + nZoomHeight - 1 + Parameters[AVYPOSITION];
	AV_SetRegister(DISPLAY_WINDOW_LEFT_A, nXStart);
	AV_SetRegister(DISPLAY_WINDOW_LEFT_B, nXStart >> 8);
	AV_SetRegister(DISPLAY_WINDOW_RIGHT_A, nXEnd);
	AV_SetRegister(DISPLAY_WINDOW_RIGHT_B, nXEnd >> 8);
	AV_SetRegister(DISPLAY_WINDOW_TOP_A, nYStart);
	AV_SetRegister(DISPLAY_WINDOW_TOP_B, nYStart >> 8);
	AV_SetRegister(DISPLAY_WINDOW_BOTTOM_A, nYEnd);
	AV_SetRegister(DISPLAY_WINDOW_BOTTOM_B, nYEnd >> 8);

	// Enable color keying and display window, and enable video output.
	if( Parameters[AVCOLORKEYENABLE] )
		{
		if( ScreenWidth > 1023 )
			AV_SetRegister( DISPLAY_CONTROL, 0x01 );
		else
			AV_SetRegister( DISPLAY_CONTROL, 0x05 );
		}
	else
		{
		if( ScreenWidth > 1023 )
			AV_SetRegister( DISPLAY_CONTROL, 0 );
		else
			AV_SetRegister( DISPLAY_CONTROL, 0x04 );
		}

	AV_SetRegister( OUTPUT_PROCESSING_CONTROL_B,
		AV_GetRegister(OUTPUT_PROCESSING_CONTROL_B) | 0x20 );

	// Make sure shadowed registers are updated.
	AV_UpdateShadowedRegisters();

	// Set acquisition address in VxP500 video buffer to 0.
	AV_SetRegister(ACQUISITION_ADDRESS_A, 0);
	AV_SetRegister(ACQUISITION_ADDRESS_B, 0);
	AV_SetRegister(ACQUISITION_ADDRESS_C, 0);

	// Set display viewport starting address in video buffer to 0.
	AV_SetRegister(DISPLAY_VIEWPORT_STARTING_ADDRESS_A, 0);
	AV_SetRegister(DISPLAY_VIEWPORT_STARTING_ADDRESS_B, 0);
	AV_SetRegister(DISPLAY_VIEWPORT_STARTING_ADDRESS_C, 0);

	// Set Video Source and format
	AV_SetVideoSource( Parameters[AVSOURCE] );
	AV_SetInputFormat( Parameters[AVINPUTFORMAT] );

	// Set Hue Control
	AV_SetExternalRegister(PHsLID, 0x07, Parameters[AVHUE] + 128);	//set Hue to Philips decoder

	// Make sure shadowed registers are updated.
	AV_UpdateShadowedRegisters();
	AV_EnableVideo();
	AV_FreezeVideo();

	// Decide whether to use single or double buffering.
	DrawBuffers = AV_GetParameter(AVBUFFERING);

	// Set memory mode for playback.
	if (((AV_GetRegister(0x18) & 0x07) != (int)DrawMode)
		|| ((AV_GetRegister(0x73) & 0x03) != (int)(DrawBuffers >> 1)))
		{
		AV_SetRegister(0x18, DrawMode);
		AV_WaitVGARetrace();
		AV_SetRegister(0x30, 0x80);		// Global reset on.
		if (DrawBuffers == 2)
			{
			AV_SetRegister(0x73, 0x01);     // Double buffering.
			AV_SetRegister(0x68, 0xFF);     // input depth = max.
			AV_SetRegister(0x69, 0x03);
			AV_SetRegister(0x93, 0x01);	// Output depth = 1.
			}
		else
			AV_SetRegister(0x73, 0x00);	// Single buffering.
		AV_SetRegister(0x38, 0x42);	// Force register changes.
		AV_SetRegister(0x30, 0x48);	// Reset off, Owner 128K.
		Value = AV_GetRegister(0x97);	// Make sure video is on.
		AV_SetRegister(0x97, Value & 0x7F);
		AV_SetRegister(0x97, Value | 0x80);
		}

	// Program other registers for playback.
	if ((DrawType == 6) || (DrawType == 106))
		{
		AV_SetRegister(0x39, 0x0C);	// YUV, ISA input, progressive.
		Value = AV_GetRegister(0x96);
		Value &= 0x06;			// Save zoom bits.
		Value |= 0x18;			// YUV conversions.
		AV_SetRegister(0x96, Value);	// YUV output.
		}
	else
		{
		AV_SetRegister(0x39, 0x0D);	// RGB16, ISA, progressive.
		Value = AV_GetRegister(0x96);
		Value &= 0x06;			// Save zoom bits.
		AV_SetRegister(0x96, Value);    // RGB output.
		}
	AV_SetRegister(0x3A, (DrawPadSrc - 1) & 0xFF);	// ISA Width.
	AV_SetRegister(0x3B, (DrawPadSrc - 1) >> 8);
	AV_SetRegister(0x3C, (BitmapHSrc - 1) & 0xFF);	// ISA Height.
	AV_SetRegister(0x3D, (BitmapHSrc - 1) >> 8);
	AV_SetRegister(0x40, 0x00);	// Turn off cropping.
	AV_SetRegister(0x41, 0x00);
	AV_SetRegister(0x44, DrawPadSrc & 0xFF);
	AV_SetRegister(0x45, DrawPadSrc >> 8);
	AV_SetRegister(0x48, 0x00);
	AV_SetRegister(0x49, 0x00);
	AV_SetRegister(0x4C, BitmapHSrc & 0xFF);
	AV_SetRegister(0x4D, BitmapHSrc >> 8);
	AV_SetRegister(0x50, 0x00);	// Turn off horizontal filtering.
	AV_SetRegister(0x54, 0x00);	// Turn off horizontal scaling.
	AV_SetRegister(0x55, 0x00);
	AV_SetRegister(0x58, 0x01);	// Turn off vertical filtering.
	AV_SetRegister(0x5C, 0x00);	// Turn off vertical scaling.
	AV_SetRegister(0x5D, 0x00);

	// Calculate horizontal zoom factor.
	if (DrawWDst < DrawWSrc)
		{
		// Use horizontal scaling, no zooming.
		// Make sure scaled line is not (N * Interleave + 1).
		DrawPadDst = DrawWDst + DrawPadSrc - DrawWSrc;
		DrawPadDst = DrawInterleave * ((DrawPadDst + DrawInterleave - 1)
			/ DrawInterleave);
		nHScale = (WORD)((((DWORD)DrawPadDst * 1024) + DrawPadSrc - 1)
			/ DrawPadSrc);
		AV_SetRegister(0x54, nHScale << 6);
		AV_SetRegister(0x55, nHScale >> 2);
		AV_SetRegister(0x96, AV_GetRegister(0x96) & 0xF9 );
		}
	else if (DrawWDst == DrawWSrc)
		{
		// Do nothing - no scaling or zooming.
		AV_SetRegister(0x96, AV_GetRegister(0x96) & 0xF9 );
		}
	else if (DrawWDst > DrawWSrc)
		{
		// Use horizontal zoom.
		AV_SetRegister(0x96, AV_GetRegister(0x96) | 0x06 );
		nHZoom = (WORD)(((DWORD)(DrawWSrc - 1) * 2048) / DrawWDst);
		AV_SetRegister(0xC4, nHZoom & 0xFF);
		AV_SetRegister(0xC5, nHZoom >> 8);
		}

	// Set output viewport width to correct size for original image.
	nWidth = ((DrawWSrc + DrawInterleave - 1) / DrawInterleave) + 2;
	AV_SetRegister(0xA4, nWidth & 0xFF);
	AV_SetRegister(0xA5, nWidth >> 8);

	// Calculate vertical zoom factor.
	if (DrawHDst < DrawHSrc)
		{
		// Use vertical scaling only.
		AV_SetRegister(0x58, 0x01);      // Disable vertical interp.
		nVScale = (WORD)(((DWORD)DrawHDst * 1024) / DrawHSrc);
		AV_SetRegister(0x5C, nVScale << 6);
		AV_SetRegister(0x5D, nVScale >> 2);

		nHeight = DrawHSrc + 2;         // Normal viewport height.
		AV_SetRegister(0xA6, nHeight & 0xFF);
		AV_SetRegister(0xA7, nHeight >> 8);

		AV_SetRegister(0x97, 0x20);      // Disable vertical zoom.
		AV_SetRegister(0x97, 0xA0);      // Force register update.
		}
	else if (DrawHDst == DrawHSrc)
		{
		// Disable all vertical transforms.
		AV_SetRegister(0x58, 0x01);      // Disable vertical interp.
		nHeight = DrawHSrc + 2;         // Normal viewport height.
		AV_SetRegister(0xA6, nHeight & 0xFF);
		AV_SetRegister(0xA7, nHeight >> 8);

		AV_SetRegister(0x97, 0x20);      // Disable vertical zoom.
		AV_SetRegister(0x97, 0xA0);      // Force register update.
		}
	else if ((DrawHDst > DrawHSrc) && (DrawHDst < DrawHSrc * 2)
		&& DrawVerticalInterpolation)
		{
		// Use vertical interpolation and vertical scaling.
		AV_SetRegister(0x58, 0x0C);      // Enable vertical interp.
		nVScale = (WORD)((((DWORD)DrawHDst * 1024)
			+ (DrawHSrc * 2 - 1) - 1)
			/ (DrawHSrc * 2 - 1));
		AV_SetRegister(0x5C, nVScale << 6);
		AV_SetRegister(0x5D, nVScale >> 2);
		AV_SetRegister(0xC0, 0x00);
		AV_SetRegister(0xC1, 0x00);

		nHeight = (DrawHSrc << 1) + 2;  // Adjust viewport height.
		AV_SetRegister(0xA6, nHeight & 0xFF);
		AV_SetRegister(0xA7, nHeight >> 8);

		AV_SetRegister(0x97, 0x20);      // Disable vertical zoom.
		AV_SetRegister(0x97, 0xA0);      // Force register update.
		}
	else if ((DrawHDst >= DrawHSrc * 2) && DrawVerticalInterpolation)
		{
		// Use vertical interpolation and vertical zoom.
		AV_SetRegister(0x58, 0x00);      // Enable vertical interp.
		nVZoom = (WORD)(((DWORD)(DrawHSrc - 1) * 2 * 2048) / DrawHDst);
		AV_SetRegister(0xC0, nVZoom & 0xFF);
		AV_SetRegister(0xC1, nVZoom >> 8);

		nHeight = (DrawHSrc << 1) + 2;  // Adjust viewport height.
		AV_SetRegister(0xA6, nHeight & 0xFF);
		AV_SetRegister(0xA7, nHeight >> 8);

		AV_SetRegister(0x97, 0x21);      // Enable vertical zoom.
		AV_SetRegister(0x97, 0xA1);      // Force register update.
		}
	else
		{
		// Use vertical zoom only.
		AV_SetRegister(0x58, 0x01);      // Disable vertical interp.
		nVZoom = (WORD)(((DWORD)(DrawHSrc - 1) * 2048) / DrawHDst);
		AV_SetRegister(0xC0, nVZoom & 0xFF);
		AV_SetRegister(0xC1, nVZoom >> 8);

		nHeight = DrawHSrc + 2;         // Adjust viewport height.
		AV_SetRegister(0xA6, nHeight & 0xFF);
		AV_SetRegister(0xA7, nHeight >> 8);

		AV_SetRegister(0x97, 0x21);      // Enable vertical zoom.
		AV_SetRegister(0x97, 0xA1);      // Force register update.
		}

	// Set Brightness, Contrast, and Saturation.
	// If RGB playback, be sure to use neutral values.
	if ((DrawType == 6) || (DrawType == 106))
		{
		AV_SetRegister(BRIGHTNESS_CONTROL, Parameters[AVBRIGHTNESS] - 128);
		AV_SetRegister(CONTRAST_CONTROL, Parameters[AVCONTRAST]);
		AV_SetRegister(SATURATION_CONTROL, Parameters[AVSATURATION]);
		}
	else
		{
		AV_SetRegister(BRIGHTNESS_CONTROL, 0);
		AV_SetRegister(CONTRAST_CONTROL, 8);
		AV_SetRegister(SATURATION_CONTROL, 8);
		}

	// Force register changes.
	AV_SetRegister(0x38, 0x42);

	// Reset input pipeline and wait for the bit to clear.
	AV_SetRegister(0x38, 0x82);
	while (AV_GetRegister(0x38) & 0x80);

	// Wait for input pipeline to get ready.
	while ((AV_GetRegister(0x39) & 0x80) == 0);
	}


//////////////////////////////////////////////////////////////////////////
//      AV_DisableChromaKey()						//
//                                                                      //
//      This function disables chroma keying of video.			//
//                                                                      //
//      Params: none							//
//                                                                      //
//      Return: none.                                                   //
//////////////////////////////////////////////////////////////////////////
void _export FAR PASCAL AV_DisableChromaKey(void)
	{
	WORD	Value;

	// Turn off chroma key bit.
	Value = AV_GetRegister(0x92);
	AV_SetRegister(0x92, Value | 0x01);
	Value = AV_GetRegister(0x96);
	AV_SetRegister(0x96,Value & 0xbf);
	AV_UpdateShadowedRegisters();

//	Value = AV_GetRegister(OUTPUT_PROCESSING_CONTROL_A);
//	AV_SetRegister(OUTPUT_PROCESSING_CONTROL_A, Value & 0xBF);
	}


//////////////////////////////////////////////////////////////////////////
//      AV_EnableChromaKey()						//
//                                                                      //
//      This function enables chroma keying of video.  The chroma key   //
//	ranges should be set first using the AV_SetChromaRange function.//
//                                                                      //
//      Params: none							//
//                                                                      //
//      Return: none.                                                   //
//////////////////////////////////////////////////////////////////////////
void _export FAR PASCAL AV_EnableChromaKey(void)
	{
	WORD	Value;

	// Turn on chroma key bit.
//	Value = AV_GetRegister(OUTPUT_PROCESSING_CONTROL_A);
//	AV_SetRegister(OUTPUT_PROCESSING_CONTROL_A, Value | 0x40);

	Value = AV_GetRegister(0x92);
	AV_SetRegister(0x92, Value & 0xFE);
	Value = AV_GetRegister(0x96);
	AV_SetRegister(0x96,Value | 0x40);
	AV_SetRegister(0xd8,0);
	AV_SetRegister(0xd9,0xff);
	AV_SetRegister(0xda,0);
	AV_SetRegister(0xdb,0xff);
	AV_SetRegister(0xdc,0xb0);
	AV_SetRegister(0xdd,0xff);
	AV_UpdateShadowedRegisters();


//commented out to test if the above code works better. SW
	// Make sure chroma key range is set.
//	AV_SetRegister(CHROMA_KEY_RED_LOW,  Parameters[AVREDLOW]);
//	AV_SetRegister(CHROMA_KEY_RED_HIGH, Parameters[AVREDHIGH]);
//	AV_SetRegister(CHROMA_KEY_GREEN_LOW,  Parameters[AVGREENLOW]);
//	AV_SetRegister(CHROMA_KEY_GREEN_HIGH, Parameters[AVGREENHIGH]);
//	AV_SetRegister(CHROMA_KEY_BLUE_LOW,  Parameters[AVBLUELOW]);
//	AV_SetRegister(CHROMA_KEY_BLUE_HIGH, Parameters[AVBLUEHIGH]);

	}


//////////////////////////////////////////////////////////////////////////
//      AV_SetChromaRange(wRedLow, wRedHigh, wGreenLow, wGreenHigh,	//
//		wBlueLow, wBlueHigh)					//
//                                                                      //
//      This function sets the range of RGB values that will be used	//
//	for chroma keying.  The function AV_EnableChromaKey should be	//
//	used to turn on the chroma key feature after ranges are set.	//
//                                                                      //
//      Params: none							//
//                                                                      //
//      Return: none.                                                   //
//////////////////////////////////////////////////////////////////////////
void _export FAR PASCAL AV_SetChromaRange(
	WORD	wRedLow,
	WORD	wRedHigh,
	WORD	wGreenLow,
	WORD	wGreenHigh,
	WORD	wBlueLow,
	WORD	wBlueHigh)
	{
	// Record chroma range parameters.
	Parameters[AVREDLOW]	= wRedLow;
	Parameters[AVREDHIGH]	= wRedHigh;
	Parameters[AVGREENLOW]	= wGreenLow;
	Parameters[AVGREENHIGH]	= wGreenHigh;
	Parameters[AVBLUELOW]	= wBlueLow;
	Parameters[AVBLUEHIGH]	= wBlueHigh;

	// Set chroma key range registers.
	AV_SetRegister(CHROMA_KEY_RED_LOW,    Parameters[AVREDLOW]);
	AV_SetRegister(CHROMA_KEY_RED_HIGH,   Parameters[AVREDHIGH]);
	AV_SetRegister(CHROMA_KEY_GREEN_LOW,  Parameters[AVGREENLOW]);
	AV_SetRegister(CHROMA_KEY_GREEN_HIGH, Parameters[AVGREENHIGH]);
	AV_SetRegister(CHROMA_KEY_BLUE_LOW,   Parameters[AVBLUELOW]);
	AV_SetRegister(CHROMA_KEY_BLUE_HIGH,  Parameters[AVBLUEHIGH]);
	}


//////////////////////////////////////////////////////////////////////////
//      AV_GetDrawInstance()						//
//                                                                      //
//      This function returns the global instance number of the draw	//
//	instance that is currently using the video overlay, or NULL if	//
//	the overlay is free.						//
//                                                                      //
//      Params: none							//
//                                                                      //
//      Return: DWORD DrawInstance	Global instance of draw window.	//
//////////////////////////////////////////////////////////////////////////
DWORD _export FAR PASCAL AV_GetDrawInstance(void)
	{
	// If overlay is in use, return global instance that is using it.
	if (dDrawInstance)
		return dDrawInstance;

	// If less than half a second has elapsed since last use,
	// return global instance that last used the overlay.
	if (GetTickCount() < (dLastTime + 500))
		return dLastInstance;

	// Otherwise return NULL to show that the overlay is available.
	return NULL;
	}


//////////////////////////////////////////////////////////////////////////
//      AV_GetLastWindow()						//
//                                                                      //
//      This function returns the window handle of the last window that	//
//	used the video overlay                                          //
//									//
//      Params: none							//
//                                                                      //
//      Return: HWND LastWindow		Window handle of last window.	//
//////////////////////////////////////////////////////////////////////////
HWND _export FAR PASCAL AV_GetLastWindow(void)
	{
	return hwndLastWindow;
	}


//////////////////////////////////////////////////////////////////////////
//      AV_SetDrawInstance(DrawInstance, DrawWindow)			//
//                                                                      //
//      This function can be used to record the global instance number	//
//	and the window handle of the draw instance that that is	going	//
//	to use the video overlay.					//
//                                                                      //
//      Params: DWORD DrawInstance	Global instance number.		//
//		HWND DrawWindow		Handle of draw window.		//
//                                                                      //
//      Return: none.							//
//////////////////////////////////////////////////////////////////////////
void _export FAR PASCAL AV_SetDrawInstance(DWORD DrawInstance, HWND DrawWindow)
	{
	// If clearing handles, save old handles for later.
	// Also record time for later.
	if (!DrawInstance)
		{
		if (hwndDrawWindow)
			hwndLastWindow = hwndDrawWindow;
		if (dDrawInstance)
			dLastInstance = dDrawInstance;
		dLastTime = GetTickCount();
		}

	// Record new instance and window handle.
	dDrawInstance = DrawInstance;
	hwndDrawWindow = DrawWindow;
	}


//////////////////////////////////////////////////////////////////////////
//      AV_SetVGAControl(Value)						//
//                                                                      //
//      This function sets the VGA CONTROL register.  The high color	//
//	control bits in this register must be programmed during the VGA	//
//	VSync period, and should be changed carefully to avoid changing	//
//	the appearance of the VGA.					//
//									//
//      Params: Value							//
//                                                                      //
//      Return: none							//
//////////////////////////////////////////////////////////////////////////
void _export FAR PASCAL AV_SetVGAControl(WORD Value)
	{
	WORD	Temp;

	// If this is 1024x768 mode, make sure HSync is active low.
	// This is to support a PAL workaround for the VxP-500.
	if (ScreenWidth >= 1024)
		Value &= 0xFB;

	// Set HSync and VSync bits first, to make sure VSync works properly.
	Temp = AV_GetRegister(VGA_CONTROL);
	Temp &= (~0x06);
	Temp |= (Value & 0x06);
	AV_SetRegister(VGA_CONTROL, Temp);

	// Then wait for VSync and set the whole register.
	AV_WaitVGARetrace();
	AV_SetRegister(VGA_CONTROL, Value);
	Parameters[AVVGACONTROL] = Value;
	}
