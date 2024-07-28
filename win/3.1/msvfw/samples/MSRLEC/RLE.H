/****************************************************************************
 *
 *   RLE.H - RLE Delta frame routine
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

/* code in playrle.c */

void PlayRleDib(HANDLE hdib, HANDLE hrle);
void PlayRle(LPBITMAPINFOHEADER lpbi, LPVOID pDib, LPVOID pRLE);

/* code in df.asm */

extern void   FAR  PASCAL DeltaFrame386(
    LPBITMAPINFOHEADER  lpbi,
    LPVOID              pbPrev,
    LPVOID              pbDib,
    LPVOID              pbRle);
