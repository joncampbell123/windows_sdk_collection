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

typedef DWORD huge * LPHISTOGRAM;

#define RGB16(r,g,b) (\
            (((WORD)(r) >> 3) << 10) |  \
            (((WORD)(g) >> 3) << 5)  |  \
            (((WORD)(b) >> 3) << 0)  )

LPHISTOGRAM     InitHistogram(LPHISTOGRAM lpHistogram);
void            FreeHistogram(LPHISTOGRAM lpHistogram);
HPALETTE        HistogramPalette(LPHISTOGRAM lpHistogram, LPBYTE lp16to8, int nColors);
BOOL            DibHistogram(LPBITMAPINFOHEADER lpbi, LPBYTE lpBits, int x, int y, int dx, int dy, LPHISTOGRAM lpHistogram);
HANDLE          DibReduce(LPBITMAPINFOHEADER lpbi, LPBYTE lpBits, HPALETTE hpal, LPBYTE lp16to8);
