	TITLE MAPA.ASM
	page 60,132

;-----------------------------------------------------------------------;
;	MAPA.ASM							;
;									;
;	Convert from YUV space to 8-bit palettized or 16-bit RGB FAST!	;
;									;
;	For AuraVision video capture driver AVCAPT.DRV.			;
;									;
;-----------------------------------------------------------------------;
;******************************************************************************
;  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;  PURPOSE.
;
;  Copyright (c) 1994 - 1995 Microsoft Corporation.	All Rights Reserved.
;******************************************************************************


?PLM=1		; PASCAL Calling convention is DEFAULT
?WIN=0		; Windows calling convention

.xlist
include cmacros.inc
.list


; Manually perform "push" dword register instruction to remove warning
PUSHD macro reg
	db	66h
	push	reg
endm

; Manually perform "pop" dword register instruction to remove warning
POPD macro reg
	db	66h
	pop	reg
endm

;---------------------------------------;
;	Segment Declarations		;
;---------------------------------------;

ifndef	SEGNAME
	SEGNAME equ <_TEXT>
endif

createSeg %SEGNAME, CodeSeg, word, public, CODE

.386

sBegin	CodeSeg
	assumes cs,CodeSeg
	assumes ds,nothing
	assumes es,nothing

UFactor	dd	(226  * 65536) / 127
VFactor	dd	(179  * 65536) / 127
YFactor	dd	(1704 * 65536) / 1000
RFactor	dd	(508  * 65536) / 1000
BFactor dd	(195  * 65536) / 1000



YUV411D4_Header	label	byte
	db	'YUV411D4'
	db	8 dup(0)

Decode	label	byte
	db	2, 8, 16, 26, 40, 60, 84, 112, 144, 172	; For Luma.
	db	196, 216, 230, 240, 248, 254
	db	2, 8, 16, 26, 40, 60, 84, 112, 144, 172	; For Chroma.
	db	196, 216, 230, 240, 248, 254


;-----------------------------------------------------------------------;
;	mapYUV422toPAL8(pDst, pSrc, pXlat, wWidth, wHeight, dxSrc)	;
;									;
;	Translates from YUV411 unpacked format to palettized 8 bit.	;
;	Simultaneously flips the result vertically into a DIB8 format.	;
;-----------------------------------------------------------------------;

cProc   mapYUV422toPAL8,<NEAR, PASCAL, PUBLIC>,<ds>
	parmD   pDst		; Destination
	parmD   pSrc		; Source data
	parmD	pXlat		; Pointer to YUV555 xlate table
	parmW	wWidth		; Width in pixels
	parmW	wHeight		; Height in pixels
	parmW	dxSrc		; Width of source in bytes

	localV	luma,8		; will contain 5 MSBs of luma
	localW	wTempWidth	; local copy of width
	localD	dwInc		; Increment to next line
cBegin
	cld
	PUSHD	si		; push esi
	PUSHD	di		; push edi

	movzx	edi, di		; zero the high words
	movzx	esi, si

	movzx   eax, dxSrc	; calc increment to next scanline
	movzx   edx, wWidth
	shl	edx, 1
	add	eax, edx
	mov	dwInc, eax

	movzx   edi, dxSrc	;Bytes per row in frame buffer (or source)
	movzx   eax, wHeight	;Change Src pointer to bottom of frame
	dec	eax
	mul	edi
	lds	si, pSrc
	add	eax, esi	
	mov	esi, eax

	lfs	bx, pXlat
	les	di, pDst

	xor	ax, ax		; zero hi byte of luma array
	mov	luma[0], ax
	mov	luma[2], ax
	mov	luma[4], ax
	mov	luma[6], ax

	mov	dx, wWidth
	shr	dx,2		; 4 pixels processed in the inner loop
	mov	wWidth,dx

	mov	ecx, 0f8f8f8f8h	; chroma and luma mask

mapYUV422toPAL8_OuterLoop:
	mov	ax, wWidth
	mov	wTempWidth, ax

	ALIGN 2

mapYUV422toPAL8_InnerLoop:
	; get lumas to array, and chromas to dx
	mov	ebx, DWORD PTR ds:[esi]	; EBX = (V)(Y2)(U)(Y1).
	add	esi, 4
	and	ebx, ecx		; Mask to 5 bits per component.
	xor	ebx, 80008000h		; Invert high bits of chroma.
	mov	luma[1], bl		; luma[1] = Y1.
	shr	bx, 11			; BX = (00000000000UUUUU).
	shl	bx, 5			; BX = (000000UUUUU00000).
	mov	dx, bx			; DX = (000000UUUUU00000).

	ror	ebx, 16			; EBX = (0)(0)(V)(Y2).
	mov	luma[3],bl		; luma[3] = Y2.
	shr	bx, 11			; BX = (00000000000VVVVV).
	or	dx, bx			; DX = (000000UUUUUVVVVV).

	mov	ebx, DWORD PTR ds:[esi]	; EBX = (V)(Y4)(U)(Y3).
	add	esi, 4
	and	ebx, ecx
	mov	luma[5], bl		; luma[5] = Y3.

	ror	ebx, 16			; EBX = (0)(0)(V)(Y4).
	mov	luma[7], bl		; luma[7] = Y4.

	; combine luma and chroma to 15 bit value
	mov	ebx, luma[0]		
	shr	ebx, 1
	or	bx, dx			; merge pix 0
	mov	al,fs:[bx]

	ror	ebx, 16

	or	bx, dx			; merge pix 1
	mov	ah,fs:[bx]
	mov	es:[edi], ax		; save 2 palette indices

	mov	ebx, luma[4]		
	shr	ebx, 1
	or	bx, dx			; merge pix 2
	mov	al,fs:[bx]

	ror	ebx, 16

	or	bx, dx			; merge pix 3
	mov	ah,fs:[bx]
	mov	es:[edi+2], ax		; save 2 palette indices

	add	edi, 4
	dec	wTempWidth
	jnz	mapYUV422toPAL8_InnerLoop

	sub	esi,dwInc

	dec	wHeight
	jnz	mapYUV422toPAL8_OuterLoop

	POPD	di			; pop edi
	POPD	si			; pop esi
cEnd


;-----------------------------------------------------------------------;
;	mapYUV422toRGB16(pDst, pSrc, pXlat, wWidth, wHeight, dxSrc)	;
;									;
;	Translates from YUV411 unpacked format to 16 bit RGB format.	;
;	Simultaneously flips the result vertically into a DIB16 format.	;
;-----------------------------------------------------------------------;

cProc   mapYUV422toRGB16,<NEAR, PASCAL, PUBLIC>,<ds>
	parmD   pDst		; Destination
	parmD   pSrc		; Source data
	parmD	pXlat		; Pointer to YUV555 xlate table
	parmW	wWidth		; Width in pixels
	parmW	wHeight		; Height in pixels
	parmW	dxSrc		; Width of source in bytes

	localV	luma,8		; will contain 5 MSBs of luma
	localW	wTempWidth	; local copy of width
	localD	dwInc		; Increment to next line
cBegin
	cld
	PUSHD	si		; push esi
	PUSHD	di		; push edi

	movzx   eax, dxSrc	; calc increment to next scanline
	movzx   edx, wWidth
	shl	edx, 1
	add	eax, edx
	mov	dwInc, eax

	lds	si, pSrc	; Point DS:ESI at start of frame buffer.
	movzx	esi, si
	movzx   edi, dxSrc	; Bytes per row in frame buffer (or source)
	movzx   eax, wHeight	; Change Src pointer to bottom of frame
	dec	eax
	mul	edi
	add	esi, eax

	lfs	bx, pXlat
	les	di, pDst
	movzx	edi, di

	xor	ax, ax		; zero hi byte of luma array
	mov	luma[0], ax
	mov	luma[2], ax
	mov	luma[4], ax
	mov	luma[6], ax

	mov	dx, wWidth
	shr	dx,2		; 4 pixels processed in the inner loop
	mov	wWidth,dx

	mov	ecx, 0f8f8f8f8h ; chroma and luma mask

mapYUV422toRGB16_OuterLoop:
	mov	ax, wWidth
	mov	wTempWidth, ax

	ALIGN 2

mapYUV422toRGB16_InnerLoop:
	; get lumas to array, and chromas to dx
	mov	ebx, DWORD PTR ds:[esi]	; EBX = (V)(Y2)(U)(Y1).
	add	esi, 4
	and	ebx, ecx		; Mask to 5 bits per component.
	xor	ebx, 80008000h		; Invert high bits of chroma.
	mov	luma[1], bl		; luma[1] = Y1.
	shr	bx, 11			; BX = (00000000000UUUUU).
	shl	bx, 5			; BX = (000000UUUUU00000).
	mov	dx, bx			; DX = (000000UUUUU00000).

	ror	ebx, 16			; EBX = (0)(0)(V)(Y2).
	mov	luma[3], bl		; luma[3] = Y2.
	shr	bx, 11			; BX = (00000000000VVVVV).
	or	dx, bx			; DX = (000000UUUUUVVVVV).

	mov	ebx, DWORD PTR ds:[esi]	; EBX = (V)(Y4)(U)(Y3).
	add	esi, 4
	and	ebx, ecx
	mov	luma[5], bl		; luma[5] = Y3.

	ror	ebx, 16			; EBX = (0)(0)(V)(Y4).
	mov	luma[7], bl		; luma[7] = Y4.

	; combine luma and chroma to 15 bit value

	shl	dx, 1
	mov	ebx, luma[0]		
	or	bx, dx			; merge pix 0
	mov	ax,fs:[bx]
	mov	es:[edi], ax		; save RGB16

	ror	ebx, 16

	or	bx, dx			; merge pix 1
	mov	ax,fs:[bx]
	mov	es:[edi+2], ax		; save RGB16

	mov	ebx, luma[4]		
	or	bx, dx			;merge pix 2
	mov	ax, fs:[bx]
	mov	es:[edi+4], ax		;save RGB16

	ror	ebx, 16

	or	bx, dx			;merge pix 3
	mov	ax, fs:[bx]
	mov	es:[edi+6], ax		;save RGB16

	add	edi, 8
	dec	wTempWidth
	jnz	mapYUV422toRGB16_InnerLoop

	sub	esi, dwInc

	dec	wHeight
	jnz	mapYUV422toRGB16_OuterLoop

	POPD	di			; pop edi
	POPD	si			; pop esi
cEnd




;-----------------------------------------------------------------------;
;	YUVtoRGB24							;
;									;
;	Input:	AL = Y value.						;
;		DH = U value.						;
;		DL = V value.						;
;		ES:EDI = pointer to RGB24 bitmap position.		;
;									;
;	Output:	RGB24 value stored at ES:EDI.				;
;		EDI = EDI + 3.						;
;		BX, CX registers changed.				;
;-----------------------------------------------------------------------;
	public	YUVtoRGB24
YUVtoRGB24	proc	near
	push	dx
	mov	cx, dx			; CH = U, CL = V.
	mov	bl, al			; BX = Y.
	xor	bh, bh

	mov	al, ch			; EAX = U.
	cbw
	cwde
	imul	UFactor			; Set AX = (226/127)*U to rescale.
	shr	eax, 16
	add	ax, bx			; Set AX = U + Y.
	jns	short @f
	xor	ax, ax			; If AX < 0, set AX = 0.
@@:
	cmp	ax, 256
	jc	short @f
	mov	ax, 255			; If AX > 255, set AX = 255.
@@:
	mov	ch, al			; CH = blue component.

	mov	al, cl			; EAX = V.
	cbw
	cwde
	imul	VFactor			; Set AX = (179/127)*V to rescale.
	shr	eax, 16
	add	ax, bx			; Set AX = V + Y.
	jns	short @f
	xor	ax, ax			; If AX < 0, set AX = 0.
@@:
	cmp	ax, 256
	jc	short @f
	mov	ax, 255			; If AX > 255, set AX = 255.
@@:
	mov	cl, al			; CL = red component.

	movzx	eax, bx			; Get EAX = Y.
	mul	YFactor			; Set AX = 1.704Y.
	shr	eax, 16
	mov	bx, ax			; Set BX = 1.704Y.
	xor	eax, eax
	mov	al, cl			; Get EAX = R.
	mul	RFactor			; Set AX = .508R.
	shr	eax, 16
	sub	bx, ax			; Set BX = 1.704Y - .508R.
	xor	eax, eax
	mov	al, ch			; Get EAX = B.
	mul	BFactor			; Set AX = .195B.
	shr	eax, 16
	sub	bx, ax			; Set BX = 1.704Y - .508R - .195B.
	or	bx, bx
	jns	short @f
	xor	bx, bx			; If BX < 0, set BX = 0.
@@:
	cmp	bx, 256
	jc	short @f
	mov	bx, 255			; If BX > 255, set BX = 255.
@@:
	mov	es:[edi], ch		; Save Blue value.
	mov	es:[edi+1], bl		; Save Green value.
	mov	es:[edi+2], cl		; Save Red value.
	add	edi, 3
	pop	dx
	ret
YUVtoRGB24	endp



;-----------------------------------------------------------------------;
;	mapYUV422toRGB24(pDst, pSrc, pXlat, wWidth, wHeight, dxSrc)	;
;									;
;	Translates from YUV411 unpacked format to 24 bit RGB format.	;
;	Simultaneously flips the result vertically into a DIB24 format.	;
;-----------------------------------------------------------------------;

cProc   mapYUV422toRGB24,<NEAR, PASCAL, PUBLIC>,<ds>
	parmD   pDst		; Destination
	parmD   pSrc		; Source data
	parmD	pXlat		; Pointer to YUV555 xlate table
	parmW	wWidth		; Width in pixels
	parmW	wHeight		; Height in pixels
	parmW	dxSrc		; Width of source in bytes

	localV	luma,8		; will contain 5 MSBs of luma
	localW	wTempWidth	; local copy of width
	localD	dwInc		; Increment to next line
	localW	Red		; Temporary color values
	localW	Green
	localW	Blue

cBegin
	cld
	PUSHD	si		; push esi
	PUSHD	di		; push edi

	movzx   eax, dxSrc	; calc increment to next scanline
	movzx   edx, wWidth
	shl	edx, 1
	add	eax, edx
	mov	dwInc, eax

	lds	si, pSrc	; Pointe DS:ESI at frame buffer.
	movzx	esi, si
	movzx   edi, dxSrc	; Bytes per row in frame buffer (or source)
	movzx   eax, wHeight	; Change Src pointer to bottom of frame
	dec	eax
	mul	edi
	add	esi, eax

	lfs	bx, pXlat
	les	di, pDst
	movzx	edi, di

	xor	ax, ax		; zero hi byte of luma array
	mov	luma[0], ax
	mov	luma[2], ax
	mov	luma[4], ax
	mov	luma[6], ax

	mov	dx, wWidth
	shr	dx,2		; 4 pixels processed in the inner loop
	mov	wWidth,dx

mapYUV422toRGB24_OuterLoop:
	mov	ax, wWidth
	mov	wTempWidth, ax

	ALIGN 2

mapYUV422toRGB24_InnerLoop:
	; get lumas to array, and chromas to dx
	mov	ebx, DWORD PTR ds:[esi]	; EBX = (V)(Y2)(U)(Y1).
	add	esi, 4
	xor	ebx, 80008000h		; Invert high bits of chroma.
	mov	luma[1], bl		; luma[1] = Y1.
	mov	dh, bh			; DH = U.

	ror	ebx, 16			; EBX = (0)(0)(V)(Y2).
	mov	luma[3], bl		; luma[3] = Y2.
	mov	dl, bh			; DL = V.

	mov	ebx, DWORD PTR ds:[esi]	; EBX = (V)(Y4)(U)(Y3).
	add	esi, 4
	mov	luma[5], bl		; luma[5] = Y3.

	ror	ebx, 16			; EBX = (0)(0)(V)(Y4).
	mov	luma[7], bl		; luma[7] = Y4.

	; Convert YUV into RGB values using color space conversion.

	mov	al, luma[1]		
	call	YUVtoRGB24		; Convert to RGB24 and save.

	mov	al, luma[3]
	call	YUVtoRGB24		; Convert to RGB24 and save.

	mov	al, luma[5]		
	call	YUVtoRGB24		; Convert to RGB24 and save.

	mov	al, luma[7]
	call	YUVtoRGB24		; Convert to RGB24 and save.

	dec	wTempWidth
	jnz	mapYUV422toRGB24_InnerLoop

	sub	esi, dwInc

	dec	wHeight
	jnz	mapYUV422toRGB24_OuterLoop

	POPD	di			; pop edi
	POPD	si			; pop esi
cEnd




;-----------------------------------------------------------------------;
;	mapYUV411D4toYUV411D4						;
;									;
;	This function copies data from the video frame buffer in YUV411	;
;	compressed format (YUV411D4) into a memory buffer in the same	;
;	format, adding a 48-byte header in the process.			;
;-----------------------------------------------------------------------;

cProc   mapYUV411D4toYUV411D4,<NEAR, PASCAL, PUBLIC>,<ds>
	parmD   pDst		; Destination
	parmD   pSrc		; Source data
	parmD	pXlat		; Pointer to YUVtoCompressed xlate table
	parmW	wWidth		; Width in pixels
	parmW	wHeight		; Height in pixels
	parmW	wPort		; VxP-500 index port
	parmW	wPadFlag	; Set to 1 if one extra pixel on every line.

	localW	wEnableMemory
	localW	wDisableMemory

cBegin
	cld
	PUSHD	si		; push esi
	PUSHD	di		; push edi

;	Copy our internal data header to the first 16 bytes of the output
;	bitmap, and the differential decode table to the next 32 bytes.

	les	di, pDst		; Point ES:DI at destination bitmap.
	movzx	edi, di
	mov	esi, offset YUV411D4_Header
	mov	ecx, 12
	rep	movs dword ptr es:[edi], dword ptr cs:[esi]

	lds	si, pSrc		; Point DS:SI at source bitmap.
	movzx	esi, si
	lfs	bx, pXlat		; Point FS:0  at compression table.

	mov	ax, wWidth		; Calculate words per line
	shr	ax, 3			;   by dividing width by 8
	mov	dx, ax			;   (we do 8 pixels per group)
	shl	ax, 1			;   and multiplying by 3
	add	ax, dx			;   (3 words per group).
	mov	wWidth, ax

	mov	dx, wPort		; AL = R15 value.
	mov	al, 15h
	out	dx, al
	inc	dx
	in	al, dx
	dec	dx
	mov	ah, al			; AH = R15 value.
	mov	al, 15h			; AL = 15
	or	ah, 20h			; AX = enable memory value.
	mov	wEnableMemory, ax	; Save for later.
	and	ah, 0DFh		; AX = disable memory value.
	mov	wDisableMemory, ax	; Save for later.

MYTC_Loop:
	cli
	mov	ax, wEnableMemory
	out	dx, ax
	jmp	$+2

	xor	esi, esi
	movzx	ecx, wWidth
	rep	movs word ptr es:[edi], word ptr ds:[esi]

	mov	cx, wPadFlag
	jcxz	@f
	mov	ax, word ptr ds:[esi]
@@:

	jmp	$+2
	mov	ax, wDisableMemory
	out	dx, ax
	sti

	dec	wHeight
	jnz	MYTC_Loop

	POPD	di		; pop edi
	POPD	si		; pop esi
cEnd





;-----------------------------------------------------------------------;
;	mapYUV411D4toYUV422						;
;									;
;	This function copies data from a memory buffer in YUV411	;
;	compressed format (YUV411D4) into a memory buffer in YUV422	;
;	format, arranged as follows:					;
;									;
;	Y1, U, Y2, V, Y3, U, Y4, V					;
;									;
;	The second U and V will be copies of the first set.		;
;-----------------------------------------------------------------------;

cProc   mapYUV411D4toYUV422,<NEAR, PASCAL, PUBLIC>,<ds>
	parmD   pDst		; Destination
	parmD   pSrc		; Source data
	parmD	pXlat		; Pointer to YUVtoCompressed xlate table
	parmW	wWidth		; Width in pixels
	parmW	wHeight		; Height in pixels
	parmW	dxSrc		; Unused

	localV	luma,8		; will contain 5 MSBs of luma
	localW	wTempWidth	; local copy of width
	localD	dwInc		; Increment to next line
	localB	OldLuma
	localB	OldChromaU
	localB	OldChromaV
	localB	Luma1
	localB	Luma2
	localB	Luma3
	localB	Luma4
	localB	ChromaU
	localB	ChromaV

cBegin
	cld
	PUSHD	si			; push esi
	PUSHD	di			; push edi

	les	di, pDst		; Point ES:DI at destination bitmap.
	movzx	edi, di
	lds	si, pSrc		; Point DS:SI at source bitmap.
	movzx	esi, si
	add	esi, 48			; Skip over 48-byte header.

	xor	bh, bh			; Always leave BH = 0.

mapYUV411D4toYUV422_OuterLoop:
	mov	ax, wWidth		; Calculate groups per line
	shr	ax, 3			;   by dividing width by 8
	mov	wTempWidth, ax

	mov	cx, ds:[esi]		; Get CX = (TV)(EY2) (TU)(TY1).
	mov	al, cl			; Set AL = (TU)(TY1).
	shl	al, 4			; Set AL = Y1 = Y Reference.
	mov	ah, cl			; Set AH = (TU)(TY1).
	and	ah, 0F0h		; Set AH = U = U Reference.
	mov	dh, ah			; Set DH = U Reference.
	mov	es:[edi], ax		; Store (U)(Y1) in buffer.

	mov	bl, ch			; Set BL = (TV)(EY2).
	and	bl, 0Fh			; Set BX = (EY2).
	add	al, Decode[bx]		; Set AL = Y2.
	mov	ah, ch			; Set AH = (TV)(EY2).
	and	ah, 0F0h		; Set AH = V = V Reference.
	mov	dl, ah			; Set DL = V Reference.
	mov	es:[edi+2], ax		; Store (V)(Y2) in buffer.

	mov	cx, ds:[esi+2]		; Get CX = (EU)(EY1) (EY4)(EY3).
	mov	bl, cl			; Set BL = (EY4)(EY3).
	and	bl, 0Fh			; Set BX = (EY3).
	add	al, Decode[bx]		; Set AL = Y3.
	mov	ah, dh			; Set AH = U.
	mov	es:[edi+4], ax		; Save (U)(Y3).

	mov	bl, cl			; Set BL = (EY4)(EY3).
	shr	bl, 4			; Set BX = (EY4).
	add	al, Decode[bx]		; Set AL = Y4.
	mov	ah, dl			; Set AH = V.
	mov	es:[edi+6], ax		; Save (V)(Y4).

	mov	bl, ch			; Set BL = (EU)(EY1).
	and	bl, 0Fh			; Set BX = (EY1).
	add	al, Decode[bx]		; Set AL = Y1.
	mov	bl, ch			; Set BL = (EU)(EY1).
	shr	bl, 4			; Set BX = (EU).
	add	dh, Decode[bx]		; Set DH = U.
	mov	ah, dh			; Set AH = U.
	mov	es:[edi+8], ax		; Save (U)(Y1).

	mov	cx, ds:[esi+4]		; Get CX = (EY4)(EY3) (EV)(EY2).
	mov	bl, cl			; Set BL = (EV)(EY2).
	and	bl, 0Fh			; Set BX = (EY2).
	add	al, Decode[bx]		; Set AL = Y2.
	mov	bl, cl			; Set BL = (EV)(EY2).
	shr	bl, 4			; Set BX = (EV).
	add	dl, Decode[bx]		; Set DL = V.
	mov	ah, dl			; Set AH = V.
	mov	es:[edi+10], ax		; Save (V)(Y2).

	mov	bl, ch			; Set BL = (EY4)(EY3).
	and	bl, 0Fh			; Set BX = (EY3).
	add	al, Decode[bx]		; Set AL = Y3.
	mov	ah, dh			; Set AH = U.
	mov	es:[edi+12], ax		; Save (U)(Y3).

	mov	bl, ch			; Set BL = (EY4)(EY3).
	shr	bl, 4			; Set BX = (EY4).
	add	al, Decode[bx]		; Set AL = Y4.
	mov	ah, dl			; Set AH = V.
	mov	es:[edi+14], ax		; Save (V)(Y4).

	add	esi, 6
	add	edi, 16
	dec	wTempWidth		; Count this as one 8-pixel group.

mapYUV411D4toYUV422_InnerLoop:
	mov	cx, ds:[esi]		; Get CX = (EV)(EY2) (EU)(EY1).
	mov	bl, cl			; Set BL = (EU)(EY1).
	and	bl, 0Fh			; Set BL = (EY1).
	add	al, Decode[bx]		; Set AL = Y1.
	mov	bl, cl			; Set BL = (EU)(EY1).
	shr	bl, 4			; Set BL = (EU).
	add	dh, Decode[bx]		; Set DH = U.
	mov	ah, dh			; Set AH = U.
	mov	es:[edi], ax		; Store (U)(Y1) in buffer.

	mov	bl, ch			; Set BL = (EV)(EY2).
	and	bl, 0Fh			; Set BX = (EY2).
	add	al, Decode[bx]		; Set AL = Y2.
	mov	bl, ch			; Set BL = (EV)(EY2).
	shr	bl, 4			; Set BL = (EV).
	add	dl, Decode[bx]		; Set DL = V.
	mov	ah, dl			; Set AH = V.
	mov	es:[edi+2], ax		; Store (U)(Y1) in buffer.

	mov	cx, ds:[esi+2]		; Get CX = (EU)(EY1) (EY4)(EY3).
	mov	bl, cl			; Set BL = (EY4)(EY3).
	and	bl, 0Fh			; Set BX = (EY3).
	add	al, Decode[bx]		; Set AL = Y3.
	mov	ah, dh			; Set AH = U.
	mov	es:[edi+4], ax		; Store (U)(Y1) in buffer.

	mov	bl, cl			; Set BL = (EY4)(EY3).
	shr	bl, 4			; Set BX = (EY4).
	add	al, Decode[bx]		; Set AL = Y4.
	mov	ah, dl			; Set AH = V.
	mov	es:[edi+6], ax		; Store (U)(Y1) in buffer.

	mov	bl, ch			; Set BL = (EU)(EY1).
	and	bl, 0Fh			; Set BX = (EY1).
	add	al, Decode[bx]		; Set AL = Y1.
	mov	bl, ch			; Set BL = (EU)(EY1).
	shr	bl, 4			; Set BX = (EU).
	add	dh, Decode[bx]		; Set DH = U.
	mov	ah, dh			; Set AH = U.
	mov	es:[edi+8], ax		; Store (U)(Y1) in buffer.

	mov	cx, ds:[esi+4]		; Get CX = (EY4)(EY3) (EV)(EY2).
	mov	bl, cl			; Set BL = (EV)(EY2).
	and	bl, 0Fh			; Set BX = (EY2).
	add	al, Decode[bx]		; Set AL = Y2.
	mov	bl, cl			; Set BL = (EV)(EY2).
	shr	bl, 4			; Set BX = (EV).
	add	dl, Decode[bx]		; Set DL = V.
	mov	ah, dl			; Set AH = V.
	mov	es:[edi+10], ax		; Store (U)(Y1) in buffer.

	mov	bl, ch			; Set BL = (EY4)(EY3).
	and	bl, 0Fh			; Set BX = (EY3).
	add	al, Decode[bx]		; Set AL = Y3.
	mov	ah, dh			; Set AH = U.
	mov	es:[edi+12], ax		; Store (U)(Y1) in buffer.

	mov	bl, ch			; Set BL = (EY4)(EY3).
	shr	bl, 4			; Set BX = (EY4).
	add	al, Decode[bx]		; Set AL = Y4.
	mov	ah, dl			; Set AH = V.
	mov	es:[edi+14], ax		; Store (U)(Y1) in buffer.

	add	esi, 6
	add	edi, 16
	dec	wTempWidth		; Count this as one 8-pixel group.
	jnz	mapYUV411D4toYUV422_InnerLoop

	dec	wHeight
	jnz	mapYUV411D4toYUV422_OuterLoop

	POPD	di			; pop edi
	POPD	si			; pop esi
cEnd





sEnd	CodeSeg
end
