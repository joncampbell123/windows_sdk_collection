        TITLE MAPA.ASM
        page 60,132

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; MAPA.ASM - Convert from YUV space to 8-bit palettized or 16-bit RGB FAST!!!
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;   PURPOSE.
;
;   Copyright (c) 1992, 1993  Microsoft Corporation.  All Rights Reserved.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

?PLM=1	    ; PASCAL Calling convention is DEFAULT
?WIN=0      ; Windows calling convention

        .xlist
        include cmacros.inc
        .list

        externW         _gwWidthBytes

ALIGN4  macro

        if @Version ge 600
            align 2
        else
            align 4
        endif

        endm

; -------------------------------------------------------
;               DATA SEGMENT DECLARATIONS
; -------------------------------------------------------

ifndef SEGNAME
    SEGNAME equ <_TEXT>
endif

createSeg %SEGNAME, CodeSeg, word, public, CODE

.386

sBegin	CodeSeg
        assumes cs,CodeSeg
        assumes ds,nothing
        assumes es,nothing

UV65 dw 0, 8h, 10h, 18h, 100h, 108h, 110h, 118h, 200h, 208h, 210h, 218h, 300h, 308h, 310h, 318h
UV43 dw 0, 2h, 04h, 06h, 040h, 042h, 044h, 046h, 080h, 082h, 084h, 086h, 0c0h, 0c2h, 0c4h, 0c6h   
UV21 dw 0, 0h, 01h, 01h,  00h,  00h,  01h,  01h,  20h,  20h,  21h,  21h, 020h, 020h, 021h, 021h   

;+======================================================================+
;|									|
;|		+=================================================+	|
;|   Word 0	| 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0 |	|
;|		| U6 U5 V6 V5 -- -- -- -- Y6 Y5 Y4 Y3 Y2 Y1 Y0 -- |	|
;|		+=================================================+	|
;|		+=================================================+	|
;|   Word 1	| 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0 |	|
;|		| U4 U3 V4 V3 -- -- -- -- Y6 Y5 Y4 Y3 Y2 Y1 Y0 -- |	|
;|		+=================================================+	|
;|		+=================================================+	|
;|   Word 2	| 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0 |	|
;|		| U2 U1 V2 V1 -- -- -- -- Y6 Y5 Y4 Y3 Y2 Y1 Y0 -- |	|
;|		+=================================================+	|
;|		+=================================================+	|
;|   Word 3	| 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0 |	|
;|		| U0 -- V0 -- -- -- -- -- Y6 Y5 Y4 Y3 Y2 Y1 Y0 -- |	|
;|		+=================================================+	|
;+======================================================================+
; Translates from the above format to palettized 8 bit
; Simultaneously flips the result vertically into a DIB format

cProc   mapUnpackedYUVto8,<NEAR, PASCAL, PUBLIC>,<ds>
        parmD   pDst            ; Destination
        parmD   pSrc            ; Source data
	parmD	pXlat           ; Pointer to YUV555 xlate table
	parmW	wWidth          ; Width in pixels
	parmW	wHeight         ; Height in pixels
	parmW	dxSrc           ; Width of source in bytes

	localV	luma,8          ; will contain 5 MSBs of luma
	localW	wTempWidth      ; local copy of width
	localD	dwInc           ; Increment to next line
cBegin
	cld
	push	esi
	push	edi

	movzx	edi, di         ; zero the high words
	movzx	esi, si

        movzx   eax, dxSrc       ; calc increment to next scanline
        movzx   edx, wWidth
        shl     edx, 1
        add     eax, edx
        mov     dwInc, eax

        movzx   edi, dxSrc      ;Bytes per row in frame buffer (or source)
        movzx   eax, wHeight    ;Change Src pointer to bottom of frame
        dec     eax
        mul     edi
	lds	si, pSrc
        add     eax, esi        
        mov     esi, eax

	lfs	bx, pXlat
        les     di, pDst

        xor     ax, ax          ;zero hi byte of luma array
        mov     luma[0], ax
        mov     luma[2], ax
        mov     luma[4], ax
        mov     luma[6], ax

ifdef DEBUG
        or      bx,bx       ; pXlat must be 16:0 !!!
        jz      @f
        int 3
        int 3
@@:
endif
        mov     dx, wWidth
        shr     dx,2         ; 4 pixels processed in the inner loop
        mov     wWidth,dx

        mov     ecx, 0f0f8f0f8h  ; chroma and luma mask

OuterYUVto8Loop:
        mov     ax, wWidth
        mov     wTempWidth, ax

        ALIGN4

mapYUVto8Loop:
        ; get lumas to array, and chromas to dx
        mov     ebx, DWORD PTR ds:[esi]    ;AL = luma 0, AH = chroma 0 (bit 7-4)
        add     esi, 4
        and     ebx, ecx                   ; mask to 5 bits per component
	mov	luma[1],bl
        shr     bx, 11
        mov     dx, cs:[bx+UV65]            ; get U6:5, V6:5 via lookup into dx

        ror     ebx, 16
	mov	luma[3],bl
        shr     bx, 11
        or      dx, cs:[bx+UV43]            ; get U4:3, V4:3 via lookup into dx

        mov     ebx, DWORD PTR ds:[esi]       ;AL = luma 0, AH = chroma 0 (bit 7-4)
        add     esi, 4
        and     ebx, ecx
	mov	luma[5],bl
        shr     bx, 11
        or      dx, cs:[bx+UV21]            ; get U2, V2 via lookup into dx

        ror     ebx, 16
	mov	luma[7],bl

        ; combine luma and chroma to 15 bit value
        mov     ebx, luma[0]                
        shr     ebx, 1
        or      bx, dx                  ;merge pix 0
        mov     al,fs:[bx]

        ror     ebx, 16

        or      bx, dx                  ;merge pix 1
        mov     ah,fs:[bx]
	stos	WORD PTR es:[edi]       ;save 2 palette indices

        mov     ebx, luma[4]                
        shr     ebx, 1
        or      bx, dx                  ;merge pix 2
        mov     al,fs:[bx]

        ror     ebx, 16

        or      bx, dx                  ;merge pix 3
        mov     ah,fs:[bx]
	stos	WORD PTR es:[edi]       ;save 2 palette indices

        dec     wTempWidth
        jnz     mapYUVto8Loop

        sub     esi,dwInc

        dec     wHeight
	jnz	OuterYUVto8Loop

	pop	edi
	pop	esi
cEnd


;+======================================================================+
;|									|
;|		+=================================================+	|
;|   Word 0	| 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0 |	|
;|		| U6 U5 V6 V5 -- -- -- -- Y6 Y5 Y4 Y3 Y2 Y1 Y0 -- |	|
;|		+=================================================+	|
;|		+=================================================+	|
;|   Word 1	| 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0 |	|
;|		| U4 U3 V4 V3 -- -- -- -- Y6 Y5 Y4 Y3 Y2 Y1 Y0 -- |	|
;|		+=================================================+	|
;|		+=================================================+	|
;|   Word 2	| 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0 |	|
;|		| U2 U1 V2 V1 -- -- -- -- Y6 Y5 Y4 Y3 Y2 Y1 Y0 -- |	|
;|		+=================================================+	|
;|		+=================================================+	|
;|   Word 3	| 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0 |	|
;|		| U0 -- V0 -- -- -- -- -- Y6 Y5 Y4 Y3 Y2 Y1 Y0 -- |	|
;|		+=================================================+	|
;+======================================================================+
; Translates from the above format to 16 bit RGB DIB format
; Simultaneously flips the result vertically into a DIB format

cProc   mapUnpackedYUVtoRGB16,<NEAR, PASCAL, PUBLIC>,<ds>
        parmD   pDst            ; Destination
        parmD   pSrc            ; Source data
	parmD	pXlat           ; Pointer to YUV555 xlate table
	parmW	wWidth          ; Width in pixels
	parmW	wHeight         ; Height in pixels
	parmW	dxSrc           ; Width of source in bytes

	localV	luma,8          ; will contain 5 MSBs of luma
	localW	wTempWidth      ; local copy of width
	localD	dwInc           ; Increment to next line
cBegin
	cld
	push	esi
	push	edi

	movzx	edi, di         ; zero the high words
	movzx	esi, si

        movzx   eax, dxSrc       ; calc increment to next scanline
        movzx   edx, wWidth
        shl     edx, 1
        add     eax, edx
        mov     dwInc, eax

        movzx   edi, dxSrc      ;Bytes per row in frame buffer (or source)
        movzx   eax, wHeight    ;Change Src pointer to bottom of frame
        dec     eax
        mul     edi
	lds	si, pSrc
        add     eax, esi        
        mov     esi, eax

	lfs	bx, pXlat
        les     di, pDst

        xor     ax, ax          ;zero hi byte of luma array
        mov     luma[0], ax
        mov     luma[2], ax
        mov     luma[4], ax
        mov     luma[6], ax

ifdef DEBUG
        or      bx,bx       ; pXlat must be 16:0 !!!
        jz      @f
        int 3
        int 3
@@:
endif
        mov     dx, wWidth
        shr     dx,2         ; 4 pixels processed in the inner loop
        mov     wWidth,dx

        mov     ecx, 0f0f8f0f8h  ; chroma and luma mask

OuterYUVto16Loop:
        mov     ax, wWidth
        mov     wTempWidth, ax

        ALIGN4

mapYUVto16Loop:
        ; get lumas to array, and chromas to dx
        mov     ebx, DWORD PTR ds:[esi]    ;AL = luma 0, AH = chroma 0 (bit 7-4)
        add     esi, 4
        and     ebx, ecx                   ; mask to 5 bits per component
	mov	luma[1],bl
        shr     bx, 11
        mov     dx, cs:[bx+UV65]            ; get U6:5, V6:5 via lookup into dx

        ror     ebx, 16
	mov	luma[3],bl
        shr     bx, 11
        or      dx, cs:[bx+UV43]            ; get U4:3, V4:3 via lookup into dx

        mov     ebx, DWORD PTR ds:[esi]       ;AL = luma 0, AH = chroma 0 (bit 7-4)
        add     esi, 4
        and     ebx, ecx
	mov	luma[5],bl
        shr     bx, 11
        or      dx, cs:[bx+UV21]            ; get U2, V2 via lookup into dx

        ror     ebx, 16
	mov	luma[7],bl

        ; combine luma and chroma to 15 bit value
        shl     dx, 1
        mov     ebx, luma[0]                
        or      bx, dx                  ;merge pix 0
        mov     ax,fs:[bx]
	stos	WORD PTR es:[edi]       ;save RGB16

        ror     ebx, 16

        or      bx, dx                  ;merge pix 1
        mov     ax,fs:[bx]
	stos	WORD PTR es:[edi]       ;save RGB16

        mov     ebx, luma[4]                
        or      bx, dx                  ;merge pix 2
        mov     ax,fs:[bx]
	stos	WORD PTR es:[edi]       ;save RGB16

        ror     ebx, 16

        or      bx, dx                  ;merge pix 3
        mov     ax,fs:[bx]
	stos	WORD PTR es:[edi]       ;save RGB16

        dec     wTempWidth
        jnz     mapYUVto16Loop

        sub     esi,dwInc

        dec     wHeight
	jnz	OuterYUVto16Loop

	pop	edi
	pop	esi
cEnd


; Fast routine to get the bytes out of the frame buffer
; so we can set the acquire bit again
cProc   RectCopyBytes,<NEAR, PASCAL, PUBLIC>,<ds>
	parmD	pDst            ; pointer to dest memory
        parmW   wDstWidth       ; width of a dest scanline (in bytes)
        parmD   pSrc            ; pointer to src
	parmW	wSrcWidth       ; width of a src scan (in bytes)
        parmW   xSrc            ; byte offset into scan
        parmW   ySrc            ; scan to start from
	parmW	dxSrc           ; number of bytes (per scan) to copy
	parmW	dySrc           ; number of scans to copy
cBegin
	cld
	push	esi
	push	edi

	movzx	edi, di
	movzx	esi, si
        xor     ecx, ecx

	lds	si, pSrc
        les     di, pDst

        movzx   eax,ySrc
        movzx   edx,wSrcWidth
        mul     edx             ; EAX = ySrc * wSrcWidth
        add     esi,eax         ; pSrc += ySrc * wSrcWidth
        movzx   eax, xSrc
        
        add     esi,eax         ; pSrc += xSrc

        mov     dx,dxSrc

        mov     bx, wSrcWidth
        sub     bx, dx

        mov     ax, wDstWidth
        sub     ax, dx

        movzx   eax,ax          ; destination scan line incrementer
        movzx   ebx,bx          ; source scan line incrementer
        shr     dx,1            ; copy word's
@@:     mov     cx,dx
        rep     movs WORD PTR es:[edi], WORD PTR ds:[esi]
        add     esi, ebx
        add     edi, eax
        dec     dySrc
        jnz     @b

	pop	edi
	pop	esi
cEnd

sEnd	CodeSeg

end
