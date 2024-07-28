/****************************************************************************
 *
 *   mapc.c
 * 
 *   Routines to handle YUV to 8-bit palette translations
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

#include <windows.h>
#include <mmsystem.h>
#include <msvideo.h>
#include <msviddrv.h>
#include "ct.h"

#define NUM_GRAY 64

/* TransInit - Translate Init
 *
 * Initalize the translation table
 *	Alloc the translate tables
 * 
 * Return 0 on success
 *
 */
WORD FAR PASCAL TransInit( void )
{
    WORD w;
    LPBYTE pb;

    /* First alloc the 32K table */
    fpTrans16to8 = GlobalLock(GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, 
        1024L * 32L));

    if (fpTrans16to8 == NULL)
        return 1;       // error no memory

    fpCopyBuffer = GlobalLock(GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, 
        (DWORD) gwWidth * (DWORD) gwHeight * 2));

    if (fpCopyBuffer == NULL)
        return 1;       // error no memory

    fpYUVtoRGB16 = (LPWORD) GlobalLock(GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, 
        (DWORD) 64L * 1024L));

    if (fpYUVtoRGB16 == NULL)
        return 1;       // error no memory

    /* set up init palette */
    palCurrent.palVersion = 0x0300;
    palCurrent.palNumEntries = NUM_GRAY;

    //
    //  make a 64 gray scale palette as default.
    //
    for (w=0; w<NUM_GRAY; w++) {
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


/* TransFini - Translate Finalize
 *
 * Clean up the translation table
 *
 * 
 */
void FAR PASCAL TransFini( void )
{
    if (fpTrans16to8) {
        GlobalFree(HIWORD(fpTrans16to8));
        fpTrans16to8 = NULL;
    }
    if (fpCopyBuffer) {
        GlobalFree(HIWORD(fpCopyBuffer));
        fpCopyBuffer = NULL;
    }
    if (fpYUVtoRGB16) {
        GlobalFree(HIWORD(fpYUVtoRGB16));
        fpYUVtoRGB16 = NULL;
    }
}


/* TransCalcNew - Calculate a new YUV translation table
 *
 * R = V * 179/127 + Y
 * B = U * 226/127 + Y
 * G = 1.706*Y + .509*R +.194*B
 */
BOOL FAR PASCAL TransRecalcPal( HPALETTE hpal )
{
    int r,g,b;
    int k;
    int y,iu,iv;
    int ScaledU, ScaledV;
    LPSTR pb;
    char u,v;

    pb = fpTrans16to8;

    for (k=0; k<256; k+=8) {
        y = k;
	for (iu=0; iu<256; iu+=8) {
	    for (iv=0; iv<256; iv+=8) {
                u = (char)(BYTE)iu;
                v = (char)(BYTE)iv;
                ScaledU = (int)((u*226L)/127); 
                ScaledV = (int)((v*179L)/127);
                r = max(0, min(255, (int) (ScaledV + (int)y)));
                b = max(0, min(255, (int) (ScaledU + (int)y)));
        	g = max(0, min(255, (int) ((y * 1706L)/1000 - ((r * 509L)/1000) - ((b * 194L)/1000))));

		*pb++ = (BYTE)GetNearestPaletteIndex(hpal, RGB(r,g,b) );
	    }
	}
    }

    palCurrent.palVersion = 0x0300;
    GetObject(hpal, sizeof(int), (LPVOID)&palCurrent.palNumEntries);
    GetPaletteEntries( hpal, 0, palCurrent.palNumEntries, palCurrent.palPalEntry);
	    
    return TRUE;
}

