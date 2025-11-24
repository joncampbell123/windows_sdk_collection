;****************************************************************************
;                                                                           *
; THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
; KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
; IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
; PURPOSE.                                                                  *
;                                                                           *
; Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
;                                                                           *
;****************************************************************************

;----------------------------------------------------------------------------
; TSENGTXT.ASM
;----------------------------------------------------------------------------
	.xlist
DOS5=1
	include cmacros.inc
	include	dibeng.inc
	include macros.inc
	incDrawMode = 1
	include	gdidefs.inc
	.list

;----------------------------------------------------------------------------
; M A C R O S
;----------------------------------------------------------------------------
WRITE_MODE_2 macro
        mov     dx,3ceh
        mov     al,5
        out     dx,al
        inc     dx
        in      al,dx
        and     al,0fch
        or      al,2
        out     dx,al
	endm

PLANAR	macro
        mov     dx,3c4h
        mov     al,4
        out     dx,al
        inc     dx
        in      al,dx
        and     al,11110111b
        out     dx,al
        endm

WRITE_MODE_0 macro
        mov     dx,3ceh
        mov     al,5
        out     dx,al
        inc     dx
        in      al,dx
        and     al,0fch
        out     dx,al
	endm

PACKED_PIXEL macro
        mov     dx,3c4h
        mov     al,4
        out     dx,al
        inc     dx
        in      al,dx
        or      al,00001000b
        out     dx,al
        endm

;----------------------------------------------------------------------------
; E Q U A T E S
;----------------------------------------------------------------------------
IS_BUSY         equ     000000010b              ;Cursor critical section.

;----------------------------------------------------------------------------
; E X T E R N S
;----------------------------------------------------------------------------
        externA		KernelsScreenSel	;equates to a000:0000
        externFP        DIB_ExtTextOut
        externFP        DIB_ExtTextOutExt

;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin	Data
	externW	wBpp
	externB bLatchBank
sEnd	Data

;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
sBegin	Code
	assumes cs,Code
	.386

cProc	TsengTxtOut,<FAR,PUBLIC,PASCAL,NODATA>
	parmD	lpDevice		;Destination device
	parmW	x			;Left origin of string
	parmW	y			;Top  origin of string
	parmD	lpClipRect		;Clipping rectangle
	parmD	lpString		;The string itself
	parmW	count			;Number of characters in the string
	parmD	lpFont  		;Font to use
	parmD	lpDrawMode		;Drawmode structure to use
	parmD	lpXform 		;Current text transform
	parmD	lpDx			;Widths for the characters
	parmD	lpOpaqueRect		;Opaquing rectangle
	parmW	etoOptions		;ExtTextOut options
cBegin <nogen>
	mov	ax,DGROUP
	mov	es,ax
	assumes	ds,nothing
	assumes	es,Data
	assumes	fs,nothing
	assumes	gs,nothing
	mov	bx,sp
        cmp     word ptr ss:[bx][26],0  ;is this an extent call ?
	jl	ETO_CallExtTextOut	;let the DIB Engine handle it.
	lfs	bx,ss:[bx][40]		;ds:[bx]-->PDevice
	test	fs:[bx].deFlags,VRAM    ;is this the screen?
	jz	ETO_CallExtTextOut	;no, let the DIB Engine handle it.
	test	fs:[bx].deFlags,BUSY	;is the screen busy?
	jnz	ETO_CallExtTextOut	;yes. Let the DIB Engine fail.
	cmp	wBpp,8			;bpp = 8?
	jne	ETO_CallExtTextOut	;let the DIB Engine handle it.
	mov	bx,sp
	lfs	bx,ss:[bx][18]		;ds:[bx]-->DrawMode
	cmp	fs:[bx].bkMode,OPAQUE	;Is bg mode Opaque?
	jne	ETO_CallExtTextOut	;No, call DIB Engine
;
; Set up for draw text bitmap call back
;
	pop	ebx			;Get caller's return address
	xor	eax,eax
	xor	ecx,ecx
	mov	ax,cs
	shl	eax,16
	mov	ax,offset DrawTextBitmap
;;	mov	cx,cs
;;	shl	ecx,16
;;	mov	cx,offset DrawOrect

PLABEL ETO_CallExtTextOutExt
	push	eax
	push	ecx
	push	ebx
	jmp	DIB_ExtTextOutExt

PLABEL ETO_CallExtTextOut
	jmp	DIB_ExtTextOut
	
PLABEL ETO_Fail
	xor	ax,ax
	mov	dx,8000h		;ds:ax = 8000:0000 to show failure.
	retf	40
cEnd

;---------------------------------------------------------------------------
; DrawTextBitmap - draw the mono bitmap rectangle to given coordinates
; and color. This mono Bitmap is the realized text string. Clips to given
; clip rectangle.
;---------------------------------------------------------------------------
cProc	DrawTextBitmap,<FAR,PUBLIC,PASCAL,NODATA>,<esi,edi>
	parmD	lpDevice		;Destination device
	parmD	lpMonoBuffer
	parmW	Flags			;0 = opaque, 1 = transparent
	parmD	BgColor
	parmD	FgColor
	parmW	x			;Left origin of string bitmap
	parmW	y			;Top  origin of string bitmap
	parmW	WidthBytes		;width of string bitmap
	parmW	Height			;height of string bitmap
	parmD	lpClipRect		;Clipping rectangle
	localD	DestNextScan
	localD	SaveEdi
	localW	InnerByteCount
	localW	Masks
	localW	SrcNextScan
	localW	NScans
	localW	SaveSi
	localW	DeviceFlags
cBegin 
	mov	ax,DGROUP
	mov	gs,ax
	assumes ds,nothing
	assumes	es,nothing
	assumes	fs,nothing
	assumes gs,Data

	lds	di,lpDevice		;ds:[di]-->pDevice
	mov	ax,BUSY			;lock the display
	xchg	[di].deFlags,ax
	mov	DeviceFlags,ax
	mov	eax,[di].deDeltaScan	;eax = width of dest
	mov	edx,eax			;
	shr	edx,2			;...in quadpixels
	mov	DestNextScan,edx	;save it.
	lds	edi,[di].deBits
	movsx	edx,y
	mul	edx
	add	edi,eax
	movsx	eax,x
	add	eax,8
	add	edi,eax			;ds:[edi]-->where to start drawing

	PLANAR
	shr	edi,2			;ds:[edi]-->screen (planar mode)

	mov	dx,KernelsScreenSel
	mov	es,dx
	mov	dx,3cdh
	in	al,dx
	mov	ah,al			;save current bank
	mov	al,bLatchBank
	out	dx,al			;switch to last bank.
	mov	al,bptr BgColor
	mov	es:[0FFFCh],al		
	mov	al,es:[0FFFCh]		;prime the latches
	mov	al,ah
	out	dx,al			;restore the bank.

	WRITE_MODE_2

	mov	dx,3ceh
	mov	ax,1803h
	out	dx,ax			;set up XOR ALU
	mov	ah,bptr BgColor
	xor	ah,bptr FgColor
	mov	al,08h			
	out	dx,ax			;BIT MASK = Bg ^ Fg colors
;---------------------------------------------------------------------------
; The vga h/w is now set up to pass through common fg & bg bits
; unchanged from bg color in latches, non-common bits come from XOR in the
; ALUs, flipped from the bg to the fg state if the glyph bit for the pixel
; in that plane is 1, still in bg state if the glyph bit for that plane is 0.
;---------------------------------------------------------------------------
	lfs	bx,lpClipRect
	mov	dx,fs:[bx].left
	mov	bx,fs:[bx].right
	call	CompByteInterval
	jc	DTB_Exit		;nothing shows (should not happen).
	mov	Masks,ax		;save off masks (al = left, ah = right)
	mov	InnerByteCount,si	;save this off too.

	les	si,lpMonoBuffer
	inc	si			;Visible data starts at buffer+1.
	xor	bx,bx			;bx = flip table index.

PLABEL	DTB_CheckLeft
	test	al,al			;Left piece?
	jz	DTB_CheckMiddle		;no.
	mov	bl,al
	call	DoEdge			;yes.
	inc	si
	add	edi,2

PLABEL	DTB_CheckMiddle
	cmp	InnerByteCount,0	;Inner piece?
	je	DTB_CheckRight		;no.
	mov	ax,Height
	mov	NScans,ax
	push	edi
	push	si
PLABEL	DTB_DoInnerBytes
	mov	edx,edi
	push	si
	mov	cx,InnerByteCount
@@:	mov	bl,es:[si]
	mov	al,cs:FlipTable[bx]
	inc	si
	mov	ah,al
	shr	al,4
	mov	ds:[edi],ax
	add	edi,2
	dec	cx
	jnz	@b
	pop	si
	add	si,WidthBytes
	mov	edi,edx
	add	edi,DestNextScan
	dec	NScans
	jnz	DTB_DoInnerBytes
	pop	si
	pop	edi
	movzx	eax,InnerByteCount
	add	si,ax
	shl	ax,1
	add	edi,eax

PLABEL DTB_CheckRight
	mov	bl,bptr Masks+1
	test	bl,bl
	jz	DTB_CleanUp
	call	DoEdge			;yes.

PLABEL DTB_CleanUp
	mov	dx,3ceh
	mov	ax,0003h
	out	dx,ax			;restore REPLACE mode in ALU
	mov	ax,0FF08h
	out	dx,ax			;restore BIT MASK

	WRITE_MODE_0
	PACKED_PIXEL

	lds	di,lpDevice		;unlock the display
	mov	ax,DeviceFlags
	xchg	[di].deFlags,ax

PLABEL DTB_Exit
cEnd

;---------------------------------------------------------------------------
; DoEdge
; Entry:
;   ds:[edi]-->screen
;   es:[si]-->mono buffer
;   bl = byte mask
;---------------------------------------------------------------------------
PPROC	DoEdge	near
	mov	SaveEdi,edi
	mov	SaveSi,Si
	mov	dx,3c4h
	mov	ax,2
	out	dx,al
	inc	dx
	mov	al,cs:FlipTable[bx]	;convert to a plane mask.
	mov	ah,al
	shr	al,4
	jz	short DE_2ndNibbleMask

PLABEL DE_1stNibbleMask
	out	dx,al
	mov	cx,Height
@@:	mov	bl,es:[si]
	mov	al,cs:FlipTable[bx]
	shr	al,4
	mov	ds:[edi],al
	add	si,WidthBytes
	add	edi,DestNextScan
	dec	cx
	jnz	@b
	mov	si,SaveSi
	mov	edi,SaveEdi

PLABEL DE_2ndNibbleMask
	test	ah,0Fh
	jz	DE_CleanUp
	inc	edi
	mov	al,ah
	out	dx,al
	mov	cx,Height
@@:	mov	bl,es:[si]
	mov	al,cs:FlipTable[bx]
	mov	ds:[edi],al
	add	si,WidthBytes
	add	edi,DestNextScan
	dec	cx
	jnz	@b
	mov	si,SaveSi
	mov	edi,SaveEdi

PLABEL DE_CleanUp
	mov	al,0Fh
	out	dx,al
	ret
DoEdge	endp
	


;---------------------------------------------------------------------------
; FlipTable
; bits 0-3, 4-7 flipped
;---------------------------------------------------------------------------
FlipTable	label	byte
db 000h,008h,004h,00ch,002h,00ah,006h,00eh,001h,009h,005h,00dh,003h,00bh,007h,00fh
db 080h,088h,084h,08ch,082h,08ah,086h,08eh,081h,089h,085h,08dh,083h,08bh,087h,08fh
db 040h,048h,044h,04ch,042h,04ah,046h,04eh,041h,049h,045h,04dh,043h,04bh,047h,04fh
db 0c0h,0c8h,0c4h,0cch,0c2h,0cah,0c6h,0ceh,0c1h,0c9h,0c5h,0cdh,0c3h,0cbh,0c7h,0cfh
db 020h,028h,024h,02ch,022h,02ah,026h,02eh,021h,029h,025h,02dh,023h,02bh,027h,02fh
db 0a0h,0a8h,0a4h,0ach,0a2h,0aah,0a6h,0aeh,0a1h,0a9h,0a5h,0adh,0a3h,0abh,0a7h,0afh
db 060h,068h,064h,06ch,062h,06ah,066h,06eh,061h,069h,065h,06dh,063h,06bh,067h,06fh
db 0e0h,0e8h,0e4h,0ech,0e2h,0eah,0e6h,0eeh,0e1h,0e9h,0e5h,0edh,0e3h,0ebh,0e7h,0efh
db 010h,018h,014h,01ch,012h,01ah,016h,01eh,011h,019h,015h,01dh,013h,01bh,017h,01fh
db 090h,098h,094h,09ch,092h,09ah,096h,09eh,091h,099h,095h,09dh,093h,09bh,097h,09fh
db 050h,058h,054h,05ch,052h,05ah,056h,05eh,051h,059h,055h,05dh,053h,05bh,057h,05fh
db 0d0h,0d8h,0d4h,0dch,0d2h,0dah,0d6h,0deh,0d1h,0d9h,0d5h,0ddh,0d3h,0dbh,0d7h,0dfh
db 030h,038h,034h,03ch,032h,03ah,036h,03eh,031h,039h,035h,03dh,033h,03bh,037h,03fh
db 0b0h,0b8h,0b4h,0bch,0b2h,0bah,0b6h,0beh,0b1h,0b9h,0b5h,0bdh,0b3h,0bbh,0b7h,0bfh
db 070h,078h,074h,07ch,072h,07ah,076h,07eh,071h,079h,075h,07dh,073h,07bh,077h,07fh
db 0f0h,0f8h,0f4h,0fch,0f2h,0fah,0f6h,0feh,0f1h,0f9h,0f5h,0fdh,0f3h,0fbh,0f7h,0ffh

;--------------------------------------------------------------------------
; DrawORect - draw a rectangle to given coordinates and color
; Clips to given clip rectangle
;---------------------------------------------------------------------------
cProc	DrawOrect,<FAR,PUBLIC,PASCAL,NODATA>,<esi,edi>
	parmD	lpDevice		;Destination device
	parmD	BgColor
	parmW	x			;Left origin of orect
	parmW	y			;Top  origin of orect
	parmW	xExt			;width of orect
	parmW	yExt			;height of orect
cBegin 
cEnd

;----------------------------------------------------------------------------
; CompByteInterval
;   A interval will be computed for byte boundaries.
;
;   A first mask and a last mask will be calculated, and possibly
;   combined into the inner loop count.  If no first byte exists,
;   the start address will be incremented to adjust for it.
;
; Entry:
;	BX    = right coordinate (exclusive)
;	DX    = left coordinate  (inclusive)
; Returns:
;	'C' clear
;	'C' if interval is null
;	SI    = inner loop count
;	AL    = first byte mask (possibly 0)
;	AH    = last  byte mask (possibly 0)
;----------------------------------------------------------------------------
PPROC	CompByteInterval near
	assumes ds,nothing
	assumes es,nothing
	assumes fs,nothing
	assumes gs,nothing
	sub	bx,dx			;Compute extent of interval
	jle	short CBI_NullInterval  ;Null interval, skip it
	dec	bx			;Make interval inclusive
;----------------------------------------------------------------------------
; We now have to determine how many bits will be affected, and how they
; are aligned within the bytes.
;
; (left_x MOD word_size) will give us the starting pixel within the
; left byte/word.  Adding the inclusive extent of the interval to
; left_x MOD word_size) and taking the result MOD word_size will give
; us the last pixel affected in the last byte/word.  These pixel
; indexes (0:7 for bytes, 0:15 for words), can be used to create the
; first and last altered bits mask.
;
; To compute the number of bytes/words in the inner loop, use the
; second calculation above:
;
;	(left_x MOD word_size) + inclusive_extent
;
; and divide it by the word size (8/16).  This yields the following:
;
;   1)	If the result is 0, then only one destination
;	byte/word is being altered.  In this case, the
;	start & ending masks should be ANDed together,
;	the innerloop count set to zero, and last_mask
;	set to to all 0's (don't alter any bits).
;
;		|      x x x x x|		|
;		|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|
;		 0 1 2 3 4 5 6 7
;
;		start MOD 8 = 3,  extent-1 = 4
;		3+7 DIV 8 = 0, only altering one byte
;
;   2)	If the result is 1, then only two bytes/words
;	will be altered.  In this case, the start and
;	ending masks are valid, and all that needs to
;	be done is set the innerloop count to 0.
;
;		|  x x x x x x x|x x x x x x x|
;		|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|
;		 0 1 2 3 4 5 6 7
;
;		start MOD 8 = 1,  extent-1 = 14
;		3+14 DIV 8 = 1.  There is a first and last
;		byte but no innerloop count
;
;   3)	If the result is > 1, then some number of entire
;	bytes/words will be altered by the innerloop.  In
;	this case the number of innerloop bytes/words will
;	be the result - 1.
;
;		|	       x|x x x x x x x x|x
;		|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|
;		 0 1 2 3 4 5 6 7
;
;		start MOD 8 = 7,  extent-1 = 9
;		7+9  DIV 8 = 2.  There is a first and last
;		byte and an innerloop count of 1 (result - 1)
;----------------------------------------------------------------------------
;----------------------------------------------------------------------------
; Compute the starting bit position on the left and the ending bit
; position on the right
;----------------------------------------------------------------------------
	and	dl,00000111b		;Compute bit index for left side
	xor	dh,dh
	add	bx,dx			;Compute bit index for right side
	mov	si,bx			;(save for inner loop count)
	and	bl,00000111b
	mov	cl,dl			;Compute left side altered bits mask
	mov	ax,0FFFFh
	mov	dx,ax			;Need this here later
	shr	al,cl			;AL = left side altered bytes mask
	mov	cl,bl			;Compute right side altered bits mask
	mov	ah,80h
	sar	ah,cl			;AH = right side altered bits mask
	shr	si,3			;Compute inner byte count
	jnz	short CBI_DontCombine   ;loop count + 1 > 0, check it out

;----------------------------------------------------------------------------
; Only one byte will be affected.  Combine the first and last byte
; masks, and set the loop count to 0.
;----------------------------------------------------------------------------
	and	al,ah			;Will use first byte mask only
	xor	ah,ah			;Want last byte mask to be 0
	inc	si			;Fall through to set 0

PLABEL CBI_DontCombine
	dec	si			;Dec inner loop count (might become 0)

;----------------------------------------------------------------------------
; If all pixels in the first byte are altered, combine the first byte
; into the inner loop and clear the first byte mask.  Ditto for the
; last byte
;----------------------------------------------------------------------------
	cmp	al,dl			;Set 'C' if not all pixels 1
	sbb	si,dx			;If no 'C', sub -1 (add 1), else sub 0
	cmp	al,dl			;Set 'C' if not all pixels 1
	sbb	al,dl			;If no 'C', sub -1 (add 1), else sub 0

	cmp	ah,dl			;Set 'C' if not all pixels 1
	sbb	si,dx			;If no 'C', sub -1 (add 1), else sub 0
	cmp	ah,dl			;Set 'C' if not all pixels 1
	sbb	ah,dl			;If no 'C', sub -1 (add 1), else sub 0
	clc				;Carry clear to show there is something
	ret

PLABEL CBI_NullInterval			;Null interval, skip it
	stc
	ret
CompByteInterval endp

sEnd	Code
	end


