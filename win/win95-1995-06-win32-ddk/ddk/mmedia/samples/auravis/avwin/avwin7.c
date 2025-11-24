//////////////////////////////////////////////////////////////////////////
//   Module:    AVWIN7.C                                                //
//   Target:    AVWIN.DLL                                               //
//                                                                      //
//   Summary:   This module contains the still image playback and       //
//		palette routines for AVWIN.DLL.				//
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
#include <io.h>
#include "avwin.h"
#include "avwinrc.h"
#include "global.h"
#pragma warning(disable:4704)


// Buildpal666 and Matchcolor666 are routines for an orthogonal palette
// The palette is combinations of 0, 0x33, 0x66, 0x99, 0xCC, and 0xFF,
//  resulting in 216 evenly spaced colors. The threshold array is values
//  that 'divide' the colors, they are 0x1B, 0x4E, 0x81, 0xB4, 0xE8, and 0x100.
//  RGB values less than 0x1B map to 0, values less than 0x4E map to 0x33, etc.
//
// Buildpal666 - initialize the 666 palette and threshold values

int buildpal666(pal, thresh)
char far *pal;
int far *thresh;
{
    int i;
    unsigned int r, g, b;

    // init to zero
    for( i = 0; i < 0x400; ++i ) {
        *(pal+i) = 0;
    }

    // fill palette array
    i = 0;
    for( r = 0; r <= 0xFF; r += 0x33 ) {
        for( g = 0; g <= 0xFF; g += 0x33 ) {
            for( b = 0; b <= 0xFF; b += 0x33 ) {
                *pal++ = b;
                *pal++ = g;
                *pal++ = r;
                *pal++ = 0;     // RGBQUAD
                ++i;            // count colors used
            }
        }
    }

    *thresh++ = 27;
    *thresh++ = 78;
    *thresh++ = 129;
    *thresh++ = 180;
    *thresh++ = 232;
    *thresh   = 256;

    return i;
}


// matchcolor666 - return the pal index for the RGB values

int matchcolor666( b, g, r, thresh )
unsigned int b, g, r;
unsigned int far *thresh;
{
    int i;

    for( i = 0; i < 6; ++i ) {
        if( r < *(thresh+i) ) break;
    }
    r = i;

    for( i = 0; i < 6; ++i ) {
        if( g < *(thresh+i) ) break;
    }
    g = i;

    for( i = 0; i < 6; ++i ) {
        if( b < *(thresh+i) ) break;
    }
    b = i;

    return( (r * 36) + (g * 6) + b );
}


// readdib24 - capture a 24 bit DIB to memory
//  reads YUV411 or YUV422 from the capture pipeline, converts to RGB, and
//  stores in user suplied buffer
//
int readdib24( lpbuf, x, y, width, height )
unsigned char huge *lpbuf;
int x, y, width, height;
{
    int status;
    unsigned int i, j, memmode, dwpad, ileave;
    int frwidth, frheight;
    unsigned int save58, save6C;
    unsigned char huge *tptr;
    long pos;
    LPBITMAPINFOHEADER lpbmpinfohdr;

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
      // pad width DOWN to pixel/interleave bound
    i = yuvpadbound[memmode];

    switch( i ) {
        case 2:
        width &= 0x0FFFE;           // width must be even
        break;

        case 4:
        width &= 0x0FFFC;           // width must align 4 bytes
        break;

        default:
        width -= (width  % i);
        break;
    }

      // calculate amount to pad at the end of each line (BMP padded to dword bytes)
    i = width * 3;
    dwpad = ((i + 3) & 0xFFFC) - i;

    lpbmpinfohdr = (LPBITMAPINFOHEADER)lpbuf;

    lpbmpinfohdr->biSize = sizeof(BITMAPINFOHEADER);
    lpbmpinfohdr->biWidth = (long)width;
    lpbmpinfohdr->biHeight = (long)height;
    lpbmpinfohdr->biPlanes = 1;
    lpbmpinfohdr->biBitCount = 24;
    lpbmpinfohdr->biCompression = 0;
    lpbmpinfohdr->biSizeImage = 0;
    lpbmpinfohdr->biXPelsPerMeter = 0;
    lpbmpinfohdr->biYPelsPerMeter = 0;
    lpbmpinfohdr->biClrUsed = 0;
    lpbmpinfohdr->biClrImportant = 0;

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

    i = AV_GetRegister(0x97);
    AV_SetRegister( 0x97,  i & 0x7F );
    AV_SetRegister( 0x97,  i | 0x80 );

    AV_SetRegister( 0x38, 0x42 );   //update the registers

    status = 1;                     //assume works ok
    j = height;

      // point to last data line in file (first line in image)
    tptr = lpbuf + (((height-1) * (unsigned long)((width*3)+dwpad))
                + sizeof(BITMAPINFOHEADER));

    AV_SetRegister( 0x80, 0x90 );   //capture reset
    AV_GetRegister( 0x64 );         //read status to clear before cap enable

    capsigntoggle = !(AV_GetRegister(0x96) & 0x01);
    AV_SetRegister( 0x80, 0xC0 );   //capture enable, active

    do {
        i = AV_GetRegister( 0x64 ) & 0x10;  //wait for capture fifo ready
    } while( i == 0 );

    while( j ) {
        i = width;

        while( i ) {
            if( memmode <= 2 ) {
                yuv4112rgb( lpFrameBuffer, tptr );      // convert YUV411 to RGB
                i -= 4;
                tptr += 12;
            } else {
                yuv4222rgb( lpFrameBuffer, tptr );      // convert YUV422 to RGB
                i -= 2;
                tptr += 6;
            }
        }

        i = dwpad;
        while( i-- ) {              // pad to dword for BMP files
            *tptr++ = 0;
        }
        tptr -= (((width * 3) + dwpad) * 2);    // back up 2 lines

        --j;
    }

    AV_WaitVGARetrace();		//;;;;
    AV_SetRegister( 0x30, 0xC0 );        // global reset
    AV_SetRegister( 0x80, 0 );           // clear capture mode
    AV_SetRegister( 0x58, save58 );
    AV_SetRegister( 0x6C, save6C );
    AV_SetRegister( 0x38, 0x42 );
    AV_SetRegister( 0x30, 0x40 );       // clear global reset
    i = AV_GetRegister(0x97);
    AV_SetRegister( 0x97, i & 0x7F );
    AV_SetRegister( 0x97, i | 0x80 );

    return status;
}


// readimagemmp - capture an mmp file to memory
//  reads YUV411 or YUV422 from the capture pipeline, converts to RGB, and
//  stores in user suplied buffer
//
int readimagemmp( lpbuf, x, y, width, height )
unsigned char huge *lpbuf;
int x, y, width, height;
{
    unsigned int i, memmode, ileave, save58, save6C;
    int status, frwidth, frheight;
    unsigned int tbuf[4];
    unsigned long size, pos;
    struct mmphd far * lpmmphdr;

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

      // pad width DOWN to pixel/interleave bound
    i = yuvpadbound[memmode];

    switch( i ) {
        case 2:
        width &= 0x0FFFE;           // width must be even
        break;

        case 4:
        width &= 0x0FFFC;           // width must align 4 bytes
        break;

        default:
        width -= (width  % i);
        break;
    }

    size = width * (unsigned long)height;       // number of pixels in image

    lpmmphdr = (struct mmphd far *)lpbuf;       // build mmp header
    lstrcpy( (char far *)lpmmphdr, (char far *)"YUV12C" );
    lpmmphdr->res1 = 0;
    lpmmphdr->res2 = 0;
    lpmmphdr->res3 = 0;
    lpmmphdr->width = width;
    lpmmphdr->height = height;

    lpbuf += sizeof(struct mmphd);              // advance pointer to data

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
                push    ds
                les     di,lpbuf
                mov     ax,uFrameSelector
                mov     ds,ax
                cld

                mov     ax,word ptr ds:[0]      // first video read
                cmp     capsigntoggle,0
                je      notog0
                xor     bx,0A00h
        notog0:
                shl     ax,4
                mov     bx,word ptr ds:[0]      // second video read
                and     bx,0FFFh
                or      al,bh
                stosw                           // save first word
                mov     ax,word ptr ds:[0]      // third video read
                and     ax,0FFFh
                ror     ax,4
                xchg    ah,bl                   // bl from second vid read
                stosw                           // save second word
                mov     ax,word ptr ds:[0]      // forth video read
                and     ax,0FFFh
                or      ah,bl
                stosw                           // save third word

                pop     ds
            };

            lpbuf += 6;
            size -= 4;
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
            }

            for( i = 0; i < 6; ++i ) {
                *lpbuf++ = tbuf[i];
            }

            size -= 4;
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


// getfiletype - reads first part of file, checks & returns type
//  returns file type, -1 for error, -2 for unknown type

int getfiletype( hfile )
HFILE hfile;
{
    int tbuf[32];       // 64 byte buffer
    LPBITMAPINFOHEADER lpbmphdr;

    // Read the file header into tbuf
    _llseek( hfile, 0L, 0 );
    if( _hread(hfile, tbuf, 54L) != 54 ) return -1;

    switch( tbuf[0] ) {
        case 0x4D42:            // BMP file
            lpbmphdr = (LPBITMAPINFOHEADER)( (unsigned char *)tbuf + sizeof(BITMAPFILEHEADER) );
            switch( lpbmphdr->biBitCount ) {
                case 1:  return BM_DIB1P;   // monochrome bitmap
                case 4:  return BM_DIB4P;   // 16 color bitmap
                case 8:  return BM_DIB8P;   // 256 color bitmap
                case 24: return BM_DIB24;   // 24 bit color bitmap
                default: return -2;
            }
        case 0x5559:            // YUV file
            return BM_YUV411;
//-     case 0x4947:            // GIF file
//-     case 0x4949:            // Intel order TIFF file
//-     case 0x4D4D:            // Motorola order TIFF file
        default:
            return -2;          // unsupported type
    }
}


// loadpicfile - Allocate buffer & load a picture file from handle hfile
// returns - handle to global mem block, or 0 for no memory, or -1 for read error

HGLOBAL loadpicfile( hfile )
HFILE hfile;
{
    unsigned long size, bytesread;
    HGLOBAL hglbTemp;

    // get num of bytes to read
    size = (unsigned long)_filelength(hfile);

    // Allocate a buffer big enough to hold the file.
    hglbTemp = GlobalAlloc( GPTR, size );

    if( hglbTemp ) {
        // Read the file into the buffer.
        _llseek( hfile, 0L, 0 );
        bytesread = _hread( hfile, MAKELP(hglbTemp, 0), size );

        if( bytesread != size ) {
            GlobalFree(hglbTemp);
            hglbTemp = (unsigned int) -1;
        }
    }
    return hglbTemp;
}


// displaybmp()
// displays a BMP file or a DIB (no BITMAPFILEHEADER) from pointer
//
int displaybmp( lppicdata, x ,y )
unsigned char far *lppicdata;
int x, y;
{
    int i, j, width, height, ilpad, memmode, sendamount;
    int picwidth;
    int ileave, frwidth, frheight;
    long pos;
    unsigned char huge *lppic;
    unsigned char huge *lppicsave;
    RGBQUAD far *lppal;
    LPBITMAPINFOHEADER lpbmphdr;
    unsigned int    tbuf[16];
    unsigned int    regsave[240];
    static unsigned int monotrans[16][2] = {
            0x0000, 0x0000,     // 0
            0x0000, 0x0100,
            0x0000, 0x0001,
            0x0000, 0x0101,
            0x0100, 0x0000,     // 4
            0x0100, 0x0100,
            0x0100, 0x0001,
            0x0100, 0x0101,     // 7
            0x0001, 0x0000,
            0x0001, 0x0100,
            0x0001, 0x0001,
            0x0001, 0x0101,
            0x0101, 0x0000,     // 12, 0x0C
            0x0101, 0x0100,
            0x0101, 0x0001,
            0x0101, 0x0101      // 15, 0x0F
        };

    if( lppicdata == NULL ) return FALSE;

    if( *((unsigned int far *)lppicdata) == 0x4D42 ) {  // if 'BM' first char
        lpbmphdr = (LPBITMAPINFOHEADER)( lppicdata + sizeof(BITMAPFILEHEADER) );
    } else {
        lpbmphdr = (LPBITMAPINFOHEADER)lppicdata;
    }
    lppal = (RGBQUAD far *)( (char far *)lpbmphdr + lpbmphdr->biSize );

      // Get bitmap information from bitmap info header
    width = (int)lpbmphdr->biWidth;
    height = (int)lpbmphdr->biHeight;

    memmode = AV_GetRegister(0x18) & 0x07;          // get memory mode
    ileave = interleave[ memmode ];
    frwidth = framewidth[ memmode ][ AV_GetRegister(0x73) & 0x03 ];
    frheight = frameheight[ memmode ][ AV_GetRegister(0x73) & 0x03 ];

    if( x > frwidth || y > frheight ) return -6;

    if( x + width > frwidth ) {         // bound width to frame right edge
        width = frwidth - x;
    }

    if( y + height > frheight ) {       // bound height to frame bottom
        height = frheight - y;
    }

    wImageWidth = width;                // save global values for SysMetrics call
    wImageHeight = height;

    pos = ((long)y * (frwidth/ileave)) + (x / ileave);   // calc position in video memory

      // adjust width by pixel groups - calc number of pixels we actually send to playback pipe
    switch( memmode ) {
        case 0:             // YUV411 sendamount dword bound
        case 1:
        case 2:
            sendamount = (width+3) & 0xFFFC;
            break;
        case 3:             // YUV422 sendamount word bound
        case 4:
            sendamount = (width+1) & 0xFFFE;
            break;
    }

      // calc amount of pad for interleave in this memory mode
    i = yuvpadbound[ memmode ];         // get pad boundry

    switch( i ) {
        case 2:
            ilpad = sendamount & 0x01;       // pad up to even number, pad is 0 or 1
            break;
        case 4:
            ilpad = 3 - ((sendamount + 3) & 0x03);
            break;
        default:
            ilpad = (i - 1) - ((sendamount + (i-1)) % i);
            break;
    }

      // calculate the width of a line in the picture image
    picwidth = (int)lpbmphdr->biWidth;

      // convert width from pixels-per-line to bytes-per-line
    switch( lpbmphdr->biBitCount ) {
        case 1:                     // monochrome bitmap 8 pix per byte
        wImageType = BM_DIB1P;
        picwidth = (picwidth + 7) >> 3;
        break;

        case 4:                     // 16 color bitmap 2 pix per byte
        wImageType = BM_DIB4P;
        picwidth = (picwidth + 1) >> 1;
        break;

        case 8:                     // 256 color bitmap 1 pix per byte
        wImageType = BM_DIB8P;
//-     picwidth = picwidth;        // NOP
        break;

        case 24:                    // 24 bit color values 3 bytes per pix
        wImageType = BM_DIB24;
        picwidth = picwidth * 3;
        break;
    }

      // pad bytes-per-line to dword boundary
    picwidth = (picwidth + 3) & 0xFFFC;

    if( lpbmphdr->biClrUsed != 0 ) {
        i = sizeof(RGBQUAD) * (int)lpbmphdr->biClrUsed;
    } else {
        switch( lpbmphdr->biBitCount ) {
            case 1:                     // monochrome bitmap
            i = sizeof(RGBQUAD) * 2;    // size of palette
            break;

            case 4:                     // 16 color bitmap
            i = sizeof(RGBQUAD) * 16;
            break;

            case 8:                     // 256 color bitmap
            i = sizeof(RGBQUAD) * 256;
            break;

            case 24:                    // 24 bit color values no palette
            i = 0;                      // palette size is zero
            break;
        }
    }

      // point to last data line in file (first line in image)
    lppic = (unsigned char huge *)lppal + i + ((height-1) * (unsigned long)picwidth);

      // Save register settings to regsave buffer
    for( i = 0; i < 240; ++i ) {
        regsave[i] = AV_GetRegister( i );
    }

    pbsigntoggle = !(AV_GetRegister(0x96) & 0x01);  // match chroma sign to decoder chroma sign

    AV_WaitVGARetrace();		//;;;;
    AV_SetRegister(0x30, 0xC0);         // Turn global reset on.
    AV_SetRegister(0x39, 0x0C);         // YUV, ISA input, progressive.
    AV_SetRegister(0x58, 0x01);         // Turn off vertical filtering.
    AV_SetRegister(0x38, 0x42);         // update registers in global reset
    AV_SetRegister(0x30, 0x44);         // Turn on tight decode mode, global reset off
    AV_UpdateShadowedRegisters();

    AV_SetRegister(0x3A, (sendamount+ilpad-1) & 0xFF);
    AV_SetRegister(0x3B, (sendamount+ilpad-1) >> 8);
    AV_SetRegister(0x3C, (height - 1) & 0xFF);
    AV_SetRegister(0x3D, (height - 1) >> 8);
    AV_SetRegister(0x40, 0);                // Turn off cropping.
    AV_SetRegister(0x41, 0);
    AV_SetRegister(0x44, (sendamount+ilpad) & 0xFF);
    AV_SetRegister(0x45, (sendamount+ilpad) >> 8);
    AV_SetRegister(0x48, 0);
    AV_SetRegister(0x49, 0);
    AV_SetRegister(0x4C, height & 0xFF);
    AV_SetRegister(0x4D, height >> 8);
    AV_SetRegister(0x50, 0);            // Turn off horizontal filtering.
    AV_SetRegister(0x54, 0);            // Turn off vertical scaling.
    AV_SetRegister(0x55, 0);
    AV_SetRegister(0x5C, 0);            // Turn off horizontal scaling.
    AV_SetRegister(0x5D, 0);
    AV_SetRegister(0x70, (int)(pos & 0xFF));
    AV_SetRegister(0x71, (int)((pos>>8) & 0xFF));
    AV_SetRegister(0x72, (int)((pos>>16) & 0x03));

    AV_SetRegister(0x38, 0x80);          // Reset input pipeline.
    AV_SetRegister(0x38, 0x42);          // Update input registers.

    while ((AV_GetRegister(0x39) & 0x80) == 0);  // Wait for ready.

    switch( lpbmphdr->biBitCount ) {
        case 1:                 // monochrome bitmap
        while( height-- ) {
            i = width;
            lppicsave = lppic;              // save start of line
            if( memmode > 2 ) {
                while( i > 0 ) {
                    j = (*lppic & 0xF0) >> 4;
                    tbuf[0] = monotrans [j] [0];
                    tbuf[1] = monotrans [j] [1];

                    palrgb2playyuv422( (char far *)tbuf, lppal,lpFrameBuffer );
                    palrgb2playyuv422( (char far *)&tbuf[1], lppal, lpFrameBuffer );

                    i -= 4;
                    if( i > 0 ) {
                        tbuf[0] = monotrans [(int)(*lppic & 0x0F)] [0];
                        tbuf[1] = monotrans [(int)(*lppic & 0x0F)] [1];

                        palrgb2playyuv422( (char far *)tbuf, lppal, lpFrameBuffer );
                        palrgb2playyuv422( (char far *)&tbuf[1], lppal, lpFrameBuffer );
                    }
                    i -= 4;
                    ++lppic;
                }
            } else {
                while( i > 0 ) {
                    j = (*lppic & 0xF0) >> 4;
                    tbuf[0] = monotrans [j] [0];
                    tbuf[1] = monotrans [j] [1];

                    palrgb2playyuv411( (char far *)tbuf, lppal, lpFrameBuffer );
                    i -= 4;

                    if( i > 0 ) {
                        tbuf[0] = monotrans [(int)(*lppic & 0x0F)] [0];
                        tbuf[1] = monotrans [(int)(*lppic & 0x0F)] [1];
                        palrgb2playyuv411( (char far *)tbuf, lppal, lpFrameBuffer );
                    }

                    i -= 4;
                    ++lppic;
                }
            }

            i = ilpad;                  // send zeros for pad up to interleave bound
            while( i-- ) {
                *lpFrameBuffer = 0;
            }

            lppic = lppicsave - picwidth;       // move source pointer back to start of next line
        }
        break;

        case 4:                 // 16 color bitmap
        while( height-- ) {
            i = width;
            lppicsave = lppic;              // save start of line
            if( memmode > 2 ) {
                while( i > 0 ) {
                    tbuf[0] = (int)(*lppic & 0x0F);
                    if( --i > 0 ) {
                        tbuf[0] += (int)(*lppic & 0xF0) << 4;
                        --i;
                    }
                    ++lppic;

                    palrgb2playyuv422( (char far *)tbuf, lppal, lpFrameBuffer );
                }
            } else {
                while( i > 0 ) {
                    tbuf[1] = 0;
                    tbuf[0] = (int)(*lppic & 0x0F);
                    if( --i > 0 ) {
                        tbuf[0] += (int)(*lppic & 0xF0) << 4;
                        if( --i > 0 ) {
                            ++lppic;
                            tbuf[1] = (int)(*lppic & 0x0F);
                        }
                        if( --i > 0 ) {
                            tbuf[1] += (int)(*lppic & 0xF0) << 4;
                            --i;
                        }
                    }
                    ++lppic;

                    palrgb2playyuv411( (char far *)tbuf, lppal, lpFrameBuffer );
                }
            }

            i = ilpad;                  // send zeros for pad up to interleave bound
            while( i-- ) {
                *lpFrameBuffer = 0;
            }

            lppic = lppicsave - picwidth;       // move source pointer back to start of next line
        }
        break;

        case 8:                     // 256 color bitmap
        while( height-- ) {
            i = width;
            lppicsave = lppic;              // save start of line
            if( memmode > 2 ) {
                while( i > 0 ) {
                    palrgb2playyuv422( lppic, lppal, lpFrameBuffer );

                    lppic += 2;
                    i -= 2;
                }
            } else {
                while( i > 0 ) {
                    palrgb2playyuv411( lppic, lppal, lpFrameBuffer );

                    lppic += 4;
                    i -= 4;
                }
            }

            i = ilpad;                  // send zeros for pad up to interleave bound
            while( i-- > 0 ) {
                *lpFrameBuffer = 0;
            }

            lppic = lppicsave - picwidth;       // move source pointer back to start of next line
        }
        break;

        case 24:                            // 24 bit color values
        while( height-- ) {
            i = width;
            lppicsave = lppic;              // save start of line
            if( memmode > 2 ) {
                while( i > 0 ) {
                    rgb2playyuv422( lppic, lpFrameBuffer );

                    lppic += 6;     // source moves up 2 pixels * 3 bytes per pix
                    i -= 2;         // width moves 2 pixels
                }

            } else {
                while( i > 0 ) {
                    rgb2playyuv411( lppic, (int far *)lpFrameBuffer );

                    lppic += 12;    // source moves up 4 pixels * 3 bytes per pix
                    i -= 4;         // width moves 4 pixels
                }
            }

            i = ilpad;                  // send zeros for pad up to interleave bound
            while( i-- ) {
                *lpFrameBuffer = 0;
            }

            lppic = lppicsave - picwidth;       // move source pointer back to start of next line
        }
        break;
    }

    AV_WaitVGARetrace();			//;;;;
    AV_SetRegister(0x30, 0xC0);
    AV_SetRegister(0x39, regsave[0x39]);
    AV_SetRegister(0x58, regsave[0x58]);
    AV_SetRegister(0x38, regsave[0x38] | 0x40);
    AV_SetRegister(0x30, regsave[0x30]);
    AV_UpdateShadowedRegisters();

    AV_SetRegister(0x40, regsave[0x40]);
    AV_SetRegister(0x41, regsave[0x41]);
    AV_SetRegister(0x44, regsave[0x44]);
    AV_SetRegister(0x45, regsave[0x45]);
    AV_SetRegister(0x48, regsave[0x48]);
    AV_SetRegister(0x49, regsave[0x49]);
    AV_SetRegister(0x4C, regsave[0x4C]);
    AV_SetRegister(0x4D, regsave[0x4D]);
    AV_SetRegister(0x50, regsave[0x50]);
    AV_SetRegister(0x54, regsave[0x54]);
    AV_SetRegister(0x55, regsave[0x55]);
    AV_SetRegister(0x5C, regsave[0x5C]);
    AV_SetRegister(0x5D, regsave[0x5D]);
    AV_SetRegister(0x70, regsave[0x70]);
    AV_SetRegister(0x71, regsave[0x71]);
    AV_SetRegister(0x72, regsave[0x72]);

    AV_SetRegister(0x38, regsave[0x38] | 0x40);
    AV_SetRegister(0x97, regsave[0x97] & 0x7F);
    AV_SetRegister(0x97, regsave[0x97] | 0x80);

    return TRUE;
}
