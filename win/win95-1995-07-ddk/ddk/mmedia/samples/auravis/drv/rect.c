//////////////////////////////////////////////////////////////////////////
//	RECT.C								//
//									//
//	This module contains the routines to handle formats, palettes,	//
//	and rectangles.							//
//									//
//	For the AuraVision video capture driver AVCAPT.DRV.		//
//									//
//	VideoIn channel Definitions:					//
//	Source - The overall dimensions of the frame buffer available	//
//		for digitizing the image.				//
//	Destination - The requested dimension and format of the image	//
//		data.  Since this hardware can only scale the		//
//		image during acquisition, setting the destination	//
//		format controls:					//
//		1.  The capture rectangle for incoming video.		//
//		2.  The size of the DIB containing the image.		//
//		3.  The size of the overlay window.			//
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

#include <windows.h>
#include <mmsystem.h>
#include <msvideo.h>
#include <msviddrv.h>
#include "avcapt.h"

#define WIDTHBYTES(i)     ((unsigned)((i+31)&(~31))/8)

extern unsigned char map5to8[];


//////////////////////////////////////////////////////////////////////////
//	YUV15toRGB15(Y, U, V)						//
//									//
//	Convert a single YUV15 to RGB15.				//
//	R = V * 179/127 + Y						//
//	B = U * 226/127 + Y						//
//	G = 1.706*Y - .509*R -.194*B					//
//									//
//	Returns: WORD (RGB15)						//
//////////////////////////////////////////////////////////////////////////

WORD NEAR PASCAL YUV15toRGB15(int Y, int U, int V)
	{
	unsigned char	rComp ,gComp ,bComp;
	int		ScaledU, ScaledV, ScaledY;

	// Now scale and form the RGB values.
	ScaledU = MulDiv(U, 462, 256);	// 229/127 = 1.803
                                	// ((int)((char)(BYTE)U) * 226)/127; 
	ScaledV = MulDiv(V, 361, 256);	// 179/127 = 1.409
					// ((int)((char)(BYTE)V) * 179)/127;
	ScaledY = map5to8[Y >> 3];
	rComp = (unsigned char) max(0, min(255, (int) (ScaledV + ScaledY)));
	bComp = (unsigned char) max(0, min(255, (int) (ScaledU + ScaledY)));

	// G = 1.706 * Y - 0.5094 * R - 0.1942 * B 
	gComp = (unsigned char) max(0, 
		min(255, (int)((ScaledY * 1747L - rComp * 522L - bComp * 199L) >> 10)));

	rComp >>= 3;
	gComp >>= 3;
	bComp >>= 3;

	return (WORD) (((gComp & 0x1f) << 5) | ((bComp & 0x1f))
		| ((rComp & 0x1f) << 10));
	}


//////////////////////////////////////////////////////////////////////////
//	YUV15toRGB15Unscaled(Y, U, V)					//
//////////////////////////////////////////////////////////////////////////

WORD NEAR PASCAL YUV15toRGB15Unscaled (int Y, int U, int V)
	{
	unsigned char	rComp, gComp, bComp;
	int		ScaledU, ScaledV, ScaledY;

	ScaledY = Y;
	ScaledU = U;
	ScaledV = V;

	rComp = (unsigned char) max(0, min(255, (int) (ScaledV + ScaledY)));
	bComp = (unsigned char) max(0, min(255, (int) (ScaledU + ScaledY)));

	// G = 1.706 * Y - 0.5094 * R - 0.1942 * B 
	gComp = (unsigned char) max(0, 
		min(255, (int)((ScaledY * 1747L - rComp * 522L - bComp * 199L) >> 10)));

	rComp >>= 3;
	gComp >>= 3;
	bComp >>= 3;

	return (WORD) (((gComp & 0x1f) << 5) | ((bComp & 0x1f))
		| ((rComp & 0x1f) << 10));
	}


//////////////////////////////////////////////////////////////////////////
//	GetInSourceRect(lprc)						//
//////////////////////////////////////////////////////////////////////////

DWORD FAR PASCAL GetInSourceRect(LPRECT lprc)
	{
	*lprc = grcSourceIn;
	return DV_ERR_OK;
	}


//////////////////////////////////////////////////////////////////////////
//	SetInSourceRect(lprc)						//
//////////////////////////////////////////////////////////////////////////

DWORD FAR PASCAL SetInSourceRect(LPRECT lprc)
	{
	// Currently we dont allow the source rect to be set to
	// anything other than the size of the entire source!

	if (lprc->left != 0
		|| lprc->top != 0
		|| lprc->right != (int)(biSource.biWidth - 1)
		|| lprc->bottom != (int)(biSource.biHeight - 1))
		return DV_ERR_PARAM1;

	grcSourceIn = *lprc;

	return DV_ERR_OK;
	}


//////////////////////////////////////////////////////////////////////////
//	GetSourceFormat(lpbi, wSize)					//
//////////////////////////////////////////////////////////////////////////

DWORD FAR PASCAL GetSourceFormat(LPBITMAPINFOHEADER lpbi, WORD wSize)
	{
	if( wSize < sizeof(BITMAPINFOHEADER))
		return DV_ERR_SIZEFIELD;

	*lpbi = biSource;
	return DV_ERR_OK;
	}


//////////////////////////////////////////////////////////////////////////
//	SetSourceFormat(lpbi, wSize)					//
//////////////////////////////////////////////////////////////////////////

DWORD FAR PASCAL SetSourceFormat(LPBITMAPINFOHEADER lpbi, WORD wSize)
	{
	RECT			rc;
	BITMAPINFOHEADER	bi;

	if (wSize < sizeof(BITMAPINFOHEADER))
		return DV_ERR_SIZEFIELD;

	if (lpbi->biSize != sizeof(BITMAPINFOHEADER))
		return DV_ERR_PARAM2;

	if (lpbi->biCompression != BI_RGB)
		return DV_ERR_PARAM2;

	if (lpbi->biPlanes != 1)
		return DV_ERR_PARAM2;

	bi = biSource;
	biSource = *lpbi;
	SetRect(&rc, 0, 0, (int)lpbi->biWidth - 1, (int)lpbi->biHeight - 1);
	SetInSourceRect(&rc);

	gwWidthBytesSource = WIDTHBYTES(lpbi->biWidth * lpbi->biBitCount);
	biSource.biSizeImage = gwWidthBytesSource * lpbi->biHeight;

	return DV_ERR_OK;
	}


//////////////////////////////////////////////////////////////////////////
//	GetDestFormat(lpbi, wSize)					//
//////////////////////////////////////////////////////////////////////////
DWORD FAR PASCAL GetDestFormat(LPBITMAPINFOHEADER lpbi, WORD wSize)
	{
	if( wSize < sizeof(BITMAPINFOHEADER))
		return DV_ERR_SIZEFIELD;

	*lpbi = biDest;
	return DV_ERR_OK;
	}


//////////////////////////////////////////////////////////////////////////
//	SetDestFormat(lpbi, wSize)					//
//									//
//	This routine can be called before the CopyBuffer and		//
//	translation buffers are allocated, so beware!			//
//////////////////////////////////////////////////////////////////////////

DWORD FAR PASCAL SetDestFormat(LPBITMAPINFOHEADER lpbi, WORD wSize)
	{
	RECT		rc;
	static BOOL	fYUVtoRGB16Initialized = FALSE;

	if( wSize < sizeof(BITMAPINFOHEADER))
		return DV_ERR_SIZEFIELD;

	if( lpbi->biSize != sizeof(BITMAPINFOHEADER))
		return DV_ERR_PARAM2;

	if (lpbi->biPlanes != 1)
		return DV_ERR_BADFORMAT;

	// Size must be a multiple of 40 and give square pixels
	if ((lpbi->biWidth % 40 != 0)
		|| (lpbi->biHeight != lpbi->biWidth * 3 / 4))
	return DV_ERR_BADFORMAT;

	switch (lpbi->biCompression)
		{
		case BI_RGB:
		switch (lpbi->biBitCount)
			{
			case 8:  gwDestFormat = IMAGE_FORMAT_PAL8;  break;
			case 16: gwDestFormat = IMAGE_FORMAT_RGB16; break;
			case 24: gwDestFormat = IMAGE_FORMAT_RGB24; break;
			default: return DV_ERR_BADFORMAT;
			}
		gwWidthBytesDest = WIDTHBYTES(lpbi->biWidth * lpbi->biBitCount);
		break;

		case ckidYUV411Compressed:
		gwDestFormat = IMAGE_FORMAT_YUV411COMPRESSED;
		gwWidthBytesDest = WIDTHBYTES(lpbi->biWidth*6);
		break;

		default:
		return DV_ERR_BADFORMAT;
		}

	biDest = *lpbi;
	SetRect(&rc, 0, 0, (int)lpbi->biWidth - 1, (int)lpbi->biHeight - 1);
	SetInDestRect(&rc);
	HW_SetDisplayRect(&rc);

	if (lpbi->biCompression == ckidYUV411Compressed)
		{
		biDest.biSizeImage = 48+(gwWidthBytesDest*lpbi->biHeight);
		biDest.biBitCount = 16;
		}
	else
		biDest.biSizeImage = gwWidthBytesDest * lpbi->biHeight;

	// If buffer size is too small for VidCap, pad it a little.
	if (biDest.biSizeImage < 0x12C0)
		biDest.biSizeImage = 0x12C0;

	// Commonly used globals (should reference biDest instead)
	gwWidth = (WORD) biDest.biWidth;
	gwHeight = (WORD) biDest.biHeight;
	gwSize40 = gwWidth / 40;

	// Allocate an extra copy buffer for smooth Overlays
	// This buffer may not be required in other implementations.

	if (fpCopyBuffer)
		{
		GlobalFree((HGLOBAL)SELECTOROF(fpCopyBuffer));
	
		fpCopyBuffer = MAKELP(GlobalAlloc(GMEM_FIXED|GMEM_SHARE,
			(DWORD) gwWidth * (DWORD) gwHeight * 2L), 0);

		if (fpCopyBuffer == NULL)
			return DV_ERR_NOMEM;
		}

	if (fpCopyBuffer2)
		{
		GlobalFree((HGLOBAL)SELECTOROF(fpCopyBuffer2));
	
		fpCopyBuffer2 = MAKELP(GlobalAlloc(GMEM_FIXED|GMEM_SHARE,
			(DWORD) gwWidth * (DWORD) gwHeight * 2L), 0);

		if (fpCopyBuffer2 == NULL)
			return DV_ERR_NOMEM;
		}

	//
	//  When converting to 16bit DIB format, calc an XLATE table
	//  the first time through this loop.
	//
	if (fpYUVtoRGB16)
		{
		if ((gwDestFormat == IMAGE_FORMAT_RGB16)
			&& !fYUVtoRGB16Initialized)
			{
			WORD	w;
			int	y, u, v;

			fYUVtoRGB16Initialized = TRUE;

			w = 0;
			for (y = 0; y <= 255; y += 8)
			    {
			    for (u = 0; u <= 255; u += 8)
				{
				for (v = 0; v <= 255; v += 8)
				    {
				    fpYUVtoRGB16[w++] = YUV15toRGB15(y,
						(int)(char)(BYTE)u,
						(int)(char)(BYTE)v);
				    }
				}
			    }
			}
		}
	return DV_ERR_OK;
	}


//////////////////////////////////////////////////////////////////////////
//	GetInDestRect(lprc)						//
//////////////////////////////////////////////////////////////////////////

DWORD FAR PASCAL GetInDestRect(LPRECT lprc)
	{
	*lprc = grcDestIn;
	return DV_ERR_OK;
	}


//////////////////////////////////////////////////////////////////////////
//	SetInDestRect(lprc)						//
//////////////////////////////////////////////////////////////////////////

DWORD FAR PASCAL SetInDestRect(LPRECT lprc)
	{
	//
	// Currently we dont allow the dest rect to be set to
	// anything other than the size of the entire dest!
	//
	if (lprc->left != 0 || lprc->top != 0
		|| lprc->right  != (int)(biDest.biWidth-1)
		|| lprc->bottom != (int)(biDest.biHeight-1))
		return DV_ERR_PARAM1;

	grcDestIn = *lprc;
	return DV_ERR_OK;
	}


//////////////////////////////////////////////////////////////////////////
//	SetExtOutSourceRect(lprc)					//
//									//
//	What portion of the frame buffer do we want to display?		//
//	This defines the pan and scroll of the overlay.			//
//////////////////////////////////////////////////////////////////////////

DWORD FAR PASCAL SetExtOutSourceRect(LPRECT lprc)
	{
	grcSourceExtOut = *lprc;
	HW_SetPanAndScroll(lprc);
	return DV_ERR_OK;
	}


//////////////////////////////////////////////////////////////////////////
//	SetExtOutDestRect(lprc)						//
//									//
//	Where (in Windows screen coords) is the overlay to appear?	//
//////////////////////////////////////////////////////////////////////////

DWORD FAR PASCAL SetExtOutDestRect(LPRECT lprc)
	{
	grcDestExtOut = *lprc;
	return DV_ERR_OK;
	}


//////////////////////////////////////////////////////////////////////////
//	GetDestPalette(lppal, wSize)					//
//////////////////////////////////////////////////////////////////////////

DWORD FAR PASCAL GetDestPalette(LPLOGPALETTE lppal, WORD wSize)
	{
	int	 i;

	if(wSize < sizeof(LOGPALETTE) +
		(palCurrent.palNumEntries-1) * sizeof(PALETTEENTRY))
		return DV_ERR_SIZEFIELD;

	// Copy the palette.

	lppal->palVersion = palCurrent.palVersion;
	lppal->palNumEntries = palCurrent.palNumEntries;

	for (i=0; i < (int)palCurrent.palNumEntries; i++)
		lppal->palPalEntry[i] = palCurrent.palPalEntry[i];

	return DV_ERR_OK;
	}


//////////////////////////////////////////////////////////////////////////
//	SetDestPalette(lppal, lpXlat)					//
//////////////////////////////////////////////////////////////////////////

DWORD FAR PASCAL SetDestPalette(LPLOGPALETTE lppal, LPBYTE lpXlat)
	{
	HPALETTE	hpal;
	WORD		w;
	DWORD		dwReturn = DV_ERR_OK;

	palCurrent.palVersion = lppal->palVersion;
	palCurrent.palNumEntries = lppal->palNumEntries;

	for (w=0; w<palCurrent.palNumEntries; w++)
		palCurrent.palPalEntry[w] = lppal->palPalEntry[w];

	// If passed an RGB15 Xlate table...
	if (lpXlat)
		{
		int	y, u, v;
		WORD	wRGB15;

		// We've been given an RGB15 Xlate table.
		// Use this to create a YUV15 Xlate table.

		w = 0;
		for (y = 0; y <= 255; y += 8)
		    {
		    for (u = 0; u <= 255; u += 8)
			{
			for (v = 0; v <= 255; v += 8)
			    {
			    wRGB15 = YUV15toRGB15 (y, (int)(char)(BYTE)u,
				(int)(char)(BYTE)v);
			    fpTrans16to8[w++] = *(lpXlat + wRGB15);
			    }
			}
		    }
		}
	else if (hpal = CreatePalette(lppal))
		{
		TransRecalcPal(hpal);
		DeleteObject(hpal);
		}
	else
		dwReturn = DV_ERR_CREATEPALETTE;

	return dwReturn;
	}
