//////////////////////////////////////////////////////////////////////////
//	MAPC.C								//
//									//
//	This module creates and destroys the tables used for YUV to RGB	//
//	conversion.							//
//									//
//	For the AuraVision video capture driver AVCAPT.DRV.		//
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

#define NUM_GRAY 64


//////////////////////////////////////////////////////////////////////////
//	TransInit()							//
//									//
//	Allocate and Initalize the translation tables.			//
//									//
//	Return 0 on success, 1 if failure (insufficient memory).	//
//////////////////////////////////////////////////////////////////////////

WORD FAR PASCAL TransInit(void)
	{
	WORD	w;
	LPBYTE	pb;

#ifdef DEBUG
	OutputDebugString("TransInit\n");
#endif

	// First alloc the 32K YUV to PAL8 table.
	fpTrans16to8 = MAKELP(GlobalAlloc(GMEM_FIXED|GMEM_SHARE, 
		1024L * 64L), 0);

	if (fpTrans16to8 == NULL)
		return 1;		// Error no memory.

	fpCopyBuffer = MAKELP(GlobalAlloc(GMEM_FIXED|GMEM_SHARE, 
		(DWORD) gwWidth * (DWORD) gwHeight * 2), 0);

	if (fpCopyBuffer == NULL)
		return 1;		// Error no memory.

	fpCopyBuffer2 = MAKELP(GlobalAlloc(GMEM_FIXED|GMEM_SHARE, 
		(DWORD) gwWidth * (DWORD) gwHeight * 2), 0);

	if (fpCopyBuffer2 == NULL)
		return 1;		// Error no memory.

	fpYUVtoRGB16 = (LPWORD)MAKELP(GlobalAlloc(GMEM_FIXED|GMEM_SHARE,
		(DWORD)64L * 1024L), 0);

	if (fpYUVtoRGB16 == NULL)
		return 1;		// Error no memory.

	// Set up init palette.
	palCurrent.palVersion = 0x0300;
	palCurrent.palNumEntries = NUM_GRAY;

	// Make a 64 gray scale palette as default.
	for (w=0; w<NUM_GRAY; w++)
		{
		palCurrent.palPalEntry[w].peRed   = (BYTE)(w * 255/(NUM_GRAY-1));
		palCurrent.palPalEntry[w].peGreen = (BYTE)(w * 255/(NUM_GRAY-1));
		palCurrent.palPalEntry[w].peBlue  = (BYTE)(w * 255/(NUM_GRAY-1));
		palCurrent.palPalEntry[w].peFlags = 0;
		}

	pb = fpTrans16to8;
	for (w = 0; w < 0x8000; w++)
		*pb++ = (BYTE)(WORD)((WORD) w / ((WORD) 0x8000 / NUM_GRAY));

	return 0;   // no error
	}


//////////////////////////////////////////////////////////////////////////
//	TransFini()							//
//									//
//	Deinitialize the translation tables.				//
//////////////////////////////////////////////////////////////////////////

void FAR PASCAL TransFini(void)
	{
	if (fpTrans16to8)
		{
		GlobalFree((HGLOBAL)SELECTOROF(fpTrans16to8));
		fpTrans16to8 = NULL;
		}
	if (fpCopyBuffer)
		{
		GlobalFree((HGLOBAL)SELECTOROF(fpCopyBuffer));
		fpCopyBuffer = NULL;
		}
	if (fpCopyBuffer2)
		{
		GlobalFree((HGLOBAL)SELECTOROF(fpCopyBuffer2));
		fpCopyBuffer2 = NULL;
		}
	if (fpYUVtoRGB16)
		{
		GlobalFree((HGLOBAL)SELECTOROF(fpYUVtoRGB16));
		fpYUVtoRGB16 = NULL;
		}
	}


//////////////////////////////////////////////////////////////////////////
//	TransRecalcPal							//
//									//
//	Calculate YUV to RGB translation table entries:			//
//	R = V * 179/127 + Y						//
//	B = U * 226/127 + Y						//
//	G = 1.706*Y + .509*R +.194*B					//
//////////////////////////////////////////////////////////////////////////

BOOL FAR PASCAL TransRecalcPal(HPALETTE hpal)
	{
	int	r, g, b;
	int	k;
	int	y, iu, iv;
	int	ScaledU, ScaledV;
	LPSTR	pb;
	char	u, v;

	pb = fpTrans16to8;

	for (k = 0; k < 256; k += 8)
		{
		y = k;
		for (iu = 0; iu < 256; iu += 8)
			{
			for (iv = 0; iv < 256; iv += 8)
				{
				u = (char)(BYTE)iu;
				v = (char)(BYTE)iv;
				ScaledU = (int)((u*226L)/127); 
				ScaledV = (int)((v*179L)/127);
				r = max(0, min(255, (int)(ScaledV + (int)y)));
				b = max(0, min(255, (int)(ScaledU + (int)y)));
				g = max(0, min(255, (int)((y * 1706L)/1000
					- ((r * 509L)/1000) - ((b * 194L)/1000))));

				*pb++ = (BYTE)GetNearestPaletteIndex(hpal,
					RGB(r,g,b));
				}
			}
		}

	palCurrent.palVersion = 0x0300;
	GetObject(hpal, sizeof(int), (LPVOID)&palCurrent.palNumEntries);
	GetPaletteEntries(hpal, 0, palCurrent.palNumEntries,
		palCurrent.palPalEntry);

	return TRUE;
	}
