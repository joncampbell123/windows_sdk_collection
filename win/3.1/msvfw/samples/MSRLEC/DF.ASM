	 page    ,132
;-----------------------------Module-Header-----------------------------;
; Module Name:  DF.ASM - DeltaFrame module
;
;   This module contains the DeltaFrame386 routine
;
; Exported Functions:   none
;
; Public Functions:     DeltaFrame386
;
; Public Data:          none
;
; General Description:
;
; Restrictions:
;
;-----------------------------------------------------------------------;
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

        .xlist
if @Version ge 600
	include cmacros.new
else
        include cmacros.inc
endif
        include windows.inc
        .list


RLE_ESCAPE  equ 0
RLE_EOL     equ 0
RLE_EOF     equ 1
RLE_JMP     equ 2
RLE_MINABS  equ 3

MinJumpLength equ 4 ; minimum jump allowed

; The following structure should be used to access high and low
; words of a DWORD.  This means that "word ptr foo[2]" -> "foo.hi".

LONG	struc
lo	dw	?
hi	dw	?
LONG	ends

FARPOINTER	struc
off	dw	?
sel	dw	?
FARPOINTER      ends

wptr    equ     <word ptr>
bptr    equ     <byte ptr>

min_ax  macro   REG
        sub     ax,REG
	cwd
	and	ax,dx
        add     ax,REG
	endm

max_ax  macro   REG
        sub     ax,REG
	cwd
	not	dx
        and     ax,dx
        add     ax,REG
	endm

; -------------------------------------------------------
;		DATA SEGMENT DECLARATIONS
; -------------------------------------------------------

sBegin  Data

sEnd  Data

; -------------------------------------------------------
;               CODE SEGMENT DECLARATIONS
; -------------------------------------------------------

ifndef SEGNAME
    SEGNAME equ <_TEXT>
endif

createSeg %SEGNAME, CodeSeg, word, public, CODE
.386

sBegin  CodeSeg
        assumes cs,CodeSeg
        assumes ds,nothing
        assumes es,nothing

;---------------------------Public-Routine------------------------------;
; DeltaFrame386
;
;   computes the diff of two 8bpp DIBs
;
; Entry:
;       lpbi            bitmap info
;       pbPrev          previous DIB
;       pbDib           DIB to compress
;       pbRle           RLE bits output
;
; Returns:
;       pbRle contains RLE bits for diference
;       lpbi->biSizeImage contains size
; Error Returns:
;	None
; Registers Preserved:
;       all
; Registers Destroyed:
;       ES,FS,GS,EFLAGS
; Calls:
;	nothing
;-----------------------------------------------------------------------;
        assumes ds,nothing
        assumes es,nothing

cProc DeltaFrame386,<FAR,PUBLIC,PASCAL>,<ds>
        ParmD   lpbi            ; bitmap info
        ParmD   pbPrev          ; previous DIB
        ParmD   pbDib           ; DIB to compress
	ParmD	pbRle		; RLE bits output

        LocalW  curX
        LocalW  curY

        LocalW  ImageWidth
        LocalW  ImageHeight

        LocalW  JumpX
        LocalW  JumpY

        LocalD  NextScan
        LocalD  WidthBytes
cBegin
        pushad                  ; save them all

        movzx   eax,ax          ; make all the hiword's zero
        movzx   ebx,bx
        movzx   ecx,cx
        movzx   edx,dx
        movzx   esi,si
        movzx   edi,di
        movzx   ebp,bp

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   get info from the passed BITMAPINFO and copy it to local
;   storage.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
        lds     si,lpbi                 ; DS:SI --> BITMAPINFO

        mov     ax,wptr [si].biHeight   ; get image size
        mov     ImageHeight,ax

        mov     ax,wptr [si].biWidth
        mov     ImageWidth,ax

        mov     dx,ax                   ; compute scanline width
        add     ax,3
        and     ax,not 3

        movzx   eax,ax
        mov     WidthBytes,eax

        sub     ax,dx                   ; amount to get from end of scan to next
        mov     NextScan,eax

        xor     ax,ax
        mov     curX,ax
        mov     curY,ax

        mov     JumpX,ax
        mov     JumpY,ax

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   init pointers into buffers, the following registers will be constant
;   for the entire DeltaFrame process.
;
;   register usage:
;
;       DS:ESI      -->     DIB
;       ES:EDI      -->     Rle bits
;       FS:         -->     Prev DIB
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
        lds     si,pbDib
        les     di,pbRle
        lfs     ax,pbPrev

        mov     pbPrev,eax
        mov     pbRle,edi
        mov     pbDib,esi

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;
;   Spatial compression
;
;   the frame is to be compressed without relying on the previous frame.
;
;   if pbPrev is NULL, no Temporal compression is wanted, just RLE
;   the DIB and return.
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
DeltaFrameSpatial:
        mov     ax,fs
        or      ax,ax                       ; is pbPrev.sel == 0?
        jnz     short DeltaFrameTemporal    ; ...no go do Temporal

DeltaFrameSpatialLoop:
        mov     cx,ImageWidth               ; encode entire line
        call    EncodeFragment              ; ...go do it
        add     esi, NextScan               ; point pbDib to next scan

        dec     ImageHeight                 ; another scan to do?
        jz      DeltaFrameDone              ; ...no generate EOF and exit

        mov     ax,RLE_ESCAPE+(RLE_EOL*256) ; generate EOL, and go for more
        stos    wptr es:[edi]

        jmp     short DeltaFrameSpatialLoop

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;
;   Temporal compression
;
;   the frame is to be compressed assuming the previous frame is visible
;   any pixels that are the same in both frames will be skiped over.
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

DeltaFrameTemporal:
        xchg    edi,pbPrev              ; fs:edi --> previous DIB

DeltaFrameTemporalLoop:
	mov	cx,ImageWidth		; compute amount of pixels left
	sub	cx,curX 		; on the scanline.
	jz	DeltaFrameEOL		; are we at EOL?

	call	FindFragmentLength	; calc frag length and jump value

	or	ax,ax
	jz	DeltaFrameJump

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   we have a fragment (ie a part of the image that changed) to encode
;
;   first thing we need to do is generate any outstanding jumps we have
;   in (jump.x, jump.y)
;
;	AX is fragment length
;	BX is jump length
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
	add	edi,eax
	xchg	edi,pbPrev		; es:edi --> RLE bits

	push	bx			; save jump size
	push	ax			; save fragment size

	xor	cx,cx
	xor	bx,bx
	xchg	cx,JumpX		; check if we need to gen a jump
	xchg	bx,JumpY

DeltaFrameDoJump:
        mov     ax,cx                   ; check if we need to gen a jump
	or	ax,bx
	jz	DeltaFrameFragment	; no jump needed generate a frag.
        js      DeltaFrameNegY          ; negative, need a EOL

	mov	ax,RLE_ESCAPE+(RLE_JMP*256)
	stos	wptr es:[edi]

	mov	ax,255
	min_ax	cx
	stos	bptr es:[edi]
	sub	cx,ax

	mov	ax,255
	min_ax	bx
	stos	bptr es:[edi]
        sub     bx,ax

        jmp     short DeltaFrameDoJump

DeltaFrameNegY:
	mov	ax,RLE_ESCAPE+(RLE_EOL*256)
	stos	wptr es:[edi]
        mov     cx,curX
        dec     bx
        jmp     short DeltaFrameDoJump

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
DeltaFrameFragment:
        pop     cx
        add     curX,cx
	call	EncodeFragment

	xchg	edi,pbPrev		; fs:edi --> Prev DIB
	pop	bx

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
DeltaFrameJump:
	add	jumpX,bx
	add	curX,bx
	add	esi,ebx
	add	edi,ebx
        jmp     DeltaFrameTemporalLoop

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
DeltaFrameEOL:
        inc     jumpY
        dec     ImageHeight
        jz      DeltaFrameTemporalDone

        mov     eax,NextScan
        add     esi,eax
        add     edi,eax

        mov     ax,curX             ; jumpX -= curX
        sub     jumpX,ax

        xor     ax,ax
        mov     curX,ax
        jz      DeltaFrameTemporalLoop

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
DeltaFrameTemporalDone:
        xchg    edi,pbPrev          ; es:edi --> rle data
        errn$   DeltaFrameDone

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   we are all done!
;
;   generate the final EOF and update the biSizeImage field in passed
;   bitmapinfo and return.
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
DeltaFrameDone:
        mov     ax,RLE_ESCAPE+(RLE_EOF*256) ; generate EOF
        stos    wptr es:[edi]

        lds     si,lpbi                     ; DS:SI --> BITMAPINFO

        sub     edi,pbRle                   ; compute length
        mov     [si].biSizeImage,edi        ; and store it.

        mov     wptr [si].biCompression,BI_RLE8

        errn$   DeltaFrameExit              ; return to caller

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
DeltaFrameExit:
        popad
cEnd

;---------------------------Public-Routine------------------------------;
; EncodeFragment
;
;   RLE encodes a run of 8 bit pixels, no Temporal compression is done.
;
; Entry:
;       SS:BP           --> frame of DeltaFrame386
;       CX                  number of pixels to RLE
;       DS:ESI          --> DIB pixels to RLE
;       ES:EDI          --> place to store RLE data
;
; Returns:
;       DS:ESI          advanced
;       ES:EDI          advanced
;       CX              zero.
; Error Returns:
;	None
; Registers Preserved:
;       EBP,ES,DS,FS,GS
; Registers Destroyed:
;       EAX,EBX,ECX,EDX,ESI,EDI,EFLAGS
;
;-----------------------------------------------------------------------;
        assumes ds,nothing
        assumes es,nothing

EncodeFragment proc near

        or      cx,cx                   ; anything at all to do?
        jnz     EncodeFragmentLoop
        jmp     EncodeFragmentExit

EncodeFragmentLoop:
        mov     bx,dx
        mov     ax,cx                   ; ax = pixels left
        min_ax  255                     ; ax = min(ax,255)  (255 = maximum run)
        shl     ecx,16                  ; save old cx
        mov     cx,ax                   ; cx = maximum run allowed
        mov     dx,bx

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   look for a run of same pixels and generate a single RLE run.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
EncodeFragmentSolid:
        xor     ebx,ebx                 ; bx = 0 (run count)
        mov     ah,bptr ds:[esi]        ; get first pixel

EncodeFragmentSolidScan:
        inc     bx
        cmp     bx,cx
        je      EncodeFragmentSolidRun

	mov	al,bptr ds:[esi+ebx]	; get pixel
	cmp	al,ah
	je	EncodeFragmentSolidScan

EncodeFragmentSolidRun:
        cmp     bx,1                    ; is run greater than one?
        jbe     EncodeFragmentAbs

EncodeFragmentSolidEncode:
        mov     al,bl                   ; store solid run (cnt, color)
        stos    wptr es:[edi]
        add     esi,ebx                 ; advance pbDib

        shr     ecx,16                  ; restore cx (length)
        sub     cx,bx
        jz      EncodeFragmentExit      ; any pixels left to encode?
        jmp     EncodeFragmentLoop

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   look for a run of pixels that are not the same
;   note. we cant generate a abs run less than 3 pixels, so if we have
;   a abs run <3 encode it as a bunch of count=1 rle runs
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
EncodeFragmentAbs:
        cmp     cx,RLE_MINABS           ; enough room left for a min abs run?
        jb      EncodeFragmentSolidEncode

        mov     bx,RLE_MINABS-1

EncodeFragmentAbsScan:
        inc     bx                      ; add another pixel to the run

        ; we want at least 4 pixels in a row to be the same before we
        ; stop ABS mode.  otherwise leaving ABS mode to do a short run
        ; then re-entering ABS mode would be bigger
        ;
        ; if there are not 4 pixels left on the line, encode the max
        ; amount, else the four pixels must be the same

        mov     ax,cx                   ; get remaining length
        sub     ax,bx
        xchg    bx,cx                   ; cx = run, bx = max
        cmp     ax,4                    ; are there 4 or more pixels left?
        jb      EncodeFragmentAbsRun    ; no encode the max amount
        xchg    bx,cx                   ; cx = max, bx = run

        mov     al,bptr ds:[esi+ebx]    ; get first pixel

        cmp     al,bptr ds:[esi+ebx+1]  ; are they the same?
        jne     EncodeFragmentAbsScan

        cmp     al,bptr ds:[esi+ebx+2]  ; are they the same?
        jne     EncodeFragmentAbsScan

        cmp     al,bptr ds:[esi+ebx+3]  ; are they the same?
        jne     EncodeFragmentAbsScan

EncodeFragmentAbsRun:
        xor     al,al                   ; store abs run (0, cnt)
        mov     ah,bl
        stos    wptr es:[edi]

        shr     ecx,16                  ; restore cx (length)
        sub     cx,bx                   ; subtract run length from total

        xchg    cx,bx
        shr     cx,1
        rep     movs wptr es:[edi], wptr ds:[esi]
        adc     cx,cx
        rep     movs bptr es:[edi], bptr ds:[esi]
        mov     cx,bx

        inc     edi                     ; word align RLE data
        and     di,not 1                

        jcxz    EncodeFragmentExit      ; any pixels left to encode?
        jmp     EncodeFragmentLoop      ; and do it again.

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
EncodeFragmentExit:
        ret

EncodeFragment endp

;---------------------------Public-Routine------------------------------;
; FindFragmentLength
;
;   determine the number of pixels that are not the same
;   as the previous frame, this run of pixels need to be encoded.
;
;   a fragment ends when we run out of pixels or we find a run of similar
;   pixels greater than MinJumpLength
;
; Entry:
;       SS:BP           --> frame of DeltaFrame386
;       CX                  number of pixels in line
;       DS:ESI          --> DIB pixels to RLE
;       FS:EDI          --> Previous DIB image
;
; Returns:
;	AX		fragment length
;	BX		run length
;       CX              remaining pixels in line
; Error Returns:
;	None
; Registers Preserved:
;       EBP,ES,DS,FS,GS,ESI,EDI
; Registers Destroyed:
;       EAX,EBX,ECX,EDX,EFLAGS
;-----------------------------------------------------------------------;
        assumes ds,nothing
        assumes es,nothing

FindFragmentLength proc near

	xor	ax,ax
	xor	bx,bx
	jcxz	FindFragmentLengthExit

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   look for a run of pixels that are not the same
;   to the previous frame, we must find MinJumpLength pixels that
;   are the same before we stop.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
        push    bp                      ; save bp
        mov     bp,-1

FindFragmentLengthLoop1:
        mov     bx,-1

FindFragmentLengthLoop:
        inc     bx
	inc	bp			; another one not the same
	cmp	bp,cx			; do we got enough?
	je	FindFragmentLengthDone	; ...yes all done

	mov	ah,fs:[edi+ebp] 	; !!!use words!!!
	mov	al,ds:[esi+ebp]
	cmp	al,ah			; is it exact?
	je	FindFragmentLengthLoop	; the same keep going (and counting)

        cmp     bx,MinJumpLength        ; big enough run to stop?
        jb      FindFragmentLengthLoop1 ; no, zero "same" count and keep going

FindFragmentLengthDone:
	sub	cx,bp
	mov	ax,bp			; return length - jump
	sub	ax,bx
	pop	bp

FindFragmentLengthExit:
        ret

FindFragmentLength endp

sEnd

end
