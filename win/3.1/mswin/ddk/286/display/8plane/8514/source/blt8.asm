;this module contains eight bit per pixel boardblt code

.xlist
include cmacros.inc
include 8514.inc
include drvpal.inc
include gdidefs.inc
.list
.286

externA __AHIncr

sBegin	Data
externB TranslateTable
externB ITranslateTable
sEnd	Data

createSeg   _FEIGHT, FixedEight, word, public, CODE
sBegin	FixedEight

assumes cs, FixedEight
assumes ds, nothing

cProc	BogusDummy8, <NEAR>
	include boardblt.inc
cBegin	<nogen>
cEnd	<nogen>

;On entry: DS:SI-->source memory bitmap
;   BX:  8514's ROP



cProc	WriteScreenColor8, <FAR, PUBLIC>

cBegin	<nogen>

	CheckFIFOSpace	EIGHT_WORDS
	mov	ax, bx
	or	al,40h			;set the foreground & background
	mov	dx,FUNCTION_0_PORT	;functions to "user defined" data
	out	dx,ax
	mov	dx,FUNCTION_1_PORT
	out	dx,ax
	mov	ax,xExt 		;set the xExt on board
	dec	ax
	mov	dx,RECT_WIDTH_PORT
	out	dx,ax
	mov	ax,StartingScanline	;get the starting Y within this segment
	mov	di,[si+6]		;get nbr of bytes per line
	mul	di			;now AX has offset of starting line
	sub	di,xExt 		;now DI has the line-to-line increment
	lds	si,[si+10]		;get pointer to start of bits
	add	si,ax			;now DS:SI points to starting line
	add	si,SrcxOrg		;now DS:SI points to starting byte
	mov	ax,ds
	add	ax,SegmentAdder
	mov	ds,ax
	mov	cx,yExt 		;get the y-extent as a loop counter
	mov	bx,DstyOrg		;get the Y-origin

BWC_LOOP:               
;	 CheckFIFOSpace  THREE_WORDS
	push	cx			;save our line loop counter
	mov	ax,DstxOrg		;set the X-origin
	mov	dx,Srcx_PORT
	out	dx,ax
	mov	ax,bx			;get next Y-coordinate
	mov	dx,Srcy_PORT
	out	dx,ax
	mov	dx,COMMAND_FLAG_PORT
	mov	cx,xExt 		;get nbr of bytes to put out
	test	PaletteXlateFlag, 01h
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
	cmp	ScanlinesPerSegment,0	;have we a small bitmap?
	je	BWC_EL_LOOP		;yes, continue
	dec	ScanlinesLeftInSegment	;bump down available scanlines counter
	jz	BWC_HUGE_BITMAP 	;if there's still room, continue on

BWC_EL_LOOP:
	loop	BWC_LOOP		;and continue
BWC_GetOutOfHere:
	ret

BWC_HUGE_BITMAP:

;We have reached the end of a segment.  We must reset the source bitmap to
;the next segment, offset 0.

	dec	cx
	jcxz	BWC_GetOutOfHere
	inc	cx
	mov	si,ScanlinesPerSegment	;reset ScanlinesLeftInSegment
	mov	ScanlinesLeftInSegment,si
	mov	si,ds			;get current segment
	add	si, __AHIncr		;bump to next segment
	mov	ds,si			;and reset it into DS
	mov	si,SrcxOrg		;get to starting byte in scanline
	jmp	short BWC_EL_LOOP
cEnd	<nogen>

cProc	ReadScreenColor8, <FAR, PUBLIC, WIN, PASCAL>

cBegin	<nogen>

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

	mov	ax,xExt
	dec	ax			;as per manual
	mov	dx,RECT_WIDTH_PORT
	out	dx,ax

;Now, set up for our read loop:

	sub	cx,ax			;now CX has the line-to-line increment
	dec	cx			;(because we decremented above)
	mov	si,cx			;put it into SI
	add	di,DstxOrg		;now ES:DI points to starting byte
	mov	cx,yExt 		;get the Y-extent as a loop counter
	mov	bx,SrcyOrg		;get the Y-origin

public  RSCLoop
RSCLoop:
	CheckFIFOSpace	THREE_WORDS
	push	cx			;save our line loop counter
	mov	ax,SrcxOrg		;set the X-origin
	mov	dx,Srcx_PORT
	out	dx,ax
	mov	ax,bx			;get next Y-coordinate
	mov	dx,Srcy_PORT
	out	dx,ax
	mov	dx,COMMAND_FLAG_PORT
	mov	cx,xExt 		;get nbr of bytes to suck in
	test	PaletteXlateFlag, 01h
	jnz	RSCTranslate
	mov	ax,3318h		;get command to send
	out	dx,ax

public  RSCCheckStatus
RSCCheckStatus:
	GetDataCheckStatus
;	 mov	 dx,9ae8h		 ;get status to see if there's data to
;	 in	 ax,dx			 ;read from the board
;	 and	 ah,1			 ;is there data available?
;	 jz	 RSCCheckStatus 	 ;nope, keep waiting
	mov	dx,PATTERN_DEFINE_PORT	;set DX to variable data port
	shr	cx,1			;make it nbr of words
rep	insw				;slam 'em in there
	jnc	RSCNextLine		;if no odd byte, keep looping
	in	ax,dx			;otherwise, get odd byte
	stosb				;and put it into destinaion
	jmp	short RSCNextLine
RSCLoopNode:
	jmp	short RSCLoop
public	RSCTranslate
RSCTranslate:
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

public  RSCNextLine
RSCNextLine:
	add	di,si			;add on the line increment
	inc	bx			;bump the Y-coordinate
	cmp	ScanlinesLeftInSegment,0
					;have we a small bitmap?
	je	RSCEndLoop		;yes, continue
	dec	ScanlinesLeftInSegment	;bump down available scanlines counter
	jnz	RSCEndLoop		;if there's still room, continue on

;We have reached the end of a segment.  We must reset the destination bitmap to
;the next segment, offset 0.

	pop	cx			;if we have reached the end of the
	dec	cx			;loop, don't update the selector!
	jcxz	RSCWeAreDone
	inc	cx
	push	cx
	mov	di,ScanlinesPerSegment	;reset ScanlinesLeftInSegment
	mov	ScanlinesLeftInSegment,di
	mov	di,es			;get current segment
	add	di, __AHIncr		;bump to next segment
	mov	es,di			;and reset it into ES
	mov	di,DstxOrg		;get to starting byte in scanline

public  RSCEndLoop
RSCEndLoop:
	pop	cx			;restore saved line loop counter
	loop	RSCLoopNode
RSCWeAreDone:
	ret

cEnd	<nogen>

sEnd	FixedEight
end
