//////////////////////////////////////////////////////////////////////////
//   Module:    AVWIN8.C                                                //
//   Target:    AVWIN.DLL                                               //
//                                                                      //
//   Summary:   This module contains the still image playback and color //
//		space conversion routines for AVWIN.DLL.		//
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


int displaymmp( lppicdata, x, y )
unsigned char far *lppicdata;
int x, y;
{
    unsigned char huge *lppic;
    int i, width, height;
    int memmode, ileave, frwidth, frheight;
    long pos;
    unsigned int regsave[240];

    if( lppicdata == NULL ) return FALSE;

    // get image info
    lppic = (unsigned char huge *)lppicdata + 12;
    width = *((int far *)lppic);
    lppic += 2;
    height = *((int far *)lppic);
    lppic += 2;                 // now points to data

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

    wImageType = BM_YUV411;
    wImageWidth = width;                // save global values for SysMetrics call
    wImageHeight = height;

    pos = ((long)y * (frwidth/ileave)) + (x / ileave);   // calc position in video memory

      // Save register settings to regsave buffer
    for( i = 0; i < 240; ++i ) {
        regsave[i] = AV_GetRegister( i );
    }

    pbsigntoggle = !(AV_GetRegister(0x96) & 0x01);  // match chroma sign to decoder chroma sign

    AV_WaitVGARetrace();		//;;;;
    AV_SetRegister(0x30, 0xC0);          // Turn global reset on.
    AV_SetRegister(0x39, 0x0C);          // YUV, ISA input, progressive
    AV_SetRegister(0x58, 0x01);          // Turn off vertical filtering.
    AV_SetRegister(0x38, 0x42);          // Update input registers.
    AV_SetRegister(0x30, 0x44);          // Turn on tight decode mode, global reset off
    AV_UpdateShadowedRegisters();

    AV_SetRegister(0x3A, (width - 1) & 0xFF);
    AV_SetRegister(0x3B, (width - 1) >> 8);
    AV_SetRegister(0x3C, (height - 1) & 0xFF);
    AV_SetRegister(0x3D, (height - 1) >> 8);
    AV_SetRegister(0x40, 0x00);          // Turn off cropping.
    AV_SetRegister(0x41, 0x00);
    AV_SetRegister(0x44, width & 0xFF);
    AV_SetRegister(0x45, width >> 8);
    AV_SetRegister(0x48, 0x00);
    AV_SetRegister(0x49, 0x00);
    AV_SetRegister(0x4C, height & 0xFF);
    AV_SetRegister(0x4D, height >> 8);
    AV_SetRegister(0x50, 0x00);          // Turn off horizontal filtering.
    AV_SetRegister(0x54, 0x00);          // Turn off vertical scaling.
    AV_SetRegister(0x55, 0x00);
    AV_SetRegister(0x5C, 0x00);          // Turn off horizontal scaling.
    AV_SetRegister(0x5D, 0x00);
    AV_SetRegister(0x70, (int)(pos & 0xFF));
    AV_SetRegister(0x71, (int)((pos>>8) & 0xFF));
    AV_SetRegister(0x72, (int)((pos>>16) & 0x03));

    AV_SetRegister(0x38, 0x80);          // Reset input pipeline.
    AV_SetRegister(0x38, 0x42);          // Update input registers.

    while ((AV_GetRegister(0x39) & 0x80) == 0);  // Wait for ready.

    width /= 4;             // HOT! mmp width always multiple of 4 (?)
    if( memmode > 2 ) {
        while( height-- ) {
            i = width;
            while( i-- ) {
                pyuv2yuv422( (unsigned int far *)lppic, (unsigned int far *)lpFrameBuffer );
                lppic += 6;
            }
        }
    } else {
        while( height-- ) {
            i = width;
            while( i-- ) {
                pyuv2yuv411( (unsigned int far *)lppic, (unsigned int far *)lpFrameBuffer );
                lppic += 6;
            }
        }
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


// yuv4112rgb   Entry - pointer to Aura yuv411 uncompressed data (8 bytes)
//              Exit  - 12 bytes RGB stored, in BGR order for BMP files

void yuv4112rgb( yuvptr, rgbptr )
unsigned int far *yuvptr;
unsigned char huge *rgbptr;
{
    int i;
    int scaledu, scaledv, y, r, b;
    unsigned int g, tbuf[4];

    for( i = 0; i < 4; ++i ) {  // read to local buffer so that C optimizer
        tbuf[i] = *yuvptr++;    // won't mess up frame buffer reads
    }                           // (need to read exactly 4 words)

    _asm {
// ;build U and V values from scattered bits in 4 passed integers

        mov     ax,word ptr tbuf[0]     ;read first YUV value
        mov     bh,ah           ;collect U in bh
        mov     bl,ah           ;collect V in bl
        and     bx,0C03h
        shl     bx,2

        mov     ax,word ptr tbuf[2]
        mov     al,ah
        and     ax,0C03h
        or      bx,ax
        shl     bx,2

        mov     ax,word ptr tbuf[4]
        mov     al,ah
        and     ax,0C03h
        or      bx,ax
        shl     bl,2

        mov     ax,word ptr tbuf[6]
        mov     al,ah
        and     ax,0C03h
        shr     ah,2
        or      bx,ax

        cmp     capsigntoggle,0
        je      notog
        xor     bx,8080h
notog:

// ;U in bh, V in bl - now scale U and V for RGB calculations

        mov     al,bh           ;get U in al
        cbw
        mov     cx,456
        imul    cx
        mov     al,ah
        mov     ah,dl           ;longish shift right 8 (divide by 256)
        mov     scaledu,ax      ;save scaled U (signed int)

        mov     al,bl           ;al is unscaled V
        cbw
        mov     cx,361
        imul    cx
        mov     al,ah
        mov     ah,dl           ;shift right 8 (divide by 256)
        mov     scaledv,ax      ;v1 = scaled V

// scaled V and scaled U calculated (signed integers)
    }

    for( i = 0; i < 4; ++i ) {      // loop for 4 Y values
        y = tbuf[i] & 0xFF;

        b = y + scaledu;
        if( b < 0 ) b = 0;
        if( b > 255 ) b = 255;
        *rgbptr++ = b;              // store blue

        r = y + scaledv;
        if( r < 0 ) r = 0;
        if( r > 255 ) r = 255;

        // this calculation in asm to get precise control over signed/unsigned
        // manipulations (imul/mul, jg/ja, etc)
        _asm {
            mov     ax,word ptr b
            mov     dl,25           ;do part of green calc
            mul     dl              ;ax = B * 25
            mov     bx,ax           ;save for green calculation

            mov     dx, word ptr r
            mov     al,65           ;do part of green calc
            mul     dl              ;ax = R * 65
            add     bx,ax           ;add into green calculation

            mov     ax,word ptr y   ;get Y back
            xor     ah,ah
            mov     dh,218
            mul     dh              ;ax = Y * 218
            cmp     ax,bx
            ja      notneg3         ;unsigned compare, jump if ax is larger
            xor     ax,ax           ;use zero if negative number
            jmp     short store3

    notneg3:
            sub     ax,bx           ;subtract [.508R + .195B]
            shr     ax,7
            cmp     ax,255
            jna     store3          ;if not too big
            mov     ax,255
    store3: mov     word ptr g,ax   ;save green
        }

        *rgbptr++ = g;          // store green, red
        *rgbptr++ = r;
    }
}


//// yuv4112rgb   Entry - pointer to Aura yuv411 uncompressed data (8 bytes)
////              Exit  - 12 bytes RGB stored, in BGR order for BMP files
//
//void yuv4112rgb( yuvptr, rgbptr )
//unsigned int far *yuvptr;
//unsigned char far *rgbptr;
//{
//    int scaledu, scaledv;
//
//    _asm {
//        push    ds
//
//        lds     si,yuvptr
//        cld
//
//// ;build U and V values from scattered bits in 4 passed integers
//
//        lodsw                   ;read first YUV value
//        mov     bh,ah           ;collect U in bh
//        mov     bl,ah           ;collect V in bl
//        and     bx,0C03h
//        shl     bx,1
//        shl     bx,1
//
//        lodsw
//        mov     al,ah
//        and     ax,0C03h
//        or      bx,ax
//        shl     bx,1
//        shl     bx,1
//
//        lodsw
//        mov     al,ah
//        and     ax,0C03h
//        or      bx,ax
//        shl     bl,1
//        shl     bl,1
//
//        lodsw
//        mov     al,ah
//        and     ax,0C03h
//        shr     ah,1
//        shr     ah,1
//        or      bx,ax
//
//// ;U in bh, V in bl - now scale U and V for RGB calculations
//
//        mov     al,bh           ;get U in al
//        cbw
//        mov     cx,456
//        imul    cx
//        mov     al,ah
//        mov     ah,dl           ;shift right 8 (divide by 256)
//        mov     scaledu,ax      ;save scaled U (signed int)
//
//        mov     al,bl           ;al is unscaled V
//        cbw
//        mov     cx,361
//        imul    cx
//        mov     al,ah
//        mov     ah,dl           ;shift right 8 (divide by 256)
//        mov     scaledv,ax      ;v1 = scaled V
//
//// scaled V and scaled U calculated (signed integers)
//
//        lds     si,yuvptr       ;set up for Y loop
//        les     di,rgbptr
//        mov     cx,4            ;cl(cx) is counter for all 4 y values
//y4:
//        lodsw                   ;get Y value in al
//        mov     dh,al           ;save Y value in dh
//        xor     ah,ah           ;ax = Y
//        add     ax,scaledu      ;add scaled U
//        or      ax,ax
//        jge     notneg1
//        xor     ax,ax           ;if negative, use zero
//notneg1:
//        cmp     ax,255
//        jna     notbig1         ;if not too big
//        mov     al,255
//notbig1:
//        stosb                   ;store blue, truncate to 0 - 255
//        mov     dl,25           ;do part of green calc
//        mul     dl              ;ax = B * 25
//        mov     bx,ax           ;save for green calculation
//
//        mov     al,dh           ;get Y back from dh
//        xor     ah,ah
//        add     ax,scaledv      ;add scaled V
//        or      ax,ax
//        jge     notneg2         ;if not negative
//        xor     ax,ax           ;use zero if negative
//notneg2:
//        cmp     ax,255
//        jna     notbig2         ;if not too big
//        mov     al,255
//notbig2:
//        mov     dl,al           ;save red value in dl
//        mov     al,65           ;do part of green calc
//        mul     dl              ;ax = R * 65
//        add     bx,ax           ;add into green calculation
//
//        mov     al,dh           ;get Y back
//        xor     ah,ah
//        mov     dh,218
//        mul     dh              ;ax = Y * 218
//        cmp     ax,bx
//        ja      notneg3         ;unsigned compare, jump if ax is larger
//        xor     al,al           ;use zero if negative number
//        jmp     short store3
//
//notneg3:
//        sub     ax,bx           ;subtract [.508R + .195B]
//        shr     ax,7
//        cmp     ax,255
//        jna     store3          ;if not too big
//        mov     al,255
//store3: stosb                   ;store green
//
//        mov     al,dl           ;get red back
//        stosb                   ;store red
//        loop    y4
//
//        pop     ds
//    }
//}


// yuv4222rgb   Entry - pointer to Aura playback uncompressed yuv422 data (4 bytes)
//              Exit  - 6 bytes RGB stored, in BGR order for BMP files

void yuv4222rgb( yuvptr, rgbptr )
unsigned int far *yuvptr;
unsigned char huge *rgbptr;
{
    int i;
    int scaledu, scaledv, y, r, b;
    unsigned int g, tbuf[2];

    tbuf[0] = *yuvptr++;        // read to local buffer so that C optimizer
    tbuf[1] = *yuvptr++;        // won't mess up frame buffer reads
                                // (need to read exactly 2 words)

    _asm {
// ;calculate scaled V and scaled U (signed integers)

        mov     al,byte ptr tbuf[1]     ;get U in al

        cmp     capsigntoggle,0
        je      notog1
        xor     al,80h
notog1:

        cbw
        mov     cx,456
        imul    cx
        mov     al,ah
        mov     ah,dl           ;shift right 8 (divide by 256)
        mov     scaledu,ax      ;save scaled U (signed int)

        mov     al,byte ptr tbuf[3]     ;get V in al
        cmp     capsigntoggle,0
        je      notog2
        xor     al,80h
notog2:
        cbw
        mov     cx,361
        imul    cx              ;signed answer in dx:ax
        mov     al,ah
        mov     ah,dl           ;shift right 8 (divide by 256)
        mov     scaledv,ax      ;save scaled V
    };

    for( i = 0; i < 2; ++i ) {
        y = tbuf[i];

        _asm {
            mov     ax,word ptr y   ;get Y in al
            mov     dh,al           ;save Y value in dh
            xor     ah,ah           ;ax = Y
            add     ax,scaledu      ;add scaled U
            or      ax,ax
            jge     notneg1
            xor     ax,ax           ;if negative, use zero
    notneg1:
            cmp     ax,255
            jna     notbig1         ;if not too big
            mov     ax,255
    notbig1:
            mov     word ptr b,ax   ;save blue
            mov     dl,25           ;do part of green calc
            mul     dl              ;ax = B * 25
            mov     bx,ax           ;save for green calculation

            mov     al,dh           ;get Y back from dh
            xor     ah,ah
            add     ax,scaledv      ;add scaled V
            or      ax,ax
            jge     notneg2         ;if not negative
            xor     ax,ax           ;use zero if negative
    notneg2:
            cmp     ax,255
            jna     notbig2         ;if not too big
            mov     al,255
    notbig2:
            mov     dl,al           ;save red value in dl
            mov     al,65           ;do part of green calc
            mul     dl              ;ax = R * 65
            add     bx,ax           ;add into green calculation

            mov     al,dh           ;get Y back
            xor     ah,ah
            mov     dh,218
            mul     dh              ;ax = Y * 218
            cmp     ax,bx
            ja      notneg3         ;unsigned compare, jump if ax is larger
            xor     ax,ax           ;use zero if negative number
            jmp     short store3

    notneg3:
            sub     ax,bx           ;subtract [.508R + .195B]
            shr     ax,7
            cmp     ax,255
            jna     store3          ;if not too big
            mov     ax,255
    store3: mov     word ptr g,ax   ;save green

            mov     al,dl           ;get red back
            mov     word ptr r,ax   ;save red
        }

        *rgbptr++ = b;
        *rgbptr++ = g;
        *rgbptr++ = r;
    }
}


//// yuv4222rgb   Entry - pointer to Aura playback uncompressed yuv422 data (4 bytes)
////              Exit  - 6 bytes RGB stored, in BGR order for BMP files
//
//void yuv4222rgb( yuvptr, rgbptr )
//unsigned char far *yuvptr;
//unsigned char huge *rgbptr;
//{
//    int scaledu, scaledv;
//
//    _asm {
//        push    ds
//
//        lds     si,yuvptr
//        cld
//
//// ;calculate scaled V and scaled U (signed integers)
//
//        inc     si              ;skip Y0
//        lodsb                   ;get U in al
//        cbw
//        mov     cx,456
//        imul    cx
//        mov     al,ah
//        mov     ah,dl           ;shift right 8 (divide by 256)
//        mov     scaledu,ax      ;save scaled U (signed int)
//
//        inc     si              ;skip Y1
//        lodsb                   ;get V in al
//        cbw
//        mov     cx,361
//        imul    cx              ;signed answer in dx:ax
//        mov     al,ah
//        mov     ah,dl           ;shift right 8 (divide by 256)
//        mov     scaledv,ax      ;v1 = scaled V
//
//        lds     si,yuvptr       ;set up for Y loop
//        les     di,rgbptr
//        mov     cx,2            ;cl(cx) is counter for Y values
//y2:
//        lodsw                   ;get Y value in al
//        mov     dh,al           ;save Y value in dh
//        xor     ah,ah           ;ax = Y
//        add     ax,scaledu      ;add scaled U
//        or      ax,ax
//        jge     notneg1
//        xor     ax,ax           ;if negative, use zero
//notneg1:
//        cmp     ax,255
//        jna     notbig1         ;if not too big
//        mov     al,255
//notbig1:
//        stosb                   ;store blue, truncate to 0 - 255
//        cmp     di,0
//        jne     nz1
//        mov     di,es
//        add     di,8
//        mov     es,di
//        xor     di,di
//nz1:    mov     dl,25           ;do part of green calc
//        mul     dl              ;ax = B * 25
//        mov     bx,ax           ;save for green calculation
//
//        mov     al,dh           ;get Y back from dh
//        xor     ah,ah
//        add     ax,scaledv      ;add scaled V
//        or      ax,ax
//        jge     notneg2         ;if not negative
//        xor     ax,ax           ;use zero if negative
//notneg2:
//        cmp     ax,255
//        jna     notbig2         ;if not too big
//        mov     al,255
//notbig2:
//        mov     dl,al           ;save red value in dl
//        mov     al,65           ;do part of green calc
//        mul     dl              ;ax = R * 65
//        add     bx,ax           ;add into green calculation
//
//        mov     al,dh           ;get Y back
//        xor     ah,ah
//        mov     dh,218
//        mul     dh              ;ax = Y * 218
//        cmp     ax,bx
//        ja      notneg3         ;unsigned compare, jump if ax is larger
//        xor     al,al           ;use zero if negative number
//        jmp     short store3
//
//y3:     jmp     short y2
//
//notneg3:
//        sub     ax,bx           ;subtract [.508R + .195B]
//        shr     ax,7
//        cmp     ax,255
//        jna     store3          ;if not too big
//        mov     al,255
//store3: stosb                   ;store green
//        cmp     di,0
//        jne     nz2
//        mov     di,es
//        add     di,8
//        mov     es,di
//        xor     di,di
//nz2:
//        mov     al,dl           ;get red back
//        stosb                   ;store red
//        cmp     di,0
//        jne     nz3
//        mov     di,es
//        add     di,8
//        mov     es,di
//        xor     di,di
//nz3:
//        loop    y3
//
//        pop     ds
//    }
//}


// pyuv2yuv411  Entry - far pointer to packed yuv data (6 bytes)
//              Exit - 4 words unpacked yuv stored at far pointer

void pyuv2yuv411( pyuv, yuv )
unsigned int far *pyuv;
unsigned int far *yuv;
{

    _asm {
        mov     cx,pbsigntoggle
        push    ds

        lds     si,pyuv
        les     di,yuv
        cld

        lodsw
        jcxz    notog
        xor     ax,0A000h
notog:
        mov     bh,al
        shr     ax,4
        and     ax,0FFFh
        stosw

        lodsw
        xchg    ah,al
        xchg    ah,bh
        and     ax,0FFFh
        stosw

        lodsw
        xchg    ax,bx
        mov     al,bh
        shr     ax,4
        and     ax,0FFFh
        stosw

        mov     ax,bx
        and     ax,0FFFh
        stosw

        pop     ds
    }
}


// pyuv2yuv422  Entry - far pointer to packed YUV data (MMP file - 6 bytes)
//              Exit - 4 words unpacked YUV stored at far pointer

void pyuv2yuv422( pyuv, yuv )
unsigned int far *pyuv;
unsigned int far *yuv;
{

    _asm {
        mov     cx,pbsigntoggle
        push    ds

        lds     si,pyuv
        les     di,yuv
        cld

        lodsw
        jcxz    notog
        xor     ax,0A000h
notog:
        mov     bl,ah
        shl     bx,2                    ;Shift U7, U6 into bh
        mov     cl,bl
        shl     cx,2                    ;shift V7, V6 into ch
        shl     ax,4
        mov     dl,ah                   ;Y0 in dl
        mov     bl,al
        shl     bx,2                    ;shift U5, U4 into bh
        mov     cl,bl
        shl     cx,2                    ;shift V5, V4 into ch

        lodsw
        mov     dh,ah                   ;Y1 in dh
        mov     bl,al
        shl     bx,2                    ;shift U3, U2 into bh
        mov     cl,bl
        shl     cx,2                    ;shift V3, V2 into ch

        lodsw
        mov     bl,al                   ;save Y3 in bl
        shr     ax,4
        xchg    bl,al                   ;get UV0, 1 in bl, Y3 in al
        or      cl,ah                   ;combine Y2 in cl
        shl     bx,2                    ;shift U1, U0 into bh
        xchg    cl,bl                   ;saving Y2 in bl
        shl     cx,2                    ;shift V1, V0 into ch

        mov     ah,bh
        xchg    al,dl                   ;ax is U and Y0 (save Y3 in dl)
        stosw
        mov     ah,ch
        mov     al,dh                   ;ax is V and Y1
        stosw
        mov     ah,bh
        mov     al,bl                   ;ax is U and Y2
        stosw
        mov     ah,ch
        mov     al,dl                   ;ax is V and Y3
        stosw

        pop     ds
    }
}


// convert from paletized RGB to Aura playback YUV411

void palrgb2playyuv411( lppic, lppal, w )
unsigned char far *lppic;
RGBQUAD far *lppal;
unsigned int far *w;
{
    unsigned int i, u, v;
    int y[4];
    register unsigned int j;

    u = 0;
    v = 0;

    for( i = 0; i < 4; ++i ) {
        j = *lppic;
        y[i] = ( (lppal + j)->rgbRed * 30
                    + (lppal + j)->rgbGreen * 59
                            + (lppal + j)->rgbBlue * 11
                                    + 50 ) / 100;

        u += ( (((int)(lppal + j)->rgbBlue - y[i]) * 127) / 226 );
        v += ( (((int)(lppal + j)->rgbRed  - y[i]) * 127) / 179 );

        ++lppic;
    }

    u /= 4;
    v /= 4;

    if( pbsigntoggle ) {
        u ^= 0x80;
        v ^= 0x80;
    }

    *w++ = (((int)u & 0xC0) <<  4) + (((int)v & 0xC0) <<  2) +  y[0];
    *w++ = (((int)u & 0x30) <<  6) + (((int)v & 0x30) <<  4) +  y[1];
    *w++ = (((int)u & 0x0C) <<  8) + (((int)v & 0x0C) <<  6) +  y[2];
    *w   = (((int)u & 0x03) << 10) + (((int)v & 0x03) <<  8) +  y[3];
}


// convert from paletized RGB to Aura playback YUV422

void palrgb2playyuv422( lppic, lppal, w )
unsigned char far *lppic;
RGBQUAD far *lppal;
unsigned int far *w;
{
    unsigned int u, v;
    int y0, y1;
    register unsigned int j;

    u = 0;
    v = 0;

    j = *lppic;
    y0 = ( (lppal + j)->rgbRed * 30
                + (lppal + j)->rgbGreen * 59
                        + (lppal + j)->rgbBlue * 11
                                + 50 ) / 100;

    u += ( (((int)(lppal + j)->rgbBlue - y0) * 127) / 226 );
    v += ( (((int)(lppal + j)->rgbRed  - y0) * 127) / 179 );

    ++lppic;

    j = *lppic;
    y1 = ( (lppal + j)->rgbRed * 30
                + (lppal + j)->rgbGreen * 59
                        + (lppal + j)->rgbBlue * 11
                                + 50 ) / 100;

    u += ( (((int)(lppal + j)->rgbBlue - y1) * 127) / 226 );
    v += ( (((int)(lppal + j)->rgbRed  - y1) * 127) / 179 );

    u /= 2;
    v /= 2;

    if( pbsigntoggle ) {
        u ^= 0x80;
        v ^= 0x80;
    }

    *w++ = y0 + (u << 8);
    *w   = y1 + (v << 8);
}


// rgb2playyuv411  Entry - pointer to pic data (3 bytes/pix = 12 bytes)
//              Exit - 4 words of Aura playback YUV411

void rgb2playyuv411( lppic, w )
unsigned char huge *lppic;
unsigned int far *w;
{
    unsigned int i, u, v, r, g, b;
    int y[4];

    u = 0;
    v = 0;

    for( i = 0; i < 4; ++i ) {
        b = *lppic++;
        g = *lppic++;
        r = *lppic++;

        y[i] = ( r * 30 + g * 59 + b * 11 + 50 ) / 100;

        u += ( (((int)b - y[i]) * 127) / 226 );
        v += ( (((int)r - y[i]) * 127) / 179 );
    }

    u /= 4;
    v /= 4;

    if( pbsigntoggle ) {
        u ^= 0x80;
        v ^= 0x80;
    }

    *w++ = (((int)u & 0xC0) <<  4) + (((int)v & 0xC0) <<  2) +  y[0];
    *w++ = (((int)u & 0x30) <<  6) + (((int)v & 0x30) <<  4) +  y[1];
    *w++ = (((int)u & 0x0C) <<  8) + (((int)v & 0x0C) <<  6) +  y[2];
    *w   = (((int)u & 0x03) << 10) + (((int)v & 0x03) <<  8) +  y[3];
}


// rgb2playyuv422   Entry - pointer to pic data (3 bytes/pix = 6 bytes)
//                  Exit - 2 words of Aura playback YUV422

void rgb2playyuv422( lppic, w )
unsigned char huge *lppic;
unsigned int far *w;
{
    unsigned int u, v, r, g, b;
    int y0, y1;

    u = 0;
    v = 0;

    b = *lppic++;
    g = *lppic++;
    r = *lppic++;

    y0 = ( r * 30 + g * 59 + b * 11 + 50 ) / 100;

    u += ( (((int)b - y0) * 127) / 226 );
    v += ( (((int)r - y0) * 127) / 179 );

    b = *lppic++;
    g = *lppic++;
    r = *lppic++;

    y1 = ( r * 30 + g * 59 + b * 11 + 50 ) / 100;

    u += ( (((int)b - y1) * 127) / 226 );
    v += ( (((int)r - y1) * 127) / 179 );

    u /= 2;
    v /= 2;

    if( pbsigntoggle ) {
        u ^= 0x80;
        v ^= 0x80;
    }

    *w++ = y0 + (u << 8);
    *w   = y1 + (v << 8);
}


