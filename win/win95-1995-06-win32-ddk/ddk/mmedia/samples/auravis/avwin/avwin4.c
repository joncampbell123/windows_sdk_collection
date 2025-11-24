//////////////////////////////////////////////////////////////////////////
//   Module:    AVWIN4.C                                                //
//   Target:    AVWIN.DLL                                               //
//                                                                      //
//   Summary:   This module contains AuraVision library functions and   //
//              utilities for AVWIN.DLL.				//
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
#include "bmtype.h"
#include "mmdebug.h"

//////////////////////////////////////////////////////////////////////////
//      The following functions are new AuraVision video functions,     //
//        not PCV compatible.                                           //
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//      AV_ConfigureVideo()                                             //
//                                                                      //
//      This function will display a configuration dialog box to allow  //
//      the user to adjust most of the basic video parameters.          //
//                                                                      //
//      Params: none                                                    //
//                                                                      //
//      Return: none                                                    //
//////////////////////////////////////////////////////////////////////////
void _export FAR PASCAL AV_ConfigureVideo(void)
        {
        // Run video configuration program.
	WinExec("AVCONFIG.EXE", SW_SHOW);
        }


//////////////////////////////////////////////////////////////////////////
//      FixVGAMode()                                                    //
//                                                                      //
//      This function checks to see if the VGA is in a high color mode, //
//      and, if so, programs the VxP-500 registers properly for it.     //
//      It also sets the interlaced output bit for 1024x768 mode.       //
//                                                                      //
//      Params: none                                                    //
//                                                                      //
//      Return: TRUE                                                    //
//////////////////////////////////////////////////////////////////////////
void FAR PASCAL FixVGAMode(void)
        {
        WORD    Value = 0;
	WORD	wTimeout;
        int     i;
        char    szBuffer[128];

        AuxDebugEx (3, DEBUGLINE "Start FixVGAMode\r\n");

	// Assume VGA HSync and VSync are active high.
	Value = AV_GetRegister(VGA_CONTROL);
	AV_SetRegister(VGA_CONTROL, Value | 0x06);

	// Determine polarity of VGA VSync (0 or 1).
	VSyncActive = GetVSyncPolarity();

	// Wait for start of VGA VSync period, then wait for end of it.
	AV_WaitVGARetrace();
        wTimeout = 0xFFFF;
        while (!(AV_GetRegister(VIDEO_OUTPUT_INTERRUPT_STATUS) & 0x02)
                && wTimeout--);

	// Determine polarity of VGA HSync (0 or 1).
	HSyncActive = GetHSyncPolarity();

	// Program VxP-500 for correct sync polarity.
	Value = AV_GetRegister(VGA_CONTROL) & 0xF9;
	AV_SetRegister(VGA_CONTROL, Value | (VSyncActive << 1)
		| (HSyncActive << 2));

        // Pick up color key value we calculated earlier.
        Parameters[AVCOLORKEY] = wColorKey;

        // Determine how VGA and Color Key registers should be set.
        switch (NumColors)
                {
                case 2:
                case 16:
                case 256:
                Value = 0x00;
                break;

                case 555:
                _inp(0x3C8);
                _inp(0x3C6);
                _inp(0x3C6);
                _inp(0x3C6);
                _inp(0x3C6);
                Value = _inp(0x3C6);
                if ((Value & 0xE0) == 0x80)
                        Value = 0x08;
                else
                        Value = 0x18;
                break;

                case 565:
                Value = 0x38;
                break;

                case 655:
                Value = 0x78;
                break;

                case 664:
                Value = 0x58;
                break;

                // All other modes, including 888 and unsupported.
                default:
                Value = 0x00;
                break;
                }

        // If this is 1024x768 mode, set interlaced output bit.
        if (ScreenWidth > 1023) {
              Parameters[AVINTERLACEOUTPUT] = 1;
              Value |= 0x01;
        } else {
              Parameters[AVINTERLACEOUTPUT] = 0;
        }

	// Adjust Value for HSync and VSync polarity.
	Value |= (VSyncActive << 1) | (HSyncActive << 2);

        // Wait for VGA VSync before changing VGA_CONTROL register.
        AV_SetVGAControl(Value);

        // Fix VGA cursor white color if necessary.
        Value = _inp(0x3C8);
        _outp(0x3C8, 0xFF);
        _outp(0x3C9, 0xFF);
        _outp(0x3C9, 0xFF);
        _outp(0x3C9, 0xFF);
        _outp(0x3C8, Value);

        AuxDebugEx (3, DEBUGLINE "Almost end FixVGAMode\r\n");

        // Copy alternate palette into VGA palette if necessary for ATI.
        if (nBitsPixel != 8)
                return;
        GetPrivateProfileString("boot", "display.drv", "", szBuffer,
                sizeof(szBuffer), "system.ini");
        szBuffer[3] = '\0';
        if (lstrcmpi(szBuffer, "M32") != 0)
                return;
        for (i = 0; i < 256; i++)
                {
                _outp(0x2EB, i);
                _outp(0x3C8, i);
                _outp(0x3C9, _inp(0x2ED));
                _outp(0x3C9, _inp(0x2ED));
                _outp(0x3C9, _inp(0x2ED));
                }
        }


//////////////////////////////////////////////////////////////////////////
//      GetNumColors()                                                  //
//                                                                      //
//      This function returns a number specifying the color mode that   //
//      the VGA display is currently in.  This number is used to get    //
//      various parameters for this VGA mode.                           //
//                                                                      //
//      Params: none                                                    //
//                                                                      //
//      Return: 16      if 16-color mode.                               //
//              256     if 256-color mode.                              //
//              555     if 32K-color mode = RGB555.                     //
//              565     if 64K-color mode = RGB565.                     //
//              655     if 64K-color mode = RGB655.                     //
//              664     if 64K-color mode = RGB664.                     //
//              888     if 16M-color mode = RGB888.                     //
//              999     if unknown hicolor mode.                        //
//////////////////////////////////////////////////////////////////////////
WORD FAR PASCAL GetNumColors(void)
        {
        HDC                     hDCLocal;
        WORD                    Value;
        UINT                    bmType;

        // Calculate number of colors from GetDeviceCaps info.
        hDCLocal = GetDC(NULL);
        nPlanes = GetDeviceCaps(hDCLocal, PLANES);
        nBitsPixel = GetDeviceCaps(hDCLocal, BITSPIXEL);
        Value = GetDeviceCaps(hDCLocal, NUMCOLORS);

        // Start of changes from Auravision original
        crPColor = RGB(255, 0, 255);
        wColorKey = (WORD) GetPhysColor(hDCLocal, crPColor);
        bmType = GetBitmapType(hDCLocal);
        
        ReleaseDC(NULL, hDCLocal);

#ifdef THIS_CODE_CAUSED_GPF_IN_DISPLAY_DRIVERS
        // Determine default color key by calling Display Driver functions.
        pdBitmap.bmType         = 0;
        pdBitmap.bmWidth        = 1;
        pdBitmap.bmHeight       = 1;
        pdBitmap.bmWidthBytes   = 2;
        pdBitmap.bmPlanes       = nPlanes;
        pdBitmap.bmBitsPixel    = nBitsPixel;
        pdBitmap.bmBits         = (LPBYTE)&wBitmapBits[0];
        pdBitmap.bmWidthPlanes  = 2 * nPlanes * nBitsPixel;
        pdBitmap.bmlpPDevice    = NULL;
        pdBitmap.bmSegmentIndex = 0;
        pdBitmap.bmScanSegment  = 0;
        pdBitmap.bmFillBytes    = 0;
        crPColor = RGB(255, 0, 255);
        dmDrawMode.Rop2 = 13;
        ColorInfo(&pdBitmap, crPColor, &crPColor);
        Pixel(&pdBitmap, 0, 0, crPColor, &dmDrawMode);
        wColorKey = wBitmapBits[0];
#endif

        // Adjust color key if necessary.
        if (nPlanes == 4)
                wColorKey = 13;
        if (nBitsPixel == 4)
                wColorKey = wBitmapBits[0] >> 4;

        // Ask Windows for number of colors it supports.
        // If this is 2, 16, or 256, just return it.
        if (Value <= 16)
                return Value;
        if (Value <= 256)
                return 256;

        // If 24-bit color mode, return 888.
        if ((nPlanes * nBitsPixel) == 24)
                return 888;

        switch (bmType) {
            case BM_8BIT :      return 256;
            case BM_16555:      return 555;
            case BM_16565:      return 565;
            case BM_24RGB:      return 888;
            case BM_24BGR:      return 888;
            case BM_32RGB:      return 999;
            case BM_32BGR:      return 999;

            case BM_NULL :
            default:
                                return 999;
                break;
        }


#if 0   // deleted jaybo

        // Otherwise, we have to determine which high color mode this is
        // by examining our calculated color key value.
        switch (wColorKey)
                {
                case 0x7C1F:
                case 0xFC1F:
                Value = 555;
                break;

                case 0x801F:
                Value = 555;
                wColorKey = 0x7C1F;
                break;

                case 0xF81F:
                Value = 565;
                break;

                case 0xFC0F:
                Value = 664;
                break;

                default:
                Value = 999;
                break;
                }

        return Value;
#endif

        }


//////////////////////////////////////////////////////////////////////////
//      GetQFactor(wWindowWidth)                                        //
//                                                                      //
//      This function determines the basic Q Factor for this VGA mode   //
//      by checking various Q Factors to see if pixel dropping occurs.  //
//                                                                      //
//      Params: wWindowWidth    Actual width of video window.           //
//                                                                      //
//      Return: wQFactor        Best Q Factor for this VGA mode.        //
//////////////////////////////////////////////////////////////////////////
WORD FAR PASCAL GetQFactor(WORD wWindowWidth)
        {
        WORD    wQFactor;
        WORD    nInterleave;


		  WORD	 HeightA,HeightB;	
		  HeightA = AV_GetRegister(DISPLAY_VIEWPORT_HEIGHT_A);
		  HeightB = AV_GetRegister(DISPLAY_VIEWPORT_HEIGHT_B);

//		  AV_DisableVideo();


    	  AV_SetRegister(DISPLAY_VIEWPORT_HEIGHT_A, (ScreenHeight));
    	  AV_SetRegister(DISPLAY_VIEWPORT_HEIGHT_B, ScreenHeight >> 8);
        // Make sure shadowed registers are updated.
        AV_UpdateShadowedRegisters();


        // Wait for one frame to make sure video is stable.
		  // Redundent - Hsian
        // AV_WaitVideoFrame();

        // If an ideal Q factor of 100 works, just return it now.
		  // wQFactor = 100;

		  // if (TestQFactor())
		  //    return wQFactor;

		  // if(ScreenHeight > 480)
        // Otherwise disable display and test Q factors until you find limit.
        for (wQFactor = 100; wQFactor >= 25; wQFactor -= 5)
        //for (wQFactor = 95; wQFactor >= 30; wQFactor -= 5)
                {
                // Test this Q factor.  If sucessful, stop here.
                SetQFactor(wQFactor, wWindowWidth);
                if (TestQFactor(wQFactor))
                        break;
		  			 }

		  // hsian add	for interleave 2 Qfactor safty range
		 if((wQFactor >= 60) && (wQFactor != 100)) wQFactor -= 5;
       SetQFactor(wQFactor, wWindowWidth);
       nInterleave = Parameters[AVINTERLEAVE];
       if (wWindowWidth > 80) {
			 if (ScreenWidth >= 800) {
		       if ((nInterleave <= 2)) { 
   		      while (wQFactor >= 55) { 
					   wQFactor -= 5;
         		   SetQFactor(wQFactor, wWindowWidth);
            	}
		   	 }
       	 }
		 }
    	  AV_SetRegister(DISPLAY_VIEWPORT_HEIGHT_A, HeightA);
    	  AV_SetRegister(DISPLAY_VIEWPORT_HEIGHT_B, HeightB);
        AV_UpdateShadowedRegisters();
		  AV_EnableVideo();

        // Return the final Q factor.
        return wQFactor;
        }


//////////////////////////////////////////////////////////////////////////
//      TestQFactor()                                                   //
//                                                                      //
//      This function checks a Q Factor value to see if it allows video //
//      to be displayed with no pixel dropping.                         //
//                                                                      //
//      Params: Q Factor                                                    //
//                                                                      //
//      Return: TRUE            if sucessful (no pixel dropping).       //
//              FALSE           if unsucessful (pixel dropping).        //
//////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL TestQFactor(WORD wQFactor)
        {
        WORD    Value, wTimeout;

        SetQFactor(wQFactor, (ScreenWidth-100));

        // Wait for the start of a video frame.
        AV_WaitVideoFrame();

        // See if any pixels are dropped before the next video frame.
        Value = 0;
        wTimeout = 0xFFFF;
        while (wTimeout--)
                {
                Value |= AV_GetRegister(INPUT_FIELD_PIXEL_BUFFER_STATUS);
                if (Value & 0x0D)
                        break;
                }
        if ((Value & 0x0C) != 0){
                return FALSE;
		  }

        // Wait for the start of a video frame.
        AV_WaitVideoFrame();

        // See if any pixels are dropped before the next video frame.
        Value = 0;
        wTimeout = 0xFFFF;
        while (wTimeout--)
                {
                Value |= AV_GetRegister(INPUT_FIELD_PIXEL_BUFFER_STATUS);
                if (Value & 0x0D)
                        break;
                }


        // If no pixels were dropped, return success code.
        if ((Value & 0x0C) == 0)
                return TRUE;
        else
                return FALSE;
        }


//////////////////////////////////////////////////////////////////////////
//      SetQFactor(wQFactor)                                            //
//                                                                      //
//      This function sets a new Q Factor.                              //
//                                                                      //
//      Params: wQFactor        New Q factor to be set.                 //
//              wWindowWidth    Width of video window.                  //
//                                                                      //
//      Return: none                                                    //
//////////////////////////////////////////////////////////////////////////
void FAR PASCAL SetQFactor(WORD wQFactor, WORD wWindowWidth)
        {
        WORD    W, nInterleave, nZoomWidth, nXScale, nXZoom, nWidth;
        BYTE    bXScaleLo, bXScaleHi;
        WORD    nVideoWidth, Value;

        // Calculate video width parameters.
        nVideoWidth = Parameters[AVCROPRIGHT] - Parameters[AVCROPLEFT];
        nZoomWidth = wWindowWidth;
        if (wWindowWidth <= nVideoWidth)
                W = wWindowWidth;
        else
                W = nVideoWidth;
        nInterleave = Parameters[AVINTERLEAVE];

        // Calculate scale width based on Q factor.
        W = (WORD)(((DWORD)W * wQFactor) / 100);
        W = nInterleave * ((W + nInterleave - 1) / nInterleave);

        // Calculate x scale factor in the range 0-1024.
        nXScale = (WORD) ((((DWORD)W * 1024) + nVideoWidth - 1)
                / nVideoWidth);
        bXScaleLo = (BYTE)((nXScale << 6) & 0xC0);
        bXScaleHi = (BYTE)((nXScale >> 2) & 0xFF);
        AV_SetRegister(INPUT_HORIZONTAL_SCALING_CONTROL_A, bXScaleLo);
        AV_SetRegister(INPUT_HORIZONTAL_SCALING_CONTROL_B, bXScaleHi);

        // Set viewport size.
        nWidth = ((W + nInterleave - 1) / nInterleave) + 2;
        AV_SetRegister(DISPLAY_VIEWPORT_WIDTH_A, nWidth);
        AV_SetRegister(DISPLAY_VIEWPORT_WIDTH_B, nWidth >> 8);

		  if (W < nZoomWidth) {
           // Calculate x zoom to zoom back to original size.
           nXZoom = (WORD)(((DWORD)(W - 1) * 2048) / nZoomWidth);
		     if ((nXZoom == 0x00) || (nXZoom == 0x800)) {	
             Value = AV_GetRegister(OUTPUT_PROCESSING_CONTROL_A);
             AV_SetRegister(OUTPUT_PROCESSING_CONTROL_A, Value & 0xF9);
		     }

           Value = AV_GetRegister(OUTPUT_PROCESSING_CONTROL_A);
           AV_SetRegister(OUTPUT_PROCESSING_CONTROL_A, Value | 0x06);

           AV_SetRegister(OUTPUT_HORIZONTAL_ZOOM_CONTROL_A, nXZoom);
           AV_SetRegister(OUTPUT_HORIZONTAL_ZOOM_CONTROL_B, nXZoom >> 8);

        }
        else {
           Value = AV_GetRegister(OUTPUT_PROCESSING_CONTROL_A);
           AV_SetRegister(OUTPUT_PROCESSING_CONTROL_A, Value & 0xF9);

        }

        // Make sure shadowed registers are updated.
        AV_UpdateShadowedRegisters();
        }


//////////////////////////////////////////////////////////////////////////
//      AV_GetParameter(Index)                                          //
//                                                                      //
//      This function returns the current value of a video parameter.   //
//                                                                      //
//      Params: Index           Index of parameter to retrieve.         //
//                                                                      //
//      Return: Current value of parameter.                             //
//////////////////////////////////////////////////////////////////////////
WORD _export FAR PASCAL AV_GetParameter(WORD Index)
        {
        if (Index == AVFREEZESTATE)
                {
                // Return state of freeze bit.  Nonzero means frozen.
                return ((AV_GetRegister(INPUT_VIDEO_CONFIGURATION_B) & 0x08) != 0);
                }

        if (Index < NUMPARAMS)
                return Parameters[Index];
        else
                return 0;
        }


//////////////////////////////////////////////////////////////////////////
//      AV_SetIniFilename(lpszIni)                                      //
//                                                                      //
//      This function sets the filename that AVWIN will use for reading //
//      its configuration data.  If the pointer is NULL, the default    //
//      filename AVWIN.INI will be used.  Otherwise the pointer is      //
//      assumed to point to a zero-terminated filename string.          //
//                                                                      //
//      This filename will be used for all subsequent functions which   //
//      use the INI file, such as AV_Initialize, AV_LoadConfiguration,  //
//      and AV_SaveConfiguration.                                       //
//                                                                      //
//      Params: lpszIni         Pointer to filename string.             //
//                                                                      //
//      Return: none                                                    //
//////////////////////////////////////////////////////////////////////////
void _export FAR PASCAL AV_SetIniFilename(LPSTR lpszIni)
        {
        // Set INI file name to use. If ptr is NULL, use AVWIN.INI
        if( lpszIni != NULL )
                lstrcpy( szIniName, lpszIni );
        else
                lstrcpy(szIniName, "AVWIN.INI");
        }


//////////////////////////////////////////////////////////////////////////
//      AV_GetIniFilename(lpszIni, cbSize)                              //
//                                                                      //
//      This function gets the filename that AVWIN will use for reading //
//      its configuration data.                                         //
//                                                                      //
//      This filename will be used for all subsequent functions which   //
//      use the INI file, such as AV_Initialize, AV_LoadConfiguration,  //
//      and AV_SaveConfiguration.                                       //
//                                                                      //
//      Params: lpszIni         Pointer to filename string.             //
//              cbSize          Size of buffer to fill                  //
//                                                                      //
//      Return: none                                                    //
//////////////////////////////////////////////////////////////////////////
void _export FAR PASCAL AV_GetIniFilename(LPSTR lpszIni, WORD cbSize)
        {
        // Get INI file name. 
        if( lpszIni != NULL )
            lstrcpyn( lpszIni, szIniName, cbSize);
        return;
        }


//////////////////////////////////////////////////////////////////////////
//      AV_SetParameter(Index, Value)                                   //
//                                                                      //
//      This function sets a single video parameter to a new value.  It //
//      does not automatically update the video registers.  Use the     //
//      AV_UpdateVideo or AV_CreateWindow functions to do this.         //
//                                                                      //
//      Params: Index           Index of parameter to set.              //
//              Value           New value.                              //
//                                                                      //
//      Return: none                                                    //
//////////////////////////////////////////////////////////////////////////
void _export FAR PASCAL AV_SetParameter(WORD Index, WORD Value)
        {
        if (Index == AVSELECTOR)
                return;

        if (Index < NUMPARAMS)
                Parameters[Index] = Value;
        }


//////////////////////////////////////////////////////////////////////////
//      AV_UpdateShadowedRegisters()                                    //
//                                                                      //
//      This function forces values written to shadowed registers to    //
//      take effect.                                                    //
//                                                                      //
//      Params: none                                                    //
//                                                                      //
//      Return: none                                                    //
//////////////////////////////////////////////////////////////////////////
void _export FAR PASCAL AV_UpdateShadowedRegisters(void)
        {
        WORD Value;

        AV_SetRegister(INPUT_VIDEO_CONFIGURATION_A,
                        AV_GetRegister(INPUT_VIDEO_CONFIGURATION_A) | 0x40 );

        Value = AV_GetRegister(OUTPUT_PROCESSING_CONTROL_B);
        AV_SetRegister(OUTPUT_PROCESSING_CONTROL_B, Value & 0x7F);
        AV_SetRegister(OUTPUT_PROCESSING_CONTROL_B, Value | 0x80);
        }


//////////////////////////////////////////////////////////////////////////
//      AV_UpdateVideo()                                                //
//                                                                      //
//      This function reprograms the video registers for an existing    //
//      window, based on changes in the video parameters.               //
//                                                                      //
//      Params: none (previous AV_CreateWindow parameters will be used).//
//                                                                      //
//      Return: none                                                    //
//////////////////////////////////////////////////////////////////////////
void _export FAR PASCAL AV_UpdateVideo(void)
        {
	if (bUpdateType == 1)						//;;;;
		{							//;;;;
		AV_DrawWindow(OldX, OldY, OldW, OldH, OldDrawWSrc,	//;;;;
			OldDrawHSrc, OldBitmapWSrc, OldBitmapHSrc,	//;;;;
			OldDrawType);					//;;;;
		}
	else	{							//;;;;
	       if ((Parameters[AVINTERLEAVE] == 2) && (QFactorFix == TRUE)) {
	            AV_CreateWindow(OldX, OldY, OldW+2, OldH+2, OldFlag);
	            AV_CreateWindow(OldX, OldY, OldW, OldH, OldFlag);
               QFactorFix = FALSE;
           } 
           else {
   	        AV_CreateWindow(OldX, OldY, OldW, OldH, OldFlag);
           }
	}
  }


//////////////////////////////////////////////////////////////////////////
//      AV_WaitVideoFrame()                                             //
//                                                                      //
//      This function waits for the video to start a new frame.         //
//                                                                      //
//      Params: None                                                    //
//                                                                      //
//      Return: None                                                    //
//////////////////////////////////////////////////////////////////////////
void _export FAR PASCAL AV_WaitVideoFrame(void)
        {
        WORD    wTimeout;

        // Clear status bits and wait for start of even field.
        AV_GetRegister(INPUT_FIELD_PIXEL_BUFFER_STATUS);
        wTimeout = 0xFFFF;
        while (!(AV_GetRegister(INPUT_FIELD_PIXEL_BUFFER_STATUS) & 0x01)
                && wTimeout--);
        }


//////////////////////////////////////////////////////////////////////////
//      Functions for programming the Philips I2C protocol chips.       //
//                                                                      //
//      These functions program the Philips video decoder and related   //
//      chips using the I-squared-C protocol.  This involves toggling   //
//      two output signals, one a clock and the other a data signal.    //
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//      AV_SetExternalRegister(ID, Index, Data)                         //
//                                                                      //
//      This function programs the Philips video decoder and related    //
//      chips using the I-squared-C protocol.                           //
//                                                                      //
//        Params: id            External chip ID (defined by Philips)   //
//                Index         External chip register index            //
//                Data          Data to be programmed.                  //
//                                                                      //
//        Return: error if couldn't write                               //
//////////////////////////////////////////////////////////////////////////
WORD _export FAR PASCAL AV_SetExternalRegister(WORD id, WORD Index, WORD Data)
        {
        int Value, Error;

        // if data includes mask, combine data bits with shadowed register
        if( Data > 0xFF && Index < MAXEXTREG ) {
            if( id == PHsLID ) {
                Data = (ExRegPHLSave[Index] & (Data >> 8)) | (Data & 0xFF);
            } else if( id == PHsHID ) {
                Data = (ExRegPHHSave[Index] & (Data >> 8)) | (Data & 0xFF);
            }
        }

        Error = 0;

        // Set I2C control register to enable I2C clock and data lines.
        AV_SetRegister(I2C_CONTROL, 0);
        Value = 0;

        // Send START.
        _outp(AVDataPort, Value |= I2C_DATA);
        Error |= AVWaitSDA();                   // wait until SDA high
        _outp(AVDataPort, Value |= I2C_CLOCK);
        Error |= AVWaitSCL();                   // wait until SCL high
        _outp(AVDataPort, Value &= ~I2C_DATA);       // start condition
        _outp(AVDataPort, Value &= ~I2C_CLOCK);
        AVDelayI2C();                           // guarantee SCL low time

        // Send programming data.
        Error |= AV_SetExternalData(id);
        Error |= AV_SetExternalData(Index);
        Error |= AV_SetExternalData(Data);

        // Send STOP.
        Value = _inp(AVDataPort);
        _outp(AVDataPort, Value &= ~I2C_DATA);
        _outp(AVDataPort, Value |= I2C_CLOCK);
        Error |= AVWaitSCL();
        _outp(AVDataPort, Value |= I2C_DATA);
        Error |= AVWaitSDA();

        if( Index < MAXEXTREG ) {
            if( id == PHsLID ) {
                ExRegPHLSave[Index] = Data;
            } else if( id == PHsHID ) {
                ExRegPHHSave[Index] = Data;
            }
        }

        return Error;
        }


//////////////////////////////////////////////////////////////////////////
//      AV_SetExternalString(id, Index, *Data)                          //
//                                                                      //
//      This function programs the Philips video decoder and related    //
//      chips using the I-squared-C protocol.                           //
//      It sends a string of values to the device with specified id.    //
//      Uses the autoinc index feature of the chip (just sends data).   //
//                                                                      //
//        Params: id            External chip ID (defined by Philips)   //
//                Index         External chip register index            //
//                far *Data     Far pointer to string of values         //
//                                                                      //
//      The string should be 16 bit values terminated by -1 (0xFFFF)    //
//      The values are truncated to a byte before sending.              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
WORD _export FAR PASCAL AV_SetExternalString(WORD id, WORD Index, int far *Data)
        {
        int     Value, Error;

        Error = 0;

        // Set I2C control register to enable I2C clock and data lines.
        AV_SetRegister(I2C_CONTROL, 0);
        Value = 0;

        // Send START.
        _outp(AVDataPort, Value |= I2C_DATA);
        Error |= AVWaitSDA();                   // wait until SDA high
        _outp(AVDataPort, Value |= I2C_CLOCK);
        Error |= AVWaitSCL();                   // wait until SCL high
        _outp(AVDataPort, Value &= ~I2C_DATA);       // start condition
        _outp(AVDataPort, Value &= ~I2C_CLOCK);
        AVDelayI2C();                           // guarantee SCL low time

        // Send programming data.
        Error |= AV_SetExternalData(id);
        Error |= AV_SetExternalData(Index);

        while( *Data != -1 )
                {
                Error |= AV_SetExternalData(*Data);
                ++Data;
                }

        // Send STOP.
        Value = _inp(AVDataPort);
        _outp(AVDataPort, Value &= ~I2C_DATA);
        _outp(AVDataPort, Value |= I2C_CLOCK);
        Error |= AVWaitSCL();
        _outp(AVDataPort, Value |= I2C_DATA);
        Error |= AVWaitSDA();

        return Error;
        }


//////////////////////////////////////////////////////////////////////////
//      AV_GetExternalRegister(id, Index)                               //
//                                                                      //
//      This function reads chips using the I-squared-C protocol.       //
//      If the index is bigger than 255, the value of the shadowed      //
//        register (if any) is returned.                                //
//                                                                      //
//        Params: id            External chip ID (defined by Philips)   //
//                Index         External chip register index            //
//                                                                      //
//      Returns int value read, or -1 for error                         //
//////////////////////////////////////////////////////////////////////////
WORD _export FAR PASCAL AV_GetExternalRegister(WORD id, WORD Index)
        {
        int     Data, Value, Error;

        Error = 0;

        // index greater than FF means get shadowed register (if any)
        if( Index > 0xFF ) {
            Index &= 0xFF;
            if( Index < MAXEXTREG ) {
                if( id == PHsLID ) {
                    Data = ExRegPHLSave[Index];
                } else if( id == PHsHID ) {
                    Data = ExRegPHHSave[Index];
                } else {
                    Error = TRUE;
                }
            } else {
                Error = TRUE;
            }
        } else {
            // Set I2C control register to enable I2C clock and data lines.
            AV_SetRegister(I2C_CONTROL, 0);
            Value = 0;

            // Send START.
            _outp(AVDataPort, Value |= I2C_DATA);
            Error |= AVWaitSDA();                   // wait until SDA high
            _outp(AVDataPort, Value |= I2C_CLOCK);
            Error |= AVWaitSCL();                   // wait until SCL high
            _outp(AVDataPort, Value &= ~I2C_DATA);       // start condition
            _outp(AVDataPort, Value &= ~I2C_CLOCK);
            AVDelayI2C();                           // guarantee SCL low time

            // Send programming data.
            Error |= AV_SetExternalData(id);
            Error |= AV_SetExternalData(Index);

            // Send START again to reverse transfer
            _outp(AVDataPort, Value |= I2C_DATA);
            Error |= AVWaitSDA();                   // wait until SDA high
            _outp(AVDataPort, Value |= I2C_CLOCK);
            Error |= AVWaitSCL();                   // wait until SCL high
            _outp(AVDataPort, Value &= ~I2C_DATA);       // start condition
            _outp(AVDataPort, Value &= ~I2C_CLOCK);
            AVDelayI2C();                           // guarantee SCL low time

            Error |= AV_SetExternalData(id | 0x01);
            Data = AV_GetExternalData();

            // Send STOP.
            Value = _inp(AVDataPort);
            _outp(AVDataPort, Value &= ~I2C_DATA);
            _outp(AVDataPort, Value |= I2C_CLOCK);
            Error |= AVWaitSCL();
            _outp(AVDataPort, Value |= I2C_DATA);
            Error |= AVWaitSDA();
        }

        if( Error == 0 )
           return Data;
        else
           return 0xFFFF;       // -1
        }


//////////////////////////////////////////////////
//      AV_SetExternalData(Data)                //
//////////////////////////////////////////////////
int AV_SetExternalData(WORD Data)
        {
        int     Ack, Bit, Value;

        Ack = 0;

        // Get current value of I2C control register.
        Value = _inp(AVDataPort);

        // Send bits of new data out one at a time.
        for(Bit = 0x80; Bit != 0; Bit >>= 1 )
                {
                if (Data & Bit)
                        Value |= I2C_DATA;
                else
                        Value &= ~I2C_DATA;

                _outp(AVDataPort, Value);
                _outp(AVDataPort, Value |= I2C_CLOCK);

                Ack |= AVWaitSCL();
                _outp(AVDataPort, Value &= ~I2C_CLOCK);
                AVDelayI2C();
                }

        // Verify ACKNOWLEDGE.
        _outp(AVDataPort, Value |= I2C_DATA);
        _outp(AVDataPort, Value |= I2C_CLOCK);
        AVWaitSCL();

        Ack |= (_inp(AVDataPort) & I2C_RDDATA_D) >> 6;
        _outp(AVDataPort, Value &= ~I2C_CLOCK);
        AVDelayI2C();

        if (Ack==0)
            {
            return 0;
            }
        else
            {
            return -1;
            }
        }


//////////////////////////////////////////////////
//      AV_GetExternalData()                    //
//////////////////////////////////////////////////
int AV_GetExternalData(void)
        {
        int     Ack, Data, Error, Bit, Value;

        Error = 0;
        Data = 0;
        Ack = 0;

        // Get current value of I2C control register.
        Value = _inp(AVDataPort);

        // Get bits of new data out one at a time.
        for(Bit = 0; Bit < 8; Bit++)
                {
                _outp(AVDataPort, Value |= I2C_CLOCK);
                Error |= AVWaitSCL();
                Data <<= 1;
                Data |= (_inp(AVDataPort) & I2C_RDDATA_D);
                _outp(AVDataPort, Value &= ~I2C_CLOCK);
                AVDelayI2C();
                }
        Data >>= 6;

        // Don't Send ACKNOWLEDGE for last byte
        // (and since we only get one at at time, ours is always the last byte)
//      _outp(AVDataPort, Value &= ~I2C_DATA);
//      _outp(AVDataPort, Value |= I2C_CLOCK);
//      AVWaitSCL();

//      Ack |= (_inp(AVDataPort) & I2C_RDDATA_D) >> 6;
//      _outp(AVDataPort, Value &= ~I2C_CLOCK);
//      _outp(AVDataPort, Value |= I2C_DATA);
//      AVDelayI2C();

        if (Ack==0)
           {
           return Data;
           }
        else
           {
           return -1;
           }
        }


//////////////////////////////////////////////////
//      AVWaitSCL()                             //
//////////////////////////////////////////////////
int AVWaitSCL(void)
        {
        int delay;

        // Wait for I2C clock to float high.
        delay = 100;
        while( delay-- )
                {
                if(_inp(AVDataPort) & I2C_RDCLOCK) break;
                }

        // Delay for 4.5 usec to guarantee clock time
        _inp(AVDataPort);
        _inp(AVDataPort);
        _inp(AVDataPort);
        _inp(AVDataPort);
        _inp(AVDataPort);
        _inp(AVDataPort);

        if (delay==0)
           {
           return -1;                     // time out
           }
        else
           {
           return 0;                      // OK
           }
        }


//////////////////////////////////////////////////
//      AVWaitSDA()                             //
//////////////////////////////////////////////////
int AVWaitSDA(void)
        {
        int delay;

        // Wait for I2C data to float high.
        delay = 100;
        while( delay-- )
            {
            if(_inp(AVDataPort) & I2C_RDDATA) break;
            }

        if (delay==0)
            {
            return -1;                     // time out
            }
        else
            {
            return(0);                      // OK
            }
        }


//////////////////////////////////////////////////
//      AVDelayI2C()                            //
//////////////////////////////////////////////////
void AVDelayI2C(void)
        {
        // Delay for 4.5 usec to guarantee clock time
        _inp(AVDataPort);
        _inp(AVDataPort);
        _inp(AVDataPort);
        _inp(AVDataPort);
        _inp(AVDataPort);
        _inp(AVDataPort);
        }
