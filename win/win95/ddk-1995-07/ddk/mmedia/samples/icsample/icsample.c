/****************************************************************************
 *
 *   icsample.c
 * 
 *   ICSAMPLE is a sample installable compressor for Video for Windows.
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

#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <compddk.h>

#include <string.h> // for wcscpy()
#include "icsample.h"

/*****************************************************************************
 *
 * Sample video compressor. This code demonstrates how to implement an
 * installable compressor.
 *
 * The alogorithm we use is very simple:
 * (1)  Keep one out of every 'n' pixels where 'n' is configurable.
 *      Compression is done on a scan-line basis.
 *
 * (2)  For true-color images (16 or 24 bits) give the option of dropping
 *      some of the lower bits.
 *
 ****************************************************************************/

typedef BYTE _huge  *HPBYTE ;
typedef WORD _huge  *HPWORD ;
typedef DWORD _huge *HPDWORD ;

#define MODNAME         "ICSAMPLE"

#ifdef _WIN32
WCHAR   szDescription[] = L"Microsoft 32-bit Sample Compressor";
WCHAR   szName[]        = L"MS-Samp";
#else
char    szDescription[] = "Microsoft Sample Compressor";
char    szName[]        = "MS-Samp";
#endif
#define FOURCC_SAMP     mmioFOURCC('S','A','M','P')
#define TWOCC_SAMP      aviTWOCC('d', 'c')
#define BI_SAMP         mmioFOURCC('S','m','p','1')
#define VERSION_SAMP    0x00040000      // 4.00

extern HANDLE ghModule ;

/*****************************************************************************
 *
 * DefaultState holds the compression handler.
 *
 ****************************************************************************/

ICSTATE DefaultState = {FOURCC_SAMP,2,0};

/*****************************************************************************
 *
 * DefaultDecompressFmt are the default options that will be used if the user
 * compresses an image without configuring us at all first. In the case of
 * the sample compressor, it is the pixel keep ratio.
 *
 ****************************************************************************/
BOOL FAR PASCAL _loadds ConfigureDlgProc(HWND hdlg, UINT msg, WPARAM wParam, LONG lParam);

/*****************************************************************************
 *
 * Various macros that are useful when dealing with bitmaps.
 *
 ****************************************************************************/

#define ALIGNULONG(i)     (((i)+3)&(0xFFFFFFFC))          /* ULONG aligned ! */
#define WIDTHBYTES(i)     ((unsigned)(((i)+31)&(~31))/8)  /* ULONG aligned ! */
#define DIBWIDTHBYTES(bi) (int)WIDTHBYTES((int)(bi).biWidth * (int)(bi).biBitCount)

/*****************************************************************************
 *
 * Function for buffering bits
 *
 ****************************************************************************/

typedef struct tagBitBuffer
{
    HPWORD          lpwBuffer ;
    DWORD           dwStored ;
    WORD            wMask ;
    WORD            wFile ;
} BITBUFFER, FAR *LPBITBUFFER ;

#define MAX_MASK    (0x8000L)

void InitBitBuffer( LPBITBUFFER lpbb,void _huge *lpwBuffer )
{
    /*
    ** Same code is valid for both input and output
    */
    lpbb->lpwBuffer = lpwBuffer ;
    lpbb->dwStored = 0 ;
    lpbb->wMask = MAX_MASK ;
    lpbb->wFile = 0 ;
}

void OutputBits( LPBITBUFFER lpbb, WORD wBits, WORD wCount )
{
    WORD    wMask ;

    wMask = 1 << (wCount-1) ;
    while (wMask)
    {
        if (wMask&wBits)
            lpbb->wFile |= lpbb->wMask ;
        lpbb->wMask >>= 1 ;
        if ( !lpbb->wMask )
        {
            *lpbb->lpwBuffer++ = lpbb->wFile ;
            lpbb->dwStored += sizeof(*lpbb->lpwBuffer) ;
            lpbb->wFile = 0 ;
            lpbb->wMask = MAX_MASK ;
        }
        wMask >>= 1 ;
    }
}

void OutputBitsFlush( LPBITBUFFER lpbb )
{
    if (lpbb->wMask != MAX_MASK)
    {
        *lpbb->lpwBuffer++ = lpbb->wFile ;
        lpbb->dwStored += sizeof(*lpbb->lpwBuffer) ;
    }
}

WORD InputBits( LPBITBUFFER lpbb, WORD wCount )
{
    WORD    wMask ;
    WORD    wRet ;

    wMask = 1 << (wCount-1) ;
    wRet = 0 ;
    while (wMask)
    {
        if (lpbb->wMask == MAX_MASK)
        {
            lpbb->wFile = *lpbb->lpwBuffer++ ;
            lpbb->dwStored += sizeof(*lpbb->lpwBuffer) ;
        }
        if (lpbb->wFile&lpbb->wMask)
            wRet |= wMask ;
        wMask >>= 1 ;
        lpbb->wMask >>= 1 ;
        if (!lpbb->wMask)
            lpbb->wMask = MAX_MASK ;
    }

    return wRet ;
}

/*****************************************************************************
 *
 * getDecompressFmtPtr()
 *
 * This routine returns a pointer to the location of the decompression
 * format relative to the specified bitmapinfo header pointer.
 *
 ****************************************************************************/
LPDECOMP_FMT getDecompressFmtPtr(
    LPBITMAPINFOHEADER lpbiSrc)
{
    LPDECOMP_FMT  lpDecompressFmt;

    lpbiSrc++;
    lpDecompressFmt = (LPDECOMP_FMT)lpbiSrc;
    lpDecompressFmt->dwSize = sizeof(DECOMPRESS_INFO);
    lpbiSrc--;
    return lpDecompressFmt;
}



/*****************************************************************************
 *
 * MyCompress8
 *
 * This routine handles the actual compression of the bitmap. Note:
 *
 * 1) The use of _huge pointers as the bitmaps are likely to exceed 64k.
 *
 * 2) We must set the biCompression field of the output header.
 *
 * 3) We must set the biSizeImage field of the output header.
 *
 * 4) Uncompressed bitmap scan lines are padded out to the next DWORD
 *    boundry.
 *
 ****************************************************************************/
DWORD NEAR PASCAL MyCompress8(
    INSTINFO * pinst,
    LPBITMAPINFOHEADER lpbiInput, HPBYTE hpInput,
    LPBITMAPINFOHEADER lpbiOutput,HPBYTE hpOutput)
{
    WORD    wRealLineSize ;
    WORD    wLinePad ;
    WORD    wWidth = (WORD)lpbiInput->biWidth ;
    WORD    wHeight = (WORD)lpbiInput->biHeight ;
    WORD    x,y ;
    WORD    wPixRatio = pinst->CurrentState.dInfo.wPixelKeepRatio;
    LPDECOMP_FMT  lpDecompressFmt;

    DPF("Compress8()");

    //
    // Initialize the pointer to the decompression format which
    // is located after the BITMAPINFOHEADER.
    //
    lpDecompressFmt = getDecompressFmtPtr(lpbiOutput);

    //
    // Set the decompression format in the output header.  Colors to 
    // drop is not used in the decompression of 8 bit, so set it to 0.
    //
    lpDecompressFmt->dInfo.wPixelKeepRatio = wPixRatio;
    lpDecompressFmt->dInfo.wColorBitsToDrop = 0;
    
    //
    // Set the bit count, the compression, and the size of the compressed
    // data.  
    //
    lpbiOutput->biBitCount    = 8;
    lpbiOutput->biCompression = BI_SAMP;
    lpbiOutput->biSizeImage   = sizeof(WORD)+((wWidth+wPixRatio-1)/wPixRatio)*(DWORD)wHeight;
    lpbiOutput->biSize        = sizeof(BITMAPINFOHEADER) + sizeof(DECOMP_FORMAT);

    wRealLineSize = (WORD)ALIGNULONG((DWORD)wWidth) ;
    wLinePad      = wRealLineSize-wWidth ;

    for (y=0; y<wHeight; y++ )
    {
    //
    // Report status every STATUS_EVERY lines.  This is done by
    // invoking the Status function that is set in response to the 
    // ICM_SET_STATUS_PROC message.
    //
        if (pinst->Status && ((y % STATUS_EVERY) == 0)) {
	    if (pinst->Status(pinst->lParam,
			      ICSTATUS_STATUS,
			      (y * 100) / wHeight) != 0)
		return (DWORD) ICERR_ABORT;
	}

        for (x=0; x<wWidth; x += wPixRatio)
        {
            *hpOutput++ = *hpInput ;
            hpInput += min(wPixRatio,(WORD)lpbiInput->biWidth-x) ;
        }

        hpInput += wLinePad ;
    }

    return 0;
}

/*****************************************************************************
 *
 * 16/24 bit compression. Same thing; we just deal with a 2 or 3 bytes per pixel
 * instead of a byte.
 *
 * Note that 8-bit pixel values are indices into a palette; 16-bit and
 * about are actual color values. In particular, 16-bit values are
 * stored as:
 *
 * +-+-----+-----+-----+
 * | |  R  |  G  |  B  |
 * +-+-----+-----+-----+
 *  1 1   1
 *  5 4   0 9   5 4   0
 *
 * Bit 15 is unused; each of red, green, and blue get 5 bits of color data
 * (0-31).
 *
 * For 24 bit images, each pixel is three bytes long. The bytes hold
 * the blue, green, and red values respectively (NOTE THE ORDER IS
 * OPPOSITE OF RGB!)
 *
 ****************************************************************************/
DWORD NEAR PASCAL MyCompress16(
    INSTINFO * pinst,
    LPBITMAPINFOHEADER lpbiInput, HPWORD hpInput,
    LPBITMAPINFOHEADER lpbiOutput,HPWORD hpOutput)
{
    WORD        wRealLineSize ;
    WORD        wLinePad ;
    WORD        wWidth = (WORD)lpbiInput->biWidth ;
    WORD        wHeight = (WORD)lpbiInput->biHeight ;
    WORD        x,y ;
    WORD        wPixRatio=pinst->CurrentState.dInfo.wPixelKeepRatio;
    WORD        wDropBits=pinst->CurrentState.dInfo.wColorBitsToDrop;
    WORD        wKeepBits;
    WORD        r,g,b ;
    LPDECOMP_FMT lpDecompressFmt;
    BITBUFFER   bb ;

    DPF("Compress16()") ;
    
    //
    // Initialize the pointer to the decompression format which
    // is located after the BITMAPINFOHEADER.
    //
    lpDecompressFmt = getDecompressFmtPtr(lpbiOutput);

    //
    // Set the decompression format in the output header.  
    //
    lpDecompressFmt->dInfo.wPixelKeepRatio = wPixRatio;
    lpDecompressFmt->dInfo.wColorBitsToDrop = wDropBits;
    wKeepBits = 5 - wDropBits;

    //
    // Set the bit count, the compression, and the size of the compressed
    // data.  
    //
    lpbiOutput->biBitCount    = 16;
    lpbiOutput->biCompression = BI_SAMP;
    lpbiOutput->biSizeImage   = sizeof(WORD)+2*((wWidth+wPixRatio-1)/wPixRatio)*(DWORD)wHeight;
    lpbiOutput->biSize        = sizeof(BITMAPINFOHEADER) + sizeof(DECOMP_FORMAT);

    wRealLineSize = (WORD)ALIGNULONG((DWORD)wWidth*2) ;
    wLinePad      = wRealLineSize - (wWidth*2) ;

    InitBitBuffer( &bb,hpOutput ) ;
    
    for (y=0; y<wHeight; y++ )
    {
    //
    // Report status every STATUS_EVERY lines.  This is done by
    // invoking the Status function that is set in response to the 
    // ICM_SET_STATUS_PROC message.
    //
	if (pinst->Status && ((y % STATUS_EVERY) == 0)) {
	    if (pinst->Status(pinst->lParam,
			      ICSTATUS_STATUS,
			      (y * 100) / wHeight) != 0)
		return (DWORD) ICERR_ABORT;
	}

        for (x=0; x<wWidth; x += wPixRatio)
        {
            r = ((*hpInput>>10)&0x1F)>>wDropBits ;
            g = ((*hpInput>>5)&0x1F)>>wDropBits ;
            b = (*hpInput&0x1F)>>wDropBits ;
            hpInput += min(wPixRatio,(WORD)lpbiInput->biWidth-x) ;

            OutputBits( &bb, r, wKeepBits ) ;
            OutputBits( &bb, g, wKeepBits ) ;
            OutputBits( &bb, b, wKeepBits ) ; 
        }

        hpInput += wLinePad ;
    }

    OutputBitsFlush(&bb) ;

    lpbiOutput->biSizeImage = bb.dwStored ;

    return 0;
}   
    
DWORD NEAR PASCAL MyCompress24(
    INSTINFO * pinst,
    LPBITMAPINFOHEADER lpbiInput, HPBYTE hpInput,
    LPBITMAPINFOHEADER lpbiOutput,HPBYTE hpOutput)
{
    WORD        wRealLineSize ;
    WORD        wLinePad ;
    WORD        wWidth = (WORD)lpbiInput->biWidth ;
    WORD        wHeight = (WORD)lpbiInput->biHeight ;
    WORD        x,y ;
    WORD        wPixRatio=pinst->CurrentState.dInfo.wPixelKeepRatio;
    WORD        wDropBits=pinst->CurrentState.dInfo.wColorBitsToDrop;
    WORD        wKeepBits;
    WORD        r,g,b ;
    LPDECOMP_FMT  lpDecompressFmt;
    BITBUFFER   bb ;

    DPF("Compress24()") ;
    
    //
    // Initialize the pointer to the decompression format which
    // is located after the BITMAPINFOHEADER.
    //
    lpDecompressFmt = getDecompressFmtPtr(lpbiOutput);

    //
    // Set the decompression format in the output header.  
    //
    lpDecompressFmt->dInfo.wPixelKeepRatio = wPixRatio;
    lpDecompressFmt->dInfo.wColorBitsToDrop = wDropBits;
    wKeepBits = 8 - wDropBits;

    //
    // Set the bit count and the compression.
    //
    lpbiOutput->biBitCount    = 24 ;
    lpbiOutput->biCompression = BI_SAMP;
    lpbiOutput->biSize        = sizeof(BITMAPINFOHEADER) + sizeof(DECOMP_FORMAT);

    wRealLineSize = (WORD)ALIGNULONG((DWORD)wWidth*3) ;
    wLinePad      = wRealLineSize - (wWidth * 3);

    InitBitBuffer( &bb,hpOutput ) ;
    
    for (y=0; y<wHeight; y++ )
    {
    //
    // Report status every STATUS_EVERY lines.  This is done by
    // invoking the Status function that is set in response to the 
    // ICM_SET_STATUS_PROC message.
    //
	if (pinst->Status && ((y % STATUS_EVERY) == 0)) {
	    if (pinst->Status(pinst->lParam,
			      ICSTATUS_STATUS,
			      (y * 100) / wHeight) != 0)
		return (DWORD) ICERR_ABORT;
	}

        for (x=0; x<wWidth; x += wPixRatio)
        {
            b = (hpInput[0])>>wDropBits ;
            g = (hpInput[1])>>wDropBits ;
            r = (hpInput[2])>>wDropBits ;
            hpInput += 3 * min(wPixRatio,(WORD)lpbiInput->biWidth-x) ;

            OutputBits( &bb, r, wKeepBits ) ;
            OutputBits( &bb, g, wKeepBits ) ;
            OutputBits( &bb, b, wKeepBits ) ; 
        }

        ((HPBYTE)hpInput) += wLinePad ;
    }
    OutputBitsFlush(&bb) ;

    lpbiOutput->biSizeImage = bb.dwStored ;

    return 0;
}

/**************************************************************************
compute a pointer into a DIB handling correctly "upside" down DIBs
***************************************************************************/

static LPVOID DibXY(LPBITMAPINFOHEADER lpbi, LPVOID lpBits, LONG x, LONG y, int FAR *pWidthBytes)
{
    int WidthBytes;

    if (x > 0)
        ((BYTE FAR *)lpBits) += ((int)x * (int)lpbi->biBitCount) >> 3;

    WidthBytes = (((((int)lpbi->biWidth * (int)lpbi->biBitCount) >> 3) + 3)&~3);

    if (lpbi->biHeight < 0)
    {
        ((BYTE _huge *)lpBits) += (long)(lpbi->biSizeImage - WidthBytes);
        WidthBytes = -WidthBytes;
    }

    if (y > 0)
        ((BYTE _huge *)lpBits) += ((long)y * WidthBytes);

    if (pWidthBytes)
        *pWidthBytes = WidthBytes;

    return lpBits;
}

#pragma optimize("", off)           //huge pointer math geting hosed
    
/*****************************************************************************
 *
 * MyDecompress
 *
 * This function is the inverse of MyCompress and follows the same caveats.
 *
 * We shouldn't depend on the state to accurately tell us how to
 * decompress; this info needs to come totally from the compressed data!
 *
 ****************************************************************************/
void NEAR PASCAL MyDecompress8(
    INSTINFO * pinst,
    LPBITMAPINFOHEADER lpbiInput,  HPBYTE hpInput,
    LPBITMAPINFOHEADER lpbiOutput, HPBYTE hpOutput, int x, int y)
{
    int         xrun ;
    int         wRealLineSize ;
    long        wLinePad ;
    LPDECOMP_FMT lpDecompressFmt;
    int         wWidth  = (int)lpbiInput->biWidth;
    int         wHeight = (int)lpbiInput->biHeight;

    hpOutput = DibXY(lpbiOutput, hpOutput, x, y, &wRealLineSize);
    wLinePad = wRealLineSize - wWidth;

    //
    // Initialize the pointer to the decompression format which
    // follows the bitmapinfo header.
    //
    lpDecompressFmt = getDecompressFmtPtr(lpbiInput);
    for (y=0; y < wHeight; y++)
    {
        for (x=0; x<wWidth; )
        {
            xrun = min(wWidth-x,(int)lpDecompressFmt->dInfo.wPixelKeepRatio) ;
            x += xrun ;
            while (xrun--)
                *hpOutput++ = *hpInput ;

            hpInput++ ;
        }

        hpOutput += wLinePad ;
    }
}

void NEAR PASCAL MyDecompress16(
    INSTINFO * pinst,
    LPBITMAPINFOHEADER lpbiInput,  HPWORD hpInput,
    LPBITMAPINFOHEADER lpbiOutput, HPWORD hpOutput, int x, int y)
{
    int         xrun ;
    int         wRealLineSize ;
    long        wLinePad ;
    LPDECOMP_FMT lpDecompressFmt;
    int         wWidth  = (int)lpbiInput->biWidth ;
    int         wHeight = (int)lpbiInput->biHeight ;
    UINT        wDropBits ;
    WORD        wKeepBits ;
    WORD        r,g,b,color ;
    BITBUFFER   bb ;
 
    hpOutput = DibXY(lpbiOutput, hpOutput, x, y, &wRealLineSize);
    wLinePad = wRealLineSize - (wWidth*2) ;

    //
    // Initialize the pointer to the decompression format which
    // follows the bitmapinfo header.
    //
    lpDecompressFmt = getDecompressFmtPtr(lpbiInput);

    wDropBits = lpDecompressFmt->dInfo.wColorBitsToDrop ;
    wKeepBits = 5 - wDropBits ;

    InitBitBuffer( &bb, hpInput ) ;

    for (y=0; y < wHeight; y++)
    {
        for (x=0; x<wWidth; )
        {
            r = (WORD)InputBits( &bb, wKeepBits ) << wDropBits ;
            g = (WORD)InputBits( &bb, wKeepBits ) << wDropBits ;
            b = (WORD)InputBits( &bb, wKeepBits ) << wDropBits ;

            color = (r << 10) | (g << 5) | b ;
            
            xrun = min(wWidth-x,(int)lpDecompressFmt->dInfo.wPixelKeepRatio) ;
            x += xrun ;
            while (xrun--)
                *hpOutput++ = color ;
        }

        ((BYTE _huge *)hpOutput) += wLinePad ;
    }
}

void NEAR PASCAL MyDecompress24(
    INSTINFO * pinst,
    LPBITMAPINFOHEADER lpbiInput,  HPBYTE hpInput,
    LPBITMAPINFOHEADER lpbiOutput, HPBYTE hpOutput, int x, int y)
{
    int         xrun ;
    int         wRealLineSize ;
    long        wLinePad ;
    LPDECOMP_FMT lpDecompressFmt;
    int         wWidth  = (int)lpbiInput->biWidth ;
    int         wHeight = (int)lpbiInput->biHeight ;
    WORD        wDropBits ;  
    WORD        wKeepBits ;
    WORD        r,g,b ;
    BITBUFFER   bb ;
 
    hpOutput = DibXY(lpbiOutput, hpOutput, x, y, &wRealLineSize);
    wLinePad = wRealLineSize - (wWidth*3);

    //
    // Initialize the pointer to the decompression format which
    // follows the bitmapinfo header.
    //
    lpDecompressFmt = getDecompressFmtPtr(lpbiInput);

    wDropBits = lpDecompressFmt->dInfo.wColorBitsToDrop ;
    wKeepBits = 8 - wDropBits ;

    InitBitBuffer( &bb, hpInput ) ;

    for (y=0; y < wHeight; y++)
    {
        for (x=0; x<wWidth; )
        {
            r = (WORD)InputBits( &bb, wKeepBits ) << wDropBits ;
            g = (WORD)InputBits( &bb, wKeepBits ) << wDropBits ;
            b = (WORD)InputBits( &bb, wKeepBits ) << wDropBits ;

            xrun = min(wWidth-x,(int)lpDecompressFmt->dInfo.wPixelKeepRatio) ;
            x += xrun ;
            while (xrun--)
            {
                *hpOutput++ = (BYTE)b ;
                *hpOutput++ = (BYTE)g ;
                *hpOutput++ = (BYTE)r ;
            }
        }

        hpOutput += wLinePad ;
    }
}

#pragma optimize("", on)           //huge pointer math geting hosed

/*****************************************************************************
 *
 * Load() is called from the DRV_LOAD message.
 *
 * Tasks such as allocating global memory that is non-instance specific
 * or initializing coprocessor hardware may be performed here.
 *
 * Our simple case needs none of this.
 *
 ****************************************************************************/
BOOL NEAR PASCAL Load(void)
{
    DPF("Load()");
    return TRUE;
}

/*****************************************************************************
 *
 * Free() is called from the DRV_FREE message.
 *
 * It should totally reverse the effects of Load() in preparation for
 * the DRV being removed from memory.
 *
 ****************************************************************************/
void NEAR PASCAL Free()
{
    DPF("Free()");
}

/*****************************************************************************
 *
 * Open() is called from the ICM_OPEN message
 *
 * This message will be sent for a particular compress/decompress session.
 * Our code must verify that we are indeed being called as a video
 * compressor and create/initialize a state structure. The ICM will
 * give us back the pointer to that structure on every message dealing
 * with this session.
 *
 ****************************************************************************/
INSTINFO * NEAR PASCAL Open(ICOPEN FAR * icinfo)
{
    INSTINFO *  pinst;

    DPF("Open('%4.4ls', '%4.4ls')", (LPSTR)&icinfo->fccType, (LPSTR)&icinfo->fccHandler);

    //
    // refuse to open if we are not being opened as a Video compressor
    //
    if (icinfo->fccType != ICTYPE_VIDEO)
        return NULL;

    pinst = (INSTINFO *)LocalAlloc(LPTR, sizeof(INSTINFO));

    if (!pinst)
    {
        icinfo->dwError = ICERR_MEMORY;
        return NULL;
    }

    //
    // init structure
    //
    pinst->fccType = ICTYPE_VIDEO;
    pinst->dwFlags = icinfo->dwFlags;
    pinst->fCompress   = FALSE;
    pinst->fDecompress = FALSE;
    pinst->fDrawBegun  = FALSE;

    //
    // set the default state.
    //
    SetState(pinst, NULL, 0);

    //
    // return success.
    //
    icinfo->dwError = ICERR_OK;

    return pinst;
}

/*****************************************************************************
 *
 * Close() is called on the ICM_CLOSE message.
 *
 * This message is the complement to ICM_OPEN and marks the end
 * of a compress/decompress session. We kill any in-progress operations
 * (although this shouldn't be needed) and free our instance structure.
 *
 ****************************************************************************/
DWORD NEAR PASCAL Close(INSTINFO * pinst)
{
    DPF("Close()");

    if (pinst->fCompress)
        CompressEnd(pinst);

    if (pinst->fDecompress)
        DecompressEnd(pinst);

    if (pinst->fDrawBegun)
        DrawEnd(pinst);
   
    LocalFree((HLOCAL)pinst);
    
    return 1;
}

/*****************************************************************************
 *
 * QueryAbout() and About() handle the ICM_ABOUT message.
 *
 * QueryAbout() returns TRUE to indicate we support an about box.
 * About() displays the box.
 *
 ****************************************************************************/
BOOL NEAR PASCAL QueryAbout(INSTINFO * pinst)
{
    DPF("QueryAbout()");

    return TRUE;
}

DWORD NEAR PASCAL About(INSTINFO * pinst, HWND hwnd)
{
    #ifdef _WIN32
    // max byte size of dst Windows DBCS string same as UNICODE src
    char szAnsiDesc[sizeof szDescription];
    char szAnsiName[sizeof szName];
    #endif
    
    DPF("About()");
    #ifdef _WIN32
    WideCharToMultiByte(
        CP_ACP,	// code page 
        0,	// performance and mapping flags 
        szDescription,	// address of wide-character string 
        -1,	// -1 means null-term string number of characters in string
        szAnsiDesc,	// address of buffer for new string 
        sizeof szAnsiDesc,	// size of buffer 
        NULL,	// address of default for unmappable characters  
        NULL); 	// address of flag set when default char. used 
    WideCharToMultiByte(
        CP_ACP,	// code page 
        0,	// performance and mapping flags 
        szName,	// address of wide-character string 
        -1,	// -1 means null-term string number of characters in string
        szAnsiName,	// address of buffer for new string 
        sizeof szAnsiDesc,	// size of buffer 
        NULL,	// address of default for unmappable characters  
        NULL); 	// address of flag set when default char. used 
        	
    MessageBox(hwnd,szAnsiDesc,szAnsiName,MB_OK|MB_ICONINFORMATION);
    #else
    MessageBox(hwnd,szDescription,szName,MB_OK|MB_ICONINFORMATION);
    #endif
    return ICERR_OK;
}

/*****************************************************************************
 *
 * QueryConfigure() and Configure() implement the ICM_CONFIGURE message.
 *
 * These functions put up a dialog that allows the user, if he so
 * chooses, to modify the configuration portion of our state info.
 * 
 ****************************************************************************/
BOOL NEAR PASCAL QueryConfigure(INSTINFO * pinst)
{
    DPF("QueryConfigure()");
    return TRUE;
}

DWORD NEAR PASCAL Configure(INSTINFO * pinst, HWND hwnd)
{
    DPF("Configure()");
    //@@BEGIN_MSINTERNAL
    //It's generally a bad idea to pass near pointers. No problem in WIN32,
    //but perhaps the win16 sample should be fixed -gtb
    //@@END_MSINTERNAL
    return DialogBoxParam(ghModule,"Configure",hwnd,ConfigureDlgProc, (UINT)pinst);
}

/*****************************************************************************
 *
 * GetState() implements the ICM_GETSTATE message.
 * 
 * We copy our configuration information and return how many bytes it took.
 *
 ****************************************************************************/
DWORD NEAR PASCAL GetState(INSTINFO * pinst, LPVOID pv, DWORD dwSize)
{
    DPF("GetState(%08lX, %ld)", pv, dwSize);

    if (pv == NULL || dwSize == 0)
        return sizeof(ICSTATE);

    if (dwSize < sizeof(ICSTATE))
        return 0;

    *((ICSTATE FAR *)pv) = pinst->CurrentState;

    // return number of bytes copied
    return sizeof(ICSTATE);
}

/*****************************************************************************
 *
 * SetState() implements the ICM_SETSTATE message.
 *
 * The ICM is giving us configuration information saved by GetState()
 * earlier.
 *
 ****************************************************************************/
DWORD NEAR PASCAL SetState(INSTINFO * pinst, LPVOID pv, DWORD dwSize)
{
    DPF("SetState(%08lX, %ld)", pv, dwSize);

    //
    //  make sure we created this state information.
    //
    if (pv && ((ICSTATE FAR *)pv)->fccHandler != FOURCC_SAMP)
        return 0;

    if (pv == NULL)
        pinst->CurrentState = DefaultState;
    else if (dwSize >= sizeof(ICSTATE))
        pinst->CurrentState = *((ICSTATE FAR *)pv);
    else
        return 0;

    // return number of bytes copied
    return sizeof(ICSTATE);
}

/*****************************************************************************
 *
 * GetInfo() implements the ICM_GETINFO message
 *
 * We just fill in the structure to tell the ICM what we can do. The flags
 * (none of which this sample supports) mean the following :
 *
 * VIDCF_QUALITY - we support the quality variable. This means we look at
 *                 dwQuality in the ICINFO structure when compressing and
 *                 make a concious decision to trade quality for space.
 *                 (higher values of dwQuality mean quality is more
 *                 important). dwQuality is set by the ICM.
 *
 * VIDCF_TEMPORAL - We do interframe compression. In this algorithm, not
 *                  every frame is a "key frame"; some frames depend on
 *                  other frames to be generated. An example of this might
 *                  be to store frame buffer differences until the
 *                  differences are big enough to no longer make this
 *                  worthwhile, then storing another complete frame and
 *                  starting over. In this case, the complete frames that
 *                  are stored are key frames and should be flagged as
 *                  such.
 *
 * VIDCF_DRAW -     We will draw the decompressed image on our own. This is
 *                  useful if the decompression is assisted by the video
 *                  hardware.
 *
 ****************************************************************************/
DWORD NEAR PASCAL GetInfo(INSTINFO * pinst, ICINFO FAR *icinfo, DWORD dwSize)
{
    DPF("GetInfo()");

    if (icinfo == NULL)
        return sizeof(ICINFO);

    if (dwSize < sizeof(ICINFO))
        return 0;

    icinfo->dwSize            = sizeof(ICINFO);
    icinfo->fccType           = ICTYPE_VIDEO;
    icinfo->fccHandler        = FOURCC_SAMP;
    icinfo->dwFlags           = 0;

                //              VIDCF_QUALITY    // supports quality
                //              VIDCF_TEMPORAL   // supports inter-frame
                //              VIDCF_DRAW       // supports drawing

    icinfo->dwVersion         = VERSION_SAMP;
    icinfo->dwVersionICM      = ICVERSION;
    wcscpy(icinfo->szDescription, szDescription);
    wcscpy(icinfo->szName, szName);

    return sizeof(ICINFO);
}

/*****************************************************************************
 *
 * CompressQuery() handles the ICM_COMPRESSQUERY message
 *
 * This message basically asks, "Can you compress this into this?"
 *
 * We look at the input and output bitmap info headers and determine
 * if we can.
 *
 ****************************************************************************/
LRESULT NEAR PASCAL CompressQuery(INSTINFO * pinst, LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
    DPF("CompressQuery()");

    //
    // determine if the input DIB data is in a format we like.
    //
    if (lpbiIn == NULL ||
        (lpbiIn->biBitCount != 8 && lpbiIn->biBitCount != 16 && lpbiIn->biBitCount != 24) ||
        lpbiIn->biCompression != BI_RGB)
    {
        return ICERR_BADFORMAT;
    }

    //
    //  are we being asked to query just the input format?
    //
    if (lpbiOut == NULL)
        return ICERR_OK;

    //
    // make sure we can handle the format to compress to also.
    //
    if (lpbiOut->biCompression != BI_SAMP ||    // must be 'Smp1'
        lpbiOut->biBitCount != lpbiIn->biBitCount ||
        lpbiOut->biSize != (sizeof(BITMAPINFOHEADER) + sizeof(DECOMP_FORMAT)) ||
        lpbiOut->biWidth  != lpbiIn->biWidth || // must be 1:1 (no stretch)
        lpbiOut->biHeight != lpbiIn->biHeight)
    {
        return ICERR_BADFORMAT;
    }

    return ICERR_OK;
}

/*****************************************************************************
 *
 * CompressGetFormat() implements ICM_GETFORMAT
 *
 * This message asks, "If I gave you this bitmap, how much memory would it
 * be compressed?"
 *
 * If the output bitmap info header is NULL, we just return how big the
 * header would be (header + format information (if any) + palette, actually).  
 * The format is any information that the decompressor might need to 
 * decompress the data.
 *
 * Otherwise, we fill in the header, most importantly the biSizeImage.
 * This field must contain an upper bound on the size of the compressed
 * frame. A value that is too high here will result in inefficient
 * memory allocation at compression time, but will not be reflected
 * to the stored bitmap - the compression algorithm may chop biSizeImage
 * down to the actual amount with no ill effects.
 * 
 ****************************************************************************/
LRESULT NEAR PASCAL CompressGetFormat(INSTINFO * pinst, LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
    DWORD dw;
    LPDECOMP_FMT  lpDecompressFmt;

    DPF("CompressGetFormat()");

    if (dw = CompressQuery(pinst, lpbiIn, NULL))
        return dw;

    //
    // if lpbiOut == NULL then, return the size required to hold a output
    // format.  Remember, if you have decompress format information
    // in the header, then make room for it.    
    //
    if (lpbiOut == NULL)
        return (int)sizeof(BITMAPINFOHEADER) + (int)sizeof(DECOMP_FORMAT) +
               (int)lpbiIn->biClrUsed * sizeof(RGBQUAD);

    // 
    // First copy the bitmapinfo header.
    //
    hmemcpy(lpbiOut, lpbiIn, (int)lpbiIn->biSize);
    
    //
    // Now copy the decompression formation information.
    //
    lpDecompressFmt = getDecompressFmtPtr(lpbiOut);
    lpDecompressFmt->dInfo.wPixelKeepRatio  = pinst->CurrentState.dInfo.wPixelKeepRatio;
    lpDecompressFmt->dInfo.wColorBitsToDrop = pinst->CurrentState.dInfo.wColorBitsToDrop;
    lpDecompressFmt->dwSize                 = sizeof(DECOMPRESS_INFO);

    //
    // Copy the colors. 
    //
    lpbiOut->biSize = sizeof(BITMAPINFOHEADER) + sizeof(DECOMP_FORMAT);

    hmemcpy(
        (LPBYTE)lpbiOut + (int)lpbiOut->biSize,
        (LPBYTE)lpbiIn + (int)lpbiIn->biSize,
        (int)lpbiIn->biClrUsed * sizeof(RGBQUAD));

    lpbiOut->biBitCount    = lpbiIn->biBitCount;
    lpbiOut->biCompression = BI_SAMP;
    lpbiOut->biSizeImage   = CompressGetSize(pinst, lpbiIn, lpbiOut); 

    return ICERR_OK;
}

/*****************************************************************************
 *
 * CompressBegin() implements ICM_COMPRESSBEGIN
 *
 * We're about to start compressing, initialize coprocessor, etc.
 * 
 ****************************************************************************/
LRESULT NEAR PASCAL CompressBegin(INSTINFO * pinst, LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
    DWORD dw;

    DPF("CompressBegin()");

    if (dw = CompressQuery(pinst, lpbiIn, lpbiOut))
        return dw;

    //
    // initialize for compression, for real....
    //

    pinst->fCompress = TRUE;

    return ICERR_OK;
}

/*****************************************************************************
 *
 * CompressGetSize() implements ICM_COMPRESS_GET_SIZE
 *
 * This function returns how much (upper bound) memory a compressed frame
 * will take.
 *
 ****************************************************************************/
LRESULT NEAR PASCAL CompressGetSize(INSTINFO * pinst, LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
    int dx,dy;
    WORD    wPixelSize ;

    DPF("CompressGetSize()");

    dx = (int)lpbiIn->biWidth;
    dy = (int)lpbiIn->biHeight;

    wPixelSize = (lpbiOut->biBitCount+7)/8 ;

    return wPixelSize*(DWORD)dx*dy ;
}

/*****************************************************************************
 *
 * Compress() implements ICM_COMPRESS
 *
 * Everything is set up; call the actual compression routine.
 *
 * Note:
 *
 * 1) We set the ckid in icinfo to a two-character code indicating how we
 *    compressed. This code will be returned to us at decompress time to
 *    allow us to pick a decompression algorithm to match. This is different
 *    from icinfo->fccHandler, which tells which driver to use!
 *
 * 2) We set the key-frame flag on every frame since we do no
 *    temporal (inter-frame) compression.
 *
 ****************************************************************************/
LRESULT NEAR PASCAL Compress(INSTINFO * pinst, ICCOMPRESS FAR *icinfo, DWORD dwSize)
{
    DWORD dw;
    BOOL  fBegin;

    DPF("Compress()");

    if (dw = CompressQuery(pinst, icinfo->lpbiInput, icinfo->lpbiOutput))
        return dw;

    //
    // check for being called without a BEGIN message
    //
    if (!(fBegin = pinst->fCompress))
    {
        if (dw = CompressBegin(pinst, icinfo->lpbiInput, icinfo->lpbiOutput))
            return dw;
    }

    if (pinst->Status)
    //
    // Start the intervals to the Status procedure.
    //
	pinst->Status(pinst->lParam, ICSTATUS_START, 0);
    
    /* do the compression */
    switch(icinfo->lpbiInput->biBitCount)
    {
        case 8 :
            dw = MyCompress8(pinst,icinfo->lpbiInput, icinfo->lpInput, 
                icinfo->lpbiOutput, icinfo->lpOutput);
            break;

        case 16 :
            dw = MyCompress16(pinst,icinfo->lpbiInput, icinfo->lpInput, 
                icinfo->lpbiOutput, icinfo->lpOutput);
            break;

        case 24 :
            dw = MyCompress24(pinst,icinfo->lpbiInput, icinfo->lpInput, 
                icinfo->lpbiOutput, icinfo->lpOutput);
            break;
    }

    //
    // Stop the intervals to the Status procedure.
    //
    if (pinst->Status)
	pinst->Status(pinst->lParam, ICSTATUS_END, 0);
    
    //
    // return the chunk id
    //
    if (icinfo->lpckid)
        *icinfo->lpckid = TWOCC_SAMP;

    //
    // set the AVI index flags,
    //
    //    make it a keyframe
    //
    if (icinfo->lpdwFlags)
        *icinfo->lpdwFlags = AVIIF_KEYFRAME;

    //
    // do a CompressEnd() for the caller if needed
    //
    if (!fBegin)
    {
        CompressEnd(pinst);
    }

    return dw;
}

/*****************************************************************************
 *
 * CompressEnd() is called on ICM_COMPRESS_END
 *
 * This function is a chance to flush buffers, deinit hardware, etc.
 * after compressing a single frame.
 *
 ****************************************************************************/
LRESULT NEAR PASCAL CompressEnd(INSTINFO * pinst)
{
    DPF("CompressEnd()");

    if (!pinst->fCompress)
        return ICERR_ERROR;

    /* *** your code to clean up here *** */

    pinst->fCompress = FALSE;

    return ICERR_OK;
}

/*****************************************************************************
 ****************************************************************************/
LRESULT NEAR PASCAL DecompressQueryFmt(
    INSTINFO * pinst,
    LPBITMAPINFOHEADER lpbiSrc)
{
    //
    // determine if the input DIB data is in a format we like.
    //
    if (lpbiSrc == NULL ||
        !(lpbiSrc->biBitCount == 24 ||
          lpbiSrc->biBitCount == 16 ||
          lpbiSrc->biBitCount == 8) ||
        lpbiSrc->biCompression != BI_SAMP ||
        lpbiSrc->biSize != (sizeof(BITMAPINFOHEADER) + sizeof(DECOMP_FORMAT))) {
        return ICERR_BADFORMAT;
    }
    return ICERR_OK;
}

/*****************************************************************************
 *
 * DecompressQuery() implements ICM_DECOMPRESS_QUERY
 *
 * See CompressQuery()
 * 
 ****************************************************************************/
LRESULT NEAR PASCAL DecompressQuery(
    INSTINFO * pinst,
    DWORD dwFlags,
    LPBITMAPINFOHEADER lpbiSrc,
    LPVOID pSrc,
    int xSrc,
    int ySrc,
    int dxSrc,
    int dySrc,
    LPBITMAPINFOHEADER lpbiDst,
    LPVOID pDst,
    int xDst,
    int yDst,
    int dxDst,
    int dyDst)
{
    LRESULT l;

    DPF("DecompressQuery()");

    if (l = DecompressQueryFmt(pinst, lpbiSrc))
        return l;

    //
    // allow (-1) as a default width/height
    //
    if (dxSrc == -1)
        dxSrc = (int)lpbiSrc->biWidth;

    if (dySrc == -1)
        dySrc = (int)lpbiSrc->biHeight;

    //
    //  we cant clip the source.
    //
    if (xSrc != 0 || ySrc != 0) 
        return ICERR_BADPARAM;

    if (dxSrc != (int)lpbiSrc->biWidth || dySrc != (int)lpbiSrc->biHeight)
        return ICERR_BADPARAM;

    //
    //  are we being asked to query just the input format?
    //
    if (lpbiDst == NULL)
        return ICERR_OK;

    //
    // allow (-1) as a default width/height
    //
    if (dxDst == -1)
        dxDst = (int)lpbiDst->biWidth;

    if (dyDst == -1)
        dyDst = abs((int)lpbiDst->biHeight);

    //
    // make sure we can handle the format to decompress too.
    //
    if (lpbiDst->biCompression != BI_RGB ||             // must be full dib
        lpbiDst->biBitCount != lpbiSrc->biBitCount ||   // must be same bpp
        lpbiDst->biSize != sizeof(BITMAPINFOHEADER) ||
        dxSrc != dxDst || dySrc != dyDst)               // must be 1:1 (no stretch)
    {
        return ICERR_BADFORMAT;
    }

    return ICERR_OK;
}

/*****************************************************************************
 *
 * DecompressGetFormat() implements ICM_DECOMPRESS_GET_FORMAT
 *
 * See CompressGetFormat()
 *
 ****************************************************************************/
LRESULT NEAR PASCAL DecompressGetFormat(INSTINFO * pinst, LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
    DWORD dw;
    int dx,dy;
    WORD    wBytesPerPixel ;

    DPF("DecompressGetFormat()");

    if (dw = DecompressQueryFmt(pinst, lpbiIn))
        return dw;

    //
    // if lpbiOut == NULL then, return the size required to hold an output
    // format.  Remember to copy the bitmapinfo header and the colors,
    // but not the decompression format which gives the driver information
    // on how to decompress the data.  
    //
    if (lpbiOut == NULL)
        return (int)sizeof(BITMAPINFOHEADER) + 
               (int)lpbiIn->biClrUsed * sizeof(RGBQUAD);

    //
    // First copy the bitmapinfo header, then skip over the decompression
    // information in the IN buffer, and copy the colors.  The decompression
    // information is not part of the decompression format.
    //
    hmemcpy(lpbiOut, lpbiIn,
        (int)sizeof(BITMAPINFOHEADER));

    lpbiOut->biSize        = sizeof(BITMAPINFOHEADER);
    lpbiOut->biClrUsed     = lpbiIn->biClrUsed;
    lpbiOut->biBitCount    = lpbiIn->biBitCount ; 
    lpbiOut->biCompression = BI_RGB;

    dx = (int)lpbiIn->biWidth;
    dy = (int)lpbiIn->biHeight;
    wBytesPerPixel = (lpbiIn->biBitCount+7)/8 ;
    
    lpbiOut->biSizeImage   = wBytesPerPixel*(DWORD)dy*(DWORD)((dx+3)&~3);

    hmemcpy(
        (LPBYTE)lpbiOut + (int)lpbiOut->biSize,
        (LPBYTE)lpbiIn  + (int)lpbiIn->biSize,
        (int)lpbiIn->biClrUsed * sizeof(RGBQUAD));

    return ICERR_OK;
}

/*****************************************************************************
 *
 * DecompressGetPalette() implements ICM_GET_PALETTE
 *
 * This function has no Compress...() equivalent
 *
 * It is used to pull the palette from a frame in order to possibly do
 * a palette change.
 * 
 ****************************************************************************/
LRESULT NEAR PASCAL DecompressGetPalette(INSTINFO * pinst, LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
    DWORD dw;

    DPF("DecompressGetPalette()");

    if (dw = DecompressQueryFmt(pinst, lpbiIn))
        return dw;

    if (lpbiOut->biBitCount != 8) {
        return ICERR_BADFORMAT;
    }

    //
    // if you decompress full-color to 8 bit you need to put the "dither"
    // palette in lpbiOut
    //
    if (lpbiIn->biBitCount != 8) {
        return ICERR_BADFORMAT;
    }
    if (lpbiIn->biClrUsed == 0)
        lpbiIn->biClrUsed = 256;

    //
    // return the 8bit palette used for decompression.  
    //
    lpbiOut->biSize = sizeof(BITMAPINFOHEADER);
    hmemcpy(
        (LPBYTE)lpbiOut + (int)lpbiOut->biSize,
        (LPBYTE)lpbiIn + (int)lpbiIn->biSize,
        (int)lpbiIn->biClrUsed * sizeof(RGBQUAD));

    lpbiOut->biClrUsed = lpbiIn->biClrUsed;

    return ICERR_OK;
}

/*****************************************************************************
 *
 * DecompressBegin() implements ICM_DECOMPRESS_BEGIN
 *
 * See CompressBegin()
 * 
 ****************************************************************************/
LRESULT NEAR PASCAL DecompressBegin(
    INSTINFO * pinst,
    DWORD dwFlags,
    LPBITMAPINFOHEADER lpbiSrc,
    LPVOID pSrc,
    int xSrc,
    int ySrc,
    int dxSrc,
    int dySrc,
    LPBITMAPINFOHEADER lpbiDst,
    LPVOID pDst,
    int xDst,
    int yDst,
    int dxDst,
    int dyDst)
{
    LONG l;

    if (l = DecompressQuery(pinst, dwFlags, lpbiSrc, pSrc, xSrc, ySrc, dxSrc, dySrc, lpbiDst, pDst, xDst, yDst, dxDst, dyDst))
        return l;

    //
    //  make sure biSizeImage is set, the decompress code needs it to be
    //
    if (lpbiDst->biSizeImage == 0)
        lpbiDst->biSizeImage = (DWORD)(WORD)abs((int)lpbiDst->biHeight)*(DWORD)(WORD)DIBWIDTHBYTES(*lpbiDst);

    pinst->fDecompress = TRUE;

    return ICERR_OK;
}

/*****************************************************************************
 *
 * Decompress() implements ICM_DECOMPRESS
 *
 * See DecompressBegin()
 * 
 ****************************************************************************/
LRESULT NEAR PASCAL Decompress(
    INSTINFO * pinst,
    DWORD dwFlags,
    LPBITMAPINFOHEADER lpbiSrc,
    LPVOID lpSrc,
    int xSrc,
    int ySrc,
    int dxSrc,
    int dySrc,
    LPBITMAPINFOHEADER lpbiDst,
    LPVOID lpDst,
    int xDst,
    int yDst,
    int dxDst,
    int dyDst)
{
    BOOL	fBegin;

    //
    //  if we are called without a begin do the begin now, but dont make
    //  the begin "stick"
    //
    if (!(fBegin = pinst->fDecompress))
    {
        LRESULT l;

        if (l = DecompressBegin(pinst, dwFlags, lpbiSrc, lpSrc, xSrc, ySrc, dxSrc, dySrc, lpbiDst, lpDst, xDst, yDst, dxDst, dyDst))
            return l;

    }

    //
    //  because 'SAMP' frames are key frames we dont need to do any thing if
    //  behind.
    //
    if (dwFlags & ICDECOMPRESS_HURRYUP)
        return ICERR_OK;

    /* do the decompression */
    switch(lpbiSrc->biBitCount)
    {
        case 8 :
            MyDecompress8(pinst,lpbiSrc, lpSrc, lpbiDst, lpDst, xDst, yDst);
            break ;

        case 16 :
            MyDecompress16(pinst,lpbiSrc, lpSrc, lpbiDst, lpDst, xDst, yDst);
            break ;

        case 24 :
            MyDecompress24(pinst,lpbiSrc, lpSrc, lpbiDst, lpDst, xDst, yDst);
            break ;

    }

    if (!fBegin)
	DecompressEnd(pinst);

    return ICERR_OK;
}

/*****************************************************************************
 *
 * DecompressEnd() implements ICM_DECOMPRESS_END
 *
 * See CompressEnd()
 *
 ****************************************************************************/
LRESULT NEAR PASCAL DecompressEnd(INSTINFO * pinst)
{
    DPF("DecompressEnd()");

    if (!pinst->fDecompress)
        return ICERR_ERROR;

    //
    //  clean up decompress stuff.
    //

    pinst->fDecompress = FALSE;
    return ICERR_OK;
}

/*****************************************************************************
 *
 * DrawQuery() implements ICM_DRAW_QUERY
 *
 ****************************************************************************/
BOOL NEAR PASCAL DrawQuery(INSTINFO * pinst, LPBITMAPINFOHEADER lpbiInput)
{
    return FALSE;
}

/*****************************************************************************
 *
 * DrawBegin() implements ICM_DRAW_BEGIN
 *
 * This is just like DecompressBegin() except that we also must prepare to
 * actually draw the bitmap on the screen. ICDRAWBEGIN provides info specific
 * to this task.
 *
 ****************************************************************************/
LRESULT NEAR PASCAL DrawBegin(INSTINFO * pinst,ICDRAWBEGIN FAR *icinfo, DWORD dwSize)
{
    DPF("DrawBegin()");

    if (1)  // we dont draw!
        return ICERR_UNSUPPORTED;

    if (pinst->fDrawBegun)
        return ICERR_OK;

    pinst->fDrawBegun = TRUE;
    
    //
    // but if we did draw we would get ready to draw here
    //

    return ICERR_OK;
}

/*****************************************************************************
 *
 * Draw implements ICM_DRAW
 *
 * Decompress and draw
 *
 ****************************************************************************/
LRESULT NEAR PASCAL Draw(INSTINFO * pinst, ICDRAW FAR *icinfo, DWORD dwSize)
{
    DPF("Draw()");

    if (!pinst->fDrawBegun)
        return ICERR_ERROR;

    return ICERR_UNSUPPORTED;
}

/*****************************************************************************
 *
 * DrawEnd() implements ICM_DRAW_END
 *
 * See DecompressEnd()
 *
 ****************************************************************************/
LRESULT NEAR PASCAL DrawEnd(INSTINFO * pinst)
{
    DPF("DrawEnd()");

    if (1)  // we dont draw!
        return ICERR_UNSUPPORTED;

    if (!pinst->fDrawBegun)
        return ICERR_ERROR;

    pinst->fDrawBegun = FALSE;

    //
    // but if we did we would clean up here
    //

    return ICERR_OK;
}

/*****************************************************************************
 *
 * ConfigureDlgProc() is called by Configure
 *
 * This is a standard dialog proc which allows the user to
 * pick config options for the driver.
 *
 ****************************************************************************/
BOOL FAR PASCAL _loadds ConfigureDlgProc(
HWND hdlg, 
UINT msg, 
WPARAM wParam, 
LONG lParam)
{
    int             id;
    static int      s1;
    static int      s2;
    HWND            hsb;
    char            ach[10];
    
    static INSTINFO *pinst;

    #define SCROLL_MIN  1       
    #define SCROLL_MAX  16      

    #define SCROLL2_MIN  0       
    #define SCROLL2_MAX  4      

        
    switch (msg)
    {
        case WM_COMMAND:
            switch(GET_WM_COMMAND_ID(wParam, lParam))
            {
                case IDOK:
                    hsb = GetDlgItem(hdlg,ID_SCROLL);
                    pinst->CurrentState.dInfo.wPixelKeepRatio = s1 ;
                    hsb = GetDlgItem(hdlg,ID_SCROLL2);
                    pinst->CurrentState.dInfo.wColorBitsToDrop = s2 ;
                    EndDialog(hdlg,TRUE);
                    break;

                case IDCANCEL:
                    EndDialog(hdlg,FALSE);
                    break;
            }
            break;

        case WM_HSCROLL:
            hsb = GET_WM_HSCROLL_HWND(wParam, lParam);
            id = GetDlgCtrlID(hsb);

            switch( id )
            {
                case ID_SCROLL:
                    s1 = GetScrollPos(hsb,SB_CTL);

                    switch (GET_WM_HSCROLL_CODE(wParam, lParam))
                    {
                        case SB_LINEDOWN:      s1 += 1; break;
                        case SB_LINEUP:        s1 -= 1; break;
                        case SB_PAGEDOWN:      s1 += 4; break;
                        case SB_PAGEUP:        s1 -= 4; break;
                        case SB_THUMBTRACK:
                        case SB_THUMBPOSITION: s1 = (short)GET_WM_HSCROLL_POS(wParam, lParam); break;
                        default:               return TRUE;
                    }           
                
                    s1 = max(SCROLL_MIN,min(SCROLL_MAX,s1));
                    SetScrollPos(hsb,SB_CTL,s1,TRUE);
                    wsprintf(ach, "%02d", s1);
                    SetDlgItemText(hdlg,ID_TEXT,ach);
                    return TRUE;

                case ID_SCROLL2:
                    s2 = GetScrollPos(hsb,SB_CTL);

                    switch (GET_WM_HSCROLL_CODE(wParam, lParam))
                    {
                        case SB_LINEDOWN:      s2 += 1; break;
                        case SB_LINEUP:        s2 -= 1; break;
                        case SB_PAGEDOWN:      s2 += 4; break;
                        case SB_PAGEUP:        s2 -= 4; break;
                        case SB_THUMBTRACK:
                        case SB_THUMBPOSITION: s2 = (short)GET_WM_HSCROLL_POS(wParam, lParam); break;
                        default:               return TRUE;
                    }           

                    s2 = max(SCROLL2_MIN,min(SCROLL2_MAX,s2));
                    SetScrollPos(hsb,SB_CTL,s2,TRUE);
                    wsprintf(ach, "%02d", s2);
                    SetDlgItemText(hdlg,ID_TEXT2,ach);
                    return TRUE;

            }
            return TRUE ;


        case WM_INITDIALOG:
            pinst = (INSTINFO *)lParam;

            hsb = GetDlgItem(hdlg,ID_SCROLL);
            s1 = pinst->CurrentState.dInfo.wPixelKeepRatio;
            SetScrollRange(hsb,SB_CTL,SCROLL_MIN, SCROLL_MAX, TRUE);
            SetScrollPos(hsb,SB_CTL,s1,TRUE);
            wsprintf(ach, "%02d", s1);
            SetDlgItemText(hdlg,ID_TEXT,ach);

            hsb = GetDlgItem(hdlg,ID_SCROLL2);
            s2 = pinst->CurrentState.dInfo.wColorBitsToDrop;
            SetScrollRange(hsb,SB_CTL,SCROLL2_MIN, SCROLL2_MAX, TRUE);
            SetScrollPos(hsb,SB_CTL,s2,TRUE);
            wsprintf(ach, "%02d", s2);
            SetDlgItemText(hdlg,ID_TEXT2,ach);

            return TRUE;
    }
    return FALSE;
}

/*****************************************************************************
 *
 * dprintf() is called by the DPF macro if DEBUG is defined at compile time.
 *
 * The messages will be send to COM1: like any debug message. To 
 * enable debug output, add the following to WIN.INI :
 *
 * [debug]
 * ICSAMPLE=1
 *
 ****************************************************************************/

#ifdef DEBUG

void FAR cdecl dprintf(LPSTR szFormat, ...)
{
    char ach[128];

    static BOOL fDebug = -1;

    if (fDebug == -1)
        fDebug = GetProfileInt("Debug", MODNAME, FALSE);

    if (!fDebug)
        return;

    lstrcpy(ach, MODNAME ": ");
    wvsprintf(ach+lstrlen(ach),szFormat,(LPSTR)(&szFormat+1));
    lstrcat(ach, "\r\n");

    OutputDebugString(ach);
}

#endif
