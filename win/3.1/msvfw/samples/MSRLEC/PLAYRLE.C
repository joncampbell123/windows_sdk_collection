/*****************************************************************************
 *
 *  PlayRLE.C - RLE display code
 *
 ****************************************************************************/
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
#include "rle.h"

#define RLE_ESCAPE  0
#define RLE_EOL     0
#define RLE_EOF     1
#define RLE_JMP     2
#define RLE_RUN     3

extern WORD PASCAL __WinFlags;
#define WinFlags (WORD)(&__WinFlags)

typedef BYTE huge * HPRLE;
typedef BYTE far  * LPRLE;

void NEAR PASCAL DecodeRle(LPBITMAPINFOHEADER lpbi, HPRLE  pb, HPRLE prle);

//
// these are in RLEA.ASM
//
void NEAR PASCAL DecodeRle286(LPBITMAPINFOHEADER lpbi, LPBYTE pb, LPBYTE prle);
void NEAR PASCAL DecodeRle386(LPBITMAPINFOHEADER lpbi, LPBYTE pb, LPBYTE prle);

#define DibPtr(hdib) ((LPBYTE)GlobalLock(hdib) + \
            (int)((LPBITMAPINFOHEADER)GlobalLock(hdib))->biSize + \
            (int)((LPBITMAPINFOHEADER)GlobalLock(hdib))->biClrUsed * sizeof(RGBQUAD) )

void PlayRle(LPBITMAPINFOHEADER lpbi, LPVOID pDib, LPVOID pRLE)
{
    DWORD dw;

    dw = (DWORD)(WORD)lpbi->biHeight * (DWORD)(WORD)lpbi->biWidth;

    if (dw < 65536l)
        DecodeRle286(lpbi, pDib, pRLE);
    else if (WinFlags & WF_CPU286)
        DecodeRle(lpbi, pDib, pRLE);
    else
        DecodeRle386(lpbi, pDib, pRLE);
}

//  PlayRleDib
//
//  Play back a RLE buffer into a DIB
//
//      hdib        - dest DIB
//      x,y         - position in dest DIB where to place RLE
//      hrle        - src RLE
//
//  returns
//
//      none
//
void PlayRleDib(HANDLE hdib, HANDLE hrle)
{
    PlayRle((LPBITMAPINFOHEADER) GlobalLock(hdib), DibPtr(hdib), DibPtr(hrle));
}

//
//  DecodeRle   - 'C' version
//
//  Play back a RLE buffer into a DIB buffer
//
//  returns
//      none
//
void NEAR PASCAL DecodeRle(LPBITMAPINFOHEADER lpbi, HPRLE pb, HPRLE prle)
{
    BYTE    cnt;
    BYTE    b;
    WORD    x;
    WORD    dx,dy;
    WORD    wWidthBytes;

    wWidthBytes = (WORD)lpbi->biWidth+3 & ~3;

    x = 0;

    for(;;)
    {
        cnt = *prle++;
        b   = *prle++;

        if (cnt == RLE_ESCAPE)
        {
            switch (b)
            {
                case RLE_EOF:
                    return;

                case RLE_EOL:
                    pb += wWidthBytes - x;
                    x = 0;
                    break;

                case RLE_JMP:
                    dx = (WORD)*prle++;
                    dy = (WORD)*prle++;

                    pb += (DWORD)wWidthBytes * dy + dx;
                    x  += dx;

                    break;

                default:
                    cnt = b;
                    x  += cnt;
                    while (cnt-- > 0)
                        *pb++ = *prle++;

                    if (b & 1)
                        prle++;

                    break;
            }
        }
        else
        {
            x += cnt;

            while (cnt-- > 0)
                *pb++ = b;
        }
    }
}
