page            ,132
title           Block Write Bitmap to the IBM 8514
.286c


.xlist
include CMACROS.INC
include 8514.INC
include gdidefs.inc
.list                  

sBegin	Data
externD WriteScreenColor
sEnd	Data

subttl          Block Write Process
page +
sBegin          Code
assumes         cs,Code
assumes         ds,Data

externA     __AHIncr			; imported from kernel for prot. mode

cProc	BlockWrite,<NEAR,PUBLIC>

        include BOARDBLT.INC            ;frame definitions

cBegin		<nogen>
	push	ds			;save our DS
	push	ax			;save our passed function
	mov	ax, word ptr WriteScreenColor
	mov	dx, word ptr WriteScreenColor+2
	mov	off_WriteColor, ax	;copy color boardblt routine address
	mov	seg_WriteColor, dx	;to local stack

	lds	si,lpSrcDev		;DS:SI points at our source PDevice
	mov	ax, [si].bmWidth	;Clip extents passed as arguments in
	sub	ax, SrcxOrg		;the BitBlt to the extents in the
	js	BW_Error_exit
	cmp	ax, xExt		;bitmap header
	ja	BWClippedInX
	mov	xExt, ax
BWClippedInX:
	mov	ax, [si].bmHeight	;do clipping in Y
	sub	ax, SrcyOrg
	js	BW_Error_exit		;if AX < 0, something is very wrong
	cmp	ax, yExt
	ja	BWClippedInY
	mov	yExt, ax
BWClippedInY:

;Now, determine if we have a huge bitmap and call the procedure which sets
;up the variables and counters necesary to handle huge bitmap processing.

	mov	ax,SrcyOrg		;initialise the starting Y
	xor	bx,bx			;assume we have a small bitmap
	xor	dx,dx			;and init these return values
	mov	cx,[si].bmSegmentIndex	;get the segment-to-segment adder
	mov	wSegmentIndex, cx
	jcxz	BW_DISPATCH		;if 0, we have a small bitmap
	cCall	GetHugeBitmapParams	;otherwise, set up for a huge bitmap

public  BW_DISPATCH
BW_DISPATCH:

;Now, get the bitmap colour format (either 4 or 8 bits per pixel = colour
;or 1 bit per pixel = monochrome) and decide which output procedure to use:

	mov	StartingScanline,ax	;save starting scanline within segment
	mov	SegmentAdder,bx 	;save the adder to correct segment
	mov	ScanlinesLeftInSegment,cx
					;save nbr of scanlines left in segment
	mov	ScanlinesPerSegment,dx	;save nbr of scanlines per seg (or 0 if
					;a small bitmap)
;Set the mode to "variable data":

	MakeEmptyFIFO
	mov	ax,0a080h		;set the mode to "variable data"
	mov	dx,MODE_PORT		;use pattern source select
	out	dx,ax

;Set the Y-extent on board:

	xor	ax,ax			;Y-extent in board terms is always 1
	.errnz	RECT_HEIGHT_PORT-MODE_PORT
	out	dx,ax

	mov	bl,[si].bmBitsPixel	;get the colour format
	rcr	bl,1			;get the 1's bit into carry
	jc	BW_MONO 		;if 1's bit is set, it's mono format
	jmp	BW_COLOUR		;if no carry, it's colour format

public	BW_Error_exit
BW_Error_exit:				;we only get here if clip error detected
	pop	ax			;clean up the stack
	jmp	BW_EXIT 		;and get out!

public  BW_MONO
BW_MONO:				;6 slots left in FIFO at this point
	pop	bx			;restore saved function to BL
	push	DstxOrg 		;save destination X,Y
	push	DstyOrg

	mov	al,ForegroundColour	;get the background colour
	mov	dx,COLOUR_0_PORT
	out	dx,ax
	mov	al,BackgroundColour	;get the foreground colour
	mov	dx,COLOUR_1_PORT
	out	dx,ax

	mov	ax, bx
	mov	dx,FUNCTION_0_PORT	;set the background to "use colour 0"
	out	dx,ax
	or	al,20h			;set foreground to "use colour 1"
	mov	dx,FUNCTION_1_PORT
	out	dx,ax

	mov	di, [si].bmWidthBytes
	mov	wBytesPerScanline, di
	mov	bx, word ptr [si].bmBits[2] ;BX: 1st bitmap selector
	mov	si, word ptr [si].bmBits[0] ;BX:SI-->bitmap bits
	mov	ax, StartingScanline	;AX: starting scanline in segment
	mul	di			;AX: starting byte offset in segment
	add	si, ax
	add	bx, SegmentAdder
	mov	ds, bx			;DS:SI-->1st byte in source scanline
	assumes ds, nothing

	mov	dx, CLIP_PORT
	mov	cx, xExt
	mov	ax, DstxOrg
	mov	bx, ax
	or	ah, 20h
	out	dx, ax			;set X min
	mov	ax, bx
	add	ax, cx
	dec	ax			;this is an inclusive clip region
	or	ah, 40h
	out	dx, ax			;set X max
	mov	ax, SrcxOrg
	mov	dx, ax			;AX, DX: source X origin
	shr	dx, 3
	mov	wByteOffset, dx 	;this is the number of bytes to skip
	and	ax, 07h 		;AX: source X origin modulo 8
	add	cx, ax			;add that to the X extent and
	sub	bx, ax			;subtract it from Dst X origin
	add	cx, 07h 		;round X extent to next BYTE
	CheckFIFOSpace	ONE_WORD	;now make room in device's FIFO
	mov	ax, cx
	and	ax, 07f8h		;byte allign X extent
	dec	ax
	mov	dx, RECT_WIDTH_PORT	;set width to board
	out	dx, ax
	mov	ax, bx
	mov	DstxOrg, ax		;save modified destination X origin
	and	al, 03h 		;AL: nibble phase of dest X
	mov	LoopShiftFactor, al	;this will be the shift factor
	shr	cx, 3
	mov	wByteCount, cx		;store actual # of bytes to do
	add	cx, wByteOffset
	sub	di, cx			;DI=#of bytes to skip at end of line

	mov	bl, LoopShiftFactor
	mov	cl, bl			;now compute the AND mask to select the
	sub	bh, bh			;left over bits from the previous byte
	stc				;of data
	rcr	bh, cl
	dec	bh
	not	bh
	mov	cx, yExt		; color value
	jcxz	BWMBeCareful

BWMLineLoop:
	push	cx			;save # of lines to do--outer loop ctr
	CheckFIFOSpace	FOUR_WORDS	;make enough room for next iteration
	mov	ax, DstxOrg		;set coordinates for this scanline
	mov	dx, Srcx_PORT
	out	dx, ax
	mov	ax, DstyOrg
	mov	dx, Srcy_PORT
	out	dx, ax
	inc	DstyOrg 		;advance to next scanline
	mov	dx, COMMAND_FLAG_PORT
	mov	ax, 43b3h		;write using pel accrual feature
	out	dx, ax
	mov	dx, PATTERN_DEFINE_PORT

	add	si, wByteOffset 	;skip to correct pos. in X in src
	mov	cx, wByteCount		;CH should always be zero
	jcxz	BWMBottomOfLoop 	;make sure we don't crash
	xchg	cx, bx			;CL: LoopShiftFactor, CH: AND mask
	sub	bh, bh			;make sure no trash is in BH
BWMPixelLoop:				;BL: line loop counter
	lodsb				;fetch 8 new pixels
	mov	ah, al			;copy into AH
	shr	ax, cl			;nibble align to destination
	or	ah, bh			;transfer left-over pels
	mov	bh, al			;copy left-over pixels to be
	and	bh, ch			;mask off pixels to be displayed now
	mov	al, ah			;AX: pixels to be displayed now
	shr	ah, 3			;align upper nibble to what 8514 wants
	shl	al, 1			;align lower nibble
	out	dx, ax			;send all pixels to the board
	dec	bl			;update # of bytes left to do
	jnz	BWMPixelLoop
	jcxz	BWMBottomOfLoop 	;if nibble aligned, done with this rect
	mov	ah, bh			;need to put out remaining pixels
	shr	ah, 3			;AH: remaining pixels (1..3)
	out	dx, ax			;AL: don't care
	mov	bx, cx			;BX: shift factor and mask.

BWMBottomOfLoop:
	pop	cx
	add	si, di			;now check whether we are about to
	jz	BWMUpdateSegment	;cross into a new segment for the next
	mov	ax, si			;scanline.
	add	ax, wBytesPerScanline
	jz	BWMReEntry		;if AX==0 now, we have exact fit
	jc	BWMUpdateSegment	;if CY, go to next segment
BWMReEntry:
	loop	BWMLineLoop
BWMBeCareful:
	jmp	short BWMResetClip
BWMUpdateSegment:
	cmp	cx, 1
	je	BWMResetClip
	mov	si, ds
	add	si, wSegmentIndex
	mov	ds, si
	sub	si, si
	jmp	short BWMReEntry
BWMResetClip:
	CheckFIFOSpace	TWO_WORDS
	mov	dx, CLIP_PORT
	mov	ax, 2000h
	out	dx, ax
	mov	ax, 43ffh
	out	dx, ax
BWMDone:
	pop	DstyOrg 		;restore destination X,Y
	pop	DstxOrg
	jmp	short BW_EXIT		;we're done now

page +
public  BW_COLOUR
BW_COLOUR:
	pop	bx			;restore saved function
	cCall	<dword ptr WriteColor>

BW_EXIT:                                            
	pop	ds			;restore saved DS
	ret				;return to BOARDBLT
cEnd            <nogen>


subttl          Get Parameters for Huge (>64K) Bitmap Processing
page +                                                          
cProc           GetHugeBitmapParams,<NEAR,PUBLIC>                 

cBegin

;Entry:
;       AX contains the starting Y-coordinate.
;       CX contains the segment-to-segment increment (always = 1000H).
;       DS:SI points at the BITMAP data structure.
;Exit:                                             
;       AX will contain the scanline offset into the correct segment.
;       BX contains "adder" to be added to get the starting segment of bitmap.
;       CX has nbr of lines left till the end of our segment
;       DX contains the nbr of scanlines per segment (from BITMAP).
;       DS:SI will still point at the BITMAP data structure.
;
;First determine the starting segment:

	mov	dx,[si+24]		;Get # scans per segment
	xor	bx,bx			;BX will contain final segment
					;adder to add to starting bitmap seg.
GHB_HUGE_BITMAP_SEGMENT_LOOP:
	add	bx,cx			;add segment-to-segment increment
	sub	ax,dx			;if AX becomes negative, we
					;have reached the proper 64K
	jns	GHB_HUGE_BITMAP_SEGMENT_LOOP
					;Not in current segment,try next
	add	ax,dx			;AX will now have Y-coordinate
					;within our segment
	sub	bx,cx			;and BX will have adder to
					;correct segment
	mov	cx,dx			;get total # of scanlines in segment
	sub	cx,ax			;now CX has nbr of scanlines
					;that we can do in our 1st segment
cEnd

sEnd            Code
end
