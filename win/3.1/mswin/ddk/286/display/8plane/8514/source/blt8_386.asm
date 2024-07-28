;---------------------------------------------------------------------
; BLT8_386.ASM
; This module contains eight bit per pixel boardblt code with
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
include drvpal.inc
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

sBegin	Data
externB TranslateTable
externB ITranslateTable
sEnd	Data

createSeg   _FEIGHT, FixedEight, word, public, CODE
sBegin	FixedEight
assumes cs, FixedEight
assumes ds, nothing

	.386p

cProc	BogusDummy8, <NEAR>
	include boardblt.inc
cBegin	<nogen>
cEnd	<nogen>

;On entry: DS:SI-->source memory bitmap
;   BX:  8514's ROP

cProc	WriteScreenColor8_386, <FAR, PUBLIC>
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
	mov	ax, bx
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
	sub	di,xExt_32 		;now DI has the line-to-line increment
	lds	si,[si+10]		;get pointer to start of bits
	add	si,ax			;now DS:SI points to starting line
	add	si,SrcxOrg_32		;now DS:SI points to starting byte
	mov	ax,ds
	add	ax,SegmentAdder_32
	mov	ds,ax
	mov	cx,yExt_32 		;get the y-extent as a loop counter
	mov	bx,DstyOrg_32		;get the Y-origin
BWC_LOOP:               
;	 CheckFIFOSpace  THREE_WORDS
	push	cx			;save our line loop counter
	mov	ax,DstxOrg_32		;set the X-origin
	mov	dx,Srcx_PORT
	out	dx,ax
	mov	ax,bx			;get next Y-coordinate
	mov	dx,Srcy_PORT
	out	dx,ax
	mov	dx,COMMAND_FLAG_PORT
	mov	cx,xExt_32 		;get nbr of bytes to put out
	test	PaletteXlateFlag_32, 01h
	jnz	BWC_Translate
	mov	ax,3319h		;get command to send
	out	dx,ax
	mov	dx,PATTERN_DEFINE_PORT	;set DX to variable data port
	shr	cx,1			;make it nbr of words
rep	outsw				;slam 'em out there
	jnc	BWC_END_LOOP		;if no odd byte, keep looping

;We must be very careful here.  If we are at offset FFFFH in a segment and
;we do an OUTSW, we'll crash the system!  Thus, just read it in and put it
;out like a good boy!

	lodsb				;get the last byte in the line
	out	dx,ax
	jmp	short BWC_END_LOOP
BWC_Translate:
	mov	ax, DataBASE
	mov	es, ax
	assumes es, Data
	push	bx
	lea	bx, TranslateTable
	mov	ax, 2319h
	out	dx, ax
	mov	dx, PATTERN_DEFINE_PORT
	shr	cx, 1
	pushf
	jcxz	BWC_X_FinishTransfer
BWC_X_TransferLoop:
	lodsw
	xlatb	es:[bx]
	xchg	al, ah
	xlatb	es:[bx]
	out	dx, ax
	loop	BWC_X_TransferLoop
BWC_X_FinishTransfer:
	popf
	jnc	BWC_X_End
	lodsb
	xlatb	es:[bx]
	mov	ah, al
	out	dx, ax
BWC_X_End:
	pop	bx

BWC_END_LOOP:                                                           
	add	si,di			;add on the line increment
	inc	bx			;bump the Y-coordinate
	pop	cx			;restore saved line loop counter
	cmp	ScanlinesPerSegment_32,0;have we a small bitmap?
	je	BWC_EL_LOOP		;yes, continue
	dec	ScanlinesLeftInSegment_32 ;bump down available scanlines counter
	jz	BWC_HUGE_BITMAP 	;if there's still room, continue on

BWC_EL_LOOP:
	loop	BWC_LOOP		;and continue

BWC_GetOutOfHere:
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
	jcxz	BWC_GetOutOfHere
	inc	cx
	mov	si,ScanlinesPerSegment_32 ;reset ScanlinesLeftInSegment
	mov	ScanlinesLeftInSegment_32,si
	mov	si,ds			;get current segment
	add	si, __AHIncr		;bump to next segment
	mov	ds,si			;and reset it into DS
	mov	si,SrcxOrg_32		;get to starting byte in scanline
	jmp	short BWC_EL_LOOP
cEnd	<nogen>

cProc	ReadScreenColor8_386, <FAR, PUBLIC, WIN, PASCAL>

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
;       3 words are available in FIFO
;
;Set read-plane enable to all on:

	mov	ax,0ffh
	mov	dx,READ_ENABLE_PORT
	out	dx,ax

;Set the proper mode:

	mov	ax,0a000h
	mov	dx,MODE_PORT
	out	dx,ax

;Set the X-extent:

	mov	ax,xExt_32
	dec	ax			;as per manual
	mov	dx,RECT_WIDTH_PORT
	out	dx,ax

;Now, set up for our read loop:

	sub	cx,ax			;now CX has the line-to-line increment
	dec	cx			;(because we decremented above)
	mov	si,cx			;put it into SI
	add	di,DstxOrg_32		;now ES:DI points to starting byte
	mov	cx,yExt_32 		;get the Y-extent as a loop counter
	mov	bx,SrcyOrg_32		;get the Y-origin

public  RSCLoop_386
RSCLoop_386:
	CheckFIFOSpace	THREE_WORDS
	push	cx			;save our line loop counter
	mov	ax,SrcxOrg_32		;set the X-origin
	mov	dx,Srcx_PORT
	out	dx,ax
	mov	ax,bx			;get next Y-coordinate
	mov	dx,Srcy_PORT
	out	dx,ax
	mov	dx,COMMAND_FLAG_PORT
	mov	cx,xExt_32 		;get nbr of bytes to suck in
	test	PaletteXlateFlag_32,01h
	jnz	RSCTranslate_386
	mov	ax,3318h		;get command to send
	out	dx,ax

public  RSCCheckStatus_386
RSCCheckStatus_386:
	GetDataCheckStatus
;	 mov	 dx,9ae8h		 ;get status to see if there's data to
;	 in	 ax,dx			 ;read from the board
;	 and	 ah,1			 ;is there data available?
;	 jz	 RSCCheckStatus_386 	 ;nope, keep waiting
	mov	dx,PATTERN_DEFINE_PORT	;set DX to variable data port
	shr	cx,1			;make it nbr of words
rep	insw				;slam 'em in there
	jnc	RSCNextLine_386		;if no odd byte, keep looping
	in	ax,dx			;otherwise, get odd byte
	stosb				;and put it into destinaion
	jmp	short RSCNextLine_386
RSCLoopNode:
	jmp	short RSCLoop_386
public	RSCTranslate_386
RSCTranslate_386:
	mov	ax, DataBASE
	mov	ds, ax
	assumes ds, Data
	push	bx
	lea	bx, ITranslateTable
	mov	ax, 2318h
	out	dx, ax
	GetDataCheckStatus
	mov	dx, PATTERN_DEFINE_PORT
	shr	cx, 1
	pushf
RSCXTransferLoop:
	in	ax, dx
	xlatb
	xchg	al, ah
	xlatb
	stosw
	loop	RSCXTransferLoop
	popf
	jnc	RSCXEnd
	in	ax, dx
	mov	al, ah
	xlatb
	stosb
RSCXEnd:
	pop	bx

public  RSCNextLine_386
RSCNextLine_386:
	add	di,si			;add on the line increment
	inc	bx			;bump the Y-coordinate
	cmp	ScanlinesLeftInSegment_32,0
					;have we a small bitmap?
	je	RSCEndLoop_386		;yes, continue
	dec	ScanlinesLeftInSegment_32;bump down available scanlines counter
	jnz	RSCEndLoop_386		;if there's still room, continue on

;We have reached the end of a segment.  We must reset the destination bitmap to
;the next segment, offset 0.

	pop	cx			;if we have reached the end of the
	dec	cx			;loop, don't update the selector!
	jcxz	RSCWeAreDone
	inc	cx
	push	cx
	mov	di,ScanlinesPerSegment_32;reset ScanlinesLeftInSegment
	mov	ScanlinesLeftInSegment_32,di
	mov	di,es			;get current segment
	add	di, __AHIncr		;bump to next segment
	mov	es,di			;and reset it into ES
	mov	di,DstxOrg_32		;get to starting byte in scanline

public  RSCEndLoop_386
RSCEndLoop_386:
	pop	cx			;restore saved line loop counter
	loop	RSCLoopNode
RSCWeAreDone:
;---------------------------------------------------------------------
; Switch back to Ring 3.
;---------------------------------------------------------------------
	cmp	word ptr ToRing3_32+2,0
	jz	short @f
	call	ToRing3_32		;preserves ds, es, ecx, esi, edi, ebp
@@:	ret
cEnd	<nogen>

sEnd	FixedEight
end
