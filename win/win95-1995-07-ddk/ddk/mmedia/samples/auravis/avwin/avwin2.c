//////////////////////////////////////////////////////////////////////////
//   Module:    AVWIN2.C                                                //
//   Target:    AVWIN.DLL                                               //
//                                                                      //
//   Summary:   This module contains PCV-compatible library functions   //
//		for AVWIN.DLL.						//
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
#include <dos.h>
#include "avwin.h"
#include "avwinrc.h"
#include "global.h"

#include "mmdebug.h"

//////////////////////////////////////////////////////////////////////////
//      AV_HorizontalZoom(Value)                                        //
//                                                                      //
//      This function sets simple horizontal zoom factors 1X, 2X, 4X.   //
//                                                                      //
//      Params: Value           New zoom factor.                        //
//                                                                      //
//      Return: none                                                    //
//////////////////////////////////////////////////////////////////////////
void _export FAR PASCAL AV_HorizontalZoom(int Value)
{
    unsigned int hzr, vpwidth;

    if( Value != nHZoom ) {         // if not already zoomed that amount
          // get the value of viewport width
        vpwidth = AV_GetRegister(DISPLAY_VIEWPORT_WIDTH_A)
                   + ((int)(AV_GetRegister(DISPLAY_VIEWPORT_WIDTH_B) & 0x01) << 8);

          // if horizontal zoom is enabled
        if( AV_GetRegister(OUTPUT_PROCESSING_CONTROL_A) & 0x02 ) {
              // get the value of Hzoom registers
            hzr = AV_GetRegister(OUTPUT_HORIZONTAL_ZOOM_CONTROL_A)
                       + ((int)(AV_GetRegister(OUTPUT_HORIZONTAL_ZOOM_CONTROL_B) & 0x07) << 8);

            switch( Value ) {           // switch on new value
                case 0:                 // disable
                AV_SetRegister( OUTPUT_PROCESSING_CONTROL_A,
                    AV_GetRegister(OUTPUT_PROCESSING_CONTROL_A) & ~0x02 );
                hzr = 0;
                switch( nHZoom ) {          // switch on old value
                    case 2:
                    vpwidth *= 2;
                    break;

                    case 4:
                    vpwidth *= 4;
                    break;
                }
                nHZoom = 0;
                break;

                case 1:
                switch( nHZoom ) {          // switch on old value
                    case 2:
                    hzr *= 2;
                    vpwidth *= 2;
                    break;

                    case 4:
                    hzr *= 4;
                    vpwidth *= 4;
                    break;
                }
                nHZoom = Value;
                break;

                case 2:
                switch( nHZoom ) {          // switch on old value
                    case 1:
                    hzr /= 2;
                    vpwidth /= 2;
                    break;

                    case 4:
                    hzr *= 2;
                    vpwidth *= 2;
                    break;

                }
                nHZoom = Value;
                break;

                case 4:
                switch( nHZoom ) {          // switch on old value
                    case 1:
                    hzr /= 4;
                    vpwidth /= 4;
                    break;

                    case 2:
                    hzr /= 2;
                    vpwidth /= 2;
                    break;
                }
                nHZoom = Value;
                break;
            }
            AV_SetRegister(OUTPUT_HORIZONTAL_ZOOM_CONTROL_A, hzr & 0xFF);
            AV_SetRegister(OUTPUT_HORIZONTAL_ZOOM_CONTROL_B, hzr >> 8);
            AV_SetRegister(DISPLAY_VIEWPORT_WIDTH_A, vpwidth & 0xFF );
            AV_SetRegister(DISPLAY_VIEWPORT_WIDTH_B, vpwidth >> 8 );
        } else {                    // zoom is disabled
            switch( Value ) {       // switch on new value
                case 0:
                nHZoom = 0;
                AV_SetRegister(OUTPUT_HORIZONTAL_ZOOM_CONTROL_A, 0);
                AV_SetRegister(OUTPUT_HORIZONTAL_ZOOM_CONTROL_B, 0);
                break;

                case 1:
                nHZoom = 1;
                AV_SetRegister(OUTPUT_HORIZONTAL_ZOOM_CONTROL_A, 0xFF);     // avoid infinite zoom!
                AV_SetRegister(OUTPUT_HORIZONTAL_ZOOM_CONTROL_B, 0x07);     // set hzr before enable
                AV_SetRegister( OUTPUT_PROCESSING_CONTROL_A,
                    AV_GetRegister(OUTPUT_PROCESSING_CONTROL_A) | 0x02 );
                break;

                case 2:
                nHZoom = 2;
                AV_SetRegister(OUTPUT_HORIZONTAL_ZOOM_CONTROL_A, 0xFF);     // avoid infinite zoom!
                AV_SetRegister(OUTPUT_HORIZONTAL_ZOOM_CONTROL_B, 0x03);     // set hzr before enable
                AV_SetRegister( OUTPUT_PROCESSING_CONTROL_A,
                    AV_GetRegister(OUTPUT_PROCESSING_CONTROL_A) | 0x02 );
                vpwidth /= 2;
                AV_SetRegister(DISPLAY_VIEWPORT_WIDTH_A, vpwidth & 0xFF );
                AV_SetRegister(DISPLAY_VIEWPORT_WIDTH_B, vpwidth >> 8 );
                break;

                case 4:
                nHZoom = 4;
                AV_SetRegister(OUTPUT_HORIZONTAL_ZOOM_CONTROL_A, 0xFF);     // avoid infinite zoom!
                AV_SetRegister(OUTPUT_HORIZONTAL_ZOOM_CONTROL_B, 0x01);     // set hzr before enable
                AV_SetRegister( OUTPUT_PROCESSING_CONTROL_A,
                    AV_GetRegister(OUTPUT_PROCESSING_CONTROL_A) | 0x02 );
                vpwidth /= 4;
                AV_SetRegister(DISPLAY_VIEWPORT_WIDTH_A, vpwidth & 0xFF );
                AV_SetRegister(DISPLAY_VIEWPORT_WIDTH_B, vpwidth >> 8 );
                break;
            }
        }

        AV_UpdateShadowedRegisters();

// debug stuff
//      hzr = AV_GetRegister(OUTPUT_HORIZONTAL_ZOOM_CONTROL_A)
//                     + ((int)(AV_GetRegister(OUTPUT_HORIZONTAL_ZOOM_CONTROL_B) & 0x07) << 8);
//      vpwidth = AV_GetRegister(DISPLAY_VIEWPORT_WIDTH_A)
//                 + ((int)(AV_GetRegister(DISPLAY_VIEWPORT_WIDTH_B) & 0x01) << 8);
//
//      wsprintf(szValue, "HZ = %d  %04X  %04X\n", nHZoom, i, vpwidth );
//      OutputDebugString(szValue);
    }
}


//////////////////////////////////////////////////////////////////////////
//      AV_Initialize()                                                 //
//                                                                      //
//      This function initializes the video system and prepares it for  //
//      use.  It does not enable video or create a video window.  This  //
//      should be the first video function called by an application.    //
//                                                                      //
//      The current video parameters will be read from AVWIN.INI in the //
//      Windows directory.                                              //
//                                                                      //
//      Params: none                                                    //
//                                                                      //
//      Return: 1 (TRUE)        if successful (initialization done).    //
//              0 (FALSE)       if unsuccessful (hardware not found).   //
//////////////////////////////////////////////////////////////////////////
int _export FAR PASCAL AV_Initialize(void)
        {
        BYTE    bCount, i, j;
        WORD    wArray[600];
        WORD	 Index = 0;
        DWORD   dAddress;
        WORD    nAddressLo, nAddressHi, nMemorySize;
        WORD    Value, nInterleave;
        char    szBuf[130];	

        AuxDebugEx (3, DEBUGLINE "AV_Initialize: hInst =%X\r\n", hInst);

        // Assume that initialization will be successful.
        bInitialized = TRUE;
	Parameters[AVINITIALIZED] = FALSE;

        // Figure out which VGA mode we are in, and make szMode string.
        ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
        ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
        NumColors = GetNumColors();
        wsprintf(szMode, "%04dx%04dx%05d", ScreenWidth, ScreenHeight,
                NumColors);
          
        // Get video parameters from AVWIN.INI file.
        AV_LoadConfiguration();

        AVIndexPort = Parameters[AVPORT];
        AVDataPort  = AVIndexPort + 1;
        Parameters[AVNUMCOLORS] = NumColors;
        Parameters[AVCOLORKEY]  = wColorKey;


        AuxDebugEx (3, DEBUGLINE "AV_Initialize: AVIndexPort=%X MemBase=%x IRQ=%d\r\n",
                Parameters[AVPORT], Parameters[AVADDRESS], Parameters[AVIRQLEVEL]);

        AuxDebugEx (3, DEBUGLINE "AV_Initialize: NumColors=%d wColorKey=%x\r\n",
                NumColors, wColorKey);

        // Do a quick test to see if the video hardware is there.
        // If not, return FALSE for failure code.
        bInitialized = FALSE;
        _outp(AVIndexPort, 0x00);
        _outp(AVDataPort,  0xFF);
        Value = _inp(AVIndexPort);

        AuxDebugEx (1, DEBUGLINE "AV_Initialize: Index 0, Wrote FF, Should Read=0, Read=%X\r\n", 
                Value);

        if (Value != 0) return FALSE;
        _outp(AVIndexPort, 0x01);
        _outp(AVDataPort, 0xFF);
        Value = _inp(AVIndexPort);

        AuxDebugEx (1, DEBUGLINE "AV_Initialize: Index 1, Wrote FF, Should Read=1, Read=%X\r\n", 
                Value);

        if (Value != 1) return FALSE;
        bInitialized = TRUE;

	// Read VxP-500 initialization values from AVWIN.INI, and
	// program the chip with them.
        for (i = 1; i < 50; i++)
                {
                wsprintf(szEntry, "Line%d", i);
                bCount = AVGetIniArray((LPWORD)&wArray[Index], 20,
                                    "AVInit", szEntry, "", szIniName );
                if (!bCount)
                        break;
					 Index += bCount;
					 }	//end FOR stmt


	AV_WaitVGARetrace();
	for (j = 0; j < Index; j += 2)
		{
                if (wArray[j] != VGA_CONTROL)
                	AV_SetRegister(wArray[j], wArray[j+1]);
                }
   GetPrivateProfileString( "ExInit", "Decoder", "PH9051411", (char *)wArray,
                          sizeof(wArray), szIniName );

   AuxDebugEx (3, DEBUGLINE "AV_Initialize: Decoder = %s\r\n",
                (LPSTR) (char *)wArray);

   if( (lstrcmp("PH7191422", (char *)wArray) == 0 ) ||
       (lstrcmp("PH7110422", (char *)wArray) == 0 ) ) {
            AV_SetRegister(0x18,0x04);
            AV_SetRegister(0x96,0x18);
    }
   if( (lstrcmp("PH7191422", (char *)wArray) == 0 ) ||
       (lstrcmp("PH9051411", (char *)wArray) == 0 ) ) {
       PHsLID = PHlowADD1;
       PHsHID = PHhighADD1;
   } 
   else if( (lstrcmp("PH7110422", (char *)wArray) == 0 ) ||
       (lstrcmp("PH7110411", (char *)wArray) == 0 ) ) {
       PHsLID = PHlowADD2;
       PHsHID = PHhighADD2;
   } 

   AuxDebugEx (3, DEBUGLINE "AV_Initialize: DecoderAddxHi = %x Lo = %x\r\n",
                PHsHID, PHsLID);



        // Make sure that the video is disabled initially.
	AV_DisableVideo();

       GetPrivateProfileString("EXInit", "Decoder", "PH9051411", szBuf,
                sizeof(szBuf), szIniName);

        // Program External registers using I2C protocol.
        for (i = 1; i < 50; i++)
                {
                wsprintf(szEntry, "Line%d", i);
                bCount = AVGetIniArray((LPWORD)wArray, sizeof(wArray)/sizeof(WORD) ,
                                    szBuf, szEntry, "", szIniName);
                if (!bCount)
                        break;
                for (j = 0; j < bCount; j += 3)
                        AV_SetExternalRegister(wArray[j], wArray[j+1], wArray[j+2]);
                }


        // Make sure that the base address is in the correct range.
        // If not, default to segment B800h.
        // If parameter is 0-15, it is assumed to be an extended
        // memory address in megabytes.
        // If parameter is in the range A000-FFFF, it is assumed to be a
        // real paragraph in the high memory area (640K to 1MB).
        if (Parameters[AVADDRESS] == 0)
                Parameters[AVADDRESS] = 0xB800;
        if ((Parameters[AVADDRESS] > 15) && (Parameters[AVADDRESS] < 0xA000))
                Parameters[AVADDRESS] = 0xB800;

#if 0
        // Get a selector to the the frame buffer lpFrameBuffer.
        if (uFrameSelector == NULL)
                {
                if (Parameters[AVADDRESS] < 16)
                        lpFrameBuffer = (LPWORD)CreatePhysicalSelector(
                                Parameters[AVADDRESS] * 0x100000L, 0xFFFFFL);
                else
                        lpFrameBuffer = (LPWORD)CreateRealSelector(
                                Parameters[AVADDRESS]);
                uFrameSelector = _FP_SEG(lpFrameBuffer);
                Parameters[AVSELECTOR] = uFrameSelector;
                }
#endif

        // Set base address for video frame buffer.
        // The memory window size will always be 32K.
        if (Parameters[AVADDRESS] < 16)
                dAddress = ((DWORD)Parameters[AVADDRESS] & 0xFFFF) << 20;
        else
                dAddress = ((DWORD)Parameters[AVADDRESS] & 0xFFFF) << 4;


        nMemorySize = 1;

#if 0   // For some reason, this original version is now
        // giving the compiler grief.  Replaced with version below.

        nAddressLo = (WORD) ((dAddress >> 14) & 0xFFFF) ;  // Original
#endif
        {
                WORD wH = HIWORD (dAddress);
                WORD wL = LOWORD (dAddress);
                nAddressLo = ((wH & 0x3fff) << 2) | ((wL & 0xc000) >> 14);
        }

        nAddressHi = (WORD) (((dAddress >> 22) & 0x03)
                | (nMemorySize << 2) | 0x20);
        AV_SetRegister(MEMORY_WINDOW_BASE_ADDRESS_A, nAddressLo);
        AV_SetRegister(MEMORY_WINDOW_BASE_ADDRESS_B, nAddressHi);
        AV_SetRegister(MEMORY_PAGE_REGISTER, 0);

        // Program VGA_CONTROL register properly for this VGA mode:
        // If VGA mode is high color, set the VxP-500 registers to match it.
        // If VGA is in 1024x768 mode, set the VxP-500 for interlaced output.
        FixVGAMode();
	if (Parameters[AVVGACONTROL] < 255)
		{
		nVGAControl = Parameters[AVVGACONTROL];
		AV_SetVGAControl(nVGAControl);
		}
	else
		{
		nVGAControl = AV_GetRegister(VGA_CONTROL);
		Parameters[AVVGACONTROL] = nVGAControl;
		}

        // Set acquisition address in VxP500 video buffer to 0.
        AV_SetRegister(ACQUISITION_ADDRESS_A, 0);
        AV_SetRegister(ACQUISITION_ADDRESS_B, 0);
        AV_SetRegister(ACQUISITION_ADDRESS_C, 0);

        // Set display viewport starting address in video buffer to 0.
        AV_SetRegister(DISPLAY_VIEWPORT_STARTING_ADDRESS_A, 0);
        AV_SetRegister(DISPLAY_VIEWPORT_STARTING_ADDRESS_B, 0);
        AV_SetRegister(DISPLAY_VIEWPORT_STARTING_ADDRESS_C, 0);

        // Get interleave parameter, and check/set memory mode based on it.
        nInterleave = Parameters[AVINTERLEAVE];
        Value = AV_GetRegister( MEMORY_CONFIGURATION_REGISTER ) & 0x07;

        // if interleave param not right for the initial mode (from ini file)
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

		  // save the old value for later use
		  QFactorFix = TRUE;

        // Set Video Source
        AV_SetVideoSource( Parameters[AVSOURCE] );

        // Set Hue Control
        Value = Parameters[AVHUE];
        AV_SetExternalRegister(PHsLID, 0x07, Value + 128);

        // Reset some variables.
        OldX = OldY = OldW = OldH = OldFlag = 0;
		  bUpdateType = 0;			//;;;; Update type = Create.

        // Return success code.
		  Parameters[AVINITIALIZED] = TRUE;
			
			
        return TRUE;
        }


//////////////////////////////////////////////////////////////////////////
//      AV_LoadClipboardFormat(hWnd, X, Y)                              //
//                                                                      //
//      This function loads an image from the clipboard into the video  //
//      frame buffer.  The format of the image is determined by the     //
//      image header.                                                   //
//                                                                      //
//      Params: hWnd            Window handle of calling application.   //
//              X               X-coordinate in frame buffer.           //
//              Y               Y-coordinate in frame buffer.           //
//                                                                      //
//      Return: 1               if successful.                          //
//              0               if error writing to frame buffer.       //
//              -1              if error clipboard empty.               //
//              -2              if error clipboard format unknown.      //
//              -3              if error in clipboard data.             //
//              -4              if error allocating memory.             //
//              -5              if error locking memory.                //
//              -6              if error unsupported file format.       //
//////////////////////////////////////////////////////////////////////////
int _export FAR PASCAL AV_LoadClipboardFormat(HWND hWnd, WORD x, WORD y)
{
    int status;
    HGLOBAL hglbTemp;
    HPALETTE   hPal;
    char far *tptr;

    if( IsClipboardFormatAvailable(CF_DIB) ) {
        if( OpenClipboard(hWnd) ) {
            if( hglbTemp = GetClipboardData(CF_DIB) ) {
                tptr = MAKELP(hglbTemp, 0);

                if( tptr ) {
                    status = AV_LoadDIB( tptr, x, y );
                } else {
                    status = -5;
                }
            } else {
                status = -3;
            }
            CloseClipboard();
        } else {
            status = -1;
        }
    } else if( IsClipboardFormatAvailable(CF_BITMAP) ) {
        if( OpenClipboard(hWnd) ) {
            if( hglbTemp = GetClipboardData(CF_BITMAP) ) {
                if( IsClipboardFormatAvailable (CF_PALETTE) ) {
                    hPal = GetClipboardData(CF_PALETTE);
                } else {
                    hPal = GetStockObject(DEFAULT_PALETTE);
                }

                hglbTemp = bitmap2DIB( hglbTemp, hPal );    // convert to DIB

                if( hglbTemp ) {
                    status = AV_LoadDIB( MAKELP(hglbTemp, 0), x, y );
                    GlobalFree( hglbTemp );
                } else {
                    status = -4;
                }
            } else {
                status = -3;
            }
            CloseClipboard();
        } else {
            status = -1;
        }
    } else {
        status = -2;
    }

    return status;
}


//   bitmap2DIB
//
//  hBitmap - Handle to device dependent bitmap compatible with default screen
//                  display device
//  hPal    - Palette to render the DDB. If NULL, use the default palette
//
//  Returns handle of DIB or NULL

HANDLE bitmap2DIB( HBITMAP hBitmap, HPALETTE hPal )
{
    int     nbpp, palsize, widthby;
    BITMAP             bitmap;
    LPBITMAPINFOHEADER lpbmInfoHdr;
    HDC                hMemDC;
    HANDLE             hDIB;
    HPALETTE           hOldPal;

//  HFILE hfile;                // for debug
//  char far * lpbits;
//  BITMAPFILEHEADER bmpfilehdr;

    if( !hBitmap ) return NULL;

    // fill bitmap structure with info
    if( !GetObject(hBitmap, sizeof(bitmap), (LPSTR)&bitmap) ) return NULL;

    nbpp = bitmap.bmPlanes * bitmap.bmBitsPixel;    // calc bits per pixel

    if( nbpp <= 1 ) {
        nbpp = 1;
        palsize = 2 * sizeof(RGBQUAD);
        widthby = (((bitmap.bmWidth+7) >> 3) + 3) & 0xFFFC;
    } else if( nbpp <= 4 ) {
        nbpp = 4;
        palsize = 16 * sizeof(RGBQUAD);
        widthby = (((bitmap.bmWidth+1) >> 1) + 3) & 0xFFFC;
    } else if( nbpp <= 8 ) {
        nbpp = 8;
        palsize = 256 * sizeof(RGBQUAD);
        widthby = (bitmap.bmWidth + 3) & 0xFFFC;
    } else {
        nbpp = 24;
        palsize = 0;
        widthby = ((bitmap.bmWidth * 3) + 3) & 0xFFFC;
    }

    // Allocate memory for the DIB
    hDIB = GlobalAlloc( GPTR, sizeof(BITMAPINFOHEADER) + palsize
                            + ((long)widthby * bitmap.bmHeight) );

    if( !hDIB ) return NULL;

      // fill bitmap info header
    lpbmInfoHdr = (LPBITMAPINFOHEADER) MAKELP( hDIB, 0 );

    lpbmInfoHdr->biSize = sizeof(BITMAPINFOHEADER);
    lpbmInfoHdr->biWidth = bitmap.bmWidth;
    lpbmInfoHdr->biHeight = bitmap.bmHeight;
    lpbmInfoHdr->biPlanes = 1;
    lpbmInfoHdr->biBitCount = nbpp;
    lpbmInfoHdr->biCompression = 0;
    lpbmInfoHdr->biSizeImage = (long)widthby * bitmap.bmHeight;
    lpbmInfoHdr->biXPelsPerMeter = 0;
    lpbmInfoHdr->biYPelsPerMeter = 0;
    lpbmInfoHdr->biClrUsed = 0;
    lpbmInfoHdr->biClrImportant = 0;

     // get a DC to hold the bitmap
    hMemDC = GetDC( NULL );

     // If a palette was passed, select it into the DC
    if( hPal ) {
        hOldPal = SelectPalette( hMemDC, hPal, FALSE );
        RealizePalette( hMemDC );
    } else {
        hOldPal = NULL;
    }

      // Call the driver to fill in the color table and bitmap bits
    if( !GetDIBits(hMemDC, hBitmap, 0, bitmap.bmHeight,
                (LPSTR)lpbmInfoHdr + sizeof(BITMAPINFOHEADER) + palsize,
                        (LPBITMAPINFO)lpbmInfoHdr, DIB_RGB_COLORS) ) {

        GlobalFree( hDIB );
        hDIB = NULL;
    }

    if( hOldPal ) SelectPalette( hMemDC, hOldPal, FALSE );
    ReleaseDC( NULL, hMemDC );

//  Debug stuff, writes DIB to a file
//
//  hfile = _lcreat("testdib.bmp", 0);
//  lpbits = MAKELP( hDIB, 0 );
//    // build bmp file header
//  bmpfilehdr.bfType = 0x4D42;                 // "BM"
//  bmpfilehdr.bfSize = ((long)widthby * bitmap.bmHeight) + palsize + sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);
//  bmpfilehdr.bfReserved1 = 0;
//  bmpfilehdr.bfReserved2 = 0;
//  bmpfilehdr.bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) + palsize;
//
//   // write the file header
//  _hwrite( hfile, (unsigned char huge *)&bmpfilehdr, sizeof(BITMAPFILEHEADER) );
//  _hwrite( hfile, lpbits, ((long)widthby * bitmap.bmHeight) + palsize + sizeof(BITMAPINFOHEADER) );
//  _lclose( hfile );

    return hDIB;
}


//////////////////////////////////////////////////////////////////////////
//      AV_LoadConfiguration()                                          //
//                                                                      //
//      This function loads the current video parameters from the file  //
//      AVWIN.INI in the Windows directory.                             //
//                                                                      //
//      Params: none                                                    //
//                                                                      //
//      Return: TRUE                                                    //
//////////////////////////////////////////////////////////////////////////
int _export FAR PASCAL AV_LoadConfiguration(void)
        {
        WORD    i, nInterleave;
        BYTE    bCount;
        WORD    wArray[100];

        AuxDebugEx (3, DEBUGLINE "AV_LoadConfiguration: nSettingsCount=%d, szIniName=%s\r\n", 
                nSettingsCount, (LPSTR) szIniName);

        // Get the decoder type string, set DecoderType
        GetPrivateProfileString( "ExInit", "Decoder", "PH9051411", (char *)wArray,
                sizeof(wArray), szIniName );

        if( (lstrcmp("PH9051411", (char *)wArray) == 0 ) || 
            (lstrcmp("411", (char *)wArray) == 0 ) ||
            (lstrcmp("PH7110411", (char *)wArray) == 0 ) ) {
            DecoderType = DCD_411;
        } else if( (lstrcmp("PH7191422", (char *)wArray) == 0 ) ||
                   (lstrcmp("422", (char *)wArray) == 0 ) ||
                   (lstrcmp("PH7110422", (char *)wArray) == 0 ) ) {
            DecoderType = DCD_422;
        } else {
            DecoderType = DCD_411;      // default is generic 411
        }


        // Get parameters from [Settings] section of the INI file.
        for (i = 0; i < nSettingsCount; i++)
                {
                bCount = AVGetIniArray((LPWORD)wArray, 1, "AVSettings",
                        enSettingsEntries[i].enString, "", szIniName);
                if (bCount == 1)
                        Parameters[enSettingsEntries[i].enIndex] = wArray[0];
                else
                        Parameters[enSettingsEntries[i].enIndex]
				= DefaultParam[enSettingsEntries[i].enIndex];
                AuxDebugEx (3, DEBUGLINE "CFG%2d %s = %X, %X\r\n", 
                        i, enSettingsEntries[i].enString, wArray[0], Parameters[enSettingsEntries[i].enIndex]);
                }

	// Determine video memory size based on interleave and decoder type.
	nInterleave = Parameters[AVINTERLEAVE];
	if (nInterleave < 2)		nInterleave = 2;
	if (nInterleave > 4)		nInterleave = 4;
	if ((DecoderType == DCD_422) && (nInterleave > 3))
		nInterleave = 3;
	Parameters[AVINTERLEAVE] = nInterleave;
	if (DecoderType == DCD_422)	nInterleave++;
	switch (nInterleave)
		{
		case 2:
		Parameters[AVMEMORYSIZE] = 768;
		break;

		case 3:
		Parameters[AVMEMORYSIZE] = 1024;
		break;

		case 4:
		Parameters[AVMEMORYSIZE] = 1536;
		}

        // Look up parameters for our VGA mode in the INI mode table.
        // If no entry is found, use reasonable defaults.
        for (i = 0; i < nModesCount; i++) wArray[i] = 0;
        bCount = AVGetIniArray((LPWORD)wArray, sizeof(wArray)/sizeof(WORD),
                        "Modes", szMode, "", szIniName);
        if (bCount == (BYTE)nModesCount)
                {
                for (i = 0; i < nModesCount; i++)
                        Parameters[enModesEntries[i].enIndex] = wArray[i];
                }
	else
                {
                for (i = 0; i < nModesCount; i++)
                        Parameters[enModesEntries[i].enIndex]
				= DefaultParam[enModesEntries[i].enIndex];
		Parameters[AVVGACONTROL] = nVGAControl;
                }

        // Set some parameters to default values if they are missing.
        if (Parameters[AVCROPLEFT] == 0)
                Parameters[AVCROPLEFT] = 20;
        if (Parameters[AVCROPRIGHT] == 0)
                Parameters[AVCROPRIGHT] = 700;
        if (Parameters[AVCROPTOP] == 0)
                Parameters[AVCROPTOP] = 26;
        if (Parameters[AVCROPBOTTOM] == 0)
                Parameters[AVCROPBOTTOM] = 506;
        if ((Parameters[AVQFACTOR] == 0) || (Parameters[AVQFACTOR] > 100))
                Parameters[AVQFACTOR] = 100;
        if (Parameters[AVINTERLEAVE] == 0)
                Parameters[AVINTERLEAVE] = 4;

        AuxDebugEx (3, DEBUGLINE "AV_LoadConfiguration: AVIndexPort=%X MemBase=%x IRQ=%d\r\n",
                Parameters[AVPORT], Parameters[AVADDRESS], Parameters[AVIRQLEVEL]);

        return TRUE;
        }


//////////////////////////////////////////////////////////////////////////
//      AV_LoadImageRect(lpFilename, X, Y)                              //
//                                                                      //
//      This function loads an image file into the video frame buffer.  //
//        The format of the image is determined by the file header.     //
//                                                                      //
//      Params: lpFilename      Far pointer to filename string.         //
//              X               X-coordinate in frame buffer.           //
//              Y               Y-coordinate in frame buffer.           //
//                                                                      //
//      Return: 1               if successful.                          //
//              0               if error writing to frame buffer.       //
//              -1              if error opening file (file not found). //
//              -2              if error allocating memory.             //
//              -3              if error loading file (corrupted file). //
//              -4              if error unrecognized file format.      //
//////////////////////////////////////////////////////////////////////////
int _export FAR PASCAL AV_LoadImageRect(LPSTR lpFilename, WORD x, WORD y)
{
    int status;
    HFILE hfile;
    HGLOBAL hglbPIC;

    hfile = _lopen(lpFilename, READ);
    if( hfile == HFILE_ERROR ) return -1;

    status = 1;

    switch( getfiletype(hfile) ) {
        case -2:
        status = -4;            // unsupported file format
        break;

        case -1:
        status = -3;            // read error
        break;

        case BM_YUV411:
        switch( hglbPIC = loadpicfile(hfile) ) {
            case 0:
            status = -2;        // memory error
            break;

            case (HGLOBAL)-1:
            status = -3;        // read error
            break;

            default:
            displaymmp( MAKELP(hglbPIC, 0), x, y );
            GlobalFree(hglbPIC);
            break;
        }
        break;

        case BM_DIB1P:
        case BM_DIB4P:
        case BM_DIB8P:
        case BM_DIB24:
        switch( hglbPIC = loadpicfile(hfile) ) {
            case 0:
            status = -2;        // memory error
            break;

            case (HGLOBAL)-1:
            status = -3;        // read error
            break;

            default:
            displaybmp( (LPBYTE)MAKELP(hglbPIC, 0) + sizeof(BITMAPFILEHEADER), x, y );
            GlobalFree(hglbPIC);
            break;
        }
        break;

        default:
        status = -4;        // unsupported file format
        break;
    }

    _lclose( hfile );
    return status;
}


//////////////////////////////////////////////////////////////////////////
//      AV_LoadDIB(lpFilename, X, Y)                                    //
//                                                                      //
//      This function loads a DIB into the video frame buffer.          //
//                                                                      //
//      Params: lpFilename      Far pointer to DIB buffer.              //
//              X               X-coordinate in frame buffer.           //
//              Y               Y-coordinate in frame buffer.           //
//                                                                      //
//      Return: 1               if successful.                          //
//              0               if error writing to frame buffer.       //
//////////////////////////////////////////////////////////////////////////
int _export FAR PASCAL AV_LoadDIB(LPSTR lpDIB, WORD x, WORD y)
{
    return( displaybmp(lpDIB, x, y) );
}


//////////////////////////////////////////////////////////////////////////
//      AV_PanWindow(X, Y)                                              //
//                                                                      //
//      This function selects the part of the frame buffer to be        //
//      displayed in the video window, to allow panning (scrolling)     //
//      of video that is larger than the video window.                  //
//                                                                      //
//      Params: X               X-coordinate in frame buffer.           //
//              Y               Y-coordinate in frame buffer.           //
//                                                                      //
//      Return: TRUE                                                    //
//////////////////////////////////////////////////////////////////////////
int _export FAR PASCAL AV_PanWindow(WORD x, WORD y)
{
    unsigned int i, memmode, ileave;
    int frwidth;
    unsigned long pos;

    memmode = AV_GetRegister(0x18) & 0x07;      // get memory mode
    ileave = interleave[memmode];               // get interleave value
    i = AV_GetRegister(0x73) & 0x03;
    frwidth = framewidth[ memmode ][ i ];

    // calc view port position in video memory
    pos = ((long)y * (frwidth/ileave)) + ((x/ileave) & panmask[memmode]);

    AV_SetRegister( 0xA0, (unsigned int)(pos & 0xFF) );
    AV_SetRegister( 0xA1, (unsigned int)((pos>>8) & 0xFF) );
    AV_SetRegister( 0xA2, (unsigned int)((pos>>16) & 0x03) );

    AV_UpdateShadowedRegisters();
    return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//      AV_ReadImageRect(lpData, X, Y, W, H, Type)                      //
//                                                                      //
//      This function reads an image from the video frame buffer and    //
//      copies it to a memory buffer supplied by the application.       //
//                                                                      //
//      Params: lpData          Far pointer to buffer to return image.  //
//              X               X-coordinate in frame buffer.           //
//              Y               Y-coordinate in frame buffer.           //
//              W               Width of image.                         //
//              H               Height of image.                        //
//              Type            Type of image to transfer:              //
//                              BM_DIB8P       8-bit  BMP format        //
//                              BM_DIB24       24-bit BMP format        //
//                              BM_YUV411      IBM MMotion MMP format   //
//                                                                      //
//      Return: 1               if successful.                          //
//              0               if error reading image.                 //
//////////////////////////////////////////////////////////////////////////
int _export FAR PASCAL AV_ReadImageRect(LPSTR lpData, WORD x, WORD y, WORD w, WORD h, WORD Type)
{
    int status, width;
    LPBITMAPFILEHEADER lpbmpfilehdr;
    LPBITMAPINFOHEADER lpbmpinfohdr;

    switch( Type ) {
        case BM_YUV411:
        status = readimagemmp( lpData, x, y, w, h );
        break;

        case BM_DIB24:
        // read DIB into buffer, leave room for BMP file header
        status = readdib24( lpData + sizeof(BITMAPFILEHEADER), x, y, w, h );

          // store bmp file header stuff
        lpbmpfilehdr = (LPBITMAPFILEHEADER)lpData;
        lpbmpinfohdr = (LPBITMAPINFOHEADER)(lpData + sizeof(BITMAPFILEHEADER));

        // get width in bytes, pad to dword
        width = (unsigned int)((lpbmpinfohdr->biWidth * 3) + 3) & 0xFFFC;

        lpbmpfilehdr->bfType = 0x4D42;                 // "BM"
        lpbmpfilehdr->bfSize = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER)
                + ((long)width  * lpbmpinfohdr->biHeight);

        lpbmpfilehdr->bfReserved1 = 0;
        lpbmpfilehdr->bfReserved2 = 0;
        lpbmpfilehdr->bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);
        break;

        default:
        status = 0;         // unsupported file format
        break;
    }

    return status;
}


//////////////////////////////////////////////////////////////////////////
//      AV_ReadDIB(lpData, X, Y, W, H, Type)                            //
//                                                                      //
//      This function reads a DIB from the video frame buffer and       //
//      copies it to a memory buffer supplied by the application.       //
//                                                                      //
//      Params: lpData          Far pointer to buffer to return image.  //
//              X               X-coordinate in frame buffer.           //
//              Y               Y-coordinate in frame buffer.           //
//              W               Width of image.                         //
//              H               Height of image.                        //
//              Type            Type of image to transfer:              //
//                              BM_DIB8P       8-bit  BMP format        //
//                              BM_DIB24       24-bit BMP format        //
//                                                                      //
//      Return: 1               if successful.                          //
//              0               if error reading image.                 //
//////////////////////////////////////////////////////////////////////////
int _export FAR PASCAL AV_ReadDIB(LPSTR lpData, WORD x, WORD y, WORD w, WORD h, WORD Type)
{
    int status;

    switch( Type ) {
        case BM_DIB24:
        status = readdib24( lpData, x, y, w, h );
        break;

        default:
        status = 0;         // unsupported file format
        break;
    }

    return status;
}


//////////////////////////////////////////////////////////////////////////
//      AV_ResetColors()                                                //
//                                                                      //
//      This function resets color parameters to their default values.  //
//                                                                      //
//      Params: None                                                    //
//                                                                      //
//      Return: None                                                    //
//////////////////////////////////////////////////////////////////////////
void _export FAR PASCAL AV_ResetColors(void)
        {
        // Reset color parameters to their default values.
        Parameters[AVBRIGHTNESS] = DefaultParam[AVBRIGHTNESS];
        Parameters[AVSATURATION] = DefaultParam[AVSATURATION];
        Parameters[AVCONTRAST] = DefaultParam[AVCONTRAST];
        Parameters[AVHUE] = DefaultParam[AVHUE];

        // Set the registers with these new values.
        AV_SetRegister(BRIGHTNESS_CONTROL, Parameters[AVBRIGHTNESS]);
        AV_SetRegister(SATURATION_CONTROL, Parameters[AVSATURATION]);
        AV_SetRegister(CONTRAST_CONTROL, Parameters[AVCONTRAST]);
        AV_SetExternalRegister(PHsLID, 0x07, Parameters[AVHUE]);
        }


//////////////////////////////////////////////////////////////////////////
//      AV_ResetSkewFactors()                                           //
//                                                                      //
//      This function resets skew parameters to their default values.   //
//                                                                      //
//      Params: None                                                    //
//                                                                      //
//      Return: None                                                    //
//////////////////////////////////////////////////////////////////////////
void _export FAR PASCAL AV_ResetSkewFactors(void)
        {
        // Reset skew parameters to their default values.
        Parameters[AVXPOSITION] = DefaultParam[AVXPOSITION];
        Parameters[AVYPOSITION] = DefaultParam[AVYPOSITION];

        // Set the registers with these new values.
        AV_UpdateVideo();
        }


//////////////////////////////////////////////////////////////////////////
//      AV_SaveClipboardFormat(hWnd, X, Y, W, H, Option, Type)          //
//                                                                      //
//      This function saves an image file from the video frame buffer   //
//      to the clipboard in the specified format.                       //
//                                                                      //
//      Params: hWnd            Window handle of calling application.   //
//              x               X-coordinate in frame buffer.           //
//              y               Y-coordinate in frame buffer.           //
//              w               Width of image in pixels.               //
//              h               Height of image in pixels.              //
//              Option          Set to 0.                               //
//              Type            Image type:                             //
//                              BM_DIB8P        8-bit  BMP format       //
//                              BM_DIB24        24-bit BMP format       //
//                                                                      //
//      Return: 1               if successful.                          //
//              0               if error reading frame buffer.          //
//              -1              if error creating file (disk full?).    //
//              -2              if error allocating memory.             //
//              -3              if error writing file (disk full?).     //
//              -4              if error creating palette.              //
//              -5              if unsupported clipboard format.        //
//////////////////////////////////////////////////////////////////////////
int _export FAR PASCAL AV_SaveClipboardFormat(HWND hWnd, WORD x, WORD y, WORD w,
        WORD h, WORD Option, WORD Type)
{
    int status;
    HGLOBAL hglbTemp;
    char far *tptr;

    if( Type == BM_DIB24 ) {
        hglbTemp = GlobalAlloc( GPTR, sizeof(BITMAPINFOHEADER)
                        + ( (((w * 3) + 3) & 0xFFFC) * (unsigned long)h ) );

        if( hglbTemp ) {
            if( tptr = MAKELP(hglbTemp, 0) ) {
                status = readdib24( tptr, x, y, w, h );     // get 24 bit DIB in memory buffer

                if( OpenClipboard(hWnd) ) {
                    EmptyClipboard();
                    SetClipboardData(CF_BITMAP  ,NULL);
                    SetClipboardData(CF_PALETTE ,NULL);
                    SetClipboardData(CF_DIB, hglbTemp);
                    CloseClipboard ();
                    status = 1;
                } else {
                    status = -1;                // can't open clipboard
                    GlobalFree( hglbTemp );
                }
            } else {
                status = -2;    // memory error
            }
        } else {
            status = -2;
        }
    } else {
        status = -5;
    }

    return status;
}


