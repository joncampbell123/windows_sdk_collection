//////////////////////////////////////////////////////////////////////////
//   Module:    GLOBAL.H                                                //
//   Target:    AVWIN.DLL                                               //
//                                                                      //
//   Summary:   This module contains the global variable declararions   //
//              for the AuraVision Dynamic Link Library file AVWIN.DLL. //
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



#ifdef	GLOBAL
	#define EXTERNAL
#else
	#define EXTERNAL	extern
#endif


//////////////////////////////////////////
//      Definitions & Macros            //
//////////////////////////////////////////

// Video controller registers.
#define CHIP_ID                                 0x00
#define CHIP_REVISION                           0x01
#define POWER_ON_CONFIGURATION                  0x10
#define POWER_UP_CONFIGURATION_REGISTER_B       0x11
#define REFRESH_COUNT_REGISTER                  0x12
#define REFRESH_PENDING_REGISTER                0x13
#define MEMORY_WINDOW_BASE_ADDRESS_A            0x14
#define MEMORY_WINDOW_BASE_ADDRESS_B            0x15
#define MEMORY_PAGE_REGISTER                    0x16
#define MEMORY_CONTROL_REGISTER                 0x17
#define MEMORY_CONFIGURATION_REGISTER           0x18
#define ISA_TIME_OUT_REGISTER                   0x19
#define PIXEL_CACHE_BOUND1                      0x1C
#define PIXEL_CACHE_BOUND2                      0x1D
#define ISA_CONTROL                             0x30
#define I2C_CONTROL                             0x34
#define INPUT_VIDEO_CONFIGURATION_A             0x38
#define INPUT_VIDEO_CONFIGURATION_B             0x39
#define ISA_SOURCE_WINDOW_WIDTH_A               0x3A
#define ISA_SOURCE_WINDOW_WIDTH_B               0x3B
#define ISA_SOURCE_WINDOW_HEIGHT_A              0x3C
#define ISA_SOURCE_WINDOW_HEIGHT_B              0x3D
#define INPUT_HORIZONTAL_CROPPING_LEFT_A        0x40
#define INPUT_HORIZONTAL_CROPPING_LEFT_B        0x41
#define INPUT_HORIZONTAL_CROPPING_RIGHT_A       0x44
#define INPUT_HORIZONTAL_CROPPING_RIGHT_B       0x45
#define INPUT_HORIZONTAL_CROPPING_TOP_A         0x48
#define INPUT_HORIZONTAL_CROPPING_TOP_B         0x49
#define INPUT_HORIZONTAL_CROPPING_BOTTOM_A      0x4C
#define INPUT_HORIZONTAL_CROPPING_BOTTOM_B      0x4D
#define INPUT_HORIZONTAL_FILTER                 0x50
#define INPUT_HORIZONTAL_SCALING_CONTROL_A      0x54
#define INPUT_HORIZONTAL_SCALING_CONTROL_B      0x55
#define INPUT_VERTICAL_INTERPOLATION_CONTROL    0x58
#define INPUT_VERTICAL_SCALING_CONTROL_A        0x5C
#define INPUT_VERTICAL_SCALING_CONTROL_B        0x5D
#define VIDEO_INPUT_INTERRUPT_CONTROL           0x60
#define VIDEO_INPUT_INTERRUPT_STATUS            0x61
#define INPUT_FIELD_PIXEL_BUFFER_STATUS         0x64
#define VIDEO_INPUT_FRAME_BUFFER_DEPTH_A        0x68
#define VIDEO_INPUT_FRAME_BUFFER_DEPTH_B        0x69
#define ACQUISITION_CONTROL                     0x6C
#define VIDEO_INPUT_TIME_SCALING_CONTROL        0x6D
#define ACQUISITION_ADDRESS_A                   0x70
#define ACQUISITION_ADDRESS_B                   0x71
#define ACQUISITION_ADDRESS_C                   0x72
#define VIDEO_BUFFER_LAYOUT_CONTROL             0x73
#define INPUT_PIXEL_BUFFER_LOW_MARKER           0x78
#define INPUT_PIXEL_BUFFER_HIGH_MARKER          0x79
#define CAPTURE_CONTROL                         0x80
#define CAPTURE_VIEWPORT_ADDRESS_A              0x81
#define CAPTURE_VIEWPORT_ADDRESS_B              0x82
#define CAPTURE_VIEWPORT_ADDRESS_C              0x83
#define CAPTURE_VIEWPORT_WIDTH_A                0x84
#define CAPTURE_VIEWPORT_WIDTH_B                0x85
#define CAPTURE_VIEWPORT_HEIGHT_A               0x86
#define CAPTURE_VIEWPORT_HEIGHT_B               0x87
#define CAPTURE_PIXEL_BUFFER_LOW                0x88
#define CAPTURE_PIXEL_BUFFER_HIGH               0x89
#define CAPTURE_MULTI_BUFFER_DEPTH_A            0x8A
#define CAPTURE_MULTI_BUFFER_DEPTH_B            0x8B
#define OUTPUT_PIXEL_BUFFER_LOW_MARK            0x90
#define OUTPUT_PIXEL_BUFFER_HIGH_MARK           0x91
#define DISPLAY_CONTROL                         0x92
#define OUTPUT_FRAME_BUFFER_DEPTH_CONTROL       0x93
#define VGA_CONTROL                             0x94
#define OUTPUT_PROCESSING_CONTROL_A             0x96
#define OUTPUT_PROCESSING_CONTROL_B             0x97
#define PALETTE_READ_ADDRESS_HIGH_NIBBLE        0x9A
#define KEY_COLOR                               0x9C
#define KEY_COLOR_BYTE_1                        0x9D
#define DISPLAY_VIEWPORT_STARTING_ADDRESS_A     0xA0
#define DISPLAY_VIEWPORT_STARTING_ADDRESS_B     0xA1
#define DISPLAY_VIEWPORT_STARTING_ADDRESS_C     0xA2
#define DISPLAY_VIEWPORT_WIDTH_A                0xA4
#define DISPLAY_VIEWPORT_WIDTH_B                0xA5
#define DISPLAY_VIEWPORT_HEIGHT_A               0xA6
#define DISPLAY_VIEWPORT_HEIGHT_B               0xA7
#define DISPLAY_VIEWPORT_ORIGIN_TOP_A           0xA8
#define DISPLAY_VIEWPORT_ORIGIN_TOP_B           0xA9
#define DISPLAY_VIEWPORT_ORIGIN_LEFT_A          0xAA
#define DISPLAY_VIEWPORT_ORIGIN_LEFT_B          0xAB
#define DISPLAY_WINDOW_LEFT_A                   0xB0
#define DISPLAY_WINDOW_LEFT_B                   0xB1
#define DISPLAY_WINDOW_RIGHT_A                  0xB4
#define DISPLAY_WINDOW_RIGHT_B                  0xB5
#define DISPLAY_WINDOW_TOP_A                    0xB8
#define DISPLAY_WINDOW_TOP_B                    0xB9
#define DISPLAY_WINDOW_BOTTOM_A                 0xBC
#define DISPLAY_WINDOW_BOTTOM_B                 0xBD
#define OUTPUT_VERTICAL_ZOOM_CONTROL_A          0xC0
#define OUTPUT_VERTICAL_ZOOM_CONTROL_B          0xC1
#define OUTPUT_HORIZONTAL_ZOOM_CONTROL_A        0xC4
#define OUTPUT_HORIZONTAL_ZOOM_CONTROL_B        0xC5
#define BRIGHTNESS_CONTROL                      0xC8
#define CONTRAST_CONTROL                        0xC9
#define SATURATION_CONTROL                      0xCA
#define OUTPUT_COLOR_PROCESSING_CONTROL_A       0xD0
#define OUTPUT_COLOR_PROCESSING_CONTROL_B       0xD1
#define VIDEO_OUTPUT_INTERRUPT_CONTROL          0xD2
#define VIDEO_OUTPUT_INTERRUPT_STATUS           0xD3
#define CHROMA_KEY_RED_LOW                      0xD8
#define CHROMA_KEY_RED_HIGH                     0xD9
#define CHROMA_KEY_GREEN_LOW                    0xDA
#define CHROMA_KEY_GREEN_HIGH                   0xDB
#define CHROMA_KEY_BLUE_LOW                     0xDC
#define CHROMA_KEY_BLUE_HIGH                    0xDD

// I2C_BUS_CONTROL bits
#define I2C_CLOCK       0x02        // bit equates for I2C_CONTROL
#define I2C_DATA        0x08
#define I2C_RDCLOCK     0x10
#define I2C_RDDATA      0x20
#define I2C_RDDATA_D    0x40

// EXTERNAL registers
#define  MAXEXTREG      50
#define  PHlowADD1      0x8A
#define  PHhighADD1     0x8E
#define  PHlowADD2      0x9C
#define  PHhighADD2     0x9E

//////////////////////////////////////////
//      Type Definitions		//
//////////////////////////////////////////

// Physical Device Structure

typedef struct {
        WORD    bmType;
        WORD    bmWidth;
        WORD    bmHeight;
        WORD    bmWidthBytes;
        BYTE    bmPlanes;
        BYTE    bmBitsPixel;
        LPBYTE  bmBits;
        DWORD   bmWidthPlanes;
        LPBYTE  bmlpPDevice;
        WORD    bmSegmentIndex;
        WORD    bmScanSegment;
        WORD    bmFillBytes;
        WORD    futureUse4;
        WORD    futureUse5;
        } PDEVICE;

typedef PDEVICE FAR* LPPDEVICE;

// Drawmode Structure

typedef struct {
        WORD    Rop2;
        WORD    bkMode;
        DWORD   bkColor;
        DWORD   TextColor;
        WORD    TBreakExtra;
        WORD    BreakExtra;
        WORD    BreakErr;
        WORD    BreakRem;
        WORD    BreakCount;
        WORD    CharExtra;
        DWORD   LbkColor;
        DWORD   LTextColor;
        } DRAWMODE;

typedef DRAWMODE FAR* LPDRAWMODE;

// Physical Color Structure

typedef DWORD PCOLOR;

typedef PCOLOR FAR* LPPCOLOR;

typedef struct tagENTRIES
        {
        WORD    enIndex;
        LPSTR   enString;
        LPSTR   enFormat;
        } ENTRIES;

struct mmphd {
        unsigned int id;
        unsigned int id2;
        unsigned int id3;
        unsigned int res1;
        unsigned int res2;
        unsigned int res3;
        unsigned int width;
        unsigned int height;
    };


//////////////////////////////////////////
//      Internal Function Prototypes    //
//////////////////////////////////////////
int FAR PASCAL          LibMain(HANDLE, WORD, WORD, LPSTR);
int FAR PASCAL          WEP(int);
BOOL FAR PASCAL         AVConfigDlgProc(HWND, UINT, WPARAM, LPARAM);
BYTE FAR PASCAL         AVGetIniArray(LPWORD, UINT, LPSTR, LPSTR, LPSTR, LPSTR);
LPBYTE FAR PASCAL       CreatePhysicalSelector(DWORD, DWORD);
LPBYTE FAR PASCAL       CreateRealSelector(WORD);
void FAR PASCAL         FixVGAMode(void);
WORD FAR PASCAL         GetNumColors(void);
WORD FAR PASCAL         GetQFactor(WORD);
BOOL FAR PASCAL         TestQFactor(WORD);
void FAR PASCAL         SetQFactor(WORD, WORD);
DWORD FAR PASCAL        Pixel(LPPDEVICE, WORD, WORD, COLORREF, LPDRAWMODE);
COLORREF FAR PASCAL     ColorInfo(LPPDEVICE, COLORREF, LPPCOLOR);
int FAR PASCAL				GetHSyncPolarity(void);
int FAR PASCAL				GetVSyncPolarity(void);

DWORD   ahtoi(LPSTR);
DWORD   DecimalToLong(LPSTR);
LPSTR   FindStartOfNumber(LPSTR);
LPSTR   FindEndOfNumber(LPSTR);
int     AV_SetExternalData(WORD);
int     AV_GetExternalData(void);
int     AVWaitSCL(void);
int     AVWaitSDA(void);
void    AVDelayI2C(void);
int     capturemmp(HFILE, int, int, int, int);
int     capturebmp24(HFILE, int, int, int, int);
int     capturebmp256(HFILE, int, int, int, int);
int     readimagemmp(unsigned char huge *, int, int, int, int);
int     readdib24(unsigned char huge *, int, int, int, int);
void    palrgb2playyuv411(unsigned char far *, RGBQUAD far *, unsigned int far *);
void    palrgb2playyuv422(unsigned char far *, RGBQUAD far *, unsigned int far *);
void    rgb2playyuv411(unsigned char huge *, unsigned int far *);
void    rgb2playyuv422(unsigned char huge *, unsigned int far *);
void    pyuv2yuv411(unsigned int far *, unsigned int far *);
void    pyuv2yuv422(unsigned int far *, unsigned int far *);
void    yuv4112rgb(unsigned int far *, unsigned char huge *);
void    yuv4222rgb(unsigned int far *, unsigned char huge *);
int     getfiletype(HFILE);
HGLOBAL loadpicfile(HFILE);
int     displaybmp(unsigned char far *, int, int);
int     displaymmp(unsigned char far *, int, int);
int     buildpal666(char far *, int far *);
int     matchcolor666(unsigned int, unsigned int, unsigned int, unsigned int far *);
HANDLE  bitmap2DIB(HBITMAP, HPALETTE);


//////////////////////////////////////////
//      Global Variables                //
//////////////////////////////////////////
EXTERNAL HANDLE  hInst;                          // Global copy of instance handle.

EXTERNAL WORD    NumColors;                      // Number of colors in Windows mode.
EXTERNAL WORD    wColorKey;                      // Default color key value for mode.
EXTERNAL int     nPlanes;                        // Number of planes in Windows mode.
EXTERNAL int     nBitsPixel;                     // Number of bits per pixel in mode.
EXTERNAL WORD    ScreenWidth, ScreenHeight;      // Dimensions of current VGA mode.
EXTERNAL int     DecoderType;                    // decoder type from ini file 0 = generic 411, 1 = generic 422, 2 = ?
EXTERNAL int     pbsigntoggle;                   // global flag to tell playback routines to toggle chroma signs
EXTERNAL int     capsigntoggle;                  // global flag to tell capture routines to toggle chroma signs

EXTERNAL WORD    OldDrawWSrc;
EXTERNAL WORD    OldDrawHSrc;
EXTERNAL WORD    OldBitmapWSrc;
EXTERNAL WORD    OldBitmapHSrc;
EXTERNAL WORD    OldDrawType;

//////////////////////////////////////////
//      Global Initialized Variables    //
//////////////////////////////////////////
#ifdef	GLOBAL

EXTERNAL HWND    hConfigHandle[NUMPARAMS];       // Table of dialog control handles.

EXTERNAL WORD    OldX = 0;                       // Current video window position.
EXTERNAL WORD    OldY = 0;
EXTERNAL WORD    OldW = 1;                       // Current video window width.
EXTERNAL WORD    OldH = 1;                       // Current video window height.
EXTERNAL BOOL    OldFlag = 0;                    // Current video scaling flag.
EXTERNAL BOOL    QFactorFix = TRUE;                  // Current interleave value

EXTERNAL WORD    Parameters[NUMPARAMS] = {0};    // Current video parameters.
EXTERNAL WORD    OldParameters[NUMPARAMS] = {0}; // Previous video parameters.

EXTERNAL char    szIniName[130] = { "AVWIN.INI" };
EXTERNAL char    szMode[50] = "";                // Current VGA mode WxHxC.

EXTERNAL char    szEntry[20] = "";               // Temporary string.
EXTERNAL char    szValue[20] = "";               // Temporary string.

EXTERNAL LPWORD  lpFrameBuffer = NULL;           // Pointer to frame buffer.
EXTERNAL UINT    uFrameSelector = NULL;          // Frame buffer selector.

EXTERNAL BOOL    bInitialized = FALSE;           // Video initialized flag.

EXTERNAL WORD    AVIndexPort = 0xAD6;            // Video controller ports.
EXTERNAL WORD    AVDataPort  = 0xAD7;
EXTERNAL WORD    PHsLID       = 0x8A;				 //default set for PH9051,7191
EXTERNAL WORD    PHsHID       = 0x8E; 

EXTERNAL WORD    wImageWidth = 0;                // Dimensions of last image loaded.
EXTERNAL WORD    wImageHeight = 0;
EXTERNAL WORD    wImageType = 0;                 // File type of last image loaded.

EXTERNAL int     nHZoom = 0;                     // Hzoom is disabled
EXTERNAL int     nVZoom = 0;                     // Vzoom is disabled

EXTERNAL int	HSyncActive = 0;		// Active low = 0, active high = 4.
EXTERNAL int	VSyncActive = 0;		// Active low = 0, active high = 2.
EXTERNAL int	nVGAControl = 255;		// VGA control register value.

// Configuration strings for [Settings] section of INI file.

EXTERNAL ENTRIES enSettingsEntries[] =
       {AVPORT,         "Port",         "%Xh",
        AVADDRESS,      "Address",      "%Xh",
 	AVIRQLEVEL,	"IRQLevel",	"%d",
        AVCROPLEFT,     "CropLeft",     "%d",
        AVCROPTOP,      "CropTop",      "%d",
        AVCROPRIGHT,    "CropRight",    "%d",
        AVCROPBOTTOM,   "CropBottom",   "%d",
        AVBRIGHTNESS,   "Brightness",   "%d",
        AVCONTRAST,     "Contrast",     "%d",
        AVSATURATION,   "Saturation",   "%d",
        AVSOURCE,       "VideoSource",  "%d",
        AVINPUTFORMAT,	"InputFormat",	"%d",
        AVHUE,          "Hue",          "%d",
        AVQFACTOR,      "QFactor",      "%d",
        AVINTERLEAVE,   "Interleave",   "%d",
        AVCOLORKEYENABLE, "Colorkey",   "%d",
        AVPLAYBACK,	"Playback",	"%d",
	AVREDLOW,	"RedLow",	"%d",
	AVREDHIGH,	"RedHigh",	"%d",
	AVGREENLOW,	"GreenLow",	"%d",
	AVGREENHIGH,	"GreenHigh",	"%d",
	AVBLUELOW,	"BlueLow",	"%d",
	AVBLUEHIGH,	"BlueHigh",	"%d",
	AVVLBUS,	"VLBus",	"%d",
	AVBUFFERING,	"AVIBuffers",	"%d" };

EXTERNAL WORD    nSettingsCount = sizeof(enSettingsEntries) / sizeof(ENTRIES);

//  Configuration strings for [Modes] section of INI file.
EXTERNAL ENTRIES enModesEntries[] =
       {AVXPOSITION,    "",     "%03d ",
        AVYPOSITION,    "",     "%03d ",
        AVVGACONTROL,   "",     "%03d ",
        AVDUMMY,        "",     "%03d ",
        AVDUMMY,        "",     "%03d ",
        AVDUMMY,        "",     "%03d ",
        AVCOLORKEY,     "",     "%05u "};

EXTERNAL WORD    nModesCount = sizeof(enModesEntries) / sizeof(ENTRIES);

EXTERNAL WORD    DefaultParam[NUMPARAMS] =       // Default parameter values.
       {0, 0,									 // X position, Y position.
		  20, 26, 700, 506,					 // Crop factors.
        0,										 // Dummy.
		  10,										 // IRQ level.
		  2,										 // AVI playback flag.
		  0, 0, 0, 0, 0, 0,					 // Chroma key ranges.
		  0, 0, 0,                        // NumColors, VLBus, Initialized.
        253,                            // Color key.
        14,                             // Base address in MB.
        0x700, 0,                       // Port & Selector.
        128, 8, 8,                      // Brightness, contrast, saturation.
        0, 0, 0,                        // Video source, freeze state, hue.
        1, 0,                           // Input format interlace output.
        100, 4, 1,                      // Q factor, Interleave, colorkey enable
		  768, 1,				   			 // Memory size, buffering.
		  255};									 // VGA Control.  

EXTERNAL BYTE    AVColorKeyTable[16] =
       {0x00, 0x0C, 0x0A, 0x0E, 0x01, 0x15, 0x23, 0x07,
        0x0F, 0x24, 0x12, 0x36, 0x09, 0x2D, 0x1B, 0x3F};

EXTERNAL BYTE    AVHorFilterTable[8]  = {3, 3, 2, 2, 1, 1, 0, 0};

EXTERNAL WORD            wBitmapBits[8] = {0};
EXTERNAL COLORREF        crPColor = 0;
EXTERNAL PDEVICE         pdBitmap = {0};
EXTERNAL DRAWMODE        dmDrawMode = {0};

EXTERNAL unsigned char interleave[] = { 2, 3, 4, 2, 3 };         // interleave for modes 0-4
EXTERNAL unsigned char yuvpadbound[] = { 4, 12, 4, 2, 6 };       // pad boundary for YUV data (playback, capture)
EXTERNAL unsigned int panmask[] = { 0xFFFE, 0xFFFC, 0xFFFF, 0xFFFF, 0xFFFE };

EXTERNAL unsigned int framewidth[5][4] = {                       // frame line width [memmode reg18] [layout reg73]
                            1024,  512,  512, 512,      // mode 0 - single, double, single, quad
                            1536,  768,  768, 384,      // mode 1 - single, double, single, quad
                            2048, 1024, 1024, 512,      // mode 2 - single, double, single, quad
                            1024,  512,  512, 512,      // mode 3 - single, double, single, quad
                            1536,  768,  768, 384       // mode 4 - single, double, single, quad
                        };

EXTERNAL unsigned int frameheight[5][4] = {                      // frame height [layout reg73]
                            512, 512, 1024, 512,        // mode 0 - single, double, single, quad
                            512, 512, 1024, 512,        // mode 1 - single, double, single, quad
                            512, 512, 1024, 512,        // mode 2 - single, double, single, quad
                            512, 512, 1024, 512,        // mode 3 - single, double, single, quad
                            512, 512, 1024, 256         // mode 4 - single, double, single, quad
                        };

EXTERNAL unsigned int ExRegPHLSave[MAXEXTREG];   // external registers IICSA=LOW sent to chip ID 8A are shadowed here
EXTERNAL unsigned int ExRegPHHSave[MAXEXTREG];   // external registers IICSA=HIGH sent to chip ID 8E are shadowed here
EXTERNAL unsigned int ExReg81Save[32];   // external registers sent to Samsung chip ID 81 are shadowed here
EXTERNAL unsigned int ExReg82Save[32];   // external registers sent to Samsung chip ID 82 are shadowed here

EXTERNAL DWORD	dDrawInstance = 0;
EXTERNAL DWORD	dLastInstance = 0;
EXTERNAL HWND	hwndDrawWindow = NULL;
EXTERNAL HWND	hwndLastWindow = NULL;
EXTERNAL DWORD	dLastTime = 0;

EXTERNAL BYTE   bUpdateType = 0;

#else

EXTERNAL HWND    hConfigHandle[];

EXTERNAL WORD    OldX;
EXTERNAL WORD    OldY;
EXTERNAL WORD    OldW;
EXTERNAL WORD    OldH;
EXTERNAL BOOL    OldFlag;
EXTERNAL WORD    QFactorFix;

EXTERNAL WORD    Parameters[];
EXTERNAL WORD    OldParameters[];

EXTERNAL char    szIniName[];
EXTERNAL char    szMode[];

EXTERNAL char    szEntry[];
EXTERNAL char    szValue[];

EXTERNAL LPWORD  lpFrameBuffer;
EXTERNAL UINT    uFrameSelector;

EXTERNAL BOOL    bInitialized;

EXTERNAL WORD    AVIndexPort;
EXTERNAL WORD    AVDataPort;

EXTERNAL WORD    PHsLID; 		 //default set for PH9051,7191
EXTERNAL WORD    PHsHID; 

EXTERNAL WORD    wImageWidth;
EXTERNAL WORD    wImageHeight;
EXTERNAL WORD    wImageType;

EXTERNAL int     nHZoom;
EXTERNAL int     nVZoom;

EXTERNAL int	HSyncActive;
EXTERNAL int	VSyncActive;
EXTERNAL int	nVGAControl;

EXTERNAL ENTRIES enSettingsEntries[];
EXTERNAL WORD    nSettingsCount;

EXTERNAL ENTRIES enModesEntries[];
EXTERNAL WORD    nModesCount;

EXTERNAL WORD    Config[];
EXTERNAL WORD    nConfigCount;

EXTERNAL WORD    MinParam[];
EXTERNAL WORD    MaxParam[];
EXTERNAL WORD    DefaultParam[];
EXTERNAL BYTE    AVColorKeyTable[];
EXTERNAL BYTE    AVHorFilterTable[];

EXTERNAL WORD            wBitmapBits[];
EXTERNAL COLORREF        crPColor;
EXTERNAL PDEVICE         pdBitmap;
EXTERNAL DRAWMODE        dmDrawMode;

EXTERNAL unsigned char interleave[];
EXTERNAL unsigned char yuvpadbound[];
EXTERNAL unsigned int panmask[];

EXTERNAL unsigned int framewidth[5][4];
EXTERNAL unsigned int frameheight[5][4];
EXTERNAL unsigned int ExRegPHLSave[];
EXTERNAL unsigned int ExRegPHHSave[];
EXTERNAL unsigned int ExReg81Save[];
EXTERNAL unsigned int ExReg82Save[];

EXTERNAL DWORD	dDrawInstance;
EXTERNAL DWORD	dLastInstance;
EXTERNAL HWND	hwndDrawWindow;
EXTERNAL HWND	hwndLastWindow;
EXTERNAL DWORD	dLastTime;

EXTERNAL BYTE   bUpdateType;

#endif

