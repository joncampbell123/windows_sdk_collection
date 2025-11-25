;;--------------------------------------------------------------------------;
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
;;  Copyright (c) 1994 - 1995 Microsoft Corporation.  All Rights Reserved.
;;
;;---------------------------------------------------------------------------
.386
.MODEL FLAT




.CODE


;; fill a verticale byte colum of memory (either system or video)

VertMemSto_Pitch PROC C PUBLIC Uses ESI EDI EBX EDX ECX,
        pSource:DWORD, \        ;; this isn't used, but it's gotta be here so I can
        pDest:DWORD, \          ;; use it in a jump table
        Height:DWORD, \
        sWidth:DWORD, \
        Pitch:DWORD, \
        Count:DWORD

        mov     ebx, pSource    ;; this is only here so the assembler doesn't tell
                                ;; me about un-used parameters. (I'm to lazy to
                                ;; figure out how to turn the warning off)

        mov     ebx, Count      ;; ebx = number of times to copy the buffers
        mov     eax, Pitch      ;; eax = the pitch of the dest

top_o_loop:
        mov     edx, pDest      ;; edx = pointer to the dest
        mov     ecx, sWidth 
VerLine:
        mov     edi, edx        ;; load the pDest       
        mov     esi, Height
pixel:
        mov     byte ptr [edi], 0       ;; store the byte (0's are nice)
        add     edi, eax        ;; skip to the next line
        dec     esi             ;; are we done with this colum?
        jnz     pixel

        inc     edx             ;; we did the colum. increment the pDest by one
        dec     ecx             ;; are we done with all the colums?
        jnz     VerLine

        dec     ebx             ;; did we do it the correct number of times?
        jnz     top_o_loop

        mov     eax, Count      ;; return the number of bytes we filled
        imul    eax, sWidth
        imul    eax, Height
        
        ret
VertMemSto_Pitch ENDP

; Copy from system memory to Video memory and
; cope with pitch.

DwordMemCopy_Pitch PROC C PUBLIC USES ESI EDI EBX ECX EDX, \
        pSource:DWORD, \
        pDest:DWORD, \
        Height:DWORD, \
        sWidth:DWORD, \
        Pitch:DWORD, \
        Count:DWORD

        mov     ebx, Count      ;; do the copy this many times
        mov     edx, sWidth     ;; set up the number of Dwords per scanline
        shr     edx, 2

top_o_loop:                     ;; every screen
        push    ebx
        mov     ebx, pDest
        mov     esi, pSource    ;; the source is linear data
        mov     eax, Height     ;; number of lines
        push    ebp
        mov     ebp, pitch
scan_line:
        mov     edi, ebx        ;; get a dest pointer to this scan line
        mov     ecx, edx        ;; reset our dword count
        rep     movsd
        add     ebx, ebp        ;; add in the offset to the next scan line

        dec     eax
        jnz     scan_line

        pop     ebp             ;; we've done a whole screen
        pop     ebx
        dec     ebx             ;; do another screen (till we're done)
        jnz     top_o_loop

        mov     eax, edx        ;; the width
        shl     eax, 2          ;; in bytes
        imul    eax, Height     ;; * height
        imul    eax, Count      ;; * number of screens we copied

        ret

DwordMemCopy_Pitch ENDP

; Copy from system memory to Video memory (in BYTES)
; and cope with pitch.

ByteMemCopy_Pitch PROC C PUBLIC USES ESI EDI EBX ECX EDX, \
        pSource:DWORD, \
        pDest:DWORD, \
        Height:DWORD, \
        sWidth:DWORD, \
        Pitch:DWORD, \
        Count:DWORD

        mov     ebx, Count      ;; do the copy this many times
        mov     edx, sWidth     ;; set up the number of bytes per scanline

top_o_loop:                     ;; every screen
        push    ebx
        mov     ebx, pDest
        mov     esi, pSource    ;; the source is linear data
        mov     eax, Height     ;; number of lines
        push    ebp
        mov     ebp, pitch
scan_line:
        mov     edi, ebx        ;; get a dest pointer to this scan line
        mov     ecx, edx        ;; reset our dword count
        rep     movsb
        add     ebx, ebp        ;; add in the offset to the next scan line

        dec     eax
        jnz     scan_line

        pop     ebp             ;; we've done a whole screen
        pop     ebx
        dec     ebx             ;; do another screen (till we're done)
        jnz     top_o_loop

        mov     eax, edx        ;; the width
        imul    eax, Height     ;; * height
        imul    eax, Count      ;; * number of screens we copied

        ret

ByteMemCopy_Pitch ENDP

;; fill memory (video or system) with 0s (DOWRD fill)

DwordMemFill_Pitch PROC C PUBLIC USES ESI EDI EBX ECX EDX, \
        pSource:DWORD, \
        pDest:DWORD, \
        Height:DWORD, \
        sWidth:DWORD, \
        Pitch:DWORD, \
        Count:DWORD

        mov     ebx, pSource    ;; this is only here so the assembler doesn't tell
                                ;; me about un-used parameters. (I'm to lazy to
                                ;; figure out how to turn the warning off)
        mov     ebx, Count              
        xor     eax, eax
        mov     edx, sWidth     ;; we want a dword count
        shr     edx, 2

screen:
        mov     esi, pDest      ;; re-load the dest
        push    ebx
        mov     ebx, Height     ;; re-load the height
        push    ebp
        mov     ebp, Pitch      ;; put this in a register
        
line:
        mov     edi, esi        ;; get the new line
        mov     ecx, edx        ;; load the count
        rep     stosd           ;; store the data (eax = 0)

        add     esi, ebp        ;; add the pitch into the pDest

        dec     ebx             ;; are we done with the screen?
        jnz     line

        pop     ebp
        pop     ebx
        dec     ebx             ;; did we do it the requested number of times?
        jnz     screen

        mov     eax, Count      ;; return how many bytes we filled      
        imul    eax, sWidth
        imul    eax, Height

        ret
DwordMemFill_Pitch ENDP

;; fill memory (video or system) with 0s (DOWRD fill)

;; same thing as above, just do it in bytes.
;; only 2 lines change. Whata waste of code space.
;; good thing it's only a test app

ByteMemFill_Pitch PROC C PUBLIC USES ESI EDI EBX ECX EDX, \
        pSource:DWORD, \
        pDest:DWORD, \
        Height:DWORD, \
        sWidth:DWORD, \
        Pitch:DWORD, \
        Count:DWORD

        mov     ebx, pSource    ;; this is here so masm wont choke.

        mov     ebx, Count
        xor     eax, eax
        mov     edx, sWidth

screen:
        mov     esi, pDest
        push    ebx
        mov     ebx, Height
        push    ebp
        mov     ebp, Pitch
        
line:
        mov     edi, esi
        mov     ecx, edx
        rep     stosb

        add     esi, ebp

        dec     ebx
        jnz     line

        pop     ebp
        pop     ebx
        dec     ebx
        jnz     screen

        mov     eax, Count
        imul    eax, sWidth
        imul    eax, Height

        ret
ByteMemFill_Pitch ENDP
END


