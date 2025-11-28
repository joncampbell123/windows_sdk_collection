/*;;--------------------------------------------------------------------------;
;;
;;  File: timing.asm
;;
;;  Description:
;;      This asm file is various methods of memory copies
;;      that copy the same data over and over for timing
;;      tests. They all assume DWORD alignment.
;;      They all take:
;;              Source. The source buffer to copy from
;;              Dest.   The dest buffer to copy to.
;;              Height. The hight of both the source and dest buffers
;;              Width.  The width of both the source and dest buffers
;;              Pitch.  The pitch of the dest buffer ONLY. The source
;;                      buffer assumes the pitch = width.
;;              Count.  The number of times you want to do the copy.
;;      They all return:
;;              The number of bytes copied
;;
;;
;;  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;;  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;;  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;;  PURPOSE.
;;
;;---------------------------------------------------------------------------
;;
;;  Copyright (c) 1994 - 1997 Microsoft Corporation.  All Rights Reserved.
;;
;;---------------------------------------------------------------------------*/

#include "windows.h"

_cdecl VertMemSto_Pitch(DWORD pSource, DWORD pDest, DWORD Height, DWORD sWidth, DWORD Pitch, DWORD Count);
_cdecl DwordMemCopy_Pitch(DWORD pSource, DWORD pDest, DWORD Height, DWORD sWidth, DWORD Pitch, DWORD Count);
_cdecl ByteMemCopy_Pitch(DWORD pSource, DWORD pDest, DWORD Height, DWORD sWidth, DWORD Pitch, DWORD Count);
_cdecl DwordMemFill_Pitch(DWORD pSource, DWORD pDest, DWORD Height, DWORD sWidth, DWORD Pitch, DWORD Count);
_cdecl ByteMemFill_Pitch(DWORD pSource, DWORD pDest, DWORD Height, DWORD sWidth, DWORD Pitch, DWORD Count);

// fill a verticale byte colum of memory (either system or video)

VertMemSto_Pitch(DWORD pSource, DWORD pDest, DWORD Height, DWORD sWidth, DWORD Pitch, DWORD Count)
{
	_asm
	{
        mov     ebx, pSource    ;; this is only here so the assembler doesn't tell me about un-used parameters. (I'm to lazy to figure out how to turn the warning off)

        mov     ebx, Count      ;; ebx = number of times to copy the buffers
        mov     eax, Pitch      ;; eax = the pitch of the dest

top_o_loop1:
        mov     edx, pDest      ;; edx = pointer to the dest
        mov     ecx, sWidth 
VerLine1:
        mov     edi, edx        ;; load the pDest       
        mov     esi, Height
pixel1:
        mov     byte ptr[edi],0 ;; store the byte (zeros are nice)
        add     edi, eax        ;; skip to the next line
        dec     esi             ;; are we done with this colum?
        jnz     pixel1

        inc     edx             ;; we did the colum. increment the pDest by one
        dec     ecx             ;; are we done with all the colums?
        jnz     VerLine1

        dec     ebx             ;; did we do it the correct number of times?
        jnz     top_o_loop1

        mov     eax, Count      ;; return the number of bytes we filled
        imul    eax, sWidth
        imul    eax, Height
	}
}

// Copy from system memory to Video memory and
// cope with pitch.

DwordMemCopy_Pitch(DWORD pSource, DWORD pDest, DWORD Height, DWORD sWidth, DWORD pitch, DWORD Count)
{
	_asm
	{
        mov     ebx, Count      ;; do the copy this many times
        mov     edx, sWidth     ;; set up the number of Dwords per scanline
        shr     edx, 2

top_o_loop2:
        push    ebx				;; every screen
        mov     ebx, pDest
        mov     esi, pSource    ;; the source is linear data
        mov     eax, Height     ;; number of lines
        push    ebp
        mov     ebp, pitch
scan_line2:
        mov     edi, ebx        ;; get a dest pointer to this scan line
        mov     ecx, edx        ;; reset our dword count
        rep     movsd
        add     ebx, ebp        ;; add in the offset to the next scan line

        dec     eax
        jnz     scan_line2

        pop     ebp             ;; we've done a whole screen
        pop     ebx
        dec     ebx             ;; do another screen (till we're done)
        jnz     top_o_loop2

        mov     eax, edx        ;; the width
        shl     eax, 2          ;; in bytes
        imul    eax, Height     ;; * height
        imul    eax, Count      ;; * number of screens we copied
	}
}

// Copy from system memory to Video memory (in BYTES)
// and cope with pitch.

ByteMemCopy_Pitch(DWORD pSource, DWORD pDest, DWORD Height, DWORD sWidth, DWORD pitch, DWORD Count)
{
	_asm
	{
        mov     ebx, Count      ;; do the copy this many times
        mov     edx, sWidth     ;; set up the number of bytes per scanline

top_o_loop3:
        push    ebx             ;; every screen
        mov     ebx, pDest
        mov     esi, pSource    ;; the source is linear data
        mov     eax, Height     ;; number of lines
        push    ebp
        mov     ebp, pitch
scan_line3:
        mov     edi, ebx        ;; get a dest pointer to this scan line
        mov     ecx, edx        ;; reset our dword count
        rep     movsb
        add     ebx, ebp        ;; add in the offset to the next scan line

        dec     eax
        jnz     scan_line3

        pop     ebp             ;; we've done a whole screen
        pop     ebx
        dec     ebx             ;; do another screen (till we're done)
        jnz     top_o_loop3

        mov     eax, edx        ;; the width
        imul    eax, Height     ;; * height
        imul    eax, Count      ;; * number of screens we copied
	}
}

// fill memory (video or system) with 0s (DOWRD fill)

DwordMemFill_Pitch(DWORD pSource, DWORD pDest, DWORD Height, DWORD sWidth, DWORD Pitch, DWORD Count)
{
	_asm
	{
        mov     ebx, pSource    ;; this is only here so the assembler doesn't tell me about un-used parameters. (I'm to lazy to figure out how to turn the warning off)
        mov     ebx, Count              
        xor     eax, eax
        mov     edx, sWidth     ;; we want a dword count
        shr     edx, 2

screen4:
        mov     esi, pDest      ;; re-load the dest
        push    ebx
        mov     ebx, Height     ;; re-load the height
        push    ebp
        mov     ebp, Pitch      ;; put this in a register
        
line4:
        mov     edi, esi        ;; get the new line
        mov     ecx, edx        ;; load the count
        rep     stosd           ;; store the data (eax = 0)

        add     esi, ebp        ;; add the pitch into the pDest

        dec     ebx             ;; are we done with the screen?
        jnz     line4

        pop     ebp
        pop     ebx
        dec     ebx             ;; did we do it the requested number of times?
        jnz     screen4

        mov     eax, Count      ;; return how many bytes we filled      
        imul    eax, sWidth
        imul    eax, Height
	}
}

// fill memory (video or system) with 0s (DOWRD fill)

// same thing as above, just do it in bytes.
// only 2 lines change. Whata waste of code space.
// good thing it's only a test app

ByteMemFill_Pitch(DWORD pSource, DWORD pDest, DWORD Height, DWORD sWidth, DWORD Pitch, DWORD Count)
{
	_asm
	{
        mov     ebx, pSource    ;; this is here so masm wont choke.

        mov     ebx, Count
        xor     eax, eax
        mov     edx, sWidth

screen5:
        mov     esi, pDest
        push    ebx
        mov     ebx, Height
        push    ebp
        mov     ebp, Pitch
        
line5:
        mov     edi, esi
        mov     ecx, edx
        rep     stosb

        add     esi, ebp

        dec     ebx
        jnz     line5

        pop     ebp
        pop     ebx
        dec     ebx
        jnz     screen5

        mov     eax, Count
        imul    eax, sWidth
        imul    eax, Height
	}
}
