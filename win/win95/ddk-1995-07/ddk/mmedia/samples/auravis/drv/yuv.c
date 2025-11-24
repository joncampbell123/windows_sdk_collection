//////////////////////////////////////////////////////////////////////////
//	YUV.C								//
//									//
//	This module contains the YUV to RGB conversion routines.	//
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

#define NOMINMAX

#include <windows.h>
#include <mmsystem.h>
#include <msvideo.h>
#include <msviddrv.h>
#include <stdio.h>
#include <stdlib.h>
#include "avcapt.h"

//////////////////////////////////////////////////////////////////////////
//	The Y component of the Phillips front end produces values	//
//	between 7 and 115 for a nominal signal.  The following table	//
//	converts this range into 0 to 255 using a 5 bit lookup.		//
//////////////////////////////////////////////////////////////////////////

unsigned char map5to8[] =
       {0,   0,   9,   19,  29,  39,  48,  58,
	68,  78,  87,  97,  107, 117, 127, 136,
	146, 156, 166, 175, 185, 195, 205, 214,
	224, 234, 244, 255, 255, 255, 255, 255};


//////////////////////////////////////////////////////////////////////////
//	HW_YUV2RGBNoInterp(lpYUVBuf, lpRGBBuf, nLineLen, nBitsPerPix)	//
//									//
//	Convert a single line of unpacked 4:1:1 YUV data to RGB data.	//
//	R = V * 179/127 + Y						//
//	B = U * 226/127 + Y						//
//	G = 1.706*Y - .509*R +.194*B					//
//									//
//	Parameters:							//
//		lpYUVBuf						//
//		Pointer to the YUV source buffer			//
//		lpRGBBuf						//
//		Pointer to the RGB destination buffer			//
//		nLineLen (<=1048)					//
//		Length of the YUV line in pixels			//
//		nBitsPerPix (16, 24, 32)				//
//		Number of bits per pixel in RGB data			//
//	Returns:							//
//		None							//
//////////////////////////////////////////////////////////////////////////
void FAR PASCAL HW_YUV2RGBNoInterp(
	BYTE huge *	lpYUVBuf,
	BYTE huge *	lpRGBBuf,
	int		nLineLen,
	int		nBitsPerPix)
	{
	int		i, j, k;
	long		inindx, outindx;
	unsigned char	rComp,gComp,bComp;
	unsigned char	Ypix[4];
	char		U, V;
	int		ScaledU, ScaledV;
	int huge *	lpnYUVBuf;
	unsigned char huge *	lpbRGBBuf;

	lpnYUVBuf = (int huge *)lpYUVBuf;
	lpbRGBBuf = (char huge *)lpRGBBuf;
	inindx = outindx = 0L;

	for (i = 0; i < nLineLen; i += 4)
		{
		// Get 4 pixels, masking the Luma lsbs.
		// Note: this routine now masks each component to 5 bits.
		// This gives better conversion results to RGB.

		k = *lpnYUVBuf++;
		Ypix[0] = map5to8[(k & 0xf8) >> 3];
		U = (char) ((k & 0xc000) >> 8);		// (8 + 2*0)
		V = (char) ((k & 0x3000) >> 6);		// (6 + 2*0)

		k = *lpnYUVBuf++;
		Ypix[1]= map5to8[(k & 0xf8) >> 3];
		U |= (k & 0xc000) >> 10;		// (8 + 2*1)
		V |= (k & 0x3000) >> 8;			// (6 + 2*1)

		k = *lpnYUVBuf++;
		Ypix[2]= map5to8[(k & 0xf8) >> 3];
		U |= (k & 0xc000) >> 12;		// (8 + 2*2)
		V |= (k & 0x3000) >> 10;		// (6 + 2*2)

		k = *lpnYUVBuf++;
		Ypix[3]= map5to8[(k & 0xf8) >> 3];
		U |= (k & 0xc000) >> 14;		// (8 + 2*3)
		V |= (k & 0x3000) >> 12;		// (6 + 2*3)

		// Only the top 7 Chroma bits are valid, so....
		U &= 0xf8;
		V &= 0xf8;

		/* Now scale and form the RGB values */
		ScaledU =  MulDiv((int)U,462,256);	// 229/127 = 1.803
		ScaledV =  MulDiv((int)V,361,256);	// 179/127 = 1.409

		for (j = 0; j < 4; ++j)
			{
			rComp = (unsigned char) max(0, 
				min(255, (int) (ScaledV + (int)Ypix[j])));
			bComp = (unsigned char) max(0, 
				min(255, (int) (ScaledU + (int)Ypix[j])));
			// G = 1.706 * Y - 0.5094 * R - 0.1942 * B 
			gComp = (unsigned char) max(0, 
				min(255, (int)((Ypix[j] * 1747L
				- rComp * 522L - bComp * 199L) >> 10)));

			if (nBitsPerPix >= 24)
				{
				*lpbRGBBuf++ = bComp;
				*lpbRGBBuf++ = gComp;
				*lpbRGBBuf++ = rComp;
				}
			else
				{
				bComp >>= 3;
				gComp >>= 3;
				rComp >>= 3;
				*lpbRGBBuf++ = (unsigned char)(bComp
					| ((gComp & 0x07) << 5));
				*lpbRGBBuf++ = (unsigned char)((rComp << 2)
					| ((gComp & 0x18) >> 3));
				}
			}
		}
	}


#ifdef NOT_USED
//////////////////////////////////////////////////////////////////////////
//	HW_RGB2YUV(lpRGBBuf, lpYUVBuf, nLineLen, nBitsPerPix)		//
//									//
//	Convert a single line of RGB data to unpacked 4:1:1 YUV.	//
//	Y = .299*R + .587*G + .114*B					//
//	V = (R-Y) * 127/179						//
//	U = (B-Y) * 127/226						//
//	Note: The low bit of each YUV componant is dropped.		//
//	This routine has been updated to handle more than one line.	//
//									//
//	Parameters:							//
//		lpRGBBuf						//
//		Pointer to the RGB source buffer			//
//		lpYUVBuf						//
//		Pointer to the YUV destination buffer			//
//		nLineLen (<=1048)					//
//		Length of the RGB line in pixels			//
//		nBitsPerPix (8, 16, 24, 32)				//
//		Number of bits per pixel in RGB data			//
//									//
//	Returns:	None						//
//////////////////////////////////////////////////////////////////////////

void FAR PASCAL HW_RGB2YUV(
	LPSTR	lpRGBBuf,
	LPSTR	lpYUVBuf,
	long	nLineLen,
	int	nBitsPerPix)
	{
	int		j;
	long		i;
	long		inindx, outindx;
	unsigned char	rComp, gComp, bComp;
	unsigned char	Ypix[4];
	char		U, V;
	int		tmpU, tmpV;
	int huge *	lpnYUVBuf;
	char huge *	lpbRGBBuf;
	static unsigned char cmask[] = {0x60,0x18,0x06,0x01};

	lpnYUVBuf = (int huge *)lpYUVBuf;
	lpbRGBBuf = (char huge *)lpRGBBuf;

	for (i = inindx = outindx = 0; i < nLineLen; i += 4)
		{
		for (j = tmpU = tmpV = 0; j < 4; ++j)
			{
			// Get a RGB pixel (24, 32, or 16 bits)
			if (nBitsPerPix >= 24)
				{
				bComp = lpbRGBBuf[inindx++];
				gComp = lpbRGBBuf[inindx++];
				rComp = lpbRGBBuf[inindx++];
				if (nBitsPerPix > 24) inindx++;
				}
			else
				{
				bComp = lpbRGBBuf[inindx] & 0x1f;
				gComp = (lpbRGBBuf[inindx++] & 0xe0) >> 5;
				gComp |= ((lpbRGBBuf[inindx] & 0x03) << 3);
				rComp = (lpbRGBBuf[inindx++] & 0x7c) >> 2;
				bComp = map5to8[bComp];
				gComp = map5to8[gComp];
				rComp = map5to8[rComp];
				}
			/* Calculate Lumas and running Chroma totals */
			Ypix[j]= (gComp*587L)/1000
				+ (rComp*299L)/1000 + (bComp*114L)/1000;
			tmpU += (((int)bComp-(int)Ypix[j])*127L)/226;
			tmpV += (((int)rComp-(int)Ypix[j])*127L)/179;
			}

		// Divide by four to get average, divide by 2 to drop LSB.
		U = (tmpU/4)/2;
		V = (tmpV/4)/2;

		// Send the pixels out in unpacked format.
		for (j = 0; j < 4; ++j)
			{
			lpnYUVBuf[outindx]=Ypix[j] & 0xfe;
			lpnYUVBuf[outindx] |= ((V & cmask[j]) << (7 + 2*j));
			lpnYUVBuf[outindx++] |= ((U & cmask[j]) << (9 + 2*j));
			}
		}
	}


//////////////////////////////////////////////////////////////////////////
//	HW_YUV2RGB(lpYUVBuf, lpRGBBuf, nLineLen, nBitsPerPix)		//
//									//
//	Convert a single line of unpacked 4:1:1 YUV data to RGB data.	//
//	R = V * 179/127 + Y						//
//	B = U * 226/127 + Y						//
//	G = 1.706*Y + .509*R +.194*B					//
//	This routine has been updated to handle more than one line.	//
//									//
//	Parameters:							//
//		lpYUVBuf						//
//		Pointer to the YUV source buffer			//
//		lpRGBBuf						//
//		Pointer to the RGB destination buffer			//
//		nLineLen (<=1048)					//
//		Length of the YUV line in pixels			//
//		nBitsPerPix (8, 16, 24, 32)				//
//		Number of bits per pixel in RGB data			//
//									//
//	Returns:	None						//
//////////////////////////////////////////////////////////////////////////

void FAR PASCAL HW_YUV2RGB(
	LPSTR	lpYUVBuf,
	LPSTR	lpRGBBuf,
	int	nLineLen,
	int	nBitsPerPix)
	{
	int		i, j;
	long		inindx, outindx;
	unsigned char	rComp,gComp,bComp;
	unsigned char	Ypix[4];
	char		U, V, lastU, lastV, Upix[4], Vpix[4];
	int		diffU, diffV;
	int huge *	lpnYUVBuf;
	char huge *	lpbRGBBuf;

	lpnYUVBuf = (int huge *)lpYUVBuf;
	lpbRGBBuf = (char huge *)lpRGBBuf;

	for (i = inindx = outindx = 0; i < nLineLen; i += 4)
		{
		// Get 4 pixels, masking the Luma lsbs (only 7 bits were saved)
		// Gather the Chromas.
		for (j = U = V = 0; j < 4; ++j)
			{
			Ypix[j]=(lpnYUVBuf[inindx] & 0xfe);
			U |= (lpnYUVBuf[inindx] & 0xc000) >> (8 + 2*j);
			V |= (lpnYUVBuf[inindx++] & 0x3000) >> (6 + 2*j);
			}
		// Only the top 7 Chroma bits are valid, so....
		U &= 0xfe;
		V &= 0xfe;

		// First set?
		if (i == 0)
			{
			lastU = U;
			lastV = V;
			}
		diffU = U - lastU;
		diffV = V - lastV;

		/* Now scale and form the RGB values */
		/* Use linear interpolation to avoid blockiness */

		for (j = 0; j < 4; ++j)
			{
			Vpix[j] = lastV + (diffV * (j+1))/4;
			Upix[j] = lastU + (diffU * (j+1))/4;

			rComp = max(0, min(255, (Vpix[j]*179L)/127
				+ (int)Ypix[j]));
			bComp = max(0, min(255, (Upix[j]*226L)/127
				+ (int)Ypix[j]));
			gComp = max(0, min(255, ((int)Ypix[j] * 1706L)/1000
				- ((rComp * 509L)/1000)
				- ((bComp * 194L)/1000)));

			if (nBitsPerPix >= 24)
				{
				lpbRGBBuf[outindx++] = bComp;
				lpbRGBBuf[outindx++] = gComp;
				lpbRGBBuf[outindx++] = rComp;
				}
			else
				{
				bComp >>= 3;
				gComp >>= 3;
				rComp >>= 3;
				lpbRGBBuf[outindx++] = bComp |
					((gComp & 0x07) << 5);
				lpbRGBBuf[outindx++] = (rComp << 2)
					| ((gComp & 0x18) >> 3);
				}
			}
		// Save last Chroma values for interpolation.
		lastU=U;
		lastV=V;
		}
	}
#endif	// NOT_USED
