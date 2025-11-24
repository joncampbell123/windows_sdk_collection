//////////////////////////////////////////////////////////////////////////
//   Module:    AVWIN3.C                                                //
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
//      AV_SaveConfiguration()                                          //
//                                                                      //
//      This function saves the current video parameters to the file    //
//      AVWIN.INI in the Windows directory.                             //
//                                                                      //
//      Params: none                                                    //
//                                                                      //
//      Return: TRUE                                                    //
//////////////////////////////////////////////////////////////////////////
int _export FAR PASCAL AV_SaveConfiguration(void)
        {
        WORD    i;
        char    szBuffer[128];

        AuxDebugEx (3, DEBUGLINE "AV_SaveConfiguration: nSettingsCount=%d, szIniName=%s\r\n", 
                nSettingsCount, (LPSTR) szIniName);

        // Write video parameters to [Settings] section of INI file.
        for (i = 0; i < nSettingsCount; i++)
                {
                lstrcpy(szEntry, enSettingsEntries[i].enString);
                wsprintf(szValue, enSettingsEntries[i].enFormat,
                        Parameters[enSettingsEntries[i].enIndex]);
                WritePrivateProfileString("AVSettings", szEntry, szValue,
                        szIniName);
                }

        // Save mode-dependent information in the [Modes] section.
        szBuffer[0] = '\0';
        for (i = 0; i < nModesCount; i++)
                {
                wsprintf(szValue, enModesEntries[i].enFormat,
                        Parameters[enModesEntries[i].enIndex]);
                lstrcat(szBuffer, szValue);
                }
        WritePrivateProfileString("Modes", szMode, szBuffer, szIniName);

        return TRUE;
        }


//////////////////////////////////////////////////////////////////////////
//      AV_SaveImageRect(lpFilename, X, Y, W, H, Option, Type)          //
//                                                                      //
//      This function saves an image file from the video frame buffer.  //
//                                                                      //
//      Params: lpFilename      Far pointer to filename string.         //
//              X               X-coordinate in frame buffer.           //
//              Y               Y-coordinate in frame buffer.           //
//              W               Width of image in pixels.               //
//              H               Height of image in pixels.              //
//              Option          Set to 0.                               //
//              Type            Image file type:                        //
//                              BM_DIB8P        8-bit  BMP format       //
//                              BM_DIB24        24-bit BMP format       //
//                              BM_YUV411       IBM MMotion MMP format  //
//                                                                      //
//      Return: 1               if successful.                          //
//              0               if error reading frame buffer.          //
//              -1              if error creating file (disk full?).    //
//              -2              if error allocating memory.             //
//              -3              if error writing file (disk full?).     //
//              -4              if error creating palette.              //
//              -5              if unsupported format.                  //
//////////////////////////////////////////////////////////////////////////
int _export FAR PASCAL AV_SaveImageRect(LPSTR lpFilename, WORD x, WORD y, WORD w,
WORD h, WORD Option, WORD Type)
{
    int status;
    HFILE hfile;

    status = 1;     // assume OK

    hfile = _lcreat(lpFilename, 0);

    if( hfile == HFILE_ERROR ) {
        return -1;
    }

    switch( Type ) {
        case BM_DIB8P:
        status = capturebmp256( hfile, x, y, w, h );
        break;

        case BM_DIB24:
        status = capturebmp24( hfile, x, y, w, h );
        break;

        case BM_YUV411:
        status = capturemmp( hfile, x, y, w, h );
        break;

        default:
        status = -5;
        break;
    }

    _lclose( hfile );
    return status;
}


//////////////////////////////////////////////////////////////////////////
//      AV_SetAcquisitionWindow(Left, Top, Right, Bottom)               //
//                                                                      //
//      This function sets the rectangle in the video frame buffer to   //
//      use for video acquisition.                                      //
//                                                                      //
//      Params: Left            Left edge in frame buffer.              //
//              Top             Top edge in frame buffer.               //
//              Right           Right edge in frame buffer.             //
//              Bottom          Bottom edge in frame buffer.            //
//                                                                      //
//      Return: TRUE                                                    //
//////////////////////////////////////////////////////////////////////////
int _export FAR PASCAL AV_SetAcquisitionWindow(WORD Left, WORD Top, WORD Right, WORD Bottom)
        {

        Top &= 0x3FE;           // should be even
        Bottom &= 0x3FE;

        // Get crop rectangle
        Parameters[AVCROPLEFT]  = Left;
        Parameters[AVCROPRIGHT] = Right;
        Parameters[AVCROPTOP]   = Top;
        Parameters[AVCROPBOTTOM]= Bottom;

        // Set crop window
        AV_SetRegister(INPUT_HORIZONTAL_CROPPING_LEFT_A, Left);
        AV_SetRegister(INPUT_HORIZONTAL_CROPPING_LEFT_B, Left >> 8);
        AV_SetRegister(INPUT_HORIZONTAL_CROPPING_RIGHT_A, Right);
        AV_SetRegister(INPUT_HORIZONTAL_CROPPING_RIGHT_B, Right >> 8);
        AV_SetRegister(INPUT_HORIZONTAL_CROPPING_TOP_A, Top);
        AV_SetRegister(INPUT_HORIZONTAL_CROPPING_TOP_B, Top >> 8);
        AV_SetRegister(INPUT_HORIZONTAL_CROPPING_BOTTOM_A, Bottom);
        AV_SetRegister(INPUT_HORIZONTAL_CROPPING_BOTTOM_B, Bottom >> 8);

        return TRUE;
        }


//////////////////////////////////////////////////////////////////////////
//      AV_SetCaptureAddress(X, Y)                                      //
//                                                                      //
//      This function sets the address in the video frame buffer to     //
//      use for video acquisition.                                      //
//                                                                      //
//      Params: x               X coordinate in frame buffer.           //
//              y               Y coordinate in frame buffer.           //
//                                                                      //
//      Return: TRUE                                                    //
//////////////////////////////////////////////////////////////////////////
int _export FAR PASCAL AV_SetCaptureAddress(WORD x, WORD y)
        {
        unsigned int memmode, ileave, frwidth;
        unsigned long pos;

        memmode = AV_GetRegister(0x18) & 0x07;      // get memory mode
        ileave = interleave[memmode];               // get interleave value
        frwidth = framewidth[ memmode ][ AV_GetRegister(0x73) & 0x03 ];

        // calc acquisition position in video memory
        pos = ((long)y * (frwidth/ileave)) + ((x/ileave) & panmask[memmode]);

        AV_SetRegister( 0x70, (unsigned int)(pos & 0xFF) );
        AV_SetRegister( 0x71, (unsigned int)((pos>>8) & 0xFF) );
        AV_SetRegister( 0x72, (unsigned int)((pos>>16) & 0x03) );

        AV_UpdateShadowedRegisters();
        return TRUE;
        }


//////////////////////////////////////////////////////////////////////////
//      AV_SetColor(Index, Value)                                       //
//                                                                      //
//      This function sets the value of a color parameter.              //
//                                                                      //
//      Params: Index           Index of parameter to set:              //
//                              BRIGHTNESS                              //
//                              SATURATION                              //
//                              CONTRAST                                //
//                              HUE                                     //
//              Value           Value to set.                           //
//                                                                      //
//      Return: None                                                    //
//////////////////////////////////////////////////////////////////////////
void _export FAR PASCAL AV_SetColor(WORD Index, BYTE Value)
        {
        switch (Index)
                {
                case BRIGHTNESS:
                Parameters[AVBRIGHTNESS] = Value;
                AV_SetRegister(BRIGHTNESS_CONTROL, Value);
                break;

                case SATURATION:
                Parameters[AVSATURATION] = Value;
                AV_SetRegister(SATURATION_CONTROL, Value);
                break;

                case CONTRAST:
                Parameters[AVCONTRAST] = Value;
                AV_SetRegister(CONTRAST_CONTROL, Value);
                break;

                case HUE:
                Parameters[AVHUE] = Value;
                AV_SetExternalRegister(PHsLID, 0x07, Value);
                break;

                default:
                break;
                }
        }


//////////////////////////////////////////////////////////////////////////
//      AV_SetColorKey(Value)                                           //
//                                                                      //
//      This function sets the value of the color key index.  This      //
//      determines which VGA palette color is used for color keying.    //
//                                                                      //
//      Params: Value           Value to set.                           //
//                                                                      //
//      Return: TRUE                                                    //
//////////////////////////////////////////////////////////////////////////
int _export FAR PASCAL AV_SetColorKey(WORD Value)
        {
        Parameters[AVCOLORKEY] = Value;

        // Set color key register.
        // Use given palette entry value if 256-color mode.
        // Look up attribute code if 16-color mode.
        Value = Parameters[AVCOLORKEY];
        if (NumColors == 16)
                Value = AVColorKeyTable[Value & 15];
        AV_SetRegister(KEY_COLOR, Value);

        return TRUE;
        }


//////////////////////////////////////////////////////////////////////////
//      AV_SetDisplayWindow(X, Y, W, H)                                 //
//                                                                      //
//      This function sets the rectangle on the screen to use for       //
//      video display.                                                  //
//                                                                      //
//      Params: X               X-coordinate of window.                 //
//              Y               Y-coordinate of window.                 //
//              W               Width of window.                        //
//              H               Height of window.                       //
//                                                                      //
//      Return: TRUE                                                    //
//////////////////////////////////////////////////////////////////////////
int _export FAR PASCAL AV_SetDisplayWindow(WORD X, WORD Y, WORD W, WORD H)
        {
        // To be implemented.
        return TRUE;
        }


//////////////////////////////////////////////////////////////////////////
//      AV_SetInputFormat(Value)                                        //
//                                                                      //
//      This function sets the video format (NTSC or PAL).              //
//                                                                      //
//      Params: Value           New video format                        //
//                              CF_PAL                PAL format.       //
//                              CF_NTSC               NTSC format.      //
//                                                                      //
//      Return: TRUE                                                    //
//////////////////////////////////////////////////////////////////////////
int _export FAR PASCAL AV_SetInputFormat(WORD Value)
        {
        int i, j;
        WORD wArray[132];
		  char szBuf[130];
 
        GetPrivateProfileString("EXInit", "Decoder", "PH9051411", szBuf,
                sizeof(szBuf), szIniName);

        switch( Value ) {
            case CF_PAL:
            Parameters[AVINPUTFORMAT] = CF_PAL;
            i = AVGetIniArray( (LPWORD)wArray, 132, szBuf, "SetPal",
                    "(8Ah,06h,02h) (8Ah,08h,38h)", szIniName );
            break;

            case CF_NTSC:
            Parameters[AVINPUTFORMAT] = CF_NTSC;
            i = AVGetIniArray( (LPWORD)wArray, 132, szBuf, "SetNTSC",
                    "(8Ah,06h,62h) (8Ah,08h,77h)", szIniName );
            break;

            case CF_YC:
            i = AVGetIniArray( (LPWORD)wArray, 132, szBuf, "SetYC",
                    "(8Ah,0Ah,22h)", szIniName );
            break;

            case CF_COMPOSITE:
            i = AVGetIniArray( (LPWORD)wArray, 132, szBuf, "SetComposite",
                    "(8Ah,0Ah,42h)", szIniName );
            break;

            case CF_COMPRESS:
            i = AVGetIniArray( (LPWORD)wArray, 132, szBuf, "SetCompress",
                    "", szIniName );
            break;

            case CF_EXPAND:
            i = AVGetIniArray( (LPWORD)wArray, 132, szBuf, "SetExpand",
                    "", szIniName );
            break;

            case CF_INPUT0:
            i = AVGetIniArray( (LPWORD)wArray, 132, szBuf, "SetInput0",
                    "(8Ah,0Ah,42h)", szIniName );   // set 0 for composite (22h for Svideo on 0)
            break;

            case CF_INPUT1:
            i = AVGetIniArray( (LPWORD)wArray, 132, szBuf, "SetInput1",
                    "(8Ah,0Ah,4Ah)", szIniName );   // set 1 for composite (2Ah for Svideo on 1)
            break;

            case CF_INPUT2:
            i = AVGetIniArray( (LPWORD)wArray, 132, szBuf, "SetInput1",
                    "(8Ah,0Ah,52h)", szIniName );   // set 2 for composite (32h for Svideo on 2)
            break;
        }

        for( j = 0; j < i; j += 3 ) {
            AV_SetExternalRegister(wArray[j], wArray[j+1], wArray[j+2]);
        }

        return TRUE;
        }


//////////////////////////////////////////////////////////////////////////
//      AV_SetPortAddress(Value)                                        //
//                                                                      //
//      This function sets the current I/O port address.                //
//                                                                      //
//      Params: Value           New port to set.                        //
//                                                                      //
//      Return: TRUE                                                    //
//////////////////////////////////////////////////////////////////////////
int _export FAR PASCAL AV_SetPortAddress(int Value)
        {
        Parameters[AVPORT] = Value;
        AVIndexPort = Value;
        AVDataPort = Value + 1;
        return TRUE;
        }


//////////////////////////////////////////////////////////////////////////
//      AV_SetRegister(Index, Data)                                     //
//                                                                      //
//      This function programs a single video register.                 //
//                                                                      //
//      Params: Index           Index of register to set.               //
//              Data            New value.                              //
//                                                                      //
//      Return: none                                                    //
//////////////////////////////////////////////////////////////////////////
void _export FAR PASCAL AV_SetRegister(WORD Index, WORD Data)
        {
        _outp(AVIndexPort, Index);
        _outp(AVDataPort, Data);
        }

//////////////////////////////////////////////////////////////////////////
//      AV_SetSamsungRegister(Index, Data)                                     //
//                                                                      //
//      This function programs a single video register.                 //
//                                                                      //
//      Params: Index           Index of register to set.               //
//              Data            New value.                              //
//                                                                      //
//      Return: none                                                    //
//////////////////////////////////////////////////////////////////////////
//void _export FAR PASCAL AV_SetRegister(WORD Index, WORD Data)
//        {
//        _outp(Port, Index);
//        _outp(AVDataPort, Data);
//        }


//////////////////////////////////////////////////////////////////////////
//      AV_SetSkewFactor(Index, Value)                                  //
//                                                                      //
//      This function sets the value of a skew (position adjustment)    //
//      parameter.                                                      //
//                                                                      //
//      Params: Index           Index of parameter to set.              //
//                              DISPWINSKEWX           X adjustment.    //
//                              DISPWINSKEWY           Y adjustment.    //
//              Value           Value to set.                           //
//                                                                      //
//      Return: TRUE                                                    //
//////////////////////////////////////////////////////////////////////////
int _export FAR PASCAL AV_SetSkewFactor(int Index, WORD Value)
        {
        switch (Index)
                {
                case SF_DISPWINSKEWX:
                Parameters[AVXPOSITION] = Value;
                break;

                case SF_DISPWINSKEWY:
                Parameters[AVYPOSITION] = Value;
                break;

                default:
                break;
                }

        // Redraw video window to use this factor.
        AV_UpdateVideo();
        return TRUE;
        }


//////////////////////////////////////////////////////////////////////////
//      AV_SetVideoAddress(Value)                                       //
//                                                                      //
//      This function sets the current video memory base address.       //
//      If this is a number in the range 0-15, it represents an address //
//      in extended memory in megabytes.  If this is a number between   //
//      0xA000-0xF000, it represents a physical memory paragraph.       //
//                                                                      //
//      Params: Value           New base address to set.                //
//                                                                      //
//      Return: TRUE    if successful                                   //
//              FALSE   if unsuccessful                                 //
//////////////////////////////////////////////////////////////////////////

int _export FAR PASCAL AV_SetVideoAddress(int Value)
        {
        DWORD   dAddress;
        WORD    nAddressLo, nAddressHi, nMemorySize;

        if (!bInitialized)
                return FALSE;

        // Make sure that the base address is in the correct range.
        // If not, default to segment B800h.
        // If parameter is 0-15, it is assumed to be an extended
        // memory address in megabytes.
        // If parameter is in the range A000-FFFF, it is assumed to be a
        // real paragraph in the high memory area (640K to 1MB).
        if (Value == 0)
                Value = 0xB800;
        if ((Value > 15) && (Value < 0xA000))
                Value = 0xB800;
        Parameters[AVADDRESS] = Value;

        // Set base address for video frame buffer.
        if (Parameters[AVADDRESS] < 16)
                dAddress = ((DWORD)Parameters[AVADDRESS] & 0xFFFF) << 20;
        else
                dAddress = ((DWORD)Parameters[AVADDRESS] & 0xFFFF) << 4;

        nMemorySize = 1 ;
#if 0   // For some reason, this original version is now
        // giving the compiler grief.  Replaced with version below.

        nAddressLo = (WORD) ((dAddress >> 14) & 0xFFFF) ;
        nAddressLo = (WORD) ((dAddress & 0xFFFF) >> 14) ;
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

        // Get a selector to the the frame buffer lpFrameBuffer.
        if (uFrameSelector == NULL)
                {
                if (Parameters[AVADDRESS] < 16)
                        {
                        lpFrameBuffer = (LPWORD)CreatePhysicalSelector(
                                Parameters[AVADDRESS] * 0x100000L, 0xFFFFFL);
                        }
                else
                        {
                        lpFrameBuffer = (LPWORD)CreateRealSelector(
                                Parameters[AVADDRESS]);
                        }
                uFrameSelector = _FP_SEG(lpFrameBuffer);
                Parameters[AVSELECTOR] = uFrameSelector;
                }

        return TRUE;
        }


//--------------------------------------------------------------------------
//  
//  int AV_SetVideoAddressEx
//  
//  Description:
//  
//  
//  Parameters:
//      WORD wSegment
//  
//      WORD wSelector
//  
//  Return (int):
//  
//  History:   Date       Author      Comment
//              7/31/94   BryanW      Modified for seg/sel
//  
//--------------------------------------------------------------------------
int _export FAR PASCAL AV_SetVideoAddressEx
(
    WORD            wSegment,
    WORD            wSelector
)
{
        DWORD   dAddress;
        WORD    nAddressLo, nAddressHi, nMemorySize;

        AuxDebugEx (3, DEBUGLINE "AV_SetVideoAddressEx: Init=%d, Segment=%X, Selector=%X\r\n", 
                bInitialized, wSegment, wSelector);

        if (!bInitialized)
                return FALSE;

        // Make sure that the base address is in the correct range,
        // default to 0xB8.
        if ((wSegment >= 0xA0) && (wSegment <= 0xE0)) {
            Parameters[ AVADDRESS ] = wSegment << 8;
            dAddress = ((DWORD)Parameters[ AVADDRESS ] << 4) ;
        }
        else if ((wSegment >= 0xC00) && (wSegment <= 0xF00)) {
            Parameters[ AVADDRESS ] = wSegment >> 8;
            dAddress = ((DWORD)Parameters[ AVADDRESS ] << 20) ;
        }
        else {
            Parameters[ AVADDRESS ] = 0xB800;
            dAddress = ((DWORD)Parameters[ AVADDRESS ] << 4) ;
        }
        // Set base address for video frame buffer.

        nMemorySize = 1 ;

#if 0   // For some reason, this original version is now
        // giving the compiler grief.  Replaced with version below.

        nAddressLo = (WORD) ((dAddress >> 14) & 0xFFFF) ;
        nAddressLo = (WORD) ((dAddress & 0xFFFF) >> 14) ;
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

        AuxDebugEx (3, DEBUGLINE "AV_SetVideoAddressEx: AVADDRESS=%X, dAddress=%lX\r\n", 
                Parameters[ AVADDRESS ], dAddress);

        // Configure pointer to frame buffer (lpFrameBuffer).

        Parameters[ AVSELECTOR ] = wSelector ;
        uFrameSelector = wSelector ;
        lpFrameBuffer = (LPWORD) MAKELONG( 0, wSelector ) ;
        
        AuxDebugEx (3, DEBUGLINE "AV_SetVideoAddressEx: lpFrameBuffer=%lX\r\n", 
                lpFrameBuffer);

        return TRUE;

} // AV_SetVideoAddress()

//////////////////////////////////////////////////////////////////////////
//      AV_SetVideoScaling(W, H, Flag)                                  //
//                                                                      //
//      This function sets the video scaling parameters for an existing //
//      video window.                                                   //
//                                                                      //
//      Params: W               Width to use for scaling calculations.  //
//              H               Height to use for scaling calculations. //
//              Flag            0 for no scaling, 1 for scaling.        //
//                                                                      //
//      Return: TRUE                                                    //
//////////////////////////////////////////////////////////////////////////
int _export FAR PASCAL AV_SetVideoScaling(WORD W, WORD H, BOOL Flag)
        {
        // To be implemented.
        return TRUE;
        }


//////////////////////////////////////////////////////////////////////////
//      AV_SetVideoSource(Value)                                        //
//                                                                      //
//      This function sets the current video input source number.       //
//      This should be a value between 0 and 2.                         //
//                                                                      //
//      Params: Value           New source number to be set.            //
//                                                                      //
//      Return: TRUE    if successful                                   //
//              FALSE   if unsuccessful                                 //
//////////////////////////////////////////////////////////////////////////

int _export FAR PASCAL AV_SetVideoSource
(
    int             Value
)
{
    int i, j;
    WORD wArray[132];
    char szBuf[130];

    if (Value < 0) Value = 0;
    if (Value > 2) Value = 2;

    Parameters[AVSOURCE] = Value;
    GetPrivateProfileString("EXInit", "Decoder", "PH9051411", szBuf,
                sizeof(szBuf), szIniName);

    switch( Value ) {
        case 0:
        i = AVGetIniArray( (LPWORD)wArray, 132, szBuf, "SetInput0",
                "(8Ah,0Ah,42h)", szIniName );   // set 0 for composite (22h for Svideo on 0)
        break;

        case 1:
        i = AVGetIniArray( (LPWORD)wArray, 132, szBuf, "SetInput1",
                "(8Ah,0Ah,4Ah)", szIniName );   // set 1 for composite (2Ah for Svideo on 1)
        break;

        case 2:
        i = AVGetIniArray( (LPWORD)wArray, 132, szBuf, "SetInput2",
                "(8Ah,0Ah,52h)", szIniName );   // set 2 for composite (32h for Svideo on 2)
        break;
    }

    for( j = 0; j < i; j += 3 ) {
        AV_SetExternalRegister(wArray[j], wArray[j+1], wArray[j+2]);
    }

    return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//      AV_SetWindowPosition(X, Y)                                      //
//                                                                      //
//      This function moves the video window to a new position.         //
//                                                                      //
//      Params: X               New X-coordinate.                       //
//              Y               New Y-coordinate.                       //
//                                                                      //
//      Return: TRUE                                                    //
//////////////////////////////////////////////////////////////////////////
int _export FAR PASCAL AV_SetWindowPosition(WORD X, WORD Y)
        {
        AV_CreateWindow(X, Y, OldW, OldH, OldFlag);
        return TRUE;
        }


//////////////////////////////////////////////////////////////////////////
//      AV_SetWindowSize(W, H)                                          //
//                                                                      //
//      This function changes the size of the video window.             //
//                                                                      //
//      Params: W               New width.                              //
//              H               New height.                             //
//              Flag            0 for no scaling, 1 to scale video.     //
//                                                                      //
//      Return: TRUE                                                    //
//////////////////////////////////////////////////////////////////////////
int _export FAR PASCAL AV_SetWindowSize(WORD W, WORD H, BOOL Flag)
        {
        AV_CreateWindow(OldX, OldY, W, H, Flag);
        return TRUE;
        }


//////////////////////////////////////////////////////////////////////////
//      AV_SetWriteProtectMask(Value)                                   //
//                                                                      //
//      This function does nothing.  It is provided for compatibility   //
//      with a PCV function.                                            //
//                                                                      //
//      Params: Value                Not used.                          //
//                                                                      //
//      Return: TRUE                                                    //
//////////////////////////////////////////////////////////////////////////
int _export FAR PASCAL AV_SetWriteProtectMask(WORD Value)
        {
        return TRUE;
        }


//////////////////////////////////////////////////////////////////////////
//      AV_TurnBorder                                                   //
//                                                                      //
//      This function does nothing.  It is provided for compatibility   //
//      with a PCV function.                                            //
//                                                                      //
//      Params: Value                Not used.                          //
//                                                                      //
//      Return: none                                                    //
//////////////////////////////////////////////////////////////////////////
void _export FAR PASCAL AV_TurnBorder(WORD Value)
        {
        }


//////////////////////////////////////////////////////////////////////////
//      AV_UnfreezeVideo()                                              //
//                                                                      //
//      This function unfreezes the display of live video by turning on //
//      video acquisition.                                              //
//                                                                      //
//      Params: none                                                    //
//                                                                      //
//      Return: TRUE    if successful                                   //
//              FALSE   if unsuccessful                                 //
//////////////////////////////////////////////////////////////////////////
int _export FAR PASCAL AV_UnfreezeVideo(void)
        {
	AV_SetRegister( INPUT_VIDEO_CONFIGURATION_B,
		AV_GetRegister(INPUT_VIDEO_CONFIGURATION_B) & 0xF7 );
        AV_UpdateShadowedRegisters();
        return TRUE;
        }


//////////////////////////////////////////////////////////////////////////
//      AV_VerticalZoom(Value)                                          //
//                                                                      //
//      This function sets simple vertical zoom factors 1X, 2X, 4X.     //
//                                                                      //
//      Params: Value           New zoom factor.                        //
//                                                                      //
//      Return: None                                                    //
//////////////////////////////////////////////////////////////////////////
void _export FAR PASCAL AV_VerticalZoom(int Value)
{
    unsigned int vzr, vpwidth;

    if( Value != nVZoom ) {         // if not already zoomed that amount
          // get the value of viewport width
        vpwidth = AV_GetRegister(DISPLAY_VIEWPORT_HEIGHT_A)
                   + ((int)(AV_GetRegister(DISPLAY_VIEWPORT_HEIGHT_B) & 0x01) << 8);

          // if vertical zoom is enabled
        if( AV_GetRegister(OUTPUT_PROCESSING_CONTROL_B) & 0x01 ) {
              // get the value of Hzoom registers
            vzr = AV_GetRegister(OUTPUT_VERTICAL_ZOOM_CONTROL_A)
                       + ((int)(AV_GetRegister(OUTPUT_VERTICAL_ZOOM_CONTROL_B) & 0x07) << 8);

            switch( Value ) {           // switch on new value
                case 0:                 // disable
                AV_SetRegister( OUTPUT_PROCESSING_CONTROL_B,
                    AV_GetRegister(OUTPUT_PROCESSING_CONTROL_B) & ~0x01 );
                vzr = 0;
                switch( nVZoom ) {          // switch on old value
                    case 2:
                    vpwidth *= 2;
                    break;

                    case 4:
                    vpwidth *= 4;
                    break;
                }
                nVZoom = 0;
                break;

                case 1:
                switch( nVZoom ) {          // switch on old value
                    case 2:
                    vzr *= 2;
                    vpwidth *= 2;
                    break;

                    case 4:
                    vzr *= 4;
                    vpwidth *= 4;
                    break;
                }
                nVZoom = Value;
                break;

                case 2:
                switch( nVZoom ) {          // switch on old value
                    case 1:
                    vzr /= 2;
                    vpwidth /= 2;
                    break;

                    case 4:
                    vzr *= 2;
                    vpwidth *= 2;
                    break;

                }
                nVZoom = Value;
                break;

                case 4:
                switch( nVZoom ) {          // switch on old value
                    case 1:
                    vzr /= 4;
                    vpwidth /= 4;
                    break;

                    case 2:
                    vzr /= 2;
                    vpwidth /= 2;
                    break;
                }
                nVZoom = Value;
                break;
            }
            AV_SetRegister(OUTPUT_VERTICAL_ZOOM_CONTROL_A, vzr & 0xFF);
            AV_SetRegister(OUTPUT_VERTICAL_ZOOM_CONTROL_B, vzr >> 8);
            AV_SetRegister(DISPLAY_VIEWPORT_HEIGHT_A, vpwidth & 0xFF );
            AV_SetRegister(DISPLAY_VIEWPORT_HEIGHT_B, vpwidth >> 8 );
        } else {                    // zoom is disabled
            switch( Value ) {       // switch on new value
                case 0:
                nVZoom = 0;
                AV_SetRegister(OUTPUT_VERTICAL_ZOOM_CONTROL_A, 0);
                AV_SetRegister(OUTPUT_VERTICAL_ZOOM_CONTROL_B, 0);
                break;

                case 1:
                nVZoom = 1;
                AV_SetRegister(OUTPUT_VERTICAL_ZOOM_CONTROL_A, 0xFF);
                AV_SetRegister(OUTPUT_VERTICAL_ZOOM_CONTROL_B, 0x07);
                AV_SetRegister( OUTPUT_PROCESSING_CONTROL_B,
                    AV_GetRegister(OUTPUT_PROCESSING_CONTROL_B) | 0x01 );
                break;

                case 2:
                nVZoom = 2;
                AV_SetRegister(OUTPUT_VERTICAL_ZOOM_CONTROL_A, 0xFF);
                AV_SetRegister(OUTPUT_VERTICAL_ZOOM_CONTROL_B, 0x03);
                AV_SetRegister( OUTPUT_PROCESSING_CONTROL_B,
                    AV_GetRegister(OUTPUT_PROCESSING_CONTROL_B) | 0x01 );
                vpwidth /= 2;
                AV_SetRegister(DISPLAY_VIEWPORT_HEIGHT_A, vpwidth & 0xFF );
                AV_SetRegister(DISPLAY_VIEWPORT_HEIGHT_B, vpwidth >> 8 );
                break;

                case 4:
                nVZoom = 4;
                AV_SetRegister(OUTPUT_VERTICAL_ZOOM_CONTROL_A, 0xFF);
                AV_SetRegister(OUTPUT_VERTICAL_ZOOM_CONTROL_B, 0x01);
                AV_SetRegister( OUTPUT_PROCESSING_CONTROL_B,
                    AV_GetRegister(OUTPUT_PROCESSING_CONTROL_B) | 0x01 );
                vpwidth /= 4;
                AV_SetRegister(DISPLAY_VIEWPORT_HEIGHT_A, vpwidth & 0xFF );
                AV_SetRegister(DISPLAY_VIEWPORT_HEIGHT_B, vpwidth >> 8 );
                break;
            }
        }

        AV_UpdateShadowedRegisters();

// debug stuff
//      vzr = AV_GetRegister(OUTPUT_VERTICAL_ZOOM_CONTROL_A)
//                     + ((int)(AV_GetRegister(OUTPUT_VERTICAL_ZOOM_CONTROL_B) & 0x07) << 8);
//      vpwidth = AV_GetRegister(DISPLAY_VIEWPORT_HEIGHT_A)
//                 + ((int)(AV_GetRegister(DISPLAY_VIEWPORT_HEIGHT_B) & 0x01) << 8);
//
//      wsprintf(szValue, "VZ = %d  %04X  %04X\n", nVZoom, i, vpwidth );
//      OutputDebugString(szValue);
    }
}


//////////////////////////////////////////////////////////////////////////
//      AV_WaitVGARetrace()                                             //
//                                                                      //
//      This function waits for the VGA to enter its vertical retrace   //
//      period.                                                         //
//                                                                      //
//      Params: None                                                    //
//                                                                      //
//      Return: None                                                    //
//////////////////////////////////////////////////////////////////////////
void _export FAR PASCAL AV_WaitVGARetrace(void)
        {
        WORD    wTimeout;

        // First wait for VSync inactive.
        wTimeout = 0xFFFF;
        while (!(AV_GetRegister(VIDEO_OUTPUT_INTERRUPT_STATUS) & 0x02)
                && wTimeout--);

        // Then wait for VSync active.
        wTimeout = 0xFFFF;
        while ((AV_GetRegister(VIDEO_OUTPUT_INTERRUPT_STATUS) & 0x02)
                && wTimeout--);
        }


//////////////////////////////////////////////////////////////////////////
//      AV_WriteImageRect(lpData, X, Y, W, H)                           //
//                                                                      //
//      This function writes an image from a memory buffer into the     //
//      video frame buffer.  The format of the image is determined by   //
//      the image header in the memory buffer.                          //
//                                                                      //
//      Params: lpData          Far pointer to image buffer             //
//              X               X-coordinate in frame buffer.           //
//              Y               Y-coordinate in frame buffer.           //
//              W               Width of image.                         //
//              H               Height of image.                        //
//                                                                      //
//      Return: 1               if successful.                          //
//              0               if error writing image.                 //
//////////////////////////////////////////////////////////////////////////
int _export FAR PASCAL AV_WriteImageRect(LPSTR lpData, WORD x, WORD y, WORD w, WORD h)
{
    int status;

    status = 1;

    switch( *((unsigned int far *)lpData) ) {       // check file type
        case 0x5559:            // YUV file
        displaymmp( lpData, x, y );
        break;

        case 0x4D42:            // BMP file
        displaybmp( lpData + sizeof(BITMAPFILEHEADER), x, y );
        break;

        default:
        status = 0;             // unsupported file format
        break;
    }

    return status;
}
