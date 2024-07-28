
/*ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
  บ  PCVIDEO.H                                                               บ
  บ     C prototypes for the Creative Labs Video Blaster PCVIDEO.DLL.        บ
  บ                                                                          บ
  บ     Copyright (c) Creative Technology Ltd, 1992. All rights reserved.    บ
  ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ*/

WORD FAR PASCAL vbcSetMasterVol (WORD);
WORD FAR PASCAL vbcSetMicVol    (WORD);
WORD FAR PASCAL vbcSetLineInVol (WORD);
WORD FAR PASCAL vbcSetFMVol     (WORD);
WORD FAR PASCAL vbcSetLine2Vol  (WORD);
WORD FAR PASCAL vbcGetMasterVol (void);
WORD FAR PASCAL vbcGetMicVol    (void);
WORD FAR PASCAL vbcGetLineInVol (void);
WORD FAR PASCAL vbcGetFMVol     (void);
WORD FAR PASCAL vbcGetLine2Vol  (void);

WORD  FAR PASCAL vbcGetDLLInitError (void);

int  FAR PASCAL vbcClearVideoRect(WORD, WORD, WORD, WORD);
void FAR PASCAL vbcCreateWindow(WORD, WORD, WORD, WORD, BOOL);
void FAR PASCAL vbcDisableFieldReplication(void);
void FAR PASCAL vbcDisableInterlace(void);
void FAR PASCAL vbcDisableVideo(void);
void FAR PASCAL vbcEnableFieldReplication(void);
void FAR PASCAL vbcEnableInterlace(void);
void FAR PASCAL vbcEnableVideo(void);
void FAR PASCAL vbcExit(void);
void FAR PASCAL vbcFreezeVideo(void);
BYTE FAR PASCAL vbcGetColor(WORD);
WORD FAR PASCAL vbcGetInputFormat(void);
int  FAR PASCAL vbcGetPortAddress(void);
BYTE FAR PASCAL vbcGetRegister(WORD);
WORD FAR PASCAL vbcGetSkewFactor(int);
WORD FAR PASCAL vbcGetSystemMetrics(WORD);
int  FAR PASCAL vbcGetVideoAddress(void);
WORD FAR PASCAL vbcGetVideoSource(void);
void FAR PASCAL vbcHorizontalZoom(int);
WORD FAR PASCAL vbcInitialize(void);
int  FAR PASCAL vbcLoadConfiguration(void);
int  FAR PASCAL vbcLoadImageRect(LPSTR, WORD, WORD);
void FAR PASCAL vbcPanWindow(WORD, WORD);
void FAR PASCAL vbcResetColors(void);
void FAR PASCAL vbcResetSkewFactors(void);
int  FAR PASCAL vbcSaveConfiguration(void);
int  FAR PASCAL vbcSaveImageRect(LPSTR, WORD, WORD, WORD, WORD, WORD, WORD);
void FAR PASCAL vbcSetColor(WORD, BYTE);
void FAR PASCAL vbcSetColorKey(WORD);
int  FAR PASCAL vbcSetPortAddress(int);
void FAR PASCAL vbcSetRegister(WORD, BYTE);
void FAR PASCAL vbcSetSkewFactor(int, WORD);
int  FAR PASCAL vbcSetVideoAddress(int);
void FAR PASCAL vbcSetVideoSource(WORD);
void FAR PASCAL vbcSetWindowPosition(WORD, WORD);
void FAR PASCAL vbcSetWindowSize(WORD, WORD, BOOL);
void FAR PASCAL vbcSetWriteProtectMask(WORD);
void FAR PASCAL vbcUnFreezeVideo(void);
void FAR PASCAL vbcVerticalZoom(int);

void FAR PASCAL vbcSetInputFormat(int);
int  FAR PASCAL vbcEnableInterrupt(WORD);
int  FAR PASCAL vbcGetIntrNo(void);
int  FAR PASCAL vbcRemoveVSyncIntr(void);
int  FAR PASCAL vbcSetVSyncIntr(FARPROC, WORD);

int  FAR PASCAL vbcSetVideoScaling(WORD, WORD, BOOL);
void FAR PASCAL vbcWaitVGARetrace(void);
void FAR PASCAL vbcEnableColorKey(void);
void FAR PASCAL vbcDisableColorKey(void);
int  FAR PASCAL vbcSetCaptureAddress(WORD, WORD);
int  FAR PASCAL vbcSetDisplayWindow(WORD, WORD, WORD, WORD);
int  FAR PASCAL vbcSetAcquisitionWindow(WORD, WORD, WORD, WORD);
int  FAR PASCAL vbcSmoothVideoImage(WORD, DWORD);

void FAR PASCAL vbcSetVideoInputStart(WORD);
void FAR PASCAL vbcSetCropWinPosition(WORD, WORD);
void FAR PASCAL vbcSetCropWinSize(WORD, WORD);
WORD FAR PASCAL vbcGetCropFactor(int);
void FAR PASCAL vbcResetCropFactors(void);

void FAR PASCAL vbcSetC800Mode(BOOL);
int  FAR PASCAL vbcGetC800Mode();

/* Chips & Technology PCVIDEO.DLL definitions */
int  FAR PASCAL PCV_CreateWindow(WORD, WORD, WORD, WORD, BOOL);
int  FAR PASCAL PCV_SetWindowPosition(WORD, WORD);
int  FAR PASCAL PCV_SetWindowSize(WORD, WORD, BOOL);
int  FAR PASCAL PCV_SetVideoScaling(WORD,WORD,BOOL);
int  FAR PASCAL PCV_SetCaptureAddress(WORD,WORD);
int  FAR PASCAL PCV_SetDisplayWindow(WORD,WORD,WORD,WORD);
int  FAR PASCAL PCV_SetAcquisitionWindow(WORD,WORD,WORD,WORD);
int  FAR PASCAL PCV_PanWindow(WORD, WORD);
int  FAR PASCAL PCV_SetColorKey(WORD);
int  FAR PASCAL PCV_FreezeVideo(void);
int  FAR PASCAL PCV_UnFreezeVideo(void);
int  FAR PASCAL PCV_SetWriteProtectMask(WORD);
int  FAR PASCAL PCV_EnableVideo(void);
int  FAR PASCAL PCV_DisableVideo(void);
int  FAR PASCAL PCV_SaveImageRect(LPSTR, WORD, WORD, WORD, WORD, WORD, WORD);
int  FAR PASCAL PCV_LoadImageRect(LPSTR, WORD, WORD);
int  FAR PASCAL PCV_SaveClipboardFormat(HWND, WORD, WORD, WORD, WORD, WORD, WORD);
int  FAR PASCAL PCV_LoadClipboardFormat(HWND, WORD, WORD);
int  FAR PASCAL PCV_ReadImageRect(PSTR, WORD, WORD, WORD, WORD, WORD, char far *);
int  FAR PASCAL PCV_WriteImageRect(PSTR, PSTR, WORD, WORD, WORD, WORD, WORD);
int  FAR PASCAL PCV_GetInputFormat(void);
int  FAR PASCAL PCV_SetInputFormat(WORD);
void FAR PASCAL PCV_SetColor(WORD, BYTE);
BYTE FAR PASCAL PCV_GetColor(WORD);
void FAR PASCAL PCV_ResetColors(void);
int  FAR PASCAL PCV_ClearVideoRect(WORD, WORD, WORD, WORD);
int  FAR PASCAL PCV_LoadConfiguration(void);
int  FAR PASCAL PCV_SaveConfiguration(void);
int  FAR PASCAL PCV_GetPortAddress(void);
int  FAR PASCAL PCV_SetPortAddress(int);
int  FAR PASCAL PCV_GetVideoAddress(void);
int  FAR PASCAL PCV_SetVideoAddress(int);
int  FAR PASCAL PCV_GetVideoSource(void);
int  FAR PASCAL PCV_SetVideoSource(int);
WORD FAR PASCAL PCV_GetSkewFactor(int);
int  FAR PASCAL PCV_SetSkewFactor(int, WORD);
void FAR PASCAL PCV_ResetSkewFactors(void);
WORD FAR PASCAL PCV_GetSystemMetrics(WORD);
void FAR PASCAL PCV_EnableInterlace(void);
void FAR PASCAL PCV_DisableInterlace(void);
void FAR PASCAL PCV_HorizontalZoom(int);
void FAR PASCAL PCV_VerticalZoom(int);
void FAR PASCAL PCV_EnableFieldReplication(void);
void FAR PASCAL PCV_DisableFieldReplication(void);
void FAR PASCAL PCV_SetRegister(WORD, BYTE);
BYTE FAR PASCAL PCV_GetRegister(WORD);
void FAR PASCAL PCV_TurnBorder(WORD);
int  FAR PASCAL PCV_Initialize(void);
int  FAR PASCAL PCV_Exit(void);
void FAR PASCAL PCV_WaitVGARetrace(void);
void FAR PASCAL PCV_EnableColorKey(void);
void FAR PASCAL PCV_DisableColorKey(void);
WORD FAR PASCAL PCV_GetProfileInt(LPSTR, LPSTR);
int  FAR PASCAL PCV_GetProfileString(LPSTR,LPSTR,LPSTR,int);

/* Color adjustments */
#define BRIGHTNESS	0
#define	SATURATION	1
#define	CONTRAST	2
#define	HUE		3
#define	RED		4
#define	GREEN		5
#define	BLUE		6

/* System metrics */
#define	SM_VIDEOWIDTH	0
#define	SM_VIDEOHEIGHT	1
#define	SM_BOARDTYPE	2
#define	SM_VERSION	3
#define	SM_INTERLACE	4
#define	SM_REPLICATE	5
#define	SM_IMAGEWIDTH	6
#define	SM_IMAGEHEIGHT	7
#define	SM_IMAGETYPE	8

#define INTERLACE_CNTL	0x50
#define	INTERLACE_ON	1
#define	REPLICATE_ON	8

/* Crop metrics */
#define CM_VIDEOSTART   0
#define CM_CROPSTARTX   1
#define CM_CROPSTARTY   2
#define CM_CROPWIDTH    3
#define CM_CROPHEIGHT   4

/* Video board types */
#define	BT_YUV411	0

/* Bitmap types */
#define	BM_DIB24	0		/* Windows DIB 24 bit true color */
#define	BM_DIB8P	1		/* Windows DIB 8 bit palettized */
#define	BM_DIB8G	2		/* Windows DIB 8 bit gray-scale */
#define	BM_DIB4D	3		/* Windows DIB 4 bit dithered */
#define	BM_TRG32	4		/* Targa 32 bit true color */
#define	BM_TRG24	5		/* Targa 24 bit true color */
#define	BM_TRG16	6		/* Targa 16 bit true color */
#define	BM_YUV411	7		/* IBM MMotion format 4:1:1 YUV */
#define	BM_TIFF24	8		/* TIFF 24 bit true color */
#define	BM_TIFF8P	9		/* TIFF 8 bit palettized */
#define	BM_TIFF8G	10		/* TIFF 8 bit gray-scale */
#define	BM_PCX8P	11		/* PCX 8 bit palettized */
#define	BM_PCX8G	12		/* PCX 8 bit gray-scale */
#define	BM_PCX4D	13		/* PCX 4 bit dithered */
#define	BM_GIF8P	14		/* GIF 8 bit palettized */
#define	BM_GIF8G	15		/* GIF 8 bit gray-scale */

/* Palette types */
#define	WM_DIB		1
#define PM_DIB          2
#define PCX8		3
#define PCX4		4
#define TIFF		5
#define GIF		6
#define CF		7

#define NO_COLOR_SETTINGS 8		/* Available color controls */
#define NO_COLOR_CONTROLS 7		/* Actual number of color controls */
#define NO_SKEW_FACTORS	7		/* Number of skew factors */
#define NO_VIDEO_MODES	16		/* Number of video modes supported */
#define NO_PHIXEL_CHIPS	4		/* Number of Phixel chips supported */

/* Configuration flags */
#define CF_PAL		0		/* Input format is PAL */
#define CF_NTSC		1		/* Input format is NTSC */
#define	CF_HASPLL	2		/* Board has a phase-locked loop */
#define	CF_REPLICATE	REPLICATE_ON	/* Replicate field feature enabled */

/* Skew adjustment IDs */
#define	SF_DISPWINSKEWX		0	/* Display Window X Skew */
#define	SF_DISPWINSKEWY		1	/* Display Window Y Skew */
#define	SF_DISPADDRSKEWX	2	/* Display Address X Skew */
#define	SF_DISPADDRSKEWY	3	/* Display Address Y Skew */
#define	SF_SHIFTCLOCKSTART	4	/* Shift Clock Start */
#define	SF_PALETTESKEW		5	/* Palette Skew */
#define	SF_PLLDIVISOR		6	/* Phase Lock Loop Divisor */

/* Skew adjustment ranges */
#define	DISPWINSKEWXRANGE	0x7FF	/* Display Window X Skew Range */
#define	DISPWINSKEWYRANGE	0x7FF	/* Display Window Y Skew Range */
#define	DISPADDRSKEWXRANGE	0x3FF	/* Display Address X Skew Range */
#define	DISPADDRSKEWYRANGE	0x3FF	/* Display Address Y Skew Range */
#define	SHIFTCLOCKSTARTRANGE	0x7F	/* Shift Clock Start Range */
#define	PALETTESKEWRANGE	0x03	/* Palette Skew Range */
#define	PLLDIVISORRANGE		0x3FF	/* Phase Lock Loop Divisor Range */

/* Smooth Method */
#define SMOOTH_AVERAGE      1
#define SMOOTH_REPLICATE    2
