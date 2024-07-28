/****************************************************************************
 *
 *   ct.h
 * 
 *   Main include file.  No device specific parameters should appear here.
 *
 *   Microsoft Video for Windows Sample Capture Driver
 *   Chips & Technologies 9001 based frame grabbers.
 *
 ***************************************************************************/
/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1992, 1993  Microsoft Corporation.  All Rights Reserved.
 * 
 **************************************************************************/


 // want lots of symbols in DEBUG .sym file..
#ifdef DEBUG
    #define static
#endif

#ifdef VCAP_MAIN
#define EXTERNAL
#else
#define EXTERNAL extern
#endif

#define DRIVER_VERSION          0x0101

#define FRAME_INTERRUPT            1 // 0 = Field interrupts, 1 = Frame 

#define MAX_CAPTURE_CHANNELS       1
#define MAX_IN_CHANNELS            1
#define MAX_OUT_CHANNELS           0
#define MAX_DISPLAY_CHANNELS       1

#define MAX_ERR_STRING          250     // max length of string table errors

#define LimitRange(Val,Low,Hi) (max(Low,(min(Val,Hi))))
#define WidthRect(rect) (rect.right - rect.left + 1)
#define HeightRect(rect) (rect.bottom - rect.top + 1)

enum { 
    IMAGE_FORMAT_PAL8,           // Supported data formats
    IMAGE_FORMAT_RGB16,
    IMAGE_FORMAT_RGB24,
    IMAGE_FORMAT_YUV411PACKED,
    IMAGE_FORMAT_YUV411UNPACKED
};

#define ckidYUV411Packed         mmioFOURCC('y','u','v','p')
#define ckidYUV411Unpacked       mmioFOURCC('y','u','v','u')

/***************************************************************************

    typedefs

 **************************************************************************/
typedef struct tag_device_init {
   WORD wIOBase;        // I/O Base Address
   BYTE bInterrupt;     // Interrupt in use
   WORD wSegment;       // Memory base segment
} DEVICE_INIT, NEAR *PDEVICE_INIT, FAR *LPDEVICE_INIT;

typedef struct tag_combobox_entry {
   WORD wValue;
   char *szText;
} COMBOBOX_ENTRY;

typedef struct {
   DWORD      fccType;              // Capture 'vcap' or codec 'vidc'
   DWORD      dwOpenType;           // Channel type IN, OUT, EXTERNAL_IN...
   DWORD      dwOpenFlags;          // Flags passed during channel open
   LPVIDEOHDR lpVHdr;               // Pointer to first buffer header
   DWORD      dwError;              // Last error for this stream
} CHANNEL, *PCHANNEL;


/***************************************************************************
    globals
***************************************************************************/
EXTERNAL HANDLE   ghModule;                 // our module handle
EXTERNAL WORD	  gwDriverUsage;            // Usage count
EXTERNAL WORD	  gwCaptureUsage;           // Usage count
EXTERNAL WORD	  gwDisplayUsage;           // Usage count
EXTERNAL WORD	  gwVideoInUsage;           // Usage count
EXTERNAL WORD	  gwVideoOutUsage;          // Usage count

/* globals set by the Configuration dialog */
EXTERNAL WORD   wPCVideoAddress;     // Address of PCVIDEO chip
EXTERNAL WORD PASCAL  gwBaseReg;     // inita.asm
EXTERNAL BYTE PASCAL  gbInt;         // inita.asm
EXTERNAL WORD   gwWaitState;         // both I/O, memory access
EXTERNAL BOOL   gfEurope;            // TRUE if PAL, European standard

// The following are controlled by the Video Source dialog
EXTERNAL WORD     gwSourceConnector; // 0 to nConnectors
EXTERNAL WORD     gwVideoCableFormat; // 0=Composite, 1=SVideo, 2=RGB

// The following are controlled by the Video Format dialog
EXTERNAL WORD     gwDestFormat;      // one of IMAGE_FORMAT enums
EXTERNAL WORD     gwWidthBytes;      // width of a scan in bytes
EXTERNAL WORD     gwWidth;           // Dimensions of the image
EXTERNAL WORD     gwHeight;          //   we are currently using
EXTERNAL WORD     gwSize40;          // Value * 40 gives capture width
EXTERNAL DWORD    dwVideoClock;      // Video frame counter

struct {                             // current palette colors.
    WORD         palVersion;
    WORD         palNumEntries;
    PALETTEENTRY palPalEntry[256];
}   palCurrent;

EXTERNAL BOOL           gfEnabled;         // has the card been enabled?
EXTERNAL BOOL           gfVideoInStarted;  // Is the InStream running?
extern   LPVOID PASCAL  glpFrameBuffer;    // Linear memory pointer
EXTERNAL LPBYTE         fpTrans16to8;      // mapc.c
EXTERNAL LPBYTE         fpCopyBuffer;      // When streaming, copy image here
EXTERNAL LPWORD         fpYUVtoRGB16;      // LUT for converting to RGB16

EXTERNAL BITMAPINFOHEADER    biSource;     // current source format.
EXTERNAL BITMAPINFOHEADER    biDest;       // current dest format.
EXTERNAL WORD           gwWidthBytesSource; // width of scan in bytes
EXTERNAL WORD           gwWidthBytesDest;   // width of scan in bytes
EXTERNAL RECT           grcDestExtIn;       // Where to digitize incoming video
EXTERNAL RECT           grcSourceIn;        // Source Rect for VideoIn
EXTERNAL RECT           grcDestIn;          // Dest   Rect for VideoIn
EXTERNAL RECT           grcSourceExtOut;    // Source Rect for ExternalOut
EXTERNAL RECT           grcDestExtOut;      // Dest   Rect for ExternalOut

#ifdef DEBUG
EXTERNAL WORD     wDebugLevel;              // debug level
#endif

/****************************************************************************

       strings

 ***************************************************************************/
#define BCODE

#ifdef VCAP_MAIN
       char BCODE gszIniFile[]                 = "system.ini";

       char BCODE gszPortKey[]                 = "PortBase";
       char BCODE gszIntKey[]                  = "Interrupt";
       char BCODE gszMemoryKey[]               = "MemoryBase";
       char BCODE gszWaitStateKey[]            = "WaitStates";
       char BCODE gszBoardTypeKey[]            = "BoardType";

       char BCODE gszHueKey[]                  = "Hue";
       char BCODE gszSatKey[]                  = "Saturation";
       char BCODE gszContrastKey[]             = "Contrast";
       char BCODE gszBrightnessKey[]           = "Brightness";
       char BCODE gszRedKey[]                  = "Red";
       char BCODE gszGreenKey[]                = "Green";
       char BCODE gszBlueKey[]                 = "Blue";
       char BCODE gszInputChannelKey[]         = "InputChannel";
       char BCODE gszVideoStandardKey[]        = "VideoStandard";
       char BCODE gszSize40Key[]               = "Size40";
       char BCODE gszVideoFormatKey[]          = "VideoFormat";
       char BCODE gszVideoCableKey[]           = "VideoCable";

       char BCODE gszHexFormat[]               = "%X";
       char BCODE gszIntFormat[]               = "%d";
       char BCODE gszNULL[]                    = "";
#else
       extern char BCODE gszDriverName[];
       extern char BCODE gszIniFile[];

       extern char BCODE gszPortKey[];
       extern char BCODE gszIntKey[];
       extern char BCODE gszMemoryKey[];
       extern char BCODE gszWaitStateKey[];
       extern char BCODE gszBoardTypeKey[];

       extern char BCODE gszHueKey[];
       extern char BCODE gszSatKey[];
       extern char BCODE gszContrastKey[];
       extern char BCODE gszBrightnessKey[];
       extern char BCODE gszRedKey[];
       extern char BCODE gszGreenKey[];
       extern char BCODE gszBlueKey[];
       extern char BCODE gszInputChannelKey[];  
       extern char BCODE gszVideoStandardKey[];      
       extern char BCODE gszSize40Key[];
       extern char BCODE gszVideoFormatKey[];
       extern char BCODE gszVideoCableKey[];

       extern char BCODE gszHexFormat[];
       extern char BCODE gszIntFormat[];
       extern char BCODE gszNULL[];
#endif 


#define IDS_ERRBADPORT      1           // boot time error message
#define IDS_ERRBADCONFIG    2           // config time error message

//
//  Product Description strings.  For this driver, all descriptions are
//  the same..
//
#define IDS_VCAPPRODUCT   16
#define IDS_VCAPIN    IDS_VCAPPRODUCT
#define IDS_VCAPOUT   IDS_VCAPPRODUCT

/***************************************************************************
    CT_ definitions
***************************************************************************/
// These follow the Bravado definitions, others may vary
#define CT_COLOR_HUE            0
#define CT_COLOR_BRIGHTNESS     1
#define CT_COLOR_SAT            2
#define CT_COLOR_CONTRAST       3
#define CT_COLOR_RED            4
#define CT_COLOR_GREEN          5
#define CT_COLOR_BLUE           6

/***************************************************************************

    prototypes

***************************************************************************/

//cap.c
DWORD FAR PASCAL InStreamError(LPDWORD lpdwErrorType, LPDWORD lpdwFramesSkipped);
DWORD FAR PASCAL InStreamGetPos( LPMMTIME lpMMTime, DWORD dwSize);
WORD  FAR PASCAL InStreamOpen( LPVIDEO_STREAM_INIT_PARMS lpStreamInitParms );
WORD  FAR PASCAL InStreamClose( void );
WORD  FAR PASCAL InStreamPrepareBuffer( LPVIDEOHDR lpVHdr );
WORD  FAR PASCAL InStreamUnprepareBuffer( LPVIDEOHDR lpVHdr );
WORD  FAR PASCAL InStreamAddBuffer( LPVIDEOHDR lpVHdr );
WORD  FAR PASCAL InStreamStart( void );
WORD  FAR PASCAL InStreamStop( void );
WORD  FAR PASCAL InStreamReset( void );
void  NEAR PASCAL InStreamISR( void );
WORD  FAR PASCAL CaptureFrame( LPVIDEOHDR lpHdr );
void FAR PASCAL videoCallback(WORD msg, DWORD dw1);

// config.c
int FAR PASCAL _loadds ConfigDlgProc(HWND hDlg, WORD msg, WORD wParam, LONG lParam);
int FAR PASCAL _loadds VideoSourceDlgProc(HWND hDlg, WORD msg, WORD wParam, LONG lParam);
int FAR PASCAL _loadds VideoFormatDlgProc(HWND hDlg, WORD msg,WORD wParam, LONG lParam);
int FAR PASCAL _loadds VideoMonitorDlgProc(HWND hDlg, WORD msg, WORD wParam, LONG lParam);

int  FAR PASCAL Config(HWND hWnd, HANDLE hModule);
void FAR PASCAL ConfigRemove(void);
BOOL FAR PASCAL ConfigCheckAllDeviceInitParms (LPDEVICE_INIT lpDI);
BOOL FAR PASCAL ConfigInit (LPDEVICE_INIT lpDI);
BOOL FAR PASCAL SetFormatFromDIB (LPBITMAPINFOHEADER lpbi);
BOOL FAR PASCAL SetFormat (WORD wBoardType, WORD wZoom, WORD wBitDepth, WORD wIndex);

// flat.asm
int FAR PASCAL GetFrameBufferPointer (BYTE bSegment);
int FAR PASCAL FreeFrameBufferSelector (void);

// ctdev.c
void FAR PASCAL  CT_WritePCVideo(int nIndex, int nValue);
int FAR PASCAL   CT_ReadPCVideo(int nIndex);
int FAR PASCAL   CT_Init(void);
void FAR PASCAL  CT_Fini(void);
void FAR PASCAL  CT_SetPortAddress(int nPort);
int FAR PASCAL   CT_GetPortAddress(void);
int FAR PASCAL   CT_GetFrameAddress(void);
int FAR PASCAL   CT_SetFrameAddress(int nVidAddr);
int FAR PASCAL   CT_LoadConfiguration(LPSTR lpszFile);
int FAR PASCAL   CT_SaveConfiguration(LPSTR lpszFile);
void FAR PASCAL  CT_SetColor(int nColorReg, int nColorValue);
HBRUSH FAR PASCAL  CT_SetKeyColor(void);
void FAR PASCAL  CT_SetVideoSource(int nSource);
int FAR PASCAL   CT_GetVideoChannelCount (void);
void FAR PASCAL  CT_SetVideoStandard(int nStandard);
BOOL FAR PASCAL  CT_HasSVideo(void);
BOOL FAR PASCAL  CT_SetVideoCableFormat (int nInputMode);
void FAR PASCAL  CT_Acquire(int fAcquire);
void FAR PASCAL  CT_PrivateAcquire(int fAcquire);
void FAR PASCAL  CT_GrabFrame(void);
void FAR PASCAL  CT_OverlayEnable(BOOL fDisplay);
int FAR PASCAL   CT_SetIRQUsed (int nIRQ);
int FAR PASCAL   CT_GetIRQUsed(void);
void FAR PASCAL  CT_IRQEnable(void);
void FAR PASCAL  CT_IRQDisable(void);
void FAR PASCAL  CT_IRQClear (void);
void FAR PASCAL  CT_Update (HWND hWnd, HDC hDC);
void FAR PASCAL  CT_SetDisplayRect (LPRECT lpRectangle);
void FAR PASCAL  CT_SetPanAndScroll (LPRECT lpRectangle);
void FAR PASCAL  CT_WaitVSync (int nSync);

// drvproc.c
LRESULT FAR PASCAL _loadds DriverProc(DWORD dwDriverID, HDRVR hDriver, UINT uiMessage, LPARAM lParam1, LPARAM lParam2);

// flat.asm
LPSTR FAR PASCAL CreatePhysicalSelector( DWORD dwBase, WORD wLimit );

// initc.c
int NEAR PASCAL LibMain(HANDLE hModule, WORD wHeapSize, LPSTR lpCmdLine);
int FAR PASCAL HardwareInit(LPDEVICE_INIT lpDI);
void FAR PASCAL HardwareFini (void);
WORD FAR PASCAL ConfigGetSettings(void);
WORD FAR PASCAL ConfigPutSettings(void);
void FAR PASCAL HardErrorMsgBox( WORD wStringId );
int FAR PASCAL GetHardwareSettingsFromINI( LPDEVICE_INIT lpDI );
int FAR PASCAL PutHardwareSettingsToINI( LPDEVICE_INIT lpDI );
int FAR PASCAL InitVerifyConfiguration(LPDEVICE_INIT lpDI);
DWORD FAR PASCAL InitCheckMem (void);

// inita.asm
WORD FAR PASCAL IRQEnable(void);
WORD FAR PASCAL IRQDisable(void);

// mapa.asm
void NEAR PASCAL mapUnpackedYUVto8( LPSTR fpDst, LPSTR fpSrc, LPSTR fpTrans,
        WORD wWidth, WORD wHeight, WORD dxSrc);
void NEAR PASCAL mapUnpackedYUVtoRGB16( LPSTR fpDst, LPSTR fpSrc, LPWORD fpTrans,
        WORD wWidth, WORD wHeight, WORD dxSrc);
void NEAR PASCAL RectCopyBytes(LPVOID pDst, WORD wDstWidth, 
                               LPVOID pSrc, WORD wSrcWidth,
                               WORD xSrc, WORD ySrc, WORD dxSrc, WORD dySrc);

// mapc.c
WORD FAR PASCAL TransInit( void );
void FAR PASCAL TransFini( void );
BOOL FAR PASCAL TransRecalcPal( HPALETTE hPal );
BOOL FAR PASCAL TransSet( LPBYTE );

//muldiv.asm
DWORD NEAR PASCAL muldiv32(DWORD,DWORD,DWORD);

//vcap.c
DWORD FAR PASCAL GetInSourceRect(LPRECT lprc);
DWORD FAR PASCAL SetInSourceRect(LPRECT lprc);
DWORD FAR PASCAL GetInDestRect(LPRECT lprc);
DWORD FAR PASCAL SetInDestRect(LPRECT lprc);
DWORD FAR PASCAL GetSourceFormat(LPBITMAPINFOHEADER lpbi, WORD wSize);
DWORD FAR PASCAL SetSourceFormat(LPBITMAPINFOHEADER lpbi, WORD wSize);
DWORD FAR PASCAL GetDestFormat(LPBITMAPINFOHEADER lpbi, WORD wSize);
DWORD FAR PASCAL SetDestFormat(LPBITMAPINFOHEADER lpbi, WORD wSize);
DWORD FAR PASCAL GetDestPalette(LPLOGPALETTE lppal, WORD wSize);
DWORD FAR PASCAL SetDestPalette(LPLOGPALETTE lppal, LPBYTE lpXlat);
DWORD FAR PASCAL SetExtOutSourceRect(LPRECT lprc);
DWORD FAR PASCAL SetExtOutDestRect(LPRECT lprc);

//vmsg.c
PCHANNEL NEAR PASCAL VideoOpen(LPVIDEO_OPEN_PARMS lpOpenParms);
DWORD NEAR PASCAL VideoClose(PCHANNEL pChannel);
DWORD NEAR PASCAL VideoConfigureStorageMessage(PCHANNEL pChannel, UINT msg, LONG lParam1, LONG lParam2);
DWORD NEAR PASCAL VideoConfigureMessage(PCHANNEL pChannel, UINT msg, LONG lParam1, LONG lParam2);
DWORD NEAR PASCAL VideoStreamMessage(PCHANNEL pChannel, UINT msg, LONG lParam1, LONG lParam2);
DWORD NEAR PASCAL VideoProcessMessage(PCHANNEL pChannel, UINT msg, LONG lParam1, LONG lParam2);

//yuv.c
void FAR PASCAL CT_RGB2YUV(LPSTR lpRGBBuf, LPSTR lpYUVBuf, long nLineLen, int nBitsPerPix);
void FAR PASCAL CT_YUV2RGB(LPSTR lpYUVBuf, LPSTR lpRGBBuf, int nLineLen, int nBitsPerPix);
void FAR PASCAL CT_YUV2RGBNoInterp(BYTE huge *lpYUVBuf, BYTE huge *lpRGBBuf, 
        int nLineLen, int nBitsPerPix);
void FAR PASCAL CT_PackYUV(LPSTR lpUnpackedBuf, LPSTR lpPackedBuf, int nLineLen);
void FAR PASCAL CT_UnpackYUV(LPSTR lpPackedBuf, LPSTR lpUnpackedBuf, int nLineLen);

