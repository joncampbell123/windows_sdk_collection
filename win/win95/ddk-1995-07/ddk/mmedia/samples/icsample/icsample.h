/****************************************************************************
 *
 *   icsample.h
 *
 ***************************************************************************/
/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1992 - 1995  Microsoft Corporation.  All Rights Reserved.
 * 
 **************************************************************************/

typedef struct {
    WORD        wPixelKeepRatio;
    WORD        wColorBitsToDrop;
} DECOMPRESS_INFO;

typedef struct {
    DWORD            fccHandler;             // this should be your handler
    DECOMPRESS_INFO  dInfo;
} ICSTATE;

typedef struct {
    DWORD            dwSize;
    DECOMPRESS_INFO  dInfo;
} DECOMP_FORMAT, *PDECOMP_FMT, FAR *LPDECOMP_FMT;

typedef struct {
/********************************************************************/
//
//  These two fields MUST be first and in this order.
//
    DWORD       fccType;         // gives type of our state info ('vidc')
                                 // this allows us to combine 'vcap' or other
                                 // stream/ICM types into one driver
    DRIVERPROC  DriverProc;      // driver proc for the instance
/********************************************************************/
    DWORD       dwFlags;         // flags from ICOPEN
    ICSTATE     CurrentState;	   // current state of compressor.
    DECOMP_FORMAT DecompressFmt; // format for decompression
    BOOL        fCompress;       // count of COMPRESS_BEGIN calls
    BOOL        fDecompress;     // count of DECOMPRESS_BEGIN calls
    BOOL        fDrawBegun;      // have we received a DRAW_BEGIN?
    LONG (CALLBACK *Status) (LPARAM lParam, UINT message, LONG l);
    LPARAM	lParam;	            // for status procedure
} INSTINFO, *PINSTINFO;

#define STATUS_EVERY	16	// report status every 16 lines....

BOOL       NEAR PASCAL Load(void);
void       NEAR PASCAL Free(void);
INSTINFO * NEAR PASCAL Open(ICOPEN FAR *icinfo);
DWORD      NEAR PASCAL Close(INSTINFO * pinst);
DWORD	   NEAR PASCAL GetState(INSTINFO * pinst, LPVOID pv, DWORD dwSize);
DWORD      NEAR PASCAL SetState(INSTINFO * pinst, LPVOID pv, DWORD dwSize);
DWORD	   NEAR PASCAL GetInfo(INSTINFO * pinst, ICINFO FAR *icinfo, DWORD dwSize);

BOOL	   NEAR PASCAL QueryAbout(INSTINFO * pinst);
DWORD      NEAR PASCAL About(INSTINFO * pinst, HWND hwnd);
BOOL	   NEAR PASCAL QueryConfigure(INSTINFO * pinst);
DWORD	   NEAR PASCAL Configure(INSTINFO * pinst, HWND hwnd);

LRESULT    NEAR PASCAL CompressBegin(INSTINFO * pinst, LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
LRESULT    NEAR PASCAL CompressQuery(INSTINFO * pinst, LPBITMAPINFOHEADER lpbiIn,LPBITMAPINFOHEADER lpbiOut);
LRESULT    NEAR PASCAL CompressGetFormat(INSTINFO * pinst, LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
LRESULT    NEAR PASCAL Compress(INSTINFO * pinst,ICCOMPRESS FAR *icinfo, DWORD dwSize);
LRESULT    NEAR PASCAL CompressGetSize(INSTINFO * pinst, LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
LRESULT    NEAR PASCAL CompressEnd(INSTINFO * lpinst);

LRESULT    NEAR PASCAL DecompressGetFormat(INSTINFO * pinst, LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
LRESULT    NEAR PASCAL DecompressGetPalette(INSTINFO * pinst, LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
LRESULT    NEAR PASCAL DecompressBegin(INSTINFO * pinst, DWORD dwFlags, LPBITMAPINFOHEADER lpbiSrc, LPVOID pSrc, int xSrc, int ySrc, int dxSrc, int dySrc, LPBITMAPINFOHEADER lpbiDst, LPVOID pDst, int xDst, int yDst, int dxDst, int dyDst);
LRESULT    NEAR PASCAL DecompressQuery(INSTINFO * pinst, DWORD dwFlags, LPBITMAPINFOHEADER lpbiSrc, LPVOID pSrc, int xSrc, int ySrc, int dxSrc, int dySrc, LPBITMAPINFOHEADER lpbiDst, LPVOID pDst, int xDst, int yDst, int dxDst, int dyDst);
LRESULT    NEAR PASCAL Decompress(INSTINFO * pinst, DWORD dwFlags, LPBITMAPINFOHEADER lpbiSrc, LPVOID pSrc, int xSrc, int ySrc, int dxSrc, int dySrc, LPBITMAPINFOHEADER lpbiDst, LPVOID pDst, int xDst, int yDst, int dxDst, int dyDst);
LRESULT    NEAR PASCAL DecompressEnd(INSTINFO * pinst);

BOOL       NEAR PASCAL DrawQuery(INSTINFO * pinst, LPBITMAPINFOHEADER lpbiInput);
LRESULT    NEAR PASCAL DrawBegin(INSTINFO * pinst,ICDRAWBEGIN FAR *icinfo, DWORD dwSize);
LRESULT    NEAR PASCAL Draw(INSTINFO * pinst,ICDRAW FAR *icinfo, DWORD dwSize);
LRESULT    NEAR PASCAL DrawEnd(INSTINFO * pinst);

/********************************************************************
 ********************************************************************/

#define ID_SCROLL   101
#define ID_TEXT     102
#define ID_SCROLL2  103
#define ID_TEXT2    104 

/********************************************************************
 ********************************************************************/

#ifdef DEBUG
    extern void FAR CDECL dprintf(LPSTR, ...);
    #define DPF dprintf
#else
    #define DPF / ## /
#endif
