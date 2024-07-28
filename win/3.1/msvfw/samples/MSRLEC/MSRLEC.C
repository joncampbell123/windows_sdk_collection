/****************************************************************************
 *
 *   msrlec.c
 * 
 *   MSRLEC is a sample installable compressor for AVI 1.0.  
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

#include <windows.h>
#include <compddk.h>

#include "msrlec.h"
#include "rle.h"

#define MODNAME         "RLEC"
char    szDescription[] = "Microsoft RLE Sample Compressor";
char    szName[]        = "MS-RLEC";

#define FOURCC_RLEC     mmioFOURCC('R','L','E','C')

#define TWOCC_DIB       aviTWOCC('d','b')
#define TWOCC_RLE       aviTWOCC('d','c')

#define VERSION_RLEC    0x00010000      // 1.00

extern HANDLE ghModule ;

RLESTATE DefaultRleState = {FOURCC_RLEC};

/*****************************************************************************
 *
 * RleLoad() is called from the DRV_LOAD message.
 *
 * Tasks such as allocating global memory that is non-instance specific
 * or initializing coprocessor hardware may be performed here.
 *
 * Our simple case needs none of this.
 *
 ****************************************************************************/
BOOL NEAR PASCAL RleLoad()
{
    DPF("RleLoad()");
    return TRUE;
}

/*****************************************************************************
 *
 * RleFree() is called from the DRV_FREE message.
 *
 * It should totally reverse the effects of Load() in preparation for
 * the DRV being removed from memory.
 *
 ****************************************************************************/
void NEAR PASCAL RleFree()
{
    DPF("RleFree()");
}

/*****************************************************************************
 *
 * RleOpen() is called from the ICM_OPEN message
 *
 * This message will be sent for a particular compress/decompress session.
 * Our code must verify that we are indeed being called as a video
 * compressor and create/initialize a state structure. The ICM will
 * give us back the pointer to that structure on every message dealing
 * with this session.
 *
 ****************************************************************************/
PRLEINST NEAR PASCAL RleOpen(ICOPEN FAR * icinfo)
{
    PRLEINST pri;

    DPF("Open('%4.4ls', '%4.4ls')", (LPSTR)&icinfo->fccType, (LPSTR)&icinfo->fccHandler);

    //
    // refuse to open if we are not being opened as a Video compressor
    //
    if (icinfo->fccType != ICTYPE_VIDEO)
        return NULL;

    pri = (PRLEINST)LocalAlloc(LPTR, sizeof(RLEINST));

    if (pri)
    {
        RleSetState(pri, NULL, 0);
    }
    return pri;
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
DWORD NEAR PASCAL RleClose(PRLEINST pri)
{
    DPF("RleClose()");

    if (!pri)
        return FALSE;

    LocalFree((LOCALHANDLE)pri);
    return TRUE;
}

/*****************************************************************************
 *
 * RleQueryAbout() and RleAbout() handle the ICM_ABOUT message.
 *
 * RleQueryAbout() returns TRUE to indicate we support an about box.
 * RleAbout() displays the box.
 *
 ****************************************************************************/
BOOL NEAR PASCAL RleQueryAbout(RLEINST * pinst)
{
    DPF("RleQueryAbout()");

    return TRUE;
}

DWORD NEAR PASCAL RleAbout(RLEINST * pinst, HWND hwnd)
{
    DPF("RleAbout()");
    MessageBox(hwnd,szDescription,szName,MB_OK|MB_ICONINFORMATION);
    return ICERR_OK;
}

/*****************************************************************************
 *
 * RleQueryConfigure() and RleConfigure() implement the ICM_CONFIGURE message.
 *
 * These functions put up a dialog that allows the user, if he so
 * chooses, to modify the configuration portion of our state info.
 * 
 ****************************************************************************/
BOOL NEAR PASCAL RleQueryConfigure(RLEINST * pinst)
{
    DPF("RleQueryConfigure()");
    return FALSE;
}

DWORD NEAR PASCAL RleConfigure(RLEINST * pinst, HWND hwnd)
{
    DPF("RleConfigure()");

    return ICERR_OK;
}

/*****************************************************************************
 *
 * RleGetState() implements the ICM_GETSTATE message.
 * 
 * We copy our configuration information and return how many bytes it took.
 *
 ****************************************************************************/
DWORD NEAR PASCAL RleGetState(PRLEINST pri, LPVOID pv, DWORD dwSize)
{
    DPF("RleGetState(%08lX, %ld)", pv, dwSize);

    if (pv == NULL || dwSize == 0)
        return sizeof(RLESTATE);

    if (pri == NULL || dwSize < sizeof(RLESTATE))
        return 0;

    *(LPRLESTATE)pv = pri->RleState;

    // return number of bytes copied
    return sizeof(RLESTATE);
}

/*****************************************************************************
 *
 * RleSetState() implements the ICM_SETSTATE message.
 *
 * The ICM is giving us configuration information saved by GetState()
 * earlier.
 *
 ****************************************************************************/
DWORD NEAR PASCAL RleSetState(PRLEINST pri, LPVOID pv, DWORD dwSize)
{
    DPF("RleSetState(%08lX, %ld)", pv, dwSize);

    if (pv == NULL || dwSize == 0)
    {
        pv = &DefaultRleState;
        dwSize = sizeof(RLESTATE);
    }

    if (pri == NULL || dwSize < sizeof(RLESTATE))
        return 0;

    // make sure it is our state information.

    if (((LPRLESTATE)pv)->fccHandler != FOURCC_RLEC)
        return 0;

    pri->RleState = *(LPRLESTATE)pv;

    // return number of bytes copied
    return sizeof(RLESTATE);
}

/*****************************************************************************
 *
 * RleGetInfo() implements the ICM_GETINFO message
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
DWORD NEAR PASCAL RleGetInfo(PRLEINST pri, ICINFO FAR *icinfo, DWORD dwSize)
{
    DPF("RleGetInfo()");

    if (icinfo == NULL)
        return sizeof(ICINFO);

    if (dwSize < sizeof(ICINFO))
        return 0;

    icinfo->dwSize	    = sizeof(ICINFO);
    icinfo->fccType	    = ICTYPE_VIDEO;
    icinfo->fccHandler      = FOURCC_RLEC;
    icinfo->dwFlags	    = VIDCF_TEMPORAL;    // supports inter-frame
    icinfo->dwVersion       = VERSION_RLEC;
    icinfo->dwVersionICM    = ICVERSION;
    lstrcpy(icinfo->szDescription, szDescription);
    lstrcpy(icinfo->szName, szName);

    return sizeof(ICINFO);
}

/*****************************************************************************
 *
 * RleCompressQuery() handles the ICM_COMPRESSQUERY message
 *
 * This message basically asks, "Can you compress this into this?"
 *
 * We look at the input and output bitmap info headers and determine
 * if we can.
 *
 ****************************************************************************/
LRESULT NEAR PASCAL RleCompressQuery(PRLEINST pri, LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
    DPF("CompressQuery()");

    //
    // determine if the input DIB data is in a format we like.
    //
    if (lpbiIn == NULL ||
        lpbiIn->biBitCount != 8 ||
        lpbiIn->biCompression != BI_RGB)
        return ICERR_BADFORMAT;

    //
    //  are we being asked to query just the input format?
    //
    if (lpbiOut == NULL)
        return ICERR_OK;

    //
    // make sure we can handle the format to compress to also.
    //
    if (lpbiOut->biCompression != BI_RLE8 ||        // must be rle format
        lpbiOut->biBitCount != 8 ||                 // must be 8bpp
        lpbiOut->biWidth  != lpbiIn->biWidth ||     // must be 1:1 (no stretch)
        lpbiOut->biHeight != lpbiIn->biHeight)
        return ICERR_BADFORMAT;

    return ICERR_OK;
}

/*****************************************************************************
 *
 * RleCompressGetFormat() implements ICM_GETFORMAT
 *
 * This message asks, "If I gave you this bitmap, what would you liek to
 * compress it to?"
 *
 * If the output bitmap info header is NULL, we just return how big the
 * header would be (header + palette, actually)
 *
 * Otherwise, we fill in the header, most importantly the biSizeImage.
 * This field must contain an upper bound on the size of the compressed
 * frame. A value that is too high here will result in inefficient
 * memory allocation at compression time, but will not be reflected
 * to the stored bitmap - the compression algorithm may chop biSizeImage
 * down to the actual amount with no ill effects.
 * 
 ****************************************************************************/
DWORD NEAR PASCAL RleCompressGetFormat(PRLEINST pri, LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
    DWORD dw;

    DPF("CompressGetFormat()");

    if (dw = (DWORD)RleCompressQuery(pri, lpbiIn, NULL))
        return dw;

    dw = lpbiIn->biSize + (int)lpbiIn->biClrUsed * sizeof(RGBQUAD);

    //
    // if lpbiOut == NULL then, return the size required to hold a output
    // format
    //
    if (lpbiOut == NULL)
        return dw;

    hmemcpy(lpbiOut, lpbiIn, dw);

    lpbiOut->biBitCount    = 8;
    lpbiOut->biCompression = BI_RLE8;
    lpbiOut->biSizeImage   = RleCompressGetSize(pri, lpbiIn, lpbiOut);

    return ICERR_OK;
}

/*****************************************************************************
 *
 * RleCompressBegin() implements ICM_COMPRESSBEGIN
 *
 * We're about to start compressing, initialize coprocessor, etc.
 * 
 ****************************************************************************/
LRESULT NEAR PASCAL RleCompressBegin(PRLEINST pri, LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
    LRESULT lResult;

    DPF("CompressBegin()");

    if (lResult = RleCompressQuery(pri, lpbiIn, lpbiOut))
        return lResult;

    return ICERR_OK;
}

/*****************************************************************************
 *
 * RleCompressGetSize() implements ICM_COMPRESS_GET_SIZE
 *
 * This function returns how much (upper bound) memory a compressed frame
 * will take.
 *
 ****************************************************************************/
DWORD NEAR PASCAL RleCompressGetSize(PRLEINST pri, LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
    int dx,dy;

    DPF("CompressGetSize()");

    //
    // we assume RLE data will never be more than twice the size of a full frame.
    //
    dx = (int)lpbiIn->biWidth;
    dy = (int)lpbiIn->biHeight;

    return (DWORD)dx * dy * 2;
}

/*****************************************************************************
 *
 * RleCompress() implements ICM_COMPRESS
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
 ****************************************************************************/
LRESULT NEAR PASCAL RleCompress(PRLEINST pri, ICCOMPRESS FAR *icinfo, DWORD dwSize)
{
    LRESULT lResult;

    if (lResult = RleCompressQuery(pri, icinfo->lpbiInput, icinfo->lpbiOutput))
        return lResult;

    DeltaFrame386(
        icinfo->lpbiOutput,
        icinfo->lpPrev,
        icinfo->lpInput,
        icinfo->lpOutput);

    if (icinfo->lpckid)
        *icinfo->lpckid = TWOCC_RLE;
    
    if (icinfo->lpdwFlags && icinfo->lpbiPrev == NULL)
        *icinfo->lpdwFlags |= AVIIF_KEYFRAME;

    return ICERR_OK;
}

/*****************************************************************************
 *
 * RleCompressEnd() is called on ICM_COMPRESS_END
 *
 * This function is a chance to flush buffers, deinit hardware, etc.
 * after compressing a single frame.
 *
 ****************************************************************************/
LRESULT NEAR PASCAL RleCompressEnd(PRLEINST pri)
{
    DPF("RleCompressEnd()");

    return ICERR_OK;
}

/*****************************************************************************
 *
 * RleDecompressQuery() implements ICM_DECOMPRESS_QUERY
 *
 * See RleCompressQuery()
 * 
 ****************************************************************************/
LRESULT NEAR PASCAL RleDecompressQuery(RLEINST * pri, LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
    DPF("RleDecompressQuery()");

    //
    // determine if the input DIB data is in a format we like.
    //
    if (lpbiIn == NULL ||
        lpbiIn->biBitCount != 8 ||
        lpbiIn->biCompression != BI_RLE8)
        return ICERR_BADFORMAT;

    //
    //  are we being asked to query just the input format?
    //
    if (lpbiOut == NULL)
        return ICERR_OK;

    //
    // make sure we can handle the format to decompress too.
    //
    if (lpbiOut->biCompression != BI_RGB ||         // must be full dib
        lpbiOut->biBitCount != 8 ||                 // must be 8bpp
        lpbiOut->biWidth  != lpbiIn->biWidth ||     // must be 1:1 (no stretch)
        lpbiOut->biHeight != lpbiIn->biHeight)
        return ICERR_BADFORMAT;

    return ICERR_OK;
}

/*****************************************************************************
 *
 * RleDecompressGetFormat() implements ICM_DECOMPRESS_GET_FORMAT
 *
 * See RleCompressGetFormat()
 *
 ****************************************************************************/
DWORD NEAR PASCAL RleDecompressGetFormat(RLEINST * pri, LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
    DWORD dw;
    int dx,dy;

    DPF("RleDecompressGetFormat()");

    if (dw = (DWORD)RleDecompressQuery(pri, lpbiIn, NULL))
        return dw;

    dw = lpbiIn->biSize + (int)lpbiIn->biClrUsed * sizeof(RGBQUAD);

    //
    // if lpbiOut == NULL then, return the size required to hold a output
    // format
    //
    if (lpbiOut == NULL)
        return dw;

    hmemcpy(lpbiOut, lpbiIn, dw);

    dx = (int)lpbiIn->biWidth;
    dy = (int)lpbiIn->biHeight;

    lpbiOut->biBitCount    = 8;
    lpbiOut->biCompression = BI_RGB;
    lpbiOut->biSizeImage   = (DWORD)dy*(DWORD)((dx+3)&~3);

    return ICERR_OK;
}

/*****************************************************************************
 *
 * RleDecompressGetPalette() implements ICM_GET_PALETTE
 *
 * This function has no Compress...() equivalent
 *
 * It is used to pull the palette from a frame in order to possibly do
 * a palette change.
 * 
 ****************************************************************************/
LRESULT NEAR PASCAL RleDecompressGetPalette(RLEINST * pinst, LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
    LRESULT lResult;

    DPF("RleDecompressGetPalette()");

    if (lResult = RleDecompressQuery(pinst, lpbiIn, lpbiOut))
        return lResult;

    if (lpbiOut->biBitCount != 8)
        return ICERR_BADFORMAT;

    if (lpbiIn->biClrUsed == 0)
        lpbiIn->biClrUsed = 256;

    //
    // return the 8bit palette used for decompression.
    //
    hmemcpy(
        (LPBYTE)lpbiOut + (int)lpbiOut->biSize,
        (LPBYTE)lpbiIn + (int)lpbiIn->biSize,
        (int)lpbiIn->biClrUsed * sizeof(RGBQUAD));

    lpbiOut->biClrUsed = lpbiIn->biClrUsed;

    return ICERR_OK;
}

/*****************************************************************************
 ****************************************************************************/
LRESULT NEAR PASCAL RleDecompressBegin(RLEINST * pri, LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
    LRESULT lResult;

    if (lResult = RleDecompressQuery(pri, lpbiIn, lpbiOut))
        return lResult;

    return ICERR_OK;
}

/*****************************************************************************
 *
 * RleDecompressBegin() implements ICM_DECOMPRESS_BEGIN
 *
 * See RleCompressBegin()
 * 
 ****************************************************************************/
LRESULT NEAR PASCAL DecompressBegin(RLEINST * pinst, LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
    LRESULT lResult;

    DPF("RleDecompressBegin()");

    if (lResult = RleDecompressQuery(pinst, lpbiIn, lpbiOut))
        return lResult;

    return ICERR_OK;
}

/*****************************************************************************
 *
 * RleDecompress() implements ICM_DECOMPRESS
 *
 * See RleDecompressBegin()
 * 
 ****************************************************************************/
LRESULT NEAR PASCAL RleDecompress(RLEINST * pri, ICDECOMPRESS FAR *icinfo, DWORD dwSize)
{
    PlayRle(icinfo->lpbiOutput, icinfo->lpOutput, icinfo->lpInput);

    return ICERR_OK;
}

/*****************************************************************************
 *
 * RleDecompressEnd() implements ICM_DECOMPRESS_END
 *
 * See RleCompressEnd()
 *
 ****************************************************************************/
LRESULT NEAR PASCAL RleDecompressEnd(RLEINST * pri)
{
    DPF("RleDecompressEnd()");

    return ICERR_OK;
}

//////////////////////////////////////////////////////////////////////////////
//
// ICM_DRAW routines.
//
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
 *
 * RleDrawQuery() implements ICM_DRAW_QUERY
 *
 ****************************************************************************/
BOOL NEAR PASCAL RleDrawQuery(RLEINST * pinst, LPBITMAPINFOHEADER lpbiInput)
{
    return FALSE;
}

/*****************************************************************************
 ****************************************************************************/
LRESULT NEAR PASCAL RleDrawBegin(RLEINST * pri,ICDRAWBEGIN FAR *icinfo, DWORD dwSize)
{
    return ICERR_UNSUPPORTED;
}

/*****************************************************************************
 ****************************************************************************/
LRESULT NEAR PASCAL RleDraw(RLEINST * pri, ICDRAW FAR *icinfo, DWORD dwSize)
{
    return ICERR_UNSUPPORTED;
}

/*****************************************************************************
 ****************************************************************************/
LRESULT NEAR PASCAL RleDrawEnd(RLEINST * pri)
{
    return ICERR_UNSUPPORTED;
}

/*****************************************************************************
 *
 * dprintf() is called by the DPF macro if DEBUG is defined at compile time.
 *
 * The messages will be send to COM1: like any debug message. To 
 * enable debug output, add the following to WIN.INI :
 *
 * [debug]
 * MSRLEC=1
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
