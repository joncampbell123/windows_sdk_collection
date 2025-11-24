//////////////////////////////////////////////////////////////////////////
//	AVWIN.H
//
//	Include file for AVWIN.DLL
//	containing definitions for
//	external programs.
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


//////////////////////////////////
//      Constants               //
//////////////////////////////////

#define NUMPARAMS               36      // Number of Parameters.
#define NUMCONFIG               27      // Number of Config dialog entries.

#define AVXPOSITION             0
#define AVYPOSITION             1
#define AVCROPLEFT              2
#define AVCROPTOP               3
#define AVCROPRIGHT             4
#define AVCROPBOTTOM            5
#define AVDUMMY                 6

#define AVIRQLEVEL		7
#define AVPLAYBACK		8

#define AVREDLOW		9
#define AVREDHIGH		10
#define AVGREENLOW		11
#define AVGREENHIGH		12
#define AVBLUELOW		13
#define AVBLUEHIGH		14

#define AVNUMCOLORS		15
#define AVVLBUS			16
#define AVINITIALIZED		17

#define AVCOLORKEY              18
#define AVADDRESS               19
#define AVPORT                  20
#define AVSELECTOR              21
#define AVBRIGHTNESS            22
#define AVCONTRAST              23
#define AVSATURATION            24
#define AVSOURCE                25
#define AVFREEZESTATE           26
#define AVHUE                   27
#define AVINPUTFORMAT           28
#define AVINTERLACEOUTPUT       29
#define AVQFACTOR               30
#define AVINTERLEAVE            31
#define AVCOLORKEYENABLE        32
#define AVMEMORYSIZE		33
#define AVBUFFERING		34
#define AVVGACONTROL		35
#define AVCHROMA			36


//////////////////////////////////
//      Function Prototypes     //
//////////////////////////////////

//      PCV Compatible Functions
int  FAR PASCAL AV_ClearVideoRect(WORD, WORD, WORD, WORD);
int  FAR PASCAL AV_CreateWindow(WORD, WORD, WORD, WORD, BOOL);
void FAR PASCAL AV_DisableColorKey(void);
void FAR PASCAL AV_DisableFieldReplication(void);
void FAR PASCAL AV_DisableInterlace(void);
int  FAR PASCAL AV_DisableVideo(void);
void FAR PASCAL AV_EnableColorKey(void);
void FAR PASCAL AV_EnableFieldReplication(void);
void FAR PASCAL AV_EnableInterlace(void);
int  FAR PASCAL AV_EnableVideo(void);
int  FAR PASCAL AV_Exit(void);
int  FAR PASCAL AV_FreezeVideo(void);
BYTE FAR PASCAL AV_GetColor(WORD);
int  FAR PASCAL AV_GetInputFormat(void);
int  FAR PASCAL AV_GetPortAddress(void);
BYTE FAR PASCAL AV_GetRegister(WORD);
WORD FAR PASCAL AV_GetSkewFactor(int);
int  FAR PASCAL AV_GetSystemMetrics(WORD);
int  FAR PASCAL AV_GetVideoAddress(void);
int  FAR PASCAL AV_GetVideoSource(void);
void FAR PASCAL AV_HorizontalZoom(int);
int  FAR PASCAL AV_Initialize(void);
int  FAR PASCAL AV_LoadClipboardFormat(HWND, WORD, WORD);
int  FAR PASCAL AV_LoadConfiguration(void);
int  FAR PASCAL AV_LoadImageRect(LPSTR, WORD, WORD);
int  FAR PASCAL AV_LoadDIB(LPSTR, WORD, WORD);
int  FAR PASCAL AV_PanWindow(WORD, WORD);
int  FAR PASCAL AV_ReadImageRect(LPSTR, WORD, WORD, WORD, WORD, WORD);
int  FAR PASCAL AV_ReadDIB(LPSTR, WORD, WORD, WORD, WORD, WORD);
void FAR PASCAL AV_ResetColors(void);
void FAR PASCAL AV_ResetSkewFactors(void);
int  FAR PASCAL AV_SaveClipboardFormat(HWND, WORD, WORD, WORD, WORD, WORD, WORD);
int  FAR PASCAL AV_SaveConfiguration(void);
int  FAR PASCAL AV_SaveImageRect(LPSTR, WORD, WORD, WORD, WORD, WORD, WORD);
int  FAR PASCAL AV_SetAcquisitionWindow(WORD, WORD, WORD, WORD);
void FAR PASCAL AV_SetColor(WORD, BYTE);
int  FAR PASCAL AV_SetColorKey(WORD);
int  FAR PASCAL AV_SetCaptureAddress(WORD, WORD);
int  FAR PASCAL AV_SetDisplayWindow(WORD, WORD, WORD, WORD);
int  FAR PASCAL AV_SetInputFormat(WORD);
int  FAR PASCAL AV_SetPortAddress(int);
void FAR PASCAL AV_SetRegister(WORD, WORD);
int  FAR PASCAL AV_SetSkewFactor(int, WORD);
int  FAR PASCAL AV_SetVideoAddress(int);
int  FAR PASCAL AV_SetVideoAddressEx(WORD, WORD);
int  FAR PASCAL AV_SetVideoScaling(WORD, WORD, BOOL);
int  FAR PASCAL AV_SetVideoSource(int);
int  FAR PASCAL AV_SetWindowPosition(WORD, WORD);
int  FAR PASCAL AV_SetWindowSize(WORD, WORD, BOOL);
int  FAR PASCAL AV_SetWriteProtectMask(WORD);
void FAR PASCAL AV_TurnBorder(WORD);
int  FAR PASCAL AV_UnfreezeVideo(void);
void FAR PASCAL AV_VerticalZoom(int);
void FAR PASCAL AV_WaitVGARetrace(void);
int  FAR PASCAL AV_WriteImageRect(LPSTR, WORD, WORD, WORD, WORD);

//      Additional AuraVision Functions
void FAR PASCAL AV_ConfigureVideo(void);
void FAR PASCAL AV_DisableChromaKey(void);
void FAR PASCAL AV_DrawWindow(WORD, WORD, WORD, WORD, WORD, WORD,
	WORD, WORD, WORD);
void FAR PASCAL AV_EnableChromaKey(void);
DWORD FAR PASCAL AV_GetDrawInstance(void);
HWND FAR PASCAL AV_GetLastWindow(void);
WORD FAR PASCAL AV_GetExternalRegister(WORD, WORD);
WORD FAR PASCAL AV_GetParameter(WORD);
WORD FAR PASCAL AV_ReadVideoMemory(WORD, LONG);
void FAR PASCAL AV_SetChromaRange(WORD, WORD, WORD, WORD, WORD, WORD);
void FAR PASCAL AV_SetDrawInstance(DWORD, HWND);
WORD FAR PASCAL AV_SetExternalRegister(WORD, WORD, WORD);
WORD FAR PASCAL AV_SetExternalString(WORD, WORD, LPINT);
void FAR PASCAL AV_SetIniFilename(LPSTR);
void FAR PASCAL AV_GetIniFilename(LPSTR lpszIni, WORD cbSize);
void FAR PASCAL AV_SetParameter(WORD, WORD);
void FAR PASCAL AV_SetVGAControl(WORD);
void FAR PASCAL AV_UpdateShadowedRegisters(void);
void FAR PASCAL AV_UpdateVideo(void);
void FAR PASCAL AV_WaitVideoFrame(void);
void FAR PASCAL AV_WriteVideoMemory(WORD, LONG, WORD);

// Color indexes
#define BRIGHTNESS      0
#define SATURATION      1
#define CONTRAST        2
#define HUE             3
#define RED             4
#define GREEN           5
#define BLUE            6

// Skew factor indexes
#define SF_DISPWINSKEWX         0
#define SF_DISPWINSKEWY         1
#define SF_DISPADDRSKEWX        2
#define SF_DISPADDRSKEWY        3
#define SF_SHIFTCLOCKSTART      4
#define SF_PALETTESKEW          5
#define SF_PLLDIVISOR           6

// Skew factor ranges
#define DISPWINSKEWXRANGE       0x7FF
#define DISPWINSKEWYRANGE       0x7FF
#define DISPADDRSKEWXRANGE      0x3FF
#define DISPADDRSKEWYRANGE      0x3FF
#define SHIFTCLOCKSTARTRANGE    0x7F
#define PALETTESKEWRANGE        0x03
#define PLLDIVISORRANGE         0x3FF

// System metrics indexes
#define SM_VIDEOWIDTH   0
#define SM_VIDEOHEIGHT  1
#define SM_BOARDTYPE    2
#define SM_VERSION      3
#define SM_INTERLACE    4
#define SM_REPLICATE    5
#define SM_IMAGEWIDTH   6
#define SM_IMAGEHEIGHT  7
#define SM_IMAGETYPE    8

// Bitmap file types
#define BM_DIB24        0               // Windows DIB 24 bit true color
#define BM_DIB8P        1               // Windows DIB 8 bit palettized
#define BM_DIB8G        2               // Windows DIB 8 bit gray-scale
#define BM_DIB4D        3               // Windows DIB 4 bit dithered
#define BM_TRG32        4               // Targa 32 bit true color
#define BM_TRG24        5               // Targa 24 bit true color
#define BM_TRG16        6               // Targa 16 bit true color
#define BM_YUV411       7               // IBM MMotion format 4:1:1 YUV
#define BM_TIFF8P       8               // TIFF 8 bit palettized
#define BM_TIFF8G       9               // TIFF 8 bit gray-scale
#define BM_TIFF4D       10              // TIFF 4 bit dithered
#define BM_PCX8P        11              // PCX 8 bit palettized
#define BM_PCX8G        12              // PCX 8 bit gray-scale
#define BM_PCX4D        13              // PCX 4 bit dithered
#define BM_GIF8P        14              // GIF 8 bit palettized
#define BM_GIF8G        15              // GIF 8 bit gray-scale
#define BM_JPEG         16              // JPEG compressed 4:2:2 YUV

#define BM_DIB1P        17              // BMP monochrome
#define BM_DIB4P        18              // BMP 16 colors palettized


// Video format types
#define CF_PAL          0
#define CF_NTSC         1
#define CF_YC           8
#define CF_COMPOSITE    9
#define CF_COMPRESS    10
#define CF_EXPAND      11
#define CF_INPUT0      12
#define CF_INPUT1      13
#define CF_INPUT2      14

// Decoder types
#define DCD_411         0               // generic 411 decoder
#define DCD_422         1               // generic 422 decoder
//#define DCD_PHIL9051  2               // specific decoders follow (special handling)
#define KS0117				3					 // decoder for Samsung chip
#define KS0116				4					 // Genlock 
#define AV_ADDR			0x700
#define AV_DATA			0x701
