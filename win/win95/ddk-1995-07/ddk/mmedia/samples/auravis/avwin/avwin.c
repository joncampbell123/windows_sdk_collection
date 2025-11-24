//////////////////////////////////////////////////////////////////////////
//   Module:    AVWIN.C                                                 //
//   Target:    AVWIN.DLL                                               //
//                                                                      //
//   Summary:   This module contains PCV-compatilbe library functions   //
//              for AVWIN.DLL.						//
//                                                                      //
//   Version:   1.6                                                     //
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

#define	GLOBAL	1
#include "global.h"

#define _INC_MMDEBUG_CODE_ TRUE // For assert and logging
#include "mmdebug.h"

//////////////////////////////////////////////////////////////////////////
//      LibMain and WEP functions are required for all DLLs.            //
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//      LibMain(hModule, wDataSeg, cbHeapSize, lpszCmdLine)             //
//                                                                      //
//      This function is called by LibEntry which is called by Windows  //
//      when the DLL is loaded.  It performs any initialization not     //
//      already done by the standard LibEntry routine.                  //
//                                                                      //
//      Params: hModule Instance handle of DLL.                         //
//              wDataSeg        DLL data segment.                       //
//              cbHeapSize      Size of local heap from DEF file.       //
//              lpszCmdLine     Pointer to command line string, if any. //
//                                                                      //
//      Return: 1 (TRUE) if successful, 0 (FALSE) if unsuccessful.      //
//////////////////////////////////////////////////////////////////////////
int FAR PASCAL LibMain(
        HANDLE  hModule,
        WORD    wDataSeg,
        WORD    cbHeapSize,
        LPSTR   lpszCmdLine)
        {
        // Just return success code.
        hInst = hModule;

      #if defined DEBUG || defined DEBUG_RETAIL
        DebugSetOutputLevel (GetProfileInt ("Debug", "AVWin", 0));
        AuxDebugEx (1, DEBUGLINE "AVWin LibMain: hModule=%x, DataSeg=%x, HeapSize=%x\r\n", 
                hModule, wDataSeg, cbHeapSize);
        AuxDebugEx (1, DEBUGLINE "AVWin LibMain: szIniName=%s\r\n", (LPSTR) szIniName);
      #endif


        return 1;
        }


//////////////////////////////////////////////////////////////////////////
//      WEP(bSystemExit)                                                //
//                                                                      //
//      This function performs any cleanup tasks needed when the DLL is //
//      unloaded.  It is called automatically by Windows.               //
//                                                                      //
//      Params: bSystemExit     Determines why DLL is being unloaded    //
//                              (usually ignored).                      //
//                                                                      //
//      Return: This function should always return 1 for success.       //
//////////////////////////////////////////////////////////////////////////
int FAR PASCAL WEP(int bSystemExit)
        {
        AuxDebugEx (1, DEBUGLINE "AVWin Wep\r\n");
        return 1;
        }



//////////////////////////////////////////////////////////////////////////
//      Video functions start here.                                     //
//      AV_Initialize() should be the first function called.            //
//      AV_Exit() should be the last function called.                   //
//      The remaining video functions can be called in any order.       //
//      This first set of functions is more-or-less PCV-compatible.     //
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//      AV_ClearVideoRect(X, Y, W, H)                                   //
//                                                                      //
//      This function erases a rectangle in the frame buffer, setting   //
//      the pixels to black.                                            //
//                                                                      //
//      Params: X               X-coordinate in frame buffer.           //
//              Y               Y-coordinate in frame buffer.           //
//              W               Width of rectangle.                     //
//              H               Height of rectangle.                    //
//                                                                      //
//      Return: TRUE                                                    //
//////////////////////////////////////////////////////////////////////////
int _export FAR PASCAL AV_ClearVideoRect(WORD x, WORD y, WORD width, WORD height)
{
    unsigned int i, memmode, ileave, frwidth, frheight;
    long pos;
    unsigned int regsave[240];

    memmode = AV_GetRegister(0x18) & 0x07;          // get memory mode
    ileave = interleave[ memmode ];
    frwidth = framewidth[ memmode ][ AV_GetRegister(0x73) & 0x03 ];
    frheight = frameheight[ memmode ][ AV_GetRegister(0x73) & 0x03 ];

    if( x > frwidth || y > frheight ) return -6;

    if( x + width > frwidth ) {         // bound width to frame right edge
        width = frwidth - x;
    }

    if( y + height > frheight ) {       // bound height to frame bottom
        height = frheight - y;
    }

    pos = ((long)y * (frwidth/ileave)) + (x / ileave);   // calc position in video memory

      // Save register settings to regsave buffer
    for( i = 0; i < 240; ++i ) {
        regsave[i] = AV_GetRegister( i );
    }

    AV_WaitVGARetrace();		 //;;;;
    AV_SetRegister(0x30, 0xC0);          // Turn global reset on.
    AV_SetRegister(0x39, 0x0C);          // YUV, ISA input, progressive
    AV_SetRegister(0x58, 0x01);          // Turn off vertical filtering.
    AV_SetRegister(0x38, 0x42);          // Update input registers.
    AV_SetRegister(0x30, 0x44);          // Turn on tight decode mode, global reset off
    AV_UpdateShadowedRegisters();

    AV_SetRegister(0x3A, (width - 1) & 0xFF);
    AV_SetRegister(0x3B, (width - 1) >> 8);
    AV_SetRegister(0x3C, (height - 1) & 0xFF);
    AV_SetRegister(0x3D, (height - 1) >> 8);
    AV_SetRegister(0x40, 0x00);          // Turn off cropping.
    AV_SetRegister(0x41, 0x00);
    AV_SetRegister(0x44, width & 0xFF);
    AV_SetRegister(0x45, width >> 8);
    AV_SetRegister(0x48, 0x00);
    AV_SetRegister(0x49, 0x00);
    AV_SetRegister(0x4C, height & 0xFF);
    AV_SetRegister(0x4D, height >> 8);
    AV_SetRegister(0x50, 0x00);          // Turn off horizontal filtering.
    AV_SetRegister(0x54, 0x00);          // Turn off vertical scaling.
    AV_SetRegister(0x55, 0x00);
    AV_SetRegister(0x5C, 0x00);          // Turn off horizontal scaling.
    AV_SetRegister(0x5D, 0x00);
    AV_SetRegister(0x70, (int)(pos & 0xFF));
    AV_SetRegister(0x71, (int)((pos>>8) & 0xFF));
    AV_SetRegister(0x72, (int)((pos>>16) & 0x03));

    AV_SetRegister(0x38, 0x80);          // Reset input pipeline.
    AV_SetRegister(0x38, 0x42);          // Update input registers.

    while ((AV_GetRegister(0x39) & 0x80) == 0);  // Wait for ready.

    while( height-- ) {
        i = width;
        while( i-- ) {
            *lpFrameBuffer = 0;
        }
    }

    AV_WaitVGARetrace();			//;;;;
    AV_SetRegister(0x30, 0xC0);
    AV_SetRegister(0x39, regsave[0x39]);
    AV_SetRegister(0x58, regsave[0x58]);
    AV_SetRegister(0x38, regsave[0x38] | 0x40);
    AV_SetRegister(0x30, regsave[0x30]);
    AV_UpdateShadowedRegisters();

    AV_SetRegister(0x40, regsave[0x40]);
    AV_SetRegister(0x41, regsave[0x41]);
    AV_SetRegister(0x44, regsave[0x44]);
    AV_SetRegister(0x45, regsave[0x45]);
    AV_SetRegister(0x48, regsave[0x48]);
    AV_SetRegister(0x49, regsave[0x49]);
    AV_SetRegister(0x4C, regsave[0x4C]);
    AV_SetRegister(0x4D, regsave[0x4D]);
    AV_SetRegister(0x50, regsave[0x50]);
    AV_SetRegister(0x54, regsave[0x54]);
    AV_SetRegister(0x55, regsave[0x55]);
    AV_SetRegister(0x5C, regsave[0x5C]);
    AV_SetRegister(0x5D, regsave[0x5D]);
    AV_SetRegister(0x70, regsave[0x70]);
    AV_SetRegister(0x71, regsave[0x71]);
    AV_SetRegister(0x72, regsave[0x72]);

    AV_SetRegister(0x97, regsave[0x97] & 0x7F);
    AV_SetRegister(0x97, regsave[0x97] | 0x80);

    return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//      AV_CreateWindow(X, Y, W, H, Flag)                               //
//                                                                      //
//      This function creates a video window in a specified rectangle   //
//      on the screen.  The coordinates and size are given in screen    //
//      pixel units, with (0,0) being the upper left corner.            //
//                                                                      //
//      Params: X       Window coordinate.                              //
//              Y       Window coordinate.                              //
//              W       Window width.                                   //
//              H       Window height.                                  //
//              Flag    1 for scaled video, 0 for unscaled video.       //
//                                                                      //
//      Return: TRUE    if successful                                   //
//              FALSE   if unsuccessful (video not initialized)         //
//////////////////////////////////////////////////////////////////////////
int _export FAR PASCAL AV_CreateWindow(WORD X, WORD Y, WORD W, WORD H, BOOL Flag)
{
    WORD    nCropLeft, nCropTop, nCropRight, nCropBottom;
    WORD    nXScale, nYScale;
    int     nXStart, nYStart;
    int     nXEnd, nYEnd;
    int     nWidth, nHeight;
    WORD    Value, nInterleave;
    WORD    nVideoWidth, nVideoHeight;
    WORD    nZoomWidth, nZoomHeight, nXZoom, nYZoom;
    BOOL    bSizeChanged = FALSE;

    // If video has not been initialized, return right away.
    if (!bInitialized)
            return FALSE;

    // If width or height has changed since last time, set a flag,
    // reset Q factor to maximum, and blank the display in preparation
    // for calculating the correct Q factor later.
    if( (W != OldW) || (H != OldH) ) {
        bSizeChanged = TRUE;
        Parameters[AVQFACTOR] = 100;
        AV_SetRegister(BRIGHTNESS_CONTROL, 0);
        AV_SetRegister(CONTRAST_CONTROL, 0);
    }

    // Save window coordinates for later.
    OldX = X;
    OldY = Y;
    OldW = W;
    OldH = H;
    OldFlag = Flag;
    bUpdateType = 0;			//;;;; Update type is Create.

    // Get crop rectangle, and calculate video width and height;
    nCropLeft = Parameters[AVCROPLEFT];
    nCropRight = Parameters[AVCROPRIGHT];
    nCropTop = Parameters[AVCROPTOP] & 0x3FE;       // Should be even.
    nCropBottom = Parameters[AVCROPBOTTOM] & 0x3FE;

    nVideoWidth = nCropRight - nCropLeft;
    nVideoHeight = nCropBottom - nCropTop;

    if ((nVideoWidth < 100) || (nVideoHeight < 100)) {
        nVideoWidth = 640;
        nVideoHeight = 460;
    }

    // Adjust width and height, if neccessary.
    // Keep old width and height for zoom purposes.
    if( (X + W) > ScreenWidth ) W = ScreenWidth - X + 1;
    if( W < 1 ) W = 1;
    if( H < 1 ) H = 1;
    nZoomWidth = W;
    nZoomHeight = H;
    if( W > nVideoWidth )  W = nVideoWidth - 4;
    if( H > nVideoHeight ) H = nVideoHeight - 1;

    // Get interleave parameter, and check/set memory mode based on it.
    nInterleave = Parameters[AVINTERLEAVE];
    Value = AV_GetRegister( MEMORY_CONFIGURATION_REGISTER ) & 0x07;

    // if interleave param not right for current mode
    if( nInterleave != interleave[Value] ) {
        if (nInterleave < 2)  nInterleave = 2;

        if( DecoderType == DCD_422 ) {  // 422 decoder - mode 3 or 4
            if (nInterleave > 3)  nInterleave = 3;
            AV_SetRegister(MEMORY_CONFIGURATION_REGISTER, nInterleave+1);
        } else {                        // 411 decoder - mode 0, 1, or 2
            if (nInterleave > 4)  nInterleave = 4;
            AV_SetRegister(MEMORY_CONFIGURATION_REGISTER, nInterleave-2);
        }
        Parameters[AVINTERLEAVE] = nInterleave;
    }

    // Round up scale width to nearest multiple of interleave factor.
    W = (WORD)(((DWORD)W * Parameters[AVQFACTOR]) / 100);
    W = nInterleave * ((W + nInterleave - 1) / nInterleave);

    // Set crop window
    AV_SetRegister(INPUT_HORIZONTAL_CROPPING_LEFT_A, nCropLeft);
    AV_SetRegister(INPUT_HORIZONTAL_CROPPING_LEFT_B, nCropLeft >> 8);
    AV_SetRegister(INPUT_HORIZONTAL_CROPPING_RIGHT_A, nCropRight);
    AV_SetRegister(INPUT_HORIZONTAL_CROPPING_RIGHT_B, nCropRight >> 8);
    AV_SetRegister(INPUT_HORIZONTAL_CROPPING_TOP_A, nCropTop);
    AV_SetRegister(INPUT_HORIZONTAL_CROPPING_TOP_B, nCropTop >> 8);
    AV_SetRegister(INPUT_HORIZONTAL_CROPPING_BOTTOM_A, nCropBottom);
    AV_SetRegister(INPUT_HORIZONTAL_CROPPING_BOTTOM_B, nCropBottom >> 8);

    // Calculate x and y scale factors in the range 0-1024.
    if (Flag) {
        nXScale = (WORD) ((((DWORD)W * 1024) + nVideoWidth - 1) / nVideoWidth);
        nYScale = (WORD) ((((DWORD)H * 1024) + nVideoHeight - 1) / nVideoHeight);
    } else {
        nXScale = 1024;
        nYScale = 1024;
    }

    AV_SetRegister(INPUT_HORIZONTAL_SCALING_CONTROL_A, (nXScale << 6) & 0xC0 );
    AV_SetRegister(INPUT_HORIZONTAL_SCALING_CONTROL_B, (nXScale >> 2) & 0xFF );
    AV_SetRegister(INPUT_VERTICAL_SCALING_CONTROL_A, (nYScale << 6) & 0xC0 );
    AV_SetRegister(INPUT_VERTICAL_SCALING_CONTROL_B, (nYScale >> 2) & 0xFF );

    // Turn on horizontal filtering if we are scaling down.
    Value = AVHorFilterTable[((nXScale - 1) >> 7) & 0x07];
    AV_SetRegister(INPUT_HORIZONTAL_FILTER, Value);

    // Set vertical interpolation mode for this scale factor.
    if( nYScale > 512 )
        Value = 0x04;
    else
        Value = 0x06;

    if( (WORD)(AV_GetRegister(INPUT_VERTICAL_INTERPOLATION_CONTROL) & 0x07) != Value ) {
        AV_SetRegister(ISA_CONTROL, 0x80);
        AV_SetRegister(INPUT_VERTICAL_INTERPOLATION_CONTROL, Value);
        AV_SetRegister(ISA_CONTROL, 0x42);
    }

    // Acquire fields, not frames.
    AV_SetRegister(ACQUISITION_CONTROL, 0x04);

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
    nWidth = ((W + nInterleave - 1) / nInterleave) + 2;
    nHeight = H + 2;

    nXStart = X + Parameters[AVXPOSITION];
    if( nXStart < 0 ) nXStart = 2;

    nYStart = Y + Parameters[AVYPOSITION];
    if( nYStart < 0 ) nYStart = 2;

    if( Parameters[AVINTERLACEOUTPUT] ) nYStart |= 1;

    AV_SetRegister(DISPLAY_VIEWPORT_WIDTH_A, nWidth);
    AV_SetRegister(DISPLAY_VIEWPORT_WIDTH_B, nWidth >> 8);
    AV_SetRegister(DISPLAY_VIEWPORT_HEIGHT_A, nHeight);
    AV_SetRegister(DISPLAY_VIEWPORT_HEIGHT_B, nHeight >> 8);
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

    // If scale width is less than window width, zoom to window width.
    if( W < nZoomWidth ) {
        nXZoom = (WORD)(((DWORD)(W - 1) * 2048) / nZoomWidth);
        Value = AV_GetRegister(OUTPUT_PROCESSING_CONTROL_A);

        AV_SetRegister(OUTPUT_PROCESSING_CONTROL_A, Value | 0x06);
        AV_SetRegister(OUTPUT_HORIZONTAL_ZOOM_CONTROL_A, nXZoom);
        AV_SetRegister(OUTPUT_HORIZONTAL_ZOOM_CONTROL_B, nXZoom >> 8);
    } else {
        Value = AV_GetRegister(OUTPUT_PROCESSING_CONTROL_A);
        AV_SetRegister(OUTPUT_PROCESSING_CONTROL_A, Value & 0xF9);
    }
	 /* moved to below for Qfactor test by hsian
    // If video height is less than window height, zoom to window height.
    if( H < nZoomHeight ) {
        nYZoom = (WORD)(((DWORD)(H - 1) * 2048) / nZoomHeight);
        Value = AV_GetRegister(OUTPUT_PROCESSING_CONTROL_B);

        AV_SetRegister(OUTPUT_PROCESSING_CONTROL_B, Value | 0x01);
        AV_SetRegister(OUTPUT_VERTICAL_ZOOM_CONTROL_A, nYZoom);
        AV_SetRegister(OUTPUT_VERTICAL_ZOOM_CONTROL_B, nYZoom >> 8);
    } else {
        Value = AV_GetRegister(OUTPUT_PROCESSING_CONTROL_B);
        AV_SetRegister(OUTPUT_PROCESSING_CONTROL_B, Value & 0xFE);
    }
	 */
    // Enable color keying and display window, and enable video output.
    if( Parameters[AVCOLORKEYENABLE] ) {
        if( ScreenWidth > 1023 )
            AV_SetRegister( DISPLAY_CONTROL, 0x01 );    // use colorkey only
        else
            AV_SetRegister( DISPLAY_CONTROL, 0x05 );
    } else {
        if( ScreenWidth > 1023 )
            AV_SetRegister( DISPLAY_CONTROL, 0 );
        else
            AV_SetRegister( DISPLAY_CONTROL, 0x04 );
    }

    AV_SetRegister( OUTPUT_PROCESSING_CONTROL_B,
                        AV_GetRegister(OUTPUT_PROCESSING_CONTROL_B) | 0x20 );

    // Make sure shadowed registers are updated.
    AV_UpdateShadowedRegisters();


	 // setup Qtest enviorment by hsian
    nYZoom = (WORD)(((DWORD)(H - 1) * 2048) / ScreenHeight);
    Value = AV_GetRegister(OUTPUT_PROCESSING_CONTROL_B);

    AV_SetRegister(OUTPUT_PROCESSING_CONTROL_B, Value | 0x01);
    AV_SetRegister(OUTPUT_VERTICAL_ZOOM_CONTROL_A, nYZoom);
    AV_SetRegister(OUTPUT_VERTICAL_ZOOM_CONTROL_B, nYZoom >> 8);

    // Calculate correct Q factor for this window, if size has changed.
    if (bSizeChanged) {
        Parameters[AVQFACTOR] = GetQFactor(nZoomWidth);
        Value = AV_GetRegister(OUTPUT_PROCESSING_CONTROL_A);
        AV_SetRegister(OUTPUT_PROCESSING_CONTROL_A, Value & 0xDF);
    }

	 // moved to here for Qfactor test by hsian
    // If video height is less than window height, zoom to window height.
    if( H < nZoomHeight ) {
        nYZoom = (WORD)(((DWORD)(H - 1) * 2048) / nZoomHeight);
        Value = AV_GetRegister(OUTPUT_PROCESSING_CONTROL_B);

        AV_SetRegister(OUTPUT_PROCESSING_CONTROL_B, Value | 0x01);
        AV_SetRegister(OUTPUT_VERTICAL_ZOOM_CONTROL_A, nYZoom);
        AV_SetRegister(OUTPUT_VERTICAL_ZOOM_CONTROL_B, nYZoom >> 8);
    } else {
        Value = AV_GetRegister(OUTPUT_PROCESSING_CONTROL_B);
        AV_SetRegister(OUTPUT_PROCESSING_CONTROL_B, Value & 0xFE);
    }



    // HOT! this stuff should probably be in av_initialize
    // Set acquisition address in VxP500 video buffer to 0.
    AV_SetRegister(ACQUISITION_ADDRESS_A, 0);
    AV_SetRegister(ACQUISITION_ADDRESS_B, 0);
    AV_SetRegister(ACQUISITION_ADDRESS_C, 0);

    // Set display viewport starting address in video buffer to 0.
    AV_SetRegister(DISPLAY_VIEWPORT_STARTING_ADDRESS_A, 0);
    AV_SetRegister(DISPLAY_VIEWPORT_STARTING_ADDRESS_B, 0);
    AV_SetRegister(DISPLAY_VIEWPORT_STARTING_ADDRESS_C, 0);

    // Set Video Source and format.
    AV_SetVideoSource( Parameters[AVSOURCE] );
    AV_SetInputFormat( Parameters[AVINPUTFORMAT] );

    // Set Hue Control
    AV_SetExternalRegister(PHsLID, 0x07, Parameters[AVHUE] + 128);

    // Set Brightness, Contrast, and Saturation (unblank display).
    AV_SetRegister(BRIGHTNESS_CONTROL, Parameters[AVBRIGHTNESS] - 128);
    AV_SetRegister(CONTRAST_CONTROL, Parameters[AVCONTRAST]);
    AV_SetRegister(SATURATION_CONTROL, Parameters[AVSATURATION]);

    // Make sure shadowed registers are updated.
    AV_UpdateShadowedRegisters();

    return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//      AV_DisableColorKey()                                            //
//                                                                      //
//      This function turns off color keying.  This will cause video to //
//      be displayed in the video window regardless of what is on the   //
//      VGA.                                                            //
//                                                                      //
//      Params: none                                                    //
//                                                                      //
//      Return: none                                                    //
//////////////////////////////////////////////////////////////////////////
void _export FAR PASCAL AV_DisableColorKey(void)
        {
        AV_SetRegister(DISPLAY_CONTROL,
                    AV_GetRegister(DISPLAY_CONTROL) & 0xFE);

        AV_UpdateShadowedRegisters();
        }


//////////////////////////////////////////////////////////////////////////
//      AV_DisableFieldReplication()                                    //
//                                                                      //
//      This function does nothing.  It is provided for compatibility   //
//      with a PCV function.                                            //
//                                                                      //
//      Params: none                                                    //
//                                                                      //
//      Return: none                                                    //
//////////////////////////////////////////////////////////////////////////
void _export FAR PASCAL AV_DisableFieldReplication(void)
        {
        }


//////////////////////////////////////////////////////////////////////////
//      AV_DisableInterlace()                                           //
//                                                                      //
//      This function turns off interlaced output, for non-interlaced   //
//      VGA modes.                                                      //
//                                                                      //
//      Params: none                                                    //
//                                                                      //
//      Return: none                                                    //
//////////////////////////////////////////////////////////////////////////
void _export FAR PASCAL AV_DisableInterlace(void)
        {
        WORD    Value;

        Parameters[AVINTERLACEOUTPUT] = 0;
        AV_WaitVGARetrace();
        Value = AV_GetRegister(VGA_CONTROL);
        AV_SetRegister(VGA_CONTROL, Value & 0xFE);
        AV_UpdateShadowedRegisters();
        }


//////////////////////////////////////////////////////////////////////////
//      AV_DisableVideo()                                               //
//                                                                      //
//      This function disables the display of live video.               //
//                                                                      //
//      Params: none                                                    //
//                                                                      //
//      Return: TRUE    if successful                                   //
//              FALSE   if unsuccessful (video not initialized)         //
//////////////////////////////////////////////////////////////////////////
int _export FAR PASCAL AV_DisableVideo(void)
        {
        // Disable windowing and color keying to turn off video overlay.
        AV_SetRegister(DISPLAY_CONTROL, 0x18);
        AV_UpdateShadowedRegisters();
        return TRUE;
        }


//////////////////////////////////////////////////////////////////////////
//      AV_EnableColorKey()                                             //
//                                                                      //
//      This function turns on color keying.  This will cause any color //
//      key pixels in the video window to be replaced by live video.    //
//                                                                      //
//      Params: none                                                    //
//                                                                      //
//      Return: none                                                    //
//////////////////////////////////////////////////////////////////////////
void _export FAR PASCAL AV_EnableColorKey(void)
{
    if( Parameters[AVCOLORKEYENABLE] ) {
            AV_SetRegister(DISPLAY_CONTROL,
                        AV_GetRegister(DISPLAY_CONTROL) | 0x01);

            AV_UpdateShadowedRegisters();
    }
}


//////////////////////////////////////////////////////////////////////////
//      AV_EnableFieldReplication()                                     //
//                                                                      //
//      This function does nothing.  It is provided for compatibility   //
//      with a PCV function.                                            //
//                                                                      //
//      Params: none                                                    //
//                                                                      //
//      Return: none                                                    //
//////////////////////////////////////////////////////////////////////////
void _export FAR PASCAL AV_EnableFieldReplication(void)
        {
        }


//////////////////////////////////////////////////////////////////////////
//      AV_EnableInterlace()                                            //
//                                                                      //
//      This function turns on interlaced output, for interlaced VGA    //
//      modes (such as 1024x768 interlaced).                            //
//                                                                      //
//      Params: none                                                    //
//                                                                      //
//      Return: none                                                    //
//////////////////////////////////////////////////////////////////////////
void _export FAR PASCAL AV_EnableInterlace(void)
        {
        WORD    Value;

        Parameters[AVINTERLACEOUTPUT] = 1;
        AV_WaitVGARetrace();
        Value = AV_GetRegister(VGA_CONTROL);
        AV_SetRegister(VGA_CONTROL, Value | 0x01);
        AV_UpdateShadowedRegisters();
        }


//////////////////////////////////////////////////////////////////////////
//      AV_EnableVideo()                                                //
//                                                                      //
//      This function enables the display of live video.                //
//                                                                      //
//      Params: none                                                    //
//                                                                      //
//      Return: TRUE    if successful                                   //
//              FALSE   if unsuccessful (video not initialized)         //
//////////////////////////////////////////////////////////////////////////
int _export FAR PASCAL AV_EnableVideo(void)
        {
        // Enable windowing, and color keying to enable video overlay.
        if( Parameters[AVCOLORKEYENABLE] ) {
            if( ScreenWidth > 1023 )
                AV_SetRegister(DISPLAY_CONTROL, 0x01);
            else
                AV_SetRegister(DISPLAY_CONTROL, 0x05);
        } else {
            if( ScreenWidth > 1023 )
                AV_SetRegister(DISPLAY_CONTROL, 0);
            else
                AV_SetRegister(DISPLAY_CONTROL, 0x04);
        }
        AV_UpdateShadowedRegisters();

        return TRUE;
        }


//////////////////////////////////////////////////////////////////////////
//      AV_Exit()                                                       //
//                                                                      //
//      This function deinitializes the video system.  This should be   //
//      the last video function called by an application.               //
//                                                                      //
//      Params: none                                                    //
//                                                                      //
//      Return: TRUE                                                    //
//////////////////////////////////////////////////////////////////////////
int _export FAR PASCAL AV_Exit(void)
        {
        // Shut off the video.
	AV_DisableVideo();
        return TRUE;
        }


//////////////////////////////////////////////////////////////////////////
//      AV_FreezeVideo()                                                //
//                                                                      //
//      This function freezes the display of live video by turning off  //
//      video acquisition.                                              //
//                                                                      //
//      Params: none                                                    //
//                                                                      //
//      Return: TRUE    if successful                                   //
//              FALSE   if unsuccessful (video not initialized)         //
//////////////////////////////////////////////////////////////////////////
int _export FAR PASCAL AV_FreezeVideo(void)
        {
	AV_SetRegister( INPUT_VIDEO_CONFIGURATION_B,
		AV_GetRegister(INPUT_VIDEO_CONFIGURATION_B) | 0x08 );
        AV_UpdateShadowedRegisters();
        return TRUE;
        }



//////////////////////////////////////////////////////////////////////////
//      AV_GetColor(Index)                                              //
//                                                                      //
//      This function returns the value of a color parameter.           //
//                                                                      //
//      Params: Index           Index of parameter to get:              //
//                              BRIGHTNESS                              //
//                              SATURATION                              //
//                              CONTRAST                                //
//                              HUE                                     //
//                                                                      //
//      Return: Value           Value of parameter.                     //
//////////////////////////////////////////////////////////////////////////
BYTE _export FAR PASCAL AV_GetColor(WORD Index)
        {
        WORD Value;

        switch (Index)
                {
                case BRIGHTNESS:
                Value = Parameters[AVBRIGHTNESS];
                break;

                case SATURATION:
                Value = Parameters[AVSATURATION];
                break;

                case CONTRAST:
                Value = Parameters[AVCONTRAST];
                break;

                case HUE:
                Value = Parameters[AVHUE];
                break;

                default:
                Value = 0;
                break;
                }
        return (BYTE)Value;
        }


//////////////////////////////////////////////////////////////////////////
//      AV_GetInputFormat()                                             //
//                                                                      //
//      This function returns the current video format (NTSC or PAL).   //
//                                                                      //
//      Params: none                                                    //
//                                                                      //
//      Return: Value           Video format:                           //
//                              CF_PAL                PAL format.       //
//                              CF_NTSC               NTSC format       //
//////////////////////////////////////////////////////////////////////////
int _export FAR PASCAL AV_GetInputFormat(void)
        {
        return (int)Parameters[AVINPUTFORMAT];
        }


//////////////////////////////////////////////////////////////////////////
//      AV_GetPortAddress()                                             //
//                                                                      //
//      This function returns the current I/O base port address.        //
//                                                                      //
//      Params: None                                                    //
//                                                                      //
//      Return: Port            Value of I/O port address.              //
//////////////////////////////////////////////////////////////////////////
int _export FAR PASCAL AV_GetPortAddress(void)
        {
        return (int)Parameters[AVPORT];
        }


//////////////////////////////////////////////////////////////////////////
//      AV_GetRegister(Index)                                           //
//                                                                      //
//      This function returns the value of a video register.            //
//                                                                      //
//      Params: Index           Index of register to get.               //
//                                                                      //
//      Return: Value of this register.                                 //
//////////////////////////////////////////////////////////////////////////
BYTE _export FAR PASCAL AV_GetRegister(WORD Index)
        {
        _outp(AVIndexPort, Index);
        return (BYTE)_inp(AVDataPort);
        }


//////////////////////////////////////////////////////////////////////////
//      AV_GetSkewFactor(Index)                                         //
//                                                                      //
//      This function returns the value of a skew (position adjustment) //
//      parameter.                                                      //
//                                                                      //
//      Params: Index           Index of parameter to get:              //
//                              SF_DISPWINSKEWX         X-adjustment    //
//                              SF_DISPWINSKEWY         Y-adjustment    //
//                                                                      //
//      Return: Value           Value of parameter.                     //
//////////////////////////////////////////////////////////////////////////
WORD _export FAR PASCAL AV_GetSkewFactor(int Index)
        {
        WORD Value;

        switch (Index)
                {
                case SF_DISPWINSKEWX:
                Value = (WORD)Parameters[AVXPOSITION];
                break;

                case SF_DISPWINSKEWY:
                Value = (WORD)Parameters[AVYPOSITION];
                break;

                default:
                Value = 0;
                break;
                }
        return Value;
        }


//////////////////////////////////////////////////////////////////////////
//      AV_GetSystemMetrics(Index)                                      //
//                                                                      //
//      This function returns one of the video system parameters.       //
//                                                                      //
//      Params: Index           Index of parameter to return:           //
//                              SM_VIDEOWIDTH                           //
//                              SM_VIDEOHEIGHT                          //
//                              SM_BOARDTYPE                            //
//                              SM_VERSION                              //
//                              SM_INTERLACE                            //
//                              SM_REPLICATE                            //
//                              SM_IMAGEWIDTH                           //
//                              SM_IMAGEHEIGHT                          //
//                              SM_IMAGETYPE                            //
//                                                                      //
//      Return: Value           Value of parameter.                     //
//////////////////////////////////////////////////////////////////////////
int _export FAR PASCAL AV_GetSystemMetrics(WORD Index)
        {
        WORD    Value;

        switch(Index)
                {
                case SM_VIDEOWIDTH:
                Value = Parameters[AVCROPRIGHT] - Parameters[AVCROPLEFT];
                break;

                case SM_VIDEOHEIGHT:
                Value = Parameters[AVCROPBOTTOM] - Parameters[AVCROPTOP];
                break;

                case SM_BOARDTYPE:
                Value = 0;
                break;

                case SM_VERSION:
                Value = 0;
                break;

                case SM_INTERLACE:
                Value = Parameters[AVINTERLACEOUTPUT];
                break;

                case SM_REPLICATE:
                Value = 0;
                break;

                case SM_IMAGEWIDTH:
                Value = wImageWidth;
                break;

                case SM_IMAGEHEIGHT:
                Value = wImageHeight;
                break;

                case SM_IMAGETYPE:
                Value = wImageType;
                break;

                default:
                Value = 0;
                break;
                }

        return Value;
        }


//////////////////////////////////////////////////////////////////////////
//      AV_GetVideoAddress()                                            //
//                                                                      //
//      This function returns the current video memory base address.    //
//      If this is a number in the range 0-15, it represents an address //
//      in extended memory in megabytes.  If this is a number between   //
//      0xA000-0xF000, it represents a physical memory paragraph.       //
//                                                                      //
//      Params: None                                                    //
//                                                                      //
//      Return: Address         Value of video memory base address.     //
//////////////////////////////////////////////////////////////////////////
int _export FAR PASCAL AV_GetVideoAddress(void)
        {
        return (int)Parameters[AVADDRESS];
        }


//////////////////////////////////////////////////////////////////////////
//      AV_GetVideoSource()                                             //
//                                                                      //
//      This function returns the current video input source number.    //
//      This should be a number between 0 and 2.                        //
//                                                                      //
//      Params: None                                                    //
//                                                                      //
//      Return: Source          Value of video input source.            //
//////////////////////////////////////////////////////////////////////////
int _export FAR PASCAL AV_GetVideoSource(void)
        {
        return (int)Parameters[AVSOURCE];
        }
