
;---------------------------------------------------------------------
; BLT4_386.ASM
; This module contains four bit per pixel boardblt code with
; 386 enhancements.
;
; Copyright (c) 1991 Microsoft Corporation
;
; History:
; July 22, 1991 -by- Ray Patrick [raypat]
; Wrote it.
; 
;---------------------------------------------------------------------
.xlist
include cmacros.inc
include 8514.inc
include gdidefs.inc
.list

;-----------------------------------------------------------------------
;	M A C R O S
;-----------------------------------------------------------------------
;---------------------------------Macro---------------------------------;
; These macros replace two working macros within cmacros.inc which allow
; ring 0 access to parameters and local variables declared within
; the context of ExtTextOut.
;-----------------------------------------------------------------------;
?pp1 macro n,t,o,a,b,cpc,ppc
ife ?PLM
n equ (t ptr [bp+b])
n equ (t ptr [ebp+b])
else
.xcref
.xcref ?PLMParm&cpc
.cref
?PLMParm&cpc &macro po
uconcat <n>,,<equ>,,<(t ptr [bp+>,%(a+po-o),<])>
uconcat <n&_32>,,<equ>,,<(t ptr [ebp+>,%(a+po-o),<])>
?PLMParm&ppc po
purge ?PLMParm&cpc
&endm
endif
endm

?al1 macro n,t,o
n equ (t [bp-o])
n&_32 equ (t [ebp-o])
endm

externA __AHIncr

createSeg   _FFOUR, FixedFour, word, public, CODE
sBegin	FixedFour

assumes cs, FixedFour
assumes ds, nothing

	.386p

cProc	BogusDummy4, <NEAR>
	include boardblt.inc
cBegin	<nogen>
;	 mov	 ax, cs
;	 ret
cEnd	<nogen>

cProc	WriteScreenColor4_386, <FAR, PUBLIC>
cBegin	<nogen>
;---------------------------------------------------------------------
; Switch to Ring 0.  This makes port instructions zippy quick.
;---------------------------------------------------------------------
	movzx	ebp,bp			
	cmp	word ptr ToRing0+2,0
	jz	short @f
	mov	cx,bx
	call	ToRing0			;preserves ds, es, ecx, esi, edi, ebp
	mov	bx,cx
@@:
	CheckFIFOSpace	EIGHT_WORDS
	mov	ax, bx			;restore saved function
	or	al,40h			;set the foreground & background
	mov	dx,FUNCTION_0_PORT	;functions to "user defined" data
	out	dx,ax
	mov	dx,FUNCTION_1_PORT
	out	dx,ax
	mov	ax,xExt_32 		;set the xExt on board
	dec	ax
	mov	dx,RECT_WIDTH_PORT
	out	dx,ax
	mov	ax,StartingScanline_32	;get the starting Y within this segment
	mov	di,[si+6]		;get nbr of bytes per line
	mul	di			;now AX has offset of starting line
;	 sub	 di,xExt_32		 ;now DI has the line-to-line increment
	lds	si,[si+10]		;get pointer to start of bits
	add	si,ax			;now DS:SI points to starting line
;	 add	 si,SrcxOrg_32		 ;now DS:SI points to starting byte
	mov	ax, SrcxOrg_32
	shr	ax, 1
	sbb	bl, bl
	mov	SrcAlignedFlag_32, bl
	add	si, ax
	mov	ax,ds
	add	ax,SegmentAdder_32
	mov	ds,ax
	mov	cx,yExt_32 		;get the y-extent as a loop counter
	mov	bx,DstyOrg_32		;get the Y-origin

BWC_LOOP:               
;	 CheckFIFOSpace  THREE_WORDS
	push	cx			;save our line loop counter
	push	si
	mov	ax,DstxOrg_32		;set the X-origin
	mov	dx,Srcx_PORT
	out	dx,ax
	mov	ax,bx			;get next Y-coordinate
	mov	dx,Srcy_PORT
	out	dx,ax
	mov	ax,2319h		;get command to send
	mov	dx,COMMAND_FLAG_PORT
	out	dx,ax
	mov	dx,PATTERN_DEFINE_PORT	;set DX to variable data port
	mov	cx,xExt_32 		;get nbr of bytes to put out
	jcxz	BWC_END_LOOP
	cmp	SrcAlignedFlag_32, 0
	jnz	BWC_OddTransfer
	shr	cx, 1
	jcxz	BWC_EvenTransferRem
	pushf
BWC_EvenTransferLoop:
	lodsb				; get one byte=2 pixels
	mov	ah, al
	shr	ah, 4
	and	al, 0fh
	out	dx, ax
	loop	BWC_EvenTransferLoop
	popf

;	 shr	 cx,1			 ;make it nbr of words
;rep	 outsw				 ;slam 'em out there
	jnc	BWC_END_LOOP		;if no odd byte, keep looping

;We must be very careful here.  If we are at offset FFFFH in a segment and
;we do an OUTSW, we'll crash the system!  Thus, just read it in and put it
;out like a good boy!

BWC_EvenTransferRem:
	lodsb				;get the last byte in the line
;	 and	 al, 0fh
	shr	al, 4
	mov	ah, al
	out	dx,ax
	jmp	short BWC_END_LOOP

BWC_OddTransfer:
	lodsb
	and	al, 0fh
	mov	ah, al			; ah contains the first pixel to write
;	 dec	 cx
;	 jcxz	 BWC_PixelReady 	  ;OddTransferRem
;	 dec	 cx
	shr	cx, 1
	jcxz	BWC_OddTransferRem
	pushf
	push	bx

BWC_OddTransferLoop:
	lodsb
	mov	bl, al
	shr	al, 4
	out	dx, ax
	mov	ah, bl
	and	ah, 0fh
	loop	BWC_OddTransferLoop
	pop	bx
	popf
BWC_OddTransferRem:
	jnc	BWC_END_LOOP
;	 lodsb
;	 shr	 al, 4
;	 mov	 ah, al
BWC_PixelReady:
;	 mov	 al, ah
	out	dx, ax

BWC_END_LOOP:                                                           
	pop	si
	add	si,di			;add on the line increment
	inc	bx			;bump the Y-coordinate
	pop	cx			;restore saved line loop counter
	cmp	ScanlinesPerSegment_32,0;have we a small bitmap?
	je	BWC_EL_LOOP		;yes, continue
	dec	ScanlinesLeftInSegment_32;bump down available scanlines counter
	jz	BWC_HUGE_BITMAP 	;if there's still room, continue on

BWC_EL_LOOP:
	loop	BWC_LOOP		;and continue
BW_EXIT4:
;---------------------------------------------------------------------
; Switch back to Ring 3.
;---------------------------------------------------------------------
	cmp	word ptr ToRing3_32+2,0
	jz	short @f
	call	ToRing3_32		;preserves ds, es, ecx, esi, edi, ebp
@@:	ret

BWC_HUGE_BITMAP:

;We have reached the end of a segment.  We must reset the source bitmap to
;the next segment, offset 0.

	dec	cx
	jcxz	BW_EXIT4
	inc	cx
	push	bx
	mov	ax,ScanlinesPerSegment_32;reset ScanlinesLeftInSegment
	mov	ScanlinesLeftInSegment_32,ax
	mov	ax,ds			;get current segment
	add	ax,__AHIncr		;bump to next segment
	mov	ds,ax			;and reset it into DS
	mov	ax,SrcxOrg_32		;get to starting byte in scanline
	shr	ax, 1
	sbb	bl, bl
	mov	bl, SrcAlignedFlag_32
	mov	si, ax
	pop	bx
	jmp	short BWC_EL_LOOP
cEnd	<nogen>

cProc		ReadScreenColor4_386, <FAR, PUBLIC>
cBegin	<nogen>
;---------------------------------------------------------------------
; Switch to Ring 0.  This makes port instructions zippy quick.
;---------------------------------------------------------------------
	movzx	ebp,bp			
	cmp	word ptr ToRing0+2,0
	jz	short @f
	call	ToRing0			;preserves ds, es, ecx, esi, edi, ebp
@@:

;At this point:     
;       CX contains nbr of bytes per line.
;       ES:DI points at start of bitmap line containing our starting Y.
;	6 words are available in FIFO
;Set read-plane enable to all on:

	mov	ax,01eh
	mov	dx,READ_ENABLE_PORT
	out	dx,ax

;Set the proper mode:

	mov	ax,0a080h
	mov	dx,MODE_PORT
	out	dx,ax

;Set the X-extent:

	mov	ax,xExt_32
	dec	ax			;as per manual
	mov	dx,RECT_WIDTH_PORT
	out	dx,ax

; set the height of the rectangle to 1

	sub	ax, ax
	mov	dx, RECT_HEIGHT_PORT
	out	dx, ax

;Now, set up for our read loop:

;	 sub	 cx,ax			 ;now CX has the line-to-line increment
;	 dec	 cx			 ;(because we decremented above)
	mov	si,cx			;put it into SI
	mov	ax, DstxOrg_32
	shr	ax, 1
	sbb	bl, bl			; if dest. is alligned to byte boundary
	mov	SrcAlignedFlag_32, bl	; bl=0, else bl = 0ffh
	add	di, ax
;	 add	 di,DstxOrg_32		 ;now ES:DI points to starting byte
	mov	cx,yExt_32 		;get the Y-extent as a loop counter
	mov	bx,SrcyOrg_32		;get the Y-origin

RSCLoop:
	CheckFIFOSpace	THREE_WORDS
	push	cx			;save our line loop counter
	push	di
	mov	ax,SrcxOrg_32		;set the X-origin
	mov	dx,Srcx_PORT
	out	dx,ax
	mov	ax,bx			;get next Y-coordinate
	mov	dx,Srcy_PORT
	out	dx,ax
	mov	ax,43b0h		;get command to send was 2318h
	mov	dx,COMMAND_FLAG_PORT
	out	dx,ax

RSCCheckStatus:
	mov	dx,9ae8h		;get status to see if there's data to
	in	ax,dx			;read from the board
	and	ah,1			;is there data available?
	jz	RSCCheckStatus		;nope, keep waiting
	mov	dx,PATTERN_DEFINE_PORT	;set DX to variable data port
	mov	cx,xExt_32 		;get nbr of bytes to suck in
	cmp	SrcAlignedFlag_32, 0	; check if on even or odd dest boundary
	jnz	RSCOddTransfer_386
	shr	cx,1			;make it nbr of words
	jcxz	RSCEvenRem
	pushf
RSCEvenReadScreenLoop:
	in	ax, dx			; read two pixel
	and	al, 0fh 		; al: second pixel read
	shl	ah, 4			; ah: first pixel read
	or	al, ah
	stosb
	loop	RSCEvenReadScreenLoop
	popf
;	 rep	 insw			 ;slam 'em in there
	jnc	RSCNextLine		;if no odd byte, keep looping
RSCEvenRem:
	in	ax,dx			;otherwise, get odd byte
	shl	ah, 4
	mov	al, es:[di]
	and	al, 0fh
	or	al, ah
	stosb
	jmp	short RSCNextLine
RSCLoopNode:
	jmp	short RSCLoop

public	RSCOddTransfer_386
RSCOddTransfer_386:
	push	bx
	dec	cx
;	 jcxz	 RSCOddReadScreenDone
	shr	cx, 1
	pushf
	in	ax, dx			; first do the partial byte
	and	ah, 0fh
	shl	al, 4
	mov	bl, al			; put second pixel into bl now
	mov	al, es:[di]
	and	al, 0f0h
	or	al, ah
	stosb
	jcxz	RSCOddRem
RSCOddReadScreenLoop:
	in	ax, dx
	and	ah, 0fh
	shl	al, 4
	xchg	al, bl
	or	al, ah
;	 mov	 bh, es:[di]
	stosb
	loop	RSCOddReadScreenLoop
RSCOddRem:
	popf
	jnc	RSCOddReadScreenDone
;	 dec	 di			 ; in case of an even pixel count, we
;	 and	 bh, 0fh		 ; need to restore the lower nibble in
	mov	al, es:[di]		; the last byte that was written,
	and	al, 0fh 		; since it was not meant to be there
	or	al, bl			; in the first place
	stosb
RSCOddReadScreenDone:
	pop	bx

RSCNextLine:
	pop	di
	add	di,si			;add on the line increment
	inc	bx			;bump the Y-coordinate
	cmp	ScanlinesLeftInSegment_32,0
					;have we a small bitmap?
	je	RSCEndLoop		;yes, continue
	dec	ScanlinesLeftInSegment_32;bump down available scanlines counter
	jz	RSCHugeBitmap		;if there's still room, continue on

RSCEndLoop:
	pop	cx			;restore saved line loop counter
	loop	RSCLoopNode		;and continue
;---------------------------------------------------------------------
; Switch back to Ring 3.
;---------------------------------------------------------------------
	cmp	word ptr ToRing3_32+2,0
	jz	short @f
	call	ToRing3_32		;preserves ds, es, ecx, esi, edi, ebp
@@:	ret

RSCHugeBitmap:
;We have reached the end of a segment.  We must reset the destination bitmap to
;the next segment, offset 0.

	pop	cx			;if we have reached the end of the
	dec	cx			;loop, don't update the selector!
	jcxz	RSCEnd
	inc	cx
	push	cx
	mov	ax,ScanlinesPerSegment_32;reset ScanlinesLeftInSegment
	mov	ScanlinesLeftInSegment_32,ax
	mov	ax,es			;get current segment
	add	ax,__AHIncr		;bump to next segment
	mov	es,ax			;and reset it into ES
	mov	ax,DstxOrg_32		;get to starting byte in scanline
	shr	ax, 1
	mov	di, ax
	sbb	al, al
	mov	SrcAlignedFlag_32, al
	jmp	short RSCEndLoop
RSCEnd:
;---------------------------------------------------------------------
; Switch back to Ring 3.
;---------------------------------------------------------------------
	cmp	word ptr ToRing3_32+2,0
	jz	short @f
	call	ToRing3_32		;preserves ds, es, ecx, esi, edi, ebp
@@:	ret
cEnd	<nogen>

sEnd	FixedFour
end
