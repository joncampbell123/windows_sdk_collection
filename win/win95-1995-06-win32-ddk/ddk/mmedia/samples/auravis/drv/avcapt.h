//////////////////////////////////////////////////////////////////////////
//	AVCAPT.H							//
//									//
//	Main include file for AVCAPT.DRV				//
//	AuraVision Video Capture Driver.				//
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

//////////////////////////////////////////
//	Definitions and Macros		//
//////////////////////////////////////////

#ifdef DEBUG
	#define static
#endif

#ifdef VCAP_MAIN
	#define EXTERNAL
#else
	#define EXTERNAL	extern
#endif

#define DRIVER_VERSION		0x0400
#define FRAME_INTERRUPT		1	// 0 = Field interrupts, 1 = Frame.
#define MAX_CAPTURE_CHANNELS	1
#define MAX_IN_CHANNELS		1
#define MAX_OUT_CHANNELS	0
#define MAX_DISPLAY_CHANNELS	1
#define MAX_ERR_STRING		250

#define LimitRange(Val,Low,Hi)	(max(Low,(min(Val,Hi))))
#define WidthRect(rect)		(rect.right - rect.left + 1)
#define HeightRect(rect)	(rect.bottom - rect.top + 1)

#define ckidYUV411Compressed	mmioFOURCC('A','U','R','A')	//;sg;

enum	{				// Supported data formats.
	IMAGE_FORMAT_YUV411COMPRESSED,	//;sg;
	IMAGE_FORMAT_PAL8,
	IMAGE_FORMAT_RGB16,
	IMAGE_FORMAT_RGB24
	};


//////////////////////////////////////////
//	Type Definitions		//
//////////////////////////////////////////

typedef struct tag_device_init
	{
	WORD	wIOBase ;       // I/O Base Address.
	BYTE	bInterrupt ;    // Interrupt in use.
	BYTE    bReserved;
	WORD	wSegment ;	// Memory base segment.
        WORD    wSelector ;
        WORD    cAcquire ;
        DWORD   dnDevNode ;     // DevNode from VIDEO_OPEN
	} DEVICE_INIT, NEAR *PDEVICE_INIT, FAR *LPDEVICE_INIT;

typedef struct tag_combobox_entry
	{
	WORD	wValue;
	char	*szText;
	} COMBOBOX_ENTRY;

typedef struct
	{
	DWORD		fccType;	// Capture 'vcap' or codec 'vidc'.
	DWORD		dwOpenType;	// Channel type IN, OUT, EXTERNAL_IN.
	DWORD		dwOpenFlags;	// Flags passed during channel open.
	LPVIDEOHDR	lpVHdr;		// Pointer to first buffer header.
	DWORD		dwError;	// Last error for this stream.
	} CHANNEL, *PCHANNEL;

struct {				// Current palette colors.
	WORD		palVersion;
	WORD		palNumEntries;
	PALETTEENTRY	palPalEntry[256];
	} palCurrent;


//////////////////////////////////////////
//	Global Variables		//
//////////////////////////////////////////
EXTERNAL HANDLE	ghModule;		// Our module handle.
EXTERNAL WORD	gwDriverUsage;		// Usage counts.
EXTERNAL WORD	gwCaptureUsage;
EXTERNAL WORD	gwDisplayUsage;
EXTERNAL WORD	gwVideoInUsage;
EXTERNAL WORD	gwVideoOutUsage;
EXTERNAL LPVOID gpVxDEntry;
EXTERNAL DEVICE_INIT	devInit;


//	Globals set by the Configuration dialog.

EXTERNAL WORD	wPCVideoAddress;	// Address of PCVIDEO chip.
EXTERNAL WORD PASCAL gwBaseReg;		// inita.asm
EXTERNAL BYTE PASCAL gbInt;		// inita.asm
EXTERNAL WORD	gwWaitState;		// Both I/O, memory access.
EXTERNAL BOOL	gfEurope;		// TRUE if PAL format.

//	The following are controlled by the Video Source dialog.

EXTERNAL WORD	gwSourceConnector;	// 0 to n Connectors.
EXTERNAL WORD	gwVideoCableFormat;	// 0=Composite, 1=SVideo, 2=RGB.

//	The following are controlled by the Video Format dialog.

EXTERNAL WORD	gwDestFormat;		// One of IMAGE_FORMAT enums.
EXTERNAL WORD	gwWidthBytes;		// Width of a scan in bytes.
EXTERNAL WORD	gwWidth;		// Dimensions of the image
EXTERNAL WORD	gwHeight;		//   we are currently using.
EXTERNAL WORD	gwSize40;		// Value * 40 gives capture width.
EXTERNAL DWORD	dwVideoClock;		// Video frame counter.

EXTERNAL WORD	gwPadFlag;		//;sg;

EXTERNAL WORD	wIRQStatus;		//;sg; Last register 0x61 read.
EXTERNAL WORD	wCapStatus;		//;sg; Last register 0x64 read.

EXTERNAL BOOL	gfEnabled;		// Has card been enabled?
EXTERNAL BOOL	gfVideoInStarted;	// Is the InStream running?
EXTERNAL LPVOID PASCAL glpFrameBuffer;	// Linear memory pointer.
EXTERNAL LPBYTE	fpTrans16to8;		// mapc.c
EXTERNAL LPBYTE	fpCopyBuffer;		// When streaming, copy image here.
EXTERNAL LPBYTE	fpCopyBuffer2;		// When streaming, copy image here.
EXTERNAL LPWORD	fpYUVtoRGB16;		// LUT for converting to RGB16.

EXTERNAL LPWORD	fpYUVtoCompressed;	//;sg; Table for compressing YUV.

EXTERNAL BITMAPINFOHEADER biSource;	// Current source format.
EXTERNAL BITMAPINFOHEADER biDest;	// Current dest format.
EXTERNAL WORD	gwWidthBytesSource;	// Width of scan in bytes.
EXTERNAL WORD	gwWidthBytesDest;	// Width of scan in bytes.
EXTERNAL RECT	grcDestExtIn;		// Where to digitize incoming video.
EXTERNAL RECT	grcSourceIn;		// Source Rect for VideoIn.
EXTERNAL RECT	grcDestIn;		// Dest Rect for VideoIn.
EXTERNAL RECT	grcSourceExtOut;	// Source Rect for ExternalOut.
EXTERNAL RECT	grcDestExtOut;		// Dest Rect for ExternalOut.

#ifdef DEBUG
	EXTERNAL WORD	wDebugLevel;	// Debug level.
#endif

EXTERNAL WORD   wAVPort;		// VxP-500 Port Address.


//////////////////////////////////////////
//	Strings				//
//////////////////////////////////////////

#define BCODE

#ifdef VCAP_MAIN
	char BCODE	gszIniFile[]		= "system.ini";

	char BCODE	gszPortKey[]		= "PortBase";
	char BCODE	gszIntKey[]		= "Interrupt";
	char BCODE	gszMemoryKey[]		= "MemoryBase";
	char BCODE	gszWaitStateKey[]	= "WaitStates";
	char BCODE	gszBoardTypeKey[]	= "BoardType";

	char BCODE	gszHueKey[]		= "Hue";
	char BCODE	gszSatKey[]		= "Saturation";
	char BCODE	gszContrastKey[]	= "Contrast";
	char BCODE	gszBrightnessKey[]	= "Brightness";
	char BCODE	gszRedKey[]		= "Red";
	char BCODE	gszGreenKey[]		= "Green";
	char BCODE	gszBlueKey[]		= "Blue";
	char BCODE	gszInputChannelKey[]	= "InputChannel";
	char BCODE	gszVideoStandardKey[]	= "VideoStandard";
	char BCODE	gszSize40Key[]		= "Size40";
	char BCODE	gszVideoFormatKey[]	= "VideoFormat";
	char BCODE	gszVideoCableKey[]	= "VideoCable";

	char BCODE	gszHexFormat[]		= "%X";
	char BCODE	gszIntFormat[]		= "%d";
	char BCODE	gszNULL[]		= "";
#else
	extern char BCODE	gszDriverName[];
	extern char BCODE	gszIniFile[];

	extern char BCODE	gszPortKey[];
	extern char BCODE	gszIntKey[];
	extern char BCODE	gszMemoryKey[];
	extern char BCODE	gszWaitStateKey[];
	extern char BCODE	gszBoardTypeKey[];

	extern char BCODE	gszHueKey[];
	extern char BCODE	gszSatKey[];
	extern char BCODE	gszContrastKey[];
	extern char BCODE	gszBrightnessKey[];
	extern char BCODE	gszRedKey[];
	extern char BCODE	gszGreenKey[];
	extern char BCODE	gszBlueKey[];
	extern char BCODE	gszInputChannelKey[];  
	extern char BCODE	gszVideoStandardKey[];      
	extern char BCODE	gszSize40Key[];
	extern char BCODE	gszVideoFormatKey[];
	extern char BCODE	gszVideoCableKey[];

	extern char BCODE	gszHexFormat[];
	extern char BCODE	gszIntFormat[];
	extern char BCODE	gszNULL[];
#endif 


#define IDS_ERRBADPORT		1	// Boot time error message.
#define IDS_ERRBADCONFIG	2	// Config time error message.

#define IDS_VCAPPRODUCT		16
#define IDS_VCAPIN		IDS_VCAPPRODUCT
#define IDS_VCAPOUT		IDS_VCAPPRODUCT


//////////////////////////////////////////
//	Color Control Definitions	//
//////////////////////////////////////////

#define HW_COLOR_HUE            0
#define HW_COLOR_BRIGHTNESS     1
#define HW_COLOR_SAT            2
#define HW_COLOR_CONTRAST       3
#define HW_COLOR_RED            4
#define HW_COLOR_GREEN          5
#define HW_COLOR_BLUE           6


//////////////////////////////////////////
//	Function Prototypes		//
//////////////////////////////////////////

//	cap.c

DWORD FAR PASCAL	InStreamError(LPDWORD lpdwErrorType, LPDWORD lpdwFramesSkipped);
DWORD FAR PASCAL	InStreamGetPos(LPMMTIME lpMMTime, DWORD dwSize);
WORD  FAR PASCAL	InStreamOpen(LPVIDEO_STREAM_INIT_PARMS lpStreamInitParms);
WORD  FAR PASCAL	InStreamClose(void);
WORD  FAR PASCAL	InStreamPrepareBuffer(LPVIDEOHDR lpVHdr);
WORD  FAR PASCAL	InStreamUnprepareBuffer(LPVIDEOHDR lpVHdr);
WORD  FAR PASCAL	InStreamAddBuffer(LPVIDEOHDR lpVHdr);
WORD  FAR PASCAL	InStreamStart(void);
WORD  FAR PASCAL	InStreamStop(void);
WORD  FAR PASCAL	InStreamReset(void);
void  NEAR PASCAL	InStreamISR(void);
WORD  FAR PASCAL	CaptureFrame(LPVIDEOHDR lpHdr);
void FAR PASCAL		videoCallback(WORD msg, DWORD dw1);

//	config.c

int FAR PASCAL _loadds	ConfigDlgProc(HWND hDlg, WORD msg, WORD wParam, LONG lParam);
int FAR PASCAL _loadds	VideoSourceDlgProc(HWND hDlg, WORD msg, WORD wParam, LONG lParam);
int FAR PASCAL _loadds	VideoFormatDlgProc(HWND hDlg, WORD msg,WORD wParam, LONG lParam);
int FAR PASCAL _loadds	VideoDisplayDlgProc(HWND hDlg, WORD msg, WORD wParam, LONG lParam);
int  FAR PASCAL		Config(HWND hWnd, HANDLE hModule);
void FAR PASCAL		ConfigRemove(void);
BOOL FAR PASCAL		ConfigCheckAllDeviceInitParms(LPDEVICE_INIT lpDI);
BOOL FAR PASCAL		ConfigInit(LPDEVICE_INIT lpDI);
BOOL FAR PASCAL		SetFormatFromDIB(LPBITMAPINFOHEADER lpbi);
BOOL FAR PASCAL		SetFormat(WORD wBoardType, WORD wZoom, WORD wBitDepth, WORD wIndex);

//	flat.asm

int FAR PASCAL		GetFrameBufferPointer(BYTE bSegment);
int FAR PASCAL		FreeFrameBufferSelector(void);

//	avdev.c

void FAR PASCAL		HW_WriteRegister(int nIndex, int nValue);	//;sg;
int FAR PASCAL		HW_ReadRegister(int nIndex);			//;sg;
void FAR PASCAL		HW_InitializeCapture(int nWidth, int nHeight,	//;sg;
	DWORD dFrameRate, DWORD dTickRate);				//;sg;
void FAR PASCAL		HW_StartCapture();
void FAR PASCAL		HW_WaitForCaptureData();			//;sg;
void FAR PASCAL		HW_StopCapture();
void FAR PASCAL		HW_DeinitializeCapture();			//;sg;
void FAR PASCAL		HW_ConfigureDialogBox();			//;sg;
BOOL FAR PASCAL		HW_CheckAcquire();				//;sg;
BOOL FAR PASCAL		HW_CheckCapture();				//;sg;
WORD FAR PASCAL		HW_GetPadFlag(WORD wWidth);			//;sg;

void FAR PASCAL		HW_WritePCVideo(int nIndex, int nValue);
int FAR PASCAL		HW_ReadPCVideo(int nIndex);
int FAR PASCAL		HW_Init(void);
void FAR PASCAL		HW_Fini(void);
void FAR PASCAL		HW_SetPortAddress(int nPort);
int FAR PASCAL		HW_GetPortAddress(void);
int FAR PASCAL		HW_GetFrameAddress(void);
int FAR PASCAL		HW_SetFrameAddress(WORD wSegment, WORD wSelector) ;
int FAR PASCAL		HW_LoadConfiguration(LPSTR lpszFile);
int FAR PASCAL		HW_SaveConfiguration(LPSTR lpszFile);
void FAR PASCAL		HW_SetColor(int nColorReg, int nColorValue);
HBRUSH FAR PASCAL	HW_SetKeyColor(void);
void FAR PASCAL		HW_SetVideoSource(int nSource);
int FAR PASCAL		HW_GetVideoSource(void);
int FAR PASCAL		HW_GetVideoChannelCount (void);
void FAR PASCAL		HW_SetVideoStandard(int nStandard);
int FAR PASCAL		HW_GetVideoStandard(void);
BOOL FAR PASCAL		HW_HasSVideo(void);
BOOL FAR PASCAL		HW_SetVideoCableFormat (int nInputMode);
void FAR PASCAL		HW_Acquire(int fAcquire);
void FAR PASCAL		HW_PrivateAcquire(int fAcquire);
void FAR PASCAL		HW_GrabFrame(void);
void FAR PASCAL		HW_OverlayEnable(BOOL fDisplay);
int FAR PASCAL		HW_SetIRQUsed (int nIRQ);
int FAR PASCAL		HW_GetIRQUsed(void);
void FAR PASCAL		HW_IRQEnable(void);
void FAR PASCAL		HW_IRQDisable(void);
void FAR PASCAL		HW_IRQClear(void);
void FAR PASCAL		HW_Update(HWND hWnd, HDC hDC);
void FAR PASCAL		HW_SetDisplayRect(LPRECT lpRectangle);
void FAR PASCAL		HW_SetPanAndScroll(LPRECT lpRectangle);
void FAR PASCAL		HW_WaitVSync(int nSync);

//	drvproc.c

LRESULT FAR PASCAL _loadds DriverProc(DWORD dwDriverID, HDRVR hDriver, UINT uiMessage, LPARAM lParam1, LPARAM lParam2);

//	flat.asm

LPSTR FAR PASCAL	CreatePhysicalSelector( DWORD dwBase, WORD wLimit );

//	initc.c

int FAR PASCAL   LibMain(HMODULE hModule, UINT uDataSeg,
						 UINT uHeapSize, LPCSTR lpCmdLine);
int FAR PASCAL		HardwareInit(LPDEVICE_INIT lpDI);
void FAR PASCAL	HardwareFini(void);
WORD FAR PASCAL	ConfigGetSettings(void);
WORD FAR PASCAL	ConfigPutSettings(void);
int FAR PASCAL		GetHardwareSettingsFromINI(LPDEVICE_INIT lpDI);
int FAR PASCAL		PutHardwareSettingsToINI(LPDEVICE_INIT lpDI);
int FAR PASCAL		InitVerifyConfiguration(LPDEVICE_INIT lpDI);
DWORD FAR PASCAL	InitCheckMem(void);

//	inita.asm

WORD FAR PASCAL		IRQEnable(void);
WORD FAR PASCAL		IRQDisable(void);

//	mapa.asm

void NEAR PASCAL	mapYUV422toPAL8( LPSTR fpDst, LPSTR fpSrc,
			LPSTR fpTrans, WORD wWidth, WORD wHeight,
			WORD dxSrc);
void NEAR PASCAL	mapYUV422toRGB16( LPSTR fpDst, LPSTR fpSrc,
			LPWORD fpTrans, WORD wWidth, WORD wHeight,
			WORD dxSrc);
void NEAR PASCAL	mapYUV422toRGB24( LPSTR fpDst, LPSTR fpSrc,
			LPWORD fpTrans, WORD wWidth, WORD wHeight,
			WORD dxSrc);

void NEAR PASCAL	mapYUV411D4toYUV411D4(			//;sg;
			LPSTR fpDst, LPSTR fpSrc,		//;sg;
			LPWORD fpTrans, WORD wWidth,		//;sg;
			WORD wHeight, WORD dxSrc,		//;sg;
			WORD wPadFlag);				//;sg;

void NEAR PASCAL	mapYUV411D4toYUV422(			//;sg;
			LPSTR fpDst, LPSTR fpSrc,		//;sg;
			LPWORD fpTrans, WORD wWidth,		//;sg;
			WORD wHeight, WORD dxSrc);		//;sg;


//	mapc.c

WORD FAR PASCAL		TransInit(void);
void FAR PASCAL		TransFini(void);
BOOL FAR PASCAL		TransRecalcPal(HPALETTE hPal);
BOOL FAR PASCAL		TransSet(LPBYTE);

//	muldiv.asm

DWORD NEAR PASCAL	muldiv32(DWORD, DWORD, DWORD);

//	vcap.c

DWORD FAR PASCAL	GetInSourceRect(LPRECT lprc);
DWORD FAR PASCAL	SetInSourceRect(LPRECT lprc);
DWORD FAR PASCAL	GetInDestRect(LPRECT lprc);
DWORD FAR PASCAL	SetInDestRect(LPRECT lprc);
DWORD FAR PASCAL	GetSourceFormat(LPBITMAPINFOHEADER lpbi, WORD wSize);
DWORD FAR PASCAL	SetSourceFormat(LPBITMAPINFOHEADER lpbi, WORD wSize);
DWORD FAR PASCAL	GetDestFormat(LPBITMAPINFOHEADER lpbi, WORD wSize);
DWORD FAR PASCAL	SetDestFormat(LPBITMAPINFOHEADER lpbi, WORD wSize);
DWORD FAR PASCAL	GetDestPalette(LPLOGPALETTE lppal, WORD wSize);
DWORD FAR PASCAL	SetDestPalette(LPLOGPALETTE lppal, LPBYTE lpXlat);
DWORD FAR PASCAL	SetExtOutSourceRect(LPRECT lprc);
DWORD FAR PASCAL	SetExtOutDestRect(LPRECT lprc);

//	vmsg.c

PCHANNEL NEAR PASCAL	VideoOpen(LPVIDEO_OPEN_PARMS lpOpenParms);
DWORD NEAR PASCAL	VideoClose(PCHANNEL pChannel);
DWORD NEAR PASCAL	VideoConfigureStorageMessage(PCHANNEL pChannel,
			UINT msg, LONG lParam1, LONG lParam2);
DWORD NEAR PASCAL	VideoConfigureMessage(PCHANNEL pChannel, UINT msg,
			LONG lParam1, LONG lParam2);
DWORD NEAR PASCAL	VideoStreamMessage(PCHANNEL pChannel, UINT msg,
			LONG lParam1, LONG lParam2);
DWORD NEAR PASCAL	VideoProcessMessage(PCHANNEL pChannel, UINT msg,
			LONG lParam1, LONG lParam2);

//	yuv.c

void FAR PASCAL		HW_RGB2YUV(LPSTR lpRGBBuf, LPSTR lpYUVBuf,
			long nLineLen, int nBitsPerPix);
void FAR PASCAL		HW_YUV2RGB(LPSTR lpYUVBuf, LPSTR lpRGBBuf,
			int nLineLen, int nBitsPerPix);
void FAR PASCAL		HW_YUV2RGBNoInterp(BYTE huge *lpYUVBuf,
			BYTE huge *lpRGBBuf, int nLineLen, int nBitsPerPix);
void FAR PASCAL		HW_PackYUV(LPSTR lpUnpackedBuf, LPSTR lpPackedBuf,
			int nLineLen);
void FAR PASCAL		HW_UnpackYUV(LPSTR lpPackedBuf, LPSTR lpUnpackedBuf,
			int nLineLen);
