//////////////////////////////////////////////////////////////////////////
//   Module:    AVWIN6.C                                                //
//   Target:    AVWIN.DLL                                               //
//                                                                      //
//   Summary:   This module contains the still image capture routines   //
//              for AVWIN.DLL.						//
//                                                                      //
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


//////////////////////////////////////////
//  Includes                            //
//////////////////////////////////////////
#include <windows.h>
#include "avwin.h"
#include "avwinrc.h"
#include "global.h"
#pragma warning(disable:4704)

// Capture and Playback routines


// capturemmp - capture an mmp file - IBM MMOTION format
//  reads YUV411 or YUV422 from the capture pipeline, converts to MMP style YUV,
//  and writes a BMP file to handle 'hfile'

int capturemmp( hfile, x, y, width, height )
HFILE hfile;
int x, y, width, height;
{
    unsigned int i, memmode, pad, ileave, save58, save6C;
    int status, frwidth, frheight;
    unsigned int tbuf[4];
    unsigned long size, pos;
    struct mmphd mmphdr;

    memmode = AV_GetRegister(0x18) & 0x07;      // get memory mode
    ileave = interleave[memmode];               // get interleave value
    i = AV_GetRegister(0x73) & 0x03;
    frwidth = framewidth[ memmode ][ i ];
    frheight = frameheight[ memmode ][ i ];

      // get current acquisition address
    pos = AV_GetRegister(0x70) + (AV_GetRegister(0x71) << 8)
                    + ((AV_GetRegister(72) & 0x03) << 16);

      // calc the x and y of current address
    x += ((int)(pos % frwidth) * ileave);
    y += (int)(pos / frwidth);
    if( x > frwidth || y > frheight ) return -6;

    if( x + width > frwidth ) {         // bound width to frame right edge
        width = frwidth - x;
    }

    if( y + height > frheight ) {       // bound height to frame bottom
        height = frheight - y;
    }

    pos = ((long)y * (frwidth/ileave)) + (x / ileave);   // calc position in video memory

      // pad width to pixel/interleave bound
    i = yuvpadbound[memmode];

    switch( i ) {
        case 2:
        pad = width & 0x01;     // pad up to even number, pad is 0 or 1
        break;

        case 4:
        pad = 3 - ((width + 3) & 0x03); // optimize '% i' for case i = 4
        break;

        default:
        pad = (i - 1) - ((width + (i-1)) % i);
        break;
    }

    if( pad ) {
        width += pad;
    }

    size = width * (unsigned long)height;       // number of pixels in image

    lstrcpy( (char *)&mmphdr, "YUV12C" );       // build mmp header
    mmphdr.res1 = 0;
    mmphdr.res2 = 0;
    mmphdr.res3 = 0;
    mmphdr.width = width;
    mmphdr.height = height;

    _lwrite( hfile, (unsigned char huge *)&mmphdr, sizeof(struct mmphd) );

    status = 1;             // assume works ok

    save58 = AV_GetRegister( 0x58 );
    save6C = AV_GetRegister( 0x6C );

    AV_WaitVGARetrace();		//;;;;
    AV_SetRegister( 0x30, 0xC0 );       //global reset
    AV_SetRegister( 0x58, 0x0D );
    AV_SetRegister( 0x6C, 0x04 );
    AV_SetRegister( 0x80, 0x80 );       //set capture mode
    AV_SetRegister( 0x68, 0xFF );
    AV_SetRegister( 0x69, 0x03 );
    AV_SetRegister( 0x39, AV_GetRegister(0x39) & 0xFC );    // make sure YUV format
    AV_SetRegister( 0x38, 0x42 );       //update registers in global reset
    AV_SetRegister( 0x30, 0x44 );       //tight decode, global reset off

    AV_SetRegister( 0x81, (int)(pos & 0xFF) );          //capture win address
    AV_SetRegister( 0x82, (int)((pos>>8) & 0xFF) );
    AV_SetRegister( 0x83, (int)((pos>>16) & 0x03) );
    i = (width / ileave) - 1;
    AV_SetRegister( 0x84, i & 0xFF );           //win width
    AV_SetRegister( 0x85, i >> 8 );
    AV_SetRegister( 0x86, (height-1) & 0xFF );  //win height
    AV_SetRegister( 0x87, (height-1) >> 8 );
    AV_SetRegister( 0x88, 0x04 );   //pix buffer low
    AV_SetRegister( 0x89, 0x0E );   //pix buffer high
    AV_SetRegister( 0x8A, 0xFF );   //multi buffer depth maximum
    AV_SetRegister( 0x8B, 0x03 );

    AV_UpdateShadowedRegisters();

    AV_SetRegister( 0x80, 0x90 );   //capture reset
    AV_GetRegister( 0x64 );         //read status to clear before cap enable

    capsigntoggle = !(AV_GetRegister(0x96) & 0x01);
    AV_SetRegister( 0x80, 0xC0 );   //capture enable, active

    do {
        i = AV_GetRegister( 0x64 ) & 0x10;  // wait for capture fifo ready
    } while( i == 0 );

    if( memmode <= 2 ) {            //YUV 411
        while( size >= 4 ) {
            // Code in asm so the C optimizer won't mess with it
            // Reads Aura uncompressed YUV 411 (4 words, 12 bits per pix), and
            //   formats packed YUV 411 used in IBM MMP files (3 words)
            _asm {
                mov     ax,uFrameSelector
                mov     es,ax
                mov     ax,word ptr es:[0]      // first video read
                cmp     capsigntoggle,0
                je      notog0
                xor     bx,0A00h
        notog0:
                shl     ax,4
                mov     bx,word ptr es:[0]      // second video read
                and     bx,0FFFh
                or      al,bh
                mov     word ptr tbuf[0],ax     // save first word
                mov     ax,word ptr es:[0]      // third video read
                and     ax,0FFFh
                ror     ax,4
                xchg    ah,bl                   // bl from second vid read
                mov     word ptr tbuf[2],ax     // save second word
                mov     ax,word ptr es:[0]      // forth video read
                and     ax,0FFFh
                or      ah,bl
                mov     word ptr tbuf[4],ax     // save third word
            };

            i = _lwrite( hfile, (unsigned char huge *)tbuf, 6 );
            size -= 4;

            if( i == HFILE_ERROR ) {
                status = -3;
                break;
            }
        }
    } else {                    // YUV 422
        while( size >= 4 ) {
            // Code in asm so the C optimizer won't mess with it
            // Reads Aura uncompressed YUV 422 (4 words, 16 bits per pix), and
            //   formats Packed YUV 411 used in IBM MMP files (3 words)
            _asm {
                mov     ax,uFrameSelector
                mov     es,ax
                mov     bx,word ptr es:[0]      // first video read UY0 in bx
                cmp     capsigntoggle,0
                je      notog1
                xor     bx,8000h
        notog1:
                xor     ah,ah
                mov     al,bh                   // al is U
                shl     ax,1
                shl     ax,1                    // U bits 7,6 into ah
                mov     bh,al                   // put U back
                mov     cx,word ptr es:[2]      // second video read VY1
                cmp     capsigntoggle,0
                je      notog2
                xor     cx,8000h
        notog2:
                mov     al,ch                   // ch is V
                shl     ax,2                    // shift 2 bits of V into ah
                mov     ch,al                   // put V back
                mov     al,bl                   // put Y0 in al
                shl     ax,4
                mov     dl,bh                   // put U in dl
                xor     dh,dh
                shl     dx,2                    // U bits 5, 6 in dh
                mov     bh,dl                   // put U back
                mov     dl,ch                   // put V in dl
                shl     dx,2
                mov     ch,dl                   // put V back
                or      al,dh                   // add bits #5 & 4 to 1st word
                mov     word ptr tbuf[0],ax     // store first word
                mov     byte ptr tbuf[3],cl     // Y1 byte
                xor     ah,ah
                xor     bl,bl
                mov     al,bh                   // get U in al
                shl     ax,2                    // shift bits U 3,2
                xchg    ah,bl
                shl     ax,2                    // shift bits U 1,0
                xchg    ah,bl
                mov     al,ch
                shl     ax,2                    // shift bits V 3,2
                xchg    ah,bl
                shl     ax,2                    // shift bits V 1,0
                xchg    ah,bl                   // UV bits 1,0 in bl
                mov     cx,word ptr es:[4]      // third video read VY2
                mov     al,cl                   // put Y2 in al
                shl     ax,4
                or      al,bl
                mov     byte ptr tbuf[2],ah     // store byte
                mov     byte ptr tbuf[5],al     // store byte
                mov     ax,word ptr es:[6]      // forth video read
                mov     byte ptr tbuf[4],al     // store Y3
            };

            i = _lwrite( hfile, (unsigned char huge *)tbuf, 6 );
            size -= 4;

            if( i == HFILE_ERROR ) {
                status = -3;
                break;
            }
        }
    }

    AV_WaitVGARetrace();		//;;;;
    AV_SetRegister( 0x30, 0xC0 );        // global reset
    AV_SetRegister( 0x80, 0 );           // clear capture mode
    AV_SetRegister( 0x58, save58 );
    AV_SetRegister( 0x6C, save6C );
    AV_SetRegister( 0x38, 0x42 );
    AV_SetRegister( 0x30, 0x40 );       // clear global reset
    AV_UpdateShadowedRegisters();

    return status;
}


// capturebmp24 - capture a 24 bit bmp file
//  reads YUV411 or YUV422 from the capture pipeline, converts to RGB, and
//  writes a BMP file to handle 'hfile'

int capturebmp24( hfile, x, y, width, height )
HFILE hfile;
int x, y, width, height;
{
    int status;
    unsigned int i, j, memmode, pad, dwpad, ileave;
    int frwidth, frheight;
    unsigned int save58, save6C;
    unsigned char *lineptr;
    unsigned char *tptr;
    long size, pos;
    BITMAPFILEHEADER bmpfilehdr;
    BITMAPINFOHEADER bmpinfohdr;

    memmode = AV_GetRegister(0x18) & 0x07;      // get memory mode
    ileave = interleave[memmode];               // get interleave value
    i = AV_GetRegister(0x73) & 0x03;
    frwidth = framewidth[ memmode ][ i ];
    frheight = frameheight[ memmode ][ i ];

      // get current acquisition address
    pos = AV_GetRegister(0x70) + (AV_GetRegister(0x71) << 8)
                    + ((AV_GetRegister(72) & 0x03) << 16);

      // calc the x and y of current address
    x += (int)(pos % frwidth);
    y += (int)(pos / frwidth);
    if( x > frwidth || y > frheight ) return -6;

    if( x + width > frwidth ) {         // bound width to frame right edge
        width = frwidth - x;
    }

    if( y + height > frheight ) {       // bound height to frame bottom
        height = frheight - y;
    }

    pos = ((long)y * (frwidth/ileave)) + (x/ileave);    // calc position in video memory

      // pad width to pixel/interleave bound
    i = yuvpadbound[memmode];

    switch( i ) {
        case 2:
        pad = width & 0x01;     // pad up to even number, pad is 0 or 1
        break;

        case 4:
        pad = 3 - ((width + 3) & 0x03); // optimize '% i' for case i = 4
        break;

        default:
        pad = (i - 1) - ((width + (i-1)) % i);
        break;
    }

    if( pad ) {
        width += pad;       // width is the amount to read from capture pipeline
    }

      // calculate amount to pad at the end of each line (BMP padded to dword bytes)
    dwpad = (((width * 3) + 3) & 0xFFFC) - (width * 3);

      // allocate a line buffer from local memory
    lineptr = (unsigned char *) LocalAlloc( LMEM_FIXED, (width+dwpad) * 3 );
    if( lineptr == NULL ) return -2;

    size = ((width * 3) + dwpad) * (unsigned long)height;    // number of bytes in image

//  wsprintf( lineptr, "CapBMP24 width = %d, pad = %d, dwpad = %d\n", width, pad, dwpad );
//  OutputDebugString(lineptr);

      // build bmp headers
    bmpfilehdr.bfType = 0x4D42;                 // "BM"
    bmpfilehdr.bfSize = size + sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);
    bmpfilehdr.bfReserved1 = 0;
    bmpfilehdr.bfReserved2 = 0;
    bmpfilehdr.bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);

    bmpinfohdr.biSize = sizeof(BITMAPINFOHEADER);
    bmpinfohdr.biWidth = (long)width;
    bmpinfohdr.biHeight = (long)height;
    bmpinfohdr.biPlanes = 1;
    bmpinfohdr.biBitCount = 24;
    bmpinfohdr.biCompression = 0;
    bmpinfohdr.biSizeImage = size;
    bmpinfohdr.biXPelsPerMeter = 0;
    bmpinfohdr.biYPelsPerMeter = 0;
    bmpinfohdr.biClrUsed = 0;
    bmpinfohdr.biClrImportant = 0;

      // write the BMP headers
    _lwrite( hfile, (unsigned char huge *)&bmpfilehdr, sizeof(BITMAPFILEHEADER) );
    _lwrite( hfile, (unsigned char huge *)&bmpinfohdr, sizeof(BITMAPINFOHEADER) );

    save58 = AV_GetRegister( 0x58 );
    save6C = AV_GetRegister( 0x6C );

    AV_WaitVGARetrace();		//;;;;
    AV_SetRegister( 0x30, 0xC0 );       //global reset
    AV_SetRegister( 0x58, 0x0D );
    AV_SetRegister( 0x6C, 0x04 );
    AV_SetRegister( 0x80, 0x80 );       //set capture mode
    AV_SetRegister( 0x68, 0xFF );
    AV_SetRegister( 0x69, 0x03 );
    AV_SetRegister( 0x38, 0x42 );       //update registers in global reset
    AV_SetRegister( 0x30, 0x44 );       //tight decode, global reset off
    AV_UpdateShadowedRegisters();

    AV_SetRegister( 0x81, (int)(pos & 0xFF) );          //capture win address
    AV_SetRegister( 0x82, (int)((pos>>8) & 0xFF) );
    AV_SetRegister( 0x83, (int)((pos>>16) & 0x03) );
    i = (width / ileave) - 1;
    AV_SetRegister( 0x84, i & 0xFF );           //win width
    AV_SetRegister( 0x85, i >> 8 );
    AV_SetRegister( 0x86, (height-1) & 0xFF );  //win height
    AV_SetRegister( 0x87, (height-1) >> 8 );
    AV_SetRegister( 0x88, 0x04 );   //pix buffer low
    AV_SetRegister( 0x89, 0x0E );   //pix buffer high
    AV_SetRegister( 0x8A, 0xFF );   //multi buffer depth maximum
    AV_SetRegister( 0x8B, 0x03 );

    AV_UpdateShadowedRegisters();

    status = 1;         // assume works ok
    j = height;

    AV_SetRegister( 0x80, 0x90 );   //capture reset
    AV_GetRegister( 0x64 );         //read status to clear before cap enable

    capsigntoggle = !(AV_GetRegister(0x96) & 0x01);
    AV_SetRegister( 0x80, 0xC0 );   //capture enable, active

    do {
        i = AV_GetRegister( 0x64 ) & 0x10;  // wait for capture fifo ready
    } while( i == 0 );

    while( j ) {
        i = width;
        tptr = lineptr;

        while( i ) {
            if( memmode <= 2 ) {
                yuv4112rgb( lpFrameBuffer, (unsigned char huge *)tptr );   // convert YUV411 to RGB
                i -= 4;
                tptr += 12;
            } else {
                yuv4222rgb( lpFrameBuffer, (unsigned char huge *)tptr );    // convert YUV422 to RGB
                i -= 2;
                tptr += 6;
            }
        }

        i = dwpad;
        while( i-- ) {              // pad to dword for BMP files
            *tptr++ = 0;
        }

        --j;
        size = j * ((long)(width * 3) + dwpad);
        _llseek( hfile, size + 54, 0 );              // seek to line position
        i = _lwrite( hfile, (unsigned char huge *)lineptr, (width * 3) + dwpad );

        if( i == HFILE_ERROR ) {
            status = -3;
            break;
        }
    }

    AV_WaitVGARetrace();		//;;;;
    AV_SetRegister( 0x30, 0xC0 );        // global reset
    AV_SetRegister( 0x80, 0 );           // clear capture mode
    AV_SetRegister( 0x58, save58 );
    AV_SetRegister( 0x6C, save6C );
    AV_SetRegister( 0x38, 0x42 );
    AV_SetRegister( 0x30, 0x40 );       // clear global reset
    AV_UpdateShadowedRegisters();

    LocalFree( (HLOCAL)lineptr );
    return status;
}


// capturebmp256 - capture an 8 bit bmp file
//  reads YUV411 or YUV422 from the capture pipeline, converts to RGB, and
//  writes a BMP file to handle 'hfile'

int capturebmp256( hfile, x, y, width, height )
HFILE hfile;
int x, y, width, height;
{
    int status, rdif38, bdif38, gdif38, rdif14, bdif14, gdif14;
    unsigned int i, j, memmode, pad, dwpad, ileave;
    int frwidth, frheight;
    unsigned int save58, save6C;
    long size, pos;
    unsigned char far *tptr;
    int far *threshptr;
    unsigned char far *line256;
    RGBQUAD far *palptr;
    RGBTRIPLE far *lptr1;
    RGBTRIPLE far *lptr2;
    RGBTRIPLE far *sptr;
    RGBTRIPLE far *nptr;
    HANDLE hglb;
    BITMAPFILEHEADER bmpfilehdr;
    BITMAPINFOHEADER bmpinfohdr;

    memmode = AV_GetRegister(0x18) & 0x07;      // get memory mode
    ileave = interleave[memmode];               // get interleave value
    i = AV_GetRegister(0x73) & 0x03;
    frwidth = framewidth[ memmode ][ i ];
    frheight = frameheight[ memmode ][ i ];

      // get current acquisition address
    pos = AV_GetRegister(0x70) + (AV_GetRegister(0x71) << 8)
                    + ((AV_GetRegister(72) & 0x03) << 16);

      // calc the x and y of current address
    x += (int)(pos % frwidth);
    y += (int)(pos / frwidth);
    if( x > frwidth || y > frheight ) return -6;

    if( x + width > frwidth ) {         // bound width to frame right edge
        width = frwidth - x;
    }

    if( y + height > frheight ) {       // bound height to frame bottom
        height = frheight - y;
    }

    pos = ((long)y * (frwidth/ileave)) + (x/ileave);    // calc position in video memory

      // pad width to pixel/interleave bound
    i = yuvpadbound[memmode];

    switch( i ) {
        case 2:
        pad = width & 0x01;     // pad up to even number, pad is 0 or 1
        break;

        case 4:
        pad = 3 - ((width + 3) & 0x03); // optimize '% i' for case i = 4
        break;

        default:
        pad = (i - 1) - ((width + (i-1)) % i);
        break;
    }

    if( pad ) {
        width += pad;       // width is the amount to read from capture pipeline
    }

      // calculate amount to pad at the end of each line (BMP padded to dword bytes)
    dwpad = 3 - ((width + 3) & 0x0003);

      // allocate buffers from memory, one alloc for lots of buffers
    hglb = GlobalAlloc( GPTR, (long)0x400 + ((width+3) * 6) + (width+dwpad) + 16 );
    if( hglb == NULL ) return -2;

    line256 = (LPBYTE) MAKELP( hglb, 0 );
    threshptr = (int far *)(line256 + (width+dwpad));
    palptr = (RGBQUAD far *)(line256 + (width+dwpad) + 16);
    lptr1 = (RGBTRIPLE far *)(line256 + (width+dwpad) + 16 + 0x400);
    lptr2 = (RGBTRIPLE far *)(line256 + (width+dwpad) + 16 + 0x400 + ((width+3) * 3));

    size = (width + dwpad) * (unsigned long)height;     // number of bytes in image

      // build bmp headers
    bmpfilehdr.bfType = 0x4D42;                 // "BM"
    bmpfilehdr.bfSize = size + 0x400 + sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);
    bmpfilehdr.bfReserved1 = 0;
    bmpfilehdr.bfReserved2 = 0;
    bmpfilehdr.bfOffBits = 0x400 + sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);

    bmpinfohdr.biSize = sizeof(BITMAPINFOHEADER);
    bmpinfohdr.biWidth = (long)width;
    bmpinfohdr.biHeight = (long)height;
    bmpinfohdr.biPlanes = 1;
    bmpinfohdr.biBitCount = 8;
    bmpinfohdr.biCompression = 0;
    bmpinfohdr.biSizeImage = size;
    bmpinfohdr.biXPelsPerMeter = 0;
    bmpinfohdr.biYPelsPerMeter = 0;
    bmpinfohdr.biClrUsed = 0;
    bmpinfohdr.biClrImportant = 0;

    buildpal666( (char far *)palptr, threshptr );             // fill the palette

      // write the BMP headers
    _lwrite( hfile, (unsigned char huge *)&bmpfilehdr, sizeof(BITMAPFILEHEADER) );
    _lwrite( hfile, (unsigned char huge *)&bmpinfohdr, sizeof(BITMAPINFOHEADER) );

      // write the palette
    _lwrite( hfile, (unsigned char huge *)palptr, 0x400 );

    save58 = AV_GetRegister( 0x58 );
    save6C = AV_GetRegister( 0x6C );

    AV_WaitVGARetrace();		//;;;;
    AV_SetRegister( 0x30, 0xC0 );       //global reset
    AV_SetRegister( 0x58, 0x0D );
    AV_SetRegister( 0x6C, 0x04 );
    AV_SetRegister( 0x80, 0x80 );       //set capture mode
    AV_SetRegister( 0x68, 0xFF );
    AV_SetRegister( 0x69, 0x03 );
    AV_SetRegister( 0x38, 0x42 );       //update registers in global reset
    AV_SetRegister( 0x30, 0x44 );       //tight decode, global reset off
    AV_UpdateShadowedRegisters();

    AV_SetRegister( 0x81, (int)(pos & 0xFF) );          //capture win address
    AV_SetRegister( 0x82, (int)((pos>>8) & 0xFF) );
    AV_SetRegister( 0x83, (int)((pos>>16) & 0x03) );
    i = (width / ileave) - 1;
    AV_SetRegister( 0x84, i & 0xFF );           //win width
    AV_SetRegister( 0x85, i >> 8 );
    AV_SetRegister( 0x86, (height-1) & 0xFF );  //win height
    AV_SetRegister( 0x87, (height-1) >> 8 );
    AV_SetRegister( 0x88, 0x04 );   //pix buffer low
    AV_SetRegister( 0x89, 0x0E );   //pix buffer high
    AV_SetRegister( 0x8A, 0xFF );   //multi buffer depth maximum
    AV_SetRegister( 0x8B, 0x03 );

    AV_UpdateShadowedRegisters();

    status = 1;         // assume works ok
    j = height;

    AV_SetRegister( 0x80, 0x90 );   //capture reset
    AV_GetRegister( 0x64 );         //read status to clear before cap enable

    capsigntoggle = !(AV_GetRegister(0x96) & 0x01);
    AV_SetRegister( 0x80, 0xC0 );   //capture enable, active

    do {
        i = AV_GetRegister( 0x64 ) & 0x10;  // wait for capture fifo ready
    } while( i == 0 );

// This capture routine uses diffusion dithering. A color is matched to the
// palette, and the error in that match is spread to the surrounding pixels.
// Three-eights of the error is given to the pixel next to the target pix (left or right),
// three-eights of the error is given to the pixel just below, and one-quarter
// of the error is given to the pixel diagonal below. To avoid a diagonal pattern
// on large single color areas, the first line is worked from left to right, and
// the next line is worked right to left.

      // get first line of 24 bit data
    i = width;
    sptr = lptr1;
    while( i ) {
        if( memmode <= 2 ) {
            yuv4112rgb( lpFrameBuffer, (unsigned char huge *)sptr );   // convert YUV411 to RGB
            i -= 4;
            sptr += 4;
        } else {
            yuv4222rgb( lpFrameBuffer, (unsigned char huge *)sptr );    // convert YUV422 to RGB
            i -= 2;
            sptr += 2;
        }
    }

    while( j ) {                // loop thru the rest of the lines
        i = width;
        sptr = lptr2;

        // get a line
        while( i ) {
            if( memmode <= 2 ) {
                yuv4112rgb( lpFrameBuffer, (unsigned char huge *)sptr );   // convert YUV411 to RGB
                i -= 4;
                sptr += 4;
            } else {
                yuv4222rgb( lpFrameBuffer, (unsigned char huge *)sptr );    // convert YUV422 to RGB
                i -= 2;
                sptr += 2;
            }
        }

        tptr = line256;
        sptr = lptr1;
        nptr = lptr2;

        i = width;
        while( i-- ) {
            *tptr = matchcolor666( sptr->rgbtBlue, sptr->rgbtGreen, sptr->rgbtRed, threshptr );

            bdif38 = ((sptr->rgbtBlue - (palptr + *tptr)->rgbBlue) * 3) / 8;
            gdif38 = ((sptr->rgbtGreen - (palptr + *tptr)->rgbGreen) * 3) / 8;
            rdif38 = ((sptr->rgbtRed - (palptr + *tptr)->rgbRed) * 3) / 8;

            bdif14 = ((sptr->rgbtBlue - (palptr + *tptr)->rgbBlue) / 4);
            gdif14 = ((sptr->rgbtGreen - (palptr + *tptr)->rgbGreen) / 4);
            rdif14 = ((sptr->rgbtRed - (palptr + *tptr)->rgbRed) / 4);

            // add 3eights of the diff to the pixel just below
            _asm {
                les     bx,nptr
                mov     al,byte ptr es:[bx]
                xor     ah,ah
                add     ax,bdif38
                jns     nb1         ;if not negative
                xor     ax,ax       ;use 0
        nb1:    test    ah,ah
                jz      nb2         ;if not overflow of byte
                mov     ax,00FFh    ;use 255
        nb2:    mov     byte ptr es:[bx],al
                inc     bx
                mov     al,byte ptr es:[bx]
                add     ax,gdif38
                jns     ng1         ;if not negative
                xor     ax,ax       ;use 0
        ng1:    test    ah,ah
                jz      ng2         ;if not overflow of byte
                mov     ax,00FFh    ;use 255
        ng2:    mov     byte ptr es:[bx],al
                inc     bx
                mov     al,byte ptr es:[bx]
                add     ax,rdif38
                jns     nr1         ;if not negative
                xor     ax,ax       ;use 0
        nr1:    test    ah,ah
                jz      nr2         ;if not overflow of byte
                mov     ax,00FFh    ;use 255
        nr2:    mov     byte ptr es:[bx],al
            };

            ++nptr;

            // add 2eights of the diff to the pixel below and to the right
            _asm {
                les     bx,nptr
                mov     al,byte ptr es:[bx]
                xor     ah,ah
                add     ax,bdif14
                jns     db1         ;if not negative
                xor     ax,ax       ;use 0
        db1:    test    ah,ah
                jz      db2         ;if not overflow of byte
                mov     ax,00FFh    ;use 255
        db2:    mov     byte ptr es:[bx],al
                inc     bx
                mov     al,byte ptr es:[bx]
                add     ax,gdif14
                jns     dg1         ;if not negative
                xor     ax,ax       ;use 0
        dg1:    test    ah,ah
                jz      dg2         ;if not overflow of byte
                mov     ax,00FFh    ;use 255
        dg2:    mov     byte ptr es:[bx],al
                inc     bx
                mov     al,byte ptr es:[bx]
                add     ax,rdif14
                jns     dr1         ;if not negative
                xor     ax,ax       ;use 0
        dr1:    test    ah,ah
                jz      dr2         ;if not overflow of byte
                mov     ax,00FFh    ;use 255
        dr2:    mov     byte ptr es:[bx],al
            };

            // add 3eights of the diff to pixel to the right (same line)
            ++sptr;         // next pixel

            _asm {
                les     bx,sptr
                mov     al,byte ptr es:[bx]
                xor     ah,ah
                add     ax,bdif38
                jns     sb1         ;if not negative
                xor     ax,ax       ;use 0
        sb1:    test    ah,ah
                jz      sb2         ;if not overflow of byte
                mov     ax,00FFh    ;use 255
        sb2:    mov     byte ptr es:[bx],al
                inc     bx
                mov     al,byte ptr es:[bx]
                add     ax,gdif38
                jns     sg1         ;if not negative
                xor     ax,ax       ;use 0
        sg1:    test    ah,ah
                jz      sg2         ;if not overflow of byte
                mov     ax,00FFh    ;use 255
        sg2:    mov     byte ptr es:[bx],al
                inc     bx
                mov     al,byte ptr es:[bx]
                add     ax,rdif38
                jns     sr1         ;if not negative
                xor     ax,ax       ;use 0
        sr1:    test    ah,ah
                jz      sr2         ;if not overflow of byte
                mov     ax,00FFh    ;use 255
        sr2:    mov     byte ptr es:[bx],al
            };

            ++tptr;
        }

        i = dwpad;
        while( i-- ) {              // pad to dword for BMP files
            *tptr++ = 0;
        }

        --j;
        _llseek( hfile, (j * (long)(width + dwpad)) + 54 + 0x400, 0 );      // seek to line position
        i = _lwrite( hfile, (unsigned char huge *)line256, width + dwpad );

        if( i == HFILE_ERROR ) {
            status = -3;
            break;
        }

        if( j == 0 ) break;

        // do next line working right to left
        i = width;
        sptr = lptr1;       // read into buffer 1

        while( i ) {
            if( memmode <= 2 ) {
                yuv4112rgb( lpFrameBuffer, (unsigned char huge *)sptr );   // convert YUV411 to RGB
                i -= 4;
                sptr += 4;
            } else {
                yuv4222rgb( lpFrameBuffer, (unsigned char huge *)sptr );    // convert YUV422 to RGB
                i -= 2;
                sptr += 2;
            }
        }

        tptr = line256 + width;
        i = dwpad;
        while( i-- ) {              // do dword pad first
            *tptr++ = 0;
        }

        tptr = line256 + (width-1);
        sptr = lptr2 + (width-1);           // source ptr is now in buff 2
        nptr = lptr1 + (width-1);

        i = width;
        while( i-- ) {
            *tptr = matchcolor666( sptr->rgbtBlue, sptr->rgbtGreen, sptr->rgbtRed, threshptr );

            bdif38 = ((sptr->rgbtBlue - (palptr + *tptr)->rgbBlue) * 3) / 8;
            gdif38 = ((sptr->rgbtGreen - (palptr + *tptr)->rgbGreen) * 3) / 8;
            rdif38 = ((sptr->rgbtRed - (palptr + *tptr)->rgbRed) * 3) / 8;

            bdif14 = ((sptr->rgbtBlue - (palptr + *tptr)->rgbBlue) / 4);
            gdif14 = ((sptr->rgbtGreen - (palptr + *tptr)->rgbGreen) / 4);
            rdif14 = ((sptr->rgbtRed - (palptr + *tptr)->rgbRed) / 4);

            // add 3eights of the diff to the pixel just below
            _asm {
                les     bx,nptr
                mov     al,byte ptr es:[bx]
                xor     ah,ah
                add     ax,bdif38
                jns     rnb1         ;if not negative
                xor     ax,ax       ;use 0
        rnb1:   test   ah,ah
                jz      rnb2         ;if not overflow of byte
                mov     ax,00FFh    ;use 255
        rnb2:   mov     byte ptr es:[bx],al
                inc     bx
                mov     al,byte ptr es:[bx]
                add     ax,gdif38
                jns     rng1        ;if not negative
                xor     ax,ax       ;use 0
        rng1:   test    ah,ah
                jz      rng2        ;if not overflow of byte
                mov     ax,00FFh    ;use 255
        rng2:   mov     byte ptr es:[bx],al
                inc     bx
                mov     al,byte ptr es:[bx]
                add     ax,rdif38
                jns     rnr1        ;if not negative
                xor     ax,ax       ;use 0
        rnr1:   test    ah,ah
                jz      rnr2        ;if not overflow of byte
                mov     ax,00FFh    ;use 255
        rnr2:   mov     byte ptr es:[bx],al
            };

            --nptr;

            // add 2eights of the diff to the pixel below and to the left
            _asm {
                les     bx,nptr
                mov     al,byte ptr es:[bx]
                xor     ah,ah
                add     ax,bdif14
                jns     rdb1        ;if not negative
                xor     ax,ax       ;use 0
        rdb1:   test    ah,ah
                jz      rdb2        ;if not overflow of byte
                mov     ax,00FFh    ;use 255
        rdb2:   mov     byte ptr es:[bx],al
                inc     bx
                mov     al,byte ptr es:[bx]
                add     ax,gdif14
                jns     rdg1        ;if not negative
                xor     ax,ax       ;use 0
        rdg1:   test    ah,ah
                jz      rdg2        ;if not overflow of byte
                mov     ax,00FFh    ;use 255
        rdg2:   mov     byte ptr es:[bx],al
                inc     bx
                mov     al,byte ptr es:[bx]
                add     ax,rdif14
                jns     rdr1        ;if not negative
                xor     ax,ax       ;use 0
        rdr1:   test    ah,ah
                jz      rdr2        ;if not overflow of byte
                mov     ax,00FFh    ;use 255
        rdr2:   mov     byte ptr es:[bx],al
            };

            // add 3eights of the diff to pixel to the left (same line)
            --sptr;         // next pixel

            _asm {
                les     bx,sptr
                mov     al,byte ptr es:[bx]
                xor     ah,ah
                add     ax,bdif38
                jns     rsb1        ;if not negative
                xor     ax,ax       ;use 0
        rsb1:   test    ah,ah
                jz      rsb2        ;if not overflow of byte
                mov     ax,00FFh    ;use 255
        rsb2:   mov     byte ptr es:[bx],al
                inc     bx
                mov     al,byte ptr es:[bx]
                add     ax,gdif38
                jns     rsg1        ;if not negative
                xor     ax,ax       ;use 0
        rsg1:   test    ah,ah
                jz      rsg2        ;if not overflow of byte
                mov     ax,00FFh    ;use 255
        rsg2:   mov     byte ptr es:[bx],al
                inc     bx
                mov     al,byte ptr es:[bx]
                add     ax,rdif38
                jns     rsr1        ;if not negative
                xor     ax,ax       ;use 0
        rsr1:   test    ah,ah
                jz      rsr2        ;if not overflow of byte
                mov     ax,00FFh    ;use 255
        rsr2:   mov     byte ptr es:[bx],al
            };

            --tptr;
        }

        --j;
        _llseek( hfile, (j * (long)(width + dwpad)) + 54 + 0x400, 0 );      // seek to line position
        i = _lwrite( hfile, (unsigned char huge *)line256, width + dwpad );

        if( i == HFILE_ERROR ) {
            status = -3;
            break;
        }
    }

    AV_WaitVGARetrace();		//;;;;
    AV_SetRegister( 0x30, 0xC0 );        // global reset
    AV_SetRegister( 0x80, 0 );           // clear capture mode
    AV_SetRegister( 0x58, save58 );
    AV_SetRegister( 0x6C, save6C );
    AV_SetRegister( 0x38, 0x42 );
    AV_SetRegister( 0x30, 0x40 );       // clear global reset
    AV_UpdateShadowedRegisters();

    GlobalFree(hglb);
    return status;
}
